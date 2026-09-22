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

#include <gemrtos.h>
#include <system.h>
#include "sync_demo.h"

/* ---- Scenario A: binary semaphore -------------------------------------- */
#define PING_PERIOD_S 2U

/* ---- Scenario B: counting semaphore pool -------------------------------- */
#define POOL_HOLDER_HOLD_S        3U
#define POOL_HOLDER_CYCLE_S       1U
#define POOL_IMPATIENT_TIMEOUT_MS 500U  /* < POOL_HOLDER_HOLD_S: guarantees a
                                          * timeout on task_pool_impatient's
                                          * first attempt, every run. */
#define POOL_IMPATIENT_RETRY_S    1U

/* ---- Scenario C: mutex + static priority-ceiling protocol ------------- */
/* CPU-bound, not gu_TaskDelay()-based: a time-based delay would elapse in
 * real time regardless of scheduling, so it could never show contention.
 *
 * Calibration history, all hardware-measured on this board (50 MHz Nios V/m,
 * no instruction cache -- see system.h INSTRUCTION_CACHE_SIZE "None", which
 * is why each iteration costs far more than its instruction count suggests):
 *
 *  - 20,000,000 (first estimate): never completed inside a 30 s
 *    UART capture.
 *  - 2,000,000: ~11-13 s per critical section while running uncontended on
 *    its own processor. Correct, but too long to show several cycles inside
 *    a normal capture window.
 *  - 500,000 (current): targets ~3 s, roughly ten MUTEX_MEDIUM_TICK_MS
 *    periods -- short enough that a single 60 s capture shows several
 *    complete acquire/release cycles rather than one unfinished one, which
 *    makes the example far easier to observe and verify. */
#define MUTEX_LOW_CRIT_SECTION_ITERATIONS 500000UL
#define MUTEX_LOW_CYCLE_DELAY_S    1U
#define MUTEX_MEDIUM_TICK_MS     300U
#define MUTEX_HIGH_CYCLE_DELAY_S   2U

/* ---- Scenario D: recursive mutex ---------------------------------------- */
#define RECURSIVE_CYCLE_DELAY_S 3U

/* Shared semaphore handles, one file-scope instance per scenario. All four
 * are created once by sync_demo_semaphores_create(), called from main()
 * before any task below runs. */
static t_semaphore_resource *g_sem_ping        = (t_semaphore_resource *) 0;
static t_semaphore_resource *g_sem_pool        = (t_semaphore_resource *) 0;
static t_semaphore_resource *g_mutex_shared    = (t_semaphore_resource *) 0;
static t_semaphore_resource *g_recursive_mutex = (t_semaphore_resource *) 0;

/* Converts gu_SystemTimeGet()'s raw tick count to milliseconds elapsed since
 * boot, for this example's UART timestamps only -- same G_TICKS_PER_MSECOND
 * conversion the web status page uses for its uptime row. */
static G_UINT32 elapsed_ms(void)
{
    return (G_UINT32) (gu_SystemTimeGet() / (G_UINT64) G_TICKS_PER_MSECOND);
}

G_UINT32 sync_demo_semaphores_create(void)
{
    g_sem_ping = gu_SemaphoreCreateBinary(0);  /* starts empty: task_pong
                                                 * blocks until the first
                                                 * Give from task_ping. */
    g_sem_pool = gu_SemaphoreCreateCounting((int) SYNC_POOL_SLOTS, (int) SYNC_POOL_SLOTS);
    g_mutex_shared = gu_SemaphoreCreateMutex();
    g_recursive_mutex = gu_SemaphoreCreateRecursiveMutex();

    if ((g_sem_ping == (t_semaphore_resource *) 0) ||
        (g_sem_pool == (t_semaphore_resource *) 0) ||
        (g_mutex_shared == (t_semaphore_resource *) 0) ||
        (g_recursive_mutex == (t_semaphore_resource *) 0)) {
        return (G_FALSE);
    }
    return (G_TRUE);
}

/* ============ Scenario A: binary semaphore as a one-shot signal ========= */

void task_ping(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    (void) pdata;

    while (1) {
        gu_TaskDelay(0, 0, PING_PERIOD_S, 0);
        gu_printf("[%s] signal sent\n", ptcb->TCB_description);
        gu_SemaphoreGive(g_sem_ping);
    }
}

void task_pong(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    (void) pdata;

    while (1) {
        gu_SemaphoreTake(g_sem_ping, (G_UINT64) G_LATEST_TIME);
        gu_printf("[%s] signal received\n", ptcb->TCB_description);
    }
}

/* ============ Scenario B: counting semaphore as a bounded pool =========== */

void task_pool_holder(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    (void) pdata;  /* only used by main() to format this task's description */

    while (1) {
        gu_SemaphoreTake(g_sem_pool, (G_UINT64) G_LATEST_TIME);
        gu_printf("[%s] slot acquired, %u remaining\n", ptcb->TCB_description,
                  (unsigned int) gu_SemaphoreGetCount(g_sem_pool));

        gu_TaskDelay(0, 0, POOL_HOLDER_HOLD_S, 0);

        gu_SemaphoreGive(g_sem_pool);
        gu_printf("[%s] slot released\n", ptcb->TCB_description);

        gu_TaskDelay(0, 0, POOL_HOLDER_CYCLE_S, 0);
    }
}

void task_pool_impatient(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    G_UINT64 timeout;
    (void) pdata;

    timeout = gu_ConvertTime(0, 0, 0, POOL_IMPATIENT_TIMEOUT_MS);

    while (1) {
        gu_printf("[%s] requesting slot, %u currently free\n", ptcb->TCB_description,
                  (unsigned int) gu_SemaphoreGetCount(g_sem_pool));

        if (gu_SemaphoreTake(g_sem_pool, timeout) == G_TRUE) {
            gu_printf("[%s] slot acquired\n", ptcb->TCB_description);
            gu_SemaphoreGive(g_sem_pool);
            gu_printf("[%s] slot released\n", ptcb->TCB_description);
        } else {
            gu_printf("[%s] slot request timed out\n", ptcb->TCB_description);
        }

        gu_TaskDelay(0, 0, POOL_IMPATIENT_RETRY_S, 0);
    }
}

/* ============ Scenario C: mutex + static priority-ceiling =============== */

void task_mutex_low(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    volatile unsigned long busy;
    (void) pdata;

    while (1) {
        gu_SemaphoreTake(g_mutex_shared, (G_UINT64) G_LATEST_TIME);
        gu_printf("[%s] mutex acquired at t=%u ms\n", ptcb->TCB_description,
                  (unsigned int) elapsed_ms());

        for (busy = 0; busy < MUTEX_LOW_CRIT_SECTION_ITERATIONS; busy++) {
            /* deliberately empty: CPU-bound "critical section" work. See
             * README.md Scenario C for why this must consume real CPU time
             * rather than sleep, for the priority-ceiling effect to be
             * observable at all. */
        }

        gu_SemaphoreGive(g_mutex_shared);
        gu_printf("[%s] mutex released at t=%u ms\n", ptcb->TCB_description,
                  (unsigned int) elapsed_ms());

        gu_TaskDelay(0, 0, MUTEX_LOW_CYCLE_DELAY_S, 0);
    }
}

void task_mutex_medium(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    unsigned int tick = 0;
    (void) pdata;

    /* Never touches g_mutex_shared. This task exists purely as the priority
     * threat GeMRTOS's static ceiling protocol must defeat once it
     * has ratcheted in -- see README.md Scenario C: watch for a gap in
     * these tick messages once task_mutex_low's priority has been boosted. */
    while (1) {
        gu_printf("[%s] tick %u at t=%u ms\n", ptcb->TCB_description, tick,
                  (unsigned int) elapsed_ms());
        tick++;
        gu_TaskDelay(0, 0, 0, MUTEX_MEDIUM_TICK_MS);
    }
}

void task_mutex_high(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    (void) pdata;

    while (1) {
        gu_printf("[%s] requesting mutex at t=%u ms\n", ptcb->TCB_description,
                  (unsigned int) elapsed_ms());
        gu_SemaphoreTake(g_mutex_shared, (G_UINT64) G_LATEST_TIME);
        gu_printf("[%s] mutex acquired at t=%u ms\n", ptcb->TCB_description,
                  (unsigned int) elapsed_ms());
        gu_SemaphoreGive(g_mutex_shared);

        gu_TaskDelay(0, 0, MUTEX_HIGH_CYCLE_DELAY_S, 0);
    }
}

/* ============ Scenario D: recursive mutex ================================ */

/* Simulates a nested critical section -- e.g. a helper called from inside
 * another helper that already holds the same lock. A plain
 * gu_SemaphoreCreateMutex() handle would deadlock a task against itself
 * here; gu_SemaphoreCreateRecursiveMutex() tracks re-entry (SEM_Recurrence)
 * and only actually releases the lock on the matching outermost Give(). */
static void recursive_inner(const char *task_description)
{
    gu_SemaphoreTake(g_recursive_mutex, (G_UINT64) G_LATEST_TIME);
    gu_printf("[%s] recursive: inner take (nested)\n", task_description);
    gu_SemaphoreGive(g_recursive_mutex);
}

void task_recursive_demo(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    (void) pdata;

    while (1) {
        gu_SemaphoreTake(g_recursive_mutex, (G_UINT64) G_LATEST_TIME);
        gu_printf("[%s] recursive: outer take\n", ptcb->TCB_description);

        recursive_inner(ptcb->TCB_description);

        gu_printf("[%s] recursive: outer give\n", ptcb->TCB_description);
        gu_SemaphoreGive(g_recursive_mutex);

        gu_TaskDelay(0, 0, RECURSIVE_CYCLE_DELAY_S, 0);
    }
}
