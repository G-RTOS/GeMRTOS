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
The <b>trigger</b> resource can be seen as the generalization of interrupts. Tasks can be registered to a trigger resource in order to be resumed or restarted when the trigger resource is <em>triggered</em>. The <b>gu_trigger_create</b> functions creates a trigger resource and it allows associating an external interrupt with the trigger resource created.
The <b>gu_trigger_register_task</b> function registers a task to a trigger resource. 
When the trigger resource is triggered, it is disabled until all the tasks that have been resumed or restarted turn into <em>waiting for a new trigger</em> of the trigger resource. The <b>gu_trigger_wait</b> function puts the task into the <em>waiting for the trigger</em> state. A task associated with a trigger resource also waits for the trigger when finishes its execution.
Trigger resources can be associated with external events (meaning hardware interrupts); in this case, all the tasks registered to the trigger resource will behave as Interrupt Service Routines (ISRs). Because several tasks may be registered to a trigger resource, each one of these tasks will behave as an ISR independently of the others. 
On the other hand, trigger resources may be triggered from tasks' code. The <b>gu_trigger_release</b> function triggers a trigger resource. If enabled, all the registered tasks will be resumed or restarted.
The function <b>gu_trigger_enable</b> enables a trigger resource, while the function <b>gu_trigger_disable</b> is used to disable it.
<h3>Enable and disable hook functions</h3>
Hook functions can be configured to be executed when a trigger resource is either enabled or disabled. The <b>gu_trigger_enable_hook</b> may be used to configure a function to be executed prior to the trigger resource being enabled. The <b>gu_trigger_disable_hook</b> may be used to configure a function to be executed after the trigger resource is disabled. These hook functions are intended to be used to execute the special code associated with the devices associated with the trigger resource.
<h3>Trigger flexibility</h3>
Trigger resources are very flexible for handling system events. They overcome the limitations of interrupts in a generalized way. For instance, UART devices (as many other peripherals) group internal events, <em>input buffer full</em>, and <em>output buffer empty</em> into a single hardware interrupt signal. Therefore, the UART driver should check if an interrupt was produced either by the input or the output buffer and execute the corresponding code. Using trigger resources, a trigger resource may be associated with the UART interrupt signal, and the task code registered to it checks whether the interrupt was produced either by the input buffer, the output buffer, or both. If the input buffer is full, it triggers a trigger resource where all the reading tasks are registered. 
\image html "https://gemrtos.com/images/trigger_uart.gif"
If the output buffer is empty, then it triggers a trigger resource where all the writing tasks are registered. In this way, there may be as many reading tasks and as many writing tasks as required without interfering with one another.
The trigger resource associated with the input buffer should have defined hook functions to enable and disable the <em>input buffer full</em> event, while the trigger resource associated with the output buffer should have defined hook functions to enable and disable the <em>output buffer empty</em> event.
Consequently, trigger resources allow the virtualization and generalization of either internal or external events.
 */

#include <gemrtos.h>


OPTIMEZE_CODE(0)

/**gu_trigger_wait
 *  \brief The <b>gu_trigger_wait</b> function puts the task into a waiting for trigger state. It can be executed in any part of the task's code. The same effect happens when the task finishes its execution (it is not a endless loop) and it is associated with a trigger resource.
 *  \return G_TRUE when executed from a task's code, G_FALSE when executed from main code.
 *  \remark gu_trg_wait
 *  \relates Trigger
 */
G_INT32 gu_trigger_wait(void)
{
    PRINT_ASSERT((G_Running == G_TRUE), "ERROR triggering before Running\n");
	GRTOS_USER_CRITICAL_SECTION_GET;
	GS_TCB *ptcb = gk_PCB_GetCurrentTCB();
	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
	gk_TRG_Wait(ptcb);
	gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);
	GRTOS_USER_CRITICAL_SECTION_RELEASE; 
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


/**
 *  \brief The <b>gu_trigger_release</b> function triggers a trigger resource. If the trigger resource is enabled, and all tasks associated with it are in <em>waiting for trigger</em> state, then the tasks are resumed or restarted. 
 *  \param irq_nbr The IRQ number of the hardware interrupt, or the pointer to trigger resource returned by the gu_trigger_create function. The pointer to the trigger resource should be cast to int.
 *  \return G_TRUE when successful, G_FALSE otherwise*  
 *  \relates Trigger  
 *  \remark gu_trg_release
 */
G_INT32 gu_trigger_release (int irq_nbr) {
    GRTOS_USER_CRITICAL_SECTION_GET;
        gk_TRG_Release((G_INT32) irq_nbr);
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
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

/**gu_trigger_register_task
 *  \brief The <b>gu_trigger_register_task</b> function associates a task with a trigger resource. 
 *  \param ptcb    It is a pointer to the GS_TCB structure of the task wanting to be associated with the trigger resource.
 *  \param irq_nbr The IRQ number of the hardware interrupt associated with the trigger, or the pointer to trigger resource returned by the gu_trigger_create function. The pointer to the trigger resource should be cast to int.
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if IRQ number or TCB pointer al valid.
 *  \todo Implement the reverse function to convert a ISR into a task
 *  \remark gu_trg_register_task
 *  \relates Trigger
 */
G_INT32 gu_trigger_register_task(struct gs_tcb *ptcb, G_INT32 irq_nbr)
{
    G_RCB *ptrigger;

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");

    GRTOS_USER_CRITICAL_SECTION_GET;
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

    GRTOS_USER_CRITICAL_SECTION_RELEASE;

    return (G_TRUE);
}

/**gu_trigger_enable
 *  \brief The <b>gu_trigger_enable</b> functions enables the trigger resource to be triggered with the <em>gu_trigger_release</em> or the hardware interrupt associated with.
 *  \param IRQ_ID  The IRQ number of the hardware interrupt associated with the trigger, or the pointer to trigger resource returned by the gu_trigger_create function. The pointer to the trigger resource should be cast to int.
 *  \return G_TRUE if successful
 *  \remark gu_trg_enable  
 *  \relates Trigger
 */
G_INT32 gu_trigger_enable(int IRQ_ID)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        gk_TRG_Enable(IRQ_ID);
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    
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
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, ptrigger->trigger.IRQ_ID=%d, IRQ_ID=%d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID,(int) ptrigger->trigger.IRQ_ID, (int) IRQ_ID);
            PRINT_ASSERT((ptrigger->trigger.IRQ_ID >= 0 && ptrigger->trigger.IRQ_ID < G_NUMBER_OF_IRQ_TCB),"ERROR RCB is not valid\n");
            GRTOS_CMD_IRQ_ENB_SET(ptrigger->trigger.IRQ_ID);
        }
		// Execute the enable callback function
		PRINT_ASSERT(((void *) ptrigger->trigger.enable_code != (void *) 0),"ERROR function is not valid\n");
        ptrigger->trigger.enable_code(ptrigger->trigger.enable_arg);
		
		ptrigger->trigger.TRG_Enabled = G_TRUE;
    }
    
	return (G_TRUE);
}

/**gu_trigger_disable
 *  \brief The <b>gu_trigger_disable</b> functions disables the trigger resource to be triggered with the <em>gu_trigger_release</em> or the hardware interrupt associated with.
 *  \param IRQ_ID  The IRQ number of the hardware interrupt associated with the trigger, or the pointer to trigger resource returned by the gu_trigger_create function. The pointer to the trigger resource should be cast to int.
 *  \return G_TRUE if successful
 *  \remark gu_trg_disable
 *  \relates Trigger
 */
G_INT32 gu_trigger_disable(unsigned int IRQ_ID)	   
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        gk_TRG_Disable(IRQ_ID);
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    
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
        GRTOS_CMD_IRQ_ENB_CLR(ptrigger->trigger.IRQ_ID);
    }
	// Execute the disable callback function
	PRINT_ASSERT(((void *) ptrigger->trigger.disable_code != (void *) 0),"ERROR function is not valid\n");
    (ptrigger->trigger.disable_code)(ptrigger->trigger.disable_arg);

	ptrigger->trigger.TRG_Enabled = G_FALSE;

	return (G_TRUE);
}

 void *gk_TRG_null_hook(void *arg)
 {
    // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    return ((void *) 0);
 }

/**
 *  \brief The <b>gu_trigger_create</b> function creates a trigger trigger resource. It accepts an IRQ_ID argument to allow associating the trigger resource to a hardware interrupt.
 *  
 *  \param IRQ_ID The number of the hardware interrupt wanted to be associated with. Set this argument equal to -1 if no association with hardware interrupt is wanted.
 *  \return The <b>gu_trigger_create</b> returns the poitner to the trigger resource created. This pointer should be used to reference the trigger resource in all the trigger related functions.
 *  \remark gu_trg_create
 *  \relates Trigger
 */
G_RCB *gu_trigger_create(int IRQ_ID)
{
    G_RCB *prcb = (G_RCB *) 0;

    GRTOS_USER_CRITICAL_SECTION_GET;

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

                GRTOS_CMD_IRQ_ENB_CLR((G_INT32) IRQ_ID);

            }
            else
            {
                prcb = g_kcb.KCB_NextRCBTRGL[IRQ_ID];
            }
        }
        else
        {
            // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d, IRQ_ID=%d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID,(int) IRQ_ID);        
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
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
    
    return (prcb);
}

/**
 *  \brief The <b>gu_trigger_enable_hook</b> function sets the hook function to be called prior enabling the trigger resource.
 *  \param IRQ_ID The IRQ number of the hardware interrupt associated with the trigger, or the pointer to trigger resource returned by the gu_trigger_create function. The pointer to the trigger resource should be cast to int.
 *  \param code_callback The function name of the function to be called. This parameter defines the hook function to be executed when trigger resource is enabled.
 *  \param p_arg The value to be passed when the hook function is called. Therefore, the same hook function can be parametrized for several trigger resources.
 *  \return G_TRUE if successful.
 *  \remark gu_trg_enable_hook
 *  \relates Trigger
 */
G_INT32 gu_trigger_enable_hook(int IRQ_ID, void *code_callback, void *p_arg)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        gk_TRG_enable_hook((int) IRQ_ID, (void *) code_callback, (void *) p_arg);
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
 *  \brief The <b>gu_trigger_disable_hook</b> function sets the hook function to be called after disabling the trigger resource.
 *  \param IRQ_ID The IRQ number of the hardware interrupt associated with the trigger, or the pointer to trigger resource returned by the gu_trigger_create function. The pointer to the trigger resource should be cast to int.
 *  \param code_callback The function name of the function to be called. This parameter defines the hook function to be executed when trigger resource is disabled.
 *  \param p_arg The value to be passed when the hook function is called. Therefore, the same hook function can be parametrized for several trigger resources.
 *  \return G_TRUE if successful.
 *  \remark gu_trg_disable_hook
 *  \relates Trigger
 */
G_INT32 gu_trigger_disable_hook(int IRQ_ID, void *code_callback, void *p_arg)
{
    GRTOS_USER_CRITICAL_SECTION_GET;
        gk_TRG_disable_hook((int) IRQ_ID, (void *) code_callback, (void *) p_arg);
    GRTOS_USER_CRITICAL_SECTION_RELEASE;
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

