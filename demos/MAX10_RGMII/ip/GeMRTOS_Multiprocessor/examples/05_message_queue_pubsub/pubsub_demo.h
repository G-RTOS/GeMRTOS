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

#ifndef PUBSUB_DEMO_H_
#define PUBSUB_DEMO_H_

#include <gemrtos.h>

/* Ready/run priority in GeMRTOS: LARGER numeric value = LOWER priority (see
 * gu_TaskReadyPrioritySet()/gu_TaskRunPrioritySet() in gemrtos_core.h). Every
 * task in this example runs at the same priority -- ordering between
 * scenarios is deliberately not a variable here, unlike 03_semaphores'
 * Scenario C. */
#define PUBSUB_PRIORITY_DEFAULT 15U

/* Receive buffers are fixed-size, one per subscribed task, sized generously
 * for this example's short printf-formatted messages (see
 * gu_MessageQueueSubscribe()'s buffer_length contract: longer messages are
 * truncated, not rejected). */
#define PUBSUB_RECV_BUF_LEN 80U

/* Start offsets, staggered so every scenario's precondition already holds
 * before its first observable event (documented per-task in main.c). */
#define PUBSUB_START_OFFSET_A_SUBSCRIBERS_S 0U
#define PUBSUB_START_OFFSET_A_PUBLISHER_S   1U
#define PUBSUB_START_OFFSET_B_SUBSCRIBERS_S 0U
#define PUBSUB_START_OFFSET_B_PUBLISHER_MS  200U
#define PUBSUB_START_OFFSET_C_EARLY_S        0U
#define PUBSUB_START_OFFSET_C_PUBLISHER_S    0U
#define PUBSUB_START_OFFSET_C_LATE_S          3U

/** \brief Creates every message queue used by this example (one per
 *         scenario). Must be called once from main(), before any of the
 *         task functions below are scheduled.
 *  \return G_TRUE if every queue was created; G_FALSE if the queue pool was
 *          exhausted (same fail-fast convention main.c uses for
 *          gu_TaskCreate() failures). */
G_UINT32 pubsub_demo_queues_create(void);

/* ---- Scenario A: one publisher, two subscribers (fan-out broadcast) ---- */
void task_sensor_publisher(void *pdata);
void task_logger_subscriber(void *pdata);
void task_display_subscriber(void *pdata);

/* ---- Scenario B: a slow subscriber forces a guaranteed partial delivery
 *      (the recoverable-error / timeout path) ------------------------- */
void task_bursty_publisher(void *pdata);
void task_fast_subscriber(void *pdata);
void task_slow_subscriber(void *pdata);

/* ---- Scenario C: a late subscriber only sees messages published after it
 *      joins -- subscription is not retroactive ------------------------- */
void task_heartbeat_publisher(void *pdata);
void task_early_subscriber(void *pdata);
void task_late_subscriber(void *pdata);

#endif /* PUBSUB_DEMO_H_ */
