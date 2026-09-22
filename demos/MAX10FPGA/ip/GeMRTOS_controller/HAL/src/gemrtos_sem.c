/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS Semaphore functions
 *  \details This file contains the functions for semaphore implementation.
 *  \version 0.9a
 *  \date      2015-2025
 *
 *  SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
 *  Copyright (c) 2013-2026 GeMRTOS Project Contributors
 *
 *  Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)
 *
 *  This file is part of GeMRTOS, licensed under the PolyForm Noncommercial
 *  License 1.0.0. You may use, copy, modify, and distribute this file for
 *  any noncommercial purpose. Commercial use requires a separate written
 *  license from GeMRTOS. Full terms:
 *  https://polyformproject.org/licenses/noncommercial/1.0.0
 */



/** \class Semaphore_and_Mutex
 * \brief The <b>semaphore</b> is a resource used to synchronize the execution of different tasks. GeMRTOS supports both <b>binary</b> and <b>counting</b> semaphores. Counting semaphores initialize the counter to the number of allowed concurrent executions, while binary semaphores initialize the counter to 1. When a task requests a semaphore, it is granted only if the current count is greater than or equal to 1; upon granting, the count decreases by one. When a task releases the semaphore, the count increases. The semaphore request function can be configured as blocking or non-blocking, with an optional timeout for the blocking case.<br /> The <b>mutex</b> is a resource that prevents concurrent execution of critical code sections accessing shared resources. It must be acquired by a task before accessing shared resources and released when no longer needed. The task that grants the mutex inherits the maximum priority among all requesting tasks (ceiling protocol). This priority inheritance reduces priority inversion issues caused when lower-priority tasks block higher-priority ones by holding the mutex. 
 * \exception sec_fun_intro The <b>Semaphore</b> and <b>Mutex</b> category in GeMRTOS encompasses functions and macros designed to facilitate synchronization and resource management among concurrent tasks within the real-time operating system. <b>Semaphores</b> and <b>mutexes</b> are essential for controlling access to shared resources, preventing race conditions, and ensuring data integrity by regulating how tasks interact with one another. <br /> By utilizing the <b>Semaphore</b> and <b>Mutex</b> category, developers can create and manage both binary and counting semaphores, and mutexes, allowing for fine-grained control over task execution and resource allocation. The functions within this category enable operations such as semaphore and mutex creation, waiting, and signaling, effectively coordinating task activities and synchronizing their behavior. <br /> The use of semaphores and mutexes is crucial in environments where multiple tasks need to access shared resources without conflict, as it helps maintain system stability and performance. Additionally, by leveraging semaphores and mutexes, developers can enhance the efficiency of their applications, ensuring that critical tasks are executed in a timely manner while preventing task starvation and optimizing resource utilization. 
 */

#include <gemrtos.h>

/** \brief Guard offset subtracted from G_LATEST_TIME to create a near-infinite timeout ECB value.
 *  Prevents the timed-event ECB from landing exactly on G_LATEST_TIME, which would
 *  be misinterpreted as "no timeout". Must be > 0. */
#define G_LATEST_TIME_GUARD_TICKS  10U

OPTIMIZE_CODE


/** gu_SemaphoreCreateCounting
 *  \brief Creates a counting semaphore and returns a handle by which the newly created semaphore can be referenced.
 *  \exception par_following The function takes the following parameters:
 *  \param [in] max_count The maximum count value that can be reached. When the semaphore reaches this value it can no longer be 'given'.
 *  \param [in] initial_count The count value assigned to the semaphore when it is created.
 *  \return If the semaphore is created successfully then a handle to the semaphore is returned, NULL otherwise
 *  \remark gu_SemaphoreCreateCounting
 *  \relates Semaphore_and_Mutex
*/
GS_RCB * gu_SemaphoreCreateCounting( int max_count,
                                    int initial_count)
{
	gm_GeMRTOSCriticalSectionEnter();
	
		// Get a Free resource
		GS_RCB *presource = gk_RCB_GetFree();

		if (presource != 0) {
			// Set the default values of the semaphore
			presource->RCBType = GS_RCBType_SEM_COUNTING;
			presource->semaphore.SEM_GrantedPriority.i64 = (G_UINT64) G_LOWEST_PRIORITY;
			
			// Initialize the counters		
			presource->semaphore.SEM_Maximum_Count       = (G_UINT32) max_count;
			presource->semaphore.SEM_Current_Count       = (G_UINT32) initial_count;
		}
	
	gm_GeMRTOSCriticalSectionExit();
	
	return((GS_RCB *) presource);	
}

/** gu_SemaphoreGetCount
 *  \brief Returns the count of a semaphore. The semaphore must have previously been created with a call to gu_SemaphoreCreateBinary(), gu_SemaphoreCreateMutex() or gu_SemaphoreCreateCounting().  
 *  \exception par_following The function takes the following parameters.
 *  \param [in] presource A handle to the semaphore being taken - obtained when the semaphore was created.
 *  \return If the semaphore is a counting semaphore then the semaphores current count value is returned. If the semaphore is a binary semaphore then 1 is returned if the semaphore is available, and 0 is returned if the semaphore is not available.
 *  \remark gu_SemaphoreGetCount
 *  \relates Semaphore_and_Mutex
*/
G_UINT32 gu_SemaphoreGetCount(const GS_RCB *presource)
{
	gm_GeMRTOSCriticalSectionEnter();
		PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid, value = %p\n", (void *) presource);	
		G_UINT32 count = presource->semaphore.SEM_Current_Count;
	gm_GeMRTOSCriticalSectionExit();
	return (count);
}

/** gu_SemaphoreCreateBinary
 *  \brief Creates a binary semaphore, and returns a handle by which the semaphore can be referenced. The semaphore is created in the 'empty' state, meaning the semaphore must first be given using the gu_SemaphoreGive() 
 *  \exception par_following The function takes no parameters.
 *  \param [in] initial_count The count value assigned to the semaphore when it is created.
 *  \return If the semaphore is created successfully then a handle to the semaphore is returned, NULL otherwise
 *  \remark gu_SemaphoreCreateBinary
 *  \relates Semaphore_and_Mutex
*/
GS_RCB *gu_SemaphoreCreateBinary(int initial_count)
{
	gm_GeMRTOSCriticalSectionEnter();
		GS_RCB *presource = gu_SemaphoreCreateCounting( 1, initial_count);
		if (presource == (GS_RCB *) 0) {
			gm_GeMRTOSCriticalSectionExit();
			return (GS_RCB *) 0;
		}
		presource->RCBType = GS_RCBType_SEM_BINARY;
	gm_GeMRTOSCriticalSectionExit();
	return presource;
}


/** gu_SemaphoreCreateMutex
 *  \brief Creates a mutex, and returns a handle by which the created mutex can be referenced. Mutexes are taken using gu_SemaphoreTake(), and given using gu_SemaphoreGive(). 
 *  \exception par_following The function takes no parameters.
 *  \return If the mutex type semaphore was created successfully then a handle to the created mutex is returned, NULL otherwise
 *  \remark gu_SemaphoreCreateMutex
 *  \relates Semaphore_and_Mutex
*/
GS_RCB * gu_SemaphoreCreateMutex(void)
{
	gm_GeMRTOSCriticalSectionEnter();
		GS_RCB *presource = gu_SemaphoreCreateCounting( 1, 1);
		if (presource == (GS_RCB *) 0) {
			gm_GeMRTOSCriticalSectionExit();
			return (GS_RCB *) 0;
		}
		presource->RCBType = GS_RCBType_SEM_MUTEX;
		presource->semaphore.SEM_Recurrence = 0;
	gm_GeMRTOSCriticalSectionExit();
	return presource;
}

/** gu_SemaphoreCreateRecursiveMutex
 *  \brief Creates a recursive mutex, and returns a handle by which the mutex can be referenced. Recursive mutexes are 'taken' (obtained) using the gu_SemaphoreTakeRecursive() and given (released) using the gu_SemaphoreGiveRecursive() API functions respectively.  
 *  \exception par_following The function takes no parameters.
 *  \return If the mutex type semaphore was created successfully then a handle to the created mutex is returned, NULL otherwise
 *  \remark gu_SemaphoreCreateRecursiveMutex
 *  \relates Semaphore_and_Mutex
*/
GS_RCB * gu_SemaphoreCreateRecursiveMutex(void)
{
	gm_GeMRTOSCriticalSectionEnter();
		GS_RCB *presource = gu_SemaphoreCreateCounting( 1, 1);
		if (presource == (GS_RCB *) 0) {
			gm_GeMRTOSCriticalSectionExit();
			return (GS_RCB *) 0;
		}
		presource->RCBType = GS_RCBType_SEM_RECURSIVEMUTEX;
		presource->semaphore.SEM_Recurrence = 0;
	gm_GeMRTOSCriticalSectionExit();
	return presource;
}

/** gu_SemaphoreDestroy
 *  \brief Destroys a semaphore or mutex created by one of the gu_SemaphoreCreate*
 *         functions, returning its Resource Control Block to the free pool.
 *         The resource must be idle: no task may currently hold it (mutex granted
 *         event) or be blocked waiting on it.  If a task holds or waits on the
 *         resource the function does nothing and returns G_FALSE â€” the caller
 *         must release/abort those tasks first (e.g. gu_TaskKill).
 *  \exception par_following The function accepts one parameter:
 *  \param [in] psemaphore A pointer to the GS_RCB of the semaphore/mutex to destroy.
 *  \return G_TRUE if the resource was destroyed; G_FALSE if it is still in use.
 *  \sa gu_SemaphoreCreateBinary, gu_SemaphoreCreateCounting, gu_SemaphoreCreateMutex
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gu_SemaphoreDestroy(GS_RCB *psemaphore)
{
	G_UINT32 result = G_FALSE;
	gm_GeMRTOSCriticalSectionEnter();
		PRINT_ASSERT((gkm_RCB_IsValid(psemaphore) == G_TRUE),"ERROR RCB not valid, %p\n", (void *) psemaphore);
		PRINT_ASSERT(((psemaphore->RCBType == (G_UINT32) GS_RCBType_SEM_BINARY)        ||
		              (psemaphore->RCBType == (G_UINT32) GS_RCBType_SEM_COUNTING)      ||
		              (psemaphore->RCBType == (G_UINT32) GS_RCBType_SEM_MUTEX)         ||
		              (psemaphore->RCBType == (G_UINT32) GS_RCBType_SEM_RECURSIVEMUTEX)),
		              "ERROR not a semaphore, RCBType= %d\n", (int) psemaphore->RCBType);

		/* Idle only: no granted holder (GEL) and no blocked waiter (WEL). */
		if ((get_pointed_field(psemaphore, RCB_NextRCBGEL) == (struct gs_ecb *) 0) &&
		    (get_pointed_field(psemaphore, RCB_NextRCBWEL) == (struct gs_ecb *) 0)) {
			set_pointed_field(psemaphore, RCBType, GS_RCBType_FREE);
			gk_RCBFL_Link(psemaphore);
			result = G_TRUE;
		}
	gm_GeMRTOSCriticalSectionExit();
	return (result);
}

/**
 * \brief Create and arm the pair of ECBs needed for a timed semaphore wait.
 *
 * Allocates a wait ECB (type \c G_ECBType_SEM_WAITING, priority-keyed) and a
 * timeout ECB (type \c G_ECBType_SEM_WAITING_TIMEOUT), inserts both into their
 * respective lists, and links them together via the AEL.
 *
 * \param [in] presource  Pointer to the semaphore resource control block.
 * \param [in] ptcb       Pointer to the calling task's TCB.
 * \param [in] timeout    Ticks to wait; \c G_LATEST_TIME means "wait indefinitely".
 * \return Pointer to the wait ECB (needed by the caller to read the wake-up result).
 */
static GS_ECB *gk_SEM_CreateTimeoutECB(GS_RCB *presource, GS_TCB *ptcb, G_UINT64 timeout)
{
	/* Create and link the wait ECB */
	GS_ECB *pevent = gk_ECB_GetFree(ptcb);
	if (pevent == (GS_ECB *) 0) { return (GS_ECB *) 0; }
	set_pointed_field(pevent, ECBType, G_ECBType_SEM_WAITING);
	set_pointed_field(pevent, ECBValue.i64, ptcb->TCBCurrentPriority);
	gk_RCBWEL_Link((GS_RCB *) presource, ptcb, pevent);

	/* Create and link the timeout ECB */
	GS_ECB *ptimed = gk_ECB_GetFree(ptcb);
	if (ptimed == (GS_ECB *) 0) {
		gk_RCBWEL_Unlink(pevent);
		gk_ECBFL_Link(pevent);
		return (GS_ECB *) 0;
	}
	if (timeout >= G_LATEST_TIME)
		set_pointed_field(ptimed, ECBValue.i64, G_LATEST_TIME - G_LATEST_TIME_GUARD_TICKS);
	else
		set_pointed_field(ptimed, ECBValue.i64, gu_SystemTimeGet() + timeout);
	set_pointed_field(ptimed, ECBType, (G_UINT32) G_ECBType_SEM_WAITING_TIMEOUT);
	gk_ECBTL_Link(ptimed);

	/* Associate wait ECB and timeout ECB */
	gk_ECBAEL_Link(pevent, ptimed);
	PRINT_ASSERT((pevent->ECB_AssocTCB == ptimed->ECB_AssocTCB), "ERROR diferent TCBs");

	return pevent;
}

/** gu_SemaphoreTake
 *  \brief The gu_SemaphoreTake function attempts to acquire a semaphore or mutex. If the resource's current count is greater than 0, the resource is granted to the calling task, and the count is decremented.  If the count is 0, the behavior depends on the blocking parameter: if blocking is G_TRUE, the task is suspended until the semaphore becomes available; if blocking is G_FALSE, the function returns immediately without blocking. When blocking, the task's ready priority is determined by its ready priority.
 *  \exception par_following The function uses the following parameters:
 *  \param [in] presource A pointer to the GS_RCB structure representing the semaphore resource, as returned by gu_SemaphoreCreate.
 *  \param [in] ticks_to_wait The time to wait in system time units. If zero, no time to wait an returns immediatly. Set G_LATEST_TIME if want to wait for until the resource become available.
 *  \return The gu_SemaphoreTake function returns G_TRUE if the semaphore was granted to the task, and G_FALSE when the semaphore was unavailable and blocking was G_FALSE.
 *  \sa gu_SemaphoreGive
 *  \remark gu_SemaphoreTake
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gu_SemaphoreTake(GS_RCB *presource, G_UINT64 ticks_to_wait)
{
	GS_ECB *pevent;
	GS_TCB *ptcb;
	G_UINT32 result = G_FALSE;
    gm_GeMRTOSCriticalSectionEnter();

		PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid, value = %p\n", (void *) presource);
		
		ptcb = gk_PCB_GetCurrentTCB();
		pevent = gk_TCB_in_RCBGEL(presource, ptcb);
		
		// Check if task holds mutex
		if (pevent != 0) {
			if (presource->RCBType == GS_RCBType_SEM_RECURSIVEMUTEX) {
				presource->semaphore.SEM_Recurrence++;
				result = G_TRUE;
			}
		}
		else
		{
			// Check if it is available to be taken
			if (presource->semaphore.SEM_Current_Count > 0)
			{   // resource available granted without waiting
				if ((presource->RCBType == GS_RCBType_SEM_BINARY) || (presource->RCBType == GS_RCBType_SEM_COUNTING)) {
					presource->semaphore.SEM_Current_Count--;
					result = G_TRUE;
				}
				else
				{	// It is a MUTEX
					pevent = gk_ECB_GetFree(ptcb);
					if (pevent == (GS_ECB *) 0) {
						gm_GeMRTOSCriticalSectionExit();
						return G_FALSE;
					}
					gk_SemaphoreGrant(presource, pevent);
					result = G_TRUE;
				}
			} 
			else 
			{
				if (ticks_to_wait != 0) {
					pevent = gk_SEM_CreateTimeoutECB(presource, ptcb, ticks_to_wait);
					if (pevent == (GS_ECB *) 0) {
						result = G_FALSE;
					} else {
					gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_WAITING);  //Switch tasks
					/* From suspended it can became:
						- pevent->ECBType = G_ECBType_SEM_WAITING_EXPIRED
								when timeout happens in gk_TimeCallback function
						- pevent->ECBType = G_ECBType_SEM_GRANTED
								when timeout happens in gu_SemaphoreGive function
					*/

					PRINT_ASSERT((gm_IsPrcGranted()),"ERROR SIGNAL in critical section\n");

					switch (pevent->ECBType) {
						case G_ECBType_SEM_WAITING_EXPIRED:  // Semaphore not granted
							PRINT_ASSERT((pevent->ECB_NextECBAEL == (struct gs_ecb *) 0), "ERROR event not alone, %s", ptcb->TCB_description);
							gk_ECBFL_Link(pevent);
							result = G_FALSE;
							break;

						case G_ECBType_SEM_GRANTED:         // If it is a semaphore then the event is free
							PRINT_ASSERT((pevent->ECB_NextECBAEL == (struct gs_ecb *) 0), "ERROR event not alone, %s", ptcb->TCB_description);
							if ((presource->RCBType == GS_RCBType_SEM_BINARY) || (presource->RCBType == GS_RCBType_SEM_COUNTING)) gk_ECBFL_Link(pevent);
							result = G_TRUE;  // resource available after waiting
							break;

						default:
							PRINT_ASSERT((G_TRUE == G_FALSE), "pevent->ECBType= %d, task= %s In %s, %d \n",pevent->ECBType, ptcb->TCB_description,  __FUNCTION__, __LINE__);
							G_DEBUG_WHILEFOREVER;
							break;
					}
					} /* end pevent != NULL */
				}
				else 
				{
					result = G_FALSE;
				}
			}
		}
    gm_GeMRTOSCriticalSectionExit();

    return (result);	
}


/**gu_SemaphoreGive
 *  \brief The gu_SemaphoreGive function releases a semaphore or mutex previously acquired. If tasks are waiting to acquire the resource, the highest-priority waiting task will be granted the resource. If no tasks are waiting, the semaphore's internal count is incremented.
 *  \exception par_following The function accepts one parameter:
 *  \param [in] presource A pointer to the GS_RCB structure representing the semaphore resource. This pointer was returned by the gu_SemaphoreCreate function.
 *  \return The gu_SemaphoreGive function returns G_TRUE if the semaphore was successfully released, and G_FALSE otherwise.
 *  \sa gu_SemaphoreTake
 *  \remark gu_SemaphoreGive
 *  \relates Semaphore_and_Mutex
 */ 
G_UINT32 gu_SemaphoreGive(GS_RCB *presource)
{
	GS_ECB *pevent;

	GS_TCB *ptcb;
	G_UINT32 result = G_FALSE;

	gm_GeMRTOSCriticalSectionEnter();

		ptcb   = gk_PCB_GetCurrentTCB();
		
		switch (presource->RCBType) {
			case GS_RCBType_SEM_MUTEX:
			case GS_RCBType_SEM_RECURSIVEMUTEX:
				// Check if task holds mutex
				pevent = gk_TCB_in_RCBGEL(presource, ptcb);
				PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB is not valid gu_SemaphoreGive\n");
				if (pevent != (GS_ECB *) 0) {
					if (presource->RCBType == GS_RCBType_SEM_RECURSIVEMUTEX) presource->semaphore.SEM_Recurrence--;
					else presource->semaphore.SEM_Recurrence = 0;
					PRINT_ASSERT(((G_INT32) presource->semaphore.SEM_Recurrence >= 0), "ERROR in recurrece SEM_Recurrence=%x \n", presource->semaphore.SEM_Recurrence);

					if (presource->semaphore.SEM_Recurrence == 0)  // it may be non-recurrent mutex also
					{
						// Release the Mutex
						gk_RCBGEL_Unlink(pevent);
						gk_ECBFL_Link(pevent);
						presource->semaphore.SEM_Current_Count++;

						gk_SemaphoreGrantWaiting(presource);
					}
					result = G_TRUE;
				}
				break;
			
			case GS_RCBType_SEM_COUNTING:
			case GS_RCBType_SEM_BINARY:
				if (presource->semaphore.SEM_Current_Count >= presource->semaphore.SEM_Maximum_Count) {
					result = G_FALSE;
				} else {
					presource->semaphore.SEM_Current_Count++;
					gk_SemaphoreGrantWaiting(presource);
					result = G_TRUE;
				}
				break;
				
			default:
				/* MISRA C:2012 Rule 21.6: PRINT_ASSERT instead of fprintf(stderr). */
				PRINT_ASSERT((G_FALSE == G_TRUE), "ERROR presource->RCBType= %d\n", (int) presource->RCBType);
				G_DEBUG_WHILEFOREVER;
				break;
		}

	gm_GeMRTOSCriticalSectionExit();

	return (result);	
}


/**
 *  \brief Handles an expired semaphore timed-wait event.
 *         Called by the GeMRTOS kernel when a timeout ECB fires while a task
 *         is waiting for a semaphore.  The function unlinks the task's waiting
 *         event from the resource wait list, marks it as expired, frees the
 *         timeout ECB, and moves the task back to the ready list so it can
 *         detect the timeout on its next run.
 *  \param [in] ptimed  Pointer to the timed ECB (G_ECBType_SEM_WAITING_TIMEOUT)
 *                      that has expired.
 *  \return G_TRUE always.
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gk_SemaphoreTimeout(GS_ECB *ptimed)
{
	GS_ECB *pevent;
	GS_TCB *ptcb;

	GK_SAMPLE_FUNCTION_BEGIN(10082)

	ptcb = get_pointed_field(ptimed,ECB_AssocTCB);
	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ECB not valid\n");
	
	pevent = get_pointed_field(ptimed, ECB_NextECBAEL);
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");
	
	PRINT_ASSERT((ptcb == pevent->ECB_AssocTCB),"ERROR ECBs associated to different TCB\n");
	
	gk_RCBWEL_Unlink(pevent);
	set_pointed_field(pevent, ECBType, G_ECBType_SEM_WAITING_EXPIRED);
	set_pointed_field(pevent, ECBValue.i64, G_LOWEST_PRIORITY);
	gk_ECBFL_Link(ptimed);
    
	gk_TCB_Unlink(ptcb);
	gk_TCBRDYL_Link(ptcb);

	GK_SAMPLE_FUNCTION_END(10082)

	return (G_TRUE);
}


/**
 *  \brief Cleans up all semaphore-related ECBs when their owning task is killed.
 *         Dispatches on the ECB type: G_ECBType_SEM_GRANTED returns the mutex
 *         count and grants the resource to the next waiter;
 *         G_ECBType_SEM_WAITING cancels the wait and frees the timeout ECB;
 *         G_ECBType_SEM_WAITING_EXPIRED is a no-op (already unlinked);
 *         G_ECBType_SEM_WAITING_TIMEOUT cancels both the wait and timeout ECBs.
 *  \param [in] pevent  Pointer to the semaphore ECB associated with the killed task.
 *  \return G_TRUE always.
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gk_SemaphoreKill(GS_ECB *pevent)
{
	GS_ECB *ptimed;

	GK_SAMPLE_FUNCTION_BEGIN(10083)

    GS_RCB *prcb = (GS_RCB *) get_pointed_field(pevent, ECB_AssocRCB);

	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");

    switch (pevent->ECBType)
    {
        case G_ECBType_SEM_GRANTED:
			/* Counting semaphores are not held by tasks â€” taking one consumes a
			 * resource unit but does not establish ownership.  Count restoration
			 * on kill would be wrong by design.  Only mutexes establish
			 * ownership, so only they restore the count and re-grant to waiters. */
			PRINT_ASSERT((gkm_RCB_IsValid(prcb) == G_TRUE),"ERROR RCB is not valid, prcb= %p, pevent->ECBType= %d, task= %s\n", (GS_RCB *) prcb, pevent->ECBType, pevent->ECB_AssocTCB->TCB_description);
			PRINT_ASSERT((prcb->semaphore.SEM_Current_Count <= prcb->semaphore.SEM_Maximum_Count),"SEM ERROR COUNT, current= %d, Maximun= %d, prcb->RCBType= %d, %s", (int) prcb->semaphore.SEM_Current_Count, (int) prcb->semaphore.SEM_Maximum_Count, (int) prcb->RCBType, pevent->ECB_AssocTCB->TCB_description);
			PRINT_ASSERT((get_pointed_field(pevent, ECB_NextECBAEL) == (GS_ECB *) 0),"ERROR in G_ECBType_SEM_GRANTED, Timeout?");

			prcb->semaphore.SEM_Recurrence = 0;
            if ((prcb->RCBType == GS_RCBType_SEM_MUTEX) || (prcb->RCBType == GS_RCBType_SEM_RECURSIVEMUTEX)) {
				prcb->semaphore.SEM_Current_Count++;
                gk_RCBGEL_Unlink(pevent);
				gk_SemaphoreGrantWaiting(prcb);
            }
            break;
            
        case G_ECBType_SEM_WAITING:		
			ptimed = get_pointed_field(pevent, ECB_NextECBAEL);
			PRINT_ASSERT((gkm_ECB_IsValid(ptimed) == G_TRUE),"ERROR ECB not valid\n");
			gk_ECBTL_Unlink(ptimed);
			gk_ECBFL_Link(ptimed);				
			gk_RCBWEL_Unlink(pevent);
            break;

		case G_ECBType_SEM_WAITING_EXPIRED:
			break;			

		case G_ECBType_SEM_WAITING_TIMEOUT:
			/* On entry pevent IS the timeout ECB; swap so ptimed=timeout, pevent=wait ECB. */
			ptimed = pevent;
			pevent = get_pointed_field(ptimed, ECB_NextECBAEL);
			PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");
			gk_RCBWEL_Unlink(pevent);
			gk_ECBFL_Link(pevent);

			gk_ECBTL_Unlink(ptimed);
			break;
	
		default:
			/* MISRA C:2012 Rule 21.6: PRINT_ASSERT instead of fprintf(stderr). */
			PRINT_ASSERT((G_FALSE == G_TRUE), "ERROR pevent->ECBType= %d\n", pevent->ECBType);
			G_DEBUG_WHILEFOREVER;
			break;
	}

	GK_SAMPLE_FUNCTION_END(10083)

	return (G_TRUE);
}


/**
 *  \brief Grants a semaphore or mutex to the highest-priority waiting task.
 *         Checks RCB_NextRCBWEL; if a task is waiting, removes its timeout ECB,
 *         calls gk_SemaphoreGrant() to mark the ECB as granted, and moves the
 *         task to the ready list.  If no task is waiting the function is a no-op.
 *  \param [in] prcb  Pointer to the semaphore or mutex Resource Control Block
 *                    whose wait list should be examined.
 *  \return G_TRUE always.
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gk_SemaphoreGrantWaiting(GS_RCB *prcb)
{
	GS_TCB *ptcb;
	
	GK_SAMPLE_FUNCTION_BEGIN(10084)
	
	/* Read only by the PRINT_ASSERT below, which compiles out when
	 * G_DEBUG_WHILEFOREVER_ENABLE == 0; consumed so production builds stay
	 * warning-free. */
	const GS_TCB *ptcbcurrent = gk_PCB_GetCurrentTCB();
	(void) ptcbcurrent;
	GS_ECB *ptimed;
	// Check if there exist a waiting event
	GS_ECB *pevent = get_pointed_field(prcb, RCB_NextRCBWEL);
	if (pevent != (GS_ECB *) 0)
	{
		// Remove the timed event. There exist because it is waiting
		ptimed = get_pointed_field(pevent, ECB_NextECBAEL);
		PRINT_ASSERT((gkm_ECB_IsValid(ptimed) == G_TRUE),"ERROR ECB not valid\n");
		gk_ECBTL_Unlink(ptimed);
		gk_ECBFL_Link(ptimed);
		
		PRINT_ASSERT((pevent->ECBType == G_ECBType_SEM_WAITING), "ERROR Not Waiting");
		
		ptcb = pevent->ECB_AssocTCB;
		
		gk_RCBWEL_Unlink(pevent);
		
		// Grant the resource
		gk_SemaphoreGrant(prcb, pevent);
		
		// Resume waiting task
		PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB not valid\n");
		PRINT_ASSERT((ptcb != ptcbcurrent),"ERROR TCB not valid, ptcb= %s, ptcbcurrent= %s\n", ptcb->TCB_description, ptcbcurrent->TCB_description);
		gk_TCB_Unlink(ptcb);
		gk_TCBRDYL_Link(ptcb);
	}
	
	GK_SAMPLE_FUNCTION_END(10084)
	
	return G_TRUE;
}

/**
 *  \brief Grants a semaphore or mutex resource to an ECB/task pair.
 *         Marks the ECB as G_ECBType_SEM_GRANTED and associates it with the
 *         resource.  For mutex and recursive-mutex types, updates the ceiling
 *         priority (SEM_GrantedPriority), links the ECB into the granted-event
 *         list, and for recursive-mutex sets SEM_Recurrence to 1.
 *         Decrements SEM_Current_Count for all semaphore types.
 *  \param [in] prcb    Pointer to the semaphore or mutex Resource Control Block.
 *  \param [in] pevent  Pointer to the waiting ECB that will receive the grant.
 *  \return G_TRUE always.
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gk_SemaphoreGrant(GS_RCB *prcb, GS_ECB * pevent)
{
	GK_SAMPLE_FUNCTION_BEGIN(10085)

	GS_TCB *ptcb = get_pointed_field(pevent, ECB_AssocTCB);
	set_pointed_field(pevent, ECBType, G_ECBType_SEM_GRANTED);
	set_pointed_field(pevent, ECB_AssocRCB, prcb);
	
	// if mutex, then update task priority to ceiling
	// SEM_GrantedPriority uses a static-ceiling protocol (intentional design):
	// the ceiling ratchets up with more-urgent holders and persists across all
	// future holders until re-initialization. Not reset on each holder release.
	if ((prcb->RCBType == GS_RCBType_SEM_MUTEX) || (prcb->RCBType == GS_RCBType_SEM_RECURSIVEMUTEX)) {
		if ((G_UINT64) ptcb->TCBCurrentPriority < (G_UINT64) prcb->semaphore.SEM_GrantedPriority.i64) {
			prcb->semaphore.SEM_GrantedPriority.i64 = (G_UINT64) ptcb->TCBCurrentPriority;
		}
		pevent->ECBValue.i64 = prcb->semaphore.SEM_GrantedPriority.i64;
		gk_RCBGEL_Link((GS_RCB *) prcb, pevent->ECB_AssocTCB, pevent);
        
        gk_TCBUpdate_Priority(ptcb);         
	}
	
	PRINT_ASSERT((prcb->semaphore.SEM_Current_Count <= prcb->semaphore.SEM_Maximum_Count),"SEM ERROR COUNT");
	PRINT_ASSERT((prcb->semaphore.SEM_Current_Count > 0),"SEM ERROR COUNT 0");
	prcb->semaphore.SEM_Current_Count--;
	if (prcb->RCBType == GS_RCBType_SEM_RECURSIVEMUTEX) prcb->semaphore.SEM_Recurrence = 1;

	GK_SAMPLE_FUNCTION_END(10085)

	return (G_TRUE);
}

OPTIMIZE_RESTORE

