/**
 *  \file 
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

#include <grtosdefinitions.h>

OPTIMEZE_CODE(3)

/**gk_CreateTask
 *  \brief 
 *  Kernel Function: creates a new task initializing TCB features
 *  \param [in] TaskCode             Pointer to the beginning of the task code
 *  \param [in] p_arg                Pointer to the argument of the first call
 *  \param [in] StkBotton            Botton of the Stack of the Task
 *  \param [in] StkSize              Size of the stack in words
 *  \param [in] TCBType              Type of the Task
 *  \param [in] TCBReadyPriority     Initial Priority of the task
 *  \param [in] TCBRunPriority       Priority of the task when it is executing
 *  \param [in] TCBDeadline          Deadline of the task
 *  \param [in] TCBPeriod            Period of the task
 *  \param [in] TCB_RDY_LCB_Index    Ready list that should be inserted
 *  \param [in] TCB_Abort_w_Deadline True when task is aborted when deadline
 *  \param [in] TCBInherPriority     Inhereted priority
 *  \param [in] TCB_INTNumber        Interrupt Number if task is ISR
 *  \return Pointer to the TCB
 *  \todo TCBInherPriority has no sense
 *  \relates Task
 */
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
                     INT64 TCBInherPriority,
                     int TCB_INTNumber)       
{
    GS_TCB  *ptcb;
    
    ptcb = gk_TCB_GetFree();
    // Initialize the gs_tcb structure of the task
    if (ptcb != (struct gs_tcb *) 0) {
        ptcb->TCB_StackBottom      = (GS_STK *) ((((INT32) StkBotton)-8)& ~0x3); ///< Botton of the Stack of the Task
        ptcb->TCB_StackTop         = (GS_STK *)((INT32) ptcb->TCB_StackBottom - ((INT32)(StkSize)& ~0x3)); ///< Top of the Stack of the task
        ptcb->TCB_TaskCode         = TaskCode;           ///< Pointer to tha begining of the task code
        ptcb->TCB_TaskArg          = p_arg;              ///< Pointer to the argument of the first call

        ptcb->TCBType              = TCBType;      ///< Type of the Task    
        ptcb->TCBReadyPriority     = TCBReadyPriority;  ///< Initial Priority of the task
        ptcb->TCBRunPriority       = TCBRunPriority;  ///< Priority of the task when it is executing
        ptcb->TCBDeadline          = TCBDeadline;    ///< Deadline of the task
        ptcb->TCBPeriod            = TCBPeriod;    ///< Period of the task
        ptcb->TCB_RDY_LCB_Index    = TCB_RDY_LCB_Index;       /*!< number of the ready list that should be inserted */
        ptcb->TCB_Abort_w_Deadline = TCB_Abort_w_Deadline;   /*!< True when task is aborted when deadline, false otherwise */
        ptcb->TCBInherPriority     = TCBInherPriority;  /* Load task priority into TCB              */
        
        // Initilize the stack of the task
        gk_TASK_STK_Init(ptcb);
        
        // Set the number of associated interrupt (valid only when TCBType == G_TASK_TYPE_ISR)
        // and link to the ISR linked list
        ptcb->TCB_INTNumber = TCB_INTNumber;
        ptcb->TCB_PrevISRTCB = (struct gs_tcb *) 0;
        if (TCBType == G_TASK_TYPE_ISR) {
            ptcb->TCB_NextISRTCB = g_kcb.KCB_ExtISR[TCB_INTNumber].G_TCB_ISR;
            if (g_kcb.KCB_ExtISR[TCB_INTNumber].G_TCB_ISR != (struct gs_tcb *) 0) {
                g_kcb.KCB_ExtISR[TCB_INTNumber].G_TCB_ISR->TCB_PrevISRTCB = (struct gs_tcb *) ptcb;
            }
            g_kcb.KCB_ExtISR[TCB_INTNumber].G_TCB_ISR = (struct gs_tcb *) ptcb;
        } else {
            ptcb->TCB_NextISRTCB = (struct gs_tcb *) 0;
        }
    }    

	return (ptcb);
}


OPTIMEZE_RESTORE