/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Definitions for User GRTOS API
 *  \details This file contains the Definitions for User GRTOS API.
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


#ifndef GRTOSUSERAPI_H_
#define GRTOSUSERAPI_H_

#include <system.h>


/**
 *  System constants definitions
 */
#define G_FALSE             0u
#define G_TRUE              1u

// TCBType group
/// \brief TCBType valid values
#define G_TCBType_UCOS                          1u   ///< \brief Task type UCOS "one run only" (with optional infinite loop)           \ingroup TCBType
#define G_TCBType_PERIODIC                      2u   ///< \brief Task type PERIODIC "with periodic release"                            \ingroup TCBType
#define G_TCBType_ISR                           4u   ///< \brief Task type ISR. It is released for an interrupt event                  \ingroup TCBType
#define G_TCBType_IDLE                          5u   ///< \brief Task type IDLE. It is executed when processor is not assigned to task \ingroup TCBType
#define G_TCBType_UNDEFINED                     7u   ///< \brief Task type UNDEFIEND. Wehn not a specific type is given to the task    \ingroup TCBType

// SCBType group
/// \brief SCBType valid values 
/// \todo Define the rest of SCB types
#define G_SCBType_TCB_ABORTED          1000u      ///< TYPE "Periodic task aborted when released before deadline"      \ingroup SCBType


/**
 * Structure Definitions
 */
// typedef unsigned long  INT32;       /* Unsigned 32 bit quantity      */
typedef unsigned INT32;       /* Unsigned 32 bit quantity      */
typedef unsigned int   GS_STK;       /* Type to Stack Pointers        */

typedef unsigned long long gt_time;
typedef unsigned long long gt_priority;


typedef struct gs_kcb      GS_KCB;
typedef struct gs_pcb      GS_PCB;
typedef struct gs_lcb      GS_LCB;
typedef struct gs_tcb      GS_TCB;
typedef struct gs_ecb      GS_ECB;
typedef struct gs_scb      GS_SCB; 
typedef struct gs_rrds     GS_RRDS;
typedef struct g_rgb       G_RCB;
typedef struct gs_mcb      GS_MCB; 
typedef struct g_rgb       t_message_resource; 
typedef struct g_rgb       t_semaphore_resource;


typedef struct gs_tm {
	int	tm_msec;	/* Seconds: 0-59 (K&R says 0-61?) */
	int	tm_sec;		/* Seconds: 0-59 (K&R says 0-61?) */
	int	tm_min;		/* Minutes: 0-59 */
	int	tm_hour;	/* Hours since midnight: 0-23 */
	int	tm_day;     /* Day of the month: 1-31 */
	int	tm_year;	/* Years since 1900 */
} gt_tm;


INT32  gu_TASK_Sleep(INT32 hours, INT32 minutes, INT32 seconds, INT32 ms);
INT32  gu_TASK_Sleep_Time(gt_time ticks);

/**
 *  Function Headers for user usage
 */
int     gu_Get_CPU_ID(void);
int     gu_Get_IRQ_Register(void);
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
INT32    gu_get_halt_status(void);
INT32    gu_get_irq_status(void);
INT32    gu_get_reserved_mutex_processor(void);
gt_time gu_get_now(void);
gt_time gu_get_frozen_threshold(void);

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
                       

#endif /* GRTOSUSERAPI_H_ */


