/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS controller register mapping functions
 *  \details This file contains the GeMRTOS controller register mapping functions.
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


#ifndef __GRTOS_REGS_H__
#define __GRTOS_REGS_H__

/* =========================================================================
 * GLOBAL REGISTER MAP  —  base: GEMRTOS_DRIVER_BASE
 * Used with gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_*, data)
 * ========================================================================= */
#define ADDR_CTRL_STATUS         0  // RW  — control/status register (see bit defs below)
#define ADDR_SMP                 1  // RW
#define ADDR_IRQ_RQS             2  // R   — pending IRQ bitmap
#define ADDR_PRC_IRQ_SECTION     3  // R

#define ADDR_FRZ_THR_HGH         4  // RW  — frozen-mode threshold (high word)
#define ADDR_FRZ_TM_HGH          5  // R   — frozen-mode elapsed time (high word)
#define ADDR_NXT_OCC_TM_HGH      6  //  W  — write: set next occurrence time
#define ADDR_TM_CNT_HGH          6  // R   — read:  current tick counter (same offset, dual-use)
#define ADDR_SYS_TM_HGH          7  // R   — system time (high word)

/* =========================================================================
 * TESTING / DEBUG REGISTER MAP  —  base: GEMRTOS_DRIVER_BASE_TESTING
 * Used with gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_*, ...)
 * Note: offsets overlap with GLOBAL MAP — always specify the correct base.
 * ========================================================================= */
#define ADDR_MTX_GRN            0   // R   — mutex granted state
#define ADDR_WAITING_IN_ISR     1   // R   — for testing
#define ADDR_MTX_RSV_PRC        2   // R   — for testing
#define ADDR_PRC_INT            3   // R   — for testing
#define ADDR_PRC_INT_ENB        4   // R   — for testing
#define ADDR_slave_irq          5   // R   — for testing
#define ADDR_PRC_INT_STA        6   // R   — for testing
#define ADDR_TIMED_INT          7   // R   — for testing
#define ADDR_EXTERN_INT         8   // R   — for testing
#define ADDR_NXT_TM_EVN_PRC     9   // R   — for testing
#define ADDR_FOR_TESTING        10   // FOR TESTING
#define ADDR_IRQ_ENB           11   // R   — for testing
#define ADDR_DIRQI             12   // R   — for testing
#define ADDR_MTX_RLS_BIT       13   // R   — for testing
#define ADDR_MTX_RST           14   // R
#define ADDR_INOUTS            15   // RW  — physical I/O conduit

/* =========================================================================
 * CONTROL / STATUS BIT POSITIONS  —  used with ADDR_CTRL_STATUS
 * ========================================================================= */
/* Write bits (set a command) */
#define CTRL_WATCHDOG_BIT       14

/* gm_WatchogTimeoutSet(timeout) stores (timeout >> G_WATCHDOG_TIMEOUT_SHIFT) & 0xFFF
 * into the 12-bit watchdog-timeout field below CTRL_WATCHDOG_BIT. At the shared
 * 10MHz tick base (GRTOS_TICKS_PER_MS, gemrtos_controller_windows.c), shift=16 gives
 * a representable range of ~6.55ms (register=1) to ~26.8s (register=0xFFF) --
 * confirmed with Ricardo (2026-08-25) as the adequate range for a hung-task
 * watchdog. Shared by both platforms: the real controller must reconstruct the
 * threshold using this SAME shift, or the two sides will silently disagree
 * about how long the configured timeout actually is. */
#define G_WATCHDOG_TIMEOUT_SHIFT 16
#define CTRL_HLT_PRC_MTX        15  // internal use only
#define CTRL_KERNEL_MODE_BIT    16
#define CTRL_TM_PSC_BIT         17
#define CTRL_LOW_PRC_BIT        18
#define CTRL_IRQ_ENB_SET_BIT    19
#define CTRL_IRQ_ENB_CLR_BIT    20
#define CTRL_NTX_TM_PRC_BIT     21
#define CTRL_TRG_INT_PRC_BIT    22
#define CTRL_ERET_ADDR_BIT      23
#define CTRL_MTX_RLS_DSB_BIT    24
#define CTRL_FRZ_MDE_ENB_BIT    25
#define CTRL_UNFRZ_MDE_CLR_BIT  26
#define CTRL_FRZ_MDE_CLR_BIT    27
#define CTRL_TM_CNTS_CLR_BIT    28
#define CTRL_TM_CNTS_HLD_BIT    29
#define CTRL_GTD_CLK_BIT        30
#define CTRL_RESET_BIT          31

/* Read bits (status) */
#define STATUS_KERNEL_MODE      25
#define STATUS_TM_CNTS_CLR_BIT  26
#define STATUS_MTX_NESTED       27
#define STATUS_HLD_CNT_BIT      28
#define STATUS_FRZ_ACT_BIT      29
#define STATUS_FRZ_ENB_BIT      30
#define STATUS_TIME_HOLD_BIT    31

/* =========================================================================
 * PROCESSOR REGISTER MAP  —  base: GEMRTOS_DRIVER_PROCESSOR_BASE
 * Used with gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_*, data)
 * ========================================================================= */
#define ADDR_REG_0  0
#define ADDR_REG_1  1
#define ADDR_REG_2  2

/* Processor control bit positions */
#define PRC_HLT_ENB_BIT            0   //  ADDR_REG_0

#define PRC_INT_ENB_BIT            0   //  ADDR_REG_1
#define PRC_MTX_RSV_BIT            1   //  ADDR_REG_1
#define PRC_IRQ_ACK_BIT            2   //  ADDR_REG_1
#define PRC_IDL_ENB_BIT            3   //  ADDR_REG_1
#define PRC_MTX_RLS_BIT            4   //  ADDR_REG_1
#define PRC_WATCHDOG_ENB_BIT       5   //  ADDR_REG_1

/* =========================================================================
 * STACK FRAME OFFSETS  —  used by context-switch assembly code
 * Nios II uses OFFSET_REG_0 .. OFFSET_REG_27  (G_STACK_STATUS_LENGTH = 28)
 * Nios V  uses OFFSET_REG_0 .. OFFSET_REG_33  (G_STACK_STATUS_LENGTH = 34)
 * OFFSET_REG_34 .. OFFSET_REG_39 are reserved for future architectures.
 * ========================================================================= */

#define OFFSET_REG_0        0
#define OFFSET_REG_1        1
#define OFFSET_REG_2        2
#define OFFSET_REG_3        3
#define OFFSET_REG_4        4
#define OFFSET_REG_5        5
#define OFFSET_REG_6        6
#define OFFSET_REG_7        7
#define OFFSET_REG_8        8
#define OFFSET_REG_9        9
#define OFFSET_REG_10      10
#define OFFSET_REG_11      11
#define OFFSET_REG_12      12
#define OFFSET_REG_13      13
#define OFFSET_REG_14      14
#define OFFSET_REG_15      15
#define OFFSET_REG_16      16
#define OFFSET_REG_17      17
#define OFFSET_REG_18      18
#define OFFSET_REG_19      19
#define OFFSET_REG_20      20
#define OFFSET_REG_21      21
#define OFFSET_REG_22      22
#define OFFSET_REG_23      23
#define OFFSET_REG_24      24
#define OFFSET_REG_25      25
#define OFFSET_REG_26      26
#define OFFSET_REG_27      27
#define OFFSET_REG_28      28
#define OFFSET_REG_29      29
#define OFFSET_REG_30      30
#define OFFSET_REG_31      31
#define OFFSET_REG_32      32
#define OFFSET_REG_33      33
#define OFFSET_REG_34      34  /* reserved */
#define OFFSET_REG_35      35  /* reserved */
#define OFFSET_REG_36      36  /* reserved */
#define OFFSET_REG_37      37  /* reserved */
#define OFFSET_REG_38      38  /* reserved */
#define OFFSET_REG_39      39  /* reserved */

#endif /* __GRTOS_REGS_H__ */
