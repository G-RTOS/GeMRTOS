/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Network device state, topology wiring, and lwIP lifecycle
 *         callbacks for 09_network_httpd. See net_init.h for what each
 *         piece is responsible for.
 *  \version 1.0
 *  \date      2026-08-03
 *
 *  SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
 *  Copyright (c) 2013-2026 GeMRTOS Project Contributors
 *
 *  Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)
 *
 *  This file is part of GeMRTOS, licensed under the PolyForm Noncommercial
 *  License 1.0.0. You may use, copy, modify, and distribute this file for
 *  any noncommercial purpose. Commercial use requires a separate written
 *  license from GeMRTOS. Full terms:
 *  https://polyformproject.org/licenses/noncommercial/1.0.0
 */

#include "net_init.h"

/* MISRA C:2012 Rule 21.3: static storage replaces malloc. */
static struct netif  tse_netif_static;
static unsigned char tse_tx_frame_static[2048U] __attribute__((aligned(4)));

/* DMA frame buffers -- must be in on-chip memory. */
unsigned char buffer_tx_frame[TSE_DMA_FRAME_BUFFER_SIZE] __attribute__((aligned(4), section(".tse_buffers")));
unsigned char buffer_rx_frame[TSE_DMA_FRAME_BUFFER_SIZE] __attribute__((aligned(4), section(".tse_buffers")));

/* One-element array: all information for this example's single Ethernet
 * interface. */
gemrtos_tse_system_info gemrtos_tse_mac_device[1] __attribute__((aligned(4))) = {
    GEMRTOS_TSE_SYSTEM("en", ETH_TSE, 1, MSGDMA_TX, MSGDMA_RX, 0),
};

G_UINT32 net_init_topology(void)
{
    /* Network Interface structure for lwIP */
    gemrtos_tse_mac_device[0].pnetif = &tse_netif_static;
    /* Relates netif and gemrtos_tse_mac_device structures */
    gemrtos_tse_mac_device[0].pnetif->state = (void *) &gemrtos_tse_mac_device[0];

    /* Name must match the device associated with this entry in
     * gemrtos_tse_mac_device[]. */
    gemrtos_tse_mac_device[0].pnetif->name[0] = 'e';
    gemrtos_tse_mac_device[0].pnetif->name[1] = 'n';

#ifdef __niosX_arch__
    /* Load platform specific MAC address into netif.
     * !!!  Replace TSE_MAC_BYTE_* constants (net_init.h) with your hardware MAC.  !!! */
    gemrtos_tse_mac_device[0].pnetif->hwaddr_len = ETH_HWADDR_LEN;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[0] = TSE_MAC_BYTE_0;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[1] = TSE_MAC_BYTE_1;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[2] = TSE_MAC_BYTE_2;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[3] = TSE_MAC_BYTE_3;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[4] = TSE_MAC_BYTE_4;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[5] = TSE_MAC_BYTE_5;

    if (strcmp(ALT_DEVICE_FAMILY, "AGILEX3") == 0) {
        gemrtos_tse_mac_device[0].pnetif->hwaddr[0] = TSE_MAC_AGILEX3_BYTE_0;
        gemrtos_tse_mac_device[0].pnetif->hwaddr[1] = TSE_MAC_AGILEX3_BYTE_1;
    }

    /* Create message queue for TSE transmission */
    gemrtos_tse_mac_device[0].p_tse_queue_out = gu_MessageQueueCreate();
    if (gemrtos_tse_mac_device[0].p_tse_queue_out == (GS_RCB *) 0) {
        return (G_FALSE);
    }
    gemrtos_tse_mac_device[0].tx_frame = tse_tx_frame_static;
    PRINT_ASSERT(((unsigned int) gemrtos_tse_mac_device[0].tx_frame % 4 == 0), "ERROR FRAME NOT ALIGNED");
    PRINT_ASSERT(((unsigned int) &buffer_tx_frame[0] % 4 == 0), "ERROR FRAME NOT ALIGNED");
    PRINT_ASSERT(((unsigned int) &buffer_rx_frame[0] % 4 == 0), "ERROR FRAME NOT ALIGNED");
    PRINT_ASSERT(((void *) &buffer_rx_frame[0] == (void *) buffer_rx_frame),
                 "ERROR FRAME NOT ALIGNED, &buffer_rx_frame[0]= %p, (void *) buffer_rx_frame= %p",
                 (void *) &buffer_rx_frame[0], (void *) buffer_rx_frame);

    gemrtos_tse_mac_device[0].buffer_tx_frame = (unsigned char *) &buffer_tx_frame;
    gemrtos_tse_mac_device[0].buffer_rx_frame = (unsigned char *) &buffer_rx_frame;
#endif  // __niosX_arch__
    /* On host, netif_pcap_init() (pcap_netif.c) sets pnetif->hwaddr from
     * PCAP_NETIF_MAC_BYTE_* when tse_receive_server()'s host body calls
     * netif_add() -- there is no hardware MAC to load here, and the TX
     * message-queue indirection above exists only for the mSGDMA path. */

    return (G_TRUE);
}

void StatusCallback(struct netif *pnetif)
{
    char buf[255];

    gu_fprintf("[ethernet] Acquired IP address via DHCP client for interface: %s\n", pnetif->name);

    print_ipad(pnetif->ip_addr.addr, buf);
    gu_fprintf("[ethernet] IP address : %s\n", buf);

    print_ipad(pnetif->netmask.addr, buf);
    gu_fprintf("[ethernet] Subnet     : %s\n", buf);

    print_ipad(pnetif->gw.addr, buf);
    gu_fprintf("[ethernet] Gateway    : %s\n", buf);

    /* Validate that the assigned IP and gateway are coherent, i.e. reachable
     * on the same subnet. An incoherent pair indicates a stale lease from a
     * previous network segment. A gateway of 0.0.0.0 means none was offered
     * at all -- a valid "no default route" configuration, not a mismatch --
     * so it is accepted without comparing subnets (this file has its own
     * copy of the same check hellogemrtos.c has and needed the identical
     * fix). */
    if (pnetif->gw.addr == IPADDR_ANY) {
        gu_fprintf("[ethernet] IP validation PASSED: no gateway offered (no default route)\n");
        return;
    }

    /* An IP of 0.0.0.0 is the normal transient state right after
     * dhcp_release() (e.g. on LINK DOWN, LinkCallback() releases the lease
     * before this callback fires) -- not a wrong-subnet condition. Without
     * this guard, a stale gw.addr from the previous lease (not yet cleared)
     * always fails the subnet-coherence check below and re-triggers
     * dhcp_release()/dhcp_start() from inside this status callback, which
     * lwIP itself calls synchronously from within dhcp_release()'s own call
     * chain -- direct DHCP-client re-entrancy. Reproduced on real hardware
     * (2026-08-17): a physical link down/up cycle drove exactly this path
     * and hung the processor running this task permanently, with this
     * function's own "Triggering DHCP re-negotiation..." line the last
     * output ever printed. */
    if (pnetif->ip_addr.addr == IPADDR_ANY) {
        gu_fprintf("[ethernet] IP validation PASSED: no IP yet (DHCP in progress)\n");
        return;
    }

    G_UINT32 ip_network = pnetif->ip_addr.addr & pnetif->netmask.addr;
    G_UINT32 gw_network = pnetif->gw.addr & pnetif->netmask.addr;

    if (ip_network != gw_network) {
        gu_fprintf("[ERROR] IP validation FAILED: IP and gateway not in same subnet\n");
        gu_fprintf("[ERROR]   IP network:      0x%08lx\n", (unsigned long) ip_network);
        gu_fprintf("[ERROR]   Gateway network: 0x%08lx\n", (unsigned long) gw_network);
        gu_fprintf("[ERROR] Triggering DHCP re-negotiation...\n");

        dhcp_release(pnetif);
        dhcp_start(pnetif);
    } else {
        gu_fprintf("[ethernet] IP validation PASSED: configuration is coherent\n");
    }
}

/* On LINK UP, only the event is logged; DHCP restart is driven by the
 * caller (tse_receive_server()'s main loop in tse_servers.c), not by this
 * callback. Stated explicitly because its absence here once read as a
 * missing dhcp_start() call -- it is not: tse_servers.c already has the
 * PHY-poll and not-bound safety net (2026-09-03) that decide when to
 * reconnect and call dhcp_start() through tse_force_reconnect(). */
void LinkCallback(struct netif *pnetif)
{
    if (netif_is_link_up(pnetif)) {
        gu_fprintf("[ethernet] Link Callback: LINK UP for interface: %s\n", pnetif->name);
    } else {
        char buf[255];

        print_ipad(pnetif->ip_addr.addr, buf);
        gu_fprintf("[ethernet] Link Callback: LINK DOWN for interface: %s\n", pnetif->name);
        gu_fprintf("[ethernet]   Previous IP released: %s\n", buf);

        /* Release the DHCP lease immediately so the next dhcp_start() performs
         * a DISCOVERY rather than a RENEW against a network that may no longer be
         * reachable through this cable. */
        dhcp_stop(pnetif);
        dhcp_release(pnetif);
    }
}
