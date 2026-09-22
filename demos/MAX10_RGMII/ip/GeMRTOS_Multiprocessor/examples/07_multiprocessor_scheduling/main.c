/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS multiprocessor scheduling: partitioned scheduling (CPU
 *         affinity), global scheduling, and mutual exclusion between
 *         scheduling lists -- the GeMRTOS differentiator.
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

/* Three independent scheduling-list configurations, each with its own
 * worker task(s) -- see README.md for the full picture and topology.c/.h
 * for the per-list setup rationale:
 *
 *   Global (global_worker 0, 1)     No custom association -- ordinary
 *                                    G_TASK_LCB_DEFAULT tasks, free to run
 *                                    on either processor.
 *   Affinity (affinity_worker)      Assigned to g_lcb_affinity, associated
 *                                    with only one processor -- always
 *                                    reports the same processor.
 *   Exclusive (exclusive_worker 0,  Assigned to g_lcb_exclusive, associated
 *              1)                   with every processor but capped at
 *                                    TOPOLOGY_EXCLUSION_LIMIT concurrent
 *                                    runners -- their ENTER/EXIT brackets
 *                                    never interleave.
 *
 * This example requires G_NUMBER_OF_PCB >= 2 -- enforced at compile time
 * in topology.h.
 */

#include <gemrtos.h>
#include <system.h>
#include "topology.h"
#include "workload.h"

int main(void)
{
    GS_TCB *ptcb;
    unsigned int i;

    if (topology_setup() != G_TRUE) {
        gu_printf("ERROR: could not set up the scheduling-list topology (pool exhausted)\n");
        while (1);
    }

    /* ---- Global: ordinary default-list tasks -------------------------- */
    for (i = 0; i < WORKLOAD_GLOBAL_WORKERS; i++) {
        ptcb = gu_TaskCreate((void *) task_global_worker, (void *) 0, "global_worker %u", i);
        if (ptcb != (GS_TCB *) 0) {
            gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
            gu_TaskReadyPrioritySet(ptcb, (G_UINT64) WORKLOAD_PRIORITY_DEFAULT);
            gu_TaskRunPrioritySet(ptcb, (G_UINT64) WORKLOAD_PRIORITY_DEFAULT);
            gu_TaskStartWithOffset(ptcb, 0, 0, WORKLOAD_START_OFFSET_GLOBAL_S, 0);
        } else {
            gu_printf("ERROR: could not create global_worker %u\n", i);
            while (1);
        }
    }

    /* ---- Affinity: pinned to TOPOLOGY_AFFINITY_PROCESSOR --------------- */
    ptcb = gu_TaskCreate((void *) task_affinity_worker, (void *) 0, "affinity_worker");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        /* Must be assigned to its list before it starts running, so the
         * very first scheduling decision already sees the right list. */
        if (gu_SchedulingListAssociateTask(ptcb, g_lcb_affinity) != G_TRUE) {
            gu_printf("ERROR: could not associate affinity_worker with g_lcb_affinity\n");
            while (1);
        }
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) WORKLOAD_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) WORKLOAD_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, WORKLOAD_START_OFFSET_AFFINITY_S, 0);
    } else {
        gu_printf("ERROR: could not create affinity_worker\n");
        while (1);
    }

    /* ---- Exclusive: capped at TOPOLOGY_EXCLUSION_LIMIT concurrent ------ */
    for (i = 0; i < 2U; i++) {
        ptcb = gu_TaskCreate((void *) task_exclusive_worker, (void *) 0, "exclusive_worker %u", i);
        if (ptcb != (GS_TCB *) 0) {
            gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
            if (gu_SchedulingListAssociateTask(ptcb, g_lcb_exclusive) != G_TRUE) {
                gu_printf("ERROR: could not associate exclusive_worker %u with g_lcb_exclusive\n", i);
                while (1);
            }
            gu_TaskReadyPrioritySet(ptcb, (G_UINT64) WORKLOAD_PRIORITY_DEFAULT);
            gu_TaskRunPrioritySet(ptcb, (G_UINT64) WORKLOAD_PRIORITY_DEFAULT);
            gu_TaskStartWithOffset(ptcb, 0, 0, WORKLOAD_START_OFFSET_EXCLUSIVE_S, 0);
        } else {
            gu_printf("ERROR: could not create exclusive_worker %u\n", i);
            while (1);
        }
    }

    return (0);
}
