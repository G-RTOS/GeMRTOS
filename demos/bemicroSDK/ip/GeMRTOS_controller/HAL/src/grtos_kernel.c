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

#ifdef __niosX_arch__     // !!!!D
	// 19-09-2024 !!!!D const struct gemrtos_jtag_uart_info G_UART_INFO[] = GRTOS_DRIVER_JTAG_ARRAY;
#endif                    // !!!!D

const int *GRTOS_DRIVER_PROCESSOR_BASE[] = GRTOS_DRIVER_GRTOS_PROCESSOR_BASE;

const unsigned int PRC_MASK[] = {(unsigned int) 0x1, (unsigned int) 0x2, (unsigned int) 0x4, (unsigned int) 0x8, (unsigned int) 0x10, (unsigned int) 0x20, (unsigned int) 0x40, (unsigned int) 0x80, (unsigned int) 0x100, (unsigned int) 0x200, (unsigned int) 0x400, (unsigned int) 0x800, (unsigned int) 0x1000, (unsigned int) 0x2000, (unsigned int) 0x4000, (unsigned int) 0x8000, (unsigned int) 0x10000, (unsigned int) 0x20000, (unsigned int) 0x40000, (unsigned int) 0x80000, (unsigned int) 0x100000, (unsigned int) 0x200000, (unsigned int) 0x400000, (unsigned int) 0x800000, (unsigned int) 0x1000000, (unsigned int) 0x2000000, (unsigned int) 0x4000000, (unsigned int) 0x8000000,(unsigned int) 0x10000000, (unsigned int) 0x20000000, (unsigned int) 0x40000000, (unsigned int) 0x80000000};

// GLOBAL VARIABLES to hold states for assembler interface
volatile GS_STK G_TCB_CURRENT __attribute__((aligned(4)));    ///< \brief Holds the STKpointer of current task to switch
volatile GS_TCB *G_TCB_FROM __attribute__((aligned(4)));      ///< \brief Holds the TCB of current task to switch
volatile G_INT32 G_SCB_PENDING __attribute__((aligned(4)));   ///< \brief Holds the SCB for pending signal
volatile G_INT32 G_SCB_CODE __attribute__((aligned(4)));      ///< \brief Holds the Code Address for pending signal
volatile G_INT32 G_SCB_ARG __attribute__((aligned(4)));       ///< \brief Holds the Args Address for pending signal

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

// 01-07-2024 FILE* fpuart[G_NUMBER_OF_PCB] __attribute__((aligned(4)));    ///< \brief Files for JTAG-UARTs


volatile G_INT32 G_TASK_TYPE_DEFAULT __attribute__((aligned(4)));        ///< \brief Default assignment for Task Type
volatile GS_LCB *G_TASK_LCB_DEFAULT __attribute__((aligned(4)));       ///< \brief Default assignment for Task LCB
volatile G_INT32 G_TASK_LCB_DEFAULT_PRIORITY __attribute__((aligned(4)));       ///< \brief Priority of the default LCB
volatile G_INT64 G_TASK_PRIORITY_DEFAULT __attribute__((aligned(4)));    ///< \brief Default assignment for Task Priority
volatile G_INT64 G_TASK_PERIOD_DEFAULT __attribute__((aligned(4)));      ///< \brief Default assignment for Task Period

#ifdef __niosX_arch__
struct _reent impure_data;
#endif

// 03-07-2024 struct _reent *impure_data_ptr;

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
	g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB = G_TRUE;
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
    
	// gm_GeMRTOSCriticalSectionEnter;  	
	GRTOS_CMD_CRITICAL_SECTION_GET;    /// Get into critical section
	g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB = G_TRUE;

	/* Set the Mutex nested register with the current task */
	IOWR_GRTOS_MTX_NESTED(g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_MTX_NESTED);
	g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB = G_FALSE;
	
    // 16-10-2024 GRTOS_DISABLE_PROC_INTERRUPTS;  /// Disable processor interrupt

    // NIOS2_WRITE_SP((G_INT32) g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackPointer);    
    
    // PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == gm_ProcessorId),"ERROR SIGNAL in critical section= %d, proc=%d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) gm_ProcessorId);
    // gk_KERNEL_TASK_START ();    
    
}


void gk_IRQ_HANDLER (void)
{
    // ********************************************* //
    // DO NOT DEFINE LOCAL VARIABLES.                //
    // THE FUNCTION CHANGES THE STACK POINTER        //
    // AND CALL START_TASK TO BALANCE FRAME POINTER  //
    // ********************************************* //
	g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB = G_TRUE;
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
	// 08-07-2024 fprintf(stderr,"%s, gm_IrqStatusGet=%x, event=%x", __FUNCTION__, (unsigned int) gm_IrqStatusGet, (unsigned int) event_code);
	if (event_code != 0)
	{
        switch (event_code) {
            case EVN_CODE_TIMED:
                // Time Event happened
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
				
				GS_SCB *psignal = gk_KCBASL_GetSCB(G_SCBType_FROZEN_MODE);
				if (psignal != (GS_SCB *) 0) {
					psignal = gk_SCB_Copy((GS_SCB *) psignal); 
					gk_KCBPSL_Link((GS_SCB *) psignal); 
				}				
				
                break;
            default: 
                event_code--;  /// An External IRQ happened: IRQ = event_code, IRQ index = event_code - 1
                // gm_IrqEnable(event_code); 
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
            PRINT_ASSERT((ptcbfrom->TCB_AssocPCB == (int) gm_ProcessorId ),"ERROR associated task=%d, proc=%d\n",ptcbfrom->TCB_AssocPCB, (int) gm_ProcessorId);
			gk_TCB_Unlink(ptcbfrom); 
			gk_TCBRDYL_Link(ptcbfrom); 
		}
		gk_TCBRDYL_Unlink(ptcbto); 
		gk_TCBRUNL_Link(ptcbto); 
	}

	gk_SetLowestProcessor();  
	gk_SetNextTimeProcessor(); 
    
	//Load the processor status and return to execute the task
    PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == gm_ProcessorId),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) gm_ProcessorId, __FUNCTION__, __LINE__);
	
	
#ifdef __nios2_arch__
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    int dest;
    NIOS2_READ_STATUS(dest);	
    PRINT_ASSERT((dest == 0),"ERROR interrupt=%d,not disabled in critical section section= %d, proc=%d, in %s, %d\n", (int) dest, (int) GRTOS_MTX_PRC_GRANTED, (int) gm_ProcessorId, __FUNCTION__, __LINE__);
#endif
#endif


	/* Set the Mutex nested register with the current task */
	IOWR_GRTOS_MTX_NESTED(get_pointed_field(ptcbto, TCB_MTX_NESTED));
	
	/* Update critical section for task after interrupt return */
	g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB = G_FALSE;
	
    GK_SAMPLE_FUNCTION_END(10045)
	
	GRTOS_Start_Task();

#ifdef __niosX_arch__
	/* Never should be executed this */
	printf("Error in GRTOS_STart. Processor %d", gm_ProcessorId ); 
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

	GRTOS_CMD_CRITICAL_SECTION_GET;   /// Enter in Critical Section
	g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB = G_TRUE;
	
    GRTOS_DISABLE_PROC_INTERRUPTS; /// Disable processor interrupt
	GRTOS_Change_SP_to_IDLE();     /// this function calls gk_KERNEL_TASK_COMPLETE in IDLE task stack

#ifdef __niosX_arch__
	while(1);
#endif

}


void  gk_KERNEL_TASK_COMPLETE(void)
{   
	GK_SAMPLE_FUNCTION_BEGIN(10046)

	GS_TCB *ptcb = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB; 

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR Completed task is not valid %p \n",(void *) ptcb);
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_RUNNING),"ERROR Completed task (%p) is not running, TCBState= %d\n",(void *) ptcb, (int) ptcb->TCBState);
    PRINT_ASSERT((ptcb->TCB_AssocPCB == gm_ProcessorId), "ERROR task not assigned to processor assign to=%d, proc=%d\n",(int) ptcb->TCB_AssocPCB, (int) gm_ProcessorId);

#ifdef __niosX_arch__
    void *StackPointer=(void *)0;
    GRTOS_READ_SP(StackPointer);
    PRINT_ASSERT(((void *) g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackBottom >= StackPointer && (void *) g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackTop <= StackPointer),"ERROR Initiliasing the current stack, ptcb->TCB_StackBottom= %p, ptcb->TCB_StackTop= %p, StackPointer= %p \n", (void *) g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackBottom, (void *) g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackTop, (void *) StackPointer);
#endif
    
#endif

	// gk_TCB_Unlink(ptcb); 
    gk_TCBRUNL_Unlink(ptcb);

	gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);  /// Restore TCB to waiting list

	// If task is an ISR, then enable it for next request 
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
        gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);  /* Suspend the current task */
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

void gk_KERNEL_TASK_SUSPEND_CURRENT(enum tcbstate TCBSTATE)
{
    GRTOS_DISABLE_PROC_INTERRUPTS;  /// Disable processor interrupt
	/* Check if Executing task is running or if it was desalocated   */
	GS_TCB *ptcbfrom = gk_PCB_GetCurrentTCB(); 

    GK_SAMPLE_FUNCTION_BEGIN(10028);

    PRINT_ASSERT((gkm_TCB_IsValid(ptcbfrom) == G_TRUE),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcbfrom);
    PRINT_ASSERT((ptcbfrom->TCBState == G_TCBState_RUNNING),"ERROR Current task is not runnnig, Task=%p, TCBState= %d, %s\n", (void *) ptcbfrom, (int) ptcbfrom->TCBState, ptcbfrom->TCB_description);
    PRINT_ASSERT((g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB == ptcbfrom),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcbfrom);
    PRINT_ASSERT((g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_AssocPCB == gm_ProcessorId),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcbfrom);    

    // Assembler code reads this variable to storage the sp register
	G_TCB_CURRENT = (volatile GS_STK) &(ptcbfrom->TCB_StackPointer);

#ifndef __niosX_arch__
	G_TCB_FROM    = ptcbfrom;
#endif
    
    gk_TCBRUNL_Unlink(ptcbfrom);
	
	switch (TCBSTATE) {
		case G_TCBState_WAITING:
			gk_TCBWL_Link(ptcbfrom, G_TCBState_WAITING);
			break;
			
		case G_TCBState_READY:
			gk_TCBRDYL_Link(ptcbfrom);
			break;
			
		default:
			G_DEBUG_WHILEFOREVER;
	}


    GK_SAMPLE_FUNCTION_END(10028);

	/* Suspend current task and execute next ready */
	
	// goto_start_task();
	GRTOS_Suspend_Task(); 

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
// void gk_INIT_KERNEL(void) __attribute__ ((constructor));
void gk_INIT_KERNEL(void)
{
	unsigned int i = 0;
    G_Running  = G_FALSE;

	// 12-07-2024 while(1) {
	// 12-07-2024 	printf("processor %u\n", (unsigned int) gm_ProcessorId);
	// 12-07-2024 }
	/**********************************************************************************/
	/* Check it is reduced_device_drivers enabled */
#ifdef __niosX_arch__
	const char message_reduced_device_driver[] = "ERROR: check and set hal.enable_reduced_device_drivers in BSP settings";
	for (unsigned int n_irq=0 ;  n_irq < ALT_NIRQ ; n_irq++)
	{
		if (alt_irq[n_irq].handler != (void *) 0) {
			for (unsigned int character = 0; character < strlen(message_reduced_device_driver); character++) {
				IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDOUT_BASE , message_reduced_device_driver[character]);
				IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDERR_BASE , message_reduced_device_driver[character]);
			}
			IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDOUT_BASE , '\n');
			IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDERR_BASE , '\n');
			while(1);
		} 
	}
#endif
    /**********************************************************************************/


    GRTOS_DISABLE_DEVICES_IRQs;             /// Disable the interrupts
    GRTOS_DISABLE_PROC_INTERRUPTS;          /// Disable processor interrupt
	
	// Copy start-up to rest of processor to GEMRTOS controller
#ifdef __niosX_arch__
	for (unsigned int j = 0; j < (unsigned int) ((GRTOS_jump_to_reset_end - GRTOS_jump_to_reset) / 4); j++) {
		IOWR(GRTOS_DRIVER_GRTOS_BASE, j, ((unsigned int *) GRTOS_jump_to_reset)[j]);
	}
#endif
	
	GRTOS_CMD_UNRST_GRTOS(0);      /// Unreset the rest of processor assigning 0 to R_PRC_RST register
	
    GRTOS_CMD_LOW_PRC_SET((int)1); // set lowest priority processor equal to 1
    							   // mainly for checking consistency at the beginning

#ifndef __niosX_arch__
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	// g_kcb.fake_grtos_cmd_prc_id = 1;
	Reset_GeMRTOS_controller_registers();

	// 10-07-2024 InitializeCriticalSection(&g_kcb.GeMRTOS_MUTEX_section);

	// 10-07-2024 InitializeConditionVariable(&g_kcb.GeMRTOS_RESERVE_MUTEX_condition);

	// 10-07-2024 InitializeCriticalSection(&g_kcb.kcb_critical_pthread_mutex);

	// FUNCTIONS FOR CRITICAL AND WAITING SECTIONS !!!!!!!!!!!!!!!!!!!!
	// EnterCriticalSection (&g_kcb.GeMRTOS_MUTEX_section);
	// SleepConditionVariableCS (&g_kcb.GeMRTOS_RESERVE_MUTEX_condition, &g_kcb.GeMRTOS_MUTEX_section, INFINITE);
	// WakeConditionVariable (&g_kcb.GeMRTOS_MUTEX_section);
	// LeaveCriticalSection (&g_kcb.GeMRTOS_MUTEX_section);
	for (i=0; i<G_NUMBER_OF_PCB ; i++) g_kcb.G_PCBTbl[i].ThreadId = 0;

	g_kcb.G_PCBTbl[0].ThreadId = GetCurrentThreadId();

	// 15-01-2024 pthread_mutex_init((pthread_mutex_t *) &g_kcb.kcb_critical_pthread_mutex, NULL);
	// 15-01-2024 pthread_mutex_init((pthread_mutex_t *) &g_kcb.R_PRC_RST_mutex, NULL);
    for (unsigned int j=0; j< G_NUMBER_OF_PCB; j++) {

    	getcontext(&g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE);
    	g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE.uc_stack.ss_size = 16 * 1024;
    	g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE_stack = malloc(g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE.uc_stack.ss_size);
    	g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE.uc_stack.ss_sp = g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE_stack;
    	g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE.uc_stack.ss_flags = 0;
    	makecontext(&g_kcb.G_PCBTbl[j].uctx_gk_ENTRY_TASK_COMPLETE, gk_ENTRY_TASK_COMPLETE, 0);


		getcontext(&g_kcb.G_PCBTbl[j].uctx_main);
		g_kcb.G_PCBTbl[j].uctx_main.uc_stack.ss_size = 16 * 1024;
		g_kcb.G_PCBTbl[j].uctx_main_stack = malloc(g_kcb.G_PCBTbl[j].uctx_main.uc_stack.ss_size);
		g_kcb.G_PCBTbl[j].uctx_main.uc_stack.ss_sp = g_kcb.G_PCBTbl[j].uctx_main_stack;
		g_kcb.G_PCBTbl[j].uctx_main.uc_stack.ss_flags = 0;
		makecontext(&g_kcb.G_PCBTbl[j].uctx_main, gk_ENTRY_TASK_COMPLETE, 0);
    }

    for (unsigned int j=2; j <= G_NUMBER_OF_PCB; j++) {
    	// pthread_create((pthread_t *) &g_kcb.G_PCBTbl[j-1].ThreadId, NULL, gu_processor, (void *) j);
    	CreateThread(
                NULL,                   // default security attributes
                0,                      // use default stack size
				gu_processor,           // thread function name
                (void *) j,             // argument to thread function
				0,                      // use default creation flags
				&g_kcb.G_PCBTbl[j-1].ThreadId);   // returns the thread identifier
    };

// CREATE_SUSPENDED
   //  R_PRC_RST = 1;



#else
	// 01-07-2024 char filename[100];  /// String to hold the de name
	// 01-07-2024 for (i=0; i<G_NUMBER_OF_PCB ; i++) {
    // 01-07-2024     sprintf(filename, "/dev/%s_jtag_uart_%d", GRTOS_DRIVER_SYSTEM_NAME, i+2);
    // 01-07-2024     //Open file for reading and writing for each processor starting in 2 (0 for stdio, 1 for stderr)
    // 01-07-2024     fpuart[i] = fopen (filename, "r+");   
    // 01-07-2024 }
	
    // Read the pending and the enabled External Interrupts of the Processor
    // NIOS2_READ_IPENDING(G_IRQ_PENDING);
    // NIOS2_READ_IENABLE(G_IRQ_ENABLED);


    // Uninstall the interrupt device drivers of the HAL
    for (i=0; i<32 ; i++) {
        alt_ic_irq_disable(0,i); 
    }
	
#endif

	PRINT_ASSERT((sizeof(intptr_t ) == sizeof(G_INT32)),"Code should be compile for 32bit system");
    PRINT_ASSERT((gm_ProcessorId  == (int) 1),"ERROR Processor is not 1, CPUID= %d\n",(int) gm_ProcessorId);

	/* Set First_Run to each processor to true */
	for (i=0; i<G_NUMBER_OF_PCB ; i++) {
		g_kcb.G_PCBTbl[i].PCB_Critical_PCB = G_TRUE;
	}
	
    GRTOS_CMD_CRITICAL_SECTION_GET;
	// It is set beforehand, but it is left here because it is running in kernel mode (no task)
	// Make it easier to find when start a kernel critical section
	g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB = G_TRUE;

    G_DEBUG_SAMPLE_BEGIN_ENABLE = 0;
    G_DEBUG_SAMPLE_END_ENABLE = 0;

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    GU_FPRINTF(stderr,"[ MESSAGE ] INITIALIZE THE KCB STRUCTURE\n");
#endif

    gm_SystemTimePrescaleSet((int) GRTOS_DRIVER_PRESCALE);

	// INITIALIZE THE KCB STRUCTURE
    g_kcb.KCB_NextECBTL    = (struct gs_ecb *) 0; 
    g_kcb.KCB_NextTCBWL    = (struct gs_tcb *) 0;     /* No task waiting  */
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

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    g_kcb.KCB_NUMBER_OF_TCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_PCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_ECBs  = 0; 
    g_kcb.KCB_NUMBER_OF_RCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_LCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_SCBs  = 0; 
    g_kcb.KCB_NUMBER_OF_RRDSs = 0;
	g_kcb.KCB_NUMBER_OF_PCBAssocLCBs = 0;	

    g_kcb.KCB_ROOT_TCBs   = (struct gs_tcb  *) 0;
    g_kcb.KCB_ROOT_ECBs   = (struct gs_ecb  *) 0;
    g_kcb.KCB_ROOT_RCBs   = (struct g_rcb   *) 0;
    g_kcb.KCB_ROOT_LCBs   = (struct gs_lcb  *) 0;
    g_kcb.KCB_ROOT_SCBs   = (struct gs_scb  *) 0;
    g_kcb.KCB_ROOT_RRDSs  = (struct gs_rrds *) 0;
    g_kcb.KCB_ROOT_RDYs   = (struct gs_pcb_rdy_lcbl *) 0;
#endif

    /*************************************************************************************/
	/*  Assign default values                                                            */
	/*************************************************************************************/

    G_TASK_TYPE_DEFAULT         = G_TCBType_OneShot;
    G_TASK_LCB_DEFAULT_PRIORITY = (G_INT32) G_DEFAULT_LCB_PRIORITY;
    G_TASK_LCB_DEFAULT          = gk_Get_LCB(GS_LCBTypeFP);

    G_TASK_PRIORITY_DEFAULT     = (G_INT64) G_LOWEST_PRIORITY;
    G_TASK_PERIOD_DEFAULT       = (G_INT64) G_LOWEST_PRIORITY;

    /*************************************************************************************/
	/*  Initialize the structure of all processors                                       */
	/*************************************************************************************/
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	GU_FPRINTF(stderr,"[ MESSAGE ] CREATING PCB STRUCTURES\n");    
#endif
    /* Initialize processors */
    gk_Init_PCBs();

    /*************************************************************************************/
	/*  Create IDLE tasks for all processor and set as EXEC for current processor        */
	/*************************************************************************************/
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	GU_FPRINTF(stderr,"[ MESSAGE ] CREATING IDLE TASKS\n");    
#endif
	gk_Create_IDLE_tasks();
	
	// Assign the IDLE task to current processor and assign it as running.
	// The main is executed as preempting the IDLE task of processor 1
	gk_TCBRUNL_Link(g_kcb.G_PCBTbl[0].PCB_IDLETCB);

	// Start nesting the entry to critical sections
	g_kcb.G_PCBTbl[0].PCB_Critical_PCB = G_FALSE;	
	// Enter section critial as task DEFAULT in processor 1
	gm_GeMRTOSCriticalSectionEnter;

    /*************************************************************************************/
	/*  RTOS Variables and GRTOS Controller Initialization                               */
	/*  to be used in assembly codes                                                     */
	/*  (they are constant but store in variables for easy porting)                      */
	/*************************************************************************************/
    gs_sizeof_G_PCBTbl               = sizeof(GS_PCB);
    gs_addressof_G_PCBTbl            = (void *) &g_kcb.G_PCBTbl[0];
    gs_offsetof_PCB_IDLETCB          = (G_INT32) offsetof(struct gs_pcb, PCB_IDLETCB);
    gs_offsetof_PCB_EXECTCB          = (G_INT32) offsetof(struct gs_pcb, PCB_EXECTCB);
    gs_offsetof_TCB_StackPointer     = (G_INT32) offsetof(struct gs_tcb, TCB_StackPointer);

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    GU_FPRINTF(stderr,"[ MESSAGE ] INITIALIZE THE TIMED EVENT LIST\n");    
#endif
    
    // INITIALIZE THE TIMED EVENT LIST
    g_kcb.KCB_NextECBTL = (struct gs_ecb *) gk_ECB_GetFree();

    PRINT_ASSERT((gkm_ECB_IsValid(g_kcb.KCB_NextECBTL) == G_TRUE),"ERROR not valid event obtained\n");

    // Set the Next Occurence time equal to G_LATEST_TIME (is g_kcb.KCB_NextECBTL->ECBValue.i64)
    GRTOS_CMD_NXT_OCC_TM_EVN_SET(G_LATEST_TIME);

    // Link an ECB with the latest time
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
        gu_TriggerCreate(i);
    }
    
	GU_FPRINTF(stderr, "[ OK ] Finishing gk_INIT_KERNEL \n");

    gm_FrozenModeThresholdSet(1000000);  // Set the frozen threshold
    gm_FrozenModeEnable;              // Enable the frozen mode

    // Print BOOTING information
    GU_FPRINTF(stderr,"[ MESSAGE ] GeMRTOS\n");
	GU_FPRINTF(stderr,"[ MESSAGE ] #####################################################################\n");
    GU_FPRINTF(stderr,"[ MESSAGE ] Processor type  = %s\n", GRTOS_DRIVER_PROCESSOR_TYPE);	
    GU_FPRINTF(stderr,"[ MESSAGE ] Processors      = %d\n", (int)GRTOS_DRIVER_NPROCESSORS);
    GU_FPRINTF(stderr,"[ MESSAGE ] Clock frequency = %d MHz\n", (int)GRTOS_DRIVER_GRTOSFREQUENCY);
    GU_FPRINTF(stderr,"[ MESSAGE ] Clock prescaler = %d\n", (int)GRTOS_DRIVER_PRESCALE);
    GU_FPRINTF(stderr,"[ MESSAGE ] Reg. prescaler  = %d\n", (int)gm_SystemTimePrescaleGet);
    GU_FPRINTF(stderr,"[ MESSAGE ] Interrupts      = %#010x\n", (int)GRTOS_DRIVER_GRTOSINTERRUPTS);  
	GU_FPRINTF(stderr,"[ MESSAGE ] #####################################################################\n");	
    GU_FPRINTF(stderr,"[ MESSAGE ] Generation ID   = %d\n", (int)GRTOS_DRIVER_GRTOSGENERATIONID);
    GU_FPRINTF(stderr,"[ MESSAGE ] ICache size     = %s\n", GRTOS_DRIVER_INSTRUCTION_CACHE_SIZE);
    GU_FPRINTF(stderr,"[ MESSAGE ] ICache Burst    = %s\n", GRTOS_DRIVER_INSTRUCTION_CACHE_BURST);
    GU_FPRINTF(stderr,"[ MESSAGE ] HSP Access      = %s\n", GRTOS_DRIVER_ENABLE_HPS_MAP_ACCESS);
    GU_FPRINTF(stderr,"[ MESSAGE ] Processor rate  = %d\n", (int)GRTOS_DRIVER_NIOS_CLOCK_FREQUENCY);
    GU_FPRINTF(stderr,"[ MESSAGE ] Bus rate        = %d\n", (int)GRTOS_DRIVER_BUS_CLOCK_FREQUENCY);
    GU_FPRINTF(stderr,"[ MESSAGE ] Address width   = %d\n", (int)GRTOS_DRIVER_BUS_WIDTH);
    GU_FPRINTF(stderr,"[ MESSAGE ] Program memory  = %d %s\n", (int)GRTOS_DRIVER_EXTERNAL_MEMORY_SPAN, GRTOS_DRIVER_EXTERNAL_MEMORY_UNIT);
    GU_FPRINTF(stderr,"[ MESSAGE ] GeMRTOS controller information ###########\n");
    GU_FPRINTF(stderr,"[ MESSAGE ] System time register = 0x%llx\n", (unsigned long long) gm_SystemTotalTimeGet);
    GU_FPRINTF(stderr,"[ MESSAGE ] Mutex time register = 0x%llx\n", (unsigned long long) gm_GeMRTOSCriticalSectionGrantedTime);
    GU_FPRINTF(stderr,"[ MESSAGE ] System time register = 0x%llx\n", (unsigned long long) gm_SystemTotalTimeGet);
    GU_FPRINTF(stderr,"[ MESSAGE ] Mutex time register = 0x%llx\n", (unsigned long long) gm_GeMRTOSCriticalSectionGrantedTime);  
    GU_FPRINTF(stderr,"[ MESSAGE ] Frozen threshold register (R_FRZ_TM_THR) = 0x%llx\n", (unsigned long long) gm_FrozenModeThresholdGet);
    GU_FPRINTF(stderr,"[ MESSAGE ] GCC information ###########\n");
    GU_FPRINTF(stderr,"[ MESSAGE ] sizeof(unsigned)   = %d\n", (int) sizeof(unsigned));
    GU_FPRINTF(stderr,"[ MESSAGE ] sizeof(unsigned long)   = %d\n", (int) sizeof(unsigned long));
    GU_FPRINTF(stderr,"[ MESSAGE ] sizeof(unsigned long long)   = %d\n", (int) sizeof(unsigned long long));
    GU_FPRINTF(stderr,"[ MESSAGE ] sizeof(intptr_t )   = %d\n", (int) sizeof(intptr_t ));
    // 15-07-2024 GU_FPRINTF(stderr,"[ MESSAGE ] Bits in integers = %d\n", (int) sizeof(unsigned int) * CHAR_BIT);
    GU_FPRINTF(stderr,"[ MESSAGE ] GRTOS_DRIVER_GRTOSFREQUENCY %d\n", (int) GRTOS_DRIVER_GRTOSFREQUENCY);
    GU_FPRINTF(stderr,"[ MESSAGE ] GRTOS_DRIVER_PRESCALE %d\n", (int) GRTOS_DRIVER_PRESCALE);
    GU_FPRINTF(stderr,"[ MESSAGE ] G_TICKS_PER_SECOND %d\n", (int) G_TICKS_PER_SECOND);

#ifdef __niosX_arch__  // !!!!D
	// 19-09-2024 !!!!D GU_FPRINTF(stderr,"[ MESSAGE ] _REENT %p\n", (void *) _REENT);
	// 19-09-2024 !!!!D // 15-07-2024 GU_FPRINTF(stderr,"[ MESSAGE ] &_impure_ptr %p\n", (void *) &_impure_ptr);
	// 19-09-2024 !!!!D // 15-07-2024 GU_FPRINTF(stderr,"[ MESSAGE ] _impure_ptr %p\n", (void *) _impure_ptr);
	// 19-09-2024 !!!!D // 15-07-2024 GU_FPRINTF(stderr,"[ MESSAGE ] size of _impure_ptr %u\n", (unsigned int) sizeof(struct _reent));
	// 19-09-2024 !!!!D GU_FPRINTF(stderr,"[ MESSAGE ] size of G_UART_INFO %u\n", (unsigned int) sizeof(G_UART_INFO));
	// 19-09-2024 !!!!D GU_FPRINTF(stderr,"[ MESSAGE ] size of struct gemrtos_jtag_uart_info %u\n", (unsigned int) sizeof(struct gemrtos_jtag_uart_info));
	// 19-09-2024 !!!!D GU_FPRINTF(stderr,"[ MESSAGE ] number of struct gemrtos_jtag_uart_info %u\n", (unsigned int) (sizeof(G_UART_INFO) / sizeof(struct gemrtos_jtag_uart_info)));
	// 19-09-2024 !!!!D for (int i=0; i < (unsigned int) (sizeof(G_UART_INFO) / sizeof(struct gemrtos_jtag_uart_info)) ;i++) {
	// 19-09-2024 !!!!D 	if ((void *) ALT_STDOUT_BASE == G_UART_INFO[i].JTAG_UART_BASE) {
	// 19-09-2024 !!!!D 		GU_FPRINTF(stderr,"[ MESSAGE ] STDOUT_BASE = %u\n", (unsigned int) G_UART_INFO[i].JTAG_UART_BASE);
	// 19-09-2024 !!!!D 		GU_FPRINTF(stderr,"[ MESSAGE ] STDOUT_IRQ = %u\n", (unsigned int) G_UART_INFO[i].JTAG_UART_IRQ);
	// 19-09-2024 !!!!D 		GU_FPRINTF(stderr,"[ MESSAGE ] STDOUT_JTAG_UART_IRQ_INTERRUPT_CONTROLLER_ID = %u\n", (unsigned int) G_UART_INFO[i].JTAG_UART_IRQ_INTERRUPT_CONTROLLER_ID);
	// 19-09-2024 !!!!D 		GU_FPRINTF(stderr,"[ MESSAGE ] STDOUT_JTAG_UART_NAME = %s\n", (char *) G_UART_INFO[i].JTAG_UART_NAME);
	// 19-09-2024 !!!!D 		GU_FPRINTF(stderr,"[ MESSAGE ] STDOUT_JTAG_UART_READ_DEPTH = %u\n", (unsigned int) G_UART_INFO[i].JTAG_UART_READ_DEPTH);
	// 19-09-2024 !!!!D 	}
	// 19-09-2024 !!!!D }
#endif      // !!!!D

	// 02-07-2024 !!!!D  /* List all device names registered in the HAL */ // 26-06-2024
	// 02-07-2024 !!!!D  alt_llist *llist = &alt_dev_list;
	// 02-07-2024 !!!!D  alt_dev* next = (alt_dev*) llist->next;
	// 02-07-2024 !!!!D  while (next != (alt_dev*) llist)
	// 02-07-2024 !!!!D  {	
	// 02-07-2024 !!!!D  	GU_FPRINTF(stderr,"[ MESSAGE ] list %s\n", (char *) next->name);
	// 02-07-2024 !!!!D  	next = (alt_dev*) next->llist.next;
	// 02-07-2024 !!!!D  }
	// 02-07-2024 !!!!D  
	// 02-07-2024 !!!!D  /* Show a list of the state of the interrupts */ // 26-06-2024
	// 02-07-2024 !!!!D  for (unsigned int proc_irq = 0; proc_irq <= 31; proc_irq ++) {
	// 02-07-2024 !!!!D  	GU_FPRINTF(stderr,"[ MESSAGE ] interrupt %u, state=%d, pending=%u\n", (unsigned int) proc_irq, alt_ic_irq_enabled (0, proc_irq), (unsigned int) alt_irq_pending());
	// 02-07-2024 !!!!D  }
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

	PRINT_ASSERT((gm_ProcessorId  <= G_NUMBER_OF_PCB),"ERROR: wrong gm_ProcessorId = %u", (unsigned int) gm_ProcessorId);

    if (gm_ProcessorId  == (int) 1) {
		
		// IOWR_GRTOS_PROC_REENT(&_impure_data);     // !!!!D
#ifdef __niosX_arch__
		GK_SAMPLE_FUNCTION_BEGIN(10056)
#endif
#ifdef __nios2_arch__
		alt_instruction_exception_register(handler);
#endif
#ifdef ALT_INCLUDE_INSTRUCTION_RELATED_EXCEPTION_API
		alt_instruction_exception_register(handler);
#endif
#ifdef __niosX_arch__
		GK_SAMPLE_FUNCTION_END(10056);
#endif
		// Create semaphore for gu_fprintf exclusion
		gu_fprintf_sem = gu_SemaphoreCreate((int) 1);

		// Create semaphore for gu_printf exclusion
		gu_printf_sem = gu_SemaphoreCreate((int) 1);

		// Release critical section of task DEFAULT of processor 1
		gm_GeMRTOSCriticalSectionExit;
    
		G_Running = G_TRUE;  /* First processor is CPU 1 because G_Runninh flag is set to TRUE here */		
	}
	
	// ###################################

    GRTOS_DISABLE_DEVICES_IRQs;             /// Disable the interrupts (for CPUID /= 1)
    GRTOS_DISABLE_PROC_INTERRUPTS;          /// Disable processor interrupt

#ifndef __niosX_arch__                                                                                        // !!!!D
    if (gm_ProcessorId != 1) {                                                                              // !!!!D
    	fprintf(stderr, "proc %u in %s, line %d\n", (unsigned int) gm_ProcessorId, __FUNCTION__, __LINE__); // !!!!D
	 	// while(1) {SwitchToThread();};                                                                      // !!!!D
    }                                                                                                         // !!!!D
#endif                                                                                                        // !!!!D

	// Get the mutex to enter in start task without nesting
	GRTOS_CMD_CRITICAL_SECTION_GET;
	// Set running as kernal mode
	g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB = G_TRUE;

    if (gm_ProcessorId  != (int) 1) {

		// Include the processor in the GRTOS
		ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[gm_ProcessorId -1];
		PRINT_ASSERT((struct_test(ppcb) == G_TRUE),"ERROR PCB is not valid\n");

		// Set the ppcb->PCB_IDLETCB running in the current processor
		// Assign the IDLE task to current processor and assign it as running.
		gk_TCBRUNL_Link(ppcb->PCB_IDLETCB);
    }

	GRTOS_ENABLE_GeMRTOS_IRQ;            /// Enable Processor Interrupt from gemrtos controller (IRQ 0)

	GU_FPRINTF(stderr,"[ MESSAGE ] Processor %d STARTED\n", (int) gm_ProcessorId);

    gk_KERNEL_TASK_START();            /// Start the first next task

	// ###################################

#ifdef __niosX_arch__                                                                // !!!!D
	printf("******************************************** \n");                       
	printf("ERROR Processor %d Init GRTOS \n", gm_ProcessorId );                   
	printf("******************************************** \n");                       
    /****************************************************/                           
    /****************************************************/                           
    /* Code to produce assembler in objdump file        */                           
    /* IT IS NEVER EXECUTED                             */                           
    /****************************************************/                           
    /****************************************************/                           
	while(1);                                                                        
#else                                                                                // !!!!D
	__gcov_dump();                                                                   // !!!!D
	fflush(stdout);                                                                  // !!!!D
#endif                                                                               // !!!!D

}

/**gk_TCB_GetPendingSCB
 *  \brief Configures system to execute next pending signal. Call from switch routine
 *  \relates Task
 */
void gk_TCB_GetPendingSCB(void)
{
	GS_TCB *ptcb;
	GS_SCB *psignal;
	
    GK_SAMPLE_FUNCTION_BEGIN(10024)
    
    PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == gm_ProcessorId),"ERROR in critical section= %d, proc=%d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) gm_ProcessorId);
    
    // 15-10-2024 GRTOS_DISABLE_PROC_INTERRUPTS;
	
	G_SCB_PENDING = 0;               /* Default is no pending SCB */
	ptcb = gk_PCB_GetCurrentTCB();   /* Get Current Task          */	
	
	/* Check KCB pending signals */
	psignal = g_kcb.KCB_NextKCBPSL;
	PRINT_ASSERT(((psignal == (GS_SCB *)  0) || (gkm_SCB_IsValid(psignal) == G_TRUE)),"ERROR SCB is not valid\n");
	
	/* There is a pending SCB and task is not an IDLE one   */
	if ((psignal != (struct gs_scb *) 0) && (ptcb->TCBType != G_TCBType_IDLE))    
	{
		if (get_pointed_field(psignal, SCBState) == G_SCBState_PENDING) { /* Signal is pending      */
			/* Signal to execution     */
			set_pointed_field(psignal, SCBState, G_SCBState_EXECUTING);
		    G_SCB_CODE    = (G_INT32) get_pointed_field(psignal, SCB_TaskCode) + G_RETURN_OFFSET_FROM_INTERRUPT; 
			G_SCB_ARG     = (G_INT32) get_pointed_field(psignal, SCB_TaskArg);       /* Task Argument                 */
		    gk_KCBPSL_Unlink((GS_SCB *) psignal);            /* Unlink SCB from TCB       */
			
			PRINT_ASSERT((g_kcb.KCB_NextKCBPSL == (GS_SCB *)  0), "ERROR another signal delete after testing");
			
		    gk_SCBFL_Link((GS_SCB *) psignal);               /* Link SCB to free list     */
			G_SCB_PENDING = 1;                               /* Set the Pending Signal status */			
		}
	}		
	
	/* Check TCB pending signals */
	psignal = get_pointed_field(ptcb, TCB_NextTCBPSL);  /* Get Next Pending Signal   */

	if ((psignal != (struct gs_scb *) 0) && (G_SCB_PENDING == 0))    /* There is a pending SCB    */
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
    GU_FPRINTF(stderr,"[ EXCEPTION ] cause= %d, address= 0x%x, bad_addr: 0x%x\n", (int) cause, (unsigned int) addr, (unsigned int) bad_addr);
    while(1);
    return (alt_exception_result) 0;
}
#endif
void gk_restore(void)
{
	GRTOS_DISABLE_DEVICES_IRQs;                 /// Disable the interrupts
    // GRTOS_DISABLE_PROC_INTERRUPTS;          /// Disable processor interrupted
	
    printf("autorestore proc= %u, RST_REG= %u\n", (unsigned int) gm_ProcessorId, (unsigned int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR));
    while(1);
}

/**********************************************************************************
 *                        	IDLE TASK DOING NOTHING
 *********************************************************************************/
void gk_WAIT_IDLE_TASK(void)
{
	GRTOS_CMD_HLT_IDL_PRC;
	/* get the address in order to avoid calculating its accessing memory each time¨*/
	// 02-07-2024 volatile register int *idlestate = &g_kcb.G_PCBTbl[gm_ProcessorId -1].GRTOS_PROCESSOR_BASE[ADDR_REG_0];
	// 02-07-2024 while (*idlestate == 1) {
	// 02-07-2024 	while(0);
	// 02-07-2024 }
	
	while (IORD(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, ADDR_REG_0) == 1) {
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
#ifdef __niosX_arch__
        #if (G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE == 1)
    	    void *StackPointer = (void *) 0;   // Check that SP is in the stack of the IDLE task of the processor
            GRTOS_READ_SP(StackPointer);
            // gu_fprintf("I, prc=%d\n", gm_ProcessorId);
            PRINT_ASSERT(((void *) StackPointer <= (void *) g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackBottom),"ERROR SP out of bottom = 0x%p\n",(void *) GRTOS_CMD_PRC_SP);
            PRINT_ASSERT(((void *) StackPointer >= (void *) g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackTop),"ERROR SP out of top = 0x%p\n",(void *) GRTOS_CMD_PRC_SP);
        #endif
#endif
        #ifdef ALT_SEMIHOSTING
            gu_fprintf("ALT_SEMIHOSTING defined\n");
        #endif
        #ifdef ALT_USE_DIRECT_DRIVERS
            gu_fprintf("ALT_USE_DIRECT_DRIVERS defined\n");
        #endif        

		// Put the procesor in HALT mode
		gm_ProcessorHalt;

		PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED != gm_ProcessorId), "ERROR in IDLE task MTX= %u, PROC=%u\n", (unsigned int) GRTOS_MTX_PRC_GRANTED, (unsigned int) gm_ProcessorId);
    }
}


OPTIMEZE_RESTORE

