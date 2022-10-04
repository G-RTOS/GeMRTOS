/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS Semaphore functions
 *  \details This file contains the functions for semaphore implementation.
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


#include <gemrtos_core.h>

OPTIMEZE_CODE(3)

/***********************************************************************************
*********************** SEMAPHORE                        ***************************
***********************************************************************************/

/**gu_sem_create
 *  \brief 
 *  Creates a new semaphore resource and returns the pointer to the G_RCB structure
 *  or NULL if there is not a G_RCB structure available
 *  \param [in] RCB_Priority      Default priority of the Resource for granted and waiting events if not specified
 *  \param [in] initial_count     Initial count of the semaphore to allow simultaneous granted tasks
 *  \param [in] RCBWaitingTimeout Default waiting timeout
 *  \param [in] RCBGrantedTimeout Default granted timeout
 *  \return Pointer to a RCB structure or NULL if there is not a RCB structure available
 *  \relates Semaphore
 */
t_semaphore_resource *gu_sem_create(gt_time RCB_Priority,
                     int initial_count,
                     gt_time RCBWaitingTimeout,
                     gt_time RCBGrantedTimeout)
{
	G_RCB *presource = (G_RCB * ) 0;
    GRTOS_USER_CRITICAL_SECTION_GET;    
        // Get a Free resource
        presource = gk_RCB_GetFree();
        if (presource != 0) {
            // Set the default values of the semaphore
            presource->RCBCount = initial_count;
            presource->RCBGrantedPriority = (TIMEPRIORITY) RCB_Priority;
            presource->RCBGrantedTimeout = (TIMEPRIORITY) RCBGrantedTimeout;
            presource->RCB_NextRCBGEL = (struct gs_ecb *) 0;
            presource->RCB_NextRCB = (struct g_rcb *) 0;
            presource->RCB_NextRCBASL = (struct gs_scb *) 0;
            presource->RCB_NextRCBWEL  = (struct gs_ecb *) 0;
            presource->RCBPriority = (TIMEPRIORITY) RCB_Priority;
            presource->RCBState = GK_RCBState_SEM;
            presource->RCBType = GK_RCBType_SEM;
            presource->RCBWaitingTimeout = (TIMEPRIORITY) RCBWaitingTimeout;
            presource->semaphore.SEMMaxCount = initial_count;
        }
	GRTOS_CMD_CRITICAL_SECTION_RELEASE;
	return((t_semaphore_resource *) presource);
}

/**gu_sem_destroy
 *  \brief 
 *  Links the RCB of a semaphore to the free list if there is no event granted or waiting
 *  \param [in] presource Pointer to the RCB of the semaphore
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Analyse to remove all list and trigger signal to tasks
 *  \relates Semaphore
 */
INT32 gu_sem_destroy(t_semaphore_resource *presource)
{
	if (presource->RCB_NextRCBWEL == (struct gs_ecb *) 0 &&
		presource->RCB_NextRCBGEL == (struct gs_ecb *) 0)
	{
		gk_RCBFL_Link((G_RCB *) presource);
		return((INT32) G_TRUE);
	}
	return((INT32) G_FALSE);
}
 
/**gu_sem_wait
 *  \brief 
 *  Lock the semaphore (get the semaphore) if it is avaible, otherwise waits for grant it.
 *  \param [in] presource          Pointer to the semaphore t_semaphore_resource
 *  \param [in] waiting_priority   priority to wait for the semaphore to be granted 
 *  \param [in] RCBGrantedPriority Priority when semaphore is granted
 *  \param [in] RCBWaitingTimeout  Timeout for waiting for the semaphore (0, no waiting for semaphore)
 *  \param [in] RCBGrantedTimeout  Timeout for granted (0, no timeout when it is granted)
 *  \param [in] blocking           If G_TRUE the blocks the executing waiting for the resource
 *  \return G_TRUE if semaphore is granted, G_FALSE otherwise
 *  \relates Resource
 */
INT32 gu_sem_wait(t_semaphore_resource *presource,
				gt_priority waiting_priority,
				gt_priority RCBGrantedPriority,
				gt_time RCBWaitingTimeout,
				gt_time RCBGrantedTimeout,
                int blocking)
{
	GS_RRDS *prrds;
	GS_TCB  *ptcb;
	GS_ECB  *pevent;
	GS_ECB  *peventime;
    int retorno = G_FALSE;
    
    if (presource != (t_semaphore_resource *) 0) {
        GRTOS_USER_CRITICAL_SECTION_GET;
            ptcb = gk_PCB_GetCurrentTCB();
            pevent = gk_TASK_RESOURCE_CREATE((G_RCB *) presource, 
                                             (GS_TCB *) ptcb, 
                                             (INT64) waiting_priority,
		                                     (INT64) RCBGrantedPriority,
			                                 (INT64) RCBWaitingTimeout,
				                             (INT64) RCBGrantedTimeout);
            if (pevent != (GS_ECB *) 0) {
                peventime = pevent->ECB_NextECBAEL;
                prrds      = (GS_RRDS *) pevent->ECB_RRDS;

                if (presource->RCBCount > 0) {
                    // gk_TASK_RESOURCE_GRANT(presource, pevent);
                    presource->RCBCount--;
                    pevent->ECBType      = G_ECBType_SEM_GRANTED;
                    peventime->ECBType   = G_ECBType_TIMEOUT_SEM_GRANTED;
                    pevent->ECBValue.i64 = (INT64) prrds->RRDSGrantedPriority.i64;
                    if (prrds->RRDSGrantedTimeout.i64 != (INT64) 0) {    
                        peventime->ECBValue.i64 = (INT64) GRTOS_now + (INT64) prrds->RRDSGrantedTimeout.i64;
                    } else {
                        peventime->ECBValue.i64 = (INT64) G_LATEST_TIME - (INT64) 100;
                    }                    
                    gk_RCBGEL_Link((G_RCB *) presource, pevent);     
                    gk_ECBTL_Link(peventime);                    
                    retorno = G_TRUE;     // resource available granted without waiting
                    
                } else {
                    pevent->ECBType      = G_ECBType_SEM_WAITING;
                    if (blocking == G_TRUE) {
                        // gk_TASK_RESOURCE_WAIT((G_RCB *)presource, (GS_ECB *) pevent);
                        // pevent->ECBType      = G_ECBType_SEM_WAITING;
                        pevent->ECBValue.i64 = (INT64) prrds->RRDSWaitingPriority.i64;     // Insert it in waiting event list
                        if (prrds->RRDSWaitingTimeout.i64 != (INT64) 0) {    
                            peventime->ECBValue.i64 = (INT64) GRTOS_now + (INT64) prrds->RRDSWaitingTimeout.i64;
                        } else {
                            peventime->ECBValue.i64 = (INT64) G_LATEST_TIME - (INT64) 100;
                        }
                        peventime->ECBType = G_ECBType_TIMEOUT_SEM_WAITING;
                        gk_ECBTL_Link(peventime);
                        gk_RCBWEL_Link((G_RCB *) presource, pevent);              

                        gk_KERNEL_TASK_SUSPEND_CURRENT();    //Switch tasks                         
                        // #################################################
                        // #################################################
                        GRTOS_USER_CRITICAL_SECTION_GET;
                        // 1) Resource was granted
                        //   pevent->ECBState    = GS_ECBState_GRANTED_RESOURCE 
                        //   pevent->ECBType     = G_ECBType_SEM_GRANTED
                        //   peventime->ECBState = GS_ECBState_WAITING_TIME
                        // 2) Waiting Timeout happened (gk_timeout_ECB_SEM_wait was executed)
                        //   pevent->ECBState    = GS_ECBState_UNLINKED
                        //   peventime->ECBState = GS_ECBState_UNLINKED
                        
                        if (pevent->ECBType == G_ECBType_SEM_GRANTED)
                        { // The resource is granted to task
                            retorno = G_TRUE;  // resource available after waiting
                        }
                    }
                    if (pevent->ECBType == G_ECBType_SEM_WAITING) {
                        gk_TASK_RESOURCE_DESTROY((GS_ECB *) pevent);
                        retorno = G_FALSE;      // resource no available without waiting
                    }
                }
            }
            else
            {
                retorno = G_FALSE; // no control blocks for task resource
            }
        GRTOS_CMD_CRITICAL_SECTION_RELEASE;
    }
    else
    {
        retorno = G_FALSE;  // presource is not a valid resource
    }
    return retorno;
}

// ################################################# 
 
/**gu_sem_post
 *  \brief 
 *  Unlocks the semaphore (release the semaphore). Set ready the task waiting for semaphore
 *  \param [in] presource Pointer to the RCB of the semaphore
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if semaphore is granted to the task doing the post
 *  \relates Semaphore
 */ 
INT32 gu_sem_post(t_semaphore_resource *presource)
{
	GS_TCB *ptcb;
    // GS_RRDS *prrds;
	GS_ECB *pevent;
	// GS_ECB *peventime;
    int granted = G_FALSE;

	GRTOS_USER_CRITICAL_SECTION_GET;

		ptcb   = gk_PCB_GetCurrentTCB(); PRINT_DEBUG_LINE

		//Check if task is granted for this resource
		pevent = gk_TCB_in_RCBGEL((G_RCB *) presource, ptcb); 
        if (pevent != (GS_ECB *) 0) {
            granted = gk_TASK_RESOURCE_UNGRANT((G_RCB *) presource, (GS_ECB *) pevent); 
            if (granted == G_TRUE) {
                gk_TASK_RESOURCE_DESTROY((GS_ECB *) pevent);
                // peventime = pevent->ECB_NextECBAEL;
                // gk_ECBFL_Link(peventime);         
                // gk_ECBFL_Link(pevent); 
            }
        }
	GRTOS_CMD_CRITICAL_SECTION_RELEASE; PRINT_DEBUG_LINE

	return(G_TRUE);
}
 
/**gu_sem_getvalue
 *  \brief 
 *  Return the current value of the semaphore
 *  \param [in] presource Pointer to the RCB of the semaphore
 *  \return Current value of the semaphore
 *  \todo Check if this function is necessary
 *  \relates Semaphore
 */
INT32 gu_sem_getvalue(t_semaphore_resource *presource)
{
	return (presource->RCBCount);
}


/**gk_TASK_RESOURCE_WAIT
 *  \brief 
 *  Sets the waiting and timed event for waiting the semaphore
 *  \param [in] presource Pointer to the RCB of the semaphore
 *  \param [in] pevent    Pointer to de ECB of the event
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Eliminate this function replacing the code where it is called from
 *  \relates Semaphore
 */
INT32 gk_TASK_RESOURCE_WAIT(G_RCB *presource, GS_ECB *pevent) 
{
    GS_ECB *peventime;
    GS_RRDS *prrds;
    
    // Event with ECB_RRDS != 0 is the resource event, otherwise time event
    if (pevent->ECB_RRDS == (struct gs_rrds *) 0) pevent = pevent->ECB_NextECBAEL;
    peventime = pevent->ECB_NextECBAEL;
    prrds      = (GS_RRDS *) pevent->ECB_RRDS;
	switch (presource->RCBType)
	{
		case GK_RCBType_SEM:  
            pevent->ECBType = G_ECBType_SEM_WAITING;
            pevent->ECBValue.i64 = (INT64) prrds->RRDSWaitingPriority.i64;     // Insert it in waiting event list
            if (prrds->RRDSWaitingTimeout.i64 != (INT64) 0) {    
                peventime->ECBValue.i64 = (INT64) GRTOS_now + (INT64) prrds->RRDSWaitingTimeout.i64;
            } else {
                peventime->ECBValue.i64 = (INT64) G_LATEST_TIME - (INT64) 100;
            }
            peventime->ECBType = G_ECBType_TIMEOUT_SEM_WAITING;
            break;      
        
        default:
            G_DEBUG_WHILEFOREVER;
		break;
	}    
    gk_ECBTL_Link(peventime);
    gk_RCBWEL_Link((G_RCB *) presource, pevent);              

    gk_KERNEL_TASK_SUSPEND_CURRENT();    //Switch tasks    
    return(G_TRUE);
}

/**gk_TASK_RESOURCE_UNWAIT
 *  \brief 
 *  Unlinks the ECB waiting and timed set for waiting a semaphore
 *  \param [in] presource Pointer to the RCB of the semaphore
 *  \param [in] pevent    Pointer to de ECB of the event
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Eliminate the code replacing it where it is called from
 *  \relates Semaphore
 */
INT32 gk_TASK_RESOURCE_UNWAIT(G_RCB *presource, GS_ECB *pevent) 
{
    GS_ECB * peventime;
    
    // Event with ECB_RRDS != 0 is the resource event, otherwise time event
    if (pevent->ECB_RRDS == (struct gs_rrds *) 0) pevent = pevent->ECB_NextECBAEL;
    peventime = pevent->ECB_NextECBAEL;

    gk_RCBWEL_Unlink(pevent); PRINT_DEBUG_LINE        // It is waiting: unlink from waiting list
    if (peventime->ECBState == GS_ECBState_WAITING_TIME) {
        gk_ECBTL_Unlink(peventime); PRINT_DEBUG_LINE // Unlink waiting timeout event
    }
    return(G_TRUE);
}

/**gk_TASK_RESOURCE_GRANT
 *  \brief 
 *  Sets the granted and timed events when a semaphore is granted to a TCB
 *  \param [in] presource Pointer to the RCB of the semaphore
 *  \param [in] pevent    Pointer to the ECB of the event
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Eliminate the function replacing the code
 *  \relates Semaphore
 */
INT32 gk_TASK_RESOURCE_GRANT(G_RCB *presource, GS_ECB *pevent) 
{
    GS_ECB * peventime;
    GS_RRDS *prrds;
    
    // Event with ECB_RRDS != 0 is the resource event, otherwise time event
    if (pevent->ECB_RRDS == (struct gs_rrds *) 0) pevent = pevent->ECB_NextECBAEL;
    peventime = pevent->ECB_NextECBAEL;
    prrds      = (GS_RRDS *) pevent->ECB_RRDS;
    
    // Assign event type according the resource type
	switch (presource->RCBType)
	{
		case GK_RCBType_SEM:  
            presource->RCBCount--;
            pevent->ECBType    = G_ECBType_SEM_GRANTED;
            peventime->ECBType = G_ECBType_TIMEOUT_SEM_GRANTED;
            pevent->ECBValue.i64 = (INT64) prrds->RRDSGrantedPriority.i64;
            if (prrds->RRDSGrantedTimeout.i64 != (INT64) 0) {    
                peventime->ECBValue.i64 = (INT64) GRTOS_now + (INT64) prrds->RRDSGrantedTimeout.i64;
            } else {
                peventime->ECBValue.i64 = (INT64) G_LATEST_TIME - (INT64) 100;
            }
            break;
		default:
			G_DEBUG_WHILEFOREVER;
			break;
	}        
    // Link the events    
    gk_RCBGEL_Link((G_RCB *) presource, pevent);     
    gk_ECBTL_Link(peventime);    
    return(G_TRUE);
}

/**gk_TASK_RESOURCE_UNGRANT
 *  \brief 
 *  Unlinks the granted and timed events of a granted task
 *  \param [in] presource Pointer to the RCB of the semaphore
 *  \param [in] pevent    Pointer to the ECB of the event
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Eliminate replacing the code
 *  \relates Semaphore
 */
INT32 gk_TASK_RESOURCE_UNGRANT(G_RCB *presource, GS_ECB *pevent) 
{
    GS_ECB * peventime;
    // GS_RRDS *prrds;
    GS_TCB *ptcb;
    int retorno = G_FALSE;
    
    // Event with ECB_RRDS != 0 is the resource event, otherwise time event
    if (pevent->ECB_RRDS == (struct gs_rrds *) 0) pevent = pevent->ECB_NextECBAEL;
    peventime = pevent->ECB_NextECBAEL;
    // prrds      = (GS_RRDS *) pevent->ECB_RRDS;

    ptcb = pevent->ECB_AssocTCB;

    //Check if task is granted for this resource
    if (pevent->ECB_AssocRCB == (struct g_rcb *) presource) {
        switch (presource->RCBType)
        {
            case GK_RCBType_SEM:  
                presource->RCBCount++;
                break;
            default:
                G_DEBUG_WHILEFOREVER;
            break;
        }   
        gk_RCBGEL_Unlink(pevent);  
        if (peventime->ECBState == GS_ECBState_WAITING_TIME) {        
            gk_ECBTL_Unlink(peventime);
        }
        
        // Check if there is a waiting task
        pevent = presource->RCB_NextRCBWEL; 
        if (pevent != (GS_ECB *) 0)
        {
            gk_TASK_RESOURCE_UNWAIT((G_RCB *) presource, (GS_ECB *) pevent);
            gk_TASK_RESOURCE_GRANT((G_RCB *)presource, (GS_ECB *) pevent);

            // Check if inheriate priority of the task changes
            ptcb = pevent->ECB_AssocTCB;             
            ptcb->TCBInherPriority = gk_TASK_GRANTED_PRIORITY_GET(ptcb); 
            gk_TCBWL_Unlink(ptcb); 
            gk_TASK_PRIORITY_SET(ptcb, G_TCBState_READY); 
            gk_TCBRDYL_Link(ptcb); 
        }            
        retorno = G_TRUE;
    }

    return retorno;
} 



// 
/**gk_TASK_RESOURCE_CREATE
 *  \brief 
 *  Creates the structure to sopport a resource from a task
 *  \param [in] presource          Pointer to the RCB of the semaphore
 *  \param [in] ptcb               Pointer to the TCB of the task
 *  \param [in] waiting_priority   Default waiting priority
 *  \param [in] RCBGrantedPriority Default granted priority
 *  \param [in] RCBWaitingTimeout  Default waiting timeout
 *  \param [in] RCBGrantedTimeout  Default granted timeout
 *  \return Pointer to the ECB of the event of the resource 
 *  \todo Eliminate the code replacing where it is used
 *  \relates Semaphore
 */
GS_ECB *gk_TASK_RESOURCE_CREATE(G_RCB *presource, 
                                GS_TCB  *ptcb, 
                                INT64 waiting_priority,
		                        INT64 RCBGrantedPriority,
			                    INT64 RCBWaitingTimeout,
				                INT64 RCBGrantedTimeout)
{
	GS_RRDS *prrds;
	GS_ECB  *pevent;
	GS_ECB  *peventime;
    
    prrds     = gk_RRDS_GetFree();
    if (prrds == (GS_RRDS *) 0) return (GS_ECB *) 0;

    pevent    = gk_ECB_GetFree();
    if (pevent == (GS_ECB *) 0) {
        gk_RRDSFL_Link(prrds);
        return (GS_ECB *) 0;
    }
    peventime    = gk_ECB_GetFree();
    if (peventime == (GS_ECB *) 0) {
        gk_RRDSFL_Link(prrds);
        gk_ECBFL_Link(pevent);
        return (GS_ECB *) 0;
    }
    
    pevent->ECB_RRDS = (struct gs_rrds *) prrds;         // Set resource request properties
    prrds->RRDSWaitingPriority.i64 = (INT64) waiting_priority;
    prrds->RRDSGrantedPriority.i64 = (INT64) RCBGrantedPriority;
    prrds->RRDSWaitingTimeout.i64  = (INT64) RCBWaitingTimeout;
    prrds->RRDSGrantedTimeout.i64  = (INT64) RCBGrantedTimeout;
    prrds->RRDS_NextSCB = (struct gs_scb *) 0;     

    gk_TCBAEL_Link(peventime,ptcb);                
    gk_TCBAEL_Link(pevent,ptcb); 
    gk_ECBAEL_Link(pevent, peventime); 
    
    return pevent;
}

/**gk_TASK_RESOURCE_DESTROY
 *  \brief 
 *  Destroy the events associated to a task and semaphore and returns them to the free lists
 *  \param [in] presource Pointer to the RCB of the resource
 *  \param [in] pevent    Pointer to the ECB of the resource
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo   Eliminate replacing the code where it is used
 *  \relates Semaphore
 */
INT32 gk_TASK_RESOURCE_DESTROY(GS_ECB *pevent) 
{
    GS_ECB * peventime;
    // GS_RRDS *prrds;
    // GS_TCB *ptcb;
    // int retorno = G_FALSE;
    
    // Event with ECB_RRDS != 0 is the resource event, otherwise time event
    if (pevent->ECB_RRDS == (struct gs_rrds *) 0) pevent = pevent->ECB_NextECBAEL;
    peventime = pevent->ECB_NextECBAEL;

    gk_ECBFL_Link(peventime);         
    gk_ECBFL_Link(pevent); 
    return(G_TRUE);
}



/**gk_timeout_ECB_SEM_wait
 *  \brief 
 *  Time routine for event G_ECBType_TIMEOUT_SEM_WAITING. It is called from the time interrupt ISR
 *  \param [in] peventime Pointer to the timed event
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Replace gk_TASK_RESOURCE_UNWAIT
 *  \relates Semaphore
 */
INT32 gk_timeout_ECB_SEM_wait(GS_ECB *peventime) {

    GS_ECB *pevent = peventime->ECB_NextECBAEL; PRINT_DEBUG_LINE  // Get the associated event linked to resource
    GS_TCB *ptcb = peventime->ECB_AssocTCB;

    gk_TASK_RESOURCE_UNWAIT((G_RCB *) pevent->ECB_AssocRCB, (GS_ECB *) pevent);
    
    printf("\n\n\n ################## WAITING TIMEOUT ###########\n\n\n");
    gk_TCB_Unlink(ptcb); PRINT_DEBUG_LINE
    gk_TCBRDYL_Link(ptcb); PRINT_DEBUG_LINE
    return G_TRUE;
}

/**gk_timeout_ECB_SEM_post
 *  \brief 
 *  Time routine for event G_ECBType_TIMEOUT_SEM_GRANTED. It is called from the time interrupt ISR
 *  \param [in] peventime Pointer to the timed event
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Eliminate the TASK_RESOURCE routines
 *  \relates Semaphore
 */
INT32 gk_timeout_ECB_SEM_post(GS_ECB *peventime) {
    GS_ECB *pevent  = peventime->ECB_NextECBAEL;
    GS_TCB *ptcb = peventime->ECB_AssocTCB;
    GS_SCB *psignal = gk_ECBASL_GetSCB(peventime, G_ECBType_TIMEOUT_SEM_GRANTED);
    
    /* It is the timeout of a waiting event *************************************/
    printf("\n\n\n ################## GRANTING TIMEOUT ###########\n\n\n");
    
    /* It is the timeout of a waiting event *************************************/
    if (psignal != (struct gs_scb *) 0)
    {
        gk_ECBASL_Unlink(peventime, psignal); PRINT_DEBUG_LINE
        gk_TCBPSL_Link(ptcb, psignal); PRINT_DEBUG_LINE
    }
     
    gk_TASK_RESOURCE_UNGRANT((G_RCB *) pevent->ECB_AssocRCB, (GS_ECB *) pevent);
    gk_TASK_RESOURCE_DESTROY((GS_ECB *) pevent); 

    gk_TCB_Unlink(ptcb); PRINT_DEBUG_LINE
    if (gk_TASK_IS_BLOCKED(ptcb) == G_FALSE)
    {
        gk_TCBRDYL_Link(ptcb); PRINT_DEBUG_LINE
    }
    else
    {
        gk_TCBWL_Link(ptcb, G_TCBState_WAITING_COMPLETED); PRINT_DEBUG_LINE
    }
    return(G_TRUE);
}

/**gk_SEM_granted_kill
 *  \brief 
 *  Removes the ECBs from a the granted list of a semaphore resource.
 *  \param [in] pevent1 Task ECB associated to the semaphore resource
 *  \return G_TRUE when successful, G_FALSE otherwise
  *  \details 
 *  The ECBs has to be type G_ECBType_SEM_GRANTED or G_ECBType_TIMEOUT_SEM_GRANTED. 
 *  This function is called from gk_TCB_List_Unlink
 *  \relates Semaphore
 */
INT32 gk_SEM_granted_kill(GS_ECB *pevent1) {
    int granted;
    granted = gk_TASK_RESOURCE_UNGRANT((G_RCB *) pevent1->ECB_AssocRCB, (GS_ECB *) pevent1);
    if (granted == G_TRUE) gk_TASK_RESOURCE_DESTROY((GS_ECB *) pevent1);
    return G_TRUE;
}

/**gk_SEM_waiting_kill
 *  \brief 
 *  Removes the ECBs from a the waiting list of a semaphore resource.
 *  \param [in] pevent1 Task ECB associated to the semaphore resource
 *  \return G_TRUE when successful, G_FALSE otherwise
  *  \details 
 *  The ECBs has to be type G_ECBType_SEM_WAITING or G_ECBType_TIMEOUT_SEM_WAITING
 *  This function is called from gk_TCB_List_Unlink
 *  \relates Semaphore
 */
INT32 gk_SEM_waiting_kill(GS_ECB *pevent1) {
    gk_TASK_RESOURCE_UNWAIT((G_RCB *) pevent1->ECB_AssocRCB, (GS_ECB *) pevent1);
    gk_TASK_RESOURCE_DESTROY((GS_ECB *) pevent1); 
    return G_TRUE;
}

OPTIMEZE_RESTORE