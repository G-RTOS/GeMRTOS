/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Interrupt functions
 *  \details This file contains the functions for interrupt managenment.
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

///  \todo Define a new Task Type for ISR not completed but with interrupt release and create the function


#include <grtosdefinitions.h>

OPTIMEZE_CODE(3)

/**gk_INIT_IRQ
 *  \brief 
 *  Initializes the ISRs according the ISRs set by Nios HAL. It creates a TCB for each
 *  ISR and link it to the g_kcb.KCB_ExtISR[i].G_TCB_ISR accordingly.
 *  \details 
 *  It is called from gk_INIT_KERNEL()
 *  \todo Include Disabled but ready interrupt Type
 *  \todo Utilize the gk_SetTaskISR in order to be coherent if the user define a ISR in the main function
 *  \relates Interrupt
 */ 
void  gk_INIT_IRQ (void)
{
	int i; PRINT_DEBUG_LINE
	G_DEBUG_VERBOSE; PRINT_DEBUG_LINE // Print Debug information
	for (i=0 ;  i<ALT_NIRQ ;  i++)
	{
		///Create ISR TCB if IRQ is enabled
        if ((G_IRQ_ENABLED >> i) & 1) {

            g_kcb.KCB_ExtISR[i].G_TCB_ISR = gk_CreateTask((void *) alt_irq[i].handler,  ///< Pointer to the beginning of isr
                                 (void *) alt_irq[i].context,               ///< Pointer to the argument of the first call
                                 // (void *) &g_kcb.G_ISR_STACK[i][G_ISR_STACKSIZE-4],              ///< Botton of the Stack of the Task
                                 (void *) g_kcb.G_ISR_STACK + (i * G_ISR_STACKSIZE) + G_ISR_STACKSIZE-4,              ///< Botton of the Stack of the Task
                                 (unsigned int) G_ISR_STACKSIZE,         ///< Size of the stack in words
                                 (unsigned int) G_TASK_TYPE_ISR,         ///< Type of the Task
                                 (INT64) G_ISR_PRIORITY,       ///< Initial Priority of the task
                                 (INT64) G_ISR_PRIORITY,         ///< Priority of the task when it is executing
                                 (INT64) G_TASK_PERIOD_DEFAULT,            ///< Deadline of the task
                                 (INT64) G_TASK_PERIOD_DEFAULT,              ///< Period of the task
                                 (GS_LCB *) G_TASK_LCB_DEFAULT,        /*!< pointer of the ready list that should be inserted */
                                 (int) G_TRUE,     /*!< True when task is aborted when deadline */
                                 (INT64) G_ISR_PRIORITY,
                                 (int) i);  
                                     
			// g_kcb.KCB_ExtISR[i].G_TCB_ISR->TCB_INTNumber = i; 
            
            if ((G_IRQ_PENDING >> i) & 1) {                
				/* The interrupt was pending then insert in ready list and disable it */
				GRTOS_CMD_IRQ_ENB_CLR(i); 
                g_kcb.KCB_ExtISR[i].G_EXT_INT_Count++; // = (int) 1;
				gk_TCBRDYL_Link(g_kcb.KCB_ExtISR[i].G_TCB_ISR);   /* Insert Task in Ready List                */
			} else {
                gk_TCBWL_Link(g_kcb.KCB_ExtISR[i].G_TCB_ISR, G_TCBState_WAITING_COMPLETED); 
                // g_kcb.KCB_ExtISR[i].G_EXT_INT_Count = (int) 0;
				GRTOS_CMD_IRQ_ENB_SET(i); 
			}
		}
	}    
}

/**gk_ISR_COMPLETE
 *  \brief 
 *  Executed when Task finishes as ISR. If remains ready, then it will be not released next Interrupt event
 *  \param [in] ptcb Pointer to the TCB of the ISR task
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Include completes but task remains ready.
 *  \relates Interrupt
 */
INT32 gk_ISR_COMPLETE (GS_TCB *ptcb) {
    
    int irq_nbr = ptcb->TCB_INTNumber;
    
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count == (int) 0) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
#endif    
    
    if (ptcb->TCBType == G_TASK_TYPE_ISR) g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count--;
    if (g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count == (INT32) 0) {
        GRTOS_CMD_IRQ_ENB_SET(irq_nbr);
    }
    
    return G_TRUE;
}

/**gk_ISR_RELEASE
 *  \brief 
 *  Executed when an interrupt is triggered. It is called from gk_ENTRY_IRQ_HANDLER()
 *  \param [in] irq_nbr Number index of the interrupt
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Verify if task is waiting to enable the interrupt, otherwise change the Type of the ISR
 *  \relates Interrupt
 */
INT32 gk_ISR_RELEASE (int irq_nbr) {

    GS_TCB *ptcb = g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR;
    
    GRTOS_CMD_IRQ_ENB_CLR(irq_nbr); PRINT_DEBUG_LINE
    
    while (ptcb != (GS_TCB *) 0) {
        // !!!! verificar si esta en estado waiting
        /* Limpio el registro de interrupcion porque ya fue activada  */
        if (ptcb->TCBType == G_TASK_TYPE_ISR) {
            if (ptcb->TCBState == G_TCBState_WAITING_COMPLETED) {     // if task is waiting
                /* Insert Task in Ready List                */                
                gk_TCB_Unlink((GS_TCB *) ptcb); 
                gk_TASK_STK_Init((GS_TCB *) ptcb); 
                gk_TCBRDYL_Link((GS_TCB *) ptcb); 
                
                g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count++;
            }
        }
        ptcb = (GS_TCB *) ptcb->TCB_NextISRTCB;
    }

    return G_TRUE;
}

/**gu_SetTaskISR
 *  \brief 
 *  Convert a task to a ISR associated to a Interrupt Number
 *  \param [in] ptcb    Pointer to the TCB of the task to be ISR
 *  \param [in] irq_nbr Number of the IRQ to be associated to.
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if IRQ number or TCB pointer al valid.
 *  \todo Implement the reverse function to convert a ISR into a task
 *  \relates Interrupt
 */
INT32 gu_SetTaskISR(struct gs_tcb *ptcb, unsigned int irq_nbr)
{
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;
        gk_SetTaskISR(ptcb, irq_nbr);
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE; 
    return G_TRUE;
}

/**gk_SetTaskISR
 *  \brief 
 *  Convert a task to a ISR associated to a Interrupt Number in Kernal mode
 *  \param [in] ptcb    Pointer to the TCB of the task to be ISR
 *  \param [in] irq_nbr Number of the IRQ to be associated to.
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if IRQ number or TCB pointer al valid.
 *  \todo Implement the reverse function to convert a ISR into a task
 *  \relates Interrupt
 */
INT32 gk_SetTaskISR(struct gs_tcb *ptcb, unsigned int irq_nbr)
{
    // Set the number of associated interrupt (valid only when TCBType == G_TASK_TYPE_ISR)
    // and link to the ISR linked list !!! check if task is running for consistency of Count
    ptcb->TCBType = G_TASK_TYPE_ISR;
    ptcb->TCB_INTNumber = irq_nbr;
    ptcb->TCB_PrevISRTCB = (struct gs_tcb *) 0;
    ptcb->TCB_NextISRTCB = g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR;
    if (g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR != (struct gs_tcb *) 0) {
        g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR->TCB_PrevISRTCB = (struct gs_tcb *) ptcb;
    }
    g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR = (struct gs_tcb *) ptcb;
    
    if (ptcb->TCBState != G_TCBState_WAITING_COMPLETED) {
        g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count++;
    }
    
    return G_TRUE;
}

OPTIMEZE_RESTORE