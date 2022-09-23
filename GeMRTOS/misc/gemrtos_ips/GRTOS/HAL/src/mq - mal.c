/**
 * @file mq.c
 * @author Ricardo Cayssials (rcayssials@gmail.com)
 * @brief GRTOS Kernal functions
 * @version 0.1
 * @date 2013-03-27
 * 
 * @copyright Copyright (c) 2013-2020
 * 
 */

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) Ricardo L. Cayssials                                          *
* All rights reserved.                                                        *
*                                                                             *
******************************************************************************/


#include <stdio.h>
#include <grtosdefinitions.h>

OPTIMEZE_CODE(3)

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
            prcb->queue.common_resource.RCBState = GK_RCB_STATE_QUEUE;
            prcb->queue.common_resource.RCBType = (INT32) GK_RCB_TYPE_QUEUE;
	        prcb->queue.common_resource.RCBPriority.i64 = (INT64) G_LOWEST_PRIORITY - (INT64) 100;
	        prcb->queue.common_resource.RCBGrantedPriority.i64 = (INT64) G_LOWEST_PRIORITY - (INT64) 100;
	        prcb->queue.common_resource.RCBWaitingTimeout.i64 = (INT64) G_LATEST_TIME - (INT64) 100;
	        prcb->queue.common_resource.RCBGrantedTimeout.i64 = (INT64) G_LATEST_TIME - (INT64) 100;
            prcb->queue.common_resource.RCB_NextRCBWEL = (struct gs_ecb *) 0;
            prcb->queue.common_resource.RCB_NextRCBGEL = (struct gs_ecb *) 0;
            prcb->queue.common_resource.RCB_NextRCB = (struct g_rcb *) 0;
            prcb->queue.common_resource.RCB_NextRCBASL = (struct gs_scb *) 0;

            prcb->queue.MQ_Next_PAD = (struct gs_rrds *) 0;
            prcb->queue.MQ_C_PAD_W = (struct gs_rrds *) 0;
            prcb->queue.MQ_PBuffer = (void *) pbuffer;
            prcb->queue.MQ_Buffer_Length = buffer_length;
            prcb->queue.MQ_Byte_ID = (INT32) 0;
            prcb->queue.MQ_index_tail = (INT32) 0;
            prcb->queue.MQ_index_head = (INT32) 0;
        }
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
	return((G_RCB  *) prcb);
}

G_RCB  *gu_queue_consume(G_RCB  *presource, void *pbuffer, INT32 buffer_length, gt_time timeout)
{
	GS_RRDS *prrds_buffer = (GS_RRDS *) 0;
	GS_ECB  *pevent = (GS_ECB  *) 0;
    
    GRTOS_USER_CRITICAL_SECTION_GET;
        // Link a new event with default priority (0)
        pevent = gk_RCBGEL_Link((G_RCB *) presource, (GS_ECB *) 0);    
        
        prrds_buffer     = gk_RRDS_GetFree();
        if (prrds_buffer == (GS_RRDS *) 0) {
            gk_ECBFL_Link(pevent);
            return (GS_ECB *) 0;
        }

        pevent->ECBType  = G_ECB_TYPE_QUEUE_CONSUME;
        pevent->ECB_RRDS = prrds_buffer;
        prrds_buffer->RRDS_AsocECB = pevent;

        prrds_buffer->queue_buffer;

        prrds_buffer->queue_buffer.BUFFER_status = BUFFER_EMPTY;
        prrds_buffer->queue_buffer.BUFFER_RCB_index_rcb_buffer = (INT32) 0;
        prrds_buffer->queue_buffer.BUFFER_pbuffer = pbuffer;
        prrds_buffer->queue_buffer.BUFFER_buffer_length = buffer_length;
        prrds_buffer->queue_buffer.BUFFER_priority.i64 = (INT64) G_LOWEST_PRIORITY - (INT64) 100;
        prrds_buffer->queue_buffer.BUFFER_timeout.i64 = (INT64) timeout;

        prrds_buffer->queue_buffer.BUFFER_current_byte_index = (INT32) 0;
        /** Get the first byte in queue to be transfered */
        if (presource->queue.MQ_Next_PAD != (struct gs_rrds *) 0) {
            /** there exist a pad with a message to read */
            prrds_buffer->queue_buffer.BUFFER_RCB_index_rcb_buffer = presource->queue.MQ_Next_PAD->queue_pad.PAD_start_index_rcb_buffer;

        
            // transfer data from rcb buffer to ecb buffer
            // How many bytes available there are in the buffer of the buffer
            INT32 bytes_in_buffer_left = prrds_buffer->queue_buffer.BUFFER_buffer_length - prrds_buffer->queue_buffer.BUFFER_current_byte_index;
            // How many bytes are in the pad to be transmitted by this buffer
            INT32 bytes_in_pad_left = prrds_buffer->queue_buffer.BUFFER_RCB_index_rcb_buffer - presource->queue.MQ_Next_PAD->queue_pad.PAD_start_index_rcb_buffer;
            bytes_in_pad_left = presource->queue.MQ_Next_PAD->queue_pad.PAD_message_length - bytes_in_pad_left;
            // memcpy(varB, varA, 32);        
            
        }      
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    
    
}


INT32 gk_MQ_Read_Sender(void) {
    return G_FALSE;
}

#define SEND_FIRST_BYTE_ID(pad)   ((GS_RRDS *) pad)->queue_pad.PAD_start_index_rcb_buffer

INT32 gk_MQ_read_consumer(GS_RRDS *pbuffer_consumer)
{
    // GS_RRDS *prrds_buffer = (GS_RRDS *) 0;
	// GS_ECB  *pevent = (GS_ECB  *) 0; common_rrds
    
    GS_ECB  *pevent;
    GS_ECB  *peventtime;
    GS_TCB  *ptcb;
    
    INT32 sender_last_byte;
    INT32 sender_first_byte;
    INT32 sender_new_bytes; 
    INT32 sender_curr_byte;
    INT32 cons_bytes_left;
    INT32 bytes_to_move;
    INT32 cons_first_byte;
    INT32 cons_last_byte;
    
    G_RCB *prcb_queue = pbuffer_consumer->RRDS_AsocECB->ECB_AssocRCB;
    INT32 cons_cur_byte = pbuffer_consumer->queue_buffer.BUFFER_RCB_index_rcb_buffer;
    GS_RRDS *ppad = (GS_RRDS *) prcb_queue->queue.MQ_Next_PAD;

    if (ppad == (GS_RRDS *) 0) {
        return G_FALSE;
    }
    
    while (cons_cur_byte > ppad->queue_pad.PAD_start_index_rcb_buffer + ppad->queue_pad.PAD_message_length ) {
        if (ppad == (GS_RRDS *) 0) {
            return G_FALSE;
        }
        ppad = ppad->RRDS_NextRRDS;
    }    

    sender_last_byte = ppad->queue_pad.PAD_start_index_rcb_buffer + ppad->queue_pad.PAD_message_length;    
    
    
    // ppad is pointing to the pad structure with the data to read
    sender_first_byte = SEND_FIRST_BYTE_ID(ppad);
    // sender_first_byte = ppad->queue_pad.PAD_start_index_rcb_buffer;
    sender_curr_byte  = sender_first_byte + ppad->queue_pad.PAD_current_byte_index;
    sender_last_byte  = sender_first_byte + ppad->queue_pad.PAD_message_length;
    sender_new_bytes  = sender_curr_byte - cons_cur_byte;
    cons_bytes_left   = pbuffer_consumer->queue_buffer.BUFFER_buffer_length - pbuffer_consumer->queue_buffer.BUFFER_current_byte_index;
    
    if (cons_bytes_left >= sender_new_bytes) bytes_to_move = sender_new_bytes;
    else bytes_to_move = cons_bytes_left;
    
    gk_MQ_move_to_consumer((GS_RRDS *)pbuffer_consumer, (INT32) cons_cur_byte, (INT32) bytes_to_move);
    
    pbuffer_consumer->queue_buffer.BUFFER_current_byte_index += bytes_to_move;
    pbuffer_consumer->queue_buffer.BUFFER_RCB_index_rcb_buffer += bytes_to_move;
    
    cons_last_byte = pbuffer_consumer->queue_buffer.BUFFER_RCB_index_rcb_buffer;
    cons_first_byte = cons_last_byte - pbuffer_consumer->queue_buffer.BUFFER_current_byte_index;
    
    pbuffer_consumer->queue_buffer.BUFFER_status = BUFFER_EMPTY;
    
    // Check if it has to be released
    if (cons_last_byte - cons_first_byte == pbuffer_consumer->queue_buffer.BUFFER_buffer_length ||
        cons_last_byte == sender_last_byte) {
            
        if (cons_last_byte == sender_last_byte) {
            if (cons_first_byte == sender_first_byte) {
                pbuffer_consumer->queue_buffer.BUFFER_status = BUFFER_FULL_MESSAGE;
            }
            else pbuffer_consumer->queue_buffer.BUFFER_status = BUFFER_END_OF_MESSAGE;
        }
        else {
            if (cons_first_byte == sender_first_byte) {
                pbuffer_consumer->queue_buffer.BUFFER_status = BUFFER_NEW_MESSAGE;
            }
            else pbuffer_consumer->queue_buffer.BUFFER_status = BUFFER_MIDDLE_OF_MESSAGE;            
        }
        
        if (cons_last_byte == sender_last_byte) {
            // The sender message was sent to the consumer
            ppad->queue_pad.PAD_delivery_counter++;
            if (ppad->queue_pad.PAD_delivery_counter == prcb_queue->queue.MQ_Count_deliver) {
                prcb_queue->queue.MQ_Next_PAD = ppad->RRDS_NextRRDS;
                prcb_queue->queue.MQ_index_tail += ppad->queue_pad.PAD_message_length;
                if (prcb_queue->queue.MQ_index_tail >= prcb_queue->queue.MQ_Buffer_Length) {
                    prcb_queue->queue.MQ_index_tail -= prcb_queue->queue.MQ_Buffer_Length;
                }
                gk_RRDSFL_Link(ppad);
                gk_MQ_Read_Sender();
                
            }
        }
        
        
        pevent = pbuffer_consumer->RRDS_AsocECB;
        if (pevent->ECBType == G_ECB_STATE_MESSAGE_CONSUME_WAIT) {
            ptcb = pevent->ECB_AssocTCB;
            peventtime = pevent->ECB_NextECBAEL;            
            gk_ECBTL_Unlink(peventtime);
            gk_TCB_Unlink(ptcb);
            gk_TCBRDYL_Link(ptcb);
        }

    }
    return G_TRUE;
}

INT32 gk_MQ_move_to_consumer(GS_RRDS *pbuffer_consumer, INT32 start_byte_id, INT32 bytes_count)
{
    void *start_dest_address = pbuffer_consumer->queue_buffer.BUFFER_pbuffer + pbuffer_consumer->queue_buffer.BUFFER_current_byte_index;
    return G_TRUE;
}

OPTIMEZE_RESTORE