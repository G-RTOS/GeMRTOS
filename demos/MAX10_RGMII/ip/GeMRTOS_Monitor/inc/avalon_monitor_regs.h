/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS Avalon-MM monitor register map
 *  \details Register offsets and field encodings for the GeMRTOS Monitor IP
 *           (avalon_monitor.vhd), reached through its own Avalon-MM slave port.
 *  \version 0.9a
 *  \date      2015-2026
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


#ifndef __AVALON_MONITOR_REGS_H__
#define __AVALON_MONITOR_REGS_H__

/* =========================================================================
 * MONITOR REGISTER MAP  —  base: GEMRTOS_DRIVER_BASE_MONITOR
 * Used with gm_IOWR(GEMRTOS_DRIVER_BASE_MONITOR, ADDR_*, data) / gm_IORD(...)
 *
 * This is the avalon_monitor's OWN slave port, distinct from the controller's
 * GEMRTOS_DRIVER_BASE.  Offsets are word offsets (the slave is WORDS-addressed).
 * ========================================================================= */

#define ADDR_FIFO_0             0   // W: R_CTRL          R: selected processor's captured-address FIFO
#define ADDR_FIFO_1             1   // RW: PROCESSOR_SELECTED (which sN port ADDR_FIFO_0 reads back)
#define ADDR_FIFO_2             2   // R: R_CTRL
#define ADDR_FIFO_3             3   // W: reset-vector redirection (see ADDR_RESET_REDIRECT below)

/* R_CTRL bit fields (ADDR_FIFO_0 write / ADDR_FIFO_2 read) */
#define MON_CTRL_FRZ_ON_FULL_BIT   0   // 1: freeze the monitored bus when a FIFO fills
#define MON_CTRL_FRZ_BIT           1   // 1: freeze the monitored bus now
#define MON_CTRL_DRAIN_FIFO_BIT    2   // 1: keep draining the FIFO until empty

/* =========================================================================
 * ADDR_RESET_REDIRECT (== ADDR_FIFO_3, write only) — CPU-1 reset redirection
 *
 * See avalon_monitor.vhd: process Control_register (register writes) and the
 * concurrent m1_address assignment (the redirect itself).  The monitor adds
 * R_RESET_OFFSET to any address CPU 1 presents that lies in the window
 *     [ R_RESET_ADDRESS , R_RESET_ADDRESS + 16 ]
 * and passes every other address through unchanged.  Only CPU 1's master
 * (m1) is affected.
 *
 * Bit 31 of the written word selects which internal register receives it:
 *     0 -> R_RESET_ADDRESS : window base (set this to CPU 1's reset vector)
 *     1 -> R_RESET_OFFSET  : value added to an in-window CPU 1 fetch address
 * Bits [26:0] carry a WORD address = (byte address >> RESET_REDIRECT_ADDR_SHIFT).
 * The hardware reconstitutes byte = payload << 2, so the address supplied must
 * be 4-byte aligned.  Both registers are 0 at reset, so with nothing written
 * the monitor is a transparent bridge and the system behaves exactly as before.
 *
 * WRITE ORDER MATTERS.  Arm R_RESET_ADDRESS first (bit 31 = 0), then
 * R_RESET_OFFSET (bit 31 = 1): while the address settles, the offset is still
 * 0, so an in-window fetch is redirected by +0 (identity) and there is never an
 * instant of wrong redirection.  To disarm, reverse the order.
 *
 * There is no read-back at this offset.
 * ========================================================================= */

#define ADDR_RESET_REDIRECT          3            // == ADDR_FIFO_3

#define RESET_REDIRECT_SEL_BIT       31           // selector bit position
#define RESET_REDIRECT_ADDR_SHIFT    2            // byte address -> word address
#define RESET_REDIRECT_PAYLOAD_MASK  0x07FFFFFFu  // bits [26:0]

#define RESET_REDIRECT_SEL_ADDRESS   0u           // bit 31 = 0 -> R_RESET_ADDRESS
#define RESET_REDIRECT_SEL_OFFSET    1u           // bit 31 = 1 -> R_RESET_OFFSET

/** Build the 32-bit word to write at ADDR_RESET_REDIRECT.
 *  \param sel       RESET_REDIRECT_SEL_ADDRESS or RESET_REDIRECT_SEL_OFFSET
 *  \param byte_val  the 4-byte-aligned window base (for _ADDRESS) or the
 *                   byte delta to add (for _OFFSET); a negative delta is
 *                   fine — it is taken modulo the address space by the adder.
 */
#define RESET_REDIRECT_WORD(sel, byte_val)                                   \
    ( ( (((unsigned int)(byte_val)) >> RESET_REDIRECT_ADDR_SHIFT)            \
        & RESET_REDIRECT_PAYLOAD_MASK )                                      \
      | ( ((unsigned int)(sel)) << RESET_REDIRECT_SEL_BIT ) )


/* =========================================================================
 * CPU-1 reset-vector redirection — arming helpers
 *
 * Two writers, one per internal register (R_RESET_ADDRESS / R_RESET_OFFSET),
 * so callers cannot get the order wrong (see the "WRITE ORDER MATTERS" note
 * above): always call gm_MonitorResetAddressSet() first, then
 * gm_MonitorResetOffsetSet().
 *
 * Both go through gm_IOWR (an uncached, volatile MMIO store):
 *   - the write reaches the monitor even if the calling CPU has a write-back
 *     data cache (a store stuck in a dirty line when a watchdog reset fires
 *     would leave the monitor unarmed);
 *   - volatile forces the compiler to emit ADDRESS-then-OFFSET in program
 *     order, which the "OFFSET is 0 while ADDRESS settles" safety property
 *     depends on, even with the kernel's OPTIMIZE_CODE region active.
 *
 * Include this header AFTER <gemrtos.h> / <system.h> so gm_IOWR and
 * GEMRTOS_DRIVER_BASE_MONITOR are in scope.  The block self-disables when
 * they are not, so the header still compiles stand-alone as a pure map.
 * ========================================================================= */
#if defined(gm_IOWR) && defined(GEMRTOS_DRIVER_BASE_MONITOR)

/** Set R_RESET_ADDRESS — the base of the 16-byte window on CPU 1's master
 *  that gets redirected.  Call before gm_MonitorResetOffsetSet().
 *  \param byte_addr  4-byte-aligned window base (CPU 1's reset vector). */
static inline void gm_MonitorResetAddressSet(unsigned int byte_addr)
{
    gm_IOWR(GEMRTOS_DRIVER_BASE_MONITOR, ADDR_RESET_REDIRECT,
            RESET_REDIRECT_WORD(RESET_REDIRECT_SEL_ADDRESS, byte_addr));
}

/** Set R_RESET_OFFSET — added to any in-window address CPU 1 presents.
 *  Call after gm_MonitorResetAddressSet().
 *  \param byte_offset  signed byte delta (target - window base); a negative
 *                      value is fine, the adder wraps modulo the space. */
static inline void gm_MonitorResetOffsetSet(unsigned int byte_offset)
{
    gm_IOWR(GEMRTOS_DRIVER_BASE_MONITOR, ADDR_RESET_REDIRECT,
            RESET_REDIRECT_WORD(RESET_REDIRECT_SEL_OFFSET, byte_offset));
}

#endif /* gm_IOWR && GEMRTOS_DRIVER_BASE_MONITOR */


#endif /* __AVALON_MONITOR_REGS_H__ */
