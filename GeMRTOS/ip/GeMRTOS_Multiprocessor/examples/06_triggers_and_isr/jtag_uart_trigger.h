/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Own copy of the JTAG UART trigger/ISR driver for the
 *         06_triggers_and_isr example (examples are fully
 *         self-contained; no shared code between examples).
 *  \version 1.0
 *  \date      2026-07-31
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

#ifndef JTAG_UART_TRIGGER_H_
#define JTAG_UART_TRIGGER_H_

#include <gemrtos.h>

/* JTAG-UART read-trigger timeout: how long jtag_uart_read_server() waits
 * for input activity before printing a "read timeout" line. This is the
 * demonstration of the trigger timeout path -- with
 * nobody typing over the JTAG UART during an unattended hardware
 * verification run, this path fires deterministically, once per timeout
 * window, on every run. */
#define JTAG_TRIGGER_READ_TIMEOUT_MS 100U

#define JTAG_UART_BUFFER_MAX_IN  100U  /* MISRA C:2012 Rule 21.3: static max for jtag_uart_read_server buffer  */
#define JTAG_UART_BUFFER_MAX_OUT 100U  /* MISRA C:2012 Rule 21.3: static max for jtag_uart_write_server buffer */

/** \brief Per-instance state for one JTAG UART trigger/ISR driver instance:
 *         two software triggers (read, write), two message queues (input
 *         line assembly, output line delivery), and the hardware register
 *         base needed to read/write the peripheral directly from the ISR
 *         and the two server tasks. */
struct jtag_uart_info {
    GS_RCB *trigger_out;
    GS_RCB *trigger_in;
    GS_RCB *pqueue_out;
    GS_RCB *pqueue_in;
    unsigned int buffer_length_out;
    unsigned int buffer_length_in;
    unsigned int jtag_base;
};

/** \brief Global instance, exposed so main.c (or any task in this example)
 *         can publish outgoing lines via
 *         gu_MessageQueuePrintf(jtag_uart_0_info->pqueue_out, ...) — the
 *         same pattern 10_full_demo's philosophers used against the shared
 *         JTAG UART server. */
extern struct jtag_uart_info *jtag_uart_0_info;

/** \brief Creates the two triggers, two message queues, the hardware-IRQ
 *         trigger, and the three tasks (write server, read server, ISR
 *         manager) that make up this driver instance. Must be called once
 *         from main(), before any other task in the example depends on
 *         jtag_uart_0_info being usable.
 *  \return G_TRUE if every resource and task was created; G_FALSE if any
 *          pool was exhausted (same fail-fast convention every other
 *          example in this catalogue uses). */
G_UINT32 jtag_uart_trigger_init(void);

#endif /* JTAG_UART_TRIGGER_H_ */
