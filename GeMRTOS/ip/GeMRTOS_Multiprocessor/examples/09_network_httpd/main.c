/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS network showcase: LwIP + HTTP server with SSI status
 *         variables, CGI-editable runtime parameters, and client-side AJAX
 *         live updates.
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

/* Extracted from examples/10_full_demo/hellogemrtos.c as a
 * behaviour-preserving move for the network/web-glue
 * portion only -- see README.md "Extraction notes" for exactly what moved
 * where, and what was deliberately left out (dining philosophers, the JTAG
 * UART interactive server, and the unconditional G_SCBType_FROZEN_MODE
 * registration -- each already has its own dedicated example: 04, 06, 08).
 *
 * task_periodic and task_infinite_loop are simple LED-blinking tasks with
 * no network purpose of their own -- they exist so the web status page's
 * two CGI-editable rows (period_ms, loop_delay_ms) have real, live state to
 * display and change, exactly as in the original combined demo.
 *
 * Task functions and semaphore creation for the network layer live in
 * net_init.c/.h + tse_servers.c/.h + web_sources.c/.h -- the fourth
 * multi-file example in this catalogue. No
 * fs/ directory of its own: the web pages are served from the shared
 * GeMRTOS_Monitor tree (misc/gemrtos_ips/GeMRTOS_Monitor/HAL/apps/http/fs),
 * which gemrtos_build.bat's makefsdata.exe step already regenerates on
 * every build regardless of which example is selected -- duplicating that
 * chain here would only risk the makefsdata/SSI-extension fragility fixed
 * previously, for no benefit.
 */

#include <gemrtos.h>
#include <system.h>
#include "net_init.h"
#include "tse_servers.h"
#include "web_sources.h"

#define TIMEOUT_LED_PERIODIC_MS         600U  /* LED mutex timeout in both tasks below */
#define DELAY_INFINITE_LOOP_MS_DEFAULT  500U  /* task_infinite_loop() initial delay    */
#define PERIODIC_TASK_PERIOD_MS_DEFAULT 500U  /* task_periodic() initial period        */

/* Shared by task_periodic/task_infinite_loop below; philosophers.c-style
 * dedicated mutex (04_dining_philosophers is not part of this example). */
static t_semaphore_resource *mutex_leds;
static volatile unsigned int leds;

/******************************************************************/
/* periodic task: blinks LED 0; period is CGI-editable (period_ms) */
/******************************************************************/
static void task_periodic(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    G_UINT64 timeout;

    (void) pdata;

    gu_printf("task %s running\n", ptcb->TCB_description);

    timeout = gu_ConvertTime(0, 0, 0, TIMEOUT_LED_PERIODIC_MS);
    if (gu_SemaphoreTake(mutex_leds, timeout) == G_TRUE) {
        leds = leds ^ (1 << 0);
        gm_WriteOutputs(leds);
        gu_SemaphoreGive(mutex_leds);
    } else {
        gu_printf("Semaphore not taken in task %s\n", ptcb->TCB_description);
    }
}

/******************************************************************/
/* infinite-loop task: blinks LED 1; delay is CGI-editable         */
/* (loop_delay_ms)                                                 */
/******************************************************************/
static void task_infinite_loop(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    (void) pdata;

    while (G_TRUE) {
        G_UINT64 timeout;

        gu_printf("task %s running\n", ptcb->TCB_description);

        timeout = gu_ConvertTime(0, 0, 0, TIMEOUT_LED_PERIODIC_MS);
        if (gu_SemaphoreTake(mutex_leds, timeout) == G_TRUE) {
            leds = leds ^ (1 << 1);
            gm_WriteOutputs(leds);
            gu_SemaphoreGive(mutex_leds);
        } else {
            gu_printf("Semaphore not taken in task %s\n", ptcb->TCB_description);
        }

        /* Suspend task until next release; g_infinite_loop_delay_ms is re-read
         * every iteration, so a web-triggered update (see
         * gemrtos_web_cgi_set_handler(), web_sources.c) takes effect from the
         * very next delay. */
        gu_TaskDelay(0, 0, 0, g_infinite_loop_delay_ms);
    }
}

int main(void)
{
    GS_TCB *ptcb;

    g_infinite_loop_delay_ms = DELAY_INFINITE_LOOP_MS_DEFAULT;

    mutex_leds = gu_SemaphoreCreateRecursiveMutex();
    if (mutex_leds == (t_semaphore_resource *) 0) {
        gu_printf("ERROR: could not create mutex_leds (pool exhausted)\n");
        while (1);
    }

    /* ---- CGI-editable demo tasks --------------------------------------- */
    ptcb = gu_TaskCreate((void *) task_periodic, (void *) 0, "periodic task");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_Periodic);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 1);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 1);
        gu_TaskPeriodSet(ptcb, 0, 0, 0, PERIODIC_TASK_PERIOD_MS_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);
        /* Retained so the web status page can retune the period at runtime
         * via gu_TaskPeriodSet() -- see gemrtos_web_cgi_set_handler()
         * (web_sources.c). */
        g_periodic_task_tcb = ptcb;
    } else {
        gu_printf("ERROR: could not create periodic task\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) task_infinite_loop, (void *) 0, "infinite-loop task");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 1);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 1);
        gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);
    } else {
        gu_printf("ERROR: could not create infinite-loop task\n");
        while (1);
    }

    /* ---- Network: topology wiring, then RX/TX servers ------------------ */
    if (net_init_topology() != G_TRUE) {
        gu_printf("ERROR: could not initialize network topology (pool exhausted)\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) tse_receive_server, (void *) &gemrtos_tse_mac_device[0], "tse_receive_server");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 0);
        gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);
    } else {
        gu_printf("ERROR: could not create tse_receive_server task\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) tse_transmit_server, (void *) &gemrtos_tse_mac_device[0], "tse_transmit_server");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 0);
        gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);
    } else {
        gu_printf("ERROR: could not create tse_transmit_server task\n");
        while (1);
    }

    return (0);
}
