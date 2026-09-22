/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Task functions and message-queue setup for the
 *         05_message_queue_pubsub example.
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
#include "pubsub_demo.h"

/* ---- Scenario A: one publisher, two subscribers (fan-out) -------------- */
#define SCENARIO_A_PUBLISH_PERIOD_S 2U

/* ---- Scenario B: a slow subscriber forces a guaranteed partial delivery -
 * gu_MessageQueuePrintf() always calls gu_MessageQueueSend() with timeout=0
 * internally (gemrtos_mq.c), which blocks until every subscriber delivers --
 * i.e. it can never demonstrate a bounded timeout. This scenario therefore
 * calls gu_MessageQueueSend() directly, with an explicit non-zero timeout,
 * to reach the recoverable-error path an application must handle. */
#define SCENARIO_B_PUBLISH_PERIOD_MS 1000U
#define SCENARIO_B_SEND_TIMEOUT_MS    300U
#define SCENARIO_B_SLOW_BUSY_MS      2000U
#define SCENARIO_B_SUBSCRIBER_COUNT     2
#define SCENARIO_B_MSG_BUF_LEN         48U

/* ---- Scenario C: a late subscriber only sees messages published after it
 * joins -- subscription is not retroactive. Also uses gu_MessageQueueSend()
 * directly (not Printf) so the delivered-subscriber count is visible; the
 * timeout here is generous (500 ms) purely as a defensive guard, since
 * neither subscriber in this scenario is ever busy. */
#define SCENARIO_C_HEARTBEAT_PERIOD_S 1U
#define SCENARIO_C_SEND_TIMEOUT_MS   500U
#define SCENARIO_C_MSG_BUF_LEN        48U

/* Shared queue handles, one per scenario. All three are created once by
 * pubsub_demo_queues_create(), called from main() before any task below
 * runs. */
static GS_RCB *g_queue_sensor = (GS_RCB *) 0;  /* Scenario A */
static GS_RCB *g_queue_status = (GS_RCB *) 0;  /* Scenario B */
static GS_RCB *g_queue_notify = (GS_RCB *) 0;  /* Scenario C */

G_UINT32 pubsub_demo_queues_create(void)
{
    g_queue_sensor = gu_MessageQueueCreate();
    g_queue_status = gu_MessageQueueCreate();
    g_queue_notify = gu_MessageQueueCreate();

    if ((g_queue_sensor == (GS_RCB *) 0) ||
        (g_queue_status == (GS_RCB *) 0) ||
        (g_queue_notify == (GS_RCB *) 0)) {
        return (G_FALSE);
    }
    return (G_TRUE);
}

/* ==== Scenario A: fan-out broadcast ===================================== */

void task_sensor_publisher(void *pdata)
{
    (void) pdata;
    unsigned int reading = 0U;

    while (G_TRUE) {
        reading++;
        /* gu_MessageQueuePrintf() blocks until BOTH subscribers below have
         * received this exact reading -- one send, two independent copies
         * delivered: the defining property of publish/subscribe, as
         * opposed to a single-consumer queue (compare 03_semaphores'
         * counting-semaphore pool, where one holder's Take excludes the
         * other). */
        gu_MessageQueuePrintf(g_queue_sensor, "reading %u", reading);
        gu_printf("[sensor_publisher] published reading %u\n", reading);
        gu_TaskDelay(0, 0, SCENARIO_A_PUBLISH_PERIOD_S, 0);
    }
}

void task_logger_subscriber(void *pdata)
{
    (void) pdata;
    static char recv_buf[PUBSUB_RECV_BUF_LEN] __attribute__((aligned(4)));
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    gu_MessageQueueSubscribe(ptcb, g_queue_sensor, (void *) recv_buf, PUBSUB_RECV_BUF_LEN);

    while (G_TRUE) {
        (void) gu_MessageQueueReceive(g_queue_sensor);
        gu_printf("[logger_subscriber] logged: %s\n", recv_buf);
    }
}

void task_display_subscriber(void *pdata)
{
    (void) pdata;
    static char recv_buf[PUBSUB_RECV_BUF_LEN] __attribute__((aligned(4)));
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    gu_MessageQueueSubscribe(ptcb, g_queue_sensor, (void *) recv_buf, PUBSUB_RECV_BUF_LEN);

    while (G_TRUE) {
        (void) gu_MessageQueueReceive(g_queue_sensor);
        gu_printf("[display_subscriber] display: %s\n", recv_buf);
    }
}

/* ==== Scenario B: slow subscriber -> guaranteed partial delivery ======== */

void task_bursty_publisher(void *pdata)
{
    (void) pdata;
    static char msg_buf[SCENARIO_B_MSG_BUF_LEN] __attribute__((aligned(4)));
    unsigned int seq = 0U;
    G_UINT64 send_timeout = gu_ConvertTime(0, 0, 0, SCENARIO_B_SEND_TIMEOUT_MS);

    while (G_TRUE) {
        seq++;
        int len = snprintf(msg_buf, SCENARIO_B_MSG_BUF_LEN, "status %u", seq);
        int delivered = gu_MessageQueueSend(g_queue_status, msg_buf,
                                             len + 1, send_timeout);

        /* delivered > 0: reached every currently-subscribed task before the
         * timeout. delivered <= 0: the timeout fired first -- 0 or the
         * negated partial count is how many of SCENARIO_B_SUBSCRIBER_COUNT
         * actually got it. This is the recoverable-error path an
         * application must check, not just the happy path:
         * the second send is guaranteed to land while task_slow_subscriber
         * is still busy from the first one (SCENARIO_B_SLOW_BUSY_MS >
         * SCENARIO_B_PUBLISH_PERIOD_MS), so it always reports a partial
         * delivery on every run; later sends are not similarly guaranteed
         * and will vary run to run, same as 03_semaphores' Scenario C. */
        if (delivered > 0) {
            gu_printf("[bursty_publisher] status %u delivered to all %d subscribers\n",
                      seq, delivered);
        } else {
            int reached = (delivered == 0) ? 0 : -delivered;
            gu_printf("[bursty_publisher] status %u PARTIAL delivery: %d of %d subscribers (slow_subscriber likely still busy)\n",
                      seq, reached, SCENARIO_B_SUBSCRIBER_COUNT);
        }

        gu_TaskDelay(0, 0, 0, SCENARIO_B_PUBLISH_PERIOD_MS);
    }
}

void task_fast_subscriber(void *pdata)
{
    (void) pdata;
    static char recv_buf[PUBSUB_RECV_BUF_LEN] __attribute__((aligned(4)));
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    gu_MessageQueueSubscribe(ptcb, g_queue_status, (void *) recv_buf, PUBSUB_RECV_BUF_LEN);

    while (G_TRUE) {
        /* Always loops straight back to Receive() -- never busy, so it is
         * reached by every send regardless of the timeout. */
        (void) gu_MessageQueueReceive(g_queue_status);
        gu_printf("[fast_subscriber] received: %s\n", recv_buf);
    }
}

void task_slow_subscriber(void *pdata)
{
    (void) pdata;
    static char recv_buf[PUBSUB_RECV_BUF_LEN] __attribute__((aligned(4)));
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    gu_MessageQueueSubscribe(ptcb, g_queue_status, (void *) recv_buf, PUBSUB_RECV_BUF_LEN);

    while (G_TRUE) {
        (void) gu_MessageQueueReceive(g_queue_status);
        gu_printf("[slow_subscriber] received: %s\n", recv_buf);
        /* Deliberately busy for longer than one publish period, so the next
         * send from task_bursty_publisher cannot reach it in time -- see
         * the timing note in task_bursty_publisher() above. */
        gu_TaskDelay(0, 0, 0, SCENARIO_B_SLOW_BUSY_MS);
    }
}

/* ==== Scenario C: late subscriber, no retroactive delivery ============== */

void task_heartbeat_publisher(void *pdata)
{
    (void) pdata;
    static char msg_buf[SCENARIO_C_MSG_BUF_LEN] __attribute__((aligned(4)));
    unsigned int seq = 0U;
    G_UINT64 send_timeout = gu_ConvertTime(0, 0, 0, SCENARIO_C_SEND_TIMEOUT_MS);

    while (G_TRUE) {
        seq++;
        int len = snprintf(msg_buf, SCENARIO_C_MSG_BUF_LEN, "heartbeat %u", seq);
        int delivered = gu_MessageQueueSend(g_queue_notify, msg_buf,
                                             len + 1, send_timeout);
        /* Watch this count change from 1 (only task_early_subscriber exists
         * yet) to 2 once task_late_subscriber joins -- gu_MessageQueueCreate()
         * fans a message out to whoever is subscribed AT SEND TIME; a task
         * that subscribes later does not retroactively receive anything
         * sent before it joined (see README "Extraction notes" for why). */
        gu_printf("[heartbeat_publisher] heartbeat %u delivered to %d subscriber(s)\n",
                  seq, delivered);
        gu_TaskDelay(0, 0, SCENARIO_C_HEARTBEAT_PERIOD_S, 0);
    }
}

void task_early_subscriber(void *pdata)
{
    (void) pdata;
    static char recv_buf[PUBSUB_RECV_BUF_LEN] __attribute__((aligned(4)));
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    gu_MessageQueueSubscribe(ptcb, g_queue_notify, (void *) recv_buf, PUBSUB_RECV_BUF_LEN);

    while (G_TRUE) {
        (void) gu_MessageQueueReceive(g_queue_notify);
        gu_printf("[early_subscriber] received: %s\n", recv_buf);
    }
}

void task_late_subscriber(void *pdata)
{
    (void) pdata;
    static char recv_buf[PUBSUB_RECV_BUF_LEN] __attribute__((aligned(4)));
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    G_UINT32 first_receive = G_TRUE;

    /* Subscribes only after task_heartbeat_publisher has already sent
     * several heartbeats (see main.c's start offset for this task) --
     * its first received message is never "heartbeat 1". */
    gu_MessageQueueSubscribe(ptcb, g_queue_notify, (void *) recv_buf, PUBSUB_RECV_BUF_LEN);

    while (G_TRUE) {
        (void) gu_MessageQueueReceive(g_queue_notify);
        if (first_receive == G_TRUE) {
            gu_printf("[late_subscriber] first message ever seen: %s (notice it is not heartbeat 1)\n", recv_buf);
            first_receive = G_FALSE;
        } else {
            gu_printf("[late_subscriber] received: %s\n", recv_buf);
        }
    }
}
