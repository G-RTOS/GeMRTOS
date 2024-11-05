/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Definition of GeMRTOS resources
 *  \details This file contains the definition of RCB, RRDS structures and includes the 
 *  \details different kinds of resources, like semaphore and message queues.
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


#ifndef __GEMRTOS_RESOURCES__
#define __GEMRTOS_RESOURCES__


#include <mq.h>
#include <sem.h>
#include <trigger.h>

//*************************************************************************************************
/// \defgroup RCBState g_rgb::RCBState 
/// \defgroup RCBType  g_rgb::RCBType 

// G_RCB Resource Request Data Structure
/**
 *  \brief g_rgb  Resource Control Block (RCB)
 *  \details The g_rgb holds information about different kinds of resources.
 *  \todo Define a unified structure for different kind of resources. 
 */
struct g_rcb{
    struct {
        unsigned int  BLOCK_HASH;             ///< \brief BLOCK_HASH of the RCB: (GS_RCB *) + G_RCB_HASH
        // 18-10-2024 G_INT32         RCBState;               ///< \brief State of the Resource              \ingroup RCBState
        G_INT32         RCBType;                ///< \brief Type of resource control block     \ingroup RCBType
        // 18-10-2024 G_INT32         RCBCount;               ///< \brief Counter for RCB
        // 15-07-2024 void          *malloc_address;        ///< \brief Pointer memory address of the malloc block                         
        // 18-10-2024 TIMEPRIORITY  RCBPriority;            ///< \brief Priority of the resource
        // 18-10-2024 TIMEPRIORITY  RCBGrantedPriority;     ///< \brief Lowest Priority of Current Granted Task
        // 18-10-2024 TIMEPRIORITY  RCBWaitingTimeout;      ///< \brief Default waiting timeout
        // 18-10-2024 TIMEPRIORITY  RCBGrantedTimeout;      ///< \brief Default granted timeout
        struct gs_ecb *RCB_NextRCBWEL;        ///< \brief Pointer to linked list of waiting events of this event
        struct gs_ecb *RCB_NextRCBGEL;        ///< \brief Pointer to the linked highest priority event
        struct g_rcb  *RCB_NextRCB;           ///< \brief Pointer to link resources in free list
        struct gs_scb *RCB_NextRCBASL;        ///< \brief Pointer to the Linked list of signals
        
        // Fields for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        struct g_rcb *RCB_NEXT_RCBs;     ///< \brief Pointer to next RCB structure. Used for debugging purposes.
        struct g_rcb *RCB_PREV_RCBs;     ///< \brief Pointer to previous RCB structure. Used for debugging purposes.
#endif
    };    
    union {
        struct T_SEMAPHORE_RESOURCE semaphore;     ///< \brief is the semaphore resource structure
        struct T_QUEUE_RESOURCE     queue;         ///< \brief is the queue resource, defined in mq.h
        struct T_TRIGGER_RESOURCE   trigger;       ///< \brief is the trigger resource, defined in trigger.h        
    };
};

// RCBState valid values                     
// #define  GK_RCBState_SEM           3u        ///< \brief State SEM for semaphore of the RCB    \ingroup RCBState 
// #define  GK_RCBState_MQ            4u        ///< \brief State MQ for MESSAGE QUEUE of the RCB \ingroup RCBState
// #define  GK_RCBState_FREE     5u        ///< \brief State UNDEFINED of the RCB            \ingroup RCBState
// #define  GK_RCBState_QUEUE         6u        ///< \brief State QUEUE for QUEUE of the RCB      \ingroup RCBState

// RCBType valid values
#define  GK_RCBType_FREE         1u        ///< \brief Type UNUSED of the RCB               \ingroup RCBState 
// #define  GK_RCBType_FREE           2u        ///< \brief Type FREE of the RCB                 \ingroup RCBState
#define  GK_RCBType_SEM            3u        ///< \brief Type SEM for semaphore of the RCB    \ingroup RCBState
#define  GK_RCBType_QUEUE          5u        ///< \brief Type QUEUE for QUEUE of the RCB      \ingroup RCBState
#define  GK_RCBType_TRIGGER        6u        ///< \brief Type Trigger                         \ingroup RCBState


//*************************************************************************************************
/// \defgroup RRDS_State gs_rrds::SCBState 
/// \defgroup RRDS_Type  gs_rrds::SCBType 

// GS_RRDS Resource Request Data Structure
/**
 *  \brief gs_rrds  Resource Request Data Structure (RRDS)
 *  \details The gs_rrds holds information about different kinds of resource requests.
 *  \todo Define a unified srtucture for different kind of resources. 
 */
struct gs_rrds {
    struct {
            unsigned int   BLOCK_HASH;           ///< \brief BLOCK_HASH of the RRDS: (GS_RRDS *) + G_RRDS_HASH
            // 18-10-2024 G_INT32        RRDS_State;           ///< \brief STATE of the RRDS      \ingroup RRDS_State
            // 18-10-2024 G_INT32        RRDS_Type;            ///< \brief TYPE of the RRDS       \ingroup RRDS_Type           
            // 15-07-2024 void           *malloc_address;      ///< \brief Pointer memory address of the malloc block                            
            struct gs_rrds *RRDS_NextRRDS;       ///< \brief Pointer to the next RRDS structure                    
            // 15-07-2024 struct gs_scb  *RRDS_NextSCB;        ///< \brief Pointer to the Linked list of signals 
            struct gs_ecb  *RRDS_AsocECB;        ///< \brief Pointer to ECB the RRDS is linked to
            
            // Fields for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
            struct gs_rrds  *RRDS_NEXT_RRDSs;     ///< \brief Pointer to next RRDS structure. Used for debugging purposes.
            struct gs_rrds  *RRDS_PREV_RRDSs;     ///< \brief Pointer to previous RRDS structure. Used for debugging purposes.
#endif			
    };
    union {
        struct queue_buffer queue_buffer;                 ///< \brief QUEUE BUFFER structure defined in mq.h
    };
};


#endif /* __GEMRTOS_RESOURCES__ */
