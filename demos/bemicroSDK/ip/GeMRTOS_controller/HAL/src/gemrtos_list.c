/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS structure list management functions
 *  \details This file contains thefuntion for list management.
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


#include <gemrtos.h>

/** \brief Maximum list-traversal iterations before asserting an infinite loop.
 *  Used in gk_ECBAEL_Remove and any other bounded traversal loops as a
 *  safety guard against corrupt linked-list pointers. */
#define G_LIST_TRAVERSAL_MAX_ITERATIONS  1000U

/* Forward declarations for the generic NODE engine (defined in Section 7,
 * below the per-type sections that call it). Used only within this file â€”
 * see Section 7 for the full Doxygen documentation of each. */
static G_UINT32 gk_NODE_Link(void **root, void * const node, enum list_type list_name);
static G_UINT32 gk_NODE_DUAL_Unlink(void **root, void * const node, G_UINT32 const offset_next, G_UINT32 const offset_prev);
static G_UINT32 gk_NODE_SINGLE_Unlink(void **root, void *node, G_UINT32 const offset_next);

OPTIMIZE_CODE

/***********************************************************************************
 * *********************************************************************************
 *                    GeMRTOS SUPPORT FUNCTIONS
 * *********************************************************************************
 ***********************************************************************************/

/* ===========================================================================
 * SECTION 1 â€” ECB: Event Control Block list management
 * Association list (ECBAEL), signal list (ECBASL), free list (ECBFL),
 * time list (ECBTL), and full unlink helper (ECB_List_Unlink).
 * Functions: gk_ECBAEL_Link, gk_ECBAEL_Remove, gk_ECBASL_Link,
 *            gk_ECBASL_Unlink, gk_ECB_List_Unlink, gk_ECBFL_Link,
 *            gk_ECBTL_Link, gk_ECBTL_Unlink
 * =========================================================================== */
/**gk_ECBAEL_Link
 *  \brief
 *  Links pevent_b to the Associated list of pevent_a
 *  \param [in] pevent_a pointer to the ECB of event a
 *  \param [in] pevent_b pointer to the ECB of event b
 *  \return G_TRUE if successful
 */
G_UINT32 gk_ECBAEL_Link(GS_ECB * const pevent_a, GS_ECB * const pevent_b)
{
    GK_SAMPLE_FUNCTION_BEGIN(10066)
	GS_ECB *pevent;

	PRINT_ASSERT((pevent_a->ECB_AssocTCB == pevent_b->ECB_AssocTCB),"ERROR events are not associated to the same TCB, pevent_a->ECB_AssocTCB= %u, pevent_b->ECB_AssocTCB= %u\n", (unsigned int) pevent_a->ECB_AssocTCB, (unsigned int) pevent_b->ECB_AssocTCB);

	if (get_pointed_field(pevent_a, ECB_NextECBAEL) == (struct gs_ecb *) 0) pevent_a->ECB_NextECBAEL = pevent_a;
	if (get_pointed_field(pevent_b, ECB_NextECBAEL) == (struct gs_ecb *) 0) pevent_b->ECB_NextECBAEL = pevent_b;

	pevent = pevent_a->ECB_NextECBAEL;
	pevent_a->ECB_NextECBAEL = pevent_b->ECB_NextECBAEL;
	pevent_b->ECB_NextECBAEL = (struct gs_ecb *) pevent;

    GK_SAMPLE_FUNCTION_END(10066)
	return(G_TRUE);
}
 
/**gk_ECBAEL_Remove
 *  \brief 
 *  Unlinks the pevent from the associated ECB list
 *  \param [in] pevent pointer to ECB of the event
 *  \return G_TRUE if event was linked to a Associated list, G_FALSE otherwise
 */ 
G_UINT32 gk_ECBAEL_Remove(GS_ECB * const pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10029)
    GS_ECB *pevent_prev;
	G_UINT32 iterations = 0;
    G_UINT32 result = G_FALSE;

    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");

	pevent_prev = get_pointed_field(pevent, ECB_NextECBAEL);
    if (pevent_prev != (struct gs_ecb *) 0)
    {
        /* Looks for the previous ECB that points to pevent.
         *
         * The iteration bound is evaluated unconditionally, OUTSIDE
         * PRINT_ASSERT. It used to live inside the assert as `iterations++`,
         * which meant that in a production build (G_DEBUG_WHILEFOREVER_ENABLE
         * == 0) the increment was compiled out along with the check and this
         * loop became unbounded: a corrupt ECB_NextECBAEL chain that never
         * reaches pevent spins here forever, in kernel context, holding the
         * GeMRTOS mutex -- so every other processor stalls behind it and the
         * watchdog cannot recover it either (the controller suspends a
         * processor's watchdog while it owns the mutex). Exactly the failure
         * class C-1 removed from the console path, reached instead through a
         * safety guard that only existed in development builds.
         *
         * The bound now always terminates the loop; the diagnostic stays in
         * PRINT_ASSERT, so development still halts loudly at the point of
         * corruption and production degrades to a reported failure instead of
         * a system-wide hang. */
        while (get_pointed_field(pevent_prev, ECB_NextECBAEL) != (struct gs_ecb *) pevent)
        {
            if (iterations >= G_LIST_TRAVERSAL_MAX_ITERATIONS) {
                PRINT_ASSERT(G_FALSE, "ERROR pevent not found");
                gk_fprintf("[ LIST ERROR ] gk_ECBAEL_Remove: ECB_NextECBAEL chain "
                           "exceeded %u iterations without reaching pevent=%p; "
                           "list is corrupt, aborting traversal\n",
                           (unsigned int) G_LIST_TRAVERSAL_MAX_ITERATIONS,
                           (void *) pevent);
                GK_SAMPLE_FUNCTION_END(10029)
                return (G_FALSE);
            }
            iterations++;

            pevent_prev = (GS_ECB *) get_pointed_field(pevent_prev, ECB_NextECBAEL);
			PRINT_ASSERT((pevent_prev->ECB_AssocTCB == pevent->ECB_AssocTCB),"ERROR events are not associated to the same TCB, pevent_prev->ECB_AssocTCB= %u, pevent->ECB_AssocTCB= %u\n", (unsigned int) pevent_prev->ECB_AssocTCB, (unsigned int) pevent->ECB_AssocTCB);
        }
		
		set_pointed_field(pevent_prev, ECB_NextECBAEL, get_pointed_field(pevent, ECB_NextECBAEL));
		
        if (get_pointed_field(pevent_prev, ECB_NextECBAEL) == pevent_prev)
			set_pointed_field(pevent_prev, ECB_NextECBAEL, (struct gs_ecb *) 0);

        set_pointed_field(pevent, ECB_NextECBAEL, (struct gs_ecb *) 0);
        result = G_TRUE;
    }
    GK_SAMPLE_FUNCTION_END(10029)
	return(result);
}

/**gk_ECBASL_Link
 *  \brief 
 *  Links a SCB to the ECB Associated Signal List
 *  \param [in] pevent  Pointer to the ECB of the event
 *  \param [in] psignal Pointer to the SCB of the signal
 *  \relates Signal
 */ 
G_UINT32 gk_ECBASL_Link(GS_ECB * const pevent, GS_SCB * const psignal)
{
	set_pointed_field(psignal,SCBState, G_SCBState_IN_ECB);

	G_UINT32 result = gk_NODE_Link((void **) &pevent->ECB_NextECBASL,  // root to the ready list
		                           (void *) psignal, ECBASL); 
								   
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR pevent not valid\n");
	PRINT_ASSERT((gkm_SCB_IsValid(pevent->ECB_NextECBASL) == G_TRUE),"ERROR pevent->ECB_NextECBASL not valid\n");
	PRINT_ASSERT((pevent->ECB_NextECBASL->SCB_AssocXCB == (void *) pevent),"ERROR pevent->ECB_NextECBASL->SCB_AssocXCB not valid\n");

	return (result);
}


/**
 * \brief Unlink a signal SCB from any SCB-chained list using the shared SCB_NextSCB offset.
 *
 * All xASL_Unlink and xPSL_Unlink functions traverse their list through the same
 * next-pointer field (\c SCB_NextSCB at \c offsetof(GS_SCB, SCB_NextSCB)).
 * This helper encapsulates the \c gk_NODE_SINGLE_Unlink call together with the
 * mandatory state transition to \c G_SCBState_UNLINKED on success.
 *
 * \param [in]     pscb   Pointer to the signal SCB to remove.
 * \param [in,out] proot  Address of the list-head pointer (updated by gk_NODE_SINGLE_Unlink).
 * \return #G_TRUE if the node was found and unlinked; #G_FALSE otherwise.
 */
static G_UINT32 gk_SCB_Unlink_from_list(GS_SCB *pscb, void **proot)
{
	G_UINT32 result = gk_NODE_SINGLE_Unlink(proot,
	                                         (void *) pscb,
	                                         (G_UINT32) offsetof(GS_SCB, SCB_NextSCB));
	if (result == G_TRUE) {
		pscb->SCBState = G_SCBState_UNLINKED;
	}
	return result;
}

/**gk_ECBASL_Unlink
 *  \brief
 *  Unlinks a signal SCB from the ECB Associated Signal List of and event
 *  \param [in] pevent  Pointer to the ECB of the event
 *  \param [in] psignal Pointer to the SCB of the signal
 *  \return G_TRUE if successful, G_FALSE if signal is not associated with event
 */
G_UINT32  gk_ECBASL_Unlink(GS_ECB *pevent, GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10067)
	G_UINT32 result = G_FALSE;

	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR pevent not valid\n");
	PRINT_ASSERT((gkm_SCB_IsValid(pevent->ECB_NextECBASL) == G_TRUE),"ERROR psignal not valid\n");
	PRINT_ASSERT((gkm_SCB_IsValid(psignal) == G_TRUE),"ERROR psignal not valid\n");

	result = gk_SCB_Unlink_from_list(psignal, (void **) &pevent->ECB_NextECBASL);

    GK_SAMPLE_FUNCTION_END(10067)
    return(result);
}


/**gk_ECB_List_Unlink
 *  \brief 
 *  Unlinks all the structures linked to a ECB
 *  \param [in] pevent Pointer to the ECB
 *  \return G_TRUE
 *  \todo Verify when a resource is unlinked (semaphore and queue message)
 */ 
G_UINT32 gk_ECB_List_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10030)
	GS_SCB *psignal_next;
	
	// Remove ECB from waiting list ECB_NextECB and ECB_PrevECB pointed in timed or resource list
	// according the pevent->ECBState field
	
    switch (get_pointed_field(pevent, ECBState))
    {
        case GS_ECBState_WAITING_TIME:
            gk_ECBTL_Unlink(pevent);
            break;
        case GS_ECBState_GRANTED_RESOURCE:    
            gk_RCBGEL_Unlink(pevent);            
            break;
        case GS_ECBState_WAITING_RESOURCE:  
            gk_RCBWEL_Unlink(pevent);
            break;
        case GS_ECBState_UNLINKED:
            break;
        default:
            G_DEBUG_WHILEFOREVER;
            break;
    }

	pevent->ECB_AssocRCB = (GS_RCB *) 0;

    // Remove from the task list
	if (pevent->ECB_AssocTCB != (GS_TCB *)0) gk_TCBAEL_Unlink(pevent);
    
    // Remove from the ECB Associated list
	gk_ECBAEL_Remove(pevent);
	
	// Remove SCB from ASL
	while (get_pointed_field(pevent, ECB_NextECBASL) != (struct gs_scb *) 0)
	{
		psignal_next = get_pointed_field(pevent, ECB_NextECBASL);
		gk_ECBASL_Unlink(pevent, psignal_next);
		gk_SCBFL_Link(psignal_next);
	}	

    // Remove RRDS
    struct gs_rrds *prrds;
    struct gs_rrds *prrds_next;
    prrds = (struct gs_rrds *) get_pointed_field(pevent, ECB_RRDS);
    while (prrds != (struct gs_rrds *) 0) {
        prrds_next = (struct gs_rrds *) get_pointed_field(prrds, RRDS_NextRRDS);
        gk_RRDSFL_Link(prrds);
        prrds = prrds_next;
    }
    set_pointed_field(pevent, ECB_RRDS, (struct gs_rrds *) 0);
    
    GK_SAMPLE_FUNCTION_END(10030)
	
    return(G_TRUE);
}

/**gk_ECBFL_Link
 *  \brief 
 *  Link ECB to Free List. Removes the signals from ECB
 *  \param [in] pevent Pointer to the ECB to link
 *  \return G_TRUE
 */ 
G_UINT32 gk_ECBFL_Link(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10031)
    
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");
	
    // Unlink the ECB
    gk_ECB_List_Unlink(pevent);	
	
    PRINT_ASSERT((pevent->ECBState == GS_ECBState_UNLINKED),"ERROR ECBState= %d\n", (int) pevent->ECBState);
	PRINT_ASSERT((pevent->ECB_AssocRCB == (GS_RCB *) 0),"ERROR pevent->ECB_AssocRCB= %p\n", pevent->ECB_AssocRCB);
	PRINT_ASSERT((pevent->ECB_AssocTCB == (GS_TCB *) 0),"ERROR pevent->ECB_AssocTCB= %p\n", pevent->ECB_AssocTCB);
	PRINT_ASSERT((pevent->ECB_NextECBAEL == (GS_ECB *) 0),"ERROR pevent->ECB_NextECBAEL= %p\n", pevent->ECB_NextECBAEL);
	PRINT_ASSERT((pevent->ECB_NextECB == (GS_ECB *) 0),"ERROR pevent->ECB_NextECB= %p\n", pevent->ECB_NextECB);
	PRINT_ASSERT((pevent->ECB_PrevECB == (GS_ECB *) 0),"ERROR pevent->ECB_PrevECB= %p\n", pevent->ECB_PrevECB);

    // Do not free the structure to avoid memory fragmentation
    set_pointed_field(pevent, ECB_NextECB, g_kcb.KCB_FREE_ECBs);
    g_kcb.KCB_FREE_ECBs = pevent;
    
    set_pointed_field(pevent, ECBState, GS_ECBState_FREE);
    
    GK_SAMPLE_FUNCTION_END(10031)
	
	return(G_TRUE);
}


/**gk_ECBTL_Link
 *  \brief 
 *  Links the ECB from the Time Event List
 *  \param [in] pevent Pointer to the ECB of the event
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Check if time is a valid time
 */  
G_UINT32 gk_ECBTL_Link (GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10001)
 
    PRINT_ASSERT((gkm_ECB_IsValid(g_kcb.KCB_NextECBTL) == G_TRUE),"ERROR no event in root time list\n");
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR in pevent= %p\n", pevent);
    PRINT_ASSERT((pevent->ECBState == GS_ECBState_UNLINKED),"ERROR ECBState= %d\n", (int) pevent->ECBState);

	gk_NODE_Link((void **) &g_kcb.KCB_NextECBTL,         // root to the ready list
				 (void *) pevent, ECBTL);                         // ascending order sorting
	
    /* Change Status of the Event */
    set_pointed_field(pevent, ECBState, GS_ECBState_WAITING_TIME);

    PRINT_ASSERT((gkm_ECB_IsValid(g_kcb.KCB_NextECBTL) == G_TRUE),"ERROR in time list with no event\n");
	
    GK_SAMPLE_FUNCTION_END(10001)
    return(G_TRUE);
}

/**gk_ECBTL_Unlink
 *  \brief 
 *  Unlinks the ECB from the Time Event List
 *  When starts:
 *     pevent->ECBState = GS_ECBState_WAITING_TIME
 *  When finishes:
 *     pevent->ECBState = GS_ECBState_UNLINKED
 *  \param [in] pevent Pointer to the ECB of the event to unlink
 *  \return G_TRUE if successful, G_FALSE if was not a time event
 *  \todo Implement G_FALSE when no a time event
 */
G_UINT32 gk_ECBTL_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10019)
    
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");
    PRINT_ASSERT((pevent->ECBState == GS_ECBState_WAITING_TIME),"ERROR ECBState= %d\n", (int) pevent->ECBState);    
	
	gk_NODE_DUAL_Unlink((void **) &g_kcb.KCB_NextECBTL,            // root to the ready list
	                    (void *)  pevent,                          // node to unlink
						(G_UINT32) offsetof(GS_ECB,ECB_NextECB),    // next field offset
						(G_UINT32) offsetof(GS_ECB,ECB_PrevECB));   // previous field offset
						
	pevent->ECBState = GS_ECBState_UNLINKED;

    PRINT_ASSERT((gkm_ECB_IsValid(g_kcb.KCB_NextECBTL) == G_TRUE),"ERROR in root time list\n");
	PRINT_ASSERT((g_kcb.KCB_NextECBTL != (GS_ECB *) 0),"ERROR in root time list\n");
	PRINT_ASSERT((g_kcb.KCB_NextECBTL->ECB_PrevECB == (GS_ECB *) 0),"ERROR in root time list\n");
	
	PRINT_ASSERT((pevent->ECB_NextECB == (GS_ECB *) 0),"ERROR in root time list\n");
	PRINT_ASSERT((pevent->ECB_PrevECB == (GS_ECB *) 0),"ERROR in root time list\n");
	
	// next
	
    GK_SAMPLE_FUNCTION_END(10019)
    return(G_TRUE);
}

/* ===========================================================================
 * SECTION 2 â€” KCB: Kernel Control Block signal lists
 * Associated signal list (KCBASL) and pending signal list (KCBPSL).
 * Functions: gk_KCBASL_Link, gk_KCBASL_Unlink,
 *            gk_KCBPSL_Link, gk_KCBPSL_Unlink
 * =========================================================================== */
/**gk_KCBASL_Link
 *  \brief
 *  Links a SCG to the Associated Signal list of KCB
 *  \param [in] psignal Pointer to the SCB of the signal to link
 *  \return G_TRUE if successful
 *  \relates Signal
 */
G_UINT32  gk_KCBASL_Link(GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10091)
    set_pointed_field(psignal, SCBState, G_SCBState_IN_KCB);
    G_UINT32 result = gk_NODE_Link((void **) &g_kcb.KCB_NextKCBASL, (void *) psignal, KCBASL);
    GK_SAMPLE_FUNCTION_END(10091)
    return result;
}


/**gk_KCBASL_Unlink
 *  \brief 
 *  Unlinks a SCG from the Associated Signal list of KCB
 *  \param [in] psignal Pointer to the SCB of the signal to unlink
 *  \return G_TRUE if successful, G_FALSE if not such signal linked to KCBASL
 *  \relates Signal
 */ 
G_UINT32  gk_KCBASL_Unlink(GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10068)
    G_UINT32 result = gk_SCB_Unlink_from_list(psignal, (void **) &g_kcb.KCB_NextKCBASL);
    GK_SAMPLE_FUNCTION_END(10068)
    return(result);
}

/**gk_KCBPSL_Link
 *  \brief 
 *  Link a SCB to KCB Pending Signal List
 *  \param [in] psignal Pointer to the SCB
 *  \return G_TRUE
 *  \relates Task 
 *  \sa Signal
 */ 
G_UINT32  gk_KCBPSL_Link(GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10092)
    set_pointed_field(psignal, SCBState, G_SCBState_PENDING);
    G_UINT32 result = gk_NODE_Link((void **) &g_kcb.KCB_NextKCBPSL, (void *) psignal, KCBPSL);
    GK_SAMPLE_FUNCTION_END(10092)
    return result;
}


/**gk_TCBPSL_Unlink
 *  \brief 
 *  Unlinks SCB from KCB Pending Signal List
 *  \param [in] psignal Pointer to the SCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Return G_FALSE when it is not associated SCB to TCB
 *  \relates Task
 *  \sa Signal
 */
G_UINT32  gk_KCBPSL_Unlink(GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10032)
    PRINT_ASSERT((psignal->SCBState != G_SCBState_UNLINKED),"ERROR SCBState= %d\n",(int) psignal->SCBState);
    G_UINT32 result = gk_SCB_Unlink_from_list(psignal, (void **) &g_kcb.KCB_NextKCBPSL);
    PRINT_ASSERT((psignal->SCBState == G_SCBState_UNLINKED),"ERROR SCBState= %d\n",(int) psignal->SCBState);
    GK_SAMPLE_FUNCTION_END(10032)
    return (result);
}


/* ===========================================================================
 * SECTION 3 â€” LCB: Scheduling List Control Block management
 * LCB run list (LCBL) and free-processor list (LCBFPL).
 * Functions: gk_LCBL_Link, gk_LCBL_Unlink,
 *            gk_LCBFPL_Link, gk_LCBFPL_Unlink
 * =========================================================================== */
/**gk_LCBL_Link
 *  \brief
 *  Inserts a RunList in LCB Runining list ordered by background list and then priority
 *  \param [in] plcb Pointer to the LCB list
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Check everything about this function
 *  \todo Define the G_FALSE return
 *  \relates Scheduling_List
 */
G_UINT32 gk_LCBL_Link(GS_LCB *plcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10043)
	GS_LCB *plcb1 = g_kcb.KCB_NextLCBL;

    PRINT_ASSERT((plcb->LCBState == GS_LCBState_UNLINKED), "ERROR LCB is not unlinked= %d\n", (int) plcb->LCBState);
    PRINT_ASSERT((gkm_LCB_IsValid(plcb) == G_TRUE),"ERROR PCB not valid\n");

    set_pointed_field(plcb, LCB_PrevLCBL, (struct gs_lcb *) 0);
    set_pointed_field(plcb, LCB_NextLCBL, (struct gs_lcb *) plcb1);
	if (plcb1 != (struct gs_lcb *) 0) set_pointed_field(plcb1, LCB_PrevLCBL, plcb);
    g_kcb.KCB_NextLCBL = plcb;
    
	set_pointed_field(plcb, LCBState, GS_LCBState_LINKED);
	
    GK_SAMPLE_FUNCTION_END(10043)
    return(G_TRUE);
}
 
 
/**gk_LCBL_Unlink
 *  \brief 
 *  Unlinks the LCB from the LCB List
 *  \param [in] plcb Pointer to the LCB to unlink
 *  \return G_TRUE if successful, G_FALSE otherwise 
 *  \todo Implement G_FALSE returns
 *  \relates Scheduling_List
 */ 
G_UINT32 gk_LCBL_Unlink(GS_LCB *plcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10069) 

    PRINT_ASSERT((gkm_LCB_IsValid(plcb) == G_TRUE),"ERROR LCB not valid\n");
    PRINT_ASSERT(((gkm_LCB_IsValid(plcb->LCB_NextLCBL) == G_TRUE) || (plcb->LCB_NextLCBL == (struct gs_lcb *) 0)),"ERROR LCB not valid\n");
    PRINT_ASSERT(((gkm_LCB_IsValid(plcb->LCB_PrevLCBL) == G_TRUE) || (plcb->LCB_PrevLCBL == (struct gs_lcb *) 0)),"ERROR LCB not valid\n");
    PRINT_ASSERT((gkm_LCB_IsValid(g_kcb.KCB_NextLCBL) == G_TRUE),"ERROR LCB not valid\n");
    PRINT_ASSERT((plcb->LCBState == GS_LCBState_LINKED), "ERROR LCB is not unlinked= %d\n", (int) plcb->LCBState);

	gk_NODE_DUAL_Unlink((void **) &g_kcb.KCB_NextLCBL,              // root to the ready list
	                    (void *)  plcb,                             // node to unlink
						(G_UINT32) offsetof(GS_LCB,LCB_NextLCBL),    // next field offset
						(G_UINT32) offsetof(GS_LCB,LCB_PrevLCBL));   // previous field offset
	
    plcb->LCBState = GS_LCBState_UNLINKED;
    
    GK_SAMPLE_FUNCTION_END(10069) 
    return(G_TRUE);
}
 
/**gk_LCBFPL_Link
 *  \brief 
 *  Links a PCB to the LCB Free Processor List of foreground list of the processor
 *  \param [in] processorID Processor ID
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \details When a processor is free, it is linked to the foreground LCB free processor list
 *  \todo Check if index is correct and return G_FASEL if not.
 *  \relates Processor
 *  \sa Scheduling_List
 */
G_UINT32 gk_LCBFPL_Link(G_UINT32 processorID)
{
    GK_SAMPLE_FUNCTION_BEGIN(10007)

	GS_PCB *ppcb = (GS_PCB *) &(g_kcb.G_PCBTbl[processorID-1]);
	GS_PCBAssocLCB *pcb_assoclcb = get_pointed_field( ppcb , PCB_AssocLCB);
    GS_LCB *plcb = get_pointed_field(pcb_assoclcb, PCB_RDY_LCBL);

    PRINT_ASSERT((processorID >= 1 && processorID <= G_NUMBER_OF_PCB),"ERROR processorID= %d\n", (int) processorID);
    PRINT_ASSERT((ppcb->PCBState != GS_PCBState_FREE),"ERROR ppcb->PCBState= %d\n", (int) ppcb->PCBState);
    PRINT_ASSERT((ppcb->PCB_NextPCB == (struct gs_pcb *) 0), "ERROR ppcb->PCB_NextPCB\n");
    PRINT_ASSERT((ppcb->PCB_PrevPCB == (struct gs_pcb *) 0), "ERROR ppcb->PCB_PrevPCB\n");

    if (get_pointed_field(plcb, LCB_NextLCBFPL) != (struct gs_pcb *) 0) {
        set_pointed_field(get_pointed_field(plcb, LCB_NextLCBFPL), PCB_PrevPCB, ppcb);
    }

    set_pointed_field(ppcb, PCB_PrevPCB, (struct gs_pcb *) 0);
    set_pointed_field(ppcb, PCB_NextPCB, get_pointed_field(plcb, LCB_NextLCBFPL));
    set_pointed_field(plcb, LCB_NextLCBFPL, (struct gs_pcb *) ppcb);

    ppcb->PCBState = GS_PCBState_FREE;

    GK_SAMPLE_FUNCTION_END(10007)
    return(G_TRUE);
} 

/**gk_LCBFPL_Unlink
 *  \brief 
 *  Unlinks a PCB from the LCB Free Processor List
 *  \param [in] processorID Processor ID
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Check if Processor ID is valid and return G_FALSE
 *  \relates Processor
 *  \sa Scheduling_List
 */
G_UINT32 gk_LCBFPL_Unlink(G_UINT32 processorID)
{
    GK_SAMPLE_FUNCTION_BEGIN(10008)
	
	GS_PCB *ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[processorID-1];
	GS_PCBAssocLCB *pcb_assoclcb = get_pointed_field( ppcb , PCB_AssocLCB);
    GS_LCB *plcb = get_pointed_field(pcb_assoclcb, PCB_RDY_LCBL);  // This is the foreground list

    PRINT_ASSERT((processorID >= 1 && processorID <= G_NUMBER_OF_PCB),"ERROR processorID= %d\n", (int) processorID);
    PRINT_ASSERT((g_kcb.G_PCBTbl[processorID-1].PCBState == GS_PCBState_FREE),"ERROR ppcb->PCBState= %d\n", (int) g_kcb.G_PCBTbl[processorID-1].PCBState);
	
	gk_NODE_DUAL_Unlink((void **) &plcb->LCB_NextLCBFPL,              // root to the ready list
	                    (void *)  ppcb,                              // node to unlink
						(G_UINT32) offsetof(GS_PCB,PCB_NextPCB),      // next field offset
						(G_UINT32) offsetof(GS_PCB,PCB_PrevPCB));     // previous field offset	
						
	set_pointed_field(ppcb, PCBState, GS_PCBState_RUNNING);
    
    GK_SAMPLE_FUNCTION_END(10008)
    return(G_TRUE);
}

/* ===========================================================================
 * SECTION 4 â€” RCB: Resource Control Block list management
 * Associated signal list (RCBASL), free list (RCBFL), grant event list
 * (RCBGEL), wait event list (RCBWEL), and task-release helper (TASK_RELEASE).
 * Functions: gk_RCBASL_Link, gk_RCBASL_Unlink, gk_RCBFL_Link,
 *            gk_TASK_RELEASE, gk_RCBGEL_Link, gk_RCBGEL_Unlink,
 *            gk_RCBWEL_Link, gk_RCBWEL_Unlink
 * =========================================================================== */
/**gk_RCBASL_Link
 *  \brief
 *  Links a signal SCB to the resource RCB Associated Signal List
 *  \param [in] presource Pointer to the RCB of the resource
 *  \param [in] psignal   Pointer to the SCB of the signal
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo
 */
G_UINT32 gk_RCBASL_Link(GS_RCB *presource, GS_SCB *psignal)
{
	set_pointed_field(psignal,SCBState, G_SCBState_IN_RCB);

	return	(gk_NODE_Link((void **) &presource->RCB_NextRCBASL,  // root to the ready list
		                 (void *) psignal, RCBASL));             // ascending order sorting
}


/**gk_RCBASL_Unlink
 *  \brief 
 *  Unlinks a SCB of a signal from the RCB Associated Signal List of a resource
 *  \param [in] presource Pointer to the RCB of the resource
 *  \param [in] psignal   Pointer to the SCB of the signal
 *  \return G_TRUE is successfull, G_FALSE otherwise 
 *  \todo Return G_FALSE when it is not linked
 */ 
G_UINT32 gk_RCBASL_Unlink(GS_RCB *presource, GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10070)
    PRINT_ASSERT((psignal->SCBState != G_SCBState_UNLINKED),"ERROR SCBState= %d\n", (int) psignal->SCBState);
    G_UINT32 result = gk_SCB_Unlink_from_list(psignal, (void **) &presource->RCB_NextRCBASL);
    PRINT_ASSERT((psignal->SCBState == G_SCBState_UNLINKED),"ERROR SCBState= %d\n", (int) psignal->SCBState);
    GK_SAMPLE_FUNCTION_END(10070)
    return(result);
}

/**gk_RCBFL_Link
 *  \brief 
 *  Links the RCB of a resource to Free List
 *  \param [in] presource Pointer to the RCB of the resource
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Return G_FALSE when it is not unused
 */
G_UINT32 gk_RCBFL_Link(GS_RCB *presource)
{
    GK_SAMPLE_FUNCTION_BEGIN(10071)
    
    PRINT_ASSERT((presource->RCBType == GS_RCBType_FREE),"ERROR RCBType= %d\n", (int) presource->RCBType);

    // RCBs linked list for debugging.
    // P047: only unlink RCBs that are actually tracked in KCB_ROOT_RCBs (the
    // dynamic-overflow registry).  Statically pooled RCBs â€” the normal case â€”
    // are never added to it, so an unconditional unlink would dereference a
    // NULL RCB_PREV_RCBs.  An RCB is in the registry iff it is the root head or
    // has a non-NULL prev/next link.
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    if ((g_kcb.KCB_ROOT_RCBs == (struct g_rcb *) presource) ||
        (presource->RCB_PREV_RCBs != (struct g_rcb *) 0) ||
        (presource->RCB_NEXT_RCBs != (struct g_rcb *) 0)) {
        if (g_kcb.KCB_ROOT_RCBs != (struct g_rcb *) presource) presource->RCB_PREV_RCBs->RCB_NEXT_RCBs = presource->RCB_NEXT_RCBs;
        else g_kcb.KCB_ROOT_RCBs = (struct g_rcb *) presource->RCB_NEXT_RCBs;
        if (presource->RCB_NEXT_RCBs != (GS_RCB *) 0) presource->RCB_NEXT_RCBs->RCB_PREV_RCBs = presource->RCB_PREV_RCBs;
    }
#endif

    // Does not free the structure to avoid fragmentation
    presource->RCB_NextRCB = g_kcb.KCB_FREE_RCBs;
    g_kcb.KCB_FREE_RCBs = (struct g_rcb *) presource;    
    
    GK_SAMPLE_FUNCTION_END(10071)
    return(G_TRUE);
}


/**gk_TASK_RELEASE
 *  \brief 
 *  Release a new invocation of the task. Preserves the Associated signal list.
 *  If task is not completed, then the Abort signal is triggered
 *  \param [in] ptcb Pointer to the TCB of the task to release
 *  \return G_TRUE if successful 
 *  \todo Check Ready and Running state to trigger the abort signal
 *  \relates Task
 */
G_UINT32 gk_TASK_RELEASE(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10039)

    // Preserve the associated signal list
    GS_SCB *psignal_asl = get_pointed_field(ptcb, TCB_NextTCBASL);
    set_pointed_field(ptcb, TCB_NextTCBASL, (struct gs_scb *) 0);

    // Preserve the current state of the task
    G_UINT32 state = get_pointed_field(ptcb, TCBState);


    gk_TCB_Unlink(ptcb);                                // Unlink task TCB
    gk_TCB_List_Unlink(ptcb);                           // Remove all the lists from the TCB
    gk_TASK_STK_INIT(ptcb);                             // Initiates the stack of the task
    set_pointed_field(ptcb, TCB_MTX_NESTED, 0);

    // Put back the associated signal list
    set_pointed_field(ptcb, TCB_NextTCBASL, (struct gs_scb *) psignal_asl);

    // Check if there is G_SCBType_TCB_ABORTED signal
    if ((state == G_TCBState_WAITING) || (state == G_TCBState_RUNNING) || (state == G_TCBState_READY)) {
        GS_SCB *psignal_abort = gk_TCBASL_GetSCB(ptcb, G_SCBType_TCB_ABORTED);
        if (psignal_abort != (GS_SCB *) 0) {
            psignal_abort = gk_SCB_Copy((GS_SCB *) psignal_abort);
            gk_TCBPSL_Link(ptcb, (GS_SCB *) psignal_abort);
        }
    }
    GK_SAMPLE_FUNCTION_END(10039)
    return(G_TRUE);
}

/** gk_RCBGEL_Link
 *  \brief 
 *  Links ECB to RCB granted list and return pointer to the ECB linked
 *  \param [in] presource Pointer to the RCB of the resource
 *  \param [in] ptcb Pointer to the TCB of the task
 *  \param [in] pevent    Pointer to the ECB of the event. If NULL the a free ECB is chosen
 *  \return Pointer to ECB, NULL if no ECB is available
 *  \todo Check priority for different structures
 */
GS_ECB *gk_RCBGEL_Link(GS_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent)
{
	
    GK_SAMPLE_FUNCTION_BEGIN(10025)

	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ECB not valid\n");
	PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid\n");
	
	if (pevent == (struct gs_ecb *) 0) pevent = gk_ECB_GetFree(ptcb);

	// Insert the event in the Granted list of the Resource
	gk_NODE_Link((void **) &presource->RCB_NextRCBGEL,         // root to the ready list
				 (void *) pevent, RCBGEL);                     // ascending order sorting	
	
	// Associate resource and set the state of the event linked
	set_pointed_field(pevent, ECB_AssocRCB, (struct g_rcb *) presource);
	set_pointed_field(pevent, ECBState, GS_ECBState_GRANTED_RESOURCE);
	
    GK_SAMPLE_FUNCTION_END(10025)
	
    return(pevent);
}

/**
 *  \brief  Unlink pevent from the resource's granted-event list.
 *  \param [in] pevent  Pointer to the ECB to remove from the granted list.
 *                      Must be in GS_ECBState_GRANTED_RESOURCE state.
 *  \return void
 */
void  gk_RCBGEL_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10020)

	GS_RCB *presource = (GS_RCB *) get_pointed_field(pevent, ECB_AssocRCB);
	PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid\n");

    PRINT_ASSERT((pevent->ECBState == GS_ECBState_GRANTED_RESOURCE),"ERROR ECBState= %d \n",(int) pevent->ECBState);
    PRINT_ASSERT((gkm_ECB_IsValid(presource->RCB_NextRCBGEL) == G_TRUE),"ERROR RCB_NextRCBGEL not valid\n");

	// Unlink from the resource granted list
	gk_NODE_DUAL_Unlink((void **) &presource->RCB_NextRCBGEL,      // root to the ready list
	                    (void *)  pevent,                          // node to unlink
						(G_UINT32) offsetof(GS_ECB,ECB_NextECB),    // next field offset
						(G_UINT32) offsetof(GS_ECB,ECB_PrevECB));   // previous field offset
	pevent->ECBState = GS_ECBState_UNLINKED;

    GK_SAMPLE_FUNCTION_END(10020)
}


/**gk_RCBWEL_Link
 *  \brief 
 *  Links an event ECB to the resource waiting list of RCB. If no ECB is given then a new ECB is obtained
 *  and the priority of the resource is given as the priority of the event
 *  \param [in] presource Pointer to the RCB of the resource
 *  \param [in] ptcb Pointer to the TCB of the task
 *  \param [in] pevent    Pointer to the ECB of the event, NULL if an event should be return
 *  \return Return a pointer to the ECB of the event
 *  \todo Considered when no free ECB are available
 */
GS_ECB *gk_RCBWEL_Link(GS_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10079)

	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ECB not valid\n");
	PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid\n");

	// Get event if it is NULL
	if (pevent == (struct gs_ecb *) 0)	pevent = gk_ECB_GetFree(ptcb);

	// Set the priority of the event
	set_pointed_field(pevent, ECBValue.i64, ptcb->TCBCurrentPriority);
	
	gk_NODE_Link((void **) &presource->RCB_NextRCBWEL,   // root to the ready list
				 (void *) pevent, RCBWEL);               // ascending order sorting		
	
	pevent->ECBState = GS_ECBState_WAITING_RESOURCE;
	pevent->ECB_AssocRCB = (struct g_rcb *) presource;
    GK_SAMPLE_FUNCTION_END(10079)
    
    return(pevent);
}

/**
 *  \brief  Unlink pevent from the resource's waiting-event list.
 *  \param [in] pevent  Pointer to the ECB to remove from the waiting list.
 *                      Must NOT be in GS_ECBState_UNLINKED state when called.
 *  \details
 *  Pre-condition:  pevent->ECBState != GS_ECBState_UNLINKED \n
 *  Post-condition: pevent->ECBState == GS_ECBState_UNLINKED
 *  \return void
 */
void gk_RCBWEL_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10021)
	
	GS_RCB *presource = (GS_RCB *) get_pointed_field(pevent, ECB_AssocRCB);
	PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB not valid\n");	
	
    PRINT_ASSERT((pevent->ECBState != GS_ECBState_UNLINKED),"ERROR ECBState= %d\n", (int) pevent->ECBState);

	gk_NODE_DUAL_Unlink((void **) &presource->RCB_NextRCBWEL,      // root to the ready list
	                    (void *)  pevent,                          // node to unlink
						(G_UINT32) offsetof(GS_ECB,ECB_NextECB),    // next field offset
						(G_UINT32) offsetof(GS_ECB,ECB_PrevECB));   // previous field offset
	pevent->ECBState = GS_ECBState_UNLINKED;	
	
    GK_SAMPLE_FUNCTION_END(10021)
}


/* ===========================================================================
 * SECTION 5 â€” SCB: Signal Control Block management
 * Free list (SCBFL), copy helper, and associated pending signal list (SCBAPSL).
 * Functions: gk_SCBFL_Link, gk_SCB_Copy,
 *            gk_SCBAPSL_Link, gk_SCBAPSL_Unlink
 * =========================================================================== */
/**gk_SCBFL_Link
 *  \brief
 *  Links a SCB to the Free List
 *  \param [in] psignal Pointer to the SCB to be linked
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Check is ECB signal is ready for linking to free list without links.
 *  \relates Signal
 */
G_UINT32 gk_SCBFL_Link(GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10034)

    PRINT_ASSERT((psignal->SCBState == G_SCBState_UNLINKED),"ERROR SCBState= %d\n", (int) psignal->SCBState);

    // Does not free the structure to avoid fragmentation
    set_pointed_field(psignal, SCB_NextSCB, g_kcb.KCB_FREE_SCBs);
    g_kcb.KCB_FREE_SCBs = psignal;
    GK_SAMPLE_FUNCTION_END(10034)
    return(G_TRUE);
}


/**gk_SCB_Copy
 *  \brief 
 *  Returns a pointer to a SCB with same values that psignal. Used when an associated signal has to be copied for pending list.
 *  \param [in] psignal Pointer to the SCB to be copied
 *  \return Pointer to the new SCB or NULL if no SCB is available
 *  \relates Signal
 */
GS_SCB *gk_SCB_Copy(GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10015)
    GS_SCB *psignal1 = gk_SCB_GetFree();
    PRINT_ASSERT((psignal1 != (struct gs_scb *) 0), "ERROR gk_SCB_Copy");
    if (psignal1 != (struct gs_scb *) 0){
        set_pointed_field(psignal1, SCBState        , get_pointed_field(psignal, SCBState));
        set_pointed_field(psignal1, SCBType         , get_pointed_field(psignal, SCBType));
        set_pointed_field(psignal1, SCBPriority     , get_pointed_field(psignal, SCBPriority));
        set_pointed_field(psignal1, SCB_TaskCode    , get_pointed_field(psignal, SCB_TaskCode));
        set_pointed_field(psignal1, SCB_TaskArg     , get_pointed_field(psignal, SCB_TaskArg));
        set_pointed_field(psignal1, SCB_NextSCB     , (struct gs_scb *) 0);
        set_pointed_field(psignal1, SCB_AssocXCB    , get_pointed_field(psignal, SCB_AssocXCB));
    }
    GK_SAMPLE_FUNCTION_END(10015)
    return(psignal1);
}

/**
 *  \brief  Prepend pscb_pending to the application-signal list rooted at *pscb_root.
 *
 *  The new SCB is inserted at the head of the list: pscb_pending->SCB_NextSCB is
 *  set to the current (*pscb_root)->SCB_NextSCB, then *pscb_root is updated to
 *  pscb_pending so the caller's root pointer reflects the new head.
 *
 *  \param [in,out] pscb_root    Address of the caller's root pointer.  *pscb_root is
 *                               updated to pscb_pending on return.
 *  \param [in]     pscb_pending Pointer to the SCB to prepend.
 *  \return void
 *  \note Interface asymmetry: this function takes \c GS_SCB** (pointer-to-root-pointer;
 *        root is updated on return). The complementary gk_SCBAPSL_Unlink() takes
 *        \c GS_SCB* (a sentinel node; traversal starts from pscb_root->SCB_NextSCB).
 *        This is intentional â€” Link replaces the root; Unlink uses a sentinel model.
 *  \relates Signal
 */
void gk_SCBAPSL_Link(GS_SCB **pscb_root, GS_SCB *pscb_pending)
{
    GK_SAMPLE_FUNCTION_BEGIN(10072)
	pscb_pending->SCB_NextSCB = (*pscb_root)->SCB_NextSCB; /* dereference to read the old next */
	*pscb_root = pscb_pending;                              /* write through the pointer so the caller's root is updated */
    GK_SAMPLE_FUNCTION_END(10072)
}

/**
 *  \brief  Remove pscb_pending from the application-signal list rooted at pscb_root.
 *
 *  Searches the singly-linked SCB_NextSCB chain starting at pscb_root->SCB_NextSCB
 *  for pscb_pending and splices it out.  On success, sets pscb_pending->SCBState
 *  to G_SCBState_UNLINKED.
 *
 *  \param [in] pscb_root    Head node of the application-signal list.
 *  \param [in] pscb_pending SCB to remove from the list.
 *  \return #G_TRUE if pscb_pending was found and removed;
 *          #G_FALSE if pscb_pending was not present in the list.
 *  \relates Signal
 */
G_UINT32 gk_SCBAPSL_Unlink(GS_SCB *pscb_root, GS_SCB *pscb_pending)
{
    GK_SAMPLE_FUNCTION_BEGIN(10073)

    G_UINT32 result = gk_NODE_SINGLE_Unlink((void **) &pscb_root->SCB_NextSCB, // root to the list
                                             (void *) pscb_pending,             // node to remove
                                             (G_UINT32) offsetof(GS_SCB, SCB_NextSCB));
    if (result == G_TRUE) {
        pscb_pending->SCBState = G_SCBState_UNLINKED;
    }

    GK_SAMPLE_FUNCTION_END(10073)
    return(result);
}

/* ===========================================================================
 * SECTION 6 â€” TCB: Task Control Block list management (part A)
 * Association event list (TCBAEL), associated signal list (TCBASL),
 * free list (TCBFL), pending signal list (TCBPSL), ready list (TCBRDYL),
 * run list link (TCBRUNL_Link), priority update, and full unlink helper.
 * NOTE: gk_TCBRUNL_Unlink, gk_TCBWL_Link/Unlink appear in Section 8
 * (after the NODE engine on which they depend).
 * Functions: gk_TCBAEL_Link, gk_TCBAEL_Unlink, gk_TCBASL_Link,
 *            gk_TCBASL_Unlink, gk_TCB_List_Unlink, gk_TCBFL_Link,
 *            gk_TCBPSL_Link, gk_TCBPSL_Unlink, gk_TCBRDYL_Link,
 *            gk_TCBRDYL_Unlink, gk_TCBRUNL_Link, gk_TCBUpdate_Priority
 * =========================================================================== */
/**gk_TCBAEL_Link
 *  \brief
 *  Links an ECB to the associated ECB list of TCB
 *  \param [in] pevent Pointer to the ECB
 *  \param [in] ptcb   Ponter to the TCB
 *  \return G_TRUE when sucessful, G_FALSE otherwise
 *  \todo Check validity of XCB
 *  \relates Task
 *  \sa Event
 */
G_UINT32 gk_TCBAEL_Link(GS_ECB *pevent, GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10022)
    
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ptcb\n");        

	/* Associate EVENT to the Task            */
    set_pointed_field(pevent, ECB_AssocTCB, (struct gs_tcb *) ptcb); 
	
	/* Associate TASK to EVENT         */
    set_pointed_field(pevent, ECB_NextTCBAEL, get_pointed_field(ptcb, TCB_NextTCBAEL)); 
	
    set_pointed_field(pevent, ECB_PrevTCBAEL, (struct gs_ecb *) 0);

    if (get_pointed_field(ptcb, TCB_NextTCBAEL) != (struct gs_ecb *) 0) {
		PRINT_ASSERT((ptcb->TCB_NextTCBAEL->ECB_AssocTCB == ptcb),"ERROR ptcb\n"); 
		set_pointed_field(get_pointed_field(ptcb, TCB_NextTCBAEL), ECB_PrevTCBAEL,(struct gs_ecb *) pevent);
    }
	
    set_pointed_field(ptcb, TCB_NextTCBAEL, (struct gs_ecb *) pevent);
	
    GK_SAMPLE_FUNCTION_END(10022)
	
    return(G_TRUE);
}

/**gk_TCBAEL_Unlink
 *  \brief 
 *  Unlinks the ECB from the associated TCB
 *  \param [in] pevent Ponter to the ECB
 *  \return G_TRUE if successful, G_FALSE otherwise
 */ 
G_UINT32 gk_TCBAEL_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10018)
	G_UINT32 result = G_FALSE;
    
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR pevent\n");
    PRINT_ASSERT((gkm_TCB_IsValid(pevent->ECB_AssocTCB) == G_TRUE),"ERROR pevent->ECB_AssocTCB\n");

    if (get_pointed_field(pevent, ECB_AssocTCB) != (GS_TCB *)0)
    {

		gk_NODE_DUAL_Unlink((void **) &pevent->ECB_AssocTCB->TCB_NextTCBAEL,  // root to the ready list
							(void *)  pevent,                                 // node to unlink
							(G_UINT32) offsetof(GS_ECB,ECB_NextTCBAEL),        // next field offset
							(G_UINT32) offsetof(GS_ECB,ECB_PrevTCBAEL));       // previous field offset
		pevent->ECBState = GS_ECBState_UNLINKED;	

        /* Set the event unlink from task list */
        set_pointed_field(pevent, ECB_AssocTCB  , (struct gs_tcb *) 0);
		
        result = G_TRUE;
    }
    GK_SAMPLE_FUNCTION_END(10018)
    return(result);
}

/**gk_TCBASL_Link
 *  \brief 
 *  Links SCB to TCB Associated Signal List
 *  \param [in] ptcb    Pointer to the TCB
 *  \param [in] psignal Pointer to the SCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \relates Task
 *  \sa Signal
 */
G_UINT32 gk_TCBASL_Link(GS_TCB *ptcb, GS_SCB *psignal)
{
	set_pointed_field(psignal,SCBState, G_SCBState_IN_TCB);

	return	(gk_NODE_Link((void **) &ptcb->TCB_NextTCBASL,       // root to the ready list
		                 (void *) psignal, TCBASL));                               // ascending order sorting
}


/**gk_TCBASL_Unlink
 *  \brief 
 *  Unlinks SCB from a TCB Associated Signal List
 *  \param [in] ptcb    Pointer to the TCB of the task
 *  \param [in] psignal Pointer to the SCB of the signal
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Define the G_FALSE return when signal is not associated with the TCB
 *  \relates Task
 *  \sa Signal
 */ 
G_UINT32  gk_TCBASL_Unlink(GS_TCB *ptcb, GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10074)
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ptcb\n");
    PRINT_ASSERT((gkm_SCB_IsValid(psignal) == G_TRUE),"ERROR psignal\n");
    PRINT_ASSERT((psignal->SCBState != G_SCBState_UNLINKED),"ERROR SCBState= %d\n", (int) psignal->SCBState);
    G_UINT32 result = gk_SCB_Unlink_from_list(psignal, (void **) &ptcb->TCB_NextTCBASL);
    PRINT_ASSERT((psignal->SCBState == G_SCBState_UNLINKED),"ERROR SCBState= %d\n", (int) psignal->SCBState);
    GK_SAMPLE_FUNCTION_END(10074)
    return (result);
}

/**gk_TCB_List_Unlink
 *  \brief 
 *  Unlinks the TCB from events and signals and links them to the free lists respectively
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Check unlink for different resources (semaphore and messages) when they are granted
 *  \relates Task
 */
G_UINT32 gk_TCB_List_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10038)
	GS_SCB *psignal_cur;
	GS_ECB *pevent_cur;

	// Remove SCB from ASL
	while (get_pointed_field(ptcb, TCB_NextTCBASL) != (struct gs_scb *) 0)
	{
        PRINT_ASSERT((gkm_SCB_IsValid((GS_SCB *) ptcb->TCB_NextTCBASL ) == G_TRUE),"ERROR SCB not valid\n");
		psignal_cur = (GS_SCB *) get_pointed_field(ptcb, TCB_NextTCBASL);
		gk_TCBASL_Unlink(ptcb, psignal_cur);
		gk_SCBFL_Link(psignal_cur);
	}

	// Remove SCB from PSL
	while (get_pointed_field(ptcb, TCB_NextTCBPSL) != (struct gs_scb *) 0)
	{
        PRINT_ASSERT((gkm_SCB_IsValid((GS_SCB *) ptcb->TCB_NextTCBPSL) == G_TRUE),"ERROR SCB not valid\n");
		psignal_cur = (GS_SCB *) get_pointed_field(ptcb, TCB_NextTCBPSL);
		gk_TCBPSL_Unlink(ptcb, psignal_cur);
		gk_SCBFL_Link(psignal_cur);
	}

    // Remove ECB from AEL
    // UNGRANT and UNWAIT resource related event
    pevent_cur = (GS_ECB *) get_pointed_field(ptcb, TCB_NextTCBAEL);
	while (pevent_cur != (struct gs_ecb *) 0)
	{
        PRINT_ASSERT(((pevent_cur->ECB_AssocRCB == (GS_RCB *) 0) || (gkm_RCB_IsValid(pevent_cur->ECB_AssocRCB) == G_TRUE)),"ERROR RCB is not valid, ECB_AssocRCB= %p\n", pevent_cur->ECB_AssocRCB);
        PRINT_ASSERT((pevent_cur->ECB_AssocTCB == ptcb),"ERROR RCB is associated to task TCB\n");
        gk_ResourceECBKillCallback((GS_ECB *) pevent_cur);
        gk_ECBFL_Link(pevent_cur);
        pevent_cur = (GS_ECB *) get_pointed_field(ptcb, TCB_NextTCBAEL);
    }
    GK_SAMPLE_FUNCTION_END(10038)
    return(G_TRUE);
}

/**gk_TCBFL_Link
 *  \brief 
 *  Links a TCB to the Free TCB List removing associated and pending SCBs and ECBs
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \relates Task
 */ 
G_UINT32 gk_TCBFL_Link(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10075)

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB (%p) not valid\n",(void *) ptcb);
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_UNLINKED),"ERROR TCBState= %d\n",(int) ptcb->TCBState);

	/* Remove the links from TCB        */
    gk_TCB_List_Unlink(ptcb);

    // Does not free the structure to avoid fragmentation
    ptcb->TCB_NextTCB = g_kcb.KCB_FREE_TCBs;
    g_kcb.KCB_FREE_TCBs = ptcb;    

    GK_SAMPLE_FUNCTION_END(10075)
    return(G_TRUE);
}


/**gk_TCBPSL_Link
 *  \brief 
 *  Link a SCB to TCB Pending Signal List
 *  \param [in] ptcb    Pointer to the TCB
 *  \param [in] psignal Pointer to the SCB
 *  \return G_TRUE
 *  \relates Task 
 *  \sa Signal
 */ 
G_UINT32  gk_TCBPSL_Link(GS_TCB *ptcb, GS_SCB *psignal)
{
	set_pointed_field(psignal,SCBState, G_SCBState_PENDING);
	return	(gk_NODE_Link((void **) &ptcb->TCB_NextTCBPSL, (void *) psignal, TCBPSL));
}


/**gk_TCBPSL_Unlink
 *  \brief 
 *  Unlinks SCB from TCB Pending Signal List
 *  \param [in] ptcb    Pointer to the TCB
 *  \param [in] psignal Pointer to the SCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Return G_FALSE when it is not associated SCB to TCB
 *  \relates Task
 *  \sa Signal
 */
G_UINT32  gk_TCBPSL_Unlink(GS_TCB *ptcb, GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10093)
    PRINT_ASSERT((psignal->SCBState != G_SCBState_UNLINKED),"ERROR SCBState= %d\n",(int) psignal->SCBState);
    G_UINT32 result = gk_SCB_Unlink_from_list(psignal, (void **) &ptcb->TCB_NextTCBPSL);
    PRINT_ASSERT((psignal->SCBState == G_SCBState_UNLINKED),"ERROR SCBState= %d\n",(int) psignal->SCBState);
    GK_SAMPLE_FUNCTION_END(10093)
    return (result);
}

/**gk_TCBRDYL_Link
 *  \brief 
 *  Links the TCB in the Ready Task List sorted by its priority
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Implement G_FALSE return
 *  \relates Task
 */ 
G_UINT32 gk_TCBRDYL_Link(GS_TCB * const ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10013)

    GS_LCB *readylist = get_pointed_field(ptcb, TCB_RDY_LCB_Index); 
	
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_UNLINKED),"ERROR TCBState= %d\n",(int) ptcb->TCBState);  

	gk_TaskPrioritySetCallback(ptcb, G_TCBState_READY);
    ptcb->TCBCurrentPriority = gk_TaskInheritedPriority(ptcb, G_TCBState_READY);
	
	if (get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE) /* Idle task are not inserted in Ready Lists */
	{
		gk_NODE_Link((void **) &readylist->LCB_NextTCBRDYL,       // root to the ready list
		             (void *) ptcb, TCBRDYL);                     // ascending order sorting
					 
        set_pointed_field(ptcb, TCB_AssocPCB, (G_UINT32) 0); 
	}
	set_pointed_field(ptcb, TCBState, G_TCBState_READY); 

    GK_SAMPLE_FUNCTION_END(10013)
    return(G_TRUE);
}

/**gk_TCBRDYL_Unlink
 *  \brief 
 *  Unlinks the TCB from the Ready Task List
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Implement G_FALSE return when error
 *  \relates Task
 */
G_UINT32 gk_TCBRDYL_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10017)
	GS_LCB *readylist = (GS_LCB *) get_pointed_field(ptcb, TCB_RDY_LCB_Index);

    PRINT_ASSERT((gkm_TCB_IsValid(readylist->LCB_NextTCBRDYL) == G_TRUE || readylist->LCB_NextTCBRDYL == 0),"ERROR in ready list");

    if (get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE)  /* Task Idle is not inserted in ready lists */
    {
		gk_NODE_DUAL_Unlink((void **) &readylist->LCB_NextTCBRDYL,        // root to the ready list
							(void *)  ptcb,                               // node to unlink
							(G_UINT32) offsetof(GS_TCB,TCB_NextTCB),       // next field offset
							(G_UINT32) offsetof(GS_TCB,TCB_PrevTCB));      // previous field offset

    }

	set_pointed_field(ptcb, TCBState, G_TCBState_UNLINKED);

    PRINT_ASSERT((readylist->LCB_NextTCBRDYL == 0 || gkm_TCB_IsValid(readylist->LCB_NextTCBRDYL) == G_TRUE),"ERROR in ready list");

    GK_SAMPLE_FUNCTION_END(10017)
    return(G_TRUE);
}

/**gk_TCBRUNL_Link
 *  \brief 
 *  Links the TCB to the Run Task List (to be executed by the current processor)
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Implement G_FALSE return
 *  \relates Task
 */ 
G_UINT32  gk_TCBRUNL_Link(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10012)

    GS_LCB *RunList = (GS_LCB *) get_pointed_field(ptcb, TCB_RDY_LCB_Index);
	GS_PCB *ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[gm_ProcessorId() -1];
	
    PRINT_ASSERT(((get_pointed_field(RunList, LCB_NextTCBRUNL) == (struct gs_tcb *) 0) || (gkm_TCB_IsValid(get_pointed_field(RunList, LCB_NextTCBRUNL)) == G_TRUE)),"ERROR TCB not valid\n");
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_UNLINKED),"ERROR TCBState= %d\n",(int) ptcb->TCBState);    
  
	gk_TaskPrioritySetCallback(ptcb, G_TCBState_RUNNING);
    ptcb->TCBCurrentPriority = gk_TaskInheritedPriority(ptcb, G_TCBState_RUNNING);
    
	gk_NODE_Link((void **) &RunList->LCB_NextTCBRUNL,      // root to the ready list
	             (void *) ptcb, TCBRUNL);                  // ascending order sorting

    set_pointed_field(ptcb, TCBState, G_TCBState_RUNNING);
	set_pointed_field(ppcb, PCB_EXECTCB, (struct gs_tcb *) ptcb);
    set_pointed_field(ptcb, TCB_AssocPCB, gm_ProcessorId());

    if (get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE) RunList->LCBCurrentRunning++;
    
    /***********************************************/
    /* Set the processor to ptcb                   */
    PRINT_ASSERT((ptcb->TCB_StackPointer >= ptcb->TCB_StackTop - 300) && (ptcb->TCB_StackPointer <= ptcb->TCB_StackBottom),"ERROR TCB_StackPointer out of range\n");

    // If the task does not belong to the processor's primary ready list, or is IDLE, mark processor free
    if ((get_pointed_field(ptcb, TCB_RDY_LCB_Index) != (GS_LCB *) get_pointed_field(get_pointed_field(ppcb,PCB_AssocLCB),PCB_RDY_LCBL)) || (get_pointed_field(ptcb, TCBType) == G_TCBType_IDLE))
    {
        if (get_pointed_field(ppcb, PCBState) != GS_PCBState_FREE)
            gk_LCBFPL_Link(gm_ProcessorId()); // Link the processor to the free list
    }
    else
    {
        // If the task belongs to the processor's primary ready list, remove processor from the free list
        if (get_pointed_field(ppcb, PCBState) == GS_PCBState_FREE)
            gk_LCBFPL_Unlink((G_UINT32) gm_ProcessorId()); // Unlink the processor from the free list
    }
    
    GK_SAMPLE_FUNCTION_END(10012)
    return(G_TRUE);
}

/**
 *  \brief  Re-insert ptcb into its current list at the correct position after its
 *          inherited priority changes.
 *
 *  Recalculates \p ptcb's inherited priority via gk_TaskInheritedPriority() and,
 *  if the value changed, removes the task from its current list (READY or RUN) and
 *  re-inserts it so the ordering invariant is preserved.  Has no effect when the
 *  task is in any other state (e.g. WAITING, UNLINKED).
 *
 *  \param [in] ptcb  Pointer to the TCB whose priority shall be updated.
 *  \return G_TRUE always.
 *
 *  \pre  ptcb must be a valid non-IDLE task.
 *  \post ptcb->TCBCurrentPriority reflects the latest inherited value.
 *        List ordering is preserved.
 *  \note Caller must hold the GeMRTOS mutex.
 */
G_UINT32 gk_TCBUpdate_Priority(GS_TCB *ptcb)
{
    G_UINT64 old_priority = ptcb->TCBCurrentPriority;
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE), "ERROR ptcb is not valid\n");
    PRINT_ASSERT((get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE), "ERROR cannot change IDLE priority");
    GS_LCB *RunList = (GS_LCB *) get_pointed_field(ptcb, TCB_RDY_LCB_Index);

    ptcb->TCBCurrentPriority = gk_TaskInheritedPriority(ptcb, ptcb->TCBState);
    if (old_priority != ptcb->TCBCurrentPriority)
    {
        switch (ptcb->TCBState)
        {
            case G_TCBState_READY:
                PRINT_ASSERT((gkm_TCB_IsValid(RunList->LCB_NextTCBRDYL) == G_TRUE), "ERROR TCB not valid\n");
                gk_NODE_DUAL_Unlink((void **) &RunList->LCB_NextTCBRDYL,
                                    (void *)  ptcb,
                                    (G_UINT32) offsetof(GS_TCB, TCB_NextTCB),
                                    (G_UINT32) offsetof(GS_TCB, TCB_PrevTCB));
                gk_NODE_Link((void **) &RunList->LCB_NextTCBRDYL, (void *) ptcb, TCBRDYL);
                break;

            case G_TCBState_RUNNING:
                PRINT_ASSERT((gkm_TCB_IsValid(RunList->LCB_NextTCBRUNL) == G_TRUE), "ERROR TCB not valid\n");
                gk_NODE_DUAL_Unlink((void **) &RunList->LCB_NextTCBRUNL,
                                    (void *)  ptcb,
                                    (G_UINT32) offsetof(GS_TCB, TCB_NextTCB),
                                    (G_UINT32) offsetof(GS_TCB, TCB_PrevTCB));
                gk_NODE_Link((void **) &RunList->LCB_NextTCBRUNL, (void *) ptcb, TCBRUNL);
                break;

            default:
                break;
        }
    }

    return G_TRUE;
}

/* ===========================================================================
 * SECTION 7 â€” NODE ENGINE: Core sorted intrusive list algorithm
 * Implements ordered insertion and removal for all kernel list types
 * (TCB, ECB, SCB, LCB, etc.) using offset-based field access.
 * All Section 1-6 link/unlink operations delegate to these three functions.
 * Functions: gk_NODE_Link  (O(N) ordered insertion),
 *            gk_NODE_DUAL_Unlink  (doubly-linked removal),
 *            gk_NODE_SINGLE_Unlink  (singly-linked removal)
 * =========================================================================== */
/**
 * @ingroup Scheduler
 * @brief Insert a node into a sorted intrusive list.
 *
 * Performs ordered insertion of a node into a kernel intrusive list.
 * The function supports multiple list types and node structures by
 * using offset-based field access.
 *
 * The insertion preserves list ordering according to the policy
 * associated with @p list_name.
 *
 * Supported Features:
 *  - Heterogeneous node types (TCB, ECB, SCB, etc.)
 *  - Single or double linked lists
 *  - Ascending or descending ordering
 *  - 32-bit or 64-bit comparison keys
 *  - Development and production unified implementation
 *
 * The list is intrusive, meaning that link fields are embedded
 * directly inside the node structure.
 *
 * ---
 *
 * @param[in,out] root
 *      Pointer to the list head pointer.
 *      The head may be modified if the inserted node becomes
 *      the new first element.
 *
 * @param[in] node
 *      Pointer to the node to be inserted into the list.
 *
 * @param[in] list_name
 *      Identifier describing the list policy:
 *          - link field offsets
 *          - ordering direction
 *          - comparison value
 *          - single/double linkage
 *
 * @return
 *      ::G_TRUE on successful insertion.
 *
 * ---
 *
 * @pre
 * - @p root must be a valid pointer.
 * - @p node must not be NULL.
 * - Node must not already belong to another list.
 *
 * @post
 * - Node is linked into the list respecting ordering rules.
 * - Head pointer is updated if required.
 * - List integrity is preserved.
 *
 * ---
 *
 * @par Ordering Policy
 * Depending on @p list_name:
 *
 * - READY lists:
 *      ascending priority ordering
 * - RUN lists:
 *      descending execution order
 * - Event lists:
 *      ordered by 64-bit value
 *
 * Conceptual behavior:
 * @code
 * if list empty OR node precedes head:
 *     insert at head
 * else:
 *     locate insertion position
 *     relink neighbours
 * @endcode
 *
 * ---
 *
 * @par Intrusive List Model
 * Nodes contain their own linkage fields:
 *
 * @code
 * struct node {
 *     node *next;
 *     node *prev;
 *     key   sort_value;
 * };
 * @endcode
 *
 * No memory allocation is performed.
 *
 *
 * ---
 *
 * @par Safety Notes
 * PRINT_ASSERT checks validate:
 *  - input parameters
 *  - list configuration
 *  - structural assumptions
 *
 * Production builds remain functional even when assertions
 * are compiled out.
 *
 * ---
 *
 * @par Complexity
 * Time Complexity: O(N)
 * where N is the number of nodes in the list.
 *
 * Space Complexity: O(1)
 *
 * ---
 *
 * @warning
 * Inserting the same node twice without unlinking first
 * results in undefined behavior.
 *
 * @note
 * Offset-based access macros (get_pointed_offset /
 * get_pointed_field) allow unified implementation for
 * development instrumentation and optimized production builds.
 */
static G_UINT32 gk_NODE_Link(void **root,
                      void * const node,
                      enum list_type list_name)
{
    PRINT_ASSERT(root != NULL, "ERROR root NULL\n");
    PRINT_ASSERT(node != NULL, "ERROR node NULL\n");

    void *current = *root;

    G_UINT32 offset_next  = 0;
    G_UINT32 offset_prev  = 0;
    G_UINT32 offset_value = 0;

    G_UINT32 dual_linked = G_FALSE;
    G_UINT32 data64      = G_FALSE;
    G_UINT32 asc_sort    = G_TRUE;

    /*-----------------------------------------------------------
     * Select list policy
     *-----------------------------------------------------------*/
    switch (list_name)
    {
        case ECBASL:
        case KCBASL:
        case KCBPSL:
        case RCBASL:
        case TCBPSL:
        case TCBASL:
            offset_next  = offsetof(GS_SCB, SCB_NextSCB);
            offset_prev  = offsetof(GS_SCB, SCB_NextSCB); 
            offset_value = offsetof(GS_SCB, SCBType);
            break;
       
        case ECBTL:
        case RCBGEL:
        case RCBWEL:
            offset_next  = offsetof(GS_ECB, ECB_NextECB);
            offset_prev  = offsetof(GS_ECB, ECB_PrevECB);
            offset_value = offsetof(GS_ECB, ECBValue.i64);
            data64       = G_TRUE;
            dual_linked  = G_TRUE;
            break;

        case TCBRDYL:
            offset_next  = offsetof(GS_TCB, TCB_NextTCB);
            offset_prev  = offsetof(GS_TCB, TCB_PrevTCB);
            offset_value = offsetof(GS_TCB, TCBCurrentPriority);
            data64       = G_TRUE;
            dual_linked  = G_TRUE;
            break;

        case TCBRUNL:
            offset_next  = offsetof(GS_TCB, TCB_NextTCB);
            offset_prev  = offsetof(GS_TCB, TCB_PrevTCB);
            offset_value = offsetof(GS_TCB, TCBCurrentPriority);
            data64       = G_TRUE;
            dual_linked  = G_TRUE;
            asc_sort     = G_FALSE;
            break;

        default:
            PRINT_ASSERT(G_FALSE, "ERROR unknown list type\n");
            return G_FALSE;
    }

    /*-----------------------------------------------------------
     * Comparator helpers
     *-----------------------------------------------------------*/
#define NODE_VALUE64(n) \
    get_pointed_offset(G_UINT64,(n),offset_value)

#define NODE_VALUE32(n) \
    get_pointed_offset(G_UINT32,(n),offset_value)

#define NODE_NEXT(n) \
    get_pointed_offset(void*,(n),offset_next)

#define NODE_PREV(n) \
    get_pointed_offset(void*,(n),offset_prev)

#define SHOULD_INSERT_BEFORE(a,b) \
    (data64 ? \
        (asc_sort ? (NODE_VALUE64(a) > NODE_VALUE64(b)) \
                  : (NODE_VALUE64(a) < NODE_VALUE64(b))) \
      : \
        (asc_sort ? (NODE_VALUE32(a) > NODE_VALUE32(b)) \
                  : (NODE_VALUE32(a) < NODE_VALUE32(b))))

    /*-----------------------------------------------------------
     * Insert as new head
     *-----------------------------------------------------------*/
    if (current == NULL || SHOULD_INSERT_BEFORE(current, node))
    {
        NODE_NEXT(node) = current;

        if (dual_linked)
        {
            NODE_PREV(node) = NULL;

            if (current != NULL)
                NODE_PREV(current) = node;
        }

        *root = node;
        return G_TRUE;
    }

    /*-----------------------------------------------------------
     * Find insertion position
     *-----------------------------------------------------------*/
    while (NODE_NEXT(current) != NULL &&
           !SHOULD_INSERT_BEFORE(NODE_NEXT(current), node))
    {
        current = NODE_NEXT(current);
    }

    void *next = NODE_NEXT(current);

    NODE_NEXT(node) = next;
    NODE_NEXT(current) = node;

    if (dual_linked)
    {
        NODE_PREV(node) = current;

        if (next != NULL)
            NODE_PREV(next) = node;
    }

#undef NODE_VALUE64
#undef NODE_VALUE32
#undef NODE_NEXT
#undef NODE_PREV
#undef SHOULD_INSERT_BEFORE

    return G_TRUE;
}

/**
 * @ingroup Scheduler
 * @brief Remove a node from a doubly linked intrusive list.
 *
 * Unlinks a node from a doubly linked kernel list using
 * offset-based field access. The function updates neighbour
 * links and detaches the node from the container.
 *
 * No memory allocation or deallocation is performed.
 *
 * @param[in,out] root
 *      Pointer to the list head pointer.
 *      Updated if the removed node is the head element.
 *
 * @param[in] node
 *      Node to be removed from the list.
 *
 * @param[in] offset_next
 *      Offset of the "next" pointer inside the node structure.
 *
 * @param[in] offset_prev
 *      Offset of the "previous" pointer inside the node structure.
 *
 * @return
 *      ::G_TRUE on successful unlink operation.
 *
 * @pre
 * - @p root must be valid.
 * - @p node must belong to the list.
 * - Scheduler/container lock must be held by caller.
 *
 * @post
 * - Node is detached from the list.
 * - Next and previous pointers inside node are cleared.
 * - List integrity is preserved.
 *
 * @par Intrusive List Model
 * The list is intrusive: linkage fields exist inside the node.
 *
 * @code
 * struct node {
 *     node *next;
 *     node *prev;
 * };
 * @endcode
 *
 * @par Concurrency
 * This function performs no synchronization.
 * Caller must guarantee exclusive access to the list.
 *
 * @par Safety
 * PRINT_ASSERT checks detect list corruption during
 * development builds. Production builds remain functional
 * without assertions.
 *
 * @par Complexity
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 *
 * @warning
 * Calling unlink on a node not belonging to the list
 * results in undefined behavior.
 *
 * @see gk_NODE_Link
 */
static G_UINT32 gk_NODE_DUAL_Unlink(void **root,
                             void * const node,
                             G_UINT32 const offset_next,
                             G_UINT32 const offset_prev)
{
    PRINT_ASSERT(root != NULL, "ERROR root NULL\n");
    PRINT_ASSERT(node != NULL, "ERROR node NULL\n");

    void *next = get_pointed_offset(void *, node, offset_next);
    void *prev = get_pointed_offset(void *, node, offset_prev);

    /*-----------------------------------------------------------
     * Update list head if required
     *-----------------------------------------------------------*/
    if (*root == node)
    {
        *root = next;
    }
    else
    {
        PRINT_ASSERT(prev != NULL,
                     "ERROR corrupted list (prev NULL)\n");

        get_pointed_offset(void *, prev, offset_next) = next;
    }

    /*-----------------------------------------------------------
     * Relink next node
     *-----------------------------------------------------------*/
    if (next != NULL)
    {
        get_pointed_offset(void *, next, offset_prev) = prev;
    }

    /*-----------------------------------------------------------
     * Clear linkage (node becomes detached)
     *-----------------------------------------------------------*/
    get_pointed_offset(void *, node, offset_next) = NULL;
    get_pointed_offset(void *, node, offset_prev) = NULL;

    return G_TRUE;
} 


/**
 * @ingroup Scheduler
 * @brief Remove a node from a singly linked intrusive list.
 *
 * Unlinks a node from a kernel intrusive singly linked list
 * using offset-based access to the linkage field.
 *
 * The function searches the list for the specified node and
 * updates linkage pointers to preserve list integrity.
 *
 * No dynamic memory operations are performed.
 *
 * @param[in,out] root
 *      Pointer to the list head pointer.
 *      Updated if the removed node is the first element.
 *
 * @param[in] node
 *      Node to be removed from the list.
 *
 * @param[in] offset_next
 *      Offset of the "next" pointer inside the node structure.
 *
 * @return
 *      ::G_TRUE  if node was found and removed.
 *      ::G_FALSE if node was not present in the list.
 *
 * @pre
 * - @p root must be valid.
 * - @p node must reference a valid structure.
 * - Caller must guarantee exclusive access to the list.
 *
 * @post
 * - Node is detached from the list.
 * - Node next pointer is cleared.
 * - List structure remains consistent.
 *
 * @par Intrusive List Model
 * Nodes contain their own linkage fields:
 *
 * @code
 * struct node {
 *     node *next;
 * };
 * @endcode
 *
 * @par Concurrency
 * This function performs no synchronization.
 * Caller must hold scheduler lock or equivalent protection.
 *
 * @par Complexity
 * Time Complexity: O(N)
 * Space Complexity: O(1)
 *
 * @warning
 * Removing a node that does not belong to the list
 * has no effect but clears its next pointer.
 *
 * @see gk_NODE_Link
 * @see gk_NODE_DUAL_Unlink
 */
static G_UINT32 gk_NODE_SINGLE_Unlink(void **root,
                               void *node,
                               G_UINT32 const offset_next)
{
    PRINT_ASSERT(root != NULL, "ERROR root NULL\n");
    PRINT_ASSERT(node != NULL, "ERROR node NULL\n");

    void *current = *root;
    G_UINT32 result = G_FALSE;

    /*-----------------------------------------------------------
     * Empty list
     *-----------------------------------------------------------*/
    if (current == NULL)
        return G_FALSE;

    /*-----------------------------------------------------------
     * Case 1: node is list head
     *-----------------------------------------------------------*/
    if (current == node)
    {
        *root = get_pointed_offset(void *, node, offset_next);
        result = G_TRUE;
    }
    else
    {
        /*-------------------------------------------------------
         * Search previous node
         *-------------------------------------------------------*/
        while (get_pointed_offset(void *, current, offset_next) != NULL &&
               get_pointed_offset(void *, current, offset_next) != node)
        {
            current = get_pointed_offset(void *, current, offset_next);
        }

        /*-------------------------------------------------------
         * Relink if found
         *-------------------------------------------------------*/
        if (get_pointed_offset(void *, current, offset_next) == node)
        {
            void *next =
                get_pointed_offset(void *, node, offset_next);

            get_pointed_offset(void *, current, offset_next) = next;

            result = G_TRUE;
        }
    }

    /*-----------------------------------------------------------
     * Detach node (safe even if not found)
     *-----------------------------------------------------------*/
    get_pointed_offset(void *, node, offset_next) = NULL;

    return result;
}

/* ===========================================================================
 * SECTION 8 â€” TCB: Task Control Block list management (part B)
 * These functions appear after the NODE engine (Section 7) because they
 * call gk_NODE_DUAL_Unlink directly.  Logically they belong with Section 6.
 * Also includes gk_TCB_in_RCBGEL (search) and gk_RRDSFL_Link (RRDS free list).
 * Functions: gk_TCBRUNL_Unlink, gk_TCBWL_Link, gk_TCBWL_Unlink,
 *            gk_TCB_in_RCBGEL, gk_RRDSFL_Link
 * =========================================================================== */
/**gk_TCBRUNL_Unlink
 *  \brief
 *  Unlinks TCB from Running List
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Include G_FALSE when TCB is not in RUN list
 *  \relates Task
 */
G_UINT32  gk_TCBRUNL_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10005)
	G_UINT32 cpu_id = get_pointed_field(ptcb, TCB_AssocPCB);
    
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_RUNNING),"ERROR TCBState= %d, should be Running\n", (int) ptcb->TCBState);

    GS_LCB *RunList = (GS_LCB *) get_pointed_field(ptcb, TCB_RDY_LCB_Index);

    PRINT_ASSERT((gkm_TCB_IsValid(RunList->LCB_NextTCBRUNL) == G_TRUE),"ERROR TCB not valid\n");
    PRINT_ASSERT((cpu_id >= 1 && cpu_id <= G_NUMBER_OF_PCB),"ERROR TCB_AssocPCB= %d\n", (int) cpu_id);    

    // Stop the processor executing running task triggering its interrupt
    if (cpu_id != gm_ProcessorId() ) {
        gm_ProcessorInterrupt(cpu_id);
    }

    if (g_kcb.G_PCBTbl[cpu_id - 1].PCBState != GS_PCBState_FREE)
		gk_LCBFPL_Link(cpu_id);  // Link processor to free list

	gk_NODE_DUAL_Unlink((void **) &RunList->LCB_NextTCBRUNL,        // root to the ready list
						(void *)  ptcb,                               // node to unlink
						(G_UINT32) offsetof(GS_TCB,TCB_NextTCB),       // next field offset
						(G_UINT32) offsetof(GS_TCB,TCB_PrevTCB));      // previous field offset

    /* Detach the task from the processor */
    g_kcb.G_PCBTbl[cpu_id - 1].PCB_EXECTCB = g_kcb.G_PCBTbl[cpu_id-1].PCB_IDLETCB;
    
    if (get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE)
    {
        PRINT_ASSERT(RunList->LCBCurrentRunning > 0, "ERROR LCBCurrentRunning underflow in gk_TCBRUNL_Unlink\n");
        RunList->LCBCurrentRunning--;
    }
    
	set_pointed_field(ptcb, TCBState, G_TCBState_UNLINKED); 
    set_pointed_field(ptcb, TCB_AssocPCB, 0);
    

	GK_SAMPLE_FUNCTION_END(10005)
    return(G_TRUE);
}
 
/**gk_TCBWL_Link
 *  \brief 
 *  Links a TCB in the waiting list
 *  \param [in] ptcb  Pointer to the TCB of the task
 *  \param [in] state Type of the waiting state
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Define the types of waiting states
 *  \relates Task
 */ 
G_UINT32  gk_TCBWL_Link(GS_TCB *ptcb, unsigned int state)
{
    GK_SAMPLE_FUNCTION_BEGIN(10027)
	
	GS_TCB *ptcb1 = g_kcb.KCB_NextTCBWL;
    
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB not valid\n");
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_UNLINKED),"ERROR Invalid TCBState= %d\n", ptcb->TCBState);    

    /* Insert the task in the Waiting List   */
    set_pointed_field(ptcb, TCB_NextTCB, ptcb1);
    set_pointed_field(ptcb, TCB_PrevTCB,(struct gs_tcb *) 0);

    if (ptcb1 != (struct gs_tcb *) 0) set_pointed_field(ptcb1, TCB_PrevTCB, (struct gs_tcb *) ptcb);

    g_kcb.KCB_NextTCBWL = (struct gs_tcb *) ptcb;
    set_pointed_field(ptcb, TCBState, state);

    // Initialise the stack if it is completed
    if (state == G_TCBState_WAITING_COMPLETED) {
		gk_TASK_STK_INIT((GS_TCB *) ptcb);
	    set_pointed_field(ptcb, TCB_MTX_NESTED, 0);
	}
    
    GK_SAMPLE_FUNCTION_END(10027)
    return(G_TRUE);
}

/**gk_TCBWL_Unlink
 *  \brief 
 *  Unlinks the TCB from the waiting list
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Implement G_FALSE return when is not waiting
 *  \relates Task
 */
G_UINT32 gk_TCBWL_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10010)
    
    PRINT_ASSERT(((ptcb->TCBState == G_TCBState_WAITING_COMPLETED) || (ptcb->TCBState == G_TCBState_WAITING)),"ERROR TCB should be waiting\n");
    PRINT_ASSERT((ptcb->TCBType != G_TCBType_IDLE),"ERROR TCB type\n");  
    
	gk_NODE_DUAL_Unlink((void **) &g_kcb.KCB_NextTCBWL,               // root to the ready list
						(void *)  ptcb,                               // node to unlink
						(G_UINT32) offsetof(GS_TCB,TCB_NextTCB),       // next field offset
						(G_UINT32) offsetof(GS_TCB,TCB_PrevTCB));      // previous field offset
	
    set_pointed_field(ptcb, TCBState, G_TCBState_UNLINKED);

    GK_SAMPLE_FUNCTION_END(10010)
    return(G_TRUE);
}


/**gk_TCB_in_RCBGEL
 *  \brief 
 *  Returns the event of the TCB that grants the resource
 *  \param [in] presource Pointer to RCB
 *  \param [in] ptcb      Pointer to TCB
 *  \return Pointer to the ECB granted the resource or NULL if not granted
 */
GS_ECB *gk_TCB_in_RCBGEL(GS_RCB *presource, GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10037)
	GS_ECB *pevent; 

	PRINT_ASSERT((gkm_RCB_IsValid(presource) == G_TRUE),"ERROR RCB is not valid\n");
	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");

    pevent = get_pointed_field(presource, RCB_NextRCBGEL);
    while (pevent != (struct gs_ecb *) 0 &&
           get_pointed_field(pevent, ECB_AssocTCB) != ptcb)
    {
        pevent = get_pointed_field(pevent, ECB_NextECB);
    }
	
	PRINT_ASSERT(((pevent == (struct gs_ecb *) 0) || (pevent->ECB_AssocTCB == ptcb)),"ERROR ECB is not valid\n");
	
    GK_SAMPLE_FUNCTION_END(10037)
	return(pevent); 
}

/**gk_RRDSFL_Link
 *  \brief 
 *  Links a RRDS to the Free List
 *  \param [in] prrds Pointer to the RRDS
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check the RRDS according structure (queue, semaphore)
 *  \todo Check if RRDS is to ECB or TCB too (I am not sure)  
 */
G_UINT32 gk_RRDSFL_Link(GS_RRDS *prrds)
{
    GK_SAMPLE_FUNCTION_BEGIN(10036)

    set_pointed_field(prrds, RRDS_NextRRDS, g_kcb.KCB_FREE_RRDSs);
    g_kcb.KCB_FREE_RRDSs = prrds;
    set_pointed_field(prrds, RRDS_AsocECB, (GS_ECB *) 0);
    
    GK_SAMPLE_FUNCTION_END(10036)
    return(G_TRUE);
}


/***********************************************************************************
*********************** GRTOS INTERNAL FUNCTIONS  *********************************
***********************************************************************************/
/**
 * 
 * @return pointer to TCB of task executed by processor
 */
 
/**gk_PCB_GetCurrentTCB
 *  \brief 
 *  Returns the task that the current processor was executing
 *  \return Pointer to the TCB of the task being executed by the current processor
 *  \relates Task
 */
GS_TCB *gk_PCB_GetCurrentTCB(void)
{
	GK_SAMPLE_FUNCTION_BEGIN(10023)

	GS_TCB *ptcb = g_kcb.G_PCBTbl[gm_ProcessorId() - 1].PCB_EXECTCB;
	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid PCB_EXECTCB= %s\n", (char *) ptcb->TCB_description);

    GK_SAMPLE_FUNCTION_END(10023)
	return(ptcb);
}

/**
 * @brief Select next Task Control Block to execute on a processor.
 *
 * This function implements the processor scheduling decision.
 * It determines which task shall be executed next based on the
 * ready task lists associated with the provided Processor Control
 * Block (PCB).
 *
 * The scheduler follows these rules:
 *
 * - The processor idle task is selected by default.
 * - All ready LCB (List Control Block) queues associated with the
 *   processor are scanned in priority oreder.
 * - If a runnable task exists in a different ready queue and the
 *   exclusion condition allows execution, that task is selected.
 * - If the current executing task belongs to the same ready queue,
 *   a priority comparison is performed:
 *      - A higher priority ready task preempts the current task.
 *      - Otherwise, the current task continues execution.
 *
 * The function guarantees that a valid TCB pointer is returned.
 * If no runnable task is available, the processor idle task is used.
 *
 * @param[in] ppcb
 *      Pointer to the Processor Control Block representing the
 *      current processor.
 *
 * @return
 *      Pointer to the next Task Control Block to execute.
 *      The returned pointer is never NULL.
 *
 * @pre
 * - @p ppcb must reference a valid initialized PCB.
 * - PCB_EXECTCB and PCB_IDLETCB must contain valid TCB objects.
 *
 * @post
 * - Returned TCB is in state:
 *      - ::G_TCBState_READY or
 *      - ::G_TCBState_RUNNING
 *
 * @invariant
 * - Scheduler always returns a runnable task.
 * - Idle task acts as fallback execution context.
 *
 * @note
 * This function is used by the processor dispatcher during
 * scheduling or context switch operations.
 *
 * @note
 * Development builds rely on PRINT_ASSERT diagnostics for
 * structural validation. Production builds remain safe even
 * when assertions are disabled.
 *
 * @warning
 * This function does not perform synchronization. Caller must
 * ensure proper GeMRTOS mutex grant.
 *
 * @par Scheduling Policy
 * @code
 * default = idle_task
 *
 * for each associated ready list:
 *     if ready task exists:
 *         if different queue:
 *             check exclusion rule
 *         else:
 *             evaluate priority preemption
 * @endcode
 *
 * @par Complexity
 * Time Complexity: O(N)
 * where N is the number of associated ready LCB lists.
 *
 * @par Related Components
 * @see GS_PCB
 * @see GS_TCB
 * @see GS_PCBAssocLCB
 * @see GS_LCB
 *
 * @relates Processor
 */
GS_TCB *gk_PCB_GetNextTCB(GS_PCB *ppcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10002);

    PRINT_ASSERT(ppcb != NULL,
                 "ERROR PCB pointer NULL\n");

    /*-----------------------------------------------------------
     * Current and default task
     *-----------------------------------------------------------*/
    GS_TCB *ptcb       = ppcb->PCB_EXECTCB;
    GS_TCB *ptcb_next  = ppcb->PCB_IDLETCB;
    GS_PCBAssocLCB *ppcbalcb = ppcb->PCB_AssocLCB;

    PRINT_ASSERT(gkm_TCB_IsValid(ptcb) == G_TRUE, "ERROR current TCB not valid\n");

    PRINT_ASSERT(gkm_TCB_IsValid(ptcb_next) == G_TRUE,
                 "ERROR idle TCB not valid\n");

    /*-----------------------------------------------------------
     * Scan ready LCB lists
     *-----------------------------------------------------------*/
    while (ppcbalcb != NULL)
    {
        /* Validate each node after the loop guard confirms non-NULL.
         * The first node is always non-NULL (gk_Init_PCBs invariant);
         * subsequent nodes may be NULL at end-of-list.               */
        PRINT_ASSERT(gkm_PCBAssocLCB_IsValid(ppcbalcb) == G_TRUE,
                     "ERROR AssocLCB not valid (%p)\n",
                     (void *)ppcbalcb);

        GS_LCB *plcb = get_pointed_field(ppcbalcb, PCB_RDY_LCBL);

        PRINT_ASSERT(gkm_LCB_IsValid(plcb) == G_TRUE, "ERROR LCB not valid\n");

        GS_TCB *pready = get_pointed_field(plcb, LCB_NextTCBRDYL);        
        
        /*-------------------------------------------------------
         * No ready task in this list.
         *
         * If this is ptcb's OWN list, its ready queue being empty does
         * NOT mean "nothing of interest here" -- it means ptcb itself
         * (currently RUNNING, hence absent from the READY queue) is
         * already the sole/best contender this list has to offer. Any
         * list scanned after this point is necessarily lower priority
         * (association lists are ordered by priority), so nothing further
         * down the chain may ever preempt ptcb. Stop here and keep it
         * running -- otherwise CASE 1 below would let a lower-priority
         * list's ready task look preferable purely because this list
         * "looked" empty, which is a multi-LCB livelock: two processors
         * each treat the other's foreground task as absent (because it's
         * RUNNING, not READY) and keep bouncing it back to READY forever
         * (confirmed via JTAG memory trace).
         *
         * Otherwise (some other list is merely empty), keep scanning.
         *-------------------------------------------------------*/
        if (pready == NULL)
        {
            if (ptcb->TCB_RDY_LCB_Index == plcb)
            {
                ptcb_next = ptcb;
                break;
            }

            ppcbalcb = get_pointed_field(ppcbalcb, gs_pcb_rdy_lcbl_next);
            continue;
        }
        
        PRINT_ASSERT(gkm_TCB_IsValid(pready) == G_TRUE, 
                     "ERROR ready TCB not valid\n");        
        
        /*=======================================================
         * CASE 1: Different ready queue
         *=======================================================*/
        if (ptcb->TCB_RDY_LCB_Index != plcb)
        {
            if (get_pointed_field(plcb, LCBExclusion) > 
                get_pointed_field(plcb, LCBCurrentRunning))
            {
                /* runnable task found */
                ptcb_next = pready;
                break;
            }
        }        
        
        /*=======================================================
         * CASE 2: Same ready queue â†’ preemption decision
         *=======================================================*/
        else
        {
            if (ptcb->TCBCurrentPriority > pready->TCBReadyPriority)
            {
                /* higher priority ready task */
                ptcb_next = pready;
            }
            else
            {
                /* keep executing current */
                ptcb_next = ptcb;
            }

            break;
        }
        
        /* Move to next associated ready list */
        ppcbalcb = get_pointed_field(ppcbalcb, gs_pcb_rdy_lcbl_next);        

    }

    PRINT_ASSERT(ptcb_next->TCBState == G_TCBState_READY ||
                 ptcb_next->TCBState == G_TCBState_RUNNING,
                 "ERROR next task neither READY nor RUNNING\n");

    GK_SAMPLE_FUNCTION_END(10002);
    
    return(ptcb_next);
}

/**gk_PCBAssocLCBFL_Link
 *  \brief 
 *  Links a PCBAssocLCB to the Free PCBAssocLCB List
 *  \param [in] ppcbalcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \relates Task
 */ 
G_UINT32 gk_PCBAssocLCBFL_Link(GS_PCBAssocLCB *ppcbalcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10076)

    PRINT_ASSERT((gkm_PCBAssocLCB_IsValid(ppcbalcb) == G_TRUE),"ERROR gkm_GS_PCBAssocLCB_IsValid (%p) not valid\n",(void *) ppcbalcb);
    PRINT_ASSERT((gkm_LCB_IsValid(ppcbalcb->PCB_RDY_LCBL) == G_TRUE),"ERROR gkm_LCB_IsValid not valid\n");
    
    ppcbalcb->gs_pcb_rdy_lcbl_next = g_kcb.KCB_FREE_RDYs;
    g_kcb.KCB_FREE_RDYs = ppcbalcb;    

    GK_SAMPLE_FUNCTION_END(10076)
    return(G_TRUE);
}


/**gk_TCB_Unlink
 *  \brief 
 *  Unlinks the TCB according to the list it is linked
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Implement the G_FALSE when no linked
 *  \relates Task
 */
G_UINT32 gk_TCB_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10006)

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");

	switch (ptcb->TCBState)
	{
		case G_TCBState_RUNNING:           gk_TCBRUNL_Unlink(ptcb); break;
		case G_TCBState_READY:             gk_TCBRDYL_Unlink(ptcb); break;
		case G_TCBState_WAITING_COMPLETED:
        case G_TCBState_WAITING:           gk_TCBWL_Unlink(ptcb); break;
		case G_TCBState_UNLINKED:
			if (get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE) G_DEBUG_WHILEFOREVER;
			break;
		default:
			G_DEBUG_WHILEFOREVER;
			break;
	}
    GK_SAMPLE_FUNCTION_END(10006)
    return(G_TRUE);
}

inline GS_SCB *gk_TCBPSL_GetSCB(GS_TCB *ptcb, G_UINT32 SignalType)
{
	return (gk_GetSCB_from_list(ptcb->TCB_NextTCBPSL, SignalType));
}


/**gk_KCBASL_GetSCB
 *  \brief 
 *  Returns a pointer to the SCB linked to the KCBASL with the same SignalType
 *  \param [in] SignalType Signal type to search in the KCB
 *  \return Pointer to the SCB found or NULL if no SCB with such a type
 *  \todo Check if this function is necessary
 */ 
inline GS_SCB *gk_KCBASL_GetSCB(G_UINT32 SignalType)
{
	return (gk_GetSCB_from_list(g_kcb.KCB_NextKCBASL, SignalType));
}


/**gk_TCBASL_GetSCB
 *  \brief 
 *  Returns the pointer to the SCB associated to a task, NULL otherwise
 *  \param [in] ptcb       Pointer to the TCB
 *  \param [in] SignalType Signal type to search in the associated signal list of the TCB
 *  \return Returns the pointer to the SCB associated to a task, NULL otherwise
 *  \relates Task
 *  \sa Signal
 */
inline GS_SCB *gk_TCBASL_GetSCB(GS_TCB *ptcb, G_UINT32 SignalType)
{
	return (gk_GetSCB_from_list(ptcb->TCB_NextTCBASL, SignalType));
}


/**gk_ECBASL_GetSCB
 *  \brief 
 *  Returns a pointer to the GS_SCB in the associated GS_SCBs of an event or NULL if any
 *  \param [in] pecb       Pointer to the ECB
 *  \param [in] SignalType Signal type to search
 *  \return Returns a pointer to the GS_SCB in the associated GS_SCBs of a event or NULL if any
 */ 
inline GS_SCB *gk_ECBASL_GetSCB(GS_ECB *pecb, G_UINT32 SignalType)
{
	return (gk_GetSCB_from_list(pecb->ECB_NextECBASL, SignalType));
}


/**gk_RCBASL_GetSCB
 *  \brief 
 *  Returns the pointer to the SCB of a resource or NULL otherwise
 *  \param [in] prcb       Pointer to the RCB of the resource
 *  \param [in] SignalType Signal Type og the signal
 *  \return Pointer to the SCB of NULL if was not found
 *  \relates Signal
 */
inline GS_SCB *gk_RCBASL_GetSCB(GS_RCB *prcb, G_UINT32 SignalType)
{
	return (gk_GetSCB_from_list(prcb->RCB_NextRCBASL, SignalType));
}


/**gk_GetSCB_from_list
 *  \brief 
 *  Returns the pointer to the SCB in a SCB linked list or NULL otherwise
 *  \param [in] head_list       Pointer to the root of the list
 *  \param [in] SignalType Signal Type og the signal
 *  \return Pointer to the SCB or NULL if was not found
 *  \relates Signal
 */
GS_SCB *gk_GetSCB_from_list(GS_SCB *head_list, G_UINT32 SignalType)
{
    GK_SAMPLE_FUNCTION_BEGIN(10094)
    GS_SCB *psignal  = (GS_SCB *) 0;
    GS_SCB *psignal1 = (GS_SCB *) head_list;

    while (psignal1 != (struct gs_scb *) 0)
    {
        PRINT_ASSERT((gkm_SCB_IsValid(psignal1) == G_TRUE),"ERROR no valid psignal1\n");
        if (psignal1->SCBType == SignalType)
        {
            psignal = psignal1;
            break;
        }
        psignal1 = psignal1->SCB_NextSCB;
    }
    GK_SAMPLE_FUNCTION_END(10094)
    return(psignal);
}
/***********************************************************************************
*********************** GRTOS COMPLEMENTARY FUNCTIONS  ****************************
***********************************************************************************/

/**gk_TASK_STK_INIT
 *  \brief 
 *  Initializes the Task Stack acording to the TCB parameters
 *  \param [in] ptcb Pointer to the TCB of the task to initilise
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Implement validity checks
 *  \relates Task
 */
G_UINT32 gk_TASK_STK_INIT(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10009)
	G_SSTK *stk;

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    /* GRTOS_READ_SP reads the hardware stack-pointer register via a
     * Nios II/V-specific assembly macro.  No Windows equivalent exists;
     * the assertion is silently omitted on the host build. */
    #ifdef __niosX_arch__
        // not changing the current stack
        void *StackPointer=(void *)0;
        GRTOS_READ_SP(StackPointer);
        PRINT_ASSERT(((void *) ptcb->TCB_StackBottom < StackPointer || (void *) ptcb->TCB_StackTop > StackPointer),"ERROR Initiliasing the current stack, ptcb->TCB_StackBottom= %p, ptcb->TCB_StackTop= %p, StackPointer= %p \n", (void *) ptcb->TCB_StackBottom, (void *) ptcb->TCB_StackTop, (void *) StackPointer);
        StackPointer = (void *) get_pointed_field(ptcb, TCB_StackPointer);
        PRINT_ASSERT(((void *) ptcb->TCB_StackBottom >= StackPointer && (void *) ptcb->TCB_StackTop <= StackPointer),"ERROR Initiliasing the current task stack, ptcb->TCB_StackBottom= %p, ptcb->TCB_StackTop= %p, StackPointer= %p \n", (void *) ptcb->TCB_StackBottom, (void *) ptcb->TCB_StackTop, (void *) StackPointer);
    #endif
#endif

	stk = (G_SSTK *) get_pointed_field(ptcb, TCB_StackBottom) - G_STACK_STATUS_LENGTH;  /* It has to be aligned */
	set_pointed_field(ptcb, TCB_StackPointer, stk);                /* Load Stack pointer in TCB */

    /* Initialise stack frame for first call */
	stk[OFFSET_REG_0]  = 1;                                                            /* estatus         */
	stk[OFFSET_REG_1]  = (G_UINT32) gk_ENTRY_TASK_COMPLETE;                             /* register r31 ra */
	
	stk[OFFSET_REG_2]  = (G_UINT32) get_pointed_field(ptcb, TCB_TaskCode) + G_RETURN_OFFSET_FROM_INTERRUPT; /* register r29 ea */

	stk[OFFSET_REG_3]  = (G_UINT32) get_pointed_field(ptcb, TCB_StackPointer);   /* register r28 fp */
	stk[OFFSET_REG_4]  = G_INITIAL_REGISTER_VALUE;                               /* register r15    */
	stk[OFFSET_REG_5]  = G_INITIAL_REGISTER_VALUE;                               /* register r14    */
	stk[OFFSET_REG_6]  = G_INITIAL_REGISTER_VALUE;                               /* register r13    */
	stk[OFFSET_REG_7]  = G_INITIAL_REGISTER_VALUE;                               /* register r12    */
	stk[OFFSET_REG_8]  = G_INITIAL_REGISTER_VALUE;                               /* register r11    */
	stk[OFFSET_REG_9]  = G_INITIAL_REGISTER_VALUE;                               /* register r10    */
	stk[OFFSET_REG_10] = G_INITIAL_REGISTER_VALUE;                               /* register r9     */
	stk[OFFSET_REG_11] = G_INITIAL_REGISTER_VALUE;                               /* register r8     */
	stk[OFFSET_REG_12] = G_INITIAL_REGISTER_VALUE;                               /* register r7     */
	stk[OFFSET_REG_13] = G_INITIAL_REGISTER_VALUE;                               /* register r6     */
	stk[OFFSET_REG_14] = G_INITIAL_REGISTER_VALUE;                               /* register r5     */
	stk[OFFSET_REG_15] = (G_UINT32) get_pointed_field(ptcb, TCB_TaskArg);        /* register r4     */
	stk[OFFSET_REG_16] = G_INITIAL_REGISTER_VALUE;                               /* register r3     */
	stk[OFFSET_REG_17] = (G_UINT32) get_pointed_field(ptcb,TCB_TaskArg);         /* register r2     */
	stk[OFFSET_REG_18] = G_INITIAL_REGISTER_VALUE;                               /* register r1     */
	stk[OFFSET_REG_19] = G_INITIAL_REGISTER_VALUE;                               /* register r24    */
	stk[OFFSET_REG_20] = G_INITIAL_REGISTER_VALUE;                               /* register r23    */
	stk[OFFSET_REG_21] = G_INITIAL_REGISTER_VALUE;                               /* register r22    */
	stk[OFFSET_REG_22] = G_INITIAL_REGISTER_VALUE;                               /* register r21    */
	stk[OFFSET_REG_23] = G_INITIAL_REGISTER_VALUE;                               /* register r20    */
	stk[OFFSET_REG_24] = G_INITIAL_REGISTER_VALUE;                               /* register r19    */
	stk[OFFSET_REG_25] = G_INITIAL_REGISTER_VALUE;                               /* register r18    */
	stk[OFFSET_REG_26] = G_INITIAL_REGISTER_VALUE;                               /* register r17    */
	stk[OFFSET_REG_27] = G_INITIAL_REGISTER_VALUE;                               /* register r16    */

    /* Platform dispatch â€” on Windows, task context switching uses POSIX
     * ucontext_t (getcontext/makecontext).  On embedded Nios the stack frame
     * set up above is sufficient; the hardware handles context save/restore on
     * interrupt entry.  This block is compiled only for the Windows host build. */
#ifndef __niosX_arch__
    getcontext(&ptcb->uctx);
    if (ptcb->uctx_stack == NULL) {
        ptcb->uctx.uc_stack.ss_size  = (16U * 1024U);
        ptcb->uctx_stack             = malloc(ptcb->uctx.uc_stack.ss_size);
        ptcb->uctx.uc_stack.ss_sp    = ptcb->uctx_stack;
        ptcb->uctx.uc_stack.ss_flags = 0;
    }
    /* A task's uctx.fiber_handle here may belong to a different OS thread than the
     * one calling makecontext (task migrated processors between releases) -- see
     * grtos_fiber.c's makecontext() for how the stale fiber is handled. */
    makecontext(&ptcb->uctx, (void (*)()) gu_pthread, 1, ptcb);

#endif

    GK_SAMPLE_FUNCTION_END(10009)
    return(G_TRUE);
}

/**gk_TaskInheritedPriority
 *  \brief Determine the priority of a tasks according to the resources granted
 *  \param [in] ptcb Pointer to the TCB of the task
 *  \param [in] task_state Task state to determine the task priority
 *  \return Priority determined
 *  \todo Check according the type of reource
 *  \relates Task
 */
G_UINT64 gk_TaskInheritedPriority(GS_TCB *ptcb, enum tcbstate task_state)
{
    GK_SAMPLE_FUNCTION_BEGIN(10078)
	GS_ECB *pevent;
	G_UINT64 priority;

        if (ptcb->TCBRunPriority > ptcb->TCBReadyPriority) ptcb->TCBRunPriority = ptcb->TCBReadyPriority;

        if (task_state == G_TCBState_RUNNING) priority = ptcb->TCBRunPriority;
        else priority = ptcb->TCBReadyPriority;

        PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ptcb invalid= %p\n",ptcb);

        pevent = (GS_ECB *) ptcb->TCB_NextTCBAEL;
        while ((pevent != (struct gs_ecb *) 0))
        {
            if (pevent->ECBState != GS_ECBState_WAITING_TIME) {
                if (pevent->ECBValue.i64 < priority) priority = pevent->ECBValue.i64;
            }
            pevent = pevent->ECB_NextECB;
        }
    GK_SAMPLE_FUNCTION_END(10078)
	return (priority);
}

OPTIMIZE_RESTORE

