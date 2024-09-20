/**
 *  \file sem.c
 *  \author Ricardo Cayssials
 *  \brief GRTOS Semaphore functions
 *  \details This file contains the functions for semaphore implementation.
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

// License Agreement                                                          
// Copyright (c) Ricardo L. Cayssials                                         
// All rights reserved.                                                       

/** \class Semaphore
The <b>semaphore</b> is a resource for preventing concurrent execution of code sections. GeMRTOS is capable of using either <b>binary</b> or <b>counting</b> semaphores. While counting semaphores set the starting count to the number of enabled concurrent executions, binary semaphores set the initial count to 1. If a task requests a semaphore, it will only be allowed if the current count is more than or equal to 1, and if it is granted, it will decrease the current count. When a task that is granting the semaphore releases it, the current count is increased. As a result, the initial count of the semaphore will be equal to the maximum number of concurrent executions of a code section that is excluded by the semaphore.

The semaphore request function may be configured as blocking or non-blocking. It can be specified a timeout for the blocking alternative.
 */

#include <gemrtos.h>



OPTIMEZE_CODE(0)



/** gu_sem_create
 *  \brief The <b>gu_sem_create</b> function creates a new semaphore resource. The semaphore is built using the G_RCB structure with some fields from the T_SEMAPHORE_RESOURCE structure appended. A waiting and a granted event list will be included in the newly constructed semaphore resource. Tasks requesting the semaphore link a waiting event in the waiting list if sempahore is already granted and the requests is a blocking one. 
 *  The <b>gu_sem_create</b> can be called from either the main code or a task code. However, when it is executed within a task code, it must come before the execution of a request for or a release of function, or an error will be produced.
 *  \param [in] initial_count Initial count of the semaphore to allow simultaneous granted tasks. A value equal to 1 determines a binary semaphore. 
 *  \return The <b>gu_sem_create</b> function returns a pointer to the G_RCB structure that implements the semaphore resource. It should be used in all the functions related to the semaphore. The function returns a NULL pointer if there is not enough memory to implement the semaphore.
 *  \remark gu_sem_create
 *  \relates Semaphore
 */
G_RCB *gu_sem_create(int initial_count)
{
	G_RCB *presource = (G_RCB * ) 0;
	
    GRTOS_USER_CRITICAL_SECTION_GET;
	
	// Get a Free resource
	presource = gk_RCB_GetFree();
	PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid, value = %p\n", (void *) presource);
	if (presource != 0) {
		// Set the default values of the semaphore
		presource->RCBGrantedPriority.i64 = (G_INT64) G_LOWEST_PRIORITY;
		presource->RCBGrantedTimeout.i64 = (G_INT64) G_LATEST_TIME - (G_INT64) 1;
		presource->RCB_NextRCBGEL = (struct gs_ecb *) 0;
		presource->RCB_NextRCBWEL  = (struct gs_ecb *) 0;
		presource->RCB_NextRCB = (struct g_rcb *) 0;
		presource->RCB_NextRCBASL = (struct gs_scb *) 0;
		presource->RCBPriority.i64 = (G_INT64) G_LOWEST_PRIORITY;
		presource->RCBState = GK_RCBState_SEM;
		presource->RCBType = GK_RCBType_SEM;
		presource->RCBWaitingTimeout.i64 = (G_INT64) G_LATEST_TIME - (G_INT64) 1;

		presource->semaphore.SEM_Maximum_Count = (G_INT32) initial_count;
		presource->semaphore.SEM_Current_Count = (G_INT32) initial_count;
	}
	
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
	
	return((G_RCB *) presource);
}



/**
 *  \brief The <b>gu_sem_wait</b> function requests a semaphore resource. If the current count is greater than 1, then the semaphore is granted to the task executing the gu_sem_wait function, and the semaphore count is decreased. If the current semaphore count is equal to 0, then either (1) the task is suspended when the <em>blocking</em> parameter is equal to G_TRUE, or (2) the function returns G_FALSE without suspending the task. When the <em>blocking</em> parameter is equal to G_TRUE, the waiting priority is defined by the ready priority of the task.
 *  
 *  \param presource It is the pointer to the semaphore resource returned by the gu_sem_create function when it was created.
 *  \param blocking If the <em>blocking</em> parameter is set to G_TRUE, then the function blocks the task waiting for the semaphore when it is not available. When the <em>blocking</em> parameter is set to G_FALSE, the function returns G_FALSE without blocking execution if the semaphore is not available.
 *  \return The <b>gu_sem_wait</b> function returns G_TRUE if the semaphore was granted to the task, G_FALSE otherwise.
 *  \remark gu_sem_wait
 *  \relates Semaphore
 *  \todo Implement waiting timeout
 */
G_INT32 gu_sem_wait(G_RCB *presource, int blocking)
{
	GS_TCB  *ptcb;
	GS_ECB  *pevent;

	G_INT32 retorno = G_FALSE;

    GRTOS_USER_CRITICAL_SECTION_GET;
    PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid, value = %p\n", (void *) presource);
	
	ptcb = gk_PCB_GetCurrentTCB();
	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR RCB not valid\n");

	pevent    = gk_ECB_GetFree();
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");

	gk_TCBAEL_Link(pevent,ptcb);
	


	if (presource->semaphore.SEM_Current_Count > 0)
	{
		presource->semaphore.SEM_Current_Count--;
		set_pointed_field(pevent, ECBType, G_ECBType_SEM_GRANTED);
		gk_RCBGEL_Link((G_RCB *) presource, ptcb, pevent);
		retorno = G_TRUE;     // resource available granted without waiting
	}
	else
	{
		set_pointed_field(pevent, ECBType, G_ECBType_SEM_WAITING);
		if (blocking == G_TRUE) {
			// Insert it in waiting event list
			pevent->ECBValue.i64 = ptcb->TCBReadyPriority;
			gk_RCBWEL_Link((G_RCB *) presource, ptcb, pevent);

			gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);  //Switch tasks
			// #################################################
			// #################################################
			// 04-09-2023 GRTOS_USER_CRITICAL_SECTION_GET;
			
			// 1) Resource was granted
			//   pevent->ECBState    = GS_ECBState_GRANTED_RESOURCE
			//   pevent->ECBType     = G_ECBType_SEM_GRANTED
			//   peventime->ECBState = GS_ECBState_WAITING_TIME
			// 2) Waiting Timeout happened (gk_timeout_ECB_SEM_wait was executed)
			//   pevent->ECBState    = GS_ECBState_UNLINKED
			//   peventime->ECBState = GS_ECBState_UNLINKED

			PRINT_ASSERT((pevent->ECBType == G_ECBType_SEM_GRANTED),"ERROR ECB is not granted\n");
			if (get_pointed_field(pevent, ECBType) == G_ECBType_SEM_GRANTED)
			{ // The resource is granted to task
				retorno = G_TRUE;  // resource available after waiting
			}
		}
		else
		{
			gk_ECB_List_Unlink(pevent);
			gk_ECBFL_Link(pevent);
			retorno = G_FALSE;  // resource no available without waiting
		}
	}
	
    GRTOS_USER_CRITICAL_SECTION_RELEASE;

    return (retorno);
}

// ################################################# 
 
/**gu_sem_post
 *  \brief The <b>gu_sem_post</b> function releases the granted semaphore from the task executing the function (when semaphore was granted to the task). If a task is waiting for the semaphore on the waiting list, then the highest waiting priority task will be granted to the semaphore. If no task is waiting for the semaphore, the current count of the semaphore resource is increased.
 *  \param [in] presource It is the pointer to the semaphore resource returned by the gu_sem_create function when it was created.
 *  \return The <b>gu_sem_post</b> function returns G_TRUE when successful, G_FALSE otherwise.
 *  \remark gu_sem_post
 *  \relates Semaphore
 */ 
G_INT32 gu_sem_post(G_RCB *presource)
{
	GS_TCB *ptcb;

	GRTOS_USER_CRITICAL_SECTION_GET;
	
        ptcb   = gk_PCB_GetCurrentTCB(); 
        gk_sem_post(presource, ptcb);
		
	GRTOS_USER_CRITICAL_SECTION_RELEASE;

	return(G_TRUE);
}

G_INT32 gk_sem_post(G_RCB *presource, GS_TCB *ptcb)
{

	GS_ECB *pevent;

    PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid\n");
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB not valid\n");

    //Check if task is granted for this resource
    pevent = gk_TCB_in_RCBGEL((G_RCB *) presource, ptcb);
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");
    PRINT_ASSERT((pevent->ECB_AssocTCB == ptcb),"ERROR RCB is not associated to task TCB\n");
    PRINT_ASSERT((pevent->ECB_AssocRCB == (struct g_rcb *) presource),"ERROR RCB is not associated to RCB\n");
    PRINT_ASSERT((pevent != (GS_ECB *) 0),"ERROR post not granting\n");
    if (pevent != (GS_ECB *) 0) {
        // Task was granting, remove the event
        PRINT_ASSERT((presource->semaphore.SEM_Current_Count >=0),"ERROR post not granting\n");
        presource->semaphore.SEM_Current_Count++;
        // 29-06-2023 gk_ECB_List_Unlink(pevent);
        gk_ECBFL_Link(pevent);
        
        // Check if there exist waiting event
        pevent = get_pointed_field(presource, RCB_NextRCBWEL);
        if (pevent != (GS_ECB *) 0)
        {
            PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");
            presource->semaphore.SEM_Current_Count--;
            gk_RCBWEL_Unlink(pevent);
            set_pointed_field(pevent, ECBType, G_ECBType_SEM_GRANTED);
            gk_RCBGEL_Link((G_RCB *) presource, ptcb, pevent);
            
            // Resume waiting task
            PRINT_ASSERT((gkm_TCB_IsValid(pevent->ECB_AssocTCB) == G_TRUE),"ERROR TCB not valid\n");
            gk_TCB_Unlink(pevent->ECB_AssocTCB);
            gk_TCBRDYL_Link(pevent->ECB_AssocTCB);                
        }
    }

	return(G_TRUE);
}



OPTIMEZE_RESTORE

