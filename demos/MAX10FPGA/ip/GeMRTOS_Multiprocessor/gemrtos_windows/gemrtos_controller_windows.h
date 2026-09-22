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

#ifndef GEMRTOS_CONTROLLER_WINDOWS_H_
#define GEMRTOS_CONTROLLER_WINDOWS_H_

#define __IO_H__

#define IOWR_32DIRECT(base,offset,data)   IOWR((unsigned int)(uintptr_t)(base),(offset)/4,(data)) 
#define IORD_32DIRECT(base,offset)        IORD((unsigned int)(uintptr_t)(base),(offset)/4)

#define GRTOS_ENABLE_PRC_IRQ
#include <stdatomic.h>

#include <pthread.h>
#include <conio.h>
#include <windows.h>
#ifdef s_addr
#undef s_addr
#endif
#include "grtos_fiber.h"

typedef union timepriority TIMEPRIORITY;

extern void GRTOS_Halt_Processor(void);
extern void Reset_GeMRTOS_controller_registers(void);

#undef GRTOS_CMD_HALT_PROCESSOR
#define GRTOS_CMD_HALT_PROCESSOR

#undef GRTOS_CMD_IDLE_PROCESSOR
#define GRTOS_CMD_IDLE_PROCESSOR GRTOS_Halt_Processor();

#undef NO_OP
#define NO_OP

#undef G_IDLE_WAIT_WITH_HALT
#define G_IDLE_WAIT_WITH_HALT 1


#define ALT_NIRQ  12
#define G_NUMBER_OF_GRTOS_REGISTERS 64
#define GRTOS_DISABLE_PROC_INTERRUPTS
#define NIOS2_READ_STATUS(dest) dest=0
#define GRTOS_READ_SP(dest)  dest=0
#define NIOS2_READ_IPENDING(dest)  0
#define NIOS2_READ_IENABLE(status)
#define LASTESTEVENT 0x3FFFFFFFFFFFFFFF
#define PreScale 31

#define G_RETURN_OFFSET_FROM_INTERRUPT 0
#define G_STACK_STATUS_LENGTH  28
#define GRTOS_DISABLE_DEVICES_IRQs                   /// Disable the interrupts
#define GRTOS_DISABLE_PROC_INTERRUPTS                /// Disable processor interrupt
#define G_NUMBER_OF_IDLE_PROCESSORS 0
#define GRTOS_ENABLE_GeMRTOS_IRQ
#define GRTOS_DISABLE_PRC_GENERAL_IRQ


extern void GRTOS_Suspend_Task(void);

extern unsigned int R_LOW_PRI_PRC;     /* Lowest Priority Processor register */
extern unsigned int R_INT_ENB;         /* Interrupt Enable register */
extern unsigned int R_NXT_TM_EVN_PRC;  /* Next Timed Event Processor register */
extern unsigned int R_IRQ_ENB;         /* IRQ Enable register */
extern unsigned int R_IRQ_PND;         /* IRQ Pending register  */
extern unsigned int R_TRG_PRC_INT;     /* Trigger Processor Interrupt register */
extern unsigned int R_TM_PSC;          /* Time Prescale register */
extern unsigned int R_TM_PSC_CNT;      /* Time Prescale counter  */
extern unsigned int R_MTX_NESTED;
extern unsigned int R_HLT_IDL_PRC_ENB;   /* Enable bit to halt processor when idle */
extern unsigned int R_HLT_IDL_PRC_ACT;   /* Active bit for halt processor when idle (enabled and reading the processor port) */
extern unsigned int DEBUG_HOLD;          /* when 1 then debug hold enable, disabled when 0. */
extern unsigned int RESET_HOLD;          /* when 1 then debug hold enable, disabled when 0. */
extern unsigned int HOLD_COUNTER_R_CTLR; /* when 0 then counters are hold. */
extern unsigned int HALT_ENABLED_R_CTLR; /* when 1, halt is disable for eclipse debugging */
extern unsigned int R_CTLR;  /* Control Register  */
extern unsigned int R_STATUS;                /* Status Register */
extern unsigned int C1_FRZ_MDE_ENB;          /* Frozen trigger enabled */
extern unsigned int R_ELP_TM_CNT;	          /* Elapsed Time Counter       */
extern unsigned int R_LST_EVN;               /* Last Event register */
extern unsigned int R_PRC_INT;	              /* Processor Interrupt Register */
extern unsigned int R_INT_CLR;               /* Interrupt Clear Register	 */
extern unsigned int R_PRC_RST;	              /* Processor Reset Register */
extern unsigned int R_PRC_RST_CNT;	          /* Reset Time Counter           */
extern unsigned int R_INT_RQS;  /* Interrupt Request register */
extern unsigned int C1_IRQ_PND;                      /* IRQ pendig flag */
extern unsigned int C1_MTX_GRN;            /* Mutex is Granted to a processor   */
extern unsigned int C1_ENABLE_NEWLIB;            /* Newlib will be Granted to a processor  */
extern unsigned int REG_LEDS;
extern unsigned int C1_FRZ_TM_OCC;                      /* Frozen Threshold has been reached */
extern unsigned int C1_FRZ_TM_OCC_Delayed;                      /* Frozen Threshold has been reached delayed to get edge */
extern G_UINT64 R_NXT_ABS_FRZ_TM;            /* Absolute time frozen event     */
extern unsigned int R_MTX_PRC_GRN;  /* Storage the processor ID that holds the mutex */
extern unsigned int R_MTX_BLK_INT;
extern unsigned int R_MTX_BLK_INT_NXT;
extern unsigned int R1_MTX_DEBUG_RSV;                      /* Flag for mutex reservation for debug */
extern unsigned int C_MTX_PRC_GRN_BIT;  /* Bit decoder logic of granted processor */
extern unsigned int R_MTX_RSV_PRC;  /* Processor Mutex Request Register */
extern unsigned int R_MTX_RSV_PRC_LOWEST_INDEX;  /* Processor requesting mutex with lowest index */
extern unsigned int GATED_CLK;
extern G_UINT64 C_TMP;  /* 64 bit Temporal register */
extern unsigned int C1_NXT_EVN_PRC_ZRO;                      /* No processor is Next Timed Event */
extern unsigned int R_PRC_INT_STA;  /* Processor Interrupt Status register     */
extern unsigned int R_WAITING_IN_ISR;
extern unsigned int R_WAIT_SYS_CALL;
extern G_UINT64 R_PRC_IDLE_TIME[GEMRTOS_DRIVER_NPROCESSORS];    /* Time the processor is in idle state */
extern G_UINT64 R_PRC_WAITING_TIME[GEMRTOS_DRIVER_NPROCESSORS];    /* Time the processor is waiting in the ISR */
extern G_UINT64 R_PRC_MUTEX_TIME[GEMRTOS_DRIVER_NPROCESSORS];    /* Time the processor is granting the mutex     */
extern G_UINT64 R_SYS_MUTEX_TIME; /* Time the system is in critical section */
extern G_UINT64 R_PRC_IDLE_TIME_SAMPLE[GEMRTOS_DRIVER_NPROCESSORS];  /* Register to sample all the R_PRC_IDLE_TIME_SAMPLE time counters */
extern G_UINT64 R_PRC_WAITING_TIME_SAMPLE[GEMRTOS_DRIVER_NPROCESSORS];  /* Register to sample all the R_PRC_WAITING_TIME time counters */
extern G_UINT64 R_SYS_MUTEX_TIME_SAMPLE; /* Register to sample the R_SYS_MUTEX_TIME_SAMPLE time counters  */
extern unsigned int R_MONITOR_REGISTER;
extern unsigned int C_MONITOR_FIFO_READ;
extern unsigned int C_MONITOR_FIFO_WRITE;
extern unsigned int C_MONITOR_FIFO_FULL;
extern unsigned int C_MONITOR_SAMPLE_MISSED;
extern unsigned int C_MONITOR_FIFO_DATAOUT;
extern unsigned int hold_counters;
extern unsigned int slave_gRTOS_address_int;


extern 	void __gcov_dump();


#define C1_EVN_TM_OCC ((g_controller.R_TM_CNT.i64 >= g_controller.R_NXT_EVN_CNT.i64) ? 1 : 0)


void gu_pthread(void *ptcb);
DWORD WINAPI gu_processor(void *v_ptcb);

/* Called from grtos_fiber.c's swapcontext() (the generic resume point for
 * every previously-suspended task) as well as from gu_pthread() itself
 * (first dispatch). See its definition in gemrtos_controller_windows.c for
 * the full rationale. */
void GRTOS_ReleaseKernelMutexIfPending(void);

unsigned int R_MSK_1(unsigned int value);
unsigned int R_MSK_0(unsigned int value);

unsigned int IORD(uintptr_t base, unsigned int register_address);
unsigned int IOWR(uintptr_t base, unsigned int register_address, unsigned int value);

G_UINT32 gk_Get_current_CPUID(void);


#endif /* GEMRTOS_CONTROLLER_WINDOWS_H_ */
