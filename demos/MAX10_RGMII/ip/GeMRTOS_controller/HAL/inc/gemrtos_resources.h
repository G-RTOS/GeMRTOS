/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Definition of GeMRTOS resources
 *  \details This file contains the definition of RCB, RRDS structures and includes the different kinds of resources, like semaphore and message queues.
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


#ifndef __GEMRTOS_RESOURCES__
#define __GEMRTOS_RESOURCES__

#include <gemrtos_mq.h>
#include <gemrtos_sem.h>
#include <gemrtos_trigger.h>

//*************************************************************************************************
/// \defgroup RCBState g_rgb::RCBState 
/// \defgroup RCBType  g_rgb::RCBType 

// GS_RCB Resource Request Data Structure
/**
 *  \brief g_rgb  Resource Control Block (RCB)
 *  \details The g_rgb holds information about different kinds of resources.
 *  \note A future revision may unify the control structures of the different resource kinds.
 */
struct g_rcb{
    struct {
        unsigned int   BLOCK_HASH;            ///< \brief BLOCK_HASH of the RCB: (GS_RCB *) + G_RCB_HASH
        G_UINT32       RCBType;               ///< \brief Type of resource control block     \ingroup RCBType
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

// RCBType valid values
typedef enum {
	GS_RCBType_FREE                   = ENUM_CODE(GE_RCBType_HASH, 1),
	GS_RCBType_SEM                    = ENUM_CODE(GE_RCBType_HASH, 2),
	GS_RCBType_QUEUE                  = ENUM_CODE(GE_RCBType_HASH, 3),
	GS_RCBType_TRIGGER                = ENUM_CODE(GE_RCBType_HASH, 4),
	GS_RCBType_SEM_COUNTING           = ENUM_CODE(GE_RCBType_HASH, 5),
	GS_RCBType_SEM_BINARY             = ENUM_CODE(GE_RCBType_HASH, 6),
	GS_RCBType_SEM_MUTEX              = ENUM_CODE(GE_RCBType_HASH, 7),
	GS_RCBType_SEM_RECURSIVEMUTEX	  = ENUM_CODE(GE_RCBType_HASH, 8)
} GE_RCBType;


//*************************************************************************************************
// GS_RRDS Resource Request Data Structure
/**
 *  \brief gs_rrds  Resource Request Data Structure (RRDS)
 *  \details The gs_rrds holds information about different kinds of resource requests.
 *  \note A future revision may unify the request structures of the different resource kinds.
 */
struct gs_rrds {
    struct {
        unsigned int   BLOCK_HASH;         ///< \brief BLOCK_HASH of the RRDS: (GS_RRDS *) + G_RRDS_HASH
        struct gs_rrds *RRDS_NextRRDS;     ///< \brief Pointer to the next RRDS structure                    
        struct gs_ecb  *RRDS_AsocECB;      ///< \brief Pointer to ECB the RRDS is linked to

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        // Fields for debugging
        struct gs_rrds  *RRDS_NEXT_RRDSs;  ///< \brief Pointer to next RRDS structure. Used for debugging purposes.
        struct gs_rrds  *RRDS_PREV_RRDSs;  ///< \brief Pointer to previous RRDS structure. Used for debugging purposes.
#endif			
    };
    union {
        struct queue_buffer queue_buffer;  ///< \brief QUEUE BUFFER structure defined in mq.h
    };
};

#endif /* __GEMRTOS_RESOURCES__ */
