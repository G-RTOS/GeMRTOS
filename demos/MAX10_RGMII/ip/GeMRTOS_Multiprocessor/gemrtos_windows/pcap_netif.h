/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Windows-simulation network emulation — Npcap netif seam.
 *  \details Host-only replacement for the two hardware seam points
 *           (`netif_tse_init`/`tse_mac_raw_send` in `lwip_main.c`): opens a real
 *           adapter via Npcap and moves raw Ethernet frames between it and lwIP,
 *           unchanged above this point (ARP, DHCP, TCP, httpd, SSI, CGI all run the
 *           same code the board runs). Never compiled for an embedded target.
 *  \version 0.9a
 *  \date      2026
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

#ifndef PCAP_NETIF_H_
#define PCAP_NETIF_H_

#ifndef __niosX_arch__

#include <lwip/netif.h>
#include <lwip/err.h>
#include <lwip/pbuf.h>

/* --------------------------------------------------------------------------
 * User-facing settings — edit before building, same convention as the
 * TSE_MAC_BYTE_0..5 constants in hellogemrtos.c: adapter selection and MAC
 * configuration are exposed as settings.
 * ----------------------------------------------------------------------- */

/** Substring matched (case-sensitive) against each candidate adapter's pcap
 *  description. Leave empty to require exactly one non-loopback adapter to
 *  be present; if more than one qualifies, netif_pcap_init() prints the full
 *  enumerated list to stderr and fails rather than guessing which one you
 *  meant. Set this once you know which adapter you want, e.g.
 *  "Intel(R) 82574L Gigabit Network Connection" (without "#2", so it does
 *  not also match the second port). */
#define PCAP_NETIF_ADAPTER_SUBSTRING  ""

/* Emulated MAC — deliberately distinct from the board's (TSE_MAC_BYTE_0..5 =
 * AC:EF:AC:EF:AC:EF) so a Windows instance on the same LAN as a running
 * board never collides. Locally-administered unicast (first byte's
 * low-order 2 bits = 10b). */
#define PCAP_NETIF_MAC_BYTE_0  0x02U
#define PCAP_NETIF_MAC_BYTE_1  0x00U
#define PCAP_NETIF_MAC_BYTE_2  0x00U
#define PCAP_NETIF_MAC_BYTE_3  0x47U
#define PCAP_NETIF_MAC_BYTE_4  0x65U
#define PCAP_NETIF_MAC_BYTE_5  0x01U

/** Promiscuous mode is required: the adapter's own MAC differs from
 *  PCAP_NETIF_MAC_BYTE_*, so frames addressed to the emulated MAC would
 *  otherwise never reach us. */
#define PCAP_NETIF_PROMISCUOUS  1

/** pcap_open_live()/pcap_next_ex() read timeout, in milliseconds. Only
 *  bounds how long a single non-blocking poll may internally wait for the
 *  OS to hand back a buffered packet; does not block the caller beyond
 *  that. */
#define PCAP_NETIF_READ_TIMEOUT_MS  1

/* --------------------------------------------------------------------------
 * Netif seam API — mirrors netif_tse_init()/tse_mac_raw_send() in lwip_main.c
 * ----------------------------------------------------------------------- */

/** \brief lwIP netif init callback for the Npcap-backed interface.
 *  \details Passed to netif_add() exactly where netif_tse_init() is passed
 *           on embedded. Selects the adapter (PCAP_NETIF_ADAPTER_SUBSTRING or,
 *           if empty, the sole non-loopback candidate), opens it, and sets
 *           pnetif->hwaddr/mtu/flags/linkoutput.
 *  \param [in,out] pnetif  The netif being brought up; pnetif->state must
 *                          already point at the owning gemrtos_tse_system_info.
 *  \return ERR_OK on success; ERR_IF if no adapter could be opened (message
 *          already printed to stderr — the caller should treat this as fatal,
 *          the same way the embedded path treats netif_add() failure).
 *  \thread_safety Called once, from the receive-server task, before any other
 *                 pcap_netif_* function.
 */
err_t netif_pcap_init(struct netif *pnetif);

/** \brief Transmit one frame — the host equivalent of tse_mac_raw_send().
 *  \param [in] pnetif  Unused beyond validation; the adapter handle is
 *                      process-global (one Npcap netif per simulation
 *                      instance).
 *  \param [in] pkt     lwIP pbuf chain to send; copied out before return.
 *  \return ERR_OK on success, ERR_IF if pcap_sendpacket() failed.
 *  \thread_safety Must be called only from the GeMRTOS task that owns the
 *                 netif (NO_SYS = 1 is a hard constraint); pcap
 *                 itself is not the issue, lwIP's lack of locking is.
 */
err_t pcap_netif_send(struct netif *pnetif, struct pbuf *pkt);

/** \brief Non-blocking poll for one received frame.
 *  \details The host equivalent of the mSGDMA-descriptor-plus-trigger wait
 *           in tse_receive_server(): call this once per iteration of the
 *           receive server's main loop instead of gu_TriggerWait().
 *  \return A freshly allocated PBUF_RAM pbuf ready for pnetif->input(), or
 *          NULL if no frame was waiting. Ownership passes to the caller.
 *  \thread_safety Same task-affinity constraint as pcap_netif_send().
 */
struct pbuf *pcap_netif_poll_receive(struct netif *pnetif);

#endif /* !__niosX_arch__ */

#endif /* PCAP_NETIF_H_ */
