/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Windows Fiber implementation of the POSIX ucontext API (interface)
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
 *  grtos_fiber.h â€” Windows Fiber-based replacement for POSIX ucontext
 *
 *  Provides getcontext / makecontext / swapcontext / setcontext using the
 *  Windows Fiber API (CreateFiber / SwitchToFiber / ConvertThreadToFiberEx).
 *  Fibers are created lazily inside swapcontext so that each fiber is always
 *  owned by the processor thread that first switches to it.
 *
 *  This header defines UCONTEXT_H so that ucontext.h (Hadjidoukas, based on
 *  GetThreadContext/SetThreadContext) is silently skipped when both headers
 *  appear in the same translation unit.
 */

#ifndef UCONTEXT_H
#define UCONTEXT_H

#include <windows.h>
#include <stddef.h>

/* -------------------------------------------------------------------------
 * stack_t  â€” same layout as ucontext.h; uc_stack.ss_size is used by
 *             makecontext to size the fiber stack; ss_sp is accepted but
 *             ignored (the Fiber API manages its own stack).
 * ----------------------------------------------------------------------- */
typedef struct __stack {
    void   *ss_sp;
    size_t  ss_size;
    int     ss_flags;
} stack_t;

typedef unsigned long __sigset_t;

/* -------------------------------------------------------------------------
 * grtos_fiber_info_t â€” trampoline data stored inside every ucontext_t.
 *   argc == 0 : calls func_noarg()
 *   argc == 1 : calls func_onearg(arg)
 * ----------------------------------------------------------------------- */
typedef struct __grtos_fiber_info {
    void (*func_noarg)(void);
    void (*func_onearg)(void *);
    void  *arg;
    int    argc;
} grtos_fiber_info_t;

/* -------------------------------------------------------------------------
 * ucontext_t â€” Fiber-based context descriptor.
 *   fiber_handle     NULL until the first swapcontext that targets this context
 *                    (lazy creation ensures fiber is initially created in the
 *                    calling processor thread).
 *   fiber_owner_tid  Thread ID currently "hosting" this fiber.  Used only for
 *                    diagnostic logging of cross-processor task migration.
 *                    Windows SwitchToFiber updates the calling thread's TEB
 *                    (StackBase/StackLimit) when switching to any fiber, so
 *                    cross-thread SwitchToFiber works correctly â€” the task
 *                    resumes from its suspension point on the new processor.
 * ----------------------------------------------------------------------- */
typedef struct __ucontext {
    unsigned long int   uc_flags;
    struct __ucontext  *uc_link;
    stack_t             uc_stack;         /* ss_size used for CreateFiber stack   */
    LPVOID              fiber_handle;     /* Windows Fiber handle; NULL = pending */
    DWORD               fiber_owner_tid; /* thread currently hosting this fiber   */
    grtos_fiber_info_t  fiber_info;       /* function + arg for trampoline        */
} ucontext_t;

/* -------------------------------------------------------------------------
 * POSIX-compatible function declarations
 * ----------------------------------------------------------------------- */
int getcontext(ucontext_t *ucp);
int setcontext(const ucontext_t *ucp);
int makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);
int swapcontext(ucontext_t *oucp, const ucontext_t *ucp);

/* -------------------------------------------------------------------------
 * ensure_fiber â€” convert the calling thread to a fiber if it isn't one yet
 * (IsThreadAFiber() + ConvertThreadToFiberEx()), otherwise return its current
 * fiber handle.  Exported so callers outside grtos_fiber.c (notably
 * Swap_kernel_to_task in gemrtos_controller_windows.c) can safely query "my current
 * fiber" without risking Windows' non-NULL sentinel return from a raw
 * GetCurrentFiber() call on a thread that was never converted.
 * ----------------------------------------------------------------------- */
LPVOID ensure_fiber(void);

#endif /* UCONTEXT_H */
