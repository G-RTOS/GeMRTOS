/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS Semaphore definitions
 *  \details This file contains the Semaphore definitions.
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


#ifndef GEMRTOS_SEM_H_
#define GEMRTOS_SEM_H_

/**
 * \defgroup Semaphore_API Semaphore and Mutex User API
 * \brief Public semaphore and mutex functions exposed to application tasks.
 * \relates Semaphore_and_Mutex
 * @{
 */

/** \brief Creates a counting semaphore and returns a handle by which the
 *         newly created semaphore can be referenced.
 *  \param [in] max_count     The maximum count value that can be reached.
 *                            When the semaphore reaches this value it can no
 *                            longer be 'given'.
 *  \param [in] initial_count The count value assigned to the semaphore when
 *                            it is created.
 *  \return Handle to the newly created semaphore, or NULL if creation failed.
 *  \relates Semaphore_and_Mutex
 */
GS_RCB *gu_SemaphoreCreateCounting(int max_count, int initial_count);

/** \brief Returns the current count of a semaphore.
 *         The semaphore must have previously been created with
 *         gu_SemaphoreCreateBinary(), gu_SemaphoreCreateMutex(), or
 *         gu_SemaphoreCreateCounting().
 *  \param [in] presource A handle to the semaphore — obtained when the
 *                        semaphore was created.  The pointed-to resource is
 *                        not modified (MISRA C:2012 Rule 8.13).
 *  \return Current count value for a counting semaphore; 1 if a binary
 *          semaphore is available, 0 otherwise.
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gu_SemaphoreGetCount(const GS_RCB *presource);

/** \brief Creates a binary semaphore and returns a handle by which the
 *         semaphore can be referenced.
 *  \param [in] initial_count The count value assigned to the semaphore when
 *                            it is created (0 = empty, 1 = available).
 *  \return Handle to the newly created semaphore, or NULL if creation failed.
 *  \relates Semaphore_and_Mutex
 */
GS_RCB *gu_SemaphoreCreateBinary(int initial_count);

/** \brief Destroys a semaphore/mutex and returns its RCB to the free pool.
 *         Fails (returns G_FALSE) if a task currently holds or waits on it.
 *  \param [in] psemaphore Handle returned by a gu_SemaphoreCreate* function.
 *  \return G_TRUE if destroyed; G_FALSE if still in use.
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gu_SemaphoreDestroy(GS_RCB *psemaphore);

/** \brief Creates a mutex and returns a handle by which the created mutex
 *         can be referenced.  Mutexes are taken using gu_SemaphoreTake()
 *         and given using gu_SemaphoreGive().
 *  \return Handle to the newly created mutex, or NULL if creation failed.
 *  \relates Semaphore_and_Mutex
 */
GS_RCB *gu_SemaphoreCreateMutex(void);

/** \brief Creates a recursive mutex and returns a handle by which the mutex
 *         can be referenced.  Recursive mutexes may be taken multiple times
 *         by the same task without deadlocking; each take must be matched by
 *         a corresponding give.
 *  \return Handle to the newly created recursive mutex, or NULL if creation
 *          failed.
 *  \relates Semaphore_and_Mutex
 */
GS_RCB *gu_SemaphoreCreateRecursiveMutex(void);

/** \brief Attempts to acquire a semaphore or mutex.
 *         If the resource's current count is greater than 0, it is granted to
 *         the calling task and the count is decremented.  If the count is 0
 *         and \p ticks_to_wait is non-zero, the calling task is suspended until
 *         the semaphore becomes available or the timeout expires.
 *  \param [in] presource    A pointer to the GS_RCB structure representing
 *                           the semaphore, as returned by the creation
 *                           function.
 *  \param [in] ticks_to_wait  Time to wait in system ticks.  Pass 0 to return
 *                             immediately without blocking; pass G_LATEST_TIME
 *                             to wait indefinitely.
 *  \return G_TRUE if the semaphore was granted; G_FALSE if it was unavailable
 *          and the timeout elapsed.
 *  \sa gu_SemaphoreGive
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gu_SemaphoreTake(GS_RCB *presource, G_UINT64 ticks_to_wait);

/** \brief Releases a semaphore or mutex previously acquired.
 *         If tasks are waiting to acquire the resource, the highest-priority
 *         waiting task is granted the resource.  If no tasks are waiting, the
 *         semaphore's internal count is incremented.
 *  \param [in] presource A pointer to the GS_RCB structure representing the
 *                        semaphore, as returned by the creation function.
 *  \return G_TRUE if the semaphore was successfully released; G_FALSE
 *          otherwise.
 *  \sa gu_SemaphoreTake
 *  \relates Semaphore_and_Mutex
 */
G_UINT32 gu_SemaphoreGive(GS_RCB *presource);

/* gu_SemaphoreCreate(), gu_SemaphoreWait(), and gu_SemaphorePost() are not
 * implemented.  Use the following functions instead:
 *   - gu_SemaphoreCreateCounting() / gu_SemaphoreCreateBinary()
 *   - gu_SemaphoreTake()
 *   - gu_SemaphoreGive()
 */

/** @} */ /* end Semaphore_API */

/**
 *  \brief The <b>T_SEMAPHORE_RESOURCE</b> is defined as the field <em>"semaphore"</em> in a GS_RCB resource. So, the fields of the T_SEMAPHORE_RESOURCE structure should be addressed as: <em>(GS_RCB *)->semaphore.\<field\></em>
 *  \relates Semaphore_and_Mutex
 */
struct T_SEMAPHORE_RESOURCE {
	TIMEPRIORITY  SEM_GrantedPriority; ///< \brief Priority to be assigned to the task granting the semaphore
    G_UINT32  SEM_Current_Count;        ///< \brief It is the current count of the semaphore. If it is equal to 0, no more grants are allowed. It is initialized with the SEM_Maximum_Count when the semaphore is created with the <em>gu_SemaphoreCreate</em> function.
    G_UINT32  SEM_Maximum_Count;        ///< \brief It is maximun number of times the semaphore can be taken.
    G_UINT32  SEM_Recurrence;           ///< \brief For recurrence mutex, the number of times the mutex has be taken.
};



#endif /* GEMRTOS_SEM_H_ */
