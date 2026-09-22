/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS User Interface API
 *  \details This file contains the main API functions without resource specific user functions.
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

/* Forward declaration: defined after its only caller in this file. */
static G_UINT32 gk_StartTaskCallback(GS_TCB *ptcb, G_UINT64 ticks_offset);

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
 *  \class Time_Management
 *  \exception sec_fun_intro In GeMRTOS, the core time management tasks revolve around manipulating system times and time intervals to support precise task synchronization and scheduling. These functions enable setting, reading, and adjusting system clocks, as well as defining time intervals for specific operations. Through effective time manipulation, developers can coordinate task execution and ensure activities occur at exact moments, maintaining the system’s deterministic behavior.
 */


OPTIMIZE_CODE

/**
 *  \brief gu_ConvertTime returns absolute time from hours, minutes, seconds and ms
 *  \exception par_following The gu_ConvertTime function has the following parameters
 *  \exception class_name Time_Management
 *  \exception prototype G_UINT64 gu_ConvertTime(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
 *  \param hours Number of hours of the absolute time.
 *  \param minutes Number of minutes of the absolute time.
 *  \param seconds Number of seconds of the absolute time.
 *  \param ms Number of mili-seconds of the absolute time.
 *  \return gu_ConvertTime returns the absolute time
 *  \remark gu_ConvertTime
 *  \relates Time_Management
 */
G_UINT64 gu_ConvertTime(unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
	G_UINT64 time_ticks;

	time_ticks = (G_UINT64) ms * (G_UINT64) G_TICKS_PER_MSECOND;
	time_ticks = time_ticks + (G_UINT64) seconds * (G_UINT64) G_TICKS_PER_SECOND;
	time_ticks = time_ticks + (G_UINT64) minutes * (G_UINT64) (G_TICKS_PER_SECOND * G_SECONDS_IN_MINUTE);
	time_ticks = time_ticks + (G_UINT64) hours * (G_UINT64) G_TICKS_PER_SECOND * (G_UINT64) G_SECONDS_IN_HOUR;
	return (time_ticks);
}


/**gu_SystemTotalTimeGet
 *  \brief gu_SystemTotalTimeGet returns the total system time. It is the time in non frozen mode plus the time in frozen mode.
 *  \exception par_following The gu_SystemTotalTimeGet macro has no parameter. It has to be noted that temporal constraints (deadline, period) are related to system time, obtained using the gu_SystemTimeGet function, which is the time in non frozen mode.
 *  \exception class_name Time_Management
 *  \exception prototype G_UINT64 time = gu_SystemTotalTimeGet
 *  \return The gu_SystemTotalTimeGet macro returns the total system time. 
 *  \remark gu_SystemTotalTimeGet
 *  \relates Time_Management
 */
G_UINT64 gu_SystemTotalTimeGet(void)
{
	TIMEPRIORITY hw_time;
	
	gm_GeMRTOSCriticalSectionEnter();
		hw_time.i32[1] = (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SYS_TM_HGH);
		hw_time.i32[0] = (unsigned) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SMP);
	gm_GeMRTOSCriticalSectionExit();

	return ((G_UINT64)hw_time.i64); 
}

/**gu_FrozenModeTimeGet
 *  \brief gu_FrozenModeTimeGet returns the accumulated time the system was in Frozen Mode. This time is hold in the R_FRZ_CNT register of the GeMRTOS controller. 
 *  \exception par_following The gu_FrozenModeTimeGet function has no parameter.
 *  \exception class_name Frozen_Mode
 *  \exception prototype G_UINT64 time = gu_FrozenModeTimeGet
 *  \return The gu_FrozenModeTimeGet function returns the accumulated time the system was in Frozen Mode, hold in the the R_FRZ_CNT register of the GeMRTOS controller.
 *  \remark gu_FrozenModeTimeGet
 *  \relates Frozen_Mode
 */
G_UINT64 gu_FrozenModeTimeGet(void)
{
	TIMEPRIORITY hw_time;
	
	gm_GeMRTOSCriticalSectionEnter();
		hw_time.i32[1] = (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_FRZ_TM_HGH);
		hw_time.i32[0] = (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SMP);
	gm_GeMRTOSCriticalSectionExit();

	return ((G_UINT64) hw_time.i64);
}


/**gu_FrozenModeThresholdGet
 *  \brief The gu_FrozenModeThresholdGet function retrieves the value of the Frozen Time Threshold register from the GeMRTOS controller (R_FRZ_TM_THR). By default, the frozen mode is disabled, and the frozen threshold is set to zero. This function is useful for determining the current threshold value, which is critical for managing the activation of frozen mode. 
 *  \exception par_following The gu_FrozenModeThresholdGet function does not accept any parameters
 *  \exception class_name Frozen_Mode
 *  \exception prototype TIMEPRIORITY interval = gu_FrozenModeThresholdGet
 *  \return The gu_FrozenModeThresholdGet function returns the current value of the Frozen Time Threshold register in the GeMRTOS controller.
 *  \sa gu_FrozenModeThresholdSet, gm_FrozenModeEnable(), gm_FrozenModeDisable(), gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \remark gu_FrozenModeThresholdGet
 *  \relates Frozen_Mode
 */
G_UINT64 gu_FrozenModeThresholdGet(void) 
{
    TIMEPRIORITY hw_time;
	gm_GeMRTOSCriticalSectionEnter();
		hw_time.i32[1] = (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_FRZ_THR_HGH);
		hw_time.i32[0] = (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SMP);
	gm_GeMRTOSCriticalSectionExit();
	
    return ((G_UINT64) hw_time.i64); 
}

/**
 *  \brief The gu_FrozenModeThresholdSet function sets the value of the Frozen Time Threshold register in the GeMRTOS controller. By default, the frozen mode is disabled, and the frozen threshold is initialized to zero. This function is essential for configuring the threshold that determines when the frozen mode becomes active when it is enabled.
 *  \exception par_following The gu_FrozenModeThresholdSet function accepts the following parameter:
 *  \exception class_name Frozen_Mode
 *  \param [in] time_set The frozen threshold value specified in system ticks units. This value establishes the interval of delay in processing timed events after which the frozen mode will be triggered when frozen mode is enabled. 
 *  \return The gu_FrozenModeThresholdSet function does not return any value.
 *  \sa gu_FrozenModeThresholdGet, gm_FrozenModeEnable(), gm_FrozenModeDisable(), gm_ProcessorInterrupt, gu_ProcessorWaitForIrq
 *  \relates Frozen_Mode
 *  \remark gu_FrozenModeThresholdSet
 */
G_UINT32 gu_FrozenModeThresholdSet(G_UINT64 time_set) 
{
	TIMEPRIORITY time_reg;
	gm_GeMRTOSCriticalSectionEnter();
		time_reg.i64 = (G_UINT64) time_set;
		gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_SMP, (unsigned long)time_reg.i32[0]);
		gm_IOWR(GEMRTOS_DRIVER_BASE, ADDR_FRZ_THR_HGH, (unsigned long)time_reg.i32[1]); 
	gm_GeMRTOSCriticalSectionExit(); 
	return (G_TRUE);
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
 *  \brief The gu_TaskTypeSet function sets the type of a task.  The valid task types are G_TCBType_Periodic and G_TCBType_OneShot.
 *  \exception par_following The function takes these parameters:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task whose type is to be modified.
 *  \param [in] type The type must be either G_TCBType_Periodic or G_TCBType_OneShot. Any other value will result in failure.
 *  \return The gu_TaskTypeSet function returns G_TRUE if the task type was successfully set, and G_FALSE otherwise.
 *  \sa gu_TaskCreate, gu_TaskRunPrioritySet, gu_TaskReadyPrioritySet, gu_TaskPeriodSet
 *  \note Type is not range-checked at runtime; caller must pass a valid GS_TCBType value.
 *  \relates Task
 */ 
G_UINT32 gu_TaskTypeSet(struct gs_tcb *ptcb, enum tcbtype type)
{
    gm_GeMRTOSCriticalSectionEnter();
        ptcb->TCBType = type;
    gm_GeMRTOSCriticalSectionExit();
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
 *  \note The plcb pointer is validated by PRINT_ASSERT on debug builds (G_DEBUG_WHILEFOREVER_ENABLE == 1).
 */
G_UINT32 gu_SchedulingListAssociateTask(struct gs_tcb *ptcb, struct gs_lcb *plcb)
{
    gm_GeMRTOSCriticalSectionEnter();
        PRINT_ASSERT((gkm_LCB_IsValid(plcb) == G_TRUE),"ERROR LCB is not valid\n");
        PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
        ptcb->TCB_RDY_LCB_Index = plcb;
    gm_GeMRTOSCriticalSectionExit();
    return (G_TRUE);
}

/**gu_TaskReadyPrioritySet
 *  \brief The gu_TaskReadyPrioritySet function sets the ready priority of a task.  This priority determines the task's position in the ready queue and influences its scheduling order.  Note that larger values of priority represent lower priority; smaller values indicate higher priority.
 *  \exception par_following The function takes two parameters:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task whose ready priority is to be set.
 *  \param [in] priority A G_UINT64 value representing the new ready priority for the task. Larger values indicate lower priority; smaller values indicate higher priority.
 *  \return The gu_TaskReadyPrioritySet function always returns G_TRUE.
 *  \sa gu_TaskRunPrioritySet, gu_TaskTypeSet, gu_TaskPeriodSet, gu_TaskSuspend, gu_TaskResume
 *  \relates Task
 */
G_UINT32 gu_TaskReadyPrioritySet(struct gs_tcb *ptcb, G_UINT64 priority)
{
    gm_GeMRTOSCriticalSectionEnter();
        set_pointed_field(ptcb, TCBReadyPriority, (G_UINT64) priority);
        if (ptcb->TCBRunPriority > ptcb->TCBReadyPriority) ptcb->TCBRunPriority = ptcb->TCBReadyPriority;
        if (ptcb->TCBCurrentPriority > ptcb->TCBReadyPriority) ptcb->TCBCurrentPriority = ptcb->TCBReadyPriority;
    gm_GeMRTOSCriticalSectionExit();
    return (G_TRUE);
}

/**gu_TaskRunPrioritySet
 *  \brief The gu_TaskRunPrioritySet function sets the run-time priority of a task. This priority determines the task's execution order when it is running.  Larger values of priority represent lower priority; smaller values represent higher priority.
 *  \exception par_following The function takes these parameters:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task whose run-time priority is to be set.
 *  \param [in] priority A G_UINT64 value specifying the new run-time priority. Larger values mean lower priority, and smaller values mean higher priority.
 *  \return The gu_TaskRunPrioritySet function always returns G_TRUE.
 *  \sa gu_TaskReadyPrioritySet, gu_TaskTypeSet, gu_TaskPeriodSet, gu_TaskSuspend, gu_TaskResume
 *  \relates Task
 */
G_UINT32 gu_TaskRunPrioritySet(struct gs_tcb *ptcb, G_UINT64 priority)
{
    gm_GeMRTOSCriticalSectionEnter();
        set_pointed_field(ptcb, TCBRunPriority, (G_UINT64) priority);
        if (ptcb->TCBCurrentPriority > ptcb->TCBRunPriority) ptcb->TCBCurrentPriority = ptcb->TCBRunPriority;
    gm_GeMRTOSCriticalSectionExit();
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
G_UINT32 gu_TaskPeriodSet(struct gs_tcb *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    /* Compute the total number of ticks required for the period */
    gm_GeMRTOSCriticalSectionEnter();
		set_pointed_field(ptcb, TCBPeriod, gu_ConvertTime(hours, minutes, seconds, ms));
		PRINT_ASSERT((ptcb->TCBPeriod > 0), "ERROR in task period");
    gm_GeMRTOSCriticalSectionExit();
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
 *  \remark gu_task_start
 *  \relates Task
 */
G_UINT32 gu_TaskStartWithOffset(GS_TCB *ptcb, unsigned int hours, unsigned int minutes, unsigned int seconds, unsigned int ms)
{
    G_UINT64 ticks = gu_ConvertTime(hours, minutes, seconds, ms);

    gm_GeMRTOSCriticalSectionEnter();
		gk_StartTaskCallback(ptcb, ticks);
    gm_GeMRTOSCriticalSectionExit();
    
    return (G_TRUE);
}


/**gu_TaskCreate
 *  \brief The gu_TaskCreate function creates a task with default settings and returns a pointer to its GS_TCB structure.  Task parameters can be modified before creation by adjusting default settings or after creation using task-related functions. While the function requires only task_code and p_arg, it allows for optional task description formatting using a printf-style format string and arguments.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] task_code A pointer to the function that implements the task's code (the task's entry point). It is the name of the function that implements the task code.
 *  \param [in] p_arg A pointer to an argument that will be passed to the task_code function each time the task is invoked. This is a void * and can be cast to other types within the task code.
 *  \param [in] format A format string, similar to printf, used to create a description string for the task (up to G_TCB_DESCRIPTION_LENGTH characters). This string can contain format specifiers that are replaced by subsequent arguments.
 *  \return The gu_TaskCreate function returns a pointer to the GS_TCB structure of the newly created task.  This pointer should be used in all subsequent calls related to that task. A NULL return indicates task creation failure.
 *  \sa gu_TaskGetCurrentTCB, gu_TaskTypeSet, gu_TaskStartWithOffset, gu_TaskKill
 *  \remark gu_create_task
 *  \relates Task
 */
void *gu_TaskCreate(void *task_code, void *p_arg, const char *format, ...)
{
    GS_TCB  *ptcb;
    
    gm_GeMRTOSCriticalSectionEnter();
	
		char desc_buf[G_TCB_DESCRIPTION_LENGTH + 4];
		desc_buf[G_TCB_DESCRIPTION_LENGTH + 3] = '\0';

		va_list args;
		va_start (args, format);
		vsnprintf(desc_buf, G_TCB_DESCRIPTION_LENGTH, format, args);
		va_end (args);		
		
		ptcb = gk_TCB_GetFree((unsigned int) G_TASK_STACKSIZE);

		PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB not valid in gu_TaskCreate\n");
		
		// Initialize the gs_tcb structure of the task
		if (ptcb != (struct gs_tcb *) 0) {

			ptcb->TCB_TaskCode         = task_code;           // Pointer to the begining of the task code
			ptcb->TCB_TaskArg          = p_arg;              // Pointer to the argument of the first call

			ptcb->TCBType              = G_TASK_TYPE_DEFAULT;      // Type of the Task
			ptcb->TCBReadyPriority     = G_TASK_PRIORITY_DEFAULT;  // Initial Priority of the task
			ptcb->TCBRunPriority       = G_TASK_PRIORITY_DEFAULT;  // Priority of the task when it is executing
			ptcb->TCBCurrentPriority   = G_TASK_PRIORITY_DEFAULT;  // Priority of the task when it is executing
			ptcb->TCBPeriod            = G_TASK_PERIOD_DEFAULT;    // Period of the task
			ptcb->TCB_RDY_LCB_Index    = (GS_LCB *) G_TASK_LCB_DEFAULT; /* number of the ready list that should be inserted */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
			strncpy((char *) ptcb->TCB_description, (char *) desc_buf, G_TCB_DESCRIPTION_LENGTH-1);
#pragma GCC diagnostic pop

			// Initilize the stack of the task
			gk_TASK_STK_INIT(ptcb);
			set_pointed_field(ptcb, TCB_MTX_NESTED, 0);	
		}

    gm_GeMRTOSCriticalSectionExit();
    
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
G_UINT32  gu_TaskDelay(G_UINT32 hours, G_UINT32 minutes, G_UINT32 seconds, G_UINT32 ms)
{
    /* Compute the total number of periods required   */
    G_UINT64 ticks = gu_ConvertTime(hours, minutes, seconds, ms);
    if (ticks > 0){
    	gu_TaskDelayTime((G_UINT64) ticks);
    }
    return (G_TRUE);
}

/**gu_TaskGetCurrentTCB
 *  \brief The gu_TaskGetCurrentTCB function retrieves a pointer to the Task Control Block (GS_TCB) of the currently executing task.
 *  \exception par_following This function takes no parameters.
 *  \return The gu_TaskGetCurrentTCB function returns a pointer to the GS_TCB structure of the currently running task.
 *  \sa gu_TaskCreate, gu_TaskRunPrioritySet, gu_TaskReadyPrioritySet
 *  \remark gu_TaskGetCurrentTCB
 *  \relates Task
 */ 
GS_TCB *gu_TaskGetCurrentTCB(void)
{
    GS_TCB *ptcb;
    gm_GeMRTOSCriticalSectionEnter();
        ptcb = gk_PCB_GetCurrentTCB();
    gm_GeMRTOSCriticalSectionExit();
    return (ptcb);
}


/**gu_TaskDelayTime
 *  \brief The gu_TaskDelayTime function suspends the execution of the current task for a specified number of system clock ticks.  This function provides a more direct way to specify sleep duration compared to gu_TaskDelay, using the system's time units directly.
 *  \exception par_following The function takes one parameter:
 *  \param [in] ticks The number of system clock ticks for which the task should sleep.
 *  \return The gu_TaskDelayTime function always returns G_TRUE.
 *  \sa gu_TaskDelay, gu_TaskStartWithOffset, gu_TaskPeriodSet, gu_TaskRunPrioritySet, gu_TaskReadyPrioritySet
 *  \note Uses gk_KERNEL_TASK_SUSPEND_CURRENT directly since the delaying task is always the current one.
 *  \relates Task
 */ 
G_UINT32 gu_TaskDelayTime(gt_time ticks)
{
	GS_TCB  *ptcbcurrent;
	GS_ECB *pevent;

    PRINT_ASSERT((!(gm_TimeCountersResetGet())), "ERROR: Task sleep called but GeMRTOS is not running");
    gm_GeMRTOSCriticalSectionEnter();
    ptcbcurrent = gk_PCB_GetCurrentTCB(); /* Get the current task */
	pevent = gk_ECB_GetFree(ptcbcurrent);

	set_pointed_field(pevent, ECBValue.i64, gu_SystemTimeGet() + ticks);
	set_pointed_field(pevent, ECBType,      (G_UINT32) G_ECBType_OSTimeDly);

	gk_ECBTL_Link(pevent);

	gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);

	gm_GeMRTOSCriticalSectionExit(); 

    return (G_TRUE);
}

/**
 *  \brief
 *  Destroys a task and return the data structure to the TCB free list.
 *  Only called from gu_TaskKill; moved here from gemrtos_list.c (F4).
 *  \param [in] ptcb Pointer to the TCB of the task to destroy
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \details Unlinks all the linked lists and link to the Free TCB list
 *  \relates Task
 */
G_UINT32 gk_TASK_KILL(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10077)
    gk_TCB_Unlink(ptcb);       /* Unlinks the TCB structure               */
    gk_TCB_List_Unlink(ptcb);  /* Unlinks all the linked lists of the TCB */
    gk_TCBFL_Link(ptcb);       /* Links the TCB to the TCBFL              */
    GK_SAMPLE_FUNCTION_END(10077)
    return (G_TRUE);
}

/**gu_TaskKill
 *  \brief The gu_TaskKill function terminates a task and releases all associated resources, returning them to the free lists.
 *  \exception par_following The function takes one parameter:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task to be terminated.
 *  \return The gu_TaskKill function always returns G_TRUE.
 *  \sa gu_TaskSuspend, gu_TaskResume, gu_TaskCreate
 *  \relates Task
 */
G_UINT32 gu_TaskKill(GS_TCB *ptcb)
{
    gm_GeMRTOSCriticalSectionEnter();
        gk_TASK_KILL((GS_TCB *) ptcb);
	gm_GeMRTOSCriticalSectionExit();
    return (G_TRUE);
}

/**gu_TaskSuspend
 *  \brief The gu_TaskSuspend function suspends a task, changing its state to waiting.
 *  \details The task is suspended only when it is currently in the
 *           G_TCBState_READY or G_TCBState_RUNNING state. When the task is in any
 *           other state (already waiting, waiting-completed, or unlinked) the
 *           function takes no action and returns G_FALSE, so an inadvertent call
 *           on a non-runnable task can no longer halt a debug build inside
 *           gk_KERNEL_TASK_SUSPEND. This makes the suspend/resume pair symmetric:
 *           gu_TaskResume acts only on waiting tasks, gu_TaskSuspend acts only on
 *           ready/running tasks. The task state is evaluated inside the GeMRTOS
 *           critical section.
 *
 *           A suspend request for a task RUNNING on another processor also
 *           requires that processor to reach a kernel entry point. On the
 *           embedded target that always happens, because the request is a
 *           hardware interrupt. On the Windows simulation a task executing
 *           user code cannot be preempted asynchronously, so the request is
 *           bounded and may be refused: this function then returns G_FALSE
 *           with the task left untouched, exactly as for a non-runnable task.
 *           Applications that must suspend such a task should retry, as
 *           example 02_tasks_and_delays does.
 *  \exception par_following The function takes one parameter:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task to be suspended.
 *  \return G_TRUE if the task was in the G_TCBState_READY or G_TCBState_RUNNING
 *          state and has been suspended; G_FALSE otherwise (the task is left
 *          unchanged).
 *  \sa gu_TaskResume, gu_TaskKill, gu_TaskDelay
 *  \relates Task
 */
G_UINT32 gu_TaskSuspend(GS_TCB *ptcb)
{
    G_UINT32 result = G_FALSE;
    PRINT_ASSERT((!(gm_TimeCountersResetGet())), "ERROR task suspend but GeMRTOS is not running");
    gm_GeMRTOSCriticalSectionEnter(); // Is called in mutex section
        /* Suspend only a task that is currently READY or RUNNING. Any other
         * state is reported as failure instead of asserting inside
         * gk_KERNEL_TASK_SUSPEND. State is read under the critical section,
         * mirroring gu_TaskResume. */
        if ((ptcb->TCBState == G_TCBState_READY) || (ptcb->TCBState == G_TCBState_RUNNING)) {
            result = gk_KERNEL_TASK_SUSPEND((GS_TCB *) ptcb);
        }
	gm_GeMRTOSCriticalSectionExit();
    return (result);
}

/**gu_TaskResume
 *  \brief The gu_TaskResume function resumes a task that is currently in a waiting state.
 *  \exception par_following The function takes one parameter:
 *  \param [in] ptcb A pointer to the GS_TCB structure of the task to be resumed.
 *  \return The gu_TaskResume function returns G_TRUE if the task was successfully resumed and G_FALSE otherwise.
 *  \sa gu_TaskSuspend, gu_TaskKill, gu_TaskDelay
 *  \relates Task
 */
G_UINT32 gu_TaskResume(GS_TCB *ptcb)
{
	G_UINT32 result = G_FALSE;
    gm_GeMRTOSCriticalSectionEnter();
        if (((ptcb->TCBState == G_TCBState_WAITING_COMPLETED) || (ptcb->TCBState == G_TCBState_WAITING)))	{
            gk_TCB_Unlink(ptcb);
            gk_TCBRDYL_Link(ptcb);
            result = G_TRUE;
        }
	gm_GeMRTOSCriticalSectionExit();
    return (result);
}


/***********************************************************************************
*********************** SIGNAL FUNCTIONS                 ***************************
***********************************************************************************/
 
/** gu_SignalCreate
 *  \brief The gu_SignalCreate function creates a signal of a specified type and associates it with a task or other system entity.  The signal's priority determines its execution order when multiple signals are pending.
 *  \exception par_following The function takes the following parameters:
 *  \param [in] type     An enumeration value specifying the type of signal to create (e.g., G_SCBType_TCB_ABORTED).
 *  \param [in] priority An integer representing the priority of the signal. Higher priority signals are executed before lower priority signals when multiple signals are pending.
 *  \param [in] pxcb A pointer to a control structure. This structure could represent various system entities like tasks, resources, processors, or events, to which the signal is linked.
 *  \param [in] signal_code A pointer to the function that implements the signal's behavior (the signal handler).
 *  \param [in] signal_arg A pointer to an argument that will be passed to the signal_code function when the signal is executed.
 *  \return The gu_SignalCreate function returns a pointer to the newly created GS_SCB structure.  A NULL return indicates failure.
 *  \sa gu_SignalDestroy
 *  \relates Signal
 *  \remark gu_SignalCreate
 */ 
GS_SCB *gu_SignalCreate(enum scbtype type, G_UINT32 priority, void *pxcb, void *signal_code, void *signal_arg)
{
    gm_GeMRTOSCriticalSectionEnter();
        GS_SCB *pscb = gk_SCB_GetFree();
        PRINT_ASSERT((gkm_SCB_IsValid(pscb) == G_TRUE),"ERROR psignal in gu_SignalCreate\n");
        
        pscb->SCBPriority  = priority;
        pscb->SCBType      = type;
        pscb->SCB_TaskCode = signal_code;
        pscb->SCB_TaskArg  = (void *) signal_arg; //(void *) pscb;

        pscb->SCB_AssocXCB = pxcb;
        switch (type){
            // Types for KCB
            case G_SCBType_FROZEN_MODE:
                gk_KCBASL_Link(pscb);
				break;

            case G_SCBType_UNFROZEN_MODE:
                gk_KCBASL_Link(pscb);
				break;

            // Types for TCB
            case G_SCBType_TCB_ABORTED:
                gk_TCBASL_Link((GS_TCB *)pxcb, pscb);
                pscb->SCBState = G_SCBState_IN_TCB;
                break;

            default:
    			G_DEBUG_WHILEFOREVER;
    			break;
        }
    gm_GeMRTOSCriticalSectionExit();
	return((GS_SCB *) pscb);
}

/**gu_SignalDestroy
 *  \brief The gu_SignalDestroy function removes a signal from a control block.  This disassociates the signal from its associated task or system entity, preventing further execution of the signal's handler.
 *  \exception par_following The function accepts one parameter:
 *  \param [in] pscb A pointer to the GS_SCB structure representing the signal to be removed.
 *  \return The gu_SignalDestroy function returns G_TRUE if the signal was successfully removed, and G_FALSE otherwise.
 *  \sa gu_SignalCreate
 *  \relates Signal
 */
G_UINT32 gu_SignalDestroy(GS_SCB *pscb)
{
    gm_GeMRTOSCriticalSectionEnter();
        PRINT_ASSERT((gkm_SCB_IsValid(pscb) == G_TRUE),"ERROR STRUCTURE pointer is not valid pointer = %p\n", (void *) pscb);

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
			 	gk_RCBASL_Unlink((GS_RCB *)pscb->SCB_AssocXCB, (GS_SCB *) pscb);
			 	pscb->SCBState = G_SCBState_UNLINKED;
			 	break;

			// Types for PCB
			case G_SCBState_IN_PCB:

			// Types for LCB
			case G_SCBState_IN_LCB:
				
            default:
				PRINT_ASSERT((G_TRUE == G_FALSE), "ERROR in pscb->SCBState= %u",(unsigned int) pscb->SCBState);
    			G_DEBUG_WHILEFOREVER;
    			break;
		}
		PRINT_ASSERT((pscb->SCBState == G_SCBState_UNLINKED), "ERROR set UNLINKED in UnLink functions: pscb->SCBState= %u",(unsigned int) pscb->SCBState);
		gk_SCBFL_Link((GS_SCB *) pscb);

    gm_GeMRTOSCriticalSectionExit();
	return(G_TRUE);
}

/***********************************************************************************
*********************** USER FUNCTIONS                   ***************************
***********************************************************************************/

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
G_UINT32 gu_SchedulingListExclusionSet(GS_LCB *plcb, G_UINT32 exclusion)
{
    gm_GeMRTOSCriticalSectionEnter();
        plcb->LCBExclusion = exclusion;
    gm_GeMRTOSCriticalSectionExit();
	return(G_TRUE);
}


/**
 *  \brief Called when a timed event fires; dispatches by ECB type to wake
 *         delayed tasks, re-queue periodic tasks, or handle deadline violations.
 *  \param [in] pevent Pointer to the ECB of the expired timed event.
 */
void gk_TimeCallback(GS_ECB *const pevent)
{
    GS_TCB *ptcb = pevent->ECB_AssocTCB;
    
    /* PROCESS ACCORDING TO THE TYPE OF THE EVENT     */
    switch (pevent->ECBType)
    {
        case G_ECBType_OSTimeDly:  /* It is a Delay Event */
            /* Task Should be inserted in Ready List if it is not waiting more events */
			gk_TCBWL_Unlink(ptcb);
			gk_TCBRDYL_Link(ptcb);
			gk_ECBFL_Link((GS_ECB *) pevent);
            break;

        case G_ECBType_PERIODIC:  /* It is a Periodic Release of the task */
            gk_TCBAEL_Unlink(pevent);
            gk_TASK_RELEASE((GS_TCB *) ptcb);
            pevent->ECBValue.i64 = (G_UINT64) pevent->ECBValue.i64 + ptcb->TCBPeriod;
            gk_TCBAEL_Link(pevent, ptcb);
            gk_ECBTL_Link(pevent);
            gk_TCBRDYL_Link(ptcb);
            break;
            
        case G_ECBType_MESSAGE_SENDER_TIMEOUT:
            gk_MessageQueueTimeoutECBSender(pevent);
            break;
            
        case G_ECBType_MESSAGE_RECEIVER_WAIT:
            gk_MessageQueueTimeoutECBReceiver(pevent);                
            break;

		case G_ECBType_SEM_WAITING_TIMEOUT:
			gk_SemaphoreTimeout(pevent);
			break;

		case G_ECBType_TRIGGER_TIMEOUT:
			gk_TriggerTimeout(pevent);
			break;
            
        case G_ECBType_LASTEST_TIME:
            /* The end of the times */
            G_DEBUG_WHILEFOREVER;
            break;

        case G_ECBType_ONESHOT_START_TIME:
            gk_TCBAEL_Unlink(pevent);
			gk_TCBWL_Unlink(ptcb); 
			gk_TCBRDYL_Link(ptcb); 
			gk_ECBFL_Link((GS_ECB *) pevent);
            break;
           
        default:
            G_DEBUG_WHILEFOREVER; 
            break; 
    }
}

/**
 *  \brief Called when a resource ECB is killed; dispatches by ECB type to
 *         clean up semaphore, message queue, trigger, or timer ECBs.
 *  \param [in] pevent Pointer to the resource ECB being killed.
 */
void gk_ResourceECBKillCallback(GS_ECB *pevent)
{
    
    switch (pevent->ECBType)
    {
        case G_ECBType_SEM_GRANTED:
        case G_ECBType_SEM_WAITING:
		case G_ECBType_SEM_WAITING_EXPIRED:
		case G_ECBType_SEM_WAITING_TIMEOUT:
			gk_SemaphoreKill(pevent);
            break;
            
        case G_ECBType_MESSAGE_RECEIVER_BUSY:
        case G_ECBType_MESSAGE_RECEIVER_WAIT:
        case G_ECBType_MESSAGE_RECEIVER_SUBSCRIBED:  
            gk_MessageQueueKillECBReceiver(pevent);
            break;

        case G_ECBType_MESSAGE_SENDER_DELIVERED:        
        case G_ECBType_MESSAGE_SENDER_WAIT:
            gk_MessageQueueKillECBSender(pevent);
            break;
        
        case G_ECBType_MESSAGE_SENDER_TIMEOUT:
        case G_ECBType_MESSAGE_RECEIVER_EXPIRED:
        case G_ECBType_MESSAGE_SENDER_EXPIRED:
            /* These are standalone events; the caller
             * (gk_TCB_List_Unlink) frees the walked event with gk_ECBFL_Link.
             * Freeing here too was a latent double free (same class as T1). */
            break;

        /* Standalone timer events of periodic / deferred-start /
         * delayed tasks.  They sit only in the timer list; the caller's
         * gk_ECBFL_Link unlinks them (WAITING_TIME -> gk_ECBTL_Unlink) and
         * frees them, so the callback is a no-op. */
        case G_ECBType_PERIODIC:
        case G_ECBType_ONESHOT_START_TIME:
        case G_ECBType_OSTimeDly:
            break;

        /* An ISR task that is WAITING for its interrupt has its event in
         * the trigger's waiting list (WEL).  Detach it — gk_RCBWEL_Unlink leaves
         * it UNLINKED so the caller's gk_ECBFL_Link just frees it.  No trigger
         * state change: a task waiting for an interrupt does not affect the
         * trigger's enable state. */
        case G_ECBType_EXT_ISR_WAITING:
            gk_RCBWEL_Unlink(pevent);
            break;

        /* An ISR task that was RELEASED by a firing interrupt has its
         * event in the trigger's granted list (GEL), and the trigger is disabled
         * until every released task completes.  Detach the event, then — if this
         * kill drained the granted list — re-arm the trigger, mirroring the
         * re-enable that gk_KERNEL_TASK_COMPLETE performs on the last ISR
         * completion.  Without this, killing the last released ISR task would
         * leave the interrupt line disabled forever.  (Only the RELEASE path
         * re-arms; killing a merely-WAITING task must NOT enable the trigger.) */
        case G_ECBType_EXT_ISR_RELEASE:
        {
            GS_RCB *ptrigger = (GS_RCB *) get_pointed_field(pevent, ECB_AssocRCB);
            gk_RCBGEL_Unlink(pevent);
            if ((ptrigger != (GS_RCB *) 0) &&
                (ptrigger->trigger.TRG_Enabled == G_FALSE) &&
                (get_pointed_field(ptrigger, RCB_NextRCBGEL) == (struct gs_ecb *) 0)) {
                gk_TRG_Enable(ptrigger);
            }
            break;
        }

		case G_ECBType_TRIGGER_TIMEOUT:
			G_DEBUG_WHILEFOREVER;
			break;

        default:
			G_DEBUG_WHILEFOREVER;
            gk_ECBFL_Link(pevent);
            break;
    }
}

/**
 *  \brief Called by the kernel when the system exits a frozen-mode event.
 *
 *  Executed inside the critical section.  Prints a diagnostic message to the
 *  debug UART identifying the processor that resumed normal execution.
 *
 *  \return void
 */
void gk_UnFrozenCallback(void)
{
	// Executed inside the critical section.
	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR SIGNAL in critical section\n");
	gu_printf("UNFROZEN Proc: %d\n", gm_ProcessorId());
}


/**
 *  \brief Called by the kernel when the system enters a frozen-mode event.
 *
 *  Executed inside the critical section.  Prints a diagnostic message to the
 *  debug UART identifying the processor that entered frozen mode.
 *
 *  \return void
 */
void gk_FrozenCallback(void)
{
	// Executed inside the critical section.
	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR SIGNAL in critical section\n");
	gu_printf("FROZEN Proc: %d\n", gm_ProcessorId());
}


/**
 *  \brief Recomputes the current priority of a task based on its scheduling
 *         list type (EDF or FP) and updates TCBReadyPriority / TCBRunPriority.
 *  \param [in] ptcb       Pointer to the TCB.
 *  \param [in] task_state State of the task (G_TCBState_READY, G_TCBState_RUNNING).
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \note The task state is validated by PRINT_ASSERT on debug builds (G_DEBUG_WHILEFOREVER_ENABLE == 1).
 *  \relates Task
 */
G_UINT32 gk_TaskPrioritySetCallback(GS_TCB *ptcb, enum tcbstate task_state)
{
    GS_ECB *pevent;
    GK_SAMPLE_FUNCTION_BEGIN(10011)

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid, PTCB= %p\n", (void *) ptcb);
    
    G_UINT32 lcb_type = get_pointed_field(get_pointed_field(ptcb, TCB_RDY_LCB_Index), LCBType);

    switch (lcb_type)
    {
        case GS_LCBTypeEDF:
            if (get_pointed_field(ptcb, TCBType) == G_TCBType_Periodic)
            {
                /* Get the next occurrence time associated with the task */
                pevent = get_pointed_field(ptcb, TCB_NextTCBAEL);
                while (pevent != (GS_ECB *) 0)
                {
                    if (get_pointed_field(pevent, ECBType) == G_ECBType_PERIODIC)
                    {
						set_pointed_field(ptcb, TCBReadyPriority, pevent->ECBValue.i64);
                        set_pointed_field(ptcb, TCBRunPriority, pevent->ECBValue.i64);
                        break;
                    }
                    pevent = (GS_ECB *) get_pointed_field(pevent, ECB_NextTCBAEL);
                }
                PRINT_ASSERT(((GS_ECB *) pevent != (GS_ECB *) 0),"ERROR G_ECBType_PERIODIC not found\n");
            }
            break;

        case GS_LCBTypeFP:           
            break;
			
		default:
			G_DEBUG_WHILEFOREVER;
			break;
    }

    GK_SAMPLE_FUNCTION_END(10011)
    return(G_TRUE);
}

/**
 * \ingroup GeMRTOSAPI
 * \brief   Schedules a task for its first execution at an optional tick offset.
 *
 * Computes the absolute start tick as gu_SystemTimeGet() + ticks_offset and
 * enqueues the task according to its type:
 *
 * - **OneShot**: if \p ticks_offset is 0 the task is inserted directly into the
 *   ready list; otherwise a #G_ECBType_ONESHOT_START_TIME timed event is created
 *   and the task is moved to the waiting list.
 * - **Periodic / Periodic_Skip**: a #G_ECBType_PERIODIC event is created at the
 *   computed start tick.  When the list discipline is EDF the task's ready and
 *   run priorities are set equal to the deadline (ECBValue).  If \p ticks_offset
 *   is 0 the task enters the ready list immediately; otherwise it waits.
 *
 * \param [in] ptcb         Pointer to the task control block to schedule.
 *                          Must satisfy gkm_TCB_IsValid() and must not be an ISR task.
 * \param [in] ticks_offset Ticks from the current system time before the task
 *                          becomes ready.  Pass 0 to make the task immediately ready.
 * \return #G_TRUE on success. Halts via #G_DEBUG_WHILEFOREVER for unknown task types.
 * \relates GS_TCB
 */
static G_UINT32 gk_StartTaskCallback(GS_TCB *ptcb, G_UINT64 ticks_offset)
{
    GS_ECB *pevent;
	
	GK_SAMPLE_FUNCTION_BEGIN(10086)
	
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE), "ERROR: ptcb= %p\n", (void *) ptcb);
    PRINT_ASSERT((ptcb->TCBType != G_TCBType_ISR), "ERROR: ptcb= %p\n", (void *) ptcb);

    /* Get the starting time of the task depending whether the RTOS is running or not */
    ticks_offset = gu_SystemTimeGet() + ticks_offset;
    
    switch (ptcb->TCBType){
        case G_TCBType_OneShot:
            if (ticks_offset == (G_UINT64) 0){
                gk_TCBRDYL_Link(ptcb);  /* Insert Task in Ready List                */
            } else {
                pevent = gk_ECB_GetFree(ptcb);
                pevent->ECBType = (G_UINT32) G_ECBType_ONESHOT_START_TIME;

                pevent->ECBValue.i64 = (G_UINT64) ticks_offset;
                gk_ECBTL_Link(pevent);
            
                gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);
            }
            break;
			
        case G_TCBType_Periodic:
		case G_TCBType_Periodic_Skip:

            pevent = gk_ECB_GetFree(ptcb);
			pevent->ECBType = (G_UINT32) G_ECBType_PERIODIC;
			
            pevent->ECBValue.i64 = (G_UINT64) ticks_offset;
            gk_ECBTL_Link(pevent);

			// If Schedule is EDF, then priority is equal to its deadline
			if (get_pointed_field(get_pointed_field(ptcb, TCB_RDY_LCB_Index), LCBType) == GS_LCBTypeEDF) {
				ptcb->TCBReadyPriority = pevent->ECBValue.i64;
                ptcb->TCBRunPriority   = pevent->ECBValue.i64;
			}
            if (ticks_offset == (G_UINT64) 0){
                /* Task Ready and Set next Release in Period Time from start time */
                gk_TCBRDYL_Link(ptcb);  /* Insert Task in Ready List                */
            }
            else
            {
                gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);
            }
            break;

        default:
            G_DEBUG_WHILEFOREVER;
            break;
    }
	GK_SAMPLE_FUNCTION_END(10086)
    return(G_TRUE);
}

OPTIMIZE_RESTORE
