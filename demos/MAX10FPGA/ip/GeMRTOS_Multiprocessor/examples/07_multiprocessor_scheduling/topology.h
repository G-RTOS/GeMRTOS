/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Scheduling-list topology for the 07_multiprocessor_scheduling
 *         example: creates the two dedicated GS_LCB scheduling lists that
 *         demonstrate CPU affinity (partitioned scheduling) and mutual
 *         exclusion between lists, on top of the always-present default
 *         (global) list.
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

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include <gemrtos.h>

/* This example is GeMRTOS's multiprocessor showcase and has no meaningful
 * single-processor behaviour -- fail the build with a clear diagnostic
 * rather than link and misbehave on a BSP configured for one processor.
 * G_NUMBER_OF_PCB expands to GEMRTOS_DRIVER_NPROCESSORS (system.h,
 * Platform-Designer-generated), so this is checkable purely at compile
 * time. */
#if (G_NUMBER_OF_PCB < 2)
#error "07_multiprocessor_scheduling requires G_NUMBER_OF_PCB >= 2 (see system.h: GEMRTOS_DRIVER_NPROCESSORS). This BSP is configured for a single processor."
#endif

/* Association priority for the affinity list: lower numeric value = higher
 * priority (gu_SchedulingListAssociateProcessor()). The default list is
 * associated at G_DEFAULT_LCB_PRIORITY (0x7FFFFFF0, gemrtos_core.h) on
 * every processor at boot, so any modest value here makes this list the
 * FOREGROUND list on the processor it is associated with -- its ready
 * tasks always preempt DEFAULT's, per the documented multi-LCB design
 * model. */
#define TOPOLOGY_AFFINITY_PRIORITY   50U

/* 1-based processor index pinned to the affinity list. NOTE: despite the
 * inline comment on gu_SchedulingListAssociateProcessor()'s prototype
 * (gemrtos_core.h) claiming a "zero-based processor index", the actual
 * implementation (gemrtos_core.c) asserts
 * `(cpu_id >= 1) && (cpu_id <= G_NUMBER_OF_PCB)` and indexes
 * `G_PCBTbl[cpu_id-1]` -- cpu_id is 1-based, matching PCBID and
 * gm_ProcessorId(). Verified by reading the implementation, not the
 * header comment, rather than assuming the documentation is up to date.
 * Confirmed as a real doc defect, reported separately. */
#define TOPOLOGY_AFFINITY_PROCESSOR  2U

/* Association priority for the shared/exclusive list. 100 is deliberately
 * worse (numerically larger) than TOPOLOGY_AFFINITY_PRIORITY -- the two
 * custom lists never compete with each other for this example's purposes,
 * only against DEFAULT -- and matches the value hardware-confirmed working
 * against this exact fix. */
#define TOPOLOGY_EXCLUSIVE_PRIORITY 100U

/* Maximum number of gu_SchedulingListExclusionSet()'s LIST's tasks allowed
 * RUNNING anywhere in the system at once (gemrtos_task.c's Doxygen comment
 * on gu_SchedulingListExclusionSet() is the accurate one -- the terse
 * inline comment on the gemrtos_core.h prototype, "bitmask of processor
 * IDs", is a second confirmed doc defect: the field it actually sets,
 * GS_LCB::LCBExclusion, is documented at its declaration as "Maximum
 * number of running task", and gk_PCB_GetNextTCB() checks it as a count
 * (`LCBExclusion > LCBCurrentRunning`), not a mask). 1 means at most one
 * of this list's tasks may run anywhere, on any processor, at a time --
 * mutual exclusion between processors with no semaphore involved. */
#define TOPOLOGY_EXCLUSION_LIMIT      1U

/** \brief Scheduling list associated with only TOPOLOGY_AFFINITY_PROCESSOR
 *         -- tasks assigned to it can never run on any other processor.
 *         Valid only after topology_setup() returns G_TRUE. */
extern GS_LCB *g_lcb_affinity;

/** \brief Scheduling list associated with every processor but capped by
 *         TOPOLOGY_EXCLUSION_LIMIT -- tasks assigned to it may run on any
 *         processor, but never more than TOPOLOGY_EXCLUSION_LIMIT of them
 *         concurrently, system-wide. Valid only after topology_setup()
 *         returns G_TRUE. */
extern GS_LCB *g_lcb_exclusive;

/** \brief Creates g_lcb_affinity and g_lcb_exclusive and configures their
 *         processor associations and exclusion limits. Must be called
 *         once from main(), before any task is assigned to either list
 *         via gu_SchedulingListAssociateTask().
 *  \return G_TRUE if both lists were created and configured; G_FALSE if
 *          any underlying pool was exhausted or association failed. */
G_UINT32 topology_setup(void);

#endif /* TOPOLOGY_H_ */
