/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief TSE receive and transmit server tasks for 09_network_httpd.
 *         Extracted from examples/10_full_demo/hellogemrtos.c as a
 *         behaviour-preserving move -- the bodies of
 *         both functions are unchanged from the original; only their
 *         surrounding file changed. See tse_servers.h for what each does
 *         and README.md "Extraction notes" for exactly what moved where.
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

#include "tse_servers.h"

#ifndef __niosX_arch__
/* Host body. Same treatment as
 * examples/10_full_demo/hellogemrtos.c's tse_receive_server() -- this file
 * was extracted from that one (see file header) but never got the host-body
 * update applied to its own copy, which verify_windows.ps1 caught by
 * actually trying to build and run this example on Windows. Same reasoning
 * as there: no PHY debounce state machine (a real OS-managed NIC already
 * owns link state), everything from lwip_init() through the httpd/SSI/CGI
 * setup is copied verbatim from the embedded body below, because that
 * layer does not change. */
void tse_receive_server(gemrtos_tse_system_info *pgemrtos_tse_mac_device)
{
    struct netif *pnetif = pgemrtos_tse_mac_device->pnetif;

    lwip_set_status_callback(StatusCallback);
    lwip_set_link_callback(LinkCallback);

    lwip_init();

    if (netif_add(pnetif, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  pnetif->state, netif_pcap_init, netif_input) == NULL) {
        GU_FPRINTF(stderr, "Fatal error initializing pcap netif...\n");
        for (;;);
    }
    /* Same diagnostic line the embedded body prints after its own netif_add()
     * (kept for output-fidelity parity between platforms --
     * verify_windows.ps1 reuses each example's verify.expect verbatim
     * across both tiers, and this line is one of its patterns). */
    GU_FPRINTF(stderr, "tse_receive_server: netif->state = %p\n", (void *) pnetif->state);

    netif_set_status_callback(pnetif, netif_status_callback);
    netif_set_link_callback(pnetif, netif_link_callback_dispatch);
    netif_set_default(pnetif);
    netif_set_up(pnetif);
    netif_set_link_up(pnetif);

    #if USE_DHCP
        dhcp_start(pnetif);
    #else
        ip_addr_t lwipStaticIp;
        IP4_ADDR(&lwipStaticIp, 192, 168, 0, 15);
        netif_set_ipaddr(pnetif, &lwipStaticIp);
        netif_set_up(pnetif);
        dhcp_inform(pnetif);
    #endif

    GU_FPRINTF(stderr, "IP address= %s\n", ip_ntoa(&pnetif->ip_addr));

    #if BUILD_HTTPD
        http_set_ssi_handler(gemrtos_web_ssi_handler, gemrtos_web_ssi_tags,
                              (int) (sizeof(gemrtos_web_ssi_tags) / sizeof(gemrtos_web_ssi_tags[0])));

        #if LWIP_HTTPD_CGI
            http_set_cgi_handlers(gemrtos_web_cgi_table,
                                  (int) (sizeof(gemrtos_web_cgi_table) / sizeof(gemrtos_web_cgi_table[0])));
        #endif

        httpd_init();
    #endif

    while (1) {
        struct pbuf *p = pcap_netif_poll_receive(pnetif);

        if (p != NULL) {
            if (pnetif->input(p, pnetif) != ERR_OK) {
                GU_FPRINTF(stderr, "pnetif->input(p, pnetif) != ERR_OK \n");
                pbuf_free(p);
            }
        }

        sys_check_timeouts();
        gu_TaskDelay(0, 0, 0, 5);
    }
}

void tse_transmit_server(void *pdata)
{
    (void) pdata;
    for (;;) {
        gu_TaskDelay(0, 0, 1, 0);
    }
}
#else
/**
 * \brief Read only the PHY's raw link-status bit -- no MDIO write, ever.
 *
 * Deliberately does none of what gemrtos_UpdateTSEInfo() (lwip_main.c) does:
 * no speed/duplex read, no comparison against stored state, and above all
 * no possibility of triggering that function's embedded auto-negotiation
 * restart. Exists so the periodic poll in tse_receive_server() can ask
 * "does this look different?" as many times as it wants, on a fixed
 * schedule, at zero risk -- see PHY_POLL_DEBOUNCE_SAMPLES for why that
 * question must never be answered by the heavier function directly.
 *
 * Reads the identical register/bit gemrtos_UpdateTSEInfo() reads for
 * link_alive (register 0x11, bit 0x400) so the two never disagree about
 * what "link alive" means.
 *
 * \param [in] pnetif  Pointer to the lwIP network interface.
 * \return G_TRUE if the PHY currently reports link alive, G_FALSE otherwise.
 */
static G_UINT32 tse_raw_link_alive(struct netif *pnetif)
{
    gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *) pnetif->state;
    int *tse_phy = (int *) tse_info->tse_mac_base;
    unsigned int phy_addr = get_pointed_field(tse_info, tse_phy_mdio_address);

    return ((mdio_read_register(tse_phy, phy_addr, 0x11) & 0x400U) != 0U) ? G_TRUE : G_FALSE;
}

/**
 * \brief Debounce the physical link, then bring the netif up and (re)start DHCP.
 *
 * Waits for the PHY to report link-alive across LINK_DEBOUNCE_SAMPLES
 * consecutive reads, LINK_DEBOUNCE_INTERVAL_MS apart (a single PHY register
 * read is susceptible to transient bounce while a connector mates or a link
 * partner renegotiates) before trusting the link. Used identically at boot
 * and on every runtime link-change event, so both give the DHCP client the
 * same stability guarantee -- previously the boot path skipped debounce
 * entirely and connected on a single, unconfirmed PHY read while the
 * runtime path required 5 stable reads; there was no technical reason for
 * the two to differ, and unifying them costs ~200ms at boot in the common
 * case (LINK_DEBOUNCE_SAMPLES x LINK_DEBOUNCE_INTERVAL_MS), negligible.
 *
 * The debounce loop itself never busy-waits: gu_TaskDelay() between every
 * PHY read yields the processor to other tasks for the full duration,
 * including the case where the link never comes back (production-lens
 * requirement -- a missing cable must not monopolize a processor).
 *
 * \param [in,out] pnetif  Pointer to the lwIP network interface.
 */
static void tse_link_bring_up(struct netif *pnetif)
{
    gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *) pnetif->state;
    G_UINT32 link_confirmed_stable = G_FALSE;

    /* A reconnect (as opposed to first boot) means whatever was on the other
     * end of the cable a moment ago -- gateway, DHCP server -- may not be
     * the same device any more, or may have forgotten this interface's MAC
     * entirely (e.g. a modem/router reboot). netif_set_link_down() (called
     * by every caller of this function, immediately before it) does not
     * purge the ARP cache on its own -- that only happens for
     * netif_set_down(), a different, admin-state call this code never
     * makes. Flushing it here means the reconnect never attempts to reach
     * the old gateway's now-possibly-stale hardware address. Harmless at
     * boot, where the cache is already empty. */
    etharp_cleanup_netif(pnetif);

    while (!link_confirmed_stable) {
        unsigned int stable_reads;

        do {
            if ((gemrtos_UpdateTSEInfo(pnetif) == G_TRUE) && (tse_info->link_alive)) {
                gemrtos_InitialiseTSE(pnetif);  /* Initialise TSE if info changed */
            }
            if (!(tse_info->link_alive)) gu_TaskDelay(0, 0, 0, 200);
        } while (!(tse_info->link_alive));

        for (stable_reads = 1U; stable_reads < LINK_DEBOUNCE_SAMPLES; stable_reads++) {
            gu_TaskDelay(0, 0, 0, LINK_DEBOUNCE_INTERVAL_MS);
            gemrtos_UpdateTSEInfo(pnetif);
            if (!(tse_info->link_alive)) {
                break;  /* bounce -- restart outer wait */
            }
        }

        if (stable_reads == LINK_DEBOUNCE_SAMPLES) {
            link_confirmed_stable = G_TRUE;
        }
    }

    /* Physical link confirmed alive (debounced) -- notify lwIP, then restart address config. */
    netif_set_link_up(pnetif);

    #if USE_DHCP
        dhcp_stop(pnetif);
        dhcp_start(pnetif);
    #else
        ip_addr_t lwipStaticIp;
        IP4_ADDR(&lwipStaticIp, 192, 168, 0, 15);
        netif_set_ipaddr(pnetif, &lwipStaticIp);
        netif_set_up(pnetif);
        dhcp_inform(pnetif);
    #endif
}

/**
 * \brief Notify lwIP the link went down, bring it back up, and record when.
 *
 * The single place that performs a runtime reconnect, called from either of
 * two independent triggers in tse_receive_server()'s main loop (an RX-path
 * anomaly, or the direct PHY-poll/not-bound safety net below) -- centralised
 * so both go through the identical sequence and the reconnect bookkeeping
 * cannot drift out of sync with the action it is counting.
 *
 * \param [in,out] pnetif               Pointer to the lwIP network interface.
 * \param [in,out] preconnect_count     Incremented on every call.
 * \param [out]    plast_reconnect_ticks  Set to the current time, for the
 *                                        "how long ago" figure in the status line.
 */
static void tse_force_reconnect(struct netif *pnetif, G_UINT32 *preconnect_count,
                                 G_UINT64 *plast_reconnect_ticks)
{
    netif_set_link_down(pnetif);
    tse_link_bring_up(pnetif);
    (*preconnect_count)++;
    *plast_reconnect_ticks = gu_SystemTimeGet();
}

void tse_receive_server(gemrtos_tse_system_info *pgemrtos_tse_mac_device)
{
    struct netif *pnetif = pgemrtos_tse_mac_device->pnetif;
    gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *)((gemrtos_tse_system_info *)(pnetif->state));

    GS_TRGStatus trigger_status;
    G_UINT32 link_is_alive;
    unsigned int timeout_count = 0;
    G_UINT32 link_has_changed = G_FALSE;

    /* Runtime reconnect robustness state -- see PHY_POLL_INTERVAL_MS and
     * DHCP_NOT_BOUND_TIMEOUT_MINUTES in tse_servers.h. All bounded by elapsed
     * real time (gu_SystemTimeGet()), never by loop-iteration or poll count,
     * so the intervals hold regardless of frame rate or system load. */
    G_UINT64 last_phy_poll_ticks       = 0U;
    unsigned int phy_poll_mismatch_count = 0U;
    G_UINT32 not_bound_timer_running   = G_FALSE;
    G_UINT64 not_bound_since_ticks     = 0U;
    G_UINT32 reconnect_count           = 0U;
    G_UINT64 last_reconnect_ticks      = 0U;
    const G_UINT64 phy_poll_interval_ticks =
        gu_ConvertTime(0, 0, 0, PHY_POLL_INTERVAL_MS);
    const G_UINT64 not_bound_timeout_ticks =
        gu_ConvertTime(0, DHCP_NOT_BOUND_TIMEOUT_MINUTES, 0, 0);

    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    /* Initialise lwIP stack, network interface, DHCP and HTTP server. */

// ##########################################

    lwip_set_status_callback(StatusCallback);
    lwip_set_link_callback(LinkCallback);

    lwip_init();

    if (netif_add(pnetif, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  pnetif->state, netif_tse_init, netif_input) == NULL) {
        /* gu_fprintf, not GU_FPRINTF: the for(;;) below is unconditional and
         * unrecoverable in every build, so this is the only diagnostic that
         * will ever exist for this failure -- it must survive into a
         * production build (G_DEBUG_WHILEFOREVER_ENABLE == 0), where
         * GU_FPRINTF compiles to nothing and this would otherwise be
         * a silent infinite loop with zero explanation. */
        gu_fprintf("Fatal error initializing...\n");
        for (;;);
    }
    gu_fprintf("tse_receive_server: netif->state = %p\n",
               (void *) ((gemrtos_tse_system_info *) (pnetif->state))->tse_mac_base);

    /* Create the receive trigger with TIMEOUT_TSE_RECEIVE_MS timeout */
    G_UINT64 timeout = gu_ConvertTime(0, 0, 0, TIMEOUT_TSE_RECEIVE_MS);
    tse_info->sgdma_trigger_receive = gu_TriggerCreate(tse_info->tse_msgdma_rx_irq, timeout);
    if (tse_info->sgdma_trigger_receive == (GS_RCB *) 0) {
        gu_printf("Error when creating sgdma_trigger_receive\n");
        while (1);
    }

    /* Set periodic timeout type */
    gu_TriggerSetTimeoutType(tse_info->sgdma_trigger_receive,
                             GS_TRGTimeOutType_restart_when_timeout);

    /* Register this task as the DMA receive ISR */
    gu_TriggerRegisterTask(ptcb, tse_info->sgdma_trigger_receive);

    netif_set_status_callback(pnetif, netif_status_callback);
    /* Wires StatusCallback()/LinkCallback() (net_init.c) to actually run -- without this,
     * lwip_set_status_callback()/lwip_set_link_callback() only store a pointer that
     * nothing reads. See netif_status_callback()'s comment in lwip_main.c for
     * the full finding; this line and the dispatch functions it wires are the fix. */
    netif_set_link_callback(pnetif, netif_link_callback_dispatch);
    netif_set_default(pnetif);
    netif_set_up(pnetif);

    /* Confirm the physical link (debounced) before starting DHCP -- the same
     * tse_link_bring_up() sequence every runtime reconnect uses below.
     * Previously this boot path trusted a single, unconfirmed PHY read;
     * unified so boot and runtime reconnection give DHCP the same
     * stability guarantee (2026-08-17, requested by Ricardo after the
     * DHCP-reentrancy hang investigation). */
    netif_set_link_down(pnetif);
    tse_link_bring_up(pnetif);

    gu_fprintf("IP address= %s\n", ip_ntoa(&pnetif->ip_addr));

    #if BUILD_HTTPD
        /* Register the system-status SSI handler before starting the server, so
         * the web status page (fs/status.html, tag <!--#sysvars-->) can render
         * live values from the first request onward. See gemrtos_web_ssi_handler()
         * and gemrtos_web_sources[] (web_sources.c) for how tags are resolved to
         * content. */
        http_set_ssi_handler(gemrtos_web_ssi_handler, gemrtos_web_ssi_tags,
                              (int) (sizeof(gemrtos_web_ssi_tags) / sizeof(gemrtos_web_ssi_tags[0])));

        #if LWIP_HTTPD_CGI
            /* Register the "/set.cgi" form handler that lets the web page change
             * the periodic task's period and the infinite-loop task's delay at
             * runtime. See gemrtos_web_cgi_set_handler() (web_sources.c). */
            http_set_cgi_handlers(gemrtos_web_cgi_table,
                                  (int) (sizeof(gemrtos_web_cgi_table) / sizeof(gemrtos_web_cgi_table[0])));
        #endif

        /* Start HTTPD */
        httpd_init();
    #endif

// ##########################################

    /* Reset the dispatcher and wait until finishing */
    IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_rx_csr_base,
                                   ALTERA_MSGDMA_CSR_RESET_MASK);
    while (IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base)
           & ALTERA_MSGDMA_CSR_RESET_STATE_MASK);

    /* Clear msgDMA status */
    IOWR_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base, 0xFFFFFFFFu);

    /* Main loop */
    while (1) {
        PRINT_ASSERT((!(gm_IsPrcGranted())), "ERROR in task MTX task= %s, TCB_MTX_NESTED= %u\n",
                     g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_description,
                     (unsigned int) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED);

        /* Direct, RX-traffic-independent PHY poll (PHY_POLL_INTERVAL_MS,
         * tse_servers.h). Two stages, deliberately: a cheap, side-effect-free
         * raw read every tick (tse_raw_link_alive()), and the heavier
         * gemrtos_UpdateTSEInfo() -- which can restart PHY negotiation for
         * real, see PHY_POLL_DEBOUNCE_SAMPLES -- only once
         * PHY_POLL_DEBOUNCE_SAMPLES consecutive ticks agree something
         * actually changed. A change confirmed this way feeds the same
         * link_has_changed flag the RX-idle/DMA-error triggers below use, so
         * it goes through the identical, already-verified confirm-or-dismiss
         * logic in the block right after this one -- nothing about that
         * logic needed to change, only how it gets woken up. */
        if ((gu_SystemTimeGet() - last_phy_poll_ticks) >= phy_poll_interval_ticks) {
            last_phy_poll_ticks = gu_SystemTimeGet();

            if (tse_raw_link_alive(pnetif) != tse_info->link_alive) {
                phy_poll_mismatch_count++;
                if (phy_poll_mismatch_count >= PHY_POLL_DEBOUNCE_SAMPLES) {
                    phy_poll_mismatch_count = 0U;
                    if (gemrtos_UpdateTSEInfo(pnetif)) {
                        link_has_changed = G_TRUE;
                    }
                }
            } else {
                phy_poll_mismatch_count = 0U;
            }
        }

        if (link_has_changed == G_TRUE) {
            link_has_changed = G_FALSE;

            /* link_has_changed is also raised by RX-idle (30 consecutive RX-trigger
             * timeouts, ~7.5s with zero frames received), by an mSGDMA descriptor
             * error, and by the direct PHY poll above -- none of these guarantee
             * the PHY link actually changed. Confirm against the PHY before
             * tearing down lwIP/DHCP: a spurious wake-up (link never left the up
             * state) must not flush the ARP cache or force a full DHCP restart.
             * Investigation: a quiet
             * network segment (no incoming frames for >~7.5s) was cycling DHCP from
             * BOUND back to a fresh DISCOVER indefinitely, even though the physical
             * link never dropped. */
            G_UINT32 tse_info_changed = gemrtos_UpdateTSEInfo(pnetif);
            if (tse_info_changed && tse_info->link_alive) {
                gemrtos_InitialiseTSE(pnetif);  /* keep MAC config in sync, same as the debounce loop below */
            }

            if (!(tse_info->link_alive) || tse_info_changed) {
                /* Debounce, then bring the link back up and restart DHCP --
                 * the same sequence tse_link_bring_up() runs at boot. */
                tse_force_reconnect(pnetif, &reconnect_count, &last_reconnect_ticks);
// ########################
            }
            /* else: false alarm (RX-idle or a transient DMA hiccup) -- PHY confirmed
             * unchanged, nothing to reconfigure. */
        }

        /* Defence-in-depth safety net (DHCP_NOT_BOUND_TIMEOUT_MINUTES,
         * tse_servers.h): force a reconnect if the interface has looked
         * link-alive without ever completing DHCP for too long, regardless of
         * why -- including a cause this code does not (yet) have a name for.
         * Runs every pass, not gated on link_has_changed, because its whole
         * purpose is to catch the case where nothing here ever noticed a
         * problem. */
        if (tse_info->link_alive && !netif_ip_is_up(pnetif)) {
            if (!not_bound_timer_running) {
                not_bound_timer_running = G_TRUE;
                not_bound_since_ticks = gu_SystemTimeGet();
            } else if ((gu_SystemTimeGet() - not_bound_since_ticks) >= not_bound_timeout_ticks) {
                not_bound_timer_running = G_FALSE;
                gu_fprintf("[ethernet] No DHCP lease after %u minute(s) with link up -- forcing reconnect\n",
                           (unsigned int) DHCP_NOT_BOUND_TIMEOUT_MINUTES);
                tse_force_reconnect(pnetif, &reconnect_count, &last_reconnect_ticks);
            }
        } else {
            /* Bound, or link not alive (the other paths above own that case) --
             * either way there is nothing to time. */
            not_bound_timer_running = G_FALSE;
        }

        /* Re-arm the mSGDMA RX descriptor for the next frame */
        unsigned int control;
    #ifdef __ALTERA_MSGDMA
        /* Wait while mSGDMA is busy */
        while ((IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base) & ALTERA_MSGDMA_CSR_DESCRIPTOR_BUFFER_EMPTY_MASK) == 0);

        /* Stop the dispatcher from issuing more descriptors */
        control = ALTERA_MSGDMA_CSR_STOP_DESCRIPTORS_MASK;
        IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_rx_csr_base, control);

        /* Clear any previous status register information that might occlude error checking */
        IOWR_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base,
                                       IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base));

        /* Create the descriptor for receiving the next frame into buffer_rx_frame */
        IOWR_ALTERA_MSGDMA_DESCRIPTOR_WRITE_ADDRESS(tse_info->tse_msgdma_rx_descriptor_slave_base,
                                                     (alt_u32) tse_info->buffer_rx_frame);
        IOWR_ALTERA_MSGDMA_DESCRIPTOR_LENGTH(tse_info->tse_msgdma_rx_descriptor_slave_base,
                                              TSE_DMA_FRAME_BUFFER_SIZE);

        /* Run; stop on error */
        control = (ALTERA_MSGDMA_CSR_STOP_ON_ERROR_MASK |
                   ALTERA_MSGDMA_CSR_GLOBAL_INTERRUPT_MASK |
                   ALTERA_MSGDMA_CSR_STOP_ON_EARLY_TERMINATION_MASK);
        control &= (~ALTERA_MSGDMA_CSR_STOP_DESCRIPTORS_MASK);
        IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_rx_csr_base, control);

        IOWR_ALTERA_MSGDMA_DESCRIPTOR_CONTROL_STANDARD(tse_info->tse_msgdma_rx_descriptor_slave_base,
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_GO_MASK |
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_ERROR_IRQ_MASK |
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_EARLY_DONE_ENABLE_MASK |
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_TRANSFER_COMPLETE_IRQ_MASK |
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_END_ON_EOP_MASK);
    #else
    #error "mSGDMA module should exist for TSE support"
    #endif

        /* Log current link / DHCP status before blocking on the trigger */
        link_is_alive = lwip_is_interface_up(pnetif);
        if (tse_info->phy_speed == GE_PHY_SPEED_1000) gu_fprintf("SPEED= 1Gb, ");
        if (tse_info->phy_speed == GE_PHY_SPEED_100)  gu_fprintf("SPEED= 100Mb, ");
        if (tse_info->phy_speed == GE_PHY_SPEED_10)   gu_fprintf("SPEED= 10Mb, ");
        if (tse_info->phy_duplex == GE_PHY_DUPLEX_HALF) gu_fprintf("HALF, ");
        if (tse_info->phy_duplex == GE_PHY_DUPLEX_FULL) gu_fprintf("FULL, ");

        /* dhcp->tries and reconnects/last-reconnect-elapsed added 2026-09-03
         * so a stuck-DHCP report can be read straight off this line -- no
         * packet capture needed. dhcp->tries climbing distinguishes "DHCP is
         * genuinely retrying but getting no OFFER" (network/server-side) from
         * "the retry timer itself is not running" (a lwIP/GeMRTOS-side
         * defect); reconnects/elapsed distinguishes "this code never noticed
         * a problem" (reconnects stays 0) from "it reconnected but DHCP still
         * did not complete" (reconnects > 0, still not bound). */
        {
            struct dhcp *pdhcp = (struct dhcp *) netif_dhcp_data(pnetif);
            G_UINT32 secs_since_reconnect = (reconnect_count == 0U) ? 0U
                : (G_UINT32) ((gu_SystemTimeGet() - last_reconnect_ticks) / G_TICKS_PER_SECOND);

            gu_fprintf("dhcp->state= %u, dhcp->tries= %u, ",
                       (unsigned int) pdhcp->state, (unsigned int) pdhcp->tries);
            gu_fprintf("netif_ip_is_up= %u, ", (netif_ip_is_up(pnetif)));
            gu_fprintf("Link_alive= %u, reconnects= %u (last %u s ago), IP address: %s\n",
                       link_is_alive, (unsigned int) reconnect_count,
                       (unsigned int) secs_since_reconnect, ip_ntoa(&pnetif->ip_addr));
        }

        /* Wait until receive descriptor transfer is complete or timeout */
        while ((trigger_status = gu_TriggerWait()) != G_TRGStatus_triggered) {
            timeout_count++;
            if (timeout_count > 30) {
                /* ERROR in receiving frames -- update Ethernet link */
                timeout_count = 0;
                link_has_changed = G_TRUE;
                break;
            }
        }

        /* Cyclic lwIP timers check -- unconditional, once per outer-loop pass,
         * regardless of whether a frame arrived. Previously this call lived
         * inside the while loop above and only ran on a TIMEOUT return from
         * gu_TriggerWait(): under any steady incoming traffic (ARP, mDNS, the
         * DHCP server's own broadcasts -- nothing unusual), the trigger tends
         * to fire before its own 250ms timeout on most passes, so the loop
         * body -- and this call -- could go unserviced far longer than the
         * 500ms/60s cadence dhcp_fine_tmr()/dhcp_coarse_tmr() require. A DHCP
         * client stuck in RENEWING (state 5) waiting on a response that never
         * arrives then has nothing left to notice the request timed out or to
         * fall back to REBINDING/restart -- it stays there indefinitely, and
         * netif_ip_is_up() (which requires DHCP_STATE_BOUND) stays 0. Moving
         * the call outside the while loop guarantees it runs exactly once per
         * pass either way. */
        sys_check_timeouts();

        if (trigger_status == G_TRGStatus_triggered) {
            timeout_count = 0;

            // ****************************************************
            // PRESERV READ ORDER, OTHERWISE PROCESSOR IS HALTED
            // DO NOT READ AGAIN !!!!!!!!!!!!!!!!!!!!!!!!!!!
            G_UINT32 bytes_transferred = IORD_ALTERA_MSGDMA_RESPONSE_ACTUAL_BYTES_TRANSFERRED(tse_info->tse_msgdma_rx_response_base);
            G_UINT32 response_reg      = IORD_ALTERA_MSGDMA_RESPONSE_ERRORS_REG(tse_info->tse_msgdma_rx_response_base);
            // ****************************************************

            if ((unsigned int) response_reg != 0) {
                /* ERROR in descriptor transfer -- update Ethernet link */
                link_has_changed = G_TRUE;
                gu_fprintf("AFTER RECEIVE WHILE STATUS response_reg= 0x%x, bytes_transferred= %u\n",
                            (unsigned int) response_reg, (unsigned int) bytes_transferred);

                /* Reset the dispatcher and wait until finishing */
                IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_rx_csr_base, ALTERA_MSGDMA_CSR_RESET_MASK);
                while (IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base) & ALTERA_MSGDMA_CSR_RESET_STATE_MASK);
            }
            else
            {
                if ((bytes_transferred == 0U) ||
                    (bytes_transferred > TSE_DMA_FRAME_BUFFER_SIZE)) {
                    gu_fprintf("Dropping invalid RX frame length= %u\n", (unsigned int) bytes_transferred);
                    link_has_changed = G_TRUE;
                }
                else
                {
                    /* Check for received frames, feed them to lwIP */
                    u16_t len = (u16_t) bytes_transferred;
                    PRINT_ASSERT((bytes_transferred == (G_UINT32) len), "ERROR in len representation in 16 bits");

                    struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
                    if (p != NULL) {
                        /* Copy ethernet frame into pbuf */
                        if (pbuf_take(p, tse_info->buffer_rx_frame, len) != ERR_OK) {
                            gu_fprintf("pbuf_take failed for RX length= %u\n", (unsigned int) len);
                            pbuf_free(p);
                        }
                        else if (pnetif->input(p, pnetif) != ERR_OK) {
                            gu_fprintf("pnetif->input(p, pnetif) != ERR_OK \n");
                            pbuf_free(p);
                        }
                    }
                    else {
                        gu_fprintf("pbuf_alloc failed for RX length= %u\n", (unsigned int) len);
                    }
                }
            }
        }
    }
}

void tse_transmit_server(void *pdata)
{
    unsigned int size, control;

    gemrtos_tse_system_info *pgemrtos_tse_mac_device = (gemrtos_tse_system_info *) pdata;
    struct netif *pnetif = pgemrtos_tse_mac_device->pnetif;

    gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *) ((gemrtos_tse_system_info *) (pnetif->state));

    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    /* Subscribe task as consumer to the producer/consumer TSE queue */
    PRINT_ASSERT(((void *) pgemrtos_tse_mac_device->buffer_tx_frame == (void *) &buffer_tx_frame), "ERROR tse_info->buffer_tx_frame= %p, &buffer_tx_frame= %p\n", (void *) pgemrtos_tse_mac_device->buffer_tx_frame, (void *) &buffer_tx_frame);
    gu_MessageQueueSubscribe(ptcb, pgemrtos_tse_mac_device->p_tse_queue_out,
                             (void *) pgemrtos_tse_mac_device->buffer_tx_frame,
                             TSE_TX_QUEUE_BUFFER_SIZE);

    /* Reset the dispatcher and wait until finishing */
    IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_tx_csr_base, ALTERA_MSGDMA_CSR_RESET_MASK);
    while(IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base) & ALTERA_MSGDMA_CSR_RESET_STATE_MASK);

    /* Clear msgDMA status */
    IOWR_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base, 0xFFFFFFFFu);

    while (1) {
        PRINT_ASSERT((!(gm_IsPrcGranted())), "ERROR in task MTX task= %s, TCB_MTX_NESTED= %u\n", g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_description, (unsigned int) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED);

        /* receive next frame plus length to transmit from producer/consumer queue */
        size = gu_MessageQueueReceive(pgemrtos_tse_mac_device->p_tse_queue_out);

        if (((G_UINT32) size == (G_UINT32) ERR_RECEIVE_ABORT) ||
            (size <= 0) ||
            ((unsigned int) size > TSE_TX_QUEUE_BUFFER_SIZE) ||
            ((unsigned int) size > TSE_DMA_FRAME_BUFFER_SIZE)) {
            gu_fprintf("Dropping invalid TX frame length= %d\n", size);
            continue;
        }

        /* when packet is received it is because the receive task configured pnetif structure */
        if (tse_info->sgdma_trigger_transmit == (GS_RCB *) 0) {
            /* Create the transmit trigger */
            tse_info->sgdma_trigger_transmit = gu_TriggerCreate(tse_info->tse_msgdma_tx_irq, (G_UINT64) 0);
            if (tse_info->sgdma_trigger_transmit == (GS_RCB *) 0) {
                gu_printf("Error when creating sgdma_trigger_transmit\n");
                while(1);
            }
            /* Define this task as an ISR associated to sgdma_trigger_transmit */
            gu_TriggerRegisterTask(ptcb, (GS_RCB *) tse_info->sgdma_trigger_transmit);
        }

        /* wait while mSGDAM is busy */
        while ((IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base) & ALTERA_MSGDMA_CSR_DESCRIPTOR_BUFFER_EMPTY_MASK) == 0);

        /* Stop the msgdma dispatcher from issuing more descriptors to the read or write masters */
        control = ALTERA_MSGDMA_CSR_STOP_DESCRIPTORS_MASK;
        IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_tx_csr_base, control);

        /*
        * Clear any (previous) status register information
        * that might occlude our error checking later.
        */
        IOWR_ALTERA_MSGDMA_CSR_STATUS(
            tse_info->tse_msgdma_tx_csr_base,
            IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base));

        /* Create descriptor for Avalon-MM to Avalon-ST mSGDAM transfer */
        PRINT_ASSERT(((void *) tse_info->buffer_tx_frame == (void *) &buffer_tx_frame), "ERROR tse_info->buffer_tx_frame= %p, &buffer_tx_frame= %p\n", (void *) tse_info->buffer_tx_frame, (void *) &buffer_tx_frame);
        IOWR_ALTERA_MSGDMA_DESCRIPTOR_READ_ADDRESS(tse_info->tse_msgdma_tx_descriptor_slave_base, (alt_u32) tse_info->buffer_tx_frame);

        IOWR_ALTERA_MSGDMA_DESCRIPTOR_LENGTH(tse_info->tse_msgdma_tx_descriptor_slave_base, (unsigned int) size);

        IOWR_ALTERA_MSGDMA_DESCRIPTOR_CONTROL_STANDARD(tse_info->tse_msgdma_tx_descriptor_slave_base,
                                                       ALTERA_MSGDMA_DESCRIPTOR_CONTROL_GO_MASK |
                                                       ALTERA_MSGDMA_DESCRIPTOR_CONTROL_TRANSFER_COMPLETE_IRQ_MASK |
                                                       ALTERA_MSGDMA_DESCRIPTOR_CONTROL_GENERATE_SOP_MASK |
                                                       ALTERA_MSGDMA_DESCRIPTOR_CONTROL_GENERATE_EOP_MASK );

        /*
        *  Set up controller to:
        *  - Run
        *  - Stop on an error with any particular descriptor
        */
        control = (ALTERA_MSGDMA_CSR_STOP_ON_ERROR_MASK |
                    ALTERA_MSGDMA_CSR_GLOBAL_INTERRUPT_MASK );
        control &=  (~ALTERA_MSGDMA_CSR_STOP_DESCRIPTORS_MASK);
        IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_tx_csr_base, control);

        /* Wait until transmit descriptor transfer is complete */
        if (gu_TriggerWait() != G_TRGStatus_triggered) {
            GU_FPRINTF(stderr, "STATUS = %x, %s, %d \n", IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base), __FUNCTION__, __LINE__);
        }
    }
}
#endif  // !__niosX_arch__
