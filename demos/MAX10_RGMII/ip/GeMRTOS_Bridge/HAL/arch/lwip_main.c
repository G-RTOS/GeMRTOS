/**
 * \file lwip_main.c
 *
 * \brief This is the main interface API to initialize the LwIP stack.
 *
 * \details
 * Owns LwIP stack bring-up (lwip_init(), lwip_initialize()) and Triple-Speed
 * Ethernet (TSE) MAC / PHY initialization for a single network interface
 * (tse_mac_init(), gemrtos_InitialiseTSE(), gemrtos_UpdateTSEInfo()). PHY
 * detection is by IEEE OUI + model number read over MDIO; the two currently
 * supported PHYs are Marvell 88E1111 and TI DP83867 (see the `switch (OUI)`
 * block in tse_mac_init()). Adding a new PHY model follows the template
 * comment immediately before that switch statement.
 *
 * \note
 * Several historical code paths in this file remain intentionally preserved
 * engineering rules) -- they document earlier design iterations (an
 * alternative multi-interface bring-up path, an alt_alarm-driven LwIP timer
 * task, etc.) that are not part of the active GeMRTOS integration but may be
 * useful reference for future work.
 *
 * This is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License (version 2) as published by the
 * Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
 * >>>NOTE<<< The modification to the GPL is included to allow you to
 * distribute a combined work that includes FreeRTOS without being obliged to
 * provide the source code for proprietary components outside of the FreeRTOS
 * kernel.  FreeRTOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details. You should have received a copy of the GNU General Public
 * License and the FreeRTOS license exception along with FreeRTOS; if not it
 * can be viewed here: http://www.freertos.org/a00114.html and also obtained
 * by writing to Richard Barry, contact details for whom are available on the
 * FreeRTOS WEB site.
 *
 * Created by Engineering Spirit (c) 2012 http://engineering-spirit.nl/
 */

#include <gemrtos.h>

// include libC headers
#include <unistd.h>

// include Altera headers
#include <system.h>
#include <alt_types.h>

#include <sys/alt_alarm.h>

// include LwIP headers
#include <lwip/opt.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/dhcp.h>
#include <lwip/tcp.h>
#include <lwip/stats.h>
#include <lwip/sys.h>
#include <lwip/ip_addr.h>
#include <lwip/tcpip.h>
#include <lwip/sockets.h>

#include <lwip/prot/dhcp.h>

#include <netif/etharp.h>



// don't forget our header
#include "lwip_main.h"

/* MY_TIMER guards the FreeRTOS-based periodic LwIP timer task in lwip_initialize()
 * (the #if MY_TIMER block below). It is always 0 in GeMRTOS: TCP/ARP timers are
 * serviced elsewhere in the platform's task model, not via this alt_alarm-driven
 * task. Kept as an active macro (not disabled) because lwip_initialize() still
 * tests it. */
#define MY_TIMER					0


static netif_status_callback_fn status_callback = NULL;
static netif_status_callback_fn link_callback = NULL;



void lwip_example_app_platform_assert(const char *msg, int line, const char *file)
{
  printf("Assertion \"%s\" failed at line %d in %s\n", msg, line, file);
  while(1);
}

#define TSE_MDIO_SPACE0 0x80


// General function to read MDIO registers (TSE map reaches up to register 0x31)
G_UINT32 mdio_read_register(void *tse_base, unsigned int phy_addr, unsigned int reg_addr)
{
    if (reg_addr <= 0x31) {
        gm_IOWR(tse_base , 0x0f, phy_addr);
        return (gm_IORD(tse_base, TSE_MDIO_SPACE0 + reg_addr));
    } else {
        gm_IOWR(tse_base , 0x0f, phy_addr);
        gm_IOWR(tse_base, TSE_MDIO_SPACE0 + 0xD, 0x1F);
        gm_IOWR(tse_base, TSE_MDIO_SPACE0 + 0xE, reg_addr);
        gm_IOWR(tse_base, TSE_MDIO_SPACE0 + 0xD, 0x401F); 
        return (gm_IORD(tse_base, TSE_MDIO_SPACE0 + 0xE));
    }
}

// General function to write MDIO registers (TSE map reaches up to register 0x31)
G_UINT32 mdio_write_register(void *tse_base, unsigned int phy_addr, unsigned int reg_addr, unsigned int value)
{
    if (reg_addr <= 0x31) {
        gm_IOWR(tse_base , 0x0f, phy_addr);
        gm_IOWR(tse_base, TSE_MDIO_SPACE0 + reg_addr, value);
    } else {
        gm_IOWR(tse_base , 0x0f, phy_addr);
        gm_IOWR(tse_base, TSE_MDIO_SPACE0 + 0xD, 0x1F);
        gm_IOWR(tse_base, TSE_MDIO_SPACE0 + 0xE, reg_addr);
        gm_IOWR(tse_base, TSE_MDIO_SPACE0 + 0xD, 0x401F);
        gm_IOWR(tse_base, TSE_MDIO_SPACE0 + 0xE, value);
    }
    return (G_TRUE);
}


/** \brief Max polling iterations while waiting for the PHY's internal
 *         reset bit (register 0x0, bit 0x8000) to self-clear after an
 *         auto-negotiation restart. This is a fast, PHY-internal
 *         self-clear (not the negotiation itself), so a CPU-cycle-bound
 *         busy-wait is adequate here â€” matches the bound already used for
 *         the TSEMAC software-reset wait in gemrtos_InitialiseTSE(). */
#define TSE_PHY_NEGOTIATION_MAX_POLL  10000U

/** \brief Real wall-clock timeout (ms) for the AUTO_NEGOTIATION_COMPLETE
 *         wait below. IEEE 802.3 clause 28 permits autonegotiation to take
 *         several seconds, particularly when the peer's speed differs from
 *         the link's previous state (e.g. reconnecting a port that had
 *         negotiated 1000BASE-T to a 100BASE-TX-only switch). A fixed CPU
 *         busy-wait iteration count does not track wall-clock time at all;
 *         on real hardware it was observed to expire well before
 *         negotiation actually completed, so gemrtos_UpdateTSEInfo()
 *         returned pre-negotiation link_alive/speed/duplex values and the
 *         P3 debounce loop in tse_lwip_init() could never observe 5
 *         consecutive stable reads â€” each retry re-triggered this same
 *         restart-and-immediately-timeout cycle indefinitely. */
#define TSE_PHY_NEGOTIATION_TIMEOUT_MS        3000U

/** \brief Interval (ms) between AUTO_NEGOTIATION_COMPLETE polls. Using
 *         gu_TaskDelay() instead of a busy-wait yields the CPU to other
 *         tasks for the (up to few-second) duration of a real
 *         negotiation, instead of monopolizing it. */
#define TSE_PHY_NEGOTIATION_POLL_INTERVAL_MS  10U

/** \brief When 1, cap the link at 100BASE-TX even where the PHY and link
 *         partner could negotiate 1000BASE-T.
 *
 *  Withdraws the 1000BASE-T capability from the auto-negotiation
 *  advertisement (register 9, bits 9:8) in tse_mac_init(); auto-negotiation
 *  itself stays enabled, so MDI/MDI-X, duplex resolution and the 10/100
 *  fallbacks are unaffected -- the link partner settles on the best common
 *  10/100 mode (100BASE-TX full duplex against any modern switch).
 *  Register 9 is a standard IEEE 802.3 clause-22 register, identical on
 *  both supported PHYs (Marvell 88E1111, TI DP83867), so the cap is applied
 *  once, outside the per-vendor switch in tse_mac_init().
 *
 *  Rationale: the 1000BASE-T RGMII receive path has far less timing margin
 *  than 100BASE-TX (125 MHz vs 25 MHz) and, on marginal cabling or
 *  connectors, shows a low but non-zero rate of PHY-level receive errors
 *  (surfaced as mSGDMA response_reg=0x1 in tse_receive_server()). Where the
 *  upstream network is 100 Mb anyway, gigabit on the board buys nothing and
 *  this trades it for a more robust link. This is a board/site deployment
 *  choice, so it lives here as a single build-time switch general to both
 *  supported boards rather than in any per-project BSP configuration.
 *
 *  Off by default: behaviour is byte-for-byte unchanged unless set to 1. */
#define TSE_PHY_LIMIT_100M  0

G_UINT32 gemrtos_UpdateTSEInfo (struct netif *netif)
{

	G_UINT32 changed = G_FALSE;

    GU_FPRINTF(stderr, "LWIP = %s, %d \n", __FUNCTION__, __LINE__);

	// Get the TSE information
	gemrtos_tse_system_info *tse_info = netif->state;
	int *tse_phy = (int *) tse_info->tse_mac_base;
    unsigned int phy_addr = get_pointed_field(tse_info, tse_phy_mdio_address);

    unsigned int physts = mdio_read_register(tse_phy, phy_addr, 0x11);

    tse_info->link_alive = ((physts & 0x400) == 0) ? G_FALSE : G_TRUE;
    if (tse_info->link_alive == G_FALSE ) changed = G_TRUE;

    /* PHYSTS bit 11, SPEED DUPLEX RESOLVED -- same bit position on both
     * supported PHYs (Marvell 88E1111, TI DP83867 datasheets): 0 while
     * negotiation/link training is still settling, 1 once the speed/duplex
     * bits below are final. Gigabit (1000BASE-T) training takes noticeably
     * longer than 10/100 and can report transient speed/duplex values while
     * still converging; evaluating those transient reads as a real change
     * caused an infinite restart loop here (each restart re-armed
     * auto-negotiation before training ever settled, so the "changed" check
     * kept firing forever -- observed only when negotiating gigabit;
     * TSE_PHY_LIMIT_100M's 100BASE-TX cap trains fast enough to never hit
     * this window, which is why it masked the problem rather than fixing
     * it). While the link is already up but not yet resolved, defer: leave
     * phy_speed/phy_duplex and changed untouched and let the next poll
     * re-check, instead of restarting negotiation out from under itself. */
    if ((tse_info->link_alive == G_TRUE) && ((physts & 0x0800) == 0)) {
        GU_FPRINTF(stderr, "DIAG: link alive, speed/duplex not yet resolved (PHYSTS=0x%04x) -- deferring\n", physts);
        return changed;
    }

    // Get the speed
    unsigned int speed = (physts & (unsigned int) 0xC000);
    GE_PHY_SPEED speed_previous = tse_info->phy_speed;

    tse_info->phy_speed = (tse_info->link_alive == G_FALSE) ? GE_PHY_NO_SPEED :
                          ( speed == 0x8000) ? GE_PHY_SPEED_1000 :
                          ( speed == 0x4000) ? GE_PHY_SPEED_100 :
                          GE_PHY_SPEED_10;

    if (speed_previous != tse_info->phy_speed ) changed = G_TRUE;

    // Get the duplex
    unsigned int duplex = (physts & (unsigned int) 0x2000);

    GE_PHY_DUPLEX duplex_previous = tse_info->phy_duplex;

    tse_info->phy_duplex = (tse_info->link_alive == G_FALSE) ? GE_PHY_NO_DUPLEX :
                           ( duplex == 0x2000) ? GE_PHY_DUPLEX_FULL : GE_PHY_DUPLEX_HALF;

    if (duplex_previous != tse_info->phy_duplex ) changed = G_TRUE;


    if (tse_info->link_alive == G_TRUE) {
        if (changed == G_TRUE) {
            /* no Software reset the PHY chip with auto-negotation and wait.
             * Bounded (was an unbounded while()) â€” a PHY that never finishes
             * negotiation (link flap, marginal cable/EMI) used to hang this
             * task forever with no error message. The caller in
             * tse_receive_server() already retries gemrtos_UpdateTSEInfo()
             * every 200ms until link_alive is true, so returning here on
             * timeout is safe: it just gets retried. */
            mdio_write_register(tse_phy, phy_addr, 0x0, mdio_read_register(tse_phy, phy_addr, 0x0) | 0x200);

            unsigned int poll_count;
            for (poll_count = 0U;
                 ((mdio_read_register(tse_phy, phy_addr, 0x0) & 0x8000) != 0) && (poll_count < TSE_PHY_NEGOTIATION_MAX_POLL);
                 poll_count++) (void) 0;
            if (poll_count >= TSE_PHY_NEGOTIATION_MAX_POLL) {
                GU_FPRINTF(stderr, "PHY reset bit never cleared! (auto-negotiation restart)\n");
            }

            /* DP83867-specific: TI's Troubleshooting Guide (SNLA246C, Section 3.4
             * "Unstable Link Up Debug in 1Gbps communication") documents that this
             * PHY can drop into low-power mode after receiving a 1000Base-T
             * auto-negotiation page, correctable by writing register 0x01D5 =
             * 0xF508. tse_mac_init() already applies this once at boot; reapplied
             * here on every runtime restart too, mirroring that boot-time
             * calibration, so a low-power-mode re-entry triggered by a *runtime*
             * renegotiation is corrected the same way. A 2026-09-16
             * investigation root-caused the gigabit-never-settling failure on Atum A3 to a
             * different, unrelated bug (see the SPEED_DUPLEX_RESOLVED/LINK_STATUS
             * wait below) -- this write is not what fixed it, and hardware testing
             * with it alone in place showed no effect on that failure. Kept
             * anyway as a low-cost, vendor-documented defensive measure for the
             * separate low-power-mode condition TI describes, which this session's
             * testing neither triggered nor ruled out. Marvell 88E1111 (max10) has
             * no such register/errata; gated on phy_model so this is a no-op
             * there. */
            if (tse_info->phy_model == GE_SUPPORTED_PHY_TI_DP83867) {
                mdio_write_register(tse_phy, phy_addr, 0x01D5, 0xF508);
            }

            GU_FPRINTF(stderr, "Waiting for AUTO_NEGOTATION_COMPLETE\n");

            G_UINT32 negotiation_elapsed_ms = 0U;
            while (((mdio_read_register(tse_phy, phy_addr, 0x1) & (unsigned int) pow(2,5)) == 0) &&
                   (negotiation_elapsed_ms < TSE_PHY_NEGOTIATION_TIMEOUT_MS)) {
                gu_TaskDelay(0, 0, 0, TSE_PHY_NEGOTIATION_POLL_INTERVAL_MS);
                negotiation_elapsed_ms += TSE_PHY_NEGOTIATION_POLL_INTERVAL_MS;
            }

            if (negotiation_elapsed_ms >= TSE_PHY_NEGOTIATION_TIMEOUT_MS) {
                GU_FPRINTF(stderr, "PHY AUTO_NEGOTIATION_COMPLETE never set!\n");
            } else {
                /* Negotiation genuinely completed within the timeout above --
                 * refresh link_alive/speed/duplex from the now-settled PHY
                 * registers so the caller (gemrtos_InitialiseTSE(), invoked
                 * immediately after this function returns) configures the MAC
                 * for the actual negotiated state, not the transitional values
                 * sampled at the top of this function before the restart.
                 *
                 * BMSR AUTO_NEGOTIATION_COMPLETE (reg 0x1 bit 5), just waited
                 * for above, only means the clause-28 *ability exchange*
                 * finished. For 1000BASE-T the PHY's link training/startup
                 * state machine (clause 40) can still be converging for a
                 * further interval, during which PHYSTS (reg 0x11) keeps
                 * reading transient speed/duplex values -- the same
                 * SPEED_DUPLEX_RESOLVED transience the deferred check at the
                 * top of this function guards against (see the comment
                 * there). Reading PHYSTS unconditionally right here, as this
                 * code previously did, could latch one of those transient
                 * values into tse_info->phy_speed/phy_duplex; the *next*
                 * call would then see the (by-then genuinely settled) value
                 * differ from it, evaluate "changed" as true again, and
                 * re-enter this entire reset-and-renegotiate block --
                 * indefinitely, never reaching link-up.
                 *
                 * Confirmed on real Atum A3 hardware (JTAG UART capture,
                 * 2026-09-15): with gigabit negotiation enabled
                 * (TSE_PHY_LIMIT_100M == 0) this looped over 60 times
                 * without ever completing, each cycle re-triggering
                 * "Waiting for AUTO_NEGOTATION_COMPLETE" with neither
                 * timeout message ever printing -- i.e. BMSR completed and
                 * PHYSTS looked "resolved" every time, but on a value that
                 * kept disagreeing with the previous one. Waiting here for
                 * the same SPEED_DUPLEX_RESOLVED bit (0x0800) before
                 * trusting the read closes that gap symmetrically with the
                 * entry-point check. Bounded by the same
                 * TSE_PHY_NEGOTIATION_TIMEOUT_MS budget used above: a PHY
                 * that never resolves must not hang this task forever
                 * either.
                 *
                 * 2026-09-16: SPEED_DUPLEX_RESOLVED alone was not
                 * enough. Temporary diagnostic instrumentation (since
                 * removed) traced a real Atum A3 infinite-loop run and found
                 * PHYSTS's own LINK_STATUS bit (0x0400) reading 0 on 100% of
                 * post-restart samples immediately after RESOLVED went 1,
                 * with the *next* poll's read (~40ms later) reliably showing
                 * both bits set -- corroborated by BMSR's own latching
                 * link-status bit doing the same 0-then-1 transition across
                 * those same two reads (the standard IEEE 802.3
                 * read-a-latching-bit-twice pattern). That single premature
                 * "link down" reading was being trusted immediately below,
                 * forcing phy_speed/phy_duplex to
                 * GE_PHY_NO_SPEED/GE_PHY_NO_DUPLEX; the following call then
                 * always saw the genuinely-settled 1000/FULL reading as
                 * "changed" against that self-inflicted zero and re-entered
                 * this whole block -- an infinite loop with no real link
                 * instability behind it. Waiting here for LINK_STATUS as
                 * well as RESOLVED, before trusting either, closes that race
                 * the same bounded way. Hardware-confirmed on Atum A3
                 * (2026-09-16): link now comes up at SPEED=1Gb FULL,
                 * DHCP binds, 0 reconnects for the full capture. */
                G_UINT32 resolve_elapsed_ms = 0U;
                unsigned int post_physts = mdio_read_register(tse_phy, phy_addr, 0x11);
                while ((((post_physts & 0x0800) == 0) || ((post_physts & 0x0400) == 0)) &&
                       (resolve_elapsed_ms < TSE_PHY_NEGOTIATION_TIMEOUT_MS)) {
                    gu_TaskDelay(0, 0, 0, TSE_PHY_NEGOTIATION_POLL_INTERVAL_MS);
                    resolve_elapsed_ms += TSE_PHY_NEGOTIATION_POLL_INTERVAL_MS;
                    post_physts = mdio_read_register(tse_phy, phy_addr, 0x11);
                }
                if (resolve_elapsed_ms >= TSE_PHY_NEGOTIATION_TIMEOUT_MS) {
                    GU_FPRINTF(stderr, "PHY speed/duplex never resolved after renegotiation!\n");
                }

                tse_info->link_alive = ((post_physts & 0x400) == 0) ? G_FALSE : G_TRUE;

                speed = (post_physts & (unsigned int) 0xC000);
                tse_info->phy_speed = (tse_info->link_alive == G_FALSE) ? GE_PHY_NO_SPEED :
                                      ( speed == 0x8000) ? GE_PHY_SPEED_1000 :
                                      ( speed == 0x4000) ? GE_PHY_SPEED_100 :
                                      GE_PHY_SPEED_10;

                duplex = (post_physts & (unsigned int) 0x2000);
                tse_info->phy_duplex = (tse_info->link_alive == G_FALSE) ? GE_PHY_NO_DUPLEX :
                                       ( duplex == 0x2000) ? GE_PHY_DUPLEX_FULL : GE_PHY_DUPLEX_HALF;
            }
        }
    }

	return (changed);
}

G_UINT32 gemrtos_InitialiseTSE (struct netif *netif)
{	
	unsigned int dat, x;
	// Triple-speed Ethernet MegaCore base address
	gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *) netif->state;	
	int * tse_phy = (int *) tse_info->tse_mac_base;

    GU_FPRINTF(stderr, "LWIP = %s, %d \n", __FUNCTION__, __LINE__);
    
    /* Disable transmit and receive */
    dat = IORD_ALTERA_TSEMAC_CMD_CONFIG(tse_phy);
    dat &= ~(ALTERA_TSEMAC_CMD_TX_ENA_MSK | ALTERA_TSEMAC_CMD_RX_ENA_MSK);
    IOWR_ALTERA_TSEMAC_CMD_CONFIG(tse_phy, dat);

	/* reset the mac and wait until reset is gone*/
    dat |= ALTERA_TSEMAC_CMD_SW_RESET_MSK;
    IOWR_ALTERA_TSEMAC_CMD_CONFIG(tse_phy, dat);

    for (x=0; ((IORD_ALTERA_TSEMAC_CMD_CONFIG(tse_phy) & ALTERA_TSEMAC_CMD_SW_RESET_MSK) && (x < 10000)); x++) (void) 0;
    
	if (x >= 10000)	GU_FPRINTF(stderr, "TSEMAC SW reset bit never cleared!\n");
	
	/* Initialize MAC registers */
	IOWR_ALTERA_TSEMAC_TX_SECTION_EMPTY(tse_phy, tse_info->tse_tx_depth - 16);    
 	IOWR_ALTERA_TSEMAC_TX_ALMOST_FULL(tse_phy,  3);
	IOWR_ALTERA_TSEMAC_TX_ALMOST_EMPTY(tse_phy, 8);
	IOWR_ALTERA_TSEMAC_RX_SECTION_EMPTY(tse_phy, tse_info->tse_rx_depth - 16);
	IOWR_ALTERA_TSEMAC_RX_ALMOST_FULL(tse_phy, 8);    
	IOWR_ALTERA_TSEMAC_RX_ALMOST_EMPTY(tse_phy, 8);
	IOWR_ALTERA_TSEMAC_TX_SECTION_FULL(tse_phy,  16);
	IOWR_ALTERA_TSEMAC_RX_SECTION_FULL(tse_phy,  16);

	/* Set the MAC address */
    IOWR_ALTERA_TSEMAC_MAC_0(tse_phy, (unsigned int)((unsigned int) netif->hwaddr[0]) |
						              (unsigned int)((unsigned int) netif->hwaddr[1] <<  8) |
						              (unsigned int)((unsigned int) netif->hwaddr[2] << 16) |
						              (unsigned int)((unsigned int) netif->hwaddr[3] << 24));
                                      
    IOWR_ALTERA_TSEMAC_MAC_1(tse_phy, (unsigned int) (((unsigned int)((unsigned int) netif->hwaddr[4]) | 
                                      (unsigned int)((unsigned int) netif->hwaddr[5] <<  8)) & 0xFFFF));                                   
	
    IOWR_ALTERA_TSEMAC_FRM_LENGTH(tse_phy, 1536);

    // Minimum Inter Packet Gap is 12 bytes
    IOWR_ALTERA_TSEMAC_TX_IPG_LENGTH(tse_phy, 12);

    // Maximum Pause Quanta Value for Flow Control
    IOWR_ALTERA_TSEMAC_PAUSE_QUANT(tse_phy, 0xFFFF);


	/* enable MAC */
    
    
	dat = ALTERA_TSEMAC_CMD_RX_ERR_DISC_MSK  |  // discard erroneous frames received (bit 26)
#if ENABLE_PHY_LOOPBACK
		  ALTERA_TSEMAC_CMD_PROMIS_EN_MSK    |  // promiscuous mode (bit 4)
		  ALTERA_TSEMAC_CMD_LOOPBACK_MSK     |  // local loopback enables (bit 15)
#endif
		  ALTERA_TSEMAC_CMD_TX_ADDR_INS_MSK;

	switch (tse_info->phy_speed) {
		case GE_PHY_SPEED_1000:
			dat |= ALTERA_TSEMAC_CMD_ETH_SPEED_MSK;
			dat &= ~ALTERA_TSEMAC_CMD_ENA_10_MSK;
			break;
    
		case GE_PHY_SPEED_100:
			dat &= ~ALTERA_TSEMAC_CMD_ETH_SPEED_MSK;
			dat &= ~ALTERA_TSEMAC_CMD_ENA_10_MSK;
			break;
    
		case GE_PHY_SPEED_10:
			dat &= ~ALTERA_TSEMAC_CMD_ETH_SPEED_MSK;
			dat |= ALTERA_TSEMAC_CMD_ENA_10_MSK;
			break;
			
		default:
			dat &= ~ALTERA_TSEMAC_CMD_ETH_SPEED_MSK;
			dat &= ~ALTERA_TSEMAC_CMD_ENA_10_MSK;
			break;		
	}
	
	switch (tse_info->phy_duplex) {
		case GE_PHY_DUPLEX_HALF:
			dat |= ALTERA_TSEMAC_CMD_HD_ENA_MSK;
			break;
		case GE_PHY_DUPLEX_FULL:
			dat &= ~ALTERA_TSEMAC_CMD_HD_ENA_MSK;
			break;
		default:
			dat &= ~ALTERA_TSEMAC_CMD_HD_ENA_MSK;		
			break;
	}

    dat |= ALTERA_TSEMAC_CMD_CNTL_FRM_ENA_MSK | 
           ALTERA_TSEMAC_CMD_PAD_EN_MSK | 
           ALTERA_TSEMAC_CMD_RX_ENA_MSK | 
           ALTERA_TSEMAC_CMD_TX_ENA_MSK;

    
    // Enable receive and transmit TSE
    dat |= ALTERA_TSEMAC_CMD_TX_ENA_MSK | ALTERA_TSEMAC_CMD_RX_ENA_MSK;
            
	IOWR_ALTERA_TSEMAC_CMD_CONFIG(tse_phy, dat);
	return G_TRUE;
}

/*
 * gemrtos_GetTSEInfo obtains PHY information and set the fields in the gemrtos_tse_system_info structure
 * it has to be call for each PHY device
 */
gemrtos_tse_system_info *gemrtos_GetTSEInfo (gemrtos_tse_system_info *tse_info)
{
	unsigned int phyid1, phyid2, OUI, ModelNumber;

    GU_FPRINTF(stderr, "LWIP = %s, %d \n", __FUNCTION__, __LINE__);

    PRINT_ASSERT((tse_info->pnetif->state == tse_info ), "ERROR tse_info->pnetif->state != tse_info");

    // Get the phy infomation to complete structure
    int *tse_phy = (int *) tse_info->tse_mac_base;
    unsigned int phy_addr = get_pointed_field(tse_info, tse_phy_mdio_address);
    
    
    // Get the PHY ID
    phyid1 = mdio_read_register(tse_phy, phy_addr, 0x02);    // Read the PHY_IDENTIFIER1 from MDIO
    phyid2 = mdio_read_register(tse_phy, phy_addr, 0x03);    // Read the PHY_IDENTIFIER2 from MDIO
    
    
    PRINT_ASSERT((((phyid1 != phyid2) && (phyid1 != 0xffff))), "ERROR no PHY found");
     
    OUI = (phyid1 << 6) | ((phyid2 >> 10) & 0x3f);
    ModelNumber = ((phyid2 >> 4) & 0x3f);
    
    
    switch (OUI) {
        case OUI_MARVELL:
            if (ModelNumber == ModelNumber_88E1111) {
                gu_fprintf("*********************************\n");
                gu_fprintf("Marvel 88E1111 Ethernet PHY found\n");
                gu_fprintf("*********************************\n");
                tse_info->phy_model = GE_SUPPORTED_PHY_MARVELL_88E1111;
                // MARVELL : Mode changed to RGMII/Modified MII to Copper mode
                mdio_write_register(tse_phy, phy_addr, 0x1B , (mdio_read_register(tse_phy, phy_addr, 0x1B) & 0xfff0) | 0xb);

                // Write to register 20 of the PHY chip to reset delay for input/output clk
                mdio_write_register(tse_phy, phy_addr, 0x14, mdio_read_register(tse_phy, phy_addr, 0x14) | 0x0082);
                
                // Software reset the PHY chip and wait
                mdio_write_register(tse_phy, phy_addr, 0x0 , (mdio_read_register(tse_phy, phy_addr, 0x0) | 0x8000));
                while ( mdio_read_register(tse_phy, phy_addr, 0x0) & 0x8000  );
                
            }
            break;
  
        case OUI_TEXAS_INSTRUMENT:
            if (ModelNumber == ModelNumber_DP83867) {
                gu_fprintf("*********************************\n");
                gu_fprintf("TI DP83867 Ethernet PHY found\n");
                gu_fprintf("*********************************\n");
                tse_info->phy_model = GE_SUPPORTED_PHY_TI_DP83867;
                
                // Software reset the PHY chip and wait
                mdio_write_register(tse_phy, phy_addr, 0x0 , (mdio_read_register(tse_phy, phy_addr, 0x0) | 0x8000));
                while ( mdio_read_register(tse_phy, phy_addr, 0x0) & 0x8000  );
            }
            break;
  
        default:
            tse_info->phy_model = GE_NOT_SUPPORTED;
            gu_fprintf("\nERROR Ethernet PHY not found\n");
            break;
    };
    
    PRINT_ASSERT((tse_info->phy_model != GE_NOT_SUPPORTED), "ERROR PHY chipset not supported OUT= 0x%x, ModelNumber= 0x%x", (unsigned int) OUI, (unsigned int) ModelNumber);


    return (gemrtos_tse_system_info *) tse_info;
}

/* @Function Description: TSE MAC Initialization routine. This function opens the
 *                        device handle, configure the callback function and interrupts ,
 *                        for SGDMA TX and SGDMA RX block associated with the TSE MAC,
 *                        Initialize the MAC Registers for the RX FIFO and TX FIFO
 *                        threshold watermarks, initialize the tse device structure,
 *                        set the MAC address of the device and enable the MAC   
 * 
 * @API TYPE: Internal
 * @Param iface index of the NET structure associated with TSE instance
 * @Return 0 if ok, else -1 if error
 */
int tse_mac_init(struct netif *pnetif)
{

    GU_FPRINTF(stderr, "LWIP = %s, %d \n", __FUNCTION__, __LINE__);

	// ############################################################
	// page 88 and 112 of ug_ethernet-683402-833881.pdf
	// https://www.intel.com/content/www/us/en/docs/programmable/813669/25-1/system-with-mii-gmii-or-rgmii.html
	// ############################################################

    PRINT_ASSERT((((gemrtos_tse_system_info *) pnetif->state)->pnetif == pnetif), "ERROR gemrtos_tse != pnetif->state");
    
    gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *) pnetif->state;
	// Triple-speed Ethernet MegaCore base address
    
    // Get the phy infomation to complete structure
	int *tse_phy = (int *) tse_info->tse_mac_base;
    unsigned int phy_addr = get_pointed_field(tse_info, tse_phy_mdio_address);


	unsigned int phyid1, phyid2, OUI, ModelNumber;

    GU_FPRINTF(stderr, "LWIP = %s, %d \n", __FUNCTION__, __LINE__);

    PRINT_ASSERT((tse_info->pnetif->state == tse_info ), "ERROR tse_info->pnetif->state != tse_info");

    phyid1 = mdio_read_register(tse_phy, phy_addr, 0x02);    // Read the PHY_IDENTIFIER1 from MDIO
    phyid2 = mdio_read_register(tse_phy, phy_addr, 0x03);    // Read the PHY_IDENTIFIER2 from MDIO
    
    PRINT_ASSERT((((phyid1 != phyid2) && (phyid1 != 0xffff))), "ERROR no PHY found");
     
    OUI = (phyid1 << 6) | ((phyid2 >> 10) & 0x3f);
    ModelNumber = ((phyid2 >> 4) & 0x3f);

    /* Template for adding support for a new PHY (e.g. Realtek RTL8211F):
     *   1. Add the OUI / ModelNumber_* constants to lwip_main.h.
     *   2. Add a `case OUI_<vendor>:` block below, following the existing
     *      Marvell/TI structure: check ModelNumber, set tse_info->phy_model,
     *      then issue the vendor-specific MDIO register writes required to
     *      bring the PHY into RGMII/copper mode for this board.
     *   3. Register write sequences obtained by empirical tuning (rather than
     *      directly from the datasheet's documented register map) must be
     *      commented as such and validated on real hardware before merging
     *      (see the DP83867 block below for the expected documentation style).
     *   4. This detection preamble (PHY ID read + OUI/ModelNumber decode) is
     *      duplicated in gemrtos_GetTSEInfo() above; keep both in sync if the
     *      OUI/ModelNumber extraction logic itself ever changes. */
    switch (OUI) {
        case OUI_MARVELL:
            if (ModelNumber == ModelNumber_88E1111) {
                gu_fprintf("*********************************\n");
                gu_fprintf("Marvel 88E1111 Ethernet PHY found\n");
                gu_fprintf("*********************************\n");
                tse_info->phy_model = GE_SUPPORTED_PHY_MARVELL_88E1111;
                // MARVELL : Mode changed to RGMII/Modified MII to Copper mode
                mdio_write_register(tse_phy, phy_addr, 0x1B , (mdio_read_register(tse_phy, phy_addr, 0x1B) & 0xfff0) | 0xb);

                // Write to register 20 of the PHY chip to reset delay for input/output clk
                mdio_write_register(tse_phy, phy_addr, 0x14, mdio_read_register(tse_phy, phy_addr, 0x14) | 0x0082);
                
                // Software reset the PHY chip and wait
                mdio_write_register(tse_phy, phy_addr, 0x0 , (mdio_read_register(tse_phy, phy_addr, 0x0) | 0x8000));
                {
                    /* Bounded, CPU-cycle busy-wait -- same reasoning and same
                     * bound as the identical reset-bit wait in
                     * gemrtos_UpdateTSEInfo(): fast, PHY-internal self-clear,
                     * not the negotiation itself. */
                    unsigned int poll_count;
                    for (poll_count = 0U;
                         ((mdio_read_register(tse_phy, phy_addr, 0x0) & 0x8000) != 0) && (poll_count < TSE_PHY_NEGOTIATION_MAX_POLL);
                         poll_count++) (void) 0;
                    if (poll_count >= TSE_PHY_NEGOTIATION_MAX_POLL) {
                        GU_FPRINTF(stderr, "PHY reset bit never cleared! (Marvell 88E1111 init)\n");
                    }
                }

            }
            break;

        case OUI_TEXAS_INSTRUMENT:
            if (ModelNumber == ModelNumber_DP83867) {
                gu_fprintf("*********************************\n");
                gu_fprintf("TI DP83867 Ethernet PHY found\n");
                gu_fprintf("*********************************\n");
                tse_info->phy_model = GE_SUPPORTED_PHY_TI_DP83867;


// Improving Link-Up margings for short cables (Application specific debugs)
// DP83867 Hard reset
mdio_write_register(tse_phy, phy_addr, 0x001F, 0x8000);

                // Software reset the PHY chip and wait
                mdio_write_register(tse_phy, phy_addr, 0x0 , (mdio_read_register(tse_phy, phy_addr, 0x0) | 0x8000 | 0x200));
                {
                    /* Bounded, CPU-cycle busy-wait -- see the Marvell block above. */
                    unsigned int poll_count;
                    for (poll_count = 0U;
                         ((mdio_read_register(tse_phy, phy_addr, 0x0) & 0x8000) != 0) && (poll_count < TSE_PHY_NEGOTIATION_MAX_POLL);
                         poll_count++) (void) 0;
                    if (poll_count >= TSE_PHY_NEGOTIATION_MAX_POLL) {
                        GU_FPRINTF(stderr, "PHY reset bit never cleared! (TI DP83867 init)\n");
                    }
                }


/* TI DP83867 extended/test-page register writes (registers >= 0x0053 are outside the
 * IEEE-standard MDIO register map and are not documented in the public DP83867
 * datasheet's main register table). This exact sequence and these exact values were
 * arrived at empirically ("Application specific debugs" above) to fix short-cable
 * link-up margin issues on this board; the per-register bit-level meaning is not
 * independently documented here because it is not known with certainty, and
 * an assumption must not be presented as fact. Treat this block as an
 * opaque, board-validated calibration sequence: do not reorder, add, or remove
 * writes without re-validating link-up behavior on real hardware afterward. */
mdio_write_register(tse_phy, phy_addr, 0x0053, 0x2054);
mdio_write_register(tse_phy, phy_addr, 0x00E4, 0x0080);
mdio_write_register(tse_phy, phy_addr, 0x00EF, 0x3840);
mdio_write_register(tse_phy, phy_addr, 0x0102, 0x7477);
mdio_write_register(tse_phy, phy_addr, 0x0104, 0x4577);
mdio_write_register(tse_phy, phy_addr, 0x010C, 0x7777);
mdio_write_register(tse_phy, phy_addr, 0x01C2, 0x7FDE);
mdio_write_register(tse_phy, phy_addr, 0x0115, 0x5555);
mdio_write_register(tse_phy, phy_addr, 0x0118, 0x0771);
mdio_write_register(tse_phy, phy_addr, 0x011D, 0x6DB2);
mdio_write_register(tse_phy, phy_addr, 0x011E, 0x3FFB);
mdio_write_register(tse_phy, phy_addr, 0x01C3, 0xFFC6);
mdio_write_register(tse_phy, phy_addr, 0x01c4, 0x0FC2);
mdio_write_register(tse_phy, phy_addr, 0x01C5, 0x0FF0);
mdio_write_register(tse_phy, phy_addr, 0x012C, 0x0E81);
mdio_write_register(tse_phy, phy_addr, 0x01D5, 0xF508);


                // 0x0032:7  = 1 RGMII_EN, enable RGMII interface
                // 0x0032:1  = 1 transmit clock is shifted relative to receive data
                // 0x0032:0  = 1 receive clock is shifted relative to receive data
                mdio_write_register(tse_phy, phy_addr, 0x32, mdio_read_register(tse_phy, phy_addr, 0x32) | 0x0083 );

                // 0x0086:7-4 and 3-0 delay straps
                // 0x006f 6-4 and 2-0 strap register
                unsigned int tx_delay = 0x7;
                unsigned int rx_delay = 0x7;
                mdio_write_register(tse_phy, phy_addr, 0x86, ((tx_delay << 4) | rx_delay) );

// DP83867 Soft reset
mdio_write_register(tse_phy, phy_addr, 0x001F, 0x4000);

            }
            break;
  
        default:
            tse_info->phy_model = GE_NOT_SUPPORTED;
            gu_fprintf("\nERROR Ethernet PHY not found\n");
            break;
    };
    
    PRINT_ASSERT((tse_info->phy_model != GE_NOT_SUPPORTED), "ERROR PHY chipset not supported OUT= 0x%x, ModelNumber= 0x%x", (unsigned int) OUI, (unsigned int) ModelNumber);


    PRINT_ASSERT((tse_info == pnetif->state), "ERROR tse_info != pnetif->state");
    
	// Reset should be inactive as well as transmit and receive
	PRINT_ASSERT(((IORD_ALTERA_TSEMAC_CMD_CONFIG(tse_phy) & 0x2002) == 0), "ERROR PHY not reset");

#if TSE_PHY_LIMIT_100M
    /* Cap the advertised speed at 100BASE-TX -- see TSE_PHY_LIMIT_100M.
     * Applied here, after the per-vendor PHY reset above (which restores
     * register 9 to its gigabit-capable default) and before the
     * negotiation-complete wait below, so negotiation settles directly on
     * the capped advertisement. gemrtos_UpdateTSEInfo()'s runtime
     * auto-negotiation restart only writes register 0, never the
     * advertisement, so the cap persists across every runtime reconnect
     * without being reapplied. Register 9 / register 0 are standard
     * clause-22, identical on both supported PHYs. */
    {
        unsigned int gbcr = mdio_read_register(tse_phy, phy_addr, 0x09);
        mdio_write_register(tse_phy, phy_addr, 0x09, gbcr & ~0x0300U);   /* clear bits 9:8 (advertise 1000 FD/HD) */
        mdio_write_register(tse_phy, phy_addr, 0x00,
                            mdio_read_register(tse_phy, phy_addr, 0x00) | 0x1200U);  /* AN enable + restart AN */
        gu_fprintf("TSE: 1000BASE-T advertisement withdrawn (TSE_PHY_LIMIT_100M) -- link capped at 100BASE-TX\n");
    }
#endif

	GU_FPRINTF(stderr, "Waiting for AUTO_NEGOTATION_COMPLETE\n");

    /* DO NOT REMOVE -AUTONEGOTATION SHOULD BE COMPLETED. Bounded, yielding
     * wait -- same TSE_PHY_NEGOTIATION_TIMEOUT_MS/_POLL_INTERVAL_MS pattern
     * gemrtos_UpdateTSEInfo() already uses for the identical wait on the
     * runtime link-recovery path (IEEE 802.3 clause 28 permits several
     * seconds; a boot-time PHY that never completes negotiation must not
     * monopolize this processor forever the way an unbounded busy-wait
     * would). */
    {
        G_UINT32 negotiation_elapsed_ms = 0U;
        while (((mdio_read_register(tse_phy, phy_addr, 0x1) & (unsigned int) pow(2,5)) == 0) &&
               (negotiation_elapsed_ms < TSE_PHY_NEGOTIATION_TIMEOUT_MS)) {
            gu_TaskDelay(0, 0, 0, TSE_PHY_NEGOTIATION_POLL_INTERVAL_MS);
            negotiation_elapsed_ms += TSE_PHY_NEGOTIATION_POLL_INTERVAL_MS;
        }
        if (negotiation_elapsed_ms >= TSE_PHY_NEGOTIATION_TIMEOUT_MS) {
            GU_FPRINTF(stderr, "PHY AUTO_NEGOTIATION_COMPLETE never set! (tse_mac_init)\n");
        }
    }

	// Update the link information
    gemrtos_UpdateTSEInfo(pnetif);
    gemrtos_InitialiseTSE(pnetif);  // Initialise TSE if info changed

	return tse_info->link_alive;
}


u32_t sys_now(void)
{
	TIMEPRIORITY value64;
	value64.i64 = (G_UINT64)(gu_SystemTimeGet() / (G_UINT64) G_TICKS_PER_MSECOND);
	return (u32_t) value64.i64;
}


// Function which initializes the LwIP TCP/IP stack
void lwip_initialize(int waitForAll)
{

#if MY_TIMER
	// Setup the timer task needed for LwIP book keeping
	// For that we need a semaphore
	if (sys_sem_new(&lwip_timer_semaphore, 0) != ERR_OK) {
		printf("[LwIP] Failed to create the timer semaphore, aborting initialize.\n");
		return;
	}

	// A task in a high priority which waits for the timer semaphore to be released
	// this way it will be scheduled ASAP when the timer callback returns
	if (sys_thread_new("LwIP timer", lwip_process_timers, NULL, KB(2), TCPIP_THREAD_PRIO) == NULL) {
		printf("[LwIP] Couldn't create timer timer task, aborting initialize.\n");
		sys_sem_free(&lwip_timer_semaphore);
		return;
	}
#endif


	// Initialize the TCP/IP stack and give our configuration function as callback
	// not for NO_SYS tcpip_init(lwip_handle_interfaces, NULL);
	lwip_init();

}


void netif_status_callback(struct netif *pnetif)
{
    // get IP and stuff
    GU_FPRINTF(stderr, "[ethernet] Acquired IP address via DHCP client for interface: %.*s\n", (int) sizeof(((struct netif *)0)->name), pnetif->name);

    GU_FPRINTF(stderr, "[ethernet] IP address : %s\n", ip4addr_ntoa(netif_ip4_addr(pnetif)));
    GU_FPRINTF(stderr, "[ethernet] Subnet     : %s\n", ip4addr_ntoa(netif_ip4_netmask(pnetif)));
    GU_FPRINTF(stderr, "[ethernet] Gateway    : %s\n", ip4addr_ntoa(netif_ip4_gw(pnetif)));

    /* Forward to the application-registered status callback (lwip_set_status_callback()),
     * if any. This forwarding call was once missing -- status_callback was
     * being SET (lwip_set_status_callback()) but never READ anywhere reachable (the one
     * call site that did, lwip_status_callback(), has been commented out since before
     * this file's current history begins), so every application-level status callback
     * registered through this mechanism -- across every example and hellogemrtos.c --
     * has been silently unreachable dead code. Confirmed via two independent hardware
     * captures (09_network_httpd): StatusCallback()'s own IP/gateway
     * subnet-coherence check printed "Acquired IP address"/"IP
     * address"/"Subnet"/"Gateway" (all printed directly by THIS function, not by the
     * unreachable callback) but never "IP validation PASSED"/"FAILED", on either run. */
    if (status_callback != NULL) {
        status_callback(pnetif);
    }
}

/**
 * \brief Forwards a link-state change to the application-registered link callback
 *        (lwip_set_link_callback()), if any.
 *
 * lwIP core never calls this on its own -- an application must register it via
 * netif_set_link_callback(pnetif, netif_link_callback_dispatch()) at the same point it
 * calls netif_set_status_callback(pnetif, netif_status_callback()), for the same reason:
 * lwip_set_link_callback() only stores the callback pointer, it does not wire it to
 * anything. This dispatcher accompanies the netif_status_callback() forwarding
 * fix above -- see that function's comment for the full finding.
 *
 * \param [in] pnetif  The lwIP network interface whose link state changed.
 */
void netif_link_callback_dispatch(struct netif *pnetif)
{
    if (link_callback != NULL) {
        link_callback(pnetif);
    }
}

/* @Function Description -  TSE transmit API to send data to the MAC
 *                          
 * 
 * @API TYPE - Public
 * @param  net  - NET structure associated with the TSE MAC instance
 * @param  data - pointer to the data payload
 * @param  data_bytes - number of bytes of the data payload to be sent to the MAC
 * @return SUCCESS if success, else a negative value
 */
err_t tse_mac_raw_send(struct netif *pnetif, struct pbuf *pkt)
{
    gemrtos_tse_system_info * tse_info = (gemrtos_tse_system_info *) pnetif->state;
	pbuf_copy_partial(pkt, tse_info->tx_frame, pkt->tot_len, 0);
	gu_MessageQueueSend(tse_info->p_tse_queue_out, (char *) tse_info->tx_frame, pkt->tot_len, 0);	
	
	return ERR_OK;
}


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void low_level_init(struct netif *netif)
{
	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
    
    GU_FPRINTF(stderr, "LWIP = %s, %d \n", __FUNCTION__, __LINE__);
	
	/* maximum transfer unit */
	netif->mtu = IP_FRAG_MAX_MTU;
	
	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	
#if LWIP_IGMP
	netif->flags |= NETIF_FLAG_IGMP;
#endif

	tse_mac_init(netif);
	GU_FPRINTF(stderr, "low_level_init: netif->state = %p\n", (void *) ((gemrtos_tse_system_info *) (netif->state))->tse_mac_base);
}



netif_status_callback_fn lwip_set_status_callback(netif_status_callback_fn callback)
{
	netif_status_callback_fn old = status_callback;

	status_callback = callback;

	return old;
}

netif_status_callback_fn lwip_set_link_callback(netif_status_callback_fn callback)
{
	netif_status_callback_fn old = link_callback;

	link_callback = callback;

	return old;
}



int lwip_is_interface_up(struct netif *netif)
{
	int *tse_phy;
    
	// Get the TSE information
	gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *) netif->state;
	tse_phy = (int *) tse_info->tse_mac_base;
    unsigned int phy_addr = get_pointed_field(tse_info, tse_phy_mdio_address);

	// Check the PHY chip
    tse_info->link_alive = ((mdio_read_register(tse_phy, phy_addr, 0x11) & 0x400) == 0) ? G_FALSE : G_TRUE;

	return (tse_info->link_alive == G_TRUE) ? ETH_INTERFACE_UP : ETH_INTERFACE_DOWN;
}



int netif_ip_is_up(struct netif *netif)
{
    if (netif == NULL) return 0;

    /* debe estar marcado como UP y link fÃ­sico arriba */
    if (!netif_is_up(netif) || !netif_is_link_up(netif)) return 0;

    /* comprobar que no sea 0.0.0.0 */
#if LWIP_IPV4
    if (ip4_addr_isany_val(*netif_ip4_addr(netif))) return 0;
#endif

#if LWIP_DHCP
    /* si DHCP estÃ¡ siendo usado, asegurar que llegÃ³ a BOUND */
    struct dhcp *dhcp = netif_dhcp_data(netif);
    if (dhcp != NULL) {
        if (dhcp->state != DHCP_STATE_BOUND) return 0;
    }
#endif

    /* si pasÃ³ todas las comprobaciones, la IP estÃ¡ establecida */
    return 1;
}


err_t netif_tse_init(struct netif *pnetif)
{

	
	PRINT_ASSERT((pnetif != NULL), "pnetif != NULL");

	#if LWIP_NETIF_HOSTNAME
	  /* Initialize interface hostname */
	  pnetif->hostname = "lwip";
	#endif /* LWIP_NETIF_HOSTNAME */
	
	/*
	* Initialize the snmp variables and counters inside the struct netif.
	* The last argument should be replaced with your link speed, in units
	* of bits per second.
	*/
	NETIF_INIT_SNMP(*pnetif, snmp_ifType_ethernet_csmacd, 100000000);

	/* We directly use etharp_output() here to save a function call.
	* You can instead declare your own function an call etharp_output()
	* from it if you have to do some checks before sending (e.g. if link
	* is available...) */
	pnetif->output = etharp_output;
	pnetif->linkoutput = tse_mac_raw_send;
	pnetif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;


	/* initialize the low level hardware */
	low_level_init(pnetif);

	return ERR_OK;
}


const char* print_ipad(alt_u32 ip, char* buf)
{
	alt_u8 bip[4];
	alt_u32 idx = 0;

	for (; idx < sizeof(bip); idx++) {
		bip[idx] = ip & 0xFF;
		ip >>= 8;
	}

	snprintf(buf, 17, "%d.%d.%d.%d", bip[0], bip[1], bip[2], bip[3]);

	return buf;
}
 
