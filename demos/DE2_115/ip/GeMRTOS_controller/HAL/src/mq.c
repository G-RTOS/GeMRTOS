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

/** \class MessageQueue
The <b>message queue</b> is a resource that enables independent tasks to exchange information. Information is sent using messages by a task (denoted <em>producer</em>) to the <em>message queue</em>. A message remains associated with the message queue resources until (1) all the tasks associated with the message queue read it (aka <em>consume</em> it), or (2) a timeout expires. 
The producer task remain suspended until the message was consumed by all the consumers or the timeout of the message expires. The tasks that want to receive the messages sent to the message queue, have to subscribe to the message queue.
\image html "https://gemrtos.com/images/message_queue.gif"
If a consumer task already received all the messages sent, then it remains suspended waiting until the next message.
 */
 


/**
 *  \brief The <b>gu_queue_receive</b> function receives the next message from the message queue resource. The task should be subscribed previously with the <em>gu_queue_subscribe</em> function. The new message is stored into the buffer pointed by the <em>buffer_msg</em> parameter. Up to <em>buffer_length</em> bytes are copied from the message sent. When the sent message is larger than the consumer buffer, then the message recieved is truncated in the <em>buffer_length</em> size.
 *  \param prcb It is the pointer to the message queue resource already subscribed to.
 *  \param buffer_msg It is the pointer to the message buffer where the message received will be store into.
 *  \param buffer_length The maximum number of bytes to be received. It should be the length of the receiving buffer.
 *  \return The <b>gu_queue_receive</b> function returns the number of bytes received.
 *  \relates MessageQueue
 *  \remark gu_queue_receive
 */
int gu_queue_receive(G_RCB *prcb, void * buffer_msg, G_INT32 buffer_length) {
    
    // G_RCB   *prcb;
    GS_TCB *ptcb;
    GS_ECB *pevent1;
    GS_ECB *pevent;
    int length = 0;
    
    PRINT_ASSERT((G_Running == G_TRUE),"Function have to be called from task\n");
    
    GRTOS_USER_CRITICAL_SECTION_GET;
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
    GRTOS_USER_CRITICAL_SECTION_RELEASE;

    return (length);
}

/**gu_queue_send
 *  \brief The <b>gu_queue_send</b> function sends a message to a message queue resource. The function returns when the message was delivered to all the consumer subscribed or the timeout, meanwhile the task remains suspended.
 *  \param [in] prcb  It is the pointer to the message queue resource. The pointer is the value returned by the <em>gu_queue_create</em> function when the message queue resource was created.
 *  \param pmsg   It is the pointer to the message to be sent.
 *  \param msg_length It is the lenght of the message to be sent.
 *  \param timeout   It is the timeout to send the message.
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates MessageQueue
 *  \remark gu_queue_send
 */
int gu_queue_send(G_RCB *prcb, char *pmsg, int msg_length, gt_time timeout) {
    
	GS_ECB  *pevent  = (GS_ECB  *) 0; 
    GS_ECB  *pevent1 = (GS_ECB  *) 0;
    GS_ECB  *peventime = (GS_ECB  *) 0; 
    int     delivered = 0;
    
    GRTOS_USER_CRITICAL_SECTION_GET;
		GK_SAMPLE_FUNCTION_BEGIN(10048)
    
        PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");

        if (get_pointed_field(prcb,RCB_NextRCBGEL) == (struct gs_ecb *) 0) { // No consumer in queue, return 0 messages deliverred
			GK_SAMPLE_FUNCTION_END(10048)
			// GU_FPRINTF(stderr, "Leaving gu_queue_send without printing");
            GRTOS_USER_CRITICAL_SECTION_RELEASE;
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

        // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, consumers=%d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID,(int) pevent->ECB_RRDS->queue_buffer.rrds_producer.consumers);

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
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (delivered);

}

/**gu_queue_printf
 *  \brief The <b>gu_queue_printf</b> function serves to print a message to a message queue resource. This function operates in a manner where it will return once the message has been successfully delivered to all subscribed consumers or when a timeout occurs. During this period, the task invoking the <b>gu_queue_printf</b> function remains suspended. The primary purpose of this function is to transmit a formatted message to the designated queue resource, facilitating communication within a GeMRTOS-based system.
 *  \param [in] prcb  It is the pointer to the message queue resource. The pointer is the value returned by the <em>gu_queue_create</em> function when the message queue resource was created. It serves as a reference to the specific message queue where the formatted message will be sent using <b>gu_queue_printf</b>.
 *  \param format   It is the formatted message to be sent. The valid format of this message is similar to the original printf function, allowing it to contain embedded format tags. These format tags are replaced by the values specified in subsequent additional arguments and formatted as specified. This allows for flexible and customized message formatting within the <b>gu_queue_printf</b> function, akin to how the printf function operates.
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates MessageQueue
 *  \remark gu_queue_printf
 */
int gu_queue_printf(G_RCB *prcb, char *format, ...)
{

    GRTOS_USER_CRITICAL_SECTION_GET;
	// 29-02-2024 GK_SAMPLE_FUNCTION_BEGIN(10049)
    
	// 05-06-2024 PROC_INTERRUPT_DISABLE
    // from https://stackoverflow.com/questions/12746885/why-use-asprintf-instead-of-sprintf    
    const unsigned int BUF_LEN = 126;
    char x[BUF_LEN + 2];
	x[BUF_LEN + 1] = 0;
    
	// 01-03-2024 GRTOS_USER_CRITICAL_SECTION_GET;
    // from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
    va_list args;        
    va_start (args, format);
    vsnprintf(x, BUF_LEN, format, args);
    va_end (args);
	
	// 05-06-2024 PROC_INTERRUPT_ENABLE
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
	
#ifdef __niosX_arch__
	gu_queue_send(prcb, x, (G_INT32) (strlen(x) + 1), (gt_time) 0);
#else
    printf("%s", x);
#endif

    // 29-02-2024 GK_SAMPLE_FUNCTION_END(10049)
    // 29-02-2024 GRTOS_USER_CRITICAL_SECTION_RELEASE;
	return (G_TRUE);

}

/**
 *  \brief The <b>gu_queue_create</b> function creates a new message queue resource. The message queue resource is built using the G_RCB structure with some fields from the T_QUEUE_RESOURCE structure appended. A producer (associated with the waiting eevnt list) and a consumer (associated with the granted event list) event lists will be included in the newly constructed message queue resource. Producer tasks link a waiting event in the producer list of the message queue resource. 
 *  The <b>gu_queue_create</b> can be called from either the main code or a task code. However, when it is executed within a task code, it must come before the execution of a send or a receive message function, or an error will be produced.
 *  \return The <b>gu_queue_create</b> function returns a pointer to the message queue resource created, or 0 if there is not more resources available. It should be used in all the functions related to the message queue resource created. The function returns a NULL pointer if there is not enough memory to implement the message queue resource.
 *  \relates MessageQueue
 *  \remark gu_queue_create
 */
G_RCB  *gu_queue_create(void)
{
    G_RCB *prcb;
    GRTOS_USER_CRITICAL_SECTION_GET;
		GK_SAMPLE_FUNCTION_BEGIN(10050)
    
	prcb = gk_RCB_GetFree();
	if (prcb != (G_RCB *) 0) {
		prcb->RCBState = GK_RCBState_QUEUE;
		prcb->RCBType = (G_INT32) GK_RCBType_QUEUE;
		prcb->RCBPriority.i64 = (G_INT64) G_LOWEST_PRIORITY - (G_INT64) 1;
		prcb->RCBGrantedPriority.i64 = (G_INT64) G_LOWEST_PRIORITY - (G_INT64) 1;
		prcb->RCBWaitingTimeout.i64 = (G_INT64) 0;
		prcb->RCBGrantedTimeout.i64 = (G_INT64) 0;
		// prcb->RCB_NextRCBWEL = (struct gs_ecb *) 0;
		// prcb->RCB_NextRCBGEL = (struct gs_ecb *) 0;
		// prcb->RCB_NextRCBASL = (struct gs_scb *) 0;

		prcb->queue.MQ_msg_seq = (G_INT32) 0;
		prcb->queue.MQ_priority_send = (G_INT32) 0;

	}
		GK_SAMPLE_FUNCTION_END(10050)
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
	return((G_RCB  *) prcb);
}


/**
 *  \brief The <b>gu_queue_subscribe</b> function subscribes the current executing task to the message queue resource specified. A consumer tasks should be previously subscribe to the message queue resource in order to be able to receive messages with the <em>gu_queue_receive</em> function. The Message Queue should be created with gu_queue_create.
 *  \param ptcb It is a pointer to the task's TCB to be subscribe to the queue
 *  \param presource It is a pointer to the message queue resource to be subscribe to.
 *  \return The <b>gu_queue_subscribe</b> function returns a pointer to the GS_ECB structure associated with the message queue resource.
 *  \relates MessageQueue
 *  \remark gu_queue_subscribe
 */
GS_ECB *gu_queue_subscribe(GS_TCB  *ptcb, G_RCB  *presource)
{
	GS_ECB  *pevent = (GS_ECB  *) 0;
    
    GRTOS_USER_CRITICAL_SECTION_GET;
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
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
    // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, pevent=%d\n", __FUNCTION__, __LINE__, GRTOS_CMD_PRC_ID, (int) pevent1);
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
