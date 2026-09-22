/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Task bodies for the 07_multiprocessor_scheduling example: one
 *         workload per scheduling-list configuration (global/default,
 *         affinity-pinned, mutually-exclusive), each printing its own
 *         processor ID so the task-to-processor mapping is directly
 *         observable on the UART, not merely claimed.
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

#ifndef WORKLOAD_H_
#define WORKLOAD_H_

#include <gemrtos.h>

/* Ready/run priority: LARGER numeric value = LOWER priority (see
 * gu_TaskReadyPrioritySet()/gu_TaskRunPrioritySet(), gemrtos_core.h). Every
 * task in this example shares one priority band -- within each scheduling
 * list, which list a task belongs to (topology.h) is what this example
 * demonstrates, not intra-list priority ordering. */
#define WORKLOAD_PRIORITY_DEFAULT 20U

/* How many independent global-list workers to create; each prints its own
 * processor on every iteration, so migration between processors 1 and 2
 * over the run is what makes "global scheduling" observable. */
#define WORKLOAD_GLOBAL_WORKERS 2U

/* Start offsets, staggered so the three configurations' first outputs
 * don't all land on the same tick and interleave unreadably. */
#define WORKLOAD_START_OFFSET_GLOBAL_S    1U
#define WORKLOAD_START_OFFSET_AFFINITY_S  2U
#define WORKLOAD_START_OFFSET_EXCLUSIVE_S 3U

#define WORKLOAD_GLOBAL_PERIOD_S    2U
#define WORKLOAD_AFFINITY_PERIOD_S  2U
#define WORKLOAD_EXCLUSIVE_PERIOD_S 3U

/* Bounded CPU-bound work inside each exclusive-worker cycle, so the two
 * exclusive workers actually hold their RUNNING slot for a visible
 * duration instead of yielding immediately -- otherwise the exclusion
 * limit would have nothing to exclude. Calibrated the same way as
 * 03_semaphores/sync_demo.h's busy loop (empirically, not derived from a
 * clock frequency): large enough to keep ENTER/EXIT pairs from
 * interleaving under normal capture windows, small enough that a full
 * verification run observes several cycles from both workers. */
#define WORKLOAD_EXCLUSIVE_BUSY_ITERATIONS 500000UL

/** \brief Global/default-list worker: created with no scheduling-list
 *         association, so it competes in G_TASK_LCB_DEFAULT like any
 *         ordinary GeMRTOS task and may run on whichever processor is
 *         free -- demonstrating global scheduling. Identifies itself in
 *         its output via TCB_description (set from main()'s gu_TaskCreate()
 *         format string), following this catalogue's existing convention
 *         (see 03_semaphores/sync_demo.c) rather than an index in pdata.
 *  \param [in] pdata  Unused. */
void task_global_worker(void *pdata);

/** \brief Affinity-list worker: must be assigned to g_lcb_affinity via
 *         gu_SchedulingListAssociateTask() before being started. Always
 *         reports TOPOLOGY_AFFINITY_PROCESSOR as its running processor --
 *         demonstrating partitioned scheduling / CPU affinity.
 *  \param [in] pdata  Unused. */
void task_affinity_worker(void *pdata);

/** \brief Exclusive-list worker: must be assigned to g_lcb_exclusive via
 *         gu_SchedulingListAssociateTask() before being started. Two
 *         instances run concurrently eligible on two different
 *         processors, yet their ENTER/EXIT brackets never interleave --
 *         demonstrating mutual exclusion between lists with no semaphore.
 *  \param [in] pdata  Unused. */
void task_exclusive_worker(void *pdata);

#endif /* WORKLOAD_H_ */
