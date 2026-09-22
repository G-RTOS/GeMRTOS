/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS foundation types
 *  \details The lowest layer of the GeMRTOS header hierarchy: the scalar
 *           types, the boolean constants and the register-transfer union that
 *           every other header needs. It depends on nothing and includes
 *           nothing, so it can always be included first.
 *
 *           It exists to break a mutual dependency. gemrtos_primitives.h
 *           (hardware and register access) and gemrtos_core.h (kernel data
 *           structures) each used definitions owned by the other:
 *           primitives needed TIMEPRIORITY from core, while core needed the
 *           scalar typedefs from primitives. That cycle is harmless as long
 *           as every primitive is a macro, because a macro body is only
 *           expanded at its call site, where both headers have been read.
 *           It becomes a compile error the moment a primitive is written as
 *           a static inline function, whose body is compiled where it is
 *           defined. Moving the shared foundation below both makes the
 *           dependency one-way:
 *
 *               gemrtos_config.h   gemrtos_types.h
 *                        \             /    \
 *                         \           /      \
 *                    gemrtos_primitives.h --> gemrtos_core.h
 *
 *  \version 0.9a
 *  \date      2026-08-13
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

#ifndef GEMRTOS_TYPES_H_
#define GEMRTOS_TYPES_H_

/*******************************************************************************
 *  Boolean constants
 *
 *  Kernel code returns these rather than 0/1 so that a truth value is
 *  distinguishable from a count or an index at the point of use.
 *******************************************************************************/
#define G_FALSE             0u
#define G_TRUE              1u

/*******************************************************************************
 *  Compiler attribute shims
 *
 *  G_PRINTF_LIKE tells the compiler that a function takes a printf-style
 *  format string, so that -Wformat checks its arguments at every call site.
 *
 *  This matters more here than in ordinary code. GeMRTOS routes all console
 *  output through its own gu_/gk_ printf family rather than through the C
 *  library, and a function the compiler does not know to be printf-like gets
 *  no argument checking at all -- silently. Without this attribute a wrong
 *  conversion specifier reaches the console as garbage precisely on the paths
 *  where the message is the only diagnostic available, such as PRINT_ASSERT.
 *
 *  \param fmt_index   1-based index of the format-string parameter.
 *  \param first_arg   1-based index of the first variadic argument.
 *
 *  A GCC/Clang extension, and the only one this project relies on for
 *  diagnostics (ISO C17 offers no equivalent). It is purely advisory: it
 *  affects warnings, never code generation, so a compiler that lacks it
 *  builds identical output with less checking.
 *******************************************************************************/
#if defined(__GNUC__)
    #define G_PRINTF_LIKE(fmt_index, first_arg) \
                __attribute__((format(printf, fmt_index, first_arg)))
#else
    #define G_PRINTF_LIKE(fmt_index, first_arg)
#endif

/*******************************************************************************
 *  Scalar types
 *
 *  Fixed-width aliases used throughout GeMRTOS. They are spelled out here
 *  rather than taken from <stdint.h> because this header must remain
 *  includable from contexts that have no C library available, and because the
 *  widths are pinned to the controller's register width rather than to the
 *  host's.
 *******************************************************************************/
typedef unsigned long long G_UINT64;       /**< \brief Unsigned 64 bit quantity  */
typedef unsigned int       G_UINT32;       /**< \brief Unsigned 32 bit quantity  */
typedef int                G_INT32;        /**< \brief Signed 32 bit quantity    */
typedef unsigned int       G_SSTK;         /**< \brief Type for stack pointers   */

typedef unsigned long long gt_time;        /**< \brief System time, in ticks     */

/*******************************************************************************
 *  Register-transfer union
 *
 *  The GeMRTOS controller exposes its 64-bit time registers as two 32-bit
 *  halves. This union performs that split without shifts or masks, and is a
 *  register-access concern rather than a kernel data structure -- which is why
 *  it belongs at this layer and not in gemrtos_core.h, where it originally
 *  sat.
 *
 *  The 4-byte alignment on the 64-bit member is deliberate: the Nios targets
 *  do not require 8-byte alignment for a 64-bit load, and forcing it would
 *  pad every structure that embeds this union.
 *******************************************************************************/
typedef union timepriority TIMEPRIORITY;

union timepriority {
    G_UINT64 i64  __attribute__((aligned(4)));  /**< \brief The value as one 64-bit quantity   */
    G_UINT32 i32[2];                            /**< \brief The same value as two 32-bit halves */
};

#endif /* GEMRTOS_TYPES_H_ */
