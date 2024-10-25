/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Message queue definitions
 *  \details This file contains the Message queue definitions.
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

// Check https://stackoverflow.com/questions/1154709/how-can-i-hide-the-declaration-of-a-struct-in-c

#ifndef TRIGGER_H_
#define TRIGGER_H_

 
/**
 *  \brief The <b>T_TRIGGER_RESOURCE</b> is defined as the field <em>"trigger"</em> in a G_RCB resource. So, the fields of the T_TRIGGER_RESOURCE structure should be addressed as: <em>(G_RCB *)->trigger.\<field\></em>
 *  \relates Trigger
 */
struct T_TRIGGER_RESOURCE {
    G_INT32 associated_IRQ;         ///< \brief G_TRUE if associated with IRQ, G_FALSE otherwise
    G_INT32 IRQ_ID;                 ///< \brief Number of IRQ associated with.
    G_INT32 TRG_Enabled;            ///< \brief G_TRUE if trigger is enabled    
    void  *(*enable_code)(void *);  ///< \brief Pointer to the function to be executed to reenable trigger
    void  *enable_arg;              ///< \brief Pointer to the argument of the first call
    void  *(*disable_code)(void *); ///< \brief Pointer to the function to be executed to reenable trigger
    void  *disable_arg;             ///< \brief Pointer to the argument of the first call 
};


G_INT32 gu_TriggerWait(void);

G_INT32 gk_TRG_Wait (GS_TCB *ptcb);
void  gk_TRG_Release (G_INT32 irq_nbr);
G_INT32 gu_TriggerRelease (int irq_nbr);
G_INT32 gu_TriggerRegisterTask(struct gs_tcb *ptcb, G_INT32 irq_nbr);
G_RCB *gu_TriggerCreate(int IRQ_ID);
G_INT32 gu_TriggerDisableHook(int IRQ_ID, void *code_callback, void *p_arg);
void gk_TRG_disable_hook(int IRQ_ID, void *code_callback, void *p_arg);
G_INT32 gu_TriggerEnableHook(int IRQ_ID, void *code_callback, void *p_arg);
void gk_TRG_enable_hook(int IRQ_ID, void *code_callback, void *p_arg);
G_INT32 gu_TriggerEnable(int IRQ_ID);
G_INT32 gk_TRG_Enable(int IRQ_ID);
G_INT32 gu_TriggerDisable(unsigned int IRQ_ID);
G_INT32 gk_TRG_Disable(unsigned int IRQ_ID);
void *gk_TRG_null_hook(void *arg);
G_INT32 gk_trg_monitor(G_INT32 partial);


#endif /* TRIGGER_H__H_ */
