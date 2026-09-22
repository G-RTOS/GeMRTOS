/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Fault-handling task bodies and signal callbacks for
 *         08_signals_and_frozen_mode. See fault_handlers.h for the
 *         mechanism behind each fault.
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

#include "fault_handlers.h"

void fault_glitchy_worker(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    static unsigned int release = 0U;
    volatile unsigned long busy;

    (void) pdata;

    release++;

    if ((release % FAULT_GLITCH_OVERRUN_EVERY_N) == 0U) {
        gu_printf("[%s] release %u: deliberately overrunning (busy loop, period is %u ms)\n",
                  ptcb->TCB_description, release, (unsigned int) FAULT_GLITCH_PERIOD_MS);
        for (busy = 0UL; busy < FAULT_GLITCH_OVERRUN_BUSY_ITERATIONS; busy++) {
            /* deliberately empty: simulates real work exceeding the period */
        }
        gu_printf("[%s] release %u: overrun finished (too late -- the next release was already due)\n",
                  ptcb->TCB_description, release);
    } else {
        gu_printf("[%s] release %u: normal, finishes well within period\n",
                  ptcb->TCB_description, release);
    }
}

void fault_signal_task_aborted(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    (void) pdata;

    /* Signal callbacks run outside the GeMRTOS critical section -- same
     * invariant 04_dining_philosophers/philosophers.c's
     * signal_philosopher_task_aborted() asserts. */
    PRINT_ASSERT((!(gm_IsPrcGranted())), "ERROR signal callback holding critical section\n");

    gu_printf("[%s] ABORTED: previous release was still running when this one came due -- recovered automatically\n",
              ptcb->TCB_description);
}

void fault_burst_member(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    (void) pdata;

    gu_printf("[%s] released\n", ptcb->TCB_description);
}

void fault_signal_frozen(void *pdata)
{
    (void) pdata;
    gu_printf("fault_handlers: FROZEN_MODE signal delivered on processor %u -- timed-event backlog exceeded threshold\n",
              (unsigned int) gm_ProcessorId());
}

void fault_signal_unfrozen(void *pdata)
{
    (void) pdata;
    gu_printf("fault_handlers: UNFROZEN_MODE signal delivered on processor %u -- backlog drained, recovered\n",
              (unsigned int) gm_ProcessorId());
}
