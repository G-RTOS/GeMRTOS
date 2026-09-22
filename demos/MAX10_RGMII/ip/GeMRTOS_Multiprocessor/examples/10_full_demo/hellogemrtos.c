/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS dining philosophers example
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
/* Include the GeMRTOS library */

#include <gemrtos.h>
#include <system.h>


#ifdef __INTEL_ETH_TSE
#define __ALTERA_ETH_TSE
#endif  // __INTEL_ETH_TSE

#ifdef __niosX_arch__
#define INCLUDE_PHILOSOPHERS        1
#define INCLUDE_TASK_PERIODIC       1
#define INCLUDE_TASK_INFINITE_LOOP  1
#define INCLUDE_JTAG_UART_SERVER    1
#define INCLUDE_TSE                 1
#else
#define INCLUDE_PHILOSOPHERS        0
#define INCLUDE_TASK_PERIODIC       1
#define INCLUDE_TASK_INFINITE_LOOP  1
/* The Windows build has a per-processor TCP
 * console (jtag_uart_tcp.c/.h) to read/write, so jtag_uart_read_server()/
 * write_server() -- previously compiled out entirely on host -- now build
 * and run there too, unmodified above the read()/write() seam. */
#define INCLUDE_JTAG_UART_SERVER    1
/* The Windows build has a real netif
 * (pcap_netif.c/.h, Npcap-backed) to hand to lwIP, so the whole TSE/lwIP/
 * httpd/SSI/CGI block below -- previously compiled out entirely on host --
 * now builds and runs there too, unmodified above the netif seam. */
#define INCLUDE_TSE                 1
#endif  // __niosX_arch__


#if (INCLUDE_TSE == 1) && defined(__ALTERA_ETH_TSE)
#include "lwip_main.h"
#ifndef __niosX_arch__
#include "pcap_netif.h"
#endif  // !__niosX_arch__
#endif  // (INCLUDE_TSE == 1) && defined(__ALTERA_ETH_TSE)

// ####################################################################################
// Defining the name of the JTAG-UART component selected as JTAG-UART server
// Only valid when INCLUDE_JTAG_UART_SERVER is equal to 1
#define JTAG_UART_SERVER     JTAG_UART_0


// ####################################################################################
// Defining number of philosopher task
// Only valid when INCLUDE_PHILOSOPHERS is equal to 1
#define NUMBER_PHILOSOPHERS 30

/* Timeout constants - milliseconds, passed as last argument to gu_ConvertTime() */
#define TIMEOUT_FORK_MS         100U  /* fork semaphore acquisition timeout (philosopher task)  */
#define TIMEOUT_LED_MUTEX_MS     10U  /* LED mutex timeout inside philosopher eating block      */
#define TIMEOUT_LED_PERIODIC_MS 600U  /* LED mutex timeout in periodic and infinite-loop tasks  */
#define TIMEOUT_TSE_RECEIVE_MS  250U  /* TSE SGDMA receive trigger timeout                      */
#define TIMEOUT_JTAG_READ_MS    100U  /* JTAG UART read trigger periodic timeout                */
#define DELAY_INFINITE_LOOP_MS_DEFAULT 500U  /* task_infinite_loop() initial delay             */
#define PERIODIC_TASK_PERIOD_MS_DEFAULT 500U /* task_periodic() initial period                 */

/* Web-editable runtime parameters (see gemrtos_web_cgi_set_handler() below).
 * DELAY_INFINITE_LOOP_MS_DEFAULT / PERIODIC_TASK_PERIOD_MS_DEFAULT above are only
 * the boot-time defaults; these variables hold the live, currently-active values. */
static G_UINT32 g_infinite_loop_delay_ms = DELAY_INFINITE_LOOP_MS_DEFAULT;
static GS_TCB  *g_periodic_task_tcb = (GS_TCB *) 0;

/* Link debounce parameters - a single PHY register read is susceptible to
 * transient bounce while a connector mates. LINK_DEBOUNCE_SAMPLES consecutive
 * link-alive reads, LINK_DEBOUNCE_INTERVAL_MS apart, are required before the
 * link is treated as stable (see tse_lwip_init() reconnection loop). */
#define LINK_DEBOUNCE_SAMPLES        5U  /* consecutive stable reads required                     */
#define LINK_DEBOUNCE_INTERVAL_MS   40U  /* delay between debounce reads (5 x 40ms = 200ms total)  */

/* Runtime reconnect robustness (added after a real modem power-cycle on
 * Atum A3 left the interface never reconnecting, while every OS-managed
 * NIC on the same segment reconnected normally -- 2026-09-03).
 *
 * tse_receive_server()'s only view of the PHY at runtime used to be
 * indirect: it re-checked the PHY only after 30 consecutive RX-trigger
 * timeouts (~7.5s of total silence) or an mSGDMA descriptor error. On a LAN
 * busy enough that some frame from another device keeps arriving within
 * that window, a genuine physical link bounce -- e.g. the upstream
 * modem/router rebooting -- can complete before that silence ever
 * accumulates, and is never noticed. An OS network stack does not have
 * this gap: it gets a hardware link-state interrupt regardless of traffic.
 * PHY_POLL_INTERVAL_MS closes it by checking the PHY directly on a fixed
 * schedule, independent of RX activity.
 *
 * The poll is gated on !netif_ip_is_up() at its call site: it runs only
 * while the interface has no DHCP lease, and stops entirely once bound. On
 * a bound, healthy link its once-per-second MDIO read was found to disturb
 * the RGMII receive path on one board (sustained ~10% frame loss, absent
 * before the poll and absent on a second board with identical firmware).
 * The stuck-retrying-DHCP failure it targets is a not-bound state by
 * definition, so nothing this poll is for is lost by the gate. See the
 * call site in tse_receive_server() for the full reasoning. */
#define PHY_POLL_INTERVAL_MS         1000U

/* How many consecutive PHY_POLL_INTERVAL_MS polls must agree the raw link
 * bit no longer matches the last known-stable state before the poll is
 * allowed to call gemrtos_UpdateTSEInfo() at all.
 *
 * Mandatory, not a tuning knob: gemrtos_UpdateTSEInfo() (lwip_main.c) does
 * three separate MDIO reads and, if ANY of them disagrees with its last
 * stored value, unconditionally issues an MDIO write that restarts the
 * PHY's auto-negotiation and blocks for up to
 * TSE_PHY_NEGOTIATION_TIMEOUT_MS (3s) -- a real, physical renegotiation,
 * not a software-only check. That is fine on the two pre-existing call
 * sites (boot and confirmed RX-path anomalies), which are already rare and
 * already gated behind tse_link_bring_up()'s own 5-consecutive-read
 * debounce. Calling it unconditionally once a second, forever -- what the
 * first version of this poll did -- turns any single noisy MDIO read into
 * an unnecessary real link renegotiation, and every renegotiation corrupts
 * whatever frames are in flight at that instant. Confirmed on Atum A3: the
 * first version of this poll produced frequent
 * "AFTER RECEIVE WHILE STATUS response_reg=0x1" reports (a
 * runt/errored-frame indication from the mSGDMA response port) that did not
 * occur before this poll existed. This debounce restores the same
 * "do not trust one read" principle LINK_DEBOUNCE_SAMPLES already applies
 * elsewhere, using a side-effect-free raw read (see tse_raw_link_alive())
 * so the debounce itself never touches the PHY. */
#define PHY_POLL_DEBOUNCE_SAMPLES    3U

/* Consecutive mSGDMA RX response-errors (response_reg != 0), with no good
 * frame received in between, required before a single errored frame is
 * treated as a link-level event.
 *
 * A lone CRC / RX_ER error on an otherwise healthy link is normal Ethernet
 * behaviour -- lwIP already drops a frame it cannot use. The pre-existing
 * handler instead set link_has_changed on the very first errored frame,
 * which drives gemrtos_UpdateTSEInfo() (lwip_main.c): an MDIO burst that
 * briefly makes the RX path deaf. Any frame arriving in
 * that window is itself truncated or CRC-errored and re-triggers the
 * handler -- a self-sustaining loop that turns a ~0.1% raw error rate into
 * ~10% packet loss (reproduced 2026-09-09 after an upstream
 * equipment change).
 *
 * Requiring a run of RX_ERROR_RUN_LIMIT consecutive errors before
 * escalating mirrors the RX-idle trigger's own 30-consecutive-timeout
 * requirement. A genuinely wedged RX path produces the errors back-to-back
 * and trips this within milliseconds at line rate; scattered errors are
 * counted, the frame dropped, and reception continues untouched. Tunable;
 * 12 sits comfortably above the largest error burst seen on a healthy link
 * (~5) and is still negligible latency for a real fault. */
#define RX_ERROR_RUN_LIMIT  12U

/* Defence in depth for whatever PHY_POLL_INTERVAL_MS does not catch
 * (including a cause this code cannot name): if the interface has reported
 * link_alive without ever reaching netif_ip_is_up() for this long, force a
 * full reconnect cycle unconditionally. Set well above DHCP's own
 * worst-case retry/backoff window (lwIP backs off 1,2,4,8,16,32s then caps
 * at 60s between DISCOVER attempts, retrying indefinitely) so a normal, if
 * slow, negotiation is never interrupted by this safety net. */
#define DHCP_NOT_BOUND_TIMEOUT_MINUTES  5U

#define JTAG_UART_BUFFER_MAX_IN  100U  /* MISRA C:2012 Rule 21.3: static max for jtag_uart_read_server buffer  */
#define JTAG_UART_BUFFER_MAX_OUT 100U  /* MISRA C:2012 Rule 21.3: static max for jtag_uart_write_server buffer */

/* !!!  REPLACE THESE WITH YOUR HARDWARE'S ACTUAL MAC ADDRESS  !!!
 * Every board on the network must have a unique MAC address.
 * Using the default values on more than one board causes ARP conflicts.
 * For production use, read the MAC from a NIC register or OTP memory. */
#define TSE_MAC_BYTE_0  0xACU
#define TSE_MAC_BYTE_1  0xEFU
#define TSE_MAC_BYTE_2  0xACU
#define TSE_MAC_BYTE_3  0xEFU
#define TSE_MAC_BYTE_4  0xACU
#define TSE_MAC_BYTE_5  0xEFU
/* Agilex3 override - still replace with real values for production. */
#define TSE_MAC_AGILEX3_BYTE_0  0xCAU
#define TSE_MAC_AGILEX3_BYTE_1  0xFEU




// ####################################################################################
// Global structure for task information storage

// Create a Mutex for LEDs access from multiple tasks
t_semaphore_resource *mutex_leds;
/* leds variable retains the current value written to GeMRTOS outputs */
volatile unsigned int leds;

#if (INCLUDE_JTAG_UART_SERVER == 1)
	#ifndef __niosX_arch__
	#include "jtag_uart_tcp.h"
	#endif  // !__niosX_arch__

	struct jtag_uart_info {
		GS_RCB *trigger_out;
		GS_RCB *trigger_in;
		GS_RCB *pqueue_out;
		GS_RCB *pqueue_in;
		unsigned int buffer_length_out;
		unsigned int buffer_length_in;
		unsigned int jtag_base;
	};

	/* Structure for JTAG_UART information */
	static struct jtag_uart_info  jtag_uart_0_info_storage;        /* MISRA C:2012 Rule 21.3: static storage replaces malloc */
	struct jtag_uart_info        *jtag_uart_0_info = &jtag_uart_0_info_storage;
#endif // INCLUDE_JTAG_UART_SERVER

#if (INCLUDE_TSE == 1) && defined(__ALTERA_ETH_TSE)
#define TSE_TX_FRAME_SIZE  2048U  /* MISRA C:2012 Rule 21.3: replaces malloc(2048) in init_tse_subsystem */
static struct netif      tse_netif_static;                              /* replaces malloc(sizeof(struct netif)) */
static unsigned char     tse_tx_frame_static[TSE_TX_FRAME_SIZE]  __attribute__((aligned(4)));                                        /* replaces malloc(2048) - DMA tx buffer */
#endif  // (INCLUDE_TSE == 1) && defined(__ALTERA_ETH_TSE)


#if (INCLUDE_PHILOSOPHERS == 1)

    /* Semaphore to exclude forks variable access */
    t_semaphore_resource *forks[NUMBER_PHILOSOPHERS];
    #define left_fork(i)  (i)
    #define right_fork(i) ((i + 1) % NUMBER_PHILOSOPHERS)

/******************************************************************/
/* generic task function                                          */
/******************************************************************/
void task_philosopher(void* pdata)
{
	unsigned int task_index = (unsigned int) pdata;
    t_semaphore_resource *first_sem, *second_sem;


	/* get the pointer to the TCB of the current task */
	GS_TCB *ptcb = gu_TaskGetCurrentTCB();
	
	/* get the semaphores in order */
	if (( task_index & 1) == 0) {                   // philosopher is even
		first_sem  = forks[left_fork(task_index)];
		second_sem = forks[right_fork(task_index)];
	} else {                                        // philosopher is odd
		second_sem = forks[right_fork(task_index)];
		first_sem  = forks[left_fork(task_index)];
	}
	G_UINT64 timeout_semaphore = gu_ConvertTime(0, 0, 0, TIMEOUT_FORK_MS);
    
	while (G_TRUE) {
		if (gu_SemaphoreTake(first_sem,  timeout_semaphore) == G_TRUE) {
			if (gu_SemaphoreTake(second_sem,  timeout_semaphore) == G_TRUE) {
				
				
				/* HALF of philosophers write in queue, the rest change LEDs */
				if (task_index > NUMBER_PHILOSOPHERS / 2)
	#if (INCLUDE_JTAG_UART_SERVER == 1)
					gu_MessageQueuePrintf(jtag_uart_0_info->pqueue_out,"%s is eating in processor %d\n", ptcb->TCB_description, gm_ProcessorId());
    #else
                    gu_printf("%s is eating in processor %d\n", ptcb->TCB_description, gm_ProcessorId());
	#endif // INCLUDE_JTAG_UART_SERVER
				else 
				{
                    gu_printf("%s is eating in processor %d\n", ptcb->TCB_description, gm_ProcessorId());
					/* Update LED under mutex (TIMEOUT_LED_MUTEX_MS ms timeout) */
                    G_UINT64 timeout = gu_ConvertTime(0, 0, 0, TIMEOUT_LED_MUTEX_MS); /* Define Mutex timeout (600ms) */
                    if (gu_SemaphoreTake(mutex_leds, timeout) == G_TRUE) {             /* Request LEDs Mutex           */
                        leds = leds + 4;
                        gm_WriteOutputs(leds);
                        gu_SemaphoreGive(mutex_leds);
                    } 
                    else
                    {
                        gu_printf("UNSUCCESSFULL SEMAPHORE TAKEN %s\n", ptcb->TCB_description);
                    }
				}
				gu_SemaphoreGive(second_sem);
			}
			gu_SemaphoreGive(first_sem);
		}
		else
		{
			gu_printf("UNSUCCESSFULL SEMAPHORE TAKEN %s\n", ptcb->TCB_description);
		}
		gu_TaskDelay( 0, 0, 1, 0);                                  /* Supend task 1 second         */
	}
}


/******************************************************************/
/* Signal function when task is aborted (the deadline is missed)  */
/******************************************************************/
void signal_philosopher_task_aborted(void *pdata)
{
	PRINT_ASSERT((!(gm_IsPrcGranted())), "ERROR in task MTX task= %s, TCB_MTX_NESTED= %u\n", g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_description, (unsigned int) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED);
    gu_printf("\nTASK %u ABORTED\n", (unsigned int)(uintptr_t) pdata);
}

#endif   // (INCLUDE_PHILOSOPHERS == 1)

/******************************************************************/
/* Signal function when system goes to frozen mode                */
/******************************************************************/
void sig_frozen_mode(void *pdata)
{
	PRINT_ASSERT(((gm_IsPrcGranted())),"ERROR SIGNAL in critical section\n");
    gu_printf("\nSYSTEM IN FROZEN MODE %u\n", (unsigned int) pdata);
}

#if (INCLUDE_TASK_PERIODIC == 1)

/******************************************************************/
/* periodic task function                                         */
/******************************************************************/
void task_periodic(void* pdata)
{
    
	/* get the pointer to the TCB of the current task */
	GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    gu_printf("task %s running\n", ptcb->TCB_description);

    /* Blink LED under mutex (TIMEOUT_LED_PERIODIC_MS ms timeout) */
    G_UINT64 timeout = gu_ConvertTime(0, 0, 0, TIMEOUT_LED_PERIODIC_MS); /* Define Mutex timeout */
    if (gu_SemaphoreTake(mutex_leds, timeout) == G_TRUE) {                /* Request LEDs Mutex   */
        leds = leds ^ (1 << 0);                                           /* Invert LED 0         */
        gm_WriteOutputs(leds);
        gu_SemaphoreGive(mutex_leds);
    } 
    else
    {
        gu_printf("Semaphore not taken in task %s\n", ptcb->TCB_description);
    }
    
}
#endif  // (INCLUDE_TASK_PERIODIC == 1)


#if (INCLUDE_TASK_INFINITE_LOOP == 1)
/******************************************************************/
/* infinite_loop task function                                          */
/******************************************************************/
void task_infinite_loop(void* pdata)
{
	/* get the pointer to the TCB of the current task */
	GS_TCB *ptcb = gu_TaskGetCurrentTCB();
    
	while(1) {

        gu_printf("task %s running\n", ptcb->TCB_description);

        /* Blink LED under mutex with DELAY_INFINITE_LOOP_MS (TIMEOUT_LED_PERIODIC_MS ms timeout) */
        G_UINT64 timeout = gu_ConvertTime(0, 0, 0, TIMEOUT_LED_PERIODIC_MS); /* Define Mutex timeout */
        if (gu_SemaphoreTake(mutex_leds, timeout) == G_TRUE) {                /* Request LEDs Mutex   */
            leds = leds ^ (1 << 1);                                           /* Invert LED 1         */
            gm_WriteOutputs(leds);
            gu_SemaphoreGive(mutex_leds);
        } 
        else
        {
            gu_printf("Semaphore not taken in task %s\n", ptcb->TCB_description);
        }        
        
        /* Suspend task until next release; g_infinite_loop_delay_ms is re-read
         * every iteration, so a web-triggered update (see
         * gemrtos_web_cgi_set_handler()) takes effect from the very next delay. */
		gu_TaskDelay( 0, 0, 0, g_infinite_loop_delay_ms);
	}
}
#endif // (INCLUDE_TASK_INFINITE_LOOP == 1)

#if (INCLUDE_JTAG_UART_SERVER == 1)


#ifndef __niosX_arch__
/* trigger_in/trigger_out are created with -1 (software, not tied
 * to a hardware interrupt -- see the task-creation code below), so these
 * enable/disable hooks are still registered and still called by the trigger
 * subsystem on host, but there is no hardware register to touch. No-ops. */
void jtag_uart_read_server_enable(void *pjtag_info)  { (void) pjtag_info; }
void jtag_uart_read_server_disable(void *pjtag_info) { (void) pjtag_info; }
#else
/**
 * \brief Enable a JTAG UART interrupt by setting ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK bit in the control register.
 */
void jtag_uart_read_server_enable(void *pjtag_info)
{

	/* Point to the JTAG UART information structure */
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pjtag_info;


	/* Enable JTAG UART read interrupt */
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base);
    
	IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base, status | ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK);

    // Dummy read to take effect (just in case)
	status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base);
    
}

/**
 * \brief Disable a JTAG UART interrupt by clearing ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK bits in the control register.
 */
void jtag_uart_read_server_disable(void *pjtag_info)
{
    
	/* Point to the JTAG UART information structure */
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pjtag_info;	


	/* Disable JTAG UART read interrupt */
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base);

	IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base, status & ~(ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK));

    // Dummy read to take effect (just in case)
    status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base);

}
#endif  // !__niosX_arch__

#ifndef __niosX_arch__
/* Host body. Reads bytes from this task's processor's own TCP
 * console (jtag_uart_tcp.c) instead of polling the mSGDMA-adjacent JTAG UART
 * data register, and buffers/forwards to pqueue_in exactly like the embedded
 * body below -- everything above this point (whoever consumes pqueue_in) is
 * unchanged. No hardware trigger to wait on, so this polls with a short
 * delay instead of gu_TriggerWait(); the idle_ms accumulator reproduces the
 * same TIMEOUT_JTAG_READ_MS timeout-path message the embedded body prints
 * (found missing by verify_windows.ps1 actually running
 * 06_triggers_and_isr's own copy of this pattern and comparing against its
 * verify.expect; fixed here too for output-fidelity parity between
 * platforms, even though nothing currently checks for it in this file). */
void jtag_uart_read_server(void *pdata)
{
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pdata;
	unsigned int index_msg = 0;
	unsigned int idle_ms = 0;
	const unsigned int poll_ms = 20;

	static char buffer_msg[JTAG_UART_BUFFER_MAX_IN] __attribute__((aligned(4)));
		PRINT_ASSERT((jtag_uart_0_info->buffer_length_in <= JTAG_UART_BUFFER_MAX_IN),
			"ERROR buffer_length_in %u exceeds JTAG_UART_BUFFER_MAX_IN", jtag_uart_0_info->buffer_length_in);

	while (1) {
		char data;
		unsigned int got = jtag_uart_tcp_read_processor((unsigned int) gm_ProcessorId(), &data, 1U);

		if (got == 0U) {
			gu_TaskDelay(0, 0, 0, poll_ms);
			idle_ms += poll_ms;
			if (idle_ms >= TIMEOUT_JTAG_READ_MS) {
				idle_ms = 0;
				gu_fprintf("Read timeout in JTAG UART\n");
				if (index_msg != 0) {
					buffer_msg[index_msg] = '\0';
					gu_fprintf("\nINPUT FROM JTAG-UART = %s\n", buffer_msg);
					index_msg = 0;
				}
			}
			continue;
		}

		idle_ms = 0;
		buffer_msg[index_msg++] = data;

		if ((data == '\r') || (data == '\n') || (index_msg >= jtag_uart_0_info->buffer_length_in - 2U)) {
			buffer_msg[index_msg] = '\0';
			gu_fprintf("\nINPUT FROM JTAG-UART = %s\n", buffer_msg);
			gu_MessageQueuePrintf(jtag_uart_0_info->pqueue_in, "%s", buffer_msg);
			index_msg = 0;
		}
	}
}
#else
void jtag_uart_read_server(void *pdata)
{
	/* Point to the structure with the JTAG_UART information */
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pdata;

	unsigned int index_msg = 0;
	unsigned int uart_data;

	/* Create a local buffer to hold the messages */
	static char buffer_msg[JTAG_UART_BUFFER_MAX_IN] __attribute__((aligned(4))); /* MISRA C:2012 Rule 21.3: static replaces malloc */
		PRINT_ASSERT((jtag_uart_0_info->buffer_length_in <= JTAG_UART_BUFFER_MAX_IN),
			"ERROR buffer_length_in %u exceeds JTAG_UART_BUFFER_MAX_IN", jtag_uart_0_info->buffer_length_in);
		PRINT_ASSERT(((unsigned int) buffer_msg % 4 == 0), "ERROR memory is not alligned");

    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();	
	
    /* Define this task as an ISR associated to trigger_in of jtag_uart_0 */
    gu_TriggerRegisterTask(ptcb, jtag_uart_0_info->trigger_in);

	/* Infinite-loop */
	while (1) {
		/* wait for trigger to read the JTAG UART input data */
		if  (gu_TriggerWait() != G_TRGStatus_triggered) {
			/* There was a timeout */
			gu_fprintf("Read timeout in JTAG UART\n");
            
			if (index_msg != 0) { /* Print the characters received */
				buffer_msg[index_msg] = '\0'; // end of string
				gu_fprintf("\nINPUT FROM JTAG-UART = %s\n", buffer_msg);
				index_msg = 0;
			}
		}
		else
		{
			/* while there exist valid data in JTAG UART read FIFO */
			while ((uart_data = IORD_ALTERA_AVALON_JTAG_UART_DATA(DEVICE_BASE(JTAG_UART_SERVER))) & ALTERA_AVALON_JTAG_UART_DATA_RVALID_MSK) {

				/* Read the data in the JTAG UART and put it as char */
				char data = (char) ((uart_data & ALTERA_AVALON_JTAG_UART_DATA_DATA_MSK) >> ALTERA_AVALON_JTAG_UART_DATA_DATA_OFST);

				/* Store the char as next char received */
				buffer_msg[index_msg++] = data;
				
				/* if end of message is received or buffer is near full, print the message in stderr */
				if ((data == '\r') || (data == '\n') || (index_msg >= jtag_uart_0_info->buffer_length_in - 2)) {
					buffer_msg[index_msg] = '\0'; // end of string
					
					/* Print in stderr the message received */
					gu_fprintf("\nINPUT FROM JTAG-UART = %s\n", buffer_msg);


					index_msg = 0;
				}
			}
		}
	}
}
#endif  // !__niosX_arch__ (embedded jtag_uart_read_server body)

#ifndef __niosX_arch__
/* See jtag_uart_read_server_enable/_disable above -- same
 * reasoning, no hardware register to touch for a software trigger. */
void jtag_uart_write_server_enable(void *pjtag_info)  { (void) pjtag_info; }
void jtag_uart_write_server_disable(void *pjtag_info) { (void) pjtag_info; }
#else
/**
 * \brief Enable a JTAG UART interrupt by setting ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK bit in the control register.
 */
void jtag_uart_write_server_enable(void *pjtag_info)
{

	/* Point to the JTAG UART information structure */
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pjtag_info;

	PRINT_ASSERT((gm_IsPrcGranted()), "ERROR MTX \n");

	/* Enable interrupt in JTAG UART component */
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base);
    
	IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base, status | ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK);

    // Dummy read to take effect (just in case)
    status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base);
    
}

/**
 * \brief Disable a JTAG UART interrupt by clearing ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK bits in the control register.
 */
void jtag_uart_write_server_disable(void *pjtag_info)
{

	/* Point to the JTAG UART information structure */
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pjtag_info;

	PRINT_ASSERT((gm_IsPrcGranted()), "ERROR MTX \n");

	/* Disable interrupt in JTAG UART component */
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base);
    
	IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base, status & ~(ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK));

    // Dummy read to take effect (just in case)
    status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base);

}
#endif  // !__niosX_arch__ (embedded jtag_uart_write_server_enable/_disable bodies)

#ifndef __niosX_arch__
/* Host body. pcap_sendpacket()-style direct write instead of
 * polling hardware FIFO space -- a TCP socket send buffer is not a 64-byte
 * hardware FIFO, so the space_fifo/gu_TriggerWait() flow-control loop below
 * has no host equivalent; jtag_uart_tcp_write_processor() just sends the
 * whole message each time. Still subscribes to pqueue_out exactly like the
 * embedded body, so nothing above this point (whoever posts to pqueue_out)
 * needs to change. */
void jtag_uart_write_server(void *pjtag_info)
{
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pjtag_info;

	static char buffer_msg[JTAG_UART_BUFFER_MAX_OUT] __attribute__((aligned(4)));
		PRINT_ASSERT((jtag_uart_0_info->buffer_length_out <= JTAG_UART_BUFFER_MAX_OUT),
			"ERROR buffer_length_out %u exceeds JTAG_UART_BUFFER_MAX_OUT", jtag_uart_0_info->buffer_length_out);

	int size;

	GS_TCB *ptcb = gu_TaskGetCurrentTCB();
	gu_MessageQueueSubscribe(ptcb, jtag_uart_0_info->pqueue_out, (void *) buffer_msg, jtag_uart_0_info->buffer_length_out);

	while (1) {
		size = gu_MessageQueueReceive(jtag_uart_0_info->pqueue_out);
		if (size > 1) {
			jtag_uart_tcp_write_processor((unsigned int) gm_ProcessorId(), buffer_msg, (unsigned int) (size - 1));
		}
	}
}
#else
void jtag_uart_write_server(void *pjtag_info)
{
	/* Point to the JTAG UART information structure */
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pjtag_info;

	/* Get a buffer to store the messaje to send to the jtag-uart */
	static char buffer_msg[JTAG_UART_BUFFER_MAX_OUT] __attribute__((aligned(4))); /* MISRA C:2012 Rule 21.3: static replaces malloc */
		PRINT_ASSERT((jtag_uart_0_info->buffer_length_out <= JTAG_UART_BUFFER_MAX_OUT),
			"ERROR buffer_length_out %u exceeds JTAG_UART_BUFFER_MAX_OUT", jtag_uart_0_info->buffer_length_out);
		PRINT_ASSERT(((unsigned int) buffer_msg % 4 == 0), "ERROR memory is not alligned");
    
    int size;
    int index_msg;
	
    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

	/* suscribe task as receiver to the pqueue_out message queue */
	gu_MessageQueueSubscribe(ptcb, jtag_uart_0_info->pqueue_out, (void *) buffer_msg, jtag_uart_0_info->buffer_length_out);

    /* Define this task as an ISR associated to trigger_out jtag_uart_0_info */
    gu_TriggerRegisterTask(ptcb, jtag_uart_0_info->trigger_out);

    while (1) {
		/* receive next message from queue_out Message Queue */
        size = gu_MessageQueueReceive(jtag_uart_0_info->pqueue_out);
		
		/* Iterate for each character of the message */
        index_msg = 0;
        while (index_msg < size-1)
        {
			/* Get the available space in jtag uart write FIFO */
			int space_fifo = (((G_UINT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base)) & ((G_UINT32) ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_OFST;

			/* Send as many characters up to the end of message or as possible for jtag uart space */
			while (space_fifo > 0 && index_msg < size-1)
			{
				PRINT_ASSERT((buffer_msg[index_msg] != '\0'), "Error char zero= %d, size= %d", (int) buffer_msg[index_msg], size);

				/* write next message character to jtag uart output fifo */
				IOWR_ALTERA_AVALON_JTAG_UART_DATA(jtag_uart_0_info->jtag_base, buffer_msg[index_msg++]);

				/* Update iteration variables */
				space_fifo--;
			}

			/* There exist more characters to send, so wait to next trigger_out from JTAG UART device */
			if (index_msg < size-1) {
				if  (gu_TriggerWait() != G_TRGStatus_triggered) {
					size = 0; // Timeout happens, remove message;
				}
			}
        }
    }
}
#endif  // !__niosX_arch__ (embedded jtag_uart_write_server body)

#ifdef __niosX_arch__
/* Embedded-only. Dispatches the JTAG UART peripheral's real
 * hardware interrupt to the read/write triggers above; there is no hardware
 * interrupt on host to dispatch at all -- jtag_uart_tcp.c's non-blocking
 * socket polling inside the host read/write server bodies above replaces
 * this entire mechanism, not just the register-level details of it. */
/**
 * \brief JTAG UART interrupt service routine - dispatches read and write trigger events.
 *
 * Registered as the hardware ISR for the JTAG UART peripheral.  On each interrupt
 * it reads the UART control register to determine which interrupt flags are set, then
 * releases the appropriate GeMRTOS trigger (read or write) so the corresponding server
 * task is woken.
 *
 * \param [in] pjtag_info  Pointer to the \c jtag_uart_info structure for the UART
 *                         instance.  Cast internally to <tt>struct jtag_uart_info *</tt>.
 */
void jtag_uart_irq_manager(void *pjtag_info)
{
	struct jtag_uart_info *p_uart_info = (struct jtag_uart_info *) pjtag_info;
	
	gm_GeMRTOSCriticalSectionEnter();

		/* Read the state of interrupt flags from the JTAG UART status register */
		volatile G_UINT32 uart_ctrl_register = (G_UINT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(p_uart_info->jtag_base);

		/* Get the state of the write interrupt (output buffer empty) */
		volatile G_UINT32 write_interrupt = (uart_ctrl_register  & ((G_UINT32) ALTERA_AVALON_JTAG_UART_CONTROL_WI_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_WI_OFST;

		/* Get the state of the read interrupt (input buffer full) */
		volatile G_UINT32 read_interrupt = (uart_ctrl_register  & ((G_UINT32) ALTERA_AVALON_JTAG_UART_CONTROL_RI_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_RI_OFST;

		/* If write interrupt is active, then release the jtag_uart_0_out trigger */
		if ((write_interrupt == (G_UINT32) 1)) {
			PRINT_ASSERT((p_uart_info->trigger_out->trigger.TRG_Enabled == G_TRUE),"ERROR trigger write is not enabled\n");
			gu_TriggerRelease (p_uart_info->trigger_out);
		}

		/* If read interrupt is active, then release the jtag_uart_0_in trigger_in */
		if ((read_interrupt == (G_UINT32) 1)) {
			PRINT_ASSERT((p_uart_info->trigger_in->trigger.TRG_Enabled == G_TRUE),"ERROR trigger read is not enabled\n");
			gu_TriggerRelease (p_uart_info->trigger_in);
		}

	gm_GeMRTOSCriticalSectionExit();
}
#endif  // __niosX_arch__ (jtag_uart_irq_manager)
#endif  // INCLUDE_JTAG_UART_SERVER

#if (INCLUDE_TSE == 1) && defined(__ALTERA_ETH_TSE)

/******************************************************************************
 * Functions for TSE
 *****************************************************************************/

/* Forward declarations for TSE callback functions defined later in this file */
static void StatusCallback(struct netif *pnetif);
static void LinkCallback(struct netif *pnetif);

/* Forward declaration for the web status page SSI handler (defined later in this
 * file, registered with http_set_ssi_handler() before httpd_init() below). */
static u16_t gemrtos_web_ssi_handler(int tag_index, char *insert, int insert_len,
                                      u16_t current_tag_part, u16_t *next_tag_part);
/* "period_ms" / "loop_ms" are bare-value tags (no <tr> wrapper) meant to sit
 * inside <input value="..."> in the configuration form, so the form
 * pre-fills with the live value -- see gemrtos_web_ssi_handler() and
 * LWIP_HTTPD_SSI_INCLUDE_TAG in lwipopts.h (must be 0 for this to render
 * correctly inside a quoted HTML attribute). */
static const char *gemrtos_web_ssi_tags[] = { "sysvars", "period_ms", "loop_ms" };

#if LWIP_HTTPD_CGI
/* Forward declaration for the web status page's CGI form handler (defined later
 * in this file, registered with http_set_cgi_handlers() before httpd_init()
 * below). Lets the web page change g_infinite_loop_delay_ms and the periodic
 * task's period at runtime. */
static const char *gemrtos_web_cgi_set_handler(int cgi_index, int num_params,
                                                char *params[], char *values[]);
static const tCGI gemrtos_web_cgi_table[] = {
    { "/set.cgi", gemrtos_web_cgi_set_handler },
};
#endif /* LWIP_HTTPD_CGI */

/* TSE compile-time configuration (moved from inside tse_receive_server) */
#define USE_DHCP    1  /* 1 = acquire IP via DHCP; 0 = use static IP 192.168.0.15 */
#define BUILD_HTTPD 1  /* 1 = start the raw HTTP server after network comes up     */


    static void StatusCallback(struct netif* pnetif);
    static void LinkCallback(struct netif* pnetif);


    /* Get a buffer to store the messaje to send to and receive from the tse */
    /* IT HAS TO BE IN ON-CHIP MEMORY                                        */
    #define TSE_DMA_FRAME_BUFFER_SIZE 1700U
    #define TSE_TX_QUEUE_BUFFER_SIZE  1700U
    unsigned char buffer_tx_frame[TSE_DMA_FRAME_BUFFER_SIZE] __attribute__((aligned(4), section(".tse_buffers"))); // = { 0 };
    unsigned char buffer_rx_frame[TSE_DMA_FRAME_BUFFER_SIZE] __attribute__((aligned(4), section(".tse_buffers"))); // = { 0 };
    
    // Define an array of one element to store all the information of the ethernet interface
    gemrtos_tse_system_info gemrtos_tse_mac_device[1] __attribute__((aligned(4))) = {
        GEMRTOS_TSE_SYSTEM("en", ETH_TSE, 1, MSGDMA_TX, MSGDMA_RX, 0),
    };


// Allocate descriptors in the descriptor_memory (onchip memory)





#ifndef __niosX_arch__
/* Host body. Everything from lwip_init() through the httpd/SSI/CGI
 * setup below is copied verbatim from the embedded body in the #else branch --
 * the whole point of this split is that this layer does not change. Only the netif init
 * callback (netif_pcap_init instead of netif_tse_init) and the receive loop
 * (pcap_netif_poll_receive() instead of the mSGDMA descriptor/trigger wait)
 * differ, because there is no PHY to debounce and no descriptor ring on a host
 * NIC -- the OS already owns link-state management for a real adapter. */
void tse_receive_server(gemrtos_tse_system_info *pgemrtos_tse_mac_device)
{
    struct netif *pnetif = pgemrtos_tse_mac_device->pnetif;

    lwip_set_status_callback(StatusCallback);
    lwip_set_link_callback(LinkCallback);

    lwip_init();

    if (netif_add(pnetif, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  pnetif->state, netif_pcap_init, netif_input) == NULL) {
        GU_FPRINTF(stderr, "Fatal error initializing pcap netif...\n");
        for (;;);
    }
    /* Same diagnostic line the embedded body prints after its own netif_add()
     * (kept for output-fidelity parity between platforms --
     * verify_windows.ps1 reuses each example's verify.expect verbatim
     * across both tiers, and this line is one of its patterns). */
    GU_FPRINTF(stderr, "tse_receive_server: netif->state = %p\n", (void *) pnetif->state);

    netif_set_status_callback(pnetif, netif_status_callback);
    netif_set_link_callback(pnetif, netif_link_callback_dispatch);
    netif_set_default(pnetif);
    netif_set_up(pnetif);
    netif_set_link_up(pnetif);  /* a real OS-managed adapter's link state is not ours to debounce */

    #if USE_DHCP
        dhcp_start(pnetif);
    #else
        ip_addr_t lwipStaticIp;
        IP4_ADDR(&lwipStaticIp, 192, 168, 0, 15);
        netif_set_ipaddr(pnetif, &lwipStaticIp);
        netif_set_up(pnetif);
        dhcp_inform(pnetif);
    #endif

    GU_FPRINTF(stderr, "IP address= %s\n", ip_ntoa(&pnetif->ip_addr));

    #if BUILD_HTTPD
        /* Register the system-status SSI handler before starting the server, so
         * the web status page (fs/status.html, tag <!--#sysvars-->) can render
         * live values from the first request onward. See gemrtos_web_ssi_handler()
         * and gemrtos_web_sources[] below for how tags are resolved to content. */
        http_set_ssi_handler(gemrtos_web_ssi_handler, gemrtos_web_ssi_tags,
                              (int) (sizeof(gemrtos_web_ssi_tags) / sizeof(gemrtos_web_ssi_tags[0])));

        #if LWIP_HTTPD_CGI
            /* Register the "/set.cgi" form handler that lets the web page change
             * the periodic task's period and the infinite-loop task's delay at
             * runtime. See gemrtos_web_cgi_set_handler() below. */
            http_set_cgi_handlers(gemrtos_web_cgi_table,
                                  (int) (sizeof(gemrtos_web_cgi_table) / sizeof(gemrtos_web_cgi_table[0])));
        #endif

        /* Start HTTPD */
        httpd_init();
    #endif

    /* Main loop: poll Npcap for one frame at a time (non-blocking), feed it to
     * lwIP, run lwIP's own periodic timers, then yield. No PRINT_ASSERT on
     * gm_IsPrcGranted() here -- that assertion in the embedded body guards
     * against re-entering the mSGDMA trigger wait while holding the mutex,
     * which does not apply to a plain non-blocking poll. */
    while (1) {
        struct pbuf *p = pcap_netif_poll_receive(pnetif);

        if (p != NULL) {
            if (pnetif->input(p, pnetif) != ERR_OK) {
                GU_FPRINTF(stderr, "pnetif->input(p, pnetif) != ERR_OK \n");
                pbuf_free(p);
            }
        }

        sys_check_timeouts();

        /* Yield so this task does not busy-spin the processor between frames;
         * short enough to keep DHCP/TCP timing responsive. */
        gu_TaskDelay(0, 0, 0, 5);
    }
}
#else
/**
 * \brief Read only the PHY's raw link-status bit -- no MDIO write, ever.
 *
 * Deliberately does none of what gemrtos_UpdateTSEInfo() (lwip_main.c) does:
 * no speed/duplex read, no comparison against stored state, and above all
 * no possibility of triggering that function's embedded auto-negotiation
 * restart. Exists so the periodic poll in tse_receive_server() can ask
 * "does this look different?" as many times as it wants, on a fixed
 * schedule, at zero risk -- see PHY_POLL_DEBOUNCE_SAMPLES for why that
 * question must never be answered by the heavier function directly.
 *
 * Reads the identical register/bit gemrtos_UpdateTSEInfo() reads for
 * link_alive (register 0x11, bit 0x400) so the two never disagree about
 * what "link alive" means.
 *
 * \param [in] pnetif  Pointer to the lwIP network interface.
 * \return G_TRUE if the PHY currently reports link alive, G_FALSE otherwise.
 */
static G_UINT32 tse_raw_link_alive(struct netif *pnetif)
{
    gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *) pnetif->state;
    int *tse_phy = (int *) tse_info->tse_mac_base;
    unsigned int phy_addr = get_pointed_field(tse_info, tse_phy_mdio_address);

    return ((mdio_read_register(tse_phy, phy_addr, 0x11) & 0x400U) != 0U) ? G_TRUE : G_FALSE;
}

/**
 * \brief Debounce the physical link, then bring the netif up and (re)start DHCP.
 *
 * Waits for the PHY to report link-alive across LINK_DEBOUNCE_SAMPLES
 * consecutive reads, LINK_DEBOUNCE_INTERVAL_MS apart (a single PHY register
 * read is susceptible to transient bounce while a connector mates or a link
 * partner renegotiates) before trusting the link. Used identically at boot
 * and on every runtime link-change event, so both give the DHCP client the
 * same stability guarantee -- previously the boot path skipped debounce
 * entirely and connected on a single, unconfirmed PHY read while the
 * runtime path required 5 stable reads; there was no technical reason for
 * the two to differ, and unifying them costs ~200ms at boot in the common
 * case (LINK_DEBOUNCE_SAMPLES x LINK_DEBOUNCE_INTERVAL_MS), negligible.
 *
 * The debounce loop itself never busy-waits: gu_TaskDelay() between every
 * PHY read yields the processor to other tasks for the full duration,
 * including the case where the link never comes back (production-lens
 * requirement -- a missing cable must not monopolize a processor).
 *
 * \param [in,out] pnetif  Pointer to the lwIP network interface.
 */
static void tse_link_bring_up(struct netif *pnetif)
{
    gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *) pnetif->state;
    G_UINT32 link_confirmed_stable = G_FALSE;

    /* A reconnect (as opposed to first boot) means whatever was on the other
     * end of the cable a moment ago -- gateway, DHCP server -- may not be
     * the same device any more, or may have forgotten this interface's MAC
     * entirely (e.g. a modem/router reboot). netif_set_link_down() (called
     * by every caller of this function, immediately before it) does not
     * purge the ARP cache on its own -- that only happens for
     * netif_set_down(), a different, admin-state call this code never
     * makes. Flushing it here means the reconnect never attempts to reach
     * the old gateway's now-possibly-stale hardware address. Harmless at
     * boot, where the cache is already empty. */
    etharp_cleanup_netif(pnetif);

    while (!link_confirmed_stable) {
        unsigned int stable_reads;

        do {
            if ((gemrtos_UpdateTSEInfo(pnetif) == G_TRUE) && (tse_info->link_alive)) {
                gemrtos_InitialiseTSE(pnetif);  /* Initialise TSE if info changed */
            }
            if (!(tse_info->link_alive)) gu_TaskDelay(0, 0, 0, 200);
        } while (!(tse_info->link_alive));

        for (stable_reads = 1U; stable_reads < LINK_DEBOUNCE_SAMPLES; stable_reads++) {
            gu_TaskDelay(0, 0, 0, LINK_DEBOUNCE_INTERVAL_MS);
            gemrtos_UpdateTSEInfo(pnetif);
            if (!(tse_info->link_alive)) {
                break;  /* bounce -- restart outer wait */
            }
        }

        if (stable_reads == LINK_DEBOUNCE_SAMPLES) {
            link_confirmed_stable = G_TRUE;
        }
    }

    /* Physical link confirmed alive (debounced) - notify lwIP, then restart address config. */
    netif_set_link_up(pnetif);

    #if USE_DHCP
        dhcp_stop(pnetif);
        dhcp_start(pnetif);
    #else
        ip_addr_t lwipStaticIp;
        IP4_ADDR(&lwipStaticIp, 192, 168, 0, 15);
        netif_set_ipaddr(pnetif, &lwipStaticIp);
        netif_set_up(pnetif);
        dhcp_inform(pnetif);
    #endif
}

/**
 * \brief Notify lwIP the link went down, bring it back up, and record when.
 *
 * The single place that performs a runtime reconnect, called from either of
 * two independent triggers in tse_receive_server()'s main loop (an RX-path
 * anomaly, or the direct PHY-poll/not-bound safety net below) -- centralised
 * so both go through the identical sequence and the reconnect bookkeeping
 * cannot drift out of sync with the action it is counting.
 *
 * \param [in,out] pnetif               Pointer to the lwIP network interface.
 * \param [in,out] preconnect_count     Incremented on every call.
 * \param [out]    plast_reconnect_ticks  Set to the current time, for the
 *                                        "how long ago" figure in the status line.
 */
static void tse_force_reconnect(struct netif *pnetif, G_UINT32 *preconnect_count,
                                 G_UINT64 *plast_reconnect_ticks)
{
    netif_set_link_down(pnetif);
    tse_link_bring_up(pnetif);
    (*preconnect_count)++;
    *plast_reconnect_ticks = gu_SystemTimeGet();
}

void tse_receive_server(gemrtos_tse_system_info *pgemrtos_tse_mac_device)
{
    struct netif *pnetif = pgemrtos_tse_mac_device->pnetif;
    gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *)((gemrtos_tse_system_info *)(pnetif->state));

    GS_TRGStatus trigger_status;
    G_UINT32 link_is_alive;
    unsigned int timeout_count = 0;
    G_UINT32 link_has_changed = G_FALSE;

    /* Runtime reconnect robustness state -- see PHY_POLL_INTERVAL_MS and
     * DHCP_NOT_BOUND_TIMEOUT_MINUTES above. All bounded by elapsed real time
     * (gu_SystemTimeGet()), never by loop-iteration or poll count, so the
     * intervals hold regardless of frame rate or system load. */
    G_UINT64 last_phy_poll_ticks       = 0U;
    unsigned int phy_poll_mismatch_count = 0U;
    unsigned int rx_error_run            = 0U;  /* consecutive mSGDMA RX response-errors, no good frame between; see RX_ERROR_RUN_LIMIT */
    G_UINT32 not_bound_timer_running   = G_FALSE;
    G_UINT64 not_bound_since_ticks     = 0U;
    G_UINT32 reconnect_count           = 0U;
    G_UINT64 last_reconnect_ticks      = 0U;
    const G_UINT64 phy_poll_interval_ticks =
        gu_ConvertTime(0, 0, 0, PHY_POLL_INTERVAL_MS);
    const G_UINT64 not_bound_timeout_ticks =
        gu_ConvertTime(0, DHCP_NOT_BOUND_TIMEOUT_MINUTES, 0, 0);

    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    /* Initialise lwIP stack, network interface, DHCP and HTTP server. */

// ##########################################

    lwip_set_status_callback(StatusCallback);
    lwip_set_link_callback(LinkCallback);

    lwip_init();

    if (netif_add(pnetif, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  pnetif->state, netif_tse_init, netif_input) == NULL) {
        /* gu_fprintf, not GU_FPRINTF: the for(;;) below is unconditional and
         * unrecoverable in every build, so this is the only diagnostic that
         * will ever exist for this failure -- it must survive into a
         * production build (G_DEBUG_WHILEFOREVER_ENABLE == 0), where
         * GU_FPRINTF compiles to nothing and this would otherwise be
         * a silent infinite loop with zero explanation. */
        gu_fprintf("Fatal error initializing...\n");
        for (;;);
    }
    gu_fprintf("tse_receive_server: netif->state = %p\n",
               (void *) ((gemrtos_tse_system_info *) (pnetif->state))->tse_mac_base);

    /* Create the receive trigger with TIMEOUT_TSE_RECEIVE_MS timeout */
    G_UINT64 timeout = gu_ConvertTime(0, 0, 0, TIMEOUT_TSE_RECEIVE_MS);
    tse_info->sgdma_trigger_receive = gu_TriggerCreate(tse_info->tse_msgdma_rx_irq, timeout);
    if (tse_info->sgdma_trigger_receive == (GS_RCB *) 0) {
        gu_printf("Error when creating sgdma_trigger_receive\n");
        while (1);
    }

    /* Set periodic timeout type */
    gu_TriggerSetTimeoutType(tse_info->sgdma_trigger_receive,
                             GS_TRGTimeOutType_restart_when_timeout);

    /* Register this task as the DMA receive ISR */
    gu_TriggerRegisterTask(ptcb, tse_info->sgdma_trigger_receive);

    netif_set_status_callback(pnetif, netif_status_callback);
    /* Wires StatusCallback()/LinkCallback() (below) to actually run -- without this,
     * lwip_set_status_callback()/lwip_set_link_callback() only store a pointer that
     * nothing reads. See netif_status_callback()'s comment in lwip_main.c for
     * the full finding; this line and the dispatch function it wires are the fix. */
    netif_set_link_callback(pnetif, netif_link_callback_dispatch);
    netif_set_default(pnetif);
    netif_set_up(pnetif);

    /* Confirm the physical link (debounced) before starting DHCP -- the same
     * tse_link_bring_up() sequence every runtime reconnect uses below.
     * Previously this boot path trusted a single, unconfirmed PHY read;
     * unified so boot and runtime reconnection give DHCP the same
     * stability guarantee (2026-08-17, requested by Ricardo after the
     * DHCP-reentrancy hang investigation). */
    netif_set_link_down(pnetif);
    tse_link_bring_up(pnetif);

    gu_fprintf("IP address= %s\n", ip_ntoa(&pnetif->ip_addr));

    #if BUILD_HTTPD
        /* Register the system-status SSI handler before starting the server, so
         * the web status page (fs/status.html, tag <!--#sysvars-->) can render
         * live values from the first request onward. See gemrtos_web_ssi_handler()
         * and gemrtos_web_sources[] below for how tags are resolved to content. */
        http_set_ssi_handler(gemrtos_web_ssi_handler, gemrtos_web_ssi_tags,
                              (int) (sizeof(gemrtos_web_ssi_tags) / sizeof(gemrtos_web_ssi_tags[0])));

        #if LWIP_HTTPD_CGI
            /* Register the "/set.cgi" form handler that lets the web page change
             * the periodic task's period and the infinite-loop task's delay at
             * runtime. See gemrtos_web_cgi_set_handler() below. */
            http_set_cgi_handlers(gemrtos_web_cgi_table,
                                  (int) (sizeof(gemrtos_web_cgi_table) / sizeof(gemrtos_web_cgi_table[0])));
        #endif

        /* Start HTTPD */
        httpd_init();
    #endif

// ##########################################

    /* Reset the dispatcher and wait until finishing */
    IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_rx_csr_base,
                                   ALTERA_MSGDMA_CSR_RESET_MASK);
    while (IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base)
           & ALTERA_MSGDMA_CSR_RESET_STATE_MASK);

    /* Clear msgDMA status */
    IOWR_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base, 0xFFFFFFFFu);    

    /* Main loop */
    while (1) {
        PRINT_ASSERT((!(gm_IsPrcGranted())), "ERROR in task MTX task= %s, TCB_MTX_NESTED= %u\n",
                     g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_description,
                     (unsigned int) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED);

        /* Direct, RX-traffic-independent PHY poll (PHY_POLL_INTERVAL_MS above).
         *
         * Gated on !netif_ip_is_up(): this poll runs only while the interface
         * has no DHCP lease. Once bound it stops completely, so steady-state
         * reception performs no MDIO transaction at all -- exactly as it did
         * before this poll existed. Rationale: on a bound, healthy link the
         * once-per-second MDIO read in tse_raw_link_alive() was a measurable
         * disturbance to the RGMII receive path on one board (sustained ~10%
         * ICMP loss with continuous mSGDMA response-errors, none of it present
         * before this poll was added and none of it present on a second board
         * running byte-identical firmware). An MDIO read has no place on the
         * healthy-link hot path. The failure this poll exists to shorten --
         * an upstream modem power-cycle leaving the interface stuck retrying
         * DHCP forever (see PHY_POLL_INTERVAL_MS) -- is by definition a
         * not-bound state, so the gate keeps the fast (~PHY_POLL_INTERVAL_MS)
         * detection precisely where it is needed and drops it where it did
         * harm. A link that drops while bound is still caught by the
         * pre-existing RX-idle trigger (30 consecutive RX-trigger timeouts,
         * ~7.5s), which forces a reconnect, clears the bound state and
         * re-engages this poll; DHCP_NOT_BOUND_TIMEOUT_MINUTES stays the
         * unconditional backstop for whatever neither path catches.
         *
         * Two stages, deliberately: a cheap, side-effect-free raw read every
         * tick (tse_raw_link_alive()), and the heavier gemrtos_UpdateTSEInfo()
         * -- which can restart PHY negotiation for real, see
         * PHY_POLL_DEBOUNCE_SAMPLES -- only once PHY_POLL_DEBOUNCE_SAMPLES
         * consecutive ticks agree something actually changed. A change
         * confirmed this way feeds the same link_has_changed flag the
         * RX-idle/DMA-error triggers below use, so it goes through the
         * identical, already-verified confirm-or-dismiss logic in the block
         * right after this one -- nothing about that logic needed to change,
         * only how it gets woken up. */
        if (netif_ip_is_up(pnetif)) {
            /* Bound: poll disabled. Discard any partial debounce so the next
             * not-bound episode starts a fresh mismatch count. */
            phy_poll_mismatch_count = 0U;
        } else if ((gu_SystemTimeGet() - last_phy_poll_ticks) >= phy_poll_interval_ticks) {
            last_phy_poll_ticks = gu_SystemTimeGet();

            if (tse_raw_link_alive(pnetif) != tse_info->link_alive) {
                phy_poll_mismatch_count++;
                if (phy_poll_mismatch_count >= PHY_POLL_DEBOUNCE_SAMPLES) {
                    phy_poll_mismatch_count = 0U;
                    if (gemrtos_UpdateTSEInfo(pnetif)) {
                        link_has_changed = G_TRUE;
                    }
                }
            } else {
                phy_poll_mismatch_count = 0U;
            }
        }

        if (link_has_changed == G_TRUE) {
            link_has_changed = G_FALSE;

            /* link_has_changed is also raised by RX-idle (30 consecutive RX-trigger
             * timeouts, ~7.5s with zero frames received), by an mSGDMA descriptor
             * error, and by the direct PHY poll above -- none of these guarantee
             * the PHY link actually changed. Confirm against the PHY before
             * tearing down lwIP/DHCP: a spurious wake-up (link never left the up
             * state) must not flush the ARP cache or force a full DHCP restart.
             * Investigation: a quiet
             * network segment (no incoming frames for >~7.5s) was cycling DHCP from
             * BOUND back to a fresh DISCOVER indefinitely, even though the physical
             * link never dropped. */
            G_UINT32 tse_info_changed = gemrtos_UpdateTSEInfo(pnetif);
            if (tse_info_changed && tse_info->link_alive) {
                gemrtos_InitialiseTSE(pnetif);  /* keep MAC config in sync, same as the debounce loop below */
            }

            if (!(tse_info->link_alive) || tse_info_changed) {
                /* Debounce, then bring the link back up and restart DHCP --
                 * the same sequence tse_link_bring_up() runs at boot. */
                tse_force_reconnect(pnetif, &reconnect_count, &last_reconnect_ticks);
            }
            /* else: false alarm (RX-idle or a transient DMA hiccup) -- PHY confirmed
             * unchanged, nothing to reconfigure. */
        }

        /* Defence-in-depth safety net (DHCP_NOT_BOUND_TIMEOUT_MINUTES above):
         * force a reconnect if the interface has looked link-alive without
         * ever completing DHCP for too long, regardless of why -- including a
         * cause this code does not (yet) have a name for. Runs every pass, not
         * gated on link_has_changed, because its whole purpose is to catch the
         * case where nothing here ever noticed a problem. */
        if (tse_info->link_alive && !netif_ip_is_up(pnetif)) {
            if (!not_bound_timer_running) {
                not_bound_timer_running = G_TRUE;
                not_bound_since_ticks = gu_SystemTimeGet();
            } else if ((gu_SystemTimeGet() - not_bound_since_ticks) >= not_bound_timeout_ticks) {
                not_bound_timer_running = G_FALSE;
                gu_fprintf("[ethernet] No DHCP lease after %u minute(s) with link up -- forcing reconnect\n",
                           (unsigned int) DHCP_NOT_BOUND_TIMEOUT_MINUTES);
                tse_force_reconnect(pnetif, &reconnect_count, &last_reconnect_ticks);
            }
        } else {
            /* Bound, or link not alive (the other paths above own that case) --
             * either way there is nothing to time. */
            not_bound_timer_running = G_FALSE;
        }

        /* Re-arm the mSGDMA RX descriptor for the next frame */

        unsigned int control;
    #ifdef __ALTERA_MSGDMA
        /* Wait while mSGDMA is busy */
        while ((IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base) & ALTERA_MSGDMA_CSR_DESCRIPTOR_BUFFER_EMPTY_MASK) == 0);

        /* Stop the dispatcher from issuing more descriptors */
        control = ALTERA_MSGDMA_CSR_STOP_DESCRIPTORS_MASK;
        IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_rx_csr_base, control);

        /* Clear any previous status register information that might occlude error checking */
        IOWR_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base,
                                       IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base));

        /* Create the descriptor for receiving the next frame into buffer_rx_frame */
        IOWR_ALTERA_MSGDMA_DESCRIPTOR_WRITE_ADDRESS(tse_info->tse_msgdma_rx_descriptor_slave_base,
                                                     (alt_u32) tse_info->buffer_rx_frame);
        IOWR_ALTERA_MSGDMA_DESCRIPTOR_LENGTH(tse_info->tse_msgdma_rx_descriptor_slave_base,
                                              TSE_DMA_FRAME_BUFFER_SIZE);

        /* Run; stop on error */
        control = (ALTERA_MSGDMA_CSR_STOP_ON_ERROR_MASK |
                   ALTERA_MSGDMA_CSR_GLOBAL_INTERRUPT_MASK |
                   ALTERA_MSGDMA_CSR_STOP_ON_EARLY_TERMINATION_MASK);
        control &= (~ALTERA_MSGDMA_CSR_STOP_DESCRIPTORS_MASK);
        IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_rx_csr_base, control);

        IOWR_ALTERA_MSGDMA_DESCRIPTOR_CONTROL_STANDARD(tse_info->tse_msgdma_rx_descriptor_slave_base,
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_GO_MASK |
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_ERROR_IRQ_MASK |
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_EARLY_DONE_ENABLE_MASK |
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_TRANSFER_COMPLETE_IRQ_MASK |
                                                        ALTERA_MSGDMA_DESCRIPTOR_CONTROL_END_ON_EOP_MASK);
    #else
    #error "mSGDMA module should exist for TSE support"
    #endif


        /* Log current link / DHCP status before blocking on the trigger */
        link_is_alive = lwip_is_interface_up(pnetif);
        if (tse_info->phy_speed == GE_PHY_SPEED_1000) gu_fprintf("SPEED= 1Gb, ");
        if (tse_info->phy_speed == GE_PHY_SPEED_100)  gu_fprintf("SPEED= 100Mb, ");
        if (tse_info->phy_speed == GE_PHY_SPEED_10)   gu_fprintf("SPEED= 10Mb, ");
        if (tse_info->phy_duplex == GE_PHY_DUPLEX_HALF) gu_fprintf("HALF, ");
        if (tse_info->phy_duplex == GE_PHY_DUPLEX_FULL) gu_fprintf("FULL, ");

        /* dhcp->tries and reconnects/last-reconnect-elapsed added 2026-09-03
         * so a stuck-DHCP report can be read straight off this line -- no
         * packet capture needed. dhcp->tries climbing distinguishes "DHCP is
         * genuinely retrying but getting no OFFER" (network/server-side) from
         * "the retry timer itself is not running" (a lwIP/GeMRTOS-side
         * defect); reconnects/elapsed distinguishes "this code never noticed
         * a problem" (reconnects stays 0) from "it reconnected but DHCP still
         * did not complete" (reconnects > 0, still not bound). */
        {
            struct dhcp *pdhcp = (struct dhcp *) netif_dhcp_data(pnetif);
            G_UINT32 secs_since_reconnect = (reconnect_count == 0U) ? 0U
                : (G_UINT32) ((gu_SystemTimeGet() - last_reconnect_ticks) / G_TICKS_PER_SECOND);

            gu_fprintf("dhcp->state= %u, dhcp->tries= %u, ",
                       (unsigned int) pdhcp->state, (unsigned int) pdhcp->tries);
            gu_fprintf("netif_ip_is_up= %u, ", (netif_ip_is_up(pnetif)));
            gu_fprintf("Link_alive= %u, reconnects= %u (last %u s ago), IP address: %s\n",
                       link_is_alive, (unsigned int) reconnect_count,
                       (unsigned int) secs_since_reconnect, ip_ntoa(&pnetif->ip_addr));
        }

        /* Wait for DMA completion trigger; process the received frame */

        /* Wait until receive descriptor transfer is complete or timeout */
        while ((trigger_status = gu_TriggerWait()) != G_TRGStatus_triggered) {
            timeout_count++;
            if (timeout_count > 30) {
                /* ERROR in receiving frames - update Ethernet link */
                timeout_count = 0;
                link_has_changed = G_TRUE;
                break;
            }
        }

        /* Cyclic lwIP timers check -- unconditional, once per outer-loop pass,
         * regardless of whether a frame arrived. Previously this call lived
         * inside the while loop above and only ran on a TIMEOUT return from
         * gu_TriggerWait(): under any steady incoming traffic (ARP, mDNS, the
         * DHCP server's own broadcasts -- nothing unusual), the trigger tends
         * to fire before its own 250ms timeout on most passes, so the loop
         * body -- and this call -- could go unserviced far longer than the
         * 500ms/60s cadence dhcp_fine_tmr()/dhcp_coarse_tmr() require. A DHCP
         * client stuck in RENEWING (state 5) waiting on a response that never
         * arrives then has nothing left to notice the request timed out or to
         * fall back to REBINDING/restart -- it stays there indefinitely, and
         * netif_ip_is_up() (which requires DHCP_STATE_BOUND) stays 0. Moving
         * the call outside the while loop guarantees it runs exactly once per
         * pass either way. */
        sys_check_timeouts();

        if (trigger_status == G_TRGStatus_triggered) {
            timeout_count = 0;

            // PRESERV READ ORDER, OTHERWISE PROCESSOR IS HALTED
            // DO NOT READ AGAIN !!!!!!!!!!!!!!!!!!!!!!!!!!!
            G_UINT32 bytes_transferred = IORD_ALTERA_MSGDMA_RESPONSE_ACTUAL_BYTES_TRANSFERRED(tse_info->tse_msgdma_rx_response_base);
            G_UINT32 response_reg      = IORD_ALTERA_MSGDMA_RESPONSE_ERRORS_REG(tse_info->tse_msgdma_rx_response_base);

            if ((unsigned int) response_reg != 0) {
                /* One mSGDMA RX response-error. Recover the DMA path and drop
                 * the frame; escalate to a link re-check only after
                 * RX_ERROR_RUN_LIMIT of these occur back-to-back with no good
                 * frame between. A single errored frame on a healthy link is
                 * normal, and escalating each one drives an MDIO burst that
                 * deafens RX and sustains the error
                 * (RX_ERROR_RUN_LIMIT). */
                rx_error_run++;
                gu_fprintf("AFTER RECEIVE WHILE STATUS response_reg= 0x%x, bytes_transferred= %u (run %u/%u)\n",
                            (unsigned int) response_reg, (unsigned int) bytes_transferred,
                            rx_error_run, (unsigned int) RX_ERROR_RUN_LIMIT);

                if (rx_error_run >= RX_ERROR_RUN_LIMIT) {
                    rx_error_run = 0U;
                    link_has_changed = G_TRUE;
                    gu_fprintf("RX response-error run reached %u -- forcing link re-check\n",
                               (unsigned int) RX_ERROR_RUN_LIMIT);
                }

                /* Reset the dispatcher only when it actually stopped. A
                 * frame-level receive error (CRC / RGMII RX_ER) on a
                 * descriptor that otherwise completed does not halt the
                 * dispatcher -- the re-arm block below then continues
                 * normally and the only cost is the one dropped frame. A
                 * full CSR reset here is what makes RX deaf long enough for
                 * the *next* frame to be truncated too (the recurring
                 * "run 2" ~53-byte runt), so it is now gated on the
                 * dispatcher genuinely being in a stopped state. STOP_STATE
                 * (bit 5) and STOPPED_ON_ERROR (bit 7) are both checked --
                 * STOP_ON_ERROR is armed in the re-arm block, so a halting
                 * error sets both; a non-halting frame error sets neither. */
                {
                    G_UINT32 rx_csr_status =
                        IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base);
                    if ((rx_csr_status & (ALTERA_MSGDMA_CSR_STOPPED_ON_ERROR_MASK |
                                          ALTERA_MSGDMA_CSR_STOP_STATE_MASK)) != 0U) {
                        /* Reset the dispatcher and wait until finishing */
                        IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_rx_csr_base, ALTERA_MSGDMA_CSR_RESET_MASK);
                        while (IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_rx_csr_base) & ALTERA_MSGDMA_CSR_RESET_STATE_MASK);
                    }
                }
            }
            else
            {
                /* Clean mSGDMA completion -- the RX path is delivering
                 * frames, so any prior error run is broken. */
                rx_error_run = 0U;
                // GU_FPRINTF(stderr, "Length= %u, error= %u, Frame = ",
                //           (unsigned int) bytes_transferred,

                if ((bytes_transferred == 0U) ||
                    (bytes_transferred > TSE_DMA_FRAME_BUFFER_SIZE)) {
                    gu_fprintf("Dropping invalid RX frame length= %u\n", (unsigned int) bytes_transferred);
                    link_has_changed = G_TRUE;
                }
                else
                {
                    /* Check for received frames, feed them to lwIP */
                    u16_t len = (u16_t) bytes_transferred;
                    PRINT_ASSERT((bytes_transferred == (G_UINT32) len), "ERROR in len representation in 16 bits");

                    struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
                    if (p != NULL) {
                        /* Copy ethernet frame into pbuf */
                        if (pbuf_take(p, tse_info->buffer_rx_frame, len) != ERR_OK) {
                            gu_fprintf("pbuf_take failed for RX length= %u\n", (unsigned int) len);
                            pbuf_free(p);
                        }
                        else if (pnetif->input(p, pnetif) != ERR_OK) {
                            gu_fprintf("pnetif->input(p, pnetif) != ERR_OK \n");
                            pbuf_free(p);
                        }
                    }
                    else {
                        gu_fprintf("pbuf_alloc failed for RX length= %u\n", (unsigned int) len);
                    }
                }
            }
        }


    }
}
#endif  // !__niosX_arch__


#ifndef __niosX_arch__
/* On host, pcap_netif_send() (see pcap_netif.c) is called directly
 * and synchronously from pnetif->linkoutput -- pcap_sendpacket() is a fast OS
 * call, unlike mSGDMA, so there is no need for the queue-to-transmit-task
 * indirection the embedded body below uses to hand frames to a DMA engine.
 * This task therefore has nothing to do; kept only so the unchanged
 * gu_TaskCreate((void*) tse_transmit_server, ...) call site below needs no
 * guard of its own. */
void tse_transmit_server(void *pdata)
{
    (void) pdata;
    for (;;) {
        gu_TaskDelay(0, 0, 1, 0);
    }
}
#else
void tse_transmit_server(void *pdata)
{
	unsigned int size, control;

    gemrtos_tse_system_info *pgemrtos_tse_mac_device = (gemrtos_tse_system_info *) pdata;
    struct netif *pnetif = pgemrtos_tse_mac_device->pnetif;

	gemrtos_tse_system_info *tse_info = (gemrtos_tse_system_info *) ((gemrtos_tse_system_info *) (pnetif->state));	
    
    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

	/* suscribe task as consumer to the consumer/producer of tse queue */
    PRINT_ASSERT(((void *) pgemrtos_tse_mac_device->buffer_tx_frame == (void *) &buffer_tx_frame), "ERROR tse_info->buffer_tx_frame= %p, &buffer_tx_frame= %p\n", (void *) pgemrtos_tse_mac_device->buffer_tx_frame, (void *) &buffer_tx_frame);
	gu_MessageQueueSubscribe(ptcb, pgemrtos_tse_mac_device->p_tse_queue_out,
	                         (void *) pgemrtos_tse_mac_device->buffer_tx_frame,
	                         TSE_TX_QUEUE_BUFFER_SIZE);

    /* Reset the dispatcher and wait until finishing */
    IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_tx_csr_base, ALTERA_MSGDMA_CSR_RESET_MASK);
    while(IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base) & ALTERA_MSGDMA_CSR_RESET_STATE_MASK);

    /* Clear msgDMA status */
    IOWR_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base, 0xFFFFFFFFu);


	while (1) {
		PRINT_ASSERT((!(gm_IsPrcGranted())), "ERROR in task MTX task= %s, TCB_MTX_NESTED= %u\n", g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_description, (unsigned int) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED);
        
		/* receive next frame plus legth to transmit from producer/consumer queue */
		size = gu_MessageQueueReceive(pgemrtos_tse_mac_device->p_tse_queue_out);

		if (((G_UINT32) size == (G_UINT32) ERR_RECEIVE_ABORT) ||
		    (size <= 0) ||
		    ((unsigned int) size > TSE_TX_QUEUE_BUFFER_SIZE) ||
		    ((unsigned int) size > TSE_DMA_FRAME_BUFFER_SIZE)) {
			gu_fprintf("Dropping invalid TX frame length= %d\n", size);
			continue;
		}

		/* when packet is received it is because the receive task configured pnetif structure */
		if (tse_info->sgdma_trigger_transmit == (GS_RCB *) 0) {
			/* Create the receive and transmit triggers */
			tse_info->sgdma_trigger_transmit = gu_TriggerCreate(tse_info->tse_msgdma_tx_irq, (G_UINT64) 0);
			if (tse_info->sgdma_trigger_transmit == (GS_RCB *) 0) {
				gu_printf("Error when creating sgdma_trigger_transmit\n");
				while(1);
			}
			/* Define this task as an ISR associated to sgdma_trigger_transmit */
			gu_TriggerRegisterTask(ptcb, (GS_RCB *) tse_info->sgdma_trigger_transmit);
		}

        // /* wait while mSGDAM is busy */
        while ((IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base) & ALTERA_MSGDMA_CSR_DESCRIPTOR_BUFFER_EMPTY_MASK) == 0);

        /* Stop the msgdma dispatcher from issuing more descriptors to the read or write masters  */
        /* stop issuing more descriptors */
        control = ALTERA_MSGDMA_CSR_STOP_DESCRIPTORS_MASK;
        IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_tx_csr_base, control);


        /*
        * Clear any (previous) status register information
        * that might occlude our error checking later.
        */
        IOWR_ALTERA_MSGDMA_CSR_STATUS(
            tse_info->tse_msgdma_tx_csr_base, 
            IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base));

        /* Create descriptor for Avalon-MM to Avalon-ST mSGDAM transfer */ 
        PRINT_ASSERT(((void *) tse_info->buffer_tx_frame == (void *) &buffer_tx_frame), "ERROR tse_info->buffer_tx_frame= %p, &buffer_tx_frame= %p\n", (void *) tse_info->buffer_tx_frame, (void *) &buffer_tx_frame);
        IOWR_ALTERA_MSGDMA_DESCRIPTOR_READ_ADDRESS(tse_info->tse_msgdma_tx_descriptor_slave_base, (alt_u32) tse_info->buffer_tx_frame);



                                                    
        IOWR_ALTERA_MSGDMA_DESCRIPTOR_LENGTH(tse_info->tse_msgdma_tx_descriptor_slave_base, (unsigned int) size);


        IOWR_ALTERA_MSGDMA_DESCRIPTOR_CONTROL_STANDARD(tse_info->tse_msgdma_tx_descriptor_slave_base, 
                                                       ALTERA_MSGDMA_DESCRIPTOR_CONTROL_GO_MASK |
                                                       ALTERA_MSGDMA_DESCRIPTOR_CONTROL_TRANSFER_COMPLETE_IRQ_MASK |
                                                       ALTERA_MSGDMA_DESCRIPTOR_CONTROL_GENERATE_SOP_MASK |
                                                       ALTERA_MSGDMA_DESCRIPTOR_CONTROL_GENERATE_EOP_MASK );
                                                       // ALTERA_MSGDMA_DESCRIPTOR_CONTROL_ERROR_IRQ_MASK |
                                                       // ALTERA_MSGDMA_DESCRIPTOR_CONTROL_EARLY_DONE_ENABLE_MASK );

        /*
        *  Set up controller to:
        *  - Run
        *  - Stop on an error with any particular descriptor
        */
        control = (ALTERA_MSGDMA_CSR_STOP_ON_ERROR_MASK |
                    ALTERA_MSGDMA_CSR_GLOBAL_INTERRUPT_MASK );
        control &=  (~ALTERA_MSGDMA_CSR_STOP_DESCRIPTORS_MASK);
        IOWR_ALTERA_MSGDMA_CSR_CONTROL(tse_info->tse_msgdma_tx_csr_base, control);
        

        /* Wait until transmit descriptor transfer is complete */
        if (gu_TriggerWait() != G_TRGStatus_triggered) {
            GU_FPRINTF(stderr, "STATUS = %x, %s, %d \n", IORD_ALTERA_MSGDMA_CSR_STATUS(tse_info->tse_msgdma_tx_csr_base), __FUNCTION__, __LINE__);    
        }
    }
}
#endif  // !__niosX_arch__

#define BIT_MASK(data, bit)  (((data) & (1U << (unsigned int)(bit))) ? 1U : 0U) 




/******************************************************************************
 * Web status page -- generic SSI variable table mechanism
 *
 * Exposes live system values on the web status page (fs/status.html, tag
 * <!--#sysvars-->) as an HTML table, one row per value. To add a new value to
 * the page, add one "source" below (a row_count() + render_row() pair) to
 * gemrtos_web_sources[] -- no other code, and no change to status.html, is
 * needed. A source may contribute a fixed number of rows (e.g. 1, for a
 * single scalar) or a variable number determined at request time (e.g. one
 * row per item in a list); the dispatch logic in gemrtos_web_ssi_handler()
 * does not need to know which.
 *****************************************************************************/

typedef G_UINT32 (*gemrtos_web_row_count_fn)(void);
typedef u16_t (*gemrtos_web_render_row_fn)(G_UINT32 row_index, char *buf, u16_t buf_len);

typedef struct {
    gemrtos_web_row_count_fn  row_count;   /* how many rows this source has right now */
    gemrtos_web_render_row_fn render_row;  /* render row [0, row_count()) as "<tr>...</tr>" */
} gemrtos_web_source_t;

/* --- Source: number of tasks currently known to the kernel --- */

static G_UINT32 gemrtos_web_kernel_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_kernel_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    /* NOTE: g_kcb.KCB_NUMBER_OF_TCBs is NOT a live count of
     * active tasks -- it only increments when the static TCB pool is
     * exhausted and a TCB is allocated dynamically as a development-mode
     * fallback (see gk_TCB_GetFree(), gemrtos_core.c). In a correctly-sized
     * production build it is always 0, which is what made this row look
     * broken on real hardware. G_NUMBER_OF_TCB (the configured static pool
     * capacity) is reported instead -- a compile-time constant, always
     * correct, and safe to read without touching any live kernel list.
     * Reporting the true number of currently-active tasks would require
     * walking g_kcb.KCB_FREE_TCBs, which is mutated by other processors and
     * needs a synchronization answer first (the same open question as the
     * deferred task-name-table source). */
    (void) row_index;
    return (u16_t) snprintf(buf, buf_len,
        "<tr><td>Configured task pool size</td><td>%lu</td></tr>",
        (unsigned long) G_NUMBER_OF_TCB);
}

/* --- Source: system uptime, in seconds --- */

static G_UINT32 gemrtos_web_uptime_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_uptime_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    /* NOTE: the first version of this row printed
     * milliseconds as a 64-bit value via snprintf(..., "%llu", (unsigned long
     * long) ...) and produced nonsensical, non-reproducible output on real
     * hardware. "%llu" is used elsewhere in this codebase (gemrtos_monitor.c,
     * gemrtos_kernel.c) and presumed safe by precedent, but always through
     * fprintf()/printf(), never snprintf() -- this row was the first place a
     * 64-bit value went through snprintf(), and evidence points at that
     * specific combination: lwIP's own sys_now() (lwip_main.c) does the exact
     * same gu_SystemTimeGet()/G_TICKS_PER_MSECOND division and is proven
     * correct indirectly (the whole TCP/IP stack depends on it and works), so
     * the division itself is not the suspect. Reporting whole seconds as a
     * 32-bit value computed before the snprintf() call sidesteps 64-bit
     * varargs entirely and is still a useful, correct value (32 bits of
     * seconds covers ~136 years of uptime). Before reintroducing a 64-bit
     * value into any snprintf() call on this target, verify the combination
     * actually works on real hardware first -- don't assume it from the
     * fprintf()-only precedent elsewhere in the codebase. */
    G_UINT32 uptime_s;

    (void) row_index;
    uptime_s = (G_UINT32) (gu_SystemTimeGet() / (G_UINT64) G_TICKS_PER_SECOND);
    return (u16_t) snprintf(buf, buf_len,
        "<tr><td>Uptime (s)</td><td>%lu</td></tr>",
        (unsigned long) uptime_s);
}

/* --- Source: current IP address of the default network interface --- */

static G_UINT32 gemrtos_web_ipaddr_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_ipaddr_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    /* print_ipad() internally hardcodes snprintf(buf, 17, ...) regardless of the
     * buffer size passed in, so this buffer must be at least 17 bytes or it
     * would overflow. */
    char ip_buf[17];

    (void) row_index;
    if (netif_default == NULL) {
        return (u16_t) snprintf(buf, buf_len, "<tr><td>IP address</td><td>(no interface)</td></tr>");
    }
    print_ipad(netif_default->ip_addr.addr, ip_buf);
    return (u16_t) snprintf(buf, buf_len, "<tr><td>IP address</td><td>%s</td></tr>", ip_buf);
}

/* --- Source: periodic task's current period, in ms (web-editable, see
 * gemrtos_web_cgi_set_handler() below) --- */

/**
 * \brief Returns the periodic task's current period in milliseconds, or 0 if
 *        the task was never created.
 *
 * Shared by the status-table row (gemrtos_web_period_render_row()) and the
 * bare-value SSI tag used to pre-fill the configuration form
 * (gemrtos_web_ssi_handler()) so the tick-to-ms conversion is written once.
 *
 * TCBPeriod is a G_UINT64 storing TICKS (gu_TaskPeriodSet() stores the result
 * of gu_ConvertTime(), not raw milliseconds) -- convert back to ms and
 * truncate to 32 bits, same rationale as the uptime row above: keep 64-bit
 * values out of snprintf()'s varargs on this target.
 */
static G_UINT32 gemrtos_web_current_period_ms(void)
{
    if (g_periodic_task_tcb == (GS_TCB *) 0) {
        return 0U;
    }
    return (G_UINT32) (g_periodic_task_tcb->TCBPeriod / (G_UINT64) G_TICKS_PER_MSECOND);
}

static G_UINT32 gemrtos_web_period_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_period_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    (void) row_index;
    if (g_periodic_task_tcb == (GS_TCB *) 0) {
        return (u16_t) snprintf(buf, buf_len, "<tr><td>Periodic task period (ms)</td><td>(not created)</td></tr>");
    }
    return (u16_t) snprintf(buf, buf_len,
        "<tr><td>Periodic task period (ms)</td><td>%lu</td></tr>",
        (unsigned long) gemrtos_web_current_period_ms());
}

/* --- Source: infinite-loop task's current delay, in ms (web-editable) --- */

static G_UINT32 gemrtos_web_loopdelay_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_loopdelay_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    (void) row_index;
    return (u16_t) snprintf(buf, buf_len,
        "<tr><td>Infinite-loop delay (ms)</td><td>%lu</td></tr>",
        (unsigned long) g_infinite_loop_delay_ms);
}

/* Registry of all sources contributing rows to the <!--#sysvars--> table.
 * Add a new { row_count, render_row } pair here to expose a new value. */
static const gemrtos_web_source_t gemrtos_web_sources[] = {
    { gemrtos_web_kernel_row_count,     gemrtos_web_kernel_render_row     },
    { gemrtos_web_uptime_row_count,     gemrtos_web_uptime_render_row     },
    { gemrtos_web_ipaddr_row_count,     gemrtos_web_ipaddr_render_row     },
    { gemrtos_web_period_row_count,     gemrtos_web_period_render_row     },
    { gemrtos_web_loopdelay_row_count,  gemrtos_web_loopdelay_render_row  },
};
#define GEMRTOS_WEB_SOURCE_COUNT \
    (sizeof(gemrtos_web_sources) / sizeof(gemrtos_web_sources[0]))

enum {
    GEMRTOS_WEB_SSI_TAG_SYSVARS = 0,
    GEMRTOS_WEB_SSI_TAG_PERIOD_MS,
    GEMRTOS_WEB_SSI_TAG_LOOP_MS,
};

/**
 * \brief Total number of rows currently available across all registered sources.
 *
 * \returns the sum of row_count() over gemrtos_web_sources[]. Recomputed on
 *          every call since sources such as a future task-list source report a
 *          count that can change between requests.
 */
static G_UINT32 gemrtos_web_total_row_count(void)
{
    G_UINT32 total = 0U;
    G_UINT32 i;

    for (i = 0U; i < GEMRTOS_WEB_SOURCE_COUNT; i++) {
        total += gemrtos_web_sources[i].row_count();
    }
    return total;
}

/**
 * \brief SSI tag handler for the web status page's tags: <!--#sysvars-->
 *        (multi-row status table), and the bare-value <!--#period_ms-->/
 *        <!--#loop_ms--> tags used to pre-fill the configuration form.
 *
 * The <!--#sysvars--> case maps a linear row index (current_tag_part) onto
 * one row from one source in gemrtos_web_sources[], renders it, and tells
 * the httpd core (via *next_tag_part) whether another row follows so it can
 * call this handler again. The two bare-value tags are single-shot: they
 * write one number and leave *next_tag_part untouched (no further parts).
 * Registered with http_set_ssi_handler() in tse_lwip_init() before
 * httpd_init() is called.
 *
 * \param [in]  tag_index        Index into gemrtos_web_ssi_tags[].
 * \param [out] insert           Buffer to write the tag's replacement text into.
 * \param [in]  insert_len       Size of \p insert, in bytes.
 * \param [in]  current_tag_part Zero-based row index for this call
 *                                (<!--#sysvars--> only; ignored otherwise).
 * \param [out] next_tag_part    Set to current_tag_part + 1 if another row
 *                                follows; left untouched (defaults to "no more
 *                                parts") otherwise.
 * \returns the number of bytes written into \p insert.
 */
static u16_t gemrtos_web_ssi_handler(int tag_index, char *insert, int insert_len,
                                      u16_t current_tag_part, u16_t *next_tag_part)
{
    G_UINT32 remaining_index;
    G_UINT32 source_idx;

    if (tag_index == GEMRTOS_WEB_SSI_TAG_PERIOD_MS) {
        return (u16_t) snprintf(insert, (u16_t) insert_len, "%lu",
                                (unsigned long) gemrtos_web_current_period_ms());
    }
    if (tag_index == GEMRTOS_WEB_SSI_TAG_LOOP_MS) {
        return (u16_t) snprintf(insert, (u16_t) insert_len, "%lu",
                                (unsigned long) g_infinite_loop_delay_ms);
    }
    if (tag_index != GEMRTOS_WEB_SSI_TAG_SYSVARS) {
        return 0;
    }

    remaining_index = (G_UINT32) current_tag_part;

    for (source_idx = 0U; source_idx < GEMRTOS_WEB_SOURCE_COUNT; source_idx++) {
        G_UINT32 rows = gemrtos_web_sources[source_idx].row_count();

        if (remaining_index < rows) {
            u16_t written = gemrtos_web_sources[source_idx].render_row(
                remaining_index, insert, (u16_t) insert_len);

            /* snprintf() returns the length it WOULD have written; clamp to what
             * actually landed in the buffer so we never report more bytes than
             * were written (see LWIP_HTTPD_MAX_TAG_INSERT_LEN in lwipopts.h). */
            if (written >= (u16_t) insert_len) {
                written = (u16_t) (insert_len - 1);
            }

            if ((G_UINT32) (current_tag_part + 1U) < gemrtos_web_total_row_count()) {
                *next_tag_part = (u16_t) (current_tag_part + 1U);
            }
            return written;
        }
        remaining_index -= rows;
    }

    /* current_tag_part points past the last available row -- nothing to emit. */
    return 0;
}

#if LWIP_HTTPD_CGI

/* Web-editable parameters are clamped to this range rather than rejected outright,
 * so a stray/malformed form submission cannot set the infinite-loop delay to 0
 * (which would busy-loop the task) or the periodic task's period to 0 (which
 * would trip PRINT_ASSERT((ptcb->TCBPeriod > 0), ...) inside gu_TaskPeriodSet()
 * and halt the system via G_DEBUG_WHILEFOREVER). */
#define GEMRTOS_WEB_MIN_PERIOD_MS    10U
#define GEMRTOS_WEB_MAX_PERIOD_MS  60000U

/**
 * \brief Parses a non-negative decimal string, clamped to
 *        [GEMRTOS_WEB_MIN_PERIOD_MS, GEMRTOS_WEB_MAX_PERIOD_MS].
 *
 * A small hand-rolled parser is used instead of atol()/strtoul() to avoid
 * pulling in <stdlib.h>, which this codebase deliberately does not include
 * project-wide (see the <stdlib.h> note at the top of gemrtos.h).
 *
 * \param [in] value  NUL-terminated decimal string (from the CGI parameter value).
 * \returns the parsed value, clamped to the valid range. Non-digit characters
 *          (including an empty string) parse as 0, which then clamps up to
 *          GEMRTOS_WEB_MIN_PERIOD_MS.
 */
static G_UINT32 gemrtos_web_parse_clamped_ms(const char *value)
{
    G_UINT32 result = 0U;

    while ((*value >= '0') && (*value <= '9')) {
        result = (result * 10U) + (G_UINT32) (*value - '0');
        value++;
    }

    if (result < GEMRTOS_WEB_MIN_PERIOD_MS) {
        result = GEMRTOS_WEB_MIN_PERIOD_MS;
    } else if (result > GEMRTOS_WEB_MAX_PERIOD_MS) {
        result = GEMRTOS_WEB_MAX_PERIOD_MS;
    }
    return result;
}

/**
 * \brief CGI handler for "/set.cgi" -- applies the web status page's editable
 *        runtime parameters (periodic task period, infinite-loop delay).
 *
 * Registered with http_set_cgi_handlers() in tse_lwip_init() before httpd_init().
 * Invoked for GET requests to "/set.cgi?param=value&..." (see the form in
 * fs/status.html). Recognises "period_ms" (periodic task period) and
 * "loop_delay_ms" (infinite-loop task delay); unrecognised parameters are
 * ignored. Always redirects back to the status page so the applied values are
 * visible immediately via the existing <!--#sysvars--> rows.
 *
 * \param [in] cgi_index   Index into gemrtos_web_cgi_table[] (unused, only one entry).
 * \param [in] num_params  Number of parameters in \p params / \p values.
 * \param [in] params      Array of parameter names.
 * \param [in] values      Array of parameter values (parallel to \p params).
 * \returns the URI to serve as the response ("/status.html").
 */
static const char *gemrtos_web_cgi_set_handler(int cgi_index, int num_params,
                                                char *params[], char *values[])
{
    int i;

    (void) cgi_index;

    for (i = 0; i < num_params; i++) {
        if (strcmp(params[i], "period_ms") == 0) {
            G_UINT32 new_period_ms = gemrtos_web_parse_clamped_ms(values[i]);
            if (g_periodic_task_tcb != (GS_TCB *) 0) {
                gu_TaskPeriodSet(g_periodic_task_tcb, 0, 0, 0, new_period_ms);
                GU_FPRINTF(stderr, "[web] Periodic task period set to %lu ms\n",
                           (unsigned long) new_period_ms);
            }
        } else if (strcmp(params[i], "loop_delay_ms") == 0) {
            g_infinite_loop_delay_ms = gemrtos_web_parse_clamped_ms(values[i]);
            GU_FPRINTF(stderr, "[web] Infinite-loop delay set to %lu ms\n",
                       (unsigned long) g_infinite_loop_delay_ms);
        }
    }

    return "/status.html";
}

#endif /* LWIP_HTTPD_CGI */

/**
 * \brief lwIP status callback -- called by the DHCP subsystem when an IP address is acquired.
 *
 * Prints the newly assigned IP address, subnet mask, and gateway to the debug UART, then
 * validates that the assigned IP and gateway belong to the same subnet
 * (i.e. (ip & netmask) == (gateway & netmask)) -- unless no gateway was
 * offered at all, which is a legitimate DHCP configuration (see below).
 *
 * This validation guards against two failure modes observed after cable reconnections
 * to a different network segment:
 *   - A stale lease from the previous segment is renewed instead of a fresh lease being
 *     requested, leaving the interface with an IP that does not match the new gateway.
 *   - The DHCP server hands out a gateway on a subnet the assigned IP cannot reach.
 * In both cases the interface would otherwise silently remain unreachable. When the
 * validation fails, the current lease is released and a fresh DHCP negotiation is
 * requested (DISCOVERY, not RENEW).
 *
 * \b gw \b = \b 0.0.0.0 \b is accepted without re-negotiating:
 * some DHCP servers legitimately hand out no gateway at all -- e.g. an
 * isolated test/board-bring-up segment with no route out -- which just means
 * this interface has no default route, not that the lease is stale or wrong.
 * The original check could never distinguish that from a real mismatch,
 * since `0.0.0.0 & netmask` is never equal to a nonzero `ip_network`, so it
 * rejected every gateway-less lease and looped release/re-negotiate forever
 * without ever converging. Confirmed on real hardware: this
 * exact code path is shared, unmodified, between the
 * board and the Windows simulation, so the bug was not Windows-specific --
 * only found because the Windows simulation happened to be brought up
 * against a gateway-less segment first.
 *
 * \param [in] pnetif  Pointer to the lwIP network interface that received an address.
 */
static void StatusCallback(struct netif* pnetif)
{
    // get IP and stuff
    gu_fprintf("[ethernet] Acquired IP address via DHCP client for interface: %s\n", pnetif->name);

    char buf[255];

    print_ipad(pnetif->ip_addr.addr, buf);
    gu_fprintf("[ethernet] IP address : %s\n", buf);

    print_ipad(pnetif->netmask.addr, buf);
    gu_fprintf("[ethernet] Subnet     : %s\n", buf);

    print_ipad(pnetif->gw.addr, buf);
    gu_fprintf("[ethernet] Gateway    : %s\n", buf);

    /* Validate that the assigned IP and gateway are coherent, i.e. reachable
     * on the same subnet. An incoherent pair indicates a stale lease from a
     * previous network segment. A gateway of 0.0.0.0 means none was offered
     * at all -- a valid "no default route" configuration, not a mismatch --
     * so it is accepted without comparing subnets. */
    if (pnetif->gw.addr == IPADDR_ANY) {
        gu_fprintf("[ethernet] IP validation PASSED: no gateway offered (no default route)\n");
        return;
    }

    /* An IP of 0.0.0.0 is the normal transient state right after
     * dhcp_release() (e.g. on LINK DOWN, LinkCallback() releases the lease
     * before this callback fires) -- not a wrong-subnet condition. Without
     * this guard, a stale gw.addr from the previous lease (not yet cleared)
     * always fails the subnet-coherence check below and re-triggers
     * dhcp_release()/dhcp_start() from inside this status callback, which
     * lwIP itself calls synchronously from within dhcp_release()'s own call
     * chain -- direct DHCP-client re-entrancy. Reproduced on real hardware
     * (2026-08-17): a physical link down/up cycle drove exactly this path
     * and hung the processor running this task permanently, with this
     * function's own "Triggering DHCP re-negotiation..." line the last
     * output ever printed. */
    if (pnetif->ip_addr.addr == IPADDR_ANY) {
        gu_fprintf("[ethernet] IP validation PASSED: no IP yet (DHCP in progress)\n");
        return;
    }

    G_UINT32 ip_network = pnetif->ip_addr.addr & pnetif->netmask.addr;
    G_UINT32 gw_network = pnetif->gw.addr & pnetif->netmask.addr;

    if (ip_network != gw_network) {
        gu_fprintf("[ERROR] IP validation FAILED: IP and gateway not in same subnet\n");
        gu_fprintf("[ERROR]   IP network:      0x%08lx\n", (unsigned long) ip_network);
        gu_fprintf("[ERROR]   Gateway network: 0x%08lx\n", (unsigned long) gw_network);
        gu_fprintf("[ERROR] Triggering DHCP re-negotiation...\n");

        dhcp_release(pnetif);
        dhcp_start(pnetif);
    } else {
        gu_fprintf("[ethernet] IP validation PASSED: configuration is coherent\n");
    }
}

/**
 * \brief lwIP link callback -- called by the lwIP stack when the Ethernet link state changes.
 *
 * Logs the link-state change event to the debug UART.
 *
 * On LINK DOWN, the current DHCP lease is stopped and released. Without this,
 * lwIP's DHCP client remains in the BOUND state with the old lease, so the next
 * dhcp_start() (issued when the link comes back up, see tse_lwip_init()) performs a
 * RENEW instead of a fresh DISCOVERY. A RENEW request targets the previous network's
 * DHCP server and is silently ignored or NAKed if the cable has been moved to a
 * different network segment, leaving the interface without a valid address. Releasing
 * the lease on link down guarantees that reconnection always starts from a clean
 * DISCOVERY, regardless of which network the cable is plugged into next.
 *
 * On LINK UP, only the event is logged; DHCP restart is driven by the caller
 * (tse_lwip_init() main loop), not by this callback.
 *
 * \param [in] pnetif  Pointer to the lwIP network interface whose link state changed.
 */
static void LinkCallback(struct netif* pnetif)
{
    if (netif_is_link_up(pnetif)) {
        gu_fprintf("[ethernet] Link Callback: LINK UP for interface: %s\n", pnetif->name);
    } else {
        char buf[255];
        print_ipad(pnetif->ip_addr.addr, buf);
        gu_fprintf("[ethernet] Link Callback: LINK DOWN for interface: %s\n", pnetif->name);
        gu_fprintf("[ethernet]   Previous IP released: %s\n", buf);

        /* Release the DHCP lease immediately so the next dhcp_start() performs
         * a DISCOVERY rather than a RENEW against a network that may no longer be
         * reachable through this cable. */
        dhcp_stop(pnetif);
        dhcp_release(pnetif);
    }
}


#endif  // (INCLUDE_TSE == 1) && defined(__ALTERA_ETH_TSE)


int main(void)
{
    GS_TCB *ptcb;

	// Create semaphore for exclusion in task
	mutex_leds = gu_SemaphoreCreateRecursiveMutex();

#if (INCLUDE_TASK_PERIODIC == 1)
    
	ptcb = gu_TaskCreate((void *) task_periodic,  // Pointer to the task code
					     (void *) 1,              // Call argument
					     "periodic task");        // task description

	if (ptcb != (GS_TCB *) 0) { /* Task was created successfully */
		/* Set the task as periodic */
		gu_TaskTypeSet(ptcb, G_TCBType_Periodic);   	
		
		gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 1);  /* Ready priority  */
		gu_TaskRunPrioritySet(ptcb,(G_UINT64) 1);     /* Run priority  */
		gu_TaskPeriodSet(ptcb, 0, 0, 0, PERIODIC_TASK_PERIOD_MS_DEFAULT);
		gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);     /* Starting time */

		/* Retain the TCB so the web status page can retune the period at
		 * runtime via gu_TaskPeriodSet() (see gemrtos_web_cgi_set_handler()). */
		g_periodic_task_tcb = ptcb;
	} else {
		gu_printf("Error when creating task\n");
		while(1);
	}
    
#endif  // (INCLUDE_TASK_PERIODIC == 1)

#if (INCLUDE_TASK_INFINITE_LOOP == 1)

	ptcb = gu_TaskCreate((void *) task_infinite_loop,  // Pointer to the task code
					     (void *) 2,                   // Call argument
					     "infinite-loop task");        // task description

	if (ptcb != (GS_TCB *) 0) { /* Task was created successfully */
		/* Set the task as one_shoot */
		gu_TaskTypeSet(ptcb, G_TCBType_OneShot);   	
		
		gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 1);  /* Ready priority  */
		gu_TaskRunPrioritySet(ptcb,(G_UINT64) 1);     /* Run priority  */

		gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);     /* Starting time at 5 s*/
	} else {
		gu_printf("Error when creating infinite_loop task\n");
		while(1);
	}

#endif // (INCLUDE_TASK_INFINITE_LOOP == 1)


#if (INCLUDE_PHILOSOPHERS == 1)

    /* Create semaphores for forks */
    for (int i = 0; i < NUMBER_PHILOSOPHERS; i++) {
        forks[i] = gu_SemaphoreCreateBinary(1);
        if (forks[i] == (t_semaphore_resource *) 0) {
            gu_printf("Error when creating fork semaphore %d\n", i);
            while(1);
        }
    }

    /* Create philosopher tasks */
    for (unsigned int i = 0; i < NUMBER_PHILOSOPHERS; i++) {
        ptcb = gu_TaskCreate((void *) task_philosopher,
                             (void *) i,
                             "task %d", i);
        if (ptcb != (GS_TCB *) 0) {
            gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
            gu_TaskReadyPrioritySet(ptcb, (G_UINT64) i + 10);
            gu_TaskRunPrioritySet(ptcb, (G_UINT64) i + 10);
            gu_TaskPeriodSet(ptcb, 0, 0, 1, (i + 1) * 100);
            gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);
            gu_SignalCreate(G_SCBType_TCB_ABORTED, 0, (void *) ptcb,
                            (void *) signal_philosopher_task_aborted, (void *) i);
        } else {
            gu_printf("Error when creating task\n");
            while(1);
        }
    }

#endif  // (INCLUDE_PHILOSOPHERS == 1)

    /* Execute the function sig_frozen_mode when enter in frozen mode                         */
	gu_SignalCreate(G_SCBType_FROZEN_MODE, 0, (void *) 0, (void *) sig_frozen_mode, (void *) 1);

#if (INCLUDE_TSE == 1) && defined(__ALTERA_ETH_TSE)

    /* Network Interface structure for LWIP interface */
    gemrtos_tse_mac_device[0].pnetif = &tse_netif_static; /* MISRA C:2012 Rule 21.3: static instead of malloc */
    /* relates netif and gemrtos_tse_mac_device structures */
    gemrtos_tse_mac_device[0].pnetif->state = (void *) &gemrtos_tse_mac_device[0];

    // It is important the name to associate it with the device in gemrtos_tse_mac_device array
    gemrtos_tse_mac_device[0].pnetif->name[0] = 'e';
    gemrtos_tse_mac_device[0].pnetif->name[1] = 'n';

#ifdef __niosX_arch__
    //  Load platform specific MAC address into netif.
    //  !!!  Replace TSE_MAC_BYTE_* constants (defined near top of file) with your hardware MAC.  !!!
    gemrtos_tse_mac_device[0].pnetif->hwaddr_len = ETH_HWADDR_LEN;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[0] = TSE_MAC_BYTE_0;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[1] = TSE_MAC_BYTE_1;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[2] = TSE_MAC_BYTE_2;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[3] = TSE_MAC_BYTE_3;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[4] = TSE_MAC_BYTE_4;
    gemrtos_tse_mac_device[0].pnetif->hwaddr[5] = TSE_MAC_BYTE_5;

    if (strcmp(ALT_DEVICE_FAMILY, "AGILEX3") == 0) {
        gemrtos_tse_mac_device[0].pnetif->hwaddr[0] = TSE_MAC_AGILEX3_BYTE_0;
        gemrtos_tse_mac_device[0].pnetif->hwaddr[1] = TSE_MAC_AGILEX3_BYTE_1;
    }

    /* Create message queue for TSE transmission */
    gemrtos_tse_mac_device[0].p_tse_queue_out = gu_MessageQueueCreate();
    if (gemrtos_tse_mac_device[0].p_tse_queue_out == (GS_RCB *) 0) {
        gu_printf("Error when creating TSE p_tse_queue_out\n");
        while(1);
    }
    gemrtos_tse_mac_device[0].tx_frame = tse_tx_frame_static; /* MISRA C:2012 Rule 21.3: static instead of malloc */

    gemrtos_tse_mac_device[0].buffer_tx_frame = (unsigned char *) &buffer_tx_frame;
    gemrtos_tse_mac_device[0].buffer_rx_frame = (unsigned char *) &buffer_rx_frame;
#endif  // __niosX_arch__
    /* On host, netif_pcap_init() (called from inside
     * tse_receive_server() via netif_add(), which runs later when that task
     * starts) sets pnetif->hwaddr/hwaddr_len from PCAP_NETIF_MAC_BYTE_* in
     * pcap_netif.h -- there is no hardware MAC to load here, and the TX
     * message-queue indirection above exists only for the mSGDMA path. */

    /***************************************/
    /* Create task for TSE receive server  */
	ptcb = gu_TaskCreate((void *) tse_receive_server,         // Pointer to the task code
					     (void *) &gemrtos_tse_mac_device[0], // Call argument
					     "tse_receive_server");               // task description

	if (ptcb != (GS_TCB *) 0) { /* Task was created successfully */
		/* Set the task as one_shoot */
		gu_TaskTypeSet(ptcb, G_TCBType_OneShot);   	
		
		gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);  /* Ready priority  */
		gu_TaskRunPrioritySet(ptcb,(G_UINT64) 0);     /* Run priority  */

		gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);     /* Starting time at 5 s*/
	} else {
		gu_printf("Error when creating task\n");
		while(1);
	}

    /***************************************/
    /* Create task for TSE transmit server */
	ptcb = gu_TaskCreate((void *) tse_transmit_server,        // Pointer to the task code
					     (void *) &gemrtos_tse_mac_device[0], // Call argument
					     "tse_transmit_server");              // task description

	if (ptcb != (GS_TCB *) 0) { /* Task was created successfully */
		/* Set the task as one_shoot */
		gu_TaskTypeSet(ptcb, G_TCBType_OneShot);   	
		
		gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);  /* Ready priority  */
		gu_TaskRunPrioritySet(ptcb,(G_UINT64) 0);     /* Run priority  */

		gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);     /* Starting time at 5 s*/
	} else {
		gu_printf("Error when creating task\n");
		while(1);
	}

#endif  // (INCLUDE_TSE == 1) && defined(__ALTERA_ETH_TSE)
    
#if (INCLUDE_JTAG_UART_SERVER == 1)

    /* Create a jtag_uart_info structure to store the information for output server consumer */
    /* jtag_uart_0_info already points to jtag_uart_0_info_storage - MISRA C:2012 Rule 21.3 */

    /* Create a trigger_out for JTAG UART writing (-1: not associated with hardware interrupt) */
    jtag_uart_0_info->trigger_out = gu_TriggerCreate(-1, 0);
    if (jtag_uart_0_info->trigger_out == (GS_RCB *) 0) {
        gu_printf("Error when creating JTAG UART trigger_out\n");
        while(1);
    }

    /* Create a trigger_in for JTAG UART reading (-1: not associated with hardware interrupt) */
    G_UINT64 timeout_trigger_in = gu_ConvertTime(0, 0, 0, TIMEOUT_JTAG_READ_MS);
    jtag_uart_0_info->trigger_in = gu_TriggerCreate(-1, timeout_trigger_in);
    if (jtag_uart_0_info->trigger_in == (GS_RCB *) 0) {
        gu_printf("Error when creating JTAG UART trigger_in\n");
        while(1);
    }

    /* Create a message queue for JTAG-UART jtag_uart_0 output */
    jtag_uart_0_info->pqueue_out = gu_MessageQueueCreate();
    if (jtag_uart_0_info->pqueue_out == (GS_RCB *) 0) {
        gu_printf("Error when creating JTAG UART pqueue_out\n");
        while(1);
    }

    /* Create a message queue for JTAG-UART jtag_uart_0 input */
    jtag_uart_0_info->pqueue_in = gu_MessageQueueCreate();
    if (jtag_uart_0_info->pqueue_in == (GS_RCB *) 0) {
        gu_printf("Error when creating JTAG UART pqueue_in\n");
        while(1);
    }

    jtag_uart_0_info->buffer_length_in  = JTAG_UART_BUFFER_MAX_IN;  
    jtag_uart_0_info->buffer_length_out = JTAG_UART_BUFFER_MAX_OUT; 
    jtag_uart_0_info->jtag_base = DEVICE_BASE(JTAG_UART_SERVER);

    /* Create task for JTAG output server */
	ptcb = gu_TaskCreate((void *) jtag_uart_write_server,        // Pointer to the task code
					     (void *) jtag_uart_0_info,              // Call argument
					     "jtag_uart_write_server");              // task description

	if (ptcb != (GS_TCB *) 0) { /* Task was created successfully */
		/* Set the task as one_shoot */
		gu_TaskTypeSet(ptcb, G_TCBType_OneShot);   	
		
		gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);  /* Ready priority  */
		gu_TaskRunPrioritySet(ptcb,(G_UINT64) 0);     /* Run priority  */

		gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);     /* Starting time at 5 s*/
	} else {
		gu_printf("Error when creating jtag_uart_write_server task\n");
		while(1);
	}

    /* Set the JTAG write server hook functions */
    gu_TriggerEnableHook(jtag_uart_0_info->trigger_out, jtag_uart_write_server_enable,
                         (void *) jtag_uart_0_info);
    gu_TriggerDisableHook(jtag_uart_0_info->trigger_out, jtag_uart_write_server_disable,
                          (void *) jtag_uart_0_info);

    /* Create task for JTAG input server */
	ptcb = gu_TaskCreate((void *) jtag_uart_read_server,        // Pointer to the task code
					     (void *) jtag_uart_0_info,             // Call argument
					     "jtag_uart_read_server");              // task description

	if (ptcb != (GS_TCB *) 0) { /* Task was created successfully */
		/* Set the task as one_shoot */
		gu_TaskTypeSet(ptcb, G_TCBType_OneShot);   	
		
		gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);  /* Ready priority  */
		gu_TaskRunPrioritySet(ptcb,(G_UINT64) 0);     /* Run priority  */

		gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);     /* Starting time at 5 s*/
	} else {
		gu_printf("Error when creating jtag_uart_read_server task\n");
		while(1);
	}

    /* Set the JTAG input server hook functions */
    gu_TriggerEnableHook(jtag_uart_0_info->trigger_in, jtag_uart_read_server_enable,
                         (void *) jtag_uart_0_info);
    gu_TriggerDisableHook(jtag_uart_0_info->trigger_in, jtag_uart_read_server_disable,
                          (void *) jtag_uart_0_info);

#ifdef __niosX_arch__
    /* Embedded-only -- jtag_uart_irq_manager itself is
     * embedded-only (see its definition above); there is no hardware IRQ on
     * host to create a trigger for. */

    /* Create an ISR task for JTAG UART hardware interrupt */
    ptcb = gu_TaskCreate((void *) jtag_uart_irq_manager,
                         (void *) jtag_uart_0_info,
                         "jtag_uart_irq_manager");
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE), "ERROR TCB is not valid\n");
    if (ptcb != (GS_TCB *) 0) {
        gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
        gu_TaskReadyPrioritySet(ptcb, (G_UINT64) 0);
        gu_TaskRunPrioritySet(ptcb, (G_UINT64) 0);
    }

    /* Associate the ISR task with the JTAG UART hardware IRQ trigger */
    GS_RCB *jtag_uart_trigger = gu_TriggerCreate(DEVICE_IRQ(JTAG_UART_SERVER), 0);
    if (jtag_uart_trigger == (GS_RCB *) 0) {
        gu_printf("Error when creating jtag_uart_trigger\n");
        while(1);
    }

    /* Register the jtag_uart_irq_manager task to the JTAG UART IRQ */
    gu_TriggerRegisterTask(ptcb, jtag_uart_trigger);

    /* Start system with JTAG UART IRQ enabled */
    gu_TriggerEnable(jtag_uart_trigger);
#endif  // __niosX_arch__ (JTAG UART hardware IRQ manager)

#endif  // (INCLUDE_JTAG_UART_SERVER == 1)
	
	
    return (0);
}
