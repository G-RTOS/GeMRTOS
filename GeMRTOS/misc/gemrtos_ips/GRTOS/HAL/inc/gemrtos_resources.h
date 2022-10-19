/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Definition of GeMRTOS resources
 *  \details This file contains the definition of RCB, RRDS structures and includes the 
 *  \details different kinds of resources, like semaphore and message queues.
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


#ifndef __GEMRTOS_RESOURCES__
#define __GEMRTOS_RESOURCES__

#include <mq.h>
#include <sem.h>

//*************************************************************************************************
/// \defgroup RCBState g_rgb::RCBState 
/// \defgroup RCBType  g_rgb::RCBType 

/// G_RCB Resource Request Data Structure
/**
 *  \brief g_rgb  Resource Control Block (RCB)
 *  \details The g_rgb holds information about different kinds of resources.
 *  \todo Define a unified structure for different kind of resources. 
 */
struct g_rgb{
    struct {
        unsigned int  BLOCK_HASH;             ///< \brief BLOCK_HASH of the RCB: (GS_RCB *) + 3
        INT32         RCBState;               ///< \brief State of the Resource              \ingroup RCBState
        INT32         RCBType;                ///< \brief Type of resource control block     \ingroup RCBType
        INT32         RCBCount;               ///< \brief Counter for RCB
        void          *malloc_address;        ///< \brief Pointer memory address of the malloc block                         
        TIMEPRIORITY  RCBPriority;            ///< \brief Priority of the resource
        TIMEPRIORITY  RCBGrantedPriority;     ///< \brief Lowest Priority of Current Granted Task
        TIMEPRIORITY  RCBWaitingTimeout;      ///< \brief Default waiting timeout
        TIMEPRIORITY  RCBGrantedTimeout;      ///< \brief Default granted timeout
        struct gs_ecb *RCB_NextRCBWEL;        ///< \brief Pointer to linked list of waiting events of this event
        struct gs_ecb *RCB_NextRCBGEL;        ///< \brief Pointer to the linked highest priority event
        struct g_rcb  *RCB_NextRCB;           ///< \brief Pointer to link resources in free list
        struct gs_scb *RCB_NextRCBASL;        ///< \brief Pointer to the Linked list of signals
        
        /// Fields for debugging
        struct g_rgb *RCB_NEXT_RCBs;     ///< \brief Pointer to next RCB structure. Used for debugging purposes.
        struct g_rgb *RCB_PREV_RCBs;     ///< \brief Pointer to previous RCB structure. Used for debugging purposes.
    };    
    union {
        struct T_SEMAPHORE_RESOURCE semaphore;         ///< \brief is the semaphore resource structure
        struct T_QUEUE_RESOURCE     queue;             ///< \brief is the queue resource, defined in mq.h
    };
};

/// RCBState valid values                     
#define  GK_RCBState_SEM           3u        ///< \brief State SEM for semaphore of the RCB    \ingroup RCBState 
#define  GK_RCBState_MQ            4u        ///< \brief State MQ for MESSAGE QUEUE of the RCB \ingroup RCBState
#define  GK_RCBState_UNDEFINED     5u        ///< \brief State UNDEFINED of the RCB            \ingroup RCBState
#define  GK_RCBState_QUEUE         6u        ///< \brief State QUEUE for QUEUE of the RCB      \ingroup RCBState

/// RCBType valid values
#define  GK_RCBType_UNUSED         1u        ///< \brief Type UNUSED of the RCB               \ingroup RCBState 
#define  GK_RCBType_FREE           2u        ///< \brief Type FREE of the RCB                 \ingroup RCBState
#define  GK_RCBType_SEM            3u        ///< \brief Type SEM for semaphore of the RCB    \ingroup RCBState
#define  GK_RCBType_MQ             4u        ///< \brief Type MQ for MESSAGE QUEUE of the RCB \ingroup RCBState
#define  GK_RCBType_QUEUE          5u        ///< \brief Type QUEUE for QUEUE of the RCB      \ingroup RCBState


//*************************************************************************************************
/// \defgroup RRDS_State gs_rrds::SCBState 
/// \defgroup RRDS_Type  gs_rrds::SCBType 

/// GS_RRDS Resource Request Data Structure
/**
 *  \brief gs_rrds  Resource Request Data Structure (RRDS)
 *  \details The gs_rrds holds information about different kinds of resource requests.
 *  \todo Define a unified srtucture for different kind of resources. 
 */
struct gs_rrds {
    struct {
            unsigned int   BLOCK_HASH;           ///< \brief BLOCK_HASH of the RRDS: (GS_RRDS *) + 5
            INT32          RRDS_State;           ///< \brief STATE of the RRDS      \ingroup RRDS_State
            INT32          RRDS_Type;            ///< \brief TYPE of the RRDS       \ingroup RRDS_Type           
            void           *malloc_address;      ///< \brief Pointer memory address of the malloc block                            
            struct gs_rrds *RRDS_NextRRDS;       ///< \brief Pointer to the next RRDS structure                    
            struct gs_scb  *RRDS_NextSCB;        ///< \brief Pointer to the Linked list of signals 
            struct gs_ecb  *RRDS_AsocECB;        ///< \brief Pointer to ECB the RRDS is linked to
            
            /// Fields for debugging
            struct gs_rrds  *RRDS_NEXT_RRDSs;     ///< \brief Pointer to next RRDS structure. Used for debugging purposes.
            struct gs_rrds  *RRDS_PREV_RRDSs;     ///< \brief Pointer to previous RRDS structure. Used for debugging purposes.            
    };
    union {
        struct {         
            TIMEPRIORITY   RRDSWaitingTimeout;   ///< \brief Timeout for waiting the resource      
            TIMEPRIORITY   RRDSGrantedTimeout;   ///< \brief Timeout for granting the resource     
            TIMEPRIORITY   RRDSWaitingPriority;  ///< \brief Priority for waiting the resource     
            TIMEPRIORITY   RRDSGrantedPriority;  ///< \brief Priority for granting the resource    
        };
        struct queue_buffer queue_buffer;                 ///< \brief QUEUE BUFFER structure defined in mq.h
    };
};



#endif /* __GEMRTOS_RESOURCES__ */
