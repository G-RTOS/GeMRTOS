/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS task model: periodic vs one-shot tasks, ready/run priority,
 *         start offsets, and external suspend/resume.
 *  \version 1.0
 *  \date      2026-07-30
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

/* Three tasks, three roles, one file — see README.md for the full picture:
 *
 *   task_periodic_beacon  G_TCBType_Periodic. The kernel re-releases it
 *                         every period automatically; the task body does
 *                         its work and returns, it never delays itself.
 *
 *   task_worker           G_TCBType_OneShot with its own while(1) loop,
 *                         released once and re-scheduling itself via
 *                         gu_TaskDelay() — the "recurring task" pattern
 *                         used everywhere else in this codebase (see
 *                         10_full_demo's task_infinite_loop()).
 *
 *   task_supervisor        G_TCBType_OneShot, highest priority. Every few
 *                         seconds it suspends task_worker, holds it
 *                         suspended for a while, then resumes it —
 *                         demonstrating gu_TaskSuspend()/gu_TaskResume()
 *                         as external control over a task's own delay loop.
 *
 * Ready/run priority in GeMRTOS: LARGER numeric value = LOWER priority (see
 * gu_TaskReadyPrioritySet()/gu_TaskRunPrioritySet() in gemrtos_core.h).
 * Here: supervisor=0 (highest) > beacon=5 > worker=10 (lowest).
 */

#include <gemrtos.h>
#include <system.h>

#define BEACON_PERIOD_MS            1000U  /* task_periodic_beacon release period */
#define BEACON_START_OFFSET_S          2U  /* beacon's first release             */
#define WORKER_LOOP_DELAY_S             1U  /* task_worker delay, seconds part    */
#define WORKER_LOOP_DELAY_MS          500U  /* task_worker delay, ms part (1.5 s total) */
#define WORKER_START_OFFSET_S           2U  /* worker starts alongside the beacon */
#define SUPERVISOR_START_OFFSET_MS    100U  /* supervisor starts before the others */
#define SUPERVISOR_CYCLE_DELAY_S        3U  /* gap between suspend attempts        */
#define SUPERVISOR_HOLD_DELAY_S          2U  /* how long worker stays suspended     */

/* gu_TaskSuspend() only succeeds while its target is READY or RUNNING (see
 * gu_TaskSuspend()'s documentation in gemrtos_task.c) — it correctly returns
 * G_FALSE, rather than asserting, if task_worker happens to be inside its own
 * gu_TaskDelay() at the moment the supervisor tries. WORKER_BUSY_ITERATIONS
 * gives task_worker a deterministic, non-trivial RUNNING window each
 * iteration so the retry loop below reliably succeeds within a handful of
 * attempts regardless of clock speed, instead of only ever catching the
 * task during its single-instruction wake-up.
 *
 * The embedded value below is tuned against real
 * Nios V hardware and does not translate to host CPU speed. Measured
 * directly on the Windows-simulation dev machine, with this tier's own
 * build flags (mingw gcc, "-g", no explicit optimization level, matching
 * verify_windows.ps1's gccArgs): 2,000,000 iterations take ~5 ms there —
 * not comfortably longer than SUPERVISOR_RETRY_DELAY_MS's 5 ms poll
 * granularity, so whether a given poll catches task_worker RUNNING is close
 * to a coin flip instead of "reliably within a handful of attempts" as the
 * comment above promises. 12,000,000 measured ~27–31 ms across three runs on
 * the same machine — roughly 6x the poll granularity, restoring the margin
 * the embedded value already has on real hardware. Host-only: embedded
 * keeps the original, unchanged value (this is a single numeric constant,
 * not an algorithm, so a guarded #define is the smaller and clearer
 * change). */
#ifndef __niosX_arch__
    #define WORKER_BUSY_ITERATIONS  12000000U
#else
    #define WORKER_BUSY_ITERATIONS  2000000U
#endif
#define SUPERVISOR_RETRY_DELAY_MS       5U
#define SUPERVISOR_MAX_RETRIES        400U  /* ~2 s worst case at 5 ms/retry */

/* task_supervisor needs task_worker's TCB to suspend/resume it; assigned in
 * main() before any task actually starts running. */
static GS_TCB *g_worker_tcb = (GS_TCB *) 0;

/******************************************************************/
/* task_periodic_beacon: strictly periodic, re-released by the    */
/* kernel every BEACON_PERIOD_MS. Does its work and returns.      */
/******************************************************************/
void task_periodic_beacon(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    static unsigned int tick = 0;

    gu_printf("[%s] tick %u\n", ptcb->TCB_description, tick);
    tick++;
}

/******************************************************************/
/* task_worker: OneShot task that keeps itself running with its   */
/* own while(1) + gu_TaskDelay(), the way most "recurring" tasks  */
/* in GeMRTOS are written. Runs a deterministic busy window before */
/* delaying, so task_supervisor has a real chance to observe it   */
/* READY/RUNNING and suspend it (see task_supervisor() below).    */
/******************************************************************/
void task_worker(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    unsigned int iteration = 0;
    volatile unsigned int busy;

    while (1) {
        gu_printf("[%s] iteration %u starting\n", ptcb->TCB_description, iteration);

        for (busy = 0; busy < WORKER_BUSY_ITERATIONS; busy++) {
            /* deliberately empty: simulates real work taking real time */
        }

        gu_printf("[%s] iteration %u done\n", ptcb->TCB_description, iteration);
        iteration++;

        gu_TaskDelay(0, 0, WORKER_LOOP_DELAY_S, WORKER_LOOP_DELAY_MS);
    }
}

/******************************************************************/
/* task_supervisor: highest priority. Periodically suspends       */
/* task_worker, holds it suspended, then resumes it — demonstrating */
/* gu_TaskSuspend()/gu_TaskResume() as control exercised by one    */
/* task over another, distinct from a task delaying itself.       */
/******************************************************************/
void task_supervisor(void *pdata)
{
    while (1) {
        gu_TaskDelay(0, 0, SUPERVISOR_CYCLE_DELAY_S, 0);

        G_UINT32 suspended = G_FALSE;
        unsigned int attempt;

        for (attempt = 0; attempt < SUPERVISOR_MAX_RETRIES; attempt++) {
            suspended = gu_TaskSuspend(g_worker_tcb);
            if (suspended == G_TRUE) {
                break;
            }
            gu_TaskDelay(0, 0, 0, SUPERVISOR_RETRY_DELAY_MS);
        }

        if (suspended == G_TRUE) {
            gu_printf("supervisor: worker suspended (attempt %u)\n", attempt + 1);

            gu_TaskDelay(0, 0, SUPERVISOR_HOLD_DELAY_S, 0);

            /* task_worker was suspended while READY/RUNNING, so it is now
             * WAITING and stays there until this call — gu_TaskResume()
             * only acts on a WAITING/WAITING_COMPLETED task, so this
             * always succeeds here with no retry needed. */
            gu_TaskResume(g_worker_tcb);
            gu_printf("supervisor: worker resumed\n");
        } else {
            gu_printf("supervisor: could not suspend worker within %u attempts, "
                      "skipping this cycle\n", SUPERVISOR_MAX_RETRIES);
        }
    }
}

int main(void)
{
    GS_TCB *ptcb;

    /* ---- beacon: strictly periodic ---------------------------------- */
    ptcb = gu_TaskCreate((void *) task_periodic_beacon, (void *) 0, "beacon");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_Periodic);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 5);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 5);
        gu_TaskPeriodSet(ptcb, 0, 0, 0, BEACON_PERIOD_MS);
        gu_TaskStartWithOffset(ptcb, 0, 0, BEACON_START_OFFSET_S, 0);
    } else {
        gu_printf("ERROR: could not create beacon task\n");
        while (1);
    }

    /* ---- worker: OneShot, self-looping, lowest priority -------------- */
    ptcb = gu_TaskCreate((void *) task_worker, (void *) 0, "worker");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 10);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 10);
        gu_TaskStartWithOffset(ptcb, 0, 0, WORKER_START_OFFSET_S, 0);
        g_worker_tcb = ptcb;
    } else {
        gu_printf("ERROR: could not create worker task\n");
        while (1);
    }

    /* ---- supervisor: OneShot, highest priority, starts first --------- */
    ptcb = gu_TaskCreate((void *) task_supervisor, (void *) 0, "supervisor");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 0);
        gu_TaskStartWithOffset(ptcb, 0, 0, 0, SUPERVISOR_START_OFFSET_MS);
    } else {
        gu_printf("ERROR: could not create supervisor task\n");
        while (1);
    }

    return (0);
}
