/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Dining-philosophers task and fork semaphores for the
 *         04_dining_philosophers example.
 *  \version 1.0
 *  \date      2026-08-03 (G_SCBType_TCB_ABORTED registration removed; see
 *             README.md "Extraction notes")
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

#ifndef PHILOSOPHERS_H_
#define PHILOSOPHERS_H_

#include <gemrtos.h>

/* Number of philosophers (and forks: one per philosopher, shared in a ring
 * with each neighbour). Unchanged from the original combined demo
 * (examples/10_full_demo/hellogemrtos.c). */
#define NUMBER_PHILOSOPHERS 30

/* Fork semaphore acquisition timeout, milliseconds. Unchanged from the
 * original. */
#define TIMEOUT_FORK_MS 100U

/* LED mutex timeout inside the eating block, milliseconds. Unchanged from
 * the original. */
#define TIMEOUT_LED_MUTEX_MS 10U

/** \brief Creates the per-fork binary semaphores and the shared LED mutex.
 *         Must be called once from main(), before any task_philosopher()
 *         instance is scheduled.
 *  \return G_TRUE if every semaphore was created; G_FALSE if the semaphore
 *          pool was exhausted. */
G_UINT32 philosophers_semaphores_create(void);

/** \brief The dining-philosopher task body. One instance is created per
 *         philosopher; pdata is that philosopher's index
 *         (0..NUMBER_PHILOSOPHERS-1), cast to void* the same way the
 *         original demo does. */
void task_philosopher(void *pdata);

#endif /* PHILOSOPHERS_H_ */
