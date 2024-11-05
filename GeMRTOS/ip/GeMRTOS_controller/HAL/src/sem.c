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
 * \brief The <b>semaphore</b> is a resource for preventing concurrent execution of code sections. GeMRTOS is capable of using either <b>binary</b> or <b>counting</b> semaphores. While counting semaphores set the starting count to the number of enabled concurrent executions, binary semaphores set the initial count to 1. If a task requests a semaphore, it will only be allowed if the current count is more than or equal to 1, and if it is granted, it will decrease the current count. When a task that is granting the semaphore releases it, the current count is increased. As a result, the initial count of the semaphore will be equal to the maximum number of concurrent executions of a code section that is excluded by the semaphore. <br /> The semaphore request function may be configured as blocking or non-blocking. It can be specified a timeout for the blocking alternative.
 * \exception sec_fun_intro The Semaphore category in GeMRTOS encompasses functions and macros designed to facilitate synchronization and resource management among concurrent tasks within the real-time operating system. Semaphores are essential for controlling access to shared resources, preventing race conditions, and ensuring data integrity by regulating how tasks interact with one another. <br /> By utilizing the Semaphore category, developers can create and manage both binary and counting semaphores, allowing for fine-grained control over task execution and resource allocation. The functions within this category enable operations such as semaphore creation, and waiting, effectively coordinating task activities and synchronizing their behavior. <br /> The use of semaphores is crucial in environments where multiple tasks need to access shared resources without conflict, as it helps maintain system stability and performance. Additionally, by leveraging semaphores, developers can enhance the efficiency of their applications, ensuring that critical tasks are executed in a timely manner while preventing task starvation and optimizing resource utilization. 
 */

#include <gemrtos.h>



OPTIMEZE_CODE(0)



/** gu_SemaphoreCreate
 *  \brief The gu_SemaphoreCreate function creates a new semaphore resource.  The semaphore is implemented using a G_RCB structure extended with fields from a T_SEMAPHORE_RESOURCE structure.  The created semaphore includes event lists for tasks waiting to acquire the semaphore and tasks that currently hold the semaphore. Tasks requesting the semaphore and encountering a blocking condition (semaphore already acquired) will add themselves to the waiting list.  This function can be called from either the main application code or from within a task; however, if called within a task, it must be called before any semaphore request or release operations are performed, or an error will result.
 *  \exception par_following The function takes one parameter:
 *  \param [in] initial_count An integer specifying the initial count of the semaphore. This value determines the number of tasks that can simultaneously acquire the semaphore. A value of 1 creates a binary semaphore. 
 *  \return The gu_SemaphoreCreate function returns a pointer (G_RCB *) to the G_RCB structure that implements the semaphore resource. This pointer is used in all subsequent semaphore operations. A NULL pointer is returned if there is insufficient memory to create the semaphore.  The return value should always be checked for errors.
 *  \sa gu_SemaphoreWait, gu_SemaphorePost
 *  \remark gu_SemaphoreCreate
 *  \relates Semaphore
 */
G_RCB *gu_SemaphoreCreate(int initial_count)
{
	G_RCB *presource = (G_RCB * ) 0;
	
    gm_GeMRTOSCriticalSectionEnter;
	
	// Get a Free resource
	presource = gk_RCB_GetFree();
	PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid, value = %p\n", (void *) presource);
	if (presource != 0) {
		// Set the default values of the semaphore
		// 18-10-2024 presource->RCBGrantedPriority.i64 = (G_INT64) G_LOWEST_PRIORITY;
		// 18-10-2024 presource->RCBGrantedTimeout.i64 = (G_INT64) G_LATEST_TIME - (G_INT64) 1;
		// 18-10-2024 presource->RCB_NextRCBGEL = (struct gs_ecb *) 0;
		// 18-10-2024 presource->RCB_NextRCBWEL  = (struct gs_ecb *) 0;
		// 18-10-2024 presource->RCB_NextRCB = (struct g_rcb *) 0;
		// 18-10-2024 presource->RCB_NextRCBASL = (struct gs_scb *) 0;
		// 18-10-2024 presource->RCBPriority.i64 = (G_INT64) G_LOWEST_PRIORITY;
		// presource->RCBState = GK_RCBState_SEM;
		presource->RCBType = GK_RCBType_SEM;
		// 18-10-2024 presource->RCBWaitingTimeout.i64 = (G_INT64) G_LATEST_TIME - (G_INT64) 1;

		presource->semaphore.SEM_Maximum_Count = (G_INT32) initial_count;
		presource->semaphore.SEM_Current_Count = (G_INT32) initial_count;
	}
	
	gm_GeMRTOSCriticalSectionExit;
	
	return((G_RCB *) presource);
}



/** gu_SemaphoreWait
 *  \brief The gu_SemaphoreWait function attempts to acquire a semaphore resource. If the semaphore's current count is greater than 0, the semaphore is granted to the calling task, and the count is decremented.  If the count is 0, the behavior depends on the blocking parameter: if blocking is G_TRUE, the task is suspended until the semaphore becomes available; if blocking is G_FALSE, the function returns immediately without blocking. When blocking, the task's waiting priority is determined by its ready priority.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] presource A pointer to the G_RCB structure representing the semaphore resource, as returned by gu_SemaphoreCreate.
 *  \param [in] blocking An integer flag. If G_TRUE, the task blocks until the semaphore is available; if G_FALSE, the function returns immediately if the semaphore is unavailable.
 *  \return The gu_SemaphoreWait function returns G_TRUE if the semaphore was granted to the task, and G_FALSE when the semaphore was unavailable and blocking was G_FALSE.
 *  \sa gu_SemaphorePost, gu_SemaphoreCreate
 *  \remark gu_SemaphoreWait
 *  \relates Semaphore
 *  \todo Implement waiting timeout
 */
G_INT32 gu_SemaphoreWait(G_RCB *presource, int blocking)
{
	GS_TCB  *ptcb;
	GS_ECB  *pevent;

	G_INT32 retorno = G_FALSE;

    gm_GeMRTOSCriticalSectionEnter;
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
			// 04-09-2023 gm_GeMRTOSCriticalSectionEnter;
			
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
	
    gm_GeMRTOSCriticalSectionExit;

    return (retorno);
}

// ################################################# 
 
/**gu_SemaphorePost
 *  \brief The gu_SemaphorePost function releases a semaphore previously acquired by the currently executing task. If tasks are waiting to acquire the semaphore, the highest-priority waiting task will be granted the semaphore. If no tasks are waiting, the semaphore's internal count is incremented.
 *  \exception par_following The function accepts one parameter:
 *  \param [in] presource A pointer to the G_RCB structure representing the semaphore resource. This pointer was returned by the gu_SemaphoreCreate function.
 *  \return The gu_SemaphorePost function returns G_TRUE if the semaphore was successfully released, and G_FALSE otherwise.
 *  \sa gu_SemaphoreWait, gu_SemaphoreCreate
 *  \remark gu_SemaphorePost
 *  \relates Semaphore
 */ 
G_INT32 gu_SemaphorePost(G_RCB *presource)
{
	GS_TCB *ptcb;

	gm_GeMRTOSCriticalSectionEnter;
	
        ptcb   = gk_PCB_GetCurrentTCB(); 
        gk_sem_post(presource, ptcb);
		
	gm_GeMRTOSCriticalSectionExit;

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

