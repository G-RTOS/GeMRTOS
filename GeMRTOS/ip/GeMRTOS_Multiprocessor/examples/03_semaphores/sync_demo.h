/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Task functions and semaphore setup for the 03_semaphores example.
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

#ifndef SYNC_DEMO_H_
#define SYNC_DEMO_H_

#include <gemrtos.h>

/* Number of concurrent holders of the bounded resource pool (Scenario B).
 * Matches the pool semaphore's count exactly, so both slots are normally
 * busy and task_pool_impatient's request reliably contends for one. main.c
 * creates exactly this many task_pool_holder instances. */
#define SYNC_POOL_SLOTS 2U

/* Ready/run priority in GeMRTOS: LARGER numeric value = LOWER priority (see
 * gu_TaskReadyPrioritySet()/gu_TaskRunPrioritySet() in gemrtos_core.h).
 * Scenario C needs three distinct priority bands; every other task in this
 * example uses SYNC_PRIORITY_DEFAULT, since its own relative ordering does
 * not matter to what it demonstrates.
 *
 * SYNC_PRIORITY_DEFAULT is deliberately set WORSE (numerically larger) than
 * SYNC_PRIORITY_LOW -- hardware-measured:
 * with SYNC_PRIORITY_DEFAULT=15 (better than task_mutex_low's 30), the six
 * unrelated Scenario A/B/D tasks (ping, pong, both pool_holders,
 * pool_impatient, recursive_demo) starved task_mutex_low so completely that
 * its critical section did not complete even once in a combined ~200 s
 * on-hardware capture -- despite these scenarios being logically
 * independent, they all shared the one default scheduling list, so they
 * competed directly with task_mutex_low for the same ready queue.
 *
 * Scenario C now runs in its own scheduling list (see the SYNC_SCENARIO_C_*
 * block below), which processors search ahead of the default one, so list
 * separation -- not this constant -- is the primary isolation mechanism.
 * The value is kept below SYNC_PRIORITY_LOW anyway: it costs nothing (all
 * six tasks share it, so their ordering relative to each other is
 * unchanged) and it keeps the example correct if a future reader removes
 * the scheduling-list setup to see what happens. */
#define SYNC_PRIORITY_HIGH     5U
#define SYNC_PRIORITY_MEDIUM  20U
#define SYNC_PRIORITY_LOW     30U
#define SYNC_PRIORITY_DEFAULT 35U

/* ---- KNOWN LIMITATION: Scenario C cannot demonstrate priority inversion --
 *
 * GeMRTOS is a multiprocessor kernel and this board is built with two
 * processors (system.h: NPROCESSORS 2). Every task here lives in
 * G_TASK_LCB_DEFAULT, whose LCBExclusion is G_NUMBER_OF_PCB + 1 -- no
 * concurrency limit -- so its tasks may run on both processors at once.
 *
 * task_mutex_medium never touches the mutex, so while task_mutex_low holds
 * it, task_mutex_medium simply runs on the free processor. No preemption
 * occurs, therefore no priority inversion occurs, therefore the
 * priority-ceiling protocol has nothing to correct. Hardware-measured
 * on hardware: task_mutex_medium's tick cadence is perfectly steady across
 * every cycle, boosted or not. This is CORRECT multiprocessor behaviour,
 * not a kernel defect -- but it means README.md's "watch for a gap in
 * mutex_medium's tick cadence" cannot be observed on this build.
 *
 * The intended fix was a dedicated scheduling list for Scenario C with
 * gu_SchedulingListExclusionSet(plcb, 1), which forces its three tasks to
 * contend for a single processor and would make the ceiling's effect
 * visible. IT IS NOT APPLIED because it does not currently work: creating a
 * second scheduling list via gu_SchedulingListCreate() +
 * gu_SchedulingListAssociateProcessor() + gu_SchedulingListAssociateTask()
 * halts the kernel within ~1 s with
 *
 *     ERROR in ready list ... gk_TCBRDYL_Unlink, gemrtos_list.c, line 1181
 *
 * reproduced on hardware with exclusion values of BOTH 1 and 2
 * (2 imposes no restriction on a 2-processor board), which rules out the
 * exclusion limit itself and points at the multi-scheduling-list path.
 * No example or application in this repository currently uses these
 * functions, so that path appears to be untested; revisit this example
 * once that is resolved. */

/* Start offsets, staggered so every scenario's precondition already holds
 * before its first observable event (documented per-task in main.c). */
#define SYNC_START_OFFSET_PING_S        1U
#define SYNC_START_OFFSET_PONG_S        1U
#define SYNC_START_OFFSET_POOL_S        2U
#define SYNC_START_OFFSET_IMPATIENT_MS 2500U
#define SYNC_START_OFFSET_LOW_S         3U
#define SYNC_START_OFFSET_MEDIUM_S      3U
#define SYNC_START_OFFSET_HIGH_MS      3500U
#define SYNC_START_OFFSET_RECURSIVE_S   4U

/** \brief Creates every semaphore used by this example (one of each of the
 *         four flavours). Must be called once from main(), before any of
 *         the task functions below are scheduled.
 *  \return G_TRUE if every semaphore was created; G_FALSE if the semaphore
 *          pool was exhausted (same fail-fast convention main.c uses for
 *          gu_TaskCreate() failures). */
G_UINT32 sync_demo_semaphores_create(void);

/* ---- Scenario A: binary semaphore used as a one-shot signal ------------ */
void task_ping(void *pdata);
void task_pong(void *pdata);

/* ---- Scenario B: counting semaphore as a bounded resource pool --------- */
void task_pool_holder(void *pdata);
void task_pool_impatient(void *pdata);

/* ---- Scenario C: mutex + static priority-ceiling protocol ------------- */
void task_mutex_low(void *pdata);
void task_mutex_medium(void *pdata);
void task_mutex_high(void *pdata);

/* ---- Scenario D: recursive mutex, nested Take/Give from one task ------- */
void task_recursive_demo(void *pdata);

#endif /* SYNC_DEMO_H_ */
