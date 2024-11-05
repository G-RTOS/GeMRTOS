/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Message queue definitions
 *  \details This file contains the Message queue definitions.
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

// Check https://stackoverflow.com/questions/1154709/how-can-i-hide-the-declaration-of-a-struct-in-c

#ifndef MQ_H_
#define MQ_H_

/**
 *  \brief The <b>T_QUEUE_RESOURCE</b> is defined as the field <em>"queue"</em> in a G_RCB resource. So, the fields of the T_QUEUE_RESOURCE structure should be addressed as: <em>(G_RCB *)->queue.\<field\></em>
 *  \relates Message_Queue
 */
struct T_QUEUE_RESOURCE {
    // void          *MQ_PBuffer;              ///< \brief Pointer to the buffer
    // G_INT32         MQ_Count_deliver;         ///< \brief Number of times a message has to be delivered
    // G_INT32         MQ_consumer_count;        ///< \brief Number of times a message was delivered
    // G_INT32         MQ_Buffer_Length;         ///< \brief Length of the buffer
    // G_INT32         MQ_msg_Length;            ///< \brief Length of the current message                       
    G_INT32         MQ_priority_send;         ///< \brief Priority for the next ECB to send (to put last) 
    // G_INT32         MQ_priority_consume;      ///< \brief Priority for the next ECB to cons (to put last)     
    G_INT32         MQ_msg_seq;               ///< \brief Number of sequence of the current message       
    // struct gs_tcb *MQ_CUR_TCB ;             ///< \brief Pointer to the task of the TCB that send cur msg
};

//*************************************************************************************************
/// \defgroup BUFFER_status queue_buffer::BUFFER_status 
/**
 *  \brief BUFFER CONTROL BLOCK FOR UNION WITH RRDS STRUCT
 */
 
struct rrds_producer {
    G_INT32 deliveries;   ///< \brief Times the message was delivered
    G_INT32 consumers;    ///< \brief number of consumer should be delivered
    G_INT32 MQ_msg_seq;   ///< \brief Identification of the message
    char  *pmsg;        ///< \brief Pointer to the message
    G_INT32 msg_length;   ///< \brief Length of the message
};

struct rrds_consumer {
    G_INT32 MQ_msg_seq;   ///< \brief Consumed seq ID
    GS_ECB *pproducer;  ///< \brief Pointer to the event of the current consuming producer
    char *buffer_msg;   ///< \brief Pointer to the consumer buffer as destination of the message sent
    // 15-07-2024 G_INT32 buffer_length;  ///< \brief Length of the consumer buffer
    G_INT32 msg_length;     ///< \brief Length of the producer message (0 if not message produced)
};

struct queue_buffer {     
    union {
        struct rrds_producer rrds_producer;
        struct rrds_consumer rrds_consumer;    
    };
};


G_RCB  *gu_MessageQueueCreate(void);
GS_ECB *gu_MessageQueueSubscribe(GS_TCB  *ptcb, G_RCB  *presource);
G_INT32 gk_MQ_move_to_consumer(GS_RRDS *pbuffer_consumer, G_INT32 start_byte_id, G_INT32 bytes_count);
G_INT32 gk_MQ_Read_Sender(void);
int gu_MessageQueueSend(G_RCB  *prcb, char *pmsg, int msg_length, gt_time timeout);
int gu_MessageQueuePrintf(G_RCB *prcb, char *format, ...);

int gu_MessageQueueReceive(G_RCB *prcb, void * buffer_msg, G_INT32 buffer_length);
G_INT32 gk_timeout_ECB_MQ_consume(GS_ECB *peventime);
G_INT32 gk_queue_kill_producer(GS_ECB *pevent);
G_INT32 gk_timeout_ECB_MQ_send(GS_ECB *peventime);
// G_INT32 gk_QUEUE_granted_kill(GS_ECB *pevent);


#endif /* MQ_H__H_ */
