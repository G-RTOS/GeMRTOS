/**
 *  \file grtosuserfunctions.c
 *  \author Ricardo Cayssials
 *  \brief GRTOS User Interface API
 *  \details This file contains the main API functions without resource specific user functions.
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



#include <stdlib.h>

/**
 * \class Task
A task in GeMRTOS is defined as a unit of execution. The main components of a task implementation are:
\li <b>GS_TCB</b>: this is a control block that stores the information required for task management.
\li <b>task code</b>: this is the executable code, loaded into the system memory, that implements a task function when it is executed by a processor. Task code is implemented a subroutine function.
\li <b>task stack</b>: this is data storage to preserve the status of a suspended task in order to resume it properly and store subroutine data. The task stack memory is reserved when the task is created. The amount of memory assigned to the task stack is defined by default.
 */
 
/**
 * \class List
In GeMRTOS, each task is assigned to a scheduling list and one or more processors may serve that scheduling list. Processors may serve one or more scheduling lists, thereby allowing the predictability of partitioned scheduling with a flexible balance of processor loads. Scheduling overhead is reduced using a timer-tickless scheduler. Floating scheduling reduces the overhead for the highest priority tasks and isolates the design of the system from the number of processors of the hardware architecture. The generic feature allows partial configuring, global and partitioned scheduling. Hybrid partition scheduling are based on scheduling lists which allow assigning tasks and processors to scheduling the different subsystems of the application.
\image html "https://gemrtos.com/images/hibrid_scheduling.gif"
Resources may be shared among tasks from several scheduling lists, as shown in the figure. Each scheduling list may implement a different scheduling priority and task may be migrated from one scheduling list to another scheduling list during runtime. 
Processors may be assigned to several scheduling lists, and their assignment may change at any time during runtime
 */

/**
 * \class Signal
Signals in operating systems handle exceptional conditions that may happen during runtime. Signals allow tasks to execute associated code when a certain condition takes place. Some of these conditions may be defined, for instance, as when a task is created, a resource is granted or a task is blocked. In GeMRTOS, the GS_SCB data structure is defined to store the signal information and it is linked to data structures accordingly. 
The GS_SCB data structure points to the code to be executed when the condition of the signal occurs. This code is executed prior to the execution of the code of the task with which the signal is associated.
In GeMRTOS, there exist different signal types.
 * \todo Define the rest of SCB types
 */

/**
 *  \class Configuration
 *  \class Miscellaneous
 *  \class Validation
 *  \class Resource
 *  \class Event
 *  \class RRDS
 *  \class Interrupt
 *  \class Kernel
 *  \class Processor
 *  \class RRSD
 *  \class Time
 *  \class Monitor
 *  \class Core
 */

OPTIMEZE_CODE(0)

/**
 *  \brief gu_Convert_time returns absolute time from hours, minutes, seconds and ms
 *  
 *  \param hours Number of hours of the absolute time.
 *  \param minutes Number of minutes of the absolute time.
 *  \param seconds Number of seconds of the absolute time.
 *  \param ms Number of mili-seconds of the absolute time.
 *  \return gu_Convert_time returns the absolute time
 *  \remark 
 */
G_INT64 gu_Convert_time(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
	ms = ms * (unsigned int) G_TICKS_PER_MSECOND;
	seconds = seconds * (unsigned int) G_TICKS_PER_SECOND;
	minutes = minutes * (unsigned int) G_TICKS_PER_SECOND * G_SECONDS_IN_MINUTE;
	hours   = hours   * (unsigned int) G_TICKS_PER_SECOND * G_SECONDS_IN_HOUR;
	return ((G_INT64) (G_INT64) hours + (G_INT64) minutes + (G_INT64) seconds + (G_INT64) ms);
}

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
G_INT32 gu_Set_Default_Task_Type(unsigned int type)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        G_TASK_TYPE_DEFAULT = type;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
}


/**
 *  \brief 
 *  Return the default type assigned to new tasks 
 *  \return Current default type
 *  
 *  \details The default type is assigned with gu_Set_Default_Task_Type
 *  \relates Configuration
  */
G_INT32 gu_Get_Default_Task_Type(void)
{
    return (G_TASK_TYPE_DEFAULT);
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
G_INT32 gu_Set_Default_Task_List(GS_LCB *list)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        G_TASK_LCB_DEFAULT = list;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    
    return (G_TRUE);
}

/**
 *  \brief 
 *  Return the current default list number
 *  \return G_INT32 with the default list number
 *  
 *  \details Return the current default list number
 *  \relates Configuration
 */
G_INT32 gu_Get_Default_Task_List(void)
{
    return ((G_INT32) G_TASK_LCB_DEFAULT);
}

/**
 *  \brief 
 *  Set the default priority for task created afterwards. 
 *  Priority is assigned with an G_INT32 level and an G_INT32 priority in such level.
 *  \param [in] level    G_INT32 level
 *  \param [in] priority G_INT32 level
 *  \relates Configuration
 */
void gu_Set_Default_Task_Priority(G_INT32 level, G_INT32 priority)
{
    TIMEPRIORITY pri_aux;
    GRTOS_USER_CRITICAL_SECTION_GET;
        pri_aux.i32[0] = (G_INT32) priority;
        pri_aux.i32[1] = (G_INT32) level;
        G_TASK_PRIORITY_DEFAULT = (G_INT64) pri_aux.i64;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
}


/**gu_Set_Default_Task_Period
 *  \brief 
 *  Set the default periodo for task created afterwards.
 *  \param [in] hours   G_INT32 hours
 *  \param [in] minutes G_INT32 minutes
 *  \param [in] seconds G_INT32 seconds
 *  \param [in] ms      G_INT32 ms
 *  \relates Configuration
 */
void gu_Set_Default_Task_Period(G_INT32 hours, G_INT32 minutes, G_INT32 seconds, G_INT32 ms)
{
    /* Compute the total number of ticks required for the period */
    // G_INT64 ticks;
    GRTOS_USER_CRITICAL_SECTION_GET;
    	G_TASK_PERIOD_DEFAULT = gu_Convert_time(hours, minutes, seconds, ms);
        // ms = ms * (unsigned int) G_TICKS_PER_MSECOND;
        // seconds = seconds * (unsigned int) G_TICKS_PER_MSECOND * 1000;
        // minutes = minutes * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60;
        // hours   = hours   * (unsigned int) G_TICKS_PER_MSECOND * 1000 * 60 * 60;
        // ticks   = (G_INT64) hours + (G_INT64) minutes + (G_INT64) seconds + (G_INT64) ms;
        
        // G_TASK_PERIOD_DEFAULT = ticks;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
G_INT32 gu_SetTaskType(struct gs_tcb *ptcb, unsigned int type)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        ptcb->TCBType = type;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
}

/**gu_SetTaskList
 *  \brief The <b>gu_SetTaskList</b> function assigns a task into a scheduling list. Consequently, the task will be schedule according the priority discipline of the scheduling list.
 *  \param ptcb       It is a pointer to the GS_TCB structure of the task
 *  \param plcb       It is a pointer to the GS_LCB structure of the scheduling list
 *  \return The <b>gu_SetTaskList</b> function returns G_TRUE if successfull, G_FALSE otherwise
 *  \relates List
 *  \remark gu_list_assign_task
 *  \todo Check if index is valid
 *  \todo Unlinks from current list and link to the new link accordingly.
 */
G_INT32 gu_SetTaskList(struct gs_tcb *ptcb, struct gs_lcb *plcb)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        PRINT_ASSERT((gkm_LCB_IsValid(plcb) == G_TRUE),"ERROR LCB is not valid\n");
        PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
        ptcb->TCB_RDY_LCB_Index = plcb;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
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
G_INT32 gu_SetTaskReadyPriority(struct gs_tcb *ptcb, long long priority)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        ptcb->TCBReadyPriority = (G_INT64) priority;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
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
G_INT32 gu_SetTaskRunPriority(struct gs_tcb *ptcb, long long priority)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        ptcb->TCBRunPriority = (G_INT64) priority;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
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
G_INT32 gu_SetTaskDeadline(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    /* Compute the total number of ticks required for the period */
    G_INT64 ticks;
    GRTOS_USER_CRITICAL_SECTION_GET;
        ms = ms * (unsigned int) G_TICKS_PER_MSECOND;
        seconds = seconds * (unsigned int) G_TICKS_PER_SECOND;
        minutes = minutes * (unsigned int) G_TICKS_PER_SECOND * G_SECONDS_IN_MINUTE;
        hours   = hours   * (unsigned int) G_TICKS_PER_SECOND * G_SECONDS_IN_HOUR;
        ticks   = (G_INT64) hours + (G_INT64) minutes + (G_INT64) seconds + (G_INT64) ms;
        ptcb->TCBDeadline = ticks;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
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
G_INT32 gu_SetTaskPeriod(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    /* Compute the total number of ticks required for the period */
    G_INT64 ticks;
    GRTOS_USER_CRITICAL_SECTION_GET;
        ms = ms * (unsigned int) G_TICKS_PER_MSECOND;
        seconds = seconds * (unsigned int) G_TICKS_PER_SECOND ;
        minutes = minutes * (unsigned int) G_TICKS_PER_SECOND * G_SECONDS_IN_MINUTE;
        hours   = hours   * (unsigned int) G_TICKS_PER_SECOND * G_SECONDS_IN_HOUR;
        ticks   = (G_INT64) hours + (G_INT64) minutes + (G_INT64) seconds + (G_INT64) ms;
        ptcb->TCBPeriod = ticks;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
}

/**gu_StartTaskwithOffset
 *  \brief The <b>gu_StartTaskwithOffset</b> function sets a previously created task for execution. The <b>gu_StartTaskwithOffset</b> function allows defingning an offset for the first time execution of the task. This offset helps to schedule the starting time of the tasks. 
 *  \param ptcb    It is a pointer to the GS_TCB structure of the task (returned by the gu_GetTask() function when task was created).
 *  \param hours   Number of hours of the starting offset.
 *  \param minutes Number of minutes of the starting offset.
 *  \param seconds Number of seconds of the starting offset.
 *  \param ms      Number of miliseconds of the starting offset.
 *  \return The <b>gu_StartTaskwithOffset</b> function returns G_TRUE when successful.
 *  \todo Include start of a ISR
 *  \todo Include start of an IDLE task??
 *  \remark gu_task_start
 *  \relates Task
 */
G_INT32 gu_StartTaskwithOffset(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    G_INT64 ticks;

    GRTOS_USER_CRITICAL_SECTION_GET;
        ms = ms * (unsigned int) G_TICKS_PER_MSECOND;
        seconds = seconds * (unsigned int) G_TICKS_PER_SECOND;
        minutes = minutes * (unsigned int) G_TICKS_PER_SECOND * G_SECONDS_IN_MINUTE;
        hours   = hours   * (unsigned int) G_TICKS_PER_SECOND * G_SECONDS_IN_HOUR;
        ticks   = (G_INT64) hours + (G_INT64) minutes + (G_INT64) seconds + (G_INT64) ms;
    
        gk_START_TASK_CALLBACK(ptcb, ticks);

        /* Si está ejecutandose el GRTOS, hago cambio de tarea si corresponde */
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    
    return (G_TRUE);
}

/**gu_SetTaskAbortwhenDeadline
 *  \brief 
 *  Set if task should be aborted when misses a deadline
 *  \param [in] ptcb                Pointer to the TCB of the task
 *  \param [in] abort_when_deadline G_TRUE when task should be aborted when deadline is missed, G_FALSE otherwise
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \relates Task
 */
G_INT32 gu_SetTaskAbortwhenDeadline(struct gs_tcb *ptcb, G_INT32 abort_when_deadline)
{
    if ((abort_when_deadline != G_TRUE) && (abort_when_deadline != G_FALSE)) {return (G_FALSE);}
    GRTOS_USER_CRITICAL_SECTION_GET;
        ptcb->TCB_Abort_w_Deadline = abort_when_deadline;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
}


/**gu_GetTask
 *  \brief The <b>gu_GetTask</b> function creates a task with all the default assignments and returns a pointer to a GS_TCB structure of the new task. All the parameters of the task may be determining by changing the default assignments prior to task creation and most of them may be change after task creation through task related functions. The <b>gu_GetTask</b> function requieres only two parameters: (1) a pointer to the function subroutine that implements the task code (i.e. it is the name of the function), and (2) an argument to be passed each time that the task is invoked.
The task is created with the following default values:
\li Type: defined in the G_TASK_TYPE_DEFAULT parameter, set with the <em>gu_Set_Default_Task_Type()</em> function. After creation, <em>type</em> parameter of the task may be modified with: (1) the <em>gu_trigger_register_task()</em> function for triggerable task, or (2) <em>gu_SetTaskType()</em> function.
\li Ready Priority: defined in the G_TASK_PRIORITY_DEFAULT parameter, set with the <em>gu_Set_Default_Task_Priority()</em> function. After creation, <em>ready priority</em> parameter may be set with the <em>gu_SetTaskReadyPriority()</em> function.
\li Run Priority: defined in the G_TASK_PRIORITY_DEFAULT parameter, set with the <em>gu_Set_Default_Task_Priority()</em> function. After creation, <em>run priority</em> parameter may be set with the <em>gu_SetTaskRunPriority()</em> function.
\li Deadline: defined in the G_TASK_PERIOD_DEFAULT parameter, set with the <em>gu_Set_Default_Task_Period()</em> function. After creation, <em>deadline</em> parameter may be set with the <em>gu_SetTaskDeadline()</em> function.
\li Period: defined in the G_TASK_PERIOD_DEFAULT parameter, set with the <em>gu_Set_Default_Task_Period()</em> function. After creation, <em>period</em> parameter may be set with the <em>gu_SetTaskPeriod()</em> function.
\li Scheduling list: defined in the G_TASK_LCB_DEFAULT parameter, set with the <em>gu_Set_Default_Task_List()</em> function. After creation, <em>scheduling list</em> parameter may be set with the <em>gu_SetTaskList()</em> function.
\li Abort when invoked: set to G_TRUE by default. By default, task is aborted when it is released before completion. After creation, <em>abort when invoked</em> parameter may be set with the <em>gu_SetTaskAbortwhenDeadline()</em> function.
 *  \param TaskCode  it is a pointer to the beginning of the task code. It is the name of the function that implements the task code.
 *  \param p_arg it is a pointer that it is passed to the task function each time it is invoked. It is defined as a "void *" type, but it may be casted for different types inside the task code.
 *  \param format Formatted task_description string for description of the task up to G_TCB_DESCRIPTION_LENGTH character length. The valid format of this task_description is similar to the original printf function, allowing it to contain embedded format tags. These format tags can be replaced by the values specified in subsequent additional arguments and formatted as specified. This allows for flexible and customized task_description formatting within the <b>gu_GetTask</b> function, akin to how the printf function operates.
 *  \return The <b>gu_GetTask</b> function returns a pointer to the GS_TCB structure of the task created. This value should be used to reference all the function called related to the task created.
 *  \remark gu_create_task
 *  \relates Task
 */
void *gu_GetTask(void *TaskCode, void *p_arg, char *format, ...)
{
    GS_TCB  *ptcb;
    
    GRTOS_USER_CRITICAL_SECTION_GET;
	
		const unsigned int BUF_LEN = G_TCB_DESCRIPTION_LENGTH;
		char x[BUF_LEN + 2];
		x[BUF_LEN + 1] = 0;
		
		va_list args;        
		va_start (args, format);
		vsnprintf(x, BUF_LEN, format, args);
		va_end (args);		
		
		ptcb = gk_TCB_GetFree();

		PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB not valid in gu_GetTask\n");
		// Initialize the gs_tcb structure of the task
		if (ptcb != (struct gs_tcb *) 0) {

			ptcb->TCB_TaskCode         = TaskCode;           // Pointer to the begining of the task code
			ptcb->TCB_TaskArg          = p_arg;              // Pointer to the argument of the first call

			ptcb->TCBType              = G_TASK_TYPE_DEFAULT;      // Type of the Task
			ptcb->TCBReadyPriority     = G_TASK_PRIORITY_DEFAULT;  // Initial Priority of the task
			ptcb->TCBRunPriority       = G_TASK_PRIORITY_DEFAULT;  // Priority of the task when it is executing
			ptcb->TCBDeadline          = G_TASK_PERIOD_DEFAULT;    // Deadline of the task
			ptcb->TCBPeriod            = G_TASK_PERIOD_DEFAULT;    // Period of the task
			ptcb->TCB_RDY_LCB_Index    = (GS_LCB *) G_TASK_LCB_DEFAULT; /* number of the ready list that should be inserted */
			ptcb->TCB_Abort_w_Deadline = G_TRUE;   /* True when task is aborted when deadline, false otherwise */

			ptcb->TCBInherPriority     = G_TASK_PRIORITY_DEFAULT;  /* Load task priority into TCB              */
			// strncpy((char *) ptcb->TCB_description, (char *) task_description, G_TCB_DESCRIPTION_LENGTH);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
			strncpy((char *) ptcb->TCB_description, (char *) x, G_TCB_DESCRIPTION_LENGTH-1);
#pragma GCC diagnostic push

			// Initilize the stack of the task
			gk_TASK_STK_Init(ptcb);
		}

		#ifndef __niosX_arch__
			ptcb->TaskThreadHandle = CreateThread(
	            NULL,                         // default security attributes
	            0,                            // use default stack size
				(void *) gu_pthread,          // thread function name
				(void *) ptcb,                // argument to thread function
				CREATE_SUSPENDED,             // use default creation flags
	            NULL);
		#endif

    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    
	return ((void *) ptcb);
}

/**gu_TASK_Sleep
 *  \brief The <b>gu_TASK_Sleep</b> function suspends the execution of the current task for a defined interval. This function may be used inside of the infinite loop of a <em>run-once</em> task, in order to produce a repetitive execution of the code in the loop.
 *  \param hours   Number of hours of the suspension interval.
 *  \param minutes Number of minutes of the suspension interval.
 *  \param seconds Number of seconds of the suspension interval.
 *  \param ms      Number of miliseconds of the suspension interval.
 *  \return The <b>gu_TASK_Sleep</b> function returns G_TRUE.
 *  \remark gu_task_sleep
 *  \relates Task
 */
G_INT32  gu_TASK_Sleep(G_INT32 hours, G_INT32 minutes, G_INT32 seconds, G_INT32 ms)
{
    /* Compute the total number of periods required   */
    G_INT64 ticks;
    
    ms = ms * (G_INT32) G_TICKS_PER_MSECOND;
    seconds = seconds * (G_INT32) G_TICKS_PER_SECOND;
    minutes = minutes * (G_INT32) G_TICKS_PER_SECOND * G_SECONDS_IN_MINUTE;
    hours = hours * (G_INT32) G_TICKS_PER_SECOND * G_SECONDS_IN_HOUR;
    ticks = (G_INT64) hours + (G_INT64) minutes + (G_INT64) seconds + (G_INT64) ms;
    
    if (ticks != 0){
    	gu_TASK_Sleep_Time((G_INT64) ticks);
    }
    return (G_TRUE);
}

/**gu_PCB_GetCurrentTCB
*  \brief The <b>gu_PCB_GetCurrentTCB</b> function returns a pointer to the TCB structure of the current task being executed..
 *  \return The <b>gu_PCB_GetCurrentTCB</b> function returns a pointer to the TCB structure of the task being executed.
 *  \relates Task
 */ 
GS_TCB *gu_PCB_GetCurrentTCB(void)
{
    GS_TCB *ptcb;
    GRTOS_USER_CRITICAL_SECTION_GET;
        ptcb = gk_PCB_GetCurrentTCB();
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (ptcb);
}


/**gu_TASK_Sleep_Time
*  \brief The <b>gu_TASK_Sleep</b> function suspends the execution of the current task for a defined interval in number of time system clock. This function may be used inside of the infinite loop of a <em>run-once</em> task, in order to produce a repetitive execution of the code in the loop.
 *  \param ticks Number of time system clock to sleep the task
 *  \return The <b>gu_TASK_Sleep_Time</b> function returns G_TRUE.
 *  \details It is executed in user mode and gu_TASK_Sleep should be used instead
 *  \todo Check if it can be implemented only with gk_KERNEL_TASK_SUSPEND instead of gk_KERNEL_TASK_SUSPEND_CURRENT
 *  \relates Task
 */ 
G_INT32 gu_TASK_Sleep_Time(gt_time ticks)
{
	GS_TCB  *ptcbcurrent;
	GS_ECB *pevent;

    PRINT_ASSERT((G_Running == G_TRUE), "ERROR: Task sleep called but GeMRTOS is not running");
    GRTOS_USER_CRITICAL_SECTION_GET; /* Entro a la seccion critica del GRTOS Controller */
    
    ptcbcurrent = gk_PCB_GetCurrentTCB(); /* Get the current task */
	pevent = gk_ECB_GetFree();

    PRINT_ASSERT(((gkm_TCB_IsValid(ptcbcurrent) == G_TRUE) && (ptcbcurrent != (struct gs_tcb *) 0)), "ERROR: ptcbcurrent= %p\n", (void *) ptcbcurrent);
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE), "ERROR: pevent= %p\n", (void *) pevent);

	pevent->ECBValue.i64 = GRTOS_now + ticks;

	pevent->ECBType = (G_INT32) G_ECBType_OSTimeDly;
    gk_TCBAEL_Link(pevent, ptcbcurrent);
	gk_ECBTL_Link(pevent);

	gk_KERNEL_TASK_SUSPEND_CURRENT();

	GRTOS_USER_CRITICAL_SECTION_RELEASE; // 04-09-2023 

    /* Retorna fuera de la seccion critica */
    // GRTOS_USER_CRITICAL_SECTION_GET;
    // 
    // gk_TCBAEL_Unlink(pevent);
	// gk_ECBFL_Link((GS_ECB *) pevent);
    // 
	// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
}

/**gu_TASK_Kill
 *  \brief 
 *  The task is eliminated and all the structures involved are return to the free lists.
 *  \param [in] ptcb Pointer to the TCB of the task to be killed
 *  \return G_TRUE
 *  \todo Check that all the structures are adequatelly treated
 *  \relates Task
 */
G_INT32 gu_TASK_Kill(GS_TCB *ptcb)
{
    G_DEBUG_VERBOSE
    GRTOS_USER_CRITICAL_SECTION_GET;
        gk_TASK_Kill((GS_TCB *) ptcb);
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
}

/**gu_TASK_SUSPEND
 *  \brief 
 *  Change to waiting the state of a task
 *  \param [in] ptcb Pointer to the TCB of the task to suspend
 *  \return G_TRUE
 *  \todo Check gk_KERNEL_TASK_SUSPEND with current task
 *  \relates Task
 */
G_INT32 gu_TASK_SUSPEND(GS_TCB *ptcb)
{
	G_DEBUG_VERBOSE
    PRINT_ASSERT((G_Running == G_TRUE), "ERROR task suspend but GeMRTOS is not running");
    GRTOS_USER_CRITICAL_SECTION_GET; // Is called in mutex section
        gk_KERNEL_TASK_SUSPEND((GS_TCB *) ptcb);
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (G_TRUE);
}

/**gu_TASK_IS_BLOCKED
 *  \brief 
 *  Return G_TRUE if task is waiting for a resource, G_FALSE otherwise
 *  \param [in] ptcb Pointer to the TCB of the task
 *  \return G_TRUE if task is waiting for a resource, G_FALSE otherwise
 *  \todo Check correcteness for diferent resources (semaphore and message queue)
 *  \relates Task
 */
G_INT32 gu_TASK_IS_BLOCKED(GS_TCB *ptcb)
{
    G_INT32 value;
	G_DEBUG_VERBOSE
    GRTOS_USER_CRITICAL_SECTION_GET;
        value = gk_TASK_IS_BLOCKED((GS_TCB *) ptcb);
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (value);
}

/**gu_TASK_RESUME
 *  \brief 
 *  Resume a task that it is in waiting state
 *  \param [in] ptcb Pointer to the TCB of the task to be resumed
 *  \return G_TRUE if task was resumed, G_FALSE otherwise
 *  \todo Check that GRTOS is running
 *  \relates Task
 */
G_INT32 gu_TASK_RESUME(GS_TCB *ptcb)
{
	G_INT32 return_value = G_FALSE;
    GRTOS_USER_CRITICAL_SECTION_GET;
        if (GRTOS_TASK_STATE_WAITING(ptcb))	{
            gk_TCB_Unlink(ptcb); PRINT_DEBUG_LINE
            gk_TCBRDYL_Link(ptcb); PRINT_DEBUG_LINE
            return_value = G_TRUE;
        }
        
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
    return (return_value);
}


/***********************************************************************************
*********************** SIGNAL FUNCTIONS                 ***************************
***********************************************************************************/
 
/**gu_signal_create
 *  \brief The <b>gu_signal_create</b> function creates and associates a type signal to a task. The priority given to the signal created determines the execution order when there exist more than a signal code to be executed.
 *  \param Type        It is the signal type to be associated with. It is the name of the signal type, for instance: G_SCBType_TCB_ABORTED.
 *  \param Priority    It is the priority given to the signal create and determines the execution order when more than one signal code has to be executed.
 *  \param pxcb        It is the control structure to link the signal created. This control structure may be resources, processors, tasks, or events.
 *  \param Signal_Code It is a pointer to the function that imlpements the signal code: i.e. the function name of the signal.
 *  \param Signal_Arg  It is the argument to pass to the signal function when it is executed.
 *  \return The <b>gu_signal_create</b> function returns a pointer to the GS_SCB structure created.
 *  \todo Check SCB availability
 *  \todo Include signal for the rest of control blocks
 *  \todo Define signals for different events and resources
 *  \relates Signal
 *  \remark gu_signal_create
 */ 
GS_SCB *gu_signal_create(enum scbtype Type, G_INT32 Priority, void *pxcb, void *Signal_Code, void *Signal_Arg)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        GS_SCB *pscb = gk_SCB_GetFree();
        PRINT_ASSERT((gkm_SCB_IsValid(pscb) == G_TRUE),"ERROR psignal in gu_signal_create\n");
        
        pscb->SCBPriority  = Priority;
        pscb->SCBType      = Type;
        pscb->SCB_TaskCode = Signal_Code;
        pscb->SCB_TaskArg  = (void *) Signal_Arg; //(void *) pscb;
        pscb->SCB_NextSCBAPSL = (struct gs_scb *) 0;
        pscb->SCB_AssocXCB = pxcb;
        switch (Type){
            // Types for KCB
            // case 1:
            // case 2:
            //     gk_KCBASL_Link(pscb);
            //     pscb->SCBState = G_SCBState_IN_KCB;
            //     break;

            // Types for ECB
            // case 3:
            //     gk_ECBASL_Link((GS_ECB *)pxcb, pscb);
            //     pscb->SCBState = G_SCBState_IN_ECB;
            //     break;

            // Types for TCB
            case G_SCBType_TCB_ABORTED:
                gk_TCBASL_Link((GS_TCB *)pxcb, pscb);
                pscb->SCBState = G_SCBState_IN_TCB;
                break;

            // Types for RCB
            // case 5:
            //     gk_RCBASL_Link((G_RCB *)pxcb, pscb);
            //     pscb->SCBState = G_SCBState_IN_RCB;
            //     break;

            // Types for PCB
            //case 6:
            //	pscb->SCBState = G_SCBState_IN_PCB;
            //	break;

            // Types for LCB
            //case 7:
            //	pscb->SCBState = G_SCBState_IN_LCB;
            //	break;
            default:
    			G_DEBUG_WHILEFOREVER;
    			break;
        }
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
G_INT32 gu_signal_destroy(GS_SCB *pscb)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
	// void *pxcb = (void *) pscb->SCB_AssocXCB;
	GS_SCB *pscb_pending = pscb->SCB_NextSCBAPSL;
	G_INT32 result = G_TRUE;

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
			// case 1:
			// case 2:
			// 	gk_KCBASL_Unlink((GS_SCB *) pscb);
			// 	pscb->SCBState = G_SCBState_UNLINKED;
			// 	break;

			// Types for ECB
			// case 3:
			// 	gk_ECBASL_Unlink((GS_ECB *)pxcb, (GS_SCB *) pscb);
			// 	pscb->SCBState = G_SCBState_UNLINKED;
			// 	break;

			// Types for TCB
			// case 4:
			// 	gk_TCBASL_Unlink((GS_TCB *)pxcb, (GS_SCB *) pscb);
			// 	pscb->SCBState = G_SCBState_UNLINKED;
			// 	break;

			// Types for RCB
			// case 5:
			// 	gk_RCBASL_Unlink((G_RCB *)pxcb, (GS_SCB *) pscb);
			// 	pscb->SCBState = G_SCBState_UNLINKED;
			// 	break;

			// Types for PCB
			//case 6:
			//	pscb->SCBState = G_SCBState_UNLINKED;
			//	break;

			// Types for LCB
			//case 7:
			//	pscb->SCBState = G_SCBState_UNLINKED;
			//	break;
            default:
    			G_DEBUG_WHILEFOREVER;
    			break;
		}
		gk_SCBFL_Link((GS_SCB *) pscb);
	}
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
gt_time  gu_Convert_Time(G_INT32 days, G_INT32 hours, G_INT32 minutes, G_INT32 seconds, G_INT32 ms)
{
	const gt_time THOUSAND = 1000;
	const gt_time MINUTES_IN_HOUR = 60;
    /* Compute the total number of periods required   */
    gt_time ticks = (gt_time) ((gt_time)days * (gt_time) G_HOURS_IN_DAY);
    ticks = (ticks + (gt_time) hours) * (gt_time) MINUTES_IN_HOUR;
    ticks = (ticks + (gt_time) minutes) * (gt_time) G_SECONDS_IN_MINUTE;
    ticks = (ticks + (gt_time) seconds) * THOUSAND;
    ticks = (ticks + (gt_time) ms) * ((gt_time) G_TICKS_PER_MSECOND);
    return ((gt_time) ticks);
}

/**gu_get_irq_status
 *  \brief 
 *  Return the status of the system IRQs
 *  \return Status of the system IRQS
 *  \todo Change to return status of one IRQ in order to cover more than a G_INT32 scope
 *  \relates Miscellaneous
 */
G_INT32 gu_get_irq_status(void)
{
	G_INT32 status;
    GRTOS_USER_CRITICAL_SECTION_GET;
        status = GRTOS_CMD_IRQ_RQS;
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
	division = lldiv((long long int) ticks, (long long int) (G_TICKS_PER_SECOND)*(long long int) G_SECONDS_IN_HOUR*(long long int) G_HOURS_IN_DAY*(long long int) G_DAYS_IN_YEAR);
	return_time.tm_year  = (int) division.quot;
	division = lldiv((long long int)  division.rem, (long long int) (G_TICKS_PER_SECOND)*(long long int) G_SECONDS_IN_HOUR*(long long int) G_HOURS_IN_DAY);
	return_time.tm_day  = (int) division.quot;
	division = lldiv((long long int) division.rem, (long long int) (G_TICKS_PER_SECOND)*(long long int) G_SECONDS_IN_HOUR);
	return_time.tm_hour  = (int) division.quot;
	division = lldiv((long long int)  division.rem, (long long int) (G_TICKS_PER_SECOND)*(long long int) G_SECONDS_IN_MINUTE);
	return_time.tm_min  = (int) division.quot;
	division = lldiv((long long int)  division.rem, (long long int) (G_TICKS_PER_SECOND));
	return_time.tm_sec  = (int) division.quot;
	return_time.tm_msec = (int) ((int)division.rem/(int) G_TICKS_PER_MSECOND);
	return(return_time);
}

/**gu_GetCurrentTCB
 *  \brief 
 *  Returns the task TCB that the current processor was executing
 *  \return Pointer to the TCB of the task being executed by the current processor
 *  \relates Task
 *  \sa Processor
 */
GS_TCB *gu_GetCurrentTCB(void)
{
    GS_TCB *ptcb;
    GRTOS_USER_CRITICAL_SECTION_GET;
        ptcb = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
	return(ptcb);
}

/**gu_SetLCB_Exclusion
 *  \brief The <b>gu_SetLCB_Exclusion</b> function lets set the exclusion of a scheduling list. The exclusion of a scheduling list limits the simultaneous tasks in execution state. The exclusion may be used either to implement load balancing strategies of to assure real-time properties during runtime. From a real-time poitn of view, the real-time scheduling of the tasks assigned to a scheduling list may be protected from multiprocessor anomalies if scheduling list exclusion is set to 1. 
 *  Set the exclusion number for the LCB pointed by plcb
 *  \remark gu_list_exclusion
 *  \relates List
 *  \sa Processor
 */
G_INT32 gu_SetLCB_Exclusion(GS_LCB *plcb, G_INT32 exclusion)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        plcb->LCBExclusion = exclusion;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
	return(G_TRUE);
}

/**gu_Get_CPU_ID
 *  \brief 
 *  Returns the ID of the current processor
 *  \return Returns the ID of the current processor
 *  \todo Check if return can be changed to G_INT32
 *  \todo Change to user API files
 *  \relates Processor
 */
G_INT32 gu_Get_CPU_ID(void)
{
    G_INT32 ID;
    GU_SAMPLE_FUNCTION_BEGIN(72);
    GRTOS_USER_CRITICAL_SECTION_GET;
        GK_SAMPLE_FUNCTION_BEGIN(72)
        ID = GRTOS_CMD_PRC_ID;
        GK_SAMPLE_FUNCTION_END(72)
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    GU_SAMPLE_FUNCTION_END(72);    
	return(ID);
}


/**gu_Get_Next_Occurrence_Time
 *  \brief 
 *  Returns the Next Occurrence Time
 *  \return Returns the Next Occurrence Time
 *  \todo Change to user API files
 *  \relates Time
 */
gt_time gu_Get_Next_Occurrence_Time(void)
{
    gt_time time;
    GU_SAMPLE_FUNCTION_BEGIN(72);
    GRTOS_USER_CRITICAL_SECTION_GET;
        GK_SAMPLE_FUNCTION_BEGIN(73)
        time = g_kcb.KCB_NextECBTL->ECBValue.i64;
        GK_SAMPLE_FUNCTION_END(73)
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    GU_SAMPLE_FUNCTION_END(72);    
    return(time);
}



/**gu_GetCurrentPriority
 *  \brief 
 *  Returns the task TCB that the current processor was executing
 *  \return Current priority of the task being executed by the current processor
 *  \relates Task
 *  \sa Processor
 */
G_INT64 gu_GetCurrentPriority(void)
{
    G_INT64 priority;
    GRTOS_USER_CRITICAL_SECTION_GET;
        priority = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCBCurrentPriority;
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
	return(priority);
}

/** gk_CRITICAL_SECTION_HIGHEST_PRIORITY_PROCESSOR_CALLBACK
 *  \brief The gk_CRITICAL_SECTION_HIGHEST_PRIORITY_PROCESSOR_CALLBACK is call when the processor
 *  grants the critical section and may set the highest priority processor for the next 
 *  critical section assignment
 *  \return No value is returned
 *  \remark 
 *  \details More details
 */
void gk_CRITICAL_SECTION_HIGHEST_PRIORITY_PROCESSOR_CALLBACK(void)
{
	GRTOS_CMD_MTX_HGHST_PRC(0);
	// if (GRTOS_CMD_PRC_ID == G_NUMBER_OF_PCB) GRTOS_CMD_MTX_HGHST_PRC(0);
	// else GRTOS_CMD_MTX_HGHST_PRC(GRTOS_CMD_PRC_ID);
}

/**gk_TIME_CALLBACK
 *  \brief This function is called when a time event happened. It has to resolve according the event type.
 *  \param [in] pevent Pointer to the ECB of the event time happened
 *  \details 
 *  \todo
 *  \relates Time
 */
void gk_TIME_CALLBACK(GS_ECB *const pevent)
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
            
            gk_TASK_RELEASE((GS_TCB *) ptcb);  // Initialise the TCB and stack preserving signals
            
            /* Set a new event in the future */
            pevent->ECBValue.i64 = (G_INT64) pevent->ECBValue.i64 + ptcb->TCBPeriod;
            gk_TCBAEL_Link(pevent, ptcb);  // Link the periodic event back to the tcb
            gk_ECBTL_Link(pevent);
            
            gk_TCBRDYL_Link(ptcb); PRINT_DEBUG_LINE
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
            break;
            
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
    GS_ECB *pevent1;
    G_RCB *prcb;
    
    switch (pevent->ECBType)
    {
        case G_ECBType_SEM_GRANTED:
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, pevent->ECBType=%d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID,(int) pevent->ECBType);
            // gk_sem_post(pevent->ECB_AssocRCB, pevent->ECB_AssocTCB);
            prcb = (G_RCB *) pevent->ECB_AssocRCB;
            PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");
            prcb->semaphore.SEM_Current_Count++;
            // gk_ECB_List_Unlink(pevent);
            gk_RCBGEL_Unlink(pevent);
            
            // Check if there exist waiting event
            pevent1 = prcb->RCB_NextRCBWEL;
            if (pevent1 != (GS_ECB *) 0)
            {
                PRINT_ASSERT((gkm_ECB_IsValid(pevent1) == G_TRUE),"ERROR ECB not valid\n");
                prcb->semaphore.SEM_Current_Count--;
                gk_RCBWEL_Unlink(pevent1);
                pevent1->ECBType      = G_ECBType_SEM_GRANTED;         
                gk_RCBGEL_Link((G_RCB *) prcb, pevent1);
                
                // Resume waiting task
                PRINT_ASSERT((gkm_TCB_IsValid(pevent1->ECB_AssocTCB) == G_TRUE),"ERROR TCB not valid\n");
                gk_TCB_Unlink(pevent1->ECB_AssocTCB);
                gk_TCBRDYL_Link(pevent1->ECB_AssocTCB);                
            }            
            
            break;
            
        case G_ECBType_SEM_WAITING:
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, pevent->ECBType=%d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID,(int) pevent->ECBType);        
            // gk_SEM_waiting_kill((GS_ECB *)pevent);
            gk_RCBWEL_Unlink(pevent);
            break;
            
        case G_ECBType_QUEUE_CONSUME:
        case G_ECBType_MESSAGE_CONSUME_WAIT:
            if (pevent->ECB_RRDS->queue_buffer.rrds_consumer.pproducer != (GS_ECB *) 0) {
                PRINT_ASSERT((gkm_ECB_IsValid(pevent->ECB_RRDS->queue_buffer.rrds_consumer.pproducer) == G_TRUE),"ERROR ECB is not valid\n");
                pevent->ECB_RRDS->queue_buffer.rrds_producer.consumers--;
                if (pevent->ECB_RRDS->queue_buffer.rrds_producer.deliveries >= pevent->ECB_RRDS->queue_buffer.rrds_producer.consumers) {
                    // Message was delivered and task should be resumed               
                    pevent->ECBType = G_ECBType_MESSAGE_PRODUCE_DELIVERED;
                    gk_TCB_Unlink(pevent->ECB_AssocTCB);
                    gk_TCBRDYL_Link(pevent->ECB_AssocTCB);              
                }
            }
            if (pevent->ECB_RRDS != (struct gs_rrds *) 0) {
                gk_RRDSFL_Link(pevent->ECB_RRDS);
                gk_ECB_List_Unlink(pevent);
            }
            gk_ECB_List_Unlink((GS_ECB *) pevent);
            gk_TCBAEL_Unlink(pevent);
            gk_ECBFL_Link((GS_ECB *)pevent);
            break;
            
        case G_ECBType_MESSAGE_PRODUCE_DELIVERED:        
        case G_ECBType_MESSAGE_PRODUCE_WAIT:
            gk_queue_kill_producer(pevent);
            break;
        
        case G_ECBType_MESSAGE_SEND_WAIT:
        case G_ECBType_MESSAGE_SEND_EXPIRED:
        case G_ECBType_MESSAGE_CONSUME_EXPIRED:
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, pevent->ECBType=%d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID,(int) pevent->ECBType);
            gk_ECB_List_Unlink(pevent);
            gk_TCBAEL_Unlink(pevent);
            gk_ECBFL_Link(pevent);
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
    // gu_fprintf("FROZEN Proc: %d\n", GRTOS_CMD_PRC_ID);
	printf("FROZEN Proc: %d\n", GRTOS_CMD_PRC_ID);
}


/**gk_TASK_PRIORITY_SET_CALLBACK
 *  \brief 
 *  Computes the current priority of the task
 *  \param [in] ptcb       Pointer to the TCB
 *  \param [in] task_state State of the task (G_TCBState_READY, G_TCBState_RUNNING)
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if state is valid
 *  \relates Task
 */
G_INT32 gk_TASK_PRIORITY_SET_CALLBACK(GS_TCB *ptcb, G_INT32 task_state)
{
    GS_ECB *pevent;
    GK_SAMPLE_FUNCTION_BEGIN(10011)

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid, PTCB= %p\n", (void *) ptcb);
    
    G_INT32 lcbtype = get_pointed_field(get_pointed_field(ptcb, TCB_RDY_LCB_Index), LCBType);

    switch (lcbtype)
    {
        case GS_LCBType_EDF:
            if (get_pointed_field(ptcb, TCBType) == G_TCBType_PERIODIC)
            {
                /* Get the next occurrence time associated with the task */
                pevent = get_pointed_field(ptcb, TCB_NextTCBAEL);
                while (pevent != (GS_ECB *) 0)
                {
                    PRINT_ASSERT((gkm_ECB_IsValid((GS_ECB *) pevent) == G_TRUE),"ERROR ECB is not valid, pevent= %p\n", (void *) pevent);
                    if (get_pointed_field(pevent, ECBType) == G_ECBType_PERIODIC)
                    {
                        set_pointed_field(ptcb, TCBCurrentPriority, pevent->ECBValue.i64);
                        break;
                    }
                    pevent = (GS_ECB *) get_pointed_field(pevent, ECB_NextTCBAEL);
                }
                PRINT_ASSERT(((GS_ECB *) pevent != (GS_ECB *) 0),"ERROR G_ECBType_PERIODIC not found\n");
            }
            else
            {
                switch (task_state)
                {
                    case G_TCBState_READY:   /* Insert in Ready list */
                        set_pointed_field(ptcb, TCBCurrentPriority, ptcb->TCBReadyPriority);
                        break;
                    case G_TCBState_RUNNING: /* Insert in Running list */
                        set_pointed_field(ptcb, TCBCurrentPriority, ptcb->TCBRunPriority);
                        break;
                    default:
            			G_DEBUG_WHILEFOREVER;
            			break;
                }                
            }
            break;

        case GS_LCBType_UNSPECIFIED:            
        case GS_LCBType_FP:
            switch (task_state)
            {
                case G_TCBState_READY:   /* Insert in Ready list */
                    set_pointed_field(ptcb, TCBCurrentPriority, ptcb->TCBReadyPriority);
                    break;
                case G_TCBState_RUNNING: /* Insert in Running list */
                    set_pointed_field(ptcb, TCBCurrentPriority, ptcb->TCBRunPriority);
                    break;
                default:
        			G_DEBUG_WHILEFOREVER;
        			break;
            }             
            break;
		default:
			G_DEBUG_WHILEFOREVER;
			break;
    }

	// if (ptcb->TCBReadyPriority < ptcb->TCBInherPriority) {ptcb->TCBCurrentPriority = ptcb->TCBReadyPriority;}
	// else {ptcb->TCBCurrentPriority = ptcb->TCBInherPriority;}
	// if (task_state == G_TCBState_RUNNING && ptcb->TCBCurrentPriority > ptcb->TCBRunPriority) {
	// 	ptcb->TCBCurrentPriority = ptcb->TCBRunPriority;}
    GK_SAMPLE_FUNCTION_END(10011)
    return(G_TRUE);
}


G_INT32 gk_START_TASK_CALLBACK(GS_TCB *ptcb, G_INT64 ticks_offset)
{
    GS_ECB *pevent;
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE), "ERROR: ptcb= %p\n", (void *) ptcb);
    PRINT_ASSERT((ptcb->TCBType != G_TCBType_ISR), "ERROR: ptcb= %p\n", (void *) ptcb);
    
    switch (ptcb->TCBType){
        case G_TCBType_UCOS:
            gk_TCBRDYL_Link(ptcb);  /* Insert Task in Ready List                */
            break;
        case G_TCBType_PERIODIC:
            /* Get the starting time of the task depending whether the RTOS is running or not */
            if (G_Running != G_FALSE) ticks_offset = GRTOS_now + ticks_offset;
            
            pevent = gk_ECB_GetFree();
            pevent->ECBValue.i64 = (G_INT64) ticks_offset;
            pevent->ECBType = (G_INT32) G_ECBType_PERIODIC;
            gk_TCBAEL_Link(pevent, ptcb);
            if (ticks_offset == (G_INT64) 0){ /*TAsk is ready and event is next event */
                pevent->ECBValue.i64 = (G_INT64)  ptcb->TCBPeriod;
            }
            else /* is waiting and next event is the offset */
            {
                pevent->ECBValue.i64 = (G_INT64) ticks_offset;
            }
            gk_ECBTL_Link(pevent);
            if (ticks_offset == (G_INT64) 0){
                /* Task Ready and Set next Release in Period Time from start time */
                gk_TCBRDYL_Link(ptcb);  /* Insert Task in Ready List                */
            }
            else
            {
                gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);
            }
            break;
            
        // case G_TCBType_ISR:
        //     pevent = gk_ECB_GetFree();
        //     pevent->ECBValue.i64 = (G_INT64) 1;
        //     gk_TCBAEL_Link(pevent, ptcb);        
        //     gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);
        //     
        //     // Link to ISR linked list
        //     gk_ECBISRL_Link(pevent, ptcb);
        //     break;
        
        // case G_TCBType_IDLE:
        //     gk_TCBRDYL_Link(ptcb);  /* Insert Task in Ready List                */
        //     break;
        default:
            G_DEBUG_WHILEFOREVER;
            break;
    }
    return(G_TRUE);
}



OPTIMEZE_RESTORE
