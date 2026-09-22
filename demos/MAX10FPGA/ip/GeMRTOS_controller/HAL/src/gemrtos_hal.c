/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Wrap for gemrtos_hal functions for GeMRTOS
 *  \details This file contains the HAL redefinitions.
 *  \version 0.9a
 *  \date      2015-2025
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

#ifndef __niosX_arch__
#include "jtag_uart_tcp.h"  /* per-processor TCP console emulation */
#endif

/** \brief Size limit (bytes) passed to vsnprintf for gu_fprintf.
 *
 *  This is vsnprintf's `n` argument, NOT the payload length: vsnprintf writes
 *  at most n-1 characters plus a null terminator, so the largest payload that
 *  can ever reach the console is GU_FPRINTF_BUF_LEN - 1 = 253 characters.
 *  An earlier revision of this comment claimed vsnprintf wrote up to
 *  GU_FPRINTF_BUF_LEN characters, and the emitted length was clamped to match
 *  that claim -- which appended vsnprintf's null terminator to every message
 *  long enough to be truncated. Stated explicitly here because that one word
 *  of imprecision is what produced the defect.
 *
 *  The physical buffer is GU_FPRINTF_BUF_SIZE = GU_FPRINTF_BUF_LEN + 2:
 *  index GU_FPRINTF_BUF_LEN holds a defensive null terminator written by
 *  gk_VPrintfBase; index GU_FPRINTF_BUF_LEN + 1 is an alignment guard byte. */
#define GU_FPRINTF_BUF_LEN   254U
#define GU_FPRINTF_BUF_SIZE  (GU_FPRINTF_BUF_LEN + 2U)

/** \brief Largest payload vsnprintf can actually produce in gk_fmt_buf. */
#define GU_FPRINTF_MAX_PAYLOAD  (GU_FPRINTF_BUF_LEN - 1U)

/* Console destinations. On the embedded target these are the two system-wide
 * JTAG UART instances the BSP wires stdout and stderr to -- taken from the
 * BSP's own macros so this stays correct across board projects rather than
 * hard-coding an address. They are NOT per-processor: one stdout and one
 * stderr are shared by every processor, which is exactly why access has to be
 * serialised. On Windows they are plain selectors, unused by the TCP path. */
#ifdef __niosX_arch__
    #ifdef ALT_STDOUT_BASE
        #define GK_CONSOLE_OUT_BASE  ALT_STDOUT_BASE
    #else
        #define GK_CONSOLE_OUT_BASE  0U
    #endif
    #ifdef ALT_STDERR_BASE
        #define GK_CONSOLE_ERR_BASE  ALT_STDERR_BASE
    #else
        #define GK_CONSOLE_ERR_BASE  GK_CONSOLE_OUT_BASE
    #endif
#else
    #define GK_CONSOLE_OUT_BASE      0U
    #define GK_CONSOLE_ERR_BASE      1U
#endif


/* ===========================================================================
 * NON-BLOCKING CONSOLE
 *
 * The JTAG UART is a system-wide resource -- one instance for stdout, one for
 * stderr, shared by every processor -- so console output must be serialised.
 * It is serialised by the GeMRTOS mutex, which makes the cost of blocking
 * inside it total: a processor that stalls here stalls every other processor
 * waiting on that mutex, and (because the controller suspends a processor's
 * watchdog countdown while it owns the mutex) it is not even recoverable by
 * its own watchdog.
 *
 * The Altera SMALL driver's write is an unbounded spin on WSPACE with no
 * host-presence check, so a full FIFO with nobody draining it wedges the whole
 * system. That is the failure this code exists to make impossible.
 *
 * The rule here is therefore absolute: NEVER block. Write only what the FIFO
 * has already been observed to have room for, and re-check for more.
 *
 * Splitting rather than truncating: a long message is written in as many
 * chunks as the FIFO needs, so no information is lost while the link works.
 * Because the whole loop runs under the mutex, no other processor can write
 * between chunks -- the split is invisible, byte-for-byte identical to a
 * single write, with no markers or merged lines.
 *
 * Bounding by progress rather than by total time: the loop gives up only after
 * GK_CONSOLE_STALL_MS of real time during which the FIFO freed no space AT
 * ALL. Any chunk written restarts that deadline, so a draining host always
 * gets the complete message however long it is, while an absent host is
 * detected within one budget. The stall deadline IS the "is the FIFO
 * draining?" test.
 * ======================================================================== */

/* The waiting budget itself is a user-facing tunable and lives in
 * gemrtos_config.h as GK_CONSOLE_STALL_MS. Converted here to controller ticks,
 * which is what gk_SystemTimeGet() returns. */
#define GK_UART_STALL_TICKS  ((G_UINT64) GK_CONSOLE_STALL_MS * (G_UINT64) G_TICKS_PER_MSECOND)

/* Early boot runs with the controller's time counters held in reset, so
 * gk_SystemTimeGet() returns a constant and a time deadline can never expire.
 * That window is detected explicitly (gm_TimeCountersResetGet(), the same
 * guard the critical-section macros use) and this small poll budget governs
 * instead. Deliberately small: an earlier version used 1000000 here as a
 * nominal "safety net" without doing the arithmetic -- ~0.3 s per stalled
 * message -- which turned a boot into a crawl. */
#define GK_UART_STALL_POLL_CAP  200U

#ifdef __niosX_arch__

/** \brief Set once a message could not be delivered in full, cleared as soon
 *  as one is. Lets the common "no host attached" case cost a single register
 *  read instead of the whole stall budget on every subsequent call, and
 *  self-clears the moment a host reappears. Written only under the mutex. */
static G_UINT32 g_gk_uart_stalled;

/** \brief Total console bytes discarded because the FIFO stopped draining.
 *
 *  A permanent diagnostic, not instrumentation. Because the console is now
 *  non-blocking by construction, output loss is a normal and expected outcome
 *  rather than a fault -- and without this counter it is entirely invisible:
 *  a discarded message leaves no trace anywhere, so "am I losing output?" was
 *  previously unanswerable. Costs one add on a path that is already writing to
 *  a device register.
 *
 *  Ownership: this module. Written only from gk_UartWriteNB(), whose caller
 *  holds the GeMRTOS mutex, so the read-modify-write needs no further
 *  synchronisation. Monotonic, never reset, and free to wrap -- it is read as
 *  a difference between two observations, not as an absolute. Not declared in
 *  any header on purpose: it is read externally over the JTAG-to-Avalon master
 *  by address, and exposing it as API would freeze a debug aid into the
 *  published interface.
 *
 *  The companion measurement counter g_gk_uart_maxstall_ticks was removed once
 *  it had done its job (real host drain latency was measured at
 *  ~1.07 s per 512-byte FIFO, which is what sized GK_CONSOLE_STALL_MS and
 *  established that no bounded wait can emit a burst). That number is recorded
 *  here; re-measuring it does not need to cost a global on every build. */
volatile G_UINT32 g_gk_uart_dropped;

/**
 * \brief Writes to a JTAG UART without ever blocking.
 * \details Writes only what the FIFO is observed to have room for, re-polling
 *          for more until the message is complete or the FIFO stops draining.
 * \param [in] base  JTAG UART slave base address.
 * \param [in] buf   Bytes to write.
 * \param [in] len   Number of bytes.
 * \return Bytes actually written; less than \p len if the FIFO stopped draining.
 * \pre The caller holds the GeMRTOS mutex.
 * \note Never blocks, never takes the mutex, safe before the kernel is running.
 */
static G_UINT32 gk_UartWriteNB(G_UINT32 base, const char *buf, G_UINT32 len)
{
    G_UINT32 written  = 0U;
    G_UINT32 polls    = 0U;
    G_UINT64 stall_t0 = 0U;
    G_UINT32 stalling = 0U;
    /* Is the controller's time counter running? If not (early boot) the time
     * deadline is meaningless and the poll budget governs instead. */
    G_UINT32 timed    = (gm_TimeCountersResetGet() == 0U) ? 1U : 0U;

    /* Presumed-absent fast path: one register read, then out. Without this,
     * every call in a hostless system would burn the full stall budget under
     * the mutex. */
    if (g_gk_uart_stalled != 0U) {
        if ((((G_UINT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(base)) &
             (G_UINT32) ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK) == 0U) {
            /* Must account for this drop here: an earlier version returned
             * without doing so, which made every message discarded through
             * this fast path invisible to g_gk_uart_dropped and led to the
             * counter contradicting the captured output. */
            g_gk_uart_dropped += len;
            return 0U;
        }
    }

    while (written < len) {
        G_UINT32 space =
            (((G_UINT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(base)) &
              (G_UINT32) ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK)
            >> (G_UINT32) ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_OFST;

        if (space > 0U) {
            G_UINT32 pending = len - written;
            G_UINT32 n       = (pending <= space) ? pending : space;
            G_UINT32 i;

            /* Cannot block: every byte here has observed room. */
            for (i = 0U; i < n; i++) {
                IOWR_ALTERA_AVALON_JTAG_UART_DATA(base, buf[written + i]);
            }
            written += n;
            stalling = 0U;       /* progress -- restart the drain deadline */
        } else if (timed != 0U) {
            G_UINT64 now = gk_SystemTimeGet();

            if (stalling == 0U) {
                stalling = 1U;
                stall_t0 = now;  /* first zero-space poll of this stall */
            } else if ((now - stall_t0) >= GK_UART_STALL_TICKS) {
                /* Compared at full 64-bit width. The previous form truncated
                 * the elapsed time to 32 bits first, which existed only to
                 * feed a 32-bit measurement counter, now removed; the deadline
                 * itself never needed it. Unreachable difference either way --
                 * 2^32 ticks is ~7 min against a budget of at most 2 s. */
                break;           /* not draining -- abandon the remainder */
            }
        } else {
            /* Early boot: no usable clock, so bound by polls instead. */
            polls++;
            if (polls >= (G_UINT32) GK_UART_STALL_POLL_CAP) {
                break;
            }
        }
    }

    /* "Absent" means NO progress at all, not merely an incomplete message.
     * A burst larger than the FIFO (the boot banner is several times its size)
     * legitimately produces partial writes against a host that is draining
     * perfectly well, just slower than we can fill. Latching the absent flag
     * on those cost 47 of the banner's 61 lines when this was first measured:
     * one partial write armed the fast path, and every later message then took
     * the bail-out. Any byte written proves the FIFO is draining. */
    g_gk_uart_stalled = (written == 0U) ? 1U : 0U;
    g_gk_uart_dropped += (len - written);
    return written;
}

#endif /* __niosX_arch__ */

/**
 * \brief Shared body of gk_printf/gk_fprintf and, through them, of the gu_ pair.
 * \param [in] base    Destination JTAG UART base (embedded); ignored on Windows.
 * \param [in] format  printf format string.
 * \param [in] args    Already-started va_list.
 * \pre The caller holds the GeMRTOS mutex.
 * \note The formatting buffer is a single static buffer, not a stack array:
 *       the mutex precondition already guarantees one writer at a time, so the
 *       256 bytes are spent once instead of on every kernel stack that prints.
 */
static void gk_VPrintfBase(G_UINT32 base, const char *format, va_list args)
{
    static char gk_fmt_buf[GU_FPRINTF_BUF_SIZE];
    int         formatted;
    G_UINT32    len;

    gk_fmt_buf[GU_FPRINTF_BUF_LEN]      = '\0';
    gk_fmt_buf[GU_FPRINTF_BUF_LEN + 1U] = '\0';

    formatted = vsnprintf(gk_fmt_buf, GU_FPRINTF_BUF_LEN, format, args);
    if (formatted <= 0) {
        return;                                  /* nothing to emit */
    }
    /* vsnprintf returns what it WOULD have written: clamp to what it did,
     * which is at most GU_FPRINTF_BUF_LEN - 1 characters -- clamping to
     * GU_FPRINTF_BUF_LEN instead emits vsnprintf's null terminator as a
     * payload byte. */
    len = ((G_UINT32) formatted > GU_FPRINTF_MAX_PAYLOAD) ? GU_FPRINTF_MAX_PAYLOAD
                                                          : (G_UINT32) formatted;

#ifdef __niosX_arch__
    (void) gk_UartWriteNB(base, gk_fmt_buf, len);
#else
    /* Windows: route to this processor's own TCP console. The processor id is
     * read once, here: a task can migrate between processors mid-call, so a
     * second read could route the tail of a message to a different console. */
    {
        unsigned int pid = (unsigned int) gm_ProcessorId();
        (void) base;
        jtag_uart_tcp_write_processor(pid, gk_fmt_buf, (unsigned int) len);
    }
#endif
}

/**
 *  \brief Kernel-level printf to standard output. Never blocks.
 *  \param [in] format printf format string.
 *  \pre The caller already holds the GeMRTOS mutex. Unlike gu_printf(), this
 *       function does NOT acquire it -- that is what makes it usable from
 *       kernel context, from the watchdog recovery path, and before the kernel
 *       is running.
 *  \relates System
 *  \remark gk_printf
 */
void gk_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    gk_VPrintfBase((G_UINT32) GK_CONSOLE_OUT_BASE, format, args);
    va_end(args);
}

/**
 *  \brief Kernel-level fprintf to standard error. Never blocks.
 *  \param [in] format printf format string.
 *  \pre The caller already holds the GeMRTOS mutex -- see gk_printf().
 *  \relates System
 *  \remark gk_fprintf
 */
void gk_fprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    gk_VPrintfBase((G_UINT32) GK_CONSOLE_ERR_BASE, format, args);
    va_end(args);
}


/**
 *  \brief The gu_printf function formats text and writes it to standard output.
 *  \exception par_following The following parameter is required for the gu_printf function:
 *  \param [in] format A string that may contain format specifiers like %d, %s, etc., which control the formatting of subsequent arguments.
 *  \return void — no return value.
 *  \relates System
 *  \remark gu_printf
 */
void gu_printf(const char *format, ...)
{
	gm_GeMRTOSCriticalSectionEnter();
		va_list args;
		va_start (args, format);
		gk_VPrintfBase((G_UINT32) GK_CONSOLE_OUT_BASE, format, args);
		va_end (args);
	gm_GeMRTOSCriticalSectionExit();
}


/**
 *  rief The gu_fprintf function formats text and writes it to standard error output (stderr).
 *  \exception par_following The following parameter is required for the gu_fprintf function:
 *  \param [in] format A string that may contain format specifiers like %d, %s, etc., which control the formatting of subsequent arguments.
 *  
eturn void -- no return value.
 *  
elates System
 *  
emark gu_fprintf
 */
void gu_fprintf(const char *format, ...)
{
	gm_GeMRTOSCriticalSectionEnter();
		va_list args;
		va_start (args, format);
		gk_VPrintfBase((G_UINT32) GK_CONSOLE_ERR_BASE, format, args);
		va_end (args);
	gm_GeMRTOSCriticalSectionExit();
}

/* Nios newlib hooks — the embedded C library (newlib) calls
 * __malloc_lock / __malloc_unlock / __env_lock / __env_unlock to serialise
 * malloc and environment access.  GeMRTOS hooks these to acquire and release
 * the hardware critical section.  No equivalent hooks exist on Windows. */
#ifdef __niosX_arch__

/**
 * \brief Shared body for __malloc_lock and __env_lock — acquire the GeMRTOS critical section.
 *
 * Acquires the hardware critical section via GRTOS_CMD_CRITICAL_SECTION_GET and
 * increments the per-TCB mutex nesting counter to track re-entrancy.  No-op before
 * the kernel is running (guarded by gm_TimeCountersResetGet()).
 */
static void gk_HAL_LockAcquire(void)
{
    if (!(gm_TimeCountersResetGet())) {
        GRTOS_CMD_CRITICAL_SECTION_GET;
        if ((gm_KernelModeGet() == G_FALSE)) {
            g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED++;
        }
    }
}

/**
 * \brief Shared body for __malloc_unlock and __env_unlock — release the GeMRTOS critical section.
 *
 * Decrements the per-TCB mutex nesting counter and releases the hardware critical
 * section via GRTOS_MTX_RLS.  No-op before the kernel is running.
 */
static void gk_HAL_LockRelease(void)
{
    if (!(gm_TimeCountersResetGet())) {
        if ((gm_KernelModeGet() == G_FALSE)) {
            g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED--;
            gm_MutexNestedValue(g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED);
        }
        GRTOS_MTX_RLS;
    }
}

/**
 * \brief newlib thread-safety hook — acquire the GeMRTOS critical section before malloc.
 * \param [in] _r  Pointer to the newlib reentrancy structure (unused on this target).
 * \note Delegates to gk_HAL_LockAcquire(). Must not be called from an ISR context.
 */
void __malloc_lock ( struct _reent *_r )   { (void) _r; gk_HAL_LockAcquire(); }

/**
 * \brief newlib thread-safety hook — release the GeMRTOS critical section after malloc.
 * \param [in] _r  Pointer to the newlib reentrancy structure (unused on this target).
 * \note Delegates to gk_HAL_LockRelease(). Must be paired with __malloc_lock().
 */
void __malloc_unlock ( struct _reent *_r ) { (void) _r; gk_HAL_LockRelease(); }

/**
 * \brief newlib thread-safety hook — acquire the GeMRTOS critical section before env access.
 * \param [in] _r  Pointer to the newlib reentrancy structure (unused on this target).
 * \note Delegates to gk_HAL_LockAcquire(). Environment variables must not be accessed from ISRs.
 */
void __env_lock ( struct _reent *_r )      { (void) _r; gk_HAL_LockAcquire(); }

/**
 * \brief newlib thread-safety hook — release the GeMRTOS critical section after env access.
 * \param [in] _r  Pointer to the newlib reentrancy structure (unused on this target).
 * \note Delegates to gk_HAL_LockRelease(). Must be paired with __env_lock().
 */
void __env_unlock ( struct _reent *_r )    { (void) _r; gk_HAL_LockRelease(); }


#endif 
