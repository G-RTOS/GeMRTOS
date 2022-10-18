/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Message queue definitions
 *  \details This file contains the Message queue definitions.
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

// Check https://stackoverflow.com/questions/1154709/how-can-i-hide-the-declaration-of-a-struct-in-c

#ifndef MQ_H_
#define MQ_H_

/**
 *  \brief Message Queue Resource
 *  \details It is used as union in G_RCB
 *  \relates Resource
 */
struct T_QUEUE_RESOURCE {
    void          *MQ_PBuffer;              ///< \brief Pointer to the buffer                           
    INT32         MQ_Count_deliver;         ///< \brief Number of times a message has to be delivered   
    INT32         MQ_consumer_count;        ///< \brief Number of times a message was delivered         
    INT32         MQ_Buffer_Length;         ///< \brief Length of the buffer                            
    INT32         MQ_msg_Length;            ///< \brief Length of the current message                       
    INT32         MQ_priority_send;         ///< \brief Priority for the next ECB to send (to put last) 
    INT32         MQ_priority_consume;      ///< \brief Priority for the next ECB to cons (to put last)     
    INT32         MQ_msg_seq;               ///< \brief Number of sequence of the current message       
    struct gs_tcb *MQ_CUR_TCB ;            ///< \brief Pointer to the task of the TCB that send cur msg
};

//*************************************************************************************************
/// \defgroup BUFFER_status queue_buffer::BUFFER_status 
/**
 *  \brief BUFFER CONTROL BLOCK FOR UNION WITH RRDS STRUCT
 */
struct queue_buffer {         
    INT32   BUFFER_status;                ///< \brief EMPTY, NEW_MESSAGE, END_OF_MESSAGE, MIDDLE_OF_MESSAGE  \ingroup BUFFER_status      
    void    *BUFFER_pbuffer;              ///< \brief pointer to the buffer of the message to transmit         
    INT32   BUFFER_buffer_length;         ///< \brief current index to the byte to transfer to resource buffer 
    INT32   BUFFER_current_byte_index;    ///< \brief current index to the byte transfered to the buffer       
    INT32   BUFFER_msg_seq;               ///< \brief Byte index in the RCB buffer copied                      
    TIMEPRIORITY BUFFER_priority;         ///< \brief Priority assigned to the consumed TCB                    
    TIMEPRIORITY BUFFER_timeout;          ///< \brief Timeout assigned to the waiting for message                  
};

/** Values for BUFFER_STATUS */
#define BUFFER_status_READ                  1u     ///< \brief Buffer is READ           \ingroup BUFFER_status
#define BUFFER_status_NOT_READ              2u     ///< \brief Buffer is NOT READ       \ingroup BUFFER_status

G_RCB  *gu_queue_create(void *pbuffer, INT32 buffer_length);
GS_ECB  *gu_queue_consume(G_RCB  *presource, void *pbuffer, INT32 buffer_length, gt_time timeout);
INT32 gk_check_consume(GS_ECB * pbuffer);
INT32 gk_MQ_move_to_consumer(GS_RRDS *pbuffer_consumer, INT32 start_byte_id, INT32 bytes_count);
INT32 gu_MQ_consumer_mark_read(GS_ECB * pevent);
INT32 gk_MQ_Read_Sender(void);
INT32 gk_MQ_send(G_RCB  *prcb, void *pmsg, INT32 msg_length, gt_time timeout);
INT32 gu_MQ_receive(GS_ECB  *pevent);
INT32 gk_timeout_ECB_MQ_consume(GS_ECB *peventime);
INT32 gk_timeout_ECB_MQ_send(GS_ECB *peventime);
INT32 gk_QUEUE_granted_kill(GS_ECB *pevent);



#endif /* MQ_H__H_ */
