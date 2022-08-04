/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Semaphore definitions
 *  \details This file contains the Semaphore definitions.
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

#ifndef SEM_H_
#define SEM_H_

#include <grtosuserapi.h>

/**
 * Semaphore related functions 
 */
t_semaphore_resource *gu_sem_create(gt_time RCB_Priority, int initial_count, gt_time RCBWaitingTimeout, gt_time RCBGrantedTimeout);
INT32   gu_sem_destroy(t_semaphore_resource *presource);
INT32   gu_sem_wait(t_semaphore_resource *presource,
		            gt_priority waiting_priority,
		            gt_priority RCBGrantedPriority,
		            gt_time RCBWaitingTimeout,
		            gt_time RCBGrantedTimeout,
                    int blocking);
INT32	gu_sem_post(t_semaphore_resource *presource);
INT32 	gu_sem_getvalue(t_semaphore_resource *presource);
INT32   gk_timeout_ECB_SEM_wait(GS_ECB *peventime);
INT32   gk_timeout_ECB_SEM_post(GS_ECB *peventime);
INT32 gk_SEM_granted_kill(GS_ECB *pevent1);
INT32 gk_SEM_waiting_kill(GS_ECB *pevent1);


/**
 *  \brief Semaphore Resource
 *  \details It is used as union in G_RCB
 *  \relates Resource
 */
struct T_SEMAPHORE_RESOURCE {
    INT32        SEMMaxCount;           /** Semaphore maximum MCB_Count       */
};









#endif /* SEM_H */
