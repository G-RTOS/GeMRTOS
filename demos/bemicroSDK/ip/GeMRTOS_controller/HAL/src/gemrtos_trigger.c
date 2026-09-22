/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS Trigger functions
 *  \details This file contains the trigger resource functions.
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


/** \class Trigger
 *  \details The concept of a "trigger resource" serves as a generalization of interrupts, enabling tasks to be resumed or restarted when a specific event occurs. Tasks can be registered with a trigger resource, and when the trigger resource is activated, the associated tasks are resumed or restarted.
The gu_TriggerCreate function creates a trigger resource and allows associating an external interrupt with it. The gu_TriggerRegisterTask function registers a task with a trigger resource. When the trigger resource is activated, it is disabled until all registered tasks that have been resumed or restarted transition to a waiting state for a new trigger.
Tasks associated with a trigger resource enter a waiting state for the trigger when they finish execution. Additionally, the gu_TriggerWait function explicitly places a task into the waiting for the trigger state.
Trigger resources can be associated with external events, such as hardware interrupts. In this case, all tasks registered with the trigger resource behave as Interrupt Service Routines (ISRs). Importantly, these tasks operate independently of each other, ensuring that multiple tasks can be associated with a single trigger resource.
Trigger resources can also be triggered from tasks' code using the gu_TriggerRelease function. When triggered, all registered tasks will be resumed or restarted if the trigger resource is enabled. The gu_TriggerEnable function enables a trigger resource, while the gu_TriggerDisable function disables it.
<h3>Enable and disable hook functions</h3>
The system provides flexibility in handling events by allowing hook functions to be executed when trigger resources are enabled or disabled. The gu_TriggerEnableHook function allows configuring a function to be executed before a trigger resource is enabled, while gu_TriggerDisableHook configures a function to be executed after the trigger resource is disabled. These hook functions are intended to execute specific code associated with the devices associated with the trigger resource.
<h3>Trigger flexibility</h3>
The system provides flexibility in handling events by allowing hook functions to be executed when trigger resources are enabled or disabled. The gu_TriggerEnableHook function allows configuring a function to be executed before a trigger resource is enabled, while gu_TriggerDisableHook configures a function to be executed after the trigger resource is disabled. These hook functions are intended to execute specific code associated with the devices associated with the trigger resource.
Trigger resources offer a flexible approach to handling system events, overcoming the limitations of traditional interrupts. For example, Universal Asynchronous Receiver/Transmitter (UART) devices, like many peripherals, typically group various internal events, such as input buffer full and output buffer empty, into a single hardware interrupt signal. This requires the UART driver to determine which event triggered the interrupt and execute the corresponding code.
With trigger resources, a trigger resource can be associated with the UART interrupt signal. The task code registered to this trigger resource then checks which event triggered the interrupt: input buffer full, output buffer empty, or both. If the input buffer is full, the trigger resource is activated, resuming all tasks associated with reading from the buffer.
This example demonstrates the flexibility of trigger resources by allowing for the specific handling of individual events within a single interrupt signal. This approach eliminates the need for complex logic within the interrupt service routine to determine the cause of the interrupt and execute the appropriate actions.
By leveraging trigger resources and hook functions, the GeMRTOS system provides a powerful and flexible mechanism for managing system events, enhancing the efficiency and modularity of the overall system design. 
\image html "https://gemrtos.com/wp-content/uploads/2023/04/trigger-scaled.jpg"
If the output buffer is empty, the system activates a trigger resource associated with writing tasks. This allows for an arbitrary number of reading and writing tasks to operate concurrently without interfering with each other.
The trigger resource associated with the input buffer should have defined hook functions to enable and disable the input buffer full event. Similarly, the trigger resource associated with the output buffer should have defined hook functions to enable and disable the output buffer empty event.
This approach allows for the virtualization and generalization of both internal and external events through the use of trigger resources.
 * \exception sec_fun_intro The Trigger category in GeMRTOS encompasses functions and macros that facilitate event-driven mechanisms within the real-time operating system. These functions enable tasks to respond to specific events, interrupts, or conditions, enhancing the system's interactivity and responsiveness. Triggers play a crucial role in synchronization, allowing tasks to be activated based on the occurrence of defined events, thereby optimizing resource utilization and improving overall system efficiency. 
 */

#include <gemrtos.h>

/** \brief Sentinel value stored in TRGIRQ_ID when a trigger is not associated
 *  with any hardware interrupt line (software-only trigger).
 *  Value is G_NUMBER_OF_IRQ_TCB + 1 â€” one past the last valid IRQ index.
 *  Pass irq_id = -1 to gu_TriggerCreate() to create a software-only trigger. */
#define G_TRIGGER_NO_IRQ  (G_NUMBER_OF_IRQ_TCB + 1U)

/* Forward declarations: defined after their only callers in this file. */
static void gk_TRG_EnableHook(GS_RCB *ptrigger, void (*code_callback)(void *), void *p_arg);
static void gk_TRG_DisableHook(GS_RCB *ptrigger, void (*code_callback)(void*), void *p_arg);

OPTIMIZE_CODE
 
/**
 * \brief Apply the active timeout-type policy for a trigger resource.
 *
 * Shared helper called by both \c gu_TriggerSetTimeoutType and \c gk_TRG_Enable
 * after confirming that the trigger is enabled.  Selects the correct ECBValue
 * computation variant based on \c ptrigger->trigger.TRGType, asserts the ECB
 * state/type invariants, and links the timeout ECB into the timed-event list
 * when a timeout is active.
 *
 * \param [in,out] ptrigger  Pointer to the trigger resource control block.
 */
static void gk_TRG_ApplyTimeoutType(GS_RCB *ptrigger)
{
	G_UINT32 trg_timeout_active = G_FALSE;

	switch (ptrigger->trigger.TRGType) {
		case GS_TRGTimeOutType_disabled:
			break;

		case GS_TRGTimeOutType_restart_when_timeout:
		case GS_TRGTimeOutType_no_restart:
			PRINT_ASSERT((ptrigger->trigger.TRGTimeout != 0), "ERROR trigger timeout cannot be zero");
			if (get_pointed_field(ptrigger->trigger.TRG_NextECB, ECBValue.i64) == (G_UINT64) 0) { /* First run */
				set_pointed_field(ptrigger->trigger.TRG_NextECB, ECBValue.i64, gu_SystemTimeGet() + (G_UINT64) ptrigger->trigger.TRGTimeout);
			}
			trg_timeout_active = G_TRUE;
			break;

		case GS_TRGTimeOutType_restart_when_enable:
			PRINT_ASSERT((ptrigger->trigger.TRGTimeout != 0), "ERROR trigger timeout cannot be zero");
			set_pointed_field(ptrigger->trigger.TRG_NextECB, ECBValue.i64, gu_SystemTimeGet() + (G_UINT64) ptrigger->trigger.TRGTimeout);
			trg_timeout_active = G_TRUE;
			break;

		default:
			G_DEBUG_WHILEFOREVER;
			break;
	}

	PRINT_ASSERT((ptrigger->trigger.TRG_NextECB->ECBState != GS_ECBState_WAITING_TIME), "ERROR trigger timeout should not be in GS_ECBState_WAITING_TIME");
	PRINT_ASSERT((ptrigger->trigger.TRG_NextECB->ECBType == G_ECBType_TRIGGER_TIMEOUT), "ERROR trigger ECB type= %u", (unsigned int) ptrigger->trigger.TRG_NextECB->ECBType);

	if (trg_timeout_active == G_TRUE) {
		gk_ECBTL_Link(ptrigger->trigger.TRG_NextECB);
	}
}

/** gu_TriggerSetTimeoutType
 * \brief Sets the trigger timeout type for a given Trigger. This function assigns the specified timeout type to the provided task control block's TRGType member.
 * \exception par_following The gu_TriggerSetTimeoutType requires the following parameters:
 * \param[in,out] ptrigger Pointer to the trigger (GS_RCB *) whose timeout type is to be set.
 * \param[in] type The timeout type to be assigned (valid values are: GS_TRGTimeOutType_disabled, GS_TRGTimeOutType_restart_when_enable, GS_TRGTimeOutType_restart_when_timeout, GS_TRGTimeOutType_no_restart.
 * \return G_UINT32 The function returns G_TRUE to indicate success.
 * \remark gu_TriggerSetTimeoutType
 * \relates Trigger
 */
G_UINT32 gu_TriggerSetTimeoutType(GS_RCB *ptrigger, GS_TRGTimeOutType type)
{
	gm_GeMRTOSCriticalSectionEnter();

        GS_ECB *pevent = ptrigger->trigger.TRG_NextECB;
        
        PRINT_ASSERT((ptrigger->trigger.TRGTimeout != (G_UINT64) 0), "ERROR in Triger timeout");
        PRINT_ASSERT(gkm_ECB_IsValid(ptrigger->trigger.TRG_NextECB), "ERROR not event in trigger");

        if (get_pointed_field(pevent, ECBState) == GS_ECBState_WAITING_TIME) {
            gk_ECBTL_Unlink(ptrigger->trigger.TRG_NextECB);
        }

        set_pointed_field(ptrigger, trigger.TRGType, type);

        if (ptrigger->trigger.TRG_Enabled == G_TRUE) {
            gk_TRG_ApplyTimeoutType(ptrigger);
        }

	gm_GeMRTOSCriticalSectionExit();
	
	return (G_TRUE);
}


/** gu_TriggerWait
 *  \brief The gu_TriggerWait function places the task into a waiting state for the trigger resource it is registered to. It can be executed anywhere in the task's code, and the same effect occurs when the task completes its execution (assuming it's not an infinite loop).
 *  \exception par_following The gu_TriggerWait function does not require any parameters, as the task automatically waits for the trigger resource it is associated with.
 *  \return The function returns G_TRUE if it is executed from within a task's code; otherwise, it returns G_FALSE if it is executed from the main code.
 *  \sa gu_TriggerCreate, gu_TriggerRegisterTask, gu_TriggerRelease
 *  \remark gu_trg_wait
 *  \relates Trigger
 */
GS_TRGStatus gu_TriggerWait(void)
{
	GS_TRGStatus result;

    PRINT_ASSERT((!(gm_TimeCountersResetGet())), "ERROR triggering before Running\n");
	gm_GeMRTOSCriticalSectionEnter();
		GS_TCB *ptcb = gk_PCB_GetCurrentTCB();

		result = gk_TRG_Wait(ptcb);

		PRINT_ASSERT(((result == G_TRGStatus_triggered) || (result == G_TRGStatus_timeout)), "ERROR status not valid= %u", (unsigned int) result);
		PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);
	gm_GeMRTOSCriticalSectionExit();
	
	return(result);
}


/**
 *  \brief Suspends the current ISR task until its next trigger event.
 *
 *  Moves the task ECB from the granted list back to the wait list and
 *  re-enables the trigger if no other granted tasks remain.  Called
 *  implicitly when an ISR task completes its handler body, and directly
 *  by gu_TriggerWait().
 *
 *  \param [in] ptcb Pointer to the TCB of the ISR task.
 *  \return G_TRGStatus_triggered when woken by a trigger release;
 *          G_TRGStatus_timeout when woken by a trigger timeout.
 *  \relates Trigger
 */
GS_TRGStatus gk_TRG_Wait (GS_TCB *ptcb) 
{
	GS_RCB *ptrigger;
    
	GK_SAMPLE_FUNCTION_BEGIN(10060)
    
	ptrigger = get_pointed_field(ptcb, TCB_INTNumber);
	
    PRINT_ASSERT(((int) ptrigger != -1),"ERROR Task -%s- is not IRQ ISR= %d\n", ptcb->TCB_description, (int) ptrigger);
    PRINT_ASSERT((ptcb->TCBType == G_TCBType_ISR),"ERROR TCBType not G_TCBType_ISR= %d\n", (int) ptcb->TCBType);
    
    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
    PRINT_ASSERT((ptrigger->RCB_NextRCBGEL != (struct gs_ecb *) 0),"ERROR IRQ complete but not count= %p, %s\n", (void *) ptrigger->RCB_NextRCBGEL, ptcb->TCB_description);
    
    GS_ECB *pevent = gk_TCB_in_RCBGEL(ptrigger, ptcb);
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid, task not registered: %s\n", ptcb->TCB_description);
    PRINT_ASSERT((pevent->ECBType == (G_UINT32) G_ECBType_EXT_ISR_RELEASE), "ERROR ECBType is not valid%d\n",(int)pevent->ECBType);
    
    gk_RCBGEL_Unlink(pevent);
    set_pointed_field(pevent, ECBType, (G_UINT32) G_ECBType_EXT_ISR_WAITING);
    gk_RCBWEL_Link(ptrigger, ptcb, pevent);
	
    if (get_pointed_field(ptrigger, RCB_NextRCBGEL) == (struct gs_ecb *) 0) {
        gk_TRG_Enable(ptrigger);
    }

	if (ptcb->TCBState == G_TCBState_RUNNING) {
		gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);		
	}

	GK_SAMPLE_FUNCTION_END(10060)
   
    return (ptrigger->trigger.TRGStatus);
}


/** gu_TriggerRelease  
 *  \brief The function activates a trigger resource. If the trigger resource is enabled and all associated tasks are in a waiting state for the trigger, then the tasks are resumed or restarted.
 *  \exception par_following The trigger resource is activated using the following parameter:
 *  \param [in] ptrigger Handle of the trigger.
 *  \return The trigger status after activation (GS_TRGStatus).
 *  \sa gu_TriggerCreate, gu_TriggerRegisterTask, gu_TriggerWait
 *  \relates Trigger
 *  \remark gu_trg_release
 */
GS_TRGStatus gu_TriggerRelease(GS_RCB *ptrigger)
{
    gm_GeMRTOSCriticalSectionEnter();
		
		PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR trigger not valid\n");
		ptrigger->trigger.TRGStatus = G_TRGStatus_triggered;
        gk_TRG_Release(ptrigger);	
		
    gm_GeMRTOSCriticalSectionExit();
    return (ptrigger->trigger.TRGStatus);
}


/**
 *  \brief Fires the trigger: disables the hardware interrupt line and
 *         moves all waiting ISR tasks to the ready list.
 *
 *  Called by gk_ENTRY_IRQ_HANDLER() on a hardware interrupt and by
 *  gu_TriggerRelease() for software triggers.
 *
 *  \param [in] ptrigger Handle of the trigger resource.
 *  \relates Trigger
 */
void gk_TRG_Release (GS_RCB *ptrigger)
{
    GS_ECB *pevent;
    GS_TCB *ptcb;
	
	GK_SAMPLE_FUNCTION_BEGIN(10061)	

    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");


    if (ptrigger->trigger.TRG_Enabled == G_TRUE) {
		gk_TRG_Disable(ptrigger);

		// Trigger all the tasks        
		while ((pevent = get_pointed_field(ptrigger, RCB_NextRCBWEL)) != (GS_ECB *) 0) {
			
			PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid\n");
			PRINT_ASSERT((pevent->ECB_AssocRCB == (struct g_rcb *) ptrigger),"ERROR RCB is not valid\n");
			
			PRINT_ASSERT((pevent->ECBType == (G_UINT32) G_ECBType_EXT_ISR_WAITING), "ERROR ECBType is not valid%d\n",(int)pevent->ECBType);

			set_pointed_field(pevent, ECBType, (G_UINT32) G_ECBType_EXT_ISR_RELEASE);
			gk_RCBWEL_Unlink(pevent);
			
			ptcb = get_pointed_field(pevent, ECB_AssocTCB);
			PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
			
			gk_RCBGEL_Link(ptrigger, ptcb, pevent);
			PRINT_ASSERT((ptcb->TCBType == G_TCBType_ISR),"ERROR in TCB Type= %d\n", ptcb->TCBType);
			PRINT_ASSERT((ptcb->TCBState != G_TCBState_RUNNING), "ERROR TCB is running but should be waiting");

			/* Insert Task in Ready List                */
			gk_TCB_Unlink((GS_TCB *) ptcb);         
			gk_TCBRDYL_Link((GS_TCB *) ptcb);
		}
	}
	GK_SAMPLE_FUNCTION_END(10061)
}

/**
 *  \brief Associates a task with a trigger resource, converting it to an ISR task.
 *  \exception par_following The task registration with the trigger resource is performed using the following parameters:
 *  \param [in] ptcb This is a pointer to the GS_TCB structure of the task to be associated with the trigger resource.
 *  \param [in] ptrigger Handle of the trigger.
 *  \return G_TRUE if the task registration was successful; G_FALSE if no free ECB was available.
 *  \sa gu_TriggerCreate, gu_TriggerWait, gu_TriggerRelease
 *  \remark gu_trg_register_task
 *  \relates Trigger
 */
G_UINT32 gu_TriggerRegisterTask(struct gs_tcb *ptcb, GS_RCB *ptrigger)
{

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");

    gm_GeMRTOSCriticalSectionEnter();
        // Set the number of associated interrupt (valid only when TCBType == G_TCBType_ISR)
        // and link to the ISR linked list !!! check if task is running for consistency of Count
        set_pointed_field(ptcb, TCBType, G_TCBType_ISR);
        set_pointed_field(ptcb, TCB_INTNumber, ptrigger);
		
        PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid %p\n The interrupt may not be connected\n",(void *) ptrigger);

        if (get_pointed_field(ptcb,TCBState) == G_TCBState_UNLINKED) gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);
        
        // Get an associated event
        GS_ECB *pevent = gk_ECB_GetFree(ptcb);
        if (pevent == (GS_ECB *) 0) {
            gm_GeMRTOSCriticalSectionExit();
            return G_FALSE;
        }

        pevent->ECBValue.i64 = (G_UINT64) 1;
        
        PRINT_ASSERT(((ptcb->TCBState == G_TCBState_RUNNING) || (ptcb->TCBState == G_TCBState_READY) || (ptcb->TCBState == G_TCBState_WAITING) || (ptcb->TCBState == G_TCBState_WAITING_COMPLETED)),"ERROR ptcb->TCBState is not valid= %d\n",ptcb->TCBState);

        if ((ptcb->TCBState == G_TCBState_WAITING_COMPLETED) || (ptcb->TCBState == G_TCBState_WAITING)) {
            set_pointed_field(pevent, ECBType, (G_UINT32) G_ECBType_EXT_ISR_WAITING);			
            gk_RCBWEL_Link(ptrigger, ptcb, pevent);
        } else {
            set_pointed_field(pevent, ECBType, (G_UINT32) G_ECBType_EXT_ISR_RELEASE);
            gk_RCBGEL_Link(ptrigger, ptcb, pevent);
        }

    gm_GeMRTOSCriticalSectionExit();

    return (G_TRUE);
}

/**gu_TriggerEnable
 *  \brief The gu_TriggerEnable function enables the trigger resource, allowing it to be activated using either the gu_TriggerRelease function or the associated hardware interrupt.
 *  \exception par_following The enabling of the trigger resource is performed with the following parameters:
 *  \param [in] ptrigger Handle of the trigger.
 *  \return The function returns G_TRUE if the operation was successful.
 *  \sa gu_TriggerCreate, gu_TriggerDisable, gu_TriggerEnableHook, gu_TriggerRelease, gu_TriggerRegisterTask, gu_TriggerWait
 *  \remark gu_trg_enable
 *  \relates Trigger
 */
G_UINT32 gu_TriggerEnable(GS_RCB *ptrigger)
{
    gm_GeMRTOSCriticalSectionEnter();
        gk_TRG_Enable(ptrigger);
    gm_GeMRTOSCriticalSectionExit();
    
	return (G_TRUE);
}


G_UINT32 gk_TRG_Enable(GS_RCB *ptrigger)
{
	GK_SAMPLE_FUNCTION_BEGIN(10062)

    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");

	gk_TRG_ApplyTimeoutType(ptrigger);

	if (ptrigger->trigger.TRGIRQ_ID < G_NUMBER_OF_IRQ_TCB )
	{
		// It is associated to a hardware interrupt
		PRINT_ASSERT((ptrigger->trigger.TRGIRQ_ID < G_NUMBER_OF_IRQ_TCB),"ERROR RCB is not valid\n");
		gm_IrqEnable(ptrigger->trigger.TRGIRQ_ID);
	}

	// Execute the enable callback function
	if (ptrigger->trigger.enable_code != (void *) 0)
		ptrigger->trigger.enable_code(ptrigger->trigger.enable_arg);
	
	ptrigger->trigger.TRG_Enabled = G_TRUE;
	ptrigger->trigger.TRGStatus = G_TRGStatus_waiting;
	
	GK_SAMPLE_FUNCTION_END(10062)
	
	return (G_TRUE);
}

G_UINT32 gk_TriggerTimeout(GS_ECB *ptimed)
{
	GS_RCB *ptrigger = get_pointed_field(ptimed,ECB_AssocRCB);
	
	GK_SAMPLE_FUNCTION_BEGIN(10063)
	
	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);
	PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB not valid in gk_TriggerTimeout\n");
	PRINT_ASSERT((ptrigger->trigger.TRG_NextECB->ECBState == GS_ECBState_UNLINKED), "ERROR trigger timeout should be in GS_ECBState_UNLINKED");

	PRINT_ASSERT((ptrigger->trigger.TRG_NextECB->ECBType == G_ECBType_TRIGGER_TIMEOUT), "ERROR trigger ECB type= %u", (unsigned int) ptrigger->trigger.TRG_NextECB->ECBType);
	
	if (ptrigger->trigger.TRGType == GS_TRGTimeOutType_restart_when_timeout) {
		set_pointed_field(ptrigger->trigger.TRG_NextECB, ECBValue.i64, ptrigger->trigger.TRG_NextECB->ECBValue.i64 + ptrigger->trigger.TRGTimeout);
	}
	
	PRINT_ASSERT((ptrigger->trigger.TRG_Enabled == G_TRUE), "ERROR trigger.TRG_Enabled");
	PRINT_ASSERT((ptrigger->trigger.TRGStatus == G_TRGStatus_waiting), "ERROR trigger.TRGStatus");
	PRINT_ASSERT((get_pointed_field(ptrigger, RCB_NextRCBGEL) == (struct gs_ecb *) 0), "ERROR Enabling trigger with granting tasks");

	ptrigger->trigger.TRGStatus = G_TRGStatus_timeout;
	gk_TRG_Release(ptrigger);
	
	GK_SAMPLE_FUNCTION_END(10063)

	return (G_TRUE);
}


/**gu_TriggerDisable
 *  \brief The gu_TriggerDisable function disables the trigger resource, preventing it from being activated using either the gu_TriggerRelease function or the associated hardware interrupt.
 *  \exception par_following The disabling of the trigger resource is performed with the following parameter:
 *  \param [in] ptrigger Handle of the trigger.
 *  \return The function returns G_TRUE if the operation was successful.
 *  \sa gu_TriggerCreate, gu_TriggerEnable, gu_TriggerDisableHook, gu_TriggerRelease
 *  \remark gu_trg_disable
 *  \relates Trigger
 */
G_UINT32 gu_TriggerDisable(GS_RCB *ptrigger)	   
{
    gm_GeMRTOSCriticalSectionEnter();
        gk_TRG_Disable(ptrigger);
    gm_GeMRTOSCriticalSectionExit();
    
	return (G_TRUE);
}

G_UINT32 gk_TRG_Disable(GS_RCB *ptrigger)	   
{
	GK_SAMPLE_FUNCTION_BEGIN(10064)	

	PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
	
    if (ptrigger->trigger.TRGIRQ_ID != G_TRIGGER_NO_IRQ)
    {
		// It is associated to a hardware interrupt
        PRINT_ASSERT((ptrigger->trigger.TRGIRQ_ID < G_NUMBER_OF_IRQ_TCB),"ERROR RCB is not valid\n");
        gm_IrqDisable(ptrigger->trigger.TRGIRQ_ID);
    }
	// Execute the disable callback function
	if (ptrigger->trigger.disable_code != (void *) 0)
		(ptrigger->trigger.disable_code)(ptrigger->trigger.disable_arg);

	ptrigger->trigger.TRG_Enabled = G_FALSE;

	PRINT_ASSERT((gkm_ECB_IsValid(ptrigger->trigger.TRG_NextECB) == G_TRUE),"ERROR in pevent %p\n", ptrigger->trigger.TRG_NextECB);
	PRINT_ASSERT((ptrigger->trigger.TRG_NextECB->ECBType == G_ECBType_TRIGGER_TIMEOUT), "ERROR trigger ECB type= %u", (unsigned int) ptrigger->trigger.TRG_NextECB->ECBType);

	if (ptrigger->trigger.TRG_NextECB->ECBState != GS_ECBState_UNLINKED) {
		gk_ECBTL_Unlink(ptrigger->trigger.TRG_NextECB);
	}
	PRINT_ASSERT((ptrigger->trigger.TRG_NextECB->ECBState != GS_ECBState_WAITING_TIME),"ERROR trigger timeout event error");
	PRINT_ASSERT((ptrigger->trigger.TRG_NextECB->ECBType == G_ECBType_TRIGGER_TIMEOUT), "ERROR trigger ECB type= %u", (unsigned int) ptrigger->trigger.TRG_NextECB->ECBType);

	GK_SAMPLE_FUNCTION_END(10064)

	return (G_TRUE);
}

/**
 * \brief No-operation hook for triggers that have no associated IRQ action.
 *
 * Used as a safe default for the enable-hook and disable-hook fields of a
 * \c T_TRIGGER_RESOURCE when no hardware interrupt action is required.
 * The function suppresses the unused-parameter warning and returns NULL.
 *
 * \param [in] arg  Unused context pointer (stored in the trigger resource struct).
 * \return NULL (void *) always.
 */
void *gk_TRG_null_hook(const void *arg) /* arg never read; const per MISRA C:2012 Rule 8.13 */
{
    (void) arg;
    return ((void *) 0);
}

/**
 *  \brief The function creates a trigger resource. It accepts an irq_id argument to allow associating the trigger resource with a hardware interrupt.
 *  \exception par_following The trigger is created and initilized with the following paramenters:
 *  \param [in] irq_id The irq_id argument specifies the number of the hardware interrupt to associate with the trigger resource. Setting this argument to -1 indicates that no association with a hardware interrupt is desired.
 *  \param [in] ticks_to_wait Timeout for trigger in number of ticks.
 *  \return The gu_TriggerCreate function returns a pointer to the newly created trigger resource. This pointer must be used to reference the trigger resource in all subsequent trigger-related functions.
 *  \sa gu_TriggerEnable, gu_TriggerDisable, gu_TriggerEnableHook, gu_TriggerDisableHook, gu_TriggerRegisterTask, gu_TriggerRelease, gu_TriggerWait
 *  \remark gu_trg_create
 *  \relates Trigger
 */
GS_RCB *gu_TriggerCreate(int irq_id, G_UINT64 ticks_to_wait)
{
    GS_RCB *prcb = (GS_RCB *) 0;
    int prcb_newly_alloc = 0; /* 1 when prcb was taken from the free pool here */

    gm_GeMRTOSCriticalSectionEnter();

        if (irq_id >= 0 && irq_id < G_NUMBER_OF_IRQ_TCB)
        {   // It is an external interrupt
            if (g_kcb.KCB_NextRCBTRGL[irq_id] == (GS_RCB *) 0)
            {
                prcb = gk_RCB_GetFree();
                if (prcb == (GS_RCB *) 0) {
                    gm_GeMRTOSCriticalSectionExit();
                    return (GS_RCB *) 0;
                }
                prcb_newly_alloc = 1;
                g_kcb.KCB_NextRCBTRGL[irq_id] = prcb;
                prcb->trigger.TRGIRQ_ID = (G_UINT32) irq_id ;
            }
            else
            {
                prcb = g_kcb.KCB_NextRCBTRGL[irq_id];
				PRINT_ASSERT((prcb->trigger.TRGIRQ_ID == (G_UINT32) irq_id),"ERROR in IRQ creation");
            }
			PRINT_ASSERT((gkm_RCB_IsValid(g_kcb.KCB_NextRCBTRGL[irq_id]) == G_TRUE),"ERROR RCB is not valid\n");
			gm_IrqDisable(prcb->trigger.TRGIRQ_ID);
        }
        else
        {
            prcb = gk_RCB_GetFree();
            if (prcb == (GS_RCB *) 0) {
                gm_GeMRTOSCriticalSectionExit();
                return (GS_RCB *) 0;
            }
            prcb_newly_alloc = 1;
            prcb->trigger.TRGIRQ_ID = G_TRIGGER_NO_IRQ;
        }
        
		set_pointed_field(prcb, RCBType, GS_RCBType_TRIGGER);
		
        // Set NULL callbacks for enable and disable
	    prcb->trigger.enable_code = (void *) 0;
		prcb->trigger.disable_code = (void *) 0;


		prcb->trigger.TRG_NextECB = gk_ECB_GetFree(0);
		if (prcb->trigger.TRG_NextECB == (GS_ECB *) 0) {
			if (prcb_newly_alloc) {
				if (irq_id >= 0 && irq_id < G_NUMBER_OF_IRQ_TCB) {
					g_kcb.KCB_NextRCBTRGL[irq_id] = (struct g_rcb *) 0;
				}
				set_pointed_field(prcb, RCBType, GS_RCBType_FREE);
				gk_RCBFL_Link(prcb);
			}
			gm_GeMRTOSCriticalSectionExit();
			return (GS_RCB *) 0;
		}

		prcb->trigger.TRG_NextECB->ECB_AssocRCB = prcb;
		prcb->trigger.TRG_NextECB->ECBType = G_ECBType_TRIGGER_TIMEOUT;
		prcb->trigger.TRG_NextECB->ECB_AssocTCB = (GS_TCB *) 0;
        prcb->trigger.TRG_NextECB->ECBValue.i64 = (G_UINT64) 0;
		
		prcb->trigger.TRGTimeout = ticks_to_wait;
		
		if (prcb->trigger.TRGTimeout == 0) {
			prcb->trigger.TRGType = GS_TRGTimeOutType_disabled;
		} else {
			prcb->trigger.TRGType = GS_TRGTimeOutType_restart_when_enable;
		}
		
		prcb->trigger.TRG_Enabled = G_FALSE;
		prcb->trigger.TRGStatus = G_TRGStatus_not_valid;

		PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR ECB not valid\n");

    gm_GeMRTOSCriticalSectionExit();

    return (prcb);
}

/** gu_TriggerDestroy
 *  \brief Destroys a trigger created by gu_TriggerCreate, returning its Resource
 *         Control Block and its timeout event to the free pools.  The trigger must
 *         have no registered ISR tasks waiting (WEL) and no released ISR tasks
 *         pending (GEL); if any remain the function does nothing and returns
 *         G_FALSE â€” the caller must remove those tasks first (e.g. gu_TaskKill).
 *         On success the trigger is disabled, detached from the kernel IRQ
 *         registry, its timeout ECB is freed, and the RCB returns to the pool.
 *  \exception par_following The function accepts one parameter:
 *  \param [in] ptrigger A pointer to the GS_RCB of the trigger to destroy.
 *  \return G_TRUE if the trigger was destroyed; G_FALSE if tasks are still registered.
 *  \sa gu_TriggerCreate, gu_TriggerRegisterTask, gu_TriggerDisable
 *  \relates Trigger
 */
G_UINT32 gu_TriggerDestroy(GS_RCB *ptrigger)
{
    G_UINT32 result = G_FALSE;
    gm_GeMRTOSCriticalSectionEnter();
        PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB not valid, %p\n", (void *) ptrigger);
        PRINT_ASSERT((ptrigger->RCBType == (G_UINT32) GS_RCBType_TRIGGER),"ERROR not a trigger, RCBType= %d\n", (int) ptrigger->RCBType);

        /* Idle only: no registered (WEL) and no released (GEL) ISR task. */
        if ((get_pointed_field(ptrigger, RCB_NextRCBGEL) == (struct gs_ecb *) 0) &&
            (get_pointed_field(ptrigger, RCB_NextRCBWEL) == (struct gs_ecb *) 0)) {

            /* Disable (unlinks the timeout ECB from the timer list, masks the IRQ). */
            if (ptrigger->trigger.TRG_Enabled == G_TRUE) {
                gk_TRG_Disable(ptrigger);
            }

            /* Remove the trigger from the hardware IRQ registry so the line can be
             * re-created later. */
            if (ptrigger->trigger.TRGIRQ_ID < (G_UINT32) G_NUMBER_OF_IRQ_TCB) {
                g_kcb.KCB_NextRCBTRGL[ptrigger->trigger.TRGIRQ_ID] = (struct g_rcb *) 0;
            }

            /* Free the always-allocated trigger timeout event. */
            if (ptrigger->trigger.TRG_NextECB != (GS_ECB *) 0) {
                gk_ECBFL_Link(ptrigger->trigger.TRG_NextECB);
                ptrigger->trigger.TRG_NextECB = (GS_ECB *) 0;
            }

            set_pointed_field(ptrigger, RCBType, GS_RCBType_FREE);
            gk_RCBFL_Link(ptrigger);
            result = G_TRUE;
        }
    gm_GeMRTOSCriticalSectionExit();

    return (result);
}

/** gu_TriggerEnableHook
 *  \brief The gu_TriggerEnableHook sets the hook function to be called before the trigger resource is enabled.
 *  \exception par_following The gu_TriggerEnableHook function requires the following paramenters:
 *  \param [in] ptrigger Handle of the trigger.
 *  \param [in] code_callback This parameter defines the name of the function to be executed as a hook function when the trigger resource is enabled.
 *  \param [in] p_arg This parameter represents the value to be passed to the hook function when it is called. This allows the same hook function to be used for multiple trigger resources with different parameter values.
 *  \return The function returns G_TRUE if the enable hook function was successfully configured; otherwise, it returns G_FALSE.
 *  \sa gu_TriggerCreate, gu_TriggerEnable, gu_TriggerDisableHook
 *  \remark gu_trg_enable_hook
 *  \relates Trigger
 */
G_UINT32 gu_TriggerEnableHook(GS_RCB *ptrigger, void (*code_callback)(void *), void *p_arg)
{
    gm_GeMRTOSCriticalSectionEnter();
        gk_TRG_EnableHook(ptrigger, code_callback, (void *) p_arg);
    gm_GeMRTOSCriticalSectionExit();
    return (G_TRUE);
}

static void gk_TRG_EnableHook(GS_RCB *ptrigger, void (*code_callback)(void *), void *p_arg)
{
	GK_SAMPLE_FUNCTION_BEGIN(10065)
    
    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
    
    ptrigger->trigger.enable_code = code_callback;
    ptrigger->trigger.enable_arg  = p_arg;
	
	GK_SAMPLE_FUNCTION_END(10065)
}

/**
 *  \brief The gu_TriggerDisableHook function sets the hook function to be called after the trigger resource is disabled.
 *  \exception par_following The disable hook function is specified using the following parameters:
 *  \param [in] ptrigger Handle of the trigger.
 *  \param [in] code_callback This parameter defines the name of the function to be executed as a hook function when the trigger resource is disabled.
 *  \param [in] p_arg This parameter represents the value to be passed to the hook function when it is called. This allows the same hook function to be used for multiple trigger resources with different parameter values.
 *  \return The function returns G_TRUE if the disable hook function was successfully configured; otherwise, it returns G_FALSE.
 *  \sa gu_TriggerCreate, gu_TriggerDisable, gu_TriggerEnable, gu_TriggerEnableHook, gu_TriggerRelease
 *  \remark gu_trg_disable_hook
 *  \relates Trigger
 */
G_UINT32 gu_TriggerDisableHook(GS_RCB *ptrigger, void (*code_callback)(void *), void *p_arg)
{
    gm_GeMRTOSCriticalSectionEnter();
        gk_TRG_DisableHook(ptrigger, code_callback, (void *) p_arg);
    gm_GeMRTOSCriticalSectionExit();
    return (G_TRUE);
}

static void gk_TRG_DisableHook(GS_RCB *ptrigger, void (*code_callback)(void*), void *p_arg)
{
    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
    
    ptrigger->trigger.disable_code = code_callback;
    ptrigger->trigger.disable_arg  = p_arg;
}


OPTIMIZE_RESTORE

