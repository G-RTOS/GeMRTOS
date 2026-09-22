/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS minimum viable application: a single task printing over the JTAG UART.
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

/* This is the smallest application that exercises the whole GeMRTOS
 * startup path: one task, created in main() and released with
 * gu_TaskStartWithOffset(), printing over the JTAG UART every second.
 * See README.md for what to expect on the terminal and what to try next. */

#include <gemrtos.h>
#include <system.h>

/* Delay between prints, expressed the same way gu_TaskDelay() takes it. */
#define HELLO_TASK_DELAY_MS 1000U

/******************************************************************/
/* hello_task: prints a greeting and a running counter forever.   */
/******************************************************************/
void hello_task(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    unsigned int count = 0;

    while (1) {
        gu_printf("[%s] Hello from GeMRTOS! count=%u\n", ptcb->TCB_description, count);
        count++;
        gu_TaskDelay(0, 0, 0, HELLO_TASK_DELAY_MS);
    }
}

int main(void)
{
    GS_TCB *ptcb;

    ptcb = gu_TaskCreate((void *) hello_task,   /* task entry point   */
                          (void *) 0,            /* no argument needed */
                          "hello_task");         /* task description   */

    if (ptcb != (GS_TCB *) 0) {
        /* hello_task is a self-looping OneShot task (see enum tcbtype in
         * gemrtos_core.h): it is released once and keeps itself running
         * with its own while(1) + gu_TaskDelay(), rather than being
         * re-released by the scheduler on a fixed period. */
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);

        /* A freshly created task is UNLINKED and will never run until
         * started explicitly. Zero offset: become ready immediately. */
        gu_TaskStartWithOffset(ptcb, 0, 0, 0, 0);
    } else {
        gu_printf("ERROR: could not create hello_task (TCB pool exhausted)\n");
        while (1);
    }

    return (0);
}
