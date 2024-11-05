/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Message queue functions
 *  \details This file contains the Message queue functions.
 *  \version 0.9a
 *  \date      2015-2020
 *  \bug  None known
 *  \copyright Ricardo Cayssials  (rcayssials@gemrtos.com)
 *  \copyright All rights reserved.
 *  \copyright Copyright (c) 2013-2020
 *  \warning This product was produced by Ricardo Cayssials in the hope that it 
 *  will be useful, but WITHOUT ANY WARRANTY; without even the implied 
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  \warning DISCLAIMER OF WARRANTY. THIS PRODUCT IS LICENSED "AS IS." YOU BEAR THE RISK OF USING IT. 
 *  \warning RICARDO CAYSSIALS GIVES NO EXPRESS WARRANTIES, GUARANTEES, OR CONDITIONS. 
 *  TO THE EXTENT PERMITTED UNDER APPLICABLE LAWS, RICARDO CAYSSIALS EXCLUDES ALL IMPLIED WARRANTIES,
 *  INCLUDING MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
 */
  
/******************************************************************************
*                                                                             *
* License Agreement                                                           *
* Copyright (c) Ricardo L. Cayssials                                          *
* All rights reserved.                                                        *
*                                                                             *
******************************************************************************/

#include <gemrtos.h>


OPTIMEZE_CODE(0)

/** \class Message_Queue
The <b>message queue</b> is a resource that enables independent tasks to exchange information. Information is sent using messages by a task (denoted <em>producer</em>) to the <em>message queue</em>. A message remains associated with the message queue resources until (1) all the tasks associated with the message queue read it (aka <em>consume</em> it), or (2) a timeout expires. 
The producer task remain suspended until the message was consumed by all the consumers or the timeout of the message expires. The tasks that want to receive the messages sent to the message queue, have to subscribe to the message queue.
\image html "https://gemrtos.com/images/message_queue.gif"
If a consumer task already received all the messages sent, then it remains suspended waiting until the next message.
 *  \exception sec_fun_intro The Message Queue category in GeMRTOS provides essential functions and macros for implementing inter-task communication through message passing mechanisms. This category enables tasks to exchange data and synchronize their operations efficiently, facilitating seamless collaboration within a real-time system. <br /> By utilizing the Message Queue category, developers can create, send, and receive messages between tasks, allowing for asynchronous communication that enhances system responsiveness. The functions within this category support various operations, including message queue creation, message enqueuing and dequeuing to ensure robust data exchange. <br /> With the capability to configure message priorities and handling, the Message Queue category not only streamlines communication but also aids in managing task dependencies and resource sharing. This is particularly important in complex applications where timely and reliable message transfer is critical. By ensuring effective inter-task communication, the Message Queue category plays a vital role in optimizing performance and contributing to the overall reliability of the GeMRTOS environment.
 */


/** gu_MessageQueueReceive
 *  \brief The gu_MessageQueueReceive function retrieves the next message from a message queue. The calling task must have previously subscribed to the queue using gu_MessageQueueSubscribe. The received message is copied into the buffer specified by buffer_msg.  If the message is larger than buffer_length, it will be truncated to fit the buffer.
 *  \exception par_following The function takes three parameters:
 *  \param [in] prcb A pointer to the G_RCB structure of the message queue from which to receive the message.
 *  \param [in] buffer_msg A pointer to the memory buffer where the received message will be stored.
 *  \param [in] buffer_length An integer specifying the maximum number of bytes to receive. This should be equal to or greater than the size of the buffer_msg buffer.
 *  \return The gu_MessageQueueReceive function returns an integer representing the number of bytes actually received. This value may be less than buffer_length if the received message was shorter than the buffer or if the message was truncated due to buffer size limitations.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSend, gu_MessageQueueSubscribe, gu_MessageQueuePrintf
 *  \relates Message_Queue
 *  \remark gu_MessageQueueReceive
 */
int gu_MessageQueueReceive(G_RCB *prcb, void * buffer_msg, G_INT32 buffer_length) 
{
    GS_TCB *ptcb;
    GS_ECB *pevent1;
    GS_ECB *pevent;
    int length = 0;
    
    PRINT_ASSERT((G_Running == G_TRUE),"Function have to be called from task\n");
    
    gm_GeMRTOSCriticalSectionEnter;
		GK_SAMPLE_FUNCTION_BEGIN(10047)
    
        PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");
        
        ptcb = gk_PCB_GetCurrentTCB();
			PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
        
        pevent = gk_TCB_in_RCBGEL((G_RCB *) prcb, (GS_TCB *) ptcb);
			PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid\n");
			PRINT_ASSERT((prcb == (G_RCB *) pevent->ECB_AssocRCB),"ERROR RCB is task associated\n");
			PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");

        pevent->ECB_RRDS->queue_buffer.rrds_consumer.buffer_msg    = buffer_msg;
        // 15-07-2024 pevent->ECB_RRDS->queue_buffer.rrds_consumer.buffer_length = buffer_length;
			PRINT_ASSERT((buffer_length < 300), "ERROR in buffer length");
        
        // Release the current producer for this consumer
        if (pevent->ECB_RRDS->queue_buffer.rrds_consumer.pproducer != (GS_ECB  *) 0) {
            pevent1 = pevent->ECB_RRDS->queue_buffer.rrds_consumer.pproducer;  // Producer
				PRINT_ASSERT((gkm_ECB_IsValid(pevent1) == G_TRUE),"ERROR ECB is not valid\n");
            pevent1->ECB_RRDS->queue_buffer.rrds_producer.deliveries++;
            if (pevent1->ECB_RRDS->queue_buffer.rrds_producer.deliveries >= pevent1->ECB_RRDS->queue_buffer.rrds_producer.consumers) {
                // Message was delivered and task should be resumed
                pevent1->ECBType = G_ECBType_MESSAGE_PRODUCER_DELIVERED;
					PRINT_ASSERT((gkm_TCB_IsValid(pevent1->ECB_AssocTCB) == G_TRUE),"ERROR TCB is not valid\n");
					PRINT_ASSERT((pevent1->ECB_AssocTCB->TCBState == G_TCBState_WAITING),"ERROR TCB is not wiating=%d\n", (int) pevent1->ECB_AssocTCB->TCBState);                
                gk_TCB_Unlink(pevent1->ECB_AssocTCB);
                gk_TCBRDYL_Link(pevent1->ECB_AssocTCB);
            }
            pevent->ECB_RRDS->queue_buffer.rrds_consumer.pproducer = (GS_ECB  *) 0;
            pevent->ECB_RRDS->queue_buffer.rrds_consumer.MQ_msg_seq = pevent1->ECB_RRDS->queue_buffer.rrds_producer.MQ_msg_seq;
        }
        
        pevent1 = prcb->RCB_NextRCBWEL;
        while (pevent1 != (GS_ECB *) 0) {
            PRINT_ASSERT((gkm_ECB_IsValid(pevent1) == G_TRUE),"ERROR ECB is not valid\n");
            // Check if there exist a new message not consumed
            if (pevent1->ECB_RRDS->queue_buffer.rrds_producer.MQ_msg_seq > pevent->ECB_RRDS->queue_buffer.rrds_consumer.MQ_msg_seq) {
                // New message to consume
                pevent->ECB_RRDS->queue_buffer.rrds_consumer.pproducer = pevent1;
                
                pevent->ECBType = G_ECBType_QUEUE_CONSUME;
                
                PRINT_ASSERT((buffer_length > pevent1->ECB_RRDS->queue_buffer.rrds_producer.msg_length),"ERROR No enough buffer length\n");
                PRINT_ASSERT((gkm_RRDS_IsValid(pevent1->ECB_RRDS) == G_TRUE),"ERROR RRDS is not valid\n");
                
                // PRINT_ASSERT((pevent1->ECB_RRDS->queue_buffer.rrds_producer.msg_length < pevent->ECB_RRDS->queue_buffer.rrds_consumer.buffer_length),"ERROR string too long\n");
                
				PRINT_ASSERT(((unsigned int) pevent->ECB_RRDS->queue_buffer.rrds_consumer.buffer_msg % 4 == 0), "ERROR memory is not alligned");
				PRINT_ASSERT(((unsigned int) pevent1->ECB_RRDS->queue_buffer.rrds_producer.pmsg % 4 == 0), "ERROR memory is not alligned");
				
                // 09-07-2024 memcpy(pevent->ECB_RRDS->queue_buffer.rrds_consumer.buffer_msg, pevent1->ECB_RRDS->queue_buffer.rrds_producer.pmsg, pevent1->ECB_RRDS->queue_buffer.rrds_producer.msg_length);

				// Typecast src and dest addresses to (char *) 
				char *csrc = (char *)pevent1->ECB_RRDS->queue_buffer.rrds_producer.pmsg; 
				char *cdest = (char *)pevent->ECB_RRDS->queue_buffer.rrds_consumer.buffer_msg; 

				// Copy contents of src[] to dest[] 
				for (int i = 0; i < pevent1->ECB_RRDS->queue_buffer.rrds_producer.msg_length; i++) {
					cdest[i] = csrc[i]; 
				} 


                pevent->ECB_RRDS->queue_buffer.rrds_consumer.msg_length = pevent1->ECB_RRDS->queue_buffer.rrds_producer.msg_length;
                break;
            }
            pevent1 = pevent1->ECB_NextECB;
        }

        if (pevent1 == (GS_ECB *) 0) {
            // No new message, wait for next message
            pevent->ECBType = G_ECBType_MESSAGE_CONSUME_WAIT;
            pevent->ECB_RRDS->queue_buffer.rrds_consumer.msg_length = 0;
            
            gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);
            // #################################################


            PRINT_ASSERT((pevent->ECB_RRDS->queue_buffer.rrds_consumer.msg_length != 0),"ERROR ECB is not valid=%d\n", (int) pevent->ECB_RRDS->queue_buffer.rrds_consumer.msg_length);
            PRINT_ASSERT((pevent->ECBType == G_ECBType_QUEUE_CONSUME),"ERROR ECB is not valid=%d\n", (int) pevent->ECBType);
        }


    length = (int) pevent->ECB_RRDS->queue_buffer.rrds_consumer.msg_length;
    PRINT_ASSERT((length > 0),"ERROR ECB is not valid=%d\n", (int) length);
    PRINT_ASSERT((length < buffer_length),"ERROR ECB is not valid=%d, buffer_length=%d\n", (int) length, (int) buffer_length);
    
	GK_SAMPLE_FUNCTION_END(10047)
    gm_GeMRTOSCriticalSectionExit;

    return (length);
}

/** gu_MessageQueueSend
 *  \brief The gu_MessageQueueSend function transmits a message to a message queue. The sending task blocks until the message has been successfully delivered to all subscribed consumers or until a timeout occurs.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] prcb A pointer to the G_RCB structure representing the message queue resource. This pointer was returned by gu_MessageQueueCreate when the queue was created.
 *  \param [in] pmsg A pointer to the message data to be sent
 *  \param [in] msg_length An integer representing the length of the message to be sent, in bytes.
 *  \param [in] timeout A gt_time value specifying the timeout period for sending the message.
 *  \return The gu_MessageQueueSend function returns G_TRUE if the message was successfully sent within the timeout period, and G_FALSE otherwise.  G_FALSE indicates either a timeout or another error condition.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueReceive, gu_MessageQueueSubscribe, gu_MessageQueuePrintf
 *  \relates Message_Queue
 *  \remark gu_MessageQueueSend
 *  \todo implement error return conditions.
 */
int gu_MessageQueueSend(G_RCB *prcb, char *pmsg, int msg_length, gt_time timeout) {
    
	GS_ECB  *pevent  = (GS_ECB  *) 0; 
    GS_ECB  *pevent1 = (GS_ECB  *) 0;
    GS_ECB  *peventime = (GS_ECB  *) 0; 
    int     delivered = 0;
    
    gm_GeMRTOSCriticalSectionEnter;
		GK_SAMPLE_FUNCTION_BEGIN(10048)
    
        PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");

        if (get_pointed_field(prcb,RCB_NextRCBGEL) == (struct gs_ecb *) 0) { // No consumer in queue, return 0 messages deliverred
			GK_SAMPLE_FUNCTION_END(10048)
			// GU_FPRINTF(stderr, "Leaving gu_MessageQueueSend without printing");
            gm_GeMRTOSCriticalSectionExit;
            return (delivered);
        }
		
        // Event for producer list in Resource Waiting list
        GS_TCB *ptcb = gk_PCB_GetCurrentTCB();
        PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
        
        pevent    = gk_ECB_GetFree();
        PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid\n");
        gk_TCBAEL_Link(pevent, ptcb);
        
        set_pointed_field(pevent, ECBType, G_ECBType_MESSAGE_PRODUCE_WAIT);
        pevent->ECBValue.i64 = (G_INT64) prcb->queue.MQ_priority_send;
        prcb->queue.MQ_priority_send++;
        gk_RCBWEL_Link(prcb, ptcb, pevent);
        
        pevent->ECB_RRDS = gk_RRDS_GetFree(); // Link the RRDS structure to producer
        PRINT_ASSERT((gkm_RRDS_IsValid(pevent->ECB_RRDS) == G_TRUE),"ERROR RRDS is not valid\n");
        pevent->ECB_RRDS->RRDS_AsocECB = pevent;
        
        pevent->ECB_RRDS->queue_buffer.rrds_producer.deliveries = 0;
        pevent->ECB_RRDS->queue_buffer.rrds_producer.consumers = 0;
        
        pevent->ECB_RRDS->queue_buffer.rrds_producer.pmsg       = pmsg;
        pevent->ECB_RRDS->queue_buffer.rrds_producer.msg_length = (G_INT32) msg_length;
        
        prcb->queue.MQ_msg_seq++;
        pevent->ECB_RRDS->queue_buffer.rrds_producer.MQ_msg_seq = prcb->queue.MQ_msg_seq;
        
        // Event for producer timeout in Resource Waiting list
        if (timeout != (G_INT64) 0) {
            peventime    = gk_ECB_GetFree();
			PRINT_ASSERT((gkm_ECB_IsValid(peventime) == G_TRUE),"ERROR ECB is not valid\n");
            gk_TCBAEL_Link(peventime, ptcb);            
            set_pointed_field(peventime, ECBType, G_ECBType_MESSAGE_SEND_WAIT);
            peventime->ECBValue.i64 = (G_INT64) timeout;
            gk_ECBTL_Link (peventime);
        }   

        // activate all waiting consumers 
        pevent1 = prcb->RCB_NextRCBGEL;       
        while (pevent1 != (GS_ECB *) 0) {
            PRINT_ASSERT((gkm_ECB_IsValid(pevent1) == G_TRUE),"ERROR ECB is not valid=%p\n", (void *) pevent1);
            if (get_pointed_field(pevent1, ECBType) == G_ECBType_MESSAGE_CONSUME_WAIT) {
                set_pointed_field(pevent1, ECBType, G_ECBType_QUEUE_CONSUME);
                pevent1->ECB_RRDS->queue_buffer.rrds_consumer.pproducer = pevent;
                
                // 15-07-2024 PRINT_ASSERT((pevent->ECB_RRDS->queue_buffer.rrds_producer.msg_length < pevent1->ECB_RRDS->queue_buffer.rrds_consumer.buffer_length),"ERROR string too long\n");
                
				PRINT_ASSERT(((unsigned int) pevent1->ECB_RRDS->queue_buffer.rrds_consumer.buffer_msg % 4 == 0), "ERROR memory is not alligned");
				PRINT_ASSERT(((unsigned int) pevent->ECB_RRDS->queue_buffer.rrds_producer.pmsg % 4 == 0), "ERROR memory is not alligned");
				
                // 08-07-2024 memcpy(pevent1->ECB_RRDS->queue_buffer.rrds_consumer.buffer_msg, pevent->ECB_RRDS->queue_buffer.rrds_producer.pmsg, pevent->ECB_RRDS->queue_buffer.rrds_producer.msg_length);
				
				// Typecast src and dest addresses to (char *) 
				char *csrc = (char *)pevent->ECB_RRDS->queue_buffer.rrds_producer.pmsg; 
				char *cdest = (char *)pevent1->ECB_RRDS->queue_buffer.rrds_consumer.buffer_msg; 

				// Copy contents of src[] to dest[] 
				for (int i = 0; i < pevent->ECB_RRDS->queue_buffer.rrds_producer.msg_length; i++) {
					cdest[i] = csrc[i]; 
				} 
				
				
                pevent1->ECB_RRDS->queue_buffer.rrds_consumer.msg_length = pevent->ECB_RRDS->queue_buffer.rrds_producer.msg_length;
                
                PRINT_ASSERT((gkm_TCB_IsValid(pevent1->ECB_AssocTCB) == G_TRUE),"ERROR TCB is not valid\n");
                PRINT_ASSERT(((pevent1->ECB_AssocTCB->TCBState != G_TCBState_READY) && (pevent1->ECB_AssocTCB->TCBState != G_TCBState_RUNNING)),"ERROR TCBState is not valid= %d\n", pevent1->ECB_AssocTCB->TCBState);
                gk_TCB_Unlink(pevent1->ECB_AssocTCB);
                gk_TCBRDYL_Link(pevent1->ECB_AssocTCB);
            }
            pevent->ECB_RRDS->queue_buffer.rrds_producer.consumers++;
            pevent1 = pevent1->ECB_NextECB;
        }

        // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, consumers=%d\n",__FUNCTION__,__LINE__,gm_ProcessorId,(int) pevent->ECB_RRDS->queue_buffer.rrds_producer.consumers);

        gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);    //Switch tasks and wait to deliver or timeout
        // #################################################
      
        if (pevent->ECBType == G_ECBType_MESSAGE_PRODUCER_DELIVERED) 
        {
            delivered = (int) pevent->ECB_RRDS->queue_buffer.rrds_producer.deliveries;
        }
        else 
        {
            delivered = (-1 * (int) pevent->ECB_RRDS->queue_buffer.rrds_producer.deliveries);
        }
        
        if (timeout != (G_INT64) 0) {
            gk_TCBAEL_Unlink(peventime);
            gk_ECBFL_Link(peventime); 
        }
        
        gk_ECB_List_Unlink(pevent);
        gk_ECBFL_Link(pevent); 

		GK_SAMPLE_FUNCTION_END(10048)
    gm_GeMRTOSCriticalSectionExit;
    return (delivered);

}

/**gu_MessageQueuePrintf
 *  \brief The gu_MessageQueuePrintf function sends a formatted message to a message queue.  The calling task will block until the message is successfully delivered to all consumers subscribed to the queue.
 *  \exception par_following The function accepts the following parameters:
 *  \param [in] prcb  A pointer to the G_RCB structure representing the message queue. This pointer is the value returned by gu_MessageQueueCreate when the queue was created.
 *  \param format A null-terminated string containing the format string, similar to the standard printf function. This string can include format specifiers (e.g., %d, %s, %x) that are replaced by subsequent arguments.
 *  \return The function returns G_TRUE if the message was successfully sent to the queue, and G_FALSE otherwise.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSend, gu_MessageQueueReceive, gu_MessageQueueSubscribe
 *  \relates Message_Queue
 *  \remark gu_MessageQueuePrintf
 *  \todo insert timeout
 */
int gu_MessageQueuePrintf(G_RCB *prcb, char *format, ...)
{

    // 15-10-2024 gm_GeMRTOSCriticalSectionEnter;
    
    // from https://stackoverflow.com/questions/12746885/why-use-asprintf-instead-of-sprintf    
    const unsigned int BUF_LEN = 126;
    char x[BUF_LEN + 2];
	x[BUF_LEN + 1] = 0;
    
    // from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
    va_list args;        
    va_start (args, format);
    vsnprintf(x, BUF_LEN, format, args);
    va_end (args);
	

	// 15-10-2024 gm_GeMRTOSCriticalSectionExit;
	
#ifdef __niosX_arch__
	gu_MessageQueueSend(prcb, x, (G_INT32) (strlen(x) + 1), (gt_time) 0);
#else
    printf("%s", x);
#endif

	return (G_TRUE);

}

/**
 *  \brief The gu_MessageQueueCreate function creates a new message queue resource.  This resource is implemented using a G_RCB structure, extended with fields from a T_QUEUE_RESOURCE structure. The created queue includes event lists for producers (waiting to send) and consumers (waiting to receive) messages. Producer tasks add themselves to the producer event list when they are waiting to send a message.  This function can be called from either the main application code or from within a task.  If called within a task, it must be called before any message send or receive operations; otherwise, an error will occur.
 *  \exception par_following The gu_MessageQueueCreate function takes no parameters.
 *  \return The gu_MessageQueueCreate function returns a pointer (G_RCB *) to the newly created message queue resource. This pointer is essential for all subsequent operations on the queue. A NULL pointer is returned if there is insufficient memory to create the queue or if no more queue resources are available.
 *  \sa gu_MessageQueueSend, gu_MessageQueueReceive, gu_MessageQueueSubscribe, gu_MessageQueuePrintf
 *  \relates Message_Queue
 *  \remark gu_MessageQueueCreate
 */
G_RCB  *gu_MessageQueueCreate(void)
{
    G_RCB *prcb;
    gm_GeMRTOSCriticalSectionEnter;
		GK_SAMPLE_FUNCTION_BEGIN(10050)
    
	prcb = gk_RCB_GetFree();
	if (prcb != (G_RCB *) 0) {
		// prcb->RCBState = GK_RCBState_QUEUE;
		prcb->RCBType = (G_INT32) GK_RCBType_QUEUE;
		// 18-10-2024 prcb->RCBPriority.i64 = (G_INT64) G_LOWEST_PRIORITY - (G_INT64) 1;
		// 18-10-2024 prcb->RCBGrantedPriority.i64 = (G_INT64) G_LOWEST_PRIORITY - (G_INT64) 1;
		// 18-10-2024 prcb->RCBWaitingTimeout.i64 = (G_INT64) 0;
		// 18-10-2024 prcb->RCBGrantedTimeout.i64 = (G_INT64) 0;
		// prcb->RCB_NextRCBWEL = (struct gs_ecb *) 0;
		// prcb->RCB_NextRCBGEL = (struct gs_ecb *) 0;
		// prcb->RCB_NextRCBASL = (struct gs_scb *) 0;

		prcb->queue.MQ_msg_seq = (G_INT32) 0;
		prcb->queue.MQ_priority_send = (G_INT32) 0;

	}
		GK_SAMPLE_FUNCTION_END(10050)
    gm_GeMRTOSCriticalSectionExit;
	return((G_RCB  *) prcb);
}


/** gu_MessageQueueSubscribe
 *  \brief The gu_MessageQueueSubscribe function subscribes the task to a message queue resource. This subscription is crucial for message delivery;  a consumer task must be subscribed to a queue before it can receive messages using gu_MessageQueueReceive.  Furthermore, the execution of this function for each receiving task allows the message queue to track the number of consumers subscribed. This count is essential for producers; a producer message is only considered fully delivered when it has been received by every subscribed consumer. The message queue must have been previously created using gu_MessageQueueCreate.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] ptcb A pointer to the Task Control Block (GS_TCB) of the task being subscribed to the queue.
 *  \param [in] presource A pointer to the G_RCB structure representing the message queue resource to which the task is subscribing.
 *  \return The gu_MessageQueueSubscribe function returns a pointer to the GS_ECB structure associated with the message queue resource.  A NULL return value likely indicates an error.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSend, gu_MessageQueueReceive
 *  \relates Message_Queue
 *  \remark gu_MessageQueueSubscribe
 */
GS_ECB *gu_MessageQueueSubscribe(GS_TCB  *ptcb, G_RCB  *presource)
{
	GS_ECB  *pevent = (GS_ECB  *) 0;
    
    gm_GeMRTOSCriticalSectionEnter;
	GK_SAMPLE_FUNCTION_BEGIN(10051)

	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");

	pevent = gk_RCBGEL_Link(presource, ptcb, (GS_ECB *) 0); // Create and link an ECB to RCBGEL
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid\n");

	gk_TCBAEL_Link(pevent,ptcb);
	pevent->ECBType  = G_ECBType_QUEUE_CONSUME;

	pevent->ECB_RRDS = gk_RRDS_GetFree();
	PRINT_ASSERT((gkm_RRDS_IsValid(pevent->ECB_RRDS) == G_TRUE),"ERROR RRDS is not valid\n");
	pevent->ECB_RRDS->RRDS_AsocECB = pevent;

	pevent->ECB_RRDS->queue_buffer.rrds_consumer.MQ_msg_seq = (G_INT32) 0; 
	pevent->ECB_RRDS->queue_buffer.rrds_consumer.pproducer = (GS_ECB  *) 0;
	pevent->ECB_RRDS->queue_buffer.rrds_consumer.msg_length = 0;

	GK_SAMPLE_FUNCTION_END(10051)
    gm_GeMRTOSCriticalSectionExit;
    return (pevent);
}


/**gk_timeout_ECB_MQ_send
 *  \brief This function is called from the timed IRQ when a send timeout G_ECBType_MESSAGE_SEND_WAIT happens
 *  \param [in] peventime Pointer to the timed ECB happened
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Resource
 */
G_INT32 gk_timeout_ECB_MQ_send(GS_ECB *peventime) {
    GS_TCB  *ptcb;
	GK_SAMPLE_FUNCTION_BEGIN(10052)
    
	peventime->ECBType = G_ECBType_MESSAGE_SEND_EXPIRED;    
    ptcb = peventime->ECB_AssocTCB;
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
    gk_TCB_Unlink(ptcb);
    gk_TCBRDYL_Link(ptcb);
	
	GK_SAMPLE_FUNCTION_END(10052)
    return (G_TRUE);
}

/**gk_timeout_ECB_MQ_consume
 *  \brief This function is called from the timed IRQ when a consume timeout G_ECBType_MESSAGE_CONSUME_WAIT happens
 *  \param [in] peventime Description for peventime
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Resource
 */
G_INT32 gk_timeout_ECB_MQ_consume(GS_ECB *peventime) {
    GS_TCB  *ptcb;
	GK_SAMPLE_FUNCTION_BEGIN(10053)
	
    peventime->ECBType = G_ECBType_MESSAGE_CONSUME_EXPIRED;    
    ptcb = peventime->ECB_AssocTCB;
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
    gk_TCB_Unlink(ptcb);
    gk_TCBRDYL_Link(ptcb);
	
	GK_SAMPLE_FUNCTION_END(10053)
    return (G_TRUE);
}

G_INT32 gk_queue_kill_producer(GS_ECB *pevent)
{
	GK_SAMPLE_FUNCTION_BEGIN(10054)
    // Producer is kill, then consumer with this producer should be canceled
    G_RCB *prcb = (G_RCB *) pevent->ECB_AssocRCB;
    GS_ECB *pevent1;
    PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");
    
    pevent1 = prcb->RCB_NextRCBGEL;
    PRINT_ASSERT((gkm_ECB_IsValid(pevent1) == G_TRUE),"ERROR ECB is not valid\n");
    // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, pevent=%d\n", __FUNCTION__, __LINE__, gm_ProcessorId, (int) pevent1);
    while (pevent1 != (GS_ECB *) 0) {
        PRINT_ASSERT((gkm_ECB_IsValid(pevent1) == G_TRUE),"ERROR ECB is not valid\n");
        if (pevent1->ECB_RRDS->queue_buffer.rrds_consumer.pproducer == pevent) 
        {
            pevent1->ECB_RRDS->queue_buffer.rrds_consumer.pproducer = (GS_ECB *) 0;
            pevent->ECB_RRDS->queue_buffer.rrds_consumer.MQ_msg_seq = pevent1->ECB_RRDS->queue_buffer.rrds_producer.MQ_msg_seq;
        }
        pevent1 = pevent1->ECB_NextECB;
    }
	
	GK_SAMPLE_FUNCTION_END(10054)
    return (G_TRUE);
}

// G_INT32 gk_QUEUE_granted_kill(GS_ECB *pevent) {
//     gk_ECB_List_Unlink((GS_ECB *) pevent);
//     gk_TCBAEL_Unlink(pevent);
//     gk_ECBFL_Link((GS_ECB *)pevent);
//     return G_TRUE;
// 
// }

OPTIMEZE_RESTORE
