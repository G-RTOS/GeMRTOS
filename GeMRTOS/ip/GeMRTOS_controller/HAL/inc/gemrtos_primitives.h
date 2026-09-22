/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS general definition
 *  \details Main file for definition
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


#ifndef GEMRTOS_PRIMITIVES_H_
#define GEMRTOS_PRIMITIVES_H_

// User-facing configuration (pool sizes, stack sizes, debug/compilation
// flags) lives in gemrtos_config.h -- copy/edit that file to suit your
// project. Everything below this line is kernel-internal.
#include "gemrtos_config.h"
#include "gemrtos_types.h"

/**
 * OPTIMIZE_CODE and OPTIMIZE_RESTORE
 * Optimize the code between them accoding to the optimization setting in the macro.
 */
#define _STRING(S) #S
#define STRING(S) _STRING(S)

/* Platform dispatch — on embedded Nios, OPTIMIZE_CODE / OPTIMIZE_RESTORE
 * wrap source sections with GCC O3 pragmas to maximise throughput on the
 * critical kernel path.  On Windows host builds, both macros expand to nothing
 * and the compiler uses its default optimisation level. */
#ifdef __niosX_arch__
    #define OPTIMIZE_CODE \
        _Pragma("GCC push_options") \
        _Pragma(STRING(GCC optimize ("O3")))

    #define OPTIMIZE_RESTORE \
        _Pragma("GCC pop_options")
#else
	#define OPTIMIZE_CODE   
		
	#define OPTIMIZE_RESTORE   	
#endif


/**
 * set_pointed_field
 * Set the field of a structure checking consistency if it is in debug mode.
 *
 * Must stay a macro (M5, 2026-08-16): `pointer` is a user-facing field-access
 * macro by design (see the project's own naming-convention notes: no gm_
 * prefix on purpose), and `field` is a real struct member name, never an
 * expression -- parenthesising it would break `->field` entirely. `data`
 * was unparenthesised in the debug branch (the one G_DEBUG_SET_POINTED_FIELD_CHECK's
 * default of 1 actually compiles); the non-debug branch already had it.
 * No call site currently passes a compound expression for `data`, so this
 * was latent, not triggered.
 */
#if (G_DEBUG_SET_POINTED_FIELD_CHECK == 1)
	#define set_pointed_field(pointer,field,data) \
				({ \
					PRINT_ASSERT((struct_test(pointer) == G_TRUE),"ERROR STRUCTURE pointer is not valid pointer = %p\n", (void *) pointer); \
					(pointer)->field = (data) ; \
				})
#else
	#define set_pointed_field(pointer,field,data) \
				({ \
					(pointer)->field = (data); \
				})
#endif

/* Must stay a macro (M5, 2026-08-16): `type` is a type name, not a
 * function-convertible expression. `pointer`/`offset` were unparenthesised
 * before `+` -- a real precedence hazard for a compound-expression
 * argument, though no call site currently passes one. */
#define get_pointed_offset(type,pointer,offset) (*(type *) ((char *) (pointer) + (offset)))

/**
 * get_pointed_field 
 * Get the field of a structure checking consistency if it is in debug mode.
 */
#if (G_DEBUG_GET_POINTED_FIELD_CHECK == 1)
	#define get_pointed_field(pointer,field) \
				({ \
					PRINT_ASSERT((struct_test(pointer) == G_TRUE),"ERROR STRUCTURE is not valid\n"); \
					(pointer)->field; \
				})
#else
	#define get_pointed_field(pointer,field) \
				({ \
					(pointer)->field; \
				})	
#endif


// Safe enum assignment
// Produce a HASH for enumeration: CODE produce the hash and CHECK verify it
#define ENUM_HASH_MULTIPLIER  10000

/* ENUM_CODE must stay a macro: every one of its ~50 call sites is an enum
 * member initialiser (gemrtos_core.h, gemrtos_resources.h, gemrtos_trigger.h),
 * and a C enum initialiser must be a compile-time constant expression -- a
 * function call, however trivial, is not one. Already fully parenthesised
 * around every use of hash/value, so the multiple-evaluation hazard this
 * effort has been closing elsewhere cannot bite it: every call site passes
 * a hash constant and a small integer literal, never a side-effecting
 * expression. Recorded here rather than converted (2026-08-16). */
#define ENUM_CODE(hash, value)  ((((unsigned int)(value) + (unsigned int)(hash)) * (unsigned int)ENUM_HASH_MULTIPLIER) + (unsigned int)(value))

/**
 *  \brief Validates that value is a member of the enum type identified by hash.
 *  \param [in] hash  Hash key identifying the target enum type.
 *  \param [in] value Numeric value to validate.
 *  \return G_TRUE if value is a valid member of that enum type, G_FALSE otherwise.
 *
 *  A function, not a macro (2026-08-16): unlike ENUM_CODE, no call site is an
 *  enum initialiser (its one call site, gkm_ENUM_IsValid in gemrtos_monitor.c,
 *  is a plain return statement), so nothing prevents converting it. The macro
 *  form used `value`/`hash` unparenthesised before `%`, `-`, and `/` -- a
 *  caller passing a compound expression would have gotten the wrong answer
 *  from operator precedence, though the one real call site only ever passed
 *  plain variables, so this was latent, not triggered.
 */
static inline G_UINT32 ENUM_CHECK(G_UINT32 hash, G_UINT32 value)
{
    const G_UINT32 remainder = value % ENUM_HASH_MULTIPLIER;
    const G_UINT32 quotient  = (value - remainder) / ENUM_HASH_MULTIPLIER;

    return (remainder == (quotient - hash)) ? G_TRUE : G_FALSE;
}

// \brief define macro for IO write
#define gm_IOWR(addr, off, data)  IOWR((uintptr_t)(addr), (off), (data))
// \brief define macro for IO read
#define gm_IORD(addr, off)        IORD((uintptr_t)(addr), (off))

/* Per-processor register bit mask, indexed by processor id - 1. Declared here
 * rather than in gemrtos_core.h because it maps a processor id onto a
 * controller register bit -- a hardware concern -- and the primitives in this
 * header index it. Defined in gemrtos_kernel.c. */
extern const unsigned int PRC_MASK[];


/* This is the definition for Nios32.  */
/* Scalar typedefs moved to gemrtos_types.h, the foundation layer both
 * this header and gemrtos_core.h depend on. */


/* Windows port: gemrtos_controller_windows.h/grtos_fiber.h need the
 * G_UINT64/G_UINT32 typedefs just above, so they cannot be included any
 * earlier than this. They must in turn precede this file's own gemrtos_core.h
 * include below, which needs DWORD/CRITICAL_SECTION/CONDITION_VARIABLE (from
 * <windows.h>, pulled in by gemrtos_controller_windows.h) and ucontext_t (from
 * grtos_fiber.h). This is therefore the one position that satisfies both
 * dependency directions. Embedded builds skip this block entirely. */
#ifndef __niosX_arch__
#include "gemrtos_controller_windows.h"
#include "grtos_fiber.h"
#endif

/************************************************************************************
 *  GRTOS INTERNAL DEFINITIONS
 ************************************************************************************/
// \brief G_IDLE_PRIORITY_MASK mask for the priority when processor is idle
#define G_IDLE_PRIORITY_MASK      0x40000000

// \brief G_IDLE_PRIORITY_MASK mask for the priority when processor is unavailable
#define G_UNAVAILABLE_PCB_MASK    0x80000000

// \brief G_INITIAL_REGISTER_VALUE initial value of the registers initilized into the stack
#define G_INITIAL_REGISTER_VALUE  0

// \brief G_NUMBER_OF_PCB Number of processors of the system
#define G_NUMBER_OF_PCB      GEMRTOS_DRIVER_NPROCESSORS

/// \brief G_TCB_DESCRIPTION_LENGTH defines the maximum number of characters in the TCB description
/// \brief IT SHOULD BE MULTIPLE OF 4 TO AVOID MEMORY UNALIGMENT
#define G_TCB_DESCRIPTION_LENGTH     48

#ifndef __niosX_arch__
/* ALT_NIRQ (the Nios HAL's IRQ count) is defined only by the embedded-only
 * header chain pulled in under __riscv (intel_niosv.h -> sys/alt_irq.h),
 * which host/Windows builds never include. Provide the same value real
 * hardware uses (NIOSV_NIRQ = 16) so G_NUMBER_OF_IRQ_TCB below sizes the
 * trigger table identically on host and embedded. */
#ifndef ALT_NIRQ
#define ALT_NIRQ 16
#endif
#endif

/// \brief G_NUMBER_OF_IRQ_TCB Number of IRQs defined in Qsys/Platform Designer
#define G_NUMBER_OF_IRQ_TCB			         ALT_NIRQ

/// \brief G_TICKS_PER_SECOND 
/// \todo Define a function to get this value during runtime
#define G_TICKS_PER_SECOND  (int)(GEMRTOS_DRIVER_GRTOSFREQUENCY/(GEMRTOS_DRIVER_PRESCALE))

/// \brief G_TICKS_PER_MSECOND 
/// \todo Define a function to get this value during runtime
#define G_TICKS_PER_MSECOND (int)(G_TICKS_PER_SECOND/1000)

/// \brief G_SECONDS_IN_MINUTE is the number of seconds in a minute 
#define G_SECONDS_IN_MINUTE       60

/// \brief G_SECONDS_IN_HOUR is the number of seconds in an hour
#define G_SECONDS_IN_HOUR       3600

/// \brief Event codes for gk_ENTRY_IRQ_HANDLER
#define EVN_CODE_TIMED    0x21
#define EVN_CODE_FROZEN   0x22
#define EVN_CODE_UNFROZEN 0x23

/************************************************************************************
 *  NEWLIB definitions 
 *  from https://www.cs.ccu.edu.tw/~pahsiung/courses/ese/resources/newlib.pdf
 ************************************************************************************/

/**
 * G_DEBUG_WHILEFOREVER
 * Executed when G_DEBUG_WHILEFOREVER_ENABLE is 1 and an inconsistency is found
 */
/* gm_ProcessorId() is defined here, well above the register-command
 * section it belongs to, because G_DEBUG_WHILEFOREVER below expands it.
 * Any static inline function in this header that uses PRINT_ASSERT
 * therefore needs it already defined at that point. Converting it to a
 * function (M6-reversal, 2026-08-16) does not change this: it stays first,
 * calls only gm_IORD (which stays a macro), so nothing here depends on
 * anything not yet defined at this point in the file. */
/** \fn gm_ProcessorId()
 *  \brief The gm_ProcessorId() function retrieves the ID of the current processor. This function is useful for identifying the processor.
 *  \exception par_following The gm_ProcessorId() function does not accept any parameters.
 *  \exception class_name Processor
 *  \exception prototype G_UINT32 prcID = gm_ProcessorId()
 *  \return The gm_ProcessorId() function returns the ID of the current processor.
 *  \sa gm_PrcInterruptEnable, gm_PrcInterruptDisable, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq, gm_ProcessorHalt
 *  \remark gm_ProcessorId()
 */
static inline G_UINT32 gm_ProcessorId(void)
{
    return gm_IORD(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_1);
}

/* Declared in gemrtos_core.h, which is included after this header;
 * the diagnostic macros below call them, so they need declarations here too. */
G_UINT32 gkm_PrintFunctionStack(void);
void gk_printf(const char *format, ...)  G_PRINTF_LIKE(1, 2);
void gk_fprintf(const char *format, ...) G_PRINTF_LIKE(1, 2);
void gu_printf(const char *format, ...)  G_PRINTF_LIKE(1, 2);
void gu_fprintf(const char *format, ...) G_PRINTF_LIKE(1, 2);

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	#define G_DEBUG_WHILEFOREVER \
		do{volatile int dest = 0; \
			gk_printf("ERROR in for Proc %d in function %s, file = %s, line = %d \n",gm_ProcessorId() , __FUNCTION__,__FILE__,__LINE__); \
			while(1){dest++;\
		} (void) dest; }while(0)
#else
	#define G_DEBUG_WHILEFOREVER  do { while(1); } while(0)
#endif

/**
 * GU_FPRINTF redefines fprintf for use when G_DEBUG_WHILEFOREVER_ENABLE == 1
 * PRINT_ASSERT lets assert a condition, printa a message, the line where it happened and stop execution
 *
 * Both emit through the non-blocking kernel console (gk_printf/gk_fprintf)
 * rather than through the C library. The library route spins inside the
 * Altera SMALL JTAG UART driver until a host drains the FIFO, which on this
 * path would hang the whole system while it reports a defect -- and does so
 * unrecoverably, because the controller suspends a processor's watchdog
 * countdown while that processor owns the mutex.
 *
 * GU_FPRINTF is a *policy* wrapper over the public console functions, not a
 * second implementation of them. It contributes exactly two things on top of
 * gu_printf()/gu_fprintf(): it compiles to nothing when
 * G_DEBUG_WHILEFOREVER_ENABLE is 0, and it asserts that the caller does not
 * already hold the mutex. Everything else -- acquiring the critical section,
 * formatting, writing -- is delegated, so the mutex handling exists in exactly
 * one place (gu_printf/gu_fprintf in gemrtos_hal.c). It previously duplicated
 * that sequence inline; identical behaviour, two copies to keep in step.
 *
 * Its `stream` argument is evaluated only to select the destination console:
 * GeMRTOS exposes exactly two (stdout and stderr are separate JTAG UART
 * instances), and anything that is not stdout goes to stderr. The argument is
 * kept so existing call sites read as fprintf does.
 *
 * Note what the G_DEBUG_WHILEFOREVER_ENABLE gate means for a production build:
 * every GU_FPRINTF message disappears, including genuine error reporting such
 * as "[ERROR] IP validation FAILED" and "Fatal error initializing". Call sites
 * whose output must survive into production should call gu_fprintf() directly.
 *
 * PRINT_ASSERT deliberately does NOT take the mutex, and therefore calls
 * gk_printf outside gk_printf's documented precondition. This is intentional
 * and is the only defensible choice here: at every site where the assert can
 * actually fire, the mutex is in the state the code did *not* expect --
 * GRTOS_CMD_CRITICAL_SECTION_GET's assert fires precisely because acquiring
 * the mutex failed -- so acquiring it on this path would reintroduce the very
 * hang this construction removes. The residual cost is a race on the shared
 * format buffer if two processors assert at the same instant, in which case
 * both are already terminal. gkm_PrintFunctionStack(), called immediately
 * below, has had the same relaxed precondition since it was converted.
 *
 * PRINT_ASSERT writes to stdout and gkm_PrintFunctionStack() to stderr, i.e.
 * a failed assertion is split across the two physical UARTs. That predates
 * this change and is preserved; unifying it would be an observable change.
 */
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)

    #define GU_FPRINTF(stream, ...) do { \
            PRINT_ASSERT((!(gm_IsPrcGranted())), "ERROR in Mutex"); \
            ((((stream) == stdout) ? gu_printf : gu_fprintf))(__VA_ARGS__); \
        } while(0)


    #define PRINT_ASSERT(condition,...)  \
        do { \
            if (!(condition)) { \
                gk_printf( __VA_ARGS__); \
                gkm_PrintFunctionStack( ); \
                G_DEBUG_WHILEFOREVER; \
            } \
        } while(0)

#else

    /* Same parameter list as the enabled branch on purpose: an arity that
     * differs between configurations lets a call site compile in one build
     * and fail in the other.
     *
     * Both expand to ((void) 0) rather than to nothing, so that a disabled
     * call is still a *statement*. Expanding to nothing turns
     *     if (cond) GU_FPRINTF(stderr, "...");
     * into `if (cond) ;` -- which GCC reports as -Wempty-body and which is a
     * live dangling-statement hazard the moment such an `if` gains an `else`.
     * This is the same macro-hygiene rule the critical-section pair already
     * follows via do{}while(0); it was simply never applied to this branch,
     * because the production configuration had never been compiled.
     *
     * Note that `condition` is deliberately NOT evaluated here -- compiling
     * the check out is the entire point. A call site that puts a side effect
     * inside the condition therefore loses that side effect in production; see
     * gk_ECBAEL_Remove() in gemrtos_list.c, where exactly that removed a
     * loop bound. Keep side effects out of PRINT_ASSERT conditions. */
    #define GU_FPRINTF(stream, ...)      ((void) 0)

    #define PRINT_ASSERT(condition,...)  ((void) 0)

#endif


/************************************************************************************
 *  GRTOS COMMANDS
 ************************************************************************************/

/* The following register accessors were converted from macros to
 * static inline functions 2026-08-16: each is a
 * single-expression wrapper around gm_IOWR/gm_IORD (kept as macros) with
 * no cross-references to anything else in this batch, so none has an
 * ordering dependency on any other. Converting was judged worth doing for
 * a production codebase despite zero defects found -- type-checked return
 * values and parameters, working IDE navigation/hover-docs, and consistency
 * with the primitives elsewhere in this header that already made the same
 * change (gm_MutexNestedValue, gm_TriggerPrcIRQ, etc.) -- rather than for
 * fixing a hazard, none of which existed here. Behaviour is unchanged. */

/** \fn gm_WriteOutputs
 *  \brief The gm_WriteOutputs function transfer the data input to the gemrtos_phy output conduit of GeMRTOS controller.
 *  \exception par_following The gm_WriteOutputs function requires the following parameter:
 *  \exception class_name System
 *  \exception prototype gm_WriteOutputs
 *  \param [in] data Data to be transfered the the gemrtos_phy output conduit of GeMRTOS controller.
 *  \return The gm_WriteOutputs function does not return any value.
 *  \remark gm_WriteOutputs
 */
static inline void gm_WriteOutputs(G_UINT32 data)
{
    gm_IOWR(GEMRTOS_DRIVER_BASE_TESTING, ADDR_INOUTS, data);
}

/** \fn gm_ReadInputs()
 *  \brief The gm_ReadInputs() function transfer the data from the gemrtos_phy input conduit of GeMRTOS controller.
 *  \exception par_following The gm_ReadInputs() function does not require any parameter.
 *  \exception class_name System
 *  \exception prototype gm_ReadInputs()
 *  \return The gm_ReadInputs() does returns the data from the gemrtos_phy input conduit of GeMRTOS controller.
 *  \remark gm_ReadInputs()
 */
static inline G_UINT32 gm_ReadInputs(void)
{
    return gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_INOUTS);
}


/// \brief gm_PrcMutexGranted() returns the CPUID of the processor granting the GRTOS MUTEX
/// \todo Describe better and related with GRTOS controller
static inline G_UINT32 gm_PrcMutexGranted(void)
{
    return (G_UINT32) ((gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_MTX_RST) >> 8) & 0xFFu);
}


/// \brief gm_IsPrcGranted() returns G_TRUE if processor grants the GeMRTOS controller Mutex
static inline G_UINT32 gm_IsPrcGranted(void)
{
    return ((gm_IORD(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_0) & 2) == 0) ? G_FALSE : G_TRUE;
}


/** \fn gm_SystemTimePrescaleGet()
 *  \brief The gm_SystemTimePrescaleGet() function returns the time prescale. This prescale is used to obtaine the system time unit from the system clock.
 *  \exception par_following The gm_SystemTimePrescaleGet() function has no parameter.
 *  \exception class_name Time_Management
 *  \exception prototype G_UINT64 time = gm_SystemTimePrescaleGet()
 *  \return The gm_SystemTimePrescaleGet() function returns the time prescale.
 *  \remark gm_SystemTimePrescaleGet()
 *  \relates Time_Management
 */
static inline G_UINT32 gm_SystemTimePrescaleGet(void)
{
    return (gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS) >> 16) & 0x1Fu;
}

/** \fn gm_StatusResetCountingTimeGet()
 *  \brief gm_StatusResetCountingTimeGet() return the status of the time reset counter bit (G_TRUE or G_FALSE). When G_TRUE, the time counters are reset and hold. The time counters are reset and hold reset, when executing gm_TimeCountersReset(). The time counters are release to run again when execution gm_TimeCoutersUnreset().
 *  \exception par_following The gm_StatusResetCountingTimeGet() function has no parameter.
 *  \exception class_name Time_Management
 *  \exception prototype G_UINT64 time = gm_StatusResetCountingTimeGet()
 *  \return G_TRUE if the time counters are held in reset, G_FALSE otherwise.
 *  \remark gm_StatusResetCountingTimeGet()
 *  \relates Time_Management
 */
static inline G_UINT32 gm_StatusResetCountingTimeGet(void)
{
    return (gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS) & (1u << STATUS_HLD_CNT_BIT)) ? G_TRUE : G_FALSE;
}


/** \fn gm_StatusFrozenModeActiveGet()
 *  \brief gm_StatusFrozenModeActiveGet() returns the status of the frozen mode event (G_TRUE if active, G_FALSE if inactive).
 *  \exception par_following gm_StatusFrozenModeActiveGet() has no parameter.
 *  \exception class_name System
 *  \exception prototype G_UINT32 status = gm_StatusFrozenModeActiveGet()
 *  \return G_TRUE if the frozen mode event is active, G_FALSE if inactive.
 *  \remark gm_StatusFrozenModeActiveGet()
 */
static inline G_UINT32 gm_StatusFrozenModeActiveGet(void)
{
    return (gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS) & (1u << STATUS_FRZ_ACT_BIT)) ? G_TRUE : G_FALSE;
}


/** \fn gm_StatusFrozenModeEnableGet()
 *  \brief gm_StatusFrozenModeEnableGet() returns the status of the enable of the frozen mode (G_TRUE if enabled, G_FALSE if disabled). The execution of gm_FrozenModeEnable() enables the frozen mode. The execution of gm_FrozenModeDisable() disables the frozen mode.
 *  \exception par_following gm_StatusFrozenModeEnableGet() has no parameter.
 *  \exception class_name Frozen_Mode
 *  \exception prototype G_UINT32 status = gm_StatusFrozenModeEnableGet()
 *  \return G_TRUE if frozen mode is enabled, G_FALSE if disabled.
 *  \remark gm_StatusFrozenModeEnableGet()
 */
static inline G_UINT32 gm_StatusFrozenModeEnableGet(void)
{
    return (gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS) & (1u << STATUS_FRZ_ENB_BIT)) ? G_TRUE : G_FALSE;
}


/** \fn gm_StatusCountingTimeGet()
 *  \brief gm_StatusCountingTimeGet() returns the status of the TIME_HOLD bit (G_TRUE or G_FALSE). The execution of gm_TimeCountersHold() holds the time counters in its current state. The execution of gm_TimeCountersUnhold() unholds the time counters.
 *  \exception par_following gm_StatusCountingTimeGet() has no parameter.
 *  \exception class_name Time_Management
 *  \exception prototype G_UINT64 time = gm_StatusCountingTimeGet()
 *  \return G_TRUE if the TIME_HOLD bit is set, G_FALSE otherwise.
 *  \remark gm_StatusCountingTimeGet()
 */
static inline G_UINT32 gm_StatusCountingTimeGet(void)
{
    return (gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS) & (1u << STATUS_TIME_HOLD_BIT)) ? G_TRUE : G_FALSE;
}

/// \brief gm_GetEventOccurrence() returns the event happened from the GRTOS controller
/// \todo Describe better and related with GRTOS controller
static inline G_UINT32 gm_GetEventOccurrence(void)
{
    return gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS) & 0xFFu;
}

/// \brief gm_GetIRQStatus() gets the status of the External IRQ register
/// \todo Describe better and related with GRTOS controller
/// \todo Check if it is required
static inline G_UINT32 gm_GetIRQStatus(void)
{
    return gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_IRQ_RQS);
}


/// \brief gm_SetNextOccurrenceTime Sets the Next Occurence Time register of the GRTOS controller
/// to produce a timed event when system time reaches the value of timeset
/// \todo Describe better and related with GRTOS controller
static inline void gm_SetNextOccurrenceTime(G_UINT64 timeset)
{
    TIMEPRIORITY temp_aux;

    PRINT_ASSERT(gm_IsPrcGranted(),
                 "ERROR SIGNAL in critical section\n");

    temp_aux.i64 = timeset;

    /* LOW must be written before HIGH; HIGH commits the 64-bit value. */
    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_SMP,
            (unsigned long)temp_aux.i32[0]);

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_NXT_OCC_TM_HGH,
            (unsigned long)temp_aux.i32[1]);
}



/** \fn gm_ERETAddressSet
 *  \brief The gm_ERETAddressSet function sets the register where the return from interrupt code is stored in the internal registers of the GeMRTOS controller. When such an address is read, it means the processor is returning from interrupt and consequently the GeMRTOS controller Mutex is released, if it is not nested.
 *  \exception par_following The gm_ERETAddressSet has no parameter.
 *  \exception class_name System
 *  \exception prototype gm_ERETAddressSet
 *  \param [in] value The offset in the GeMRTOS Global Region where the return from interrupt instruction is located.
 *  \return The gm_ERETAddressSet function does not return any value.
 *  \remark gm_ERETAddressSet
 */
/* #define gm_ERETAddressSet(value) \gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ((unsigned int) ((G_UINT32)1 << CTRL_ERET_ADDR_BIT ) | (unsigned int) (value))) */
static inline void gm_ERETAddressSet(G_UINT32 value)
{
    const G_UINT32 reg_value =
        ((G_UINT32)1u << CTRL_ERET_ADDR_BIT) | value;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            reg_value);
}

/** \fn gm_KernelModeSet()
 *  \brief The gm_KernelModeSet() function sets the GeMRTOS controller in Kernel Mode. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_KernelModeSet() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_KernelModeSet()
 *  \return The gm_KernelModeSet() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_KernelModeSet()
 */
/* #define gm_KernelModeSet()     gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, (( 1 << CTRL_KERNEL_MODE_BIT ) | 1))*/

static inline void gm_KernelModeSet(void)
{
    const G_UINT32 value =
        (((G_UINT32)1u << CTRL_KERNEL_MODE_BIT) |
        ((G_UINT32)1u));

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}

static inline void gm_WatchogTimeoutSet(G_UINT32 timeout)
{
    const G_UINT32 value =
        (((G_UINT32)1u << CTRL_WATCHDOG_BIT) |
        ((G_UINT32) ((timeout >> G_WATCHDOG_TIMEOUT_SHIFT) & 0xFFF)));

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}



/** \fn gm_KernelModeReset()
 *  \brief The gm_KernelModeReset() function resets the GeMRTOS controller in Kernel Mode. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_KernelModeReset() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_KernelModeReset()
 *  \return The gm_KernelModeReset() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_KernelModeReset()
 */
/* #define gm_KernelModeReset()     gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_KERNEL_MODE_BIT )) */
static inline void gm_KernelModeReset(void)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_KERNEL_MODE_BIT);

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_KernelModeGet()
 *  \brief The gm_KernelModeGet() function returns G_TRUE when GeMRTOS controller is in kernel mode, otherwise it return G_FLASE. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_KernelModeGet() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_KernelModeGet()
 *  \return The gm_KernelModeGet() function returns the Kernel Mode status of the GeMRTOS controller.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_KernelModeGet()
 */
/* #define gm_KernelModeGet()   (((gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS) & ( 1 << STATUS_KERNEL_MODE )) == 0) ? G_FALSE : G_TRUE) */
static inline G_UINT32 gm_KernelModeGet(void)
{
    const G_UINT32 status =
        gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS);

    return ((status & ((G_UINT32)1u << STATUS_KERNEL_MODE)) != 0u)
               ? G_TRUE
               : G_FALSE;
}


/** \fn gm_SystemTimePrescaleSet
 *  \brief The gm_SystemTimePrescaleSet function sets the system clock prescale to get the system time unit. By default, the prescale is set to configure a 10MHz frecuency for system time units. The gm_SystemTimePrescaleGet() function returns the time prescale. 
 *  \exception par_following The gm_SystemTimePrescaleSet has one parameter:
 *  \exception class_name Time_Management
 *  \exception prototype gm_SystemTimePrescaleSet(scale)
 *  \param [in] scale The prescale configuration.
 *  \return The gm_SystemTimePrescaleSet function does not return any value.
 *  \remark gm_SystemTimePrescaleSet
 *  \relates Time_Management
 */
/* #define gm_SystemTimePrescaleSet(scale)   gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_TM_PSC_BIT ) | (scale & 0xFF)); */
static inline void gm_SystemTimePrescaleSet(G_UINT32 scale)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_TM_PSC_BIT) |
        (scale & (G_UINT32)0xFF);

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}

/**
 *  \brief The gm_MutexNestedValue function sets the GeMRTOS controller Mutex nested parameter according to the value. If value is equal to 0, the Mutex is released next time a released is performed. Otherwise, if value is not 0, the the Mutex is not released. 
 *  \exception par_following The gm_MutexNestedValue has one parameter:
 *  \exception class_name System
 *  \exception prototype gm_MutexNestedValue(scale)
 *  \param [in] data If 0 the Mutex released is enabled, otherwise the Mutex released is disabled.
 *  \return The gm_MutexNestedValue function does not return any value.
 *  \remark gm_MutexNestedValue
 *
 *  A function, not a macro: as a macro this was an unbraced if/else, so a
 *  caller could not place it inside its own if/else, and its parameter was
 *  unparenthesised, so an argument containing || or && bound to the internal
 *  comparison and selected the wrong branch. A function has neither problem,
 *  and checks its argument type as well.
 *
 *  This requires IOWR and uintptr_t to be in scope where the body is
 *  compiled, which is why gemrtos.h includes this header after the BSP
 *  headers rather than before them -- see the note at that include.
 */

static inline void gm_MutexNestedValue(G_UINT32 data)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_MTX_RLS_DSB_BIT) |
        ((data != 0u) ? (G_UINT32)1u : (G_UINT32)0u);

    gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, value);
}



/** \fn gm_FrozenModeEnable()
 *  \brief The gm_FrozenModeEnable() function enables the frozen mode event. By default, the frozen mode starts in a disabled state. Before invoking this function, ensure that the frozen threshold is properly configured using the gu_FrozenModeThresholdSet function. The gm_FrozenModeDisable() function disables the frozen mode.
 *  \exception par_following The gm_FrozenModeEnable() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_FrozenModeEnable()
 *  \return The gm_FrozenModeEnable() function does not return any value.
 *  \sa gm_FrozenModeDisable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_FrozenModeEnable()
 */
/* #define gm_FrozenModeEnable()   gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_FRZ_MDE_ENB_BIT ) | 1) */
static inline void gm_FrozenModeEnable(void)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_FRZ_MDE_ENB_BIT) |
        (G_UINT32)1u;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_FrozenModeDisable()
 *  \brief The gm_FrozenModeDisable() function disables the frozen mode event. By default, the frozen mode starts in a disabled state.
 *  \exception par_following The gm_FrozenModeDisable() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_FrozenModeDisable()
 *  \return The gm_FrozenModeDisable() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_FrozenModeDisable()
 */
/* #define gm_FrozenModeDisable()      gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_FRZ_MDE_ENB_BIT )) */
static inline void gm_FrozenModeDisable(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << CTRL_FRZ_MDE_ENB_BIT;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}

/** \fn gm_UnFrozenEventClear()
 *  \brief The gm_UnFrozenEventClear() function clears the unfrozen flag (C1_UNFRZ_EVN). It should be executed when the event is active in order to allow others event to be informed by the GeMRTOS controller. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_UnFrozenEventClear() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_UnFrozenEventClear()
 *  \return The gm_UnFrozenEventClear() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_UnFrozenEventClear()
 */
/* #define gm_UnFrozenEventClear()          gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_UNFRZ_MDE_CLR_BIT )) */
static inline void gm_UnFrozenEventClear(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << CTRL_UNFRZ_MDE_CLR_BIT;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_FrozenEventClear()
 *  \brief The gm_FrozenEventClear() function clears the frozen flag (C1_FRZ_EVN). It should be executed when the event is active in order to allow others event to be informed by the GeMRTOS controller. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_FrozenEventClear() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_FrozenEventClear()
 *  \return The gm_FrozenEventClear() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_FrozenEventClear()
 */
/* #define gm_FrozenEventClear()        gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_FRZ_MDE_CLR_BIT )) */
static inline void gm_FrozenEventClear(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << CTRL_FRZ_MDE_CLR_BIT;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}

/** \fn gm_TimeCoutersUnreset()
 *  \brief The gm_TimeCoutersUnreset() function unresets the System Time Register for running. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_TimeCoutersUnreset() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_TimeCoutersUnreset()
 *  \return The gm_TimeCoutersUnreset() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_TimeCoutersUnreset()
 */
/* #define gm_TimeCoutersUnreset()      gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_TM_CNTS_CLR_BIT )); */
static inline void gm_TimeCoutersUnreset(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << CTRL_TM_CNTS_CLR_BIT;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_TimeCountersReset()
 *  \brief The gm_TimeCountersReset() function resets and holds reset the time counters. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_TimeCountersReset() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_TimeCountersReset()
 *  \return The gm_TimeCountersReset() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_TimeCountersReset()
 */
/* #define gm_TimeCountersReset()             gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_TM_CNTS_CLR_BIT ) | 1); */
static inline void gm_TimeCountersReset(void)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_TM_CNTS_CLR_BIT) |
        (G_UINT32)1u;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_TimeCountersResetGet()
 *  \brief The gm_TimeCountersResetGet() function returns 1 when time is in reset state. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_TimeCountersResetGet() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_TimeCountersResetGet()
 *  \return The gm_TimeCountersResetGet() return 1 when tiem is in reset mode, 0 otherwise.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_TimeCountersReset()
 */
/* #define gm_TimeCountersResetGet()   ( ((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS) >> STATUS_TM_CNTS_CLR_BIT ) & 1) */
static inline G_UINT32 gm_TimeCountersResetGet(void)
{
    const G_UINT32 status =
        gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS);

    return ((status & ((G_UINT32)1u << STATUS_TM_CNTS_CLR_BIT)) != 0u)
               ? G_TRUE
               : G_FALSE;
}


/** \fn gm_TimeCountersHold()
 *  \brief The gm_TimeCountersHold() function holds the time counters. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_TimeCountersHold() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_TimeCountersHold()
 *  \return The gm_TimeCountersHold() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_TimeCountersHold()
 */
/* #define gm_TimeCountersHold()         gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_TM_CNTS_HLD_BIT ) | 1) */
static inline void gm_TimeCountersHold(void)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_TM_CNTS_HLD_BIT) |
        (G_UINT32)1u;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_TimeCountersUnhold()
 *  \brief The gm_TimeCountersUnhold() function unholds the time counters running from its current state. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_TimeCountersUnhold() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_TimeCountersUnhold()
 *  \return The gm_TimeCountersUnhold() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_TimeCountersUnhold()
 */
/* #define gm_TimeCountersUnhold()   gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_TM_CNTS_HLD_BIT )) */
static inline void gm_TimeCountersUnhold(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << CTRL_TM_CNTS_HLD_BIT;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}

/** \fn gm_SystemReset()
 *  \brief The gm_SystemReset() function resets the GeMRTOS controller. Is is executed only by the GeMRTOS kernel.
 *  \exception par_following The gm_SystemReset() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_SystemReset()
 *  \return The gm_SystemReset() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_SystemReset()
 */
/* #define gm_SystemReset()        gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_RESET_BIT )) */
static inline void gm_SystemReset(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << CTRL_RESET_BIT;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_GatedClockEnable()
 *  \brief The gm_GatedClockEnable() function enables the output clock of the the GeMRTOS controller. It may be used in special applications.
 *  \exception par_following The gm_GatedClockEnable() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_GatedClockEnable()
 *  \return The gm_GatedClockEnable() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_GatedClockEnable()
 */
/* #define gm_GatedClockEnable()     gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_GTD_CLK_BIT ) | 1) */
static inline void gm_GatedClockEnable(void)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_GTD_CLK_BIT) |
        (G_UINT32)1u;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_GatedClockDisable()
 *  \brief The gm_GatedClockDisable() function enables the output clock of the the GeMRTOS controller. It may be used in special applications.
 *  \exception par_following The gm_GatedClockDisable() function does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_GatedClockDisable()
 *  \return The gm_GatedClockDisable() function does not return any value.
 *  \sa gm_FrozenModeEnable(), gu_FrozenModeThresholdSet, gu_FrozenModeThresholdGet, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_GatedClockDisable()
 */
/* #define gm_GatedClockDisable()           gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_GTD_CLK_BIT )) */
static inline void gm_GatedClockDisable(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << CTRL_GTD_CLK_BIT;

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_IrqDisable
 *  \brief The gm_IrqDisable function disables the specified device interrupt request event (IRQ) in the GeMRTOS controller. The gm_IrqEnable(irq) function enables the irq external interrupt in the GeMRTOS controller. This function is essential for managing interrupt handling and preventing specified DIRQs from triggering.
 *  \exception par_following The gm_IrqDisable function accepts the following parameter:
 *  \exception class_name IRQ_Management
 *  \exception prototype gm_IrqDisable(irq)
 *  \param [in] irq The number of the IRQ to be disabled.
 *  \return The gm_IrqDisable function does not return any value.
 *  \sa gm_IrqEnable, gm_IRQ_STATUS_SET, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_IrqDisable
 */
/* #define gm_IrqDisable(irq)          gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_IRQ_ENB_CLR_BIT ) | ((irq-1) & 0xF)) */
static inline void gm_IrqDisable(G_UINT32 irq)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_IRQ_ENB_CLR_BIT) |
        ((irq - 1u) & 0x0Fu);

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_IrqEnable
 *  \brief The gm_IrqEnable function enables the specified device interrupt request event (DIRQ) in the GeMRTOS controller. This function is crucial for allowing specified DIRQ to trigger. The gm_IrqDisable(irq) function disables the irq external interrupt in the GeMRTOS controller.
 *  \exception par_following The gm_IrqEnable function accepts the following parameter:
 *  \exception class_name IRQ_Management
 *  \exception prototype gm_IrqEnable(irq)
 *  \param [in] irq The number of the DIRQ to be enabled.
 *  \return The gm_IrqEnable function does not return any value.
 *  \sa gm_IrqDisable, gm_IRQ_STATUS_SET, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_IrqEnable
 */
/* #define gm_IrqEnable(irq)          gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_IRQ_ENB_SET_BIT ) | ((irq-1) & 0xF)) */
static inline void gm_IrqEnable(G_UINT32 irq)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_IRQ_ENB_SET_BIT) |
        ((irq - 1u) & 0x0Fu);

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/** \fn gm_TriggerPrcIRQ
 *  \brief The gm_TriggerPrcIRQ function interrupts the processor proc. If proc interrupt is enabled in the GRTOS controller, then the processor is interrupted.
 *  \exception par_following The gm_TriggerPrcIRQ function accepts the following parameter:
 *  \exception class_name IRQ_Management
 *  \exception prototype gm_TriggerPrcIRQ(irq)
 *  \param [in] proc CPU_ID of processor to interrupt.
 *  \return The gm_TriggerPrcIRQ function does not return any value.
 *  \sa gm_IrqDisable, gm_IRQ_STATUS_SET, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gm_TriggerPrcIRQ
 */
/* #define gm_TriggerPrcIRQ(proc)    gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, (( 1 << CTRL_TRG_INT_PRC_BIT ) | (proc & 0xf))); */
static inline void gm_TriggerPrcIRQ(G_UINT32 proc)
{
    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_TRG_INT_PRC_BIT) |
        (proc & 0x0Fu);

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}


/************************************************************************************
 *  GRTOS COMMANDS
 ************************************************************************************/


// GRTOS_CMD_PRC_SP  - returns the Stack Pointer register (SP) of the current processor
#define GRTOS_CMD_PRC_SP  ({ \
        void *StackPointer=(void*)0; \
        GRTOS_READ_SP(StackPointer); \
        (G_UINT32) StackPointer; \
    })


/* FIXED: on the Windows simulation, gm_ProcessorInterrupt's while loop
 * below calls gm_TriggerPrcIRQ every iteration with no delay at all -- each call enters
 * and leaves GeMRTOS_MUTEX_CONDITION_VARIABLE (via gm_IOWR's CTRL_TRG_INT_PRC_BIT case).
 * Windows CRITICAL_SECTION is not FIFO-fair: a thread re-acquiring the same lock this
 * fast, in a tight loop, can starve out another thread (the target processor, inside
 * GRTOS_Halt_Processor_Wait()) that only needs to acquire it once -- observed as a real,
 * reproducible livelock (one processor pinned at ~100% CPU spinning here forever, the
 * other completely starved, near-0% CPU, making no progress at all -- confirmed via
 * per-thread CPU/state inspection, not just log analysis).
 *
 * First attempt used SwitchToThread(), which only yields to another thread ready to run
 * on the SAME core -- with genuine 2-core parallelism (the whole reason this bug exists),
 * the spinning and starved threads are commonly on different cores, so it did nothing;
 * reproduced the identical hang again after that fix. Sleep(1) forces a real OS-level
 * wait instead, giving the scheduler an actual multi-millisecond window regardless of
 * which core the starved thread is on. The loop condition is checked before the sleep,
 * so a first-attempt-success interrupt (the common case) still returns with zero added
 * latency; the cost only applies once contention is real, which is exactly when slowing
 * down is desirable. No-op on real hardware, where this loop waits for an actual
 * asynchronous hardware interrupt acknowledgment and there is no such concept. */
#ifndef __niosX_arch__
    #define __GRTOS_PROC_INTERRUPT_YIELD  Sleep(1);
#else
    #define __GRTOS_PROC_INTERRUPT_YIELD
#endif

/* Host-only bound for gm_ProcessorInterruptTry (see below).
 *
 * On real hardware gm_ProcessorInterrupt() waits for a genuine asynchronous
 * interrupt: the target is preempted wherever it happens to be, reaches
 * gk_ENTRY_IRQ_HANDLER, and acknowledges. The wait always terminates, so the
 * bound does not exist there and the embedded variant below is the unbounded
 * loop, unchanged.
 *
 * The Windows simulation has no asynchronous preemption at all -- fibers
 * switch only at explicit kernel entry points -- so a target executing pure
 * user computation cannot acknowledge until it chooses to call into the
 * kernel. A caller that spin-waits for that acknowledgement while holding the
 * GeMRTOS mutex therefore stalls every processor behind it. The bound turns
 * that system-wide stall into a reportable failure of the individual request.
 *
 * Value: each yield is one Sleep(1), so this is an upper bound in
 * milliseconds. It must be SHORT, and the reason is counter-intuitive: the
 * caller spins while holding the GeMRTOS mutex, and a target that needs the
 * mutex to reach its own kernel entry point therefore cannot acknowledge
 * *until the spin gives up*. Waiting longer does not make the acknowledgement
 * more likely -- it actively delays it, and blocks every other processor for
 * the whole interval. Measured: with a 200-yield bound, 02_tasks_and_delays
 * spent roughly 8 s of a 25 s run inside refused attempts and its 1 s beacon
 * advanced twice.
 *
 * 10 yields is enough for the case the bound exists to serve -- a target
 * already inside the kernel that Windows simply has not scheduled yet --
 * while keeping the mutex-held interval negligible. A target already in its
 * IRQ section acknowledges on the first check, before any sleep, so the
 * common path pays nothing at all. Callers are expected to retry, which is
 * both cheaper and more likely to succeed than one long wait. */
#ifndef __niosX_arch__
    #define G_PROC_INTERRUPT_MAX_YIELDS  10U
#endif

/** \def gm_ProcessorInterruptTry
 *  \brief Bounded variant of gm_ProcessorInterrupt: requests the interrupt of
 *         processor \a proc and reports whether it acknowledged.
 *  \details Identical to gm_ProcessorInterrupt() except that it evaluates to a
 *           result instead of being a statement, and that on the Windows
 *           simulation it gives up after G_PROC_INTERRUPT_MAX_YIELDS attempts.
 *           On the embedded target it is exactly gm_ProcessorInterrupt()
 *           followed by G_TRUE -- the wait is unbounded and cannot fail, so
 *           embedded behaviour is unchanged.
 *
 *           The caller must treat G_FALSE as "the target processor was not
 *           acquired" and abandon whatever operation required it, WITHOUT
 *           modifying any kernel state: the target is still running its task.
 *  \exception par_following The gm_ProcessorInterruptTry macro accepts the following parameter:
 *  \exception class_name Processor
 *  \exception prototype G_UINT32 acquired = gm_ProcessorInterruptTry(proc)
 *  \param [in] proc The ID of the processor to be interrupted.
 *  \return G_TRUE when the target processor announced its IRQ section;
 *          G_FALSE when the request was abandoned (Windows simulation only).
 *  \sa gm_ProcessorInterrupt, gm_PrcInterruptEnable, gm_PrcInterruptDisable, gm_ProcessorId()
 *  \remark gm_ProcessorInterruptTry
 *
 *  Implemented as a statement expression rather than a static inline function
 *  to stay consistent with the surrounding register-access macros and with
 *  GRTOS_CMD_PRC_SP above, and to avoid adding a function definition to a
 *  header whose include order is deliberately constrained.
 */
#ifdef __niosX_arch__
    #define gm_ProcessorInterruptTry(proc) \
        ({ \
            gm_ProcessorInterrupt(proc); \
            (G_UINT32) G_TRUE; \
        })
#else
    #define gm_ProcessorInterruptTry(proc) \
        ({ \
            G_UINT32 _gm_pit_acquired = (G_UINT32) G_TRUE; \
            G_UINT32 _gm_pit_yields   = (G_UINT32) 0; \
            while (gm_PrcIRQSecGet(proc) == 0) { \
                if (_gm_pit_yields >= (G_UINT32) G_PROC_INTERRUPT_MAX_YIELDS) { \
                    _gm_pit_acquired = (G_UINT32) G_FALSE; \
                    break; \
                } \
                gm_TriggerPrcIRQ(proc); \
                __GRTOS_PROC_INTERRUPT_YIELD \
                _gm_pit_yields++; \
            } \
            _gm_pit_acquired; \
        })
#endif


/**
 *  \def gm_ProcessorHalt
 *  \brief The gm_ProcessorHalt macro places the processor into halt mode, effectively stopping its execution until an interrupt is issued for this processor by the GeMRTOS controller. This mode is often used to conserve power or to wait for external events before resuming normal operation. When G_DEBUG_WAITING_LOOP is defined equal to 0, gm_ProcessorHalt is implemented by asserting the waitrequest signal when processor reads the wait bit of ADDR_REG_0 address in the processor memory of the GeMRTOS controller. When G_DEBUG_WAITING_LOOP is defined not equal to 0, gm_ProcessorHalt is implemented as a waiting loop reading the the wait bit of ADDR_REG_0 address in the processor memory of the GeMRTOS controller in order to be complilance with debugging tools that does not support processor halted with waitrequest signal.
 *  \exception par_following The gm_ProcessorHalt macro does not require any parameters.
 *  \exception class_name Processor
 *  \exception prototype gm_ProcessorHalt
 *  \return The gm_ProcessorHalt macro returns control to the calling function when the processor is interrupted and the ISR routine executed, allowing it to resume execution.
 *  \sa gm_PrcInterruptEnable, gm_PrcInterruptDisable, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq, gm_ProcessorId()
 *  \remark gm_ProcessorHalt
 */
#if (G_DEBUG_WAITING_LOOP == 0)
	#define gm_ProcessorHalt  \
		do { \
			gm_HaltIdlePrcSet(); \
			gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_0, 0); \
		} while (0)
#else
	#define gm_ProcessorHalt  \
		do { \
			gk_WAIT_IDLE_TASK(); \
		} while (0)
#endif


/// \brief gm_HaltIdlePrcSet() set to halt the current processor when it is idle
/// \todo Describe better and related with GRTOS controller
/* #define gm_HaltIdlePrcSet()       gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_1, ( 1 << PRC_IDL_ENB_BIT )); */
static inline void gm_HaltIdlePrcSet(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << PRC_IDL_ENB_BIT;

    gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE,
            ADDR_REG_1,
            value);
}


/// \brief gm_PrcIRQSecGet(prc) reads the status of processor if it is waiting to enter in IRQ handler
/// \todo Describe better and related with GRTOS controller
/* #define gm_PrcIRQSecGet(prc) (gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_PRC_IRQ_SECTION) & PRC_MASK[(prc-1)]) */
static inline G_UINT32 gm_PrcIRQSecGet(G_UINT32 prc)
{
    return gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_PRC_IRQ_SECTION) &
           PRC_MASK[prc - 1u];
}

/**
 *  \fn gm_ProcessorInterrupt
 *  \brief gm_ProcessorInterrupt issues an interrupt for the processor with the specified ID and waits until it reaches the ISR and disables its interrupt in the GeMRTOS controller.
 *  \exception par_following The gm_ProcessorInterrupt function accepts the following parameter:
 *  \exception class_name Processor
 *  \exception prototype gm_ProcessorInterrupt(proc)
 *  \param [in] proc The ID of the processor to be interrupted. This specifies which processor will receive the interrupt signal.
 *  \return The gm_ProcessorInterrupt function returns when the target processor disables its interrupt in the GeMRTOS controller.
 *  \sa gm_PrcInterruptEnable, gm_PrcInterruptDisable, gu_ProcessorWaitForIrq, gm_ProcessorHalt, gm_ProcessorId()
 *  \remark gm_ProcessorInterrupt
 *
 *  A function, not a macro (2026-08-16): as a macro, `proc` was evaluated
 *  twice per loop iteration (once in the wait condition, once in the
 *  trigger call) with no parentheses -- a side-effecting argument would
 *  have been evaluated repeatedly and unparenthesised. Both call sites pass
 *  a plain variable, so this was latent, not triggered; a function has
 *  neither problem regardless.
 *
 *  Placed here, after gm_PrcIRQSecGet, not at its original position near
 *  gm_TriggerPrcIRQ: it calls both, and a function may only
 *  reference what is already defined earlier in the same file -- unlike a
 *  macro, which expands at its call site where everything has been read.
 *  gm_TriggerPrcIRQ is defined earlier already; gm_PrcIRQSecGet was not.
 */
static inline void gm_ProcessorInterrupt(G_UINT32 proc)
{
    while (gm_PrcIRQSecGet(proc) == 0u)
    {
        gm_TriggerPrcIRQ(proc);
        __GRTOS_PROC_INTERRUPT_YIELD
    }
}

/**
 *  \fn gm_PrcIRQSecSet()
 *  \brief The gm_PrcIRQSecSet() function set the status of processor as waiting to enter in IRQ handler. It is used only by the GeMRTOS kernel.
 *  \exception par_following The gm_PrcIRQSecSet() function does not require any parameters.
 *  \exception class_name Processor
 *  \exception prototype gm_PrcIRQSecSet()
 *  \return The gm_PrcIRQSecSet() function returns control to the calling function when the processor is interrupted and the ISR routine executed, allowing it to resume execution.
 *  \sa gm_PrcInterruptEnable, gm_PrcInterruptDisable, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq, gm_ProcessorId()
 *  \remark gm_PrcIRQSecSet()
 */
/* #define gm_PrcIRQSecSet()    gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_1, ( 1 << PRC_IRQ_ACK_BIT )) */
static inline void gm_PrcIRQSecSet(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << PRC_IRQ_ACK_BIT;

    gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE,
            ADDR_REG_1,
            value);
}

/// \brief gm_MutexReserveSet() sets the reserve bit of the current processor to grant the mutex
/* #define gm_MutexReserveSet()     gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_1, ( 1 << PRC_MTX_RSV_BIT )) */
static inline void gm_MutexReserveSet(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << PRC_MTX_RSV_BIT;

    gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE,
            ADDR_REG_1,
            value);
}

/**
 *  \fn gm_PrcInterruptEnable()
 *  \brief The gm_PrcInterruptEnable() function enables the processor interrupt from the GeMRTOS controller.
 *  \exception par_following The gm_PrcInterruptEnable() function does not require any parameters.
 *  \exception class_name Processor
 *  \exception prototype gm_PrcInterruptEnable()
 *  \return The gm_PrcInterruptEnable() function does not return any value; it writes the processor interrupt-enable bit directly and returns immediately.
 *  \sa gm_PrcInterruptDisable, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq, gm_ProcessorId()
 *  \remark gm_PrcInterruptEnable()
 */
/* #define gm_PrcInterruptEnable() gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_1, ( 1 << PRC_INT_ENB_BIT )); */
static inline void gm_PrcInterruptEnable(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << PRC_INT_ENB_BIT;

    gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE,
            ADDR_REG_1,
            value);
}

/**
 *  \fn gm_PrcInterruptDisable()
 *  \brief The gm_PrcInterruptDisable() function disables the processor interrupt from the GeMRTOS controller.
 *  \exception par_following The gm_PrcInterruptDisable() function does not require any parameters.
 *  \exception class_name Processor
 *  \exception prototype gm_PrcInterruptDisable()
 *  \return The gm_PrcInterruptDisable() function does not return any value; it writes the processor interrupt-disable bit directly and returns immediately.
 *  \sa gm_PrcInterruptEnable, gm_ProcessorInterrupt, gu_ProcessorWaitForIrq, gm_ProcessorId()
 *  \remark gm_PrcInterruptDisable()
 */
/* #define gm_PrcInterruptDisable() gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_2, ~(( 1 << PRC_INT_ENB_BIT ))); */
static inline void gm_PrcInterruptDisable(void)
{
    const G_UINT32 value =
        ~((G_UINT32)1u << PRC_INT_ENB_BIT);

    gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE,
            ADDR_REG_2,
            value);
}

static inline void gm_PrcWatchdogEnable(void)
{
    const G_UINT32 value =
        (G_UINT32)1u << PRC_WATCHDOG_ENB_BIT;

    gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE,
            ADDR_REG_1,
            value);
}

static inline void gm_PrcWatchdogDisable(void)
{
    const G_UINT32 value =
        ~((G_UINT32)1u << PRC_WATCHDOG_ENB_BIT);

    gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE,
            ADDR_REG_2,
            value);
}

/************************************************************************************
 *  GRTOS CRITICAL SECTION COMMANDS
 ************************************************************************************/
/**  
 *  \brief GRTOS_CMD_CRITICAL_SECTION_GET 
 *  Defines the entry to a critical section to handle system variables and ISR routines.
 *  It asks for critical section and puts the processor in halt mode. 
 *  GRTOS controller enables the processor when section is granted to it.
 *  Only call Kernel functions in a critical section in order to avoid critical section reentrancy
 */
#define GRTOS_CMD_CRITICAL_SECTION_GET \
	do{ \
		while (!(gm_IsPrcGranted())) { \
			gm_MutexReserveSet(); \
			gm_ProcessorHalt; \
		};\
		PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__); \
	} while(0)


/// \brief GRTOS_MTX_RLS releases the mutex from the current processor
#define GRTOS_MTX_RLS    \
    do { \
		PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__); \
        gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_1, ( 1 << PRC_MTX_RLS_BIT )); \
    } while(0)


/* The two operations below read g_kcb, which this header cannot see complete,
 * so they could not be functions in this header. They stay macros deliberately, for three reasons
 * that hold together:
 *
 *  1. This header is what the host test harness's stubs replace wholesale so the kernel
 *     runs single-threaded on the host, and neutralising synchronisation is
 *     exactly what that substitution exists for.
 *  2. They take no parameters, so a function form would add no type checking.
 *  3. Their call convention is function-like: every call site writes
 *     `gm_GeMRTOSCriticalSectionEnter();` with parentheses (143/145 sites
 *     verified 2026-08-15; the last 2 were the bare object-like form and were
 *     converted in the same pass). A bare, parenthesis-less reference to a
 *     function-like macro is not expanded by the preprocessor at all -- it
 *     reaches the compiler as an undeclared identifier and fails to build.
 *     That is the deliberate difference from the `static inline` attempt this
 *     header reverted from: a missed call site here cannot compile into a
 *     silently-skipped critical section, it can only fail loudly.
 *
 * A macro expands at its call site, where the kernel types are complete. Each
 * body is wrapped in do{}while(0) so `gm_GeMRTOSCriticalSectionEnter();` is a
 * single statement -- safe as the sole body of an unbraced if/else, which the
 * bare `if { ... }` form before this was not. */

#define gm_GeMRTOSCriticalSectionEnter()  \
    do { \
        if (!(gm_TimeCountersResetGet())) { \
            GRTOS_CMD_CRITICAL_SECTION_GET; \
            if ((gm_KernelModeGet() == G_FALSE)) { \
                g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED++; \
            } \
        } \
    } while (0)

#define gm_GeMRTOSCriticalSectionExit()  \
    do { \
        if (!(gm_TimeCountersResetGet())) { \
            if ((gm_KernelModeGet() == G_FALSE)) { \
                g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED--; \
                gm_MutexNestedValue(g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED); \
            } \
            GRTOS_MTX_RLS; \
        } \
    } while (0)

		
	

/************************************************************************************
 *  TESTING COMMANDS
 ************************************************************************************/
 
/* Converted from macros to static inline functions 2026-08-16
 * -- same reasoning as the block above: each is a
 * single-expression gm_IORD wrapper with no cross-references, no ordering
 * risk, no defect to fix, converted for type safety and tooling/IDE
 * support consistency in a production codebase. */

/** \fn gm_GetMutexGrantedState()
 *  \brief The gm_GetMutexGrantedState() function returns the value of the Mutex register of the GeMRTOS controller.
 *  \exception par_following The gm_GetMutexGrantedState() function does not accept any parameters.
 *  \exception class_name Critical_Section
 *  \exception prototype gm_GetMutexGrantedState()
 *  \return The gm_GetMutexGrantedState() function returns the value of the Mutex register of the GeMRTOS controller.
 *  \remark gm_GetMutexGrantedState
 */
static inline G_UINT32 gm_GetMutexGrantedState(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_MTX_GRN);
}

static inline G_UINT32 gm_GetPrcWaitinginISR(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_WAITING_IN_ISR);
}

static inline G_UINT32 gm_GetMtxRsv(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_MTX_RSV_PRC);
}

static inline G_UINT32 gm_GetPrcINT(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_PRC_INT);
}

static inline G_UINT32 gm_GetPrcINTEnable(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_PRC_INT_ENB);
}

static inline G_UINT32 gm_GetPrcSlaveIRQ(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_slave_irq);
}

static inline G_UINT32 gm_GetPrcIntSta(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_PRC_INT_STA);
}

static inline G_UINT32 gm_GetPrcIntTimed(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_TIMED_INT);
}

static inline G_UINT32 gm_GetExternaINT(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_EXTERN_INT);
}

static inline G_UINT32 gm_GetNxtEventPrc(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_NXT_TM_EVN_PRC);
}

static inline G_UINT32 gm_GetIRQEnable(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_IRQ_ENB);
}

static inline G_UINT32 gm_GetDIRQI(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_DIRQI);
}

static inline G_UINT32 gm_GetMTXRelease(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_MTX_RLS_BIT);
}

static inline G_UINT32 gm_GetMTXReset(void)
{
    return (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_MTX_RST) & 0xFFu;
}
  

/********************************************************************************* */

#endif /* GEMRTOS_PRIMITIVES_H_ */
