/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS Kernel functions
 *  \details This file contains the Kernel functions.
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


#include <gemrtos.h>

OPTIMIZE_CODE

/*****************************************************************************/
/*****************************************************************************/
/**         SYSTEM VARIABLES                                                 */
/*****************************************************************************/
/*****************************************************************************/

/* Data Structure */
GS_KCB  g_kcb __attribute__((aligned(4)));           ///< \brief Structure of GS_KCB



// Mask for processor bit avoiding shift instruction penalties
const unsigned int PRC_MASK[] = {(unsigned int) 0x1, (unsigned int) 0x2, (unsigned int) 0x4, (unsigned int) 0x8, (unsigned int) 0x10, (unsigned int) 0x20, (unsigned int) 0x40, (unsigned int) 0x80, (unsigned int) 0x100, (unsigned int) 0x200, (unsigned int) 0x400, (unsigned int) 0x800, (unsigned int) 0x1000, (unsigned int) 0x2000, (unsigned int) 0x4000, (unsigned int) 0x8000, (unsigned int) 0x10000, (unsigned int) 0x20000, (unsigned int) 0x40000, (unsigned int) 0x80000, (unsigned int) 0x100000, (unsigned int) 0x200000, (unsigned int) 0x400000, (unsigned int) 0x800000, (unsigned int) 0x1000000, (unsigned int) 0x2000000, (unsigned int) 0x4000000, (unsigned int) 0x8000000,(unsigned int) 0x10000000, (unsigned int) 0x20000000, (unsigned int) 0x40000000, (unsigned int) 0x80000000};

// GLOBAL VARIABLES to hold states for assembler interface
volatile G_UINT32 G_SCB_PENDING __attribute__((aligned(4)));   ///< \brief Holds the SCB for pending signal
volatile G_UINT32 G_SCB_CODE __attribute__((aligned(4)));      ///< \brief Holds the Code Address for pending signal
volatile G_UINT32 G_SCB_ARG __attribute__((aligned(4)));       ///< \brief Holds the Args Address for pending signal



volatile G_UINT32  gs_sizeof_G_PCBTbl __attribute__((aligned(4)));           ///< \brief Values for assembler code
volatile void     *gs_addressof_G_PCBTbl __attribute__((aligned(4)));        ///< \brief Values for assembler code
volatile G_UINT32  gs_offsetof_PCB_EXECTCB __attribute__((aligned(4)));      ///< \brief Values for assembler code
volatile G_UINT32  gs_offsetof_TCB_StackPointer __attribute__((aligned(4))); ///< \brief Values for assembler code
volatile G_UINT32  gs_offsetof_PCB_IDLETCB __attribute__((aligned(4)));      ///< \brief Values for assembler code
volatile G_UINT32  gs_offsetof_PCB_SUSPENDED_STK_PTR __attribute__((aligned(4)));

volatile G_UINT32 G_Testing_Counter __attribute__((aligned(4)));

/* Semaphore to exclude gu_fprintf access */
t_semaphore_resource *gu_printf_sem;


volatile G_UINT32 G_TASK_TYPE_DEFAULT __attribute__((aligned(4)));         ///< \brief Default assignment for Task Type
volatile GS_LCB  *G_TASK_LCB_DEFAULT __attribute__((aligned(4)));          ///< \brief Default assignment for Task LCB
volatile G_UINT32 G_TASK_LCB_DEFAULT_PRIORITY __attribute__((aligned(4))); ///< \brief Priority of the default LCB
volatile G_UINT64 G_TASK_PRIORITY_DEFAULT __attribute__((aligned(4)));     ///< \brief Default assignment for Task Priority
volatile G_UINT64 G_TASK_PERIOD_DEFAULT __attribute__((aligned(4)));       ///< \brief Default assignment for Task Period





/***********************************************************************************
*********************** GRTOS INTERRUPT FUNCTIONS  ********************************
***********************************************************************************/
/**
 *  \brief Executes the ISR of GRTOS.
 *  \details This routine is executed each time the processor is interrupted.
 */

void gk_ENTRY_IRQ_HANDLER (void)
{
    // ********************************************* //
    // DO NOT DEFINE LOCAL VARIABLES.                //
    // THE FUNCTION CHANGES THE STACK POINTER        //
    // AND CALL START_TASK TO BALANCE FRAME POINTER  //
    // ********************************************* //

	gm_PrcIRQSecSet();         /// Indicate the processor is waiting for IRQ Handler
                                       /// when interrupt is pending
	GRTOS_CMD_CRITICAL_SECTION_GET;    /// Get into critical section
    gm_KernelModeSet();
	gm_MutexNestedValue(1);

    gk_KERNEL_TASK_START ();
}


/**********************************************************************************
 *                        	BAD RETURN FUNCTION IN TASK STACK
 * THIS IS THE RETURN ADDRESS WHEN A BAD RETURN IS EXECUTED IN A TASK SWITCH
 *********************************************************************************/
/**
 *  \brief BAD RETURN FUNCTION IN TASK STACK
 *  \details THIS IS THE RETURN ADDRESS WHEN A BAD RETURN IS EXECUTED IN A TASK SWITCH
 */
void gk_ENTRY_BAD_TASK_RETURN(void)
{
    PRINT_ASSERT((G_TRUE == G_FALSE), "ERROR, BAD TASK RETURN In %s, %d \n", __FUNCTION__, __LINE__);
    while(1);
}

/***********************************************************************************
****Return from Signal: get critical section and return to                    ******
**** GRTOS_Return_from_Signal in grtos_switch.S file
***********************************************************************************/
/**
 *  \brief Return from Signal.
 *  \details Gets the critical section and returns to GRTOS_Return_from_Signal in grtos_switch_XXXX.S file.
 */
void gk_ENTRY_SIGNAL_RETURN(void)
{
    // ********************************************* //
    // DO NOT DEFINE LOCAL VARIABLES.                //
    // THE FUNCTION CHANGES THE STACK POINTER        //
    // AND CALL START_TASK TO BALANCE FRAME POINTER  //
    // ********************************************* //
	PRINT_ASSERT((!(gm_IsPrcGranted())),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);
	
	GRTOS_CMD_CRITICAL_SECTION_GET;    /// Get into critical section
    gm_KernelModeSet();
	gm_MutexNestedValue(1);

}

/**gk_LCB_CheckInversion
 *  \brief 
 *  Checks if there is a processor that has to be interrupted to switch task
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Processor
 *  \sa Task
 */
static G_UINT32 gk_LCB_CheckInversion(void)
{   
    GK_SAMPLE_FUNCTION_BEGIN(10004)
    for (unsigned int proc_id = 1; proc_id <= G_NUMBER_OF_PCB; proc_id++)
    {
        if ((proc_id != gm_ProcessorId()) && (g_kcb.G_PCBTbl[proc_id - 1].PCBState != (int) GS_PCBState_NOTRUNNING))
        {
            GS_TCB *ptcb_current = g_kcb.G_PCBTbl[proc_id - 1].PCB_EXECTCB;
            GS_TCB *ptcb_next    = gk_PCB_GetNextTCB(&g_kcb.G_PCBTbl[proc_id - 1]);

            PRINT_ASSERT((gkm_TCB_IsValid(ptcb_current) == G_TRUE),"ERROR TCB next is not valid\n");
            PRINT_ASSERT((gkm_TCB_IsValid(ptcb_next) == G_TRUE),"ERROR TCB next is not valid\n");
            PRINT_ASSERT((ptcb_current->TCBCurrentPriority <= ptcb_current->TCBReadyPriority),"ERROR TCB current priority is not valid= %s, Currentpriority= %llu, ReadyPriority= %llu\n", ptcb_current->TCB_description, ptcb_current->TCBCurrentPriority, ptcb_current->TCBReadyPriority);
            PRINT_ASSERT((ptcb_next->TCBCurrentPriority <= ptcb_next->TCBReadyPriority),"ERROR TCB next priority is not valid= %s, Currentpriority= %llu, ReadyPriority= %llu\n", ptcb_next->TCB_description, ptcb_next->TCBCurrentPriority, ptcb_next->TCBReadyPriority);
            if (ptcb_current != ptcb_next)
            {
                gm_ProcessorInterrupt(proc_id);
                break;
            }
        }
    }

    GK_SAMPLE_FUNCTION_END(10004)
	return(G_TRUE);
}

/**
 *  \brief Start the execution of the next task.
 */
void gk_KERNEL_TASK_START (void)
{
	GS_SCB *psignal;
    GK_SAMPLE_FUNCTION_BEGIN(10045);

	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);

	PRINT_ASSERT((gm_KernelModeGet() == G_TRUE), "ERROR in Kernel mode");

	unsigned int event_code;
    GS_ECB *pevent;

	// Processor handles just one event and continue to avoid starving other processors from mutex
	event_code = gm_GetEventOccurrence();

	if (event_code != 0)
	{
        switch (event_code) {
            case EVN_CODE_TIMED:
                // Time Event happened
                pevent = g_kcb.KCB_NextECBTL;
                
                PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR no valid timed event\n");
                PRINT_ASSERT((( pevent->ECB_AssocTCB == (GS_TCB *) 0) || (gkm_TCB_IsValid(pevent->ECB_AssocTCB) == G_TRUE)),"ERROR no valid TASK associated with event\n");

                /* Unlink the EVENT from Time Waiting Event List     */
                gk_ECBTL_Unlink((GS_ECB *)pevent);
                // Call the TIME_CALLBACK funtion to resolve according to the event type
                gk_TimeCallback((GS_ECB *) pevent);
                break;

            case EVN_CODE_UNFROZEN:
                // The Frozen event is acknowledge to process the following events
                gm_UnFrozenEventClear();
                // Calls the gk_UnFrozenCallback() routine in grtosfunctions.c file where 
                // user may inplement frozen mode strategy
                gk_UnFrozenCallback();

				psignal = gk_KCBASL_GetSCB(G_SCBType_UNFROZEN_MODE);
				if (psignal != (GS_SCB *) 0) {
					PRINT_ASSERT((gkm_SCB_IsValid(psignal) == G_TRUE),"ERROR no valid G_SCBType_UNFROZEN_MODE\n");
					psignal = gk_SCB_Copy((GS_SCB *) psignal); 
					PRINT_ASSERT((gkm_SCB_IsValid(psignal) == G_TRUE),"ERROR no valid COPY G_SCBType_UNFROZEN_MODE\n");
					gk_KCBPSL_Link((GS_SCB *) psignal); 
				}
                break;
               
            case EVN_CODE_FROZEN:
                // The Frozen event is acknowledge to process the following events
                gm_FrozenEventClear();
                // Calls the gk_FrozenCallback() routine in grtosfunctions.c file where 
                // user may inplement frozen mode strategy
                gk_FrozenCallback();
				
				psignal = gk_KCBASL_GetSCB(G_SCBType_FROZEN_MODE);
				if (psignal != (GS_SCB *) 0) {
					PRINT_ASSERT((gkm_SCB_IsValid(psignal) == G_TRUE),"ERROR no valid G_SCBType_FROZEN_MODE\n");
					psignal = gk_SCB_Copy((GS_SCB *) psignal); 
					PRINT_ASSERT((gkm_SCB_IsValid(psignal) == G_TRUE),"ERROR no valid COPY G_SCBType_FROZEN_MODE\n");
					gk_KCBPSL_Link((GS_SCB *) psignal); 
				}
                break;
				
            default: 

                PRINT_ASSERT((gkm_RCB_IsValid(g_kcb.KCB_NextRCBTRGL[event_code]) == G_TRUE),"ERROR RCB is not valid\n");
                PRINT_ASSERT((g_kcb.KCB_NextRCBTRGL[event_code]->RCB_NextRCBWEL != (struct gs_ecb *) 0),"ERROR Interrupt happened but without an associated ISR\n");

                PRINT_ASSERT((g_kcb.KCB_NextRCBTRGL[event_code]->RCB_NextRCBGEL == (struct gs_ecb *) 0),"ERROR Interrupt happened but with running ISR, %s\n", g_kcb.KCB_NextRCBTRGL[event_code]->RCB_NextRCBGEL->ECB_AssocTCB->TCB_description);

				g_kcb.KCB_NextRCBTRGL[event_code]->trigger.TRGStatus = G_TRGStatus_triggered;
                gk_TRG_Release (g_kcb.KCB_NextRCBTRGL[event_code]);

                break;
		}
	}

    // #####################################################
	/* Check if Executing task is running or if it was desalocated   */
	GS_TCB *ptcb_from = gk_PCB_GetCurrentTCB();
	GS_TCB *ptcb_to   = gk_PCB_GetNextTCB(&g_kcb.G_PCBTbl[gm_ProcessorId() -1]);


	if (ptcb_from != ptcb_to) {
		if (get_pointed_field(ptcb_from, TCBState) == G_TCBState_RUNNING){
            PRINT_ASSERT((ptcb_from->TCB_AssocPCB == (int) gm_ProcessorId() ),"ERROR associated task=%d, proc=%d\n",(int) ptcb_from->TCB_AssocPCB, (int) gm_ProcessorId());
			gk_TCB_Unlink(ptcb_from);
			gk_TCBRDYL_Link(ptcb_from);
		}
		gk_TCBRDYL_Unlink(ptcb_to);
		gk_TCBRUNL_Link(ptcb_to);
	}

    PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);

    // Check if there exist a processor with priority inversion
    gk_LCB_CheckInversion();

    GK_SAMPLE_FUNCTION_END(10045)

	GRTOS_Start_Task();

/* Embedded-only — after GRTOS_Start_Task the processor must never
 * return; if it does, this block prints an error and halts.
 * No Windows equivalent: on the host build this path does not exist. */
#ifdef __niosX_arch__
	/* Never should be executed this */
	gk_fprintf("Error in GRTOS_STart. Processor %d", gm_ProcessorId() );
	G_DEBUG_WHILEFOREVER;
    while(1);
#endif
}

/***********************************************************************************
*********************** GRTOS TASK FINISH FUNCTION  *******************************
***********************************************************************************/
/**
 *  \brief Executed when a task finishes its execution.
 */
void  gk_ENTRY_TASK_COMPLETE(void)
{
    // ********************************************* //
    // DO NOT DEFINE LOCAL VARIABLES.                //
    // THE FUNCTION CHANGES THE STACK POINTER        //
    // AND CALL gk_KERNEL_TASK_COMPLETE              //
    // TO BALANCE FRAME POINTER                      //
    // ********************************************* //

	GRTOS_CMD_CRITICAL_SECTION_GET;   /// Enter in Critical Section
    gm_KernelModeSet();
	gm_MutexNestedValue(1);
	
	GRTOS_Change_SP_to_IDLE();     /// this function calls gk_KERNEL_TASK_COMPLETE in IDLE task stack

/* Embedded-only — after GRTOS_Change_SP_to_IDLE the processor must not
 * return; a bare while(1) traps any spurious continuation on hardware.
 * No Windows equivalent: the host build does not use real stack-pointer switching. */
#ifdef __niosX_arch__
	while(1);
#endif

}


/**
 *  \brief Performs post-execution cleanup after a task completes.
 *  \details Called on the IDLE-task stack after GRTOS_Change_SP_to_IDLE. Unlinks
 *           the completed task from the running list, places it in the
 *           waiting-completed state, and re-arms ISR triggers if applicable.
 *  \relates Task
 */
void  gk_KERNEL_TASK_COMPLETE(void)
{
	GS_RCB *ptrigger;
	GS_ECB *pevent;
	GK_SAMPLE_FUNCTION_BEGIN(10046)

	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);

	GS_TCB *ptcb = g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB; 


    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR Completed task is not valid %p \n",(void *) ptcb);
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_RUNNING),"ERROR Completed task (%p) is not running, TCBState= %d\n",(void *) ptcb, (int) ptcb->TCBState);
    PRINT_ASSERT((ptcb->TCB_AssocPCB == gm_ProcessorId()), "ERROR task not assigned to processor assign to=%d, proc=%d\n",(int) ptcb->TCB_AssocPCB, (int) gm_ProcessorId());
    
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
/* GRTOS_READ_SP reads the hardware stack-pointer register via a
 * Nios II/V-specific assembly macro. No Windows-simulation equivalent
 * exists, so this block is skipped there (that flavour never defines
 * __niosX_arch__ at all).
 *
 * GEMRTOS_TEST_NO_AUTOSTART additionally excludes it from the host
 * kernel-unit-test build (a development-tree harness that is not published, which DOES define __niosX_arch__
 * -- see gemrtos_core.h's own comment on this flag): the assertion is
 * fundamentally unanswerable there, since no real stack-pointer switch
 * ever happens on host (ordinary nested C calls on the one real OS thread
 * stack, never actually relocated onto the IDLE task's small static
 * buffer). Diagnostic-only -- removing it from this one build changes no
 * production logic, only whether this specific debug check runs. */
#if defined(__niosX_arch__) && !defined(GEMRTOS_TEST_NO_AUTOSTART)
    void *StackPointer=(void *)0;
    GRTOS_READ_SP(StackPointer);
    PRINT_ASSERT(((void *) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_IDLETCB->TCB_StackBottom >= StackPointer && (void *) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_IDLETCB->TCB_StackTop <= StackPointer),"ERROR Initiliasing the current stack, ptcb->TCB_StackBottom= %p, ptcb->TCB_StackTop= %p, StackPointer= %p \n", (void *) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_IDLETCB->TCB_StackBottom, (void *) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_IDLETCB->TCB_StackTop, (void *) StackPointer);
#endif
#endif

    gk_TCBRUNL_Unlink(ptcb);
	gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);  /// Restore TCB to waiting list

	// If task is an ISR, then for next interrupt request 
	if (get_pointed_field(ptcb, TCBType) == G_TCBType_ISR) {
		
		ptrigger = get_pointed_field(ptcb, TCB_INTNumber);
			PRINT_ASSERT(((int) ptrigger != -1),"ERROR Task -%s- is not IRQ ISR= %d\n", ptcb->TCB_description, (int) ptrigger);
			PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
			PRINT_ASSERT((ptrigger->RCB_NextRCBGEL != (struct gs_ecb *) 0),"ERROR IRQ complete but not count= %p, %s\n", (void *) ptrigger->RCB_NextRCBGEL, ptcb->TCB_description);
		
		pevent = gk_TCB_in_RCBGEL(ptrigger, ptcb);
			PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid, task not registered: %s\n", ptcb->TCB_description);
			PRINT_ASSERT((pevent->ECBType == (G_UINT32) G_ECBType_EXT_ISR_RELEASE), "ERROR ECBType is not valid%d\n",(int)pevent->ECBType);
			
		gk_RCBGEL_Unlink(pevent);
		set_pointed_field(pevent, ECBType, (G_UINT32) G_ECBType_EXT_ISR_WAITING);
		gk_RCBWEL_Link(ptrigger, ptcb, pevent);
		
		if (get_pointed_field(ptrigger, RCB_NextRCBGEL) == (struct gs_ecb *) 0) {
			gk_TRG_Enable(ptrigger);
		}
	}

	GK_SAMPLE_FUNCTION_END(10046)
	gk_ENTRY_IRQ_HANDLER();
}


/***********************************************************************************
*********************** GRTOS SUSPEND TASK FUNCTION  *******************************
***********************************************************************************/
/**
 *  \brief Suspends the execution of the task pointed by ptcb.
 *  \details When \a ptcb is not the calling task and is RUNNING on another
 *           processor, that processor must first be brought to a kernel entry
 *           point, otherwise it would keep executing a task this function is
 *           about to unlink. That acquisition is attempted BEFORE any kernel
 *           state is modified, so a refusal leaves the system exactly as it
 *           was found and can simply be reported to the caller.
 *
 *           On the embedded target the acquisition cannot fail -- a hardware
 *           interrupt always preempts the target -- so this function always
 *           returns G_TRUE there and its behaviour is unchanged. On the
 *           Windows simulation, where a task executing user code cannot be
 *           preempted asynchronously, the attempt is bounded; see
 *           gm_ProcessorInterruptTry().
 *
 *           When the acquisition succeeds the target is already inside its
 *           IRQ section, so the gm_ProcessorInterrupt() that gk_TCBRUNL_Unlink()
 *           performs for the same processor completes without waiting.
 *  \param [in] ptcb Pointer to the TCB of the task to suspend.
 *  \return G_TRUE when the task has been suspended; G_FALSE when the target
 *          processor could not be acquired and the task was left unchanged.
 *  \sa gu_TaskSuspend, gm_ProcessorInterruptTry, gk_KERNEL_TASK_SUSPEND_CURRENT
 */
G_UINT32 gk_KERNEL_TASK_SUSPEND(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10057);

	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcb);
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_READY) || (ptcb->TCBState == G_TCBState_RUNNING),"ERROR Task to suspend in state (%d) is not valid\n",(int) ptcb->TCBState);

    if (ptcb == gk_PCB_GetCurrentTCB()) {
        gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);  /* Suspend the current task */
		PRINT_ASSERT((gm_IsPrcGranted()),"ERROR SIGNAL in critical section\n");
    } else {
        /* Acquire the processor running the task before touching any list.
         * Nothing has been modified at this point, so abandoning here is a
         * complete and consistent no-operation. */
        if (get_pointed_field(ptcb, TCBState) == G_TCBState_RUNNING) {
            G_UINT32 cpu_id = get_pointed_field(ptcb, TCB_AssocPCB);
            if (cpu_id != gm_ProcessorId()) {
                G_UINT32 acquired = gm_ProcessorInterruptTry(cpu_id);
                if (acquired != (G_UINT32) G_TRUE) {
                    GK_SAMPLE_FUNCTION_END(10057);
                    return (G_FALSE);
                }
            }
        }
        gk_TCB_Unlink(ptcb);                      /* Unlink the task from ready or running */
        gk_TCBWL_Link(ptcb, G_TCBState_WAITING);  /* Link the task to the waiting list */
    }
    GK_SAMPLE_FUNCTION_END(10057);
    return (G_TRUE);
}


/**gk_KERNEL_TASK_SUSPEND_CURRENT
 *  \brief Suspends the current task and dispatches the highest-priority ready task.
 *  \param [in] task_state The waiting state the current task is placed into
 *              (e.g., G_TCBState_WAITING).
 *  \note Caller must hold the hardware critical section. This function releases
 *        and re-acquires the critical section internally during the context switch.
 *  \relates Task
 */
void gk_KERNEL_TASK_SUSPEND_CURRENT(enum tcbstate task_state)
{
    GK_SAMPLE_FUNCTION_BEGIN(10028);
	
	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR SIGNAL in critical section\n");

    gm_KernelModeSet();
	gm_MutexNestedValue(1);
	
	/* Check if Executing task is running or if it was desalocated   */
	GS_TCB *ptcb_from = gk_PCB_GetCurrentTCB();

    PRINT_ASSERT((ptcb_from->TCBState == G_TCBState_RUNNING),"ERROR Current task is not runnnig, Task=%p, TCBState= %d, %s\n", (void *) ptcb_from, (int) ptcb_from->TCBState, ptcb_from->TCB_description);
    PRINT_ASSERT((g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB == ptcb_from),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcb_from);
    PRINT_ASSERT((g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_AssocPCB == gm_ProcessorId()),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcb_from);    

    // Assembler code reads this variable to storage the sp register
	g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_SUSPENDED_STK_PTR = (void *) &(ptcb_from->TCB_StackPointer);

    gk_TCBRUNL_Unlink(ptcb_from);
	
	switch (task_state) {
		case G_TCBState_WAITING:
			gk_TCBWL_Link(ptcb_from, G_TCBState_WAITING);
			break;
			
		case G_TCBState_READY:
			gk_TCBRDYL_Link(ptcb_from);
			break;
			
		default:
			G_DEBUG_WHILEFOREVER;
			break;
	}

	/* Suspend current task and execute next ready */
	GRTOS_Suspend_Task(); 
	
    GK_SAMPLE_FUNCTION_END(10028);	
}


/***********************************************************************************
*********************** GRTOS INITIALISATION FUNCTIONS  ****************************
***********************************************************************************/
/* -----------------------------------------------------------------------
 * gk_INIT_KERNEL helpers — called while hardware mutex is held (CS active).
 * These functions must not acquire or release the critical section.
 * ----------------------------------------------------------------------- */

/**
 * \brief Zero all KCB list pointers and populate object pools from static storage.
 * \pre Hardware mutex held (called from gk_INIT_KERNEL critical section).
 */
static void gk_Init_KCBLists(void)
{
    g_kcb.KCB_NextECBTL    = (struct gs_ecb *) 0;
    g_kcb.KCB_NextTCBWL    = (struct gs_tcb *) 0;
    g_kcb.KCB_NextLCBL     = (struct gs_lcb *) 0;
    g_kcb.KCB_NextKCBASL   = (struct gs_scb *) 0;
    g_kcb.KCB_NextKCBPSL   = (struct gs_scb *) 0;

    g_kcb.KCB_FREE_TCBs   = (struct gs_tcb  *) 0;
    g_kcb.KCB_FREE_ECBs   = (struct gs_ecb  *) 0;
    g_kcb.KCB_FREE_RCBs   = (struct g_rcb   *) 0;
    g_kcb.KCB_FREE_LCBs   = (struct gs_lcb  *) 0;
    g_kcb.KCB_FREE_SCBs   = (struct gs_scb  *) 0;
    g_kcb.KCB_FREE_RRDSs  = (struct gs_rrds *) 0;
    g_kcb.KCB_FREE_RDYs   = (struct gs_pcb_rdy_lcbl *) 0;

    /* MISRA C:2012 Rule 21.3: populate free lists from static pools. */
    gk_Init_Pools();

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    g_kcb.KCB_NUMBER_OF_TCBs         = 0;
    g_kcb.KCB_NUMBER_OF_PCBs         = 0;
    g_kcb.KCB_NUMBER_OF_ECBs         = 0;
    g_kcb.KCB_NUMBER_OF_RCBs         = 0;
    g_kcb.KCB_NUMBER_OF_LCBs         = 0;
    g_kcb.KCB_NUMBER_OF_SCBs         = 0;
    g_kcb.KCB_NUMBER_OF_RRDSs        = 0;
    g_kcb.KCB_NUMBER_OF_PCBAssocLCBs = 0;
    g_kcb.KCB_ROOT_TCBs   = (struct gs_tcb  *) 0;
    g_kcb.KCB_ROOT_ECBs   = (struct gs_ecb  *) 0;
    g_kcb.KCB_ROOT_RCBs   = (struct g_rcb   *) 0;
    g_kcb.KCB_ROOT_LCBs   = (struct gs_lcb  *) 0;
    g_kcb.KCB_ROOT_SCBs   = (struct gs_scb  *) 0;
    g_kcb.KCB_ROOT_RRDSs  = (struct gs_rrds *) 0;
    g_kcb.KCB_ROOT_RDYs   = (struct gs_pcb_rdy_lcbl *) 0;
#endif
}

/**
 * \brief Set task defaults, initialise processor control blocks, and create IDLE tasks.
 * \pre Hardware mutex held (called from gk_INIT_KERNEL critical section).
 */
static void gk_Init_TaskDefaults(void)
{
    G_TASK_TYPE_DEFAULT         = G_TCBType_OneShot;
    G_TASK_LCB_DEFAULT_PRIORITY = (G_UINT32) G_DEFAULT_LCB_PRIORITY;
    G_TASK_LCB_DEFAULT          = gk_Get_LCB(GS_LCBTypeFP);
    G_TASK_PRIORITY_DEFAULT     = (G_UINT64) G_LOWEST_PRIORITY;
    G_TASK_PERIOD_DEFAULT       = (G_UINT64) G_LOWEST_PRIORITY;

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    gk_fprintf("[ MESSAGE ] CREATING PCB STRUCTURES\n");
#endif
    gk_Init_PCBs();

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    gk_fprintf("[ MESSAGE ] CREATING IDLE TASKS\n");
#endif
    gk_Create_IDLE_tasks();
    gk_TCBRUNL_Link(g_kcb.G_PCBTbl[0].PCB_IDLETCB);

}

/**
 * \brief Initialise assembler-interface variables used by context-switch assembly code.
 * \pre Hardware mutex held (called from gk_INIT_KERNEL critical section).
 */
static void gk_Init_AssemblerInterface(void)
{
    gs_sizeof_G_PCBTbl                = sizeof(GS_PCB);
    gs_addressof_G_PCBTbl             = (void *) &g_kcb.G_PCBTbl[0];
    gs_offsetof_PCB_IDLETCB           = (G_UINT32) offsetof(struct gs_pcb, PCB_IDLETCB);
    gs_offsetof_PCB_EXECTCB           = (G_UINT32) offsetof(struct gs_pcb, PCB_EXECTCB);
    gs_offsetof_TCB_StackPointer      = (G_UINT32) offsetof(struct gs_tcb, TCB_StackPointer);
    gs_offsetof_PCB_SUSPENDED_STK_PTR = (G_UINT32) offsetof(struct gs_pcb, PCB_SUSPENDED_STK_PTR);
}

/**
 * \brief Initialise the timed-event sentinel ECB, create one trigger per IRQ line,
 *        and enable frozen mode.
 * \pre Hardware mutex held (called from gk_INIT_KERNEL critical section).
 */
static void gk_Init_EventSystem(void)
{
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    gk_fprintf("[ MESSAGE ] INITIALIZE THE TIMED EVENT LIST\n");
#endif
    g_kcb.KCB_NextECBTL = (struct gs_ecb *) gk_ECB_GetFree((GS_TCB *) 0);
    gm_SetNextOccurrenceTime(G_LATEST_TIME);
    g_kcb.KCB_NextECBTL->ECBValue.i64    = (G_UINT64) G_LATEST_TIME;
    g_kcb.KCB_NextECBTL->ECB_AssocTCB    = (struct gs_tcb *) 0;
    g_kcb.KCB_NextECBTL->ECB_PrevTCBAEL  = (struct gs_ecb *) 0;
    g_kcb.KCB_NextECBTL->ECB_NextTCBAEL  = (struct gs_ecb *) 0;
    g_kcb.KCB_NextECBTL->ECB_PrevECB     = (struct gs_ecb *) 0;
    g_kcb.KCB_NextECBTL->ECB_NextECB     = (struct gs_ecb *) 0;
    g_kcb.KCB_NextECBTL->ECB_NextECBASL  = (struct gs_scb *) 0;
    g_kcb.KCB_NextECBTL->ECBType         = (G_UINT32) G_ECBType_LASTEST_TIME;
    g_kcb.KCB_NextECBTL->ECBState        = GS_ECBState_WAITING_TIME;

    for (int i = 0; i < G_NUMBER_OF_IRQ_TCB; i++) {
        gu_TriggerCreate(i, 0);
    }

    G_UINT64 threshold_ticks = gu_ConvertTime(0, 0, 0, 500);
    gu_FrozenModeThresholdSet(threshold_ticks);
    gm_FrozenModeEnable();
}

/**
 *  \brief Routine executed BEFORE the main() function, ONLY by PROCESSOR 1.
 *  \details Processors with ID different from 1 start in a different reset vector.
 *  The gk_START_KERNEL() function is executed AFTER the main() function.
 *  Initialises all the structures to start the GRTOS.
 *  \sa gk_START_KERNEL
 */

void gk_INIT_KERNEL(void)
{
    
#ifndef __niosX_arch__
	/** \brief Stack size (bytes) for each host-simulation ucontext stack.
	 *  Used for uctx_gk_ENTRY_TASK_COMPLETE and uctx_main per PCB entry.
	 *  Not compiled for the Nios V embedded target. */
	#define G_HOST_CONTEXT_STACK_BYTES  (16U * 1024U)

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    Reset_GeMRTOS_controller_registers();


    for (unsigned int i=0; i<G_NUMBER_OF_PCB ; i++) g_kcb.G_PCBTbl[i].ThreadId = 0;

    g_kcb.G_PCBTbl[0].ThreadId = GetCurrentThreadId();
    gk_printf("INIT KERNEL g_kcb.G_PCBTbl[0].ThreadId = 0x%lx, GetCurrentThreadId()= 0x%lx\n", g_kcb.G_PCBTbl[0].ThreadId, GetCurrentThreadId());
    

    for (unsigned int j=0; j< G_NUMBER_OF_PCB; j++) {

        getcontext(&g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE);
        g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE.uc_stack.ss_size = G_HOST_CONTEXT_STACK_BYTES;
        g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE_stack = malloc(g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE.uc_stack.ss_size);
        g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE.uc_stack.ss_sp = g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE_stack;
        g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE.uc_stack.ss_flags = 0;
        makecontext(&g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE, (void (*)()) gk_ENTRY_TASK_COMPLETE, 0);

        getcontext(&g_kcb.G_PCBTbl[j].uctx_main);
        g_kcb.G_PCBTbl[j].uctx_main.uc_stack.ss_size = G_HOST_CONTEXT_STACK_BYTES;
        g_kcb.G_PCBTbl[j].uctx_main_stack = malloc(g_kcb.G_PCBTbl[j].uctx_main.uc_stack.ss_size);
        g_kcb.G_PCBTbl[j].uctx_main.uc_stack.ss_sp = g_kcb.G_PCBTbl[j].uctx_main_stack;
        g_kcb.G_PCBTbl[j].uctx_main.uc_stack.ss_flags = 0;
        makecontext(&g_kcb.G_PCBTbl[j].uctx_main, (void (*)())gk_ENTRY_TASK_COMPLETE, 0);
    }

    for (unsigned int j=2; j <= G_NUMBER_OF_PCB; j++) {
        CreateThread(
                NULL,                             // default security attributes
                0,                                // use default stack size
                gu_processor,                     // thread function name
                (void *) j,                       // argument to thread function
                0,                                // use default creation flags
                &g_kcb.G_PCBTbl[j-1].ThreadId);   // returns the thread identifier
    };

#else

#endif

    // Start without nesting the mutex
    gm_KernelModeReset();
   
    // Set the prescale
    gm_SystemTimePrescaleSet((int) GEMRTOS_DRIVER_PRESCALE);


	// Copy return from interrupt code

    // #############################################
    // Copy start-up to rest of processor to GEMRTOS controller
    for (unsigned int j = 0; j < (unsigned int) ((GRTOS_jump_to_reset_end - GRTOS_jump_to_reset) / 4); j++) {
        gm_IOWR(GEMRTOS_DRIVER_BASE_GMEMORY, j, ((unsigned int *) GRTOS_jump_to_reset)[j]);
    }

    PRINT_ASSERT(((unsigned int) ((GRTOS_jump_to_reset_end - GRTOS_jump_to_reset) / 4) < 8), "ERROR boot sector too long");

    // Copy return from interrupt code
    for (unsigned int j = 0; j < (unsigned int) ((GRTOS_RETURN_CONTEXT_END - GRTOS_RETURN_CONTEXT) / 4); j++) {
        gm_IOWR(GEMRTOS_DRIVER_BASE_GMEMORY, j + 8, ((unsigned int *) GRTOS_RETURN_CONTEXT)[j]);
    }

    PRINT_ASSERT(((unsigned int) ((GRTOS_RETURN_CONTEXT_END - GRTOS_RETURN_CONTEXT) / 4) < 8), "ERROR return from context switching sector too long");
    
    // #############################################
	
	// Set GeMRTOS address of return code
    gm_ERETAddressSet((( (GRTOS_RETURN_CONTEXT_END - GRTOS_RETURN_CONTEXT)) / 4) + 7);

   
#if (G_DEBUG_SYSTEM_CHECK_ENABLE == 1)         // USED TO CHECK SYSTEM INTEGRITY AT START-UP
    gk_SYSTEM_START_UP_CHECK();                // 
#endif  // G_DEBUG_SYSTEM_CHECK_ENABLE == 1    // 
    
	// This is in critical section only for processor 1
	GRTOS_CMD_CRITICAL_SECTION_GET;
    
    gm_KernelModeSet();
	gm_MutexNestedValue(1);


#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	current_function = 0;
    maximum_time = 0;
	maximum_time_function = 0;
	for (unsigned int f = 0; f < 100; f++) {
		SAMPLINGS[f].max_time = 0; 
		SAMPLINGS[f].accumulate_time = 0; 
	}
#endif

	/**********************************************************************************/
	/* Check it is reduced_device_drivers enabled */
/* Embedded-only — scans the Nios BSP IRQ table (alt_irq[]) and writes
 * a JTAG UART error message if any pre-installed HAL IRQ handlers remain.
 * These BSP symbols (alt_irq, IOWR_ALTERA_AVALON_JTAG_UART_DATA) do not exist
 * on Windows; the block is omitted from the host build entirely. */
#ifdef __niosX_arch__
	const char message_reduced_device_driver[] = "ERROR: check and set hal.enable_reduced_device_drivers in BSP settings";
	for (unsigned int n_irq=0 ;  n_irq < ALT_NIRQ ; n_irq++)
	{
		if (alt_irq[n_irq].handler != (void *) 0) {
			alt_ic_irq_disable(0,n_irq);
			for (unsigned int character = 0; character < strlen(message_reduced_device_driver); character++) {
				IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDOUT_BASE , message_reduced_device_driver[character]);
				IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDERR_BASE , message_reduced_device_driver[character]);
			}
			IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDOUT_BASE , '\n');
			IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDERR_BASE , '\n');
		} 
	}
#endif

	PRINT_ASSERT((sizeof(intptr_t ) == sizeof(G_UINT32)),"Code should be compile for 32bit system");
	PRINT_ASSERT((gm_ProcessorId()  == (int) 1),"ERROR Processor is not 1, CPUID= %d\n",(int) gm_ProcessorId());


    gk_Init_KCBLists();            
    gk_Init_TaskDefaults();        
    gk_Init_AssemblerInterface();  
    gk_Init_EventSystem();         

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    gk_fprintf("[ OK ] Finishing gk_INIT_KERNEL\n");
    fflush(stderr);
#endif

}

/***********************************************************************************
*********************** GRTOS RESET FUNCTIONS FOR ID=1 PROCESSOR       *************
***********************************************************************************/

/**
 *  \brief Sets the Lowest Processor register in GRTOS Controller.
 *  Moved from gemrtos_list.c (F5) — only called from gk_START_KERNEL and the
 *  gu_ProcessorWaitForIrq macro, both kernel-level operations.
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Processor
 */
G_UINT32 gk_SetLowestProcessor (void)
{
	TIMEPRIORITY priority, priority2;
    G_UINT32 lowpro = 0;
    for (unsigned int j=0; j< G_NUMBER_OF_PCB; j++) {
        if (g_kcb.G_PCBTbl[j].PCBType != (int) GS_PCBType_UNAVAILABLE &&
            g_kcb.G_PCBTbl[j].PCBState != (int) GS_PCBState_NOTRUNNING) {

            if (lowpro == 0) {
                lowpro = j + 1;
                priority.i64 = g_kcb.G_PCBTbl[j].PCB_EXECTCB->TCBCurrentPriority;
                if (g_kcb.G_PCBTbl[j].PCBState == GS_PCBState_FREE) priority.i32[1] = priority.i32[1] | 0x80000000;
            } else {
                priority2.i64 = g_kcb.G_PCBTbl[j].PCB_EXECTCB->TCBCurrentPriority;
                if (g_kcb.G_PCBTbl[j].PCBState == GS_PCBState_FREE) priority2.i32[1] = priority2.i32[1] | 0x80000000;
                if (priority2.i64 > priority.i64) {
                    priority.i64 = priority2.i64;
                    lowpro = j + 1;
                }
            }
        }
    }
    gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS, ( 1 << CTRL_LOW_PRC_BIT ) | (lowpro & 0xF));
	return(G_TRUE);
}

/* Per-processor watchdog recovery (ANALYSIS-processor-watchdog-reset.md).
 * One global handler, not per-processor -- see gu_WatchdogHandlerSet()'s own
 * doc comment (gemrtos_core.h) for why. Owner: whichever code calls
 * gu_WatchdogHandlerSet(), normally application init, before any processor's
 * watchdog is armed; lifetime is the life of the system; not synchronized
 * against a concurrent setter call (same documented restriction). NULL
 * (its zero-init default) means "no handler registered". */
static G_WatchdogHandler g_watchdog_handler = (G_WatchdogHandler) 0;

void gu_WatchdogHandlerSet(G_WatchdogHandler handler)
{
	g_watchdog_handler = handler;
}

/* Per-processor watchdog recovery (ANALYSIS-processor-watchdog-reset.md).
 * Shared by BOTH branches of gk_START_KERNEL below (CPU1 and non-CPU-1) --
 * factored out so the two call sites cannot silently drift apart, not for
 * its own sake.
 *
 * Detecting "is this actually a recovery" is NOT shared, and deliberately
 * split out of this function (see each call site): non-CPU-1 processors can
 * reuse PCB_EXECTCB != NULL for that (set to NULL exactly once, at true
 * cold boot -- gk_Init_PCBs(), gemrtos_core.c -- and a processor reset,
 * real watchdog or the Windows monitor's terminate+recreate, does not clear
 * g_kcb). **CPU1 cannot reuse that same signal** (confirmed by testing: a
 * false-positive "recovery" on CPU1's own genuine first boot, corrupting
 * state and crashing a DIFFERENT processor shortly after) -- main() runs on
 * CPU1 itself, before gk_START_KERNEL ever first runs there (it is a GCC
 * destructor, fired only after main() returns), and starting the
 * application's own first task during main() already leaves CPU1's
 * PCB_EXECTCB legitimately non-NULL by the time this code's first-ever
 * entry happens -- unlike a non-CPU-1 processor, whose PCB_EXECTCB is
 * still genuinely untouched at its own first entry. CPU1's call site uses
 * a plain "have I ever run before" flag instead; this function assumes its
 * caller has ALREADY decided a recovery is real and ppcb->PCB_EXECTCB is
 * the abandoned task -- it does not re-derive that decision itself. */
static void gk_Watchdog_Recovery_Cleanup(GS_PCB *ppcb)
{
	GS_TCB *abandoned = ppcb->PCB_EXECTCB;
	struct g_watchdog_report report;

	/* Precondition, same idiom used at the top of every other gk_ function
	 * in this file (e.g. gk_TCB_GetPendingSCB() above): this function must
	 * only ever run with THIS processor's own mutex grant already held --
	 * both call sites reach it only after gk_START_KERNEL's own
	 * GRTOS_CMD_CRITICAL_SECTION_GET, never before. Not to be confused
	 * with the mutex-held SCOPE GUARD discussed below, which is a
	 * different question (was the mutex held by the ABANDONED task at the
	 * moment its watchdog fired, before this recovery even started) and is
	 * correctly NOT checked here, for the reason explained there. */
	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);

	/* NOTE on the mutex-held scope guard (Ricardo, 2026-08-25: this
	 * watchdog targets a hung TASK, not the kernel/mutex -- a legitimate
	 * firing never happens while the mutex is held). That check does NOT
	 * belong here: by this point, THIS processor's own
	 * GRTOS_CMD_CRITICAL_SECTION_GET at the top of gk_START_KERNEL
	 * (unchanged, runs identically on a first join) has already acquired
	 * the mutex for its own normal, expected use -- checking
	 * gm_IsPrcGranted() here would always read true regardless of the
	 * abandoned task's actual history and block every recovery, not just
	 * genuine defects (confirmed by testing: this exact mistake was caught
	 * and reverted, see ANALYSIS-processor-watchdog-reset.md). The only
	 * point where "was the mutex held at the moment of firing" is still
	 * knowable is at the trigger itself, before any reset/recovery
	 * activity begins -- GRTOS_Watchdog_Fire() (Windows simulation,
	 * gemrtos_controller_windows.c) already checks R_MTX_PRC_GRN there,
	 * correctly. The real embedded target's equivalent (Phase 2) is still
	 * open: it needs either the real hardware to guarantee this itself, or
	 * a status bit read here that survives the reset -- not yet designed. */

	report.CPUID             = (G_UINT32) gm_ProcessorId();
	report.was_idle          = (abandoned == ppcb->PCB_IDLETCB) ? 1u : 0u;
	report.TaskCode          = abandoned->TCB_TaskCode;
	report.TaskArg           = abandoned->TCB_TaskArg;
	report.TaskReadyPriority = abandoned->TCBReadyPriority;
	report.TaskRunPriority   = abandoned->TCBRunPriority;
	memcpy(report.TaskDescription, (const void *) abandoned->TCB_description, G_TCB_DESCRIPTION_LENGTH);

	if (report.was_idle) {
		/* IDLE's linkage was never touched by the reset (only this
		 * processor's PC/registers were) -- it is still correctly
		 * G_TCBState_RUNNING and already linked in TCBRUNL. Nothing to
		 * clean up. */
	} else {
		/* Still fully, correctly linked (nothing was mid-mutation when
		 * the reset happened, per the scope guard above) -- a normal-
		 * shaped cleanup, reusing the same routine gu_TaskKill() uses
		 * (unlink + return to the free pool). */
		gk_TASK_KILL(abandoned);
	}

	/* In EITHER case above, ppcb->PCB_IDLETCB is left in whatever state
	 * normal scheduling already had it in when this processor was
	 * preempted away from it (confirmed by testing: G_TCBState_READY when
	 * a real task -- not IDLE -- had taken over, never necessarily
	 * G_TCBState_UNLINKED) -- never the first-join precondition
	 * gk_TCBRUNL_Link() requires at either call site. Calling it
	 * unconditionally (the original, first-join-only code) would either
	 * double-link an already-linked IDLE or trip its own UNLINKED assert;
	 * the caller must skip it entirely on a recovery.
	 *
	 * PCB_EXECTCB itself must NOT be left NULL here (confirmed by testing:
	 * a null-pointer crash inside the context-switch machinery immediately
	 * following, which reads PCB_EXECTCB->uctx as the "resume from"
	 * context for gk_KERNEL_TASK_START()'s dispatch -- unlike a genuine
	 * first join, this processor's context-switch machinery is already
	 * live and expects a real object here, same as during any ordinary
	 * task-to-task handoff mid-operation). IDLE's own uctx is a valid,
	 * resumable saved context regardless of its TCBState -- point
	 * PCB_EXECTCB there directly (no relinking needed, see above) as the
	 * safe baseline gk_KERNEL_TASK_START() dispatches from next, exactly
	 * mirroring what an ordinary mid-operation handoff already does
	 * (matches gk_TCBRUNL_Unlink()'s own "detach the task from the
	 * processor" convention, gemrtos_list.c -- already redundant with
	 * that call for the real-task case above, harmless, and the only
	 * assignment at all for the IDLE case). */
	ppcb->PCB_EXECTCB = ppcb->PCB_IDLETCB;

	if (g_watchdog_handler != (G_WatchdogHandler) 0) {
		enum g_watchdog_action action = g_watchdog_handler(&report);

		/* G_WATCHDOG_ACTION_HALT is honoured unconditionally.
		 *
		 * This used to be expressed as PRINT_ASSERT((action !=
		 * G_WATCHDOG_ACTION_HALT), ...), which meant the handler's decision
		 * was acted upon ONLY when G_DEBUG_WHILEFOREVER_ENABLE == 1. In a
		 * production build the assert compiles away, `action` becomes an
		 * unused variable, and a handler that explicitly asked the system to
		 * treat the recovery as fatal was silently ignored -- the system
		 * resumed instead. That broke a documented public API contract
		 * (gu_WatchdogHandlerSet / enum g_watchdog_action, gemrtos_core.h:
		 * "Treat the recovery as fatal: halt with diagnostics") in exactly the
		 * configuration where a safety decision matters most, and it went
		 * unnoticed because the production configuration had never been
		 * compiled.
		 *
		 * Reporting before halting is possible in both configurations only
		 * because C-1 made the console a plain, always-compiled, non-blocking
		 * service: gk_fprintf() exists in production, needs no host attached,
		 * and cannot itself hang while reporting. */
		if (action == G_WATCHDOG_ACTION_HALT) {
			gk_fprintf("ERROR: watchdog handler requested HALT -- cpu=%u task=\"%s\"\n",
				(unsigned int) report.CPUID, report.TaskDescription);
			gkm_PrintFunctionStack();
			G_DEBUG_WHILEFOREVER;
		}
	}
}

/**
 *  \brief Start up of GRTOS OS. Executed after main() returns.
 *  \details gk_INIT_KERNEL is executed before main().
 *  \sa gk_INIT_KERNEL
 */

void gk_START_KERNEL (void)
{
    GS_PCB * ppcb;

    GRTOS_DISABLE_PRC_GENERAL_IRQ;      /// Disable the interrupts in processor (for CPUID /= 1, 1 in INIT_KERNEL)

    /* W-1 watchdog-recovery deadlock break (2026-09-01).
     *
     * A processor reset by its own per-processor watchdog re-enters the
     * kernel here, NOT through gk_ENTRY_IRQ_HANDLER.  If, at the instant the
     * watchdog fired, a peer processor was blocked inside
     * gm_ProcessorInterrupt(this_processor) -- spinning while holding the
     * controller mutex until this processor confirms it has reached its ISR
     * (R_PRC_IRQ_SECTION) -- that peer never releases the mutex, and the
     * GRTOS_CMD_CRITICAL_SECTION_GET just below would block forever: a
     * circular wait between the two processors.
     *
     * The controller keeps the pending processor interrupt (R_PRC_INT)
     * asserted across the reset -- it is cleared only by a full controller
     * reset, not by the per-processor slave reset.  Acknowledge it here,
     * BEFORE contending for the mutex, exactly as gk_ENTRY_IRQ_HANDLER does
     * (and for the same reason): gm_PrcIRQSecSet() sets R_PRC_IRQ_SECTION,
     * which releases the spinning peer, and needs no mutex.
     *
     * Guarded on R_PRC_INT actually being pending for this processor so the
     * ordinary path (cold boot, or a recovery with no peer waiting) is left
     * byte-for-byte unchanged.  The controller AND-gates the acknowledge
     * with R_PRC_INT internally (PRC_IRQ_ACK), so an unguarded call would
     * also be correct; the guard just keeps the common path untouched. */

    if ((gm_GetPrcINT() & ((G_UINT32) 1u << (gm_ProcessorId() - 1u))) != 0u) {
        gm_PrcIRQSecSet();
    }


	// Get the mutex to enter in start task without nesting
	GRTOS_CMD_CRITICAL_SECTION_GET;

    gm_KernelModeSet();
	gm_MutexNestedValue(1);
	
	GK_SAMPLE_FUNCTION_BEGIN(10056)
	
	gm_PrcInterruptEnable();	                 // Enable processor interrupt in GeMRTOS controller
	
	PRINT_ASSERT((gm_ProcessorId()  <= G_NUMBER_OF_PCB),"ERROR: wrong gm_ProcessorId() = %u", (unsigned int) gm_ProcessorId());

/* Platform dispatch — on Windows host builds only, prints the processor
 * number at gk_START_KERNEL entry for debugging.  On embedded Nios, the JTAG
 * UART is not yet initialised at this point so the log is omitted. */


    if (gm_ProcessorId()  == (int) 1) {

		/* CPU1 has no equivalent of gu_processor()/GRTOS_Watchdog_Recovery_
		 * Entry() to call it back into (its very first entry here is a GCC
		 * destructor -- gemrtos_core.h -- fired once, automatically, by the
		 * C runtime after main() returns; nothing in application code calls
		 * gk_START_KERNEL() for CPU1 directly). A watchdog recovery for
		 * CPU1 instead calls this same function directly from a fresh
		 * thread (GRTOS_Watchdog_Recovery_Entry() is CPUID-agnostic, works
		 * for CPU1 too), landing back here exactly like the destructor-
		 * triggered first entry did.
		 *
		 * Detecting the difference uses a plain "have I ever run before"
		 * flag, NOT ppcb->PCB_EXECTCB (unlike the non-CPU-1 branch below) --
		 * confirmed by testing that PCB_EXECTCB is NOT a reliable cold-boot
		 * signal for CPU1 specifically: main() runs on CPU1 itself, before
		 * this destructor-triggered first entry, and starting the
		 * application's own first task during main() already leaves
		 * PCB_EXECTCB legitimately non-NULL by the time this code's real
		 * first entry happens (a false "recovery" here corrupted state and
		 * crashed a DIFFERENT processor shortly after -- see
		 * ANALYSIS-processor-watchdog-reset.md). The two steps below
		 * (gu_printf_sem creation, the system-wide time-counter reset) must
		 * run exactly once, ever -- re-running them on a recovery would
		 * leak/orphan the old semaphore and corrupt every OTHER processor's
		 * sense of elapsed time (gm_TimeCountersReset() is not scoped to
		 * this processor). */
		static G_UINT32 g_cpu1_first_entry_done = G_FALSE;

		ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[0];

		if (!g_cpu1_first_entry_done) {
			g_cpu1_first_entry_done = G_TRUE;

/* Embedded-only — registers the GeMRTOS instruction-exception handler
 * with the Nios BSP (alt_instruction_exception_register).  This BSP API does
 * not exist on Windows; the block is omitted from the host build entirely. */
#ifdef __niosX_arch__
	#ifdef ALT_INCLUDE_INSTRUCTION_RELATED_EXCEPTION_API
		alt_instruction_exception_register(handler);
	#endif      // ALT_INCLUDE_INSTRUCTION_RELATED_EXCEPTION_API
#endif     // __niosX_arch__

			// Create semaphore for gu_printf exclusion
			gu_printf_sem = gu_SemaphoreCreateMutex();

			gm_TimeCountersReset();       // Reset frozen and non-frozen time counters
			gm_TimeCoutersUnreset();      // Unreset frozen and non-frozen time counter
		} else {
			/* A genuine recovery -- ppcb->PCB_EXECTCB is whatever CPU1 was
			 * legitimately running when its watchdog fired. */
			gk_Watchdog_Recovery_Cleanup(ppcb);
		}
		/* No gk_TCBRUNL_Link(ppcb->PCB_IDLETCB) call here, on either path --
		 * never had one, even for the original first-entry case. CPU1's own
		 * first dispatch is instead handled by Swap_kernel_to_task()'s own
		 * defensive NULL-substitution ("PCB_EXECTCB NULL on proc %u,
		 * dispatching IDLE", gemrtos_controller_windows.c) the first time it
		 * runs with ppcb->PCB_EXECTCB still NULL. On a recovery,
		 * gk_Watchdog_Recovery_Cleanup() above already leaves PCB_EXECTCB
		 * pointing at IDLE directly, so that same dispatch path finds a
		 * real object either way. */

	} else {  // if (gm_ProcessorId()  != (int) 1)

        gk_printf("RUNNING -> gm_ProcessorId= %u, %s, %s, %d \n", gm_ProcessorId(), __FILE__, __FUNCTION__, __LINE__);
		// Include the processor in the GRTOS
		ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[gm_ProcessorId() -1];
		PRINT_ASSERT((gkm_PCB_IsValid(ppcb) == G_TRUE),"ERROR PCB is not valid: ppcb= %p, Haash= %p\n", (void *) ppcb, (void *) ppcb->BLOCK_HASH);

		/* Unlike CPU1 (see above), PCB_EXECTCB itself IS the reliable
		 * cold-boot-vs-recovery signal here: this processor never runs
		 * anything (not even the equivalent of main()) before its own
		 * gk_START_KERNEL entry, so PCB_EXECTCB is still genuinely NULL at
		 * a real first join. */
		if (ppcb->PCB_EXECTCB != (struct gs_tcb *) 0) {
			gk_Watchdog_Recovery_Cleanup(ppcb);
		} else {
			// Set the ppcb->PCB_IDLETCB running in the current processor
			// Assign the IDLE task to current processor and assign it as running.
			gk_TCBRUNL_Link(ppcb->PCB_IDLETCB);
		}
    }

	GRTOS_ENABLE_PRC_IRQ;            /// Enable Processor Interrupt in the processor

	GK_SAMPLE_FUNCTION_END(10056);

    gk_KERNEL_TASK_START();            /// Start the first next task


/* Platform dispatch — on embedded Nios, gk_KERNEL_TASK_START must not
 * return; if it does, this block prints an error and halts (while(1)).
 * On Windows host builds, the return path is expected and triggers
 * __gcov_dump() to flush coverage data before the process exits. */
#ifdef __niosX_arch__                                                                
	gk_fprintf("******************************************** \n");
	gk_fprintf("ERROR Processor %d Init GRTOS \n", gm_ProcessorId() );
	gk_fprintf("******************************************** \n");
    /****************************************************/
    /****************************************************/
    /* Code to produce assembler in objdump file        */
    /* IT IS NEVER EXECUTED                             */
    /****************************************************/
    /****************************************************/
	while(1);
#else                                                                                
	__gcov_dump();                                                                   
	fflush(stdout);                                                                  
#endif                                                                               

}


/**
 *  \brief Configures system to execute the next pending signal. Called from the switch routine.
 *  \relates Task
 */
void gk_TCB_GetPendingSCB(void)
{
	GS_TCB *ptcb;
	GS_SCB *psignal;

    GK_SAMPLE_FUNCTION_BEGIN(10024)
	
	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);
	PRINT_ASSERT((gm_KernelModeGet() == G_TRUE), "ERROR in Kernel mode");
    
	G_SCB_PENDING = 0;               /* Default is no pending SCB */
	ptcb = gk_PCB_GetCurrentTCB();   /* Get Current Task          */	
	
	/* Check KCB pending signals */
	psignal = g_kcb.KCB_NextKCBPSL;
	
	/* There is a pending SCB to be executed in kernel mode */   
	while ((psignal != (struct gs_scb *) 0))    
	{
		PRINT_ASSERT(((get_pointed_field(psignal, SCBState) == G_SCBState_PENDING)),"ERROR SCBState= %u\n", get_pointed_field(psignal, SCBState));
	
		/* Signal to execution     */
		set_pointed_field(psignal, SCBState, G_SCBState_EXECUTING);
		G_SCB_CODE  = (G_UINT32) get_pointed_field(psignal, SCB_TaskCode); 
		G_SCB_ARG     = (G_UINT32) get_pointed_field(psignal, SCB_TaskArg);       /* Task Argument                 */
		gk_KCBPSL_Unlink((GS_SCB *) psignal);            /* Unlink SCB    */
				
		gk_SCBFL_Link((GS_SCB *) psignal);               /* Link SCB to free list     */
	
		PRINT_ASSERT(((gm_IsPrcGranted())), "ERROR in IDLE task\n");
		PRINT_ASSERT((gm_KernelModeGet() == G_TRUE), "ERROR in Kernel mode");
		
		void (*funcptr)(void *) = (void (*)(void *)) G_SCB_CODE;
		(*funcptr)((void *) G_SCB_ARG);
		
		PRINT_ASSERT(((gm_IsPrcGranted())), "ERROR in IDLE task \n");
		PRINT_ASSERT((gm_KernelModeGet() == G_TRUE), "ERROR in Kernel mode");
        
		psignal = g_kcb.KCB_NextKCBPSL;
	}

	/* Check TCB pending signals */
	psignal = get_pointed_field(ptcb, TCB_NextTCBPSL);  /* Get Next Pending Signal   */
	if ((psignal != (struct gs_scb *) 0))    /* There is a pending SCB    */
	{
		PRINT_ASSERT(((get_pointed_field(psignal, SCBState) == G_SCBState_PENDING)),"ERROR SCBState= %u\n", get_pointed_field(psignal, SCBState));
		
		/* Signal to execution     */
		set_pointed_field(psignal, SCBState, G_SCBState_EXECUTING);
		/* Task Code                     */
		G_SCB_CODE    = (G_UINT32) get_pointed_field(psignal, SCB_TaskCode) + G_RETURN_OFFSET_FROM_INTERRUPT; 
		G_SCB_ARG     = (G_UINT32) get_pointed_field(psignal, SCB_TaskArg);  /* Task Argument                 */
		gk_TCBPSL_Unlink(ptcb, (GS_SCB *) psignal);                         /* Unlink SCB from TCB           */
		gk_SCBFL_Link((GS_SCB *) psignal);                                  /* Link SCB to free list         */
		G_SCB_PENDING = 1;                                                  /* Set the Pending Signal status */
	}

	PRINT_ASSERT(((gm_IsPrcGranted())), "ERROR in IDLE task\n");
	PRINT_ASSERT((gm_KernelModeGet() == G_TRUE), "ERROR in Kernel mode");

    // Set the lowest processor in GeMRTOS controller
    gk_SetLowestProcessor();

	// Set next occurrence time and processor
     
	gk_NextOccTimeProcessor();
	gm_SetNextOccurrenceTime(g_kcb.KCB_NextECBTL->ECBValue.i64);

	/* Set the Mutex nested register with the current task */
	gm_MutexNestedValue(ptcb->TCB_MTX_NESTED);

	/* Update critical section for task after interrupt return */
    gm_KernelModeReset();
   

    GK_SAMPLE_FUNCTION_END(10024)
}

/**********************************************************************************
 *                        	Exception handler
 *  from http://www-ug.eecg.toronto.edu/msl/manuals/n2sw_nii52006.pdf
 *********************************************************************************/
/* Embedded-only — the Nios instruction-exception handler function.
 * Uses Nios BSP types (alt_exception_result, alt_exception_cause, alt_u32)
 * and is registered via alt_instruction_exception_register (see above).
 * No equivalent exists on Windows; the entire function is omitted from the
 * host build. */
#ifdef __niosX_arch__
alt_exception_result handler(alt_exception_cause cause,
                              alt_u32 addr,
                              alt_u32 bad_addr )
{
    gk_fprintf("[ EXCEPTION ] cause= %d, address= 0x%x, bad_addr: 0x%x\n", (int) cause, (unsigned int) addr, (unsigned int) bad_addr);
    while(1);
    return (alt_exception_result) 0;
}
#endif

/**gk_restore
 *  \brief Emergency restore handler called when a processor fails to return normally.
 *  \details Disables interrupts, prints the failing processor ID, and halts in a
 *           while(1) loop. Indicates a fatal scheduling error; not expected in
 *           normal operation.
 *  \note This function never returns.
 *  \relates Processor
 */
void gk_restore(void)
{
	GRTOS_DISABLE_PRC_GENERAL_IRQ;                 /// Disable the interrupts
	
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    gk_fprintf("autorestore proc= %u\n", (unsigned int) gm_ProcessorId());
#endif
    while(1);
}

/**********************************************************************************
 *                        	IDLE TASK DOING NOTHING
 *********************************************************************************/
/**gk_WAIT_IDLE_TASK
 *  \brief Halts the idle processor until the GeMRTOS Controller signals it to resume.
 *  \details Sets the processor to idle-halt mode via gm_HaltIdlePrcSet(), then
 *           spins until the ADDR_REG_0 halt bit is cleared by the controller.
 *           Called from gk_CODE_IDLE_TASK when no runnable task is available.
 *  \note Must not be called from a task context. Only the IDLE task may call this.
 *  \relates Processor
 */
void gk_WAIT_IDLE_TASK(void)
{
	gm_HaltIdlePrcSet();
	while ((gm_IORD(GEMRTOS_DRIVER_PROCESSOR_BASE, ADDR_REG_0) & 1) == 1) {
		while(0);
	}
}

/**gk_CODE_IDLE_TASK
 *  \brief IDLE task. This code is executed when no task is ready. The processor is halted.
 *  \param [in] pdata Pointer to data
 *  \details Task can be changed. The processors run this task with different stacks
 *  \relates Task
 */
void gk_CODE_IDLE_TASK(void* pdata)
{
	/*********************************************************************/
	/* DO NOT USE gu_printf NEITHER ANY FUNCTION THAT SUSPENDS THE TASK  */
	/* IDLE TASK CANNOT BE SUSPENDED                                     */
	/*********************************************************************/

	while(1)
    {
        #ifdef ALT_SEMIHOSTING
            gu_fprintf("ALT_SEMIHOSTING defined\n");
        #endif
        #ifdef ALT_USE_DIRECT_DRIVERS
            gu_fprintf("ALT_USE_DIRECT_DRIVERS defined\n");
        #endif

		PRINT_ASSERT((!(gm_IsPrcGranted())), "ERROR in IDLE task\n");

		// Put the procesor in HALT mode
		gm_ProcessorHalt;

		/* GRTOS_READ_SP reads the hardware stack-pointer register via a
		 * Nios II/V-specific assembly macro.  No Windows-simulation equivalent
		 * exists (that flavour never defines __niosX_arch__). Also excluded
		 * from the host kernel-unit-test build (GEMRTOS_TEST_NO_AUTOSTART) --
		 * see the identical reasoning on gk_KERNEL_TASK_COMPLETE's own copy
		 * of this check, above. Diagnostic-only. */
		#if defined(__niosX_arch__) && !defined(GEMRTOS_TEST_NO_AUTOSTART)
			#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    	    void *StackPointer = (void *) 0;   // Check that SP is in the stack of the IDLE task of the processor
            GRTOS_READ_SP(StackPointer);

            PRINT_ASSERT(((void *) StackPointer <= (void *) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_IDLETCB->TCB_StackBottom),"ERROR SP out of bottom = 0x%p\n",(void *) GRTOS_CMD_PRC_SP);

            PRINT_ASSERT(((void *) StackPointer >= (void *) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_IDLETCB->TCB_StackTop),"ERROR SP out of top = 0x%p\n",(void *) GRTOS_CMD_PRC_SP);

			PRINT_ASSERT(( g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_IDLETCB == g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB), "ERROR Exec task is %s but should be IDLE", g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_description);

			#endif
		#endif

		if (gm_IsPrcGranted()) {
			gk_fprintf("R_STATUS= 0x%x\n", (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS));
			gk_fprintf("R_PRC_IRQ_SECTION= 0x%x\n", (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_PRC_IRQ_SECTION));
			PRINT_ASSERT((gkm_Check_GeMRTOS(0) == G_TRUE),"ERROR in GK_SAMPLE_FUNCTION_END\n");
		}

		PRINT_ASSERT((!(gm_IsPrcGranted())), "ERROR in IDLE task task= %s, TCB_MTX_NESTED= %u\n", g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_description, (unsigned int) g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_EXECTCB->TCB_MTX_NESTED);
    }
}

/**
 *  \brief Entry point for processors other than CPU 1 during boot.
 *  \details Logs the processor ID if testing mode is active, then calls
 *           gk_START_KERNEL() to begin normal execution.
 *  \relates Processor
 */
void gk_PROCESSOR_NO1_START_UP_CHECK(void)           
{
    /* Reached from the boot stub, BEFORE gk_START_KERNEL takes the mutex --
     * and also on the watchdog-recovery path, where blocking here would stall
     * every processor and could not be broken by the recovery itself. The
     * gk_ console is used precisely because it never blocks; it carries the
     * same single-writer exposure the raw fprintf it replaced already had
     * (processors reach this point staggered by the controller's start-up
     * reset sequencing), so this is no weaker than before and cannot hang. */
    // Check if processor is being testing
    if (G_Testing_Counter != 0) {
        gk_fprintf("[ MESSAGE ] #####################################################################\n");
        gk_fprintf("[ MESSAGE ] Processor %u running, Testing= %u\n", gm_ProcessorId(), G_Testing_Counter);
        gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");
        G_Testing_Counter++;
    }
    gk_fprintf("[ MESSAGE ] Processor %u running\n", gm_ProcessorId());
    gk_START_KERNEL();
}

/**
 *  \brief Boot-time self-test; verifies timers, mutex, and hardware registers.
 *  \return G_TRUE when all checks pass.
 *  \relates Processor
 */
G_UINT32 gk_SYSTEM_START_UP_CHECK(void)           
{

    TIMEPRIORITY hw_time;

    PRINT_ASSERT((gm_KernelModeGet() == G_FALSE), "ERROR in Mutex\n");
    PRINT_ASSERT((gk_CheckTimePriorityLayout()),"ERROR system is no little endiand\n");
    G_Testing_Counter = 1;

    // Print BOOTING information
    gk_fprintf("[ MESSAGE ] #####################################################################\n");
    gk_fprintf("[ MESSAGE ] GeMRTOS SYSTEM CHECK\n");
    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");
    gk_fprintf("[ MESSAGE ] Processor type  = %s\n", GEMRTOS_DRIVER_PROCESSOR_TYPE);
    gk_fprintf("[ MESSAGE ] Processors      = %d\n", (int)GEMRTOS_DRIVER_NPROCESSORS);
    gk_fprintf("[ MESSAGE ] Generation ID   = %d\n", (int)GEMRTOS_DRIVER_GRTOSGENERATIONID);
    gk_fprintf("[ MESSAGE ] Processor rate  = %d MHz \n", (int)GEMRTOS_DRIVER_NIOS_CLOCK_FREQUENCY);
    
    gk_fprintf("[ MESSAGE ] Clock prescaler = %d (from configuration) \n", (int)GEMRTOS_DRIVER_PRESCALE);
    gk_fprintf("[ MESSAGE ] Reg. prescaler  = %d (from register)\n", (int)gm_SystemTimePrescaleGet());
    PRINT_ASSERT(((int)GEMRTOS_DRIVER_PRESCALE == (int)gm_SystemTimePrescaleGet()), "ERROR Prescaller do not configured correctly");
    gk_fprintf("[ MESSAGE ] G_TICKS_PER_SECOND %d\n", (int) G_TICKS_PER_SECOND);
    
    gk_fprintf("[ MESSAGE ] HSP Access      = %s\n", GEMRTOS_DRIVER_ENABLE_HPS_MAP_ACCESS);

    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");
    gk_fprintf("[ MESSAGE ] GeMRTOS Memory spece\n");
    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");
    gk_fprintf("[ MESSAGE ] Program memory  = %d %s\n", (int)GEMRTOS_DRIVER_EXTERNAL_MEMORY_SPAN, GEMRTOS_DRIVER_EXTERNAL_MEMORY_UNIT);
    gk_fprintf("[ MESSAGE ] Address width   = %d\n", (int)GEMRTOS_DRIVER_BUS_WIDTH);
    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");


    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");
    gk_fprintf("[ MESSAGE ] GeMRTOS Mutex check\n");
    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");

    gk_fprintf("[ MESSAGE ] STATUS : R1_MTX_NESTED   = %d\n", (int) ((((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS)) >> STATUS_MTX_NESTED) & 0x1));
    PRINT_ASSERT((((((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS)) >> STATUS_MTX_NESTED) & 0x1) == 0), "ERROR NESTED hold");

    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");
    gk_fprintf("[ MESSAGE ] GeMRTOS Timing check\n");
    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");
    gk_fprintf("[ MESSAGE ] Checking System time\n");
        
    PRINT_ASSERT((gm_TimeCountersResetGet() == 1), "ERROR in time reseted\n");
    
    // System time should be equal to 0 
    hw_time.i32[1] = (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SYS_TM_HGH);
    hw_time.i32[0] = (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SMP);    
    PRINT_ASSERT((hw_time.i64 == (G_UINT64) 0), "ERROR System Time does not reset, value= 0x%llx\n", hw_time.i64);
    
    gm_TimeCoutersUnreset();      // Unreset frozen and non-frozen time counter
    // System time should be equal to 0 
    hw_time.i32[1] = (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SYS_TM_HGH);
    hw_time.i32[0] = (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SMP);    
    PRINT_ASSERT((hw_time.i64 != (G_UINT64) 0), "ERROR System Time does not reset");    
    
    gm_TimeCountersReset();       // Reset frozen and non-frozen time counters
    // System time should be equal to 0 
    hw_time.i32[1] = (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SYS_TM_HGH);
    hw_time.i32[0] = (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SMP);
    PRINT_ASSERT((hw_time.i64 == (G_UINT64) 0), "ERROR System Time does not reset");

    /* hw_time is read only by the PRINT_ASSERTs above, which compile out when
     * G_DEBUG_WHILEFOREVER_ENABLE == 0. The register reads themselves are
     * unconditional and stay; only the value becomes unused. Consumed here so
     * that a production build compiles warning-free -- a build with standing
     * warnings is how the six real production-only warnings this addresses
     * went unnoticed in the first place. */
    (void) hw_time;
    
    gk_fprintf("[ MESSAGE ] System time PASS\n");
    
    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n"); 
    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");
    gk_fprintf("[ MESSAGE ] GeMRTOS Processor reset check\n");
    gk_fprintf("[ MESSAGE ] ---------------------------------------------------------------------\n");
    gk_fprintf("[ MESSAGE ] Checking System initial state\n");

/* Hardware register checks — reads GeMRTOS controller registers via
 * GEMRTOS_DRIVER_BASE_TESTING (MMIO base address defined by the Nios BSP).
 * These MMIO addresses and the PRINT_ASSERT checks require the embedded
 * hardware to be present; the entire block is omitted on Windows host builds. */
#ifdef __niosX_arch__
    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_WAITING_IN_ISR) == 0),"ERROR in ADDR_WAITING_IN_ISR = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_WAITING_IN_ISR));

    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_MTX_RSV_PRC) == 0),"ERROR in ADDR_MTX_RSV_PRC = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_MTX_RSV_PRC));

    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_PRC_INT) == 0),"ERROR in ADDR_PRC_INT = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_PRC_INT));

    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_PRC_INT_ENB) == 0),"ERROR in ADDR_PRC_INT_ENB = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_PRC_INT_ENB));

    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_slave_irq) == 0),"ERROR in ADDR_slave_irq = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_slave_irq));

    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_PRC_INT_STA) == 0),"ERROR in ADDR_PRC_INT_STA = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_PRC_INT_STA));

    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_TIMED_INT) == 0),"ERROR in ADDR_TIMED_INT = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_TIMED_INT));

    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_EXTERN_INT) == 0),"ERROR in ADDR_EXTERN_INT = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_EXTERN_INT));

    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_NXT_TM_EVN_PRC) == 0),"ERROR in ADDR_NXT_TM_EVN_PRC = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_NXT_TM_EVN_PRC));


    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_IRQ_ENB) == 0),"ERROR in ADDR_IRQ_ENB = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_IRQ_ENB));                

    PRINT_ASSERT(((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_MTX_RLS_BIT) == 0),"ERROR in ADDR_MTX_RLS_BIT = 0x%x\n", gm_IORD(GEMRTOS_DRIVER_BASE_TESTING, ADDR_MTX_RLS_BIT));                 

#endif  // __niosX_arch__

    gk_fprintf("[ MESSAGE ] CTRL_STATUS      = 0x%x\n", (unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS));
    gk_fprintf("[ MESSAGE ] STATUS : Prescaller      = %d\n", (int) ((((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS)) >> 16) & 0x1F));
	gk_fprintf("[ MESSAGE ] STATUS : R1_MTX_NESTED   = %d\n", (int) ((((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS)) >> STATUS_MTX_NESTED) & 0x1));
	gk_fprintf("[ MESSAGE ] STATUS : R1_HLD_CNT_RST   = %d\n", (int) ((((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS)) >> STATUS_HLD_CNT_BIT) & 0x1));
	gk_fprintf("[ MESSAGE ] STATUS : FRZ_ACT_BIT   = %d\n", (int) ((((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS)) >> STATUS_FRZ_ACT_BIT) & 0x1));
	gk_fprintf("[ MESSAGE ] STATUS : FRZ_ENB_BIT   = %d\n", (int) ((((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS)) >> STATUS_FRZ_ENB_BIT) & 0x1));
	gk_fprintf("[ MESSAGE ] STATUS : TIME_HOLD_BIT   = %d\n", (int) ((((unsigned int) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_CTRL_STATUS)) >> STATUS_TIME_HOLD_BIT) & 0x1));
    
    gk_fprintf("[ MESSAGE ] System initial state PASS\n");
    
    gk_fprintf("[ MESSAGE ] #####################################################################\n");
    
    
    G_UINT64 threshold_ticks = gu_ConvertTime(0, 0, 0, 500); // Define the timeout
    gu_FrozenModeThresholdSet(threshold_ticks);               // Set the frozen threshold
    gm_FrozenModeEnable();                                    // Enable the frozen mode

    gk_fprintf("[ MESSAGE ] #####################################################################\n");	



    gk_fprintf("[ MESSAGE ] GeMRTOS controller information ###########\n");

    gk_fprintf("[ MESSAGE ] System time register = 0x%llx\n", (unsigned long long) gu_SystemTotalTimeGet());
 
    gk_fprintf("[ MESSAGE ] Frozen threshold register (R_FRZ_TM_THR) = 0x%llx\n", (unsigned long long) gu_FrozenModeThresholdGet());
    gk_fprintf("[ MESSAGE ] GCC information ###########\n");
    gk_fprintf("[ MESSAGE ] sizeof(unsigned)   = %d\n", (int) sizeof(unsigned));
    gk_fprintf("[ MESSAGE ] sizeof(unsigned long)   = %d\n", (int) sizeof(unsigned long));
    gk_fprintf("[ MESSAGE ] sizeof(unsigned long long)   = %d\n", (int) sizeof(unsigned long long));
    gk_fprintf("[ MESSAGE ] sizeof(intptr_t )   = %d\n", (int) sizeof(intptr_t ));
    gk_fprintf("[ MESSAGE ] GEMRTOS_DRIVER_PRESCALE %d\n", (int) GEMRTOS_DRIVER_PRESCALE);

    
    return (G_TRUE);                               
}                                                  


OPTIMIZE_RESTORE

