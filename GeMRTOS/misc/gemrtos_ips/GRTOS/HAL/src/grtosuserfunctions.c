/**
 *  \file grtosuserfunctions.c
 *  \author Ricardo Cayssials
 *  \brief GRTOS User Interface API
 *  \details This file contains the main API functions without resource specific user functions.
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


#include <gemrtos_core.h>
#include <grtos.h>
#include <stdlib.h>

OPTIMEZE_CODE(3)

/**
 *  \class Configuration
 *  \class List
 *  \class Task
 *  \class Signal
 *  \class Miscellaneous
 *  \class Validation
 *  \class Resource
 *  \class Event
 *  \class RRDS
 *  \class Interrupt
 *  \class Kernel
 *  \class Processor
 *  \class Semaphore
 *  \class RRSD
 *  \class TASK
 *  \class Time
 *  \class Monitor
 *  \class Core
 */


/***********************************************************************************
 * *********************************************************************************
 *                    gRTOS Default Setting functions
 * *********************************************************************************
 ***********************************************************************************/
/**
 *  \brief gu_Set_Default_Task_Type(unsigned int type)
 *  Set the default type of the task created.
 *  
 *  \param [in] type Default type: G_TCBType_UCOS for uCOS type, and
 *                                 G_TCBType_PERIODIC for periodic type.  
 *  \return G_TRUE if type is valid, G_FALSE
 *  
 *  \details The type defined as default will be used as the default type for the task created afterwards.
 *  \relates Configuration
 *  \todo Check validity of type previous to change it
 */
INT32 gu_Set_Default_Task_Type(unsigned int type)
{
    G_TASK_TYPE_DEFAULT = type; 
    return G_TRUE;
}


/**
 *  \brief 
 *  Return the default type assigned to new tasks 
 *  \return Current default type
 *  
 *  \details The default type is assigned with gu_Set_Default_Task_Type
 *  \relates Configuration
  */
INT32 gu_Get_Default_Task_Type(void)
{
    return G_TASK_TYPE_DEFAULT;
} 

/**
 *  \brief 
 *  Set the default list number for task created afterwards
 *  \param [in] list Number of list to be set as defult
 *  \return G_TRUE if set was successfull, G_FALSE otherwise
 *  
 *  \details Set the default list number
 *  \todo Check if list is a valid list and return accordingly
 *  \relates Configuration
 */
INT32 gu_Set_Default_Task_List(GS_LCB *list)
{
    G_TASK_LCB_DEFAULT = list;
    return G_TRUE;
}

/**
 *  \brief 
 *  Return the current default list number
 *  \return INT32 with the default list number
 *  
 *  \details Return the current default list number
 *  \relates Configuration
 */
INT32 gu_Get_Default_Task_List(void)
{
    return (INT32) G_TASK_LCB_DEFAULT;
}

/**
 *  \brief 
 *  Set the default priority for task created afterwards. 
 *  Priority is assigned with an INT32 level and an INT32 priority in such level.
 *  \param [in] level    INT32 level
 *  \param [in] priority INT32 level
 *  \relates Configuration
 */
void gu_Set_Default_Task_Priority(INT32 level, INT32 priority)
{
    TIMEPRIORITY pri_aux; 
    pri_aux.i32[0] = (INT32) priority;
    pri_aux.i32[1] = (INT32) level;
    G_TASK_PRIORITY_DEFAULT = (INT64) pri_aux.i64;
}


/**gu_Set_Default_Task_Period
 *  \brief 
 *  Set the default periodo for task created afterwards.
 *  \param [in] hours   INT32 hours
 *  \param [in] minutes INT32 minutes
 *  \param [in] seconds INT32 seconds
 *  \param [in] ms      INT32 ms
 *  \relates Configuration
 */
void gu_Set_Default_Task_Period(INT32 hours, INT32 minutes, INT32 seconds, INT32 ms)
{
    /* Compute the total number of ticks required for the period */
    INT64 ticks;
    
    ms = ms * (unsigned int) G_TICKS_PER_MSECOND;
    seconds = seconds * (unsigned int) G_TICKS_PER_MSECOND * 1000;
    minutes = minutes * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60;
    hours   = hours   * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60 * 60;
    ticks   = (INT64) hours + (INT64) minutes + (INT64) seconds + (INT64) ms;
    
    G_TASK_PERIOD_DEFAULT = ticks;
}



/***********************************************************************************
*********************** TASK SYSTEM CALLS            ***************************
***********************************************************************************/
 
/**gu_SetTaskType
 *  \brief 
 *  Set the type of a task. 
 *  \param [in] ptcb Pointer to the TCB of the task
 *  \param [in] type Type of task to assign
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Validate type previous to assign
 *  \relates Task
 */ 
INT32 gu_SetTaskType(struct gs_tcb *ptcb, unsigned int type)
{
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;    
        ptcb->TCBType = type;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE;  
    return G_TRUE;
}

/**gu_SetTaskList
 *  \brief 
 *  Set the list of a task.
 *  \param [in] ptcb       Pointer to the TCB of the task
 *  \param [in] plcb       Pointer to the CB of the list
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Check if index is valid
 *  \todo Unlinks from current list and link to the new link accordingly.
 *  \relates Task
 */
INT32 gu_SetTaskList(struct gs_tcb *ptcb, struct gs_lcb *plcb)
{
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;    
        ptcb->TCB_RDY_LCB_Index = plcb;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**gu_SetTaskReadyPriority
 *  \brief 
 *  Set the ready priority of a task
 *  \param [in] ptcb     Pointer to the TCB of the task
 *  \param [in] priority Priority to assing
 *  \return G_TRUE
 *  \todo Unlink and link if task is ready
 *  \todo Put priority as level and priority
 *  \relates Task
 */
INT32 gu_SetTaskReadyPriority(struct gs_tcb *ptcb, long long priority)
{
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;    
        ptcb->TCBReadyPriority = priority;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**gu_SetTaskRunPriority
 *  \brief 
 *  Set the run priority of a task
 *  \param [in] ptcb     Pointer to the TCB of the task
 *  \param [in] priority Priority to assing
 *  \return G_TRUE
 *  \todo Unlink and link if task is running
 *  \todo Put priority as level and priority
 *  \relates Task
 */
INT32 gu_SetTaskRunPriority(struct gs_tcb *ptcb, long long priority)
{
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;    
        ptcb->TCBRunPriority = priority;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**gu_SetTaskDeadline
 *  \brief 
 *  Set the deadline of the task for the next invocation
 *  \param [in] ptcb    Pointer to the TCB of the task
 *  \param [in] hours   Hours of the deadline
 *  \param [in] minutes Minutes of the deadline
 *  \param [in] seconds Seconds of the deadline
 *  \param [in] ms      miliseconds for the deadline
 *  \return G_TRUE
 *  \details The current deadline remains. The new deadline is considered for the next invocation.
 *  \todo Implement the change of the deadline in the currrent invocation. It should considered the difference with the current deadline
 *  \relates Task
 */
INT32 gu_SetTaskDeadline(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    /* Compute the total number of ticks required for the period */
    INT64 ticks;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;    
        ms = ms * (unsigned int) G_TICKS_PER_MSECOND;
        seconds = seconds * (unsigned int) G_TICKS_PER_MSECOND * 1000;
        minutes = minutes * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60;
        hours   = hours   * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60 * 60;
        ticks   = (INT64) hours + (INT64) minutes + (INT64) seconds + (INT64) ms;
        ptcb->TCBDeadline = ticks;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**gu_SetTaskPeriod
 *  \brief 
 *  Set the task period for the next invocation
 *  \param [in] ptcb    Pointer to the TCB of the task
 *  \param [in] hours   Hours of the period
 *  \param [in] minutes Minutes of the period
 *  \param [in] seconds Seconds of the period
 *  \param [in] ms      miliseconds for the period
 *  \return G_TRUE
 *  \details The current period remains. The new period is considered for the next invocation
 *  \todo Implement the change of the period in the currrent invocation. It should considered the difference with the current period
 *  \relates Task
 */
INT32 gu_SetTaskPeriod(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    /* Compute the total number of ticks required for the period */
    INT64 ticks;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;
        ms = ms * (unsigned int) G_TICKS_PER_MSECOND;
        seconds = seconds * (unsigned int) G_TICKS_PER_MSECOND * 1000;
        minutes = minutes * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60;
        hours   = hours   * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60 * 60;
        ticks   = (INT64) hours + (INT64) minutes + (INT64) seconds + (INT64) ms;
        ptcb->TCBPeriod = ticks;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE;  
    return G_TRUE;
}

/**gu_StartTaskwithOffset
 *  \brief 
 *  Activates the execution of a task. An offset can be configured.
 *  \param [in] ptcb    Pointer to the TCB of the task
 *  \param [in] hours   Hours of the offset
 *  \param [in] minutes Minutes of the offset
 *  \param [in] seconds Seconds of the offset
 *  \param [in] ms      miliseconds for the offset
 *  \return G_TRUE 
 *  \todo Include start of a ISR
 *  \todo Include start of an IDLE task??
 *  \relates Task
 */
INT32 gu_StartTaskwithOffset(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    INT64 ticks;
    GS_ECB *pevent;
    
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;
        ms = ms * (unsigned int) G_TICKS_PER_MSECOND;
        seconds = seconds * (unsigned int) G_TICKS_PER_MSECOND * 1000;
        minutes = minutes * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60;
        hours   = hours   * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60 * 60;
        ticks   = (INT64) hours + (INT64) minutes + (INT64) seconds + (INT64) ms;
    
        switch (ptcb->TCBType){
            case G_TCBType_UCOS:
                gk_TCBRDYL_Link(ptcb);  /* Insert Task in Ready List                */
                break;
            case G_TCBType_PERIODIC:
                /* Get the starting time of the task depending whether the  RTOS is running or not */
                if (G_Running != G_FALSE) ticks = GRTOS_now + ticks;
                // else time = GRTOS_now + ticks;

                if (ticks == (INT64) 0){
                    /* Task Ready and Set next Release in Period Time from start time */
                    gk_TCBRDYL_Link(ptcb);  /* Insert Task in Ready List                */
                    ticks = (INT64) ticks + ptcb->TCBPeriod;  
                }
                else
                {
                    gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);
                }
                pevent = gk_ECB_GetFree();
                pevent->ECBValue.i64 = (INT64) ticks;
                pevent->ECBType = (INT32) G_ECBType_PERIODIC;
                gk_TCBAEL_Link(pevent, ptcb);
                gk_ECBTL_Link(pevent);
                break;
            // case G_TCBType_ISR:
            //     gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);
            //     break;
            // case G_TCBType_IDLE:
            //     gk_TCBRDYL_Link(ptcb);  /* Insert Task in Ready List                */
            //     break;
            default:
                G_DEBUG_WHILEFOREVER;
                break;
        }

        /* Si está ejecutandose el GRTOS, hago cambio de tarea si corresponde */
    if (G_Running == G_TRUE) gk_KERNEL_TASK_SUSPEND_CURRENT();
    
    return G_TRUE;
}

/**gu_SetTaskAbortwhenDeadline
 *  \brief 
 *  Set if task should be aborted when misses a deadline
 *  \param [in] ptcb                Pointer to the TCB of the task
 *  \param [in] abort_when_deadline G_TRUE when task should be aborted when deadline is missed, G_FALSE otherwise
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \relates Task
 */
INT32 gu_SetTaskAbortwhenDeadline(struct gs_tcb *ptcb, unsigned int abort_when_deadline)
{
    if ((abort_when_deadline != G_TRUE) && (abort_when_deadline != G_FALSE)) return G_FALSE;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;    
        ptcb->TCB_Abort_w_Deadline = abort_when_deadline;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}


/**gu_GetTask
 *  \brief 
 *  Return a pointer to a TCB for a new task
 *  \param [in] TaskCode  Pointer to the beginning of the task code
 *  \param [in] p_arg     Pointer to the argument of the first call
 *  \param [in] StkBotton Botton of the Stack of the Task  
 *  \param [in] StkSize   Size of the stack in words
 *  \return Pointer to the TCB of the task created
 *  \details Task is created but has to be activated with gu_StartTaskwithOffset
 *  \relates Task
 */
void *gu_GetTask(void         *TaskCode,
				 void         *p_arg,      
				 void         *StkBotton,  
                 unsigned int  StkSize)	   
{
    GS_TCB  *ptcb;
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;    
    
    ptcb = gk_CreateTask(TaskCode,                
				         p_arg,                   
				         StkBotton,               
                         StkSize,                 
                         G_TASK_TYPE_DEFAULT,     
                         G_TASK_PRIORITY_DEFAULT, 
                         G_TASK_PRIORITY_DEFAULT, 
                         G_TASK_PERIOD_DEFAULT,   
                         G_TASK_PERIOD_DEFAULT,   
                         (GS_LCB *) G_TASK_LCB_DEFAULT,      
                         G_TRUE,                  
                         G_TASK_PRIORITY_DEFAULT,
                         0); 
   
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE;
	return ((void *) ptcb);
}


/**gu_TASK_Sleep
 *  \brief 
 *  Sleeps the current task the interval specified.
 *  \param [in] hours   Hours to sleep
 *  \param [in] minutes Minutes to sleep
 *  \param [in] seconds Seconds to sleep
 *  \param [in] ms      miliseconds to sleep
 *  \return G_TRUE
 *  \relates Task
 */
INT32  gu_TASK_Sleep(INT32 hours, INT32 minutes, INT32 seconds, INT32 ms)
{
    /* Compute the total number of periods required   */
    INT64 ticks;
    
    ms = ms * (INT32) G_TICKS_PER_MSECOND;
    seconds = seconds * (INT32) G_TICKS_PER_MSECOND * (INT32) 1000;
    minutes = minutes * (INT32) G_TICKS_PER_MSECOND * (INT32) 1000 * (INT32) 60;
    hours = hours * (INT32) G_TICKS_PER_MSECOND * (INT32) 1000 * (INT32) 60 * (INT32) 60;
    ticks = (INT64) hours + (INT64) minutes + (INT64) seconds + (INT64) ms;
    
    if (ticks != 0){
    	gu_TASK_Sleep_Time((INT64) ticks);
    }
    return (G_TRUE);
}


/**gu_TASK_Sleep_Time
 *  \brief 
 *  Set event to sleep the current task for a certain number of clock ticks
 *  \param [in] ticks Number of clock ticks to sleep the task
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \details It is executed in user mode and gu_TASK_Sleep should be used instead
 *  \todo Check if it can be implemented only with gk_KERNEL_TASK_SUSPEND instead of gk_KERNEL_TASK_SUSPEND_CURRENT
 *  \relates Task
 */ 
INT32 gu_TASK_Sleep_Time(gt_time ticks)
{
	GS_TCB  *ptcbcurrent;
	GS_ECB *pevent;

    GRTOS_USER_CRITICAL_SECTION_GET; /* Entro a la seccion critica del GRTOS Controller */
    
    ptcbcurrent = gk_PCB_GetCurrentTCB(); /* Get the current task */
	pevent = gk_ECB_GetFree();

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if ((TCB_IsValid(ptcbcurrent) != G_TRUE) && (ptcbcurrent == (struct gs_tcb *) 0)) G_DEBUG_WHILEFOREVER;
	if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

	pevent->ECBValue.i64 = GRTOS_now + ticks;

	pevent->ECBType = (INT32) G_ECBType_OSTimeDly;
    gk_TCBAEL_Link(pevent, ptcbcurrent);
	gk_ECBTL_Link(pevent);

	gk_KERNEL_TASK_SUSPEND_CURRENT();

    /* Retorna fuera de la seccion critica */
    // GRTOS_USER_CRITICAL_SECTION_GET;
    // 
    // gk_TCBAEL_Unlink(pevent);
	// gk_ECBFL_Link((GS_ECB *) pevent);
    // 
	// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**gu_TASK_Kill
 *  \brief 
 *  The task is eliminated and all the structures involved are return to the free lists.
 *  \param [in] ptcb Pointer to the TCB of the task to be killed
 *  \return G_TRUE
 *  \todo Check that all the structures are adequatelly treated
 *  \relates Task
 */
INT32 gu_TASK_Kill(GS_TCB *ptcb)
{
    G_DEBUG_VERBOSE
    GRTOS_USER_CRITICAL_SECTION_GET;
        gk_TASK_Kill((GS_TCB *) ptcb);
	GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**gu_TASK_SUSPEND
 *  \brief 
 *  Change to waiting the state of a task
 *  \param [in] ptcb Pointer to the TCB of the task to suspend
 *  \return G_TRUE
 *  \todo Check gk_KERNEL_TASK_SUSPEND with current task
 *  \relates Task
 */
INT32 gu_TASK_SUSPEND(GS_TCB *ptcb)
{
	G_DEBUG_VERBOSE
    GRTOS_USER_CRITICAL_SECTION_GET;
        gk_KERNEL_TASK_SUSPEND((GS_TCB *) ptcb);
	// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return G_TRUE;
}

/**gu_TASK_IS_BLOCKED
 *  \brief 
 *  Return G_TRUE if task is waiting for a resource, G_FALSE otherwise
 *  \param [in] ptcb Pointer to the TCB of the task
 *  \return G_TRUE if task is waiting for a resource, G_FALSE otherwise
 *  \todo Check correcteness for diferent resources (semaphore and message queue)
 *  \relates Task
 */
INT32 gu_TASK_IS_BLOCKED(GS_TCB *ptcb)
{
    INT32 value;
	G_DEBUG_VERBOSE
    GRTOS_USER_CRITICAL_SECTION_GET;
        value = gk_TASK_IS_BLOCKED((GS_TCB *) ptcb);
	GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return value;
}

/**gu_TASK_RESUME
 *  \brief 
 *  Resume a task that it is in waiting state
 *  \param [in] ptcb Pointer to the TCB of the task to be resumed
 *  \return G_TRUE if task was resumed, G_FALSE otherwise
 *  \todo Check that GRTOS is running
 *  \relates Task
 */
INT32 gu_TASK_RESUME(GS_TCB *ptcb)
{
    int return_value = G_FALSE;
    GRTOS_USER_CRITICAL_SECTION_GET;
        if (GRTOS_TASK_STATE_WAITING(ptcb))	{
            gk_TCB_Unlink(ptcb); PRINT_DEBUG_LINE
            gk_TCBRDYL_Link(ptcb); PRINT_DEBUG_LINE
            return_value = G_TRUE;
        }
        
	GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return return_value;
}


/***********************************************************************************
*********************** SIGNAL FUNCTIONS                 ***************************
***********************************************************************************/
 
/**gu_signal_create
 *  \brief 
 *  Creates an associated SCB to a structure according to the Type
 *  \param [in] Type        Signal type
 *  \param [in] Priority    Priority of the signal
 *  \param [in] pxcb        Pointer to the Control Block of the structure to link the signal created
 *  \param [in] Signal_Code Pointer to the first address of the Signal_Code
 *  \return Pointer to the SCB created
 *  \todo Check SCB availability
 *  \todo Include signal for the rest of control blocks
 *  \todo Define signals for different events and resources
 *  \todo ASigne TaskArg to the argument different from Type
 *  \relates Signal
 */ 
GS_SCB *gu_signal_create(INT32 Type, INT32 Priority, void *pxcb, void *Signal_Code, void *Signal_Arg)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
	GS_SCB *pscb = gk_SCB_GetFree();

	pscb->SCBPriority  = Priority;
	pscb->SCBType      = Type;
	pscb->SCB_TaskCode = Signal_Code;
    pscb->SCB_TaskArg  = (void *) Signal_Arg; //(void *) pscb;
	pscb->SCB_NextSCBAPSL = (struct gs_scb *) 0;
	pscb->SCB_AssocXCB = pxcb;
	switch (Type){
		// Types for KCB
		case 1:
		case 2:
			gk_KCBASL_Link(pscb);
			pscb->SCBState = G_SCBState_IN_KCB;
			break;

		// Types for ECB
		case 3:
			gk_ECBASL_Link((GS_ECB *)pxcb, pscb);
			pscb->SCBState = G_SCBState_IN_ECB;
			break;

		// Types for TCB
		case G_SCBType_TCB_ABORTED:
			gk_TCBASL_Link((GS_TCB *)pxcb, pscb);
			pscb->SCBState = G_SCBState_IN_TCB;
			break;

		// Types for RCB
		case 5:
			gk_RCBASL_Link((G_RCB *)pxcb, pscb);
			pscb->SCBState = G_SCBState_IN_RCB;
			break;

		// Types for PCB
		//case 6:
		//	pscb->SCBState = G_SCBState_IN_PCB;
		//	break;

		// Types for LCB
		//case 7:
		//	pscb->SCBState = G_SCBState_IN_LCB;
		//	break;
	}
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
	return((GS_SCB *) pscb);
}

/**gu_signal_destroy
 *  \brief 
 *  Unlink the signal associated to a control block
 *  \param [in] pscb Pointer to the SCB of the signal
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Check everything of this function
 *  \relates Signal
 */
INT32 gu_signal_destroy(GS_SCB *pscb)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
	void *pxcb = (void *) pscb->SCB_AssocXCB;
	GS_SCB *pscb_pending = pscb->SCB_NextSCBAPSL;
	int result = G_TRUE;

    // Check if the signal is pending
	while (result == G_TRUE && pscb_pending != (struct gs_scb *) 0)
	{
		result = gk_SCBAPSL_UnLink((GS_SCB *) pscb, pscb_pending);
		gk_SCBFL_Link(pscb_pending);
		pscb_pending = pscb->SCB_NextSCBAPSL;
	}

	if (pscb->SCB_NextSCBAPSL == (struct gs_scb *) 0)
	{   // remove root becuase no pending are linked
		switch (pscb->SCBType){
			// Types for KCB
			case 1:
			case 2:
				gk_KCBASL_Unlink((GS_SCB *) pscb);
				pscb->SCBState = G_SCBState_UNLINKED;
				break;

			// Types for ECB
			case 3:
				gk_ECBASL_Unlink((GS_ECB *)pxcb, (GS_SCB *) pscb);
				pscb->SCBState = G_SCBState_UNLINKED;
				break;

			// Types for TCB
			case 4:
				gk_TCBASL_Unlink((GS_TCB *)pxcb, (GS_SCB *) pscb);
				pscb->SCBState = G_SCBState_UNLINKED;
				break;

			// Types for RCB
			case 5:
				gk_RCBASL_Unlink((G_RCB *)pxcb, (GS_SCB *) pscb);
				pscb->SCBState = G_SCBState_UNLINKED;
				break;

			// Types for PCB
			//case 6:
			//	pscb->SCBState = G_SCBState_UNLINKED;
			//	break;

			// Types for LCB
			//case 7:
			//	pscb->SCBState = G_SCBState_UNLINKED;
			//	break;
		}
		gk_SCBFL_Link((GS_SCB *) pscb);
	}
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;    
	return(result);
}

/***********************************************************************************
*********************** USER FUNCTIONS                   ***************************
***********************************************************************************/

/**gu_Convert_Time
 *  \brief 
 *  Convert days,hours, minutes, seconds and miliseconds to system time unit.
 *  \param [in] days    Days
 *  \param [in] hours   Hours
 *  \param [in] minutes Minutes
 *  \param [in] seconds Seconds
 *  \param [in] ms      miliseconds
 *  \return Time in system time units
 *  \todo Define if gt_time should be used for user
 *  \relates Miscellaneous
 */
gt_time  gu_Convert_Time(INT32 days, INT32 hours, INT32 minutes, INT32 seconds, INT32 ms)
{
    /* Compute the total number of periods required   */
    gt_time ticks = (gt_time) ((gt_time)days * (gt_time) 24);
    ticks = (ticks + (gt_time) hours) * (gt_time) 60;
    ticks = (ticks + (gt_time) minutes) * (gt_time) 60;
    ticks = (ticks + (gt_time) seconds) * (gt_time) 1000;
    ticks = (ticks + (gt_time) ms) * ((gt_time) G_TICKS_PER_MSECOND);
    return ((gt_time) ticks);
}

/**gu_get_irq_status
 *  \brief 
 *  Return the status of the system IRQs
 *  \return Status of the system IRQS
 *  \todo Change to return status of one IRQ in order to cover more than a INT32 scope
 *  \relates Miscellaneous
 */
INT32 gu_get_irq_status(void)
{
	INT32 status;
    GRTOS_USER_CRITICAL_SECTION_GET;  
        status = GRTOS_CMD_IRQ_RQS;
	GRTOS_CMD_CRITICAL_SECTION_RELEASE;
	return(status);
}

/**gu_get_now
 *  \brief 
 *  Return the system time in system time units 
 *  \return Return the system time in system time units 
 *  \todo Analise if gu_Clock is more adecuate
 *  \relates Miscellaneous
 */
gt_time gu_get_now(void)
{
	gt_time status;
    GRTOS_USER_CRITICAL_SECTION_GET;
        G_DEBUG_VERBOSE    
        status = (gt_time) GRTOS_now;
	GRTOS_CMD_CRITICAL_SECTION_RELEASE;
	return(status);
}

/**gu_get_frozen_threshold
 *  \brief Returns the value in the frozen threshold register (R_FRZ_TM_THR)
 *  
 *  \return the value in the frozen threshold register (R_FRZ_TM_THR)
 *  
 *  \details FOr use in user mode
 *  \todo
 *  \relates Time
 */
gt_time gu_get_frozen_threshold(void)
{
	gt_time status;
    GRTOS_USER_CRITICAL_SECTION_GET;
        G_DEBUG_VERBOSE    
        status = (gt_time) GRTOS_CMD_FRZ_TM_THR_GET;
	GRTOS_CMD_CRITICAL_SECTION_RELEASE;
	return(status);
}

/**gu_get_mutex_time
 *  \brief 
 *  Return the time the mutex is granted in system time units 
 *  \relates Miscellaneous
 */
gt_time gu_get_mutex_time(void)
{
	gt_time status;
    GRTOS_USER_CRITICAL_SECTION_GET;
        G_DEBUG_VERBOSE    
        status = (gt_time) GRTOS_CMD_MTX_TM_GET;
	GRTOS_CMD_CRITICAL_SECTION_RELEASE;
	return(status);
}

/**gu_Clock
 *  \brief 
 *  Convert to gt_tm structure a time in gt_time format (time in system time units)
 *  \param [in] ticks Time to convert in gt_time format
 *  \return return a gt_tm structure with the time converted
 *  \todo Analise to use gt_tm as time format for user
 *  \relates Miscellaneous
 */
gt_tm gu_Clock(gt_time ticks)
{
	gt_tm return_time;

	lldiv_t division;
	division = lldiv(ticks, (INT64)(G_TICKS_PER_MSECOND)*(INT64)1000*(INT64)60*(INT64)60*(INT64)24*(INT64)365);
	return_time.tm_year  = (int) division.quot;
	division = lldiv((INT64) division.rem, (INT64)(G_TICKS_PER_MSECOND)*(INT64)1000*(INT64)60*(INT64)60*(INT64)24);
	return_time.tm_day  = (int) division.quot;
	division = lldiv((INT64) division.rem, (INT64)(G_TICKS_PER_MSECOND)*(INT64)1000*(INT64)60*(INT64)60);
	return_time.tm_hour  = (int) division.quot;
	division = lldiv((INT64) division.rem, (INT64)(G_TICKS_PER_MSECOND)*(INT64)1000*(INT64)60);
	return_time.tm_min  = (int) division.quot;
	division = lldiv((INT64) division.rem, (INT64)(G_TICKS_PER_MSECOND)*(INT64)1000);
	return_time.tm_sec  = (int) division.quot;
	return_time.tm_msec = (int) ((int)division.rem/(int) G_TICKS_PER_MSECOND);
	return(return_time);
}

/**gk_TIME_CALLBACK
 *  \brief This function is called when a time event happened. It has to resolve according the event type.
 *  \param [in] pevent Pointer to the ECB of the event time happened
 *  \details 
 *  \todo
 *  \relates Time
 */
void gk_TIME_CALLBACK(GS_ECB *pevent)
{
    GS_TCB *ptcb = pevent->ECB_AssocTCB;; 
    
    /* PROCESS ACCORDING TO THE TYPE OF THE EVENT     */
    switch (pevent->ECBType)
    {
        case G_ECBType_OSTimeDly:  /* It is a Delay Event ****************************************************/
            /* Task Should be inserted in REady List if it is not waiting more events */
            if (gk_TASK_IS_BLOCKED(ptcb) == G_FALSE)
            {
                gk_TCBWL_Unlink(ptcb); 
                gk_TCBRDYL_Link(ptcb); 
                gk_TCBAEL_Unlink(pevent);
                gk_ECBFL_Link((GS_ECB *) pevent);
            }
            else
            {
                /* No puede estar bloqueada pues no prodria haber ejecutar el Delay */
                G_DEBUG_WHILEFOREVER;  ///< \todo raise a signal when it is blocked 
            }
            break; 

        case G_ECBType_PERIODIC:  /* It is a Periodic Release of the task */

            gk_TCBAEL_Unlink(pevent);  // Preserve the Periodic event to remove the rest of events for a clean release
            
            gk_TASK_RELEASE((GS_TCB *) ptcb);
            
            /* Set a new event in the future */
            pevent->ECBValue.i64 = (INT64) pevent->ECBValue.i64 + ptcb->TCBPeriod;
            gk_TCBAEL_Link(pevent, ptcb);  // Link the periodic event back to the tcb
            gk_ECBTL_Link(pevent);                 
            break; 

        case G_ECBType_TIMEOUT_SEM_WAITING:
            /* It is the timeout of a waiting event *************************************/
            gk_timeout_ECB_SEM_wait(pevent);
            break; 

        case G_ECBType_TIMEOUT_SEM_GRANTED:
            gk_timeout_ECB_SEM_post(pevent);         
            break; 
            
        case G_ECBType_MESSAGE_SEND_WAIT:
            gk_timeout_ECB_MQ_send(pevent);
            break;
            
        case G_ECBType_MESSAGE_CONSUME_WAIT:
            gk_timeout_ECB_MQ_consume(pevent);                
            break;
            
        case G_ECBType_LASTEST_TIME:
            /* The end of the times */
            G_DEBUG_WHILEFOREVER;
            
        default:
            G_DEBUG_WHILEFOREVER; 
            break; 
    }
}

/**gk_RESOURCE_ECB_KILL_CALLBACK
 *  \brief This function is called when a resource event is kill. It has to resolve according the event type.
 *  \param [in] pevent Pointer to the ECB of the event related
 *  \relates Resource
 */ 
void gk_RESOURCE_ECB_KILL_CALLBACK(GS_ECB *pevent)
{
    switch (pevent->ECBType)
    {
        case G_ECBType_SEM_GRANTED:
        case G_ECBType_TIMEOUT_SEM_GRANTED:  
            gk_SEM_granted_kill((GS_ECB *) pevent);
            break;
            
        case G_ECBType_SEM_WAITING:
        case G_ECBType_TIMEOUT_SEM_WAITING:  
            gk_SEM_waiting_kill((GS_ECB *)pevent);
            break;
            
        case G_ECBType_QUEUE_CONSUME:
            gk_QUEUE_granted_kill((GS_ECB *) pevent);
            break;
        default:
            gk_TCBAEL_Unlink(pevent);
            gk_ECBFL_Link(pevent);
            break;
    }
}

/**gk_FROZEN_CALLBACK
 *  \brief This function is called when a frozzen event happened
 *  \relates Time
 */
void gk_FROZEN_CALLBACK(void)
{
    fprintf(stderr,"[ MESSAGE ] FROZEN REACHED . Proc: %d\n", GRTOS_CMD_PRC_ID);
    // G_DEBUG_WHILEFOREVER;   // Remove when frozen strategy is implemented
}

OPTIMEZE_RESTORE