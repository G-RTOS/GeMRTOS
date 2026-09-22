/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS hardware triggers and ISR-as-task: a real interrupt
 *         (the JTAG UART), demonstrating both the trigger happy path and
 *         the trigger timeout path.
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

/* jtag_uart_trigger.c/.h (this example's own copy of the JTAG UART
 * trigger/ISR driver) creates the hardware-IRQ trigger, its ISR
 * task, and the two software-trigger-driven read/write server tasks. This
 * file only adds task_uart_announcer, a small periodic task that publishes
 * an outgoing line through the write server every ANNOUNCE_PERIOD_S
 * seconds -- proof, deterministic and observable on every run, that the
 * write-side trigger's happy path (FIFO drain on a real interrupt) works,
 * without requiring anyone to type over the JTAG UART by hand. The read
 * side's trigger TIMEOUT path fires on its own,
 * whenever nobody is typing -- see jtag_uart_trigger.c.
 */

#include <gemrtos.h>
#include <system.h>
#include "jtag_uart_trigger.h"

#define ANNOUNCE_PERIOD_S       3U
#define ANNOUNCE_START_OFFSET_S 6U  /* after the driver's own 5 s start offset */

static void task_uart_announcer(void *pdata)
{
    (void) pdata;
    unsigned int count = 0U;

    while (G_TRUE) {
        count++;
        /* Published through the write server's trigger-driven FIFO drain --
         * see jtag_uart_write_server() in jtag_uart_trigger.c. */
        gu_MessageQueuePrintf(jtag_uart_0_info->pqueue_out,
                               "announcer: tick %u\n", count);
        gu_TaskDelay(0, 0, ANNOUNCE_PERIOD_S, 0);
    }
}

int main(void)
{
    GS_TCB *ptcb;

    if (jtag_uart_trigger_init() != G_TRUE) {
        gu_printf("ERROR: could not initialize the JTAG UART trigger driver\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) task_uart_announcer, (void *) 0, "uart_announcer");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 0);
        gu_TaskStartWithOffset(ptcb, 0, 0, ANNOUNCE_START_OFFSET_S, 0);
    } else {
        gu_printf("ERROR: could not create uart_announcer task\n");
        while (1);
    }

    return (0);
}
