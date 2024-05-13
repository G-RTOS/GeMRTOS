/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Kernal functions
 *  \details This file contains the Kernal functions.
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
#include <gemrtos.h>


OPTIMEZE_CODE(0)

/*****************************************************************************/
/*****************************************************************************/
/**         SYSTEM VARIABLES                                                 */
/*****************************************************************************/
/*****************************************************************************/

/* Data Structure */
GS_KCB  g_kcb __attribute__((aligned(4)));           ///< \brief Structure of GS_KCB

// const unsigned int G_PROCESSOR_MASK[] = {0x0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0xFFFFFFFF};

/// GLOBAL VARIABLES to hold states for assembler interface
volatile GS_STK G_TCB_CURRENT __attribute__((aligned(4)));    ///< \brief Holds the TCB of current task to switch
volatile G_INT32 G_SCB_PENDING __attribute__((aligned(4)));     ///< \brief Holds the SCB for pending signal
volatile G_INT32 G_SCB_CODE __attribute__((aligned(4)));        ///< \brief Holds the Code Address for pending signal
volatile G_INT32 G_SCB_ARG __attribute__((aligned(4)));         ///< \brief Holds the Args Address for pending signal

/// \brief IRQ number of the GRTOS controller in processors
/// \todo explain correctelly
// volatile int GRTOS_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID;

/// \brief Define
/// \todo explain correctelly
volatile int GRTOS_S_PROCESSOR1_IRQ;

/* Interrupt status before GRTOS start */
volatile int G_IRQ_ENABLED __attribute__((aligned(4)));  ///< \brief Holds the enable status of the IRQs
volatile int G_IRQ_PENDING __attribute__((aligned(4)));  ///< \brief Holds the Pendign IRQs at start up

/// \brief GRTOS Controller addresses to be used in assembler code 
// volatile G_INT32 GRTOS_MutexReleaseRegisterAddress __attribute__((aligned(4)));
// volatile G_INT32 GRTOS_MutexRequestRegisterAddress __attribute__((aligned(4)));
// volatile G_INT32 GRTOS_InterruptEnableRegisterAddress __attribute__((aligned(4)));
// volatile G_INT32 GRTOS_InterruptDisableRegisterAddress __attribute__((aligned(4)));


volatile G_INT32  G_Running __attribute__((aligned(4)));  ///< \brief Flag indicating Scheduler is enabled: G_TRUE or G_FALSE
volatile G_INT32  G_Running = G_FALSE; 


// volatile G_INT32  G_RUNNING2 __attribute__((aligned(4)));  ///< \brief Flag indicating Scheduler is enabled: G_TRUE or G_FALSE
// volatile G_INT32  G_RUNNING2 = G_FALSE; 

volatile G_INT32  gs_sizeof_G_PCBTbl __attribute__((aligned(4)));           ///< \brief Values for assembler code
volatile void   *gs_addressof_G_PCBTbl __attribute__((aligned(4)));           ///< \brief Values for assembler code
volatile G_INT32  gs_offsetof_PCB_EXECTCB __attribute__((aligned(4)));      ///< \brief Values for assembler code
volatile G_INT32  gs_offsetof_TCB_StackPointer __attribute__((aligned(4))); ///< \brief Values for assembler code
volatile G_INT32  gs_offsetof_PCB_IDLETCB __attribute__((aligned(4)));      ///< \brief Values for assembler code

volatile int G_DEBUG_SAMPLE_BEGIN_ENABLE __attribute__((aligned(4)));     ///< \brief Values to enable sampling function starts (0xffffffff for all functions)
volatile int G_DEBUG_SAMPLE_END_ENABLE __attribute__((aligned(4)));       ///< \brief Values to enable sampling function ends (0xffffffff for all functions)

/* Semaphore to exclude gu_fprintf access */
t_semaphore_resource *gu_fprintf_sem;
t_semaphore_resource *gu_printf_sem;

FILE* fpuart[G_NUMBER_OF_PCB] __attribute__((aligned(4)));    ///< \brief Files for JTAG-UARTs


volatile G_INT32 G_TASK_TYPE_DEFAULT __attribute__((aligned(4)));        ///< \brief Default assignment for Task Type
volatile GS_LCB *G_TASK_LCB_DEFAULT __attribute__((aligned(4)));       ///< \brief Default assignment for Task LCB
volatile G_INT32 G_TASK_LCB_DEFAULT_PRIORITY __attribute__((aligned(4)));       ///< \brief Priority of the default LCB
volatile G_INT64 G_TASK_PRIORITY_DEFAULT __attribute__((aligned(4)));    ///< \brief Default assignment for Task Priority
volatile G_INT64 G_TASK_PERIOD_DEFAULT __attribute__((aligned(4)));      ///< \brief Default assignment for Task Period

TIMEPRIORITY OC_priority;
TIMEPRIORITY OC_priority2;

/***********************************************************************************
*********************** GRTOS INTERRUPT FUNCTIONS  ********************************
***********************************************************************************/
/**gk_ENTRY_IRQ_HANDLER
 *  \brief Executes the ISR of GRTOS
 *  \details This routine is executed each time processor is interrupted
 */

void gk_ENTRY_IRQ_HANDLER (void)
{
    // ********************************************* //
    // DO NOT DEFINE LOCAL VARIABLES.                //
    // THE FUNCTION CHANGES THE STACK POINTER        //
    // AND CALL START_TASK TO BALANCE FRAME POINTER  //
    // ********************************************* //
    
	GRTOS_CMD_PRC_INT_DSB;             /// Disable processor interrupt in GRTOS controller
	GRTOS_CMD_INT_PRC_PND_CLR;         /// Remove pending interrupt of current processor
		
	GRTOS_CMD_CRITICAL_SECTION_GET;   /// Get into critical section
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_TRUE;
    GRTOS_DISABLE_PROC_INTERRUPTS;  /// Disable processor interrupt

    gk_KERNEL_TASK_START ();
}


/**********************************************************************************
 *                        	BAD RETURN FUNCTION IN TASK STACK
 * THIS IS THE RETURN ADDRESS WHEN A BAD RETURN IS EXECUTED IN A TASK SWITCH
 *********************************************************************************/
/**gk_ENTRY_BAD_TASK_RETURN
 *  \brief BAD RETURN FUNCTION IN TASK STACK
 *  \details THIS IS THE RETURN ADDRESS WHEN A BAD RETURN IS EXECUTED IN A TASK SWITCH
 *  \todo implement a safe recovery of the system
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
/**gk_ENTRY_SIGNAL_RETURN
 *  \brief Return from Signal:
 *  \details Get critical section and return to GRTOS_Return_from_Signal in grtos_switch_XXXX.S file
 */
void gk_ENTRY_SIGNAL_RETURN(void)
{
    // ********************************************* //
    // DO NOT DEFINE LOCAL VARIABLES.                //
    // THE FUNCTION CHANGES THE STACK POINTER        //
    // AND CALL START_TASK TO BALANCE FRAME POINTER  //
    // ********************************************* //
    
	// GRTOS_USER_CRITICAL_SECTION_GET;  	
	GRTOS_CMD_CRITICAL_SECTION_GET;    /// Get into critical section
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_TRUE;

	/* Set the Mutex nested register with the current task */
	IOWR_GRTOS_MTX_NESTED(g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_MTX_NESTED);
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_FALSE;
	
    GRTOS_DISABLE_PROC_INTERRUPTS;  /// Disable processor interrupt
    // NIOS2_WRITE_SP((G_INT32) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackPointer);    
    
    // PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == GRTOS_CMD_PRC_ID),"ERROR SIGNAL in critical section= %d, proc=%d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) GRTOS_CMD_PRC_ID);
    // gk_KERNEL_TASK_START ();    
    
}

/**
 *  \brief gk_KERNEL_PROCESSOR_START_UP is called to configure the processor´s structures
 */
// void gk_KERNEL_PROCESSOR_START_UP (void)
// {
//     GS_PCB * ppcb;
//     // G_NUMBER_OF_IDLE_PROCESSORS is the number of processors wanted to be in idle state forever
//     // They are sent to the gk_RST_MONITOR_HANDLER in order to left them out of the system runtime
//     if (GRTOS_CMD_PRC_ID  > G_NUMBER_OF_PCB - G_NUMBER_OF_IDLE_PROCESSORS) gk_RST_MONITOR_HANDLER();
// 	
//     GRTOS_DISABLE_DEVICES_IRQs;             /// Disable the interrupts (for CPUID /= 1)
//     GRTOS_DISABLE_PROC_INTERRUPTS;          /// Disable processor interrupt	
// 
// 	// Get the mutex to enter in start task without nesting
// 	GRTOS_CMD_CRITICAL_SECTION_GET;
// 	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_TRUE;
// 
//     if (GRTOS_CMD_PRC_ID  != (int) 1) {
// 		// GRTOS_CMD_CRITICAL_SECTION_GET;  /// Enter in Critial Section
// 
// 		/// Include the processor in the GRTOS
// 		ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1];
// 		PRINT_ASSERT((struct_test(ppcb) == G_TRUE),"ERROR PCB is not valid\n");
// 
// 		// Set the ppcb->PCB_IDLETCB running in the current processor
// 		// Assign the IDLE task to current processor and assign it as running.
// 		gk_TCBRUNL_Link(ppcb->PCB_IDLETCB);
//     }
//     
//     G_Running = G_TRUE;  /* First processor is CPU 1 because G_Runninh flag is set to TRUE here */
// 
// 	GRTOS_ENABLE_GeMRTOS_IRQ;            /// Enable Processor Interrupt from gemrtos controller (IRQ 0)
// 	// alt_ic_irq_enable ((alt_u32) GRTOS_DRIVER_GRTOS_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID, (alt_u32) GRTOS_DRIVER_GRTOS_S_PROCESSOR1_IRQ);
// 
//     gk_KERNEL_TASK_START();            /// Start the first next task
// 
//     while(1);
// }



void gk_IRQ_HANDLER (void)
{
    // ********************************************* //
    // DO NOT DEFINE LOCAL VARIABLES.                //
    // THE FUNCTION CHANGES THE STACK POINTER        //
    // AND CALL START_TASK TO BALANCE FRAME POINTER  //
    // ********************************************* //
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_TRUE;
    GRTOS_DISABLE_PROC_INTERRUPTS;  /// Disable processor interrupt    
    gk_KERNEL_TASK_START ();    

}


/**gk_KERNEL_TASK_START
 *  \brief Start the execution of the next tasks
 */
// void gk_KERNEL_TASK_START (void) __attribute__ ((noreturn));  
void gk_KERNEL_TASK_START (void)
{
    GK_SAMPLE_FUNCTION_BEGIN(10045);
	
	unsigned int event_code;
    GS_ECB *pevent;
    
	// Processor handles just one event and continue to avoid starving other processors from mutex
	event_code = GRTOS_CMD_EVN_OCC;
	if (event_code != 0)
	{
        switch (event_code) {
            case EVN_CODE_TIMED:
                /// Time Event happened
                pevent = g_kcb.KCB_NextECBTL; 
                
                PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR no valid timed event\n");
                PRINT_ASSERT((gkm_TCB_IsValid(pevent->ECB_AssocTCB) == G_TRUE),"ERROR no valid TASK associated with event\n");
                PRINT_ASSERT((g_kcb.KCB_NextECBTL != (struct gs_ecb *) 0),"ERROR no next event in timed event list\n");

                /* Unlink the EVENT from Time Waiting Event List     */        
                gk_ECBTL_Unlink((GS_ECB *)pevent);     
                // Call the TIME_CALLBACK funtion to resolve according to the event type
                gk_TIME_CALLBACK((GS_ECB *) pevent);
                break;
                
            case EVN_CODE_FROZEN:
                // The Frozen event is acknowledge to process the following events
                GRTOS_CMD_FRZ_EVN_CLR;
                // Calls the gk_FROZEN_CALLBACK() routine in grtosfunctions.c file where 
                // user may inplement frozen mode strategy
                gk_FROZEN_CALLBACK();
                break;
            default: 
                event_code--;  /// An External IRQ happened: IRQ = event_code, IRQ index = event_code - 1
                // GRTOS_CMD_IRQ_ENB_CLR(event_code); 
                // gk_TRG_Disable((int) event_code);

                PRINT_ASSERT((gkm_RCB_IsValid(g_kcb.KCB_NextRCBTRGL[event_code]) == G_TRUE),"ERROR RCB is not valid\n");
                PRINT_ASSERT((g_kcb.KCB_NextRCBTRGL[event_code]->RCB_NextRCBWEL != (struct gs_ecb *) 0),"ERROR Interrupt happened but without an associated ISR\n");

                PRINT_ASSERT((g_kcb.KCB_NextRCBTRGL[event_code]->RCB_NextRCBGEL == (struct gs_ecb *) 0),"ERROR Interrupt happened but with running ISR\n");

                gk_TRG_Release ((G_INT32) event_code);
                break;
		}
	}    

    // #####################################################
	/* Check if Executing task is running or if it was desalocated   */
    gk_LCB_CheckInvertion();
	GS_TCB *ptcbfrom = gk_PCB_GetCurrentTCB();     
	GS_TCB *ptcbto   = gk_PCB_GetNextTCB(); 

    PRINT_ASSERT((gkm_TCB_IsValid(ptcbfrom) == G_TRUE),"ERROR current executing task is not a valid task\n");
    PRINT_ASSERT((gkm_TCB_IsValid(ptcbto) == G_TRUE),"ERROR next executing task is not a valid task\n");
    PRINT_ASSERT((ptcbto->TCBState == G_TCBState_READY || ptcbto->TCBState == G_TCBState_RUNNING),"ERROR next executing task is neither resdy nor running\n");

	if (ptcbfrom != ptcbto) {
		if (get_pointed_field(ptcbfrom, TCBState) == G_TCBState_RUNNING){
            PRINT_ASSERT((ptcbfrom->TCB_AssocPCB == (int) GRTOS_CMD_PRC_ID ),"ERROR associated task=%d, proc=%d\n",ptcbfrom->TCB_AssocPCB, (int) GRTOS_CMD_PRC_ID);
			gk_TCB_Unlink(ptcbfrom); 
			gk_TCBRDYL_Link(ptcbfrom); 
		}
		gk_TCBRDYL_Unlink(ptcbto); 
		gk_TCBRUNL_Link(ptcbto); 
	}

	gk_SetLowestProcessor();  
	gk_SetNextTimeProcessor(); 
    
	//Load the processor status and return to execute the task
    PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == GRTOS_CMD_PRC_ID),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) GRTOS_CMD_PRC_ID, __FUNCTION__, __LINE__);
	
	
#ifdef __nios2_arch__
    int dest;
    NIOS2_READ_STATUS(dest);	
    PRINT_ASSERT((dest == 0),"ERROR interrupt=%d,not disabled in critical section section= %d, proc=%d, in %s, %d\n", (int) dest, (int) GRTOS_MTX_PRC_GRANTED, (int) GRTOS_CMD_PRC_ID, __FUNCTION__, __LINE__);
#endif


	/* Updaet critical section for task after interrupt return */
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_FALSE;
	
	/* Set the Mutex nested register with the current task */
	IOWR_GRTOS_MTX_NESTED(get_pointed_field(ptcbto, TCB_MTX_NESTED));
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_FALSE;
	
    GK_SAMPLE_FUNCTION_END(10045)
	
	GRTOS_Start_Task();

#ifdef __niosX_arch__
	/* Never should be executed this */
	printf("Error in GRTOS_STart. Processor %d", GRTOS_CMD_PRC_ID ); 
	G_DEBUG_WHILEFOREVER; 
    while(1);
#endif
}

/***********************************************************************************
*********************** GRTOS TASK FINISH FUNCTION  *******************************
***********************************************************************************/
/**gk_ENTRY_TASK_COMPLETE
 *  \brief This function is executed when a task finishes its execution
 */
void  gk_ENTRY_TASK_COMPLETE(void)
{
    // ********************************************* //
    // DO NOT DEFINE LOCAL VARIABLES.                //
    // THE FUNCTION CHANGES THE STACK POINTER        //
    // AND CALL gk_KERNEL_TASK_COMPLETE              //
    // TO BALANCE FRAME POINTER                      //
    // ********************************************* //

    // 04-10-2023 GRTOS_USER_CRITICAL_SECTION_GET;  /// Enter in Critical Section
	GRTOS_CMD_CRITICAL_SECTION_GET;
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_TRUE;
	
    GRTOS_DISABLE_PROC_INTERRUPTS; /// Disable processor interrupt
	GRTOS_Change_SP_to_IDLE();     /// this function calls gk_KERNEL_TASK_COMPLETE in IDLE task stack

#ifdef __niosX_arch__
	while(1);
#endif

}


void  gk_KERNEL_TASK_COMPLETE(void)
{   
	GK_SAMPLE_FUNCTION_BEGIN(10046)

	GS_TCB *ptcb = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB; 

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR Completed task is not valid %p \n",(void *) ptcb);
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_RUNNING),"ERROR Completed task (%p) is not running, TCBState= %d\n",(void *) ptcb, (int) ptcb->TCBState);
    PRINT_ASSERT((ptcb->TCB_AssocPCB == GRTOS_CMD_PRC_ID), "ERROR task not assigned to processor assign to=%d, proc=%d\n",(int) ptcb->TCB_AssocPCB, (int) GRTOS_CMD_PRC_ID);

#ifdef __niosX_arch__
    void *StackPointer=(void *)0;
    GRTOS_READ_SP(StackPointer);
    PRINT_ASSERT(((void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackBottom >= StackPointer && (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackTop <= StackPointer),"ERROR Initiliasing the current stack, ptcb->TCB_StackBottom= %p, ptcb->TCB_StackTop= %p, StackPointer= %p \n", (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackBottom, (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackTop, (void *) StackPointer);
#endif
    
#endif

	// gk_TCB_Unlink(ptcb); 
    gk_TCBRUNL_Unlink(ptcb);

	gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);  /// Restore TCB to waiting list

	/// If task is an ISR, then enable it for next request 
	if (get_pointed_field(ptcb, TCBType) == G_TCBType_ISR) {
        gk_TRG_Wait (ptcb); 
    }
	
	GK_SAMPLE_FUNCTION_END(10046)
	
    gk_KERNEL_TASK_START ();
}


/***********************************************************************************
*********************** GRTOS SUSPEND TASK FUNCTION  *******************************
***********************************************************************************/
/**gk_KERNEL_TASK_SUSPEND
 *  \brief Suspends the execution of the task pointed by ptcb
 *  \param [in] ptcb POinter to the TCB of the task to suspend
 */ 
void gk_KERNEL_TASK_SUSPEND(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(1004);
    
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcb);
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_READY) || (ptcb->TCBState == G_TCBState_RUNNING),"ERROR Task to suspend in state (%d) is not valid\n",(int) ptcb->TCBState);
    
    if (ptcb == gk_PCB_GetCurrentTCB()) {
        gk_KERNEL_TASK_SUSPEND_CURRENT();  /* Suspend the current task */
		// 04-09-2023 GRTOS_USER_CRITICAL_SECTION_GET;   // return to critical zone
    } else {
        /* Unlink the task from ready or running */
        gk_TCB_Unlink(ptcb); 
        /* Link the task to the waiting list */
        gk_TCBWL_Link(ptcb, G_TCBState_WAITING); 
    }

    GK_SAMPLE_FUNCTION_END(1004);
}


/**
 * 
 */
/**gk_KERNEL_TASK_SUSPEND_CURRENT
 *  \brief Suspends the execution of the current task and switch to Highest Priority Task
 */

void gk_KERNEL_TASK_SUSPEND_CURRENT(void)
{
    GRTOS_DISABLE_PROC_INTERRUPTS;  /// Disable processor interrupt
	/* Check if Executing task is running or if it was desalocated   */
	GS_TCB *ptcbfrom = gk_PCB_GetCurrentTCB(); 

    GK_SAMPLE_FUNCTION_BEGIN(10028);

    PRINT_ASSERT((gkm_TCB_IsValid(ptcbfrom) == G_TRUE),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcbfrom);
    PRINT_ASSERT((ptcbfrom->TCBState == G_TCBState_RUNNING),"ERROR Current task is not runnnig, Task=%p, TCBState= %d, %s\n", (void *) ptcbfrom, (int) ptcbfrom->TCBState, ptcbfrom->TCB_description);
    PRINT_ASSERT((g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB == ptcbfrom),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcbfrom);
    PRINT_ASSERT((g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_AssocPCB == GRTOS_CMD_PRC_ID),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcbfrom);    

    // Assembler code reads this variable to storage the sp register
	G_TCB_CURRENT = (volatile GS_STK) &(ptcbfrom->TCB_StackPointer);
	
    
    gk_TCBRUNL_Unlink(ptcbfrom); 
	gk_TCBWL_Link(ptcbfrom, G_TCBState_WAITING); 

    GK_SAMPLE_FUNCTION_END(10028);

	/* Suspend current task and execute next ready */
	
	// goto_start_task();
	GRTOS_Suspend_Task(); 

    // This code is out of the CRITICAL SECTION
    // With NIOS interrupt enabled
}



/***********************************************************************************
*********************** GRTOS INITIALISATION FUNCTIONS  ****************************
***********************************************************************************/

/**
 * 
 */

/**gk_INIT_KERNEL
 *  \brief 
 *  Routine executed BEFORE the main() function, ONLY by PROCESSOR 1
 *  \details Processors with ID different from 1 start in different reset vector.
 *  The gk_START_KERNEL() function is executed AFTER the main() function
 *  Initialises all the structures to start the GRTOS
 *  \todo Modify to allow executing with normal drivers instead of reduced ones
 *  \todo use alt_dcache_flush_all(); after mutex to flush cache
 *  \sa gk_START_KERNEL
 */
// #ifndef PRUEBA
void gk_INIT_KERNEL(void) __attribute__ ((constructor));
// #endif
void gk_INIT_KERNEL(void)
{
	unsigned int i = 0;
    G_Running  = G_FALSE;
	

    GRTOS_DISABLE_DEVICES_IRQs;             /// Disable the interrupts
    GRTOS_DISABLE_PROC_INTERRUPTS;          /// Disable processor interrupt	
	
	GRTOS_CMD_RST_GRTOS; // Reset the rest of processor assigning 0 to R_PRC_RST register
	
    GRTOS_CMD_LOW_PRC_SET((int)1); // set lowest priority processor equal to 1
    							   // mainly for checking consistency at the beginning

#ifndef __niosX_arch__
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	g_kcb.fake_grtos_cmd_prc_id = 1;

	g_kcb.MainThreadHandle = (HANDLE) 0;
	g_kcb.G_PCBTbl[0].ProcessorThreadHandle = (HANDLE) 0;

	if (!DuplicateHandle(
	    GetCurrentProcess(),
	    GetCurrentThread(),
	    GetCurrentProcess(),
	    &g_kcb.MainThreadHandle,
	    0,
	    FALSE,
	    DUPLICATE_SAME_ACCESS)) {
		printf("ERROR duplicating handle 1");
	}

	if (!DuplicateHandle(
	    GetCurrentProcess(),
	    GetCurrentThread(),
	    GetCurrentProcess(),
	    &g_kcb.G_PCBTbl[0].ProcessorThreadHandle,
	    0,
	    FALSE,
	    DUPLICATE_SAME_ACCESS)) {
		printf("ERROR duplicating handle 2");
	}

	printf("%lu %lu %lu\n", GetThreadId(g_kcb.MainThreadHandle), GetThreadId(g_kcb.G_PCBTbl[0].ProcessorThreadHandle), GetCurrentThreadId());

	PRINT_ASSERT((GetThreadId(g_kcb.G_PCBTbl[0].ProcessorThreadHandle) == GetThreadId(g_kcb.MainThreadHandle)),"ERROR pthread ID of processor 1 does not equal to main_ID");

	InitializeConditionVariable(&g_kcb.GeMRTOS_RESERVE_MUTEX_condition);
	InitializeCriticalSection(&g_kcb.GeMRTOS_MUTEX_section);
	InitializeCriticalSection(&g_kcb.kcb_critical_pthread_mutex);

	// FUNCTIONS FOR CRITICAL AND WAITING SECTIONS !!!!!!!!!!!!!!!!!!!!
	// EnterCriticalSection (&g_kcb.GeMRTOS_MUTEX_section);
	// SleepConditionVariableCS (&g_kcb.GeMRTOS_RESERVE_MUTEX_condition, &g_kcb.GeMRTOS_MUTEX_section, INFINITE);
	// WakeConditionVariable (&g_kcb.GeMRTOS_MUTEX_section);
	// LeaveCriticalSection (&g_kcb.GeMRTOS_MUTEX_section);


	// 15-01-2024 pthread_mutex_init((pthread_mutex_t *) &g_kcb.kcb_critical_pthread_mutex, NULL);
	// 15-01-2024 pthread_mutex_init((pthread_mutex_t *) &g_kcb.R_PRC_RST_mutex, NULL);

    // lock the R_PRC_RST_mutex to make the CPU threads to wait it
	// 15-01-2024 pthread_mutex_lock((pthread_mutex_t *) &g_kcb.R_PRC_RST_mutex);
    // create the rest of CPU threads (the main is for processor ID = 1
    for (unsigned int i=1; i < GRTOS_DRIVER_NPROCESSORS; i++) {
    	printf("Creating processor %u\n",i);
		g_kcb.G_PCBTbl[i].ProcessorThreadHandle = CreateThread(
            NULL,                                // default security attributes
            0,                                   // use default stack size
			(void *) GeMRTOS_Processor_thread,   // thread function name
			(void *) i+1,                        // argument to thread function
			CREATE_SUSPENDED,                    // use default creation flags
            NULL);                               // returns the thread identifier
	};

   // 15-01-2024 pthread_create((pthread_t *) &g_kcb.GeM_controlertid, NULL, &gu_GeMRTOS_controller, NULL);
    g_kcb.GeM_controlertid = CreateThread(
        NULL,                                // default security attributes
        0,                                   // use default stack size
		(void *) gu_GeMRTOS_controller,      // thread function name
		(void *) i+1,                        // argument to thread function
		CREATE_SUSPENDED,                    // use default creation flags
        NULL);                               // returns the thread identifier

    printf("Processors and GeMRTOS controller thread created\n");
    R_PRC_RST = 1;

#else
	char filename[100];  /// String to hold the de name
	for (i=0; i<G_NUMBER_OF_PCB ; i++) {
        sprintf(filename, "/dev/%s_jtag_uart_%d", GRTOS_DRIVER_SYSTEM_NAME, i+2);
        //Open file for reading and writing for each processor starting in 2 (0 for stdio, 1 for stderr)
        fpuart[i] = fopen (filename, "r+");   
    }
	
    /// Read the pending and the enabled External Interrupts of the Processor
    // NIOS2_READ_IPENDING(G_IRQ_PENDING);
    // NIOS2_READ_IENABLE(G_IRQ_ENABLED);

    /// Uninstall the interrupt device drivers of the HAL
    for (i=0; i<32 ; i++) {
        alt_ic_irq_disable(0,i); 
    }
	
#endif

	PRINT_ASSERT((sizeof(intptr_t ) == sizeof(G_INT32)),"Code should be compile for 32bit system");
    PRINT_ASSERT((GRTOS_CMD_PRC_ID  == (int) 1),"ERROR Processor is not 1, CPUID= %d\n",(int) GRTOS_CMD_PRC_ID);

	/* Set First_Run to each processor to true */
	for (i=0; i<G_NUMBER_OF_PCB ; i++) {
		g_kcb.G_PCBTbl[i].PCB_Critical_PCB = G_TRUE;
	}
	
    GRTOS_CMD_CRITICAL_SECTION_GET;
	// It is set beforehand, but it is left here because it is running in kernel mode (no task)
	// Make it easier to find when start a kernel critical section
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_TRUE;

    G_DEBUG_SAMPLE_BEGIN_ENABLE = 0;
    G_DEBUG_SAMPLE_END_ENABLE = 0;

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] INITIALIZE THE KCB STRUCTURE\n");
    #endif

    GRTOS_CMD_SET_TIME_PRESCALE((int) GRTOS_DRIVER_PRESCALE);

	/// INITIALIZE THE KCB STRUCTURE
    g_kcb.KCB_NextECBTL    = (struct gs_ecb *) 0; 
    g_kcb.KCB_NextTCBWL    = (struct gs_tcb *) 0;     /* No task waiting  */
    g_kcb.KCB_NextLCBL     = (struct gs_lcb *) 0; 
    g_kcb.KCB_NextKCBASL   = (struct gs_scb *) 0; 
    
    g_kcb.KCB_NUMBER_OF_TCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_PCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_ECBs  = 0; 
    g_kcb.KCB_NUMBER_OF_RCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_LCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_SCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_RRDSs = 0;
	
	g_kcb.KCB_NUMBER_OF_PCBAssocLCBs = 0;
	
	g_kcb.function_max_interval = 0;
    
    g_kcb.KCB_FREE_TCBs   = (struct gs_tcb  *) 0;
    g_kcb.KCB_FREE_ECBs   = (struct gs_ecb  *) 0;
    g_kcb.KCB_FREE_RCBs   = (struct g_rcb   *) 0;
    g_kcb.KCB_FREE_LCBs   = (struct gs_lcb  *) 0;
    g_kcb.KCB_FREE_SCBs   = (struct gs_scb  *) 0;
    g_kcb.KCB_FREE_RRDSs  = (struct gs_rrds *) 0;
    g_kcb.KCB_FREE_RDYs   = (struct gs_pcb_rdy_lcbl *) 0;

    g_kcb.KCB_ROOT_TCBs   = (struct gs_tcb  *) 0;
    g_kcb.KCB_ROOT_ECBs   = (struct gs_ecb  *) 0;
    g_kcb.KCB_ROOT_RCBs   = (struct g_rcb   *) 0;
    g_kcb.KCB_ROOT_LCBs   = (struct gs_lcb  *) 0;
    g_kcb.KCB_ROOT_SCBs   = (struct gs_scb  *) 0;
    g_kcb.KCB_ROOT_RRDSs  = (struct gs_rrds *) 0;
    g_kcb.KCB_ROOT_RDYs   = (struct gs_pcb_rdy_lcbl *) 0;

    /*************************************************************************************/
	/*  Assign default values                                                            */
	/*************************************************************************************/

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	fprintf(stderr,"[ MESSAGE ] ASSIGNING DEFAULT VALUES\n");    
#endif

    G_TASK_TYPE_DEFAULT         = G_TCBType_UCOS;
    G_TASK_LCB_DEFAULT_PRIORITY = (G_INT32) G_DEFAULT_LCB_PRIORITY;
    G_TASK_LCB_DEFAULT          = gk_Get_LCB(GS_LCBType_FP);

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	fprintf(stderr,"[ MESSAGE ] ASSIGNING DEFAULT PERIODS AND PRIORITIES\n");    
#endif

    G_TASK_PRIORITY_DEFAULT     = (G_INT64) G_LOWEST_PRIORITY;
    G_TASK_PERIOD_DEFAULT       = (G_INT64) G_LOWEST_PRIORITY;

    /*************************************************************************************/
	/*  Initialize the structure of all processors                                       */
	/*************************************************************************************/
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	fprintf(stderr,"[ MESSAGE ] CREATING PCB STRUCTURES\n");    
#endif
    /* Initialize processors */
    gk_Init_PCBs();

    /*************************************************************************************/
	/*  Create IDLE tasks for all processor and set as EXEC for current processor        */
	/*************************************************************************************/
	gk_Create_IDLE_tasks();
	
	// 05-09-2023 // Set the ppcb->PCB_IDLETCB running in the current processor
	// 05-09-2023 // Assign the IDLE task to current processor and assign it as running.
	// 05-09-2023 // The main is executed as preempting the IDLE task of processor 1
	gk_TCBRUNL_Link(g_kcb.G_PCBTbl[0].PCB_IDLETCB);

	// Start nesting the entry to critical sections
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_FALSE;	
	// Enter section critial as task DEFAULT in processor 1
	GRTOS_USER_CRITICAL_SECTION_GET;


    /*************************************************************************************/
	/*  RTOS Variables and GRTOS Controller Initialization                               */
	/*  to be used in assembly codes                                                     */
	/*  (they are constant but store in variables for easy porting)                      */
	/*************************************************************************************/
    gs_sizeof_G_PCBTbl                    = sizeof(GS_PCB);
    gs_addressof_G_PCBTbl                 = (void *) &g_kcb.G_PCBTbl[0];
    gs_offsetof_PCB_IDLETCB               = (G_INT32) offsetof(struct gs_pcb, PCB_IDLETCB);
    gs_offsetof_PCB_EXECTCB               = (G_INT32) offsetof(struct gs_pcb, PCB_EXECTCB);
    gs_offsetof_TCB_StackPointer          = (G_INT32) offsetof(struct gs_tcb, TCB_StackPointer);

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] INITIALIZE THE TIMED EVENT LIST\n");    
    #endif
    
    /// INITIALIZE THE TIMED EVENT LIST
    g_kcb.KCB_NextECBTL = (struct gs_ecb *) gk_ECB_GetFree();

    PRINT_ASSERT((gkm_ECB_IsValid(g_kcb.KCB_NextECBTL) == G_TRUE),"ERROR not valid event obtained\n");

    /// Set the Next Occurence time equal to G_LATEST_TIME (is g_kcb.KCB_NextECBTL->ECBValue.i64)
    GRTOS_CMD_NXT_OCC_TM_EVN_SET(G_LATEST_TIME);

    /// Link an ECB with the latest time
	g_kcb.KCB_NextECBTL->ECBValue.i64 = (G_INT64) G_LATEST_TIME; 
	g_kcb.KCB_NextECBTL->ECB_AssocTCB = (struct gs_tcb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_PrevTCBAEL = (struct gs_ecb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_NextTCBAEL = (struct gs_ecb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_PrevECB = (struct gs_ecb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_NextECB = (struct gs_ecb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_NextECBASL = (struct gs_scb *) 0; 
	g_kcb.KCB_NextECBTL->ECBType = (G_INT32) G_ECBType_LASTEST_TIME; 
	g_kcb.KCB_NextECBTL->ECBState = GS_ECBState_WAITING_TIME; 



    // Initialize the External Interrupt structure
	// one trigger associated to each external interrupt
	for (int i=0 ;  i<G_NUMBER_OF_IRQ_TCB ;  i++)
	{   
        G_RCB *trg = gu_trigger_create(i);
		PRINT_ASSERT((gkm_RCB_IsValid(trg) == G_TRUE),"ERROR Trigger is not valid\n");
    }
    
    fprintf(stderr, "[ OK ] Finishing gk_INIT_KERNEL \n");

    GRTOS_CMD_FRZ_TM_THR_SET(1000000);  // Set the frozen threshold
    GRTOS_CMD_FRZ_ENB_SET;            // Enable the frozen mode

    // Print BOOTING information
    fprintf(stderr,"[ MESSAGE ] GeMRTOS\n");
	fprintf(stderr,"[ MESSAGE ] #####################################################################\n");
    fprintf(stderr,"[ MESSAGE ] Processor type  = %s\n", GRTOS_DRIVER_PROCESSOR_TYPE);	
    fprintf(stderr,"[ MESSAGE ] Processors      = %d\n", (int)GRTOS_DRIVER_NPROCESSORS);
    fprintf(stderr,"[ MESSAGE ] Clock frequency = %d MHz\n", (int)GRTOS_DRIVER_GRTOSFREQUENCY);
    fprintf(stderr,"[ MESSAGE ] Clock prescaler = %d\n", (int)GRTOS_DRIVER_PRESCALE);
    fprintf(stderr,"[ MESSAGE ] Reg. prescaler  = %d\n", (int)GRTOS_CMD_GET_TIME_PRESCALE);
    fprintf(stderr,"[ MESSAGE ] Interrupts      = %#010x\n", (int)GRTOS_DRIVER_GRTOSINTERRUPTS);  
	fprintf(stderr,"[ MESSAGE ] #####################################################################\n");	
    fprintf(stderr,"[ MESSAGE ] Generation ID   = %d\n", (int)GRTOS_DRIVER_GRTOSGENERATIONID);
    fprintf(stderr,"[ MESSAGE ] ICache size     = %s\n", GRTOS_DRIVER_INSTRUCTION_CACHE_SIZE);
    fprintf(stderr,"[ MESSAGE ] ICache Burst    = %s\n", GRTOS_DRIVER_INSTRUCTION_CACHE_BURST);
    fprintf(stderr,"[ MESSAGE ] HSP Access      = %s\n", GRTOS_DRIVER_ENABLE_HPS_MAP_ACCESS);
    fprintf(stderr,"[ MESSAGE ] Processor rate  = %d\n", (int)GRTOS_DRIVER_NIOS_CLOCK_FREQUENCY);
    fprintf(stderr,"[ MESSAGE ] Bus rate        = %d\n", (int)GRTOS_DRIVER_BUS_CLOCK_FREQUENCY);
    fprintf(stderr,"[ MESSAGE ] Address width   = %d\n", (int)GRTOS_DRIVER_BUS_WIDTH);
    fprintf(stderr,"[ MESSAGE ] Program memory  = %d %s\n", (int)GRTOS_DRIVER_EXTERNAL_MEMORY_SPAN, GRTOS_DRIVER_EXTERNAL_MEMORY_UNIT);
    fprintf(stderr,"[ MESSAGE ] GeMRTOS controller information ###########\n");
    fprintf(stderr,"[ MESSAGE ] System time register = 0x%llx\n", (unsigned long long) GRTOS_CMD_SYS_TM_GET);
    fprintf(stderr,"[ MESSAGE ] Mutex time register = 0x%llx\n", (unsigned long long) GRTOS_CMD_MTX_TM_GET);
    fprintf(stderr,"[ MESSAGE ] System time register = 0x%llx\n", (unsigned long long) GRTOS_CMD_SYS_TM_GET);
    fprintf(stderr,"[ MESSAGE ] Mutex time register = 0x%llx\n", (unsigned long long) GRTOS_CMD_MTX_TM_GET);  
    fprintf(stderr,"[ MESSAGE ] Frozen threshold register (R_FRZ_TM_THR) = 0x%llx\n", (unsigned long long) gu_get_frozen_threshold());
    fprintf(stderr,"[ MESSAGE ] GCC information ###########\n");
    fprintf(stderr,"[ MESSAGE ] sizeof(unsigned)   = %d\n", (int) sizeof(unsigned));
    fprintf(stderr,"[ MESSAGE ] sizeof(unsigned long)   = %d\n", (int) sizeof(unsigned long));
    fprintf(stderr,"[ MESSAGE ] sizeof(unsigned long long)   = %d\n", (int) sizeof(unsigned long long));
    fprintf(stderr,"[ MESSAGE ] sizeof(intptr_t )   = %d\n", (int) sizeof(intptr_t ));
    fprintf(stderr,"[ MESSAGE ] Bits in integers = %d\n", (int) sizeof(unsigned int) * CHAR_BIT);
    fprintf(stderr,"[ MESSAGE ] GRTOS_DRIVER_GRTOSFREQUENCY %d\n", (int) GRTOS_DRIVER_GRTOSFREQUENCY);
    fprintf(stderr,"[ MESSAGE ] GRTOS_DRIVER_PRESCALE %d\n", (int) GRTOS_DRIVER_PRESCALE);
    fprintf(stderr,"[ MESSAGE ] G_TICKS_PER_SECOND %d\n", (int) G_TICKS_PER_SECOND);
	
	// PRINT_ASSERT((gkm_Check_list_LCBL_IsValid(0) == G_TRUE),"ERROR in Init Kernel");
}

/***********************************************************************************
*********************** GRTOS RESET FUNCTIONS FOR ID=1 PROCESSOR       *************
***********************************************************************************/

/**gk_START_KERNEL
 *  \brief 
 *  Start up of GRTOS OS. It is executed when return from main()
 *  \details gk_Prologe is executed before main()
 *  \todo
 *  \sa gk_Prologe 
 */
// static void gk_START_KERNEL (void) __attribute__ ((noreturn)); 

// void gk_START_KERNEL (void) __attribute__ ((destructor)); 
void gk_START_KERNEL (void)
{

    GS_PCB * ppcb;
    // G_NUMBER_OF_IDLE_PROCESSORS is the number of processors wanted to be in idle state forever
    // They are sent to the gk_RST_MONITOR_HANDLER in order to left them out of the system runtime
    if (GRTOS_CMD_PRC_ID  > G_NUMBER_OF_PCB - G_NUMBER_OF_IDLE_PROCESSORS) gk_RST_MONITOR_HANDLER();

    if (GRTOS_CMD_PRC_ID  == (int) 1) {
#ifdef __niosX_arch__

		GK_SAMPLE_FUNCTION_BEGIN(10056)

		/// Install the GRTOS Controller interrupt device driver and
		/// Disable the interrupt (the last parameter (flags) is NULL). Values in system.h
		//28-12-2023 alt_ic_isr_register((alt_u32) GRTOS_DRIVER_GRTOS_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID,
		//28-12-2023                     (alt_u32) GRTOS_DRIVER_GRTOS_S_PROCESSOR1_IRQ,
		//28-12-2023                     (alt_isr_func) grtos_irq_entry,
		//28-12-2023                     (void *) NULL,
		//28-12-2023                     (void *) NULL);

#endif
#ifdef __nios2_arch__
		alt_instruction_exception_register(handler);
#endif
#ifdef __niosX_arch__
		GK_SAMPLE_FUNCTION_END(10056);
#endif
		// Create semaphore for gu_fprintf exclusion
		gu_fprintf_sem = gu_sem_create((int) 1);

		// Create semaphore for gu_printf exclusion
		gu_printf_sem = gu_sem_create((int) 1);

		// Release critical section of task DEFAULT of processor 1
		GRTOS_USER_CRITICAL_SECTION_RELEASE;
    
		G_Running = G_TRUE;  /* First processor is CPU 1 because G_Runninh flag is set to TRUE here */		
	}
	
	// ###################################
	// gk_KERNEL_PROCESSOR_START_UP();  	// Start the GRTOS for processor 1

    GRTOS_DISABLE_DEVICES_IRQs;             /// Disable the interrupts (for CPUID /= 1)
    GRTOS_DISABLE_PROC_INTERRUPTS;          /// Disable processor interrupt	

	// Get the mutex to enter in start task without nesting
	GRTOS_CMD_CRITICAL_SECTION_GET;
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB = G_TRUE;

    if (GRTOS_CMD_PRC_ID  != (int) 1) {
		// GRTOS_CMD_CRITICAL_SECTION_GET;  /// Enter in Critial Section

		/// Include the processor in the GRTOS
		ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1];
		PRINT_ASSERT((struct_test(ppcb) == G_TRUE),"ERROR PCB is not valid\n");

		// Set the ppcb->PCB_IDLETCB running in the current processor
		// Assign the IDLE task to current processor and assign it as running.
		gk_TCBRUNL_Link(ppcb->PCB_IDLETCB);
    }

	GRTOS_ENABLE_GeMRTOS_IRQ;            /// Enable Processor Interrupt from gemrtos controller (IRQ 0)
	// alt_ic_irq_enable ((alt_u32) GRTOS_DRIVER_GRTOS_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID, (alt_u32) GRTOS_DRIVER_GRTOS_S_PROCESSOR1_IRQ);

	fprintf(stderr,"[ MESSAGE ] Processor %d STARTED\n", (int) GRTOS_CMD_PRC_ID);

    gk_KERNEL_TASK_START();            /// Start the first next task

#ifdef __niosX_arch__
    while(1) {
		while(0);
	};
#else
	G_INT32 CPU_ID;
	GS_TCB *ptcb;

    while (1)
    {
		// __gcov_dump();

		CPU_ID = gk_Get_current_CPUID();
		ptcb = g_kcb.G_PCBTbl[CPU_ID-1].PCB_EXECTCB;

		pthread_mutex_lock((pthread_mutex_t *) &ptcb->pthread_mutex);
		while(ptcb->TCBState == G_TCBState_RUNNING) pthread_cond_wait((pthread_cond_t *) &ptcb->pthread_condition, &ptcb->pthread_mutex);
		pthread_mutex_unlock((pthread_mutex_t *) &ptcb->pthread_mutex);

    }


	printf("FINISING processor %d\n",CPU_ID);
	__gcov_dump();
	fflush(stdout);


#endif
	
	// ###################################

#ifdef __niosX_arch__
	printf("******************************************** \n");
	printf("ERROR Processor %d Init GRTOS \n", GRTOS_CMD_PRC_ID );
	printf("******************************************** \n");
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

void gk_CheckASM(unsigned int idata)
{


#if __riscv
	unsigned int data;
	NIOSV_READ_MSTATUS(data);
	fprintf(stderr,"INFO: MSTATUS = %x\n", data);
	NIOSV_READ_MTVEC(data);
	fprintf(stderr,"INFO: MTVEC = %x\n", data);
	NIOSV_READ_MCAUSE(data);
	fprintf(stderr,"INFO: MCAUSE = %x\n", data);
	NIOSV_READ_MIE(data);
	fprintf(stderr,"INFO: MIE = %x\n", data);
	NIOSV_READ_MIP(data);
	fprintf(stderr,"INFO: MIP = %x\n", data);
	NIOSV_READ_MEPC(data);
	fprintf(stderr,"INFO: MEPC = %x\n", data);
	GRTOS_READ_SP(data);
	fprintf(stderr,"INFO: SP = %p\n", (void *) data);
#endif
}

/**gk_TCB_GetPendingSCB
 *  \brief 
 *  Configures system to execute next pending signal. Call from switch routine
 *  \relates Task
 */
void gk_TCB_GetPendingSCB(void)
{
    GK_SAMPLE_FUNCTION_BEGIN(10024)
    
    PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == GRTOS_CMD_PRC_ID),"ERROR in critical section= %d, proc=%d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) GRTOS_CMD_PRC_ID);
    
    GRTOS_DISABLE_PROC_INTERRUPTS;
	GS_TCB *ptcb = gk_PCB_GetCurrentTCB();   /* Get Current Task          */
	GS_SCB *psignal = get_pointed_field(ptcb, TCB_NextTCBPSL);  /* Get Next Pending Signal   */

	G_SCB_PENDING = 0;               /* Default is no pending SCB */

	if (psignal != (struct gs_scb *) 0)    /* There is a pending SCB    */
	{
		if (get_pointed_field(psignal, SCBState) == G_SCBState_PENDING) { /* Signal is pending      */
			/* Signal to execution     */
			set_pointed_field(psignal, SCBState, G_SCBState_EXECUTING);
			/* Task Code                     */
		    G_SCB_CODE    = (G_INT32) get_pointed_field(psignal, SCB_TaskCode) + G_RETURN_OFFSET_FROM_INTERRUPT; 
			G_SCB_ARG     = (G_INT32) get_pointed_field(psignal, SCB_TaskArg);       /* Task Argument                 */
		    gk_TCBPSL_Unlink(ptcb, (GS_SCB *) psignal);            /* Unlink SCB from TCB       */
		    gk_SCBFL_Link((GS_SCB *) psignal);                     /* Link SCB to free list     */
			G_SCB_PENDING = 1;                       /* Set the Pending Signal status */
		}
	}
    // alt_dcache_flush_all();
    GK_SAMPLE_FUNCTION_END(10024)
}

/**********************************************************************************
 *                        	Exception handler
 *  from http://www-ug.eecg.toronto.edu/msl/manuals/n2sw_nii52006.pdf
 *********************************************************************************/
#ifdef __niosX_arch__
alt_exception_result handler(alt_exception_cause cause,
                              alt_u32 addr,
                              alt_u32 bad_addr )
{
    fprintf(stderr,"[ EXCEPTION ] cause= %d, address= 0x%x, bad_addr: 0x%x\n", (int) cause, (unsigned int) addr, (unsigned int) bad_addr);
    while(1);
    return (alt_exception_result) 0;
}
#endif
void gk_restore(void)
{
	GRTOS_DISABLE_DEVICES_IRQs;                 /// Disable the interrupts
    GRTOS_DISABLE_PROC_INTERRUPTS;          /// Disable processor interrupt	
    printf("autorestore proc= %u", GRTOS_CMD_PRC_ID);
    while(1);
}

/**********************************************************************************
 *                        	IDLE TASK DOING NOTHING
 *********************************************************************************/
void gk_WAIT_IDLE_TASK(void)
{
	GRTOS_CMD_HLT_IDL_PRC;
	/* get the address in order to avoid calculating its accessing memory each time¨*/
	volatile register int *idlestate = &g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].GRTOS_PROCESSOR_BASE[0];
	while (*idlestate == 1) { \
		while(0); \
	}
}

/**gk_CODE_IDLE_TASK
 *  \brief IDLE task. This code is executed when no task is ready. The processor is halted.
 *  
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
#ifdef __niosX_arch__
		// gk_CheckASM(15);
        #if G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE == 1
    	    void *StackPointer = (void *) 0;   // Check that SP is in the stack of the IDLE task of the processor
            GRTOS_READ_SP(StackPointer);
            // gu_fprintf("I, prc=%d\n", GRTOS_CMD_PRC_ID);
            PRINT_ASSERT(((void *) StackPointer <= (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackBottom),"ERROR SP out of bottom = 0x%p\n",(void *) GRTOS_CMD_PRC_SP);
            PRINT_ASSERT(((void *) StackPointer >= (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackTop),"ERROR SP out of top = 0x%p\n",(void *) GRTOS_CMD_PRC_SP);
        #endif
        #ifdef ALT_SEMIHOSTING
            // gu_fprintf("ALT_SEMIHOSTING defined\n");
        #endif
        #ifdef ALT_USE_DIRECT_DRIVERS
            // gu_fprintf("ALT_USE_DIRECT_DRIVERS defined\n");
        #endif        
#endif

		GRTOS_CMD_HALT_PROCESSOR; // Put the procesor in HALT mode

        if (GRTOS_MTX_PRC_GRANTED == GRTOS_CMD_PRC_ID) {
            gu_fprintf("Low proc= %d, int=%d, grn=%d, idle=%d, mtx_rsv=%d, prc_trg=%d, c_mtx_prc_grn_bit=%d, R_INT_ENB=%d\n",
            		(int) GRTOS_CMD_LOW_PRC_GET,
					(int) GRTOS_CMD_IRQ_RQS,
					(int) GRTOS_MTX_PRC_GRANTED,
					(int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_ENB),
					(int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RSV_SET),
					(int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TRG_INT_PRC),
					(int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_PRC_GRN_BIT),
					(int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET));
            while(1);
        }
    }
}



OPTIMEZE_RESTORE

