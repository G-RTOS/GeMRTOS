/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Task bodies for 07_multiprocessor_scheduling. See workload.h for
 *         the responsibility of each task and topology.h for the
 *         scheduling lists they run in.
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

#include "workload.h"
#include "topology.h"

void task_global_worker(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    unsigned int iteration = 0U;

    (void) pdata;

    while (G_TRUE) {
        iteration++;
        gu_printf("[%s]: iteration %u on processor %u\n",
                  ptcb->TCB_description, iteration,
                  (unsigned int) gm_ProcessorId());
        gu_TaskDelay(0, 0, WORKLOAD_GLOBAL_PERIOD_S, 0);
    }
}

void task_affinity_worker(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    unsigned int iteration = 0U;

    (void) pdata;

    while (G_TRUE) {
        iteration++;
        gu_printf("[%s]: iteration %u on processor %u (pinned to %u)\n",
                  ptcb->TCB_description, iteration,
                  (unsigned int) gm_ProcessorId(),
                  (unsigned int) TOPOLOGY_AFFINITY_PROCESSOR);
        gu_TaskDelay(0, 0, WORKLOAD_AFFINITY_PERIOD_S, 0);
    }
}

void task_exclusive_worker(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    unsigned int cycle = 0U;
    volatile unsigned long busy;

    (void) pdata;

    while (G_TRUE) {
        cycle++;
        gu_printf("[%s]: ENTER cycle %u on processor %u\n",
                  ptcb->TCB_description, cycle,
                  (unsigned int) gm_ProcessorId());

        /* Bounded CPU-bound work -- see WORKLOAD_EXCLUSIVE_BUSY_ITERATIONS
         * in workload.h for why this loop exists at all. */
        for (busy = 0UL; busy < WORKLOAD_EXCLUSIVE_BUSY_ITERATIONS; busy++) {
            /* deliberately empty */
        }

        gu_printf("[%s]: EXIT cycle %u on processor %u\n",
                  ptcb->TCB_description, cycle,
                  (unsigned int) gm_ProcessorId());
        gu_TaskDelay(0, 0, WORKLOAD_EXCLUSIVE_PERIOD_S, 0);
    }
}
