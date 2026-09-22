/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Master GeMRTOS include â€” provides the full GeMRTOS API and HAL.
 *  \details Include this single header in application code. It transitively
 *           includes gemrtos_core.h (types, sampling macros), gemrtos_primitives.h
 *           (configuration constants), the processor-specific header (Nios II or
 *           Nios V HAL macros), and grtosinterface.h (HAL interface layer).
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


#ifndef GEMRTOS_H_
#define GEMRTOS_H_



#include "system.h"

#include <grtos_regs.h>

/* Only the ISA flag and the BSP register headers live here. Everything
 * that used to be defined in this block (GRTOS_READ_SP/WRITE_SP,
 * GRTOS_DISABLE/ENABLE_PRC_IRQ, G_RETURN_OFFSET_FROM_INTERRUPT,
 * G_STACK_STATUS_LENGTH) moved to gemrtos_core.h 2026-08-18 -- it is
 * CPU-core register access consumed by kernel-core logic (gk_TASK_STK_INIT,
 * gemrtos_kernel.c's dispatch/task-start code), not GeMRTOS Controller IP
 * register access, which is what gemrtos_primitives.h is for. This part
 * must stay here: gemrtos.h's own later #ifdef __niosX_arch__ checks
 * (io.h pre-emption, embedded-only BSP includes, below) need the flag
 * defined before gemrtos_core.h is even reached. */
#ifdef __nios2_arch__
    #define __niosX_arch__
    #include "nios2.h"
#endif

#ifdef __riscv
    #define __niosX_arch__
    #include "intel_niosv.h"
#endif



#include <stdio.h>
#include <stdarg.h>

/* <stdlib.h> is deliberately not included here: malloc() is only used in the
 * pool dynamic-fallback (G_DEBUG_WHILEFOREVER_ENABLE == 1), where
 * gemrtos_core.c includes it conditionally.  <malloc.h> is a GCC extension
 * superseded by <stdlib.h>. */
#include <string.h>  // for memcpy
#include <math.h>


/* Windows-sim only: pre-empt io.h's real Nios II inline-asm IORD/IOWR before
 * altera_avalon_jtag_uart_regs.h below can pull them in. gemrtos_controller_windows.h
 * (included further down, after gemrtos_primitives.h) supplies the
 * Windows-compatible IORD/IOWR/IOWR_32DIRECT/IORD_32DIRECT in their place --
 * this just has to win the race against io.h's own include guard, since by
 * the time gemrtos_controller_windows.h itself runs, io.h has already been
 * processed via this same line for embedded. altera_avalon_jtag_uart_regs.h's
 * own register-bit #defines and IORD_ALTERA_AVALON_JTAG_UART_*(base) macros
 * are unaffected either way -- they reference IORD/IOWR by name and only
 * resolve them where they are themselves used, later. */
#ifndef __niosX_arch__
#define __IO_H__
#endif

#include <altera_avalon_jtag_uart_regs.h>


/* Embedded-only includes â€” Nios BSP headers (alt_irq, alt_exceptions,
 * alt_types) and the exception handler declaration.  These headers do not exist
 * on Windows; the entire block is omitted from the host build. */
#ifdef __niosX_arch__

#include "sys/alt_irq.h"
#include "priv/alt_irq_table.h"
#include <inttypes.h>

#include <sys/alt_exceptions.h>

#include "alt_types.h"
alt_exception_result handler(alt_exception_cause cause, alt_u32 addr, alt_u32 bad_addr );

#endif // __niosX_arch__

/* gemrtos_primitives.h is included HERE, after the BSP headers, not before
 * them. Two of its macros expand IOWR/IORD (gm_IOWR, gm_IORD), which arrive
 * with io.h via <altera_avalon_jtag_uart_regs.h> above; uintptr_t arrives
 * with <inttypes.h>. Any static inline function in that header therefore has
 * both in scope in every translation unit that includes this file --
 * including lwIP sources, which reach it through arch/cc.h and never call
 * those macros at all. Placed before <gemrtos_core.h>, which needs the
 * G_UINT32/G_UINT64 typedefs gemrtos_primitives.h defines.
 *
 * gemrtos_controller_windows.h/grtos_fiber.h follow for the same reason they
 * always did: they need those typedefs. On host builds gemrtos_primitives.h
 * has already pulled them in internally, so this is a harmless second
 * inclusion via the include guard. */
#include <gemrtos_primitives.h>

#ifndef __niosX_arch__                       // Windows version
#include <gemrtos_controller_windows.h>       // Windows version
#include <grtos_fiber.h>                     // Windows version
#endif // NOT  __niosX_arch__                // Windows version

#include <gemrtos_core.h>
#include <gemrtos_resources.h>





// Macro to get the property of a device from its name
#define SERVER_NX(server, param)       server##param
#define DEVICE_BASE(server)            SERVER_NX(server, _BASE)
#define DEVICE_IRQ(server)             SERVER_NX(server, _IRQ)


#endif /* GEMRTOS_H_ */

