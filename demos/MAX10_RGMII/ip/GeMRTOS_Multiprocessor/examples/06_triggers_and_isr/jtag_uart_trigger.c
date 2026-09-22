/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Own copy of the JTAG UART trigger/ISR driver for the
 *         06_triggers_and_isr example (examples are fully
 *         self-contained; no shared code between examples).
 *  \version 1.0
 *  \date      2026-07-31
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

/* Extracted, behaviour-preserving, from the JTAG UART
 * server subsystem in 10_full_demo/hellogemrtos.c -- three tasks working
 * together around three triggers:
 *
 *   jtag_uart_irq_manager  The actual hardware ISR (registered to a trigger
 *                          created with the peripheral's real IRQ number).
 *                          On every JTAG UART interrupt it reads the
 *                          control register and releases whichever of the
 *                          two software triggers below caused it.
 *   jtag_uart_read_server  Registered to trigger_in, created WITH a
 *                          timeout (JTAG_TRIGGER_READ_TIMEOUT_MS). Loops on
 *                          gu_TriggerWait(): the happy path drains the
 *                          receive FIFO and assembles a line; the timeout
 *                          path prints whatever partial line has
 *                          accumulated (or nothing, if idle) -- this is
 *                          the trigger timeout path every example should
 *                          demonstrate, not only the
 *                          happy path.
 *   jtag_uart_write_server Registered to trigger_out, created WITHOUT a
 *                          timeout (an output line, once queued, always
 *                          eventually gets FIFO space). Loops on
 *                          gu_MessageQueueReceive() to pick up outgoing
 *                          lines, gu_TriggerWait() to know when there is
 *                          FIFO space to drain them.
 *
 * gu_TriggerEnableHook() / gu_TriggerDisableHook() re-arm and mask the
 * peripheral's own read/write interrupt-enable bits -- the trigger
 * mechanism does not touch hardware registers on its own; the hook
 * functions are how a peripheral-specific driver plugs itself in.
 */

#include <gemrtos.h>
#include <system.h>
#include "jtag_uart_trigger.h"

#ifndef __niosX_arch__
#include "jtag_uart_tcp.h"  /* host body needs jtag_uart_tcp_write_processor/read_processor */
#endif

/* Defining the name of the JTAG-UART component selected as JTAG-UART
 * server for this example -- only valid as a stand-in for DEVICE_BASE()/
 * DEVICE_IRQ() below. */
#define JTAG_UART_SERVER JTAG_UART_0

static struct jtag_uart_info  jtag_uart_0_info_storage;  /* MISRA C:2012 Rule 21.3: static storage replaces malloc */
struct jtag_uart_info        *jtag_uart_0_info = &jtag_uart_0_info_storage;

/* Driver tasks start 5 s after boot, matching the exact offset already
 * hardware-verified in 10_full_demo -- this extraction changes nothing
 * about the timing this pattern has run under. */
#define JTAG_DRIVER_START_OFFSET_S 5U

#ifndef __niosX_arch__
/* Host bodies. Same treatment as
 * examples/10_full_demo/hellogemrtos.c's jtag_uart_read_server()/
 * write_server() -- this file was extracted from that one (see file
 * header) but never got the host-body update applied to its own copy,
 * which verify_windows.ps1 caught by actually trying to build and run
 * this example on Windows. Enable/disable
 * hooks are no-ops (software triggers still invoke them, just with no
 * hardware register to touch); the read/write server bodies poll/send on
 * this task's own processor's TCP console instead of JTAG UART FIFO
 * registers, still feeding the same pqueue_in/pqueue_out queues unchanged. */
static void jtag_uart_read_server_enable(void *pjtag_info)   { (void) pjtag_info; }
static void jtag_uart_read_server_disable(void *pjtag_info)  { (void) pjtag_info; }
static void jtag_uart_write_server_enable(void *pjtag_info)  { (void) pjtag_info; }
static void jtag_uart_write_server_disable(void *pjtag_info) { (void) pjtag_info; }

static void jtag_uart_read_server(void *pdata)
{
    struct jtag_uart_info *info = (struct jtag_uart_info *) pdata;
    unsigned int index_msg = 0U;
    unsigned int idle_ms = 0U;
    const unsigned int poll_ms = 20U;

    static char buffer_msg[JTAG_UART_BUFFER_MAX_IN] __attribute__((aligned(4)));
    PRINT_ASSERT((info->buffer_length_in <= JTAG_UART_BUFFER_MAX_IN),
        "ERROR buffer_length_in %u exceeds JTAG_UART_BUFFER_MAX_IN", info->buffer_length_in);

    while (1) {
        char data;
        unsigned int got = jtag_uart_tcp_read_processor((unsigned int) gm_ProcessorId(), &data, 1U);

        if (got == 0U) {
            gu_TaskDelay(0, 0, 0, poll_ms);
            idle_ms += poll_ms;
            /* Same timeout-path behaviour as the embedded body's
             * gu_TriggerWait() timeout: with nobody typing, this fires
             * deterministically every JTAG_TRIGGER_READ_TIMEOUT_MS -- the
             * whole point of this example (see file header). */
            if (idle_ms >= JTAG_TRIGGER_READ_TIMEOUT_MS) {
                idle_ms = 0U;
                gu_fprintf("Read timeout in JTAG UART\n");
                if (index_msg != 0U) {
                    buffer_msg[index_msg] = '\0';
                    gu_fprintf("\nINPUT FROM JTAG-UART = %s\n", buffer_msg);
                    index_msg = 0U;
                }
            }
            continue;
        }

        idle_ms = 0U;
        buffer_msg[index_msg++] = data;

        if ((data == '\r') || (data == '\n') || (index_msg >= info->buffer_length_in - 2U)) {
            buffer_msg[index_msg] = '\0';
            gu_fprintf("\nINPUT FROM JTAG-UART = %s\n", buffer_msg);
            gu_MessageQueuePrintf(info->pqueue_in, "%s", buffer_msg);
            index_msg = 0U;
        }
    }
}

static void jtag_uart_write_server(void *pjtag_info)
{
    struct jtag_uart_info *info = (struct jtag_uart_info *) pjtag_info;

    static char buffer_msg[JTAG_UART_BUFFER_MAX_OUT] __attribute__((aligned(4)));
    PRINT_ASSERT((info->buffer_length_out <= JTAG_UART_BUFFER_MAX_OUT),
        "ERROR buffer_length_out %u exceeds JTAG_UART_BUFFER_MAX_OUT", info->buffer_length_out);

    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    gu_MessageQueueSubscribe(ptcb, info->pqueue_out, (void *) buffer_msg, info->buffer_length_out);

    while (1) {
        int size = gu_MessageQueueReceive(info->pqueue_out);
        if (size > 1) {
            jtag_uart_tcp_write_processor((unsigned int) gm_ProcessorId(), buffer_msg, (unsigned int) (size - 1));
        }
    }
}
#else
/**
 * \brief Enables the JTAG UART read interrupt (RE bit). Registered as
 *        trigger_in's enable hook -- called whenever gu_TriggerEnable() is
 *        invoked for it, including every re-arm after a wait completes.
 */
static void jtag_uart_read_server_enable(void *pjtag_info)
{
    struct jtag_uart_info *info = (struct jtag_uart_info *) pjtag_info;

    PRINT_ASSERT((gm_IsPrcGranted()), "ERROR MTX\n");

    unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base);
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base, status | ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK);
    status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base);  /* dummy read: force effect */
}

/**
 * \brief Disables the JTAG UART read interrupt (RE bit). Registered as
 *        trigger_in's disable hook.
 */
static void jtag_uart_read_server_disable(void *pjtag_info)
{
    struct jtag_uart_info *info = (struct jtag_uart_info *) pjtag_info;

    PRINT_ASSERT((gm_IsPrcGranted()), "ERROR MTX\n");

    unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base);
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base, status & ~(ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK));
    status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base);  /* dummy read: force effect */
}

/**
 * \brief Read-server task: assembles lines typed over the JTAG UART and
 *        publishes each complete line to pqueue_in. Demonstrates BOTH the
 *        trigger happy path (RX data arrived) and the timeout path (no RX
 *        activity within JTAG_TRIGGER_READ_TIMEOUT_MS) -- with nobody
 *        typing during an unattended verification run, the timeout path is
 *        the one that fires deterministically, every window, every run.
 */
static void jtag_uart_read_server(void *pdata)
{
    struct jtag_uart_info *info = (struct jtag_uart_info *) pdata;
    unsigned int index_msg = 0U;
    unsigned int uart_data;

    static char buffer_msg[JTAG_UART_BUFFER_MAX_IN] __attribute__((aligned(4)));
    PRINT_ASSERT((info->buffer_length_in <= JTAG_UART_BUFFER_MAX_IN),
        "ERROR buffer_length_in %u exceeds JTAG_UART_BUFFER_MAX_IN", info->buffer_length_in);

    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    gu_TriggerRegisterTask(ptcb, info->trigger_in);

    while (1) {
        if (gu_TriggerWait() != G_TRGStatus_triggered) {
            /* Timeout path: no read interrupt within the window. */
            gu_fprintf("Read timeout in JTAG UART\n");
            if (index_msg != 0U) {
                buffer_msg[index_msg] = '\0';
                gu_fprintf("\nINPUT FROM JTAG-UART = %s\n", buffer_msg);
                index_msg = 0U;
            }
        } else {
            /* Happy path: drain every byte currently in the RX FIFO. */
            while ((uart_data = IORD_ALTERA_AVALON_JTAG_UART_DATA(DEVICE_BASE(JTAG_UART_SERVER))) & ALTERA_AVALON_JTAG_UART_DATA_RVALID_MSK) {
                char data = (char) ((uart_data & ALTERA_AVALON_JTAG_UART_DATA_DATA_MSK) >> ALTERA_AVALON_JTAG_UART_DATA_DATA_OFST);
                buffer_msg[index_msg++] = data;

                if ((data == '\r') || (data == '\n') || (index_msg >= info->buffer_length_in - 2U)) {
                    buffer_msg[index_msg] = '\0';
                    gu_fprintf("\nINPUT FROM JTAG-UART = %s\n", buffer_msg);
                    gu_MessageQueuePrintf(info->pqueue_in, "%s", buffer_msg);
                    index_msg = 0U;
                }
            }
        }
    }
}

/**
 * \brief Enables the JTAG UART write interrupt (WE bit). Registered as
 *        trigger_out's enable hook.
 */
static void jtag_uart_write_server_enable(void *pjtag_info)
{
    struct jtag_uart_info *info = (struct jtag_uart_info *) pjtag_info;

    PRINT_ASSERT((gm_IsPrcGranted()), "ERROR MTX\n");

    unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base);
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base, status | ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK);
    status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base);  /* dummy read: force effect */
}

/**
 * \brief Disables the JTAG UART write interrupt (WE bit). Registered as
 *        trigger_out's disable hook.
 */
static void jtag_uart_write_server_disable(void *pjtag_info)
{
    struct jtag_uart_info *info = (struct jtag_uart_info *) pjtag_info;

    PRINT_ASSERT((gm_IsPrcGranted()), "ERROR MTX\n");

    unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base);
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base, status & ~(ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK));
    status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base);  /* dummy read: force effect */
}

/**
 * \brief Write-server task: subscribes to pqueue_out, then for every
 *        message received drains it into the JTAG UART write FIFO,
 *        waiting on trigger_out (no timeout -- FIFO space always
 *        eventually frees up) whenever the FIFO fills mid-message.
 */
static void jtag_uart_write_server(void *pjtag_info)
{
    struct jtag_uart_info *info = (struct jtag_uart_info *) pjtag_info;

    static char buffer_msg[JTAG_UART_BUFFER_MAX_OUT] __attribute__((aligned(4)));
    PRINT_ASSERT((info->buffer_length_out <= JTAG_UART_BUFFER_MAX_OUT),
        "ERROR buffer_length_out %u exceeds JTAG_UART_BUFFER_MAX_OUT", info->buffer_length_out);

    GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    gu_MessageQueueSubscribe(ptcb, info->pqueue_out, (void *) buffer_msg, info->buffer_length_out);
    gu_TriggerRegisterTask(ptcb, info->trigger_out);

    while (1) {
        int size = gu_MessageQueueReceive(info->pqueue_out);
        int index_msg = 0;

        while (index_msg < size - 1) {
            int space_fifo = (int) (((G_UINT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base) &
                                      (G_UINT32) ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK) >> ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_OFST);

            while ((space_fifo > 0) && (index_msg < size - 1)) {
                IOWR_ALTERA_AVALON_JTAG_UART_DATA(info->jtag_base, buffer_msg[index_msg++]);
                space_fifo--;
            }

            if (index_msg < size - 1) {
                if (gu_TriggerWait() != G_TRGStatus_triggered) {
                    size = 0;  /* timeout: abandon the rest of this message */
                }
            }
        }
    }
}

/**
 * \brief The hardware ISR itself, dispatched by the trigger mechanism for
 *        the JTAG UART's real interrupt line. Reads the control register
 *        once and releases whichever of trigger_in/trigger_out the
 *        interrupt was actually for.
 */
static void jtag_uart_irq_manager(void *pjtag_info)
{
    struct jtag_uart_info *info = (struct jtag_uart_info *) pjtag_info;

    gm_GeMRTOSCriticalSectionEnter();

        volatile G_UINT32 uart_ctrl = (G_UINT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(info->jtag_base);
        volatile G_UINT32 write_interrupt = (uart_ctrl & (G_UINT32) ALTERA_AVALON_JTAG_UART_CONTROL_WI_MSK) >> ALTERA_AVALON_JTAG_UART_CONTROL_WI_OFST;
        volatile G_UINT32 read_interrupt  = (uart_ctrl & (G_UINT32) ALTERA_AVALON_JTAG_UART_CONTROL_RI_MSK) >> ALTERA_AVALON_JTAG_UART_CONTROL_RI_OFST;

        if (write_interrupt == (G_UINT32) 1) {
            PRINT_ASSERT((info->trigger_out->trigger.TRG_Enabled == G_TRUE), "ERROR trigger write is not enabled\n");
            gu_TriggerRelease(info->trigger_out);
        }
        if (read_interrupt == (G_UINT32) 1) {
            PRINT_ASSERT((info->trigger_in->trigger.TRG_Enabled == G_TRUE), "ERROR trigger read is not enabled\n");
            gu_TriggerRelease(info->trigger_in);
        }

    gm_GeMRTOSCriticalSectionExit();
}
#endif  // !__niosX_arch__

G_UINT32 jtag_uart_trigger_init(void)
{
    GS_TCB *ptcb;

    jtag_uart_0_info->trigger_out = gu_TriggerCreate(-1, 0);
    if (jtag_uart_0_info->trigger_out == (GS_RCB *) 0) {
        gu_printf("ERROR: could not create JTAG UART trigger_out\n");
        return (G_FALSE);
    }

    G_UINT64 timeout_trigger_in = gu_ConvertTime(0, 0, 0, JTAG_TRIGGER_READ_TIMEOUT_MS);
    jtag_uart_0_info->trigger_in = gu_TriggerCreate(-1, timeout_trigger_in);
    if (jtag_uart_0_info->trigger_in == (GS_RCB *) 0) {
        gu_printf("ERROR: could not create JTAG UART trigger_in\n");
        return (G_FALSE);
    }

    jtag_uart_0_info->pqueue_out = gu_MessageQueueCreate();
    if (jtag_uart_0_info->pqueue_out == (GS_RCB *) 0) {
        gu_printf("ERROR: could not create JTAG UART pqueue_out\n");
        return (G_FALSE);
    }

    jtag_uart_0_info->pqueue_in = gu_MessageQueueCreate();
    if (jtag_uart_0_info->pqueue_in == (GS_RCB *) 0) {
        gu_printf("ERROR: could not create JTAG UART pqueue_in\n");
        return (G_FALSE);
    }

    jtag_uart_0_info->buffer_length_in  = JTAG_UART_BUFFER_MAX_IN;
    jtag_uart_0_info->buffer_length_out = JTAG_UART_BUFFER_MAX_OUT;
    jtag_uart_0_info->jtag_base = DEVICE_BASE(JTAG_UART_SERVER);

    ptcb = gu_TaskCreate((void *) jtag_uart_write_server, (void *) jtag_uart_0_info, "jtag_uart_write_server");
    if (ptcb == (GS_TCB *) 0) {
        gu_printf("ERROR: could not create jtag_uart_write_server task\n");
        return (G_FALSE);
    }
    gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
    gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);
    gu_TaskRunPrioritySet(ptcb, (G_UINT64) 0);
    gu_TaskStartWithOffset(ptcb, 0, 0, JTAG_DRIVER_START_OFFSET_S, 0);
    gu_TriggerEnableHook(jtag_uart_0_info->trigger_out, jtag_uart_write_server_enable, (void *) jtag_uart_0_info);
    gu_TriggerDisableHook(jtag_uart_0_info->trigger_out, jtag_uart_write_server_disable, (void *) jtag_uart_0_info);

    ptcb = gu_TaskCreate((void *) jtag_uart_read_server, (void *) jtag_uart_0_info, "jtag_uart_read_server");
    if (ptcb == (GS_TCB *) 0) {
        gu_printf("ERROR: could not create jtag_uart_read_server task\n");
        return (G_FALSE);
    }
    gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
    gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);
    gu_TaskRunPrioritySet(ptcb, (G_UINT64) 0);
    gu_TaskStartWithOffset(ptcb, 0, 0, JTAG_DRIVER_START_OFFSET_S, 0);
    gu_TriggerEnableHook(jtag_uart_0_info->trigger_in, jtag_uart_read_server_enable, (void *) jtag_uart_0_info);
    gu_TriggerDisableHook(jtag_uart_0_info->trigger_in, jtag_uart_read_server_disable, (void *) jtag_uart_0_info);

#ifdef __niosX_arch__
    /* Embedded-only -- jtag_uart_irq_manager itself is embedded-only
     * (see its definition above); there is no hardware IRQ on host to
     * create a trigger for. */
    ptcb = gu_TaskCreate((void *) jtag_uart_irq_manager, (void *) jtag_uart_0_info, "jtag_uart_irq_manager");
    if (ptcb == (GS_TCB *) 0) {
        gu_printf("ERROR: could not create jtag_uart_irq_manager task\n");
        return (G_FALSE);
    }
    gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
    gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);
    gu_TaskRunPrioritySet(ptcb, (G_UINT64) 0);

    GS_RCB *jtag_uart_trigger = gu_TriggerCreate(DEVICE_IRQ(JTAG_UART_SERVER), 0);
    if (jtag_uart_trigger == (GS_RCB *) 0) {
        gu_printf("ERROR: could not create jtag_uart_trigger (hardware IRQ)\n");
        return (G_FALSE);
    }
    gu_TriggerRegisterTask(ptcb, jtag_uart_trigger);
    gu_TriggerEnable(jtag_uart_trigger);
#endif  // __niosX_arch__

    return (G_TRUE);
}
