/// \file 
/// \author Ricardo Cayssials
/// \brief GRTOS general definition
/// \details Main file for definition
/// \version 0.9a
/// \date      Created on: Sep 16, 2013
/// \bug   None known
/// \copyright Ricardo Cayssials 
/// \copyright All rights reserved.
/// \warning This product was produced by Ricardo Cayssials in the hope that it 
/// will be useful, but WITHOUT ANY WARRANTY; without even the implied 
/// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
/// \warning DISCLAIMER OF WARRANTY. THIS PRODUCT IS LICENSED "AS IS." YOU BEAR THE RISK OF USING IT. 
/// \warning RICARDO CAYSSIALS GIVES NO EXPRESS WARRANTIES, GUARANTEES, OR CONDITIONS. 
/// TO THE EXTENT PERMITTED UNDER APPLICABLE LAWS, RICARDO CAYSSIALS EXCLUDES ALL IMPLIED WARRANTIES,
/// INCLUDING MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.



/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) Ricardo L. Cayssials                                          *
* All rights reserved.                                                        *
*                                                                             *
******************************************************************************/

#ifndef GRTOSDEFINITIONS_H_
#define GRTOSDEFINITIONS_H_

// Fijarse aca para separar el codigo del switch
// https://stackoverflow.com/questions/4550075/can-i-append-to-a-preprocessor-macro
// https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html
// https://gcc.gnu.org/onlinedocs/cpp/Directives-Within-Macro-Arguments.html#Directives-Within-Macro-Arguments

// Check https://stackoverflow.com/questions/1154709/how-can-i-hide-the-declaration-of-a-struct-in-c


// #pragma GCC optimize('mbypass-cache')
// #pragma GCC optimize('mno-cache-volatile')

#include <system.h>
#include <stdio.h>
#include "sys/alt_irq.h"
#include <grtosuserapi.h>
#include <grtos_regs.h>
#include <assert.h>

#include <ext_isr.h>
#include <sys/alt_cache.h>

#include "../runtime/rts_gemrtos.h"

#include <inttypes.h>

/****************************************************************************************
 *                   GRTOS SYSTEM GENERAL DEFINITIONS
 ***************************************************************************************/
#define G_ISR_STACKSIZE                          4096  ///< \brief Size of the stack of an Interrupt task 
#define G_ISR_PRIORITY                              0  ///< \brief Priority assigned to an Interrupt task 
#define G_IDLE_STACKSIZE                         4096  ///< \brief Size of the stack of an Idle task      
#define G_NUMBER_OF_LCBs_FOR_PCB                   2u  ///< \brief Number of LCB list for PCB             


/************************************************************************************
 *  COMPILATION OPTIONS
 ************************************************************************************/
 
// from https://stackoverflow.com/questions/2219829/how-to-prevent-gcc-optimizing-some-statements-in-c
// #pragma GCC push_options
// #pragma GCC optimize ("O3")

// from https://stackoverflow.com/questions/64026719/gcc-cli-option-o0-surprisingly-discards-code-after-pragma-gcc-optimize-in-macr
#define _STRING(S) #S
#define STRING(S) _STRING(S)
#define OPTIMEZE_CODE(OLEVEL) \
    _Pragma("GCC push_options") \
    _Pragma(STRING(GCC optimize ("O3")))

// _Pragma(STRING(GCC optimize ("O" #OLEVEL)))

#define OPTIMEZE_RESTORE \
    _Pragma("GCC pop_options")  

// from https://stackoverflow.com/questions/58477537/labels-redefined-in-macro-in-c
#define RTSZ(i) RTSZ1(i, __LINE__, __FUNCTION__)
#define RTSZ1(i, l, f) RTSZ2(i, l, f)
#define RTSZ2(i, l, f) RTSZ_##i##_##l##_##f


#define SAMPLE_FUNCTION_BEGIN(number) \
   //  if (G_Running == G_TRUE) { \
   //      fprintf(fpuart[GRTOS_CMD_PRC_ID-1],"Proc %d in func %s, file %s, %d \n",GRTOS_CMD_PRC_ID , __FUNCTION__,__FILE__,__LINE__); \
   //  } \
    if ((G_DEBUG_SAMPLE_BEGIN_ENABLE == 0xFFFFFFFF) ||  (G_DEBUG_SAMPLE_BEGIN_ENABLE == number)){ \
        RTSZ(0):  \
        IOWR(GRTOS_DRIVER_GRTOS_MONITOR_BASE, 0, (unsigned int) &&RTSZ(0) | 0x80000000); \
    }

#define SAMPLE_FUNCTION_END(number) \
    // if (G_Running == G_TRUE) { \
    //     if ((g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackPointer < g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackTop - 300) || (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackPointer > g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackBottom)) { \
    //         PRINT_TO_DEBUG("ERROR TCB= %p, TCB_StackBottom = %p, TCB_StackPointer = %p, TCB_StackTop = %p\n",g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB, (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackBottom, (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackPointer, (void *)  g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackTop); \
    //         PRINT_TO_DEBUG("ERROR TCB_IDLE= %p\n", (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB ); \
    //         G_DEBUG_WHILEFOREVER; } \
    // } \
    // if ((G_DEBUG_SAMPLE_END_ENABLE == 0xFFFFFFFF) ||  (G_DEBUG_SAMPLE_END_ENABLE == number)){ \
        RTSZ(0):  \
        IOWR(GRTOS_DRIVER_GRTOS_MONITOR_BASE, 0, (unsigned int) &&RTSZ(0) | 0xC0000000); \
    }


/************************************************************************************
 *  GRTOS INTERNAL DEFINITIONS
 ************************************************************************************/
/// \brief G_NUMBER_OF_PCB Number of processors defined in Qsys/Platform Designer
/// \todo Define a function to get this value during runtime
#define G_NUMBER_OF_PCB      GRTOS_DRIVER_NPROCESSORS  /**    */

/// \brief G_NUMBER_OF_IRQ_TCB Number of IRQs defined in Qsys/Platform Designer
/// \todo Define a function to get this value during runtime
#define G_NUMBER_OF_IRQ_TCB			         ALT_NIRQ  

/// \brief G_TICKS_PER_SECOND 
/// \todo Define a function to get this value during runtime
#define G_TICKS_PER_SECOND  (int)(GRTOS_DRIVER_GRTOSFREQUENCY/(GRTOS_DRIVER_PRESCALE))

/// \brief G_TICKS_PER_MSECOND 
/// \todo Define a function to get this value during runtime
#define G_TICKS_PER_MSECOND (G_TICKS_PER_SECOND/1000)


/// \Brief Event codes for gk_ENTRY_IRQ_HANDLER
#define EVN_CODE_TIMED  0x21
#define EVN_CODE_FROZEN 0x22
/************************************************************************************
 *  COMPILATION OPTIONS
 ************************************************************************************/
/**
 *  G_GRTOS_PRESERVE_HAL_ISR
 *  If 1, the HAL ISR are preserved in GRTOS (producing overhead)
 *  If 0, no ISR are preserved
 */
#define G_GRTOS_PRESERVE_HAL_ISR 0

/**
 *  G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE
 *  Used to print to the processor console when processor goes to IDLE
 *  when 1: message is printed when proessor goes to IDLE
 *  when 0: no message is printed
 */
#define G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE 1

/**
 * G_DEBUG_WHILEFOREVER_ENABLE
 * Used to insert code to check during runtime data conistency in several functions
 * when 1: checks are enable
 * when 0: checks are disable
 */
#define G_DEBUG_WHILEFOREVER_ENABLE  1


/**
 * G_DEBUG_WHILEFOREVER
 * Executed when G_DEBUG_WHILEFOREVER_ENABLE is 1 and an inconsistency is found
 */
#define G_DEBUG_WHILEFOREVER \
			 do{int dest; \
                 fprintf(fpuart[GRTOS_CMD_PRC_ID-1],"ERROR in for Proc %d in function %s, file = %s, line = %d \n",GRTOS_CMD_PRC_ID , __FUNCTION__,__FILE__,__LINE__); \
				 while(1){dest++;\
			 }}while(0)


/// \todo Check if it required
#define G_DEBUG_RUN_MONITOR_ROUTINE \
    do{ \
        GRTOS_CMD_DEBUG_HOLD_ENB; \
        fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "In %s, %d, proc %x, mtx %x \n",__FUNCTION__,__LINE__, GRTOS_CMD_PRC_ID,GRTOS_MTX_PRC_GRANTED); \
        GRTOS_CMD_DEBUG_HOLD_DIS; } while(0)

/**
 * PRINT_TO_DEBUG
 * print to the stderr console if open, open it otherwise
 */
#define PRINT_TO_DEBUG(...) \
    fprintf(fpuart[GRTOS_CMD_PRC_ID-1], ##__VA_ARGS__);

    
/// \todo Check if it required
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	#define G_DEBUG_VERBOSE_RUNTIME \
        GRTOS_CMD_DEBUG_HOLD_ENB; \
        G_DEBUG_RUN_MONITOR_ROUTINE; \
		fprintf(fpuart[GRTOS_CMD_PRC_ID-1],"Proc %d, func %s \n",GRTOS_CMD_PRC_ID , __FUNCTION__); \
        GRTOS_CMD_DEBUG_HOLD_DIS; 
#else
	#define G_DEBUG_VERBOSE_RUNTIME
#endif

/// \todo Check if it required
//#define G_DEBUG_VERBOSE G_DEBUG_RUN_MONITOR_ROUTINE
//#define G_DEBUG_VERBOSE G_DEBUG_VERBOSE_RUNTIME 
#define G_DEBUG_VERBOSE  

/**
 *  \brief PRINT_DEBUG_LINE can be defined to print a message after the execution of each line of the code.
 *  It is defined as empty when no in debug mode
 *  /// \todo Check if it required
 */
// #define PRINT_DEBUG_LINE  fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "%s, %s, %d, proc %x, mtx %x\n",,__FILE__,__LINE__, GRTOS_CMD_PRC_ID, GRTOS_MTX_PRC_GRANTED);
// #define PRINT_DEBUG_LINE  fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "%s, %d\n",__FUNCTION__,__LINE__);       
#define PRINT_DEBUG_LINE  

/************************************************************************************
 *  GRTOS TYPE DEFINITIONS
 ************************************************************************************/

/* This is the definition for Nios32.  */
//typedef unsigned long      INT32;             /* Unsigned 32 bit quantity           */
typedef unsigned long long INT64;


typedef union timepriority {
	INT64 i64;
	INT32 i32[2];
} TIMEPRIORITY;



/************************************************************************************
 *  GRTOS COMMANDS
 ************************************************************************************/

/// GRTOS_CMD_PRC_ID  - returns the cpuID of the current processor
#define GRTOS_CMD_PRC_ID  __builtin_rdctl(5)



/// \brief GRTOS_CMD_PRC_INT(proc) 
/// interrupts the processor and waits until it reaches the ISR and disables the interrupt
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_PRC_INT(proc) \
    GRTOS_CMD_TRG_PRC_INT_SET(proc); \
    while (GRTOS_CMD_IRQ_ENB_GET(proc)){ \
        while(0); \
    }


/// \brief GRTOS_CMD_HALT_PROCESSOR puts the processor in halt mode
/// The first command enable the IDLE state for the processor and the second read the first 
/// address of the processor bus because it is in waitrequest until next interrupt
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_HALT_PROCESSOR \
        GRTOS_CMD_HLT_IDL_PRC; \
        g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].GRTOS_PROCESSOR_BASE[0] = (int) 0;

/************************************************************************************
 *  GRTOS CRITICAL SECTION COMMANDS
 ************************************************************************************/

/**
 *  \brief GRTOS_CMD_CRITICAL_SECTION_GET 
 *  Defines the entry to a critical section to handle system variables and ISR routines.
 *  It asks for critical section and puts the processor in halt mode. 
 *  GRTOS controller enables the processor when section is granted to it.
 */
#define GRTOS_CMD_CRITICAL_SECTION_GET \
	do{ \
		do { \
            GRTOS_MTX_RSV_SET; \
            GRTOS_CMD_HALT_PROCESSOR \
        } while (GRTOS_CMD_PRC_ID != GRTOS_MTX_PRC_GRANTED);  \
	}while(0)


/**
 *  \brief GRTOS_CMD_CRITICAL_SECTION_RELEASE 
 *  Releases the critical section from the current processor.
 *  The final released is delayed by the controller to let the processor finishes executing 
 *  the return from the critical section.
 *  It should be executed from the interrupt routine.
 */
#define GRTOS_CMD_CRITICAL_SECTION_RELEASE \
	do{ GRTOS_CMD_PRC_INT_ENB; \
        alt_dcache_flush_all(); \
        GRTOS_MTX_RLS; \
	}while(0)
  
/**
 *  \brief GRTOS_CMD_MTX_RQS_GET 
 *  Returns the current value of the Mutex.
 */
#define GRTOS_CMD_MTX_RQS_GET GRTOS_MTX_PRC_GRANTED;

/**
 *  \brief GRTOS_USER_CRITICAL_SECTION_GET defines the entry to a critical section to handle system variables.
 *  It is called from outside an interrupt and it may be interrupted while it is waiting for mutex.
 *  It should be used in user functions that execute kernel functions or modify kernal data.
 */
#define GRTOS_USER_CRITICAL_SECTION_GET  GRTOS_CMD_CRITICAL_SECTION_GET


/**
 *  \brief GRTOS_USER_CRITICAL_SECTION_RELEASE releases the critical section from the current processor.
 *  It is called from outside of an interrupt.
 *  It used in all the user function that executes kernel functions or modify kernel data.
 */
#define GRTOS_USER_CRITICAL_SECTION_RELEASE  GRTOS_CMD_CRITICAL_SECTION_RELEASE


#include <gemrtos_core.h>
// #include <gemrtos_core_api.h>


/*
*********************************************************************************************************
*                                             VARIABLES
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



// extern void *GRTOS_BASE;
// extern void *GRTOS_MONITOR_BASE;



extern GS_KCB g_kcb;

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
extern volatile INT64 G_TASK_PRIORITY_DEFAULT;
extern volatile INT64 G_TASK_PERIOD_DEFAULT;

/**
*********************************************************************************************************
*                            TIME and PRIORITY definitions
*********************************************************************************************************
**/ 
#define G_LOWEST_PRIORITY    0x7FFFFFFFFFFFFFFCLL  ///< Lowest Priority of a Task
#define G_IDLETASK_PRIORITY  0x7FFFFFFFFFFFFFFDLL  ///< Priority of Idle Task
#define G_LATEST_TIME        0x7FFFFFFFFFFFFFFFLL  ///< Lastest Occurence Time of an EVENT


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

// GS_MCB *gk_MCB_GetFree(void);
// void    gk_MCBFL_Link(GS_MCB *pmcb);

// int     gk_MCBQL_Link(GS_MCB *pmcb, G_RCB * prcb);
// void    gk_MCBQL_Unlink(GS_MCB *pmcb, G_RCB * prcb);

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
INT32    gk_TASK_PRIORITY_SET(GS_TCB *ptcb, INT32 task_state);
INT64   gk_TASK_GRANTED_PRIORITY_GET(GS_TCB *ptcb);
INT32    gk_TASK_INHERENCE_PRIORITY_SET(GS_TCB * ptcb);


/***************************************************************/
/*  INTERNAL ROUTINES FOR TASK EXECUTION                       */
/***************************************************************/
// void gk_Start_Main(void);
void gk_KERNEL_TASK_START (void);
void gk_ENTRY_IRQ_HANDLER (void);
// void GRTOS_Start_Task(void);
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
                     INT64 TCBInherPriority, int TCB_INTNumber); 
                     
INT32 gk_TASK_STK_Init(GS_TCB *ptcb);
// void gk_TASK_TCB_Init (GS_TCB *ptcb);
// void gk_INIT_IRQ(void);
// void gk_KERNEL_TIME_IRQ_HANDLER (void);
void gk_TIME_CALLBACK(GS_ECB *event);
void gk_RESOURCE_ECB_KILL_CALLBACK(GS_ECB *pevent);
void gk_FROZEN_CALLBACK(void);
void gk_INIT_KERNEL(void);
void gk_KERNEL_TASK_COMPLETE(void);
void GRTOS_Task_GetPendingSCB(void);
// void gk_KERNEL_FROZEN_IRQ_HANDLER(void);
void gk_RST_MONITOR_HANDLER (void);


/***************************************************************/
/*  SYSTEM TASKS                                               */
/***************************************************************/
void gk_ENTRY_SIGNAL_RETURN(void);
void gk_CODE_IDLE_TASK(void* pdata);




#endif /* GRTOSDEFINITIONS_H_ */
