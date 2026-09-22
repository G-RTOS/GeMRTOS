/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief TSE (Triple Speed Ethernet) receive and transmit server tasks
 *         for the 09_network_httpd example: mSGDMA-driven RX/TX, lwIP
 *         network bring-up, DHCP/link maintenance, and (RX server only)
 *         HTTP server start-up.
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

#ifndef TSE_SERVERS_H_
#define TSE_SERVERS_H_

#include <gemrtos.h>
#include <system.h>
#include "net_init.h"
#include "web_sources.h"

#ifndef __niosX_arch__
#include "pcap_netif.h"  /* host body needs netif_pcap_init()/pcap_netif_poll_receive() */
#endif

/* TSE compile-time configuration. Unchanged from
 * examples/10_full_demo/hellogemrtos.c. */
#define USE_DHCP    1  /* 1 = acquire IP via DHCP; 0 = use static IP 192.168.0.15 */
#define BUILD_HTTPD 1  /* 1 = start the raw HTTP server after network comes up     */

#define TIMEOUT_TSE_RECEIVE_MS  250U  /* TSE SGDMA receive trigger timeout */

/* Link debounce parameters -- a single PHY register read is susceptible to
 * transient bounce while a connector mates. LINK_DEBOUNCE_SAMPLES consecutive
 * link-alive reads, LINK_DEBOUNCE_INTERVAL_MS apart, are required before the
 * link is treated as stable (see tse_receive_server()'s reconnection loop). */
#define LINK_DEBOUNCE_SAMPLES        5U  /* consecutive stable reads required                    */
#define LINK_DEBOUNCE_INTERVAL_MS   40U  /* delay between debounce reads (5 x 40ms = 200ms total) */

/* Runtime reconnect robustness (added after a real modem power-cycle on
 * Atum A3 left the interface never reconnecting, while every OS-managed
 * NIC on the same segment reconnected normally -- 2026-09-03).
 *
 * tse_receive_server()'s only view of the PHY at runtime used to be
 * indirect: it re-checked the PHY only after 30 consecutive RX-trigger
 * timeouts (~7.5s of total silence) or an mSGDMA descriptor error. On a LAN
 * busy enough that some frame from another device keeps arriving within
 * that window, a genuine physical link bounce -- e.g. the upstream
 * modem/router rebooting -- can complete before that silence ever
 * accumulates, and is never noticed. An OS network stack does not have
 * this gap: it gets a hardware link-state interrupt regardless of traffic.
 * PHY_POLL_INTERVAL_MS closes it by checking the PHY directly on a fixed
 * schedule, independent of RX activity. */
#define PHY_POLL_INTERVAL_MS         1000U

/* How many consecutive PHY_POLL_INTERVAL_MS polls must agree the raw link
 * bit no longer matches the last known-stable state before the poll is
 * allowed to call gemrtos_UpdateTSEInfo() at all.
 *
 * Mandatory, not a tuning knob: gemrtos_UpdateTSEInfo() (lwip_main.c) does
 * three separate MDIO reads and, if ANY of them disagrees with its last
 * stored value, unconditionally issues an MDIO write that restarts the
 * PHY's auto-negotiation and blocks for up to
 * TSE_PHY_NEGOTIATION_TIMEOUT_MS (3s) -- a real, physical renegotiation,
 * not a software-only check. That is fine on the two pre-existing call
 * sites (boot and confirmed RX-path anomalies), which are already rare and
 * already gated behind tse_link_bring_up()'s own 5-consecutive-read
 * debounce. Calling it unconditionally once a second, forever -- what the
 * first version of this poll did -- turns any single noisy MDIO read into
 * an unnecessary real link renegotiation, and every renegotiation corrupts
 * whatever frames are in flight at that instant. Confirmed on Atum A3: the
 * first version of this poll produced frequent
 * "AFTER RECEIVE WHILE STATUS response_reg=0x1" reports (a
 * runt/errored-frame indication from the mSGDMA response port) that did not
 * occur before this poll existed. This debounce restores the same
 * "do not trust one read" principle LINK_DEBOUNCE_SAMPLES already applies
 * elsewhere, using a side-effect-free raw read (see tse_raw_link_alive())
 * so the debounce itself never touches the PHY. */
#define PHY_POLL_DEBOUNCE_SAMPLES    3U

/* Defence in depth for whatever PHY_POLL_INTERVAL_MS does not catch
 * (including a cause this code cannot name): if the interface has reported
 * link_alive without ever reaching netif_ip_is_up() for this long, force a
 * full reconnect cycle unconditionally. Set well above DHCP's own
 * worst-case retry/backoff window (lwIP backs off 1,2,4,8,16,32s then caps
 * at 60s between DISCOVER attempts, retrying indefinitely) so a normal, if
 * slow, negotiation is never interrupted by this safety net. */
#define DHCP_NOT_BOUND_TIMEOUT_MINUTES  5U

/** \brief TSE receive server: brings up lwIP/the network interface/DHCP/the
 *         HTTP server once, then loops forever servicing received frames
 *         via mSGDMA and PHY link-state changes. G_TCBType_OneShot; never
 *         returns.
 *  \param [in] pgemrtos_tse_mac_device  The network device to service
 *                                        (gemrtos_tse_mac_device[0],
 *                                        net_init.h), passed as this
 *                                        task's creation argument. */
void tse_receive_server(gemrtos_tse_system_info *pgemrtos_tse_mac_device);

/** \brief TSE transmit server: subscribes to the device's outgoing message
 *         queue and drives mSGDMA to transmit each frame lwIP hands it.
 *         G_TCBType_OneShot; never returns.
 *  \param [in] pdata  The network device to service
 *                      (&gemrtos_tse_mac_device[0], net_init.h), cast from
 *                      this task's creation argument. */
void tse_transmit_server(void *pdata);

#endif /* TSE_SERVERS_H_ */
