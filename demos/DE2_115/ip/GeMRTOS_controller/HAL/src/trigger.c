/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Message queue functions
 *  \details This file contains the Message queue functions.
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


OPTIMEZE_CODE(0)

/** gu_TriggerWait
 *  \brief The gu_TriggerWait function places the task into a waiting state for the trigger resource it is registered to. It can be executed anywhere in the task's code, and the same effect occurs when the task completes its execution (assuming it's not an infinite loop).
 *  \exception par_following The gu_TriggerWait function does not require any parameters, as the task automatically waits for the trigger resource it is associated with.
 *  \return The function returns G_TRUE if it is executed from within a task's code; otherwise, it returns G_FALSE if it is executed from the main code.
 *  \sa gu_TriggerCreate, gu_TriggerRegisterTask, gu_TriggerRelease
 *  \remark gu_trg_wait
 *  \relates Trigger
 */
G_INT32 gu_TriggerWait(void)
{
    PRINT_ASSERT((G_Running == G_TRUE), "ERROR triggering before Running\n");
	gm_GeMRTOSCriticalSectionEnter;
	GS_TCB *ptcb = gk_PCB_GetCurrentTCB();
	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
	gk_TRG_Wait(ptcb);
	gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);
	gm_GeMRTOSCriticalSectionExit; 
	return(G_TRUE);
}


/**gk_TRG_Wait
 *  \brief 
 *  Executed when Task finishes as ISR. If remains ready, then it will be not released next Interrupt event
 *  \param [in] ptcb Pointer to the TCB of the ISR task
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Include completes but task remains ready.
 *  \relates Trigger
 */
G_INT32 gk_TRG_Wait (GS_TCB *ptcb) {
    
    int irq_nbr = get_pointed_field(ptcb, TCB_INTNumber);
    
    PRINT_ASSERT((irq_nbr != -1),"ERROR Task is not IRQ ISR= %d\n", (int) irq_nbr);
    PRINT_ASSERT((ptcb->TCBType == G_TCBType_ISR),"ERROR TCBType not G_TCBType_ISR= %d\n", (int) ptcb->TCBType);
    
    G_RCB *ptrigger;
    if (irq_nbr <= G_NUMBER_OF_IRQ_TCB) ptrigger = (G_RCB *) g_kcb.KCB_NextRCBTRGL[irq_nbr];
    else ptrigger = (G_RCB *) irq_nbr;
    
    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
    PRINT_ASSERT((ptrigger->RCB_NextRCBGEL != (struct gs_ecb *) 0),"ERROR IRQ complete but not count= %p\n", (void *) ptrigger->RCB_NextRCBGEL);
    
    GS_ECB *pevent = gk_TCB_in_RCBGEL(ptrigger, ptcb);
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid\n");
    PRINT_ASSERT((pevent->ECBType == (G_INT32) G_ECBType_EXT_ISR_RELEASE), "ERROR ECBType is not valid%d\n",(int)pevent->ECBType);
    
    gk_RCBGEL_Unlink(pevent);
    set_pointed_field(pevent, ECBType, (G_INT32) G_ECBType_EXT_ISR_WAITING);
    gk_RCBWEL_Link(ptrigger, ptcb, pevent);

    if (get_pointed_field(ptrigger, RCB_NextRCBGEL) == (struct gs_ecb *) 0) {
        gk_TRG_Enable((int) ptrigger);
    }
    
    return (G_TRUE);
}


/** gu_TriggerRelease  
 *  \brief The function activates a trigger resource. If the trigger resource is enabled and all associated tasks are in a waiting state for the trigger, then the tasks are resumed or restarted.
 *  \exception par_following The trigger resource is activated using the following parameter:
 *  \param [in] irq_nbr This parameter represents either the IRQ number of the hardware interrupt associated with the trigger resource or the pointer to the trigger resource returned by the gu_TriggerCreate function. If the irq_nbr is a pointer, it should be cast to an integer (int) for use within the function.
 *  \return The function returns G_TRUE if the trigger resource was successfully activated; otherwise, it returns G_FALSE.
 *  \sa gu_TriggerCreate, gu_TriggerRegisterTask, gu_TriggerWait
 *  \relates Trigger  
 *  \remark gu_trg_release
 */
G_INT32 gu_TriggerRelease (int irq_nbr) {
    gm_GeMRTOSCriticalSectionEnter;
        gk_TRG_Release((G_INT32) irq_nbr);
    gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}


/**gk_TRG_Release
 *  \brief 
 *  Executed when an interrupt is triggered. It is called from gk_ENTRY_IRQ_HANDLER()
 *  \param [in] irq_nbr Number index of the interrupt
 *  \todo Verify if task is waiting to enable the interrupt, otherwise change the Type of the ISR
 *  \relates Trigger
 */
void gk_TRG_Release (G_INT32 irq_nbr)
{
    GS_ECB *pevent;
    GS_TCB *ptcb;
    
    G_RCB *ptrigger;
    if (irq_nbr <= G_NUMBER_OF_IRQ_TCB) ptrigger = (G_RCB *) g_kcb.KCB_NextRCBTRGL[irq_nbr];
    else ptrigger = (G_RCB *) irq_nbr;
    
    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
    PRINT_ASSERT((ptrigger->RCB_NextRCBGEL == (GS_ECB *) 0),"ERROR triggering\n");

    gk_TRG_Disable((unsigned int) ptrigger);

    if (get_pointed_field(ptrigger, RCB_NextRCBGEL) == (GS_ECB *) 0)
    {   // Trigger all the tasks        
        while ((pevent = get_pointed_field(ptrigger, RCB_NextRCBWEL)) != (GS_ECB *) 0) {
			
            PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid\n");
            PRINT_ASSERT((pevent->ECB_AssocRCB == (struct g_rcb *) ptrigger),"ERROR RCB is not valid\n");
            
            PRINT_ASSERT((pevent->ECBType == (G_INT32) G_ECBType_EXT_ISR_WAITING), "ERROR ECBType is not valid%d\n",(int)pevent->ECBType);

            gk_RCBWEL_Unlink(pevent);
            
            set_pointed_field(pevent, ECBType, (G_INT32) G_ECBType_EXT_ISR_RELEASE);
			ptcb = get_pointed_field(pevent, ECB_AssocTCB);
			
            gk_RCBGEL_Link(ptrigger, ptcb, pevent);
            
            PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
            PRINT_ASSERT((ptcb->TCBType == G_TCBType_ISR),"ERROR in TCB Type= %d\n", ptcb->TCBType);

            /* Insert Task in Ready List                */
            gk_TCB_Unlink((GS_TCB *) ptcb);         
            gk_TCBRDYL_Link((GS_TCB *) ptcb);

        }
    }
}

/**gu_TriggerRegisterTask
 *  \brief The gu_TriggerRegisterTask function associates a task with a trigger resource.
 *  \exception par_following The task registration with the trigger resource is performed using the following parameters:
 *  \param [in] ptcb This is a pointer to the GS_TCB structure of the task to be associated with the trigger resource.
 *  \param [in] irq_nbr This is either the IRQ number of the hardware interrupt associated with the trigger or the pointer to the trigger resource returned by the gu_TriggerCreate function, cast to an integer (int).
 *  \return The gu_TriggerRegisterTask function returns G_TRUE if the task registration with the trigger resource is successful; otherwise, it returns G_FALSE.
 *  \sa gu_TriggerCreate, gu_TriggerWait, gu_TriggerRelease
 *  \todo Check if IRQ number or TCB pointer al valid.
 *  \todo Implement the reverse function to convert a ISR into a task
 *  \remark gu_trg_register_task
 *  \relates Trigger
 */
G_INT32 gu_TriggerRegisterTask(struct gs_tcb *ptcb, G_INT32 irq_nbr)
{
    G_RCB *ptrigger;

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");

    gm_GeMRTOSCriticalSectionEnter;
        // Set the number of associated interrupt (valid only when TCBType == G_TCBType_ISR)
        // and link to the ISR linked list !!! check if task is running for consistency of Count
        set_pointed_field(ptcb, TCBType, G_TCBType_ISR);
        set_pointed_field(ptcb, TCB_INTNumber, (int) irq_nbr);
		
        // Link to trigger resource
        if (irq_nbr <= (unsigned int) G_NUMBER_OF_IRQ_TCB) ptrigger = g_kcb.KCB_NextRCBTRGL[irq_nbr];
        else ptrigger = (G_RCB *) irq_nbr;
        PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid %p\n The interrupt may not be connected\n",(void *) ptrigger);

        if (get_pointed_field(ptcb,TCBState) == G_TCBState_UNLINKED) gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED);
        
        // Get an associated event
        GS_ECB *pevent = gk_ECB_GetFree();
		PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid\n");
        pevent->ECBValue.i64 = (G_INT64) 1;
        
        PRINT_ASSERT(((ptcb->TCBState == G_TCBState_RUNNING) || (ptcb->TCBState == G_TCBState_READY) || (ptcb->TCBState == G_TCBState_WAITING) || (ptcb->TCBState == G_TCBState_WAITING_COMPLETED)),"ERROR ptcb->TCBState is not valid= %d\n",ptcb->TCBState);

        // if ((ptcb->TCBState == G_TCBState_WAITING_COMPLETED) || (ptcb->TCBState == G_TCBState_WAITING)) {
        if ((ptcb->TCBState == G_TCBState_WAITING_COMPLETED)) {            
            gk_RCBWEL_Link(ptrigger, ptcb, pevent);
            set_pointed_field(pevent, ECBType, (G_INT32) G_ECBType_EXT_ISR_WAITING);
        } else {
            gk_RCBGEL_Link(ptrigger, ptcb, pevent);
            set_pointed_field(pevent, ECBType, (G_INT32) G_ECBType_EXT_ISR_RELEASE);
        }
		
        gk_TCBAEL_Link(pevent, ptcb);

    gm_GeMRTOSCriticalSectionExit;

    return (G_TRUE);
}

/**gu_TriggerEnable
 *  \brief The gu_TriggerEnable function enables the trigger resource, allowing it to be activated using either the gu_TriggerRelease function or the associated hardware interrupt.
 *  \exception par_following The enabling of the trigger resource is performed with the following parameters:
 *  \param [in] IRQ_ID This parameter represents either the IRQ number of the hardware interrupt associated with the trigger resource or the pointer to the trigger resource returned by the gu_TriggerCreate function. If the IRQ_ID is a pointer, it should be cast to an integer (int) for use within the function.
 *  \return The function returns G_TRUE if the operation was successful.
 *  \sa gu_TriggerCreate, gu_TriggerDisable, gu_TriggerEnableHook, gu_TriggerRelease, gu_TriggerRegisterTask, gu_TriggerWait
 *  \remark gu_trg_enable
 *  \relates Trigger
 */
G_INT32 gu_TriggerEnable(int IRQ_ID)
{
    gm_GeMRTOSCriticalSectionEnter;
        gk_TRG_Enable(IRQ_ID);
    gm_GeMRTOSCriticalSectionExit;
    
	return (G_TRUE);
}



G_INT32 gk_TRG_Enable(int IRQ_ID)
{
    G_RCB *ptrigger;
    if (IRQ_ID >= 0 && IRQ_ID < G_NUMBER_OF_IRQ_TCB) ptrigger = g_kcb.KCB_NextRCBTRGL[IRQ_ID];
    else ptrigger = (G_RCB *) IRQ_ID;
    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");

    if (get_pointed_field(ptrigger, RCB_NextRCBGEL) == (struct gs_ecb *) 0) {
        if (ptrigger->trigger.associated_IRQ == G_TRUE)
        {
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, ptrigger->trigger.IRQ_ID=%d, IRQ_ID=%d\n",__FUNCTION__,__LINE__,gm_ProcessorId,(int) ptrigger->trigger.IRQ_ID, (int) IRQ_ID);
            PRINT_ASSERT((ptrigger->trigger.IRQ_ID >= 0 && ptrigger->trigger.IRQ_ID < G_NUMBER_OF_IRQ_TCB),"ERROR RCB is not valid\n");
            gm_IrqDisable(ptrigger->trigger.IRQ_ID);
        }
		// Execute the enable callback function
		PRINT_ASSERT(((void *) ptrigger->trigger.enable_code != (void *) 0),"ERROR function is not valid\n");
        ptrigger->trigger.enable_code(ptrigger->trigger.enable_arg);
		
		ptrigger->trigger.TRG_Enabled = G_TRUE;
    }
    
	return (G_TRUE);
}

/**gu_TriggerDisable
 *  \brief The gu_TriggerDisable function disables the trigger resource, preventing it from being activated using either the gu_TriggerRelease function or the associated hardware interrupt.
 *  \exception par_following The disabling of the trigger resource is performed with the following parameter:
 *  \param [in] IRQ_ID This parameter represents either the IRQ number of the hardware interrupt associated with the trigger resource or the pointer to the trigger resource returned by the gu_TriggerCreate function. If the IRQ_ID is a pointer, it should be cast to an integer (int) for use within the function.
 *  \return The function returns G_TRUE if the operation was successful.
 *  \sa gu_TriggerCreate, gu_TriggerEnable, gu_TriggerDisableHook, gu_TriggerRelease
 *  \remark gu_trg_disable
 *  \relates Trigger
 */
G_INT32 gu_TriggerDisable(unsigned int IRQ_ID)	   
{
    gm_GeMRTOSCriticalSectionEnter;
        gk_TRG_Disable(IRQ_ID);
    gm_GeMRTOSCriticalSectionExit;
    
	return (G_TRUE);
}

G_INT32 gk_TRG_Disable(unsigned int IRQ_ID)	   
{
    G_RCB *ptrigger;
    if (IRQ_ID < G_NUMBER_OF_IRQ_TCB) ptrigger = g_kcb.KCB_NextRCBTRGL[IRQ_ID];
    else ptrigger = (G_RCB *) IRQ_ID;
		PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
	
    if (ptrigger->trigger.associated_IRQ == G_TRUE)
    {
        PRINT_ASSERT((ptrigger->trigger.IRQ_ID >= 0 && ptrigger->trigger.IRQ_ID < G_NUMBER_OF_IRQ_TCB),"ERROR RCB is not valid\n");
        gm_IrqEnable(ptrigger->trigger.IRQ_ID);
    }
	// Execute the disable callback function
	PRINT_ASSERT(((void *) ptrigger->trigger.disable_code != (void *) 0),"ERROR function is not valid\n");
    (ptrigger->trigger.disable_code)(ptrigger->trigger.disable_arg);

	ptrigger->trigger.TRG_Enabled = G_FALSE;

	return (G_TRUE);
}

 void *gk_TRG_null_hook(void *arg)
 {
    // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,gm_ProcessorId);
    return ((void *) 0);
 }

/**
 *  \brief The function creates a trigger resource. It accepts an IRQ_ID argument to allow associating the trigger resource with a hardware interrupt.
 *  \exception par_following The trigger is created and initilized with the following paramenters:
 *  \param [in] IRQ_ID The IRQ_ID argument specifies the number of the hardware interrupt to associate with the trigger resource. Setting this argument to -1 indicates that no association with a hardware interrupt is desired.
 *  \return The gu_TriggerCreate function returns a pointer to the newly created trigger resource. This pointer must be used to reference the trigger resource in all subsequent trigger-related functions.
 *  \sa gu_TriggerEnable, gu_TriggerDisable, gu_TriggerEnableHook, gu_TriggerDisableHook, gu_TriggerRegisterTask, gu_TriggerRelease, gu_TriggerWait
 *  \remark gu_trg_create
 *  \relates Trigger
 */
G_RCB *gu_TriggerCreate(int IRQ_ID)
{
    G_RCB *prcb = (G_RCB *) 0;

    gm_GeMRTOSCriticalSectionEnter;

        if (IRQ_ID >= 0 && IRQ_ID < G_NUMBER_OF_IRQ_TCB)
        {   // It is an external interrupt
            if (g_kcb.KCB_NextRCBTRGL[IRQ_ID] == (G_RCB *) 0)
            {
                prcb = gk_RCB_GetFree();
                g_kcb.KCB_NextRCBTRGL[IRQ_ID] = prcb;
                set_pointed_field(prcb, RCBType, GK_RCBType_TRIGGER);
                prcb->trigger.associated_IRQ  = G_TRUE;
                prcb->trigger.IRQ_ID = (G_INT32) IRQ_ID ;

                PRINT_ASSERT((gkm_RCB_IsValid(g_kcb.KCB_NextRCBTRGL[IRQ_ID]) == G_TRUE),"ERROR RCB is not valid\n");

                gm_IrqEnable((G_INT32) IRQ_ID);

            }
            else
            {
                prcb = g_kcb.KCB_NextRCBTRGL[IRQ_ID];
            }
        }
        else
        {
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, IRQ_ID=%d\n",__FUNCTION__,__LINE__,gm_ProcessorId,(int) IRQ_ID);        
            prcb = gk_RCB_GetFree();
            prcb->RCBType = GK_RCBType_TRIGGER;
            prcb->trigger.associated_IRQ  = G_FALSE;
            prcb->trigger.IRQ_ID = G_NUMBER_OF_IRQ_TCB + 1;        
        }
        PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid\n");
        
        // Set NULL callbacks for enable and disable
        gk_TRG_enable_hook((int) prcb, (void *) gk_TRG_null_hook, (void *) 0);
        gk_TRG_disable_hook((int) prcb, (void *) gk_TRG_null_hook, (void *) 0);        
        
        gk_TRG_Disable((unsigned int) prcb);
    gm_GeMRTOSCriticalSectionExit;
    
    return (prcb);
}

/** gu_TriggerEnableHook
 *  \brief The gu_TriggerEnableHook sets the hook function to be called before the trigger resource is enabled.
 *  \exception par_following The gu_TriggerEnableHook function requires the following paramenters:
 *  \param [in] IRQ_ID This parameter represents either the IRQ number of the hardware interrupt associated with the trigger resource or the pointer to the trigger resource returned by the gu_TriggerCreate function. If the IRQ_ID is a pointer, it should be cast to an integer (int) for use within the function.
 *  \param [in] code_callback This parameter defines the name of the function to be executed as a hook function when the trigger resource is enabled.
 *  \param [in] p_arg This parameter represents the value to be passed to the hook function when it is called. This allows the same hook function to be used for multiple trigger resources with different parameter values.
 *  \return The function returns G_TRUE if the enable hook function was successfully configured; otherwise, it returns G_FALSE.
 *  \sa gu_TriggerCreate, gu_TriggerEnable, gu_TriggerDisableHook
 *  \remark gu_trg_enable_hook
 *  \relates Trigger
 */
G_INT32 gu_TriggerEnableHook(int IRQ_ID, void *code_callback, void *p_arg)
{
    gm_GeMRTOSCriticalSectionEnter;
        gk_TRG_enable_hook((int) IRQ_ID, (void *) code_callback, (void *) p_arg);
    gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}

void gk_TRG_enable_hook(int IRQ_ID, void *code_callback, void *p_arg)
{
    G_RCB *ptrigger;
    
    if (IRQ_ID >= 0 && IRQ_ID < G_NUMBER_OF_IRQ_TCB) ptrigger = g_kcb.KCB_NextRCBTRGL[IRQ_ID];
    else ptrigger = (G_RCB *) IRQ_ID;
    
    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
    
    ptrigger->trigger.enable_code = code_callback;
    ptrigger->trigger.enable_arg  = p_arg;    
}

/**
 *  \brief The gu_TriggerDisableHook function sets the hook function to be called after the trigger resource is disabled.
 *  \exception par_following The disable hook function is specified using the following parameters:
 *  \param [in] IRQ_ID This parameter represents either the IRQ number of the hardware interrupt associated with the trigger resource or the pointer to the trigger resource returned by the gu_TriggerCreate function. If the IRQ_ID is a pointer, it should be cast to an integer (int) for use within the function.
 *  \param [in] code_callback This parameter defines the name of the function to be executed as a hook function when the trigger resource is disabled.
 *  \param [in] p_arg This parameter represents the value to be passed to the hook function when it is called. This allows the same hook function to be used for multiple trigger resources with different parameter values.
 *  \return The function returns G_TRUE if the disable hook function was successfully configured; otherwise, it returns G_FALSE.
 *  \sa gu_TriggerCreate, gu_TriggerDisable, gu_TriggerEnable, gu_TriggerEnableHook, gu_TriggerRelease
 *  \remark gu_trg_disable_hook
 *  \relates Trigger
 */
G_INT32 gu_TriggerDisableHook(int IRQ_ID, void *code_callback, void *p_arg)
{
    gm_GeMRTOSCriticalSectionEnter;
        gk_TRG_disable_hook((int) IRQ_ID, (void *) code_callback, (void *) p_arg);
    gm_GeMRTOSCriticalSectionExit;
    return (G_TRUE);
}

void gk_TRG_disable_hook(int IRQ_ID, void *code_callback, void *p_arg)
{
    G_RCB *ptrigger;

    if (IRQ_ID >= 0 && IRQ_ID < G_NUMBER_OF_IRQ_TCB) ptrigger = g_kcb.KCB_NextRCBTRGL[IRQ_ID];
    else ptrigger = (G_RCB *) IRQ_ID;
    
    PRINT_ASSERT((gkm_RCB_IsValid(ptrigger) == G_TRUE),"ERROR RCB is not valid\n");
    
    ptrigger->trigger.disable_code = code_callback;
    ptrigger->trigger.disable_arg  = p_arg;
   
}

G_INT32 gk_trg_monitor(G_INT32 partial)
{
	int i;
	
	GS_ECB *pevent = (GS_ECB *) 0;

	for (i=0; i<G_NUMBER_OF_IRQ_TCB; i++)
	{
		G_RCB *ptrigger = g_kcb.KCB_NextRCBTRGL[i];
		if (ptrigger != (G_RCB *) 0)
		{
			// Waiting list
			pevent = ptrigger->RCB_NextRCBWEL;
			while (pevent != (GS_ECB *) 0)
			{
				PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE), "invalid ECB");
				PRINT_ASSERT((gkm_TCB_IsValid(pevent->ECB_AssocTCB) == G_TRUE), "TCB invalid");
				PRINT_ASSERT((pevent->ECB_AssocTCB->TCB_INTNumber == i), "Invalid INT Number=%d\n",i);
				PRINT_ASSERT((pevent->ECB_AssocTCB->TCBType == G_TCBType_ISR),"Invalid Type=%d\n",pevent->ECB_AssocTCB->TCBType);
				
				if (partial == G_FALSE)
				{
					if ((pevent->ECB_AssocTCB->TCBState != G_TCBState_WAITING_COMPLETED) && (pevent->ECB_AssocTCB->TCBState != G_TCBState_WAITING)) {printf("Invalid waiting Type=%d",pevent->ECB_AssocTCB->TCBType); return (G_FALSE);}
				}
				pevent = pevent->ECB_NextECB;
			}
			
			// Granted list
			pevent = ptrigger->RCB_NextRCBGEL;
			while (pevent != (GS_ECB *) 0)
			{
				PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"invalid ECB");
				PRINT_ASSERT((gkm_TCB_IsValid(pevent->ECB_AssocTCB) == G_TRUE),"TCB invalid");
				PRINT_ASSERT((pevent->ECB_AssocTCB->TCB_INTNumber == i),"Invalid INT Number=%d\n",i);
				PRINT_ASSERT((pevent->ECB_AssocTCB->TCBType == G_TCBType_ISR),"Invalid Type=%d\n",pevent->ECB_AssocTCB->TCBType);
				
				pevent = pevent->ECB_NextECB;
			}            
		}
	}
	return (G_TRUE);
}

OPTIMEZE_RESTORE

