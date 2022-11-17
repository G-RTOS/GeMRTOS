/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GEmRTOS CORE definitions
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
typedef struct g_rgb       G_RCB;
typedef struct gs_mcb      GS_MCB;
typedef struct gs_pcb_rdy_lcbl GS_PCBAssocLCB;
typedef struct g_rgb       t_semaphore_resource;
typedef union timepriority TIMEPRIORITY;

/* This is the definition for Nios32.  */
typedef unsigned long long INT64;
typedef unsigned           INT32;       /* Unsigned 32 bit quantity      */
typedef unsigned int       GS_STK;      /* Type to Stack Pointers        */

typedef unsigned long long gt_time;
typedef unsigned long long gt_priority;

union timepriority {
    INT64 i64  __attribute__((aligned(4)));
    INT32 i32[2];
};

typedef struct gs_tm {
	int	tm_msec;	/* Seconds: 0-59 (K&R says 0-61?) */
	int	tm_sec;		/* Seconds: 0-59 (K&R says 0-61?) */
	int	tm_min;		/* Minutes: 0-59 */
	int	tm_hour;	/* Hours since midnight: 0-23 */
	int	tm_day;     /* Day of the month: 1-31 */
	int	tm_year;	/* Years since 1900 */
} gt_tm;

/**
 *  gs_ext_isr Struct to hold the actions associated  with external events. They are hold in g_kcb structure
 */
struct gs_ext_isr {
    struct gs_tcb *G_TCB_ISR;        /** Pointers to the first ISR TCBs for each interrupt number */
    INT32         G_EXT_INT_Count;  /** Holds the number of ISR released, when 0 enable it again */
    gt_priority   G_EXT_ISR_Timeout; /** Holds the timeout of the interrupts                      */
};

/************************************************************************************
 *  GeMRTOS DATA STRUCTURES
 ************************************************************************************/

//*************************************************************************************************
/// \defgroup ECBState gs_ecb::ECBState 
/// \defgroup ECBType  gs_ecb::ECBType 

/// GS_ECB  Event Control Block structure
/**
 *  \brief gs_ecb  Event Control Block structure (EBC)
 *  \details The gs_ecb holds information about events. 
 *  \todo Check if all the states and type are needed
 */
struct gs_ecb {
    unsigned int   BLOCK_HASH;         ///< \brief BLOCK_HASH of the ECB: (GS_ECB *) + 2
	INT32          ECBState;           ///< \brief Granted, Waiting, Free                                \ingroup ECBState
	INT32          ECBType;            ///< \brief Type of event control block                           \ingroup ECBType
    void           *malloc_address;    ///< \brief Pointer memory address of the malloc block         
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
    struct gs_ecb  *ECB_NEXT_ECBs;     ///< \brief Pointer to next TCB structure. Used for debugging purposes.
    struct gs_ecb  *ECB_PREV_ECBs;     ///< \brief Pointer to previous TCB structure. Used for debugging purposes.
};

/// ECBState valid values
#define  GS_ECBState_GRANTED_RESOURCE            1u      ///< \brief State event for Resource GRANTED       \ingroup ECBState
#define  GS_ECBState_WAITING_RESOURCE            2u      ///< \brief State event for Resource WAITING       \ingroup ECBState
#define  GS_ECBState_FREE                        3u      ///< \brief State for FREE event                   \ingroup ECBState
#define  GS_ECBState_WAITING_TIME                4u      ///< \brief State for TIMED EVENT                  \ingroup ECBState
#define  GS_ECBState_UNLINKED                    5u      ///< \brief State for UNLINKED event               \ingroup ECBState

/// ECBType valid values
#define  G_ECBType_OSTimeDly                   21u       ///< \brief Type for delay event                                  \ingroup ECBType
#define  G_ECBType_PERIODIC                    22u       ///< \brief Type for PERIODIC event                               \ingroup ECBType
#define  G_ECBType_LASTEST_TIME	               23u       ///< \brief Type for LASTEST TIME event at the end of time list   \ingroup ECBType
#define  G_ECBType_TIMEOUT_SEM_GRANTED         24u       ///< \brief Type for timeout granted semaphore                    \ingroup ECBType
#define  G_ECBType_TIMEOUT_SEM_WAITING         25u       ///< \brief Type for timeout waiting semaphore                    \ingroup ECBType
#define  G_ECBType_SEM_GRANTED                 28u       ///< \brief Type for semaphore granted event                      \ingroup ECBType
#define  G_ECBType_SEM_WAITING                 29u       ///< \brief Type for semaphore waiting event                      \ingroup ECBType
#define  G_ECBType_NOT_SPECIFIED               30u       ///< \brief Type for UNSPECIFIED event                            \ingroup ECBType
#define  G_ECBType_QUEUE_CONSUME               31u       ///< \brief Type for MESSAGE QUEUE consume event                  \ingroup ECBType
#define  G_ECBType_MESSAGE_CONSUME_WAIT        33u       ///< \brief Type for WAITING TIMEOUT of MQ consumer               \ingroup ECBType
#define  G_ECBType_MESSAGE_CONSUME_EXPIRED     34u       ///< \brief Type for MQ consumer when waiting timeout expired     \ingroup ECBType
#define  G_ECBType_MESSAGE_SEND_EXPIRED        35u       ///< \brief Type for WAITING TIMEOUT of MQ sender                 \ingroup ECBType
#define  G_ECBType_MESSAGE_SEND_WAIT           36u       ///< \brief Type for MQ sender when waiting timeout expired       \ingroup ECBType


//*************************************************************************************************
/// GS_KCB  Kernel Control Block structure
/**
 *  \brief gs_kcb  Kernel Control Block structure (LCB)
 *  \details The gs_kcb holds information of the system. 
  */
struct gs_kcb {
	struct gs_ecb     *KCB_NextECBTL;        ///< \brief Pointer to EVENT LIST of the Next Absolute Time 
	struct gs_tcb     *KCB_NextTCBWL;        ///< \brief Pointer to the linked list of the TCB of Waiting Tasks
	struct gs_lcb     *KCB_NextLCBL;         ///< \brief Pointer to the lowest priority run list
	struct gs_scb     *KCB_NextKCBASL;       ///< \brief Pointer to Signal Associated List for KCB
    struct gs_ext_isr KCB_ExtISR[ALT_NIRQ] __attribute__((aligned(4)));  ///< \brief Pointers to ISR TCBs for each interrupt number
    
    GS_PCB  *G_PCBTbl __attribute__((aligned(4)));                                /// \brief Pointer to PCBs dynamic array
    
    INT32             KCB_NUMBER_OF_TCBs;           ///< \brief Number of TCB in the system 
    INT32             KCB_NUMBER_OF_PCBs;           ///< \brief Number of PCB in the system 
    INT32             KCB_NUMBER_OF_ECBs;           ///< \brief Number of ECB in the system 
    INT32             KCB_NUMBER_OF_RCBs;           ///< \brief Number of RCB in the system 
    INT32             KCB_NUMBER_OF_LCBs;           ///< \brief Number of LCB in the system 
    INT32             KCB_NUMBER_OF_SCBs;           ///< \brief Number of SCB in the system 
    INT32             KCB_NUMBER_OF_RRDSs;          ///< \brief Number of RRDS in the system 
    INT32             KCB_NUMBER_OF_PCBAssocLCBs;   ///< \brief Number of RRDS in the system
    
    /// Pointers to free structures to avoid free and malloc frequently
    /// Free structures are linked to this pointers instead of free the memory
    /// New create structures pop from this list or malloc if it is empty
    struct gs_tcb    *KCB_FREE_TCBs;        ///< \brief pointer to the first free TCB structure.
    struct gs_ecb    *KCB_FREE_ECBs;        ///< \brief pointer to the first free ECB structure.
    struct g_rcb     *KCB_FREE_RCBs;        ///< \brief pointer to the first free RCB structure.
    struct gs_lcb    *KCB_FREE_LCBs;        ///< \brief pointer to the first free LCB structure.
    struct gs_scb    *KCB_FREE_SCBs;        ///< \brief pointer to the first free SCB structure.    
    struct gs_rrds   *KCB_FREE_RRDSs;       ///< \brief pointer to the first free RRDS structure.
    struct gs_pcb_rdy_lcbl *KCB_FREE_RDYs;  ///< \brief pointer to the first gs_pcb_rdy_lcbl structure. Used for debugging purposes.    
    
    /// IDLE and ISR tasks    
    volatile INT32   *G_ISR_STACK;
    volatile GS_STK  *G_IDLE_STACK;
    
    /// roots for linked list of data structures
    struct gs_tcb    *KCB_ROOT_TCBs;        ///< \brief pointer to the first TCB structure. Used for debugging purposes.
    struct gs_ecb    *KCB_ROOT_ECBs;        ///< \brief pointer to the first ECB structure. Used for debugging purposes.
    struct g_rcb     *KCB_ROOT_RCBs;        ///< \brief pointer to the first RCB structure. Used for debugging purposes.
    struct gs_lcb    *KCB_ROOT_LCBs;        ///< \brief pointer to the first LCB structure. Used for debugging purposes.
    struct gs_scb    *KCB_ROOT_SCBs;        ///< \brief pointer to the first SCB structure. Used for debugging purposes.
    struct gs_rrds   *KCB_ROOT_RRDSs;       ///< \brief pointer to the first RRDS structure. Used for debugging purposes.
    struct gs_pcb_rdy_lcbl *KCB_ROOT_RDYs;  ///< \brief pointer to the first gs_pcb_rdy_lcbl structure. Used for debugging purposes.
};

//*************************************************************************************************
/// \defgroup LCBState gs_lcb::LCBState 
/// \defgroup LCBType  gs_lcb::LCBType 

/// GS_LCB List Control Block
/**
 *  \brief gs_lcb  List Control Block structure (LCB)
 *  \details The gs_lcb holds information about processor. 
 *  \todo Check correctness for multiple list scheduling
 *  \todo define types
 *  \todo Include maximun number of running tasks and current running tasks to control exclusion
 */
struct gs_lcb {
    unsigned int  BLOCK_HASH;           ///< \brief BLOCK_HASH of the LCB: (GS_LCB *) + 6
    INT32         LCBState;             ///< \brief State of the List Control Block                         \ingroup LCBState 
    INT32         LCBType;              ///< \brief Type of the List Control Block                          \ingroup LCBType
    void          *malloc_address;      ///< \brief Pointer memory address of the malloc block         
	INT64         LCBRunPriority;      ///< \brief Current Priority (-1 PRIORITY THERE IS NO TASK RUNNING
    INT32         LCBCurrentRunning;   ///< \brief Current number of running tasks
    INT32         LCBExclusion;        ///< \brief Maximum number of running task (0 for no limit)
	struct gs_tcb *LCB_NextTCBRUNL;    ///< \brief Pointer to the TCB list of running tasks 
	struct gs_tcb *LCB_NextTCBRDYL;	   ///< \brief Pointer to the TCB of the Highest Priority Task 
	struct gs_lcb *LCB_NextLCBL;       ///< \brief Pointer to the next list ordered by priority 
	struct gs_lcb *LCB_PrevLCBL;       ///< \brief Pointer to the next list ordered by priority 
	struct gs_pcb *LCB_NextLCBFPL;     ///< \brief Next free processor for this list
    
    /// Fields for debugging
    struct gs_lcb  *LCB_NEXT_LCBs;     ///< \brief Pointer to next LCB structure. Used for debugging purposes.
    struct gs_lcb  *LCB_PREV_LCBs;     ///< \brief Pointer to previous LCB structure. Used for debugging purposes.    
};

/** State of the List Control Block */
#define  GS_LCBState_LINKED                   1u  ///< \brief State LINKED for LIST                  \ingroup LCBState
#define  GS_LCBState_UNLINKED                 2u  ///< \brief State UNLINKED for LIST                \ingroup LCBState

#define  GS_LCBType_UNSPECIFIED               2u  ///< \brief State UNLINKED for LIST                \ingroup LCBState
#define  GS_LCBType_EDF                       3u  ///< \brief List is a EDF schedule one             \ingroup LCBState
#define  GS_LCBType_FP                        4u  ///< \brief List is a FP schedule one              \ingroup LCBState

//*************************************************************************************************
/// \defgroup PCBState g_rgb::PCBState 
/// \defgroup PCBType  g_rgb::PCBType 

/// gs_pcb_rdy_lcbl Processor associated LCB list
/**
 *  \brief gs_pcb_rdy_lcbl  structure to associate a PCB to a LCB
 *  \details The gs_pcb holds a linked list of gs_pcb_rdy_lcbl with each one of the LCB associated. 
 */
struct gs_pcb_rdy_lcbl {
    unsigned int           BLOCK_HASH;               ///< \brief BLOCK_HASH of the GS_PCBAssocLCB: (GS_PCBAssocLCB *) + 7
    void                   *malloc_address;          ///< \brief Pointer memory address of the malloc block     
    struct gs_pcb_rdy_lcbl *gs_pcb_rdy_lcbl_next;
    struct gs_pcb_rdy_lcbl *gs_pcb_rdy_lcbl_prev;
    INT32                  priority;
    struct gs_lcb          *PCB_RDY_LCBL;
    
    /// Fields for debugging
    struct gs_pcb_rdy_lcbl *gs_pcb_lcbl_nexts;
    struct gs_pcb_rdy_lcbl *gs_pcb_lcbl_prevs;
};


/// GS_PCB Processor Control Block
/**
 *  \brief gs_pcb  Processor Control Block structure (PCB)
 *  \details The gs_pcb holds information about processor. 
 *  Each processors has an unique PCBID according to the ID of the associated processor.
 *  \todo Define the type for processors
 */
struct gs_pcb {
	INT32 PCBID;                                ///< \brief Processor ID 
	int PCBState;                               ///< \brief State of the PCB : GS_FREE_PROCESSOR, GS_RUNNING_PROCESSOR  \ingroup PCBState
    int PCBType;                                ///< \brief Type of the processor                                       \ingroup PCBType
    int *GRTOS_PROCESSOR_BASE __attribute__((aligned(4))); /// \brief Processor specific addresses to go to idle state
    struct gs_pcb_rdy_lcbl *PCB_AssocLCB;       ///< \brief Linked list of the associated LCBs that processor serves
	struct gs_tcb *PCB_IDLETCB;                 ///< \brief Pointer to the TCB of the IDLE Task 
	struct gs_tcb *PCB_EXECTCB;                 ///< \brief Current task assigned to processor 
	struct gs_pcb *PCB_NextPCB;                 ///< \brief Pointer to the next free processor 
	struct gs_pcb *PCB_PrevPCB;                 ///< \brief Pointer to the previous free processor 
};

/// PCBState valid values
#define  GS_PCBState_NOTRUNNING           1u    ///< \brief It is NOTRUNNING at starting time                      \ingroup PCBState
#define  GS_PCBState_RUNNING              2u    ///< \brief It is RUNNING when executing a main list task (LCB[0]) \ingroup PCBState 
#define  GS_PCBState_FREE                 3u    ///< \brief It is FREE when not executing a main list task         \ingroup PCBState

#define  GS_PCBType_UNSPECIFIED           1u    ///< \brief It is FREE when not executing a main list task         \ingroup PCBState



//*************************************************************************************************
/// \defgroup SCBState gs_scb::SCBState 
/// \defgroup SCBType  gs_scb::SCBType 

/// GS_SCB  Signal Control Block structure
/**
 *  \brief gs_scb  Signal Control Block (SCB) structure
 *  \details The gs_scb structure defines the pending and associated signals for tasks, events, resources and kernal.
 *  \todo define the rest of signal types required. 
  */
struct gs_scb {
    unsigned int  BLOCK_HASH;               ///< \brief BLOCK_HASH of the SCB: (GS_SCB *) + 4
	INT32         SCBState;                 ///< \brief STATE of the signal  \ingroup SCBState  
	INT32         SCBType;                  ///< \brief TYPE of the signal   \ingroup SCBType
    void          *malloc_address;          ///< \brief Pointer memory address of the malloc block            
    INT32         SCBPriority;              ///< \brief Priority of the SCB when it is linked
    void          *SCB_TaskCode;            ///< \brief Pointer to the code of the signal 
    void          *SCB_TaskArg;             ///< \brief Pointer to the argument of the signal 
    struct        gs_scb *SCB_NextSCB;      ///< \brief Pointer to the next SCB linked
    struct        gs_scb *SCB_NextSCBAPSL;  ///< \brief Pointer to the next pending SCB lined
    void          *SCB_AssocXCB;            ///< \brief Pointer to the data structure root of the SCBASL
    
    /// Fields for debugging
    struct gs_scb  *SCB_NEXT_SCBs;     ///< \brief Pointer to next SCB structure. Used for debugging purposes.
    struct gs_scb  *SCB_PREV_SCBs;     ///< \brief Pointer to previous SCB structure. Used for debugging purposes.    
};

// SCBState gruop
/// \brief SCBState is state of the SCB
#define  G_SCBState_FREE          1u         ///< \brief State FREE of the SCB        \ingroup SCBState
#define  G_SCBState_IN_KCB        2u         ///< \brief State SCB linked to KCB      \ingroup SCBState 
#define  G_SCBState_IN_ECB        3u         ///< \brief State SCB linked to ECB      \ingroup SCBState
#define  G_SCBState_IN_TCB        4u         ///< \brief State SCB linked to TCB      \ingroup SCBState
#define  G_SCBState_IN_RCB        5u         ///< \brief State SCB linked to RCB      \ingroup SCBState
#define  G_SCBState_IN_PCB        6u         ///< \brief State SCB linked to PCB      \ingroup SCBState
#define  G_SCBState_IN_LCB        7u         ///< \brief State SCB linked to LCB      \ingroup SCBState
#define  G_SCBState_UNLINKED      8u         ///< \brief State UNLINKED               \ingroup SCBState
#define  G_SCBState_PENDING       9u         ///< \brief State PENDING                \ingroup SCBState
#define  G_SCBState_EXECUTING    10u         ///< \brief State EXECUTING              \ingroup SCBState

// SCBType group
/// \brief SCBType valid values 
/// \todo Define the rest of SCB types
#define G_SCBType_TCB_ABORTED          1000u      ///< TYPE "Periodic task aborted when released before deadline"      \ingroup SCBType


//*************************************************************************************************
/// \defgroup TCBState gs_tcb::TCBState
/// \defgroup TCBType  gs_tcb::TCBType 

/**
 *  \brief gs_tcb  Task Control Block (TCB) structure
 *  \details The gs_tcb structure has all the fields to hold the task parameters.
 *  \todo Check if fields can be grouped into an union to save fields for differetn type of TCBs. 
 *  For instance, period and ISR links are not requiered for the same type of task.
 */
struct gs_tcb 
{
    unsigned int  BLOCK_HASH;             ///< \brief BLOCK_HASH of the TCB: (GS_TCB *) + 1 
    unsigned int  TCBState;               ///< \brief STATE of the task  \ingroup TCBState 
    unsigned int  TCBType;                ///< \brief TYPE of the task   \ingroup TCBType
    void *malloc_address;                 ///< \brief Pointer memory address of the malloc block    
    INT64  TCBReadyPriority;              ///< \brief Priority of the Task when Ready
    INT64  TCBInherPriority;              ///< \brief Priority Inhered by granted resource
    INT64  TCBRunPriority;                ///< \brief Priority when it is executed
    INT64  TCBCurrentPriority;            ///< \brief Current priority of the task according it is ready or running
	INT64  TCBPeriod;                     ///< \brief Period of the task
	INT64  TCBDeadline;                   ///< \brief Deadline of the task
    GS_STK *TCB_StackPointer;             ///< \brief Pointer to current top of stack
    GS_STK *TCB_StackBottom;              ///< \brief Botton Stack of the Task
    GS_STK *TCB_StackTop;                 ///< \brief Botton Stack of the Task
    void   *TCB_TaskCode;                 ///< \brief Pointer to the Task Code
	void   *TCB_TaskArg;                  ///< \brief Pointer to the argument of the first call
    volatile struct gs_tcb *TCB_NextTCB;  ///< \brief Pointer to next TCB in the TCB list
    struct gs_tcb *TCB_PrevTCB;           ///< \brief Pointer to previous TCB in the TCB list
    struct gs_ecb *TCB_NextTCBAEL;        ///< \brief Pointer to linked list of waiting events of this task
    struct gs_scb *TCB_NextTCBASL;        ///< \brief Pointer to the Linked list of signals
    struct gs_scb *TCB_NextTCBPSL;        ///< \brief Pointer to signals waiting to execute
    unsigned int TCB_AssocPCB;            ///< \brief Processor assigned this task (0 is no assigned)
	int TCB_INTNumber;                    ///< \brief IRQ number if it is a ISR TCB
	struct gs_lcb *TCB_RDY_LCB_Index;     ///< \brief pointer to the ready list that should be inserted
	int TCB_Abort_w_Deadline;             ///< \brief True if task should be aborted when deadline, false otherwise
    struct gs_tcb *TCB_NextISRTCB;        ///< \brief Pointer to the next TCB for the same ISR (when task is a ISR)
    struct gs_tcb *TCB_PrevISRTCB;        ///< \brief Pointer to the previous TCB for the same ISR (when task is a ISR)
    
    /// Fields for debugging
    struct gs_tcb  *TCB_NEXT_TCBs;     ///< \brief Pointer to next TCB structure. Used for debugging purposes.
    struct gs_tcb  *TCB_PREV_TCBs;     ///< \brief Pointer to previous TCB structure. Used for debugging purposes.         
};

// TCBState gruop
/// \details The TCBState is according to bits (starts in bit 0): bit 1 -> WAITING, 2 ->READY, 3 -> RUNNING, 4 ->FREE, 5->UNLINKED
#define  GRTOS_TASK_STATE_WAITING(ptcb)  ((ptcb->TCBState >> 1) & 1) ///< Returns True if task is waiting  \ingroup TCBState

#define  G_TCBState_WAITING_COMPLETED           2u   ///< \brief State WAITING, task COMPLETED     \ingroup TCBState 
#define  G_TCBState_WAITING                   258u   ///< \brief State WAITING, but NOT COMPLETED  \ingroup TCBState
#define  G_TCBState_READY                       4u   ///< \brief State READY                       \ingroup TCBState
#define  G_TCBState_RUNNING                     8u   ///< \brief State RUNNING                     \ingroup TCBState
#define  G_TCBState_FREE                       16u   ///< \def G_TCBState_FREE \brief State FREE                        \ingroup TCBState
#define  G_TCBState_UNLINKED                   32u   ///< \brief State UNLINKED                    \ingroup TCBState

// TCBType group
/// \brief TCBType valid values
#define G_TCBType_UCOS                          1u   ///< \brief Task type UCOS "one run only" (with optional infinite loop)           \ingroup TCBType
#define G_TCBType_PERIODIC                      2u   ///< \brief Task type PERIODIC "with periodic release"                            \ingroup TCBType
#define G_TCBType_ISR                           4u   ///< \brief Task type ISR. It is released for an interrupt event                  \ingroup TCBType
#define G_TCBType_IDLE                          5u   ///< \brief Task type IDLE. It is executed when processor is not assigned to task \ingroup TCBType
#define G_TCBType_UNDEFINED                     7u   ///< \brief Task type UNDEFIEND. Wehn not a specific type is given to the task    \ingroup TCBType

/*
*********************************************************************************************************
*    SYSTEM VARIABLES
*********************************************************************************************************
*/


extern volatile int G_DEBUG_SAMPLE_BEGIN_ENABLE;
extern volatile int G_DEBUG_SAMPLE_END_ENABLE;

extern volatile INT32  G_Running;      
extern volatile INT32  gs_sizeof_G_PCBTbl;
extern volatile void   *gs_addressof_G_PCBTbl;
extern volatile INT32  gs_offsetof_PCB_EXECTCB;
extern volatile INT32  gs_offsetof_TCB_StackPointer;
extern volatile INT32  gs_offsetof_PCB_IDLETCB;


extern volatile GS_KCB g_kcb;

//Variable to interface C and assembler
extern volatile GS_STK G_TCB_CURRENT;
extern volatile INT32  G_SCB_PENDING;
extern volatile INT32  G_SCB_CODE;
extern volatile INT32  G_SCB_ARG;

// extern volatile int GRTOS_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID;
extern volatile int GRTOS_S_PROCESSOR1_IRQ;

extern volatile int G_IRQ_ENABLED;
extern volatile int G_IRQ_PENDING;

extern volatile INT32 GRTOS_MutexReleaseRegisterAddress;
extern volatile INT32 GRTOS_MutexRequestRegisterAddress;
extern volatile INT32 GRTOS_InterruptEnableRegisterAddress;
extern volatile INT32 GRTOS_InterruptDisableRegisterAddress;  

extern FILE* fpuart[G_NUMBER_OF_PCB];

extern volatile INT32 G_TASK_TYPE_DEFAULT;
extern volatile GS_LCB *G_TASK_LCB_DEFAULT;
extern volatile INT32 G_TASK_LCB_DEFAULT_PRIORITY;
extern volatile INT64 G_TASK_PRIORITY_DEFAULT;
extern volatile INT64 G_TASK_PERIOD_DEFAULT;

extern volatile unsigned int new_lib_grant;
extern volatile unsigned int new_lib_counter;

/// Core functions definitions
GS_ECB *gk_Get_ECB(void);
G_RCB  *gk_Get_RCB(void);
GS_SCB *gk_Get_SCB(void);
GS_RRDS *gk_Get_RRDS(void);
GS_LCB *gk_Get_LCB(INT32 lcbtype);
INT32 gk_Create_PCBs(int Nmbr_PCB);
GS_PCBAssocLCB *gk_Get_PCBAssocLCB(void);
INT32 gk_PCBAssocLCBFL_Link(GS_PCBAssocLCB *ppcbalcb);
INT32 gk_LCB_Associate_PCB(GS_LCB *plcb, INT32 CPUID, INT32 priority);



/***************************************************************/
/* INTERNAL SYSTEM FUNCTIONS                                   */
/***************************************************************/
gt_time gu_get_mutex_time(void);
INT32     gk_TCBLowerPriorityThanTCB(GS_TCB *ptcb1, GS_TCB *ptcb2);
INT32   gk_LCB_CheckInvertion(void);
INT32    gk_SetLowestProcessor(void);
INT32    gk_SetNextTimeProcessor(void);
INT32     gk_LCBLowerPriorityThanLCB(GS_LCB *plcb1, GS_LCB *plcb2);
GS_TCB *gk_PCB_GetNextTCB(void);
GS_TCB *gk_PCB_GetCurrentTCB(void);

/***************************************************************/
/* debug function                                              */
/***************************************************************/
INT32 TCB_IsValid(GS_TCB *ptcb);
INT32 GS_PCBAssocLCB_IsValid(GS_PCBAssocLCB *ppcbalcb);

INT32 LCB_IsValid(GS_LCB *plcb);

INT32 PCB_IsValid(GS_PCB *ppcb);
INT32 ECB_IsValid(GS_ECB *pevent);
INT32 SCB_IsValid(GS_SCB *pscb);
INT32 RCB_IsValid(void *prcb);
INT32 RRDS_IsValid(GS_RRDS *prrds);

INT32 TCBState_Valid(unsigned int tcbstate);
INT32 TCBType_Valid(unsigned int tcbtype);
INT32 ECBState_Valid(unsigned int ecbstate);
INT32 ECBType_Valid(unsigned int ecbtype);

INT32 IsAListorNull(GS_LCB *plcb);


/***************************************************************/
/*  LIST AND DATA STRUCTURES PROTOTYPES                        */
/***************************************************************/

INT32   gk_ECBAEL_Link(GS_ECB *pevent1, GS_ECB *pevent2);
INT32   gk_ECBAEL_Remove(GS_ECB *pevent);

void    gk_ECBASL_Link(GS_ECB *pevent, GS_SCB *psignal);
INT32   gk_ECBASL_Unlink(GS_ECB *pevent, GS_SCB *psignal);
GS_SCB *gk_ECBASL_GetSCB(GS_ECB *pevent, INT32 SignalType);

GS_ECB *gk_ECB_GetFree(void);
// INT32   gk_ECBFL_Link(GS_ECB *pevent);

extern INT32    gk_ECBTL_Link (GS_ECB *pevent);
extern INT32    gk_ECBTL_Unlink(GS_ECB *pevent);

INT32    gk_KCBASL_Link(GS_SCB *psignal);
INT32   gk_KCBASL_Unlink(GS_SCB *psignal);
INT32    gk_LCBL_Link(GS_LCB *plcb);
GS_SCB *gk_KCBASL_GetSCB(INT32 SignalType);

INT32    gk_LCBFPL_Link(int processorID);
INT32    gk_LCBFPL_Unlink(int processorID);

INT32    gk_LCBL_UnLink(GS_LCB *plcb);
//void    gk_LCB_Link_to_LCB_List(GS_LCB *plcb);

void    gk_PCBInit(void);

G_RCB  *gk_RCB_GetFree(void);
INT32    gk_RCBFL_Link(G_RCB *presource);

INT32 gk_TASK_RELEASE(GS_TCB *ptcb);
INT32 gk_TASK_RESOURCE_WAIT(G_RCB *presource, GS_ECB *pevent);
INT32 gk_TASK_RESOURCE_UNWAIT(G_RCB *presource, GS_ECB *pevent);
INT32 gk_TASK_RESOURCE_GRANT(G_RCB *presource, GS_ECB *pevent);
INT32 gk_TASK_RESOURCE_UNGRANT(G_RCB *presource, GS_ECB *pevent) ;
GS_ECB *gk_TASK_RESOURCE_CREATE(G_RCB *presource, 
                                GS_TCB  *ptcb, 
                                INT64 waiting_priority,
		                        INT64 RCBGrantedPriority,
			                    INT64 RCBWaitingTimeout,
				                INT64 RCBGrantedTimeout);
INT32 gk_TASK_RESOURCE_DESTROY(GS_ECB *pevent);

GS_ECB *gk_RCBGEL_Link(G_RCB *presource, GS_ECB *pevent);
void    gk_RCBGEL_Unlink(GS_ECB *pevent);

INT32    gk_RCBASL_Link(G_RCB *presource, GS_SCB *psignal);
INT32    gk_RCBASL_Unlink(G_RCB *presource, GS_SCB *psignal);
GS_SCB *gk_RCBASL_GetSCB(G_RCB *presource, INT32 SignalType);

GS_ECB *gk_RCBWEL_Link(G_RCB *presource, GS_ECB *pevent);
void    gk_RCBWEL_Unlink(GS_ECB *pevent);

INT32    gk_RRDSASL_Link(GS_RRDS *prrds, GS_SCB *psignal);
INT32    gk_RRDSASL_UnLink(GS_RRDS *prrds, GS_SCB *psignal);
GS_SCB *gk_RRDSASL_GetSCB(GS_RRDS *prrds, INT32 SignalType);

GS_RRDS *gk_RRDS_GetFree(void);
INT32    gk_RRDSFL_Link(GS_RRDS *prrds);

INT32    gk_SCBFL_Link(GS_SCB *psignal);
GS_SCB *gk_SCB_GetFree(void);
GS_SCB *gk_SCB_Copy(GS_SCB *psignal);

void    gk_SCBAPSL_Link(GS_SCB *pscb_root, GS_SCB *pscb_pending);
INT32   gk_SCBAPSL_UnLink(GS_SCB *pscb_root, GS_SCB *pscb_pending);

GS_TCB *gk_TCB_GetFree(void);
INT32    gk_TCB_Unlink(GS_TCB *ptcb);

void    gk_TCBFL_Init(void);
INT32    gk_TCBFL_Link(GS_TCB *ptcb);

INT32    gk_TCBAEL_Link(GS_ECB *pevent, GS_TCB *ptcb);
INT32    gk_TCBAEL_Unlink(GS_ECB *pevent);

INT32    gk_TCBASL_Link(GS_TCB *ptcb, GS_SCB *psignal);
INT32    gk_TCBASL_Unlink(GS_TCB *ptcb, GS_SCB *psignal);
GS_SCB *gk_TCBASL_GetSCB(GS_TCB *ptcb, INT32 SignalType);
INT32    gk_TCB_List_Unlink(GS_TCB *ptcb);
INT32   gk_ECB_List_Unlink(GS_ECB *pevent);
INT32   gk_ECBFL_Link(GS_ECB *pevent);

INT32    gk_TCBPSL_Link(GS_TCB *ptcb, GS_SCB *psignal);
INT32    gk_TCBPSL_Unlink(GS_TCB *ptcb, GS_SCB *psignal);
GS_SCB *gk_TCBPSL_GetSCB(GS_TCB *ptcb, INT32 SignalType);

INT32    gk_TCBRDYL_Link(GS_TCB *ptcb);
INT32    gk_TCBRDYL_Unlink(GS_TCB *ptcb);

INT32    gk_TCBRUNL_Link(GS_TCB *ptcb);
INT32    gk_TCBRUNL_Unlink(GS_TCB *ptcb);

INT32    gk_TCBWL_Link(GS_TCB *ptcb, unsigned int state);
INT32    gk_TCBWL_Unlink(GS_TCB *ptcb);

/***************************************************************/
/*         TASK RELATED FUNCTIONS                              */
/***************************************************************/
// GS_TCB *gk_TASK_Create(void);
INT32    gk_TASK_Kill(GS_TCB *ptcb);
void    gk_KERNEL_TASK_SUSPEND(GS_TCB *ptcb);
void    gk_KERNEL_TASK_SUSPEND_CURRENT(void);
INT32   gk_TASK_IS_BLOCKED(GS_TCB *ptcb);
GS_ECB *gk_TCB_in_RCBGEL(G_RCB *presource, GS_TCB *ptcb);
INT32    gk_TASK_PRIORITY_SET_CALLBACK(GS_TCB *ptcb, INT32 task_state);
INT32 gk_START_TASK_CALLBACKK(GS_TCB *ptcb, INT64 ticks_offset);
INT64   gk_TASK_GRANTED_PRIORITY_GET(GS_TCB *ptcb);
INT32    gk_TASK_INHERENCE_PRIORITY_SET(GS_TCB * ptcb);



/***************************************************************/
/*  INTERNAL ROUTINES FOR TASK EXECUTION                       */
/***************************************************************/
// void gk_Start_Main(void);
void gk_KERNEL_TASK_START (void);
void gk_ENTRY_IRQ_HANDLER (void);
void GRTOS_Start_Task(void);
void grtos_irq_entry(void);
void GRTOS_Suspend_Task(void);
// void gk_INT_TASK_SWITCH(void);
GS_TCB *gk_CreateTask(void *TaskCode,              
                     void *p_arg,                  
                     void *StkBotton,              
                     unsigned int StkSize,         
                     unsigned int TCBType,         
                     INT64 TCBReadyPriority,       
                     INT64 TCBRunPriority,         
                     INT64 TCBDeadline,            
                     INT64 TCBPeriod,              
                     GS_LCB *TCB_RDY_LCB_Index,        
                     int TCB_Abort_w_Deadline,     
                     int TCB_INTNumber); 
                     
INT32 gk_TASK_STK_Init(GS_TCB *ptcb);
void gk_TIME_CALLBACK(GS_ECB *event);
void gk_RESOURCE_ECB_KILL_CALLBACK(GS_ECB *pevent);
void gk_FROZEN_CALLBACK(void);
void gk_INIT_KERNEL(void);
void gk_KERNEL_TASK_COMPLETE(void);
void GRTOS_Task_GetPendingSCB(void);
void gk_RST_MONITOR_HANDLER (void);
GS_TCB *gu_GetCurrentTCB(void);
INT64 gu_GetCurrentPriority(void);            

/***************************************************************/
/*  SYSTEM TASKS                                               */
/***************************************************************/
void gk_ENTRY_SIGNAL_RETURN(void);
void gk_CODE_IDLE_TASK(void* pdata);



INT32  gu_TASK_Sleep(INT32 hours, INT32 minutes, INT32 seconds, INT32 ms);
INT32  gu_TASK_Sleep_Time(gt_time ticks);

/**
 *  Function Headers for user usage
 */
int     gu_Get_CPU_ID(void);
gt_time gu_Get_Next_Occurrence_Time(void);
gt_tm   gu_Clock(gt_time gtime);

/**
 *  Default setting functions
 */
INT32 gu_Set_Default_Task_Type(unsigned int type);
INT32 gu_Get_Default_Task_Type(void);
INT32 gu_Set_Default_Task_List(GS_LCB * list);
INT32 gu_Get_Default_Task_List(void);
void gu_Set_Default_Task_Priority(INT32 level, INT32 priority);
void gu_Set_Default_Task_Period(INT32 hours, INT32 minutes, INT32 seconds, INT32 ms);
INT32 gu_SetTaskType(struct gs_tcb *ptcb, unsigned int type);
INT32 gu_SetTaskList(struct gs_tcb *ptcb, struct gs_lcb *plcb);
INT32 gu_SetTaskReadyPriority(struct gs_tcb *ptcb, long long priority);
INT32 gu_SetTaskRunPriority(struct gs_tcb *ptcb, long long priority);
INT32 gu_SetTaskDeadline(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms);
INT32 gu_SetTaskPeriod(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms);
INT32 gu_SetTaskAbortwhenDeadline(struct gs_tcb *ptcb, unsigned int abort_when_deadline);
INT32 gu_StartTaskwithOffset(struct gs_tcb *ptcb,unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms);


/**
 * Task related functions 
 */
 
void *gu_GetTask(void         *TaskCode,   
				 void         *p_arg,      
				 void         *StkBotton,  
                 unsigned int  StkSize);	
 

INT32   gu_TASK_Kill(GS_TCB *ptcb);
INT32   gu_TASK_SUSPEND(GS_TCB *ptcb);
INT32   gu_TASK_RESUME(GS_TCB *ptcb);
INT32   gu_TASK_IS_BLOCKED(GS_TCB *ptcb);


 

/**
 * Signal related functions
 */
GS_SCB *gu_signal_create(INT32 Type, INT32 Priority, void *pxcb, void *Signal_Code, void *Signal_Arg);
INT32      gu_signal_destroy(GS_SCB *pscb);

/**
 * User functions for system status and control
 */
gt_time  gu_Convert_Time(INT32 days, INT32 hours, INT32 minutes, INT32 seconds, INT32 ms);
INT32    gu_get_irq_status(void);
INT32    gu_get_reserved_mutex_processor(void);
gt_time gu_get_now(void);
gt_time gu_get_frozen_threshold(void);

void  gk_INIT_IRQ (void);
INT32 gk_ISR_COMPLETE (GS_TCB *ptcb);
void  gk_ISR_RELEASE (int irq_nbr);
INT32 gu_SetTaskISR(struct gs_tcb *ptcb, unsigned int irq_nbr);
INT32 gk_SetTaskISR(struct gs_tcb *ptcb, unsigned int irq_nbr);


// *************************************************** //
// *************************************************** //
//// SYSTEM CALL TEMPLATE
//void  gu_TASK_Sleep_Time(gt_time ticks)
//{
//	G_DEBUG_VERBOSE
//    GRTOS_USER_CRITICAL_SECTION_GET;
//    
//        <PREVIOUS SUSPEND PROCEDURE >
//    
//	gk_KERNEL_TASK_SUSPEND_CURRENT();
//    GRTOS_USER_CRITICAL_SECTION_GET;
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
INT32 Check_list_TCBAEL_IsValid(int cycles);


/**
 *   newlib FUNTIONS
 */
void gu_printf(char *format, ...);
void gu_fprintf(FILE *stream, char *format, ...);

int __wrap_printf(const char *__format, ...);

int newlib_write(int file, const void *ptr, size_t len);
int newlib_read(int file, void *ptr, size_t len);

#endif /* GEMRTOS_CORE_H_ */