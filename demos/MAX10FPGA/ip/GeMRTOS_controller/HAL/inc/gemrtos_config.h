/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS user-facing configuration
 *  \details Single point of entry for the settings a new integrator is
 *           expected to review and tune: static object-pool sizes, task
 *           stack sizes, and compile-time debug/behavior flags. Copy this
 *           file's values (or edit in place) to suit your project; the
 *           rest of gemrtos_primitives.h is kernel-internal and should not
 *           need to be touched.
 *  \version 0.9a
 *  \date      2026-07-14
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

#ifndef GEMRTOS_CONFIG_H_
#define GEMRTOS_CONFIG_H_

/****************************************************************************************
 *                   GRTOS SYSTEM GENERAL DEFINITIONS
 ***************************************************************************************/
#define G_TASK_STACKSIZE            2048  ///< \brief Size of the default stack task
#define G_TASK_STACKSIZE_IDLE       2048  ///< \brief Size of the default idle stack task

/* MISRA C:2012 Rule 21.3: Static object-pool sizes for the embedded target.
 * If the kernel reports pool exhaustion at run-time, increase the relevant
 * constant here and rebuild.  These constants are used only when
 * __niosX_arch__ is defined (i.e. the Nios V embedded build). */
#define G_NUMBER_OF_TCB          64U  ///< \brief Max task control blocks (user + IDLE tasks)
#define G_TASK_STACK_WORDS       G_TASK_STACKSIZE  ///< \brief Words in each static task stack
#define G_NUMBER_OF_ECB          80U  ///< \brief Max event control blocks
#define G_NUMBER_OF_RCB          64U  ///< \brief Max resource control blocks (semaphores + triggers)
#define G_NUMBER_OF_SCB          64U  ///< \brief Max signal control blocks
#define G_NUMBER_OF_RRDS         64U  ///< \brief Max resource-request data structures
#define G_NUMBER_OF_LCB          32U  ///< \brief Max scheduling lists
#define G_NUMBER_OF_PCBAssocLCB  64U  ///< \brief Max processor/scheduling-list associations

/************************************************************************************
 *  COMPILATION OPTIONS
 ************************************************************************************/
/**
 * G_DEBUG_SYSTEM_CHECK_ENABLE
 * Runs a full structural integrity check at kernel start-up.
 * Recommended: 1 during bring-up, 0 in production (one-time cost, negligible).
 * when 1: start-up checks enabled
 * when 0: start-up checks disabled
 */
#define G_DEBUG_SYSTEM_CHECK_ENABLE  1

/**
 * G_DEBUG_WHILEFOREVER_ENABLE
 * Enables runtime PRINT_ASSERT checks, pool-exhaustion warnings, call-stack
 * recording (monitor.c), and the debug counter infrastructure.
 * Recommended: 1 during development, 0 in production (moderate performance cost).
 * when 1: runtime checks enabled
 * when 0: runtime checks disabled (monitor.c functions become no-ops)
 */
#define G_DEBUG_WHILEFOREVER_ENABLE  1

/**
 * G_DEBUG_GET_POINTED_FIELD_CHECK
 * Validates the BLOCK_HASH sentinel on every get_pointed_field() access.
 * HIGH PERFORMANCE COST: adds a validity check before every kernel data
 * structure field read. Set to 0 for any production or performance build.
 * Recommended: 1 during bring-up, 0 in production.
 * when 1: pointer validation on every field read
 * when 0: direct field access (no overhead)
 */
#define G_DEBUG_GET_POINTED_FIELD_CHECK 1

/**
 * G_DEBUG_SET_POINTED_FIELD_CHECK
 * Validates the BLOCK_HASH sentinel on every set_pointed_field() write.
 * HIGH PERFORMANCE COST: adds a validity check before every kernel data
 * structure field write. Set to 0 for any production or performance build.
 * Recommended: 1 during bring-up, 0 in production.
 * when 1: pointer validation on every field write
 * when 0: direct field write (no overhead)
 */
#define G_DEBUG_SET_POINTED_FIELD_CHECK 1

/**
 * GEMRTOS_DEBUG_SAMPLING
 * When defined, enables the per-message sampling dump in the JTAG read server
 * (hellogemrtos.c): on every complete line received, prints the function with
 * the highest accumulated time and a full dump of all G_SAMPLING_MAX_FUNCTIONS
 * entries to stderr.
 * WARNING: enabling this in production distorts real-time behaviour â€” the dump
 * loops over 100 entries and issues ~200 fprintf() calls per JTAG message.
 * Recommended: undefined in production; define only for timing diagnostics.
 */

/**
 * G_DEBUG_WAITING_LOOP
 * Used to force a waiting loop instead of ferezing the processor
 * when 1: waiting loop require for working with debugger
 *         (debugger does not connect when processor is frozen)
 * when 0: processor waits in frozen state
           (the waitrequest signal is asserted to frozen the processor)
 */
#define G_DEBUG_WAITING_LOOP 0

/**
 * GK_CONSOLE_STALL_MS
 * How long the console write may keep waiting for a full JTAG UART transmit
 * FIFO to drain before it abandons the rest of a message. The console NEVER
 * blocks indefinitely: this is the bound that makes that guarantee, and it is
 * expressed in real milliseconds (read from the GeMRTOS controller's own time
 * counter), not in loop iterations, so it keeps its meaning across compilers
 * and clock frequencies.
 *
 * The wait restarts on every byte actually written, so this budget applies to
 * a stall -- a stretch during which the FIFO frees no space at all -- and not
 * to the message as a whole. A host that keeps draining therefore always
 * receives the complete message, however long it is.
 *
 * The trade-off is output fidelity against scheduling latency: the console
 * holds the GeMRTOS mutex while it waits, so every millisecond here delays
 * every other processor.
 *
 * Development (G_DEBUG_WHILEFOREVER_ENABLE == 1): losing a message that was
 * added to diagnose a problem costs far more than a slow boot, so this is
 * deliberately generous. Measured host drain latency on a MAX10 over
 * USB-Blaster was at least 50 ms (the previous 50 ms budget was hit exactly
 * and dropped ~2.5 KB of the boot banner); 2000 ms leaves a large margin over
 * that so bursts such as the start-up banner are not truncated.
 *
 * Production (G_DEBUG_WHILEFOREVER_ENABLE == 0): determinism wins. Console
 * output is diagnostic, and a millisecond of extra mutex-hold time is a real
 * scheduling cost, so the budget is short and output is dropped instead.
 *
 * Note it is bounded even in development. A truly absent host must never hang
 * the system -- not even a development one: an unbounded console wait is the
 * defect this bound exists to prevent, and it cost two full sessions when it
 * wedged a development board. With no host attached the budget is paid once,
 * after which the console latches "host absent" and later messages are
 * discarded at the cost of a single register read.
 */
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    #define GK_CONSOLE_STALL_MS  2000U
#else
    #define GK_CONSOLE_STALL_MS     1U
#endif

#endif /* GEMRTOS_CONFIG_H_ */
