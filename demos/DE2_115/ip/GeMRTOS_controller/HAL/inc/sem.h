/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Semaphore definitions
 *  \details This file contains the Semaphore definitions.
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

#ifndef SEM_H_
#define SEM_H_



/**
 * Semaphore related functions 
 */
G_RCB *gu_SemaphoreCreate(int initial_count);
G_INT32 gu_SemaphoreWait(G_RCB *presource, int blocking);
G_INT32	gu_SemaphorePost(G_RCB *presource);
G_INT32 gk_sem_post(G_RCB *presource, GS_TCB *ptcb);



/**
 *  \brief The <b>T_SEMAPHORE_RESOURCE</b> is defined as the field <em>"semaphore"</em> in a G_RCB resource. So, the fields of the T_SEMAPHORE_RESOURCE structure should be addressed as: <em>(G_RCB *)->semaphore.\<field\></em>
 *  \relates Semaphore
 */
struct T_SEMAPHORE_RESOURCE {
    G_INT32  SEM_Current_Count;  ///< \brief It is the current count of the semaphore. If it is equal to 0, no more grants are allowed. It is initialized with the SEM_Maximum_Count when the semaphore is created with the <em>gu_SemaphoreCreate</em> function.
    G_INT32  SEM_Maximum_Count;  ///< \brief It is the initial value of the <em>SEM_Current_Count</em> field. It is defined for debugging purposes only, and it could be removed.
};



#endif /* SEM_H */
