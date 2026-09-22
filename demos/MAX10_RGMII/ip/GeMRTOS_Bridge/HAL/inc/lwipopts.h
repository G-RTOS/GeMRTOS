/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H

#ifdef LWIP_OPTTEST_FILE
#include "lwipopts_test.h"
#else /* LWIP_OPTTEST_FILE */


#define PHY_COUNT 1
#define IP_FRAG_MAX_MTU 1260

#define LWIP_IPV4                  1
#define LWIP_IPV6                  0 // 1

#define NO_SYS                     1  // 0
#define LWIP_SOCKET                (NO_SYS==0)
#define LWIP_NETCONN               (NO_SYS==0)
#define LWIP_NETIF_API             (NO_SYS==0)

#define LWIP_IGMP                  LWIP_IPV4
#define LWIP_ICMP                  LWIP_IPV4

#define LWIP_SNMP                  LWIP_UDP
#ifdef LWIP_HAVE_MBEDTLS
#define LWIP_SNMP_V3               (LWIP_SNMP)
#endif

#define LWIP_DNS                   LWIP_UDP
#define LWIP_MDNS_RESPONDER        LWIP_UDP

#define LWIP_NUM_NETIF_CLIENT_DATA (LWIP_MDNS_RESPONDER)

#define LWIP_HAVE_LOOPIF           1
#define LWIP_NETIF_LOOPBACK        0  // 1
#define LWIP_LOOPBACK_MAX_PBUFS    10

#define TCP_LISTEN_BACKLOG         0  // 1

#define LWIP_COMPAT_SOCKETS        0  // 1
#define LWIP_SO_RCVTIMEO           0  // 1
#define LWIP_SO_RCVBUF             0  // 1

#define LWIP_TCPIP_CORE_LOCKING    0  // 1

#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#ifdef LWIP_DEBUG

#define LWIP_DBG_MIN_LEVEL         0
#define PPP_DEBUG                  LWIP_DBG_OFF
#define MEM_DEBUG                  LWIP_DBG_OFF
#define MEMP_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                 LWIP_DBG_OFF
#define API_LIB_DEBUG              LWIP_DBG_OFF
#define API_MSG_DEBUG              LWIP_DBG_OFF
#define TCPIP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                LWIP_DBG_OFF
#define SOCKETS_DEBUG              LWIP_DBG_OFF
#define DNS_DEBUG                  LWIP_DBG_OFF
#define AUTOIP_DEBUG               LWIP_DBG_OFF
#define DHCP_DEBUG                 LWIP_DBG_OFF
#define IP_DEBUG                   LWIP_DBG_OFF
#define IP_REASS_DEBUG             LWIP_DBG_OFF
#define ICMP_DEBUG                 LWIP_DBG_OFF
#define IGMP_DEBUG                 LWIP_DBG_OFF
#define UDP_DEBUG                  LWIP_DBG_OFF
#define TCP_DEBUG                  LWIP_DBG_OFF
#define TCP_INPUT_DEBUG            LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG           LWIP_DBG_OFF
#define TCP_RTO_DEBUG              LWIP_DBG_OFF
#define TCP_CWND_DEBUG             LWIP_DBG_OFF
#define TCP_WND_DEBUG              LWIP_DBG_OFF
#define TCP_FR_DEBUG               LWIP_DBG_OFF
#define TCP_QLEN_DEBUG             LWIP_DBG_OFF
#define TCP_RST_DEBUG              LWIP_DBG_OFF
#endif

#define LWIP_DBG_TYPES_ON         (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)


/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
/* MSVC port: intel processors don't need 4-byte alignment,
   but are faster that way! */
#define MEM_ALIGNMENT           4U

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high.
 *
 * Raised from 10240 to 65536: the web status page's HTTP
 * response was being silently truncated mid-way through its static tail
 * (well after all SSI tags had already been processed correctly) --
 * evidence pointed at heap exhaustion under this board's DDR3-backed heap
 * (confirmed ample headroom for this increase), since the page now involves
 * more concurrent/serial HTTP activity than before (style.css + favicon
 * requests, 3 SSI tags per response instead of 1,
 * plus the 2s AJAX poll) than the smaller single-tag page this value was
 * apparently sized against. This is a hypothesis-confirming change, not a
 * proven root cause; verify on hardware that it actually fixes the
 * truncation. */
#define MEM_SIZE               65536

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           16
/* MEMP_NUM_RAW_PCB: the number of UDP protocol control blocks. One
   per active RAW "connection". */
#define MEMP_NUM_RAW_PCB        3
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        8
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        5
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 8
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments (a GLOBAL pool, shared across every connection -- unlike
   TCP_SND_QUEUELEN below, which is a PER-connection cap).
 *
 * Raised from 16 to 64 alongside the TCP_SND_BUF increase above
 * with TCP_SND_BUF now 8192, TCP_SND_QUEUELEN (derived,
 * per-connection) becomes 32 -- a single connection alone could now need
 * more queued segments than the previous global pool of 16 could ever
 * supply, regardless of the per-connection cap. 64 keeps headroom for more
 * than one connection to need close to the per-connection maximum at once
 * (e.g. the status page's own poll plus a concurrent style.css/favicon
 * request). */
#define MEMP_NUM_TCP_SEG        64
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
#define MEMP_NUM_SYS_TIMEOUT    17

/* The following four are used only with the sequential API and can be
   set to 0 if the application only will use the raw API. */
/* MEMP_NUM_NETBUF: the number of struct netbufs. */
#define MEMP_NUM_NETBUF         2
/* MEMP_NUM_NETCONN: the number of struct netconns. */
#define MEMP_NUM_NETCONN        12
/* MEMP_NUM_TCPIP_MSG_*: the number of struct tcpip_msg, which is used
   for sequential API communication and incoming packets. Used in
   src/api/tcpip.c. */
#define MEMP_NUM_TCPIP_MSG_API   16
#define MEMP_NUM_TCPIP_MSG_INPKT 16


/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          120

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE       2048   // 256

/** SYS_LIGHTWEIGHT_PROT
 * define SYS_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task protection
 * for certain critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    (NO_SYS==0)


/* ---------- TCP options ---------- */
#define LWIP_TCP                1
#define TCP_TTL                 255

#define LWIP_ALTCP              (LWIP_TCP)
#ifdef LWIP_HAVE_MBEDTLS
#define LWIP_ALTCP_TLS          (LWIP_TCP)
#define LWIP_ALTCP_TLS_MBEDTLS  (LWIP_TCP)
#endif


/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 1024

/* TCP sender buffer space (bytes).
 *
 * Raised from 2048 to 8192 (second experiment in the status.html
 * truncation investigation).
 * The first experiment (raising MEM_SIZE, the general lwIP heap) did not
 * fix the truncation, which points away from a general heap-exhaustion
 * cause and toward this connection's specific send-side resources instead:
 * the web status page's SSI response now requires roughly a dozen separate
 * http_write()/altcp_write() calls (3 SSI tags -- one of them a 5-row
 * multipart -- interleaved with static chunks), all generated back-to-back
 * within one scheduling burst before the client has a chance to ACK
 * anything. TCP_SND_QUEUELEN (derived below) caps how many of those can be
 * queued unacknowledged per connection; raising TCP_SND_BUF raises that cap
 * proportionally and gives more room per write() before a chunk needs to be
 * queued as a separate segment at all. */
#define TCP_SND_BUF             8192

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN       (4 * TCP_SND_BUF/TCP_MSS)

/* TCP writable space (bytes). This must be less than or equal
   to TCP_SND_BUF. It is the amount of space which must be
   available in the tcp snd_buf for select to return writable */
#define TCP_SNDLOWAT           (TCP_SND_BUF/2)

/* TCP receive window. */
#define TCP_WND                 (20 * 1024)

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX              12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX           4


/* ---------- ARP options ---------- */
#define LWIP_ARP                1
#define ARP_TABLE_SIZE          10
#define ARP_QUEUEING            1


/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#define IP_FORWARD              0  // 1

/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#define IP_REASSEMBLY           0  // 1
#define IP_REASS_MAX_PBUFS      (10 * ((1500 + PBUF_POOL_BUFSIZE - 1) / PBUF_POOL_BUFSIZE))
#define MEMP_NUM_REASSDATA      IP_REASS_MAX_PBUFS
#define IP_FRAG                 0  // 1
#define IPV6_FRAG_COPYHEADER    0  // 1

/* ---------- ICMP options ---------- */
#define ICMP_TTL                255


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP               LWIP_UDP

/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#define DHCP_DOES_ARP_CHECK    (LWIP_DHCP)


/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP            (LWIP_DHCP)
#define LWIP_DHCP_AUTOIP_COOP  (LWIP_DHCP && LWIP_AUTOIP)


/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define LWIP_UDPLITE            LWIP_UDP
#define UDP_TTL                 255


/* ---------- RAW options ---------- */
#define LWIP_RAW                1


/* ---------- Statistics options ---------- */

#define LWIP_STATS              0  // 1
#define LWIP_STATS_DISPLAY      0  // 1

#if LWIP_STATS
#define LINK_STATS              0  // 1
#define IP_STATS                0  // 1
#define ICMP_STATS              0  // 1
#define IGMP_STATS              0  // 1
#define IPFRAG_STATS            0  // 1
#define UDP_STATS               0  // 1
#define TCP_STATS               0  // 1
#define MEM_STATS               0  // 1
#define MEMP_STATS              0  // 1
#define PBUF_STATS              0  // 1
#define SYS_STATS               0  // 1
#endif /* LWIP_STATS */

/* ---------- NETBIOS options ---------- */
#define LWIP_NETBIOS_RESPOND_NAME_QUERY 1

/* ---------- PPP options ---------- */

#define PPP_SUPPORT             0  // 1      /* Set > 0 for PPP */

#if PPP_SUPPORT

#define NUM_PPP                 0  // 1      /* Max PPP sessions. */


/* Select modules to enable.  Ideally these would be set in the makefile but
 * we're limited by the command line length so you need to modify the settings
 * in this file.
 */
#define PPPOE_SUPPORT           0  // 1
#define PPPOS_SUPPORT           0  // 1

#define PAP_SUPPORT             0  // 1      /* Set > 0 for PAP. */
#define CHAP_SUPPORT            0  // 1      /* Set > 0 for CHAP. */
#define MSCHAP_SUPPORT          0      /* Set > 0 for MSCHAP */
#define CBCP_SUPPORT            0      /* Set > 0 for CBCP (NOT FUNCTIONAL!) */
#define CCP_SUPPORT             0      /* Set > 0 for CCP */
#define VJ_SUPPORT              0      /* Set > 0 for VJ header compression. */
#define MD5_SUPPORT             0  // 1      /* Set > 0 for MD5 (see also CHAP) */

#endif /* PPP_SUPPORT */

/* --- HTTPD (web server) options ---
 * Enables Server-Side Includes so the web status page (fs/index.html) can embed
 * live system values via <!--#tagname--> tags. See the generic SSI source
 * registry (gemrtos_web_sources[]) and gemrtos_web_ssi_handler() in
 * hellogemrtos.c for how tags are resolved to content. */
#define LWIP_HTTPD_SSI              1  /* enable SSI tag processing */
#define LWIP_HTTPD_SSI_MULTIPART    1  /* allow a tag to emit multiple rows across calls */

/* Default is 8. httpd.c: "ATTENTION: tags longer than this are ignored, not
 * truncated!" -- our longest tag name, "period_ms", is 9 characters, so the
 * default silently dropped it (the config form's period field never got
 * pre-filled, while "loop_ms"/"sysvars" at 7 characters worked fine). Set
 * with a little headroom for future tag names. */
#define LWIP_HTTPD_MAX_TAG_NAME_LEN 12

/* Default (1) echoes the literal "<!--#tag-->" text before the inserted value.
 * Harmless where a tag sits in HTML body text (renders as an invisible
 * comment either way), but the config form pre-fills <input value="..."> from
 * a tag placed INSIDE a quoted attribute -- there, the literal tag text is
 * not a comment to the browser, just garbage characters in the attribute
 * value. Disabling this globally leaves the status table's rendering
 * unchanged and makes attribute pre-fill work correctly. */
#define LWIP_HTTPD_SSI_INCLUDE_TAG  0

/* The served page is /index.html; the default SSI extension list only
 * recognizes .shtml/.shtm/.ssi/.xml/.json, so ".html" must be added explicitly
 * for SSI tags to be parsed in it. */
#define LWIP_HTTPD_SSI_EXTENSIONS   ".shtml", ".shtm", ".ssi", ".xml", ".json", ".html"

/* Enables the "/set.cgi" form handler (gemrtos_web_cgi_set_handler() in
 * hellogemrtos.c) so the web status page can change runtime parameters
 * (periodic task period, infinite-loop delay), not just display them. */
#define LWIP_HTTPD_CGI              1

#endif /* LWIP_OPTTEST_FILE */

/* The following defines must be done even in OPTTEST mode: */

#if !defined(NO_SYS) || !NO_SYS /* default is 0 */
void sys_check_core_locking(void);
#define LWIP_ASSERT_CORE_LOCKED()  sys_check_core_locking()
#endif

#ifndef LWIP_PLATFORM_ASSERT
/* Define LWIP_PLATFORM_ASSERT to something to catch missing stdio.h includes */
extern void lwip_example_app_platform_assert(const char *msg, int line, const char *file);


#define LWIP_PLATFORM_ASSERT(x) lwip_example_app_platform_assert(x, __LINE__, __FILE__)
#endif

#endif /* LWIP_LWIPOPTS_H */

