/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS fault handling and robustness: a deliberate task-deadline
 *         overrun (G_SCBType_TCB_ABORTED) and a deliberate timed-event
 *         processing backlog (G_SCBType_FROZEN_MODE /
 *         G_SCBType_UNFROZEN_MODE), both self-recovering, neither halting
 *         the kernel.
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

/* Two independent, deliberately-induced faults -- see README.md for the
 * full mechanism of each, and fault_handlers.c/.h for the task bodies and
 * signal callbacks:
 *
 *   Fault 1 (fault_glitchy_worker)   A G_TCBType_Periodic task that
 *                                    overruns its own period every Nth
 *                                    release -- fires
 *                                    G_SCBType_TCB_ABORTED, recovers on
 *                                    the very next release.
 *   Fault 2 (fault_burst_member x N) N periodic tasks releasing on the
 *                                    exact same tick against a lowered
 *                                    frozen-mode threshold -- the
 *                                    resulting timed-event service backlog
 *                                    fires G_SCBType_FROZEN_MODE, then
 *                                    G_SCBType_UNFROZEN_MODE once the
 *                                    backlog drains.
 *
 * Neither fault touches a critical section or disables interrupts --
 * both are consequences of ordinary, documented task scheduling under
 * deliberately concentrated load, and both recover unattended.
 */

#include <gemrtos.h>
#include <system.h>
#include "fault_handlers.h"

int main(void)
{
    GS_TCB *ptcb;
    unsigned int i;

    /* ---- Fault 1: deliberate deadline overrun ------------------------- */
    ptcb = gu_TaskCreate((void *) fault_glitchy_worker, (void *) 0, "glitchy_worker");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_Periodic);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 10);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 10);
        gu_TaskPeriodSet(ptcb, 0, 0, 0, FAULT_GLITCH_PERIOD_MS);
        gu_TaskStartWithOffset(ptcb, 0, 0, FAULT_GLITCH_START_OFFSET_S, 0);
        gu_SignalCreate(G_SCBType_TCB_ABORTED, 0, (void *) ptcb,
                         (void *) fault_signal_task_aborted, (void *) 0);
    } else {
        gu_printf("ERROR: could not create glitchy_worker task\n");
        while (1);
    }

    /* ---- Fault 2: deliberate timed-event backlog ----------------------
     * Lower the frozen-mode threshold from the kernel's 500 ms default
     * (gemrtos_kernel.c:563) before any burst task is created, and
     * register the KCB-level signals (pxcb = NULL -- not task-specific). */
    gu_FrozenModeThresholdSet(gu_ConvertTime(0, 0, 0, FAULT_FROZEN_THRESHOLD_MS));
    gu_SignalCreate(G_SCBType_FROZEN_MODE, 0, (void *) 0,
                     (void *) fault_signal_frozen, (void *) 0);
    gu_SignalCreate(G_SCBType_UNFROZEN_MODE, 0, (void *) 0,
                     (void *) fault_signal_unfrozen, (void *) 0);

    for (i = 0; i < FAULT_BURST_TASK_COUNT; i++) {
        ptcb = gu_TaskCreate((void *) fault_burst_member, (void *) 0, "burst_member %u", i);
        if (ptcb != (GS_TCB *) 0) {
            gu_TaskTypeSet(ptcb, G_TCBType_Periodic);
            gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 20);
            gu_TaskRunPrioritySet(ptcb, (G_UINT64) 20);
            gu_TaskPeriodSet(ptcb, 0, 0, FAULT_BURST_PERIOD_S, 0);
            gu_TaskStartWithOffset(ptcb, 0, 0, FAULT_BURST_START_OFFSET_S, 0);
        } else {
            gu_printf("ERROR: could not create burst_member %u\n", i);
            while (1);
        }
    }

    return (0);
}
