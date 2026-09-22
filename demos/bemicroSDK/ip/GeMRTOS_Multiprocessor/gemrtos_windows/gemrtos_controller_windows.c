/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Windows-simulation software model of the GeMRTOS Controller
 *  \details Register-file emulation and mutex/IRQ/time state — the same registers
 *           gemrtos_primitives.h's gm_* macros address via gm_IOWR/gm_IORD on the
 *           embedded target, reimplemented here as plain C globals and functions.
 *           Not the context-switch mechanism (that is grtos_fiber.c/.h) and not a
 *           user-facing API — see gemrtos_sem.h, gemrtos_mq.h, gemrtos_trigger.h and
 *           gemrtos_core.h's gu_* declarations for that.
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

#include <gemrtos.h>
#include <sys/time.h>

/* The monitor register map exists only in BSPs whose Platform Designer system
 * contains an avalon_monitor (system.h then defines GEMRTOS_DRIVER_BASE_MONITOR,
 * and the header ships in that BSP's drivers/inc).  The other example BSPs have
 * neither, so both the include and the write handler below are guarded by the
 * same macro. */
#ifdef GEMRTOS_DRIVER_BASE_MONITOR
#include <avalon_monitor_regs.h>   /* ADDR_RESET_REDIRECT */
#endif

#ifndef __niosX_arch__
/* -- permanent crash diagnostic for the Windows simulation build.
 * The fiber-based context-switch machinery has produced several timing-sensitive
 * crashes that reproduce under some thread schedulings but NOT in the
 * isolated git-bash/PowerShell harness, making static reasoning unreliable. This
 * unhandled-exception filter costs nothing at runtime (fires only on an actual crash)
 * and prints the exact faulting instruction address, the module load base (so the
 * offset maps via `addr2line -e hellogemrtos.exe <offset>`), the faulting data address
 * for access violations, and a raw return-address backtrace. Kept as a permanent safety
 * net: if any latent race resurfaces, this captures its exact location on the first
 * occurrence instead of leaving only a silent exit. Installed before main() via a
 * constructor. Never compiled for the NiosX target. */
/* Fixed absolute path so the death report survives regardless of cwd (VS Code may run
 * from the workspace root or the hellogemrtos dir) and can be read back off the same
 * machine directly. Opened in append mode; each death appends one block. */
#define GRTOS_DEATH_REPORT_PATH "C:\\Quartus\\GRTOS\\GeMRTOS\\claude\\gemrtos_vscode\\hellogemrtos\\death_report.txt"

/* Write the same report to both stderr (visible in the terminal) and the fixed file. */
static void GRTOS_report_backtrace(FILE *fp, HMODULE base)
{
    void  *frames[48];
    USHORT n = CaptureStackBackTrace(0, 48, frames, NULL);
    USHORT i;
    for (i = 0; i < n; i++) {
        fprintf(fp, "  frame[%2u] = %p  (off 0x%tx)\n",
                i, frames[i], (char *) frames[i] - (char *) base);
    }
}

static LONG WINAPI GRTOS_CrashHandler(EXCEPTION_POINTERS *ep)
{
    DWORD   code = ep->ExceptionRecord->ExceptionCode;
    void   *addr = (void *) ep->ExceptionRecord->ExceptionAddress;
    HMODULE base = GetModuleHandleA(NULL);
    FILE   *out[2];
    int     k;
    out[0] = stderr;
    out[1] = fopen(GRTOS_DEATH_REPORT_PATH, "a");
    for (k = 0; k < 2; k++) {
        FILE *fp = out[k];
        if (fp == NULL) continue;
        fprintf(fp, "\n==== GRTOS CRASH (tid=%lu) ====\n", GetCurrentThreadId());
        fprintf(fp, "exception code = 0x%08lx\n", (unsigned long) code);
        fprintf(fp, "fault instr    = %p  (base %p, offset 0x%tx)\n",
                addr, (void *) base, (char *) addr - (char *) base);
        if (code == EXCEPTION_ACCESS_VIOLATION && ep->ExceptionRecord->NumberParameters >= 2) {
            fprintf(fp, "access         = %s at data %p\n",
                    ep->ExceptionRecord->ExceptionInformation[0] ? "WRITE" : "READ",
                    (void *) ep->ExceptionRecord->ExceptionInformation[1]);
        }
        GRTOS_report_backtrace(fp, base);
        fprintf(fp, "==== END CRASH ====\n");
        fflush(fp);
    }
    if (out[1]) fclose(out[1]);
    return EXCEPTION_EXECUTE_HANDLER;   /* let the process terminate after logging */
}

/* The process can sometimes "just ends" mid-output with NO
 * crash banner (the SEH filter above does not fire). That means it is NOT an access
 * violation but a clean exit()/return-from-main OR a hard fast-fail (heap corruption /
 * stack overflow, which bypass both SEH and atexit). This atexit hook fires ONLY on a
 * clean exit()/return path; if the death is clean, we see this banner + a backtrace of
 * whoever triggered the exit. Three-way triage: [CRASH banner] = access violation;
 * [ATEXIT banner] = clean exit (who? see backtrace); NEITHER = fast-fail. */
static void GRTOS_AtExitHook(void)
{
    HMODULE base = GetModuleHandleA(NULL);
    FILE   *out[2];
    int     k;
    out[0] = stderr;
    out[1] = fopen(GRTOS_DEATH_REPORT_PATH, "a");
    for (k = 0; k < 2; k++) {
        FILE *fp = out[k];
        if (fp == NULL) continue;
        fprintf(fp, "\n==== GRTOS ATEXIT (clean exit, tid=%lu) ====\n", GetCurrentThreadId());
        GRTOS_report_backtrace(fp, base);
        fprintf(fp, "==== END ATEXIT ====\n");
        fflush(fp);
    }
    if (out[1]) fclose(out[1]);
}

__attribute__((constructor))
static void GRTOS_InstallCrashHandler(void)
{
    /* FIXED -- stdout must be unbuffered on this port, not just for
     * crash diagnostics but for correctness of the whole verification tier.
     * PRINT_ASSERT (gemrtos_primitives.h) does printf(...) followed by
     * G_DEBUG_WHILEFOREVER, an infinite loop that never returns -- so a failing
     * assert NEVER reaches atexit/fflush. verify_windows.ps1 (and any other harness
     * that redirects stdout to a file) hard-kills the process after its capture
     * window; with the C runtime's default full (block) buffering on a
     * non-terminal stdout, that printf's bytes are sitting in an in-process buffer
     * that is destroyed, not flushed, by the kill. Confirmed empirically: a
     * standalone repro (five ordinary printfs + one "assert-style" printf + an
     * infinite loop, redirected and hard-killed exactly like verify_windows.ps1
     * does) produced
     * a completely EMPTY captured file -- not just the last line lost, all of it,
     * since nothing had yet forced a flush. This was masking real PRINT_ASSERT
     * firings as generic "silent stalls" with no diagnostic text. Unbuffered stdout
     * also better matches real hardware: a UART transmits each byte through its own
     * FIFO as it is written, not through a large software buffer that vanishes on a
     * hard reset the way a killed process's heap does. Must run before ANY printf in
     * the process -- placed in this constructor because it already runs before
     * main() on every example, and before it's too late: setvbuf() is only valid
     * before the first read/write on the stream. */
    setvbuf(stdout, NULL, _IONBF, 0);

    SetUnhandledExceptionFilter(GRTOS_CrashHandler);
    atexit(GRTOS_AtExitHook);
    /* Startup marker in the death report: a START with no CRASH/ATEXIT block after it
     * means neither handler fired -> hard fast-fail (heap corruption / stack overflow).
     */
    {
        FILE *fp = fopen(GRTOS_DEATH_REPORT_PATH, "a");
        if (fp) {
            fprintf(fp, "\n######## GRTOS START pid=%lu t=%lu ########\n",
                    GetCurrentProcessId(), GetTickCount());
            fflush(fp);
            fclose(fp);
        }
    }
}

#endif


/* Verbose "RUNNING ->" hot-path trace: OFF by default. These printf()s sit in the kernel
 * hot path (GRTOS_Start_Task, IOWR) and do console I/O on nearly every kernel operation.
 * Under VS Code's ConPTY the console writes block, massively perturbing thread timing (a
 * prime suspect for the intermittent death) and crushing throughput. Set to 1 to restore
 * them for debugging. */
#ifndef GRTOS_DEBUG_RUNNING_TRACE
#define GRTOS_DEBUG_RUNNING_TRACE 0
#endif
#if defined(__niosX_arch__) || (GRTOS_DEBUG_RUNNING_TRACE == 0)
#define GRTOS_RUNPRINTF(...) ((void)0)
#else
#define GRTOS_RUNPRINTF(...) printf(__VA_ARGS__)
#endif


/* =========================================================================
 * GLOBAL CONTROLLER STATE (Replicated VHDL Registers)
 * ========================================================================= */

typedef struct {
    /* === GLOBAL REGISTERS === */

    /* Control/Status Register (ADDR_CTRL_STATUS) */
    G_UINT32  CTRL_STATUS;
	G_UINT32  R_CTLR;  /* Control Register  */
    G_UINT32  R_STATUS;                /* Status Register */    

    /* -----------------------------------------------------------------
    * Flags */
	G_UINT32  R_ELP_TM_CNT;	          /* Elapsed Time Counter       */
	G_UINT32  R_LST_EVN;               /* Last Event register */
    G_UINT32  R_PRC_INT;	              /* Processor Interrupt Register */
    G_UINT32  R_INT_CLR;               /* Interrupt Clear Register	 */
    G_UINT32  R_PRC_RST;	              /* Processor Reset Register */
    G_UINT32  R_PRC_RST_CNT;	          /* Reset Time Counter           */
    G_UINT32  R_INT_RQS;  /* Interrupt Request register */
    G_UINT32  C1_IRQ_PND;                      /* IRQ pendig flag */
    G_UINT32  C1_MTX_GRN;            /* Mutex is Granted to a processor   */

    /*********************************************************************** */
    /* Global Internal Registers of the Controller */
    /* ********************************************************************** */
    G_UINT32  R_MTX_BLK_INT;
    G_UINT32  R_MTX_BLK_INT_NXT;
    G_UINT32  R1_MTX_DEBUG_RSV;                      /* Flag for mutex reservation for debug */
    G_UINT32  C_MTX_PRC_GRN_BIT;  /* Bit decoder logic of granted processor */
    G_UINT32  R_MTX_RSV_PRC_LOWEST_INDEX;  /* Processor requesting mutex with lowest index */
    G_UINT32  GATED_CLK;
    G_UINT32  IRQ_EVENT_PENDING;
    /* mirrors DIRQI_synch (grtos.vhd:718-725,1757-1758), the
     * synchronized raw external-interrupt pin state. Nothing in this Windows sim writes
     * it today (there is no external-event source), so ADDR_DIRQI/ADDR_IRQ_RQS read back
     * 0, same as a hardcoded constant would -- but backing them with a real field means a
     * future external-IRQ-simulator thread only needs one locked write site here, instead
     * of requiring the register-read layer to be reworked. */
    G_UINT32  DIRQI_synch;

    /* Mutex State Machine */
    G_UINT32  R_MTX_PRC_GRN;        /* Which processor holds mutex (one-hot or processor ID) */
    G_UINT32  R1_MTX_NESTED;        /* Nested flag: 0=can release, 1=hold (binary gate) */
    G_UINT32  R_MTX_RLS_BIT;        /* Release request bits per processor */
    /* output of GRTOS_Compute_Mutex_Next_Processor() -- the processor
     * currently designated to receive the mutex next (lowest-index priority over
     * R_WAITING_IN_ISR below, mirroring grtos.vhd's ROTATING_PRIORITY_FOR_MUTEX_RESERVATION,
     * grtos.vhd:1286-1295). Stored as a plain processor ID (0 = none eligible), matching how
     * R_MTX_PRC_GRN is already represented here (VHDL uses one-hot bitmasks for both;
     * this sim already deviated for R_MTX_PRC_GRN, so this follows suit). Only fresh
     * immediately after that function runs -- see its header comment for why. */
    G_UINT32  R_MTX_NXT_PRC;
    G_UINT32  R_MTX_PRC_RSV;        /* Reservation requests per processor */
    /* filtered eligibility subset of R_MTX_PRC_RSV, mirroring VHDL's
     * R_WAITING_IN_ISR_Next (grtos.vhd:1270-1271): if any reserving processor is in
     * R_PRC_IRQ_SECTION, ONLY those processors are eligible this round (in-ISR
     * reservers strictly preempt non-ISR reservers, not just "before" them); else falls
     * back to reservers not currently interrupted. Debug/observability mirror, maintained
     * by GRTOS_Compute_Mutex_Next_Processor(). */
    G_UINT32  R_WAITING_IN_ISR;
    G_UINT32  R_PRC_IRQ_ACK;

    /* Interrupt Management */
    G_UINT32  R_IRQ_RQS;            /* Pending IRQ bitmap */
    G_UINT32  R_PRC_IRQ_SECTION;    /* Processor in ISR section */
    G_UINT32  R_PRC_INT_ENB;        /* Per-processor interrupt enable */
    G_UINT32  R_IRQ_ENB;            /* IRQ enable mask */

    /* Time Management */
    TIMEPRIORITY  R_TM_CNT;             /* System time counter */
    TIMEPRIORITY  R_NXT_EVN_CNT;        /* Next event time */
    TIMEPRIORITY  R_FRZ_TM_THR;         /* Frozen-mode threshold */
    /* mirrors R_FRZ_TM_CNT (grtos.vhd:673-674,
     * 1245-1250) -- accumulated elapsed time while frozen mode is both enabled and
     * triggered. Maintained by GRTOS_Check_Frozen_Mode(); see that function for what
     * is/isn't modeled (detect-and-report only, does not pause R_TM_CNT/scheduling
     * elsewhere, unlike real hardware). */
    TIMEPRIORITY  R_FRZ_TM_ELP;         /* Frozen-mode elapsed time */
    TIMEPRIORITY  C_SYS_TM;             /* System Time (frozen + non-frozen) */
    TIMEPRIORITY  R_SMP;                /* Temporal register */  

    TIMEPRIORITY g_sys_tm_base;    
  
    /* Processor State Tracking */
    G_UINT32  R_PRC_IDL_ENB;        /* Idle mode enable per processor */
    /* Bitmask, one bit per processor: 1 while that processor's OS thread is
     * parked inside GRTOS_Halt_Processor_Wait()'s SleepConditionVariableCS
     * loop, 0 while actively running. Was declared but never wired up before
     * the per-processor watchdog emulation (ANALYSIS-processor-watchdog-
     * reset.md) needed exactly this signal -- the Windows equivalent of
     * "waitrequest asserted", since there is no independent hardware counter
     * here to observe halted state any other way. Set/cleared only at
     * GRTOS_Halt_Processor_Wait()'s two loop boundaries; read by the
     * watchdog monitor thread (GRTOS_Watchdog_Monitor). */
    G_UINT32  R_PRC_HLT;            /* Processor halt state */
    G_UINT32  R_KERNEL_MODE_PRC;    /* Per-processor kernel mode flag */
    /* Bitmask, one bit per processor: set by gm_PrcWatchdogEnable()/cleared
     * by gm_PrcWatchdogDisable() (ADDR_REG_1/ADDR_REG_2 bit
     * PRC_WATCHDOG_ENB_BIT). Gates whether GRTOS_Watchdog_Monitor tracks
     * that processor at all. */
    G_UINT32  R_PRC_WATCHDOG_ENB;
    /* Global raw watchdog timeout, as last written by gm_WatchogTimeoutSet()
     * (ADDR_CTRL_STATUS bits[0..11], the value's low 12 bits -- see the
     * CTRL_WATCHDOG_BIT branch in IOWR()). One shared timeout for every
     * processor, matching the register being on the GLOBAL base
     * (GEMRTOS_DRIVER_BASE), not the per-processor one. Power-up default is
     * 0xFFF (set in Reset_GeMRTOS_controller_registers()), matching the
     * documented hardware default. NOTE: the real-hardware-to-wall-clock
     * conversion of this raw value is not yet resolved -- see
     * GRTOS_Watchdog_Monitor()'s own comment. */
    G_UINT32  R_WATCHDOG_TIMEOUT_RAW;

    /* Frozen Mode State Machine */
    G_UINT32  frozen_active;        /* System in frozen mode */
    G_UINT32  frozen_enabled;       /* Frozen mode enabled */
    G_UINT32  C1_UNFRZ_EVN;
    G_UINT32  C1_FRZ_EVN;
    G_UINT32  RESET_HOLD;
    G_UINT32  R_ERET_REG;
    G_UINT32  GATED_CLK_POS;
    G_UINT32  R1_KERNEL_MODE;
    /* armed by an external JTAG-master debug tool (a Platform Designer/Qsys
     * Avalon bus-master IP, distinct from the JTAG UART console peripheral) accessing the
     * Avalon bus directly on real hardware -- no such actor exists in this Windows
     * simulation (no Avalon bus, no Qsys system), so only the storage is modeled here.
     * The VHDL-derived combinational output this arms (frozen_avalon_controller_monitor,
     * asserted when R_MTX_PRC_GRN==0 AND this is set -- grtos.vhd:983-995) has no consumer
     * in this build and is deliberately NOT modeled. */
    G_UINT32  R1_STATUS_frozen_avalon_controller_monitor;
    G_UINT32  R1_HLD_CNT_RST;
	G_UINT32  C1_FRZ_TM_OCC;                      /* Frozen Threshold has been reached */
    G_UINT32  C1_FRZ_MDE_ENB;          /* Frozen trigger enabled */
    G_UINT32  TIME_HOLD;
	G_UINT32  R_MTX_HGHST_PRC;    
    G_UINT32  R_LOW_PRI_PRC;     /* Lowest Priority Processor register */  
    G_UINT32  R_INT_ENB;         /* Interrupt Enable register */
	G_UINT32  R_NXT_TM_EVN_PRC;  /* Next Timed Event Processor register */  
	G_UINT32  R_IRQ_PND;         /* IRQ Pending register  */ 
    G_UINT32  R_TRG_PRC_INT;     /* Trigger Processor Interrupt register */
    G_UINT32  R_TM_PSC;          /* Time Prescale register */
    G_UINT32  R_PRC_CRTCL_SECTION; /* Interrupt section for processors */   

    G_UINT32  R_MTX_NESTED;
    G_UINT32  R_HLT_IDL_PRC_ENB;   /* Enable bit to halt processor when idle */
    G_UINT32  R_HLT_IDL_PRC_ACT;   /* Active bit for halt processor when idle (enabled and reading the processor port) */
    G_UINT32  DEBUG_HOLD;          /* when 1 then debug hold enable, disabled when 0. */
    G_UINT32  HOLD_COUNTER_R_CTLR; /* when 0 then counters are hold. */
    G_UINT32  HALT_ENABLED_R_CTLR; /* when 1, halt is disable for eclipse debugging */
    G_UINT32  R_INOUTS;            /* I/O conduit register (ADDR_INOUTS) */

    /* === PROCESSOR-LOCAL REGISTERS (Per-Processor) === */

	struct timespec SYSTEM_STARTING_TIME;
    G_UINT32 slave_processor_waitrequest;

    struct {
        G_UINT32 PRC_REG_0;         /* Processor-local register 0 (status/control) */
        G_UINT32 PRC_REG_1;         /* Processor-local register 1 (mutex/interrupt control) */
        G_UINT32 PRC_REG_2;         /* Processor-local register 2 (interrupt disable) */
    } processor_regs[4];            /* Support up to 4 processors */

} gemrtos_controller_t;

/* Global controller state */
static gemrtos_controller_t g_controller = {0};


/* ********************************************************************** */
/* Temporal Registers of the Controller */
/* ********************************************************************** */
/* No processor is Next Timed Event */
unsigned int R_PRC_INT_STA;  /* Processor Interrupt Status register     */
unsigned int R_WAITING_IN_ISR;


GS_TCB *gk_Get_current_TCB(void)
{
	return (g_kcb.G_PCBTbl[gm_ProcessorId() - 1].PCB_EXECTCB);
}

G_UINT32 gk_Get_current_CPUID(void) 
{
	return ((G_UINT32) gm_ProcessorId());
}


char GRTOS_jump_to_reset[128];
char GRTOS_RETURN_CONTEXT[128];
char gemrtos_gemrtos[128];
    

void GRTOS_Change_SP_to_IDLE(void)
{
	/* FIXED: on real NiosX hardware this function changes the SP
	 * register to the processor's own dedicated kernel stack BEFORE its body runs --
	 * gk_KERNEL_TASK_COMPLETE() then executes entirely off the completing task's own
	 * stack. The Windows equivalent of that always-available per-processor stack is
	 * uctx_main (already used the same way by Swap_task_to_kernel()). Without this
	 * swap, gk_KERNEL_TASK_COMPLETE()'s call to gk_TASK_STK_Init() (via
	 * gk_TCBWL_Link(ptcb, WAITING_COMPLETED)) would reinitialise ptcb->uctx -- the
	 * exact fiber still executing this code -- a genuine stack-reuse inconsistency
	 * confirmed by diagnosis. Saving into ptcb->uctx (rather than a bare
	 * setcontext) also lets gk_TASK_STK_Init's own makecontext() correctly detect this
	 * is no longer the current fiber and DeleteFiber() it, instead of orphaning it. */
	G_UINT32 CPU_ID = gk_Get_current_CPUID();
	GS_TCB  *ptcb   = g_kcb.G_PCBTbl[CPU_ID-1].PCB_EXECTCB;

	g_kcb.G_PCBTbl[CPU_ID-1].uctx_main.uc_stack.ss_sp = g_kcb.G_PCBTbl[CPU_ID-1].uctx_main_stack;
	makecontext(&g_kcb.G_PCBTbl[CPU_ID-1].uctx_main, (void (*)()) gk_KERNEL_TASK_COMPLETE, 0);
	swapcontext((ucontext_t *) &ptcb->uctx, &g_kcb.G_PCBTbl[CPU_ID-1].uctx_main);

	/* Never reached in normal operation: ptcb->uctx is reinitialised to gu_pthread by
	 * gk_TASK_STK_Init() before this could ever be resumed. Defensive only. */
	while(1);
}

/* --------------------------------------------------------------------------
 * Per-processor fiber bookkeeping for Swap_kernel_to_task().  Indexed [CPU_ID-1].
 *   g_bootstrap_fiber      : the fiber obtained when the processor thread was
 *                            first converted to a fiber.  It owns the thread's
 *                            original stack and must NEVER be deleted.
 *   g_fiber_pending_delete : a kernel fiber abandoned on the previous
 *                            kernel->task swap, freed on the next one.  Windows
 *                            forbids DeleteFiber() on the running fiber, so the
 *                            deletion is deferred by exactly one swap.
 *
 * RESTORED: this deferred-delete was briefly removed (orphaned
 * instead) while chasing what looked like a residual fiber crash. That crash
 * was actually the R_PRC_INT-never-cleared bug (see IOWR's PRC_IRQ_ACK_BIT case) -- NOT
 * this deletion, which is genuinely safe: kernel fibers are per-CPU_ID, and a
 * given CPU_ID always maps to the same OS thread, so both the create (in
 * Swap_task_to_kernel's lazy CreateFiberEx) and the deferred DeleteFiber here run
 * on that same thread (never cross-thread), and the fiber being deleted was
 * abandoned a full swap ago (never the running one). Orphaning instead leaked one
 * 64 KB fiber PER kernel->task dispatch (~110/s) -> ~7 MB/s of virtual address
 * space -> 32-bit OOM crash after several minutes. Restoring the delete stops the
 * leak. */
static LPVOID g_bootstrap_fiber[G_NUMBER_OF_PCB]      = {0};
static LPVOID g_fiber_pending_delete[G_NUMBER_OF_PCB] = {0};

/**
 *  \brief  1) Kernel -> Task.  Load the context of the task pointed by to_ptcb
 *          into the current fiber, on the processor thread the task is assigned
 *          to.  The current (kernel) fiber context is NOT saved: it is reset so
 *          that the next time the thread enters kernel mode it runs on a fresh,
 *          zero-initialised stack (gk_ENTRY_IRQ_HANDLER).
 *  \param [in] to_ptcb  TCB whose fiber (to_ptcb->uctx) is loaded and run.
 *
 *  The abandoned kernel fiber's stack is freed (deletion deferred by one swap,
 *  since Windows forbids DeleteFiber on the running fiber).
 *  Uses setcontext() (no save).  This function does not return.
 */
void Swap_kernel_to_task(GS_TCB *to_ptcb)
{
    G_UINT32 CPU_ID  = gk_Get_current_CPUID();
    /* ensure_fiber() (not raw GetCurrentFiber()): on a processor thread's very
     * first kernel->task dispatch, this thread has never been converted to a
     * fiber, and GetCurrentFiber() returns Windows' non-NULL "not a fiber"
     * sentinel instead of a real handle, corrupting g_bootstrap_fiber[] below.
     */
    LPVOID   current = ensure_fiber();


    if (to_ptcb == NULL) {
        printf("WARNING -> %s, %s, %d: PCB_EXECTCB NULL on proc %u, dispatching IDLE\n",
               __FILE__, __FUNCTION__, __LINE__, (unsigned int) CPU_ID);
        to_ptcb = g_kcb.G_PCBTbl[CPU_ID-1].PCB_IDLETCB;
        g_kcb.G_PCBTbl[CPU_ID-1].PCB_EXECTCB = to_ptcb;
    }

    /* Record the bootstrap fiber once; it owns the thread's original stack and
       must never be deleted. */
    if (g_bootstrap_fiber[CPU_ID-1] == NULL) {
        g_bootstrap_fiber[CPU_ID-1] = current;
    }

    /* Free the kernel fiber abandoned on the previous kernel->task swap (it is
       parked and not running now, so DeleteFiber is safe -- see the RESTORED
       note at g_fiber_pending_delete's declaration). */
    {
        LPVOID prev = g_fiber_pending_delete[CPU_ID-1];
        if ((prev != NULL) && (prev != current) && (prev != g_bootstrap_fiber[CPU_ID-1])) {
            DeleteFiber(prev);
        }
        g_fiber_pending_delete[CPU_ID-1] = NULL;
    }

    /* Queue the current kernel fiber for deletion on the next kernel->task swap
       (unless it is the bootstrap fiber, which we keep). */
    if (current != g_bootstrap_fiber[CPU_ID-1]) {
        g_fiber_pending_delete[CPU_ID-1] = current;
    }

    /* Reset the kernel context so the next switch back to it allocates a new
       fiber with a fresh (zero-initialised) stack running gk_ENTRY_IRQ_HANDLER. */
    g_kcb.G_PCBTbl[CPU_ID-1].uctx_main.uc_stack.ss_sp = g_kcb.G_PCBTbl[CPU_ID-1].uctx_main_stack;
    makecontext(&g_kcb.G_PCBTbl[CPU_ID-1].uctx_main, (void (*)()) gk_ENTRY_IRQ_HANDLER, 0);

    /* Load the task context into the current fiber, abandoning (not saving) the
       kernel fiber.  Does not return. */
    setcontext(&to_ptcb->uctx);
}

/**
 *  \brief  2) Task -> Kernel.  Save the current fiber's context into the current
 *          task's TCB (so the task can be resumed later) and load a freshly
 *          initialised kernel context into the current fiber.
 *
 *  The current task is g_kcb.G_PCBTbl[CPU_ID-1].PCB_EXECTCB (gk_PCB_GetCurrentTCB).
 *  Returns to the caller (the suspended task) only when the task is later
 *  resumed by a Swap_kernel_to_task() / Swap_task_to_task() targeting it.
 */
void Swap_task_to_kernel(void)
{
    G_UINT32 CPU_ID    = gk_Get_current_CPUID();
    /* Direct lookup, NOT gk_PCB_GetCurrentTCB(): that wrapper's GK_SAMPLE_FUNCTION_BEGIN
     * instrumentation asserts the mutex is already granted, but this function is called
     * (from GRTOS_Halt_Processor_Wait's interrupt-wake path) precisely when it is NOT --
     * parking/resuming a fiber does not itself require holding the kernel mutex, only
     * actual kernel-data work does (acquired later, inside gk_ENTRY_IRQ_HANDLER itself
     * via GRTOS_CMD_CRITICAL_SECTION_GET). Matches GRTOS_Start_Task's own direct lookup
     * a few lines below for the same reason. Confirmed as the root cause of
     * gk_PCB_GetCurrentTCB's "not granted" assert (listfunctions.c:2078) firing at
     * runtime. */
    GS_TCB  *from_ptcb = g_kcb.G_PCBTbl[CPU_ID-1].PCB_EXECTCB;

    /* fix, revised: this function decides for itself whether a task
     * is genuinely suspending here, instead of trusting the caller (or a proxy
     * register like R_PRC_IRQ_SECTION -- tried first, reverted; see
     * GRTOS_Halt_Processor_Wait()'s own note). from_ptcb (PCB_EXECTCB) is supposed to
     * name "the task currently running" -- when its uctx.fiber_handle is already
     * established (non-NULL, i.e. not this task's first-ever dispatch), that handle
     * must equal the fiber genuinely executing right now, or the premise is false.
     *
     * root-caused exactly this: GRTOS_Halt_Processor_Wait()'s escalation branch
     * (the only caller of this function) can fire from WITHIN
     * gk_ENTRY_IRQ_HANDLER's own GRTOS_CMD_CRITICAL_SECTION_GET retry loop -- the
     * kernel itself, failing to acquire the mutex and halting to wait -- not just from
     * a task's own code (e.g. IDLE's gk_CODE_IDLE_TASK calling gm_ProcessorHalt
     * directly). In the kernel-context case, PCB_EXECTCB still names whichever task
     * was dispatched *before* this kernel entry (typically IDLE, since IDLE is what
     * runs when nothing else is ready), but the fiber physically executing is the
     * transient per-cycle kernel fiber (uctx_main's current incarnation) -- not that
     * task's. Saving into from_ptcb->uctx there corrupts that task's fiber identity:
     * the task's uctx.fiber_handle ends up naming a fiber subject to
     * Swap_kernel_to_task()'s g_fiber_pending_delete cleanup, which has no way to know
     * it also doubles as a task's "real" fiber -- it gets DeleteFiber()'d as
     * "abandoned", Windows later recycles the freed address for an unrelated kernel
     * fiber, and the next dispatch of that task computes
     * to_ptcb->uctx.fiber_handle == current and calls SwitchToFiber() on the fiber
     * already running -- a self-switch the Windows Fiber API does not define, which
     * hangs the OS thread outright. Root-caused via direct CreateFiberEx/SwitchToFiber
     * address tracing across multiple failing runs of 05_message_queue_pubsub.
     *
     * Fix: when the identity check fails, do nothing and return -- there is no task to
     * suspend, so this mirrors real hardware exactly (a synchronous PC stall while
     * already inside the IRQ handler never re-enters it at a fresh vector; it simply
     * continues once unstalled). The caller -- GRTOS_CMD_CRITICAL_SECTION_GET's while
     * loop, still inside gk_ENTRY_IRQ_HANDLER -- re-checks gm_IsPrcGranted(),
     * re-reserves, and re-halts until the mutex is actually won. Once
     * gk_KERNEL_TASK_START() runs, its gm_GetEventOccurrence() call recomputes the
     * event code fresh from current state (ADDR_CTRL_STATUS's IORD case, not the
     * separately-tracked, unused-in-this-path R_LST_EVN field), so the interrupt/
     * timed-event condition that caused this call is not lost -- it is picked up
     * normally on the very next dispatch decision instead of forcing a redundant,
     * identity-corrupting re-entry.
     *
     * First-ever dispatch (from_ptcb->uctx.fiber_handle == NULL) is always safe to
     * proceed: nothing to mismatch against yet, and gu_pthread()'s first statement
     * handles the mutex release correctly regardless. */
    {
        LPVOID actually_running = ensure_fiber();
        G_UINT32 identity_ok = (from_ptcb->uctx.fiber_handle == NULL) ||
                                (from_ptcb->uctx.fiber_handle == actually_running);
        if (!identity_ok) {
            /* FIX: acknowledge the interrupt we are declining to
             * re-enter for. Without this, declining here left R_PRC_INT(cpu) set
             * with no remaining path to ever clear it, and that permanently
             * livelocked the processor. Root-caused from a captured failing run
             * (windows_02_tasks_and_delays_20260810_104139): the tail is an endless
             * "ENTER cpu=2 grn=0 rsv=0x2 hlt_enb=1 winner=0 nested=1" /
             * "RETURN ... loop_iters=0" cycle, whose register values have exactly
             * one solution -- R_PRC_INT(cpu)==1 while R_PRC_IRQ_SECTION(cpu)==0.
             *
             * In that state the processor is blocked BOTH ways, which is why it
             * never escapes:
             *  - it cannot win the mutex, because GRTOS_Compute_Mutex_Next_Processor()
             *    computes eligible = R_MTX_PRC_RSV & ~R_PRC_INT and therefore
             *    excludes it (winner=0, even with the mutex free and reserved by it);
             *    the in_isr branch that would rescue it needs R_PRC_IRQ_SECTION, which
             *    GRTOS_Halt_Processor_Wait() cleared on this processor's previous
             *    successful grant;
             *  - it cannot clear R_PRC_INT, because the ONLY site that clears it is
             *    IOWR's PRC_IRQ_ACK_BIT case, reached only from gm_PrcIRQSecSet() --
             *    gk_ENTRY_IRQ_HANDLER's first statement -- which is precisely the
             *    entry this early return declines to make.
             *
             * An earlier analysis argued nothing is lost here because
             * gk_KERNEL_TASK_START() recomputes the event code fresh. That remains
             * true for the EVENT, and this fix does not disturb it -- but it
             * overlooked the pending FLAG, which besides signalling also gates
             * arbiter eligibility. This is a variant of the very failure the
             * fix at IOWR's PRC_IRQ_ACK_BIT case closed (R_PRC_INT latched
             * forever); reopened that door by making the IRQ-section entry that
             * clears it skippable.
             *
             * Acknowledging is the semantically correct action, not merely a way out:
             * the identity mismatch means we are ALREADY executing inside
             * gk_ENTRY_IRQ_HANDLER further up this stack, so the interrupt genuinely
             * is being serviced and the in-progress handler subsumes this request --
             * exactly as real hardware behaves (an interrupt arriving while already in
             * the handler does not re-enter at a fresh vector). Both register writes
             * mirror IOWR's PRC_IRQ_ACK_BIT case verbatim, and setting
             * R_PRC_IRQ_SECTION is truthful here (we ARE in our IRQ/kernel-entry
             * section; it only reads 0 because the earlier grant cleared it). Setting
             * it also restores the in_isr eligibility branch, and lets an IPI sender
             * spinning on gm_PrcIRQSecGet() (gk_TCBRUNL_Unlink's cross-processor path)
             * observe its acknowledgement.
             *
             * NOTE this SETS a register the reverted first attempt merely READ:
             * that attempt failed because it GATED a decision on R_PRC_IRQ_SECTION
             * while the register had a stale-value leak. Recording a fact about this
             * processor is a different operation from branching on someone else's
             * possibly-stale bit, and the identity check above still decides the swap.
             *
             * Serialized under GeMRTOS_MUTEX_CONDITION_VARIABLE, the lock every other
             * g_controller RMW in this file already uses. */
            EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
            g_controller.R_PRC_IRQ_SECTION |= (0x1u << (CPU_ID - 1));
            g_controller.R_PRC_INT         &= ~(0x1u << (CPU_ID - 1));
            /* The eligible set just changed for everyone; re-evaluate rather than
             * leave other processors parked until their own next timeout. */
            WakeAllConditionVariable(&g_kcb.GeMRTOS_CONDITION_VARIABLE);
            LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
            return;
        }
    }

    /* Prepare a fresh kernel context (zero-initialised stack, gk_ENTRY_IRQ_HANDLER). */
    g_kcb.G_PCBTbl[CPU_ID-1].uctx_main.uc_stack.ss_sp = g_kcb.G_PCBTbl[CPU_ID-1].uctx_main_stack;
    makecontext(&g_kcb.G_PCBTbl[CPU_ID-1].uctx_main, (void (*)()) gk_ENTRY_IRQ_HANDLER, 0);

    /* Save the current task fiber into its TCB and switch to the fresh kernel. */
    swapcontext((ucontext_t *) &from_ptcb->uctx, &g_kcb.G_PCBTbl[CPU_ID-1].uctx_main);
}

/**
 *  \brief  3) Task -> Task.  Save the current fiber's context into the current
 *          task's TCB and load the context of to_ptcb so it resumes execution
 *          from where it was left.  No kernel context is involved.
 *  \param [in] to_ptcb  TCB whose fiber (to_ptcb->uctx) is resumed.
 *
 *  The current task is g_kcb.G_PCBTbl[CPU_ID-1].PCB_EXECTCB (gk_PCB_GetCurrentTCB).
 *  Returns to the caller (the saved task) only when it is later resumed.
 */
void Swap_task_to_task(GS_TCB *to_ptcb)
{
    GS_TCB *from_ptcb = gk_PCB_GetCurrentTCB();

    /* Save the current task fiber into its TCB and resume to_ptcb where it left. */
    swapcontext((ucontext_t *) &from_ptcb->uctx, &to_ptcb->uctx);
}

/* Forward declarations: GRTOS_Check_And_Escalate_Timed_Event() is defined near
 * GRTOS_Halt_Processor_Wait() below and needs get_elapsed_ticks_10MHz(), itself
 * defined later in this file. */
G_UINT64 get_elapsed_ticks_10MHz(struct timespec *SYSTEM_STARTING_TIME);
static G_UINT32 GRTOS_Check_And_Escalate_Timed_Event(void);
static void GRTOS_Check_Frozen_Mode(void);
/* same reason -- GRTOS_Compute_Mutex_Next_Processor() is defined near
 * GRTOS_Halt_Processor_Wait() below, but GRTOS_Start_Task()'s release path (just below)
 * needs to call it too. */
static G_UINT32 GRTOS_Compute_Mutex_Next_Processor(void);

/* FIXED -- confirmed root cause of the message-queue hang (gemrtos_mq.c:238's
 * receiver-state assert): the GeMRTOS controller makes "the dispatched task
 * begins executing" atomic with "the mutex is released" -- on real hardware
 * this is a single instruction's side effect (reading the eret code). On this
 * port, GRTOS_Start_Task() used to LeaveCriticalSection+WakeAllConditionVariable
 * BEFORE Swap_kernel_to_task() below, whose setcontext() is a one-way transfer
 * that does not return here -- so between the release and the dispatched task
 * actually running its own first instruction, another OS thread woken by
 * WakeAllConditionVariable could acquire the mutex and run kernel code (e.g.
 * gu_MessageQueueSend()'s receiver scan) against a task that had been marked
 * RUNNING in bookkeeping but had not yet executed anything. Reproduced directly:
 * a receiver task's TCBState read as RUNNING (170004, ENUM_CODE(13,4), decoded
 * and confirmed, not garbage) while its ECBType was still WAIT -- exactly this
 * window.
 *
 * Fix: defer the release to the two places execution PROVABLY resumes inside
 * the dispatched task, instead of releasing here on the dispatching side --
 * swapcontext()'s post-SwitchToFiber return (grtos_fiber.c; every resume of a
 * previously-suspended task, confirmed by audit to be the sole return point
 * for all three suspend paths: GRTOS_Suspend_Task, Swap_task_to_kernel,
 * GRTOS_Halt_Processor) and gu_pthread()'s first statement (every task's
 * first-ever dispatch, via gk_TASK_STK_INIT's makecontext(...,gu_pthread,...)).
 * Thread-local: the OS thread that calls GRTOS_Start_Task() is the same one
 * that will execute whichever of the two resume points comes next
 * (Swap_kernel_to_task -> setcontext -> SwitchToFiber transfers control
 * synchronously on the calling thread) -- mirrors grtos_fiber.c's own existing
 * deferred-publish pattern for exactly the same reason. */
static __thread G_UINT32 g_pending_kernel_mutex_release = 0;

void GRTOS_ReleaseKernelMutexIfPending(void)
{
    if (!g_pending_kernel_mutex_release) return;
    g_pending_kernel_mutex_release = 0;

    /* Identical body to what GRTOS_Start_Task() used to run directly -- see
     * its own comments for why the CONDITION_VARIABLE lock
     * scope covers the register-file writes too, and why R_HLT_IDL_PRC_ENB is
     * narrowed to just the round-robin winner. Unchanged; only the call site
     * moved. */
    EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
    if ((void *) GetCurrentThreadId() ==
        (void *) g_kcb.GeMRTOS_MUTEX_section.OwningThread) {
        LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_section);
        GRTOS_RUNPRINTF("RUNNING -> %s, %s, %d, g_controller.R_MTX_PRC_GRN= 0x%x \n", __FILE__, __FUNCTION__, __LINE__, g_controller.R_MTX_PRC_GRN);
        g_controller.R_MTX_PRC_GRN = 0;
        {
            G_UINT32 next_prc = GRTOS_Compute_Mutex_Next_Processor();
            if (next_prc != 0) {
                g_controller.R_HLT_IDL_PRC_ENB &= ~(0x1u << (next_prc - 1));
            }
        }
    }
    WakeAllConditionVariable(&g_kcb.GeMRTOS_CONDITION_VARIABLE);
    LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
}

void GRTOS_Start_Task(void)
{

	PRINT_ASSERT(((gm_IsPrcGranted())), "ERROR in IDLE task\n");

	/* Eager check -- if the deadline already passed, wake any other
	 * halted processor now instead of leaving it to its own poll. */
	(void) GRTOS_Check_And_Escalate_Timed_Event();
	GRTOS_Check_Frozen_Mode();

    GRTOS_RUNPRINTF("RUNNING -> %s, %s, %d \n", __FILE__, __FUNCTION__, __LINE__);

	G_UINT32 CPU_ID = gk_Get_current_CPUID();
	GS_TCB *ptcb = g_kcb.G_PCBTbl[CPU_ID-1].PCB_EXECTCB;

	// Execute SIGNAL but in critical section
    PRINT_ASSERT((g_controller.R_MTX_PRC_GRN == gm_ProcessorId()),"ERROR in critical section= %d, proc=%d\n", (int) g_controller.R_MTX_PRC_GRN, (int) gm_ProcessorId());
    
	gk_TCB_GetPendingSCB();

	while (G_SCB_PENDING != 0)
	{
		void  *(*funcion)(void *) = (void*) G_SCB_CODE;
		(funcion)((void *) G_SCB_ARG);
		gk_ENTRY_SIGNAL_RETURN();
		gk_TCB_GetPendingSCB();
	}

	// Return [G_SCB_PENDING] = 1 if SCB pending, 0 otherwise
    //		  [G_SCB_CODE]    = SCB.SCB_TaskCode + 4
	//		  [G_SCB_ARG]     = SCB.SCB_TaskArg
	//		  Return_address  = GRTOS_Return_from_Signal

    if ((g_controller.R1_MTX_NESTED == 0) && (g_controller.R1_KERNEL_MODE == 0)) {
        /* FIXED: do NOT release the mutex here -- see
         * GRTOS_ReleaseKernelMutexIfPending()'s own comment above. The
         * decision to release is still made here (same
         * condition, unchanged); only the ACTION is deferred to the point
         * where the dispatched task's own code provably starts running. */
        g_pending_kernel_mutex_release = 1;
    }

	if (g_controller.R_PRC_RST < CPU_ID) {
		g_controller.R_PRC_RST = CPU_ID;
	}

    /* FIXED: clear this processor's mutex reservation before dispatching
     * the next task. A reservation (R_MTX_PRC_RSV[cpu], set by gm_MutexReserveSet inside
     * GRTOS_CMD_CRITICAL_SECTION_GET) is only meaningful for the DURATION of the acquire
     * attempt that set it; it must not survive into a different task that starts running
     * on this processor. Without this, a reservation left set by the kernel's own
     * CRITICAL_SECTION_GET (which runs while PCB_EXECTCB still points at the interrupted
     * IDLE task) leaks into IDLE's next gm_ProcessorHalt -- IDLE, which reserves nothing,
     * then satisfies GRTOS_HALT_MUTEX_AVAILABLE_TO_ME and wins the mutex it must never
     * hold ("ERROR in IDLE task" assert; freeze when the other processor then blocks on
     * it). Proven via the [MTX] IDLE-ACQ trace (mtx_trace.txt). Clearing here is safe: the
     * dispatched task, if it needs the mutex, re-reserves fresh in its own
     * GRTOS_CMD_CRITICAL_SECTION_GET loop; and a task suspended mid-reservation re-reserves
     * when it is later resumed. This is the root-cause fix (gating IDLE out of acquisition
     * instead livelocked -- see GRTOS_HALT_MUTEX_AVAILABLE_TO_ME's comment). */
    /* FIXED: same register-file race as above -- this RMW must be
     * serialized against GRTOS_Halt_Processor_Wait's and IOWR's writes to R_MTX_PRC_RSV. */
    EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
    g_controller.R_MTX_PRC_RSV &= ~(0x1u << (CPU_ID - 1));
    LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);

    /* Kernel -> Task: load the task context, free the abandoned kernel stack;
     * the next return to kernel mode runs on a fresh zero-initialised stack.
     */
    Swap_kernel_to_task(ptcb);

}

void GRTOS_Suspend_Task(void)
{
	printf("GRTOS_Suspend_Task");

	G_INT32 CPU_ID = gk_Get_current_CPUID();
    /* FIXED: do NOT re-derive "the task being suspended" via
     * gk_PCB_GetCurrentTCB() (== g_kcb.G_PCBTbl[CPU_ID-1].PCB_EXECTCB) here. The only
     * caller, gk_KERNEL_TASK_SUSPEND_CURRENT (grtos_kernel.c), already called
     * gk_TCBRUNL_Unlink(ptcbfrom) on the real suspending task BEFORE calling this
     * function -- and that unlink's own documented side effect ("Desassociate the task
     * from the processor", listfunctions.c) resets PCB_EXECTCB to PCB_IDLETCB. So
     * gk_PCB_GetCurrentTCB() here returns IDLE, not the task actually being suspended --
     * and the swapcontext below would then save the suspending task's live register
     * state into IDLE's own uctx (ptcbfrom->uctx), corrupting it. The next time IDLE is
     * dispatched, setcontext(&idle->uctx) resumes execution back inside the ORIGINAL
     * suspending task's call stack (mid this same function's caller), whose
     * PRINT_ASSERT(gm_IsPrcGranted()) then fails -- the mutex having meanwhile been
     * released based on IDLE's own (irrelevant) nesting state -- freezing that
     * processor's thread in G_DEBUG_WHILEFOREVER and corrupting kernel state, observed
     * as a crash shortly after. Root-caused via temporary [DBG] tracing with
     * INCLUDE_TASK_INFINITE_LOOP=1 (task_infinite_loop is the first-ever caller of
     * gu_TaskDelay -- and hence gk_KERNEL_TASK_SUSPEND_CURRENT -- in any build tested
     * any build tested; task_periodic never suspends this way, only ends/is retriggered).
     * The real NiosX/assembly GRTOS_Suspend_Task (grtos_switch_niosV.S,
     * grtos_switch_nios2.S) never hits this: it locates the suspending task via
     * PCB_SUSPENDED_STK_PTR/gs_offsetof_TCB_StackPointer, which
     * gk_KERNEL_TASK_SUSPEND_CURRENT sets from the correct TCB BEFORE the unlink --
     * never re-deriving "current task" from PCB_EXECTCB afterward. This mirrors that
     * same portable mechanism instead of re-deriving via PCB_EXECTCB, without touching
     * the shared caller or the real hardware's assembly. */
    GS_TCB *ptcbfrom = (GS_TCB *) ((char *) g_kcb.G_PCBTbl[CPU_ID-1].PCB_SUSPENDED_STK_PTR
                                    - gs_offsetof_TCB_StackPointer);

    /* getcontext(&ptcbfrom->uctx) was redundant — swapcontext saves oucp->fiber_handle internally.
     * getcontext(&uctx_main) was also redundant — makecontext clears fiber_handle anyway.
     */
	getcontext(&g_kcb.G_PCBTbl[CPU_ID-1].uctx_main);
	g_kcb.G_PCBTbl[CPU_ID-1].uctx_main.uc_stack.ss_sp = g_kcb.G_PCBTbl[CPU_ID-1].uctx_main_stack;
	makecontext(&g_kcb.G_PCBTbl[CPU_ID-1].uctx_main, (void (*)()) gk_ENTRY_IRQ_HANDLER, 0);
	swapcontext((ucontext_t *) &ptcbfrom->uctx, &g_kcb.G_PCBTbl[CPU_ID-1].uctx_main);

}

G_UINT64 Convert_time_to_ticks(struct timespec *timespect)
{
    G_UINT64 delta_ns = (G_UINT64)(timespect->tv_sec  - g_controller.SYSTEM_STARTING_TIME.tv_sec)  * 1000000000ULL
                       + (G_UINT64)(timespect->tv_nsec) - (G_UINT64)(g_controller.SYSTEM_STARTING_TIME.tv_nsec);
    return delta_ns / 100ULL;  /* 10 MHz = 100 ns/tick */
}

void GRTOS_Update_controler_status(void)
{

}

void GRTOS_Halt_Processor(void)
{
	G_INT32 CPU_ID = gk_Get_current_CPUID();

	GS_TCB *ptcb = g_kcb.G_PCBTbl[CPU_ID-1].PCB_EXECTCB;

	struct timespec end;
	G_UINT64 current_time;
	G_UINT64 ticks;

	clock_gettime(CLOCK_MONOTONIC, &end);
	// Put time in ms
	G_UINT64 starting_time = (G_UINT64) g_controller.SYSTEM_STARTING_TIME.tv_sec * 1000 + (G_UINT64) g_controller.SYSTEM_STARTING_TIME.tv_nsec / 1000000;
	current_time = ((G_UINT64) end.tv_sec * 1000 + (G_UINT64) end.tv_nsec / 1000000) - starting_time;
	G_UINT64 ms = current_time % 1000;
	G_UINT64 seconds = (G_UINT64) ((current_time / 1000) % 60);
	G_UINT64 minutes = (G_UINT64) ((current_time / 60000) % 60);
	G_UINT64 hours = (G_UINT64) ((current_time / 3600000 ));
	ticks = gu_ConvertTime((G_INT32) hours, (G_INT32) minutes, (G_INT32) seconds, (G_INT32) ms);

	// G_INT32 ticks_ms = gu_Convert_time((G_INT32) 0, (G_INT32) 0, (G_INT32) 0, (G_INT32) 1);
	while ((ticks < g_controller.R_NXT_EVN_CNT.i64) && (((g_controller.R_INT_RQS >> (CPU_ID-1)) & 1 ) == 0 )) {
	 	clock_gettime(CLOCK_MONOTONIC, &end);
		current_time = ((G_UINT64) end.tv_sec * 1000 + (G_UINT64) end.tv_nsec / 1000000) - starting_time;
		ms = current_time % 1000;
		seconds = (G_INT32) ((current_time / 1000) % 60);
		minutes = (G_INT32) ((current_time / 60000) % 60);
		hours = (G_INT32) ((current_time / 3600000 ));
		ticks = gu_ConvertTime((G_INT32) hours, (G_INT32) minutes, (G_INT32) seconds, (G_INT32) ms);
	}

    /* Determine why the spin loop exited and set R_LST_EVN accordingly so that
       gm_GetEventOccurrence() (which reads ADDR_CTRL_STATUS & 0xFF = R_LST_EVN)
       returns the correct event code to gk_KERNEL_TASK_START. */
    if (g_controller.C1_FRZ_EVN != 0) {
        g_controller.R_LST_EVN = EVN_CODE_FROZEN;
    } else if (ticks >= g_controller.R_NXT_EVN_CNT.i64) {
        g_controller.R_LST_EVN = EVN_CODE_TIMED;
    } else {
        g_controller.R_LST_EVN = 0;
        for (int j = 31; j >= 0; j--) {
            if (((g_controller.IRQ_EVENT_PENDING >> j) & 1) == 1) {
                g_controller.R_LST_EVN = (G_INT32) j;
            }
        }
    }
	getcontext(&g_kcb.G_PCBTbl[CPU_ID-1].uctx_main);
	g_kcb.G_PCBTbl[CPU_ID-1].uctx_main.uc_stack.ss_sp = g_kcb.G_PCBTbl[CPU_ID-1].uctx_main_stack;
	makecontext(&g_kcb.G_PCBTbl[CPU_ID-1].uctx_main, (void (*)()) gk_ENTRY_IRQ_HANDLER, 0);
	swapcontext(&ptcb->uctx, &g_kcb.G_PCBTbl[CPU_ID-1].uctx_main);
}

/* SleepConditionVariableCS timeout for GRTOS_Halt_Processor_Wait: bounds the worst-case
 * latency of a missed wake (e.g. R_PRC_INT set by a future caller that does not also
 * call WakeAllConditionVariable) by forcing a periodic re-check of the wake conditions.
 */
#define GRTOS_HALT_WAIT_TIMEOUT_MS 10

/* How long a timed-event deadline may stay unserviced by its OWN
 * designated processor before any awake processor may take it over. See the
 * takeover block in GRTOS_Halt_Processor_Wait() for the full rationale.
 *
 * Derived from measurement, not chosen round: normal periodic-release service
 * lag on this tier measures 2.6-40.8 ms across a full run, so 100 ms sits at
 * ~2.5x the worst normal case -- comfortably outside legitimate operation, so
 * ownership is never stolen from a processor that is merely busy. It is also
 * well inside the ~600 ms window in which the pathological case persists, so a
 * genuinely stranded deadline is picked up long before its owner returns. */
#define GRTOS_TIMED_EVENT_TAKEOVER_MS 100u

/**
 *  \brief  Eagerly checks whether the next timed-event deadline has already passed and,
 *          if so, wakes every processor parked in GRTOS_Halt_Processor_Wait() right now
 *          instead of leaving each to discover it on its own next
 *          GRTOS_HALT_WAIT_TIMEOUT_MS poll. Mirrors VHDL's continuous (every-clock-cycle)
 *          combinational evaluation of C_TIMED_INT -- approximated here by calling this
 *          from IORD()/IOWR() (i.e. every gm_IORD/gm_IOWR in the whole codebase,
 *          grtosdefinitions.h:203-205) and from GRTOS_Start_Task(), which between them
 *          cover virtually all system activity.
 *          Cheap in the common (not-yet-due) case: one 64-bit compare, no lock taken.
 *  \return G_TRUE if the deadline had passed at the time of the call, G_FALSE otherwise.
 */
static G_UINT32 GRTOS_Check_And_Escalate_Timed_Event(void)
{
	/* Debounced: a deadline stays "overdue" (ticks_now >= R_NXT_EVN_CNT) for the
	 * ENTIRE time it takes the kernel to resolve it, and gm_IORD/gm_IOWR fire
	 * hundreds of times per dispatch cycle (every GK_SAMPLE_FUNCTION_BEGIN/END pair
	 * alone calls gk_SystemTimeGet() -> gm_IORD twice). Without debouncing, this
	 * function would call EnterCriticalSection+WakeAllConditionVariable+Leave on
	 * EVERY one of those calls -- confirmed via [GKTS]/[GKPR] instrumentation to
	 * cost tens-to-hundreds of ms of real cross-thread wake/scheduling overhead
	 * PER CALL, accumulating into the ~1.5-2s stalls this stage was meant to fix.
	 * Only actually wake once per distinct R_NXT_EVN_CNT value (i.e. once when a
	 * deadline first becomes due), not on every subsequent poll while it's still
	 * the same overdue deadline. It still returns G_TRUE every time so callers (e.g.
	 * GRTOS_Halt_Processor_Wait's wait loop) correctly see "due" regardless.
	 */
	static G_UINT64 last_escalated_deadline = 0;
	G_UINT64 ticks_now = get_elapsed_ticks_10MHz(&g_controller.SYSTEM_STARTING_TIME);
	if (ticks_now < g_controller.R_NXT_EVN_CNT.i64) {
		return G_FALSE;
	}
	if (g_controller.R_NXT_EVN_CNT.i64 != last_escalated_deadline) {
		last_escalated_deadline = g_controller.R_NXT_EVN_CNT.i64;
		EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
		WakeAllConditionVariable(&g_kcb.GeMRTOS_CONDITION_VARIABLE);
		LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
	}
	return G_TRUE;
}

/**
 *  \brief  Deadline-miss watchdog, detect-and-report only (no scheduling pause):
 *          computes the C1_FRZ_TM_OCC deadline-miss watchdog trigger
 *          (grtos.vhd:1728-1743: fires when a scheduled deadline
 *          (R_NXT_EVN_CNT) is overdue by more than the configured grace period
 *          (R_FRZ_TM_THR) while the GeMRTOS mutex is currently held -- a stuck/
 *          livelocked-processor detector, not a generic timer; hysteresis clears it
 *          once a fresh, not-yet-due deadline is set), fires the C1_FRZ_EVN/
 *          C1_UNFRZ_EVN edge pulses on transition (grtos.vhd:1702-1725) -- which
 *          feeds the ALREADY-LIVE EVN_CODE_FROZEN readback in IORD()'s event-code
 *          logic above -- and accumulates R_FRZ_TM_ELP (this sim's stand-in for
 *          R_FRZ_TM_CNT, grtos.vhd:1245-1250) for ADDR_FRZ_TM_HGH.
 *          Deliberately does NOT pause R_TM_CNT or any scheduling/escalation logic
 *          elsewhere in this file -- that would be full hardware fidelity (rejected
 *          as too invasive to the already-stabilized halt/escalation paths). The
 *          watchdog condition is genuinely detected and reported; it just doesn't
 *          also freeze the clock everything else reads.
 *          Cheap in the common (never-triggered) case: a few unlocked compares, no
 *          lock taken, same debouncing principle as
 *          GRTOS_Check_And_Escalate_Timed_Event() above. Only enters the lock on an
 *          actual 0<->1 transition or while genuinely in the (abnormal, expected-rare)
 *          triggered state -- an already-degraded condition where the extra lock cost
 *          is an acceptable trade for correctness, unlike the routine-deadline hot
 *          path above.
 */
static void GRTOS_Check_Frozen_Mode(void)
{
	static G_UINT64 last_accum_ticks = 0;

	if (g_controller.C1_FRZ_MDE_ENB == 0) {
		if (g_controller.C1_FRZ_TM_OCC != 0) {
			EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
			g_controller.C1_FRZ_TM_OCC = 0;
			LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
		}
		last_accum_ticks = 0;
		return;
	}

	G_UINT64 ticks_now    = get_elapsed_ticks_10MHz(&g_controller.SYSTEM_STARTING_TIME);
	G_UINT64 frz_deadline = g_controller.R_NXT_EVN_CNT.i64 + g_controller.R_FRZ_TM_THR.i64;
	G_UINT32 mutex_granted = (g_controller.R_MTX_PRC_GRN != 0);
	G_UINT32 cur_occ = g_controller.C1_FRZ_TM_OCC;
	G_UINT32 new_occ = cur_occ; /* hysteresis: hold unless a transition condition fires */

	if ((ticks_now > frz_deadline) && mutex_granted) {
		new_occ = 1;
	} else if ((ticks_now < g_controller.R_NXT_EVN_CNT.i64) && mutex_granted) {
		new_occ = 0;
	}

	if ((new_occ == cur_occ) && (new_occ == 0)) {
		last_accum_ticks = 0;
		return;   /* common case: never triggered -- no lock taken */
	}

	EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
	if ((new_occ == 1) && (g_controller.C1_FRZ_TM_OCC == 0)) {
		g_controller.C1_FRZ_EVN = 1;                /* rising edge */
	}
	if ((new_occ == 0) && (g_controller.C1_FRZ_TM_OCC == 1)) {
		g_controller.C1_UNFRZ_EVN = 1;               /* falling edge */
	}
	g_controller.C1_FRZ_TM_OCC = new_occ;
	if ((new_occ == 1) && (g_controller.R1_HLD_CNT_RST == 0)) {
		if ((last_accum_ticks != 0) && (ticks_now > last_accum_ticks)) {
			g_controller.R_FRZ_TM_ELP.i64 += (ticks_now - last_accum_ticks);
		}
		last_accum_ticks = ticks_now;
	} else {
		last_accum_ticks = 0;
	}
	LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
}

/* floor for the dynamic sleep timeout below -- bounds only the low end
 * (avoids a ~0 ms timeout degenerating into a busy-poll when a deadline is imminent or
 * already past); replaces GRTOS_HALT_WAIT_TIMEOUT_MS's old role as a fixed poll period.
 * No upper cap is needed: every other wake reason still uses WakeAllConditionVariable()
 * to wake sleepers early regardless of this timeout, so a longer computed sleep only
 * means less wasted polling when nothing is due soon. */
#define GRTOS_HALT_MIN_TIMEOUT_MS 1
/* get_elapsed_ticks_10MHz() ticks are 100 ns each (10 MHz) -> 10,000 ticks per ms. */
#define GRTOS_TICKS_PER_MS 10000ULL

/**
 *  \brief  computes how long GRTOS_Halt_Processor_Wait() should sleep before its
 *          next forced re-check, from how far away the next timed-event deadline
 *          (R_NXT_EVN_CNT) actually is -- replaces the previous fixed
 *          GRTOS_HALT_WAIT_TIMEOUT_MS poll interval with a hardware-accurate one (matches
 *          register map §5.4's dynamic-timeout note).
 */
static DWORD GRTOS_Compute_Halt_Sleep_Timeout_MS(void)
{
	G_UINT64 ticks_now = get_elapsed_ticks_10MHz(&g_controller.SYSTEM_STARTING_TIME);
	G_UINT64 deadline  = g_controller.R_NXT_EVN_CNT.i64;
	if (deadline <= ticks_now) {
		return GRTOS_HALT_MIN_TIMEOUT_MS;
	}
	G_UINT64 remaining_ms = (deadline - ticks_now) / GRTOS_TICKS_PER_MS;
	DWORD result = (remaining_ms < GRTOS_HALT_MIN_TIMEOUT_MS)
	       ? GRTOS_HALT_MIN_TIMEOUT_MS : (DWORD) remaining_ms;
	return result;
}

/**
 * round-robin mutex arbiter -- lowest-index priority over the currently
 * eligible subset of reserving processors, mirroring VHDL's
 * ROTATING_PRIORITY_FOR_MUTEX_RESERVATION (grtos.vhd:1286-1295 -- despite the name this
 * is fixed lowest-index priority, not true rotation; confirmed as the wanted behavior),
 * fed by R_WAITING_IN_ISR (grtos.vhd:1270-1271). Recomputed fresh from the current
 * R_MTX_PRC_RSV/R_PRC_IRQ_SECTION/R_PRC_INT every time it's called (mirrors VHDL's
 * combinational, every-cycle recomputation) instead of hooking every one of this file's
 * many mutation sites of those three fields to keep a cached value in sync.
 *
 * NOTE on naming vs VHDL: VHDL's R_MTX_PRC_RSV is a one-clock-cycle request pulse,
 * aggregated separately into a persistent R_MTX_RSV_PRC (grtos.vhd:602-603,1273-1281)
 * that the real arbiter reads. This Windows sim has no per-cycle semantics -- its
 * R_MTX_PRC_RSV already plays the persistent-aggregate role directly (set on
 * reservation, held until GRTOS_Halt_Processor_Wait consumes it -- see the "ONE-
 * halt-cycle signal" comment there), so this function reads it as that aggregate.
 * Deliberately NOT adding a second field named R_MTX_RSV_PRC: it would duplicate this
 * one under a near-identical VHDL name and invite exactly the kind of same-named-but-
 * different-symbol confusion already caught once here (g_controller.R_IRQ_ENB
 * vs the header's dead standalone extern R_IRQ_ENB).
 *
 * Updates g_controller.R_WAITING_IN_ISR/R_MTX_NXT_PRC as a side effect, purely for
 * observability (ADDR_WAITING_IN_ISR testing register) -- callers needing the winner
 * should use the return value; those fields are last-computed snapshots, not
 * authoritative state, and may be read/written without the lock elsewhere (same
 * accepted tolerance as other debug mirrors in this file).
 */
static G_UINT32 GRTOS_Compute_Mutex_Next_Processor(void)
{
	G_UINT32 in_isr   = g_controller.R_MTX_PRC_RSV & g_controller.R_PRC_IRQ_SECTION;
	G_UINT32 eligible = (in_isr != 0) ? in_isr
	                    : (g_controller.R_MTX_PRC_RSV & ~g_controller.R_PRC_INT);
	G_UINT32 winner = 0;
	unsigned int i;

	for (i = 1; i <= (unsigned int) GEMRTOS_DRIVER_NPROCESSORS; i++) {
		if ((eligible >> (i - 1)) & 1) {
			winner = i;
			break;
		}
	}

	g_controller.R_WAITING_IN_ISR = eligible;
	g_controller.R_MTX_NXT_PRC = winner;
	return winner;
}

/**
 *  \brief  Halt-wait primitive.  Emulates the Avalon bus stall
 *          (slave_processor_waitrequestX) the real GeMRTOS controller asserts to halt
 *          a processor: parks the current thread on GeMRTOS_CONDITION_VARIABLE while
 *          R_HLT_IDL_PRC_ENB(CPU_ID) is set, matching VHDL's
 *          R_HLT_IDL_PRC_ENB <= (R_HLT_IDL_PRC_ENB or R_PRC_IDL_ENB) and
 *          not(R_PRC_INT or R_MTX_PRC_GRN).  The latch is cleared (unblocking) at every
 *          site that sets R_PRC_INT for this processor or grants it the mutex (see
 *          IOWR's ADDR_REG_1 release case and CTRL_TRG_INT_PRC_BIT case).
 *
 *  On wake, exactly one of two things happened:
 *   (a) The mutex is free or already granted to us -> race to acquire
 *       GeMRTOS_MUTEX_section.  Losing the race to another processor is still possible
 *       in principle (the latch is cleared for *all* mutex-waiters on release, not just
 *       the designated one -- see the release-path comment in IOWR's ADDR_REG_1 case),
 *       but the arbiter (GRTOS_Compute_Mutex_Next_Processor(), gated via
 *       GRTOS_HALT_MUTEX_AVAILABLE_TO_ME below) means only the designated winner ever
 *       reaches this TryEnterCriticalSection in the first place, so in practice this is
 *       now uncontended. Losing it anyway is still not an error: the caller's own retry loop
 *       (GRTOS_CMD_CRITICAL_SECTION_GET) re-arms via gm_MutexReserveSet()+
 *       gm_ProcessorHalt and waits again.
 *   (b) R_PRC_INT(CPU_ID) is set, or the next timed event has occurred (discovered on
 *       a SleepConditionVariableCS timeout) -> swap into the IRQ handler via
 *       Swap_task_to_kernel(), which does not return until later resumed.
 */
void GRTOS_Halt_Processor_Wait(void)
{
	G_UINT32 CPU_ID = gk_Get_current_CPUID();
	int timed_event_due = G_FALSE;

	/* "Mutex available to me" is only ever a real wake reason for a processor that
	 * actually reserved it (gm_MutexReserveSet(), via GRTOS_CMD_CRITICAL_SECTION_GET) --
	 * mirrors VHDL, where only R_MTX_RSV_PRC waiters are ever granted, and only
	 * the one the round-robin arbiter designates via
	 * GRTOS_Compute_Mutex_Next_Processor(). A plain
	 * gm_ProcessorHalt() caller (e.g. IDLE, no reservation) must never opportunistically
	 * grab a mutex it never asked for, nor treat the mutex simply being free as a reason
	 * to stop waiting -- confirmed as the root cause of IDLE failing its own
	 * "not granted" invariant (gk_CODE_IDLE_TASK, grtos_kernel.c:1022) at
	 * runtime.
	 *
	 * Two attempts to GATE IDLE out of acquisition here (PCB_EXECTCB==IDLE,
	 * then also gated on R_PRC_IRQ_SECTION) both LIVELOCKED -- legitimate non-IRQ-handler
	 * acquisitions that run while PCB_EXECTCB happens to be IDLE (e.g. gk_START_KERNEL's
	 * init-time CRITICAL_SECTION_GET) got blocked, so no processor could enter the kernel
	 * and the periodic task never ran (both cores spinning). PCB_EXECTCB/R_PRC_IRQ_SECTION
	 * do NOT cleanly separate "IDLE-the-task halting" from "legitimate code acquiring while
	 * exec==IDLE". The real fix is elsewhere: the leaked reservation bit is cleared at task
	 * DISPATCH (see GRTOS_Start_Task, before Swap_kernel_to_task) so a freshly-dispatched
	 * IDLE never carries a stale reservation into its halt. This predicate is back to the
	 * plain reservation+grant gate. */
/* added the third conjunct (GRTOS_Compute_Mutex_Next_Processor() == cpu)
 * -- previously any reserving processor with the mutex free/already-ours counted as
 * "available", free-for-all style; now only the round-robin-designated processor does.
 * Evaluated both inside the while loop below (under GeMRTOS_MUTEX_CONDITION_VARIABLE,
 * same as the other fields it reads) and again at the post-loop entry check further
 * down (lock-free there, same pre-existing tolerance as that second check already had
 * for R_MTX_PRC_RSV/R_MTX_PRC_GRN -- the actual grant is still serialized under lock).
 * A non-designated reserver now correctly keeps sleeping here instead of racing
 * TryEnterCriticalSection and losing; it re-checks on every wake exactly as before. */
#define GRTOS_HALT_MUTEX_AVAILABLE_TO_ME(cpu) \
	(((g_controller.R_MTX_PRC_RSV >> ((cpu) - 1)) & 1) && \
	 ((g_controller.R_MTX_PRC_GRN == (cpu)) || (g_controller.R_MTX_PRC_GRN == 0)) && \
	 (GRTOS_Compute_Mutex_Next_Processor() == (cpu)))

	EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
	/* SAFETY NET (kept after the fix -- not temporary).
	 *
	 * Detects the exact pathological state that produced that livelock and
	 * reports it once, loudly, instead of letting this function spin silently
	 * forever. The predicate is provably abnormal rather than merely unusual:
	 * the mutex is FREE (R_MTX_PRC_GRN == 0), this processor has RESERVED it,
	 * and yet the arbiter designates NOBODY (winner == 0). A free mutex with a
	 * waiting reserver and no eligible winner means no processor can make
	 * progress through this path -- there is no legitimate steady state in
	 * which that holds for any length of time.
	 *
	 * Rationale for keeping it permanently: this function has a documented
	 * history of regressing into livelocks from well-intentioned changes (two
	 * prior IDLE-gating attempts, an earlier R_PRC_IRQ_SECTION attempt, and
	 * the livelock this net detects), and every one presented as a SILENT hang that cost
	 * a full investigation to even localise. Naming the state at the moment it
	 * occurs turns that class of failure from "no output, unknown cause" into a
	 * single line pointing at the responsible registers.
	 *
	 * Detect-and-report only: it deliberately does not alter control flow, so it
	 * cannot itself introduce the kind of regression it exists to catch.
	 *
	 * State is per-PROCESSOR (indexed by CPU_ID), never per-OS-thread: a fiber
	 * resuming after task migration can return here on a different thread, which
	 * is exactly why a __thread nesting counter was rejected here.
	 * Reads/writes are already serialized -- the caller holds
	 * GeMRTOS_MUTEX_CONDITION_VARIABLE across this block. */
	{
		static G_UINT32 halt_wedged_count[GEMRTOS_DRIVER_NPROCESSORS];
		static G_UINT32 halt_wedged_reported[GEMRTOS_DRIVER_NPROCESSORS];
		const G_UINT32 halt_wedged_threshold = 1000u;

		if ((CPU_ID >= 1) && (CPU_ID <= (G_UINT32) GEMRTOS_DRIVER_NPROCESSORS)) {
			G_UINT32 idx = CPU_ID - 1;
			G_UINT32 wedged = ((g_controller.R_MTX_PRC_GRN == 0) &&
			                   (((g_controller.R_MTX_PRC_RSV >> idx) & 1) != 0) &&
			                   (GRTOS_Compute_Mutex_Next_Processor() == 0)) ? 1u : 0u;
			if (wedged != 0u) {
				halt_wedged_count[idx]++;
				if ((halt_wedged_count[idx] >= halt_wedged_threshold) &&
				    (halt_wedged_reported[idx] == 0u)) {
					halt_wedged_reported[idx] = 1u;
					fprintf(stderr,
					        "GRTOS_Halt_Processor_Wait: WEDGED cpu=%u for %u consecutive entries "
					        "(mutex free but no eligible winner) grn=%u rsv=0x%x irq_sec=0x%x "
					        "prc_int=0x%x hlt_enb=0x%x\n",
					        (unsigned int) CPU_ID, (unsigned int) halt_wedged_count[idx],
					        (unsigned int) g_controller.R_MTX_PRC_GRN,
					        (unsigned int) g_controller.R_MTX_PRC_RSV,
					        (unsigned int) g_controller.R_PRC_IRQ_SECTION,
					        (unsigned int) g_controller.R_PRC_INT,
					        (unsigned int) g_controller.R_HLT_IDL_PRC_ENB);
				}
			} else {
				halt_wedged_count[idx]    = 0u;
				halt_wedged_reported[idx] = 0u;
			}
		}
	}
	/* R_HLT_IDL_PRC_ENB(me) is the primary gate, but it is only ever cleared by an
	 * explicit release/interrupt event (see the write sites above). The very first
	 * time a mutex is acquired -- nobody has ever held it, so nobody will ever
	 * "release" it -- there is no such event, matching VHDL's arbiter which grants
	 * immediately when R_MTX_PRC_GRN = 0 and this processor is the one the
	 * arbiter designates. So also exit as soon as the mutex is available to us (only
	 * meaningful if we reserved it AND we're the designated winner -- see above), or an
	 * interrupt targets us, exactly like before this latch existed; confirmed necessary
	 * by a real deadlock on first boot (proc 1's very first, uncontended critical-section
	 * acquire) at runtime. */
	/* Additive only -- two boundary markers around this loop, for the per-processor
	 * watchdog monitor (GRTOS_Watchdog_Monitor, ANALYSIS-processor-watchdog-reset.md):
	 * this is the Windows equivalent of "waitrequest asserted", the only place in this
	 * simulation where a processor is genuinely not executing. Nothing inside the loop's
	 * own wake-condition logic below is touched. */
	g_controller.R_PRC_HLT |= (0x1u << (CPU_ID - 1));
	while (((g_controller.R_HLT_IDL_PRC_ENB >> (CPU_ID - 1)) & 1) &&
	       !GRTOS_HALT_MUTEX_AVAILABLE_TO_ME(CPU_ID) &&
	       (((g_controller.R_PRC_INT >> (CPU_ID - 1)) & 1) == 0)) {
		/* dynamic timeout (how long until R_NXT_EVN_CNT is actually due)
		 * instead of the old fixed GRTOS_HALT_WAIT_TIMEOUT_MS poll -- every other wake
		 * reason still uses WakeAllConditionVariable() to wake sleepers early, so this
		 * only affects how soon a *missed* wake is caught, and how much idle polling
		 * happens when nothing is due for a while. */
		SleepConditionVariableCS(&g_kcb.GeMRTOS_CONDITION_VARIABLE, &g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE, GRTOS_Compute_Halt_Sleep_Timeout_MS());
		/* Check on EVERY wake, not just on a timeout: every IORD()/IOWR() that
		 * notices the deadline is also a WakeAllConditionVariable caller, so this
		 * loop is woken by a real signal far more often than by its own timeout --
		 * checking only the timeout branch would miss most of those wakes and
		 * immediately re-sleep without ever noticing the deadline, defeating the
		 * point of waking sooner. Delegates to the same helper used by
		 * IORD()/IOWR()/GRTOS_Start_Task() -- one source of truth for "is
		 * the deadline due." */
		GRTOS_Check_Frozen_Mode();
		if (GRTOS_Check_And_Escalate_Timed_Event()) {
			/* only the R_NXT_TM_EVN_PRC-designated processor escalates a
			 * passed deadline into its own interrupt -- mirrors VHDL's
			 * C_TIMED_INT <= R_NXT_TM_EVN_PRC when ... and the same set-R_PRC_INT /
			 * clear-R_HLT_IDL_PRC_ENB / mask-by-~R_PRC_IRQ_SECTION pattern already used
			 * by the CTRL_TRG_INT_PRC_BIT IPI case (IOWR, ADDR_CTRL_STATUS). Any other
			 * halted processor is NOT the designated one for this deadline and must keep
			 * waiting -- it will be woken again (by WakeAllConditionVariable above, or by
			 * its own next dynamic timeout) once something actually changes for it. This
			 * replaces the pre-Stage-4 interim behavior where any halted processor could
			 * opportunistically claim someone else's deadline. Already holding
			 * GeMRTOS_MUTEX_CONDITION_VARIABLE here (entered before this loop), so these
			 * writes are already serialized against every other RMW site.
			 *
			 * FALLBACK (grtos.vhd:1777): if R_NXT_TM_EVN_PRC is genuinely 0 (no task
			 * currently associated with the next timed event -- e.g. before anything has
			 * ever been linked into the timed-event list), VHDL's C_EXTERN_INT term routes
			 * the escalation to R_LOW_PRI_PRC instead. Without this fallback, an
			 * R_NXT_TM_EVN_PRC==0 window would mean no processor is ever "designated" ->
			 * livelock (confirmed by testing this stage without the fallback: total
			 * hang, zero periodic-task output, immediately after boot). */
			G_UINT32 timed_evn_target = (g_controller.R_NXT_TM_EVN_PRC != 0)
			                            ? g_controller.R_NXT_TM_EVN_PRC
			                            : g_controller.R_LOW_PRI_PRC;
			G_UINT32 may_service = (((timed_evn_target >> (CPU_ID - 1)) & 1) != 0) ? 1u : 0u;

			/* LAST-RESORT TAKEOVER -- a third fallback tier below
			 * the two above, covering the one case neither of them does: the designated
			 * processor EXISTS but is structurally unable to respond.
			 *
			 * Both existing tiers can name a processor that is blind. Measured directly
			 * (TIMED_REFUSED instrumentation, run
			 * windows_08_signals_and_frozen_mode_20260810_145357): of 50 refusals to
			 * service an overdue deadline, 25 had R_NXT_TM_EVN_PRC naming the blocked
			 * processor, and 19 more had R_NXT_TM_EVN_PRC == 0 with the R_LOW_PRI_PRC
			 * fallback ALSO naming it -- 44 of 50 pointing at the one processor that
			 * could not act, because it sat inside 08_signals_and_frozen_mode's
			 * deliberate ~600 ms busy loop.
			 *
			 * The designation rule itself is correct and is KEPT. On real hardware the
			 * designated processor is preempted by its own timer IRQ and always services
			 * its own deadline, so this tier has no hardware equivalent and none is
			 * needed there. The rule silently assumes "the designated processor can
			 * always respond" -- true on hardware, false in a simulation with no
			 * asynchronous preemption, where a processor executing pure user computation
			 * cannot reach a kernel entry point at all.
			 *
			 * Ownership is preserved, not bypassed: the designated processor keeps
			 * exclusive ownership for the whole takeover window, so normal operation is
			 * unchanged (measured normal service lag is 2.6-40.8 ms, far below the
			 * threshold). Only a deadline its owner has demonstrably failed to service
			 * becomes available to anyone else.
			 *
			 * Double-service is already safe by EXISTING design, not by new reasoning:
			 * if owner and taker both escalate, whichever reaches gk_KERNEL_TASK_START()
			 * second recomputes the event code fresh and sees 0, because the first has
			 * already advanced R_NXT_EVN_CNT -- the property IORD's ADDR_CTRL_STATUS case
			 * already documents ("A second processor reading after the first has
			 * processed the event sees event_code=0 and skips it"). */
			if (may_service == 0u) {
				G_UINT64 tk_now      = get_elapsed_ticks_10MHz(&g_controller.SYSTEM_STARTING_TIME);
				G_UINT64 tk_deadline = g_controller.R_NXT_EVN_CNT.i64;
				if ((tk_now > tk_deadline) &&
				    ((tk_now - tk_deadline) >= ((G_UINT64) GRTOS_TIMED_EVENT_TAKEOVER_MS *
				                                (G_UINT64) GRTOS_TICKS_PER_MS))) {
					may_service = 1u;
					/* Report the takeover, but BOUNDED. Measured at ~112 firings per run
					 * on 08_signals_and_frozen_mode, so an unbounded printf here would add
					 * exactly the kind of output traffic that perturbs the timing this fix
					 * exists to correct -- the trap the TIMED_REFUSED probe fell into
					 * during this investigation (0/4 PASS with it, 6/15 without). First
					 * few in full, then sample: enough to confirm the tier is engaging the
					 * path and how overdue deadlines get, without shaping the result.
					 * Counter is per-processor, never per-OS-thread (fiber migration). */
					if ((CPU_ID >= 1) && (CPU_ID <= (G_UINT32) GEMRTOS_DRIVER_NPROCESSORS)) {
						static G_UINT32 tk_report[GEMRTOS_DRIVER_NPROCESSORS];
						G_UINT32 tk_n = tk_report[CPU_ID - 1]++;
						if ((tk_n < 3u) || ((tk_n % 250u) == 0u)) {
							printf("GRTOS_Halt_Processor_Wait: TIMED_TAKEOVER cpu=%u n=%u "
							       "target=0x%x overdue_ms=%llu\n",
							       (unsigned int) CPU_ID, (unsigned int) tk_n,
							       (unsigned int) timed_evn_target,
							       (unsigned long long) ((tk_now - tk_deadline) / GRTOS_TICKS_PER_MS));
						}
					}
				}
			}

			if (may_service != 0u) {
				g_controller.R_PRC_INT |= (0x1u << (CPU_ID - 1)) & ~g_controller.R_PRC_IRQ_SECTION;
				g_controller.R_HLT_IDL_PRC_ENB &= ~(0x1u << (CPU_ID - 1));
				timed_event_due = (int) G_TRUE;
				break;
			}
			/* Not the designated processor for this deadline: fall through to the while
			 * condition and keep waiting, same as any other still-unmet wake reason. */
		}
	}
	/* Exit boundary matching the entry marker above -- covers both ways out of the
	 * loop (falls out the bottom, or the "timed event due" break above): both land
	 * here before this same unlock. */
	g_controller.R_PRC_HLT &= ~(0x1u << (CPU_ID - 1));
	LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);

	if (GRTOS_HALT_MUTEX_AVAILABLE_TO_ME(CPU_ID)) {
		if (TryEnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_section)) {
			/* FIXED: serialize this RMW block against every other
			 * thread's writes to the same g_controller register fields (IOWR's
			 * ADDR_REG_1 case, GRTOS_Start_Task's release path, and the other RMWs
			 * below in this same function). GeMRTOS_MUTEX_section alone isn't enough
			 * here: it models "who holds the GeMRTOS mutex", but R_MTX_PRC_RSV/
			 * R_HLT_IDL_PRC_ENB/R_PRC_IRQ_SECTION are also mutated by processors that
			 * do NOT hold GeMRTOS_MUTEX_section (e.g. reserving, releasing, acking an
			 * IRQ) -- reusing GeMRTOS_MUTEX_CONDITION_VARIABLE (already the lock used
			 * everywhere else in this file to protect g_controller bookkeeping) closes
			 * that gap without introducing a new lock or lock-ordering hazard (it is
			 * only ever entered via TryEnterCriticalSection elsewhere, never a
			 * blocking Enter, so nesting it here cannot deadlock). */
			EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
			g_controller.R_MTX_PRC_GRN = CPU_ID;
			g_controller.R_MTX_PRC_RSV &= ~(0x1u << (CPU_ID - 1)); 
			g_controller.R_HLT_IDL_PRC_ENB &= ~(0x1u << (CPU_ID - 1));
			/* FIXED: mirrors VHDL's "and
			 * not(R_MTX_PRC_GRN)" term on R_PRC_IRQ_SECTION -- once this processor
			 * actually holds the mutex, it's no longer "waiting to enter its IRQ
			 * section", so clear the flag (set in IOWR's ADDR_REG_1 PRC_IRQ_ACK_BIT
			 * case). See that case's comment for the full gm_ProcessorInterrupt()
			 * deadlock this closes. */
			g_controller.R_PRC_IRQ_SECTION &= ~(0x1u << (CPU_ID - 1));
			LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
		}
		/* Else: lost the race -- return and let the caller's retry loop re-arm. */
	} else if (((g_controller.R_PRC_INT >> (CPU_ID - 1)) & 1) || timed_event_due) {
		/* FIXED: this diverts the current attempt away from acquiring
		 * the mutex (to handle an interrupt/timed event instead) WITHOUT having
		 * consumed R_MTX_PRC_RSV[CPU_ID] -- unlike the success path above, which
		 * always clears it. If left set, it stays stale (this attempt is abandoned,
		 * not resumed for an arbitrary length of time) and can be silently
		 * "consumed" by a LATER, UNRELATED visit to this function on the SAME
		 * processor -- notably IDLE, which never reserves the mutex itself
		 * (GRTOS_HALT_MUTEX_AVAILABLE_TO_ME requires R_MTX_PRC_RSV set) but would
		 * opportunistically win it via this leaked bit if R_MTX_PRC_GRN happened to
		 * be free at that moment. Confirmed as the cause of gk_CODE_IDLE_TASK's
		 * "ERROR in IDLE task" invariant failure (grtos_kernel.c:1024,
		 * PRINT_ASSERT(!(gm_IsPrcGranted()))) firing after a few periodic-task
		 * cycles -- the same invariant the original R_MTX_PRC_RSV gating was meant
		 * to protect, reopened via this different, previously-unnoticed path.
		 * Safe to clear here: GRTOS_CMD_CRITICAL_SECTION_GET's own retry loop always
		 * re-calls gm_MutexReserveSet() before its next gm_ProcessorHalt, so the
		 * original caller re-reserves automatically once actually resumed. */
		EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
		g_controller.R_MTX_PRC_RSV &= ~(0x1u << (CPU_ID - 1));
		LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
		/* Either R_PRC_INT is set for us, or we noticed a passed timed-event deadline
		 * while waiting: both are handled identically by swapping into the IRQ
		 * handler, which performs its own mutex acquisition
		 * (GRTOS_CMD_CRITICAL_SECTION_GET) before touching kernel data.
		 *
		 * note: this call site does not itself decide whether a swap
		 * is safe -- Swap_task_to_kernel() now makes that decision internally (see its
		 * own comment), by directly comparing the fiber it is actually running on
		 * against PCB_EXECTCB's own recorded fiber, rather than this call site trying
		 * to infer it from R_PRC_IRQ_SECTION. (An earlier version of this fix gated the
		 * call here using gm_PrcIRQSecGet(CPU_ID) as a proxy for "am I in kernel
		 * context" -- reverted: that register has its own pre-existing leak, harmless
		 * before this fix because every escalation forced a fresh gk_ENTRY_IRQ_HANDLER
		 * entry regardless, but which made the proxy permanently wrong once the swap
		 * became conditional on it, livelocking every run. The direct fiber-identity
		 * check in Swap_task_to_kernel() has no such dependency.) */
		Swap_task_to_kernel();
	}

	/* FIXED: make R_MTX_PRC_RSV[cpu] a strictly ONE-halt-cycle signal.
	 * Every path that RETURNS from here (success -- already cleared above; "lost the
	 * race" -- TryEnterCriticalSection failed; and "neither" -- woke but nothing to do)
	 * clears the reservation, so it can never survive past this call. The reservation's
	 * only purpose is to tell this single Halt_Processor_Wait "the caller wants the mutex"
	 * (set by gm_MutexReserveSet immediately before gm_ProcessorHalt in
	 * GRTOS_CMD_CRITICAL_SECTION_GET's loop); a caller that still wants it re-reserves on
	 * its next loop iteration. Without this, a reservation left set on a lost-race/neither
	 * return leaked into a later unrelated halt on the same processor -- notably IDLE
	 * (which never reserves) then won the mutex it must never hold: "ERROR in IDLE task"
	 * assert, or a freeze. Proven via the [MTX] IDLE-ACQ trace: IDLE acquired even after
	 * the dispatch-time clear in GRTOS_Start_Task, because the leak also occurs here on
	 * the lost-race path. (The escalation branch's Swap_task_to_kernel() does eventually
	 * return here too -- when the suspended task is later resumed, possibly on a DIFFERENT
	 * processor after migration -- so re-read the current CPU rather than reuse the entry
	 * CPU_ID, to clear the reservation of the processor we actually resumed on.) */
	{
		G_UINT32 cpu_now = gk_Get_current_CPUID();
		EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
		g_controller.R_MTX_PRC_RSV &= ~(0x1u << (cpu_now - 1));
		LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
	}
}
#undef GRTOS_HALT_MUTEX_AVAILABLE_TO_ME

/* Per-processor watchdog monitor (Windows simulation only -- there is no VHDL
 * equivalent of this function; the real hardware's independent counter has
 * no software counterpart to write here, only its OBSERVABLE effect to
 * reproduce). See ANALYSIS-processor-watchdog-reset.md and gk_START_KERNEL's
 * recovery branch (gemrtos_kernel.c) for the platform-shared half of this
 * feature.
 *
 * Runs as its own dedicated OS thread (launched once from gk_INIT_KERNEL,
 * alongside the other gu_processor threads) precisely because a genuinely
 * wedged processor -- a task spinning with no gm_/gu_ calls at all -- never
 * re-enters any GeMRTOS runtime code where a check could otherwise be
 * inserted. Real hardware's watchdog is independent silicon for the same
 * reason; this is the closest software analogue available in a thread-based
 * simulation.
 *
 * Timing is wall-clock-approximate, not cycle-accurate: Sleep(1) is not
 * guaranteed exactly 1ms on stock Windows timer resolution (commonly ~15ms
 * unless timeBeginPeriod() is in effect elsewhere in the process) -- actual
 * firing may run later than the configured timeout, never earlier. Accepted:
 * this tier exists to catch gross hangs during fast iteration, not to
 * reproduce hardware-accurate timing (see the Phase 1/Phase 2 split in
 * ANALYSIS-processor-watchdog-reset.md). */
#define GRTOS_WATCHDOG_POLL_MS 1

/* Defined further below, alongside gu_processor() -- forward-declared here
 * since GRTOS_Watchdog_Fire() (next) is defined earlier in this file. */
DWORD WINAPI GRTOS_Watchdog_Recovery_Entry(void *v_ptcb);

/* Resets processor cpu the way this simulation's watchdog reset is
 * emulated: TerminateThread() the wedged processor thread, then
 * CreateThread() a fresh one for the same CPUID via
 * GRTOS_Watchdog_Recovery_Entry() (NOT gu_processor() again -- see that
 * function's own comment for why) -- it re-enters gk_START_KERNEL()
 * exactly like a genuine boot, hitting the same PCB_EXECTCB != NULL
 * recovery branch the real hardware path uses (the recovery logic itself
 * is shared code, gemrtos_kernel.c, not duplicated
 * here).
 *
 * Guarded by the same scope rule as the embedded side (Ricardo, 2026-08-25):
 * this watchdog targets a hung TASK, not the kernel/mutex. A legitimate
 * firing never happens while cpu holds the GeMRTOS mutex -- if it does,
 * that is a kernel defect out of scope, reported loudly and left alone
 * rather than terminated. Terminating a thread that might be inside one of
 * this file's own short internal EnterCriticalSection/LeaveCriticalSection
 * regions (not the GeMRTOS-level mutex -- that narrower, unavoidable race is
 * a separate, acknowledged residual risk of TerminateThread() itself, see
 * ANALYSIS-processor-watchdog-reset.md) would wedge every other simulated
 * processor, so this check is the one mitigation available before calling
 * it. */
static void GRTOS_Watchdog_Fire(G_UINT32 cpu)
{
    G_UINT32 mutex_held_by_this_cpu;

    /* Real hardware auto-disables a processor's own watchdog the instant it
     * fires -- mirror that first, unconditionally, so software state stays
     * consistent with hardware semantics regardless of which branch below
     * is taken. */
    EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
    g_controller.R_PRC_WATCHDOG_ENB &= ~(0x1u << (cpu - 1));
    mutex_held_by_this_cpu = (g_controller.R_MTX_PRC_GRN == cpu) ? 1u : 0u;
    LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);

    if (mutex_held_by_this_cpu) {
        fprintf(stderr,
            "GRTOS_Watchdog_Monitor: cpu=%u watchdog expired WHILE HOLDING THE MUTEX -- "
            "kernel defect, out of this watchdog's scope. NOT resetting this processor.\n",
            (unsigned int) cpu);
        return;
    }

    {
        DWORD tid = g_kcb.G_PCBTbl[cpu - 1].ThreadId;
        HANDLE h = OpenThread(THREAD_TERMINATE, FALSE, tid);
        if (h == NULL) {
            fprintf(stderr, "GRTOS_Watchdog_Monitor: cpu=%u OpenThread(tid=0x%lx) failed, gle=%lu\n",
                    (unsigned int) cpu, tid, GetLastError());
            return;
        }
        fprintf(stderr, "GRTOS_Watchdog_Monitor: cpu=%u watchdog expired -- resetting (terminate+recreate)\n",
                (unsigned int) cpu);
        TerminateThread(h, (DWORD) -1);
        CloseHandle(h);
    }

    /* GRTOS_Watchdog_Recovery_Entry(), not gu_processor() -- see that
     * function's own comment. Same call shape as gk_INIT_KERNEL's original
     * launch of this processor's thread otherwise (this file, and
     * gemrtos_kernel.c) -- the returned HANDLE is not retained here either,
     * matching that existing precedent. */
    CreateThread(NULL, 0, GRTOS_Watchdog_Recovery_Entry, (void *) (uintptr_t) cpu, 0, NULL);
}

static DWORD WINAPI GRTOS_Watchdog_Monitor(void *unused)
{
    static G_UINT32 remaining_ms[GEMRTOS_DRIVER_NPROCESSORS];
    static G_UINT32 was_enabled[GEMRTOS_DRIVER_NPROCESSORS];
    (void) unused;

    for (;;) {
        G_UINT32 enabled_mask, halted_mask, raw_timeout, full_timeout_ms;
        G_UINT64 threshold_ticks;
        unsigned int i;

        Sleep(GRTOS_WATCHDOG_POLL_MS);

        EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
        enabled_mask = g_controller.R_PRC_WATCHDOG_ENB;
        halted_mask  = g_controller.R_PRC_HLT;
        raw_timeout  = g_controller.R_WATCHDOG_TIMEOUT_RAW;
        LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);

        threshold_ticks = (G_UINT64) raw_timeout << G_WATCHDOG_TIMEOUT_SHIFT;
        full_timeout_ms = (G_UINT32) (threshold_ticks / GRTOS_TICKS_PER_MS);
        /* A configured value below one poll interval would otherwise fire on
         * the very first poll after being armed -- floor it to one interval
         * instead of treating that as a legitimate immediate expiry. */
        if (full_timeout_ms < (G_UINT32) GRTOS_WATCHDOG_POLL_MS) {
            full_timeout_ms = (G_UINT32) GRTOS_WATCHDOG_POLL_MS;
        }

        for (i = 1; i <= (unsigned int) GEMRTOS_DRIVER_NPROCESSORS; i++) {
            G_UINT32 idx     = i - 1;
            G_UINT32 enabled = (enabled_mask >> idx) & 1u;
            G_UINT32 halted  = (halted_mask  >> idx) & 1u;

            if (!enabled) {
                was_enabled[idx]  = 0u;
                continue;
            }
            /* Hardware reloads the counter to the full timeout both when
             * halted and on a fresh (re-)enable -- GRTOS_Compute_Halt_Sleep_Timeout_MS's
             * own "reload on halt" rule, applied here to just-armed watchdogs too. */
            if (!was_enabled[idx] || halted) {
                remaining_ms[idx] = full_timeout_ms;
                was_enabled[idx]  = 1u;
                continue;
            }

            if (remaining_ms[idx] <= (G_UINT32) GRTOS_WATCHDOG_POLL_MS) {
                GRTOS_Watchdog_Fire(i);
                /* Whether or not the fire above actually reset the
                 * processor (the mutex-held guard may have refused),
                 * reload here so this poll loop does not refire every
                 * single interval on the same stale countdown. */
                remaining_ms[idx] = full_timeout_ms;
            } else {
                remaining_ms[idx] -= (G_UINT32) GRTOS_WATCHDOG_POLL_MS;
            }
        }
    }
    return 0;
}

/**
 * Funtion pthread to execute the TCB task in GNU architecture
 */
DWORD WINAPI gu_processor(void *v_ptcb)
{
	G_INT32 CPUID = (G_INT32) v_ptcb;


	g_kcb.G_PCBTbl[CPUID-1].ThreadId = GetCurrentThreadId();
    /* PCB_EXECTCB is NULL here — use plain fprintf, not GU_FPRINTF (which dereferences
       PCB_EXECTCB->TCB_MTX_NESTED in gm_GeMRTOSCriticalSectionExit() and would crash). */
    fprintf(stderr,"[ MESSAGE ] INITIALIZE PROCESSOR %u\n", gm_ProcessorId());
	PRINT_ASSERT((gm_ProcessorId() == CPUID),"ERROR");

	while(g_controller.R_PRC_RST != CPUID - 1);
    fprintf(stderr,"[ MESSAGE ] RUNNING PROCESSOR %u\n", gm_ProcessorId());

	gk_START_KERNEL();

	while(1);
	return (0);
}

/* Watchdog-recovery re-entry point for GRTOS_Watchdog_Fire() -- NOT
 * gu_processor() again, deliberately. gu_processor()'s own
 * "while(g_controller.R_PRC_RST != CPUID - 1)" gate exists only for the
 * ORIGINAL boot sequencing (R_PRC_RST is a high-water mark that GRTOS_Start_Task
 * advances on every dispatch, on any processor, and never lowers -- see its
 * own comment). By the time any watchdog can fire, this processor has
 * already dispatched at least one task itself, so R_PRC_RST has already
 * moved past CPUID-1 permanently: a fresh gu_processor thread would block on
 * that gate forever and never reach gk_START_KERNEL() (confirmed by testing
 * this exact scenario, ANALYSIS-processor-watchdog-reset.md). This function
 * is gu_processor() minus that one boot-only gate -- everything else
 * (ThreadId claim, identity assert, gk_START_KERNEL() call, the same
 * while(1) fallback) is identical. */
DWORD WINAPI GRTOS_Watchdog_Recovery_Entry(void *v_ptcb)
{
	G_INT32 CPUID = (G_INT32) v_ptcb;

	g_kcb.G_PCBTbl[CPUID-1].ThreadId = GetCurrentThreadId();
	fprintf(stderr, "[ MESSAGE ] RECOVERING PROCESSOR %u (watchdog reset)\n", (unsigned int) CPUID);
	PRINT_ASSERT((gm_ProcessorId() == CPUID),"ERROR");

	gk_START_KERNEL();

	while(1);
	return (0);
}

/**
 * Funtion pthread to execute the TCB task in GNU architecture
 */
void gu_pthread(void *v_ptcb)
{
	/* FIXED: first statement, deliberately before anything
	 * else -- this is a task's first-ever dispatch (see
	 * GRTOS_ReleaseKernelMutexIfPending()'s own comment for why the release
	 * belongs here rather than in GRTOS_Start_Task()). */
	GRTOS_ReleaseKernelMutexIfPending();

	GS_TCB *ptcb = (GS_TCB *) v_ptcb;

	void *(*function_address)(void *) = ptcb->TCB_TaskCode;


	function_address((void *) ptcb->TCB_TaskArg);


	gk_ENTRY_TASK_COMPLETE();


	return;
}

unsigned int R_MSK_1(unsigned int value)
{
	PRINT_ASSERT((value <= 32),"ERROR invalue value= %d", value);
	if (value == 0) return (0);
	else return (1 << (value - 1));
}

unsigned int R_MSK_0(unsigned int value)
{
	PRINT_ASSERT((value <= 32),"ERROR invalue value= %d", value);
	if (value == sizeof(unsigned int) * CHAR_BIT) return (0);
	else return (1 << (value));
}

G_UINT64 get_elapsed_ticks_10MHz(struct timespec *SYSTEM_STARTING_TIME)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    G_UINT64 delta_ns = (G_UINT64)(now.tv_sec  - SYSTEM_STARTING_TIME->tv_sec)  * 1000000000ULL
                      + (G_UINT64)(now.tv_nsec) - (G_UINT64)(SYSTEM_STARTING_TIME->tv_nsec);

    return delta_ns / 100ULL;
}

unsigned int Get_Current_Processor_ID_from_thread()
{
    G_UINT32 processor = 0;
    for (unsigned int i=0; i<G_NUMBER_OF_PCB ; i++) {
        if (g_kcb.G_PCBTbl[i].ThreadId == GetCurrentThreadId()) {
            processor = i + 1;
            break;
        }
    }
    PRINT_ASSERT((processor >= 1 && processor <= (G_UINT32) G_NUMBER_OF_PCB), "ERROR MTX\n");
    return ((unsigned int) processor);
}


unsigned int IORD(uintptr_t ptr_base, unsigned int register_address)
{
    unsigned int base = (unsigned int) ptr_base;
    unsigned int ProcessorID = Get_Current_Processor_ID_from_thread();
    unsigned int aux_usigint;
	TIMEPRIORITY temp_aux;

	/* Every register access is an opportunity to notice a passed
	 * deadline and wake waiters immediately, mirroring VHDL's continuous
	 * evaluation of C_TIMED_INT. */
	(void) GRTOS_Check_And_Escalate_Timed_Event();
	GRTOS_Check_Frozen_Mode();

	switch (base)
	{
		case GEMRTOS_DRIVER_PROCESSOR_BASE:
			switch (register_address)
			{
                case ADDR_REG_0:
                    unsigned int R_MTX_PRC_GRN_PRC;
                    if ((void *) GetCurrentThreadId() != (void *) g_kcb.GeMRTOS_MUTEX_section.OwningThread) {
                        R_MTX_PRC_GRN_PRC = 0;
                    } else {
                        R_MTX_PRC_GRN_PRC = 1;
                    }
                    unsigned int R_PRC_INT_PRC         = (g_controller.R_PRC_INT >> (ProcessorID-1)) & 1;
                    unsigned int R_HLT_IDL_PRC_ENB_PRC = (g_controller.R_HLT_IDL_PRC_ENB >> (ProcessorID-1)) & 1;
                    aux_usigint = (R_PRC_INT_PRC << 2) |
                                  (R_MTX_PRC_GRN_PRC << 1) |
                                  R_HLT_IDL_PRC_ENB_PRC;
                    return (aux_usigint);
                    break;
                    
				case ADDR_REG_1:
					return (ProcessorID);
					break;

				default:
					printf("ERROR RD GEMRTOS_DRIVER_PROCESSOR_BASE register %d not considered", register_address);
					while(1);
					break;
			}
			break;
            
		case GEMRTOS_DRIVER_BASE:
			switch (register_address)
			{
                case ADDR_CTRL_STATUS:
                    /* Compute event code dynamically from current time so that:
                     *  - Each read reflects actual state (R_NXT_EVN_CNT may have advanced).
                     *  - A second processor reading after the first has processed the event
                     *    sees event_code=0 (ticks < new R_NXT_EVN_CNT) and skips it.
                     *  - gk_ENTRY_IRQ_HANDLER called re-entrantly (from gk_ENTRY_TASK_COMPLETE)
                     *    also sees event_code=0 and does not consume future events.
                     */
                    {
                        G_UINT64 ticks_now = get_elapsed_ticks_10MHz(&g_controller.SYSTEM_STARTING_TIME);
                        G_INT32 evt_code = 0;
                        if (g_controller.C1_FRZ_EVN != 0) {
                            evt_code = EVN_CODE_FROZEN;
                        } else if (g_controller.C1_UNFRZ_EVN != 0) {
                            /* FIXED: C1_UNFRZ_EVN was
                             * computed correctly by GRTOS_Check_Frozen_Mode() (the falling
                             * edge when the deadline backlog clears) and even had its own
                             * CTRL_UNFRZ_MDE_CLR_BIT acknowledge-clear handler below, but
                             * nothing ever read it here to actually emit EVN_CODE_UNFROZEN
                             * -- the application's G_SCBType_UNFROZEN_MODE signal could
                             * therefore never fire on this port. Found by
                             * verify_windows.ps1 actually running 08_signals_and_frozen_mode
                             * and comparing against its verify.expect; the FROZEN half of
                             * this same mechanism was already exercised and correct (per
                             * the ai-claude effort's own 60s soak test), which is why only
                             * the UNFROZEN half went unnoticed until an example specifically
                             * exercised the recovery path. */
                            evt_code = EVN_CODE_UNFROZEN;
                        } else if (ticks_now >= g_controller.R_NXT_EVN_CNT.i64) {
                            evt_code = EVN_CODE_TIMED;
                        } else {
                            for (int j = 31; j >= 0; j--) {
                                if (((g_controller.IRQ_EVENT_PENDING >> j) & 1) == 1) {
                                    evt_code = (G_INT32) j;
                                    break;
                                }
                            }
                        }
                        aux_usigint = (unsigned int)(evt_code & 0xFFFF);
                    }
                    aux_usigint |= (g_controller.R_TM_PSC & 0x1F) << 16;
                    aux_usigint |= g_controller.R1_KERNEL_MODE << STATUS_KERNEL_MODE;
                    aux_usigint |= g_controller.R1_HLD_CNT_RST << STATUS_TM_CNTS_CLR_BIT;
                    aux_usigint |= g_controller.R1_MTX_NESTED  << STATUS_MTX_NESTED;
                    aux_usigint |= g_controller.R1_HLD_CNT_RST << STATUS_HLD_CNT_BIT;
                    aux_usigint |= (g_controller.C1_FRZ_TM_OCC & g_controller.C1_FRZ_MDE_ENB) << STATUS_FRZ_ACT_BIT;
                    aux_usigint |= g_controller.C1_FRZ_MDE_ENB << STATUS_FRZ_ENB_BIT;
                    aux_usigint |= g_controller.TIME_HOLD << STATUS_TIME_HOLD_BIT;
                    return (aux_usigint);
                    break;
                    
				case ADDR_SMP:               // register 1
                    return (g_controller.R_SMP.i32[0]);
                    break;

                case ADDR_IRQ_RQS:               // register 2
                    /* R_IRQ_PND <= DIRQI_synch and R_IRQ_ENB
                     * (grtos.vhd:1758) -- masked external-IRQ-pending bitmap. Reads 0
                     * today (nothing writes DIRQI_synch), matching real hardware with no
                     * external interrupt source asserted. */
                    return (g_controller.DIRQI_synch & g_controller.R_IRQ_ENB);
                    break;

                case ADDR_PRC_IRQ_SECTION:       // register 3
                    return (g_controller.R_PRC_IRQ_SECTION);
                    break;

				case ADDR_FRZ_THR_HGH:       // register 4
                    g_controller.R_SMP.i32[0] = g_controller.R_FRZ_TM_THR.i32[0];
					return (g_controller.R_FRZ_TM_THR.i32[1]);
					break;

                case ADDR_FRZ_TM_HGH:        // register 5
                    /* R_FRZ_TM_ELP (this sim's
                     * R_FRZ_TM_CNT stand-in), maintained by GRTOS_Check_Frozen_Mode()
                     * -- see that function's comment for what is/isn't modeled. */
                    g_controller.R_SMP.i32[0] = g_controller.R_FRZ_TM_ELP.i32[0];
                    return (g_controller.R_FRZ_TM_ELP.i32[1]);
                    break;

				case ADDR_TM_CNT_HGH:        // register 6
                    g_controller.R_TM_CNT.i64 = get_elapsed_ticks_10MHz(&g_controller.SYSTEM_STARTING_TIME);
                    g_controller.R_SMP.i32[0] = g_controller.R_TM_CNT.i32[0];
					return (g_controller.R_TM_CNT.i32[1]);
					break;

                case ADDR_SYS_TM_HGH:
                    g_controller.C_SYS_TM.i64 = (g_controller.R1_HLD_CNT_RST == 0)
                                                ? (get_elapsed_ticks_10MHz(&g_controller.SYSTEM_STARTING_TIME) - g_controller.g_sys_tm_base.i64)
                                                : 0;
                    g_controller.R_SMP.i32[0] = g_controller.C_SYS_TM.i32[0];
					return (g_controller.C_SYS_TM.i32[1]);                      
                    break;


				default:
					printf("ERROR RD GEMRTOS_DRIVER_BASE register %d not considered", register_address);
					while(1);
					break;
			}
			break;
            
        case GEMRTOS_DRIVER_BASE_GMEMORY:
            printf("ERROR RD GEMRTOS_DRIVER_BASE_GMEMORY register %d not considered", register_address);
            while(1);
            break;
        

        case GEMRTOS_DRIVER_BASE_TESTING:
            GRTOS_RUNPRINTF("RUNNING -> %s, %s, %d, register_address= 0x%x \n", __FILE__, __FUNCTION__, __LINE__, register_address);
            switch (register_address) {
                /* read-only debug/testing mirrors of state that
                 * already exists and is actively maintained elsewhere in g_controller --
                 * purely additive, no behavior change to anything else. ADDR_PRC_INT_STA is
                 * deliberately NOT enabled here: its only backing storage is the standalone
                 * R_PRC_INT_STA global declared in gemrtos_controller_windows.h, which nothing in
                 * this file ever writes -- it would read back a meaningless, permanently-
                 * stale value. ADDR_WAITING_IN_ISR was excluded for the same reason until
                 * g_controller.R_WAITING_IN_ISR now has a real, actively-maintained
                 * backing field (GRTOS_Compute_Mutex_Next_Processor()) -- so it is enabled below.
                 * Note it reflects the state as of the last mutex-wait evaluation, not a
                 * truly live per-cycle value (that function is called on demand, not on a
                 * clock) -- fine for a debug mirror, same tolerance as its siblings here. */
                case ADDR_MTX_GRN:        return (g_controller.R_MTX_PRC_GRN);                break;
                case ADDR_WAITING_IN_ISR: return (g_controller.R_WAITING_IN_ISR);              break;
                case ADDR_MTX_RSV_PRC:    return (g_controller.R_MTX_PRC_RSV);                break;
                case ADDR_PRC_INT:        return (g_controller.R_PRC_INT);                     break;
                case ADDR_PRC_INT_ENB:    return (g_controller.R_PRC_INT_ENB);                 break;
                case ADDR_slave_irq:      return (g_controller.slave_processor_waitrequest);   break;
                case ADDR_TIMED_INT:      return (C1_EVN_TM_OCC);                 break;
                case ADDR_EXTERN_INT:     return (g_controller.IRQ_EVENT_PENDING);             break;
                case ADDR_NXT_TM_EVN_PRC: return (g_controller.R_NXT_TM_EVN_PRC);             break;
                case ADDR_IRQ_ENB:        return (g_controller.R_IRQ_ENB);                     break;
                case ADDR_MTX_RLS_BIT:    return (g_controller.R_MTX_RLS_BIT);                break;
                /* ADDR_DIRQI mirrors DIRQI_synch directly (raw,
                 * pre-mask -- see ADDR_IRQ_RQS above for the masked view and the field's
                 * own comment for the future-proofing rationale). */
                case ADDR_DIRQI:          return (g_controller.DIRQI_synch);                     break;
                /* ADDR_MTX_RST mirrors R_PRC_RST, the hardware boot reset-sequencer
                 * (grtos.vhd:1127-1149) -- unrelated to external IRQs, no sim counterpart
                 * since processors are created directly as OS threads, never held in a
                 * reset sequence. 0 is the accurate value: never in reset. */
                case ADDR_MTX_RST:        return (0);                                              break;
                case ADDR_INOUTS: return (g_controller.R_INOUTS); break;
                default:
                    printf("ERROR RD GEMRTOS_DRIVER_BASE_TESTING register %d not considered", register_address);
                    while(1);
            }

               
		default:
			printf("ERROR RD base 0x%x not considered", base);
			while(1);
			break;
	}
    return (G_FALSE);
}


void Reset_GeMRTOS_controller_registers(void)
{
	InitializeCriticalSection(&g_kcb.GeMRTOS_MUTEX_section);
    
    InitializeCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
	InitializeConditionVariable(&g_kcb.GeMRTOS_CONDITION_VARIABLE);

    g_controller.R1_HLD_CNT_RST = 1;
    g_controller.R1_KERNEL_MODE = 1;

	g_controller.R_NXT_EVN_CNT.i64 = LASTESTEVENT;

	/* Watchdogs start disabled (R_PRC_WATCHDOG_ENB already 0 from
	 * g_controller's static zero-init) with the timeout at its documented
	 * power-up default -- matches Ricardo's description of the real
	 * hardware's reset state. */
	g_controller.R_WATCHDOG_TIMEOUT_RAW = 0xFFF;

	clock_gettime(CLOCK_MONOTONIC, &g_controller.SYSTEM_STARTING_TIME); // R_TM_CNT = 0;

	/* Launched once here (this function's own one-time Windows-sim init
	 * role), alongside the processor threads gk_INIT_KERNEL creates right
	 * after calling this function. Harmless when no processor's watchdog is
	 * ever enabled -- it only polls state that stays all-zero. See
	 * GRTOS_Watchdog_Monitor()'s own comment for why this needs its own
	 * dedicated thread instead of hooking into existing runtime code. */
	CreateThread(NULL, 0, GRTOS_Watchdog_Monitor, NULL, 0, NULL);

}

G_INT32 lowest_rsv(void)
{
	G_INT32 lowest_prc = 0;
	for (G_INT32 i = GEMRTOS_DRIVER_NPROCESSORS; i >= 1; i--)
	{
		if (((g_controller.R_MTX_PRC_RSV >> (i-1)) & 1) == 1) lowest_prc = i;
	};
	return (lowest_prc);
}


unsigned int IOWR(uintptr_t ptr_base, unsigned int register_address, unsigned int value)
{
    unsigned int base = (unsigned int) ptr_base;
    unsigned int ProcessorID = Get_Current_Processor_ID_from_thread();
    unsigned int aux_usigint;
	TIMEPRIORITY temp_aux;

	/* Same eager check as IORD() -- see there for rationale. */
	(void) GRTOS_Check_And_Escalate_Timed_Event();
	GRTOS_Check_Frozen_Mode();

   	switch (base)
	{
        case GEMRTOS_DRIVER_PROCESSOR_BASE:
            switch (register_address)
            {   
				case ADDR_REG_0:
                    /* Matches VHDL slave_processor_waitrequest(i): only stall if this
                     * processor's idle/halt latch is already armed (via a prior
                     * ADDR_REG_1 bit-3 write); otherwise the write is a silent no-op,
                     * exactly like real hardware. */
                    if (((g_controller.R_HLT_IDL_PRC_ENB >> (ProcessorID - 1)) & 1) == 1) {
                        GRTOS_Halt_Processor_Wait();
                    }
                    break;

  
				case ADDR_REG_1:
                    /* FIXED: this whole case body reads/writes
                     * g_controller's register-file bitfields (R_MTX_PRC_RSV,
                     * R_PRC_IRQ_SECTION, R_PRC_INT, R_HLT_IDL_PRC_ENB, R_MTX_PRC_GRN,
                     * ...) with plain |=/&= -- serialize it against every other
                     * thread's RMW of the same fields (GRTOS_Halt_Processor_Wait,
                     * GRTOS_Start_Task) using the same CONDITION_VARIABLE lock already
                     * used elsewhere in this file for g_controller bookkeeping. The
                     * existing nested Enter/Leave around WakeAllConditionVariable below
                     * is a safe recursive re-entry on this same thread (Windows
                     * CRITICAL_SECTION is recursive), not a second independent lock. */
                    EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                    g_controller.R_PRC_INT_ENB |= (((value >> 0) & 0x1) << (ProcessorID - 1));
                    g_controller.R_MTX_PRC_RSV |= (((value >> 1) & 0x1) << (ProcessorID - 1));
                    g_controller.R_PRC_IRQ_ACK &= ~(((value >> 2) & 0x1) << (ProcessorID - 1));
                    /* R_PRC_IRQ_SECTION set here. FIXED: mirrors VHDL's
                     * R_PRC_IRQ_SECTION <= (R_PRC_IRQ_SECTION
                     * or R_PRC_IRQ_ACK) and not(R_MTX_PRC_GRN) -- this processor announcing
                     * PRC_IRQ_ACK_BIT (via gm_PrcIRQSecSet(), the first statement of
                     * gk_ENTRY_IRQ_HANDLER) means "I am now in my IRQ/kernel-entry section".
                     * Cleared in GRTOS_Halt_Processor_Wait() once the mutex is actually
                     * granted (the "and not(R_MTX_PRC_GRN)" term). Without this, callers of
                     * gm_ProcessorInterrupt() (e.g. gk_TCBRUNL_Unlink's cross-processor
                     * interrupt) spin forever on gm_PrcIRQSecGet(), since it always read 0. */
                    if (((value >> 2) & 0x1) == 1) {
                        g_controller.R_PRC_IRQ_SECTION |= (0x1u << (ProcessorID - 1));
                        /* FIXED: emulate the VHDL's continuous
                         * R_PRC_INT_Next <= (R_PRC_INT or R_MSK_1) and not(R_PRC_IRQ_SECTION)
                         * (grtos.vhd:1785-1786). Once this processor enters its IRQ section,
                         * its pending-interrupt request is being serviced and MUST drop to 0.
                         * The emulation only applied the not(R_PRC_IRQ_SECTION) mask at the
                         * SET site (CTRL_TRG_INT_PRC_BIT), never afterward, so R_PRC_INT stayed
                         * latched at 1 forever after the first interrupt. Consequence: every
                         * later GRTOS_Halt_Processor_Wait() saw R_PRC_INT==1, skipped the wait,
                         * and (when the mutex was held elsewhere) re-took the interrupt branch
                         * -> Swap_task_to_kernel() re-entrant on the kernel fiber, corrupting
                         * PCB_EXECTCB->uctx and spinning gk_ENTRY_IRQ_HANDLER at 100% CPU
                         * forever. Root cause of the reported hang. */
                        g_controller.R_PRC_INT &= ~(0x1u << (ProcessorID - 1));
                    }
                    g_controller.R_PRC_IDL_ENB &= ~(0x1 << (ProcessorID - 1));
                    g_controller.R_PRC_IDL_ENB |= (((value >> 3) & 0x1) << (ProcessorID - 1));
                    /* VHDL: R_HLT_IDL_PRC_ENB <= (R_HLT_IDL_PRC_ENB or R_PRC_IDL_ENB) and
                     * not(R_PRC_INT or R_MTX_PRC_GRN).  R_PRC_IDL_ENB is a one-cycle pulse;
                     * latch its OR into the real halt-enable register that gates the
                     * ADDR_REG_0 wait (see that case above). */
                    if (((value >> 3) & 0x1) == 0x1) {
                        g_controller.R_HLT_IDL_PRC_ENB |= (0x1u << (ProcessorID - 1));
                    }
                    g_controller.R_MTX_RLS_BIT &= ~(0x1 << (ProcessorID - 1));
                    g_controller.R_MTX_RLS_BIT |= (((value >> 4) & 0x1) << (ProcessorID - 1));
                    /* gm_PrcWatchdogEnable(): OR-set this processor's watchdog-enabled
                     * bit, same shape as PRC_IDL_ENB_BIT above -- a one-shot "arm" pulse
                     * latched into a persistent bitmask, cleared only via ADDR_REG_2
                     * (gm_PrcWatchdogDisable(), below) or a mutex-held guard's own halt. */
                    if (((value >> PRC_WATCHDOG_ENB_BIT) & 0x1) == 0x1) {
                        g_controller.R_PRC_WATCHDOG_ENB |= (0x1u << (ProcessorID - 1));
                    }
                    if (((value >> 4) & 0x1) == 0x1) {                                  // Release bit set
                        if (g_controller.R1_MTX_NESTED == 0) {                                       // Can release only if nested==0
                            if ((void *) GetCurrentThreadId() == (void *) g_kcb.GeMRTOS_MUTEX_section.OwningThread)
                            {    //
                                LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_section);     //
                                g_controller.R_MTX_PRC_GRN = 0;                                      // Clear grant bit
                                /* narrowed to match VHDL exactly --
                                 * R_HLT_IDL_PRC_ENB_logic (grtos.vhd:1302-1310) only ever
                                 * clears bit i when processor i itself holds R_MTX_PRC_GRN
                                 * (or is interrupted); every other processor's bit is left
                                 * untouched by a release, full stop -- VHDL has no
                                 * "clear-for-everyone-on-release" step at all. Previously
                                 * this cleared every bit unconditionally; now only the
                                 * round-robin-designated winner's bit is cleared here
                                 * (GRTOS_Halt_Processor_Wait's own grant path also clears
                                 * it for whichever processor actually claims the mutex --
                                 * redundant with this for the winner, harmless). Benefit
                                 * over the old broad clear: a non-winning waiter's
                                 * R_HLT_IDL_PRC_ENB bit stays set, so its
                                 * GRTOS_Halt_Processor_Wait while-loop goes straight back to
                                 * SleepConditionVariableCS on this wake instead of exiting
                                 * the loop, losing the (already arbiter-gated) check, and
                                 * paying a full GRTOS_CMD_CRITICAL_SECTION_GET retry
                                 * round-trip (re-reserve + re-halt) just to sleep again. */
                                {
                                    G_UINT32 next_prc = GRTOS_Compute_Mutex_Next_Processor();
                                    if (next_prc != 0) {
                                        g_controller.R_HLT_IDL_PRC_ENB &= ~(0x1u << (next_prc - 1));
                                    }
                                }
                                // Wake condition variable for waiting processors
                                EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                                WakeAllConditionVariable(&g_kcb.GeMRTOS_CONDITION_VARIABLE);
                                LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                            }
                        }
                    }
                    LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
					break;

                case ADDR_REG_2:
                    /* AND-clear counterpart to ADDR_REG_1's OR-set
                     * (grtos.vhd:1348-1350: R_PRC_INT_ENB(i) <= writedata(0) and
                     * R_PRC_INT_ENB(i)) -- must only ever CLEAR this processor's own bit
                     * (when writedata(0)==0) and leave every other bit (including other
                     * processors') untouched; writedata(0)==1 is a no-op (AND with 1).
                     * Deliberately NOT "&= ((value&1)<<(ProcessorID-1))" -- that would
                     * clobber every other processor's bit to 0 too, since ANDing with a
                     * word that has only one bit set clears everything else. Locked like
                     * every other RMW of this shared register file (see C1 fix): two
                     * processors' concurrent ADDR_REG_1/ADDR_REG_2 writes touch different
                     * bits of the same word, a lost-update hazard without this lock.
                     * Currently unreachable: gm_PrcInterruptDisable(), the only caller,
                     * has zero call sites in the codebase. */
                    if ((value & 0x1) == 0) {
                        EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                        g_controller.R_PRC_INT_ENB &= ~(0x1u << (ProcessorID - 1));
                        LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                    }
                    /* gm_PrcWatchdogDisable(): same AND-clear shape as bit 0 above,
                     * writedata(PRC_WATCHDOG_ENB_BIT)==0 clears this processor's own
                     * watchdog-enabled bit; ==1 is a no-op (the real primitive writes
                     * all-1s-except-this-bit, so every other bit here is always a
                     * deliberate no-op against whatever it currently holds). */
                    if (((value >> PRC_WATCHDOG_ENB_BIT) & 0x1) == 0) {
                        EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                        g_controller.R_PRC_WATCHDOG_ENB &= ~(0x1u << (ProcessorID - 1));
                        LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                    }
                    break;

                default:
                    printf("ERROR WR GEMRTOS_DRIVER_PROCESSOR_BASE register %d not considered", register_address);
                    while(1);
                    break;
            }
            break;
            
        case GEMRTOS_DRIVER_BASE:
            switch (register_address)
            {
                case ADDR_CTRL_STATUS:    // register 0
                    /* FIXED: this entire case was unlocked -- unlike
                     * ADDR_REG_1/ADDR_REG_2, which got the
                     * GeMRTOS_MUTEX_CONDITION_VARIABLE treatment for the same class of
                     * hazard. g_controller.R_CTLR is used here as BOTH this call's local
                     * "value" AND a shared, globally-visible field (several branches below
                     * re-read g_controller.R_CTLR instead of the value parameter) --
                     * without a lock, two processors calling IOWR(ADDR_CTRL_STATUS, ...)
                     * concurrently (gm_KernelModeSet()/gm_MutexNestedValue(), used on every
                     * kernel-entry/exit) can interleave: one thread's R_CTLR write lands
                     * between another thread's write and its later re-read, so
                     * R1_KERNEL_MODE/R1_MTX_NESTED end up reflecting the WRONG processor's
                     * intended bit. Root cause of a reproducible crash: with
                     * INCLUDE_TASK_INFINITE_LOOP enabled (a second real task, not just
                     * IDLE, now doing genuine suspend/resume kernel cycles on processor 2
                     * concurrently with processor 1's), gk_KERNEL_TASK_SUSPEND_CURRENT's
                     * resumed-task invariant PRINT_ASSERT(gm_IsPrcGranted()) failed
                     * (grtos_kernel.c:422) because GRTOS_Start_Task's release decision
                     * ("if R1_MTX_NESTED==0 && R1_KERNEL_MODE==0") raced against this
                     * unlocked write -- froze that processor's thread in
                     * G_DEBUG_WHILEFOREVER, then the whole process crashed (wild jump to
                     * address 0x1) shortly after from the resulting corrupted state.
                     * Locking the whole case (not just the individual field writes) is
                     * required because the "write value, then later re-read
                     * g_controller.R_CTLR" pattern spans the entire inner switch -- a
                     * narrower per-field lock would still leave that read-after-write gap
                     * exposed. The nested Enter/Leave around WakeAllConditionVariable
                     * inside CTRL_TRG_INT_PRC_BIT below is a safe recursive re-entry on
                     * this same thread (Windows CRITICAL_SECTION is recursive), not a
                     * second independent lock -- same pattern already used elsewhere in
                     * this file. */
                    EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                    g_controller.R_CTLR = value;
                    /* gm_WatchogTimeoutSet(): CTRL_WATCHDOG_BIT (14) combines with a
                     * 12-bit payload in value's bits[0..11] in the SAME word -- unlike
                     * every command below, whose select bit lives at bit>=16 (masked by
                     * "bits = R_CTLR & 0xFFFF0000"). Bit 14 falls outside that mask, so
                     * it can never reach the switch below and must be checked first;
                     * handled here instead of widening the shared mask, to avoid
                     * touching the well-tested one-hot dispatch below at all. */
                    if ((value & (1u << CTRL_WATCHDOG_BIT)) != 0) {
                        g_controller.R_WATCHDOG_TIMEOUT_RAW = value & 0xFFF;
                        g_controller.R_CTLR = 0;
                        LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                        break;
                    }
                    unsigned int bits = g_controller.R_CTLR & 0xFFFF0000;
                    switch (bits)
                    {
                        case (1 << CTRL_GTD_CLK_BIT):
                            g_controller.GATED_CLK_POS = g_controller.R_CTLR & 0x1;
                            break;  

                        case (1 << CTRL_TM_PSC_BIT):
                            g_controller.R_TM_PSC = g_controller.R_CTLR & 0x1F;
                            break;

                        case (1 << CTRL_KERNEL_MODE_BIT):
                            g_controller.R1_KERNEL_MODE = g_controller.R_CTLR & 0x1;
                            break;

                        case (1 << CTRL_HLT_PRC_MTX):
                            /* removes a crash-on-write landmine -- previously
                             * unhandled, would hit "default: printf(ERROR...); while(1);"
                             * if anything ever wrote this bit. Currently unreachable in
                             * practice (no gm_* macro or caller writes it anywhere in the
                             * BSP/app -- confirmed by search); only an external JTAG-master
                             * debug tool would write it on real hardware. See the field's
                             * own comment (g_controller struct) for why the derived output
                             * flag isn't modeled. */
                            g_controller.R1_STATUS_frozen_avalon_controller_monitor = g_controller.R_CTLR & 0x1;
                            break;

                        case (1 << CTRL_TM_CNTS_CLR_BIT):
                            g_controller.R1_HLD_CNT_RST = g_controller.R_CTLR & 0x1;
                            if (g_controller.R1_HLD_CNT_RST == 0) {
                                g_controller.g_sys_tm_base.i64 =    get_elapsed_ticks_10MHz(&g_controller.SYSTEM_STARTING_TIME);
                                /* mirrors grtos.vhd:1245's
                                 * R_FRZ_TM_CNT_Next <= 0 when R1_HLD_CNT_RST='1' -- held
                                 * at 0 throughout the reset-held window (GRTOS_Check_Frozen_Mode()
                                 * also refuses to accumulate while held), resuming from 0 the
                                 * moment it's released, same as g_sys_tm_base's capture above. */
                                g_controller.R_FRZ_TM_ELP.i64 = 0;
                            }
                            break;

                        case (1 << CTRL_LOW_PRC_BIT):
                            /* FIXED (defensive symmetry with the
                             * CTRL_NTX_TM_PRC_BIT fix below): processor==0 must produce a
                             * literal 0, not "1 << ((0-1)&0xFFFF)" == 1 << 0xFFFF (undefined
                             * behavior -- happens to fold to a fixed nonzero bit on this
                             * target, matching no real processor). gk_SetLowestProcessor()
                             * always passes a real processor in practice, but don't rely on
                             * that via UB. */
                            g_controller.R_LOW_PRI_PRC = ((value & 0xF) == 0) ? 0 : (1u << (((value & 0xF) - 1) & 0xFFFF));
                            break;

                        case (1 << CTRL_IRQ_ENB_SET_BIT):
                            g_controller.R_IRQ_ENB = g_controller.R_IRQ_ENB | (1 << ((value) & 0xFFFF));
                            break;
                            
                        case (1 << CTRL_IRQ_ENB_CLR_BIT):
                            g_controller.R_IRQ_ENB = g_controller.R_IRQ_ENB & ~(1 << ((value) & 0xFFFF));
                            break;
                            
                        case (1 << CTRL_NTX_TM_PRC_BIT):
                            /* FIXED: gk_NextOccTimeProcessor()'s own
                             * doc comment says this register is "0 if the next occurrence time
                             * task is not executing" -- but the old unconditional
                             * "1 << ((value-1)&0xFFFF)" computed 1 << 0xFFFF for processor==0,
                             * undefined behavior that folds (shift count mod 32 on this target)
                             * to a fixed nonzero bit matching no real processor. That silently
                             * broke VHDL's fallback rule (grtos.vhd:1777: C_EXTERN_INT takes over
                             * via R_LOW_PRI_PRC exactly when this register is genuinely all-zero)
                             * and, combined with GRTOS_Halt_Processor_Wait()'s designated-
                             * processor gating, could livelock: if this fires before any task is
                             * yet linked into the timed-event list, NEITHER real processor would
                             * ever be recognized as the deadline's owner. */
                            g_controller.R_NXT_TM_EVN_PRC = ((value & 0xF) == 0) ? 0 : (1u << (((value & 0xF) - 1) & 0xFFFF));
                            break;

                        case (1 << CTRL_ERET_ADDR_BIT):
                            g_controller.R_ERET_REG = (value & 0xFFFF);
                            break;

                        case (1 << CTRL_TM_CNTS_HLD_BIT):
                            g_controller.TIME_HOLD = g_controller.R_CTLR & 0x1;
                            break;

                        case (1 << CTRL_RESET_BIT):
                            g_controller.RESET_HOLD = 0x1;
                            break;

                        case (1 << CTRL_MTX_RLS_DSB_BIT):
                            GRTOS_RUNPRINTF("RUNNING -> %s, %s, %d, g_controller.R1_MTX_NESTED & 0x1= 0x%x \n", __FILE__, __FUNCTION__, __LINE__, g_controller.R_CTLR & 0x1);
                            g_controller.R1_MTX_NESTED = g_controller.R_CTLR & 0x1;
                            break;

                        case (1 << CTRL_FRZ_MDE_ENB_BIT):
                            g_controller.C1_FRZ_MDE_ENB = g_controller.R_CTLR & 0x1;
                            break;

                        case (1 << CTRL_FRZ_MDE_CLR_BIT):
                            g_controller.C1_FRZ_EVN = 0x0;
                            break;

                        case (1 << CTRL_UNFRZ_MDE_CLR_BIT):
                            g_controller.C1_UNFRZ_EVN = 0x0;
                            break;

                        case (1 << CTRL_TRG_INT_PRC_BIT):
                            /* IPI trigger. FIXED: previously raised R_INT_RQS, a
                             * Windows-only invention with no VHDL equivalent (grtos.vhd has no
                             * such register) that GRTOS_Halt_Processor_Wait() never checks --
                             * so a triggered IPI could never actually wake a processor parked
                             * there. Real hardware (and GRTOS_Halt_Processor_Wait's existing
                             * wake condition) uses R_PRC_INT, masked by ~R_PRC_IRQ_SECTION so a
                             * processor already servicing an IRQ isn't re-triggered:
                             * grtos.vhd:1785-1786, R_PRC_INT_Next <= (R_PRC_INT or R_MSK_1) and
                             * not(R_PRC_IRQ_SECTION). Also clears the target's
                             * R_HLT_IDL_PRC_ENB latch (an interrupt is a wake reason, same
                             * discipline as every other wake site) and wakes the
                             * condition variable so the target responds immediately instead of
                             * waiting out its poll timeout. Confirmed as part of the root cause
                             * of the gk_TCBRUNL_Unlink hang: gm_ProcessorInterrupt() spins
                             * calling this macro waiting for gm_PrcIRQSecGet() (R_PRC_IRQ_SECTION)
                             * to become set by the target -- which requires BOTH this fix (so the
                             * target actually wakes) AND R_PRC_IRQ_SECTION being maintained
                             * (see the ADDR_REG_1 PRC_IRQ_ACK_BIT case and
                             * GRTOS_Halt_Processor_Wait's grant path). The low nibble of R_CTLR
                             * is the 1-based target processor ID. */
                            {
                                unsigned int trg_prc = g_controller.R_CTLR & 0xFF;
                                PRINT_ASSERT(((trg_prc >= 1) && (trg_prc <= (unsigned int) GEMRTOS_DRIVER_NPROCESSORS)), "trg_prc= 0x%x\n",trg_prc);
                                g_controller.R_PRC_INT |= (0x1u << (trg_prc - 1)) & ~g_controller.R_PRC_IRQ_SECTION;
                                g_controller.R_HLT_IDL_PRC_ENB &= ~(0x1u << (trg_prc - 1));
                                EnterCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                                WakeAllConditionVariable(&g_kcb.GeMRTOS_CONDITION_VARIABLE);
                                LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                                GRTOS_Update_controler_status();
                            }
                            break;

                           
                        default:
                            printf("ERROR WR ADDR_CTRL_STATUS bits %u not considered", bits);
                            while(1);
                            break;
                            
                    }
                    g_controller.R_CTLR = 0;
                    LeaveCriticalSection(&g_kcb.GeMRTOS_MUTEX_CONDITION_VARIABLE);
                    break;

                case ADDR_SMP:                       // register 1
                    g_controller.R_SMP.i32[0] = value;
                    g_controller.R_SMP.i32[1] = (G_INT32) 0;
                    break;
                   
                case ADDR_FRZ_THR_HGH:               // register 4
                    g_controller.R_FRZ_TM_THR.i32[0] = (G_INT32) g_controller.R_SMP.i64;
                    g_controller.R_FRZ_TM_THR.i32[1] = (G_INT32) value;
                    break;
                   
                case ADDR_NXT_OCC_TM_HGH:            // register 6
                    g_controller.R_NXT_EVN_CNT.i32[0] = (G_INT32) g_controller.R_SMP.i64;
                    g_controller.R_NXT_EVN_CNT.i32[1] = (G_INT32) value;
                    break;
                    

                default:
                    printf("ERROR WR GEMRTOS_DRIVER_BASE register %d not considered", register_address);
                    while(1);
                    break;
            }
            break;
            
        case GEMRTOS_DRIVER_BASE_GMEMORY:
            gemrtos_gemrtos[register_address] =  value;
            break;

        case GEMRTOS_DRIVER_BASE_TESTING:
            switch (register_address) {
                case ADDR_INOUTS: g_controller.R_INOUTS = value; break;
                default:
                    printf("ERROR WR GEMRTOS_DRIVER_BASE_TESTING register %d not considered", register_address);
                    while(1);
            }
            break;

#ifdef GEMRTOS_DRIVER_BASE_MONITOR
        case GEMRTOS_DRIVER_BASE_MONITOR:
            switch (register_address) {
                case ADDR_RESET_REDIRECT:
                    /* Accepted and deliberately discarded.  On hardware this write
                     * arms the avalon_monitor's CPU-1 reset-vector redirection so a
                     * watchdog reset re-enters the kernel without clearing .bss.
                     * This simulation has no instruction bus to redirect: its own
                     * watchdog emulation (GRTOS_Watchdog_Monitor and
                     * GRTOS_Watchdog_Recovery_Entry) already recovers the processor
                     * with kernel state intact, so nothing here needs the value.
                     * Every other monitor register (bus freeze, address FIFO) stays
                     * unmodelled and still stops the run loudly below, rather than
                     * being accepted and silently ignored. */
                    break;
                default:
                    printf("ERROR WR GEMRTOS_DRIVER_BASE_MONITOR register %d not considered", register_address);
                    while(1);
            }
            break;
#endif /* GEMRTOS_DRIVER_BASE_MONITOR */

        default:
            GRTOS_RUNPRINTF("RUNNING -> %s, %s, %d \n", __FILE__, __FUNCTION__, __LINE__);
            printf("ERROR WR address 0x%x not considered", base);
            while(1);
            break;
    }
	return (G_FALSE);
}
