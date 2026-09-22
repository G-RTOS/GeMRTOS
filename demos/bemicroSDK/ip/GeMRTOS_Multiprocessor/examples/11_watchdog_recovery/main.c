/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS per-processor watchdog on processor 1: a deliberately hung
 *         task is caught and recovered automatically, while a task on
 *         processor 2 keeps running to prove the rest of the system survives.
 *  \version 1.0
 *  \date      2026-09-02
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

/* ---------------------------------------------------------------------------
 * Two tasks:
 *
 *   steady_worker  Pinned to processor 2.  Prints every second forever.  Its
 *                  uninterrupted output IS the "the system survived" signal,
 *                  on a processor this test never resets.
 *   faulty_cpu1    Pinned to processor 1.  Runs a few iterations, then arms
 *                  processor 1's watchdog, masks its interrupts and hangs on
 *                  purpose.
 *
 * What happens when the watchdog expires:
 *
 *   1. The controller raises the reset-request handshake to processor 1 and
 *      holds it until the processor acknowledges.  Only that processor is
 *      reset; the rest of the system is untouched.
 *   2. Processor 1 fetches its reset vector.  The avalon_monitor redirects
 *      that fetch to the processor 2..N boot stub, which -- unlike the normal
 *      reset path -- does NOT clear .bss, so the kernel control block g_kcb
 *      and this file's own variables survive.
 *   3. gk_START_KERNEL()'s processor-1 branch detects a recovery and calls
 *      gk_Watchdog_Recovery_Cleanup(): it kills the abandoned task, restores
 *      the kernel invariants, and calls the handler registered below.
 *
 * The handler re-reads a .bss canary set at cold boot.  Had processor 1
 * wrongly taken the normal reset path, crt0's .bss clear would have zeroed it;
 * seeing WD_CANARY_MAGIC after the recovery is direct proof the redirection
 * worked.  report->TaskDescription reading "faulty_cpu1" is the same proof for
 * g_kcb itself.
 *
 * Requires G_NUMBER_OF_PCB >= 2.
 * ------------------------------------------------------------------------- */

#include <gemrtos.h>
#include <system.h>
#include <avalon_monitor_regs.h>   /* gm_MonitorReset{Address,Offset}Set */

#if (G_NUMBER_OF_PCB < 2)
#error "11_watchdog_recovery requires G_NUMBER_OF_PCB >= 2 (see system.h: GEMRTOS_DRIVER_NPROCESSORS). This BSP is configured for a single processor."
#endif

#define WATCHDOG_DEMO_PRIORITY        20U

/* Value stored in the controller's 12-bit watchdog-timeout field.  The
 * interval is that value scaled by 2^G_WATCHDOG_TIMEOUT_SHIFT controller
 * clocks: 300 << 16 is ~0.4 s at 50 MHz -- long enough to be unambiguous,
 * far below the ~5.4 s the register allows.  gm_WatchogTimeoutSet() shifts its
 * argument right by the same amount, so the two cancel. */
#define WATCHDOG_DEMO_TIMEOUT_REG     300U

#define STEADY_PERIOD_S               1U
#define FAULTY_PERIOD_S               1U
/* A few normal iterations first, so the capture shows the task running
 * healthily before it hangs. */
#define FAULTY_HANG_AT_ITERATION      5U

#define WD_CANARY_MAGIC               0xC0FFEE01U

/* .bss: set at cold boot in main() (on processor 1) and re-read in the handler
 * (which also runs on processor 1, after the recovery).  volatile so the
 * compiler cannot assume they are unchanged across the hang and the reset. */
static volatile G_UINT32 g_wd_canary;
static volatile G_UINT32 g_wd_recoveries;

static enum g_watchdog_action watchdog_demo_handler(const struct g_watchdog_report *report)
{
    g_wd_recoveries++;
    gu_printf("[watchdog] recovered: cpu=%u was_idle=%u task=\"%s\" canary=0x%08x recoveries=%u\n",
              (unsigned int) report->CPUID,
              (unsigned int) report->was_idle,
              report->TaskDescription,
              (unsigned int) g_wd_canary,
              (unsigned int) g_wd_recoveries);
    return G_WATCHDOG_ACTION_RESUME;
}

/* Pinned to processor 2 -- never reset by this test. */
void steady_worker(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    unsigned int count = 0U;

    (void) pdata;

    while (1) {
        gu_printf("[%s] alive, count=%u, processor=%u\n",
                  ptcb->TCB_description, count, (unsigned int) gm_ProcessorId());
        count++;
        gu_TaskDelay(0, 0, STEADY_PERIOD_S, 0);
    }
}

/* Pinned to processor 1.  gm_PrcWatchdogEnable() arms the CALLING processor's
 * own watchdog, so this correctly arms only processor 1. */
void faulty_cpu1(void *pdata)
{
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    unsigned int iteration = 0U;

    (void) pdata;

    while (1) {
        iteration++;
        gu_printf("[%s] iteration %u on processor %u\n",
                  ptcb->TCB_description, iteration, (unsigned int) gm_ProcessorId());

        if (iteration == FAULTY_HANG_AT_ITERATION) {

            /* Arm the monitor's reset-vector redirection, then the watchdog,
             * then hang -- with nothing in between.
             *
             * Deliberately no gu_printf inside this window: printf acquires the
             * global GeMRTOS mutex, and the controller suspends a processor's
             * watchdog countdown while that processor owns the mutex, so a
             * print here could postpone the very timeout being demonstrated. */
            gm_MonitorResetAddressSet((unsigned int) ALT_CPU_RESET_ADDR);
            gm_MonitorResetOffsetSet((unsigned int) (GEMRTOS_DRIVER_BASE_GMEMORY - ALT_CPU_RESET_ADDR));

            gm_WatchogTimeoutSet(WATCHDOG_DEMO_TIMEOUT_REG << G_WATCHDOG_TIMEOUT_SHIFT);
            gm_PrcWatchdogEnable();

            /* Masking interrupts is what makes this a genuine hang rather than
             * a merely slow task: every kernel entry on this processor takes
             * the controller mutex, and that refreshes the watchdog.  A task
             * that still services interrupts keeps its own watchdog alive. */
            GRTOS_DISABLE_PRC_GENERAL_IRQ;

            while (1) {
                /* The fault: never halts, never yields, never touches the
                 * kernel -- so nothing refreshes the watchdog timeout. */
            }
        }

        gu_TaskDelay(0, 0, FAULTY_PERIOD_S, 0);
    }
}

int main(void)
{
    GS_TCB *ptcb;
    GS_LCB *plcb_cpu1;
    GS_LCB *plcb_cpu2;

    g_wd_canary     = WD_CANARY_MAGIC;
    g_wd_recoveries = 0U;

    gu_WatchdogHandlerSet(watchdog_demo_handler);

    plcb_cpu1 = gu_SchedulingListCreate(GS_LCBTypeFP);
    plcb_cpu2 = gu_SchedulingListCreate(GS_LCBTypeFP);
    if ((plcb_cpu1 == (GS_LCB *) 0) || (plcb_cpu2 == (GS_LCB *) 0)) {
        gu_printf("ERROR: could not create the per-processor scheduling lists (pool exhausted)\n");
        while (1);
    }
    if (gu_SchedulingListAssociateProcessor(plcb_cpu1, 1, WATCHDOG_DEMO_PRIORITY) != G_TRUE) {
        gu_printf("ERROR: could not associate the CPU1 scheduling list with processor 1\n");
        while (1);
    }
    if (gu_SchedulingListAssociateProcessor(plcb_cpu2, 2, WATCHDOG_DEMO_PRIORITY) != G_TRUE) {
        gu_printf("ERROR: could not associate the CPU2 scheduling list with processor 2\n");
        while (1);
    }

    /* ---- steady_worker: pinned to processor 2 ---------------------------- */
    ptcb = gu_TaskCreate((void *) steady_worker, (void *) 0, "steady_worker");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        if (gu_SchedulingListAssociateTask(ptcb, plcb_cpu2) != G_TRUE) {
            gu_printf("ERROR: could not associate steady_worker with its scheduling list\n");
            while (1);
        }
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) WATCHDOG_DEMO_PRIORITY);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) WATCHDOG_DEMO_PRIORITY);
        gu_TaskStartWithOffset(ptcb, 0, 0, 0, 0);
    } else {
        gu_printf("ERROR: could not create steady_worker\n");
        while (1);
    }

    /* ---- faulty_cpu1: pinned to processor 1 ------------------------------ */
    ptcb = gu_TaskCreate((void *) faulty_cpu1, (void *) 0, "faulty_cpu1");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        if (gu_SchedulingListAssociateTask(ptcb, plcb_cpu1) != G_TRUE) {
            gu_printf("ERROR: could not associate faulty_cpu1 with its scheduling list\n");
            while (1);
        }
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) WATCHDOG_DEMO_PRIORITY);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) WATCHDOG_DEMO_PRIORITY);
        gu_TaskStartWithOffset(ptcb, 0, 0, 1, 0);
    } else {
        gu_printf("ERROR: could not create faulty_cpu1\n");
        while (1);
    }

    return (0);
}
