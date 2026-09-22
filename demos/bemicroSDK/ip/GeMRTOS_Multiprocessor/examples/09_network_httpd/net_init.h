/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Network device state, topology wiring, and lwIP lifecycle
 *         callbacks for the 09_network_httpd example.
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

#ifndef NET_INIT_H_
#define NET_INIT_H_

#include <gemrtos.h>
#include <system.h>
#include "lwip_main.h"

/* !!!  REPLACE THESE WITH YOUR HARDWARE'S ACTUAL MAC ADDRESS  !!!
 * Every board on the network must have a unique MAC address. Using the
 * default values on more than one board causes ARP conflicts. For
 * production use, read the MAC from a NIC register or OTP memory.
 * Unchanged from examples/10_full_demo/hellogemrtos.c. */
#define TSE_MAC_BYTE_0  0xACU
#define TSE_MAC_BYTE_1  0xEFU
#define TSE_MAC_BYTE_2  0xACU
#define TSE_MAC_BYTE_3  0xEFU
#define TSE_MAC_BYTE_4  0xACU
#define TSE_MAC_BYTE_5  0xEFU
/* Agilex3 override -- still replace with real values for production. */
#define TSE_MAC_AGILEX3_BYTE_0  0xCAU
#define TSE_MAC_AGILEX3_BYTE_1  0xFEU

/* DMA/message-queue buffer sizing -- shared between net_init.c (buffer
 * declarations) and tse_servers.c (frame-length validation, message-queue
 * subscription). Unchanged from the original. */
#define TSE_DMA_FRAME_BUFFER_SIZE 1700U
#define TSE_TX_QUEUE_BUFFER_SIZE  1700U

/* DMA frame buffers -- must be in on-chip memory. Defined in net_init.c;
 * tse_servers.c reads them only through the pointers stored in
 * gemrtos_tse_mac_device[0] (PRINT_ASSERT identity checks against these
 * globals, unchanged from the original). */
extern unsigned char buffer_tx_frame[TSE_DMA_FRAME_BUFFER_SIZE];
extern unsigned char buffer_rx_frame[TSE_DMA_FRAME_BUFFER_SIZE];

/** \brief The one network device this example drives. Defined in
 *         net_init.c; its `.pnetif`, `.p_tse_queue_out`, `.buffer_tx_frame`
 *         and `.buffer_rx_frame` fields are populated by
 *         net_init_topology(), which must run before tse_receive_server()
 *         or tse_transmit_server() is created as a task. */
extern gemrtos_tse_system_info gemrtos_tse_mac_device[1];

/** \brief Populates gemrtos_tse_mac_device[0]'s netif structure, MAC
 *         address, and TX message queue. Must be called once from main(),
 *         before gu_TaskCreate() for tse_receive_server()/
 *         tse_transmit_server(). Extracted from the inline block that used
 *         to sit directly in main() (examples/10_full_demo/hellogemrtos.c)
 *         -- pure code motion, no logic change; see README.md "Extraction
 *         notes".
 *  \return G_TRUE on success; G_FALSE if the TX message queue pool was
 *          exhausted. */
G_UINT32 net_init_topology(void);

/** \brief lwIP status callback -- called by the DHCP subsystem when an IP
 *         address is acquired. Validates that the assigned IP and gateway
 *         are on the same subnet (guards against a stale lease or a
 *         link-local fallback address surviving a network change); if not,
 *         releases the lease and forces a fresh DHCP negotiation.
 *         Registered via lwip_set_status_callback() at the start of
 *         tse_receive_server().
 *  \param [in] pnetif  The lwIP network interface that received an address. */
void StatusCallback(struct netif *pnetif);

/** \brief lwIP link callback -- called when the Ethernet link state
 *         changes. On LINK DOWN, stops and releases the current DHCP lease
 *         (P2) so the next dhcp_start() performs a fresh DISCOVERY instead
 *         of a RENEW against a network that may no longer be reachable.
 *         Registered via lwip_set_link_callback() at the start of
 *         tse_receive_server().
 *  \param [in] pnetif  The lwIP network interface whose link state changed. */
void LinkCallback(struct netif *pnetif);

#endif /* NET_INIT_H_ */
