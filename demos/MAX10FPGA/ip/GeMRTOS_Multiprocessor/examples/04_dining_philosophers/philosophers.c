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

/* Extracted from examples/10_full_demo/hellogemrtos.c as a
 * behaviour-preserving move -- see README.md
 * "Extraction notes" for exactly what changed versus what was preserved
 * byte-for-byte (the synchronization algorithm itself: fork acquisition
 * order and timeouts). One exception: the original's
 * G_SCBType_TCB_ABORTED registration is NOT preserved here -- see
 * README.md "Extraction notes" for why it was dead code. */

#include <gemrtos.h>
#include <system.h>
#include "philosophers.h"

#define left_fork(i)  (i)
#define right_fork(i) ((i + 1) % NUMBER_PHILOSOPHERS)

/* One binary semaphore per fork, shared between adjacent philosophers. */
static t_semaphore_resource *forks[NUMBER_PHILOSOPHERS];

/* Shared by the "low half" of philosophers, which update a LED value instead
 * of just printing (see task_philosopher() below) -- a mutex because more
 * than one of them may reach that branch around the same time. */
static t_semaphore_resource *mutex_leds;
static volatile unsigned int leds;

G_UINT32 philosophers_semaphores_create(void)
{
    int i;

    mutex_leds = gu_SemaphoreCreateRecursiveMutex();
    if (mutex_leds == (t_semaphore_resource *) 0) {
        return (G_FALSE);
    }

    for (i = 0; i < NUMBER_PHILOSOPHERS; i++) {
        forks[i] = gu_SemaphoreCreateBinary(1);
        if (forks[i] == (t_semaphore_resource *) 0) {
            return (G_FALSE);
        }
    }
    return (G_TRUE);
}

/******************************************************************/
/* generic task function                                          */
/******************************************************************/
void task_philosopher(void *pdata)
{
    unsigned int task_index = (unsigned int) pdata;
    t_semaphore_resource *first_sem, *second_sem;


    /* get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    /* get the semaphores in order */
    if ((task_index & 1) == 0) {                    // philosopher is even
        first_sem  = forks[left_fork(task_index)];
        second_sem = forks[right_fork(task_index)];
    } else {                                        // philosopher is odd
        second_sem = forks[right_fork(task_index)];
        first_sem  = forks[left_fork(task_index)];
    }
    G_UINT64 timeout_semaphore = gu_ConvertTime(0, 0, 0, TIMEOUT_FORK_MS);

    while (G_TRUE) {
        if (gu_SemaphoreTake(first_sem, timeout_semaphore) == G_TRUE) {
            if (gu_SemaphoreTake(second_sem, timeout_semaphore) == G_TRUE) {

                /* HALF of philosophers write in queue, the rest change LEDs.
                 *
                 * Extraction note: the original combined demo
                 * routed this message through the JTAG UART interactive
                 * server's message queue for the "high" half whenever
                 * INCLUDE_JTAG_UART_SERVER == 1 (the case on real hardware).
                 * That server is a separate subsystem, out of scope for this
                 * standalone example -- its own driver copy belongs to
                 * 06_triggers_and_isr, which keeps its own copy. This extraction therefore
                 * always takes the plain gu_printf() path, which is the
                 * exact path the original file itself already used whenever
                 * INCLUDE_JTAG_UART_SERVER == 0 (its own non-niosX branch) --
                 * not a new code path. Fork acquisition, timeouts, and the
                 * LED-mutex branch below are otherwise unchanged. */
                if (task_index > NUMBER_PHILOSOPHERS / 2) {
                    gu_printf("%s is eating in processor %d\n", ptcb->TCB_description, gm_ProcessorId());
                } else {
                    gu_printf("%s is eating in processor %d\n", ptcb->TCB_description, gm_ProcessorId());
                    /* Update LED under mutex (TIMEOUT_LED_MUTEX_MS ms timeout) */
                    G_UINT64 timeout = gu_ConvertTime(0, 0, 0, TIMEOUT_LED_MUTEX_MS); /* Define Mutex timeout (600ms) */
                    if (gu_SemaphoreTake(mutex_leds, timeout) == G_TRUE) {             /* Request LEDs Mutex           */
                        leds = leds + 4;
                        gm_WriteOutputs(leds);
                        gu_SemaphoreGive(mutex_leds);
                    } else {
                        gu_printf("UNSUCCESSFULL SEMAPHORE TAKEN %s\n", ptcb->TCB_description);
                    }
                }
                gu_SemaphoreGive(second_sem);
            }
            gu_SemaphoreGive(first_sem);
        } else {
            gu_printf("UNSUCCESSFULL SEMAPHORE TAKEN %s\n", ptcb->TCB_description);
        }
        gu_TaskDelay(0, 0, 1, 0);                                  /* Supend task 1 second         */
    }
}
