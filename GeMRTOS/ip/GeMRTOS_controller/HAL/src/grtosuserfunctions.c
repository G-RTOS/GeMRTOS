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

/**
 * \class Task
A task in GeMRTOS is defined as a unit of execution. The main components of a task implementation are:
\li <b>GS_TCB</b>: this is a control block that stores the information required for task management.
\li <b>task code</b>: this is the executable code, loaded into the system memory, that implements a task function when it is executed by a processor. Task code is implemented a subroutine function.
\li <b>task stack</b>: this is data storage to preserve the status of a suspended task in order to resume it properly and store subroutine data. The task stack memory is reserved when the task is created. The amount of memory assigned to the task stack is defined by default.
 * \exception sec_fun_intro The Task category in GeMRTOS includes essential functions and macros for creating, managing, and scheduling tasks within the real-time operating system. This category is fundamental for implementing multitasking, allowing applications to perform multiple operations concurrently and efficiently utilize system resources. <br /> By leveraging the Task category, developers can create tasks with specified priority levels, resource requirements, and execution parameters, enabling fine control over how tasks are executed and scheduled. Functions within this category support a wide range of operations, including task creation, and suspension, as well as priority management.  <br /> The flexibility offered by the Task category supports responsive applications that can adapt to dynamic conditions in real-time environments. The Task category is crucial for building robust, efficient, and responsive applications in the GeMRTOS ecosystem, facilitating the seamless management of concurrent operations in complex real-time systems.
 */
 
/**
 * \class Scheduling_List
In GeMRTOS, each task is assigned to a scheduling list and one or more processors may serve that scheduling list. Processors may serve one or more scheduling lists, thereby allowing the predictability of partitioned scheduling with a flexible balance of processor loads. Scheduling overhead is reduced using a timer-tickless scheduler. Floating scheduling reduces the overhead for the highest priority tasks and isolates the design of the system from the number of processors of the hardware architecture. The generic feature allows partial configuring, global and partitioned scheduling. Hybrid partition scheduling are based on scheduling lists which allow assigning tasks and processors to scheduling the different subsystems of the application.
\image html "https://gemrtos.com/images/hibrid_scheduling.gif"
Resources may be shared among tasks from several scheduling lists, as shown in the figure. Each scheduling list may implement a different scheduling priority and task may be migrated from one scheduling list to another scheduling list during runtime. 
Processors may be assigned to several scheduling lists, and their assignment may change at any time during runtime.
 *  \exception sec_fun_intro The Scheduling List category in GeMRTOS encompasses functions and macros designed to facilitate the management and manipulation of hybrid scheduling lists within the real-time operating system. These tools provide essential data structure capabilities that allow developers to create, modify, and traverse collections of tasks efficiently, enabling dynamic and flexible scheduling approaches. <br /> By using the Scheduling List category functions, developers can implement effective algorithms for task prioritization, resource allocation, and event handling, all while maintaining high performance and minimal overhead. The functions within this category support various operations such as adding and removing tasks, adjusting priorities, and specifying scheduling criteria. Notably, the configuration of scheduling list exclusions can help prevent real-time anomalies, ensuring that critical tasks receive the attention they need while balancing processor loads effectively. <br /> Integration of the Scheduling List category into real-time applications enhances task organization and scheduling efficiency, enabling the system to respond rapidly to changes in workload and processor availability. This capability is critical for applications where timing, responsiveness, and resource management are paramount.
 */

/**
 * \class Signal
Signals in operating systems handle exceptional conditions that may happen during runtime. Signals allow tasks to execute associated code when a certain condition takes place. Some of these conditions may be defined, for instance, as when a task is created, a resource is granted or a task is blocked. In GeMRTOS, the GS_SCB data structure is defined to store the signal information and it is linked to data structures accordingly. 
The GS_SCB data structure points to the code to be executed when the condition of the signal occurs. This code is executed prior to the execution of the code of the task with which the signal is associated.
In GeMRTOS, there exist different signal types.
 * \exception sec_fun_intro The Signal category in GeMRTOS provides essential functions and macros for implementing event-driven synchronization mechanisms between tasks within the real-time operating system. Signals serve as lightweight notification tools that allow tasks to communicate important state changes, alerts, or operational events efficiently. <br /> By utilizing the Signal category, developers can create and manage signals that facilitate asynchronous task coordination, enabling tasks to respond promptly to specific events without polling or constant checking. Functions within this category support operations such as signal creation, allowing tasks to seamlessly be notified when critical actions need to take place.
 * \todo Define the rest of SCB types
 */

/**
 *  \class Frozen_Mode
 *  \exception sec_fun_intro The Frozen Mode category in GeMRTOS includes functions and macros that manage system behavior during critical operational states. This mode allows the system to temporarily suspend certain tasks and processes to preserve resources and maintain stability under specific conditions. By entering Frozen Mode, developers can prevent unwanted interruptions and manage timing more effectively, particularly in scenarios that require precise timing or resource allocation. Functions related to Frozen Mode enable the configuration of thresholds and control mechanisms, allowing for efficient activation and deactivation of this mode as needed. This capability is essential for optimizing performance and ensuring system reliability in real-time applications.
 */

/**
 *  \class Critical_Section
 *  \exception sec_fun_intro The Critical Section category in GeMRTOS provides essential macros for protecting shared data structures during concurrent task execution. These macros are specifically designed to manage access to GeMRTOS data structures, ensuring that operations on shared resources are executed atomically to prevent race conditions and maintain data integrity. Proper implementation of these critical section macros is crucial for protecting GeMRTOS-specific data structures from unexpected behaviors that may arise from simultaneous access by multiple tasks or processors. This category empowers developers to create secure and efficient real-time applications by ensuring that critical operations on data structures are performed without interruption or conflict.
 */

/**
 *  \class IRQ_Management
 *  \exception sec_fun_intro The IRQ Management category in GeMRTOS encompasses functions and macros designed to handle interrupt requests (IRQs) efficiently within the real-time operating system. This category provides essential tools for enabling, disabling, and managing interrupts, allowing tasks to respond promptly to external events and system signals. Effective IRQ management is crucial for optimizing system performance, as it facilitates the prioritization of tasks and ensures that critical events are addressed in a timely manner.
 */

/**
 *  \class Processor
 *  \exception sec_fun_intro The Processor category in GeMRTOS includes functions and macros that provide critical tools for managing processor-level operations and configurations within the real-time operating system. This category facilitates the control of individual processors, allowing developers to optimize task scheduling, interrupt handling, and overall system performance. Functions in this category enable manipulation of processor states, including halting, resuming, and managing processor interrupts, as well as retrieving processor-specific information. The Processor category is essential for developing robust real-time applications that require precise control over processing resources, promoting responsiveness, and achieving effective synchronization between tasks and hardware components.
 */
 
/**
 *  \class System
 *  \exception sec_fun_intro The System category in GeMRTOS encompasses critical functions and macros that provide core capabilities for managing and configuring the operating environment. This category is vital for overseeing system-level operations, resource management, and overall application behavior within the real-time operating system. <br /> By utilizing the System category, developers can access functions that facilitate system initialization, configuration of kernel parameters, and management of system states. 
 */
 
 
/**
 *  \class Configuration
 *  \class Validation
 *  \class Resource
 *  \class Event
 *  \class RRDS
 *  \class Interrupt
 *  \class Kernel
 *  \class RRSD
 *  \class Time
 *  \class Monitor
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
G_INT64 gu_Convert_time(unsigned int hours,
		                unsigned int minutes,
						unsigned int seconds,
						unsigned int ms)
{
	G_INT64 temp_aux;

	temp_aux = (G_INT64) ms * (G_INT64) G_TICKS_PER_MSECOND;
	temp_aux = temp_aux + (G_INT64) seconds * (G_INT64) G_TICKS_PER_SECOND;
	temp_aux = temp_aux + (G_INT64) minutes * (G_INT64) (G_TICKS_PER_SECOND * G_SECONDS_IN_MINUTE);
	temp_aux = temp_aux + (G_INT64) hours * (G_INT64) G_TICKS_PER_SECOND * (G_INT64) G_SECONDS_IN_HOUR;
	return (temp_aux);
}

/***********************************************************************************
 * *********************************************************************************
 *                    gRTOS Default Setting functions
 * *********************************************************************************
 ***********************************************************************************/

/***********************************************************************************
*********************** TASK SYSTEM CALLS            ***************************
***********************************************************************************/
/**gu_TaskTypeSet
 *  \brief The gu_TaskTypeSet function sets the type of a task.  The valid task types are G_TCBType_PERIODIC and G_TCBType_OneShot.
 *  \exception par_following The function takes these parameters:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task whose type is to be modified.
 *  \param [in] type The type must be either G_TCBType_PERIODIC or G_TCBType_OneShot. Any other value will result in failure.
 *  \return The gu_TaskTypeSet function returns G_TRUE if the task type was successfully set, and G_FALSE otherwise.
 *  \sa gu_TaskCreate, gu_TaskRunPrioritySet, gu_TaskReadyPrioritySet, gu_TaskPeriodSet
 *  \todo Validate type previous to assign
 *  \relates Task
 */ 
G_INT32 gu_TaskTypeSet(struct gs_tcb *ptcb, enum tcbtype type)
{
    gm_GeMRTOSCriticalSectionEnter;
        ptcb->TCBType = type;
    gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}

/**gu_SchedulingListAssociateTask
 *  \brief The gu_SchedulingListAssociateTask function assigns a task to a specific scheduling list.  Once assigned, the task will be scheduled according to the priority discipline defined for that scheduling list.
 *  \exception par_following The function accepts two parameters:
 *  \param [in] ptcb A pointer to the GS_TCB structure representing the task to be assigned.
 *  \param [in] plcb A pointer to the GS_LCB structure representing the scheduling list to which the task should be added.
 *  \return The gu_SchedulingListAssociateTask function returns G_TRUE if the task was successfully assigned to the scheduling list, and G_FALSE otherwise.
 *  \sa gu_SchedulingListCreate, gu_SchedulingListAssociateProcessor
 *  \relates Scheduling_List
 *  \remark gu_list_assign_task
 *  \todo Check if index is valid
 *  \todo Unlinks from current list and link to the new link accordingly.
 */
G_INT32 gu_SchedulingListAssociateTask(struct gs_tcb *ptcb, struct gs_lcb *plcb)
{
    gm_GeMRTOSCriticalSectionEnter;
        PRINT_ASSERT((gkm_LCB_IsValid(plcb) == G_TRUE),"ERROR LCB is not valid\n");
        PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
        ptcb->TCB_RDY_LCB_Index = plcb;
    gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}

/**gu_TaskReadyPrioritySet
 *  \brief The gu_TaskReadyPrioritySet function sets the ready priority of a task.  This priority determines the task's position in the ready queue and influences its scheduling order.  Note that larger values of priority represent lower priority; smaller values indicate higher priority.
 *  \exception par_following The function takes two parameters:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task whose ready priority is to be set.
 *  \param [in] priority A G_INT64 value representing the new ready priority for the task. Larger values indicate lower priority; smaller values indicate higher priority.
 *  \return The gu_TaskReadyPrioritySet function always returns G_TRUE.
 *  \sa gu_TaskRunPrioritySet, gu_TaskTypeSet, gu_TaskPeriodSet, gu_TaskSuspend, gu_TaskResume
 *  \todo Unlink and link if task is ready
 *  \todo Put priority as level and priority
 *  \relates Task
 */
G_INT32 gu_TaskReadyPrioritySet(struct gs_tcb *ptcb, G_INT64 priority)
{
    gm_GeMRTOSCriticalSectionEnter;
        set_pointed_field(ptcb, TCBReadyPriority, (G_INT64) priority);
    gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}

/**gu_TaskRunPrioritySet
 *  \brief The gu_TaskRunPrioritySet function sets the run-time priority of a task. This priority determines the task's execution order when it is running.  Larger values of priority represent lower priority; smaller values represent higher priority.
 *  \exception par_following The function takes these parameters:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task whose run-time priority is to be set.
 *  \param [in] priority A G_INT64 value specifying the new run-time priority. Larger values mean lower priority, and smaller values mean higher priority.
 *  \return The gu_TaskRunPrioritySet function always returns G_TRUE.
 *  \sa gu_TaskReadyPrioritySet, gu_TaskTypeSet, gu_TaskPeriodSet, gu_TaskSuspend, gu_TaskResume
 *  \todo Unlink and link if task is running
 *  \todo Put priority as level and priority
 *  \relates Task
 */
G_INT32 gu_TaskRunPrioritySet(struct gs_tcb *ptcb, G_INT64 priority)
{
    gm_GeMRTOSCriticalSectionEnter;
        set_pointed_field(ptcb, TCBRunPriority, (G_INT64) priority);
    gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}

/**gu_TaskPeriodSet
 *  \brief The gu_TaskPeriodSet function sets the period for the next invocation of a task. The current task invocation period remains unaffected; the new period will apply only to subsequent invocations.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task whose period is to be set.
 *  \param [in] hours The number of hours in the new period.
 *  \param [in] minutes The number of minutes in the new period.
 *  \param [in] seconds The number of seconds in the new period.
 *  \param [in] ms The number of milliseconds in the new period.
 *  \return The gu_TaskPeriodSet function always returns G_TRUE.
 *  \sa gu_TaskStartWithOffset, gu_TaskTypeSet, gu_TaskRunPrioritySet, gu_TaskReadyPrioritySet
 *  \relates Task
 */
G_INT32 gu_TaskPeriodSet(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    /* Compute the total number of ticks required for the period */
    gm_GeMRTOSCriticalSectionEnter;
		set_pointed_field(ptcb, TCBPeriod, gu_Convert_time(hours, minutes, seconds, ms));
		PRINT_ASSERT((ptcb->TCBPeriod > 0), "ERROR in task period");
    gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}

/**gu_TaskStartWithOffset
 *  \brief The gu_TaskStartWithOffset function starts a previously created task for execution, allowing the specification of a time offset for the task's first execution. This offset determines when the task will begin running relative to the time the function is called.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task to be started (obtained from gu_TaskCreate during task creation).
 *  \param [in] hours The number of hours in the starting offset.
 *  \param [in] minutes The number of minutes in the starting offset.
 *  \param [in] seconds The number of seconds in the starting offset.
 *  \param [in] ms The number of milliseconds in the starting offset.
 *  \return The gu_TaskStartWithOffset function returns G_TRUE upon successful task startup.
 *  \sa gu_TaskCreate, gu_TaskPeriodSet, gu_TaskRunPrioritySet, gu_TaskReadyPrioritySet, gu_TaskSuspend
 *  \todo Include start of a ISR
 *  \remark gu_task_start
 *  \relates Task
 */
G_INT32 gu_TaskStartWithOffset(GS_TCB *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    G_INT64 ticks = gu_Convert_time(hours, minutes, seconds, ms);

    gm_GeMRTOSCriticalSectionEnter;
		gk_START_TASK_CALLBACK(ptcb, ticks);
    gm_GeMRTOSCriticalSectionExit;
    
    return (G_TRUE);
}


/**gu_TaskCreate
 *  \brief The gu_TaskCreate function creates a task with default settings and returns a pointer to its GS_TCB structure.  Task parameters can be modified before creation by adjusting default settings or after creation using task-related functions. While the function requires only TaskCode and p_arg, it allows for optional task description formatting using a printf-style format string and arguments.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] TaskCode A pointer to the function that implements the task's code (the task's entry point). It is the name of the function that implements the task code.
 *  \param [in] p_arg A pointer to an argument that will be passed to the TaskCode function each time the task is invoked. This is a void * and can be cast to other types within the task code.
 *  \param [in] format A format string, similar to printf, used to create a description string for the task (up to G_TCB_DESCRIPTION_LENGTH characters). This string can contain format specifiers that are replaced by subsequent arguments.
 *  \return The gu_TaskCreate function returns a pointer to the GS_TCB structure of the newly created task.  This pointer should be used in all subsequent calls related to that task. A NULL return indicates task creation failure.
 *  \sa gu_TaskGetCurrentTCB, gu_TaskTypeSet, gu_TaskStartWithOffset, gu_TaskKill
 *  \remark gu_create_task
 *  \relates Task
 */
void *gu_TaskCreate(void *TaskCode, void *p_arg, char *format, ...)
{
    GS_TCB  *ptcb;
    
    gm_GeMRTOSCriticalSectionEnter;
	
		char x[G_TCB_DESCRIPTION_LENGTH + 4];
		x[G_TCB_DESCRIPTION_LENGTH + 3] = '\0';
		
		va_list args;        
		va_start (args, format);
		vsnprintf(x, G_TCB_DESCRIPTION_LENGTH, format, args);
		va_end (args);		
		
		ptcb = gk_TCB_GetFree((unsigned int) G_TASK_STACKSIZE);

		PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB not valid in gu_TaskCreate\n");
		
		// Initialize the gs_tcb structure of the task
		if (ptcb != (struct gs_tcb *) 0) {

			ptcb->TCB_TaskCode         = TaskCode;           // Pointer to the begining of the task code
			ptcb->TCB_TaskArg          = p_arg;              // Pointer to the argument of the first call

			ptcb->TCBType              = G_TASK_TYPE_DEFAULT;      // Type of the Task
			ptcb->TCBReadyPriority     = G_TASK_PRIORITY_DEFAULT;  // Initial Priority of the task
			ptcb->TCBRunPriority       = G_TASK_PRIORITY_DEFAULT;  // Priority of the task when it is executing
			ptcb->TCBPeriod            = G_TASK_PERIOD_DEFAULT;    // Period of the task
			ptcb->TCB_RDY_LCB_Index    = (GS_LCB *) G_TASK_LCB_DEFAULT; /* number of the ready list that should be inserted */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
			strncpy((char *) ptcb->TCB_description, (char *) x, G_TCB_DESCRIPTION_LENGTH-1);
#pragma GCC diagnostic push

#ifdef __niosX_arch__
			// Create and initialize the impure structure for this task
			// 13-07-2024 ptcb->_impure_ptr = malloc(sizeof(struct _reent)); // _sbrk_r(0, sizeof(struct _reent));
			// 13-07-2024 PRINT_ASSERT(((unsigned int) ptcb->_impure_ptr % 4 == 0), "ERROR memory is not alligned");
			// 13-07-2024 PRINT_ASSERT(((unsigned int) ptcb->_impure_ptr != 0), "ERROR no memory available");
			// 13-07-2024 // 08-07-2024 _REENT_INIT_PTR ((struct _reent*) ptcb->_impure_ptr);
			// 13-07-2024 memcpy((void *) ptcb->_impure_ptr, (void *) _impure_ptr, sizeof(struct _reent));
#endif
			// Initilize the stack of the task
			gk_TASK_STK_Init(ptcb);
		}

    gm_GeMRTOSCriticalSectionExit;
    
	return ((void *) ptcb);
}

/**gu_TaskDelay
 *  \brief The gu_TaskDelay function suspends the execution of the currently running task for a specified time interval. This function is useful within the infinite loop of a task to create periodic behavior.
 *  \exception par_following The function uses the following parameters to define the sleep interval:
 *  \param [in] hours The number of hours to sleep
 *  \param [in] minutes The number of minutes to sleep.
 *  \param [in] seconds The number of seconds to sleep.
 *  \param [in] ms The number of milliseconds to sleep.
 *  \return The gu_TaskDelay function always returns G_TRUE.
 *  \sa gu_TaskDelayTime, gu_TaskStartWithOffset, gu_TaskPeriodSet, gu_TaskSuspend, gu_TaskResume
 *  \remark gu_task_sleep
 *  \relates Task
 */
G_INT32  gu_TaskDelay(G_INT32 hours, G_INT32 minutes, G_INT32 seconds, G_INT32 ms)
{
    /* Compute the total number of periods required   */
    G_INT64 ticks = gu_Convert_time(hours, minutes, seconds, ms);
    if (ticks > 0){
    	gu_TaskDelayTime((G_INT64) ticks);
    }
    return (G_TRUE);
}

/**gu_TaskGetCurrentTCB
 *  \brief The gu_TaskGetCurrentTCB function retrieves a pointer to the Task Control Block (GS_TCB) of the currently executing task.
 *  \exception par_following This function takes no parameters.
 *  \return The gu_TaskGetCurrentTCB function returns a pointer to the GS_TCB structure of the currently running task.
 *  \sa gu_TaskCreate, gu_TASK_IS_BLOCKED, gu_TaskRunPrioritySet, gu_TaskReadyPrioritySet
 *  \relates Task
 */ 
GS_TCB *gu_TaskGetCurrentTCB(void)
{
    GS_TCB *ptcb;
    gm_GeMRTOSCriticalSectionEnter;
        ptcb = gk_PCB_GetCurrentTCB();
    gm_GeMRTOSCriticalSectionExit;
    return (ptcb);
}


/**gu_TaskDelayTime
 *  \brief The gu_TaskDelayTime function suspends the execution of the current task for a specified number of system clock ticks.  This function provides a more direct way to specify sleep duration compared to gu_TaskDelay, using the system's time units directly.
 *  \exception par_following The function takes one parameter:
 *  \param [in] ticks The number of system clock ticks for which the task should sleep.
 *  \return The gu_TaskDelayTime function always returns G_TRUE.
 *  \sa gu_TaskDelay, gu_TaskStartWithOffset, gu_TaskPeriodSet, gu_TaskRunPrioritySet, gu_TaskReadyPrioritySet
 *  \todo Check if it can be implemented only with gk_KERNEL_TASK_SUSPEND instead of gk_KERNEL_TASK_SUSPEND_CURRENT
 *  \relates Task
 */ 
G_INT32 gu_TaskDelayTime(gt_time ticks)
{
	GS_TCB  *ptcbcurrent;
	GS_ECB *pevent;

    PRINT_ASSERT((G_Running == G_TRUE), "ERROR: Task sleep called but GeMRTOS is not running");
    gm_GeMRTOSCriticalSectionEnter; /* Entro a la seccion critica del GRTOS Controller */
    
    ptcbcurrent = gk_PCB_GetCurrentTCB(); /* Get the current task */
	pevent = gk_ECB_GetFree();

    PRINT_ASSERT(((gkm_TCB_IsValid(ptcbcurrent) == G_TRUE) && (ptcbcurrent != (struct gs_tcb *) 0)), "ERROR: ptcbcurrent= %p\n", (void *) ptcbcurrent);
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE), "ERROR: pevent= %p\n", (void *) pevent);

	set_pointed_field(pevent, ECBValue.i64, gm_SystemTimeGet + ticks);
	set_pointed_field(pevent, ECBType,      (G_INT32) G_ECBType_OSTimeDly);
	
    gk_TCBAEL_Link(pevent, ptcbcurrent);
	gk_ECBTL_Link(pevent);

	gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);

	gm_GeMRTOSCriticalSectionExit; 

    return (G_TRUE);
}

/**gu_TaskKill
 *  \brief The gu_TaskKill function terminates a task and releases all associated resources, returning them to the free lists.
 *  \exception par_following The function takes one parameter:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task to be terminated.
 *  \return The gu_TaskKill function always returns G_TRUE.
 *  \sa gu_TaskSuspend, gu_TaskResume, gu_TaskCreate, gu_TASK_IS_BLOCKED
 *  \todo Check that all the structures are adequatelly treated
 *  \relates Task
 */
G_INT32 gu_TaskKill(GS_TCB *ptcb)
{
    gm_GeMRTOSCriticalSectionEnter;
        gk_TASK_Kill((GS_TCB *) ptcb);
	gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}

/**gu_TaskSuspend
 *  \brief The gu_TaskSuspend function suspends a task, changing its state to waiting.
 *  \exception par_following The function takes one parameter:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task to be suspended.
 *  \return The gu_TaskSuspend function always returns G_TRUE. 
 *  \sa gu_TaskResume, gu_TASK_IS_BLOCKED, gu_TaskKill, gu_TaskDelay
 *  \todo Check gk_KERNEL_TASK_SUSPEND with current task
 *  \relates Task
 */
G_INT32 gu_TaskSuspend(GS_TCB *ptcb)
{
    PRINT_ASSERT((G_Running == G_TRUE), "ERROR task suspend but GeMRTOS is not running");
    gm_GeMRTOSCriticalSectionEnter; // Is called in mutex section
        gk_KERNEL_TASK_SUSPEND((GS_TCB *) ptcb);
	gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}

/**gu_TASK_IS_BLOCKED
 *  \brief The gu_TASK_IS_BLOCKED function checks if a task is currently blocked, waiting for a resource.
 *  \exception par_following The function accepts one parameter:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task to check.
 *  \return The gu_TASK_IS_BLOCKED function returns G_TRUE if the task is blocked waiting for a resource, and G_FALSE otherwise.
 *  \sa gu_TaskSuspend, gu_TaskResume, gu_TaskKill
 *  \todo Check correcteness for diferent resources (semaphore and message queue)
 */
G_INT32 gu_TASK_IS_BLOCKED(GS_TCB *ptcb)
{
    G_INT32 value;
    gm_GeMRTOSCriticalSectionEnter;
        value = gk_TASK_IS_BLOCKED((GS_TCB *) ptcb);
	gm_GeMRTOSCriticalSectionExit;
    return (value);
}

/**gu_TaskResume
 *  \brief The gu_TaskResume function resumes a task that is currently in a waiting state.
 *  \exception par_following The function takes one parameter:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task to be resumed.
 *  \return The gu_TaskResume function returns G_TRUE if the task was successfully resumed and G_FALSE otherwise.
 *  \sa gu_TaskSuspend, gu_TASK_IS_BLOCKED, gu_TaskKill, gu_TaskDelay
 *  \todo Check that GRTOS is running
 *  \relates Task
 */
G_INT32 gu_TaskResume(GS_TCB *ptcb)
{
	G_INT32 return_value = G_FALSE;
    gm_GeMRTOSCriticalSectionEnter;
        if (GRTOS_TASK_STATE_WAITING(ptcb))	{
            gk_TCB_Unlink(ptcb);
            gk_TCBRDYL_Link(ptcb);
            return_value = G_TRUE;
        }
	gm_GeMRTOSCriticalSectionExit;
    return (return_value);
}


/***********************************************************************************
*********************** SIGNAL FUNCTIONS                 ***************************
***********************************************************************************/
 
/** gu_SignalCreate
 *  \brief The gu_SignalCreate function creates a signal of a specified type and associates it with a task or other system entity.  The signal's priority determines its execution order when multiple signals are pending.
 *  \exception par_following The function takes the following parameters:
 *  \param [in] Type An enumeration value specifying the type of signal to create (e.g., G_SCBType_TCB_ABORTED).
 *  \param [in] Priority An integer representing the priority of the signal. Higher priority signals are executed before lower priority signals when multiple signals are pending.
 *  \param [in] pxcb A pointer to a control structure. This structure could represent various system entities like tasks, resources, processors, or events, to which the signal is linked.
 *  \param [in] Signal_Code A pointer to the function that implements the signal's behavior (the signal handler).
 *  \param [in] Signal_Arg A pointer to an argument that will be passed to the Signal_Code function when the signal is executed.
 *  \return The gu_SignalCreate function returns a pointer to the newly created GS_SCB structure.  A NULL return indicates failure.
 *  \sa gu_SignalDestroy
 *  \todo Check SCB availability
 *  \todo Include signal for the rest of control blocks
 *  \todo Define signals for different events and resources
 *  \relates Signal
 *  \remark gu_SignalCreate
 */ 
GS_SCB *gu_SignalCreate(enum scbtype Type, G_INT32 Priority, void *pxcb, void *Signal_Code, void *Signal_Arg)
{
    gm_GeMRTOSCriticalSectionEnter;
        GS_SCB *pscb = gk_SCB_GetFree();
        PRINT_ASSERT((gkm_SCB_IsValid(pscb) == G_TRUE),"ERROR psignal in gu_SignalCreate\n");
        
        pscb->SCBPriority  = Priority;
        pscb->SCBType      = Type;
        pscb->SCB_TaskCode = Signal_Code;
        pscb->SCB_TaskArg  = (void *) Signal_Arg; //(void *) pscb;
        // 22-10-2024 pscb->SCB_NextSCBAPSL = (struct gs_scb *) 0;
        pscb->SCB_AssocXCB = pxcb;
        switch (Type){
            // Types for KCB
            case G_SCBType_FROZEN_MODE:
                gk_KCBASL_Link(pscb);
				break;

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
    gm_GeMRTOSCriticalSectionExit;
	return((GS_SCB *) pscb);
}

/**gu_SignalDestroy
 *  \brief The gu_SignalDestroy function removes a signal from a control block.  This disassociates the signal from its associated task or system entity, preventing further execution of the signal's handler.
 *  \exception par_following The function accepts one parameter:
 *  \param [in] pscb A pointer to the GS_SCB structure representing the signal to be removed.
 *  \return The gu_SignalDestroy function returns G_TRUE if the signal was successfully removed, and G_FALSE otherwise.
 *  \sa gu_SignalCreate
 *  \todo Check everything of this function
 *  \relates Signal
 */
G_INT32 gu_SignalDestroy(GS_SCB *pscb)
{
    gm_GeMRTOSCriticalSectionEnter;
	PRINT_ASSERT((struct_test(pscb) == G_TRUE),"ERROR STRUCTURE pointer is not valid pointer = %p\n", (void *) pscb);
// 23-10-2024 	// void *pxcb = (void *) pscb->SCB_AssocXCB;
// 23-10-2024 	// 22-10-2024 GS_SCB *pscb_pending = pscb->SCB_NextSCBAPSL;
// 23-10-2024 	GS_SCB *pscb_pending = pscb->SCB_NextSCB;
// 23-10-2024 	G_INT32 result = G_TRUE;
// 23-10-2024 
// 23-10-2024     // Check if the signal is pending
// 23-10-2024 	while (result == G_TRUE && pscb_pending != (struct gs_scb *) 0)
// 23-10-2024 	{
// 23-10-2024 		result = gk_SCBAPSL_UnLink((GS_SCB *) pscb, pscb_pending);
// 23-10-2024 		gk_SCBFL_Link(pscb_pending);
// 23-10-2024 		// 22-10-2024 pscb_pending = pscb->SCB_NextSCBAPSL;
// 23-10-2024 		pscb_pending = pscb->SCB_NextSCB;
// 23-10-2024 	}

	// 22-10-2024 if (pscb->SCB_NextSCBAPSL == (struct gs_scb *) 0)
// 23-10-2024 	if (pscb->SCB_NextSCB == (struct gs_scb *) 0)
// 23-10-2024 	{   // remove root because no pending are linked
		switch (pscb->SCBState){
			// Types for KCB
			case G_SCBState_IN_KCB:
			 	gk_KCBASL_Unlink((GS_SCB *) pscb);
			 	break;

			// Types for ECB
			case G_SCBState_IN_ECB:
			 	gk_ECBASL_Unlink((GS_ECB *)pscb->SCB_AssocXCB, (GS_SCB *) pscb);
			 	break;

			// Types for TCB
			 case G_SCBState_IN_TCB:
			 	gk_TCBASL_Unlink((GS_TCB *)pscb->SCB_AssocXCB, (GS_SCB *) pscb);
			 	pscb->SCBState = G_SCBState_UNLINKED;
			 	break;

			// Types for RCB
			 case G_SCBState_IN_RCB:
			 	gk_RCBASL_Unlink((G_RCB *)pscb->SCB_AssocXCB, (GS_SCB *) pscb);
			 	pscb->SCBState = G_SCBState_UNLINKED;
			 	break;

			// Types for PCB
			case G_SCBState_IN_PCB:
				// 24-10-2024 pscb->SCBState = G_SCBState_UNLINKED;
				// 24-10-2024 break;

			// Types for LCB
			case G_SCBState_IN_LCB:
				// 24-10-2024 pscb->SCBState = G_SCBState_UNLINKED;
				// 24-10-2024 break;
				
            default:
				PRINT_ASSERT((G_TRUE == G_FALSE), "ERROR in pscb->SCBState= %u",pscb->SCBState);
    			G_DEBUG_WHILEFOREVER;
    			break;
		}
		PRINT_ASSERT((pscb->SCBState == G_SCBState_UNLINKED), "ERROR set UNLINKED in UnLink functions: pscb->SCBState= %u",pscb->SCBState);
		gk_SCBFL_Link((GS_SCB *) pscb);
// 23-10-2024	}
    gm_GeMRTOSCriticalSectionExit;
	return(G_TRUE);
}

/***********************************************************************************
*********************** USER FUNCTIONS                   ***************************
***********************************************************************************/

// 10-06-2024 /**gu_get_irq_status
// 10-06-2024  *  \brief 
// 10-06-2024  *  Return the status of the system IRQs
// 10-06-2024  *  \return Status of the system IRQS
// 10-06-2024  *  \todo Change to return status of one IRQ in order to cover more than a G_INT32 scope
// 10-06-2024  *  \relates Miscellaneous
// 10-06-2024  */
// 10-06-2024 G_INT32 gu_get_irq_status(void)
// 10-06-2024 {
// 10-06-2024 	G_INT32 status;
// 10-06-2024     gm_GeMRTOSCriticalSectionEnter;
// 10-06-2024         status = GRTOS_CMD_IRQ_RQS;
// 10-06-2024 	gm_GeMRTOSCriticalSectionExit;
// 10-06-2024 	return(status);
// 10-06-2024 }


// 21-10-2024 /**gu_get_frozen_threshold
// 21-10-2024  *  \brief The gu_get_frozen_threshold function retrieves the current value stored in the frozen threshold register of the GeMRTOS controller (R_FRZ_TM_THR). It is designed for use in user mode, allowing the user to access the value of the frozen threshold.
// 21-10-2024  *  \exception par_following The gu_get_frozen_threshold function does not requiere any input parameters.
// 21-10-2024  *  \return The gu_get_frozen_threshold function returns the value of the frozen threshold register (R_FRZ_TM_THR).
// 21-10-2024  *  \relates Frozen_Mode
// 21-10-2024  */
// 21-10-2024 gt_time gu_get_frozen_threshold(void)
// 21-10-2024 {
// 21-10-2024 	gt_time status;
// 21-10-2024     gm_GeMRTOSCriticalSectionEnter;
// 21-10-2024         status = (gt_time) gm_FrozenModeThresholdGet;
// 21-10-2024 	gm_GeMRTOSCriticalSectionExit;
// 21-10-2024 	return(status);
// 21-10-2024 }

// 10-06-2024 /**gu_get_mutex_time
// 10-06-2024  *  \brief 
// 10-06-2024  *  Return the time the mutex is granted in system time units 
// 10-06-2024  *  \relates Miscellaneous
// 10-06-2024  */
// 10-06-2024 gt_time gu_get_mutex_time(void)
// 10-06-2024 {
// 10-06-2024 	gt_time status;
// 10-06-2024     gm_GeMRTOSCriticalSectionEnter;
// 10-06-2024         status = (gt_time) gm_GeMRTOSCriticalSectionGrantedTime;
// 10-06-2024 	gm_GeMRTOSCriticalSectionExit;
// 10-06-2024 	return(status);
// 10-06-2024 }


// 12-07-2024 /**gu_GetCurrentTCB
// 12-07-2024  *  \brief 
// 12-07-2024  *  Returns the task TCB that the current processor was executing
// 12-07-2024  *  \return Pointer to the TCB of the task being executed by the current processor
// 12-07-2024  *  \relates Task
// 12-07-2024  *  \sa Processor
// 12-07-2024  */
// 12-07-2024 GS_TCB *gu_GetCurrentTCB(void)
// 12-07-2024 {
// 12-07-2024     GS_TCB *ptcb;
// 12-07-2024     gm_GeMRTOSCriticalSectionEnter;
// 12-07-2024         ptcb = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB;
// 12-07-2024     gm_GeMRTOSCriticalSectionExit;
// 12-07-2024 	return(ptcb);
// 12-07-2024 }

/**gu_SchedulingListExclusionSet
 *  \brief The gu_SchedulingListExclusionSet function sets the exclusion level for a scheduling list.  The exclusion level limits the number of tasks from that list that can be simultaneously in the execution state. This mechanism can be used for load balancing or to ensure real-time properties by protecting against multiprocessor anomalies. Setting the exclusion to 1 can help safeguard real-time task scheduling from anomalies within the scheduling list.
 *  \exception par_following The function accepts two parameters:
 *  \param [in] plcb A pointer to the GS_LCB structure of the scheduling list whose exclusion level is to be modified.
 *  \param [in] exclusion An integer value that specifies the new exclusion level. A value of 1 ensures that multiple tasks from the scheduling list do not run concurrently on different processors. Values between 2 and the number of processors assigned to the scheduling list determine the number of tasks that can execute simultaneously on different processors. Additionally, values exceeding the number of processors assigned to the scheduling list will have no effect.
 *  \return The function returns a G_TRUE.
 *  \sa gu_SchedulingListCreate, gu_SchedulingListAssociateProcessor, gu_SchedulingListAssociateTask
 *  \remark gu_list_exclusion
 *  \relates Scheduling_List
 */
G_INT32 gu_SchedulingListExclusionSet(GS_LCB *plcb, G_INT32 exclusion)
{
    gm_GeMRTOSCriticalSectionEnter;
        plcb->LCBExclusion = exclusion;
    gm_GeMRTOSCriticalSectionExit;
	return(G_TRUE);
}


/** gk_CRITICAL_SECTION_HIGHEST_PRIORITY_PROCESSOR_CALLBACK
 *  \brief The gk_CRITICAL_SECTION_HIGHEST_PRIORITY_PROCESSOR_CALLBACK is call when the processor
 *  grants the critical section and may set the highest priority processor for the next 
 *  critical section assignment. It is used to rotate the highest priority processor for next selection.
 *  \remark 
 *  \details More details
 */
void gk_CRITICAL_SECTION_HIGHEST_PRIORITY_PROCESSOR_CALLBACK(void)
{
	GRTOS_CMD_MTX_HGHST_PRC(0);
	// if (gm_ProcessorId == G_NUMBER_OF_PCB) GRTOS_CMD_MTX_HGHST_PRC(0);
	// else GRTOS_CMD_MTX_HGHST_PRC(gm_ProcessorId);
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
			gk_TCBWL_Unlink(ptcb); 
			gk_TCBRDYL_Link(ptcb); 
			gk_TCBAEL_Unlink(pevent);
			gk_ECBFL_Link((GS_ECB *) pevent);
            break; 

        case G_ECBType_PERIODIC:  /* It is a Periodic Release of the task */

            gk_TCBAEL_Unlink(pevent);  // Preserve the Periodic event to remove the rest of events for a clean release
            
            gk_TASK_RELEASE((GS_TCB *) ptcb);  // Initialise the TCB and stack preserving signals
            
            /* Set a new event in the future */
            pevent->ECBValue.i64 = (G_INT64) pevent->ECBValue.i64 + ptcb->TCBPeriod;
            gk_TCBAEL_Link(pevent, ptcb);  // Link the periodic event back to the tcb
            gk_ECBTL_Link(pevent);
            
            gk_TCBRDYL_Link(ptcb); 
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
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, pevent->ECBType=%d\n",__FUNCTION__,__LINE__,gm_ProcessorId,(int) pevent->ECBType);
            // gk_sem_post(pevent->ECB_AssocRCB, pevent->ECB_AssocTCB);
            prcb = (G_RCB *) get_pointed_field(pevent, ECB_AssocRCB);
            PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");
            prcb->semaphore.SEM_Current_Count++;
            // gk_ECB_List_Unlink(pevent);
            gk_RCBGEL_Unlink(pevent);
            
            // Check if there exist waiting event
            pevent1 = get_pointed_field(prcb, RCB_NextRCBWEL);
            if (pevent1 != (GS_ECB *) 0)
            {
                PRINT_ASSERT((gkm_ECB_IsValid(pevent1) == G_TRUE),"ERROR ECB not valid\n");
                prcb->semaphore.SEM_Current_Count--;
                gk_RCBWEL_Unlink(pevent1);
                pevent1->ECBType      = G_ECBType_SEM_GRANTED;         
                gk_RCBGEL_Link((G_RCB *) prcb, pevent1->ECB_AssocTCB, pevent1);
                
                // Resume waiting task
                PRINT_ASSERT((gkm_TCB_IsValid(pevent1->ECB_AssocTCB) == G_TRUE),"ERROR TCB not valid\n");
                gk_TCB_Unlink(pevent1->ECB_AssocTCB);
                gk_TCBRDYL_Link(pevent1->ECB_AssocTCB);                
            }            
            
            break;
            
        case G_ECBType_SEM_WAITING:
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, pevent->ECBType=%d\n",__FUNCTION__,__LINE__,gm_ProcessorId,(int) pevent->ECBType);        
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
                    pevent->ECBType = G_ECBType_MESSAGE_PRODUCER_DELIVERED;
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
            
        case G_ECBType_MESSAGE_PRODUCER_DELIVERED:        
        case G_ECBType_MESSAGE_PRODUCE_WAIT:
            gk_queue_kill_producer(pevent);
            break;
        
        case G_ECBType_MESSAGE_SEND_WAIT:
        case G_ECBType_MESSAGE_SEND_EXPIRED:
        case G_ECBType_MESSAGE_CONSUME_EXPIRED:
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, pevent->ECBType=%d\n",__FUNCTION__,__LINE__,gm_ProcessorId,(int) pevent->ECBType);
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
	// It is executed in critial section ###########
    // gu_fprintf("FROZEN Proc: %d\n", gm_ProcessorId);
	printf("FROZEN Proc: %d\n", gm_ProcessorId);
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
        case GS_LCBTypeEDF:
            if (get_pointed_field(ptcb, TCBType) == G_TCBType_PERIODIC)
            {
                /* Get the next occurrence time associated with the task */
                pevent = get_pointed_field(ptcb, TCB_NextTCBAEL);
                while (pevent != (GS_ECB *) 0)
                {
                    PRINT_ASSERT((gkm_ECB_IsValid((GS_ECB *) pevent) == G_TRUE),"ERROR ECB is not valid, pevent= %p\n", (void *) pevent);
                    if (get_pointed_field(pevent, ECBType) == G_ECBType_PERIODIC)
                    {
						if (ptcb->TCBReadyPriority == ptcb->TCBRunPriority) {
							set_pointed_field(ptcb, TCBReadyPriority, pevent->ECBValue.i64);
							set_pointed_field(ptcb, TCBRunPriority, pevent->ECBValue.i64);
						} else {
							set_pointed_field(ptcb, TCBReadyPriority, pevent->ECBValue.i64);
						}
                        break;
                    }
                    pevent = (GS_ECB *) get_pointed_field(pevent, ECB_NextTCBAEL);
                }
                PRINT_ASSERT(((GS_ECB *) pevent != (GS_ECB *) 0),"ERROR G_ECBType_PERIODIC not found\n");
            }
            break;

        // 22-10-2024 case GS_LCBType_UNSPECIFIED:            
        case GS_LCBTypeFP:           
            break;
			
		default:
			G_DEBUG_WHILEFOREVER;
			break;
    }

    GK_SAMPLE_FUNCTION_END(10011)
    return(G_TRUE);
}


G_INT32 gk_START_TASK_CALLBACK(GS_TCB *ptcb, G_INT64 ticks_offset)
{
    GS_ECB *pevent;
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE), "ERROR: ptcb= %p\n", (void *) ptcb);
    PRINT_ASSERT((ptcb->TCBType != G_TCBType_ISR), "ERROR: ptcb= %p\n", (void *) ptcb);
    
    switch (ptcb->TCBType){
        case G_TCBType_OneShot:
            gk_TCBRDYL_Link(ptcb);  /* Insert Task in Ready List                */
            break;
        case G_TCBType_PERIODIC:
            /* Get the starting time of the task depending whether the RTOS is running or not */
            if (G_Running != G_FALSE) ticks_offset = gm_SystemTimeGet + ticks_offset;
            
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
