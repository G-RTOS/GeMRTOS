/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Scheduling-list topology setup for 07_multiprocessor_scheduling.
 *         See topology.h for the design rationale of each list.
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

#include "topology.h"

GS_LCB *g_lcb_affinity  = (GS_LCB *) 0;
GS_LCB *g_lcb_exclusive = (GS_LCB *) 0;

G_UINT32 topology_setup(void)
{
    G_UINT32 cpu_id;

    /* ---- Partitioned list: CPU affinity --------------------------------
     * Associated with ONLY TOPOLOGY_AFFINITY_PROCESSOR. gk_PCB_GetNextTCB()
     * (gemrtos_list.c) only ever scans a processor's own PCB_AssocLCB
     * chain, and this list is simply absent from every other processor's
     * chain -- so a task assigned here cannot be selected by any other
     * processor, full stop. Exclusion is left at GeMRTOS's own default
     * (G_NUMBER_OF_PCB + 1, set by gu_SchedulingListCreate() -- no limit):
     * affinity here comes from selective association, not from a
     * running-count restriction. */
    g_lcb_affinity = gu_SchedulingListCreate(GS_LCBTypeFP);
    if (g_lcb_affinity == (GS_LCB *) 0) {
        return G_FALSE;
    }
    if (gu_SchedulingListAssociateProcessor(g_lcb_affinity,
                                             TOPOLOGY_AFFINITY_PROCESSOR,
                                             TOPOLOGY_AFFINITY_PRIORITY) != G_TRUE) {
        return G_FALSE;
    }

    /* ---- Shared list: mutual exclusion between lists -------------------
     * Associated with EVERY processor at the same priority, but its
     * exclusion count is capped at TOPOLOGY_EXCLUSION_LIMIT: at most that
     * many of its tasks may be RUNNING anywhere in the system at once,
     * enforced by gk_PCB_GetNextTCB()'s `LCBExclusion > LCBCurrentRunning`
     * check when a processor considers a cross-list candidate. With the
     * limit at 1, two tasks assigned here and eligible on two different
     * processors still never run concurrently -- mutual exclusion with no
     * semaphore involved. */
    g_lcb_exclusive = gu_SchedulingListCreate(GS_LCBTypeFP);
    if (g_lcb_exclusive == (GS_LCB *) 0) {
        return G_FALSE;
    }
    for (cpu_id = 1U; cpu_id <= (G_UINT32) G_NUMBER_OF_PCB; cpu_id++) {
        if (gu_SchedulingListAssociateProcessor(g_lcb_exclusive, cpu_id,
                                                 TOPOLOGY_EXCLUSIVE_PRIORITY) != G_TRUE) {
            return G_FALSE;
        }
    }
    if (gu_SchedulingListExclusionSet(g_lcb_exclusive, TOPOLOGY_EXCLUSION_LIMIT) != G_TRUE) {
        return G_FALSE;
    }

    return G_TRUE;
}
