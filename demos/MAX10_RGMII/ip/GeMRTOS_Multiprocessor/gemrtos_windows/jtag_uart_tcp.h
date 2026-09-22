/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Windows-simulation JTAG UART emulation — per-processor TCP console.
 *  \details Replaces the implicit per-processor JTAG UART wiring the embedded BSP's
 *           startup code provides (each Nios processor's stdout/stderr is wired to its
 *           own physical JTAG UART instance, invisible at the C level) with an explicit
 *           per-processor TCP listener on `127.0.0.1:5000+<processor id>`. A terminal
 *           (PuTTY, telnet, nc) attaches like `juart-terminal --instance=N` does on the
 *           board. With nothing attached, output falls back to one shared console with
 *           an `[N]` prefix, so the simulation never blocks waiting for a terminal.
 *           Never compiled for an embedded target.
 *  \version 0.9a
 *  \date      2026
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

#ifndef JTAG_UART_TCP_H_
#define JTAG_UART_TCP_H_

#ifndef __niosX_arch__

/** Base TCP port; processor N listens on JTAG_UART_TCP_BASE_PORT + N,
 *  N = 1..GEMRTOS_DRIVER_NPROCESSORS, matching gm_ProcessorId()'s own
 *  1-based numbering. */
#define JTAG_UART_TCP_BASE_PORT  5000

/**
 *  \brief Write bytes to the given processor's console — the host equivalent
 *         of that processor's own physical JTAG UART instance.
 *  \details Lazily creates and binds that processor's listening socket on
 *           first use. Non-blocking throughout: if a terminal is attached,
 *           bytes are sent to it (best-effort — a full send buffer drops the
 *           write rather than blocking the caller, which holds the GeMRTOS
 *           critical section here exactly as gu_fprintf()/gu_printf() do on
 *           the embedded path); otherwise the bytes go to one shared stdout
 *           console with an "[N] " prefix.
 *  \param [in] processor_id  1-based processor number (gm_ProcessorId()).
 *  \param [in] data          Bytes to write; need not be null-terminated.
 *  \param [in] len           Number of bytes in \p data.
 *  \thread_safety Callers already serialise through the GeMRTOS critical
 *                 section (gu_printf()/gu_fprintf()'s existing
 *                 gm_GeMRTOSCriticalSectionEnter()/Exit), so this function
 *                 itself does no additional locking.
 */
void jtag_uart_tcp_write_processor(unsigned int processor_id, const char *data, unsigned int len);

/**
 *  \brief Non-blocking poll for input typed into the given processor's
 *         attached terminal — the host equivalent of a JTAG UART read.
 *  \details Lazily creates that processor's listening socket on first use,
 *           same as jtag_uart_tcp_write_processor(). With no terminal
 *           attached there is nothing to read, by construction — typing
 *           requires an attached terminal on real hardware too.
 *  \param [in]  processor_id  1-based processor number (gm_ProcessorId()).
 *  \param [out] buf           Destination buffer.
 *  \param [in]  buf_size      Capacity of \p buf, in bytes.
 *  \return Number of bytes written to \p buf (0 if none available or no
 *          terminal is attached).
 *  \thread_safety Intended to be polled from a single dedicated task per
 *                 processor (jtag_uart_read_server()'s host body); does not
 *                 itself need to be called under the GeMRTOS critical
 *                 section, unlike the write path.
 */
unsigned int jtag_uart_tcp_read_processor(unsigned int processor_id, char *buf, unsigned int buf_size);

#endif /* !__niosX_arch__ */

#endif /* JTAG_UART_TCP_H_ */
