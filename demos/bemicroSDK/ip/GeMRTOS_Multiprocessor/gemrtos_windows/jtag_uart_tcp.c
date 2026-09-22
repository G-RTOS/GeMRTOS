/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Windows-simulation JTAG UART emulation — per-processor TCP console.
 *  \details Implements the per-processor TCP listener/console-multiplexer described in
 *           jtag_uart_tcp.h. Deliberately does not include <gemrtos.h>: the only thing
 *           this file needs from the BSP is GEMRTOS_DRIVER_NPROCESSORS, which lives in
 *           the self-contained, dependency-free, Qsys-generated system.h. Pulling in the
 *           full <gemrtos.h> chain would drag in <windows.h> (via gemrtos_controller_windows.h)
 *           ahead of this file's own Winsock includes, the exact collision worked around
 *           in pcap_netif.c (WIN32_LEAN_AND_MEAN / LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS)
 *           — avoided here by simply not needing lwIP or gemrtos_controller_windows.h at all.
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

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#include <system.h>
#include "jtag_uart_tcp.h"

#define JTAG_UART_TCP_MAX_PROCESSORS  GEMRTOS_DRIVER_NPROCESSORS

typedef struct {
    int    listener_ready;   /* listen() succeeded (or was already attempted and failed) */
    SOCKET listen_sock;
    SOCKET client_sock;      /* INVALID_SOCKET if no terminal is attached */
} jtag_uart_tcp_instance_t;

/* Index 0 unused -- gm_ProcessorId() is 1-based, kept that way here to avoid
 * an off-by-one translating between the two. */
static jtag_uart_tcp_instance_t g_instances[JTAG_UART_TCP_MAX_PROCESSORS + 1];
static int g_wsa_started = 0;

static void jtag_uart_tcp_ensure_wsa(void)
{
    if (!g_wsa_started) {
        WSADATA wsa_data;
        WSAStartup(MAKEWORD(2, 2), &wsa_data);
        g_wsa_started = 1;
    }
}

/** Lazily create, bind, and listen on this processor's port. Idempotent —
 *  safe to call on every write/read poll. Leaves listen_sock as
 *  INVALID_SOCKET (and listener_ready set, so it is not retried every call)
 *  if anything failed, in which case writes fall back to the shared console
 *  and reads simply return nothing, same as "no terminal attached". */
static void jtag_uart_tcp_ensure_listener(unsigned int processor_id)
{
    jtag_uart_tcp_instance_t *inst = &g_instances[processor_id];
    struct sockaddr_in addr;
    u_long nonblock = 1;
    int reuse = 1;

    if (inst->listener_ready) {
        return;
    }
    inst->listener_ready = 1;
    inst->client_sock = INVALID_SOCKET;

    jtag_uart_tcp_ensure_wsa();

    inst->listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inst->listen_sock == INVALID_SOCKET) {
        fprintf(stderr, "[jtag_uart_tcp] socket() failed for processor %u\n", processor_id);
        return;
    }

    setsockopt(inst->listen_sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuse, sizeof(reuse));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons((unsigned short) (JTAG_UART_TCP_BASE_PORT + processor_id));

    if (bind(inst->listen_sock, (struct sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR) {
        fprintf(stderr, "[jtag_uart_tcp] bind() failed for processor %u, port %d: %d\n",
                processor_id, JTAG_UART_TCP_BASE_PORT + (int) processor_id, WSAGetLastError());
        closesocket(inst->listen_sock);
        inst->listen_sock = INVALID_SOCKET;
        return;
    }

    if (listen(inst->listen_sock, 1) == SOCKET_ERROR) {
        fprintf(stderr, "[jtag_uart_tcp] listen() failed for processor %u: %d\n",
                processor_id, WSAGetLastError());
        closesocket(inst->listen_sock);
        inst->listen_sock = INVALID_SOCKET;
        return;
    }

    ioctlsocket(inst->listen_sock, FIONBIO, &nonblock);

    fprintf(stdout, "[jtag_uart_tcp] Processor %u listening on 127.0.0.1:%d\n",
            processor_id, JTAG_UART_TCP_BASE_PORT + (int) processor_id);
}

/** Accept a pending connection if one is waiting. A newly-arriving terminal
 *  replaces whatever was previously attached -- one terminal per instance,
 *  matching the board (only one juart-terminal can usefully attach to a
 *  given instance at a time). */
static void jtag_uart_tcp_accept_if_pending(jtag_uart_tcp_instance_t *inst)
{
    SOCKET incoming;
    u_long nonblock = 1;

    if (inst->listen_sock == INVALID_SOCKET) {
        return;
    }

    incoming = accept(inst->listen_sock, NULL, NULL);
    if (incoming == INVALID_SOCKET) {
        return; /* WSAEWOULDBLOCK: nothing pending -- the common case, not an error */
    }

    if (inst->client_sock != INVALID_SOCKET) {
        closesocket(inst->client_sock);
    }
    inst->client_sock = incoming;
    ioctlsocket(inst->client_sock, FIONBIO, &nonblock);
}

void jtag_uart_tcp_write_processor(unsigned int processor_id, const char *data, unsigned int len)
{
    jtag_uart_tcp_instance_t *inst;

    if ((processor_id < 1U) || (processor_id > (unsigned int) JTAG_UART_TCP_MAX_PROCESSORS) || (len == 0U)) {
        return;
    }

    inst = &g_instances[processor_id];
    jtag_uart_tcp_ensure_listener(processor_id);
    jtag_uart_tcp_accept_if_pending(inst);

    if (inst->client_sock != INVALID_SOCKET) {
        if (send(inst->client_sock, data, (int) len, 0) != SOCKET_ERROR) {
            return;
        }
        /* Client gone (closed the terminal, or a full send buffer on a
         * stalled one) -- drop it and fall through to the console fallback
         * below for this write; the next write re-attempts accept(). */
        closesocket(inst->client_sock);
        inst->client_sock = INVALID_SOCKET;
    }

    /* No terminal attached (or it just dropped): shared console fallback,
     * every processor's output interleaved with an "[N] " prefix. */
    fprintf(stdout, "[%u] ", processor_id);
    fwrite(data, 1, len, stdout);
    fflush(stdout);
}

unsigned int jtag_uart_tcp_read_processor(unsigned int processor_id, char *buf, unsigned int buf_size)
{
    jtag_uart_tcp_instance_t *inst;
    int received;

    if ((processor_id < 1U) || (processor_id > (unsigned int) JTAG_UART_TCP_MAX_PROCESSORS) || (buf_size == 0U)) {
        return 0U;
    }

    inst = &g_instances[processor_id];
    jtag_uart_tcp_ensure_listener(processor_id);
    jtag_uart_tcp_accept_if_pending(inst);

    if (inst->client_sock == INVALID_SOCKET) {
        return 0U; /* nothing to read without an attached terminal, by construction */
    }

    received = recv(inst->client_sock, buf, (int) buf_size, 0);
    if (received > 0) {
        return (unsigned int) received;
    }
    if (received == 0) {
        /* Peer closed the connection cleanly. */
        closesocket(inst->client_sock);
        inst->client_sock = INVALID_SOCKET;
    }
    /* received < 0: WSAEWOULDBLOCK (nothing available yet) or a real error --
     * either way, nothing to hand back this poll. */
    return 0U;
}

#endif /* !__niosX_arch__ */
