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
};

/// ECBState valid values
#define  GS_ECB_STATE_GRANTED_RESOURCE            1u      ///< \brief State event for Resource GRANTED       \ingroup ECBState
#define  GS_ECB_STATE_WAITING_RESOURCE            2u      ///< \brief State event for Resource WAITING       \ingroup ECBState
#define  GS_ECB_STATE_FREE                        3u      ///< \brief State for FREE event                   \ingroup ECBState
#define  GS_ECB_STATE_WAITING_TIME                4u      ///< \brief State for TIMED EVENT                  \ingroup ECBState
#define  GS_ECB_STATE_UNLINKED                    5u      ///< \brief State for UNLINKED event               \ingroup ECBState

/// ECBType valid values
#define  G_ECB_TYPE_OSTimeDly                   21u       ///< \brief Type for delay event                                  \ingroup ECBType
#define  G_ECB_TYPE_PERIODIC                    22u       ///< \brief Type for PERIODIC event                               \ingroup ECBType
#define  G_ECB_TYPE_LASTEST_TIME	            23u       ///< \brief Type for LASTEST TIME event at the end of time list   \ingroup ECBType
#define  G_ECB_TYPE_TIMEOUT_SEM_GRANTED         24u       ///< \brief Type for timeout granted semaphore                    \ingroup ECBType
#define  G_ECB_TYPE_TIMEOUT_SEM_WAITING         25u       ///< \brief Type for timeout waiting semaphore                    \ingroup ECBType
#define  G_ECB_TYPE_SEM_GRANTED                 28u       ///< \brief Type for semaphore granted event                      \ingroup ECBType
#define  G_ECB_TYPE_SEM_WAITING                 29u       ///< \brief Type for semaphore waiting event                      \ingroup ECBType
#define  G_ECB_TYPE_NOT_SPECIFIED               30u       ///< \brief Type for UNSPECIFIED event                            \ingroup ECBType
#define  G_ECB_TYPE_QUEUE_CONSUME               31u       ///< \brief Type for MESSAGE QUEUE consume event                  \ingroup ECBType
#define  G_ECB_STATE_MESSAGE_CONSUME_WAIT       33u       ///< \brief Type for WAITING TIMEOUT of MQ consumer               \ingroup ECBType
#define  G_ECB_STATE_MESSAGE_CONSUME_EXPIRED    34u       ///< \brief Type for MQ consumer when waiting timeout expired     \ingroup ECBType
#define  G_ECB_STATE_MESSAGE_SEND_EXPIRED       35u       ///< \brief Type for WAITING TIMEOUT of MQ sender                 \ingroup ECBType
#define  G_ECB_STATE_MESSAGE_SEND_WAIT          36u       ///< \brief Type for MQ sender when waiting timeout expired       \ingroup ECBType


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
    struct gs_ext_isr KCB_ExtISR[ALT_NIRQ];  ///< \brief Pointers to ISR TCBs for each interrupt number
    
    // int     *GRTOS_PROCESSOR_BASE[G_NUMBER_OF_PCB+1] __attribute__((aligned(4))); /// \brief Processor specific addresses to go to idle state
    
    GS_PCB  *G_PCBTbl __attribute__((aligned(4)));                                /// \brief Pointer to PCBs dynamic array
    
    INT32             KCB_NUMBER_OF_TCBs;    ///< \brief Number of TCB in the system 
    INT32             KCB_NUMBER_OF_PCBs;    ///< \brief Number of PCB in the system 
    INT32             KCB_NUMBER_OF_ECBs;    ///< \brief Number of ECB in the system 
    INT32             KCB_NUMBER_OF_RCBs;    ///< \brief Number of RCB in the system 
    INT32             KCB_NUMBER_OF_LCBs;    ///< \brief Number of LCB in the system 
    INT32             KCB_NUMBER_OF_SCBs;    ///< \brief Number of SCB in the system 
    INT32             KCB_NUMBER_OF_RRDSs;      ///< \brief Number of RRDS in the system 
    
    /// IDLE and ISR tasks 
    // volatile INT32   G_ISR_STACK[ALT_NIRQ][G_ISR_STACKSIZE] __attribute__((aligned(4)));           ///< \brief Array of STACKS for ISR tasks
    // volatile GS_STK  G_IDLE_STACK[G_NUMBER_OF_PCB][G_IDLE_STACKSIZE] __attribute__((aligned(4)));  ///< \brief Array of STACKS for IDLE tasks    
    volatile INT32   *G_ISR_STACK;
    volatile GS_STK  *G_IDLE_STACK;
    
};

//*************************************************************************************************
/// \defgroup LCBState gs_lcb::LCBState 
/// \defgroup LCBType  gs_lcb::LCBType 

/// GS_LCB List Control Block
/**
 *  \brief gs_lcb  List Control Block structure (LCB)
 *  \details The gs_lcb holds information about processor. 
 *  \todo Check correctness for multiple list scheduling
 *  \todo Include maximun number of running tasks and current running tasks to control exclusion
 */
struct gs_lcb {
    unsigned int  BLOCK_HASH;             ///< \brief BLOCK_HASH of the LCB: (GS_LCB *) + 6
    INT32         LCBState;             ///< \brief State of the List Control Block                         \ingroup LCBState 
    INT32         LCBType;              ///< \brief Type of the List Control Block                          \ingroup LCBType
    void          *malloc_address;        ///< \brief Pointer memory address of the malloc block         
	INT64          LCBRunPriority;      ///< \brief Current Priority (-1 PRIORITY THERE IS NO TASK RUNNING 
	struct gs_tcb  *LCB_NextTCBRUNL;    ///< \brief Pointer to the TCB list of running tasks 
	struct gs_tcb  *LCB_NextTCBRDYL;	 ///< \brief Pointer to the TCB of the Highest Priority Task 
	struct gs_lcb  *LCB_NextLCBL;       ///< \brief Pointer to the next list ordered by priority 
	struct gs_lcb  *LCB_PrevLCBL;       ///< \brief Pointer to the next list ordered by priority 
	struct gs_pcb  *LCB_NextLCBFPL;     ///< \brief Next free processor for this list 
};

/** State of the List Control Block */
#define  GS_LCB_STATE_LINKED                        1u  ///< \brief State LINKED for LIST                      \ingroup PCBState
#define  GS_LCB_STATE_UNLINKED                      2u  ///< \brief State UNLINKED for LIST                    \ingroup PCBState


//*************************************************************************************************
/// \defgroup PCBState g_rgb::PCBState 
/// \defgroup PCBType  g_rgb::PCBType 

/// GS_PCB Processor Control Block
/**
 *  \brief gs_pcb  Processor Control Block structure (PCB)
 *  \details The gs_pcb holds information about processor. 
 *  Each processors has an unique PCBID according to the ID of the associated processor.
 *  \todo Define the type for processors
 */
struct gs_pcb {
	int PCBID;                                  ///< \brief Processor ID 
	int PCBState;                               ///< \brief State of the PCB : GS_FREE_PROCESSOR, GS_RUNNING_PROCESSOR  \ingroup PCBState
    int PCBType;                                ///< \brief Type of the processor                                       \ingroup PCBType
    int *GRTOS_PROCESSOR_BASE __attribute__((aligned(4))); /// \brief Processor specific addresses to go to idle state    
	struct gs_lcb *PCB_RDY_LCBL[G_NUMBER_OF_LCBs_FOR_PCB]; ///< \brief Ready Lists the processor services 
	struct gs_tcb *PCB_IDLETCB;                 ///< \brief Pointer to the TCB of the IDLE Task 
	struct gs_tcb *PCB_EXECTCB;                 ///< \brief Current task assigned to processor 
	struct gs_pcb *PCB_NextPCB;                 ///< \brief Pointer to the next free processor 
	struct gs_pcb *PCB_PrevPCB;                 ///< \brief Pointer to the previous free processor 
};

/// PCBState valid values
#define  GS_PCB_STATE_NOTRUNNING           1u    ///< \brief It is NOTRUNNING at starting time                                  \ingroup PCBState
#define  GS_PCB_STATE_RUNNING              2u    ///< \brief It is RUNNING when executing a main list task (LCB[0])             \ingroup PCBState 
#define  GS_PCB_STATE_FREE                 3u    ///< \brief It is FREE when not executing a main list task                     \ingroup PCBState


#include <mq.h>
#include <sem.h>

/** MESSAGE RESOURCE *************************************************************/
/**
 *  \todo Check if T_MESSAGE_RESOURCE structure is required
 */
struct T_MESSAGE_RESOURCE {
    INT32          MESSize;            /** Size of queue (maximum number of entries) */
    INT32          MESEntries;         /** Current number of entries in the queue    */
    struct gs_mcb  *MES_FirstMCB;      /** Pointer to the first MCB of queue data    */
    struct gs_mcb  *MES_LastMCB;       /** Pointer to the last MCB of queue data     */
    INT32          LAST_MCB_ID;        /** Last MCB_ID assigned to a MCB             */
};

/** QUEUE DATA STRUCTURE *********************************************************/
/**
 *  \todo Check if gs_mcb structure is required
 */
struct gs_mcb {              /** MESSAGE CONTROL BLOCK                        */
    INT32         MCBState;          /** State of the Queue: UNUSED, FREE, LINKED       */
    INT32         MCBCount;          /** Number of task to deliver the message         */
	struct gs_mcb *MCB_NextMCB;      /** Link to next queue control block in list of free blocks */
	struct gs_mcb *MCB_PrevMCB;      /** Link to next queue control block in list of free blocks */
    void          *MCB_StartMessage; /** Pointer to the start of message data        */
    void          *MCB_EndMessage;   /** Pointer to the end of message data                */
    INT32         MCB_ID;            /** Identification for the MCB     */
    INT32         MCB_RlsCount;      /** Number of task to deliver the message         */
};

/// MCBState valid values
#define  GK_MCB_STATE_UNLINKED   2u
#define  GK_MCB_STATE_FREE       3u
#define  GK_MCB_STATE_LINKED     4u


extern struct T_QUEUE_RESOURCE     queue;

//*************************************************************************************************
/// \defgroup RCBState g_rgb::RCBState 
/// \defgroup RCBType  g_rgb::RCBType 

/// G_RCB Resource Request Data Structure
/**
 *  \brief g_rgb  Resource Control Block (RCB)
 *  \details The g_rgb holds information about different kinds of resources.
 *  \todo Define a unified srtucture for different kind of resources. 
 *  \todo Re-write the message resource routines
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
    };    
    union {
        struct T_SEMAPHORE_RESOURCE semaphore;         ///< \brief is the semaphore resource structure
        struct T_MESSAGE_RESOURCE   message;           ///< \brief is the message resource structure
        struct T_QUEUE_RESOURCE     queue;             ///< \brief is the queue resource, defined in mq.h
    };
};

/// RCBState valid values                     
#define  GK_RCB_STATE_SEM           3u        ///< \brief State SEM for semaphore of the RCB    \ingroup RCBState 
#define  GK_RCB_STATE_MQ            4u        ///< \brief State MQ for MESSAGE QUEUE of the RCB \ingroup RCBState
#define  GK_RCB_STATE_UNDEFINED     5u        ///< \brief State UNDEFINED of the RCB            \ingroup RCBState
#define  GK_RCB_STATE_QUEUE         6u        ///< \brief State QUEUE for QUEUE of the RCB      \ingroup RCBState

/// RCBType valid values
#define  GK_RCB_TYPE_UNUSED         1u        ///< \brief Type UNUSED of the RCB               \ingroup RCBState 
#define  GK_RCB_TYPE_FREE           2u        ///< \brief Type FREE of the RCB                 \ingroup RCBState
#define  GK_RCB_TYPE_SEM            3u        ///< \brief Type SEM for semaphore of the RCB    \ingroup RCBState
#define  GK_RCB_TYPE_MQ             4u        ///< \brief Type MQ for MESSAGE QUEUE of the RCB \ingroup RCBState
#define  GK_RCB_TYPE_QUEUE          5u        ///< \brief Type QUEUE for QUEUE of the RCB      \ingroup RCBState


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
};

// SCBState gruop
/// \brief SCBState is state of the SCB
#define  G_SCB_STATE_FREE          1u         ///< \brief State FREE of the SCB        \ingroup SCBState
#define  G_SCB_STATE_IN_KCB        2u         ///< \brief State SCB linked to KCB      \ingroup SCBState 
#define  G_SCB_STATE_IN_ECB        3u         ///< \brief State SCB linked to ECB      \ingroup SCBState
#define  G_SCB_STATE_IN_TCB        4u         ///< \brief State SCB linked to TCB      \ingroup SCBState
#define  G_SCB_STATE_IN_RCB        5u         ///< \brief State SCB linked to RCB      \ingroup SCBState
#define  G_SCB_STATE_IN_PCB        6u         ///< \brief State SCB linked to PCB      \ingroup SCBState
#define  G_SCB_STATE_IN_LCB        7u         ///< \brief State SCB linked to LCB      \ingroup SCBState
#define  G_SCB_STATE_UNLINKED      8u         ///< \brief State UNLINKED               \ingroup SCBState
#define  G_SCB_STATE_PENDING       9u         ///< \brief State PENDING                \ingroup SCBState
#define  G_SCB_STATE_EXECUTING    10u         ///< \brief State EXECUTING              \ingroup SCBState

// SCBType group
/// \brief SCBType valid values 
/// \todo Define the rest of SCB types
#define G_SCB_TCB_ABORTED          1000u      ///< TYPE "Periodic task aborted when released before deadline"      \ingroup SCBType


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
};

// TCBState gruop
/// \details The TCBState is according to bits (starts in bit 0): bit 1 -> WAITING, 2 ->READY, 3 -> RUNNING, 4 ->FREE, 5->UNLINKED
#define  GRTOS_TASK_STATE_WAITING(ptcb)  ((ptcb->TCBState >> 1) & 1) ///< Returns True if task is waiting  \ingroup TCBState

#define  G_TASK_STATE_WAITING_COMPLETED           2u   ///< \brief State WAITING, task COMPLETED     \ingroup TCBState 
#define  G_TASK_STATE_WAITING                   258u   ///< \brief State WAITING, but NOT COMPLETED  \ingroup TCBState
#define  G_TASK_STATE_READY                       4u   ///< \brief State READY                       \ingroup TCBState
#define  G_TASK_STATE_RUNNING                     8u   ///< \brief State RUNNING                     \ingroup TCBState
#define  G_TASK_STATE_FREE                       16u   ///< \def G_TASK_STATE_FREE \brief State FREE                        \ingroup TCBState
#define  G_TASK_STATE_UNLINKED                   32u   ///< \brief State UNLINKED                    \ingroup TCBState


/// Core functions definitions
GS_ECB *gk_Get_ECB(void);
G_RCB  *gk_Get_RCB(void);
GS_SCB *gk_Get_SCB(void);
GS_RRDS *gk_Get_RRDS(void);
GS_LCB *gk_Get_LCB(void);
INT32 gk_Create_PCBs(int Nmbr_PCB);



#endif /* GEMRTOS_CORE_H_ */