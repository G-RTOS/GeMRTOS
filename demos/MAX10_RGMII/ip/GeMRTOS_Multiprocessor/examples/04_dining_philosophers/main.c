/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS dining philosophers example: classic contention and
 *         deadlock-avoidance stress demo.
 *  \version 1.0
 *  \date      2026-08-03 (G_SCBType_TCB_ABORTED registration removed; see
 *             README.md "Extraction notes")
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

/* Extracted from examples/10_full_demo/hellogemrtos.c as a
 * behaviour-preserving move -- see README.md
 * "Extraction notes" for exactly what changed versus what was preserved
 * byte-for-byte.
 *
 * NUMBER_PHILOSOPHERS philosopher tasks share NUMBER_PHILOSOPHERS forks
 * arranged in a ring: philosopher i needs fork i (left) and fork
 * (i+1) % NUMBER_PHILOSOPHERS (right). Acquiring both with a bounded
 * timeout, in an order that alternates by index parity (even philosophers
 * take left-then-right, odd take right-then-left), is the classic
 * asymmetric-ordering deadlock-avoidance strategy -- see philosophers.c's
 * task_philosopher() for the mechanism itself.
 *
 * Task functions and semaphore creation live in philosophers.c/.h, the
 * second multi-file example in this catalogue.
 *
 * No G_SCBType_TCB_ABORTED signal is registered here, unlike the original
 * combined demo: these philosophers are G_TCBType_OneShot,
 * self-rescheduled via their own gu_TaskDelay() at the end of
 * task_philosopher() -- never via the kernel's G_ECBType_PERIODIC
 * mechanism. G_SCBType_TCB_ABORTED is checked in exactly one place,
 * gk_TASK_RELEASE() (gemrtos_list.c), whose only caller is
 * gk_TimeCallback()'s G_ECBType_PERIODIC case (gemrtos_task.c), which only
 * exists for G_TCBType_Periodic/_Periodic_Skip tasks -- so the signal the
 * original registered here could never fire, for any philosopher, on any
 * run. See 08_signals_and_frozen_mode for a task that is genuinely
 * G_TCBType_Periodic and can actually reach this signal. */

#include <gemrtos.h>
#include <system.h>
#include "philosophers.h"

int main(void)
{
    GS_TCB *ptcb;
    unsigned int i;

    if (philosophers_semaphores_create() != G_TRUE) {
        gu_printf("ERROR: could not create fork/LED semaphores (pool exhausted)\n");
        while (1);
    }

    for (i = 0; i < NUMBER_PHILOSOPHERS; i++) {
        ptcb = gu_TaskCreate((void *) task_philosopher, (void *) i, "task %d", i);
        if (ptcb != (GS_TCB *) 0) {
            gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
            gu_TaskReadyPrioritySet(ptcb, (G_UINT64) i + 10);
            gu_TaskRunPrioritySet(ptcb, (G_UINT64) i + 10);
            gu_TaskPeriodSet(ptcb, 0, 0, 1, (i + 1) * 100);
            gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);
        } else {
            gu_printf("ERROR: could not create philosopher task %u\n", i);
            while (1);
        }
    }

    return (0);
}
