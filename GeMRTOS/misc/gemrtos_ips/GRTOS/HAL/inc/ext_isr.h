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



// Check https://stackoverflow.com/questions/1154709/how-can-i-hide-the-declaration-of-a-struct-in-c

#ifndef EXT_ISR_H_
#define EXT_ISR_H_

#include <grtosuserapi.h>
#include "sys/alt_irq.h"
#include "priv/alt_irq_table.h"


/**
 *  gs_ext_isr Struct to hold the actions associated  with external events. They are hold in g_kcb structure
 */
struct gs_ext_isr {
    struct gs_tcb *G_TCB_ISR;        /** Pointers to the first ISR TCBs for each interrupt number */
    INT32         G_EXT_INT_Count;  /** Holds the number of ISR released, when 0 enable it again */
    gt_priority   G_EXT_ISR_Timeout; /** Holds the timeout of the interrupts                      */
};

void  gk_INIT_IRQ (void);
INT32 gk_ISR_COMPLETE (GS_TCB *ptcb);
void  gk_ISR_RELEASE (int irq_nbr);
INT32 gu_SetTaskISR(struct gs_tcb *ptcb, unsigned int irq_nbr);
INT32 gk_SetTaskISR(struct gs_tcb *ptcb, unsigned int irq_nbr);



#endif /* EXT_ISR_H_ */
