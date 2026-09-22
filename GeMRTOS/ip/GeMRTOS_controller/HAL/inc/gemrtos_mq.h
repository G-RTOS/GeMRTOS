/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS Message queue definitions
 *  \details This file contains the Message queue definitions.
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


#ifndef GEMRTOS_MQ_H_
#define GEMRTOS_MQ_H_

#define ERR_RECEIVE_ABORT     0xFFFFFFFF

/** Maximum permitted value for the buffer_length parameter of
 *  gu_MessageQueueSubscribe(). Enforced at runtime by PRINT_ASSERT.
 *  MISRA C:2012 Rule 5.5. */
#define MQ_BUFFER_MAX_LENGTH  65535U

/**
 *  \brief The <b>T_QUEUE_RESOURCE</b> is defined as the field <em>"queue"</em> in a GS_RCB resource. So, the fields of the T_QUEUE_RESOURCE structure should be addressed as: <em>(GS_RCB *)->queue.\<field\></em>
 *  \relates Message_Queue
 */
struct T_QUEUE_RESOURCE {
	G_UINT32         MQ_subscribers;      ///< \brief Number of receive subscribers
    G_UINT32         MQ_seq_send;         ///< \brief Sequence number for the next ECB to send   
};

//*************************************************************************************************
/// \defgroup BUFFER_status queue_buffer::BUFFER_status 
/**
 *  \brief BUFFER CONTROL BLOCK FOR UNION WITH RRDS STRUCT
 */
 
struct rrds_mq_sender {
    G_UINT32 deliveries;   ///< \brief Times the message was delivered
    G_UINT32 MQreceivers;  ///< \brief Number of receivers to should be delivered
    G_UINT32 MQ_msg_seq;   ///< \brief Sequence of the message
    const char *pmsg;     ///< \brief Pointer to the message (read-only; MISRA C:2012 Rule 8.13)
    int     msg_length;   ///< \brief Length of the message to be sent
};

struct rrds_mq_receiver {
    G_UINT32 MQ_msg_seq;     ///< \brief Last of sequence of sender received
    GS_ECB *psender;        ///< \brief Pointer to the event of the current sender
    char   *buffer_msg;     ///< \brief Pointer to the receive buffer
	G_UINT32 buffer_length;  ///< \brief Length of the receive buffer
    int     msg_length;     ///< \brief Length of message received 
};

struct queue_buffer {     
    union {
        struct rrds_mq_sender rrds_mq_sender;
        struct rrds_mq_receiver rrds_mq_receiver;    
    };
};


/**
 * \defgroup MessageQueue_API Message Queue User API
 * \brief Public message queue functions exposed to application tasks.
 * \relates Message_Queue
 * @{
 */

/** \brief Creates a message queue resource.
 *         Tasks send messages via gu_MessageQueueSend() and receive them
 *         via gu_MessageQueueReceive() after subscribing with
 *         gu_MessageQueueSubscribe().
 *  \return Pointer to the newly created GS_RCB message queue resource,
 *          or NULL if no queue resource is available.
 *  \sa gu_MessageQueueSend, gu_MessageQueueReceive, gu_MessageQueueSubscribe
 *  \relates Message_Queue
 */
GS_RCB *gu_MessageQueueCreate(void);

/** \brief Destroys a message queue and returns its RCB to the free pool.
 *         Fails (returns G_FALSE) if the queue still has subscribers or pending
 *         sender/receiver events.
 *  \param [in] pqueue Handle returned by gu_MessageQueueCreate().
 *  \return G_TRUE if destroyed; G_FALSE if still in use.
 *  \relates Message_Queue
 */
G_UINT32 gu_MessageQueueDestroy(GS_RCB *pqueue);

/** \brief Subscribes a task to a message queue so it can receive messages.
 *         Must be called before the task invokes gu_MessageQueueReceive().
 *  \param [in] ptcb          Pointer to the Task Control Block (GS_TCB) of
 *                            the subscribing task.
 *  \param [in] presource     Pointer to the GS_RCB message queue resource,
 *                            as returned by gu_MessageQueueCreate().
 *  \param [in] buffer_msg    Pointer to the memory buffer where received
 *                            messages will be stored.
 *  \param [in] buffer_length Maximum number of bytes the buffer can hold.
 *  \return Pointer to the GS_ECB event associated with this subscription,
 *          or NULL on error.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSend, gu_MessageQueueReceive
 *  \relates Message_Queue
 */
GS_ECB *gu_MessageQueueSubscribe(GS_TCB *ptcb, GS_RCB *presource,
                                  void *buffer_msg, G_UINT32 buffer_length);

/** \brief Transmits a message to a message queue.
 *         The calling task blocks until the message has been delivered to all
 *         subscribed receivers or until the timeout expires.
 *  \param [in] prcb       Pointer to the GS_RCB message queue, as returned
 *                         by gu_MessageQueueCreate().
 *  \param [in] pmsg       Pointer to the message data to be sent.  The
 *                         pointed-to data is read-only (MISRA C:2012 Rule 8.13).
 *  \param [in] msg_length Length of the message in bytes.
 *  \param [in] timeout    Maximum time to wait for delivery in system ticks.
 *                         Pass 0 to attempt delivery without waiting.
 *  \return Number of receivers the message was delivered to (positive) if
 *          fully delivered; a negative value (partial delivery count negated)
 *          if the timeout expired before all receivers were reached.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueReceive, gu_MessageQueueSubscribe
 *  \relates Message_Queue
 */
int gu_MessageQueueSend(GS_RCB *prcb, const char *pmsg, int msg_length,
                         G_UINT64 timeout);

/** \brief Sends a printf-formatted string message to a message queue.
 *         The calling task blocks until the message has been delivered to all
 *         subscribed receivers.
 *  \param [in] prcb   Pointer to the GS_RCB message queue, as returned by
 *                     gu_MessageQueueCreate().
 *  \param [in] format Null-terminated printf-style format string (read-only;
 *                     MISRA C:2012 Rule 8.13).
 *  \param [in] ...    Optional arguments matching the format specifiers.
 *  \return G_TRUE always (success flag; character count is not propagated).
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSend, gu_MessageQueueReceive
 *  \relates Message_Queue
 */
G_UINT32 gu_MessageQueuePrintf(GS_RCB *prcb, const char *format, ...);

/** \brief Retrieves the next message from a message queue.
 *         The calling task must have previously subscribed to the queue using
 *         gu_MessageQueueSubscribe().  If no message is available the task
 *         blocks until one arrives.  If the received message exceeds the
 *         subscription buffer length it is silently truncated.
 *  \param [in] prcb Pointer to the GS_RCB message queue, as returned by
 *                   gu_MessageQueueCreate().
 *  \return Number of bytes actually received (may be less than the message
 *          length if the buffer was too small); ERR_RECEIVE_ABORT
 *          (0xFFFFFFFF) if the receive was aborted.
 *  \note Return type is \c int representing a byte count, NOT a G_TRUE /
 *        G_FALSE boolean.  A return value of 0 means an empty message was
 *        delivered, not a failure.
 *  \sa gu_MessageQueueCreate, gu_MessageQueueSend, gu_MessageQueueSubscribe
 *  \relates Message_Queue
 */
int gu_MessageQueueReceive(GS_RCB *prcb);

/** @} */ /* end MessageQueue_API */



#endif /* GEMRTOS_MQ_H_ */
