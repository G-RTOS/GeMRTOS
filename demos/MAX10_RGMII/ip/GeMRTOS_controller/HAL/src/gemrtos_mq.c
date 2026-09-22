/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS Message queue functions
 *  \details This file contains the Message queue functions.
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


#include <gemrtos.h>
#include <stdlib.h>  /* abs() used in gk_MessageQueueTransferMessage */

/** \brief Maximum formatted-string length (bytes) for gu_MessageQueuePrintf.
 *  The actual char array is MQ_PRINTF_BUF_LEN + 2: +1 for the guaranteed null
 *  terminator at index MQ_PRINTF_BUF_LEN, +1 alignment pad. */
#define MQ_PRINTF_BUF_LEN  126U

OPTIMIZE_CODE

/** \class Message_Queue
The <b>message queue</b> is a resource that enables independent tasks to exchange information. Information is sent using messages by a task (denoted <em>producer</em>) to the <em>message queue</em>. A message remains associated with the message queue resources until (1) all the tasks associated with the message queue read it (aka <em>consume</em> it), or (2) a timeout expires. 
The producer task remain suspended until the message was consumed by all the MQreceivers or the timeout of the message expires. The tasks that want to receive the messages sent to the message queue, have to subscribe to the message queue.
\image html "https://gemrtos.com/images/message_queue.gif"
If a consumer task already received all the messages sent, then it remains suspended waiting until the next message.
 *  \exception sec_fun_intro The Message Queue category in GeMRTOS provides essential functions and macros for implementing inter-task communication through message passing mechanisms. This category enables tasks to exchange data and synchronize their operations efficiently, facilitating seamless collaboration within a real-time system. <br /> By utilizing the Message Queue category, developers can create, send, and receive messages between tasks, allowing for asynchronous communication that enhances system responsiveness. The functions within this category support various operations, including message queue creation, message enqueuing and dequeuing to ensure robust data exchange. <br /> With the capability to configure message priorities and handling, the Message Queue category not only streamlines communication but also aids in managing task dependencies and resource sharing. This is particularly important in complex applications where timely and reliable message transfer is critical. By ensuring effective inter-task communication, the Message Queue category plays a vital role in optimizing performance and contributing to the overall reliability of the GeMRTOS environment.
 */


/** gu_MessageQueueReceive
 *  \brief The gu_MessageQueueReceive function retrieves the next message from a message queue. The calling task must have previously subscribed to the queue using gu_MessageQueueSubscribe. The received message is copied into the buffer specified by buffer_msg.  If the message is larger than buffer_length, it will be truncated to fit the buffer.
 *  \exception par_following The function takes one parameter:
 *  \param [in] prcb A pointer to the GS_RCB structure of the message queue from which to receive the message.
 *  \return The gu_MessageQueueReceive function returns an integer representing the number of bytes actually received. This value may be less than buffer_length if the received message was shorter than the buffer or if the message was truncated due to buffer size limitations.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSend, gu_MessageQueueSubscribe, gu_MessageQueuePrintf
 *  \relates Message_Queue
 *  \remark gu_MessageQueueReceive
 */
int gu_MessageQueueReceive(GS_RCB *prcb) 
{
    GS_TCB *ptcb;
    GS_ECB *pevent_sender;
    GS_ECB *pevent;

    int result;
    
    PRINT_ASSERT((!(gm_TimeCountersResetGet())),"Function have to be called from task\n");

    gm_GeMRTOSCriticalSectionEnter();
		GK_SAMPLE_FUNCTION_BEGIN(10047)
    
        PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");
        
        ptcb = gk_PCB_GetCurrentTCB();
			PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
        
        pevent = gk_TCB_in_RCBGEL((GS_RCB *) prcb, (GS_TCB *) ptcb);
			PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid\n");
			PRINT_ASSERT((prcb == (GS_RCB *) pevent->ECB_AssocRCB),"ERROR RCB is task associated\n");

        /* This also correctly handles G_ECBType_MESSAGE_RECEIVER_SUBSCRIBED
         * (the initial state gu_MessageQueueSubscribe() now sets) without any
         * dedicated branch -- it is neither ABORTED nor BUSY, so control falls
         * straight through to the psender==NULL backlog search below, exactly
         * as intended for a receiver that has never called Receive() before.
         * That search transitions it to BUSY (via gk_MessageQueueTransferMessage,
         * if a backlog message is found) or to WAIT (line below, immediately
         * before genuinely suspending) -- SUBSCRIBED never lingers past this
         * function's first call. */
        if (pevent->ECBType != G_ECBType_MESSAGE_RECEIVER_ABORTED) {
            // Release the current producer for this consumer
            pevent_sender = pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.psender;  // Sender
            if (pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_BUSY) {
                if (pevent_sender == (GS_ECB *) 0) {
                    pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length = 0;
                    pevent->ECBType = G_ECBType_MESSAGE_RECEIVER_WAIT;
                    pevent_sender = (GS_ECB  *) 0;
                }
            }

            if (pevent_sender == (GS_ECB  *) 0) {
                GS_ECB *pevent_best = (GS_ECB  *) 0;
                pevent_sender = prcb->RCB_NextRCBWEL;   // Search for undeliverable message
                while (pevent_sender != (GS_ECB *) 0) {
                    PRINT_ASSERT((gkm_ECB_IsValid(pevent_sender) == G_TRUE),"ERROR ECB1 is not valid\n");

                    if (pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.MQ_msg_seq > pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.MQ_msg_seq)
                    {
                        if ((pevent_best == (GS_ECB *) 0) || (pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.MQ_msg_seq < pevent_best->ECB_RRDS->queue_buffer.rrds_mq_sender.MQ_msg_seq)) pevent_best = pevent_sender;
                    }
                    pevent_sender = pevent_sender->ECB_NextECB;
                }
                pevent_sender = pevent_best;
            }

            if (pevent_sender != (GS_ECB *) 0) {
                PRINT_ASSERT((gkm_ECB_IsValid(pevent_sender) == G_TRUE),"ERROR ECB is not valid\n");
                gk_MessageQueueTransferMessage(pevent, pevent_sender);
            }
            else
            {   
                // No new message, wait for next message
                pevent->ECBType = G_ECBType_MESSAGE_RECEIVER_WAIT;
                pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length = 0;
                gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);
                // #################################################
                PRINT_ASSERT((gm_IsPrcGranted()),"ERROR SIGNAL in critical section\n");
                PRINT_ASSERT((pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_BUSY),"ERROR ECB is not valid=%d\n", (int) pevent->ECBType);
            }
            result = pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length;
        }
        else
        {
            pevent->ECBType = G_ECBType_MESSAGE_RECEIVER_WAIT;
            pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length = 0;
            result = ERR_RECEIVE_ABORT;
        }

        GK_SAMPLE_FUNCTION_END(10047)
    gm_GeMRTOSCriticalSectionExit();

    return (result);
}

/** \brief Creates and links the sender ECB into the message queue waiting list.
 *         Allocates an ECB for the current producer task, assigns sender-ECB type,
 *         records the sequence number, links into the resource waiting-event list,
 *         allocates and populates the associated RRDS with message metadata.
 *  \param [in] prcb       Pointer to the message queue RCB.
 *  \param [in] ptcb       Pointer to the sending task's TCB.
 *  \param [in] pmsg       Pointer to the message payload (const; not copied here).
 *  \param [in] msg_length Length of the message in bytes.
 *  \return Pointer to the newly linked sender ECB.
 */
static GS_ECB *gk_MQ_CreateSenderECB(GS_RCB *prcb, GS_TCB *ptcb, const char *pmsg, int msg_length)
{
    GS_ECB *pevent = gk_ECB_GetFree(ptcb);
    if (pevent == (GS_ECB *) 0) { return (GS_ECB *) 0; }
    set_pointed_field(pevent, ECBType, G_ECBType_MESSAGE_SENDER_WAIT);
    pevent->ECBValue.i64 = (G_UINT64) prcb->queue.MQ_seq_send;
    prcb->queue.MQ_seq_send++;
    gk_RCBWEL_Link(prcb, ptcb, pevent);

    pevent->ECB_RRDS = gk_RRDS_GetFree();
    PRINT_ASSERT((gkm_RRDS_IsValid(pevent->ECB_RRDS) == G_TRUE), "ERROR RRDS is not valid\n");
    pevent->ECB_RRDS->RRDS_AsocECB = pevent;
    pevent->ECB_RRDS->queue_buffer.rrds_mq_sender.deliveries  = 0;
    pevent->ECB_RRDS->queue_buffer.rrds_mq_sender.MQreceivers = prcb->queue.MQ_subscribers;
    pevent->ECB_RRDS->queue_buffer.rrds_mq_sender.pmsg        = pmsg;
    pevent->ECB_RRDS->queue_buffer.rrds_mq_sender.msg_length  = (G_UINT32) msg_length;
    pevent->ECB_RRDS->queue_buffer.rrds_mq_sender.MQ_msg_seq  = prcb->queue.MQ_seq_send;
    return pevent;
}

/** \brief Arms a send-timeout event for an in-progress message send.
 *         Allocates a timeout ECB, associates it with the sender ECB via ECBAEL,
 *         sets its type and absolute expiry tick, and inserts it into the timer list.
 *  \param [in] pevent  Pointer to the sender ECB already in the waiting-event list.
 *  \param [in] ptcb    Pointer to the sending task's TCB.
 *  \param [in] timeout Absolute timeout value (system ticks).
 *  \return Pointer to the newly armed timeout ECB.
 */
static GS_ECB *gk_MQ_ArmSendTimeout(GS_ECB *pevent, GS_TCB *ptcb, G_UINT64 timeout)
{
    GS_ECB *pevent_timeout = gk_ECB_GetFree(ptcb);
    if (pevent_timeout == (GS_ECB *) 0) { return (GS_ECB *) 0; }
    gk_ECBAEL_Link(pevent, pevent_timeout);
    set_pointed_field(pevent_timeout, ECBType, G_ECBType_MESSAGE_SENDER_TIMEOUT);
    pevent_timeout->ECBValue.i64 = (G_UINT64) timeout;
    gk_ECBTL_Link(pevent_timeout);
    return pevent_timeout;
}

/** gu_MessageQueueSend
 *  \brief The gu_MessageQueueSend function transmits a message to a message queue. The sending task blocks until the message has been successfully delivered to all subscribed MQreceivers or until a timeout occurs.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] prcb A pointer to the GS_RCB structure representing the message queue resource. This pointer was returned by gu_MessageQueueCreate when the queue was created.
 *  \param [in] pmsg A pointer to the message data to be sent
 *  \param [in] msg_length An integer representing the length of the message to be sent, in bytes.
 *  \param [in] timeout A gt_time value specifying the timeout period for sending the message.
 *  \return The gu_MessageQueueSend function returns G_TRUE if the message was successfully sent within the timeout period, and G_FALSE otherwise.  G_FALSE indicates either a timeout or another error condition.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueReceive, gu_MessageQueueSubscribe, gu_MessageQueuePrintf
 *  \relates Message_Queue
 *  \remark gu_MessageQueueSend
 */
int gu_MessageQueueSend(GS_RCB *prcb, const char *pmsg, int msg_length, G_UINT64 timeout) {

    GS_ECB  *pevent    = (GS_ECB  *) 0;
    GS_ECB  *pevent_receiver = (GS_ECB  *) 0;
    GS_ECB  *pevent_timeout  = (GS_ECB  *) 0;
    int      delivered = 0;

    gm_GeMRTOSCriticalSectionEnter();
        GK_SAMPLE_FUNCTION_BEGIN(10048)

        PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE), "ERROR RCB is not valid\n");

        if (get_pointed_field(prcb, RCB_NextRCBGEL) == (struct gs_ecb *) 0) { /* No consumer in queue */
            GK_SAMPLE_FUNCTION_END(10048)
            gm_GeMRTOSCriticalSectionExit();
            return (delivered);
        }

        /* Step 1: Create and configure the sender ECB */
        GS_TCB *ptcb = gk_PCB_GetCurrentTCB();
        pevent = gk_MQ_CreateSenderECB(prcb, ptcb, pmsg, msg_length);
        if (pevent == (GS_ECB *) 0) {
            GK_SAMPLE_FUNCTION_END(10048)
            gm_GeMRTOSCriticalSectionExit();
            return 0;
        }

        /* Step 2: Arm the send timeout (if requested) */
        if (timeout != (G_UINT64) 0) {
            pevent_timeout = gk_MQ_ArmSendTimeout(pevent, ptcb, timeout);
            if (pevent_timeout == (GS_ECB *) 0) {
                gk_RCBWEL_Unlink(pevent);
                gk_ECBFL_Link(pevent);
                GK_SAMPLE_FUNCTION_END(10048)
                gm_GeMRTOSCriticalSectionExit();
                return 0;
            }
        }

        /* Step 3: Activate all waiting receivers */
        pevent_receiver = prcb->RCB_NextRCBGEL;
        while (pevent_receiver != (GS_ECB *) 0) {
            PRINT_ASSERT((gkm_ECB_IsValid(pevent_receiver) == G_TRUE), "ERROR ECB is not valid=%p\n", (void *) pevent_receiver);
            if (get_pointed_field(pevent_receiver, ECBType) == G_ECBType_MESSAGE_RECEIVER_WAIT) {
                pevent_receiver->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length = 0;
                gk_MessageQueueTransferMessage(pevent_receiver, pevent);
                PRINT_ASSERT((gkm_TCB_IsValid(pevent_receiver->ECB_AssocTCB) == G_TRUE), "ERROR TCB is not valid\n");
                PRINT_ASSERT(((pevent_receiver->ECB_AssocTCB->TCBState != G_TCBState_READY) && (pevent_receiver->ECB_AssocTCB->TCBState != G_TCBState_RUNNING)), "ERROR TCBState is not valid= %d\n", pevent_receiver->ECB_AssocTCB->TCBState);
                gk_TCB_Unlink(pevent_receiver->ECB_AssocTCB);
                gk_TCBRDYL_Link(pevent_receiver->ECB_AssocTCB);
            }
            pevent_receiver = pevent_receiver->ECB_NextECB;
        }

        /* Step 4: Suspend until delivered or timed out */
        if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_WAIT) {
            gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);
            PRINT_ASSERT((gm_IsPrcGranted()), "ERROR SIGNAL in critical section\n");
        }

        if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_DELIVERED) {
            delivered = (int) pevent->ECB_RRDS->queue_buffer.rrds_mq_sender.deliveries;
        } else {
            PRINT_ASSERT((pevent->ECBType == G_ECBType_MESSAGE_SENDER_EXPIRED), "ERROR in pevent type= %d", pevent->ECBType);
            delivered = (-1 * (int) pevent->ECB_RRDS->queue_buffer.rrds_mq_sender.deliveries);
        }

        if (timeout != (G_UINT64) 0) { gk_ECBFL_Link(pevent_timeout); }
        gk_ECBFL_Link(pevent);

        GK_SAMPLE_FUNCTION_END(10048)
    gm_GeMRTOSCriticalSectionExit();
    return (delivered);
}

/**gu_MessageQueuePrintf
 *  \brief The gu_MessageQueuePrintf function sends a formatted message to a message queue.  The calling task will block until the message is successfully delivered to all MQreceivers subscribed to the queue.
 *  \exception par_following The function accepts the following parameters:
 *  \param [in] prcb  A pointer to the GS_RCB structure representing the message queue. This pointer is the value returned by gu_MessageQueueCreate when the queue was created.
 *  \param format A null-terminated string containing the format string, similar to the standard printf function. This string can include format specifiers (e.g., %d, %s, %x) that are replaced by subsequent arguments.
 *  \return The function returns G_TRUE if the message was successfully sent to the queue, and G_FALSE otherwise.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSend, gu_MessageQueueReceive, gu_MessageQueueSubscribe
 *  \relates Message_Queue
 *  \remark gu_MessageQueuePrintf
 */
G_UINT32 gu_MessageQueuePrintf(GS_RCB *prcb, const char *format, ...)
{
 
    char fmt_buf[MQ_PRINTF_BUF_LEN + 2U];
	fmt_buf[MQ_PRINTF_BUF_LEN + 1U] = 0;

    va_list args;
    va_start (args, format);
    vsnprintf(fmt_buf, MQ_PRINTF_BUF_LEN, format, args);
    va_end (args);

	/* Always routed through the kernel message queue, on every platform.
	 *
	 * This used to be a platform split: embedded called
	 * gu_MessageQueueSend(), while Windows host builds did a bare
	 * gk_printf("%s", fmt_buf) instead. That shortcut silently broke this
	 * function's own documented contract on Windows -- it neither delivered
	 * to subscribers nor blocked the sender -- so any code using message
	 * queues as a real publish/subscribe mechanism (rather than as a console
	 * pipe) simply did not work there: subscribers waited forever on a queue
	 * nothing was ever sent to, while the publisher sailed past its
	 * never-taken block point. Found by verify_windows.ps1 running
	 * 05_message_queue_pubsub, whose Scenario A is exactly that use case;
	 * its Scenarios B and C always worked
	 * because they call gu_MessageQueueSend() directly.
	 *
	 * The shortcut was justifiable when it was written: the Windows build had
	 * no jtag_uart_write_server to consume the queue (INCLUDE_JTAG_UART_SERVER
	 * was 0 on host), so routing console output into a queue would have sent
	 * it nowhere. That justification no longer holds -- the host now
	 * has a real write server subscribing to pqueue_out and draining it to
	 * that processor's TCP console.
	 *
	 * Consequence, deliberate: publishing to a queue with no subscriber now
	 * discards the message on Windows instead of printing it to stdout --
	 * which is precisely what embedded already does, so the two platforms
	 * finally agree. The only such call sites are the pqueue_in publishes in
	 * the JTAG UART read servers, whose visible text is separately printed by
	 * the gu_fprintf() immediately above each of them. */
	gu_MessageQueueSend(prcb, fmt_buf, (G_UINT32) (strlen(fmt_buf) + 1), (G_UINT64) 0);

	return (G_TRUE);
}

/**
 *  \brief The gu_MessageQueueCreate function creates a new message queue resource.  This resource is implemented using a GS_RCB structure, extended with fields from a T_QUEUE_RESOURCE structure. The created queue includes event lists for producers (waiting to send) and MQreceivers (waiting to receive) messages. Producer tasks add themselves to the producer event list when they are waiting to send a message.  This function can be called from either the main application code or from within a task.  If called within a task, it must be called before any message send or receive operations; otherwise, an error will occur.
 *  \exception par_following The gu_MessageQueueCreate function takes no parameters.
 *  \return The gu_MessageQueueCreate function returns a pointer (GS_RCB *) to the newly created message queue resource. This pointer is essential for all subsequent operations in the queue. A NULL pointer is returned if there is insufficient memory to create the queue or if no more queue resources are available.
 *  \sa gu_MessageQueueSend, gu_MessageQueueReceive, gu_MessageQueueSubscribe, gu_MessageQueuePrintf
 *  \relates Message_Queue
 *  \remark gu_MessageQueueCreate
 */
GS_RCB  *gu_MessageQueueCreate(void)
{
    GS_RCB *prcb;
    gm_GeMRTOSCriticalSectionEnter();
		GK_SAMPLE_FUNCTION_BEGIN(10050)
    
	prcb = gk_RCB_GetFree();
	if (prcb != (GS_RCB *) 0) {
		prcb->RCBType = (G_UINT32) GS_RCBType_QUEUE;

		prcb->queue.MQ_subscribers = (G_UINT32) 0;
		prcb->queue.MQ_seq_send = (G_UINT32) 0;
	}
		GK_SAMPLE_FUNCTION_END(10050)
    gm_GeMRTOSCriticalSectionExit();
	return((GS_RCB  *) prcb);
}


/** gu_MessageQueueDestroy
 *  \brief Destroys a message queue created by gu_MessageQueueCreate, returning its
 *         Resource Control Block to the free pool.  The queue must be idle: it must
 *         have no subscribers and no sender or receiver events pending.  If any task
 *         is still subscribed or blocked on the queue the function does nothing and
 *         returns G_FALSE — the caller must unsubscribe/abort those tasks first
 *         (e.g. gu_TaskKill, which removes a subscriber and drops MQ_subscribers).
 *  \exception par_following The function accepts one parameter:
 *  \param [in] pqueue A pointer to the GS_RCB of the message queue to destroy.
 *  \return G_TRUE if the queue was destroyed; G_FALSE if it is still in use.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSubscribe
 *  \relates Message_Queue
 */
G_UINT32 gu_MessageQueueDestroy(GS_RCB *pqueue)
{
	G_UINT32 result = G_FALSE;
	gm_GeMRTOSCriticalSectionEnter();
		PRINT_ASSERT((gkm_RCB_IsValid(pqueue) == G_TRUE),"ERROR RCB not valid, %p\n", (void *) pqueue);
		PRINT_ASSERT((pqueue->RCBType == (G_UINT32) GS_RCBType_QUEUE),"ERROR not a queue, RCBType= %d\n", (int) pqueue->RCBType);

		/* Idle only: no subscriber and no pending sender/receiver events. */
		if ((pqueue->queue.MQ_subscribers == (G_UINT32) 0) &&
		    (get_pointed_field(pqueue, RCB_NextRCBGEL) == (struct gs_ecb *) 0) &&
		    (get_pointed_field(pqueue, RCB_NextRCBWEL) == (struct gs_ecb *) 0)) {
			set_pointed_field(pqueue, RCBType, GS_RCBType_FREE);
			gk_RCBFL_Link(pqueue);
			result = G_TRUE;
		}
	gm_GeMRTOSCriticalSectionExit();
	return (result);
}


/** gu_MessageQueueSubscribe
 *  \brief The gu_MessageQueueSubscribe function subscribes the task to a message queue resource. This subscription is crucial for message delivery;  a consumer task must be subscribed to a queue before it can receive messages using gu_MessageQueueReceive.  Furthermore, the execution of this function for each receiving task allows the message queue to track the number of MQreceivers subscribed. This count is essential for producers; a producer message is only considered fully delivered when it has been received by every subscribed consumer. The message queue must have been previously created using gu_MessageQueueCreate.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] ptcb A pointer to the Task Control Block (GS_TCB) of the task being subscribed to the queue.
 *  \param [in] presource A pointer to the GS_RCB structure representing the message queue resource to which the task is subscribing.
 *  \param [in] buffer_msg A pointer to the memory buffer where the received message will be stored.
 *  \param [in] buffer_length An integer specifying the maximum number of bytes to receive.
 *  \return The gu_MessageQueueSubscribe function returns a pointer to the GS_ECB structure associated with the message queue resource.  A NULL return value likely indicates an error.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSend, gu_MessageQueueReceive
 *  \relates Message_Queue
 *  \remark gu_MessageQueueSubscribe
 */
GS_ECB *gu_MessageQueueSubscribe(GS_TCB  *ptcb, GS_RCB  *presource, void * buffer_msg, G_UINT32 buffer_length)
{
	GS_ECB  *pevent = (GS_ECB  *) 0;
    
    gm_GeMRTOSCriticalSectionEnter();
		GK_SAMPLE_FUNCTION_BEGIN(10051)

		PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");

		pevent = gk_ECB_GetFree(ptcb);
		if (pevent == (GS_ECB *) 0) {
			GK_SAMPLE_FUNCTION_END(10051)
			gm_GeMRTOSCriticalSectionExit();
			return (GS_ECB *) 0;
		}
		pevent->ECBValue.i64 = G_LOWEST_PRIORITY;

		pevent = gk_RCBGEL_Link(presource, ptcb, pevent); // Link the ECB to RCBGEL
		presource->queue.MQ_subscribers++;

		pevent->ECB_RRDS = gk_RRDS_GetFree();
		PRINT_ASSERT((gkm_RRDS_IsValid(pevent->ECB_RRDS) == G_TRUE),"ERROR RRDS is not valid\n");
		pevent->ECB_RRDS->RRDS_AsocECB = pevent;

		PRINT_ASSERT((buffer_length < MQ_BUFFER_MAX_LENGTH), "ERROR in buffer length");
        pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.buffer_msg    = buffer_msg;
		pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.buffer_length = buffer_length;
		pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.MQ_msg_seq = (G_UINT32) 0; 
		pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.psender = (GS_ECB  *) 0;
		pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length = 0;
        /* Not yet WAIT -- this task is still running (it is the one
         * calling Subscribe), not suspended, so it must not look deliverable-
         * and-wakeable to a concurrent gu_MessageQueueSend() until its own
         * first gu_MessageQueueReceive() call genuinely finds nothing and
         * suspends. See G_ECBType_MESSAGE_RECEIVER_SUBSCRIBED's own comment
         * (gemrtos_core.h). */
        pevent->ECBType = G_ECBType_MESSAGE_RECEIVER_SUBSCRIBED;

		GK_SAMPLE_FUNCTION_END(10051)
    gm_GeMRTOSCriticalSectionExit();
    return (pevent);
}


/**
 *  \brief Called from the timed-event handler when a send timeout
 *         (G_ECBType_MESSAGE_SENDER_TIMEOUT) fires.  Marks the sender and all
 *         paired receivers as expired and moves the sending task to ready.
 *  \param [in] pevent_timeout Pointer to the timed ECB that has expired
 *                             (type G_ECBType_MESSAGE_SENDER_TIMEOUT).
 *  \return G_TRUE always.
 */
G_UINT32 gk_MessageQueueTimeoutECBSender(GS_ECB *pevent_timeout) {
    GS_TCB  *ptcb;
	GK_SAMPLE_FUNCTION_BEGIN(10052)
    
	pevent_timeout->ECBType = G_ECBType_MESSAGE_SENDER_EXPIRED;
    
    // Abort all the receptions
    GS_ECB *pevent = pevent_timeout->ECB_NextECBAEL;
    pevent->ECBType = G_ECBType_MESSAGE_SENDER_EXPIRED;
    gk_MessageQueueKillECBSender(pevent);
    
    ptcb = pevent_timeout->ECB_AssocTCB;
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
    gk_TCB_Unlink(ptcb);
    gk_TCBRDYL_Link(ptcb);
	
	GK_SAMPLE_FUNCTION_END(10052)
    return (G_TRUE);
}

/**
 *  \brief Called from the timed-event handler when a receive timeout
 *         (G_ECBType_MESSAGE_RECEIVER_WAIT) fires.  Marks the receiver ECB
 *         as expired and moves the waiting task to ready.
 *  \param [in] pevent_timeout Pointer to the timed ECB that has expired
 *                             (type G_ECBType_MESSAGE_RECEIVER_WAIT).
 *  \return G_TRUE always.
 */
G_UINT32 gk_MessageQueueTimeoutECBReceiver(GS_ECB *pevent_timeout) {
    GS_TCB  *ptcb;
	GK_SAMPLE_FUNCTION_BEGIN(10053)
	
    pevent_timeout->ECBType = G_ECBType_MESSAGE_RECEIVER_EXPIRED;
    
    GS_ECB *pevent = pevent_timeout->ECB_NextECBAEL;
    pevent->ECBType = G_ECBType_MESSAGE_RECEIVER_EXPIRED;
    PRINT_ASSERT((pevent_timeout->ECB_AssocTCB == pevent->ECB_AssocTCB), "ERROR in associated ECB: timeout ECB and receiver ECB must belong to the same task");
    
    ptcb = pevent_timeout->ECB_AssocTCB;
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
    gk_TCB_Unlink(ptcb);
    gk_TCBRDYL_Link(ptcb);
	
	GK_SAMPLE_FUNCTION_END(10053)
    return (G_TRUE);
}

/**
 *  \brief Cancels all receiver ECBs currently waiting for a message from the
 *         killed sender ECB.
 *         Called when a sending task is killed.  Walks the resource granted-event
 *         list and, for every receiver whose psender points to pevent, nulls out
 *         the sender pointer and marks the receiver ECB as
 *         G_ECBType_MESSAGE_RECEIVER_ABORTED so the receiving task detects the
 *         abort on its next run.
 *  \param [in] pevent  Pointer to the sender ECB being killed.
 *  \return G_TRUE always.
 *  \relates Message_Queue
 */
G_UINT32 gk_MessageQueueKillECBSender(GS_ECB *pevent)
{
	GK_SAMPLE_FUNCTION_BEGIN(10054)
    // Producer was killed; receivers paired with it must be aborted.
    GS_RCB *prcb = (GS_RCB *) pevent->ECB_AssocRCB;
    PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");
	
    /* RCB_NextRCBGEL is the GEL head; it may be NULL when no receivers are
     * waiting (sender killed before anyone subscribed, or after all deliveries
     * completed).  The loop guard protects the per-node validity check.    */
    GS_ECB *pevent_receiver = prcb->RCB_NextRCBGEL;

    while (pevent_receiver != (GS_ECB *) 0) {
        PRINT_ASSERT((gkm_ECB_IsValid(pevent_receiver) == G_TRUE),"ERROR ECB is not valid\n");
        if (pevent_receiver->ECB_RRDS->queue_buffer.rrds_mq_receiver.psender == pevent)
        {
            pevent_receiver->ECB_RRDS->queue_buffer.rrds_mq_receiver.psender = (GS_ECB *) 0;
            pevent_receiver->ECBType = G_ECBType_MESSAGE_RECEIVER_ABORTED;
        }
        pevent_receiver = pevent_receiver->ECB_NextECB;
    }
	
	GK_SAMPLE_FUNCTION_END(10054)
    return (G_TRUE);
}

/**
 *  \brief Cleans up a receiver ECB when the receiving task is killed.
 *         If the receiver is paired with a sender (psender != NULL), decrements
 *         the sender's MQreceivers count.  If the sender has delivered to all
 *         remaining receivers, marks it as G_ECBType_MESSAGE_SENDER_DELIVERED
 *         and moves the sending task to the ready list.  Frees the RRDS and
 *         the receiver ECB unconditionally.
 *  \param [in] pevent  Pointer to the receiver ECB being killed.
 *  \return G_TRUE always.
 *  \relates Message_Queue
 */
G_UINT32 gk_MessageQueueKillECBReceiver(GS_ECB *pevent)
{
    GS_ECB *pevent_sender = pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.psender; 
    GK_SAMPLE_FUNCTION_BEGIN(10081)
    if (pevent_sender != (GS_ECB *) 0) {
        PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid\n");
        PRINT_ASSERT((gkm_ECB_IsValid(pevent_sender) == G_TRUE),"ERROR ECB is not valid\n");        
        pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.MQreceivers--;
        if (pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.deliveries >= pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.MQreceivers) {
            // Message was delivered and task should be resumed               
			pevent_sender->ECBType = G_ECBType_MESSAGE_SENDER_DELIVERED;
            if (pevent_sender->ECB_AssocTCB->TCBState == G_TCBState_WAITING) {
                gk_RCBWEL_Unlink(pevent_sender);
                    PRINT_ASSERT((gkm_TCB_IsValid(pevent_sender->ECB_AssocTCB) == G_TRUE),"ERROR TCB is not valid\n");
                    PRINT_ASSERT((pevent_sender->ECB_AssocTCB->TCBState == G_TCBState_WAITING),"ERROR TCB is not waiting=%d\n", (int) pevent_sender->ECB_AssocTCB->TCBState);
                gk_TCB_Unlink(pevent_sender->ECB_AssocTCB);
                gk_TCBRDYL_Link(pevent_sender->ECB_AssocTCB);
            }             
        }
    }
    /* A killed subscriber no longer participates in delivery; drop it
     * from the queue's subscriber count so a producer waiting for delivery to
     * "every subscriber" is not left waiting for a task that no longer exists.
     * ECB_AssocRCB still points to the queue here — the caller clears it when
     * it frees this event below. */
    {
        GS_RCB *prcb = (GS_RCB *) pevent->ECB_AssocRCB;
        if ((prcb != (GS_RCB *) 0) &&
            (prcb->RCBType == (G_UINT32) GS_RCBType_QUEUE) &&
            (prcb->queue.MQ_subscribers > (G_UINT32) 0)) {
            prcb->queue.MQ_subscribers--;
        }
    }

    if (pevent->ECB_RRDS != (struct gs_rrds *) 0) {
        gk_RRDSFL_Link(pevent->ECB_RRDS);
        pevent->ECB_RRDS = (struct gs_rrds *) 0;   /* no dangling RRDS after free */
    }
    /* Do NOT free pevent here.  gk_MessageQueueKillECBReceiver is
     * called from gk_ResourceECBKillCallback inside gk_TCB_List_Unlink, which
     * frees the walked event itself (gk_ECBFL_Link) — matching the semaphore
     * kill convention.  Freeing it here double-freed the block and halted the
     * kernel via G_DEBUG_WHILEFOREVER. */
   	GK_SAMPLE_FUNCTION_END(10081)
	return (G_TRUE);
}

/**
 *  \brief Copies the next pending chunk of a sender's message into a receiver's
 *         buffer and updates delivery accounting.
 *         Pairs the receiver ECB (pevent) with the sender ECB (pevent_sender), copies
 *         min(buffer_length, remaining_bytes) bytes via memcpy, and increments
 *         msg_length in the receiver RRDS.  When the full message is transferred,
 *         increments the sender's deliveries counter and, if all subscribers have
 *         received the message, marks the sender as
 *         G_ECBType_MESSAGE_SENDER_DELIVERED and moves the sending task to the
 *         ready list.
 *  \param [in,out] pevent   Pointer to the receiver ECB that will receive data.
 *  \param [in,out] pevent_sender  Pointer to the sender ECB supplying the message.
 *  \return G_TRUE always.
 *  \relates Message_Queue
 */
int gk_MessageQueueTransferMessage(GS_ECB *pevent, GS_ECB *pevent_sender)
{
	GK_SAMPLE_FUNCTION_BEGIN(10080)
	
	pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.psender = pevent_sender;
    pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.MQ_msg_seq = pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.MQ_msg_seq;
	pevent->ECBType = G_ECBType_MESSAGE_RECEIVER_BUSY;
    pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length = abs(pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length);
	
	PRINT_ASSERT((gkm_RRDS_IsValid(pevent_sender->ECB_RRDS) == G_TRUE),"ERROR RRDS is not valid\n");
	PRINT_ASSERT(((uintptr_t) pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.buffer_msg % 4U == 0U), "ERROR memory is not alligned"); /* MISRA R.11.6: uintptr_t permits pointer-to-integer for address arithmetic */
	PRINT_ASSERT(((uintptr_t) pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.pmsg % 4U == 0U), "ERROR memory is not alligned");       /* MISRA R.11.6: uintptr_t permits pointer-to-integer for address arithmetic */

	unsigned int received = pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length;
	unsigned int sent = pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.msg_length;
	unsigned int remains = sent - received;

	/* csrc is const because rrds_mq_sender.pmsg is const char * (MISRA C:2012 Rule 8.13)        */
	/* Direct pointer arithmetic replaces the previous unsigned-int cast (MISRA R.11.4 violation) */
	const char *csrc  = pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.pmsg    + (ptrdiff_t) received;
	char       *cdest = pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.buffer_msg + (ptrdiff_t) received;

	unsigned int size;
	if (pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.buffer_length > remains) size = remains;
	else size = pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.buffer_length;

	memcpy(cdest, csrc, size);
	pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length += size;
   
    if (pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length >= pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.msg_length)
    {
		pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.psender = (GS_ECB  *) 0;
        PRINT_ASSERT((gkm_ECB_IsValid(pevent_sender) == G_TRUE),"ERROR ECB is not valid\n");
        pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.deliveries++;
		if (pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.deliveries >= pevent_sender->ECB_RRDS->queue_buffer.rrds_mq_sender.MQreceivers) {
			// Message was delivered and task should be resumed
			pevent_sender->ECBType = G_ECBType_MESSAGE_SENDER_DELIVERED;
            if (pevent_sender->ECB_AssocTCB->TCBState == G_TCBState_WAITING) {
                gk_RCBWEL_Unlink(pevent_sender);
                    PRINT_ASSERT((gkm_TCB_IsValid(pevent_sender->ECB_AssocTCB) == G_TRUE),"ERROR TCB is not valid\n");
                    PRINT_ASSERT((pevent_sender->ECB_AssocTCB->TCBState == G_TCBState_WAITING),"ERROR TCB is not waiting=%d\n", (int) pevent_sender->ECB_AssocTCB->TCBState);
                gk_TCB_Unlink(pevent_sender->ECB_AssocTCB);
                gk_TCBRDYL_Link(pevent_sender->ECB_AssocTCB);
            }
		}		
    }
    else
    {
        pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length *= (-1);
    }
	
	GK_SAMPLE_FUNCTION_END(10080)
    return (pevent->ECB_RRDS->queue_buffer.rrds_mq_receiver.msg_length);
}

OPTIMIZE_RESTORE
