/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS message queues as a publish/subscribe mechanism: one
 *         publisher fanning out to many subscribers, the guaranteed
 *         partial-delivery/timeout path, and late-subscriber semantics.
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

/* Three independent scenarios, each isolated via its own message queue and
 * task set -- see README.md for the full picture:
 *
 *   Scenario A (sensor_publisher,      One send reaches every subscriber:
 *               logger_subscriber,     the defining property of pub/sub,
 *               display_subscriber)    as opposed to a single-consumer
 *                                      queue.
 *   Scenario B (bursty_publisher,      A slow subscriber makes a send
 *               fast_subscriber,       guaranteed to only partially deliver
 *               slow_subscriber)       within its timeout -- the
 *                                      recoverable-error path, same
 *                                      discipline as
 *                                      03_semaphores' Scenario B.
 *   Scenario C (heartbeat_publisher,   Subscribing is not retroactive: a
 *               early_subscriber,      late subscriber only receives
 *               late_subscriber)       messages sent after it joins.
 *
 * Task functions and message-queue creation live in pubsub_demo.c/.h, same
 * multi-file split as 03_semaphores.
 */

#include <gemrtos.h>
#include <system.h>
#include "pubsub_demo.h"

int main(void)
{
    GS_TCB *ptcb;

    if (pubsub_demo_queues_create() != G_TRUE) {
        gu_printf("ERROR: could not create one or more message queues (pool exhausted)\n");
        while (1);
    }

    /* ---- Scenario A: fan-out broadcast --------------------------------- */
    ptcb = gu_TaskCreate((void *) task_logger_subscriber, (void *) 0, "logger_subscriber");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, PUBSUB_START_OFFSET_A_SUBSCRIBERS_S, 0);
    } else {
        gu_printf("ERROR: could not create logger_subscriber task\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) task_display_subscriber, (void *) 0, "display_subscriber");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, PUBSUB_START_OFFSET_A_SUBSCRIBERS_S, 0);
    } else {
        gu_printf("ERROR: could not create display_subscriber task\n");
        while (1);
    }

    /* Publisher starts a full second after both subscribers, so both are
     * already subscribed and blocked in gu_MessageQueueReceive() before the
     * first broadcast -- otherwise the very first reading could race a
     * subscriber that has not subscribed yet. */
    ptcb = gu_TaskCreate((void *) task_sensor_publisher, (void *) 0, "sensor_publisher");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, PUBSUB_START_OFFSET_A_PUBLISHER_S, 0);
    } else {
        gu_printf("ERROR: could not create sensor_publisher task\n");
        while (1);
    }

    /* ---- Scenario B: guaranteed partial delivery ----------------------- */
    ptcb = gu_TaskCreate((void *) task_fast_subscriber, (void *) 0, "fast_subscriber");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, PUBSUB_START_OFFSET_B_SUBSCRIBERS_S, 0);
    } else {
        gu_printf("ERROR: could not create fast_subscriber task\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) task_slow_subscriber, (void *) 0, "slow_subscriber");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, PUBSUB_START_OFFSET_B_SUBSCRIBERS_S, 0);
    } else {
        gu_printf("ERROR: could not create slow_subscriber task\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) task_bursty_publisher, (void *) 0, "bursty_publisher");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, 0, PUBSUB_START_OFFSET_B_PUBLISHER_MS);
    } else {
        gu_printf("ERROR: could not create bursty_publisher task\n");
        while (1);
    }

    /* ---- Scenario C: late subscriber, no retroactive delivery ---------- */
    ptcb = gu_TaskCreate((void *) task_early_subscriber, (void *) 0, "early_subscriber");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, PUBSUB_START_OFFSET_C_EARLY_S, 0);
    } else {
        gu_printf("ERROR: could not create early_subscriber task\n");
        while (1);
    }

    ptcb = gu_TaskCreate((void *) task_heartbeat_publisher, (void *) 0, "heartbeat_publisher");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, PUBSUB_START_OFFSET_C_PUBLISHER_S, 0);
    } else {
        gu_printf("ERROR: could not create heartbeat_publisher task\n");
        while (1);
    }

    /* Joins at t=3.5s. task_heartbeat_publisher sends heartbeat 1 immediately
     * on task start (t=0s) -- its gu_TaskDelay() call runs *after* each send,
     * not before the first one -- so heartbeat N goes out at t=(N-1)s. This
     * join therefore lands strictly between the 4th heartbeat (t=3s) and the
     * 5th (t=4s), so its first-ever received message is deterministically
     * "heartbeat 5", never 1-4, on every run. Confirmed on hardware. */
    ptcb = gu_TaskCreate((void *) task_late_subscriber, (void *) 0, "late_subscriber");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) PUBSUB_PRIORITY_DEFAULT);
        gu_TaskStartWithOffset(ptcb, 0, 0, PUBSUB_START_OFFSET_C_LATE_S, 500);
    } else {
        gu_printf("ERROR: could not create late_subscriber task\n");
        while (1);
    }

    return (0);
}
