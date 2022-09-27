/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Message queue functions
 *  \details This file contains the Message queue functions.
 *  \version 0.9a
 *  \date      2015-2020
 *  \bug  None known
 *  \copyright Ricardo Cayssials  (rcayssials@gmail.com)
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


// #include <stdio.h>
#include <grtosdefinitions.h>
#include <string.h>

OPTIMEZE_CODE(3)

/**gu_MQ_receive
 *  \brief Receive the next message from the Message Queue
 *  \param [in] pevent Pointer to the ECB associated to the Message Queue
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \details The ECB should be associated previously with the gu_queue_consume function
 *  \relates Resource
 */
INT32 gu_MQ_receive(GS_ECB  *pevent) {
    
	GS_RRDS *prrds_buffer = (GS_RRDS *) 0;
    GS_ECB  *peventime = (GS_ECB  *) 0; 
    G_RCB   *prcb;
    
    GRTOS_USER_CRITICAL_SECTION_GET;
        prcb = (G_RCB   *) pevent->ECB_AssocRCB;
        prrds_buffer = pevent->ECB_RRDS;

        if (prrds_buffer->queue_buffer.BUFFER_status == BUFFER_status_READ) {
            if (prcb->queue.MQ_msg_seq == prrds_buffer->queue_buffer.BUFFER_msg_seq) { // message is in buffer
                gk_check_consume(pevent);
                GRTOS_CMD_CRITICAL_SECTION_RELEASE;
                return G_TRUE;
            }

            peventime = gk_ECB_GetFree();
            if (peventime == (GS_ECB *) 0)  {
                GRTOS_CMD_CRITICAL_SECTION_RELEASE;
                return G_FALSE;
            }
            peventime->ECBValue.i64 = (INT64) prrds_buffer->queue_buffer.BUFFER_priority.i64;
            peventime->ECBType = G_ECBType_MESSAGE_CONSUME_WAIT;
            gk_ECBTL_Link (peventime);

            gk_KERNEL_TASK_SUSPEND_CURRENT();    //Switch tasks and wait to send or timeout
            // #################################################
            GRTOS_USER_CRITICAL_SECTION_GET;     // return from timeout of with access to resource

            if (peventime->ECBType == G_ECBType_MESSAGE_CONSUME_EXPIRED) {
                gk_ECBFL_Link(peventime);
                GRTOS_CMD_CRITICAL_SECTION_RELEASE;
                return G_FALSE;
            }
            gk_ECBFL_Link(peventime);            
            
        } else {
            GRTOS_CMD_CRITICAL_SECTION_RELEASE;
            return G_FALSE;
        }

    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**gk_MQ_send
 *  \brief Sends a message to all the consumers. The function returns when the message was delivered to all
 *  the consumer or the timeout expires
 *  \param [in] prcb       Pointer to the RCB of the Message Queue
 *  \param [in] pmsg       Pointer to the message to send
 *  \param [in] msg_length Lenght of the message to send
 *  \param [in] timeout    Timeout to send the message
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Resource
 */
INT32 gk_MQ_send(G_RCB  *prcb, void *pmsg, INT32 msg_length, gt_time timeout) {
    
	// GS_RRDS *prrds_buffer = (GS_RRDS *) 0;
	GS_ECB  *pevent = (GS_ECB  *) 0; 
    GS_ECB  *peventime = (GS_ECB  *) 0; 
    INT32   granted = G_FALSE;
    // INT32   retorno = G_FALSE;
    
    
    GRTOS_USER_CRITICAL_SECTION_GET;
        
        
        if (msg_length > prcb->queue.MQ_Buffer_Length) {
            GRTOS_CMD_CRITICAL_SECTION_RELEASE;
            return G_FALSE; // Message too large
        }
        
        if (prcb->RCB_NextRCBWEL != (struct gs_ecb *) 0) { // It has to wait to send the message
            pevent    = gk_ECB_GetFree();
            if (pevent == (GS_ECB *) 0)  {
                GRTOS_CMD_CRITICAL_SECTION_RELEASE;
                return G_FALSE;
            }
            peventime    = gk_ECB_GetFree();
            if (peventime == (GS_ECB *) 0) {
                gk_ECBFL_Link(pevent);
                GRTOS_CMD_CRITICAL_SECTION_RELEASE;
                return G_FALSE;
            }
            pevent->ECBValue.i64 = (INT64) prcb->queue.MQ_priority_send;
            prcb->queue.MQ_priority_send++;
            gk_RCBWEL_Link(prcb, pevent);
            
            peventime->ECBValue.i64 = (INT64) timeout;
            peventime->ECBType = G_ECBType_MESSAGE_SEND_WAIT;
            gk_ECBTL_Link (peventime);
            
            gk_KERNEL_TASK_SUSPEND_CURRENT();    //Switch tasks and wait to send or timeout
            // #################################################
            GRTOS_USER_CRITICAL_SECTION_GET;     // return from timeout of with access to resource
            gk_ECBFL_Link(peventime);
            if (prcb->RCB_NextRCBWEL == pevent) {
                granted = G_TRUE;
            }
            
            gk_RCBWEL_Unlink(pevent);
            gk_ECBFL_Link(pevent);
            
            if (granted == G_FALSE) {
                GRTOS_CMD_CRITICAL_SECTION_RELEASE;
                return G_FALSE;
            }
        }
        
        
        if (prcb->RCB_NextRCBWEL == (struct gs_ecb *) 0 
            || granted == G_TRUE) { // It is ready to send the message
            
            prcb->queue.MQ_Count_deliver = (INT32) 0;
            prcb->queue.MQ_msg_Length = msg_length;
            prcb->queue.MQ_msg_seq++;
            memcpy(prcb->queue.MQ_PBuffer, pmsg, msg_length); 
            
            prcb->queue.MQ_CUR_TCB = (struct gs_tcb *) gk_PCB_GetCurrentTCB();
            
            pevent = prcb->RCB_NextRCBGEL;
            while (pevent != (GS_ECB *) 0) {
                gk_check_consume((GS_ECB *) pevent);
                pevent = pevent->ECB_NextECB;
            }

        } 
    
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**
 *  \brief Create a Resource for Queue with default settings. 
 *  
 *  \param [in] pbuffer Pointer to the buffer where data sent to the queue will be transfered.
 *  \param [in] buffer_length Length of the buffer of the queue
 *  \return Return a pointer to the resource of the Queue, or 0 if there is not more resources availables
 *  
 *  \details A memory buffer should be summited in order to store the data sent to the queue.
 */
G_RCB  *gu_queue_create(void *pbuffer, INT32 buffer_length)
{
    G_RCB *prcb;
    GRTOS_USER_CRITICAL_SECTION_GET;
        prcb = gk_RCB_GetFree();
        if (prcb != (G_RCB *) 0) {
            prcb->RCBState = GK_RCBState_QUEUE;
            prcb->RCBType = (INT32) GK_RCBType_QUEUE;
	        prcb->RCBPriority.i64 = (INT64) G_LOWEST_PRIORITY - (INT64) 100;
	        prcb->RCBGrantedPriority.i64 = (INT64) G_LOWEST_PRIORITY - (INT64) 100;
	        prcb->RCBWaitingTimeout.i64 = (INT64) G_LATEST_TIME - (INT64) 100;
	        prcb->RCBGrantedTimeout.i64 = (INT64) G_LATEST_TIME - (INT64) 100;
            prcb->RCB_NextRCBWEL = (struct gs_ecb *) 0;
            prcb->RCB_NextRCBGEL = (struct gs_ecb *) 0;
            prcb->RCB_NextRCB = (struct g_rcb *) 0;
            prcb->RCB_NextRCBASL = (struct gs_scb *) 0;

            prcb->queue.MQ_PBuffer = (void *) pbuffer;
            prcb->queue.MQ_Buffer_Length = buffer_length;
            prcb->queue.MQ_Count_deliver = (INT32) 0;        
            prcb->queue.MQ_consumer_count = (INT32) 0;       
            prcb->queue.MQ_priority_send = (INT32) 0;        
            prcb->queue.MQ_priority_consume = (INT32) 0; 
            prcb->queue.MQ_msg_Length = (INT32) 0;
            prcb->queue.MQ_msg_seq = (INT32) 0;
            prcb->queue.MQ_CUR_TCB = (struct gs_tcb *) 0;

        }
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
	return((G_RCB  *) prcb);
}

/**gu_queue_consume
 *  \brief Creates an ECB to consume from a Message Queue
 *  
 *  \param [in] presource     Pointer of the G_RCB of the Message Queue
 *  \param [in] pbuffer       Pointer to the buffer to store the messages consumed
 *  \param [in] buffer_length Length of the buffer
 *  \param [in] timeout       Timeout for consuming
 *  \return Pointer to the ECB associated to the Message Queue for consuming
 *  
 *  \details The Message Queue should be created with gu_queue_create and the reading with gu_MQ_receive
 *  \relates Resource
 */
GS_ECB  *gu_queue_consume(G_RCB  *presource, void *pbuffer, INT32 buffer_length, gt_time timeout)
{
	GS_RRDS *prrds_buffer = (GS_RRDS *) 0;
	GS_ECB  *pevent = (GS_ECB  *) 0;
    GS_TCB  *ptcb = gk_PCB_GetCurrentTCB();
    
    GRTOS_USER_CRITICAL_SECTION_GET;
    
        // Check whether the task is already consuming from the presource
        pevent = gk_TCB_in_RCBGEL((G_RCB *) presource, (GS_TCB *) ptcb);
        
        // pevent = presource->RCB_NextRCBGEL;
        // while (pevent != (GS_ECB  *) 0 && ptcb != pevent->ECB_AssocTCB) {
        //     pevent = pevent->ECB_NextECB;
        // }
    
        if (pevent == (GS_ECB  *) 0) { // task is not a consumer of the message queue
            pevent = gk_ECB_GetFree();
            if (pevent == (struct gs_ecb *) 0) return(pevent);  // there is not more ECB available
            prrds_buffer     = gk_RRDS_GetFree();
            if (prrds_buffer == (GS_RRDS *) 0) {
                gk_ECBFL_Link(pevent);
                return (GS_ECB *) 0;
            }
            pevent->ECB_RRDS = prrds_buffer;
            pevent = gk_RCBGEL_Link(presource, pevent);
            
            gk_TCBAEL_Link(pevent,ptcb); 
        }
        else { // task is already a consumer
            prrds_buffer = pevent->ECB_RRDS;
        }

        pevent->ECBType  = G_ECBType_QUEUE_CONSUME;
        prrds_buffer->RRDS_AsocECB = pevent;

        prrds_buffer->queue_buffer.BUFFER_status = BUFFER_status_READ;
        prrds_buffer->queue_buffer.BUFFER_pbuffer = pbuffer;
        prrds_buffer->queue_buffer.BUFFER_buffer_length = buffer_length;
        prrds_buffer->queue_buffer.BUFFER_priority.i64 = (INT64) G_LOWEST_PRIORITY - (INT64) 100;
        prrds_buffer->queue_buffer.BUFFER_timeout.i64 = (INT64) timeout;
        prrds_buffer->queue_buffer.BUFFER_current_byte_index = (INT32) 0;
        prrds_buffer->queue_buffer.BUFFER_msg_seq = presource->queue.MQ_msg_seq;
        
        // !!!!!! colocar que lea data si hay en la cola
        gk_check_consume(pevent);
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return pevent;
}

/**gk_check_consume
 *  \brief Transfer message from the resource to the consumer
 *  \param [in] pevent Pointer to the ECB associated as consumer to the Message Queue
 *  \return G_TRUE when message read, G_FALSE otherwise
 *  \relates Resource
 */
INT32 gk_check_consume(GS_ECB * pevent) {
    
    GS_RRDS *prrds_buffer = (GS_RRDS *) pevent->ECB_RRDS;
    GS_TCB  *ptcb = pevent->ECB_AssocTCB;
    G_RCB   *prcb = (G_RCB *) pevent->ECB_AssocRCB;
    INT32   bytes_buffer;
    INT32   bytes_msg_left;
    INT32   bytes_to_transfer;
    void    *source_address;
    void    *destination_address;
    
    if (prrds_buffer->queue_buffer.BUFFER_status == BUFFER_status_NOT_READ) {
        return G_FALSE;
    }
    if (prrds_buffer->queue_buffer.BUFFER_msg_seq != prcb->queue.MQ_msg_seq) {
        return G_FALSE; // consumer is ready for next message
    }
    // Here the consumer is ready to read the current message
    bytes_buffer   = prrds_buffer->queue_buffer.BUFFER_buffer_length;
    bytes_msg_left = prcb->queue.MQ_msg_Length - prrds_buffer->queue_buffer.BUFFER_current_byte_index;
    
    if (bytes_buffer > bytes_msg_left) bytes_to_transfer = bytes_msg_left;
    else bytes_to_transfer = bytes_buffer;
    
    source_address = (void *) (prcb->queue.MQ_PBuffer + prrds_buffer->queue_buffer.BUFFER_current_byte_index);
    destination_address = (void *) (prrds_buffer->queue_buffer.BUFFER_pbuffer);
    
    memcpy(destination_address, source_address, bytes_to_transfer);
    
    prrds_buffer->queue_buffer.BUFFER_current_byte_index += bytes_to_transfer;
    prrds_buffer->queue_buffer.BUFFER_status = BUFFER_status_NOT_READ;
    
    if (ptcb->TCBState == G_TCBState_WAITING) { // it is waiting but not completed
        gk_TCB_Unlink(ptcb);
        gk_TCBRDYL_Link(ptcb);
    }
    
    return G_TRUE;
}

/**gu_MQ_consumer_mark_read
 *  \brief Mark the current buffer as read.
 *  \param [in] pevent Pointer to the ECB associated as consumer to the Message Queue
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \details This function has to be executed to enable the reading of the next message
 */
INT32 gu_MQ_consumer_mark_read(GS_ECB * pevent) {
    GS_RRDS *prrds_buffer;
    G_RCB   *prcb;
    GS_TCB  *ptcb;

    GRTOS_USER_CRITICAL_SECTION_GET;
    
    prrds_buffer = (GS_RRDS *) pevent->ECB_RRDS;
    prcb = (G_RCB *) pevent->ECB_AssocRCB;

        prrds_buffer->queue_buffer.BUFFER_status = BUFFER_status_READ;
        if (prrds_buffer->queue_buffer.BUFFER_current_byte_index != prcb->queue.MQ_msg_Length) { //all the message was  not read
            gk_check_consume(pevent); // get new data
            return G_TRUE;
        }
        
        prcb->queue.MQ_consumer_count++;
        prrds_buffer->queue_buffer.BUFFER_current_byte_index = 0;
        prrds_buffer->queue_buffer.BUFFER_msg_seq++;
        
        // Check if it was the last deliver to a consumer
        if (prcb->queue.MQ_consumer_count == prcb->queue.MQ_Count_deliver) {
            // Check if there is a new sender
            if (prcb->RCB_NextRCBWEL != (struct gs_ecb *) 0) {
                ptcb = prcb->RCB_NextRCBWEL->ECB_AssocTCB;
                gk_TCB_Unlink(ptcb);
                gk_TCBRDYL_Link(ptcb);
            }
        }
        
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**gk_timeout_ECB_MQ_send
 *  \brief This function is called from the timed IRQ when a send timeout G_ECBType_MESSAGE_SEND_WAIT happens
 *  \param [in] peventime Pointer to the timed ECB happened
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Resource
 */
INT32 gk_timeout_ECB_MQ_send(GS_ECB *peventime) {
    GS_TCB  *ptcb;
    peventime->ECBType = G_ECBType_MESSAGE_SEND_EXPIRED;    
    ptcb = peventime->ECB_AssocTCB;
    gk_TCB_Unlink(ptcb);
    gk_TCBRDYL_Link(ptcb);
    return G_TRUE;
}

/**gk_timeout_ECB_MQ_consume
 *  \brief This function is called from the timed IRQ when a consume timeout G_ECBType_MESSAGE_CONSUME_WAIT happens
 *  \param [in] peventime Description for peventime
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Resource
 */
INT32 gk_timeout_ECB_MQ_consume(GS_ECB *peventime) {
    GS_TCB  *ptcb;
    peventime->ECBType = G_ECBType_MESSAGE_CONSUME_EXPIRED;    
    ptcb = peventime->ECB_AssocTCB;
    gk_TCB_Unlink(ptcb);
    gk_TCBRDYL_Link(ptcb);
    return G_TRUE;
}


INT32 gk_QUEUE_granted_kill(GS_ECB *pevent) {
    // G_RCB   *prcb = (G_RCB *) pevent->ECB_AssocRCB;
    gk_ECB_List_Unlink((GS_ECB *) pevent);
    gk_ECBFL_Link((GS_ECB *)pevent);
    return G_TRUE;

}

OPTIMEZE_RESTORE