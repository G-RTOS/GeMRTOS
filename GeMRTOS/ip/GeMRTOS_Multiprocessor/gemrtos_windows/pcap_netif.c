/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Windows-simulation network emulation — Npcap netif seam.
 *  \details Implements the two seam points the Windows build needs:
 *           netif_pcap_init() (host equivalent of netif_tse_init()) and
 *           pcap_netif_send()/pcap_netif_poll_receive() (host equivalents of
 *           tse_mac_raw_send() and the mSGDMA receive loop in tse_receive_server()).
 *           Everything above this file — ARP, DHCP, TCP, httpd.c, the SSI source
 *           registry, the CGI handler — is unmodified lwIP/application code that
 *           does not know or care that packets arrive via Npcap instead of an
 *           mSGDMA descriptor ring.
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

#ifndef __niosX_arch__

/* Must be defined before gemrtos.h pulls in <windows.h> (via
 * gemrtos_controller_windows.h): WIN32_LEAN_AND_MEAN stops windows.h from dragging
 * in the legacy <winsock.h>, so that pcap.h's own <winsock2.h>/<ws2tcpip.h>
 * (needed for pcap_t's socket types) are the only copies ever included --
 * winsock.h and winsock2.h redefine the same structs (struct in_addr,
 * IN_ADDR, ...) and cannot both be present regardless of order. Scoped to
 * this translation unit only; does not touch gemrtos_controller_windows.h/.c. */
#define WIN32_LEAN_AND_MEAN

/* lwip/def.h otherwise #defines htons/htonl/ntohs/ntohl to lwip_ht{on,ntoh}s/l
 * -- fine when lwIP is the only socket API in the file, but this file also
 * pulls in Npcap's <pcap.h>, which needs the real <winsock2.h> htonl/htons
 * (used internally by ws2tcpip.h's own declarations). With both present, the
 * macro rewrites winsock2.h's own "u_long htonl(u_long)" prototype into
 * "u_long lwip_htonl(u_long)", conflicting with lwIP's own
 * "u32_t lwip_htonl(u32_t)" prototype -- same underlying width, different C
 * types (unsigned long vs unsigned int), so the compiler rejects it as a
 * redeclaration. Not needed here regardless: this file never calls
 * htons/htonl/ntohs/ntohl under either name. */
#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS

#include <gemrtos.h>
#include <netif/etharp.h>
#include <lwip/snmp.h>
#include <pcap.h>
#include <string.h>
#include <stdio.h>

#include "pcap_netif.h"

/* One Npcap handle for the whole simulation instance: a single shared
 * gemrtos_windows/ per running instance (one netif, one adapter). */
static pcap_t *g_pcap_handle = NULL;

/* Largest frame pcap_netif_send() will copy out of a pbuf chain before
 * calling pcap_sendpacket(). 1600 covers the standard 1518-byte Ethernet
 * frame plus VLAN tag headroom, matching TSE_DMA_FRAME_BUFFER_SIZE's own
 * margin in hellogemrtos.c. */
#define PCAP_NETIF_TX_BUFFER_SIZE  1600U

/**
 *  \brief Print every adapter pcap can see, most useful when
 *         PCAP_NETIF_ADAPTER_SUBSTRING did not narrow the choice to exactly one.
 */
static void pcap_netif_print_adapters(pcap_if_t *alldevs)
{
    pcap_if_t *d;
    int index = 0;

    gu_fprintf("[pcap_netif] Available adapters:\n");
    for (d = alldevs; d != NULL; d = d->next) {
        gu_fprintf("[pcap_netif]   [%d] %s\n", index, (d->description != NULL) ? d->description : "(no description)");
        index++;
    }
}

/**
 *  \brief Select the adapter to open: the sole non-loopback candidate, or the
 *         one matching PCAP_NETIF_ADAPTER_SUBSTRING.
 *  \details Two passes when a substring is set: first look for a description
 *           that equals it exactly, then fall back to substring containment.
 *           Exact-match-first matters because one adapter's description can
 *           be a strict prefix of another's (e.g. a NIC's two ports named
 *           "...Connection" and "...Connection #2") -- containment alone
 *           would always resolve to whichever one pcap enumerates first,
 *           silently ignoring which port the user actually configured.
 *  \return A device from alldevs (borrowed — do not free independently), or
 *          NULL if selection was ambiguous or nothing qualified.
 */
static pcap_if_t *pcap_netif_select_adapter(pcap_if_t *alldevs)
{
    pcap_if_t *d;
    pcap_if_t *chosen = NULL;
    unsigned int candidate_count = 0;

    if (PCAP_NETIF_ADAPTER_SUBSTRING[0] != '\0') {
        for (d = alldevs; d != NULL; d = d->next) {
            if (((d->flags & PCAP_IF_LOOPBACK) == 0) &&
                (d->description != NULL) &&
                (strcmp(d->description, PCAP_NETIF_ADAPTER_SUBSTRING) == 0)) {
                return d;  /* exact match: unambiguous regardless of prefix collisions */
            }
        }
        for (d = alldevs; d != NULL; d = d->next) {
            if (((d->flags & PCAP_IF_LOOPBACK) == 0) &&
                (d->description != NULL) &&
                (strstr(d->description, PCAP_NETIF_ADAPTER_SUBSTRING) != NULL)) {
                return d;  /* no exact match anywhere: fall back to first containment match */
            }
        }
        return NULL;  /* substring was set but nothing matched, exactly or by containment */
    }

    for (d = alldevs; d != NULL; d = d->next) {
        if ((d->flags & PCAP_IF_LOOPBACK) != 0) {
            continue;
        }
        chosen = d;
        candidate_count++;
    }

    return (candidate_count == 1U) ? chosen : NULL;
}

err_t netif_pcap_init(struct netif *pnetif)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    pcap_if_t *chosen;

    PRINT_ASSERT((pnetif != NULL), "pnetif != NULL");

    #if LWIP_NETIF_HOSTNAME
        pnetif->hostname = "lwip";
    #endif

    NETIF_INIT_SNMP(*pnetif, snmp_ifType_ethernet_csmacd, 100000000);

    pnetif->output     = etharp_output;
    pnetif->linkoutput = pcap_netif_send;
    pnetif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    pnetif->hwaddr_len = ETHARP_HWADDR_LEN;
    pnetif->mtu         = 1500;

    pnetif->hwaddr[0] = PCAP_NETIF_MAC_BYTE_0;
    pnetif->hwaddr[1] = PCAP_NETIF_MAC_BYTE_1;
    pnetif->hwaddr[2] = PCAP_NETIF_MAC_BYTE_2;
    pnetif->hwaddr[3] = PCAP_NETIF_MAC_BYTE_3;
    pnetif->hwaddr[4] = PCAP_NETIF_MAC_BYTE_4;
    pnetif->hwaddr[5] = PCAP_NETIF_MAC_BYTE_5;

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        gu_fprintf("[pcap_netif] pcap_findalldevs failed: %s\n", errbuf);
        return ERR_IF;
    }

    chosen = pcap_netif_select_adapter(alldevs);
    if (chosen == NULL) {
        gu_fprintf("[pcap_netif] Could not pick an adapter automatically "
                   "(PCAP_NETIF_ADAPTER_SUBSTRING=\"%s\" in pcap_netif.h). "
                   "Set it to a substring of the adapter you want:\n",
                   PCAP_NETIF_ADAPTER_SUBSTRING);
        pcap_netif_print_adapters(alldevs);
        pcap_freealldevs(alldevs);
        return ERR_IF;
    }

    gu_fprintf("[pcap_netif] Selected adapter: %s\n",
               (chosen->description != NULL) ? chosen->description : chosen->name);

    g_pcap_handle = pcap_open_live(chosen->name, 65536, PCAP_NETIF_PROMISCUOUS,
                                    PCAP_NETIF_READ_TIMEOUT_MS, errbuf);
    pcap_freealldevs(alldevs);  /* g_pcap_handle no longer references alldevs past this point */

    if (g_pcap_handle == NULL) {
        gu_fprintf("[pcap_netif] pcap_open_live failed: %s\n", errbuf);
        return ERR_IF;
    }

    if (pcap_setnonblock(g_pcap_handle, 1, errbuf) == -1) {
        gu_fprintf("[pcap_netif] pcap_setnonblock failed: %s\n", errbuf);
        pcap_close(g_pcap_handle);
        g_pcap_handle = NULL;
        return ERR_IF;
    }

    return ERR_OK;
}

err_t pcap_netif_send(struct netif *pnetif, struct pbuf *pkt)
{
    static unsigned char tx_buffer[PCAP_NETIF_TX_BUFFER_SIZE];
    u16_t copied;

    (void) pnetif;

    if (g_pcap_handle == NULL) {
        return ERR_IF;
    }

    if (pkt->tot_len > PCAP_NETIF_TX_BUFFER_SIZE) {
        gu_fprintf("[pcap_netif] TX frame too large (%u > %u), dropped\n",
                   (unsigned int) pkt->tot_len, (unsigned int) PCAP_NETIF_TX_BUFFER_SIZE);
        return ERR_IF;
    }

    copied = pbuf_copy_partial(pkt, tx_buffer, pkt->tot_len, 0);

    if (pcap_sendpacket(g_pcap_handle, tx_buffer, (int) copied) != 0) {
        gu_fprintf("[pcap_netif] pcap_sendpacket failed: %s\n", pcap_geterr(g_pcap_handle));
        return ERR_IF;
    }

    return ERR_OK;
}

struct pbuf *pcap_netif_poll_receive(struct netif *pnetif)
{
    struct pcap_pkthdr *header;
    const unsigned char *data;
    struct pbuf *p;
    int result;

    (void) pnetif;

    if (g_pcap_handle == NULL) {
        return NULL;
    }

    result = pcap_next_ex(g_pcap_handle, &header, &data);
    if (result <= 0) {
        /* 0 = read timeout expired with nothing captured (normal, non-blocking);
         * -1 = error (logged once here rather than spamming every poll). */
        if (result < 0) {
            gu_fprintf("[pcap_netif] pcap_next_ex error: %s\n", pcap_geterr(g_pcap_handle));
        }
        return NULL;
    }

    if ((header->caplen == 0U) || (header->caplen > 0xFFFFU)) {
        gu_fprintf("[pcap_netif] Dropping invalid RX frame length= %u\n", (unsigned int) header->caplen);
        return NULL;
    }

    p = pbuf_alloc(PBUF_RAW, (u16_t) header->caplen, PBUF_RAM);
    if (p == NULL) {
        gu_fprintf("[pcap_netif] pbuf_alloc failed for RX length= %u\n", (unsigned int) header->caplen);
        return NULL;
    }

    if (pbuf_take(p, data, (u16_t) header->caplen) != ERR_OK) {
        gu_fprintf("[pcap_netif] pbuf_take failed for RX length= %u\n", (unsigned int) header->caplen);
        pbuf_free(p);
        return NULL;
    }

    return p;
}

#endif /* !__niosX_arch__ */
