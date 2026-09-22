/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS kernel core types, control-block definitions, and validation macros
 *  \details Defines the fundamental GeMRTOS data types (GS_TCB, GS_ECB, GS_RCB,
 *           GS_PCB, GS_LCB, GS_SCB, GS_RRDS), their state and type enumerations,
 *           the BLOCK_HASH pointer-validation system, PRINT_ASSERT debug macros,
 *           and hardware I/O accessor macros (gm_IOWR, gm_IORD).
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


#ifndef GEMRTOS_CORE_H_
#define GEMRTOS_CORE_H_

/* The foundation layer: scalar types, boolean constants and TIMEPRIORITY.
 * Included explicitly so this header compiles independently (CLAUDE.md 30),
 * rather than relying on gemrtos_primitives.h having been read first. */
#include "gemrtos_types.h"


/************************************************************************************
 *  CPU-CORE REGISTER ACCESS FOR TASK CONTEXT AND INTERRUPT CONTROL
 *
 *  Moved here from gemrtos.h 2026-08-18: these are Nios II / Nios V CPU-core
 *  register and instruction differences (stack pointer, processor interrupt
 *  enable/disable, interrupt-return offset, context-frame size) consumed by
 *  kernel-core logic -- gk_TASK_STK_INIT (below) and gemrtos_kernel.c's
 *  dispatch/task-start code. Not GeMRTOS Controller IP register access (that
 *  is gemrtos_primitives.h's gm_IOWR/gm_IORD seam); these macros never touch
 *  the Controller. gemrtos.h still defines __niosX_arch__ and includes
 *  nios2.h/intel_niosv.h earlier, before this header is reached, so
 *  NIOS2_READ_SP/NIOS2_WRITE_IENABLE/etc. are already visible here.
 ************************************************************************************/
#ifdef __nios2_arch__
    #define GRTOS_READ_SP(dest) NIOS2_READ_SP(dest)

	#define GRTOS_WRITE_SP(sp) do { __asm volatile ("mov sp, %z0" : : "rM" (sp)); } while (0)


    #define GRTOS_DISABLE_PRC_GENERAL_IRQ    do { \
                                                   NIOS2_WRITE_IENABLE(0); \
                                                   NIOS2_WRITE_STATUS(0);  \
                                              } while (0)

    #define GRTOS_ENABLE_PRC_IRQ   \
        do { \
            NIOS2_WRITE_IENABLE(1); \
            NIOS2_WRITE_STATUS(1); \
        } while(0)

    // \brief G_RETURN_OFFSET_FROM_INTERRUPT offset to add to the return offset because
    // it is returned from interrupt. In NIOS2 processor, returning from
    // interrupt decrement in 4 previous to return
    #define G_RETURN_OFFSET_FROM_INTERRUPT  4

    // \brief G_STACK_STATUS_LENGTH number of addresses required in stack to save the task status
    #define G_STACK_STATUS_LENGTH  28  /* words: addresses needed on stack to save task status */
#endif


#ifdef __riscv
    #define GRTOS_READ_SP(dest) NIOSV_READ_SP(dest)

	#define GRTOS_WRITE_SP(sp) do { __asm volatile ("addi sp, %z0, 0" : : "rM" (sp)); } while (0)

    #define GRTOS_DISABLE_PRC_GENERAL_IRQ   do { __asm volatile ("csrw mstatus, zero"); \
                                                 __asm volatile ("csrw mie, zero"); \
                                                 __asm volatile ("csrw mip, zero"); \
                                               } while (0)


    // it is bit 16 the first external hardware interrupt available
    #define GRTOS_ENABLE_PRC_IRQ  \
            do { __asm volatile ("la t0, grtos_irq_entry; \
                                  csrw mtvec, t0; \
                                  li t0, 0x80; \
                                  csrw mstatus, t0; \
                                  li t0 , 0x00010800; \
                                  csrw mie, t0"); \
            } while (0)

    #define G_RETURN_OFFSET_FROM_INTERRUPT  0
    #define G_STACK_STATUS_LENGTH  34  /* words: addresses needed on stack to save task status */

#endif


/**
*********************************************************************************************************
*                            TIME and PRIORITY definitions
*********************************************************************************************************
**/ 

#define G_LOWEST_PRIORITY      0x7FFFFFFFFFFFFFFCLL  ///< Lowest Priority of a Task
#define G_IDLETASK_PRIORITY    0x7FFFFFFFFFFFFFFDLL  ///< Priority of Idle Task
#define G_LATEST_TIME          0x7FFFFFFFFFFFFFFFLL  ///< Lastest Occurence Time of an EVENT
#define G_DEFAULT_LCB_PRIORITY 0x7FFFFFF0


/************************************************************************************
 *  System constants definitions
 ************************************************************************************/
/* G_FALSE / G_TRUE live in gemrtos_types.h, the foundation layer. */

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
typedef struct g_rcb       GS_RCB;
typedef struct gs_mcb      GS_MCB;
typedef struct gs_pcb_rdy_lcbl GS_PCBAssocLCB;
typedef struct g_rcb       t_semaphore_resource;


/* union timepriority / TIMEPRIORITY moved to gemrtos_types.h: it is a
 * register-transfer helper, and gemrtos_primitives.h needs it too. */


/************************************************************************************
 *  GeMRTOS HASH DEFINITIONS
 ************************************************************************************/
#define G_TCB_HASH                1
#define G_ECB_HASH                2
#define G_RCB_HASH                3
#define G_SCB_HASH                4
#define G_RRDS_HASH               5
#define G_LCB_HASH                6
#define G_PCBALCB_HASH            7
#define G_PCB_HASH                8
#define GE_TRGStatus_HASH         9
#define GE_TRGTimeOutType_HASH   10
#define GE_ECBState_HASH         11
#define GE_ECBType_HASH          12
#define GE_TCBState_HASH         13
#define GE_TCBType_HASH          14
#define GE_PCBState_HASH         15
#define GE_PCBType_HASH          16
#define GE_LCBState_HASH         17
#define GE_LCBType_HASH          18
#define GE_RCBType_HASH          19


/**
 * \brief Type-generic validity check â€” dispatches to the correct gkm_*_IsValid macro
 *        based on the static type of the pointer argument.
 * \param [in] x  Pointer to any GeMRTOS control block (GS_TCB *, GS_ECB *, GS_RCB *, etc.).
 * \return G_TRUE if the pointer's embedded BLOCK_HASH matches the expected sentinel; G_FALSE otherwise.
 */
 /* Test according with the structure type */
#define struct_test(x) _Generic((x),  \
        GS_TCB *             : gkm_TCB_IsValid(x),              \
		GS_PCBAssocLCB *     : gkm_PCBAssocLCB_IsValid(x),      \
        GS_PCB *             : gkm_PCB_IsValid(x),              \
        GS_ECB *             : gkm_ECB_IsValid(x),              \
        GS_SCB *             : gkm_SCB_IsValid(x),              \
        GS_RCB *             : gkm_RCB_IsValid(x),              \
        GS_RRDS *            : gkm_RRDS_IsValid(x),             \
        GS_LCB *             : gkm_LCB_IsValid(x),              \
		default              : gkm_XCB_NotValid(x))

/** \brief Validate a TCB pointer via its embedded BLOCK_HASH sentinel.
 *  \param [in] ptcb  Pointer to the task control block to validate (GS_TCB *).
 *  \return G_TRUE if the pointer refers to a valid TCB; G_FALSE otherwise. */
#define gkm_TCB_IsValid(ptcb) ((((unsigned int)((GS_TCB *)ptcb)->BLOCK_HASH) == (unsigned int) ptcb + G_TCB_HASH) ? G_TRUE : G_FALSE)

/** \brief Validate a PCBAssocLCB pointer via its embedded BLOCK_HASH sentinel.
 *  \param [in] ppcbalcb  Pointer to the processorâ€“list-control-block association to validate.
 *  \return G_TRUE if valid; G_FALSE otherwise. */
#define gkm_PCBAssocLCB_IsValid(ppcbalcb) ((((unsigned int)((GS_PCBAssocLCB *) ppcbalcb)->BLOCK_HASH) == (unsigned int) ppcbalcb + G_PCBALCB_HASH) ? G_TRUE : G_FALSE)

/** \brief Validate an RCB pointer via its embedded BLOCK_HASH sentinel.
 *  \param [in] prcb  Pointer to the resource control block to validate (GS_RCB *).
 *  \return G_TRUE if valid; G_FALSE otherwise. */
#define gkm_RCB_IsValid(prcb) ((((unsigned int)((GS_RCB *) prcb)->BLOCK_HASH) == (unsigned int) prcb + G_RCB_HASH) ? G_TRUE : G_FALSE)

/** \brief Validate a PCB pointer via its embedded BLOCK_HASH sentinel.
 *  \param [in] ppcb  Pointer to the processor control block to validate (GS_PCB *).
 *  \return G_TRUE if valid; G_FALSE otherwise. */
#define gkm_PCB_IsValid(ppcb) ((((unsigned int)((GS_PCB *)ppcb)->BLOCK_HASH) == (unsigned int) ppcb + G_PCB_HASH) ? G_TRUE : G_FALSE)

/** \brief Validate an ECB pointer via its embedded BLOCK_HASH sentinel.
 *  \param [in] pevent  Pointer to the event control block to validate (GS_ECB *).
 *  \return G_TRUE if valid; G_FALSE otherwise. */
#define gkm_ECB_IsValid(pevent) ((((unsigned int)((GS_ECB *)pevent)->BLOCK_HASH) == (unsigned int) pevent + G_ECB_HASH) ? G_TRUE : G_FALSE)

/** \brief Validate an SCB pointer via its embedded BLOCK_HASH sentinel.
 *  \param [in] pscb  Pointer to the signal control block to validate (GS_SCB *).
 *  \return G_TRUE if valid; G_FALSE otherwise. */
#define gkm_SCB_IsValid(pscb) ((((unsigned int)((GS_SCB *)pscb)->BLOCK_HASH) == (unsigned int) pscb + G_SCB_HASH) ? G_TRUE : G_FALSE)

/** \brief Validate an RRDS pointer via its embedded BLOCK_HASH sentinel.
 *  \param [in] prrds  Pointer to the resource-request descriptor to validate (GS_RRDS *).
 *  \return G_TRUE if valid; G_FALSE otherwise. */
#define gkm_RRDS_IsValid(prrds) ((((unsigned int) ((GS_RRDS *)prrds)->BLOCK_HASH) == (unsigned int) prrds + G_RRDS_HASH) ? G_TRUE : G_FALSE)

/** \brief Validate an LCB pointer via its embedded BLOCK_HASH sentinel.
 *  \param [in] plcb  Pointer to the list control block to validate (GS_LCB *).
 *  \return G_TRUE if valid; G_FALSE otherwise. */
#define gkm_LCB_IsValid(plcb) ((((unsigned int) ((GS_LCB *) plcb)->BLOCK_HASH) == (unsigned int) plcb + G_LCB_HASH) ? G_TRUE : G_FALSE)

/** \brief Fallback for struct_test() â€” always returns G_TRUE for unrecognised pointer types. */
#define gkm_XCB_NotValid(ptcb) (G_TRUE)

#define gkm_TCBState_Valid(value)       gkm_ENUM_IsValid( GE_TCBState_HASH , value)
#define gkm_TCBType_Valid(tcbtype)      gkm_ENUM_IsValid( GE_TCBType_HASH , tcbtype)
#define gkm_ECBState_Valid(ecbstate)    gkm_ENUM_IsValid( GE_ECBState_HASH , ecbstate)
#define gkm_ECBType_Valid(ecbtype)      gkm_ENUM_IsValid( GE_ECBType_HASH , ecbtype)
#define gkm_PCBState_Valid(pcbstate)    gkm_ENUM_IsValid( GE_PCBState_HASH , pcbstate)
#define gkm_PCBType_Valid(pcbtype)      gkm_ENUM_IsValid( GE_PCBType_HASH , pcbtype)
#define gkm_LCBState_Valid(lcbstate)    gkm_ENUM_IsValid( GE_LCBState_HASH , lcbstate)
#define gkm_LCBType_Valid(lcbtype)      gkm_ENUM_IsValid( GE_LCBType_HASH , lcbtype)
#define gkm_RCBType_Valid(rcbtype)      gkm_ENUM_IsValid( GE_RCBType_HASH , rcbtype)

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
typedef enum {
	/** \brief State event for Resource GRANTED
	 *  \ingroup ECBState
	 */ 
	GS_ECBState_GRANTED_RESOURCE = ENUM_CODE(GE_ECBState_HASH, 1),

	/** \brief State event for Resource WAITING
	 *  \ingroup ECBState
	 */
	GS_ECBState_WAITING_RESOURCE = ENUM_CODE(GE_ECBState_HASH, 2),

	/** \brief State for FREE event
	 *  \ingroup ECBState
	 */
	GS_ECBState_FREE = ENUM_CODE(GE_ECBState_HASH, 3),

	/** \brief State for TIMED EVENT
	 *  \ingroup ECBState
	 */
	GS_ECBState_WAITING_TIME = ENUM_CODE(GE_ECBState_HASH, 4),

	/** \brief State for UNLINKED event
	 *  \ingroup ECBState
	 */
	GS_ECBState_UNLINKED = ENUM_CODE(GE_ECBState_HASH, 5)     
} GE_ECBState;
 /** @} */

/** \defgroup ECBType  gs_ecb::ECBType 
 *  @{
 */
typedef enum {
	/** \brief Type for delay event
	 *  \ingroup ECBType
	 */
	G_ECBType_OSTimeDly = ENUM_CODE(GE_ECBType_HASH, 1),
	
	/** \brief Type for PERIODIC event
	 *  \ingroup ECBType
	 */
	G_ECBType_PERIODIC = ENUM_CODE(GE_ECBType_HASH, 2),

	/** \brief Type for LASTEST TIME event at end of time list
	 *  \ingroup ECBType
	 */
	G_ECBType_LASTEST_TIME = ENUM_CODE(GE_ECBType_HASH, 3),

	/** \brief Type for semaphore granted event
	 *  \ingroup ECBType
	 */
	G_ECBType_SEM_GRANTED = ENUM_CODE(GE_ECBType_HASH, 4),

	/** \brief Type for semaphore waiting event
	 *  \ingroup ECBType
	 */
	G_ECBType_SEM_WAITING = ENUM_CODE(GE_ECBType_HASH, 5),

	/** \brief Type for UNSPECIFIED event
	 *  \ingroup ECBType
	 */
	G_ECBType_NOT_SPECIFIED = ENUM_CODE(GE_ECBType_HASH, 6),

	/** \brief Type for MESSAGE QUEUE consume event
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_RECEIVER_BUSY = ENUM_CODE(GE_ECBType_HASH, 7),
	
	/** \brief Type for WAITING TIMEOUT of MQ consumer
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_RECEIVER_WAIT = ENUM_CODE(GE_ECBType_HASH, 8),
	
	/** \brief Type for WAITING TIMEOUT of MQ consumer            
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_SENDER_WAIT = ENUM_CODE(GE_ECBType_HASH, 9),
	
	/** \brief Type for WAITING TIMEOUT of MQ consumer
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_SENDER_DELIVERED = ENUM_CODE(GE_ECBType_HASH, 10),

	/** \brief Type for MQ consumer when waiting timeout expired  
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_RECEIVER_EXPIRED = ENUM_CODE(GE_ECBType_HASH, 11),

	/** \brief Type for MQ consumer when waiting timeout expired  
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_RECEIVER_ABORTED = ENUM_CODE(GE_ECBType_HASH, 12),
    
	/** \brief Type for WAITING TIMEOUT of MQ sender              
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_SENDER_EXPIRED = ENUM_CODE(GE_ECBType_HASH, 13),

	/** \brief Type for MQ sender when waiting timeout expired    
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_SENDER_TIMEOUT = ENUM_CODE(GE_ECBType_HASH, 14),

	/** \brief Type for Waiting External interrupt event                  
	 *  \ingroup ECBType
	 */
	G_ECBType_EXT_ISR_WAITING = ENUM_CODE(GE_ECBType_HASH, 15),

	/** \brief Type for Released External interrupt event
	 *  \ingroup ECBType
	 */
	G_ECBType_EXT_ISR_RELEASE = ENUM_CODE(GE_ECBType_HASH, 16),

	/** \brief Type for semaphore timeout event
	 *  \ingroup ECBType
	 */
	G_ECBType_SEM_WAITING_TIMEOUT = ENUM_CODE(GE_ECBType_HASH, 17),

	/** \brief Type for semaphore timeout expired
	 *  \ingroup ECBType
	 */
	G_ECBType_SEM_WAITING_EXPIRED = ENUM_CODE(GE_ECBType_HASH, 18), 
	
	/** \brief Type for trigger timeout event
	 *  \ingroup ECBType
	 */
	G_ECBType_TRIGGER_TIMEOUT = ENUM_CODE(GE_ECBType_HASH, 20),

	/** \brief Type for G_ECBType_ONESHOT_START_TIME event
	 *  \ingroup ECBType
	 */
	G_ECBType_ONESHOT_START_TIME = ENUM_CODE(GE_ECBType_HASH, 21),

	/** \brief Type for a message-queue receiver that has subscribed but has
	 *  not yet made its own first gu_MessageQueueReceive() call. Distinct
	 *  from G_ECBType_MESSAGE_RECEIVER_WAIT:
	 *  WAIT means the owning task is genuinely suspended, linked into the
	 *  wait list, and safe for gu_MessageQueueSend() to unlink and wake.
	 *  SUBSCRIBED means the owning task is still running its own code
	 *  (between gu_MessageQueueSubscribe() returning and that same task's
	 *  own first gu_MessageQueueReceive() call) -- it must never be treated
	 *  as deliverable-and-wakeable, since it was never linked into any wait
	 *  list to unlink from. gu_MessageQueueReceive() transitions this to
	 *  WAIT itself, only once it has actually found no backlog message and
	 *  is about to genuinely suspend.
	 *  \ingroup ECBType
	 */
	G_ECBType_MESSAGE_RECEIVER_SUBSCRIBED = ENUM_CODE(GE_ECBType_HASH, 22)

 } GE_ECBType;
/** @} */

// GS_ECB  Event Control Block structure
/**
 *  \brief gs_ecb  Event Control Block structure (EBC)
 *  \details The gs_ecb holds information about events. 
 *  \todo Check if all the states and type are needed
 */
struct gs_ecb {
    unsigned int   BLOCK_HASH;         ///< \brief BLOCK_HASH of the ECB: (GS_ECB *) + G_ECB_HASH
	GE_ECBState    ECBState;           ///< \brief Granted, Waiting, Free  \ingroup ECBState
	GE_ECBType     ECBType;            ///< \brief Type of event control block   \ingroup ECBType
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
	GS_PCBState_NOTRUNNING = ENUM_CODE(GE_PCBState_HASH, 1),

	/** \brief It is RUNNING when executing a main list task (LCB[0])
	 *  \ingroup PCBState 
	 */
	GS_PCBState_RUNNING = ENUM_CODE(GE_PCBState_HASH, 2),

	/** \brief It is FREE when not executing a main list task
	 *  \ingroup PCBState
	 */
	GS_PCBState_FREE = ENUM_CODE(GE_PCBState_HASH, 3)
};
 /** @} */
 
/// \defgroup PCBType  g_rgb::PCBType 
enum pcbtype {
	/** \brief It is GS_PCBType_AVAILABLE when it is available for task scheduling
	 *  \ingroup PCBType
	 */
	GS_PCBType_AVAILABLE = ENUM_CODE(GE_PCBType_HASH, 1),
	
	/** \brief It is GS_PCBType_UNAVAILABLE when it is assigned to waiting for interrupt
	 *  \ingroup PCBType
	 */
	GS_PCBType_UNAVAILABLE = ENUM_CODE(GE_PCBType_HASH, 2)
};
// GS_PCB Processor Control Block
/**
 *  \brief gs_pcb  Processor Control Block structure (PCB)
 *  \details The gs_pcb holds information about processor. 
 *  Each processors has an unique PCBID according to the ID of the associated processor.
 *  \todo Define the type for processors
 */
struct gs_pcb {
	unsigned int  BLOCK_HASH;    ///< \brief BLOCK_HASH of the PCB: (GS_PCB *) + G_PCB_HASH
	G_UINT32 PCBID;              ///< \brief Processor ID 
	enum pcbstate PCBState;      ///< \brief State of the PCB:GS_FREE_PROCESSOR,GS_RUNNING_PROCESSOR \ingroup PCBState
    enum pcbtype PCBType;        ///< \brief Type of the processor   \ingroup PCBType
    struct gs_pcb_rdy_lcbl *PCB_AssocLCB;       ///< \brief Linked list of the associated LCBs that processor serves
	struct gs_tcb *PCB_IDLETCB;                 ///< \brief Pointer to the TCB of the IDLE Task 
	struct gs_tcb *PCB_EXECTCB;                 ///< \brief Current task assigned to processor 
	struct gs_pcb *PCB_NextPCB;                 ///< \brief Pointer to the next free processor 
	struct gs_pcb *PCB_PrevPCB;                 ///< \brief Pointer to the previous free processor
	void *PCB_SUSPENDED_STK_PTR;                ///< Address of the StackPointer of suspended TCB
	
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

	struct gs_pcb G_PCBTbl[G_NUMBER_OF_PCB] __attribute__((aligned(4)));    ///< \brief System PCBs
    
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

#ifndef __niosX_arch__                                                     // Windows version
    CRITICAL_SECTION GeMRTOS_MUTEX_section;                                // Windows version
    CRITICAL_SECTION GeMRTOS_MUTEX_swapcontext;                            // Windows version
	CRITICAL_SECTION GeMRTOS_MUTEX_CONDITION_VARIABLE;                     // Windows version
	CONDITION_VARIABLE GeMRTOS_CONDITION_VARIABLE;                         // Windows version
#endif                                                                     // Windows version
    
    // roots for linked list of data structures
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    G_UINT32             KCB_NUMBER_OF_TCBs;             ///< \brief Number of TCB in the system
    G_UINT32             KCB_NUMBER_OF_PCBs;             ///< \brief Number of PCB in the system
    G_UINT32             KCB_NUMBER_OF_ECBs;             ///< \brief Number of ECB in the system
    G_UINT32             KCB_NUMBER_OF_RCBs;             ///< \brief Number of RCB in the system
    G_UINT32             KCB_NUMBER_OF_LCBs;             ///< \brief Number of LCB in the system
    G_UINT32             KCB_NUMBER_OF_SCBs;             ///< \brief Number of SCB in the system
    G_UINT32             KCB_NUMBER_OF_RRDSs;            ///< \brief Number of RRDS in the system
    G_UINT32             KCB_NUMBER_OF_PCBAssocLCBs;     ///< \brief Number of RRDS in the system	

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
	GS_LCBState_LINKED = ENUM_CODE(GE_LCBState_HASH, 1),
	
	/** \brief State UNLINKED for LIST 
	 *  \ingroup LCBState 
	 *  \remark GS_LCBState_UNLINKED
	 */
	GS_LCBState_UNLINKED = ENUM_CODE(GE_LCBState_HASH, 2)  
};
/** @}*/

/** \defgroup LCBType  gs_lcb::LCBType 
 *  @{ */
enum lcbtype {

/** \brief The <b>GS_LCBTypeEDF</b> scheduling list type implements the Earliest Deadline First (EDF) discipline among the tasks assigned to the scheduling list. In the EDF discipline, the earliest deadline, the highest priority. In order to maintain consistency, tasks that are assigned to an EDF scheduled list should be of the periodic type, with the deadline taken into account starting from the release time.
 *  \ingroup LCBType
 *  \remark GS_LCBTypeEDF
 */
	GS_LCBTypeEDF = ENUM_CODE( GE_LCBType_HASH, 3),

/** \brief The <b>GS_LCBTypeFP</b> scheduling list type implements the Fixed Priority (FP) discipline among the tasks assigned to the scheduling list. In the FP discipline, a priority is assigned to each task. Task priority may be modified during runtime. In order to maintain consistency, tasks that are assigned to an FP scheduled list should not be of the infinite-loop type without waiting for event suspension in order to avoid starving lower-priority tasks. Only the lowest-priority task could be implemented as an infinite-loop code.
 *  \ingroup LCBType
 *  \remark GS_LCBTypeFP
 */
	GS_LCBTypeFP = ENUM_CODE( GE_LCBType_HASH, 4)
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
    G_UINT32       LCBCurrentRunning;   ///< \brief Current number of running tasks
    G_UINT32       LCBExclusion;        ///< \brief Maximum number of running task (0 for no limit)
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
    G_UINT32                 PCB_RDY_priority;
    struct gs_lcb          *PCB_RDY_LCBL;
    
    /// Fields for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    struct gs_pcb_rdy_lcbl *gs_pcb_lcbl_nexts;
    struct gs_pcb_rdy_lcbl *gs_pcb_lcbl_prevs;
    G_UINT32 gs_pcb_assoc_pcb;
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
 *  \remark g_scbtype_frozen_mode
 */	
	G_SCBType_FROZEN_MODE = 1u,

/** \brief The G_SCBType_UNFROZEN_MODE signal type is defined to signaling when GeMRTOS controller enters in non frozen mode.
 *  \ingroup SCBType
 *  \remark g_scbtype_unfrozen_mode
 */   
    G_SCBType_UNFROZEN_MODE = 2u
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
    unsigned int   BLOCK_HASH;               ///< \brief BLOCK_HASH of the SCB: (GS_SCB *) + G_SCB_HASH
    enum scbstate  SCBState;                 ///< \brief STATE of the signal  \ingroup SCBState
	enum scbtype   SCBType;                  ///< \brief TYPE of the signal   \ingroup SCBType
    G_UINT32       SCBPriority;              ///< \brief Priority of the SCB when it is linked
    void          *SCB_TaskCode;             ///< \brief Pointer to the code of the signal 
    void          *SCB_TaskArg;              ///< \brief Pointer to the argument of the signal 
    struct gs_scb *SCB_NextSCB;              ///< \brief Pointer to the next SCB linked
    void          *SCB_AssocXCB;             ///< \brief Pointer to the data structure root of the SCBASL
    
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
	G_TCBState_WAITING_COMPLETED = ENUM_CODE(GE_TCBState_HASH, 1), 

	/** \brief State WAITING, but NOT COMPLETED  
	 *  \ingroup TCBState
	 */
	G_TCBState_WAITING = ENUM_CODE(GE_TCBState_HASH, 2), 

	/** \brief State READY                       
	 *  \ingroup TCBState
	 */
	G_TCBState_READY = ENUM_CODE(GE_TCBState_HASH, 3), 

	/** \brief State RUNNING                     
	 *  \ingroup TCBState
	 */
	G_TCBState_RUNNING = ENUM_CODE(GE_TCBState_HASH, 4), 
	
	/** \brief State UNLINKED                    
	 *  \ingroup TCBState	
	 */
	G_TCBState_UNLINKED = ENUM_CODE(GE_TCBState_HASH, 5) 
};
 /** @} */


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
	G_TCBType_OneShot = ENUM_CODE(GE_TCBType_HASH, 1), 

	/** \brief The <b>G_TCBType_Periodic</b> task type makes the task code to be executed periodically. The period of the task is configured when the type is specified. The period of the task and the initial offset determines the future releases times of the task. If previous invocation of the task does not completes, then the previous invocation may be defined to be aborted or the next release skipped. Periodic tasks are useful to meet Nyquist and Shannon theorems in cyber-physical applications. However, if no scheduling analysis is performed, the system may became oversaturated and the deadlines missed. 
	 *  \ingroup TCBType
	 *  \remark G_TCBType_Periodic
	 */
	G_TCBType_Periodic = ENUM_CODE(GE_TCBType_HASH, 2),
	G_TCBType_Periodic_Skip = ENUM_CODE(GE_TCBType_HASH, 3),

	/** \brief The <b>G_TCBType_ISR</b> task type determines that the task is associated with a trigger resource. The G_TCBType_ISR type of the task should be set after the task is created using the <em>gu_TriggerRegisterTask</em> function.
	 *  \ingroup TCBType
	 *  \remark G_TCBType_ISR
	 */
	G_TCBType_ISR = ENUM_CODE(GE_TCBType_HASH, 4),   

	/** \brief The <b>G_TCBType_IDLE</b> task type determines the task that a processor executes when no task requires for execution. The IDLE task is a GeMRTOS system task and there is one for each system processor. By default, the G_TCBType_IDLE task turns the processor into sleep mode in order to save energy and reduce the system bus utilization.
	 *  \ingroup TCBType
	 *  \remark G_TCBType_IDLE
	 */
	G_TCBType_IDLE = ENUM_CODE(GE_TCBType_HASH, 5),

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
    unsigned int   BLOCK_HASH;           ///< \brief BLOCK_HASH of the TCB: (GS_TCB *) + G_TCB_HASH
    enum tcbstate  TCBState;             ///< \brief STATE of the task  \ingroup TCBState 
    enum tcbtype   TCBType;              ///< \brief TYPE of the task   \ingroup TCBType
    G_UINT64       TCBReadyPriority;     ///< \brief Priority of the Task when Ready
    G_UINT64       TCBRunPriority;       ///< \brief Priority when it is executed
	G_UINT64       TCBCurrentPriority;   ///< \brief Priority inhereted
	G_UINT64       TCBPeriod;            ///< \brief Period of the task
    volatile G_SSTK *TCB_StackPointer;   ///< \brief Pointer to current top of stack
    volatile G_SSTK *TCB_StackBottom;    ///< \brief Botton Stack of the Task
    volatile G_SSTK *TCB_StackTop;       ///< \brief Botton Stack of the Task
    volatile void   *TCB_TaskCode;       ///< \brief Pointer to the Task Code
	volatile void   *TCB_TaskArg;        ///< \brief Pointer to the argument of the first call
    struct gs_tcb *TCB_NextTCB;          ///< \brief Pointer to next TCB in the TCB list
    struct gs_tcb *TCB_PrevTCB;          ///< \brief Pointer to previous TCB in the TCB list
    struct gs_ecb *TCB_NextTCBAEL;       ///< \brief Pointer to linked list of waiting events of this task
    struct gs_scb *TCB_NextTCBASL;       ///< \brief Pointer to the Linked list of signals
    struct gs_scb *TCB_NextTCBPSL;       ///< \brief Pointer to signals waiting to execute
    G_UINT32 TCB_AssocPCB;               ///< \brief Processor assigned this task (0 is no assigned)
	GS_RCB *TCB_INTNumber;               ///< \brief IRQ number if it is a ISR TCB
	struct gs_lcb *TCB_RDY_LCB_Index;    ///< \brief pointer to the ready list that should be inserted
	volatile G_UINT32 TCB_MTX_NESTED;    ///< \brief Count for Mutex nesting of the task 
	char TCB_description[G_TCB_DESCRIPTION_LENGTH];

#ifndef __niosX_arch__                   // Windows version
	ucontext_t uctx;                     // Windows version
	void *uctx_stack;                    // Windows version
#endif                                   // Windows version

	
    // Fields for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    struct gs_tcb  *TCB_NEXT_TCBs;     ///< \brief Pointer to next TCB structure. Used for debugging purposes.
    struct gs_tcb  *TCB_PREV_TCBs;     ///< \brief Pointer to previous TCB structure. Used for debugging purposes.
#endif
};

enum list_type { ECBASL, KCBASL, KCBPSL, RCBASL, TCBPSL, TCBASL, ECBTL, RCBGEL, RCBWEL, TCBRDYL, TCBRUNL };

struct GK_SAMPLE_FUNCTION {
	G_UINT64 start_time;
	G_UINT64 accumulate_time;
	G_UINT64 max_time;
	G_UINT32 previous_function;
	char function_name[40];
};

/** \brief Offset subtracted from a sampling ID to obtain the SAMPLINGS[] index.
 *  All valid sampling IDs are in the range (G_SAMPLING_BASE_ID, G_SAMPLING_BASE_ID + G_SAMPLING_MAX_FUNCTIONS). */
#define G_SAMPLING_BASE_ID        10000U
/** \brief Capacity of the SAMPLINGS[] array â€” maximum number of instrumented functions. */
#define G_SAMPLING_MAX_FUNCTIONS  100U

extern struct GK_SAMPLE_FUNCTION SAMPLINGS[G_SAMPLING_MAX_FUNCTIONS];
extern volatile G_UINT32 current_function;
extern volatile G_UINT64 current_time;
extern volatile G_UINT64 maximum_time;
extern volatile G_UINT32 maximum_time_function;

/**
 * \brief Mark the start of an instrumented critical-section function.
 *
 * \param [in] number  Unique sampling ID for this function. Must satisfy
 *                     G_SAMPLING_BASE_ID < number <
 *                     G_SAMPLING_BASE_ID + G_SAMPLING_MAX_FUNCTIONS.
 * \note Expands to a no-op when G_DEBUG_WHILEFOREVER_ENABLE == 0.
 *       Must be paired with GK_SAMPLE_FUNCTION_END(number) before the critical
 *       section is released. Accumulates wall-clock ticks in the global SAMPLINGS[] array.
 */
/**
 * \brief Mark the end of an instrumented critical-section function.
 *
 * \param [in] number  Same sampling ID used in the matching GK_SAMPLE_FUNCTION_BEGIN call.
 * \note Expands to a no-op when G_DEBUG_WHILEFOREVER_ENABLE == 0.
 *       Updates max_time and accumulate_time in SAMPLINGS[number - G_SAMPLING_BASE_ID],
 *       restores the previous_function context, and runs gkm_Check_GeMRTOS() as a
 *       post-condition integrity check.
 */
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)

	/* Must stay a macro (M5, 2026-08-16): debug-only instrumentation reading
	 * and writing caller-scope locals (current_time, current_function) that
	 * a function could not see. `number` was unparenthesised throughout --
	 * every real call site (~120, across every kernel .c file) passes a
	 * bare integer literal, so this was latent, not triggered. */
	#define GK_SAMPLE_FUNCTION_BEGIN(number)  \
		PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__); \
		current_time = gk_SystemTimeGet(); \
		PRINT_ASSERT((((number) - G_SAMPLING_BASE_ID > 0) && ((number) - G_SAMPLING_BASE_ID < G_SAMPLING_MAX_FUNCTIONS)), "number= %u",(unsigned int) (number)); \
		if (current_function != 0) { \
			PRINT_ASSERT(((current_function - G_SAMPLING_BASE_ID > 0) && (current_function - G_SAMPLING_BASE_ID < G_SAMPLING_MAX_FUNCTIONS)), "current_function= %u",current_function); \
			SAMPLINGS[current_function - G_SAMPLING_BASE_ID].accumulate_time += (current_time - SAMPLINGS[current_function - G_SAMPLING_BASE_ID].start_time); \
		} \
		if (SAMPLINGS[(number) - G_SAMPLING_BASE_ID].max_time == 0) {\
			strncpy(SAMPLINGS[(number) - G_SAMPLING_BASE_ID].function_name, __FUNCTION__, sizeof(SAMPLINGS[0].function_name) - 1U); \
			SAMPLINGS[(number) - G_SAMPLING_BASE_ID].function_name[sizeof(SAMPLINGS[0].function_name) - 1U] = '\0'; \
		}\
		SAMPLINGS[(number) - G_SAMPLING_BASE_ID].start_time = current_time; \
		SAMPLINGS[(number) - G_SAMPLING_BASE_ID].accumulate_time = 0; \
		SAMPLINGS[(number) - G_SAMPLING_BASE_ID].previous_function = current_function; \
		current_function = (number); \


	/* Must stay a macro (M5, 2026-08-16): see GK_SAMPLE_FUNCTION_BEGIN above --
	 * same caller-scope-locals reason, same latent-not-triggered parenthesis fix. */
	#define GK_SAMPLE_FUNCTION_END(number)  \
		PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__); \
		current_time = gk_SystemTimeGet(); \
		PRINT_ASSERT((((number) - G_SAMPLING_BASE_ID > 0) && ((number) - G_SAMPLING_BASE_ID < G_SAMPLING_MAX_FUNCTIONS)), "number= %u",(unsigned int) (number)); \
		SAMPLINGS[(number) - G_SAMPLING_BASE_ID].accumulate_time += (current_time - SAMPLINGS[(number) - G_SAMPLING_BASE_ID].start_time); \
		if (SAMPLINGS[(number) - G_SAMPLING_BASE_ID].accumulate_time > SAMPLINGS[(number) - G_SAMPLING_BASE_ID].max_time) { \
			SAMPLINGS[(number) - G_SAMPLING_BASE_ID].max_time = SAMPLINGS[(number) - G_SAMPLING_BASE_ID].accumulate_time; \
			if (SAMPLINGS[(number) - G_SAMPLING_BASE_ID].max_time > maximum_time) {\
				maximum_time = SAMPLINGS[(number) - G_SAMPLING_BASE_ID].max_time; \
				maximum_time_function = (number); \
			} \
		} \
		SAMPLINGS[(number) - G_SAMPLING_BASE_ID].accumulate_time = 0; \
		current_function = SAMPLINGS[(number) - G_SAMPLING_BASE_ID].previous_function; \
		if (current_function != 0) { \
			PRINT_ASSERT(((current_function - G_SAMPLING_BASE_ID > 0) && (current_function - G_SAMPLING_BASE_ID < G_SAMPLING_MAX_FUNCTIONS)), "current_function= %u",current_function); \
			SAMPLINGS[current_function - G_SAMPLING_BASE_ID].start_time = current_time; \
		} \
		if (!(gm_TimeCountersResetGet())) PRINT_ASSERT((gkm_Check_GeMRTOS(100000) == G_TRUE),"ERROR in GK_SAMPLE_FUNCTION_END number= %u\n", (unsigned int) (number));

#else
	#define GK_SAMPLE_FUNCTION_BEGIN(number)
	#define GK_SAMPLE_FUNCTION_END(number)  
#endif


/*
*********************************************************************************************************
*    SYSTEM VARIABLES
*********************************************************************************************************
*/
extern const unsigned int PRC_MASK[];

extern volatile G_UINT32 G_Testing_Counter;
extern volatile G_UINT32  gs_sizeof_G_PCBTbl;
extern volatile void     *gs_addressof_G_PCBTbl;
extern volatile G_UINT32  gs_offsetof_PCB_EXECTCB;
extern volatile G_UINT32  gs_offsetof_TCB_StackPointer;
extern volatile G_UINT32  gs_offsetof_PCB_IDLETCB;
extern volatile G_UINT32  gs_offsetof_PCB_SUSPENDED_STK_PTR;

extern GS_KCB g_kcb;

extern t_semaphore_resource *gu_printf_sem;
	
//Variable to interface C and assembler
extern volatile G_UINT32  G_SCB_PENDING;
extern volatile G_UINT32  G_SCB_CODE;
extern volatile G_UINT32  G_SCB_ARG;

extern volatile G_UINT32 G_TASK_TYPE_DEFAULT;
extern volatile GS_LCB *G_TASK_LCB_DEFAULT;
extern volatile G_UINT32 G_TASK_LCB_DEFAULT_PRIORITY;
extern volatile G_UINT64 G_TASK_PRIORITY_DEFAULT;
extern volatile G_UINT64 G_TASK_PERIOD_DEFAULT;

// Core functions definitions
/** \brief Create and initialise a new scheduling list of the given type.
 *  \param [in] lcb_type  List type: GS_LCBTypeFP (fixed priority) or
 *                       GS_LCBTypeEDF (earliest deadline first).
 *  \return Pointer to the new LCB; NULL on pool exhaustion. */
GS_LCB  *gu_SchedulingListCreate(enum lcbtype lcb_type);
/** \brief Allocate a free LCB from the pool and initialise it with the given type.
 *  \param [in] lcbtype  List type for the newly allocated LCB.
 *  \return Pointer to the new LCB; NULL on pool exhaustion. */
GS_LCB  *gk_Get_LCB(enum lcbtype lcb_type);
/** \brief Initialise all processor control blocks to their post-reset state.
 *  \return G_TRUE on success. */
G_UINT32 gk_Init_PCBs(void);
/** \brief Create the IDLE task for each processor in the system.
 *  \return G_TRUE on success. */
G_UINT32 gk_Create_IDLE_tasks(void);
/** \brief Initialise all static object pools (TCB, ECB, RCB, SCB, RRDS, LCB, PCBAssocLCB).
 *  Must be called before the first pool allocation request. */
void     gk_Init_Pools(void);
/** \brief Read the hardware system-time counter (kernel internal; must be called within critical section).
 *  \return Current system time in hardware ticks. */
G_UINT64 gk_SystemTimeGet (void);
/** \brief Read the hardware system-time counter (user-callable; enters/exits critical section).
 *  \return Current system time in hardware ticks. */
G_UINT64 gu_SystemTimeGet(void) ;
/** \brief Allocate a free PCBAssocLCB from the pool; returns NULL on exhaustion.
 *  Links a processor control block to a scheduling list. */
GS_PCBAssocLCB *gk_Get_PCBAssocLCB(void);
/** \brief Link a PCBAssocLCB into the kernel's PCBAssocLCB free list.
 *  \param [in] ppcbalcb  Pointer to the PCBAssocLCB to link.
 *  \return G_TRUE on success. */
G_UINT32 gk_PCBAssocLCBFL_Link(GS_PCBAssocLCB *ppcbalcb);
/** \brief Evaluate all processors and assign the one with the lowest current load.
 *  \return G_TRUE on success. */
G_UINT32 gk_SetLowestProcessor (void);

/************************************************************************************
 *  KERNEL OPERATIONS ON THE CONTROLLER
 *
 *  These read the Kernel Control Block and then write a controller register.
 *  They lived in gemrtos_primitives.h, which is the hardware-access layer and is
 *  included before this header; that worked only while they were macros, whose
 *  bodies are expanded at the call site. They are defined here, where the kernel
 *  structures they dereference are complete.
 ************************************************************************************/



/** \def gk_NextOccTimeProcessor
 *  \brief Sets the Next Occurrence Time Processor field in the GRTOS controller,
 *  or 0 if the next-occurrence-time task is not executing.
 *  \exception par_following gk_NextOccTimeProcessor does not have parameters.
 *  \exception class_name IRQ_Management
 *  \exception prototype gk_NextOccTimeProcessor()
 *  \return gk_NextOccTimeProcessor does not return any value.
 *  \remark gk_NextOccTimeProcessor
 *
 *  Renamed from gm_NextOccTimeProcessor (2026-08-16): per the naming
 *  convention in the Manual (ch. 13, "API Naming Conventions"), gm_ is for
 *  hardware-register primitives; gk_ is for kernel-internal logic, not part
 *  of the public API. This function walks g_kcb (the Kernel Control Block --
 *  ECB to TCB to PCB association) to decide what to write, and only then
 *  performs one register write. Its single call site (gk_START_KERNEL, via
 *  the interrupt-return path) already holds the mutex on entry -- it does
 *  not manage its own critical section, so this is not a gu_ candidate.
 */
/* #define gm_NextOccTimeProcessor()                                    \
    unsigned int processor = 0;                                      \
    if (g_kcb.KCB_NextECBTL->ECB_AssocTCB != (struct gs_tcb  *) 0)   \
        processor = g_kcb.KCB_NextECBTL->ECB_AssocTCB->TCB_AssocPCB; \
    gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_NTX_TM_PRC_BIT ) | (processor & 0xF)); */
/* gm_NextOccTimeProcessor and gm_ProcessorWaitForIrq moved to gemrtos_core.h:
 * they read g_kcb, so a function body compiled here cannot see the complete
 * kernel structures it dereferences.
 *
 * The two gm_GeMRTOSCriticalSection* operations below read g_kcb as well and
 * stay here, as macros, deliberately. This header is what the host test harness's stubs
 * replace wholesale so the kernel can run single-threaded on the host, and
 * neutralising synchronisation is precisely what that substitution exists for.
 * Moving them into gemrtos_core.h would place them in the layer under test,
 * which is pulled in with #include_next and cannot be replaced. As macros they
 * expand at the call site, where the kernel types are complete. */
static inline void gk_NextOccTimeProcessor(void)
{
    G_UINT32 processor = 0u;

    if (g_kcb.KCB_NextECBTL->ECB_AssocTCB != (struct gs_tcb *)0)
    {
        processor =
            (G_UINT32)g_kcb.KCB_NextECBTL->ECB_AssocTCB->TCB_AssocPCB;
    }

    const G_UINT32 value =
        ((G_UINT32)1u << CTRL_NTX_TM_PRC_BIT) |
        (processor & (G_UINT32)0x0Fu);

    gm_IOWR(GEMRTOS_DRIVER_BASE,
            ADDR_CTRL_STATUS,
            value);
}    


/**
 *  \def gm_GeMRTOSCriticalSectionEnter()
 *  \brief The gm_GeMRTOSCriticalSectionEnter() macro defines the entry point into a critical section for the management of kernel data. It is designed to ensure that modifications to shared kernel resources occur safely, although it may be interrupted while waiting for the GeMRTOS controller mutex. This macro should be utilized whenever there is a need to modify kernel data to prevent data corruption and maintain system stability.
 *  \exception par_following The gm_GeMRTOSCriticalSectionEnter() macro does not accept any parameters.
 *  \exception class_name Critical_Section
 *  \exception prototype gm_GeMRTOSCriticalSectionEnter()
 *  \return The gm_GeMRTOSCriticalSectionEnter() macro does not return any value but blocks the code execution until the GeMRTOS controller mutex is granted.
 *  \sa gm_GeMRTOSCriticalSectionExit(), gm_IrqEnable, gm_IrqDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_GeMRTOSCriticalSectionEnter()
 */
/* #define gm_GeMRTOSCriticalSectionEnter()  \
    if (!(gm_TimeCountersResetGet())) { \
        GRTOS_CMD_CRITICAL_SECTION_GET; \
        if ((gm_KernelModeGet() == G_FALSE)) { \
            g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED++; \
        } \
    } */
// static inline void gm_GeMRTOSCriticalSectionEnter()(void)
// {
//     if (gm_TimeCountersResetGet() == 0u)
//     {
//         GRTOS_CMD_CRITICAL_SECTION_GET;
// 
//         if (gm_KernelModeGet() == G_FALSE)
//         {
//             g_kcb.G_PCBTbl[gm_ProcessorId() - 1u]
//                 .PCB_EXECTCB->TCB_MTX_NESTED++;
//         }
//     }
// }


/** \def gm_GeMRTOSCriticalSectionExit()
 *  \brief The gm_GeMRTOSCriticalSectionExit() macro exits the critical section from the current processor, allowing other processes enter. It is essential to use this macro in all user functions that execute kernel functions or modify kernel data, ensuring that the critical section is properly released and preventing potential deadlocks or resource contention.
 *  \exception par_following The gm_GeMRTOSCriticalSectionExit() macro does not accept any parameters.
 *  \exception class_name Critical_Section
 *  \exception prototype gm_GeMRTOSCriticalSectionExit()
 *  \return The gm_GeMRTOSCriticalSectionExit() macro does not return any value.
 *  \sa gm_GeMRTOSCriticalSectionEnter(), gm_IrqEnable, gm_IrqDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_GeMRTOSCriticalSectionExit()
 */
/* #define gm_GeMRTOSCriticalSectionExit()  \
    if (!(gm_TimeCountersResetGet())) { \
        if ((gm_KernelModeGet() == G_FALSE)) { \
            g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED--; \
            gm_MutexNestedValue(g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED); \
        } \
        GRTOS_MTX_RLS; \
    } */
// static inline void gm_GeMRTOSCriticalSectionExit()(void)
// {
//     if (gm_TimeCountersResetGet() == 0u)
//     {
//         if (gm_KernelModeGet() == G_FALSE)
//         {
//             G_PCB * const pcb =
//                 &g_kcb.G_PCBTbl[gm_ProcessorId() - 1u];
// 
//             pcb->PCB_EXECTCB->TCB_MTX_NESTED--;
// 
//             gm_MutexNestedValue(pcb->PCB_EXECTCB->TCB_MTX_NESTED);
//         }
//         GRTOS_MTX_RLS;
//     }
// }


/** \def gu_ProcessorWaitForIrq
 *  \brief Halts the calling processor until an interrupt occurs on one of the
 *  specified masked IRQs. Useful for a user task or driver that wants to wait
 *  on a specific interrupt subset rather than the kernel's own generic
 *  gm_ProcessorHalt (which halts until any interrupt).
 *  \exception par_following gu_ProcessorWaitForIrq accepts the following parameter:
 *  \exception class_name Processor
 *  \exception prototype gu_ProcessorWaitForIrq(IRQ_mask)
 *  \param [in] IRQ_mask A mask of the DIRQs that the processor will wait for. This mask specifies which interrupts should wake the processor from its halted state. The interrupt should be disabled in order to be used to wake up the processor.
 *  \return gu_ProcessorWaitForIrq does not return any value.
 *  \sa gm_PrcInterruptDisable, gm_PrcInterruptEnable, gm_ProcessorInterrupt, gm_ProcessorHalt, gm_ProcessorId()
 *  \remark gu_ProcessorWaitForIrq
 *  \todo Create another macro to return the processor to available type
 *
 *  Renamed from gm_ProcessorWaitForIrq (2026-08-16): per the naming
 *  convention in the Manual (ch. 13, "API Naming Conventions"), gm_/gk_ both
 *  require the mutex to already be held on entry -- but this function enters
 *  and exits its own critical section (below), the defining trait of gu_
 *  ("manages the GeMRTOS Mutex internally, must not be called from within a
 *  critical section"). It also touches g_kcb and calls gk_SetLowestProcessor()
 *  directly, kernel-internal in body -- but its calling convention, not its
 *  body, is what the prefix describes. Zero call sites in canonical at the
 *  time of rename, so nothing needed updating to match.
 */
/* #define gm_ProcessorWaitForIrq(IRQ_mask)                                            \
	gm_GeMRTOSCriticalSectionEnter();                                               \
		g_kcb.G_PCBTbl[gm_ProcessorId()-1].PCBType  = (int) GS_PCBType_UNAVAILABLE; \
		gk_SetLowestProcessor();                                             \
	gm_GeMRTOSCriticalSectionExit();                                                 \
	gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_0, IRQ_mask) */
static inline void gu_ProcessorWaitForIrq(G_UINT32 irq_mask)
{
    gm_GeMRTOSCriticalSectionEnter();

    g_kcb.G_PCBTbl[gm_ProcessorId() - 1u].PCBType =
        GS_PCBType_UNAVAILABLE;

    gk_SetLowestProcessor();

    gm_GeMRTOSCriticalSectionExit();

    gm_IOWR(GEMRTOS_DRIVER_PROCESSOR_BASE,
            ADDR_REG_0,
            irq_mask);
}    
    

/** \brief Associate a processor with a scheduling list at the given priority level.
 *  \param [in] plcb      Scheduling list to associate.
 *  \param [in] cpu_id    One-based processor index (1..G_NUMBER_OF_PCB).
 *  \param [in] priority  Priority level for the processor on this list.
 *  \return G_TRUE on success; G_FALSE if the association already exists or parameters are invalid. */
G_UINT32 gu_SchedulingListAssociateProcessor(GS_LCB *plcb, G_UINT32 cpu_id, G_UINT32 priority);


/***************************************************************/
/* INTERNAL SYSTEM FUNCTIONS                                   */
/***************************************************************/
/** \brief Verify the consistency of all GeMRTOS data structures at start-up.
 *  \return G_TRUE if all checks pass; halts via G_DEBUG_WHILEFOREVER on failure. */
G_UINT32 gk_SYSTEM_START_UP_CHECK(void);
/** \brief Perform processor-1-specific start-up checks and signal readiness to other processors. */
void gk_PROCESSOR_NO1_START_UP_CHECK(void);
/** \brief Return the next runnable TCB for the given processor control block.
 *  \param [in] ppcb  Processor control block to query.
 *  \return Pointer to the highest-priority ready TCB; NULL if none is available.
 *  \note Called both by the kernel dispatcher (gemrtos_kernel.c) and directly
 *        by the host test harness (gemrtos_test_stubs.c, a development-tree file that is not published),
 *        which models the dispatch tail without compiling gemrtos_kernel.c
 *        (that file needs assembly context-switch support the host build
 *        does not have). External linkage is required for that reason. */
GS_TCB *gk_PCB_GetNextTCB(GS_PCB *ppcb);
/** \brief Return the TCB currently executing on the calling processor.
 *  \return Pointer to the current TCB. */
GS_TCB *gk_PCB_GetCurrentTCB(void);

/***************************************************************/
/* debug function                                              */
/***************************************************************/

/** \brief Sort-order descriptor returned by gkm_ECB_list_ORDER().
 *
 * Replaces the bare 0/1/2/3 integer that gkm_ECB_list_ORDER() formerly
 * returned.  Defined here so that the declaration below and the
 * definition in monitor.c share the same type â€” eliminating the
 * enum/integer mismatch warning that arose when the typedef lived only
 * inside monitor.c's #if G_DEBUG_WHILEFOREVER_ENABLE == 1 block. */
typedef enum {
    GS_LIST_ORDER_DECREASING     = 0, /**< sorted by decreasing ECBValue (e.g. granted semaphore list) */
    GS_LIST_ORDER_INCREASING     = 1, /**< sorted by increasing ECBValue (e.g. timed-event list)      */
    GS_LIST_ORDER_NOT_APPLICABLE = 2, /**< ordering not applicable (free ECB or ISR type)              */
    GS_LIST_ORDER_UNRECOGNISED   = 3  /**< ECBType not recognised by this function                     */
} GS_ListOrder;

/** \brief Return the root ECB of the list that contains pevent (monitor helper).
 *  \param [in] pevent  Any ECB within the list to query.
 *  \return Pointer to the root ECB of the list. */
GS_ECB      *gkm_ECB_list_ROOT(const GS_ECB *pevent);
/** \brief Return the sort order of the list that contains pevent (monitor helper).
 *  \param [in] pevent  Any ECB within the list to classify.
 *  \return GS_ListOrder enum value describing the ordering of the list. */
GS_ListOrder gkm_ECB_list_ORDER(const GS_ECB *pevent);
/** \brief Return the root TCB of the list that contains ptcb (monitor helper).
 *  \param [in] ptcb  Any TCB within the list to query.
 *  \return Pointer to the root TCB of the list. */
GS_TCB      *gkm_TCB_list_ROOT(const GS_TCB *ptcb);

/***************************************************************/
/*  LIST AND DATA STRUCTURES PROTOTYPES                        */
/***************************************************************/

/** \brief Link pevent_b into the associated-event list of pevent_a.
 *  \param [in] pevent_a  ECB that owns the associated-event list.
 *  \param [in] pevent_b  ECB to insert into the list.
 *  \return G_TRUE on success. */
G_UINT32  gk_ECBAEL_Link(GS_ECB * const pevent_a, GS_ECB * const pevent_b);
/** \brief Remove pevent from its associated-event list.
 *  \param [in] pevent  ECB to remove.
 *  \return G_TRUE on success. */
G_UINT32  gk_ECBAEL_Remove(GS_ECB * const pevent);
/** \brief Link psignal into the associated-signal list of pevent.
 *  \param [in] pevent   ECB that owns the signal list.
 *  \param [in] psignal  SCB to insert.
 *  \return G_TRUE on success. */
G_UINT32  gk_ECBASL_Link(GS_ECB * const pevent, GS_SCB * const psignal);
/** \brief Remove psignal from the associated-signal list of pevent.
 *  \param [in] pevent   ECB that owns the signal list.
 *  \param [in] psignal  SCB to remove.
 *  \return G_TRUE on success. */
G_UINT32  gk_ECBASL_Unlink(GS_ECB *pevent, GS_SCB *psignal);

/** \brief Find the first SCB of the given SignalType in pevent's associated-signal list.
 *  \param [in] pevent      ECB whose signal list is searched.
 *  \param [in] SignalType  Signal type to match.
 *  \return Pointer to the matching SCB; NULL if not found. */
GS_SCB   *gk_ECBASL_GetSCB(GS_ECB *pevent, G_UINT32 SignalType);

/** \brief Allocate a free ECB from the pool and associate it with ptcb.
 *  \param [in] ptcb  Task that will own the new ECB.
 *  \return Pointer to the new ECB; NULL on pool exhaustion. */
GS_ECB   *gk_ECB_GetFree(GS_TCB *ptcb);

/** \brief Insert pevent into the global time-ordered event list (sorted by ECBValue.i64).
 *  \param [in] pevent  ECB to insert; ECBValue.i64 must be set to the expiry time.
 *  \return G_TRUE on success. */
G_UINT32  gk_ECBTL_Link (GS_ECB *pevent);
/** \brief Remove pevent from the global time-ordered event list.
 *  \param [in] pevent  ECB to remove.
 *  \return G_TRUE on success. */
G_UINT32  gk_ECBTL_Unlink(GS_ECB *pevent);

/** \brief Link psignal into the kernel control block's associated-signal list.
 *  \param [in] psignal  SCB to insert.
 *  \return G_TRUE on success. */
G_UINT32   gk_KCBASL_Link(GS_SCB *psignal);
/** \brief Remove psignal from the kernel control block's associated-signal list.
 *  \param [in] psignal  SCB to remove.
 *  \return G_TRUE on success. */
G_UINT32  gk_KCBASL_Unlink(GS_SCB *psignal);
/** \brief Link psignal into the kernel control block's pending-signal list.
 *  \param [in] psignal  SCB to insert.
 *  \return G_TRUE on success. */
G_UINT32  gk_KCBPSL_Link(GS_SCB *psignal);
/** \brief Remove psignal from the kernel control block's pending-signal list.
 *  \param [in] psignal  SCB to remove.
 *  \return G_TRUE on success. */
G_UINT32  gk_KCBPSL_Unlink(GS_SCB *psignal);

/** \brief Insert plcb into the kernel's active scheduling-list chain.
 *  \param [in] plcb  Scheduling list to insert.
 *  \return G_TRUE on success. */
G_UINT32  gk_LCBL_Link(GS_LCB *plcb);
/** \brief Find the first SCB of the given SignalType in the KCB's associated-signal list.
 *  \param [in] SignalType  Signal type to match.
 *  \return Pointer to the matching SCB; NULL if not found. */
GS_SCB   *gk_KCBASL_GetSCB(G_UINT32 SignalType);

/** \brief Mark processor processorID as free in the LCB free-processor list.
 *  \param [in] processorID  One-based processor index (1..G_NUMBER_OF_PCB).
 *  \return G_TRUE on success. */
G_UINT32    gk_LCBFPL_Link(G_UINT32 processorID);
/** \brief Remove processorID from the LCB free-processor list (processor is now busy).
 *  \param [in] processorID  One-based processor index (1..G_NUMBER_OF_PCB).
 *  \return G_TRUE on success. */
G_UINT32    gk_LCBFPL_Unlink(G_UINT32 processorID);

/** \brief Remove plcb from the kernel's active scheduling-list chain.
 *  \param [in] plcb  Scheduling list to remove.
 *  \return G_TRUE on success. */
G_UINT32    gk_LCBL_Unlink(GS_LCB *plcb);

/** \brief Allocate a free RCB from the pool.
 *  \return Pointer to the new RCB; NULL on pool exhaustion. */
GS_RCB  *gk_RCB_GetFree(void);
/** \brief Return presource to the RCB free list.
 *  \param [in] presource  RCB to reclaim.
 *  \return G_TRUE on success. */
G_UINT32    gk_RCBFL_Link(GS_RCB *presource);

/** \brief Release ptcb from its current wait state and place it in the ready list.
 *  \param [in] ptcb  Task to release.
 *  \return G_TRUE on success. */
G_UINT32 gk_TASK_RELEASE(GS_TCB *ptcb);

/** \brief Insert pevent into a generic resource event list with the given initial state.
 *  \param [in,out] root       Pointer to the list root pointer.
 *  \param [in]     presource  Resource that owns the list.
 *  \param [in]     ptcb       Task associated with this event.
 *  \param [in]     pevent     ECB to insert (may be NULL â€” pool allocation done internally).
 *  \param [in]     ECBState   Initial state to assign to the ECB.
 *  \return Pointer to the inserted ECB; NULL on failure. */
GS_ECB *gk_RCBXEL_Link(GS_ECB **root, GS_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent, GE_ECBState ECBState);
/** \brief Insert pevent into the resource's granted-event list (task has been granted the resource).
 *  \param [in] presource  Resource that grants the event.
 *  \param [in] ptcb       Requesting task.
 *  \param [in] pevent     ECB to insert (may be NULL).
 *  \return Pointer to the inserted ECB; NULL on failure. */
GS_ECB *gk_RCBGEL_Link(GS_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent);
/** \brief Remove pevent from the resource's granted-event list.
 *  \param [in] pevent  ECB to remove. */
void    gk_RCBGEL_Unlink(GS_ECB *pevent);

/** \brief Link psignal into the resource's associated-signal list.
 *  \param [in] presource  Resource that owns the list.
 *  \param [in] psignal    SCB to insert.
 *  \return G_TRUE on success. */
G_UINT32    gk_RCBASL_Link(GS_RCB *presource, GS_SCB *psignal);
/** \brief Remove psignal from the resource's associated-signal list.
 *  \param [in] presource  Resource that owns the list.
 *  \param [in] psignal    SCB to remove.
 *  \return G_TRUE on success. */
G_UINT32    gk_RCBASL_Unlink(GS_RCB *presource, GS_SCB *psignal);
/** \brief Find the first SCB of the given SignalType in presource's associated-signal list.
 *  \param [in] presource   Resource whose signal list is searched.
 *  \param [in] SignalType  Signal type to match.
 *  \return Pointer to the matching SCB; NULL if not found. */
GS_SCB *gk_RCBASL_GetSCB(GS_RCB *presource, G_UINT32 SignalType);

/** \brief Find the first SCB of the given SignalType in an arbitrary SCB singly-linked list.
 *  \param [in] head_list   Head of the SCB list to search.
 *  \param [in] SignalType  Signal type to match.
 *  \return Pointer to the matching SCB; NULL if not found. */
GS_SCB *gk_GetSCB_from_list(GS_SCB *head_list, G_UINT32 SignalType);

/** \brief Insert pevent into the resource's waiting-event list (task is waiting for the resource).
 *  \param [in] presource  Resource the task is waiting on.
 *  \param [in] ptcb       Waiting task.
 *  \param [in] pevent     ECB to insert (may be NULL).
 *  \return Pointer to the inserted ECB; NULL on failure. */
GS_ECB *gk_RCBWEL_Link(GS_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent);
/** \brief Remove pevent from the resource's waiting-event list.
 *  \param [in] pevent  ECB to remove. */
void    gk_RCBWEL_Unlink(GS_ECB *pevent);

/** \brief Allocate a free RRDS from the pool.
 *  \return Pointer to the new RRDS; NULL on pool exhaustion. */
GS_RRDS *gk_RRDS_GetFree(void);
/** \brief Return prrds to the RRDS free list.
 *  \param [in] prrds  RRDS to reclaim.
 *  \return G_TRUE on success. */
G_UINT32    gk_RRDSFL_Link(GS_RRDS *prrds);

/** \brief Return psignal to the SCB free list.
 *  \param [in] psignal  SCB to reclaim.
 *  \return G_TRUE on success. */
G_UINT32    gk_SCBFL_Link(GS_SCB *psignal);
/** \brief Allocate a free SCB from the pool.
 *  \return Pointer to the new SCB; NULL on pool exhaustion. */
GS_SCB *gk_SCB_GetFree(void);
/** \brief Allocate a new SCB and copy all fields from psignal.
 *  \param [in] psignal  Source SCB to copy.
 *  \return Pointer to the new copy; NULL on pool exhaustion. */
GS_SCB *gk_SCB_Copy(GS_SCB *psignal);

/** \brief Prepend pscb_pending to the application-signal list rooted at *pscb_root.
 *  \param [in,out] pscb_root     Pointer to the list root; updated to point to pscb_pending.
 *  \param [in]     pscb_pending  SCB to prepend. */
void    gk_SCBAPSL_Link(GS_SCB **pscb_root, GS_SCB *pscb_pending);
/** \brief Remove pscb_pending from the application-signal list rooted at pscb_root.
 *  \param [in] pscb_root     Head of the application-signal list.
 *  \param [in] pscb_pending  SCB to remove.
 *  \return G_TRUE on success; G_FALSE if pscb_pending was not found. */
G_UINT32   gk_SCBAPSL_Unlink(GS_SCB *pscb_root, GS_SCB *pscb_pending);

/** \brief Allocate a free TCB and a stack of stack_size words from the pool.
 *  \param [in] stack_size  Number of words to allocate for the task stack.
 *  \return Pointer to the new TCB; NULL on pool exhaustion. */
GS_TCB *gk_TCB_GetFree(unsigned int stack_size);
/** \brief Remove ptcb from whichever list it currently belongs to (generic unlink).
 *  \param [in] ptcb  Task to unlink.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCB_Unlink(GS_TCB *ptcb);

/** \brief Initialise the TCB free list (called once at start-up, before gk_TCB_GetFree). */
void    gk_TCBFL_Init(void);
/** \brief Return ptcb to the TCB free list.
 *  \param [in] ptcb  TCB to reclaim.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBFL_Link(GS_TCB *ptcb);

/** \brief Associate pevent with ptcb in the task's associated-event list.
 *  \param [in] pevent  ECB to link.
 *  \param [in] ptcb    Task that will own the event.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBAEL_Link(GS_ECB *pevent, GS_TCB *ptcb);
/** \brief Remove pevent from its owning task's associated-event list.
 *  \param [in] pevent  ECB to unlink.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBAEL_Unlink(GS_ECB *pevent);

/** \brief Link psignal into ptcb's associated-signal list.
 *  \param [in] ptcb    Task that owns the signal list.
 *  \param [in] psignal SCB to insert.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBASL_Link(GS_TCB *ptcb, GS_SCB *psignal);
/** \brief Remove psignal from ptcb's associated-signal list.
 *  \param [in] ptcb    Task that owns the signal list.
 *  \param [in] psignal SCB to remove.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBASL_Unlink(GS_TCB *ptcb, GS_SCB *psignal);
/** \brief Find the first SCB of the given SignalType in ptcb's associated-signal list.
 *  \param [in] ptcb       Task whose signal list is searched.
 *  \param [in] SignalType Signal type to match.
 *  \return Pointer to the matching SCB; NULL if not found. */
GS_SCB *gk_TCBASL_GetSCB(GS_TCB *ptcb, G_UINT32 SignalType);
/** \brief Unlink ptcb from all lists it currently belongs to.
 *  \param [in] ptcb  Task to remove from all kernel lists.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCB_List_Unlink(GS_TCB *ptcb);
/** \brief Unlink pevent from all lists it currently belongs to.
 *  \param [in] pevent  ECB to remove from all kernel lists.
 *  \return G_TRUE on success. */
G_UINT32   gk_ECB_List_Unlink(GS_ECB *pevent);
/** \brief Return pevent to the ECB free list.
 *  \param [in] pevent  ECB to reclaim.
 *  \return G_TRUE on success. */
G_UINT32   gk_ECBFL_Link(GS_ECB *pevent);

/** \brief Link psignal into ptcb's pending-signal list.
 *  \param [in] ptcb    Task that owns the pending-signal list.
 *  \param [in] psignal SCB to insert.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBPSL_Link(GS_TCB *ptcb, GS_SCB *psignal);
/** \brief Remove psignal from ptcb's pending-signal list.
 *  \param [in] ptcb    Task that owns the pending-signal list.
 *  \param [in] psignal SCB to remove.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBPSL_Unlink(GS_TCB *ptcb, GS_SCB *psignal);
/** \brief Find the first SCB of the given SignalType in ptcb's pending-signal list.
 *  \param [in] ptcb       Task whose pending-signal list is searched.
 *  \param [in] SignalType Signal type to match.
 *  \return Pointer to the matching SCB; NULL if not found. */
GS_SCB *gk_TCBPSL_GetSCB(GS_TCB *ptcb, G_UINT32 SignalType);

/** \brief Insert ptcb into the ready list of its associated scheduling list, sorted by priority.
 *  \param [in] ptcb  Task to make ready.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBRDYL_Link(GS_TCB * const ptcb);
/** \brief Remove ptcb from the ready list of its associated scheduling list.
 *  \param [in] ptcb  Task to remove from the ready list.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBRDYL_Unlink(GS_TCB *ptcb);

/** \brief Insert ptcb into the running list of its assigned processor.
 *  \param [in] ptcb  Task to mark as running.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBRUNL_Link(GS_TCB *ptcb);
/** \brief Recompute ptcb's effective (inherited) priority and reposition it in the ready list.
 *  \param [in] ptcb  Task whose priority may have changed via inheritance.
 *  \return G_TRUE on success. */
G_UINT32 gk_TCBUpdate_Priority( GS_TCB *ptcb);
/** \brief Remove ptcb from the running list of its assigned processor.
 *  \param [in] ptcb  Task to remove from the running list.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBRUNL_Unlink(GS_TCB *ptcb);

/** \brief Insert ptcb into the waiting list with the given initial wait state.
 *  \param [in] ptcb   Task to place in the waiting list.
 *  \param [in] state  Wait state to assign (e.g. GS_TCBState_WAITING).
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBWL_Link(GS_TCB *ptcb, unsigned int state);
/** \brief Remove ptcb from the waiting list.
 *  \param [in] ptcb  Task to remove.
 *  \return G_TRUE on success. */
G_UINT32    gk_TCBWL_Unlink(GS_TCB *ptcb);

/***************************************************************/
/*         TASK RELATED FUNCTIONS                              */
/***************************************************************/
// GS_TCB *gk_TASK_Create(void);
/** \brief Kill ptcb: unlink it from all lists and return it to the TCB free pool.
 *  \param [in] ptcb  Task to kill.
 *  \return G_TRUE on success. */
G_UINT32    gk_TASK_KILL(GS_TCB *ptcb);
/** \brief Suspend ptcb: move it out of the running/ready list and into a wait state.
 *  \param [in] ptcb  Task to suspend.
 *  \return G_TRUE on success; G_FALSE when the processor running ptcb could not
 *          be acquired, in which case the task is left unchanged. */
G_UINT32    gk_KERNEL_TASK_SUSPEND(GS_TCB *ptcb);
/** \brief Suspend the currently running task on this processor, setting task_state as its new state.
 *  \param [in] task_state  Wait-state to assign (e.g. GS_TCBState_WAITING). */
void    gk_KERNEL_TASK_SUSPEND_CURRENT(enum tcbstate task_state);
/** \brief Search presource's granted-event list for an ECB owned by ptcb.
 *  \param [in] presource  Resource whose granted-event list is searched.
 *  \param [in] ptcb       Task to locate.
 *  \return Pointer to the ECB if found; NULL otherwise. */
GS_ECB *gk_TCB_in_RCBGEL(GS_RCB *presource, GS_TCB *ptcb);
/** \brief Update ptcb's priority for the given task state and invoke any registered callbacks.
 *  \param [in] ptcb        Task whose priority is updated.
 *  \param [in] task_state  Task state context used for priority computation.
 *  \return G_TRUE on success. */
G_UINT32    gk_TaskPrioritySetCallback(GS_TCB *ptcb, enum tcbstate task_state);
/** \brief Compute ptcb's effective (inherited) priority given a hypothetical task state.
 *  \param [in] ptcb        Task whose inherited priority is computed.
 *  \param [in] task_state  Task state context for priority resolution.
 *  \return Computed priority value (higher value = lower priority in GeMRTOS). */
G_UINT64   gk_TaskInheritedPriority(GS_TCB *ptcb, enum tcbstate task_state);

/***************************************************************/
/*  PER-PROCESSOR WATCHDOG RECOVERY                             */
/***************************************************************/
/* See Industrial_standard/reports/ANALYSIS-processor-watchdog-reset.md for
 * the full design. Summary: a per-processor hardware watchdog
 * (gm_WatchogTimeoutSet/gm_PrcWatchdogEnable/gm_PrcWatchdogDisable) resets a
 * processor that stops halting (a hung task, by design -- if it ever fires
 * while that processor holds the GeMRTOS mutex, that is a kernel defect out
 * of this watchdog's scope, not something this recovery handles). On
 * recovery, gk_START_KERNEL restores the kernel's own invariants first
 * (unlinks/frees the abandoned task, or fixes IDLE's stale TCBState) --
 * unconditionally, whether or not a handler is registered. Only after that
 * is the optional user handler (if any) invoked, with a read-only snapshot,
 * never a live pointer into kernel structures. */

/** \brief Outcome the registered watchdog-recovery handler selects.
 *  \ingroup g_watchdog_action */
enum g_watchdog_action {
	/** Default when no handler is registered: kernel invariants are already
	 *  restored (see above) -- continue normally, same as before this
	 *  feature existed. */
	G_WATCHDOG_ACTION_RESUME = 0,
	/** Treat the recovery as fatal: halt with diagnostics, the same
	 *  PRINT_ASSERT-style halt used for every other "assumption violated"
	 *  condition in this project. */
	G_WATCHDOG_ACTION_HALT
};

/** \brief Read-only snapshot of the task a watchdog recovery abandoned.
 *  \details Captured from the abandoned GS_TCB BEFORE the kernel's own
 *  cleanup runs (gk_TASK_KILL frees/reuses the TCB; IDLE's TCB is reused in
 *  place but its transient fields are not meaningful once rearmed) -- the
 *  handler always receives this already-copied snapshot, never a pointer
 *  into kernel structures. Fields are a direct subset of struct gs_tcb;
 *  no fields are synthesized. */
struct g_watchdog_report {
	G_UINT32 CPUID;                              /**< Processor that recovered. */
	G_UINT32 was_idle;                           /**< Non-zero if the abandoned task was this processor's IDLE task. */
	volatile void *TaskCode;                     /**< Abandoned task's entry function (TCB_TaskCode). */
	volatile void *TaskArg;                      /**< Abandoned task's argument (TCB_TaskArg). */
	char TaskDescription[G_TCB_DESCRIPTION_LENGTH]; /**< Copy of TCB_description. */
	G_UINT64 TaskReadyPriority;                  /**< Copy of TCBReadyPriority. */
	G_UINT64 TaskRunPriority;                    /**< Copy of TCBRunPriority. */
};

/** \brief Signature of the system-wide watchdog-recovery handler.
 *  \param [in] report  Read-only snapshot of the abandoned task; valid only
 *                       for the duration of the call.
 *  \return The action the kernel should take next. */
typedef enum g_watchdog_action (*G_WatchdogHandler)(const struct g_watchdog_report *report);

/** \brief Registers (or clears) the system-wide watchdog-recovery handler.
 *  \details One global handler, not per-processor -- report->CPUID
 *  distinguishes which processor recovered if the handler needs to. Not
 *  thread/processor-safe against a concurrent gu_WatchdogHandlerSet() call;
 *  intended to be set once during application init, before any processor's
 *  watchdog is armed.
 *  \param [in] handler  Handler to install, or NULL to uninstall (restores
 *                        the default: always G_WATCHDOG_ACTION_RESUME). */
void gu_WatchdogHandlerSet(G_WatchdogHandler handler);

/***************************************************************/
/*  INTERNAL ROUTINES FOR TASK EXECUTION                       */
/***************************************************************/
/** \brief Transfer execution to the idle task's code when the kernel starts. */
void gk_KERNEL_TASK_START (void);


/* GEMRTOS_TEST_NO_AUTOSTART: defined ONLY by the host kernel-unit-
 * test build. That build links gemrtos_kernel.o into 14+ independent Unity
 * test binaries sharing one COMMON_OBJS -- a real GCC constructor/destructor
 * firing automatically before/after EVERY one of their own main()s would run
 * a full kernel boot/shutdown sequence (resetting g_kcb, and on exit,
 * dispatching through gk_KERNEL_TASK_START() into GRTOS_Start_Task(), whose
 * "never returns" contract makes a host stub fall through to
 * G_DEBUG_WHILEFOREVER = abort()) in every one of them, unconditionally.
 * Undefined (default) everywhere else -- embedded, Windows simulation, and
 * production all keep the automatic boot hook exactly as before; this guard
 * changes nothing for any build that existed before it was added. */
#ifdef GEMRTOS_TEST_NO_AUTOSTART
	/** \brief Kernel shutdown hook: called explicitly by the host test harness, not automatically. */
	void gk_START_KERNEL (void);
	/** \brief Kernel startup hook: called explicitly by the host test harness, not automatically. */
	void gk_INIT_KERNEL(void);
#else
	/** \brief Kernel shutdown hook (destructor): called by C runtime after main() returns. */
	void gk_START_KERNEL (void) __attribute__ ((destructor));
	/** \brief Kernel startup hook (constructor): called by C runtime before main(). */
	void gk_INIT_KERNEL(void)  __attribute__ ((constructor));
#endif


/** \brief IRQ entry handler: saves processor context, identifies the pending IRQ, and dispatches it. */
void gk_ENTRY_IRQ_HANDLER (void);
/** \brief Assembly stub: restore context and start executing the current task. */
void GRTOS_Start_Task(void);
/** \brief Assembly stub: low-level IRQ entry point called by the processor hardware vector. */
void grtos_irq_entry(void);
/** \brief Assembly stub: switch the stack pointer to the idle task's stack. */
void GRTOS_Change_SP_to_IDLE(void);


/* Platform dispatch â€” on embedded Nios, context-switch stubs are
 * real functions produced by the assembler (grtos_switch_niosX.S).
 * On Windows host builds, they are exposed as extern char[] arrays so that
 * pointer arithmetic over the stub body remains well-defined.
 * The comment "Do not compile for QuestaSim" is the original intent; the
 * guard is reused here to distinguish embedded from host builds. */
#ifdef __niosX_arch__        // Do not compile for QuestaSim
	void GRTOS_jump_to_reset_end(void);
    void GRTOS_jump_to_reset(void);
    void GRTOS_RETURN_CONTEXT(void);
	void GRTOS_RETURN_CONTEXT_END(void);
#else
    extern char GRTOS_jump_to_reset[128];
    #define GRTOS_jump_to_reset_end &GRTOS_jump_to_reset[28]
    extern char GRTOS_RETURN_CONTEXT[128];
    #define GRTOS_RETURN_CONTEXT_END  &GRTOS_RETURN_CONTEXT[28]
    
    
#endif                       // Do not compile for QuestaSim

/** \brief Assembly stub: common IRQ return path â€” restores processor context after IRQ handling. */
void GRTOS_int_ret_code(void);

/** \brief Assembly stub: suspend the current task and context-switch to the next ready task. */
void GRTOS_Suspend_Task(void);
/** \brief Initialise ptcb's stack frame so the task starts at its TCB_TaskCode entry point.
 *  \param [in] ptcb  TCB whose stack is to be initialised.
 *  \return G_TRUE on success. */
G_UINT32 gk_TASK_STK_INIT(GS_TCB *ptcb);
/** \brief Invoked by the time-event list when event expires; releases the blocked task.
 *  \param [in] event  ECB representing the expiring timed event. */
void gk_TimeCallback(GS_ECB * const event);
/** \brief Invoked when a resource is destroyed while an ECB is still associated with it.
 *  \param [in] pevent  ECB to clean up. */
void gk_ResourceECBKillCallback(GS_ECB *pevent);
/** \brief Invoked by the hardware interrupt when the system enters frozen mode. */
void gk_FrozenCallback(void);
/** \brief Invoked by the hardware interrupt when the system exits frozen mode. */
void gk_UnFrozenCallback(void);

/** \brief Low-level entry point for task completion: saves context and calls gk_KERNEL_TASK_COMPLETE. */
void gk_ENTRY_TASK_COMPLETE(void);
/** \brief Kernel-side task completion handler: recycles the TCB, updates lists, and schedules the next task. */
void  gk_KERNEL_TASK_COMPLETE(void);
/** \brief Dequeue the highest-priority pending SCB for the current task and execute its signal callback. */
void gk_TCB_GetPendingSCB(void);
/** \brief Set the processor exclusion mask for scheduling list plcb.
 *  \param [in] plcb       Scheduling list to configure.
 *  \param [in] exclusion  Maximum number of tasks from this list allowed to be
 *                         simultaneously RUNNING across all processors. Not a
 *                         processor bitmask.
 *  \return G_TRUE on success. */
G_UINT32 gu_SchedulingListExclusionSet(GS_LCB *plcb, G_UINT32 exclusion);

/***************************************************************/
/*  SYSTEM TASKS                                               */
/***************************************************************/
/** \brief Entry point called when a task returns from a signal callback; resumes normal execution. */
void gk_ENTRY_SIGNAL_RETURN(void);
/** \brief Task body for the per-processor IDLE task; loops forever yielding the processor. */
void gk_CODE_IDLE_TASK(void* pdata);
/** \brief Suspend the IDLE task until the next scheduling event on this processor. */
void gk_WAIT_IDLE_TASK(void);
/** \brief Restore context for signal execution: transfers execution into the signal callback. */
void gk_restore(void);

/** \brief Suspend the current task for the specified wall-clock duration.
 *  \param [in] hours    Hours component of the delay.
 *  \param [in] minutes  Minutes component.
 *  \param [in] seconds  Seconds component.
 *  \param [in] ms       Milliseconds component.
 *  \return G_TRUE on success. */
G_UINT32  gu_TaskDelay(G_UINT32 hours, G_UINT32 minutes, G_UINT32 seconds, G_UINT32 ms);
/** \brief Suspend the current task for the specified number of hardware ticks.
 *  \param [in] ticks  Delay in hardware ticks.
 *  \return G_TRUE on success. */
G_UINT32  gu_TaskDelayTime(gt_time ticks);
/** \brief Return the TCB of the task currently executing on the calling processor.
 *  \return Pointer to the current TCB. */
GS_TCB *gu_TaskGetCurrentTCB(void);

/**
 *  Default setting functions
 */
/** \brief Convert a wall-clock time into hardware ticks.
 *  \param [in] hours    Hours component.
 *  \param [in] minutes  Minutes component.
 *  \param [in] seconds  Seconds component.
 *  \param [in] ms       Milliseconds component.
 *  \return Equivalent number of hardware ticks. */
G_UINT64 gu_ConvertTime(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms);
/** \brief Return the total elapsed system time since start-up in hardware ticks.
 *  \return Accumulated system time. */
G_UINT64 gu_SystemTotalTimeGet(void);
/** \brief Return the total time spent in frozen mode since start-up in hardware ticks.
 *  \return Total frozen-mode duration. */
G_UINT64 gu_FrozenModeTimeGet(void);
/** \brief Return the current frozen-mode threshold in hardware ticks.
 *  \return Frozen-mode threshold value. */
G_UINT64 gu_FrozenModeThresholdGet(void);
/** \brief Set the frozen-mode activation threshold.
 *  \param [in] time_set  New threshold in hardware ticks.
 *  \return G_TRUE on success. */
G_UINT32 gu_FrozenModeThresholdSet(G_UINT64 time_set);
/** \brief Set the scheduling type of ptcb (e.g. one-shot, periodic, aperiodic).
 *  \param [in] ptcb  Task to configure.
 *  \param [in] type  Task type from \c enum tcbtype.
 *  \return G_TRUE on success. */
G_UINT32 gu_TaskTypeSet(struct gs_tcb *ptcb, enum tcbtype type);
/** \brief Associate ptcb with scheduling list plcb.
 *  \param [in] ptcb  Task to associate.
 *  \param [in] plcb  Target scheduling list.
 *  \return G_TRUE on success. */
G_UINT32 gu_SchedulingListAssociateTask(struct gs_tcb *ptcb, struct gs_lcb *plcb);
/** \brief Set the ready-state priority of ptcb.
 *  \param [in] ptcb      Task to configure.
 *  \param [in] priority  Ready-state priority value (higher number = lower priority in GeMRTOS).
 *  \return G_TRUE on success. */
G_UINT32 gu_TaskReadyPrioritySet(struct gs_tcb *ptcb, G_UINT64 priority);
/** \brief Set the running-state (current) priority of ptcb.
 *  \param [in] ptcb      Task to configure.
 *  \param [in] priority  Running-state priority value.
 *  \return G_TRUE on success. */
G_UINT32 gu_TaskRunPrioritySet(struct gs_tcb *ptcb, G_UINT64 priority);
/** \brief Set the period of ptcb for periodic task scheduling.
 *  \param [in] ptcb     Periodic task to configure.
 *  \param [in] hours    Hours component of the period.
 *  \param [in] minutes  Minutes component.
 *  \param [in] seconds  Seconds component.
 *  \param [in] ms       Milliseconds component.
 *  \return G_TRUE on success. */
G_UINT32 gu_TaskPeriodSet(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms);
/** \brief Schedule ptcb to become ready after an initial offset from the current time.
 *  \param [in] ptcb     Task to start.
 *  \param [in] hours    Hours component of the offset.
 *  \param [in] minutes  Minutes component.
 *  \param [in] seconds  Seconds component.
 *  \param [in] ms       Milliseconds component.
 *  \return G_TRUE on success. */
G_UINT32 gu_TaskStartWithOffset(GS_TCB *ptcb,unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms);


/**
 * Task related functions 
 */

/** \brief Allocate a TCB, set task_code and p_arg, format the description, and place the task in the ready list.
 *  \param [in] task_code  Function pointer to the task's entry point.
 *  \param [in] p_arg      Argument passed to the task at first activation.
 *  \param [in] format     printf-style format string for the task description.
 *  \return Pointer to the new TCB cast to \c void*; NULL on pool exhaustion. */
void *gu_TaskCreate(void *task_code, void *p_arg, const char *format, ...);

/** \brief Kill ptcb: unlink it from all lists and return it to the TCB pool.
 *  \param [in] ptcb  Task to kill.
 *  \return G_TRUE on success. */
G_UINT32   gu_TaskKill(GS_TCB *ptcb);
/** \brief Suspend ptcb: move it to the waiting state (does not release held resources).
 *  \param [in] ptcb  Task to suspend.
 *  \return G_TRUE on success. */
G_UINT32   gu_TaskSuspend(GS_TCB *ptcb);
/** \brief Resume ptcb: move it from the waiting state back to the ready state.
 *  \param [in] ptcb  Task to resume.
 *  \return G_TRUE on success. */
G_UINT32   gu_TaskResume(GS_TCB *ptcb);


/**
 * Signal related functions
 */
/** \brief Allocate a new SCB and initialise it with the given type, priority, owner, and callback.
 *  \param [in] type         Signal type (from \c enum scbtype).
 *  \param [in] priority     Signal priority (lower value = higher priority).
 *  \param [in] pxcb         Pointer to the owning control block (TCB, RCB, or KCB).
 *  \param [in] signal_code  Callback function to invoke when the signal fires.
 *  \param [in] signal_arg   Argument passed to signal_code.
 *  \return Pointer to the new SCB; NULL on pool exhaustion. */
GS_SCB *gu_SignalCreate(enum scbtype type, G_UINT32 priority, void *pxcb, void *signal_code, void *signal_arg);
/** \brief Detach pscb from its owner and return it to the SCB free pool.
 *  \param [in] pscb  Signal to destroy.
 *  \return G_TRUE on success. */
G_UINT32      gu_SignalDestroy(GS_SCB *pscb);

/**
 *  Monitor functions
 */
G_UINT32 gk_CheckTimePriorityLayout(void);
/** \brief Run a full consistency check of all GeMRTOS data structures for cycles passes.
 *  \param [in] cycles  Number of consistency-check passes to perform.
 *  \return G_TRUE if all checks pass; G_FALSE on the first detected inconsistency. */
G_UINT32 gkm_Check_GeMRTOS(int cycles);
/** \brief Check the integrity of the PCBAssocLCB free list and all active associations.
 *  \param [in] partial  Non-zero to check only the free list; 0 for a full check.
 *  \return G_TRUE on success. */
G_UINT32 gkm_Check_PCBAssocLCBs(G_UINT32 partial);
/** \brief Check the integrity of all active scheduling lists (LCBs).
 *  \param [in] partial  Non-zero to check only list linkage; 0 for a full check.
 *  \return G_TRUE on success. */
G_UINT32 gkm_Check_LCBs(G_UINT32 partial);
/** \brief Check the integrity of all active resource control blocks (RCBs).
 *  \param [in] partial  Non-zero for a shallow check; 0 for a full check.
 *  \return G_TRUE on success. */
G_UINT32 gkm_Check_RCBs(G_UINT32 partial);
/** \brief Check the integrity of all active RRDS receive-descriptor structures.
 *  \param [in] partial  Non-zero for a shallow check; 0 for a full check.
 *  \return G_TRUE on success. */
G_UINT32 gkm_Check_RRDSs(G_UINT32 partial);
/** \brief Check the integrity of all processor control blocks (PCBs).
 *  \param [in] partial  Non-zero for a shallow check; 0 for a full check.
 *  \return G_TRUE on success. */
G_UINT32 gkm_Check_PCBs(G_UINT32 partial);
/** \brief Check the integrity of all event control blocks (ECBs) and the ECB free list.
 *  \param [in] partial  Non-zero for a shallow check; 0 for a full check.
 *  \return G_TRUE on success. */
G_UINT32 gkm_Check_ECBs(G_UINT32 partial);
/** \brief Check the integrity of all task control blocks (TCBs) and the TCB free list.
 *  \param [in] partial  Non-zero for a shallow check; 0 for a full check.
 *  \return G_TRUE on success. */
G_UINT32 gkm_Check_TCBs(G_UINT32 partial);
/** \brief Verify that all testing-specific counters and bookkeeping are consistent.
 *  \return G_TRUE on success. */
G_UINT32 gkm_Check_Testing(void);
/** \brief Print the kernel function call stack to stderr (debug mode only).
 *  \return G_TRUE on success. */
G_UINT32 gkm_PrintFunctionStack( void );

/** \brief Verify that value is a valid member of the enum identified by hash.
 *  \param [in] hash   Hash component of the enum (first argument to ENUM_CODE).
 *  \param [in] value  Value component to validate.
 *  \return G_TRUE if the (hash, value) pair is a known enum; G_FALSE otherwise. */
G_UINT32 gkm_ENUM_IsValid(G_UINT32 hash, G_UINT32 value);
/**
 *   newlib FUNTIONS
 */
/** \brief printf-style formatted output to standard output through the GeMRTOS JTAG UART.
 *  \param [in] format  printf format string.
 *  \note Serialized by the global GeMRTOS mutex, which it acquires and
 *        releases around the write. (It does **not** use `gu_printf_sem`;
 *        that object is created at boot but never acquired by anything.)
 *  \note **Never blocks**, in any build. Output that the JTAG UART FIFO
 *        cannot accept while the host is not draining it is discarded and
 *        counted. A message is never
 *        interleaved with another processor's output. */
void gu_printf(const char *format, ...) G_PRINTF_LIKE(1, 2);
/** \brief fprintf-style formatted output to standard error through the GeMRTOS JTAG UART.
 *  \param [in] format  printf format string.
 *  \note Same mutex, blocking and discard semantics as gu_printf(). */
void gu_fprintf(const char *format, ...) G_PRINTF_LIKE(1, 2);

/** \brief Kernel-level printf to standard output. Never blocks.
 *  \param [in] format  printf format string.
 *  \pre The caller ALREADY HOLDS the GeMRTOS mutex. Unlike gu_printf(), this
 *       does not acquire it, which is what makes it usable from kernel
 *       context, from the watchdog recovery path, and before the kernel is
 *       running. Calling it without the mutex races the shared format buffer
 *       and the shared JTAG UART.
 *  \note Output is split across as many FIFO fills as needed, so nothing is
 *        lost while the host is draining; the remainder of a message is
 *        abandoned only once the FIFO is observed to have stopped draining. */
void gk_printf(const char *format, ...) G_PRINTF_LIKE(1, 2);
/** \brief Kernel-level fprintf to standard error. Never blocks.
 *  \param [in] format  printf format string.
 *  \pre The caller ALREADY HOLDS the GeMRTOS mutex -- see gk_printf(). */
void gk_fprintf(const char *format, ...) G_PRINTF_LIKE(1, 2);


/**
 *  Semaphore kernel functions â€” moved from sem.h.
 *  Application code must never call gk_* functions directly.
 */
/** \brief Grant the semaphore prcb to the task associated with pevent; moves pevent to the granted list.
 *  \param [in] prcb    Semaphore resource to grant.
 *  \param [in] pevent  ECB that represents the requesting task.
 *  \return G_TRUE on success. */
G_UINT32 gk_SemaphoreGrant(GS_RCB *prcb, GS_ECB *pevent);
/** \brief Grant the semaphore to the highest-priority task currently waiting on it.
 *  \param [in] prcb  Semaphore resource to grant.
 *  \return G_TRUE on success; G_FALSE if no tasks are waiting. */
G_UINT32 gk_SemaphoreGrantWaiting(GS_RCB *prcb);
/** \brief Handle expiry of the timeout ECB for a semaphore wait; releases the waiting task.
 *  \param [in] pevent  Timeout ECB whose timer has expired.
 *  \return G_TRUE on success. */
G_UINT32 gk_SemaphoreTimeout(GS_ECB *pevent);
/** \brief Clean up the ECB and release the waiting task when the semaphore resource is destroyed.
 *  \param [in] pevent  ECB to clean up.
 *  \return G_TRUE on success. */
G_UINT32 gk_SemaphoreKill(GS_ECB *pevent);
/**
 *  Message queue kernel functions â€” moved from mq.h.
 *  Application code must never call gk_* functions directly.
 */
/** \brief Handle expiry of a receiver timeout ECB; releases the waiting receiver task.
 *  \param [in] pevent_timeout  Timeout ECB whose timer has expired.
 *  \return G_TRUE on success. */
G_UINT32 gk_MessageQueueTimeoutECBReceiver(GS_ECB *pevent_timeout);
/** \brief Clean up a sender ECB when the message queue is destroyed.
 *  \param [in] pevent  Sender ECB to clean up.
 *  \return G_TRUE on success. */
G_UINT32 gk_MessageQueueKillECBSender(GS_ECB *pevent);
/** \brief Clean up a receiver ECB when the message queue is destroyed.
 *  \param [in] pevent  Receiver ECB to clean up.
 *  \return G_TRUE on success. */
G_UINT32 gk_MessageQueueKillECBReceiver(GS_ECB *pevent);
/** \brief Handle expiry of a sender timeout ECB; releases the blocked sender task.
 *  \param [in] pevent_timeout  Timeout ECB whose timer has expired.
 *  \return G_TRUE on success. */
G_UINT32 gk_MessageQueueTimeoutECBSender(GS_ECB *pevent_timeout);
/** \brief Transfer a segment of pevent's message into pevent_sender's receiver buffer.
 *  \param [in] pevent   Sender ECB that holds the message.
 *  \param [in] pevent_sender  Receiver ECB that holds the destination buffer.
 *  \return Number of bytes transferred; negative on partial transfer (more segments follow). */
int gk_MessageQueueTransferMessage(GS_ECB *pevent, GS_ECB *pevent_sender); /* returns msg_length; negative on a partial transfer */

/**
 *  Trigger kernel functions â€” moved from trigger.h.
 *  Application code must never call gk_* functions directly.
 *  Note: gk_TRG_Wait() returns GS_TRGStatus (defined in trigger.h) so its
 *  prototype remains in trigger.h to avoid a circular include dependency.
 */
/** \brief Release ptrigger: move all waiting tasks to the ready state and fire any enable hook.
 *  \param [in] ptrigger  Trigger resource to release. */
void gk_TRG_Release(GS_RCB *ptrigger);
/** \brief Enable ptrigger so that the next release will wake registered tasks.
 *  \param [in] ptrigger  Trigger resource to enable.
 *  \return G_TRUE on success. */
G_UINT32 gk_TRG_Enable(GS_RCB *ptrigger);
/** \brief Disable ptrigger; registered tasks will not be woken until it is re-enabled.
 *  \param [in] ptrigger  Trigger resource to disable.
 *  \return G_TRUE on success. */
G_UINT32 gk_TRG_Disable(GS_RCB *ptrigger);
/** \brief No-operation hook used as the default enable/disable callback for triggers.
 *  \param [in] arg  Unused argument.
 *  \return NULL. */
void *gk_TRG_null_hook(const void *arg); /* const per MISRA C:2012 Rule 8.13 */
/** \brief Handle expiry of a trigger's timeout ECB; releases the waiting task with a timeout status.
 *  \param [in] ptimed  Timeout ECB whose timer has expired.
 *  \return G_TRUE on success. */
G_UINT32 gk_TriggerTimeout(GS_ECB *ptimed);


#endif /* GEMRTOS_CORE_H_ */
