/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GEmRTOS CORE definitions
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

#ifndef GEMRTOS_CORE_H_
#define GEMRTOS_CORE_H_

/************************************************************************************
 *  System constants definitions
 ************************************************************************************/
#define G_FALSE             0u
#define G_TRUE              1u

/************************************************************************************
 *  GRTOS TYPE DEFINITIONS
 ************************************************************************************/

typedef struct gs_kcb      GS_KCB;
typedef struct gs_pcb      GS_PCB;
typedef struct gs_lcb      GS_LCB;
typedef struct gs_tcb      GS_TCB;
typedef struct gs_ecb      GS_ECB;
typedef struct gs_scb      GS_SCB; 
typedef struct gs_rrds     GS_RRDS;
typedef struct g_rcb       G_RCB;
typedef struct gs_mcb      GS_MCB;
typedef struct gs_pcb_rdy_lcbl GS_PCBAssocLCB;
typedef struct g_rcb       t_semaphore_resource;
typedef union timepriority TIMEPRIORITY;



union timepriority {
    G_INT64 i64  __attribute__((aligned(4)));
    G_INT32 i32[2];
};

// 15-07-2024 typedef struct gs_tm {
// 15-07-2024 	int	tm_msec;	/* Seconds: 0-59 (K&R says 0-61?) */
// 15-07-2024 	int	tm_sec;		/* Seconds: 0-59 (K&R says 0-61?) */
// 15-07-2024 	int	tm_min;		/* Minutes: 0-59 */
// 15-07-2024 	int	tm_hour;	/* Hours since midnight: 0-23 */
// 15-07-2024 	int	tm_day;     /* Day of the month: 1-31 */
// 15-07-2024 	int	tm_year;	/* Years since 1900 */
// 15-07-2024 } gt_tm;


/************************************************************************************
 *  GeMRTOS HASH DEFINITIONS
 ************************************************************************************/
#define G_TCB_HASH       1
#define G_ECB_HASH       2
#define G_RCB_HASH       3
#define G_SCB_HASH       4
#define G_RRDS_HASH      5
#define G_LCB_HASH       6
#define G_PCBALCB_HASH   7
#define G_PCB_HASH       8

/************************************************************************************
 *  CONSTANT PARAMETERS
 ************************************************************************************/
/************************************************************************************
 *  GeMRTOS DATA STRUCTURES
 ************************************************************************************/

//*************************************************************************************************
/** \defgroup ECBState gs_ecb::ECBState
 *  @{
 */
enum ecbstate {
	/** \brief State event for Resource GRANTED
	 *  \ingroup ECBState
	 */
	GS_ECBState_GRANTED_RESOURCE = 1,

	/** \brief State event for Resource WAITING
	 *  \ingroup ECBState
	 */
	GS_ECBState_WAITING_RESOURCE = 2,

	/** \brief State for FREE event
	 *  \ingroup ECBState
	 */
	GS_ECBState_FREE = 3,

	/** \brief State for TIMED EVENT
	 *  \ingroup ECBState
	 */
	GS_ECBState_WAITING_TIME = 4,

	/** \brief State for UNLINKED event
	 *  \ingroup ECBState
	 */
	GS_ECBState_UNLINKED = 5      
};
 /** @} */

/** \defgroup ECBType  gs_ecb::ECBType 
 *  @{
 */
enum ecbtype {
	/** \brief Type for delay event
	 *  \ingroup ECBType
	 */
	G_ECBType_OSTimeDly = 21,
	
	/** \brief Type for PERIODIC event
	 *  \ingroup ECBType
	 */
	G_ECBType_PERIODIC = 22,

	/** \brief Type for LASTEST TIME event at end of time list
	 *  \ingroup ECBType
	 */
	G_ECBType_LASTEST_TIME = 23,

	/** \brief Type for semaphore granted event
	 *  \ingroup ECBType
	 */
	G_ECBType_SEM_GRANTED = 28,

	/** \brief Type for semaphore waiting event
	 *  \ingroup ECBType
	 */
	G_ECBType_SEM_WAITING = 29,

	/** \brief Type for UNSPECIFIED event
	 *  \ingroup ECBType
	 */
	G_ECBType_NOT_SPECIFIED = 30,

	/** \brief Type for MESSAGE QUEUE consume event
	 *  \ingroup ECBType
	 */
	G_ECBType_QUEUE_CONSUME = 31,
	
	/** \brief Type for WAITING TIMEOUT of MQ consumer
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_CONSUME_WAIT = 33,
	
	/** \brief Type for WAITING TIMEOUT of MQ consumer            
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_PRODUCE_WAIT = 34,
	
	/** \brief Type for WAITING TIMEOUT of MQ consumer
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_PRODUCER_DELIVERED = 35,

	/** \brief Type for MQ consumer when waiting timeout expired  
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_CONSUME_EXPIRED = 36,

	/** \brief Type for WAITING TIMEOUT of MQ sender              
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_SEND_EXPIRED = 37,

	/** \brief Type for MQ sender when waiting timeout expired    
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_SEND_WAIT = 38,

	/** \brief Type for Waiting External interrupt event                  
	 *  \ingroup ECBType
	 */
	G_ECBType_EXT_ISR_WAITING = 40,

	/** \brief Type for Released External interrupt event
	 *  \ingroup ECBType
	 */
	G_ECBType_EXT_ISR_RELEASE = 41
 };	
/** @} */

// GS_ECB  Event Control Block structure
/**
 *  \brief gs_ecb  Event Control Block structure (EBC)
 *  \details The gs_ecb holds information about events. 
 *  \todo Check if all the states and type are needed
 */
struct gs_ecb {
    unsigned int   BLOCK_HASH;         ///< \brief BLOCK_HASH of the ECB: (GS_ECB *) + G_ECB_HASH
	enum ecbstate  ECBState;           ///< \brief Granted, Waiting, Free  \ingroup ECBState
	enum ecbtype   ECBType;            ///< \brief Type of event control block   \ingroup ECBType
    // 15-07-2024 void           *malloc_address;    ///< \brief Pointer memory address of the malloc block         
	TIMEPRIORITY   ECBValue;           ///< \brief Occurrence Time of the event or Priority              
    struct gs_ecb  *ECB_NextECB;       ///< \brief Pointer to linked list of waiting events of this event
    struct gs_ecb  *ECB_PrevECB;       ///< \brief Pointer to linked list of waiting events of this event
    struct gs_tcb  *ECB_AssocTCB;      ///< \brief Pointer to the task associated with the event         
    struct g_rcb   *ECB_AssocRCB;      ///< \brief Pointer to the resource associated with the event     
    struct gs_ecb  *ECB_NextTCBAEL;    ///< \brief Pointer to the next event of the same task
    struct gs_ecb  *ECB_PrevTCBAEL;    ///< \brief Pointer to the previous event of the same task        
    struct gs_ecb  *ECB_NextECBAEL;    ///< \brief Pointer to the event associated with this (ie timeout)
    struct gs_scb  *ECB_NextECBASL;    ///< \brief Pointer to the Linked list of signals
    struct gs_rrds *ECB_RRDS;          ///< \brief Pointer to the resource request structure or MCB
    
    /// Fields for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    struct gs_ecb  *ECB_NEXT_ECBs;     ///< \brief Pointer to next TCB structure. Used for debugging purposes.
    struct gs_ecb  *ECB_PREV_ECBs;     ///< \brief Pointer to previous TCB structure. Used for debugging purposes.
#endif
};

//*************************************************************************************************
// GS_PCB  Processor Control Block structure

/** \defgroup PCBState g_rgb::PCBState
 *  @{
 */
enum pcbstate {
	/** \brief It is NOTRUNNING at starting time
     *  \ingroup PCBState
     */
	GS_PCBState_NOTRUNNING = 1,

	/** \brief It is RUNNING when executing a main list task (LCB[0])
	 *  \ingroup PCBState 
	 */
	GS_PCBState_RUNNING = 2,

	/** \brief It is FREE when not executing a main list task
	 *  \ingroup PCBState
	 */
	GS_PCBState_FREE = 3
};
 /** @} */
 
/// \defgroup PCBType  g_rgb::PCBType 
enum pcbtype {
	/** \brief It is GS_PCBType_AVAILABLE when it is available for task scheduling
	 *  \ingroup PCBType
	 */
	GS_PCBType_AVAILABLE = 1,
	
	/** \brief It is GS_PCBType_UNAVAILABLE when it is assigned to waiting for interrupt
	 *  \ingroup PCBType
	 */
	GS_PCBType_UNAVAILABLE = 2
};
// GS_PCB Processor Control Block
/**
 *  \brief gs_pcb  Processor Control Block structure (PCB)
 *  \details The gs_pcb holds information about processor. 
 *  Each processors has an unique PCBID according to the ID of the associated processor.
 *  \todo Define the type for processors
 */
struct gs_pcb {
	unsigned int  BLOCK_HASH;             ///< \brief BLOCK_HASH of the PCB: (GS_PCB *) + G_PCB_HASH
	G_INT32 PCBID;                            ///< \brief Processor ID 
	enum pcbstate PCBState; ///< \brief State of the PCB:GS_FREE_PROCESSOR,GS_RUNNING_PROCESSOR \ingroup PCBState
    enum pcbtype PCBType;    ///< \brief Type of the processor   \ingroup PCBType
	volatile G_INT32 PCB_Critical_PCB; ///< \brief G_TRUE if critical section associated to Processor, G_FALSE associated to TCB
    // 11-07-2024 int *GRTOS_PROCESSOR_BASE __attribute__((aligned(4))); /// \brief Processor specific addresses to go to idle state
    struct gs_pcb_rdy_lcbl *PCB_AssocLCB;       ///< \brief Linked list of the associated LCBs that processor serves
	struct gs_tcb *PCB_IDLETCB;                 ///< \brief Pointer to the TCB of the IDLE Task 
	struct gs_tcb *PCB_EXECTCB;                 ///< \brief Current task assigned to processor 
	struct gs_pcb *PCB_NextPCB;                 ///< \brief Pointer to the next free processor 
	struct gs_pcb *PCB_PrevPCB;                 ///< \brief Pointer to the previous free processor
	
#ifndef __niosX_arch__
	DWORD  ThreadId;
    ucontext_t uctx_main;
    void *uctx_main_stack;
    ucontext_t uctx_gk_ENTRY_TASK_COMPLETE;
    void *uctx_gk_ENTRY_TASK_COMPLETE_stack;
#endif
};

//*************************************************************************************************
// GS_KCB  Kernel Control Block structure
/**
 *  \brief gs_kcb  Kernel Control Block structure (LCB)
 *  \details The gs_kcb holds information of the system. 
  */
struct gs_kcb {
	struct gs_ecb     *KCB_NextECBTL;             ///< \brief Pointer to EVENT LIST of the Next Absolute Time
	struct gs_tcb     *KCB_NextTCBWL;             ///< \brief Pointer to the linked list of the TCB of Waiting Tasks
	struct gs_lcb     *KCB_NextLCBL;              ///< \brief Pointer to the lowest priority run list
	struct gs_scb     *KCB_NextKCBASL;            ///< \brief Pointer to Signal Associated List for KCB
    struct gs_scb     *KCB_NextKCBPSL;            ///< \brief Pointer to Signal Associated List for KCB
	
    struct g_rcb *KCB_NextRCBTRGL[G_NUMBER_OF_IRQ_TCB]; ///< \brief Pointers to the RCB of triggers
                                                      ///< \brief Triggers 0 to ALT_NIRQ-1 are associated to external interrupts
                                                      ///< \brief This array associates IRD ID with pointer to trigger RCB 
    
    // 14-09-2023 GS_PCB  *G_PCBTbl __attribute__((aligned(4)));    ///< \brief Pointer to PCBs dynamic array
	struct gs_pcb G_PCBTbl[G_NUMBER_OF_PCB];    ///< \brief System PCBs
	
	G_INT64				function_max_interval;
	G_INT32				function_current_number;
	G_INT32				function_prescaler;
    
    // Pointers to free structures to avoid free and malloc frequently
    // Free structures are linked to this pointers instead of free the memory
    // New create structures pop from this list or malloc if it is empty
    struct gs_tcb    *KCB_FREE_TCBs;        ///< \brief pointer to the first free TCB structure.
    struct gs_ecb    *KCB_FREE_ECBs;        ///< \brief pointer to the first free ECB structure.
    struct g_rcb     *KCB_FREE_RCBs;        ///< \brief pointer to the first free RCB structure.
    struct gs_lcb    *KCB_FREE_LCBs;        ///< \brief pointer to the first free LCB structure.
    struct gs_scb    *KCB_FREE_SCBs;        ///< \brief pointer to the first free SCB structure.
    struct gs_rrds   *KCB_FREE_RRDSs;       ///< \brief pointer to the first free RRDS structure.
    struct gs_pcb_rdy_lcbl *KCB_FREE_RDYs;  ///< \brief pointer to the first gs_pcb_rdy_lcbl structure. Used for debugging purposes.

#ifndef __niosX_arch__
    // G_INT32 fake_grtos_cmd_prc_id;
    CRITICAL_SECTION GeMRTOS_MUTEX_section;
    CRITICAL_SECTION GeMRTOS_MUTEX_swapcontext;
	CRITICAL_SECTION GeMRTOS_MUTEX_CONDITION_VARIABLE;
	CONDITION_VARIABLE GeMRTOS_CONDITION_VARIABLE;
#endif
    
    // roots for linked list of data structures
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    G_INT32             KCB_NUMBER_OF_TCBs;             ///< \brief Number of TCB in the system
    G_INT32             KCB_NUMBER_OF_PCBs;             ///< \brief Number of PCB in the system
    G_INT32             KCB_NUMBER_OF_ECBs;             ///< \brief Number of ECB in the system
    G_INT32             KCB_NUMBER_OF_RCBs;             ///< \brief Number of RCB in the system
    G_INT32             KCB_NUMBER_OF_LCBs;             ///< \brief Number of LCB in the system
    G_INT32             KCB_NUMBER_OF_SCBs;             ///< \brief Number of SCB in the system
    G_INT32             KCB_NUMBER_OF_RRDSs;            ///< \brief Number of RRDS in the system
    G_INT32             KCB_NUMBER_OF_PCBAssocLCBs;     ///< \brief Number of RRDS in the system	

    struct gs_tcb    *KCB_ROOT_TCBs;        ///< \brief pointer to the first TCB structure. Used for debugging purposes.
    struct gs_ecb    *KCB_ROOT_ECBs;        ///< \brief pointer to the first ECB structure. Used for debugging purposes.
    struct g_rcb     *KCB_ROOT_RCBs;        ///< \brief pointer to the first RCB structure. Used for debugging purposes.
    struct gs_lcb    *KCB_ROOT_LCBs;        ///< \brief pointer to the first LCB structure. Used for debugging purposes.
    struct gs_scb    *KCB_ROOT_SCBs;        ///< \brief pointer to the first SCB structure. Used for debugging purposes.
    struct gs_rrds   *KCB_ROOT_RRDSs;       ///< \brief pointer to the first RRDS structure. Used for debugging purposes.
    struct gs_pcb_rdy_lcbl *KCB_ROOT_RDYs;  ///< \brief pointer to the first gs_pcb_rdy_lcbl structure. Used for debugging purposes.
#endif
};

/** \defgroup LCBState LCB State group 
 *  @{ */
enum lcbstate {
	/** \brief State LINKED for LIST  
	 *  \ingroup LCBState 
	 *  \remark GS_LCBState_LINKED
	 */
	GS_LCBState_LINKED = 1u,
	
	/** \brief State UNLINKED for LIST 
	 *  \ingroup LCBState 
	 *  \remark GS_LCBState_UNLINKED
	 */
	GS_LCBState_UNLINKED =2u  
};
/** @}*/

/** \defgroup LCBType  gs_lcb::LCBType 
 *  @{ */
enum lcbtype {
// 22-10-2024	GS_LCBType_UNSPECIFIED = 2,  

/** \brief The <b>GS_LCBTypeEDF</b> scheduling list type implements the Earliest Deadline First (EDF) discipline among the tasks assigned to the scheduling list. In the EDF discipline, the earliest deadline, the highest priority. In order to maintain consistency, tasks that are assigned to an EDF scheduled list should be of the periodic type, with the deadline taken into account starting from the release time.
 *  \ingroup LCBType
 *  \remark GS_LCBTypeEDF
 */
	GS_LCBTypeEDF = 3,

/** \brief The <b>GS_LCBTypeFP</b> scheduling list type implements the Fixed Priority (FP) discipline among the tasks assigned to the scheduling list. In the FP discipline, a priority is assigned to each task. Task priority may be modified during runtime. In order to maintain consistency, tasks that are assigned to an FP scheduled list should not be of the infinite-loop type without waiting for event suspension in order to avoid starving lower-priority tasks. Only the lowest-priority task could be implemented as an infinite-loop code.
 *  \ingroup LCBType
 *  \remark GS_LCBTypeFP
 */
	GS_LCBTypeFP = 4
};
/** @}*/

// GS_LCB List Control Block
/**
 *  \brief gs_lcb  List Control Block structure (LCB)
 *  \details The gs_lcb holds information about processor. 
 *  \todo Check correctness for multiple list scheduling
 *  \todo define types
 *  \todo Include maximun number of running tasks and current running tasks to control exclusion
 */
struct gs_lcb {
    unsigned int  BLOCK_HASH;          ///< \brief BLOCK_HASH of the LCB: (GS_LCB *) + G_LCB_HASH
    enum lcbstate LCBState;            ///< \brief State of the List Control Block  \ingroup LCBState
    enum lcbtype  LCBType;             ///< \brief Type of the List Control Block   \ingroup LCBType     
    G_INT32       LCBCurrentRunning;   ///< \brief Current number of running tasks
    G_INT32       LCBExclusion;        ///< \brief Maximum number of running task (0 for no limit)
	struct gs_tcb *LCB_NextTCBRUNL;    ///< \brief Pointer to the TCB list of running tasks 
	struct gs_tcb *LCB_NextTCBRDYL;	   ///< \brief Pointer to the TCB of the Highest Priority Task 
	struct gs_lcb *LCB_NextLCBL;       ///< \brief Pointer to the next list ordered by priority 
	struct gs_lcb *LCB_PrevLCBL;       ///< \brief Pointer to the next list ordered by priority 
	struct gs_pcb *LCB_NextLCBFPL;     ///< \brief Next free processor for this list
    
    // Fields for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    struct gs_lcb *LCB_NEXT_LCBs;  ///< \brief Pointer to next LCB structure. Used for debugging purposes.
    struct gs_lcb *LCB_PREV_LCBs; ///< \brief Pointer to previous LCB structure. Used for debugging purposes
#endif
};



//*************************************************************************************************
// gs_pcb_rdy_lcbl Processor associated LCB list
/**
 *  \brief gs_pcb_rdy_lcbl  structure to associate a PCB to a LCB
 *  \details The gs_pcb holds a linked list of gs_pcb_rdy_lcbl with each one of the LCB associated. 
 */
struct gs_pcb_rdy_lcbl {
    unsigned int           BLOCK_HASH;       ///< \brief BLOCK_HASH : (GS_PCBAssocLCB *) + G_PCBALCB_HASH
    struct gs_pcb_rdy_lcbl *gs_pcb_rdy_lcbl_next;
    struct gs_pcb_rdy_lcbl *gs_pcb_rdy_lcbl_prev;
    G_INT32                 PCB_RDY_priority;
    struct gs_lcb          *PCB_RDY_LCBL;
    
    /// Fields for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    struct gs_pcb_rdy_lcbl *gs_pcb_lcbl_nexts;
    struct gs_pcb_rdy_lcbl *gs_pcb_lcbl_prevs;
    G_INT32 gs_pcb_assoc_pcb;
#endif
};




//*************************************************************************************************
 
/** \defgroup SCBType  SCBType group
 *  @{ */
enum scbtype {
/** \brief The <b>G_SCBType_TCB_ABORTED</b> signal type is defined to signaling when a task is aborted. A task aborting happens when a new release of a periodic task takes place before the previos invokation completes. The associated abortion function will be executed prior the execution of the next instance of the task.
 *  \ingroup SCBType
 *  \remark g_scbtype_tcb_aborted
 */	
	G_SCBType_TCB_ABORTED = 1000u,
	
/** \brief The G_SCBType_FROZEN_MODE signal type is defined to signaling when GeMRTOS controller enters in frozen mode.
 *  \ingroup SCBType
 *  \remark g_scbtype_tcb_aborted
 */	
	G_SCBType_FROZEN_MODE = 1u
};
/** @} */

/** \defgroup SCBState gs_scb::SCBState
 * @{
 */
enum scbstate {
	/** \brief State FREE of the SCB
	 *  \ingroup SCBState
	 */
	G_SCBState_FREE = 1,

	/** \brief State SCB linked to KCB
	 * \ingroup SCBState
	 */
	G_SCBState_IN_KCB = 2,

	/** \brief State SCB linked to ECB
	 *  \ingroup SCBState
	 */
	G_SCBState_IN_ECB = 3,

	/** \brief State SCB linked to TCB
	 *  \ingroup SCBState
	 */
	G_SCBState_IN_TCB = 4,

	/** \brief State SCB linked to RCB
	 *  \ingroup SCBState
	 */
	G_SCBState_IN_RCB = 5,

	/** \brief State SCB linked to PCB
	 *  \ingroup SCBState
	 */
	G_SCBState_IN_PCB = 6,

	/** \brief State SCB linked to LCB
	 *  \ingroup SCBState
	 */
	G_SCBState_IN_LCB = 7,

	/** \brief State UNLINKED
	 *  \ingroup SCBState
	 */
	G_SCBState_UNLINKED = 8,

	/** \brief State PENDING
	 *  \ingroup SCBState
	 */
	G_SCBState_PENDING = 9,

	/** \brief State EXECUTING
	 *  \ingroup SCBState
	 */
	G_SCBState_EXECUTING = 10
};
/** @} */



// GS_SCB  Signal Control Block structure
/**
 *  \brief gs_scb  Signal Control Block (SCB) structure
 *  \details The gs_scb structure defines the pending and associated signals for tasks, events, resources and kernal.
 *  \todo define the rest of signal types required. 
  */
struct gs_scb {
    unsigned int  BLOCK_HASH;               ///< \brief BLOCK_HASH of the SCB: (GS_SCB *) + G_SCB_HASH
    enum scbstate   SCBState;                 ///< \brief STATE of the signal  \ingroup SCBState
	enum scbtype    SCBType;                  ///< \brief TYPE of the signal   \ingroup SCBType
    G_INT32         SCBPriority;              ///< \brief Priority of the SCB when it is linked
    void          *SCB_TaskCode;            ///< \brief Pointer to the code of the signal 
    void          *SCB_TaskArg;             ///< \brief Pointer to the argument of the signal 
    struct        gs_scb *SCB_NextSCB;      ///< \brief Pointer to the next SCB linked
    // 22-10-2024 struct        gs_scb *SCB_NextSCBAPSL;  ///< \brief Pointer to the next pending SCB lined
    void          *SCB_AssocXCB;            ///< \brief Pointer to the data structure root of the SCBASL
    
    // Fields for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    struct gs_scb  *SCB_NEXT_SCBs;     ///< \brief Pointer to next SCB structure. Used for debugging purposes.
    struct gs_scb  *SCB_PREV_SCBs;     ///< \brief Pointer to previous SCB structure. Used for debugging purposes.
#endif
};



//*************************************************************************************************
/** \defgroup TCBState gs_tcb::TCBState
 *  @{
 */
enum tcbstate {
	/** \brief State WAITING, task COMPLETED     
	 *  \ingroup TCBState 
	 */
	G_TCBState_WAITING_COMPLETED = 2,

	/** \brief State WAITING, but NOT COMPLETED  
	 *  \ingroup TCBState
	 */
	G_TCBState_WAITING = 258,

	/** \brief State READY                       
	 *  \ingroup TCBState
	 */
	G_TCBState_READY = 4,

	/** \brief State RUNNING                     
	 *  \ingroup TCBState
	 */
	G_TCBState_RUNNING = 8,
	
	/** \brief State UNLINKED                    
	 *  \ingroup TCBState	
	 */
	G_TCBState_UNLINKED = 32   
};
 /** @} */

// #define  GRTOS_TASK_STATE_WAITING(ptcb)  ((ptcb->TCBState >> 1) & 1) ///< Returns True if task is waiting  \ingroup TCBState

#define  GRTOS_TASK_STATE_WAITING(ptcb)  ((ptcb->TCBState == G_TCBState_WAITING_COMPLETED) || (ptcb->TCBState == G_TCBState_WAITING))  ///< Returns True if task is waiting  \ingroup TCBState

/** \defgroup TCBType  gs_tcb::TCBType 
 *  @{
 */
enum tcbtype {
	/** \brief The <b>G_TCBType_OneShot</b> task type makes the task code to be executed just once. The task must be released once again for another execution if the task code does not contain an infinite loop. Initialization tasks may be implemented as a <b>G_TCBType_OneShot</b> task type without infinite loop in the task code. G_TCBType_OneShot tasks are often implemented as an infinite loop to keep them running. When a task with an infinite loop is executed, it will take as much processor time as possible.  It is possible to use different strategies to prevent one or many system tasks from being overly greedy about processor time and starving the others:
	 *  \li Assigning lowest priorities to tasks: in this way, infinite-loop tasks will be executed only when the highest priority tasks are not requiring for execution.
	 *  \li Suspending the task until an event: the task is suspended inside the infinite loop, waiting for an event. The events may be timed events (to execute the task regularly) or trigger events (such as waiting for an interrupt).
	 *  \li Reducing the task priority: the task priority may be reduced inside the infinite loop to let the new higher-priority task be executed. This tactic should be implemented in all the infinite-loop tasks of the scheduling list to dynamically preserve a valid relationship among the system task priorities.
	 *  \li Defining a round-robin scheduling mechanism in the scheduling list: a round-robin mechanism will execute each task during a certain interval, granting the processor access to each task in the scheduling list.
	 *  \ingroup TCBType
	 *  \remark G_TCBType_OneShot
	 */
	G_TCBType_OneShot = 1, 

	/** \brief The <b>G_TCBType_PERIODIC</b> task type makes the task code to be executed periodically. The period of the task is configured when the type is specified. The period of the task and the initial offset determines the future releases times of the task. If previous invocation of the task does not completes, then the previous invocation may be defined to be aborted or the next release skipped. Periodic tasks are useful to meet Nyquist and Shannon theorems in cyber-physical applications. However, if no scheduling analysis is performed, the system may became oversaturated and the deadlines missed. 
	 *  \ingroup TCBType
	 *  \remark G_TCBType_PERIODIC
	 */
	G_TCBType_PERIODIC = 2,   

	/** \brief The <b>G_TCBType_ISR</b> task type determines that the task is associated with a trigger resource. The G_TCBType_ISR type of the task should be set after the task is created using the <em>gu_TriggerRegisterTask</em> function.
	 *  \ingroup TCBType
	 *  \remark G_TCBType_ISR
	 */
	G_TCBType_ISR = 4,   

	/** \brief The <b>G_TCBType_IDLE</b> task type determines the task that a processor executes when no task requires for execution. The IDLE task is a GeMRTOS system task and there is one for each system processor. By default, the G_TCBType_IDLE task turns the processor into sleep mode in order to save energy and reduce the system bus utilization.
	 *  \ingroup TCBType
	 *  \remark G_TCBType_IDLE
	 */
	G_TCBType_IDLE = 5,

};
/** @} */

/**
 *  \brief gs_tcb  Task Control Block (TCB) structure
 *  \details The gs_tcb structure has all the fields to hold the task parameters.
 *  \todo Check if fields can be grouped into an union to save fields for differetn type of TCBs. 
 *  For instance, period and ISR links are not requiered for the same type of task.
 */
struct gs_tcb 
{
    unsigned int  BLOCK_HASH;             ///< \brief BLOCK_HASH of the TCB: (GS_TCB *) + G_TCB_HASH
    enum tcbstate  TCBState;               ///< \brief STATE of the task  \ingroup TCBState 
    enum tcbtype  TCBType;                ///< \brief TYPE of the task   \ingroup TCBType
    G_INT64  TCBReadyPriority;              ///< \brief Priority of the Task when Ready
    G_INT64  TCBRunPriority;                ///< \brief Priority when it is executed
	G_INT64  TCBPeriod;                     ///< \brief Period of the task
	G_INT32  TCB_PrevExclusion;         ///< \brief Previous Exclusion if task set the current Exclusion section (0 otherwise)
    volatile GS_STK *TCB_StackPointer;             ///< \brief Pointer to current top of stack
    volatile GS_STK *TCB_StackBottom;              ///< \brief Botton Stack of the Task
    volatile GS_STK *TCB_StackTop;                 ///< \brief Botton Stack of the Task
    volatile void   *TCB_TaskCode;                 ///< \brief Pointer to the Task Code
	volatile void   *TCB_TaskArg;                  ///< \brief Pointer to the argument of the first call
    struct gs_tcb *TCB_NextTCB;  ///< \brief Pointer to next TCB in the TCB list
    struct gs_tcb *TCB_PrevTCB;           ///< \brief Pointer to previous TCB in the TCB list
    struct gs_ecb *TCB_NextTCBAEL;        ///< \brief Pointer to linked list of waiting events of this task
    struct gs_scb *TCB_NextTCBASL;        ///< \brief Pointer to the Linked list of signals
    struct gs_scb *TCB_NextTCBPSL;        ///< \brief Pointer to signals waiting to execute
    G_INT32 TCB_AssocPCB;            ///< \brief Processor assigned this task (0 is no assigned)
	int TCB_INTNumber;                    ///< \brief IRQ number if it is a ISR TCB
	struct gs_lcb *TCB_RDY_LCB_Index;     ///< \brief pointer to the ready list that should be inserted
	volatile G_INT32 TCB_MTX_NESTED;               ///< \brief Count for Mutex nesting of the task 
	char TCB_description[G_TCB_DESCRIPTION_LENGTH];

#ifndef __niosX_arch__
	ucontext_t uctx;
	void *uctx_stack;
#endif

	// field for newlib impure
	// 15-07-2024 struct _reent * _impure_ptr;
	
    // Fields for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    struct gs_tcb  *TCB_NEXT_TCBs;     ///< \brief Pointer to next TCB structure. Used for debugging purposes.
    struct gs_tcb  *TCB_PREV_TCBs;     ///< \brief Pointer to previous TCB structure. Used for debugging purposes.
#endif
};

enum list_type { ECBASL, KCBASL, KCBPSL, RCBASL, TCBPSL, TCBASL, ECBTL, RCBGEL, RCBWEL, TCBRDYL, TCBRUNL };

// 19-09-2024 !!!!D enum uart_type {
// 19-09-2024 !!!!D 	/** \brief UART Types
// 19-09-2024 !!!!D 	 *  \ingroup UARTType
// 19-09-2024 !!!!D 	 */
// 19-09-2024 !!!!D 	JTAG_UART = 1,
// 19-09-2024 !!!!D 	UART      = 2,
// 19-09-2024 !!!!D };
// 19-09-2024 !!!!D 
// 19-09-2024 !!!!D 
// 19-09-2024 !!!!D /**
// 19-09-2024 !!!!D  *  \brief gemrtos_jtag_uart_info  Structure to hold JTAG-UART information of GeMRTOS IP component
// 19-09-2024 !!!!D  *  \details The gemrtos_jtag_uart_info structure has all the fields to hold the JTAG-UART parameters.
// 19-09-2024 !!!!D  *  In system.h file, GRTOS_DRIVER_JTAG_ARRAY defines the structure array to hold all the information 
// 19-09-2024 !!!!D  *  of JTAG-UART components.
// 19-09-2024 !!!!D  */
// 19-09-2024 !!!!D struct gemrtos_jtag_uart_info {
// 19-09-2024 !!!!D 	G_INT32 JTAG_ID;     ///< \brief It is the ID of the JTAG-UART (generally, 0 for STDIO). It is like COMX.
// 19-09-2024 !!!!D 	                   ///< \brief If no JTAG-UART then JTAG_ID is equal to -1
// 19-09-2024 !!!!D 	enum uart_type UART_TYPE;
// 19-09-2024 !!!!D 	void * JTAG_UART_BASE;
// 19-09-2024 !!!!D 	G_INT32 JTAG_UART_IRQ;
// 19-09-2024 !!!!D 	G_INT32 JTAG_UART_IRQ_INTERRUPT_CONTROLLER_ID;
// 19-09-2024 !!!!D 	char * JTAG_UART_NAME;
// 19-09-2024 !!!!D 	G_INT32 JTAG_UART_READ_DEPTH;
// 19-09-2024 !!!!D 	G_INT32 JTAG_UART_READ_THRESHOLD;
// 19-09-2024 !!!!D 	G_INT32 JTAG_UART_SPAN;
// 19-09-2024 !!!!D 	char * JTAG_UART_TYPE;
// 19-09-2024 !!!!D 	G_INT32 JTAG_UART_WRITE_DEPTH;
// 19-09-2024 !!!!D 	G_INT32 JTAG_UART_WRITE_THRESHOLD;
// 19-09-2024 !!!!D 	G_RCB * JTAG_UART_OUTPUT_QUEUE;
// 19-09-2024 !!!!D 	G_RCB * JTAG_UART_INPUT_QUEUE;
// 19-09-2024 !!!!D 	G_RCB * JTAG_UART_OUTPUT_TRIGGER;
// 19-09-2024 !!!!D 	G_RCB * JTAG_UART_INPUT_TRIGGER;
// 19-09-2024 !!!!D };


/*
*********************************************************************************************************
*    SYSTEM VARIABLES
*********************************************************************************************************
*/

// extern const unsigned int G_PROCESSOR_MASK[];
// 19-09-2024 !!!!D extern const struct gemrtos_jtag_uart_info G_UART_INFO[];
extern const int *GRTOS_DRIVER_PROCESSOR_BASE[];
extern const unsigned int PRC_MASK[];

extern volatile int G_DEBUG_SAMPLE_BEGIN_ENABLE;
extern volatile int G_DEBUG_SAMPLE_END_ENABLE;

extern volatile G_INT32  G_Running;
extern volatile G_INT32  gs_sizeof_G_PCBTbl;
extern volatile void   *gs_addressof_G_PCBTbl;
extern volatile G_INT32  gs_offsetof_PCB_EXECTCB;
extern volatile G_INT32  gs_offsetof_TCB_StackPointer;
extern volatile G_INT32  gs_offsetof_PCB_IDLETCB;

extern GS_KCB g_kcb;

extern t_semaphore_resource *gu_fprintf_sem;
extern t_semaphore_resource *gu_printf_sem;
	
//Variable to interface C and assembler
extern volatile GS_STK G_TCB_CURRENT;
extern volatile GS_TCB *G_TCB_FROM;
extern volatile G_INT32  G_SCB_PENDING;
extern volatile G_INT32  G_SCB_CODE;
extern volatile G_INT32  G_SCB_ARG;

// extern volatile int GRTOS_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID;
extern volatile int GRTOS_S_PROCESSOR1_IRQ;

extern volatile int G_IRQ_ENABLED;
extern volatile int G_IRQ_PENDING;

// 01-07-2024 extern FILE* fpuart[G_NUMBER_OF_PCB];


extern volatile G_INT32 G_TASK_TYPE_DEFAULT;
extern volatile GS_LCB *G_TASK_LCB_DEFAULT;
extern volatile G_INT32 G_TASK_LCB_DEFAULT_PRIORITY;
extern volatile G_INT64 G_TASK_PRIORITY_DEFAULT;
extern volatile G_INT64 G_TASK_PERIOD_DEFAULT;
extern TIMEPRIORITY OC_priority;  // __attribute__((section ("onchipdata")));
extern TIMEPRIORITY OC_priority2; //  __attribute__((section ("onchipdata")));

extern struct _reent impure_data;
// 03-07-2024 extern struct _reent *impure_data_ptr;

// 03-07-2024 extern alt_llist alt_dev_list;

// Core functions definitions
GS_ECB *gk_Get_ECB(void);
G_RCB  *gk_Get_RCB(void);
GS_SCB *gk_Get_SCB(void);
GS_RRDS *gk_Get_RRDS(void);
GS_LCB *gu_SchedulingListCreate(enum lcbtype lcbtype);
GS_LCB *gk_Get_LCB(enum lcbtype lcbtype);
G_INT32 gk_Init_PCBs(void);
G_INT32 gk_Create_IDLE_tasks(void);
GS_PCBAssocLCB *gk_Get_PCBAssocLCB(void);
G_INT32 gk_PCBAssocLCBFL_Link(GS_PCBAssocLCB *ppcbalcb);
G_INT32 gu_SchedulingListAssociateProcessor(GS_LCB *plcb, G_INT32 CPUID, G_INT32 priority);



/***************************************************************/
/* INTERNAL SYSTEM FUNCTIONS                                   */
/***************************************************************/
// gt_time gu_get_mutex_time(void);
G_INT32   gk_LCB_CheckInvertion(void);
G_INT32    gk_SetLowestProcessor(void);
G_INT32    gk_SetNextTimeProcessor(void);
GS_TCB *gk_PCB_GetNextTCB(void);
GS_TCB *gk_PCB_GetCurrentTCB(void);

/***************************************************************/
/* debug function                                              */
/***************************************************************/
G_INT32 gkm_TCB_IsValid(GS_TCB *ptcb);
G_INT32 gkm_XCB_NotValid(void *ptcb);

G_INT32 gkm_XCB_IsFieldValid(void *data);

G_INT32 gkm_GS_PCBAssocLCB_IsValid(GS_PCBAssocLCB *ppcbalcb);

G_INT32 gkm_LCB_IsValid(GS_LCB *plcb);

G_INT32 gkm_PCB_IsValid(GS_PCB *ppcb);
G_INT32 gkm_ECB_IsValid(GS_ECB *pevent);
G_INT32 gkm_SCB_IsValid(GS_SCB *pscb);
G_INT32 gkm_RCB_IsValid(void *prcb);
G_INT32 gkm_PCBAssocLCB_IsValid(void *ppcbalcb);
G_INT32 gkm_RRDS_IsValid(GS_RRDS *prrds);

G_INT32 gkm_TCBState_Valid(enum tcbstate tcbstate);
G_INT32 gkm_TCBType_Valid(enum tcbtype tcbtype);
G_INT32 gkm_ECBState_Valid(enum ecbstate ecbstate);
G_INT32 gkm_ECBType_Valid(enum ecbtype ecbtype);

GS_ECB *gkm_ECB_list_ROOT(GS_ECB *pevent);
G_INT32 gkm_ECB_list_ORDER(GS_ECB *pevent);
GS_TCB *gkm_TCB_list_ROOT(GS_TCB *ptcb);
G_INT32 gkm_PCBState_Valid(enum pcbstate pcbstate);

G_INT32 gkm_IsAListorNull(GS_LCB *plcb);


/***************************************************************/
/*  LIST AND DATA STRUCTURES PROTOTYPES                        */
/***************************************************************/

G_INT32  gk_ECBAEL_Link(GS_ECB * const pevent1, GS_ECB * const pevent2);
G_INT32  gk_ECBAEL_Remove(GS_ECB * const pevent);

G_INT32  gk_ECBASL_Link(GS_ECB * const pevent, GS_SCB * const psignal);
G_INT32  gk_ECBASL_Unlink(GS_ECB *pevent, GS_SCB *psignal);
// 19-02-2024 G_INT32  gk_SCB_SCBL_Unlink(GS_SCB **root, GS_SCB *psignal);
GS_SCB   *gk_ECBASL_GetSCB(GS_ECB *pevent, G_INT32 SignalType);

GS_ECB   *gk_ECB_GetFree(void);

G_INT32  gk_ECBTL_Link (GS_ECB *pevent);
G_INT32  gk_ECBTL_Unlink(GS_ECB *pevent);

G_INT32    gk_KCBASL_Link(GS_SCB *psignal);
G_INT32   gk_KCBASL_Unlink(GS_SCB *psignal);
G_INT32  gk_KCBPSL_Link(GS_SCB *psignal);
G_INT32  gk_KCBPSL_Unlink(GS_SCB *psignal);

G_INT32    gk_LCBL_Link(GS_LCB *plcb);
GS_SCB *gk_KCBASL_GetSCB(G_INT32 SignalType);

G_INT32    gk_LCBFPL_Link(G_INT32 processorID);
G_INT32    gk_LCBFPL_Unlink(G_INT32 processorID);

G_INT32    gk_LCBL_UnLink(GS_LCB *plcb);

void    gk_PCBInit(void);

G_RCB  *gk_RCB_GetFree(void);
G_INT32    gk_RCBFL_Link(G_RCB *presource);

G_INT32 gk_TASK_RELEASE(GS_TCB *ptcb);

GS_ECB *gk_RCBXEL_Link(GS_ECB **root, G_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent, enum ecbstate ECBState);
// 19-06-2024 GS_ECB *gk_ECB_ECBL_Link(GS_ECB **root, GS_ECB *pevent);
// 19-06-2024 GS_ECB *gk_ECB_ECBL_Unlink(GS_ECB **root, GS_ECB *pevent);
GS_ECB *gk_RCBGEL_Link(G_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent);
void    gk_RCBGEL_Unlink(GS_ECB *pevent);

G_INT32    gk_RCBASL_Link(G_RCB *presource, GS_SCB *psignal);
G_INT32    gk_RCBASL_Unlink(G_RCB *presource, GS_SCB *psignal);
GS_SCB *gk_RCBASL_GetSCB(G_RCB *presource, G_INT32 SignalType);

// 11-06-2024 #define gk_RCBASL_GetSCB(prcb, SignalType) (gk_GetSCB_from_list((void *)(prcb)->RCB_NextRCBASL, (G_INT32) (SignalType)))
// 11-06-2024 #define gk_ECBASL_GetSCB(pecb, SignalType) (gk_GetSCB_from_list((void *) ((GS_ECB *) pecb)->ECB_NextECBASL, (G_INT32) (SignalType)))
// 11-06-2024 #define gk_TCBASL_GetSCB(ptcb, SignalType) (gk_GetSCB_from_list((void *)(((GS_TCB *) ptcb)->TCB_NextTCBASL), (G_INT32) (SignalType)))
// 11-06-2024 #define gk_KCBASL_GetSCB(SignalType)       (gk_GetSCB_from_list((void *) g_kcb.KCB_NextKCBASL, (G_INT32) (SignalType)))
// 11-06-2024 #define gk_RRDSASL_GetSCB(prrds, SignalType) (gk_GetSCB_from_list((void *)(prrds)->RRDS_NextSCB,(G_INT32) (SignalType)))
// 11-06-2024 #define gk_TCBPSL_GetSCB(ptcb, SignalType) (gk_GetSCB_from_list((void *)(((GS_TCB *) ptcb)->TCB_NextTCBPSL), (G_INT32) (SignalType)))

GS_SCB *gk_GetSCB_from_list(GS_SCB *head_list, G_INT32 SignalType);

GS_ECB *gk_RCBWEL_Link(G_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent);
void    gk_RCBWEL_Unlink(GS_ECB *pevent);

// 15-07-2024 G_INT32    gk_RRDSASL_Link(GS_RRDS *prrds, GS_SCB *psignal);
// 15-07-2024 G_INT32    gk_RRDSASL_UnLink(GS_RRDS *prrds, GS_SCB *psignal);
// 15-07-2024 GS_SCB *gk_RRDSASL_GetSCB(GS_RRDS *prrds, G_INT32 SignalType);

GS_RRDS *gk_RRDS_GetFree(void);
G_INT32    gk_RRDSFL_Link(GS_RRDS *prrds);

G_INT32    gk_SCBFL_Link(GS_SCB *psignal);
GS_SCB *gk_SCB_GetFree(void);
GS_SCB *gk_SCB_Copy(GS_SCB *psignal);

void    gk_SCBAPSL_Link(GS_SCB *pscb_root, GS_SCB *pscb_pending);
G_INT32   gk_SCBAPSL_UnLink(GS_SCB *pscb_root, GS_SCB *pscb_pending);

GS_TCB *gk_TCB_GetFree(unsigned int stack_size);
G_INT32    gk_TCB_Unlink(GS_TCB *ptcb);

void    gk_TCBFL_Init(void);
G_INT32    gk_TCBFL_Link(GS_TCB *ptcb);

G_INT32    gk_TCBAEL_Link(GS_ECB *pevent, GS_TCB *ptcb);
G_INT32    gk_TCBAEL_Unlink(GS_ECB *pevent);

G_INT32    gk_TCBASL_Link(GS_TCB *ptcb, GS_SCB *psignal);
// 19-06-2024 G_INT32 gk_SCB_Link(GS_SCB **root, GS_SCB *psignal, enum scbstate State, G_INT32 offset_priority);
G_INT32    gk_TCBASL_Unlink(GS_TCB *ptcb, GS_SCB *psignal);
GS_SCB *gk_TCBASL_GetSCB(GS_TCB *ptcb, G_INT32 SignalType);
G_INT32    gk_TCB_List_Unlink(GS_TCB *ptcb);
G_INT32   gk_ECB_List_Unlink(GS_ECB *pevent);
G_INT32   gk_ECBFL_Link(GS_ECB *pevent);

G_INT32    gk_TCBPSL_Link(GS_TCB *ptcb, GS_SCB *psignal);
G_INT32    gk_TCBPSL_Unlink(GS_TCB *ptcb, GS_SCB *psignal);
GS_SCB *gk_TCBPSL_GetSCB(GS_TCB *ptcb, G_INT32 SignalType);

G_INT32    gk_TCBRDYL_Link(GS_TCB * const ptcb);
G_INT32    gk_TCBRDYL_Unlink(GS_TCB *ptcb);

G_INT32    gk_TCBRUNL_Link(GS_TCB *ptcb);
G_INT32    gk_TCBRUNL_Unlink(GS_TCB *ptcb);
G_INT32 gk_NODE_Link(void **root, void *const ptcb, enum list_type list_name);
G_INT32 gk_NODE_DUAL_Unlink(void **root, void * const pevent, G_INT32 const offset_next, G_INT32 const offset_prev);
G_INT32 gk_NODE_SINGLE_Unlink(void **root, void *psignal, G_INT32 const offset_next);

G_INT32    gk_TCBWL_Link(GS_TCB *ptcb, unsigned int state);
G_INT32    gk_TCBWL_Unlink(GS_TCB *ptcb);

/***************************************************************/
/*         TASK RELATED FUNCTIONS                              */
/***************************************************************/
// GS_TCB *gk_TASK_Create(void);
G_INT32    gk_TASK_Kill(GS_TCB *ptcb);
void    gk_KERNEL_TASK_SUSPEND(GS_TCB *ptcb);
void    gk_KERNEL_TASK_SUSPEND_CURRENT(enum tcbstate TCBSTATE);
G_INT32   gk_TASK_IS_BLOCKED(GS_TCB *ptcb);
GS_ECB *gk_TCB_in_RCBGEL(G_RCB *presource, GS_TCB *ptcb);
G_INT32    gk_TASK_PRIORITY_SET_CALLBACK(GS_TCB *ptcb, G_INT32 task_state);
G_INT32 gk_START_TASK_CALLBACK(GS_TCB *ptcb, G_INT64 ticks_offset);
G_INT64   gk_TASK_GRANTED_PRIORITY_GET(GS_TCB *ptcb);

// !!!!D 14-10-2024 G_INT32 get_CPU_ID(void);

/***************************************************************/
/*  INTERNAL ROUTINES FOR TASK EXECUTION                       */
/***************************************************************/
void gk_KERNEL_TASK_START (void);

void gk_START_KERNEL (void) __attribute__ ((destructor));
void gk_INIT_KERNEL(void)  __attribute__ ((constructor));

void gk_ENTRY_IRQ_HANDLER (void);
void GRTOS_Start_Task(void);
void grtos_irq_entry(void);
void GRTOS_Change_SP_to_IDLE(void);
void GRTOS_jump_to_reset(void);
void GRTOS_jump_to_reset_end(void);
volatile void GRTOS_Suspend_Task(void);
void gk_IRQ_HANDLER (void);                    
G_INT32 gk_TASK_STK_Init(GS_TCB *ptcb);
void gk_CRITICAL_SECTION_HIGHEST_PRIORITY_PROCESSOR_CALLBACK(void);
void gk_TIME_CALLBACK(GS_ECB * const event);
void gk_RESOURCE_ECB_KILL_CALLBACK(GS_ECB *pevent);
void gk_FROZEN_CALLBACK(void);



void gk_ENTRY_TASK_COMPLETE(void);
void  gk_KERNEL_TASK_COMPLETE(void);
void gk_TCB_GetPendingSCB(void);
G_INT32 gu_SchedulingListExclusionSet(GS_LCB *plcb, G_INT32 exclusion);

/***************************************************************/
/*  SYSTEM TASKS                                               */
/***************************************************************/
void gk_ENTRY_SIGNAL_RETURN(void);
void gk_CODE_IDLE_TASK(void* pdata);
void gk_WAIT_IDLE_TASK(void);
void gk_restore(void);



G_INT32  gu_TaskDelay(G_INT32 hours, G_INT32 minutes, G_INT32 seconds, G_INT32 ms);
G_INT32  gu_TaskDelayTime(gt_time ticks);
GS_TCB *gu_TaskGetCurrentTCB(void);

/**
 *  Default setting functions
 */
G_INT64 gu_Convert_time(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms);
G_INT32 gu_TaskTypeSet(struct gs_tcb *ptcb, enum tcbtype type);
G_INT32 gu_SchedulingListAssociateTask(struct gs_tcb *ptcb, struct gs_lcb *plcb);
G_INT32 gu_TaskReadyPrioritySet(struct gs_tcb *ptcb, G_INT64 priority);
G_INT32 gu_TaskRunPrioritySet(struct gs_tcb *ptcb, G_INT64 priority);
G_INT32 gu_TaskPeriodSet(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms);
G_INT32 gu_TaskStartWithOffset(GS_TCB *ptcb,unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms);


/**
 * Task related functions 
 */

// void *gu_TaskCreate(void *TaskCode, void *p_arg, char *task_description);
void *gu_TaskCreate(void *TaskCode, void *p_arg, char *format, ...);

G_INT32   gu_TaskKill(GS_TCB *ptcb);
G_INT32   gu_TaskSuspend(GS_TCB *ptcb);
G_INT32   gu_TaskResume(GS_TCB *ptcb);
G_INT32   gu_TASK_IS_BLOCKED(GS_TCB *ptcb);


/**
 * Signal related functions
 */
GS_SCB *gu_SignalCreate(enum scbtype Type, G_INT32 Priority, void *pxcb, void *Signal_Code, void *Signal_Arg);
G_INT32      gu_SignalDestroy(GS_SCB *pscb);

/**
 * User functions for system status and control
 */
// 10-06-2024 G_INT32    gu_get_irq_status(void);
G_INT32    gu_get_reserved_mutex_processor(void);
// 21-10-2024 gt_time gu_get_frozen_threshold(void);


// *************************************************** //
// *************************************************** //
// SYSTEM CALL TEMPLATE
//void  gu_TaskDelayTime(gt_time ticks)
//{
//    gm_GeMRTOSCriticalSectionEnter;
//    
//        <PREVIOUS SUSPEND PROCEDURE >
//    
//	gk_KERNEL_TASK_SUSPEND_CURRENT();
//    
//        <AFTER SUSPEND PROCEDURE >
//        
//	GRTOS_CMD_CRITICAL_SECTION_RELEASE;
//}
// *************************************************** //
// *************************************************** //

/**
 *  Monitor functions
 */
void gk_MONITOR_FIFO_SAMPLE(int data);
G_INT32 gkm_Check_GeMRTOS(void);
G_INT32 gkm_Check_list_LCBL_IsValid(int cycles);
G_INT32 gkm_Check_PCBAssocLCBs(G_INT32 partial);
G_INT32 gkm_Check_LCBs(G_INT32 partial);
G_INT32 gkm_Check_RCBs(G_INT32 partial);
G_INT32 gkm_Check_RRDSs(G_INT32 partial);
G_INT32 gkm_Check_PCBs(G_INT32 partial);
G_INT32 gkm_Check_ECBs(G_INT32 partial);
G_INT32 gkm_Check_TCBs(G_INT32 partial);
G_INT32 gkm_Check_Testing(G_INT32 partial);


/**
 *   newlib FUNTIONS
 */
void gu_printf(char *format, ...);
void gu_fprintf(char *format, ...);

int __wrap_printf(const char *__format, ...);
struct _reent *__getreent_gemrtos(void);

int newlib_write(int file, const void *ptr, size_t len);
int newlib_read(int file, void *ptr, size_t len);
// caddr_t newlib_sbrk(int incr);

// void *  newlib_sbrk (ptrdiff_t incr);
// caddr_t newlib_sbrk (int incr);




#endif /* GEMRTOS_CORE_H_ */
