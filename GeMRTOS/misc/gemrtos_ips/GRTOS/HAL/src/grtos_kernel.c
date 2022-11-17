/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Kernal functions
 *  \details This file contains the Kernal functions.
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

#include <gemrtos.h>
// #include <grtos.h>

#include "sys/alt_exceptions.h"
#include "nios2.h"
#include "alt_types.h"
alt_exception_result handler(alt_exception_cause cause, alt_u32 addr, alt_u32 bad_addr );

#include "priv/alt_irq_table.h"


OPTIMEZE_CODE(3)


/*****************************************************************************/
/*****************************************************************************/
/**         SYSTEM VARIABLES                                                 */
/*****************************************************************************/
/*****************************************************************************/

/* Data Structure */
volatile GS_KCB  g_kcb __attribute__((aligned(4)));           ///< \brief Structure of GS_KCB


/// GLOBAL VARIABLES to hold states for assembler interface
volatile GS_STK G_TCB_CURRENT __attribute__((aligned(4)));    ///< \brief Holds the TCB of current task to switch
volatile INT32 G_SCB_PENDING __attribute__((aligned(4)));     ///< \brief Holds the SCB for pending signal
volatile INT32 G_SCB_CODE __attribute__((aligned(4)));        ///< \brief Holds the Code Address for pending signal
volatile INT32 G_SCB_ARG __attribute__((aligned(4)));         ///< \brief Holds the Args Address for pending signal

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
volatile INT32 GRTOS_MutexReleaseRegisterAddress __attribute__((aligned(4)));
volatile INT32 GRTOS_MutexRequestRegisterAddress __attribute__((aligned(4)));
volatile INT32 GRTOS_InterruptEnableRegisterAddress __attribute__((aligned(4)));
volatile INT32 GRTOS_InterruptDisableRegisterAddress __attribute__((aligned(4)));

volatile INT32  G_Running __attribute__((aligned(4)));  ///< \brief Flag indicating Scheduler is enabled: G_TRUE or G_FALSE
volatile INT32  G_Running = G_FALSE; 

volatile INT32  gs_sizeof_G_PCBTbl __attribute__((aligned(4)));           ///< \brief Values for assembler code
volatile void   *gs_addressof_G_PCBTbl __attribute__((aligned(4)));           ///< \brief Values for assembler code
volatile INT32  gs_offsetof_PCB_EXECTCB __attribute__((aligned(4)));      ///< \brief Values for assembler code
volatile INT32  gs_offsetof_TCB_StackPointer __attribute__((aligned(4))); ///< \brief Values for assembler code
volatile INT32  gs_offsetof_PCB_IDLETCB __attribute__((aligned(4)));      ///< \brief Values for assembler code

volatile int G_DEBUG_SAMPLE_BEGIN_ENABLE __attribute__((aligned(4)));     ///< \brief Values to enable sampling function starts (0xffffffff for all functions)
volatile int G_DEBUG_SAMPLE_END_ENABLE __attribute__((aligned(4)));       ///< \brief Values to enable sampling function ends (0xffffffff for all functions)


FILE* fpuart[G_NUMBER_OF_PCB] __attribute__((aligned(4)));    ///< \brief Files for JTAG-UARTs

volatile INT32 G_TASK_TYPE_DEFAULT __attribute__((aligned(4)));        ///< \brief Default assignment for Task Type
volatile GS_LCB *G_TASK_LCB_DEFAULT __attribute__((aligned(4)));       ///< \brief Default assignment for Task LCB
volatile GS_LCB *G_TASK_LCB_DEFAULT_PRIORITY __attribute__((aligned(4)));       ///< \brief Priority of the default LCB
volatile INT64 G_TASK_PRIORITY_DEFAULT __attribute__((aligned(4)));    ///< \brief Default assignment for Task Priority
volatile INT64 G_TASK_PERIOD_DEFAULT __attribute__((aligned(4)));      ///< \brief Default assignment for Task Period


/***********************************************************************************
***********************   *************
***********************************************************************************/

/**gk_ENTRY_RST_HANDLER
 *  \brief 
 *  GRTOS RESET FUNCTIONS FOR NOT ID=1 PROCESSORS
 *  \details This is executed when processor with ID != 1 are reset.
 *  A different stack is assigned to each processor in the 
 *  grtos_start_rest_of_processors function in the rstaux section
 */
// void gk_ENTRY_RST_HANDLER (void) __attribute__ ((section ("rstaux"))); 
void gk_ENTRY_RST_HANDLER (void)
{
    /// Called from grtos_start_rest_of_processors
    
    /// Disable processor interrupt
    NIOS2_WRITE_STATUS(0); 
    /// Disable the interrupts
    NIOS2_WRITE_IENABLE (0);
    
    SAMPLE_FUNCTION_BEGIN(1000);
    
    GRTOS_USER_CRITICAL_SECTION_GET;        /// Enter in Critial Section  


    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif    

    // G_NUMBER_OF_IDLE_PROCESSORS is the number of processors wanted to be in idle state forever
    // They are sent to the gk_RST_MONITOR_HANDLER in order to left them out of the system runtime
    if (GRTOS_CMD_PRC_ID  > G_NUMBER_OF_PCB - G_NUMBER_OF_IDLE_PROCESSORS) gk_RST_MONITOR_HANDLER(); 


    /// Include the processor in the GRTOS
    g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCBState = GS_PCBState_RUNNING;  
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCBID = GRTOS_CMD_PRC_ID ; 
    g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB;
    gk_LCBFPL_Link(GRTOS_CMD_PRC_ID);
    
    SAMPLE_FUNCTION_END(1000);
	gk_KERNEL_TASK_START();              // Start the GRTOS for this processor
    
	printf("******************************************** \n"); 
	printf("ERROR Processor %d Init GRTOS \n", GRTOS_CMD_PRC_ID ); 
	printf("******************************************** \n");     
    
    while(1);      
    // ###########################################
    
}


/***********************************************************************************
*********************** GRTOS INTERRUPT FUNCTIONS  ********************************
***********************************************************************************/
/**gk_ENTRY_IRQ_HANDLER
 *  \brief Executes the ISR of GRTOS
 *  \details This routine is executed each time processor is interrupted
 */
// void gk_ENTRY_IRQ_HANDLER (void) __attribute__ ((section (".exceptions"))); 
void gk_ENTRY_IRQ_HANDLER (void)
{
	unsigned int event_code;
    GS_ECB *pevent;
    
    // Only when GeMRTOS is running
    PRINT_ASSERT((G_Running == G_TRUE),"ERROR G_Running= %d\n",G_Running);
    // Only interrupt from GRTOS supported
    PRINT_ASSERT(((alt_irq_pending() == 1) || (alt_irq_pending() == 0)),"ERROR alt_irq_pending()= %d\n",(int) alt_irq_pending());
    // Check that SP is in the stack of the IDLE task of the processor    
	PRINT_ASSERT(((GRTOS_CMD_PRC_SP <= (INT32) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackBottom) && ((INT32) GRTOS_CMD_PRC_SP >= (int) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackTop - 300)),"ERROR SP = 0x%x\n",(int) GRTOS_CMD_PRC_SP);

    
	GRTOS_CMD_PRC_INT_DSB;             /// Disable processor interrupt
	GRTOS_CMD_INT_PRC_PND_CLR;         /// Remove pending interrupt of current processor
    
    SAMPLE_FUNCTION_BEGIN(1001);
 
    // fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "In %s, %d \n", __FUNCTION__, __LINE__);                    
	GRTOS_CMD_CRITICAL_SECTION_GET;   /// Get into critical section 

	event_code = GRTOS_CMD_EVN_OCC;  
	while(event_code)
	{
        switch (event_code) {
            case EVN_CODE_TIMED:
                /// Time Event happened
                pevent = g_kcb.KCB_NextECBTL; 
                
                PRINT_ASSERT((ECB_IsValid(pevent) == G_TRUE),"ERROR no valid timed event\n");
                PRINT_ASSERT((TCB_IsValid(pevent->ECB_AssocTCB) == G_TRUE),"ERROR no valid TASK associated with event\n");
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
            
                PRINT_ASSERT((g_kcb.KCB_ExtISR[event_code].G_TCB_ISR != (struct gs_tcb *) 0),"ERROR Interrupt happened but without an associated ISR\n");
                PRINT_ASSERT((GRTOS_TASK_STATE_WAITING(g_kcb.KCB_ExtISR[event_code].G_TCB_ISR)),"ERROR Interrupt ISR is not waiting (re-entry not allowed because interrupt is enabled when ISRs are completed)\n");

                gk_ISR_RELEASE (event_code);
                break;

		}
        // Processor handler just one event and continue to avoid starving other processors from mutex
        event_code = 0; 
	}

    // #####################################################
	/* Check if Executing task is running or if it was desalocated   */
	GS_TCB *ptcbfrom = gk_PCB_GetCurrentTCB(); 
	GS_TCB *ptcbto   = gk_PCB_GetNextTCB(); 

    PRINT_ASSERT((TCB_IsValid(ptcbfrom) == G_TRUE),"ERROR current executing task is not a valid task\n");
    PRINT_ASSERT((TCB_IsValid(ptcbto) == G_TRUE),"ERROR next executing task is not a valid task\n");
    PRINT_ASSERT((ptcbto->TCBState == G_TCBState_READY || ptcbto->TCBState == G_TCBState_RUNNING),"ERROR next executing task is neither resdy nor running\n");

	if (ptcbfrom != ptcbto) {
		if (ptcbfrom->TCBState == G_TCBState_RUNNING){
			gk_TCB_Unlink(ptcbfrom); 
			gk_TCBRDYL_Link(ptcbfrom); 
		}
		gk_TCBRDYL_Unlink(ptcbto); 
		gk_TCBRUNL_Link(ptcbto); 
	}

	/* Set the GRTOS register for next interrupt                     */
	gk_SetLowestProcessor(); 
	gk_LCB_CheckInvertion(); 
	gk_SetNextTimeProcessor();
    
    SAMPLE_FUNCTION_END(1001);
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
    fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "In %s, %d \n", __FUNCTION__, __LINE__);
    while(1);
}

/***********************************************************************************
****Return from Signal: get critical section and return to                    ******
**** GRTOS_Return_from_Signal in grtos_switch.S file
***********************************************************************************/
/**gk_ENTRY_SIGNAL_RETURN
 *  \brief Return from Signal:
 *  \details Get critical section and return to GRTOS_Return_from_Signal in grtos_switch.S file
 */
void gk_ENTRY_SIGNAL_RETURN(void)
{
    // fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "Returning from Signal In %s, %d \n", __FUNCTION__, __LINE__);    
	GRTOS_CMD_CRITICAL_SECTION_GET;  	/// Get into critical section
}

/**gk_KERNEL_TASK_START
 *  \brief Start the execution of the tasks of the system
 */
void  gk_KERNEL_TASK_START (void)
{
	GS_TCB *ptcbtostart = gk_PCB_GetNextTCB();  /// Get the Next TCB to run in the current processor 
    
    SAMPLE_FUNCTION_BEGIN(1002);
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    PRINT_ASSERT((TCB_IsValid(ptcbtostart) == G_TRUE),"ERROR Start TCB is not valid\n");
    PRINT_ASSERT((ptcbtostart->TCBState == G_TCBState_READY),"ERROR Start TCB is not ready\n");

	/// Change State to Running
    gk_TCB_Unlink(ptcbtostart); 
	gk_TCBRUNL_Link(ptcbtostart); 
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
	gk_SetLowestProcessor(); 
	gk_LCB_CheckInvertion(); 
	gk_SetNextTimeProcessor(); 
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
	/// Enable Processor Interrupt
	NIOS2_WRITE_IENABLE (1); 
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    GRTOS_CMD_RLS_NXT_PRC;   // Enables the next processor to start

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
	//Load the processor status and return to execute the task
    SAMPLE_FUNCTION_END(1002);
	GRTOS_Start_Task(); 

	/* Never should be executed this */
	printf("Error in GRTOS_STart. Processor %d", GRTOS_CMD_PRC_ID ); 
	G_DEBUG_WHILEFOREVER; 
}

/***********************************************************************************
*********************** GRTOS TASK FINISH FUNCTION  *******************************
***********************************************************************************/
/**gk_KERNEL_TASK_COMPLETE
 *  \brief This function is executed when a task finishes its execution
 */
void  gk_KERNEL_TASK_COMPLETE(void)
{
	SAMPLE_FUNCTION_BEGIN(1003);
    
    GRTOS_USER_CRITICAL_SECTION_GET;  /// Enter in Critial Section
    
	GS_TCB *ptcbtostart = gk_PCB_GetCurrentTCB(); 

    PRINT_ASSERT((TCB_IsValid(ptcbtostart) == G_TRUE),"ERROR Completed task is not valid %p \n",(void *) ptcbtostart);
	PRINT_ASSERT((GRTOS_CMD_PRC_SP <= (INT32) (int) ptcbtostart->TCB_StackBottom),"ERROR SP out of bottom = %d\n",(int) GRTOS_CMD_PRC_SP);
    PRINT_ASSERT((GRTOS_CMD_PRC_SP > (INT32) (int) ptcbtostart->TCB_StackTop - 300),"ERROR SP out of top = %d\n",(int) GRTOS_CMD_PRC_SP);
    PRINT_ASSERT((ptcbtostart->TCBState == G_TCBState_RUNNING),"ERROR Completed task (%p) is not running, TCBState= %d\n",(void *) (void *) ptcbtostart, ptcbtostart->TCBState);

	gk_TCB_Unlink(ptcbtostart); 

	gk_TCBWL_Link(ptcbtostart, G_TCBState_WAITING_COMPLETED);  /// Restore TCB to waiting list

	/// If task is an ISR, then enable it for next request 
	if (ptcbtostart->TCBType == G_TCBType_ISR) {
        gk_ISR_COMPLETE (ptcbtostart);  /// \todo Check if IRQ is enabled when all ISR are complete
    }

	/// Get the Next Priority to run in the current processor
	ptcbtostart = gk_PCB_GetNextTCB(); 

    PRINT_ASSERT((TCB_IsValid(ptcbtostart) == G_TRUE),"ERROR Next start task (%p) is not valid\n",(void *) ptcbtostart);
    PRINT_ASSERT((ptcbtostart->TCBState == G_TCBState_READY),"ERROR Next start task state is not ready, TCBState= %d\n",(int) ptcbtostart->TCBState);

	gk_TCBRDYL_Unlink(ptcbtostart); 
    gk_TCBRUNL_Link(ptcbtostart); 

	gk_SetLowestProcessor(); 
	gk_LCB_CheckInvertion(); 
	gk_SetNextTimeProcessor(); 

	//Load the processor status and return to execute the HighRdy task
    SAMPLE_FUNCTION_END(1003);
	GRTOS_Start_Task(); 

	/* Never should be executed this */
	printf("Error in gk_KERNEL_TASK_COMPLETE. Processor %d", GRTOS_CMD_PRC_ID ); 
	G_DEBUG_WHILEFOREVER; 

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
    SAMPLE_FUNCTION_BEGIN(1004);
    
    PRINT_ASSERT((TCB_IsValid(ptcb) == G_TRUE),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcb);
    
	/* if not running nor ready, then nothing to do */
	if (ptcb->TCBState != G_TCBState_READY && ptcb->TCBState != G_TCBState_RUNNING)
	{
		if (ptcb != gk_PCB_GetCurrentTCB()) {
			gk_KERNEL_TASK_SUSPEND_CURRENT();  /* Suspend the current task */
		} else {
			/* Check current task state */
			if (ptcb->TCBState == G_TCBState_RUNNING){
                GRTOS_CMD_PRC_INT(ptcb->TCB_AssocPCB);  /// Stop the processor triggering its interrupt
			}
			/* Unlink the task from ready or running */
			gk_TCB_Unlink(ptcb); 
			/* Link the task to the waiting list */
			gk_TCBWL_Link(ptcb, G_TCBState_WAITING); 
		}
	}
    SAMPLE_FUNCTION_END(1004);
}


/**
 * 
 */
/**gk_KERNEL_TASK_SUSPEND_CURRENT
 *  \brief Suspends the execution of the current task and switch to Highest Priority Task
 */
void gk_KERNEL_TASK_SUSPEND_CURRENT(void)
{
	/* Check if Executing task is running or if it was desalocated   */
	GS_TCB *ptcbfrom = gk_PCB_GetCurrentTCB(); 

    SAMPLE_FUNCTION_BEGIN(1005);

    PRINT_ASSERT((TCB_IsValid(ptcbfrom) == G_TRUE),"ERROR Task to suspend (%p) is not valid\n",(void *) ptcbfrom);
    PRINT_ASSERT((ptcbfrom->TCBState == G_TCBState_RUNNING),"ERROR Current task is not runnnig, TCBState= %d\n",(int) ptcbfrom->TCBState);

    // Assembler code reads this variable to storage the sp register
    G_TCB_CURRENT = (GS_STK) &g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackPointer;
    
    gk_TCBRUNL_Unlink(ptcbfrom); 
	gk_TCBWL_Link(ptcbfrom, G_TCBState_WAITING); 

    SAMPLE_FUNCTION_END(1005);

	/* Suspend current task and execute next ready */
	GRTOS_Suspend_Task(); 
    
    // This code is out of the CRITICAL SECTION
}



/***********************************************************************************
*********************** GRTOS INITIALISATION FUNCTIONS  ****************************
***********************************************************************************/

/**
 * 
 */

/**gk_INIT_KERNEL
 *  \brief 
 *  Routine execute BEFORE the main() function, ONLY by PROCESSOR 1
 *  \details Processors with ID different from 1 start in different reset vector.
 *  The gk_START_KERNEL() function is executed AFTER the main() function
 *  Initialises all the structures to start the GRTOS
 *  \todo Modify to allow executing with normal drivers instead of reduced ones
 *  \todo use alt_dcache_flush_all(); after mutex to flush cache
 *  \sa gk_START_KERNEL
 */
 
// static void gk_Prologe (void) __attribute__ ((constructor)); 
// void gk_Prologe (void)

void gk_INIT_KERNEL(void) __attribute__ ((constructor)); 
void gk_INIT_KERNEL(void)
{
	int i,j;     

    PRINT_ASSERT((GRTOS_CMD_PRC_ID  == (int) 1),"ERROR Processor is not 1, CPUID= %d\n",(int) GRTOS_CMD_PRC_ID);

    GRTOS_CMD_RST_GRTOS; // Reset the rest of processor assigning 0 to R_PRC_RST register

    // Initialize run time 
    G_Running = G_FALSE;
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

    // volatile INT32   G_ISR_STACK[ALT_NIRQ][G_ISR_STACKSIZE] __attribute__((aligned(4)));  
    void   *mem1 = malloc(sizeof(INT32) * ALT_NIRQ * G_ISR_STACKSIZE + 31);
    g_kcb.G_ISR_STACK = (INT32 *) (((uintptr_t)mem1+15) & ~ (uintptr_t)0x0F);    

    // volatile GS_STK  G_IDLE_STACK[G_NUMBER_OF_PCB][G_IDLE_STACKSIZE] __attribute__((aligned(4)));     
    void   *mem2 = malloc(sizeof(GS_STK) * G_NUMBER_OF_PCB * G_IDLE_STACKSIZE + 31);
    g_kcb.G_IDLE_STACK = (GS_STK *) (((uintptr_t)mem2+15) & ~ (uintptr_t)0x0F);


    /*************************************************************************************/
	/*  Reserve system Conrol Blocks                                                     */
	/*************************************************************************************/
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] CREATING PCB STRUCTURES\n");    
    #endif
    gk_Create_PCBs((int) G_NUMBER_OF_PCB);


    /*************************************************************************************/
	/*  Assign default values                                                            */
	/*************************************************************************************/
    G_TASK_TYPE_DEFAULT         = G_TCBType_UCOS;
    G_TASK_LCB_DEFAULT          = gk_Get_LCB();
    G_TASK_LCB_DEFAULT_PRIORITY = (INT32) 0x7FFFFFF0;
    G_TASK_PRIORITY_DEFAULT     = (INT64) 0x7FFFFFFFFFFFFFF0LL;
    G_TASK_PERIOD_DEFAULT       = (INT64) 0x7FFFFFFFFFFFFFF0LL;  

    /*************************************************************************************/
	/*  RTOS Variables and GRTOS Controller Initialization                               */
	/*************************************************************************************/
	/// Init GLOBAL VARIABLES (used in grtos_switch.S code)
	GRTOS_MutexReleaseRegisterAddress     = (INT32) (GRTOS_DRIVER_GRTOS_BASE + 4 * ADDR_MTX_RLS); 
    GRTOS_MutexRequestRegisterAddress     = (INT32) (GRTOS_DRIVER_GRTOS_BASE + 4 * ADDR_MTX_GRN); 
    GRTOS_InterruptEnableRegisterAddress  = (INT32) (GRTOS_DRIVER_GRTOS_BASE + 4 * ADDR_INT_ENB_SET); 
    GRTOS_InterruptDisableRegisterAddress = (INT32) (GRTOS_DRIVER_GRTOS_BASE + 4 * ADD_INT_ENB_CLR);
    gs_sizeof_G_PCBTbl                    = sizeof(GS_PCB);
    gs_addressof_G_PCBTbl                 = (void *) &g_kcb.G_PCBTbl[0];
    gs_offsetof_PCB_IDLETCB               = (INT32) (&((GS_PCB *) NULL)->PCB_IDLETCB); 
    gs_offsetof_PCB_EXECTCB               = (INT32) (&((GS_PCB *) NULL)->PCB_EXECTCB); 
    gs_offsetof_TCB_StackPointer          = (INT32) (&((GS_TCB *) NULL)->TCB_StackPointer);



    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] INITIALIZE THE TIMED EVENT LIST\n");    
    #endif
    /// INITIALIZE THE TIMED EVENT LIST
    g_kcb.KCB_NextECBTL = (struct gs_ecb *) gk_ECB_GetFree();

    PRINT_ASSERT((ECB_IsValid(g_kcb.KCB_NextECBTL) == G_TRUE),"ERROR not valid event obtained\n");
    
    /// Set the Next Occurence time equal to G_LATEST_TIME (is g_kcb.KCB_NextECBTL->ECBValue.i64)
    GRTOS_CMD_NXT_OCC_TM_EVN_SET(G_LATEST_TIME);   
    /// Link an ECB with the latest time
	g_kcb.KCB_NextECBTL->ECBValue.i64 = (INT64) G_LATEST_TIME; 
	g_kcb.KCB_NextECBTL->ECB_AssocTCB = (struct gs_tcb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_PrevTCBAEL = (struct gs_ecb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_NextTCBAEL = (struct gs_ecb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_PrevECB = (struct gs_ecb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_NextECB = (struct gs_ecb *) 0; 
	g_kcb.KCB_NextECBTL->ECB_NextECBASL = (struct gs_scb *) 0; 
	g_kcb.KCB_NextECBTL->ECBType = (INT32) G_ECBType_LASTEST_TIME; 
	g_kcb.KCB_NextECBTL->ECBState = GS_ECBState_WAITING_TIME; 

	/// Initialize Processor Structures creating IDLE TCBs
	for (i = 0;  i < G_NUMBER_OF_PCB;  i++) {
        
        g_kcb.G_PCBTbl[i].GRTOS_PROCESSOR_BASE = (int *) GRTOS_DRIVER_GRTOS_PROCESSOR_BASE[i]; // Address to go to idle state
        
        g_kcb.G_PCBTbl[i].PCB_IDLETCB  = (struct gs_tcb *) gk_TCB_GetFree();
        if (g_kcb.G_PCBTbl[i].PCB_IDLETCB == (struct gs_tcb *) 0) G_DEBUG_WHILEFOREVER; /// !!! Set error NO_FREE_GS_TCB
        
        g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCBType = G_TCBType_IDLE; 
        g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCB_StackBottom = (GS_STK *) ((((INT32) g_kcb.G_IDLE_STACK + (i * G_IDLE_STACKSIZE) + G_IDLE_STACKSIZE-8)-8)& ~0x3);
        
        g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCB_StackTop = (GS_STK *)((INT32) g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCB_StackBottom - ((INT32)(G_IDLE_STACKSIZE)& ~0x3));
        g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCB_TaskCode = gk_CODE_IDLE_TASK; 
        g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCB_TaskArg = (void *) NULL; 
        g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCBReadyPriority = G_IDLETASK_PRIORITY; 
        g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCBRunPriority = G_IDLETASK_PRIORITY; 
        g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCB_RDY_LCB_Index = (struct gs_lcb *) G_TASK_LCB_DEFAULT; 

        gk_TASK_STK_Init(g_kcb.G_PCBTbl[i].PCB_IDLETCB);    
        // gk_TCBRDYL_Link(g_kcb.G_PCBTbl[i].PCB_IDLETCB);  /* Insert Task in Ready List          */
        
		g_kcb.G_PCBTbl[i].PCBState = GS_PCBState_NOTRUNNING; 
        
	    g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCBState = G_TCBState_READY; 		 
        g_kcb.G_PCBTbl[i].PCB_IDLETCB->TCB_AssocPCB = i + 1;
        
		g_kcb.G_PCBTbl[i].PCB_EXECTCB = g_kcb.G_PCBTbl[i].PCB_IDLETCB; 
        
        g_kcb.G_PCBTbl[i].PCB_AssocLCB = (struct gs_pcb_rdy_lcbl *) 0;
        
        // Assign the default list to the processor with the default priority
        gk_LCB_Associate_PCB((GS_LCB *) G_TASK_LCB_DEFAULT, (INT32) i, (INT32) G_TASK_LCB_DEFAULT_PRIORITY); 
        
        for (j = 0;  j < G_NUMBER_OF_LCBs_FOR_PCB;  j++) {         
            g_kcb.G_PCBTbl[i].PCB_RDY_LCBL[j] = (struct gs_lcb *) G_TASK_LCB_DEFAULT;  
        }
		g_kcb.G_PCBTbl[i].PCBID           = (int) 0; 
		g_kcb.G_PCBTbl[i].PCBType         = (int) GS_PCBType_UNSPECIFIED; 
		g_kcb.G_PCBTbl[i].PCB_NextPCB     = (struct gs_pcb *) 0; 
		g_kcb.G_PCBTbl[i].PCB_PrevPCB     = (struct gs_pcb *) 0;         
	}    
    // Initialize the External Interrupt structure
	for (i=0 ;  i<ALT_NIRQ ;  i++)
	{    
    	g_kcb.KCB_ExtISR[i].G_TCB_ISR = (struct gs_tcb *) 0; 
        g_kcb.KCB_ExtISR[i].G_EXT_INT_Count = (int) 0;
		GRTOS_CMD_IRQ_ENB_CLR(i);
    }
    
    fprintf(stderr, "[ OK ] Finishing gk_INIT_KERNEL \n");
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
static void gk_START_KERNEL (void) __attribute__ ((destructor)); 
void gk_START_KERNEL (void)
{
    int i;    
    char filename[100];  /// String to hold the de name
    
    SAMPLE_FUNCTION_BEGIN(1006);
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    GRTOS_USER_CRITICAL_SECTION_GET;
    GRTOS_USER_CRITICAL_SECTION_SET_RELEASE_TIME(200);

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    for (i=0; i<G_NUMBER_OF_PCB ; i++) {
        // sprintf(filename, "/dev/jtag_uart_%d", i+2);
        sprintf(filename, "/dev/%s_jtag_uart_%d", GRTOS_DRIVER_SYSTEM_NAME, i+2);
        fpuart[i] = fopen (filename, "r+");  //Open file for reading and writing one for each processor starting in 2 (0 for stdio, 1 for stderr) 
    }

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    /// Disable processor interrupt
    NIOS2_WRITE_STATUS(0);
    /// Read the pending and the enabled External Interrupts of the Processor
    NIOS2_READ_IPENDING(G_IRQ_PENDING);
    NIOS2_READ_IENABLE(G_IRQ_ENABLED);

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    /// Uninstall the interrupt device drivers of the HAL
    for (i=0; i<32 ; i++) {
        alt_ic_irq_disable(0,i); 
        // fprintf(stderr, "[ OK ] Disabling interrupt device drivers\n");
    }

    /// Install the GRTOS Controller interrupt device driver and
    /// Disable the interrupt (the last parameter (flags) is NULL). Values in system.h
    // fprintf(stderr, "[ OK ] Installing GRTOS Controller interrupt\n");
    alt_ic_isr_register((alt_u32) GRTOS_DRIVER_GRTOS_S_PROCESSOR1_IRQ_INTERRUPT_CONTROLLER_ID,
                        (alt_u32) GRTOS_DRIVER_GRTOS_S_PROCESSOR1_IRQ,
                        // (alt_isr_func) gk_ENTRY_IRQ_HANDLER,
                        (alt_isr_func) grtos_irq_entry,
                        (void *) NULL,
                        (void *) NULL);
                        
    alt_instruction_exception_register(handler);
                        
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    NIOS2_WRITE_IENABLE (0); 
    NIOS2_WRITE_STATUS(0);  

	/// Initialize the IRQ Tasks
    #if G_GRTOS_PRESERVE_HAL_ISR == 1
        gk_INIT_IRQ();       /*!< Init IRQ tasks */
    #endif

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCBState = GS_PCBState_RUNNING;  // to set it to FREE
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCBID = GRTOS_CMD_PRC_ID ; PRINT_DEBUG_LINE
    g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB;
    gk_LCBFPL_Link(GRTOS_CMD_PRC_ID);

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    G_Running = G_TRUE;     /* First processor is CPU 1 because G_Runninh flag is set to TRUE here */

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif

    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    SAMPLE_FUNCTION_END(1006);
    
	gk_KERNEL_TASK_START();  	// Start the GRTOS for processor 1

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

}

/**GRTOS_Task_GetPendingSCB
 *  \brief 
 *  Configures system to execute next pending signal. Call from switch routine
 *  \relates Task
 */
void GRTOS_Task_GetPendingSCB(void)
{
    SAMPLE_FUNCTION_BEGIN(68)
	GS_TCB *ptcb =gk_PCB_GetCurrentTCB();   /* Get Current Task          */
	GS_SCB *psignal = ptcb->TCB_NextTCBPSL;  /* Get Next Pending Signal   */

	G_SCB_PENDING = 0;               /* Default is no pending SCB */

	if (psignal != (struct gs_scb *) 0)    /* There is a pending SCB    */
	{
		if (psignal->SCBState == G_SCBState_PENDING) { /* Signal is pending      */
			psignal->SCBState = G_SCBState_EXECUTING; /* Signal to execution     */
		    G_SCB_CODE    = (INT32) psignal->SCB_TaskCode + 4; /* Task Code                     */
			G_SCB_ARG     = (INT32) psignal->SCB_TaskArg;       /* Task Argument                 */
		    gk_TCBPSL_Unlink(ptcb, psignal);            /* Unlink SCB from TCB       */
		    gk_SCBFL_Link(psignal);                     /* Link SCB to free list     */
			G_SCB_PENDING = 1;                       /* Set the Pending Signal status */
		}
	}
    alt_dcache_flush_all();
    SAMPLE_FUNCTION_END(68)
}

/**********************************************************************************
 *                        	Exception handler
 *  from http://www-ug.eecg.toronto.edu/msl/manuals/n2sw_nii52006.pdf
 *********************************************************************************/

alt_exception_result handler(alt_exception_cause cause,
                              alt_u32 addr,
                              alt_u32 bad_addr )
{
    fprintf(stderr,"[ EXCEPTION ] cause= %d, address= 0x%x, bad_addr: 0x%x\n", (int) cause, (unsigned int) addr, (unsigned int) bad_addr);
    while(1);
    return (alt_exception_result) 0;
}


/**********************************************************************************
 *                        	IDLE TASK DOING NOTHING
 *********************************************************************************/
/**gk_CODE_IDLE_TASK
 *  \brief IDLE task. This code is executed when no task is ready. The processor is halted.
 *  
 *  \param [in] pdata Pointer to data
 *  \details Task can be changed. The processors run this task with different stacks
 *  \relates Task
 */
void gk_CODE_IDLE_TASK(void* pdata)
{
    void *StackPointer = (void *) GRTOS_CMD_PRC_SP;               // Check that SP is in the stack of the IDLE task of the processor
	while(1)
    {
        PRINT_ASSERT(((void *) GRTOS_CMD_PRC_SP == StackPointer ),"ERROR SP = 0x%x, StackPointer= 0x%x\n",(int) GRTOS_CMD_PRC_SP, (int) StackPointer);
        #if G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE == 1
            NIOS2_READ_SP(StackPointer);
            gu_fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "I, proc=%d, sp=%p \n", GRTOS_CMD_PRC_ID, StackPointer);
            PRINT_ASSERT(((void *) GRTOS_CMD_PRC_SP == StackPointer ),"ERROR SP = 0x%x, StackPointer= 0x%x\n",(int) GRTOS_CMD_PRC_SP, (int) StackPointer);
        #endif
        #ifdef ALT_SEMIHOSTING
            gu_fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "ALT_SEMIHOSTING defined\n");
        #endif
        #ifdef ALT_USE_DIRECT_DRIVERS
            gu_fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "ALT_USE_DIRECT_DRIVERS defined\n");
        #endif        
       
        GRTOS_CMD_HALT_PROCESSOR; // Put the procesor in HALT mode
        PRINT_ASSERT(((void *) GRTOS_CMD_PRC_SP == StackPointer ),"ERROR SP = 0x%x, StackPointer= 0x%x\n",(int) GRTOS_CMD_PRC_SP, (int) StackPointer);
    }
}





OPTIMEZE_RESTORE
