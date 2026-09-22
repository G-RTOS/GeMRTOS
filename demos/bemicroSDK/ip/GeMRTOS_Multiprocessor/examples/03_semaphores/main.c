/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS synchronization primitives: all four semaphore flavours,
 *         acquisition timeouts, and the static priority-ceiling protocol.
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

/* Four independent scenarios, each isolated via its own semaphore handle(s)
 * and task set -- see README.md for the full picture:
 *
 *   Scenario A (ping, pong)              Binary semaphore used as a
 *                                         one-shot signal from one task to
 *                                         another.
 *   Scenario B (pool_holder x2,          Counting semaphore as a bounded
 *               pool_impatient)          resource pool; pool_impatient
 *                                         exercises the acquisition-timeout
 *                                         path deterministically.
 *   Scenario C (mutex_low, mutex_medium, Plain mutex and GeMRTOS's static
 *               mutex_high)              priority-ceiling protocol:
 *                                         mutex_high must not stay starved
 *                                         behind mutex_medium once the
 *                                         ceiling has ratcheted in.
 *   Scenario D (recursive_demo)          Recursive mutex: nested Take/Give
 *                                         from the same task, via a helper
 *                                         function.
 *
 * Task functions and semaphore creation live in sync_demo.c/.h -- the first
 * multi-file example in this catalogue.
 *
 * Ready/run priority in GeMRTOS: LARGER numeric value = LOWER priority (see
 * gu_TaskReadyPrioritySet()/gu_TaskRunPrioritySet() in gemrtos_core.h).
 */

#include <gemrtos.h>
#include <system.h>
#include "sync_demo.h"

int main(void)
{
    GS_TCB *ptcb;
    unsigned int i;

    if (sync_demo_semaphores_create() != G_TRUE) {
        gu_printf("ERROR: could not create one or more semaphores (pool exhausted)\n");
        while (1);
    }

    /* ---- Scenario A: binary semaphore signal ------------------------- */
    ptcb = gu_TaskCreate((void *) task_ping, (void *) 0, "ping");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, SYNC_START_OFFSET_PING_S, 0);
    } else {
        gu_printf("ERROR: could not create ping task\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) task_pong, (void *) 0, "pong");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, SYNC_START_OFFSET_PONG_S, 0);
    } else {
        gu_printf("ERROR: could not create pong task\n");
        while (1);
    }

    /* ---- Scenario B: counting semaphore pool -------------------------- */
    for (i = 0; i < SYNC_POOL_SLOTS; i++) {
        ptcb = gu_TaskCreate((void *) task_pool_holder, (void *) i, "pool_holder %u", i);
        if (ptcb != (GS_TCB *) 0) {
            gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
            gu_TaskReadyPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
            gu_TaskRunPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
            gu_TaskStartWithOffset(ptcb, 0, 0, SYNC_START_OFFSET_POOL_S, 0);
        } else {
            gu_printf("ERROR: could not create pool_holder %u\n", i);
            while (1);
        }
    }

    ptcb = gu_TaskCreate((void *) task_pool_impatient, (void *) 0, "pool_impatient");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, 0, SYNC_START_OFFSET_IMPATIENT_MS);
    } else {
        gu_printf("ERROR: could not create pool_impatient task\n");
        while (1);
    }

    /* ---- Scenario C: mutex + priority ceiling -------------------------- */
    ptcb = gu_TaskCreate((void *) task_mutex_low, (void *) 0, "mutex_low");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_LOW);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_LOW);
        /* task_mutex_low intentionally starts on the same tick as
         * task_mutex_medium (SYNC_START_OFFSET_LOW_S, zero ms offset). This
         * same-tick start was once suspected of causing a kernel bug that
         * silently dropped the task from the ready list; it was verified on
         * hardware to be SYNC_PRIORITY_DEFAULT starvation instead (fixed in
         * sync_demo.h), not a same-tick collision -- the timed-event list
         * handles ties correctly. */
        gu_TaskStartWithOffset(ptcb, 0, 0, SYNC_START_OFFSET_LOW_S, 0U);
    } else {
        gu_printf("ERROR: could not create mutex_low task\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) task_mutex_medium, (void *) 0, "mutex_medium");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_MEDIUM);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_MEDIUM);
        gu_TaskStartWithOffset(ptcb, 0, 0, SYNC_START_OFFSET_MEDIUM_S, 0);
    } else {
        gu_printf("ERROR: could not create mutex_medium task\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) task_mutex_high, (void *) 0, "mutex_high");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_HIGH);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_HIGH);
        gu_TaskStartWithOffset(ptcb, 0, 0, 0, SYNC_START_OFFSET_HIGH_MS);
    } else {
        gu_printf("ERROR: could not create mutex_high task\n");
        while (1);
    }

    /* ---- Scenario D: recursive mutex ------------------------------------ */
    ptcb = gu_TaskCreate((void *) task_recursive_demo, (void *) 0, "recursive_demo");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) SYNC_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, SYNC_START_OFFSET_RECURSIVE_S, 0);
    } else {
        gu_printf("ERROR: could not create recursive_demo task\n");
        while (1);
    }

    return (0);
}
