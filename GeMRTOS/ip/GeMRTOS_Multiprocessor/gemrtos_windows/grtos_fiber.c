/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Windows Fiber implementation of the POSIX ucontext API
 *  \details Provides makecontext/swapcontext/setcontext semantics on Windows so the GeMRTOS kernel context switch runs unchanged on the host.
 *  \version 1.0
 *  \date      2013-2026
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

/*
 *  grtos_fiber.c â€” Windows Fiber implementation of POSIX ucontext operations
 *
 *  Replaces the Hadjidoukas GetThreadContext/SetThreadContext implementation
 *  (ucontext.c) with the Windows Fiber API, which is the correct Windows
 *  mechanism for cooperative user-mode context switching.
 *
 *  Key design decisions:
 *
 *  1. LAZY FIBER CREATION â€” fibers are created inside swapcontext (not in
 *     makecontext) so that each fiber is always created in the processor
 *     thread that first switches to it.  This is required because Windows
 *     fibers are thread-local: SwitchToFiber must be called from the thread
 *     that owns the fiber.
 *
 *  2. LeaveCriticalSection AFTER SwitchToFiber â€” swapcontext releases
 *     GeMRTOS_MUTEX_swapcontext immediately after SwitchToFiber returns
 *     (i.e., when the saved context is resumed).  This mirrors the intent
 *     of the original ucontext.c:102 LeaveCriticalSection, which was never
 *     reached because SetThreadContext jumped away without returning.
 *     With Fibers, SwitchToFiber genuinely returns on resume.
 *
 *  3. FIBER STACK SIZE â€” uc_stack.ss_size (set by the caller before
 *     makecontext) is forwarded to CreateFiberEx.  A 64 KB minimum is
 *     enforced because Windows rejects smaller values.
 */

#include <gemrtos.h>    /* pulls in grtos_fiber.h via gemrtos_controller_windows.h  */
#include <stdarg.h>
#include <stdio.h>

#define GRTOS_FIBER_MIN_STACK  (64U * 1024U)

/* -------------------------------------------------------------------------
 * Deferred-publish mechanism for swapcontext's saved (oucp) context.
 *
 * FIXED: swapcontext used to write oucp->fiber_handle = cur
 * BEFORE calling SwitchToFiber(target) -- publishing "cur is now safely
 * resumable" while this thread was still physically running on cur (the
 * actual vacate only happens at the SwitchToFiber call a few instructions
 * later). On real multi-core hardware another thread can read that publish
 * and call SwitchToFiber(cur) -- an OS-level fiber-affinity violation,
 * since cur is still active on the original thread -- in that gap. This
 * reproduced as an intermittent access violation once diagnostic prints
 * elsewhere stopped masking the timing.
 *
 * Fix: defer the oucp write until the first instruction that provably runs
 * AFTER this thread has left cur -- which is exactly the next thing this
 * thread executes once SwitchToFiber(target) hands control to target: either
 * grtos_fiber_entry (brand-new target fiber) or the post-SwitchToFiber resume
 * point inside some earlier swapcontext call (existing target fiber, or this
 * exact context being resumed later by any thread). Both call
 * flush_pending_publish() as their first action. The pending state is
 * thread-local: it always describes "the fiber THIS OS thread most recently,
 * unconditionally, left," which is correct regardless of which fiber this
 * thread ends up running next, or which thread later resumes the fiber that
 * was just published. */
static __thread ucontext_t *g_pending_oucp = NULL;
static __thread LPVOID      g_pending_cur  = NULL;
static __thread DWORD       g_pending_tid  = 0;

static void flush_pending_publish(void)
{
    if (g_pending_oucp != NULL) {
        g_pending_oucp->fiber_handle    = g_pending_cur;
        g_pending_oucp->fiber_owner_tid = g_pending_tid;
        g_pending_oucp = NULL;
    }
}

/* -------------------------------------------------------------------------
 * Fiber trampoline â€” entry point for every fiber created by makecontext.
 * param points to the grtos_fiber_info_t embedded in the owning ucontext_t.
 * ----------------------------------------------------------------------- */
static void CALLBACK grtos_fiber_entry(LPVOID param)
{
    flush_pending_publish();
    grtos_fiber_info_t *info = (grtos_fiber_info_t *)param;
    if (info->argc == 0) {
        info->func_noarg();
    } else {
        info->func_onearg(info->arg);
    }
    /* Should never return; loop defensively. */
    for (;;) { Sleep(1000); }
}

/* -------------------------------------------------------------------------
 * ensure_fiber â€” convert the current thread to a fiber if not already one.
 * Returns the current fiber handle (never NULL on success).
 *
 * Exported (see grtos_fiber.h) so gemrtos_controller_windows.c's Swap_kernel_to_task
 * can use it instead of a raw GetCurrentFiber() call, which returns Windows'
 * documented non-NULL sentinel (not NULL) when called on a thread that has
 * never been converted to a fiber.
 * ----------------------------------------------------------------------- */
LPVOID ensure_fiber(void)
{
    if (!IsThreadAFiber()) {
        return ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH);
    }
    return GetCurrentFiber();
}

/* -------------------------------------------------------------------------
 * getcontext â€” ensure the calling thread is fiber-capable.
 *
 * Every call site in this codebase calls getcontext() immediately before
 * makecontext() on the same ucontext_t, which fully reinitialises
 * fiber_handle/fiber_info regardless of what getcontext left there.
 * Previously getcontext recorded ensure_fiber() into ucp->fiber_handle; this
 * unconditionally overwrote (and thus silently leaked/discarded) any real,
 * still-live fiber handle already stored in ucp from a prior
 * setcontext/swapcontext, and it made makecontext's own stale-fiber
 * DeleteFiber check always see fiber_handle == GetCurrentFiber() and skip it.
 * getcontext now only guarantees fiber-capability; it does not touch
 * ucp->fiber_handle/fiber_owner_tid, so makecontext's cleanup logic sees the
 * ucontext_t's actual prior state.
 * ----------------------------------------------------------------------- */
int getcontext(ucontext_t *ucp)
{
    if (ucp == NULL) return -1;
    ensure_fiber();
    return 0;
}

/* -------------------------------------------------------------------------
 * setcontext â€” switch unconditionally to the fiber described by *ucp.
 *
 * Creates the fiber lazily if fiber_handle is NULL (same logic as
 * swapcontext, minus the "save current" step).
 * Does not return on success.
 * ----------------------------------------------------------------------- */
int setcontext(const ucontext_t *ucp)
{
    if (ucp == NULL) return -1;
    ensure_fiber();

    ucontext_t *u = (ucontext_t *)(uintptr_t)ucp;   /* cast away const for lazy init */
    if (u->fiber_handle == NULL) {
        size_t ss = (u->uc_stack.ss_size >= GRTOS_FIBER_MIN_STACK)
                    ? u->uc_stack.ss_size : GRTOS_FIBER_MIN_STACK;
        u->fiber_handle = CreateFiberEx(ss, ss, FIBER_FLAG_FLOAT_SWITCH,
                                        grtos_fiber_entry, &u->fiber_info);
        u->fiber_owner_tid = GetCurrentThreadId();
        if (u->fiber_handle == NULL) return -1;
    } else if (u->fiber_owner_tid != GetCurrentThreadId()) {
        u->fiber_owner_tid = GetCurrentThreadId();
    }
    SwitchToFiber(u->fiber_handle);
    /* Reached only if SwitchToFiber loops back here â€” should not happen. */
    return 0;
}

/* -------------------------------------------------------------------------
 * makecontext â€” prepare *ucp to run func(arg...) when first switched to.
 *
 * Fiber creation is deferred to swapcontext so that CreateFiber is called
 * from the processor thread that will own the fiber.  If a fiber already
 * exists for this context (e.g., from a previous makecontext call) it is
 * deleted here, but only if it is not the currently running fiber.
 *
 * argc must be 0 or 1.  A single void* argument is supported (argc == 1).
 * ----------------------------------------------------------------------- */
int makecontext(ucontext_t *ucp, void (*func)(), int argc, ...)
{
    if (ucp == NULL || (argc != 0 && argc != 1)) return -1;

    /* Delete any previously created fiber for this context. FIXED
     * the fiber -- calling it cross-thread is undefined behavior. GeMRTOS
     * tasks can be released/reinitialised by a processor other than the one
     * that last ran them (e.g. gm_ProcessorInterrupt-driven forced release),
     * so the stale fiber here may belong to a different OS thread than the
     * one calling makecontext now. In that case the stale fiber is orphaned
     * (handle/stack leaked) rather than deleted; CreateFiberEx below (called
     * lazily by swapcontext, from the correct owning thread) allocates a
     * fresh one. Confirmed via [STKINIT] trace as the cause of an
     * intermittent cross-thread DeleteFiber crash. */
    if (ucp->fiber_handle != NULL) {
        LPVOID cur = GetCurrentFiber();
        if ((ucp->fiber_handle != cur) && (ucp->fiber_owner_tid == GetCurrentThreadId())) {
            DeleteFiber(ucp->fiber_handle);
        }
        ucp->fiber_handle    = NULL;
        ucp->fiber_owner_tid = 0;
    }

    /* Store launch parameters; fiber is created lazily in swapcontext. */
    ucp->fiber_info.argc = argc;
    if (argc == 0) {
        ucp->fiber_info.func_noarg  = (void (*)(void))func;
        ucp->fiber_info.func_onearg = NULL;
        ucp->fiber_info.arg         = NULL;
    } else {
        va_list ap;
        va_start(ap, argc);
        void *arg = va_arg(ap, void *);
        va_end(ap);
        ucp->fiber_info.func_onearg = (void (*)(void *))func;
        ucp->fiber_info.func_noarg  = NULL;
        ucp->fiber_info.arg         = arg;
    }

    return 0;
}

/* -------------------------------------------------------------------------
 * swapcontext â€” save current fiber to *oucp, then switch to *ucp.
 *
 * Creates the target fiber lazily if fiber_handle is NULL, ensuring
 * CreateFiber is called from the correct processor thread.
 *
 * After SwitchToFiber returns (i.e., when this context is resumed by a
 * future SwitchToFiber call targeting oucp->fiber_handle), releases
 * GeMRTOS_MUTEX_swapcontext â€” matching the EnterCriticalSection performed
 * by GRTOS_Start_Task / GRTOS_Halt_Processor / GRTOS_Suspend_Task before
 * calling swapcontext.
 * ----------------------------------------------------------------------- */
int swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
{
    if (oucp == NULL || ucp == NULL) return -1;

    /* Defer publishing oucp until this thread has provably left `cur` -- see
     * flush_pending_publish() above. Do NOT write oucp->fiber_handle here. */
    LPVOID cur = ensure_fiber();
    g_pending_oucp = oucp;
    g_pending_cur  = cur;
    g_pending_tid  = GetCurrentThreadId();

    /* Lazy fiber creation for the target â€” first switch creates the fiber in
     * the calling processor thread.
     *
     * Cross-processor task migration: if a suspended task is rescheduled on a
     * different processor thread, SwitchToFiber is called cross-thread. The
     * target (ucp) is safe to take over here because by the time it became
     * resumable, the thread that suspended it had already flushed its own
     * pending publish -- i.e. had provably left it. */
    ucontext_t *u = (ucontext_t *)(uintptr_t)ucp;
    if (u->fiber_handle == NULL) {
        size_t ss = (u->uc_stack.ss_size >= GRTOS_FIBER_MIN_STACK)
                    ? u->uc_stack.ss_size : GRTOS_FIBER_MIN_STACK;
        u->fiber_handle = CreateFiberEx(ss, ss, FIBER_FLAG_FLOAT_SWITCH,
                                        grtos_fiber_entry, &u->fiber_info);
        u->fiber_owner_tid = GetCurrentThreadId();
        if (u->fiber_handle == NULL) { g_pending_oucp = NULL; return -1; }
    } else if (u->fiber_owner_tid != GetCurrentThreadId()) {
        u->fiber_owner_tid = GetCurrentThreadId();
    }

    SwitchToFiber(u->fiber_handle);

    /* Reached when this context (oucp) is resumed by a future SwitchToFiber,
     * possibly on a different OS thread than the one that suspended it
     * (task migration) -- flush THIS thread's own most recent pending
     * publish (unrelated to oucp; see flush_pending_publish()'s comment). */
    flush_pending_publish();

    /* FIXED: every oucp ever passed to swapcontext() in this
     * codebase is a TASK's own uctx (never the kernel's uctx_main, which is
     * always freshly makecontext()'d before each swap and never itself saved
     * for later resumption) -- confirmed by auditing every task-uctx
     * swapcontext/setcontext call site in gemrtos_controller_windows.c. So this
     * point is unambiguously "a task is resuming", for all three suspend
     * paths (GRTOS_Suspend_Task, Swap_task_to_kernel, GRTOS_Halt_Processor).
     * See GRTOS_ReleaseKernelMutexIfPending()'s own comment for why the
     * release belongs here rather than on the dispatching
     * side. No-ops if no release is pending (e.g. a kernel fiber resuming
     * mid dispatch, or the deferred/dead paths audited and excluded --
     * GRTOS_Change_SP_to_IDLE, Swap_task_to_task). */
    GRTOS_ReleaseKernelMutexIfPending();

    return 0;
}
