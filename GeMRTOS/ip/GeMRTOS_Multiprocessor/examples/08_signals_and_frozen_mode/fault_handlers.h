/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Fault-handling task bodies and signal callbacks for the
 *         08_signals_and_frozen_mode example: a deliberate, deterministic
 *         task-deadline overrun (G_SCBType_TCB_ABORTED) and a deliberate,
 *         deterministic timed-event processing backlog
 *         (G_SCBType_FROZEN_MODE / G_SCBType_UNFROZEN_MODE).
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

#ifndef FAULT_HANDLERS_H_
#define FAULT_HANDLERS_H_

#include <gemrtos.h>

/* ---- Fault 1: deliberate deadline overrun (G_SCBType_TCB_ABORTED) ------
 *
 * G_SCBType_TCB_ABORTED can ONLY fire for a genuinely G_TCBType_Periodic
 * task (verified against the implementation: gk_TASK_RELEASE(), the sole
 * caller of the abort check in gemrtos_list.c, is itself only called from
 * gk_TimeCallback()'s G_ECBType_PERIODIC case, gemrtos_task.c:715-717 --
 * never for G_TCBType_OneShot, whose "next release" is entirely
 * self-scheduled via gu_TaskDelay() and can never race with itself). See
 * README.md for the full derivation. */

/* Period of the deliberately-overrunning periodic task. */
#define FAULT_GLITCH_PERIOD_MS       300U

/* Every this-many-th release intentionally overruns the period; every
 * other release finishes almost instantly. */
#define FAULT_GLITCH_OVERRUN_EVERY_N   4U

/* Busy-loop iterations for the overrunning release -- must exceed
 * FAULT_GLITCH_PERIOD_MS in wall-clock time. Calibrated empirically
 * against this board/toolchain, same methodology as
 * 03_semaphores/sync_demo.h's busy loop (not derived from a clock
 * frequency); see README.md "Calibration" for the measured result and
 * how to re-tune it.
 *
 * The embedded value above does not translate to
 * host CPU speed. Measured directly on the Windows-simulation dev machine
 * with this tier's own build flags (mingw gcc, "-g", no explicit
 * optimization level): 400,000 iterations take ~1 ms there -- roughly
 * 300x short of the 300 ms it needs to actually overrun
 * FAULT_GLITCH_PERIOD_MS, so gk_TASK_RELEASE() never finds the task still
 * RUNNING at the next release and G_SCBType_TCB_ABORTED never fires.
 * 175,000,000 measured ~447-482 ms across three runs on the same machine --
 * a comfortable ~1.5x margin over the 300 ms threshold ON HARDWARE, where
 * gk_TASK_RELEASE()'s own check is serviced essentially instantly relative
 * to its deadline.
 *
 * On the Windows tier specifically, that 1.5x margin is
 * not enough -- confirmed by direct instrumentation, not assumed. Under
 * FAULT_BURST_TASK_COUNT's genuine 40-task load, gk_TASK_RELEASE()'s own
 * check is itself delayed behind the one-event-per-pass queue (measured
 * service lag: tens of ms typically, up to several hundred ms observed) --
 * confirmed via GRTOS_Halt_Processor_Wait()'s own R_MTX_PRC_GRN to be
 * genuine, legitimate contention from the other 39 burst tasks (cpu 2
 * actively dispatching/completing burst_member instances throughout the
 * delay window), not a fixable kernel defect -- so the busy loop, calibrated
 * only against its own period, reliably finishes and the task returns to
 * WAITING_COMPLETED before the (delayed) check ever sees it. This is a
 * verification-tier timing gap, not a portable kernel defect: the embedded
 * value above is intentionally left untouched (that platform has no such
 * delay to compensate for). Re-tuned to comfortably exceed
 * FAULT_GLITCH_PERIOD_MS + this tier's own measured service lag, not just
 * FAULT_GLITCH_PERIOD_MS alone.
 *
 * First re-tune attempt (250,000,000) measured ~390-500 ms busy duration --
 * closed most of the gap (margin over the delayed check went from
 * "check sees WAITING_COMPLETED" to "check still ~11-34 ms too early") but
 * not quite enough, and real wall-clock busy-loop duration on this tier
 * varies run to run (confirmed: same iteration count measured anywhere in
 * that 390-500 ms range across releases in one run), so the margin must be
 * generous, not just barely sufficient for one observed run. */
#ifndef __niosX_arch__
    #define FAULT_GLITCH_OVERRUN_BUSY_ITERATIONS 350000000UL
#else
    #define FAULT_GLITCH_OVERRUN_BUSY_ITERATIONS 400000UL
#endif

#define FAULT_GLITCH_START_OFFSET_S     2U

/** \brief G_TCBType_Periodic task body. Every
 *         FAULT_GLITCH_OVERRUN_EVERY_N-th release runs a busy loop longer
 *         than its own period, so the kernel re-releases it before this
 *         instance returns -- a genuine, deterministic missed deadline,
 *         not a probabilistic contention artifact. Must be created with
 *         gu_TaskTypeSet(ptcb, G_TCBType_Periodic) and
 *         gu_TaskPeriodSet(ptcb, 0, 0, 0, FAULT_GLITCH_PERIOD_MS); does
 *         its work and returns (never loops, never delays itself -- the
 *         kernel re-releases it, exactly like
 *         02_tasks_and_delays/task_periodic_beacon).
 *  \param [in] pdata  Unused. */
void fault_glitchy_worker(void *pdata);

/** \brief G_SCBType_TCB_ABORTED signal callback for fault_glitchy_worker.
 *         Runs once, automatically, before the next release resumes,
 *         whenever an overrun actually happened -- the recovery itself
 *         (stack reinitialised, task re-linked into the ready list) is
 *         entirely the kernel's doing; this callback only reports it.
 *  \param [in] pdata  Unused (passed as the signal_arg at registration,
 *                      NULL here -- nothing task-specific to report beyond
 *                      what gu_TaskGetCurrentTCB() already gives). */
void fault_signal_task_aborted(void *pdata);

/* ---- Fault 2: deliberate timed-event backlog (G_SCBType_FROZEN_MODE) --
 *
 * Frozen mode is a hardware watchdog on timed-event SERVICE LATENCY, not
 * on CPU/interrupt liveness (confirmed directly, not inferred: the kernel
 * services one due timed event per critical-section pass -- see the
 * comment at gemrtos_kernel.c:170, "Processor handles just one event and
 * continue to avoid starving other processors from mutex" -- and compares
 * current time against the still-unattended event's own due time; if that
 * gap exceeds the threshold while frozen mode is enabled, the FROZEN event
 * fires). The default threshold, set once at boot
 * (gemrtos_kernel.c:563-565), is gu_ConvertTime(0, 0, 0, 500) -- 500 ms --
 * chosen generously enough that no existing example has ever tripped it.
 * This example lowers it deliberately and creates a genuine backlog: many
 * tasks releasing on the exact same tick, serviced one at a time, so the
 * later ones in the batch are serviced measurably after their own due
 * time. No critical section or interrupt-disable primitive is touched --
 * the fault is entirely a consequence of ordinary, documented task
 * scheduling under deliberately concentrated load. */

/* How many tasks release simultaneously each burst -- well under
 * G_NUMBER_OF_TCB (64) and G_NUMBER_OF_ECB (80), gemrtos_config.h, leaving
 * ample headroom for IDLE tasks and fault_glitchy_worker. */
#define FAULT_BURST_TASK_COUNT        40U

/* Lowered threshold, milliseconds -- 25x more aggressive than the 500 ms
 * kernel default. Empirically tuned (see README.md "Calibration") so that
 * ordinary single-task jitter never trips it, but FAULT_BURST_TASK_COUNT
 * tasks serviced back-to-back reliably does. */
#define FAULT_FROZEN_THRESHOLD_MS      20U

/* All burst tasks share this exact period and start offset, so every
 * instance releases on the same tick, cycle after cycle. */
#define FAULT_BURST_PERIOD_S            4U
#define FAULT_BURST_START_OFFSET_S      3U

/** \brief One instance of the burst: prints a short line and returns
 *         (G_TCBType_Periodic, like fault_glitchy_worker) -- the fault
 *         comes from FAULT_BURST_TASK_COUNT instances of this same body
 *         all sharing one period and one start offset, not from anything
 *         this function itself does.
 *  \param [in] pdata  This instance's 0-based index, cast from a small
 *                      integer; used only to distinguish output. */
void fault_burst_member(void *pdata);

/** \brief G_SCBType_FROZEN_MODE signal callback, registered once at KCB
 *         level (pxcb = NULL -- this signal type is not task-specific).
 *         The kernel's own gk_FrozenCallback() (gemrtos_task.c) already
 *         prints "FROZEN Proc: <n>" unconditionally, before this callback
 *         runs; this one adds the example-specific line demonstrating
 *         gu_SignalCreate(G_SCBType_FROZEN_MODE, ...) itself.
 *  \param [in] pdata  Unused. */
void fault_signal_frozen(void *pdata);

/** \brief G_SCBType_UNFROZEN_MODE signal callback, registered once at KCB
 *         level (pxcb = NULL). Fires once the timed-event backlog has
 *         drained back within threshold -- the recovery signal. The
 *         kernel's own gk_UnFrozenCallback() already prints
 *         "UNFROZEN Proc: <n>" first, same relationship as
 *         fault_signal_frozen() above.
 *  \param [in] pdata  Unused. */
void fault_signal_unfrozen(void *pdata);

#endif /* FAULT_HANDLERS_H_ */
