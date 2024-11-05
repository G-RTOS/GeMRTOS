/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS structure list management functions
 *  \details This file contains thefuntion for list management.
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

#include <gemrtos.h>

OPTIMEZE_CODE(0)

// https://gcc.gnu.org/onlinedocs/gcc/Nios-II-Options.html#index-mno-bypass-cache
// #pragma GCC optimize('mno-cache-volatile')



/***********************************************************************************
 * *********************************************************************************
 *                    gRTOS SUPPORT FUNCTIONS
 * *********************************************************************************
 ***********************************************************************************/

/**gk_ECBAEL_Link
 *  \brief 
 *  Links event2 to the Associated list of event1
 *  \param [in] pevent1 pointer to the ECB of event 1
 *  \param [in] pevent2 pointer to the ECB of event 2
 *  \return G_TRUE if successful
 *  \relates Event
 */ 
G_INT32 gk_ECBAEL_Link(GS_ECB * const pevent1, GS_ECB * const pevent2)
{
    GK_SAMPLE_FUNCTION_BEGIN(2)
	GS_ECB *pevent;

    PRINT_ASSERT((gkm_ECB_IsValid(pevent1) == G_TRUE),"ERROR ECB1 not valid\n");
    PRINT_ASSERT((gkm_ECB_IsValid(pevent2) == G_TRUE),"ERROR ECB2 not valid\n");
	
	if (pevent1->ECB_NextECBAEL == (struct gs_ecb *) 0) pevent1->ECB_NextECBAEL = pevent1;
	if (pevent2->ECB_NextECBAEL == (struct gs_ecb *) 0) pevent2->ECB_NextECBAEL = pevent2;
	
	pevent = pevent1->ECB_NextECBAEL;
	pevent1->ECB_NextECBAEL = pevent2->ECB_NextECBAEL;
	pevent2->ECB_NextECBAEL = (struct gs_ecb *) pevent;

    // if (pevent1->ECB_NextECBAEL == (struct gs_ecb *) 0)
    // {
    //     pevent2->ECB_NextECBAEL = (struct gs_ecb *) pevent1; 
    //     if (pevent2->ECB_NextECBAEL == (struct gs_ecb *) 0)
    //     {
    //         pevent1->ECB_NextECBAEL = (struct gs_ecb *) pevent2;
    //     }
    //     else
    //     {
    //         pevent1->ECB_NextECBAEL = pevent2->ECB_NextECBAEL;       
    //     }
    // }
    // else
    // {
    //     if (pevent2->ECB_NextECBAEL == (struct gs_ecb *) 0)
    //     {
    //         pevent2->ECB_NextECBAEL = pevent1->ECB_NextECBAEL;
    //         pevent1->ECB_NextECBAEL = (struct gs_ecb *) pevent2;
    //     }
    //     else
    //     {
    //         pevent = pevent1->ECB_NextECBAEL;
    //         pevent1->ECB_NextECBAEL = pevent2->ECB_NextECBAEL;
    //         pevent2->ECB_NextECBAEL = (struct gs_ecb *) pevent;
    //     }
    // }
    GK_SAMPLE_FUNCTION_END(2)
	return(G_TRUE);
}
 
/**gk_ECBAEL_Remove
 *  \brief 
 *  Unlinks the pevent from the associated ECB list
 *  \param [in] pevent pointer to ECB of the event
 *  \return G_TRUE if event was linked to a Associated list, G_FALSE otherwise
 *  \relates Event
 */ 
G_INT32 gk_ECBAEL_Remove(GS_ECB * const pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10029)
    GS_ECB *pevent1;
    G_INT32 retorno = G_FALSE;

    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");

	pevent1 = get_pointed_field(pevent, ECB_NextECBAEL);
    if (pevent1 != (struct gs_ecb *) 0)
    {
        // Looks the previous ECB that points to pevent
        while (get_pointed_field(pevent1, ECB_NextECBAEL) != (struct gs_ecb *) pevent)
        {
            pevent1 = (GS_ECB *) get_pointed_field(pevent1, ECB_NextECBAEL);
        }
		// there is more ECB
        if (get_pointed_field(pevent, ECB_NextECBAEL) != pevent1) 
			set_pointed_field(pevent1, ECB_NextECBAEL, get_pointed_field(pevent, ECB_NextECBAEL)); 
        else set_pointed_field(pevent1, ECB_NextECBAEL, (struct gs_ecb *) 0);
		
        set_pointed_field(pevent, ECB_NextECBAEL, (struct gs_ecb *) 0);
        retorno = G_TRUE;
    }
    GK_SAMPLE_FUNCTION_END(10029)
	return(retorno);
}

/**gk_ECBASL_Link
 *  \brief 
 *  Links a SCB to the ECB Associated Signal List
 *  \param [in] pevent  Pointer to the ECB of the event
 *  \param [in] psignal Pointer to the SCB of the signal
 *  \relates Signal
 */ 
G_INT32 gk_ECBASL_Link(GS_ECB * const pevent, GS_SCB * const psignal)
{
	set_pointed_field(psignal,SCBState, G_SCBState_IN_ECB);
	// return gk_SCB_Link(&pevent->ECB_NextECBASL, psignal, G_SCBState_IN_ECB, offsetof(GS_SCB, SCBType));
	G_INT32 retorno = gk_NODE_Link((void **) &pevent->ECB_NextECBASL,  // root to the ready list
		                           (void *) psignal, ECBASL); 
								   
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR pevent not valid\n");
	PRINT_ASSERT((gkm_SCB_IsValid(pevent->ECB_NextECBASL) == G_TRUE),"ERROR pevent->ECB_NextECBASL not valid\n");
	PRINT_ASSERT((pevent->ECB_NextECBASL->SCB_AssocXCB == (void *) pevent),"ERROR pevent->ECB_NextECBASL->SCB_AssocXCB not valid\n");
#endif
	return (retorno);
}


/**gk_ECBASL_Unlink
 *  \brief 
 *  Unlinks a signal SCB from the ECB Associated Signal List of and event
 *  \param [in] pevent  Pointer to the ECB of the event
 *  \param [in] psignal Pointer to the SCB of the signal
 *  \return G_TRUE if successful, G_FALSE if signal is not associated with event
 *  \relates Event
 *  \sa Signal
 */
G_INT32  gk_ECBASL_Unlink(GS_ECB *pevent, GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(5)
	G_INT32 retorno = G_FALSE;

// CHECKING #############################################
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR pevent not valid\n");
	PRINT_ASSERT((gkm_SCB_IsValid(pevent->ECB_NextECBASL) == G_TRUE),"ERROR psignal not valid\n");
	PRINT_ASSERT((gkm_SCB_IsValid(psignal) == G_TRUE),"ERROR psignal not valid\n");

	// previous
	G_INT32 antecedent = (psignal->SCB_NextSCB != (GS_SCB *) 0);
	if (!((!antecedent) || ((gkm_SCB_IsValid(psignal->SCB_NextSCB) == G_TRUE)))) {printf("Invalid psignal->SCB_NextSCB"); while(1); return (G_FALSE);}
#endif // CHECKING #############################################

	// 19-06-2024 retorno = gk_SCB_SCBL_Unlink((GS_SCB **) &pevent->ECB_NextECBASL, psignal);
	retorno = gk_NODE_SINGLE_Unlink((void **) &pevent->ECB_NextECBASL,        // root to the ready list
	                                (void *) psignal,                         // node to link
									(G_INT32)  offsetof(GS_SCB,SCB_NextSCB)); // next field offset
	if (retorno == G_TRUE) {
		psignal->SCBState = G_SCBState_UNLINKED;
	}
	
    GK_SAMPLE_FUNCTION_END(5)
    return(G_TRUE);
}



/**gk_ECB_List_Unlink
 *  \brief 
 *  Unlinks all the structures linked to a ECB
 *  \param [in] pevent Pointer to the ECB
 *  \return G_TRUE
 *  \todo Verify when a resource is unlinked (semaphore and queue message)
 *  \relates Event
 */ 
G_INT32 gk_ECB_List_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10030)
	GS_SCB *psignal1;
	
	// GS_ECB  Event Control Block structure
	// typedef struct gs_ecb {
	//	 G_INT32         ECBState;          /** Granted, Waiting, Free                                 */
	//	 G_INT32         ECBType;           /** Type of event control block (see OS_EVENT_TYPE_xxxx)   */
	//	 TIMEPRIORITY   ECBValue;          /** Occurrence Time of the event or Priority               */
	//	# struct gs_ecb  *ECB_NextECB;      /** Pointer to linked list of waiting events of this event */
	//	# struct gs_ecb  *ECB_PrevECB;      /** Pointer to linked list of waiting events of this event */
	//	 struct gs_tcb  *ECB_AssocTCB;      /** Pointer to the task associated with the event         */
	//	 struct g_rcb   *ECB_AssocRCB;      /** Pointer to the resource associated with the event     */
	//	# struct gs_ecb  *ECB_NextTCBAEL;   /** Pointer to the next event of the same task             */
	//	# struct gs_ecb  *ECB_PrevTCBAEL;   /** Pointer to the previous event of the same task         */
	//	# struct gs_ecb  *ECB_NextECBAEL;   /** Pointer to the event associated with this (ie timeout) */
	//  # struct gs_scb  *ECB_NextECBASL;   /** Pointer to the Linked list of signals                  */
	//	 struct gs_rrds *ECB_RRDS;         /** Pointer to the resource request structure or MCB       */
	// } GS_ECB;
	
	// Remove ECB from waiting list ECB_NextECB and ECB_PrevECB pointed in timed or resource list
	// according the pevent->ECBState field
    // PRINT_ASSERT((pevent->ECBState != GS_ECBState_UNLINKED),"ERROR RCB is not state\n");
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
	
    // Remove from the task list
	if (pevent->ECB_AssocTCB != (GS_TCB *)0) gk_TCBAEL_Unlink(pevent);
    
    // Remove from the ECB Associated list
	gk_ECBAEL_Remove(pevent);
	
	// Remove SCB from ASL
	while (get_pointed_field(pevent, ECB_NextECBASL) != (struct gs_scb *) 0)
	{
		psignal1 = get_pointed_field(pevent, ECB_NextECBASL);
		gk_ECBASL_Unlink(pevent, psignal1);
		gk_SCBFL_Link(psignal1);
	}	

    // Remove RRDS
    struct gs_rrds *prrds;
    struct gs_rrds *prrds1;
    prrds = (struct gs_rrds *) get_pointed_field(pevent, ECB_RRDS);
    set_pointed_field(pevent, ECB_RRDS, (struct gs_rrds *) 0);
    while (prrds != (struct gs_rrds *) 0) {
        prrds1 = (struct gs_rrds *) get_pointed_field(prrds, RRDS_NextRRDS);
        gk_RRDSFL_Link(prrds);
        prrds = prrds1;
    }
    GK_SAMPLE_FUNCTION_END(10030)
	
    return(G_TRUE);
}

/**gk_ECBFL_Link
 *  \brief 
 *  Link ECB to Free List. Removes the signals from ECB
 *  \param [in] pevent Pointer to the ECB to link
 *  \return G_TRUE
 *  \relates Event
 */ 
G_INT32 gk_ECBFL_Link(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10031)
    // Unlink the ECB
    gk_ECB_List_Unlink(pevent);
    
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");
    PRINT_ASSERT((pevent->ECBState == GS_ECBState_UNLINKED),"ERROR ECBState= %d\n", (int) pevent->ECBState);

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
 *  \relates Event
 */  
G_INT32  gk_ECBTL_Link (GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10001)
	// 02-07-2024 register GS_ECB *pevent2;
    
    PRINT_ASSERT((gkm_ECB_IsValid(g_kcb.KCB_NextECBTL) == G_TRUE),"ERROR in root time list\n");
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR in pevent\n");
    PRINT_ASSERT((pevent->ECBState == GS_ECBState_UNLINKED),"ERROR ECBState= %d\n", (int) pevent->ECBState);

	// 19-06-2024 gk_ECB_ECBL_Link((GS_ECB **) &g_kcb.KCB_NextECBTL, pevent);
	
	gk_NODE_Link((void **) &g_kcb.KCB_NextECBTL,         // root to the ready list
				 (void *) pevent, ECBTL);                         // ascending order sorting
	
	if (g_kcb.KCB_NextECBTL == pevent) GRTOS_CMD_NXT_OCC_TM_EVN_SET(pevent->ECBValue.i64);
	
    /* Change Status of the Event */
    set_pointed_field(pevent, ECBState, GS_ECBState_WAITING_TIME);

// CHECKING #############################################
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	
    PRINT_ASSERT((gkm_ECB_IsValid(g_kcb.KCB_NextECBTL) == G_TRUE),"ERROR in root time list\n");
	
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR in pevent\n");
	
	// previous
	G_INT32 antecedent = (pevent->ECB_PrevECB != (GS_ECB *) 0);
	if (!((!antecedent) || ((pevent->ECB_PrevECB->ECBValue.i64 <= pevent->ECBValue.i64)))) {printf("Invalid LCB root LCB"); while(1); return (G_FALSE);}
	
	PRINT_ASSERT((pevent->ECB_NextECB != (GS_ECB *) 0),"ERROR in root time list\n");
	
	// next
	// antecedent = (pevent->ECB_NextECB != (GS_ECB *) 0);
	if (!(pevent->ECB_NextECB->ECBValue.i64 >= pevent->ECBValue.i64)) {printf("Invalid time list"); while(1); return (G_FALSE);}
	
#endif // CHECKING #############################################

	// 11-06-2024 pevent2 = g_kcb.KCB_NextECBTL;
    // 11-06-2024 if (pevent2->ECBValue.i64 >pevent->ECBValue.i64)
	// 11-06-2024 	{/* Event inserted is the next occurence   */
	// 11-06-2024 	set_pointed_field(pevent, ECB_NextECB, pevent2);
	// 11-06-2024 	set_pointed_field(pevent, ECB_PrevECB, (struct gs_ecb *) 0);
	// 11-06-2024 	set_pointed_field(pevent2, ECB_PrevECB, (struct gs_ecb *) pevent);
	// 11-06-2024 	g_kcb.KCB_NextECBTL = (struct gs_ecb *) pevent;
	// 11-06-2024 	GRTOS_CMD_NXT_OCC_TM_EVN_SET(pevent->ECBValue.i64);
	// 11-06-2024 } else {
	// 11-06-2024 	/* Recorro la lista para buscar el punto de insercion del evento */
    // 11-06-2024     /* siempre habra un ultimo elemento */
	// 11-06-2024 	while (pevent2->ECBValue.i64 <= pevent->ECBValue.i64){
	// 11-06-2024 		pevent2 = get_pointed_field(pevent2, ECB_NextECB);
	// 11-06-2024 	}        
	// 11-06-2024 	set_pointed_field(pevent, ECB_NextECB, (struct gs_ecb *) pevent2); /* Insert the task in the linked list */
	// 11-06-2024 	set_pointed_field(pevent, ECB_PrevECB, pevent2->ECB_PrevECB);
	// 11-06-2024 	set_pointed_field(pevent2->ECB_PrevECB, ECB_NextECB, (struct gs_ecb *) pevent);
	// 11-06-2024 	set_pointed_field(pevent2, ECB_PrevECB, (struct gs_ecb *) pevent);
	// 11-06-2024 }
	

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
 *  \relates Event
 */
G_INT32 gk_ECBTL_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10019)
	
	// 02-07-2024 GS_ECB *pevent1 = g_kcb.KCB_NextECBTL;
    
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");
    PRINT_ASSERT((pevent->ECBState == GS_ECBState_WAITING_TIME),"ERROR ECBState= %d\n", (int) pevent->ECBState);    

	// 19-06-2024 gk_ECB_ECBL_Unlink((GS_ECB **) &g_kcb.KCB_NextECBTL, pevent);
	
	gk_NODE_DUAL_Unlink((void **) &g_kcb.KCB_NextECBTL,            // root to the ready list
	                    (void *)  pevent,                          // node to unlink
						(G_INT32) offsetof(GS_ECB,ECB_NextECB),    // next field offset
						(G_INT32) offsetof(GS_ECB,ECB_PrevECB));   // previous field offset
						
	pevent->ECBState = GS_ECBState_UNLINKED;
	
	GRTOS_CMD_NXT_OCC_TM_EVN_SET(g_kcb.KCB_NextECBTL->ECBValue.i64);

// CHECKING #############################################
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	
    PRINT_ASSERT((gkm_ECB_IsValid(g_kcb.KCB_NextECBTL) == G_TRUE),"ERROR in root time list\n");
	
	PRINT_ASSERT((g_kcb.KCB_NextECBTL != (GS_ECB *) 0),"ERROR in root time list\n");
	PRINT_ASSERT((g_kcb.KCB_NextECBTL->ECB_PrevECB == (GS_ECB *) 0),"ERROR in root time list\n");
	
	PRINT_ASSERT((pevent->ECB_NextECB == (GS_ECB *) 0),"ERROR in root time list\n");
	PRINT_ASSERT((pevent->ECB_PrevECB == (GS_ECB *) 0),"ERROR in root time list\n");
	
	// next
	G_INT32 antecedent = (g_kcb.KCB_NextECBTL->ECB_NextECB != (GS_ECB *) 0);
	if (!(!(antecedent) || (g_kcb.KCB_NextECBTL->ECB_NextECB->ECBValue.i64 >= g_kcb.KCB_NextECBTL->ECBValue.i64))) {printf("Invalid time list"); while(1); return (G_FALSE);}
	
#endif // CHECKING #############################################

	/* Remove from the time linked list */
	// 12-06-2024 if (pevent1 == (struct gs_ecb *) pevent)
	// 12-06-2024 {
	// 12-06-2024 	g_kcb.KCB_NextECBTL = get_pointed_field(pevent, ECB_NextECB);   
	// 12-06-2024 	pevent1 = g_kcb.KCB_NextECBTL;
	// 12-06-2024 	set_pointed_field(pevent1, ECB_PrevECB, (struct gs_ecb *) 0);
	// 12-06-2024 	GRTOS_CMD_NXT_OCC_TM_EVN_SET(pevent1->ECBValue.i64);
	// 12-06-2024 } else
	// 12-06-2024 {
    // 12-06-2024     set_pointed_field(get_pointed_field(pevent, ECB_PrevECB), ECB_NextECB, get_pointed_field(pevent, ECB_NextECB));
    // 12-06-2024     set_pointed_field(get_pointed_field(pevent, ECB_NextECB), ECB_PrevECB, get_pointed_field(pevent, ECB_PrevECB));
	// 12-06-2024 }
	// 12-06-2024 /* Set the pointer to unlinked */
	// 12-06-2024 set_pointed_field(pevent, ECB_PrevECB, (struct gs_ecb *) 0);
	// 12-06-2024 set_pointed_field(pevent, ECB_NextECB, (struct gs_ecb *) 0);
	// 12-06-2024 set_pointed_field(pevent, ECBState, GS_ECBState_UNLINKED);
	
    GK_SAMPLE_FUNCTION_END(10019)
    return(G_TRUE);
}

/**gk_KCBASL_Link
 *  \brief 
 *  Links a SCG to the Associated Signal list of KCB
 *  \param [in] psignal Pointer to the SCB of the signal to link
 *  \return G_TRUE if successful
 *  \relates Signal
 */
G_INT32  gk_KCBASL_Link(GS_SCB *psignal)
{
	set_pointed_field(psignal,SCBState, G_SCBState_IN_KCB);
	return	(gk_NODE_Link((void **) &g_kcb.KCB_NextKCBASL,  (void *) psignal, KCBASL)); 
}


/**gk_KCBASL_Unlink
 *  \brief 
 *  Unlinks a SCG from the Associated Signal list of KCB
 *  \param [in] psignal Pointer to the SCB of the signal to unlink
 *  \return G_TRUE if successful, G_FALSE if not such signal linked to KCBASL
 *  \relates Signal
 *  \sa Kernel
 */ 
G_INT32  gk_KCBASL_Unlink(GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(11)
	G_INT32 retorno = G_FALSE;
	// GS_SCB *psignal2;

	// 19-06-2024 retorno = gk_SCB_SCBL_Unlink((GS_SCB **) &g_kcb.KCB_NextKCBASL, psignal);

	retorno = gk_NODE_SINGLE_Unlink((void **) &g_kcb.KCB_NextKCBASL,          // root to the ready list
	                                (void *) psignal,                         // node to link
									(G_INT32)  offsetof(GS_SCB,SCB_NextSCB)); // next field offset
	if (retorno == G_TRUE) {
		psignal->SCBState = G_SCBState_UNLINKED;
	}	
	
    GK_SAMPLE_FUNCTION_END(11)
    return(retorno);
}

// 15-10-2024 #######################################

/**gk_KCBPSL_Link
 *  \brief 
 *  Link a SCB to KCB Pending Signal List
 *  \param [in] psignal Pointer to the SCB
 *  \return G_TRUE
 *  \relates Task 
 *  \sa Signal
 */ 
G_INT32  gk_KCBPSL_Link(GS_SCB *psignal)
{
	set_pointed_field(psignal,SCBState, G_SCBState_PENDING);
	return	(gk_NODE_Link((void **) &g_kcb.KCB_NextKCBPSL, (void *) psignal, KCBPSL));
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
G_INT32  gk_KCBPSL_Unlink(GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10032)

	G_INT32 retorno = G_FALSE;
    
    PRINT_ASSERT((psignal->SCBState != G_SCBState_UNLINKED),"ERROR SCBState= %d\n",(int) psignal->SCBState);
		
	retorno = gk_NODE_SINGLE_Unlink((void **) &g_kcb.KCB_NextKCBPSL,          // root to the ready list
	                                (void *) psignal,                         // node to link
									(G_INT32)  offsetof(GS_SCB,SCB_NextSCB)); // next field offset
	if (retorno == G_TRUE) {
		psignal->SCBState = G_SCBState_UNLINKED;
	}		
		
    PRINT_ASSERT((psignal->SCBState == G_SCBState_UNLINKED),"ERROR SCBState= %d\n",(int) psignal->SCBState);  
    
    GK_SAMPLE_FUNCTION_END(10032)
    return (retorno);
}
// 15-10-2024 #######################################


/**gk_LCBL_Link
 *  \brief 
 *  Inserts a RunList in LCB Runining list ordered by background list and then priority
 *  \param [in] plcb Pointer to the LCB list
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Check everything about this function
 *  \todo Define the G_FALSE return
 *  \relates Scheduling_List
 */
G_INT32 gk_LCBL_Link(GS_LCB *plcb)
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
 
 
/**gk_LCBL_UnLink
 *  \brief 
 *  Unlinks the LCB from the LCB List
 *  \param [in] plcb Pointer to the LCB to unlink
 *  \return G_TRUE if successful, G_FALSE otherwise 
 *  \todo Implement G_FALSE returns
 *  \relates Scheduling_List
 */ 
G_INT32 gk_LCBL_UnLink(GS_LCB *plcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(13) 

    PRINT_ASSERT((gkm_LCB_IsValid(plcb) == G_TRUE),"ERROR PCB not valid\n");
    PRINT_ASSERT(((gkm_LCB_IsValid(plcb->LCB_NextLCBL) == G_TRUE) || (plcb->LCB_NextLCBL == (struct gs_lcb *) 0)),"ERROR PCB not valid\n");
    PRINT_ASSERT(((gkm_LCB_IsValid(plcb->LCB_PrevLCBL) == G_TRUE) || (plcb->LCB_PrevLCBL == (struct gs_lcb *) 0)),"ERROR PCB not valid\n");    
    PRINT_ASSERT((gkm_LCB_IsValid(g_kcb.KCB_NextLCBL) == G_TRUE),"ERROR PCB not valid\n");
    PRINT_ASSERT((plcb->LCBState == GS_LCBState_LINKED), "ERROR LCB is not unlinked= %d\n", (int) plcb->LCBState);

	// Check if it is the first element in the list
	// 19-06-2024 if (g_kcb.KCB_NextLCBL == plcb)	{/* Remove as first element  */
	// 19-06-2024 	g_kcb.KCB_NextLCBL = plcb->LCB_NextLCBL;
    // 19-06-2024     PRINT_ASSERT((gkm_LCB_IsValid(g_kcb.KCB_NextLCBL) == G_TRUE),"ERROR PCB not valid\n");
	// 19-06-2024 	if (g_kcb.KCB_NextLCBL != (struct gs_lcb *) 0) g_kcb.KCB_NextLCBL->LCB_PrevLCBL = (struct gs_lcb *) 0;
	// 19-06-2024 } 
    // 19-06-2024 else {/* It is not the firstLCB in the list */
	// 19-06-2024 	plcb->LCB_PrevLCBL->LCB_NextLCBL = plcb->LCB_NextLCBL;
	// 19-06-2024 	if (plcb->LCB_NextLCBL != (struct gs_lcb *) 0) plcb->LCB_NextLCBL->LCB_PrevLCBL = plcb->LCB_PrevLCBL;
	// 19-06-2024 }
	
	gk_NODE_DUAL_Unlink((void **) &g_kcb.KCB_NextLCBL,              // root to the ready list
	                    (void *)  plcb,                             // node to unlink
						(G_INT32) offsetof(GS_LCB,LCB_NextLCBL),    // next field offset
						(G_INT32) offsetof(GS_LCB,LCB_PrevLCBL));   // previous field offset
	plcb->LCBState = GS_LCBState_UNLINKED;
    
    GK_SAMPLE_FUNCTION_END(13) 
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
G_INT32 gk_LCBFPL_Link(G_INT32 processorID)
{
    GK_SAMPLE_FUNCTION_BEGIN(10007)

	GS_PCB *ppcb = (GS_PCB *) &(g_kcb.G_PCBTbl[processorID-1]);
	GS_PCBAssocLCB *pcb_assoclcb = get_pointed_field( ppcb , PCB_AssocLCB);
    GS_LCB *plcb = get_pointed_field(pcb_assoclcb, PCB_RDY_LCBL);

    PRINT_ASSERT((processorID >= 1 && processorID <= G_NUMBER_OF_PCB),"ERROR processorID= %d\n", (int) processorID);
    PRINT_ASSERT((g_kcb.G_PCBTbl[processorID-1].PCBState != GS_PCBState_FREE),"ERROR ppcb->PCBState= %d\n", (int) g_kcb.G_PCBTbl[processorID-1].PCBState);
    PRINT_ASSERT((g_kcb.G_PCBTbl[processorID-1].PCB_NextPCB == (struct gs_pcb *) 0), "ERROR ppcb->PCB_NextPCB\n");
    PRINT_ASSERT((g_kcb.G_PCBTbl[processorID-1].PCB_PrevPCB == (struct gs_pcb *) 0), "ERROR ppcb->PCB_PrevPCB\n");


    if (get_pointed_field(plcb, LCB_NextLCBFPL) != (struct gs_pcb *) 0) {
        set_pointed_field(get_pointed_field(plcb, LCB_NextLCBFPL), PCB_PrevPCB, ppcb); 
    }
    
	set_pointed_field(ppcb, PCB_PrevPCB, (struct gs_pcb *) 0);
	set_pointed_field(ppcb, PCB_NextPCB, get_pointed_field(plcb, LCB_NextLCBFPL));
    set_pointed_field(plcb, LCB_NextLCBFPL, (struct gs_pcb *) ppcb);

	g_kcb.G_PCBTbl[processorID-1].PCBState = GS_PCBState_FREE;

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
G_INT32 gk_LCBFPL_Unlink(G_INT32 processorID)
{
    GK_SAMPLE_FUNCTION_BEGIN(10008)
	
	GS_PCB *ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[processorID-1];
	GS_PCBAssocLCB *pcb_assoclcb = get_pointed_field( ppcb , PCB_AssocLCB);
    GS_LCB *plcb = get_pointed_field(pcb_assoclcb, PCB_RDY_LCBL);  // This is the foreground list

    PRINT_ASSERT((processorID >= 1 && processorID <= G_NUMBER_OF_PCB),"ERROR processorID= %d\n", (int) processorID);
    PRINT_ASSERT((g_kcb.G_PCBTbl[processorID-1].PCBState == GS_PCBState_FREE),"ERROR ppcb->PCBState= %d\n", (int) g_kcb.G_PCBTbl[processorID-1].PCBState);


    // 19-06-2024 if (get_pointed_field(plcb, LCB_NextLCBFPL) == ppcb) { /* It is the first element */
    // 19-06-2024     set_pointed_field(plcb, LCB_NextLCBFPL, get_pointed_field(ppcb, PCB_NextPCB));
    // 19-06-2024     if (get_pointed_field(plcb, LCB_NextLCBFPL) != (struct gs_pcb *) 0) set_pointed_field(plcb->LCB_NextLCBFPL, PCB_PrevPCB, (struct gs_pcb *) 0);
    // 19-06-2024 }
    // 19-06-2024 else 
    // 19-06-2024 {
    // 19-06-2024     set_pointed_field(ppcb->PCB_PrevPCB, PCB_NextPCB, get_pointed_field(ppcb, PCB_NextPCB));
    // 19-06-2024     if (get_pointed_field(ppcb, PCB_NextPCB) != (struct gs_pcb *) 0) set_pointed_field(ppcb->PCB_NextPCB,PCB_PrevPCB, get_pointed_field(ppcb, PCB_PrevPCB));
    // 19-06-2024 }
	
	gk_NODE_DUAL_Unlink((void **) &plcb->LCB_NextLCBFPL,              // root to the ready list
	                    (void *)  ppcb,                              // node to unlink
						(G_INT32) offsetof(GS_PCB,PCB_NextPCB),      // next field offset
						(G_INT32) offsetof(GS_PCB,PCB_PrevPCB));     // previous field offset	
						
	set_pointed_field(ppcb, PCBState, GS_PCBState_RUNNING);
    // 19-06-2024 set_pointed_field(ppcb, PCB_NextPCB, (struct gs_pcb *) 0);
    // 19-06-2024 set_pointed_field(ppcb, PCB_PrevPCB, (struct gs_pcb *) 0);
    
    
    GK_SAMPLE_FUNCTION_END(10008)
    return(G_TRUE);
}

/**gk_RCBASL_Link
 *  \brief 
 *  Links a signal SCB to the resource RCB Associated Signal List
 *  \param [in] presource Pointer to the RCB of the resource
 *  \param [in] psignal   Pointer to the SCB of the signal
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo
 *  \relates Signal
 *  \sa Resource
 */ 
G_INT32 gk_RCBASL_Link(G_RCB *presource, GS_SCB *psignal)
{
	set_pointed_field(psignal,SCBState, G_SCBState_IN_RCB);
	// 19-06-2024 return gk_SCB_Link(&presource->RCB_NextRCBASL, psignal, G_SCBState_IN_RCB, offsetof(GS_SCB, SCBType));
	return	(gk_NODE_Link((void **) &presource->RCB_NextRCBASL,  // root to the ready list
		                 (void *) psignal, RCBASL));                               // ascending order sorting
}


/**gk_RCBASL_Unlink
 *  \brief 
 *  Unlinks a SCB of a signal from the RCB Associated Signal List of a resource
 *  \param [in] presource Pointer to the RCB of the resource
 *  \param [in] psignal   Pointer to the SCB of the signal
 *  \return G_TRUE is successfull, G_FALSE otherwise 
 *  \todo Return G_FALSE when it is not linked
 *  \relates Signal
 *  \sa Resource
 */ 
G_INT32 gk_RCBASL_Unlink(G_RCB *presource, GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(17)
	G_INT32 retorno = G_FALSE;
	// 02-07-2024 GS_SCB *psignal1;

    PRINT_ASSERT((psignal->SCBState != G_SCBState_UNLINKED),"ERROR SCBState= %d\n", (int) psignal->SCBState);

	// 12-06-2024 if (presource->RCB_NextRCBASL != (struct gs_scb *) 0)
	// 12-06-2024 {
		
	// 19-06-2024 retorno = gk_SCB_SCBL_Unlink((GS_SCB **) &presource->RCB_NextRCBASL, psignal);
		
	retorno = gk_NODE_SINGLE_Unlink((void **) &presource->RCB_NextRCBASL,     // root to the ready list
	                                (void *) psignal,                         // node to link
									(G_INT32)  offsetof(GS_SCB,SCB_NextSCB)); // next field offset
	if (retorno == G_TRUE) {
		psignal->SCBState = G_SCBState_UNLINKED;
	}		
		
    PRINT_ASSERT((psignal->SCBState == G_SCBState_UNLINKED),"ERROR SCBState= %d\n", (int) psignal->SCBState);

    GK_SAMPLE_FUNCTION_END(17)
    return(retorno);
}

/**gk_RCBFL_Link
 *  \brief 
 *  Links the RCB of a resource to Free List
 *  \param [in] presource Pointer to the RCB of the resource
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Return G_FALSE when it is not unused
 *  \relates Resource
 */
G_INT32 gk_RCBFL_Link(G_RCB *presource)
{
    GK_SAMPLE_FUNCTION_BEGIN(18)
    
    PRINT_ASSERT((presource->RCBType == GK_RCBType_FREE),"ERROR RCBType= %d\n", (int) presource->RCBType);    

    // RCBs linked list for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    if (g_kcb.KCB_ROOT_RCBs != (struct g_rcb *) presource) presource->RCB_PREV_RCBs->RCB_NEXT_RCBs = presource->RCB_NEXT_RCBs;
    else g_kcb.KCB_ROOT_RCBs = (struct g_rcb *) presource->RCB_NEXT_RCBs;
    if (presource->RCB_NEXT_RCBs != (G_RCB *) 0) presource->RCB_NEXT_RCBs->RCB_PREV_RCBs = presource->RCB_PREV_RCBs; 
#endif

    // Does not free the structure to avoid fragmentation
    presource->RCB_NextRCB = g_kcb.KCB_FREE_RCBs;
    g_kcb.KCB_FREE_RCBs = (struct g_rcb *) presource;    
    
    GK_SAMPLE_FUNCTION_END(18)
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
G_INT32 gk_TASK_RELEASE(GS_TCB *ptcb) 
{
    GK_SAMPLE_FUNCTION_BEGIN(10039)
	
    // Preserve the Associated Signal list
    GS_SCB *psignal = get_pointed_field(ptcb, TCB_NextTCBASL);
    set_pointed_field(ptcb, TCB_NextTCBASL, (struct gs_scb *) 0);
	
	// Preserve the currente state of task
    unsigned int state = get_pointed_field(ptcb, TCBState);                

    gk_TCB_Unlink(ptcb);                                // Unlink task TCB

    gk_TCB_List_Unlink(ptcb);                           // Remove all the lists from the TCB
    gk_TASK_STK_Init(ptcb);                             // Initiates the stack of the task
	set_pointed_field(ptcb, TCB_MTX_NESTED, 0);	
	
	// Put back the Associated Signal list 4
    set_pointed_field(ptcb, TCB_NextTCBASL, (struct gs_scb *) psignal);   


    // Check if there is G_SCBType_TCB_ABORTED signal
    if ((state == G_TCBState_WAITING) || (state == G_TCBState_RUNNING) || (state == G_TCBState_READY)) {  // It is waiting not completed
        psignal = gk_TCBASL_GetSCB(ptcb, G_SCBType_TCB_ABORTED); 
        if (psignal != (GS_SCB *) 0) {
            psignal = gk_SCB_Copy((GS_SCB *) psignal); 
            gk_TCBPSL_Link(ptcb, (GS_SCB *) psignal); 
        }
		// Restore exclusion if it was reduce for this task
		if (get_pointed_field(ptcb, TCB_PrevExclusion) != 0) {
			ptcb->TCB_RDY_LCB_Index->LCBExclusion = get_pointed_field(ptcb, TCB_PrevExclusion);
			set_pointed_field(ptcb, TCB_PrevExclusion, 0);
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
 *  \relates Resource
 */
GS_ECB *gk_RCBGEL_Link(G_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent)
{
	
    GK_SAMPLE_FUNCTION_BEGIN(10025)
	// 02-07-2024 GS_ECB *pevent1;

	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ECB not valid\n");
	
	if (pevent == (struct gs_ecb *) 0) pevent = gk_ECB_GetFree();

    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");

	// Set the priority of the event
	set_pointed_field(pevent, ECBValue.i64, ptcb->TCBRunPriority);
	
	gk_NODE_Link((void **) &presource->RCB_NextRCBGEL,         // root to the ready list
				 (void *) pevent, RCBGEL);                                // ascending order sorting	
	
	
	// pevent->ECBState = ECBState;
	set_pointed_field(pevent, ECB_AssocRCB, (struct g_rcb *) presource);
	set_pointed_field(pevent, ECBState, GS_ECBState_GRANTED_RESOURCE);
	
    GK_SAMPLE_FUNCTION_END(10025)
	
    return((GS_ECB *)pevent);
}

/**gk_RCBGEL_Unlink
 *  \brief 
 *  Unlinks ECB from RCB granted list
 *  \param [in] pevent Pointer to the ECB ef the event
 *  \relates Event
 *
 */ 
void  gk_RCBGEL_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10020)

	G_RCB *presource = (G_RCB *) get_pointed_field(pevent, ECB_AssocRCB);

    PRINT_ASSERT((pevent->ECBState == GS_ECBState_GRANTED_RESOURCE),"ERROR ECBState= %d \n",(int) pevent->ECBState);
    PRINT_ASSERT((gkm_ECB_IsValid(presource->RCB_NextRCBGEL) == G_TRUE),"ERROR RCB_NextRCBGEL not valid\n");

	/* Remove from the resource linked list */
	// 19-06-2024 gk_ECB_ECBL_Unlink((GS_ECB **) &presource->RCB_NextRCBGEL, pevent);
	
	gk_NODE_DUAL_Unlink((void **) &presource->RCB_NextRCBGEL,      // root to the ready list
	                    (void *)  pevent,                          // node to unlink
						(G_INT32) offsetof(GS_ECB,ECB_NextECB),    // next field offset
						(G_INT32) offsetof(GS_ECB,ECB_PrevECB));   // previous field offset
	pevent->ECBState = GS_ECBState_UNLINKED;
	
	
	// 12-06-2024 if (get_pointed_field(presource, RCB_NextRCBGEL) == pevent) {
	// 12-06-2024 	set_pointed_field(presource, RCB_NextRCBGEL, pevent->ECB_NextECB);
	// 12-06-2024 } else {
    // 12-06-2024     PRINT_ASSERT((pevent->ECB_PrevECB != (struct gs_ecb *) 0),"ERROR in gk_RCBGEL_Unlink");
    // 12-06-2024     set_pointed_field(get_pointed_field(pevent, ECB_PrevECB), ECB_NextECB, get_pointed_field(pevent, ECB_NextECB));
	// 12-06-2024 }
    // 12-06-2024 if (get_pointed_field(pevent, ECB_NextECB) != (struct gs_ecb *) 0) set_pointed_field(get_pointed_field(pevent, ECB_NextECB), ECB_PrevECB, pevent->ECB_PrevECB);
	// 12-06-2024 
	// 12-06-2024 /* Set the pointer to unlinked */
	// 12-06-2024 set_pointed_field(pevent, ECBState, GS_ECBState_UNLINKED);
	// 12-06-2024 set_pointed_field(pevent, ECB_PrevECB, (struct gs_ecb *) 0);
	// 12-06-2024 set_pointed_field(pevent, ECB_NextECB, (struct gs_ecb *) 0);
	
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
 *  \relates Resource
 *  \sa Event
 */
GS_ECB *gk_RCBWEL_Link(G_RCB *presource, GS_TCB *ptcb, GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10000)
	// 02-07-2024 register GS_ECB *pevent1;

	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ECB not valid\n");

	// Get event if it is NULL
	if (pevent == (struct gs_ecb *) 0)	pevent = gk_ECB_GetFree();
	
	PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR ECB not valid\n");

	// Set the priority of the event
	// 19-06-2024 set_pointed_field(pevent, ECBValue.i64, ptcb->TCBCurrentPriority);
	set_pointed_field(pevent, ECBValue.i64, ptcb->TCBReadyPriority);
	
	// 11-06-2024 if (get_pointed_field(pevent, ECB_RRDS) != (struct gs_rrds *) 0) set_pointed_field(pevent, ECBValue.i64, pevent->ECB_RRDS->RRDSWaitingPriority.i64);
	// 11-06-2024 else set_pointed_field(pevent, ECBValue.i64, presource->RCBPriority.i64);

	/* Get the first event */
	// 11-06-2024 pevent1 = get_pointed_field(presource, RCB_NextRCBWEL);

	// 19-06-2024 gk_ECB_ECBL_Link((GS_ECB **) &presource->RCB_NextRCBWEL, pevent);
	
	gk_NODE_Link((void **) &presource->RCB_NextRCBWEL,   // root to the ready list
				 (void *) pevent, RCBWEL);                                // ascending order sorting		
	
	// 12-06-2024 pevent1 = *root;
	// 12-06-2024 if ((pevent1 == (struct gs_ecb *) 0) || (pevent1->ECBValue.i64 > pevent->ECBValue.i64))
	// 12-06-2024 {
	// 12-06-2024     pevent->ECB_NextECB = (struct gs_ecb *) presource->RCB_NextRCBWEL;
	// 12-06-2024     pevent->ECB_PrevECB = (struct gs_ecb *) 0;
	// 12-06-2024 	if (pevent1 != (struct gs_ecb *) 0) pevent1->ECB_PrevECB = (struct gs_ecb *) pevent;
	// 12-06-2024     *root = (struct gs_ecb *) pevent;
	// 12-06-2024 }
	// 12-06-2024 else
	// 12-06-2024 {
	// 12-06-2024 	while ((pevent1->ECB_NextECB != (struct gs_ecb *) 0) && (pevent1->ECB_NextECB->ECBValue.i64 <= pevent->ECBValue.i64)) {
	// 12-06-2024 		pevent1 = pevent1->ECB_NextECB;
	// 12-06-2024 	}
	// 12-06-2024 	pevent->ECB_NextECB = pevent1->ECB_NextECB;
	// 12-06-2024 	pevent->ECB_PrevECB = pevent1;
	// 12-06-2024 	if (pevent1->ECB_NextECB != (struct gs_ecb *) 0) pevent1->ECB_NextECB->ECB_PrevECB = pevent;
	// 12-06-2024 	pevent1->ECB_NextECB = pevent;
	// 12-06-2024 }
	
	
	pevent->ECBState = GS_ECBState_WAITING_RESOURCE;
	pevent->ECB_AssocRCB = (struct g_rcb *) presource;
    GK_SAMPLE_FUNCTION_END(10000)
    return(pevent);
}

/**gk_RCBWEL_Unlink
 *  \brief 
 *  Unlinks ECB from RCB waiting list
 *  \param [in] pevent Pointer to the ECB of the event
 *  \details 
 *  When starts:
 *  - pevent->ECBState != GS_ECBState_UNLINKED
 *  When finishes:
 *  - pevent->ECBState = GS_ECBState_UNLINKED 
 *  \relates Event
 */ 
void gk_RCBWEL_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10021)
	
	G_RCB *presource = (G_RCB *) get_pointed_field(pevent, ECB_AssocRCB);
	
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	GS_ECB *pevent_aux = pevent;
#endif

    PRINT_ASSERT((pevent->ECBState != GS_ECBState_UNLINKED),"ERROR ECBState= %d\n", (int) pevent->ECBState);

	/* Remove from the resource linked list */
	// 19-06-2024 gk_ECB_ECBL_Unlink((GS_ECB **) &presource->RCB_NextRCBWEL, pevent);
	
	gk_NODE_DUAL_Unlink((void **) &presource->RCB_NextRCBWEL,      // root to the ready list
	                    (void *)  pevent,                          // node to unlink
						(G_INT32) offsetof(GS_ECB,ECB_NextECB),    // next field offset
						(G_INT32) offsetof(GS_ECB,ECB_PrevECB));   // previous field offset
	pevent->ECBState = GS_ECBState_UNLINKED;	
	
	PRINT_ASSERT((pevent_aux == pevent), "ERROR in pevent");
	
    GK_SAMPLE_FUNCTION_END(10021)
}


/**gk_SCBFL_Link
 *  \brief 
 *  Links a SCB to the Free List
 *  \param [in] psignal Pointer to the SCB to be linked
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Check is ECB signal is ready for linking to free list without links.
 *  \relates Signal
 */ 
G_INT32 gk_SCBFL_Link(GS_SCB *psignal)
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
        // 22-10-2024 set_pointed_field(psignal1, SCB_NextSCBAPSL , (struct gs_scb *) 0);
        set_pointed_field(psignal1, SCB_AssocXCB    , get_pointed_field(psignal, SCB_AssocXCB));
    }
    GK_SAMPLE_FUNCTION_END(10015)
    return(psignal1);
}

/**gk_SCBAPSL_Link
 *  \brief 
 *  Links a pending signal to the root signal to create a signal list
 *  \param [in] pscb_root    Pointer to the ROOT SCB
 *  \param [in] pscb_pending Pointer to the Pending SCB
 *  \todo Check if the function is necesary, otherwise delete
 *  \relates Signal
 */ 
void gk_SCBAPSL_Link(GS_SCB *pscb_root, GS_SCB *pscb_pending)
{
    GK_SAMPLE_FUNCTION_BEGIN(28)
	pscb_pending->SCB_NextSCB = pscb_root->SCB_NextSCB;
	pscb_root = pscb_pending;
    GK_SAMPLE_FUNCTION_END(28)
}

/**gk_SCBAPSL_UnLink
 *  \brief 
 *  Unlinks a pending signal from the root signal
 *  \param [in] pscb_root    Pointer to the ROOT SCB
 *  \param [in] pscb_pending Pointer to the Pending SCB
 *  \return G_TRUE if successfull, G_FALSE if pending is executing
 *  \relates Signal
 */
G_INT32 gk_SCBAPSL_UnLink(GS_SCB *pscb_root, GS_SCB *pscb_pending)
{
    GK_SAMPLE_FUNCTION_BEGIN(29)
	// 02-07-2024 GS_SCB * pscb;
	
	// 22-10-2024 G_INT32 retorno = gk_NODE_SINGLE_Unlink((void **) &pscb_root->SCB_NextSCBAPSL, // root to the ready list
	// 22-10-2024 								(void *) pscb_pending,                         // node to link
	// 22-10-2024 								(G_INT32)  offsetof(GS_SCB,SCB_NextSCBAPSL));  // next field offset
	G_INT32 retorno = gk_NODE_SINGLE_Unlink((void **) &pscb_root->SCB_NextSCB, // root to the ready list
									(void *) pscb_pending,                         // node to link
									(G_INT32)  offsetof(GS_SCB,SCB_NextSCB));  // next field offset
	if (retorno == G_TRUE) {
		pscb_pending->SCBState = G_SCBState_UNLINKED;
	}	
	
    GK_SAMPLE_FUNCTION_END(29)
	return(G_TRUE);
}

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
G_INT32 gk_TCBAEL_Link(GS_ECB *pevent, GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10022)
    
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ptcb\n");        

	/* Associate EVENT to the Task            */
    set_pointed_field(pevent, ECB_AssocTCB, (struct gs_tcb *) ptcb); 
	
	/* Associate TASK to EVENT         */
    set_pointed_field(pevent, ECB_NextTCBAEL, get_pointed_field(ptcb, TCB_NextTCBAEL)); 
	
    set_pointed_field(pevent, ECB_PrevTCBAEL, (struct gs_ecb *) 0);

    if (get_pointed_field(ptcb, TCB_NextTCBAEL) != (struct gs_ecb *) 0) {
		// ptcb->TCB_NextTCBAEL->ECB_PrevTCBAEL = (struct gs_ecb *) pevent;
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
 *  \relates Event
 *  \sa Task
 */ 
G_INT32 gk_TCBAEL_Unlink(GS_ECB *pevent)
{
    GK_SAMPLE_FUNCTION_BEGIN(10018)
	G_INT32 retorno = G_FALSE;
    
    PRINT_ASSERT((gkm_ECB_IsValid(pevent) == G_TRUE),"ERROR pevent\n");
    PRINT_ASSERT((gkm_TCB_IsValid(pevent->ECB_AssocTCB) == G_TRUE),"ERROR pevent->ECB_AssocTCB\n");

    if (get_pointed_field(pevent, ECB_AssocTCB) != (GS_TCB *)0)
    {
		
        // 19-06-2024 if (get_pointed_field(get_pointed_field(pevent, ECB_AssocTCB), TCB_NextTCBAEL) == pevent) {
        // 19-06-2024     set_pointed_field(get_pointed_field(pevent, ECB_AssocTCB), TCB_NextTCBAEL, get_pointed_field(pevent, ECB_NextTCBAEL));
        // 19-06-2024 }
        // 19-06-2024 else {
        // 19-06-2024     set_pointed_field(get_pointed_field(pevent, ECB_PrevTCBAEL), ECB_NextTCBAEL, get_pointed_field(pevent, ECB_NextTCBAEL));
        // 19-06-2024 }
        // 19-06-2024 if (get_pointed_field(pevent, ECB_NextTCBAEL) != (struct gs_ecb *) 0) set_pointed_field(get_pointed_field(pevent, ECB_NextTCBAEL), ECB_PrevTCBAEL, get_pointed_field(pevent, ECB_PrevTCBAEL));

		gk_NODE_DUAL_Unlink((void **) &pevent->ECB_AssocTCB->TCB_NextTCBAEL,  // root to the ready list
							(void *)  pevent,                                 // node to unlink
							(G_INT32) offsetof(GS_ECB,ECB_NextTCBAEL),        // next field offset
							(G_INT32) offsetof(GS_ECB,ECB_PrevTCBAEL));       // previous field offset
		pevent->ECBState = GS_ECBState_UNLINKED;	

        /* Set the event unlink from task list */
        // 19-06-2024 set_pointed_field(pevent, ECB_PrevTCBAEL, (struct gs_ecb *) 0);
        // 19-06-2024 set_pointed_field(pevent, ECB_NextTCBAEL, (struct gs_ecb *) 0);
        set_pointed_field(pevent, ECB_AssocTCB  , (struct gs_tcb *) 0);
		
        retorno = G_TRUE;
    }
    GK_SAMPLE_FUNCTION_END(10018)
    return(retorno);
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
G_INT32 gk_TCBASL_Link(GS_TCB *ptcb, GS_SCB *psignal)
{
	set_pointed_field(psignal,SCBState, G_SCBState_IN_TCB);
	// 19-06-2024 return gk_SCB_Link(&ptcb->TCB_NextTCBASL, psignal, G_SCBState_IN_TCB, offsetof(GS_SCB, SCBType));
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
G_INT32  gk_TCBASL_Unlink(GS_TCB *ptcb, GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(33)
	// 02-07-2024 GS_SCB *psignal1;
	G_INT32 retorno = G_FALSE;

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ptcb\n");
    PRINT_ASSERT((gkm_SCB_IsValid(psignal) == G_TRUE),"ERROR psignal\n");
    PRINT_ASSERT((psignal->SCBState != G_SCBState_UNLINKED),"ERROR SCBState= %d\n", (int) psignal->SCBState);

	// 12-06-2024 if (ptcb->TCB_NextTCBASL != (struct gs_scb *) 0)
	// 12-06-2024 {
	
	// 19-06-2024 retorno = gk_SCB_SCBL_Unlink((GS_SCB **) &ptcb->TCB_NextTCBASL, psignal);
	
	retorno = gk_NODE_SINGLE_Unlink((void **) &ptcb->TCB_NextTCBASL,          // root to the ready list
	                                (void *) psignal,                         // node to link
									(G_INT32) offsetof(GS_SCB,SCB_NextSCB));  // next field offset
	if (retorno == G_TRUE) {
		psignal->SCBState = G_SCBState_UNLINKED;
	}
	
    PRINT_ASSERT((psignal->SCBState == G_SCBState_UNLINKED),"ERROR SCBState= %d\n", (int) psignal->SCBState);

    GK_SAMPLE_FUNCTION_END(33)
    return (retorno);
}

/**gk_TCB_List_Unlink
 *  \brief 
 *  Unlinks the TCB from events and signals and links them to the free lists respectively
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Check unlink for different resources (semaphore and messages) when they are granted
 *  \relates Task
 */
G_INT32 gk_TCB_List_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10038)
	GS_SCB *psignal1;
	GS_ECB *pevent1;
    
    // GS_ECB *pevent;
    // GS_ECB *peventime;
	
	// Remove SCB from ASL
	while (get_pointed_field(ptcb, TCB_NextTCBASL) != (struct gs_scb *) 0)
	{
        PRINT_ASSERT((gkm_SCB_IsValid((GS_SCB *) ptcb->TCB_NextTCBASL ) == G_TRUE),"ERROR SCB not valid\n");
		psignal1 = (GS_SCB *) get_pointed_field(ptcb, TCB_NextTCBASL);
		gk_TCBASL_Unlink(ptcb, psignal1);
		gk_SCBFL_Link(psignal1);
	}	

	// Remove SCB from PSL
	while (get_pointed_field(ptcb, TCB_NextTCBPSL) != (struct gs_scb *) 0)
	{
        PRINT_ASSERT((gkm_SCB_IsValid((GS_SCB *) ptcb->TCB_NextTCBPSL) == G_TRUE),"ERROR SCB not valid\n");
		psignal1 = (GS_SCB *) get_pointed_field(ptcb, TCB_NextTCBPSL);
		gk_TCBPSL_Unlink(ptcb, psignal1);
		gk_SCBFL_Link(psignal1);
	}
	
    // Remove ECB from AEL
    // UNGRANT and UNWAIT resource related event
    pevent1 = (GS_ECB *) get_pointed_field(ptcb, TCB_NextTCBAEL);
	while (pevent1 != (struct gs_ecb *) 0)
	{
        PRINT_ASSERT((gkm_RCB_IsValid(pevent1->ECB_AssocRCB) == G_TRUE),"ERROR RCB is not valid\n");
        PRINT_ASSERT((pevent1->ECB_AssocTCB == ptcb),"ERROR RCB is associated to task TCB\n");
        gk_RESOURCE_ECB_KILL_CALLBACK((GS_ECB *) pevent1);
        gk_ECB_List_Unlink(pevent1);
        // gk_TCBAEL_Unlink(pevent1);
        gk_ECBFL_Link(pevent1);
        pevent1 = (GS_ECB *) get_pointed_field(ptcb, TCB_NextTCBAEL);
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
G_INT32 gk_TCBFL_Link(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(35)

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB (%p) not valid\n",(void *) ptcb);
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_UNLINKED),"ERROR TCBState= %d\n",(int) ptcb->TCBState);

	/* Remove the links from TCB        */
    gk_TCB_List_Unlink(ptcb);

    // Does not free the structure to avoid fragmentation
    
    ptcb->TCB_NextTCB = g_kcb.KCB_FREE_TCBs;
    g_kcb.KCB_FREE_TCBs = ptcb;    


    GK_SAMPLE_FUNCTION_END(35)
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
G_INT32  gk_TCBPSL_Link(GS_TCB *ptcb, GS_SCB *psignal)
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
G_INT32  gk_TCBPSL_Unlink(GS_TCB *ptcb, GS_SCB *psignal)
{
    GK_SAMPLE_FUNCTION_BEGIN(10032)

	G_INT32 retorno = G_FALSE;
    
    PRINT_ASSERT((psignal->SCBState != G_SCBState_UNLINKED),"ERROR SCBState= %d\n",(int) psignal->SCBState);
		
	retorno = gk_NODE_SINGLE_Unlink((void **) &ptcb->TCB_NextTCBPSL,          // root to the ready list
	                                (void *) psignal,                         // node to link
									(G_INT32)  offsetof(GS_SCB,SCB_NextSCB)); // next field offset
	if (retorno == G_TRUE) {
		psignal->SCBState = G_SCBState_UNLINKED;
	}		
		
    PRINT_ASSERT((psignal->SCBState == G_SCBState_UNLINKED),"ERROR SCBState= %d\n",(int) psignal->SCBState);  
    
    GK_SAMPLE_FUNCTION_END(10032)
    return (retorno);
}

/**gk_TCBRDYL_Link
 *  \brief 
 *  Links the TCB in the Ready Task List sorted by its priority
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Implement G_FALSE return
 *  \relates Task
 */ 
G_INT32 gk_TCBRDYL_Link(GS_TCB * const ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10013)
	// 02-07-2024 GS_TCB *ptcb1;

    GS_LCB *readylist = get_pointed_field(ptcb, TCB_RDY_LCB_Index); 
	
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_UNLINKED),"ERROR TCBState= %d\n",(int) ptcb->TCBState);  

	gk_TASK_PRIORITY_SET_CALLBACK(ptcb, G_TCBState_READY); 
	
	if (get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE) /* Idle task are not inserted in Ready Lists */
	{
		gk_NODE_Link((void **) &readylist->LCB_NextTCBRDYL,  // root to the ready list
		             (void *) ptcb, TCBRDYL);                                // ascending order sorting
					 
		// 11-06-2024 if ((get_pointed_field(readylist,LCB_NextTCBRDYL) == (struct gs_tcb *) 0) || (readylist->LCB_NextTCBRDYL->TCBCurrentPriority > OC_priority.i64))
		// 11-06-2024 {// It is the first TCB to insert
		// 11-06-2024 	set_pointed_field(ptcb, TCB_NextTCB, (struct gs_tcb *) get_pointed_field(readylist,LCB_NextTCBRDYL));
		// 11-06-2024 	set_pointed_field(ptcb, TCB_PrevTCB, (struct gs_tcb *) 0);
		// 11-06-2024 	if (get_pointed_field(readylist, LCB_NextTCBRDYL) != (struct gs_tcb *) 0) set_pointed_field(get_pointed_field(ptcb, TCB_NextTCB), TCB_PrevTCB, (struct gs_tcb *) ptcb);
		// 11-06-2024 	set_pointed_field(readylist, LCB_NextTCBRDYL, (struct gs_tcb *) ptcb); 
		// 11-06-2024 }
		// 11-06-2024 else {
		// 11-06-2024 	ptcb1 = get_pointed_field(readylist, LCB_NextTCBRDYL); 
		// 11-06-2024 	while ((get_pointed_field(ptcb1, TCB_NextTCB) != (struct gs_tcb *) 0) && (ptcb1->TCB_NextTCB->TCBCurrentPriority <= OC_priority.i64))
		// 11-06-2024 	{
		// 11-06-2024 		ptcb1 = (GS_TCB *) get_pointed_field(ptcb1, TCB_NextTCB);
		// 11-06-2024 	}
		// 11-06-2024 	
		// 11-06-2024 	set_pointed_field(ptcb, TCB_NextTCB, get_pointed_field(ptcb1, TCB_NextTCB));
		// 11-06-2024 	if (get_pointed_field(ptcb1, TCB_NextTCB) != (struct gs_tcb *) 0) set_pointed_field(get_pointed_field(ptcb1, TCB_NextTCB), TCB_PrevTCB, ptcb);
		// 11-06-2024 	set_pointed_field(ptcb1, TCB_NextTCB, ptcb);
		// 11-06-2024 	set_pointed_field(ptcb, TCB_PrevTCB, (struct gs_tcb *) ptcb1);
		// 11-06-2024 }

        set_pointed_field(ptcb, TCB_AssocPCB, (G_INT32) 0); 
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
G_INT32 gk_TCBRDYL_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10017)
	GS_LCB *readylist = (GS_LCB *) get_pointed_field(ptcb, TCB_RDY_LCB_Index);

    PRINT_ASSERT((gkm_TCB_IsValid(readylist->LCB_NextTCBRDYL) == G_TRUE || readylist->LCB_NextTCBRDYL == 0),"ERROR in ready list");

    if (get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE)  /* Task Idle is not inserted in ready lists */
    {
		gk_NODE_DUAL_Unlink((void **) &readylist->LCB_NextTCBRDYL,        // root to the ready list
							(void *)  ptcb,                               // node to unlink
							(G_INT32) offsetof(GS_TCB,TCB_NextTCB),       // next field offset
							(G_INT32) offsetof(GS_TCB,TCB_PrevTCB));      // previous field offset
						
		//  19-06-2024 if (get_pointed_field(readylist, LCB_NextTCBRDYL) == ptcb) /* It is the first ready task */
		//  19-06-2024 {
		//  19-06-2024 	set_pointed_field(readylist, LCB_NextTCBRDYL, (struct gs_tcb *) get_pointed_field(ptcb,TCB_NextTCB)); 
		//  19-06-2024 	if (get_pointed_field(ptcb, TCB_NextTCB) != (GS_TCB *)0) set_pointed_field(get_pointed_field(ptcb, TCB_NextTCB),TCB_PrevTCB, (struct gs_tcb *) 0); 
		//  19-06-2024 }
		//  19-06-2024 else
		//  19-06-2024 { /* It is not the first one */
		//  19-06-2024 	if (get_pointed_field(ptcb, TCB_NextTCB) != (GS_TCB *)0) set_pointed_field(get_pointed_field(ptcb, TCB_NextTCB), TCB_PrevTCB, ptcb->TCB_PrevTCB);
        //  19-06-2024     PRINT_ASSERT((ptcb->TCB_PrevTCB != (GS_TCB *)0), "ERROR gk_TCBRDYL_Unlink");
		//  19-06-2024 	set_pointed_field(get_pointed_field(ptcb, TCB_PrevTCB), TCB_NextTCB, get_pointed_field(ptcb, TCB_NextTCB)); 
		//  19-06-2024 }
    }

	set_pointed_field(ptcb, TCBState, G_TCBState_UNLINKED);
    // 19-06-2024 set_pointed_field(ptcb, TCB_PrevTCB, (GS_TCB *) 0);
    // 19-06-2024 set_pointed_field(ptcb, TCB_NextTCB, (GS_TCB *) 0);

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
G_INT32  gk_TCBRUNL_Link(GS_TCB *ptcb)
{
    
    GK_SAMPLE_FUNCTION_BEGIN(10012)

    GS_LCB *RunList = (GS_LCB *) get_pointed_field(ptcb, TCB_RDY_LCB_Index);
    // GS_TCB *ptcb1 = get_pointed_field(RunList, LCB_NextTCBRUNL);
	GS_PCB *ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[gm_ProcessorId -1];
	
    PRINT_ASSERT(((get_pointed_field(RunList, LCB_NextTCBRUNL) == (struct gs_tcb *) 0) || (gkm_TCB_IsValid(get_pointed_field(RunList, LCB_NextTCBRUNL)) == G_TRUE)),"ERROR TCB not valid\n");
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_UNLINKED),"ERROR TCBState= %d\n",(int) ptcb->TCBState);    
  
	gk_TASK_PRIORITY_SET_CALLBACK(ptcb, G_TCBState_RUNNING); 
    
	gk_NODE_Link((void **) &RunList->LCB_NextTCBRUNL,      // root to the ready list
	             (void *) ptcb, TCBRUNL);                  // ascending order sorting


    set_pointed_field(ptcb, TCBState, G_TCBState_RUNNING); 
	set_pointed_field(ppcb, PCB_EXECTCB, (struct gs_tcb *) ptcb);
    set_pointed_field(ptcb, TCB_AssocPCB, gm_ProcessorId);
    
    if (get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE) RunList->LCBCurrentRunning++;
    
    /***********************************************/
    /* Set the processor to ptcb                   */
    PRINT_ASSERT((ptcb->TCB_StackPointer >= ptcb->TCB_StackTop - 300) && (ptcb->TCB_StackPointer <= ptcb->TCB_StackBottom),"ERROR TCB_StackPointer out of range\n");

    // Si no esta en la lista principal del procesador o es tarea idle, el procesador es puesto como libre

    if ((get_pointed_field(ptcb, TCB_RDY_LCB_Index) != (GS_LCB *) get_pointed_field(get_pointed_field(ppcb,PCB_AssocLCB),PCB_RDY_LCBL)) || (get_pointed_field(ptcb, TCBType) == G_TCBType_IDLE))
	{
		if (get_pointed_field(ppcb, PCBState) != GS_PCBState_FREE)
	        gk_LCBFPL_Link(gm_ProcessorId); // Link the processor to the free list
	}
    else
	{
        // Si esta en la lista de ready principal del procesador, entocnes es sacado de la lista de libres        
		if (get_pointed_field(ppcb, PCBState) == GS_PCBState_FREE)
		    gk_LCBFPL_Unlink((G_INT32) gm_ProcessorId); // Unlink the processor from the free list
	}
    
    GK_SAMPLE_FUNCTION_END(10012)
    return(G_TRUE);
}

G_INT32 gk_NODE_Link(void **root, void * const ptcb, enum list_type list_name)
{
	void *ptcb1 = *root;
	G_INT32 dual_linked;

	// Do no compare for ECBASL, KCBASL, KCBPSL, RCBASL, TCBPSL, TCBASL
	G_INT32 offset_next  = offsetof(GS_SCB, SCB_NextSCB);
	G_INT32 offset_prev  = offsetof(GS_SCB, SCB_NextSCB); 
	G_INT32 offset_value = offsetof(GS_SCB, SCBType);
	G_INT32 data64       = G_FALSE;
	G_INT32 asc_sort     = G_TRUE;;

	switch (list_name)
	{ 
		// case ECBASL:
		// case KCBASL: 
		// case RCBASL:
		// case TCBPSL:
		// case TCBASL:
		// 	offset_next  = offsetof(GS_SCB, SCB_NextSCB);
		// 	offset_prev  = offsetof(GS_SCB, SCB_NextSCB);
		// 	offset_value = offsetof(GS_SCB, SCBType);
		// 	data64       = G_FALSE;
		// 	asc_sort     = G_TRUE;
		// 	break;
		case ECBTL:
		case RCBGEL: 
		case RCBWEL:
			offset_next  = offsetof(GS_ECB, ECB_NextECB);
			offset_prev  = offsetof(GS_ECB, ECB_PrevECB);
			offset_value = offsetof(GS_ECB, ECBValue.i64);
			data64       = G_TRUE;
			// asc_sort	 = G_TRUE;
			break;
		case TCBRDYL:
			offset_next  = offsetof(GS_TCB, TCB_NextTCB);
			offset_prev  = offsetof(GS_TCB, TCB_PrevTCB);
			offset_value = offsetof(GS_TCB, TCBReadyPriority);
			data64       = G_TRUE;
			// asc_sort	 = G_TRUE;
			break;	
		case TCBRUNL:
			offset_next  = offsetof(GS_TCB, TCB_NextTCB);
			offset_prev  = offsetof(GS_TCB, TCB_PrevTCB);
			offset_value = offsetof(GS_TCB, TCBRunPriority);
			data64       = G_TRUE;
			asc_sort	 = G_FALSE;
			break;
		default:
			break;
	}
	
	if (offset_next == offset_prev) dual_linked = G_FALSE;
	else dual_linked = G_TRUE;

	// Not equal is logical XOR
	// if ((ptcb1 == (struct gs_tcb *) 0) || ((asc_sort == G_FALSE) != (ptcb1->TCBCurrentPriority > ptcb->TCBCurrentPriority)))
	if ((ptcb1 == (void *) 0) || (((data64 == G_TRUE) && (((asc_sort == G_FALSE) != (get_pointed_offset(G_INT64,ptcb1,offset_value) > get_pointed_offset(G_INT64,ptcb,offset_value))))) ||  ((data64 == G_FALSE) && (((asc_sort == G_FALSE) != (get_pointed_offset(G_INT32,ptcb1,offset_value) > get_pointed_offset(G_INT32,ptcb,offset_value)))))))
	{// It is the first TCB to insert
		// ptcb->TCB_NextTCB = (struct gs_tcb *) ptcb1;
		get_pointed_offset(void *,ptcb,offset_next) = (struct gs_tcb *) ptcb1;
		if (dual_linked == G_TRUE) {
			// ptcb->TCB_PrevTCB = (struct gs_tcb *) 0;
			get_pointed_offset(void *,ptcb,offset_prev) = (void *) 0;
			if (ptcb1 != (void *) 0) {
				// ptcb->TCB_NextTCB->TCB_PrevTCB = (struct gs_tcb *) ptcb;
				get_pointed_offset(void *,get_pointed_offset(void *,ptcb,offset_next),offset_prev) = (void *) ptcb;
			}
		}
		*root = (void *) ptcb; 
	}
	else {
		// while ((ptcb1->TCB_NextTCB != (struct gs_tcb *) 0) && ((asc_sort == G_TRUE) != (ptcb1->TCB_NextTCB->TCBCurrentPriority > ptcb->TCBCurrentPriority)))
		while ((get_pointed_offset(void *,ptcb1,offset_next) != (void *) 0) && (((data64 == G_TRUE) && (((asc_sort == G_TRUE) != (get_pointed_offset(G_INT64,get_pointed_offset(void *,ptcb1,offset_next),offset_value) > get_pointed_offset(G_INT64,ptcb,offset_value))))) || ((data64 == G_FALSE) && (((asc_sort == G_TRUE) != (get_pointed_offset(G_INT32,get_pointed_offset(void *,ptcb1,offset_next),offset_value) > get_pointed_offset(G_INT32,ptcb,offset_value)))))))
		{
			// ptcb1 = (GS_TCB *) ptcb1->TCB_NextTCB;
			ptcb1 = (void *) get_pointed_offset(struct gs_tcb *,ptcb1,offset_next);
		}		
		// ptcb->TCB_NextTCB = ptcb1->TCB_NextTCB;
		get_pointed_offset(void *,ptcb,offset_next) = get_pointed_offset(void *,ptcb1,offset_next);

		// ptcb1->TCB_NextTCB = ptcb;
		get_pointed_offset(void *,ptcb1,offset_next) = ptcb;
		if (dual_linked == G_TRUE) {
			// ptcb->TCB_PrevTCB = (struct gs_tcb *) ptcb1;
			get_pointed_offset(void *,ptcb,offset_prev) = ptcb1;
			// if (ptcb->TCB_NextTCB != (struct gs_tcb *) 0) ptcb->TCB_NextTCB->TCB_PrevTCB = ptcb;
			if (get_pointed_offset(void *,ptcb,offset_next) != (void *) 0) 
				get_pointed_offset(void *,get_pointed_offset(void *,ptcb,offset_next),offset_prev) = ptcb;
		}
	}
    return(G_TRUE);
}

G_INT32 gk_NODE_DUAL_Unlink(void **root, void * const pevent, G_INT32 const offset_next, G_INT32 const offset_prev)
{
	if (*root == pevent) {
		// *root = pevent->ECB_NextECB;
		*root = get_pointed_offset(void *,pevent,offset_next);
	} else {
        // pevent->ECB_PrevECB->ECB_NextECB = pevent->ECB_NextECB;
		get_pointed_offset(void *,get_pointed_offset(void *,pevent,offset_prev),offset_next) = get_pointed_offset(void *,pevent,offset_next);
	}
    // if (pevent->ECB_NextECB != (struct gs_ecb *) 0) pevent->ECB_NextECB->ECB_PrevECB = pevent->ECB_PrevECB;
	if (get_pointed_offset(void *,pevent,offset_next) != (void *) 0) {
		get_pointed_offset(void *,get_pointed_offset(void *,pevent,offset_next),offset_prev) = get_pointed_offset(void *,pevent,offset_prev);
	}

	/* Set the pointer to unlinked */
	// pevent->ECB_PrevECB = (struct gs_ecb *) 0;
	// pevent->ECB_NextECB = (struct gs_ecb *) 0;	
	get_pointed_offset(void *,pevent,offset_next) = (void *) 0;
	get_pointed_offset(void *,pevent,offset_prev) = (void *) 0;

    return(G_TRUE);
}

G_INT32 gk_NODE_SINGLE_Unlink(void **root, void *psignal, G_INT32 const offset_next)
{
	void *psignal1 = *root;
	G_INT32 retorno = G_FALSE;
	
	if (psignal1 != (void *) 0) {
		if (psignal1 == psignal)
		{
			// *root = psignal->SCB_NextSCB;
			*root = get_pointed_offset(void *,psignal,offset_next);
			retorno = G_TRUE;
		}
		else
		{
			// while ((psignal1->SCB_NextSCB != (struct gs_scb *) 0) && (psignal1->SCB_NextSCB != psignal))
			while ((get_pointed_offset(void *,psignal1,offset_next) != (void *) 0) && (get_pointed_offset(void *,psignal1,offset_next) != psignal))
			{
				psignal1 = get_pointed_offset(void *,psignal1,offset_next);
			}
			if (get_pointed_offset(void *,psignal1,offset_next) == psignal) {
				get_pointed_offset(void *,psignal1,offset_next) = get_pointed_offset(void *,psignal,offset_next);
				retorno = G_TRUE;
			}
		}
		get_pointed_offset(void *,psignal,offset_next) = (void *) 0;
	}
	return(retorno);
}


/**gk_TCBRUNL_Unlink
 *  \brief 
 *  Unlinks TCB from Running List
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Include G_FALSE when TCB is not in RUN list
 *  \relates Task
 */
G_INT32  gk_TCBRUNL_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10005)
	G_INT32 CPU_ID = get_pointed_field(ptcb, TCB_AssocPCB);
    
    PRINT_ASSERT((ptcb->TCBState == G_TCBState_RUNNING),"ERROR TCBState= %d, should be Running\n", (int) ptcb->TCBState);

    GS_LCB *RunList = (GS_LCB *) get_pointed_field(ptcb, TCB_RDY_LCB_Index);

    PRINT_ASSERT((struct_test(RunList->LCB_NextTCBRUNL) == G_TRUE),"ERROR TCB not valid\n");
    PRINT_ASSERT((CPU_ID >= 1 && CPU_ID <= G_NUMBER_OF_PCB),"ERROR TCB_AssocPCB= %d\n", (int) CPU_ID);    

    // Stop the processor executing running task triggering its interrupt
    if (CPU_ID != gm_ProcessorId ) {
        gm_ProcessorInterrupt(CPU_ID);       
    }
    
    if (g_kcb.G_PCBTbl[CPU_ID - 1].PCBState != GS_PCBState_FREE)
		gk_LCBFPL_Link(CPU_ID);  // Link processor to free list

	gk_NODE_DUAL_Unlink((void **) &RunList->LCB_NextTCBRUNL,        // root to the ready list
						(void *)  ptcb,                               // node to unlink
						(G_INT32) offsetof(GS_TCB,TCB_NextTCB),       // next field offset
						(G_INT32) offsetof(GS_TCB,TCB_PrevTCB));      // previous field offset
    
    /* Desassociate the task from the processor */    
    g_kcb.G_PCBTbl[CPU_ID - 1].PCB_EXECTCB = g_kcb.G_PCBTbl[CPU_ID-1].PCB_IDLETCB;
    
    // if (ptcb->TCBType != G_TCBType_IDLE) ptcb->TCB_AssocPCB = (G_INT32) 0; 
    if (get_pointed_field(ptcb, TCBType) != G_TCBType_IDLE) RunList->LCBCurrentRunning--;
    
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
G_INT32  gk_TCBWL_Link(GS_TCB *ptcb, unsigned int state)
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
    // if (ptcb->TCBState == G_TCBState_WAITING_COMPLETED) gk_TASK_STK_Init((GS_TCB *) ptcb);     
    
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
G_INT32 gk_TCBWL_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10010)
    
    PRINT_ASSERT((GRTOS_TASK_STATE_WAITING(ptcb)),"ERROR TCB should be waiting\n");
    PRINT_ASSERT((ptcb->TCBType != G_TCBType_IDLE),"ERROR TCB type\n");  

    // Initialise the stack if it is completed
    if (get_pointed_field(ptcb, TCBState) == G_TCBState_WAITING_COMPLETED) {
		gk_TASK_STK_Init((GS_TCB *) ptcb);
	    set_pointed_field(ptcb, TCB_MTX_NESTED, 0);
	}

    // 19-06-2024 if (g_kcb.KCB_NextTCBWL == ptcb) {
    // 19-06-2024     g_kcb.KCB_NextTCBWL = (GS_TCB *) get_pointed_field(ptcb, TCB_NextTCB);
    // 19-06-2024 }
    // 19-06-2024 else set_pointed_field(ptcb->TCB_PrevTCB, TCB_NextTCB, get_pointed_field(ptcb,TCB_NextTCB));
    // 19-06-2024 if (get_pointed_field(ptcb,TCB_NextTCB) != (struct gs_tcb *) 0) set_pointed_field(get_pointed_field(ptcb,TCB_NextTCB), TCB_PrevTCB,get_pointed_field(ptcb, TCB_PrevTCB));
	
	gk_NODE_DUAL_Unlink((void **) &g_kcb.KCB_NextTCBWL,               // root to the ready list
						(void *)  ptcb,                               // node to unlink
						(G_INT32) offsetof(GS_TCB,TCB_NextTCB),       // next field offset
						(G_INT32) offsetof(GS_TCB,TCB_PrevTCB));      // previous field offset
	

    set_pointed_field(ptcb, TCBState, G_TCBState_UNLINKED);
    // 19-06-2024 set_pointed_field(ptcb, TCB_PrevTCB, (GS_TCB *) 0);
    // 19-06-2024 set_pointed_field(ptcb, TCB_NextTCB, (GS_TCB *) 0);

    GK_SAMPLE_FUNCTION_END(10010)
    return(G_TRUE);
}

/**gk_TASK_IS_BLOCKED
 *  \brief 
 *  Returns G_TRUE when TCB is blocked waiting for a resource
 *  \param [in] ptcb Pointer to TCB
 *  \return G_TRUE when BLOCKED, G_FALSE otherwise
 *  \todo Implement for different resources
 *  \relates Resource
 */
G_INT32 gk_TASK_IS_BLOCKED(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(45)
	GS_ECB *pevent; 
	G_INT32 return_int; 

	return_int = G_FALSE; 

	pevent = (GS_ECB *) ptcb->TCB_NextTCBAEL;

	while (pevent != (struct gs_ecb *) 0)
	{
        /* GET THE TYPE OF THE EVENT     */
	    switch (pevent->ECBState)
	    {
	        /* It is a Waiting for resource Event *****************************/
	        case GS_ECBState_WAITING_RESOURCE:
	        	return_int = G_TRUE; 
	    	    break;
            default:
    			G_DEBUG_WHILEFOREVER;
    			break;
	    }
    	pevent = pevent->ECB_NextECB; 
	}
    GK_SAMPLE_FUNCTION_END(45)
	return (return_int);
}

/**gk_TCB_in_RCBGEL
 *  \brief 
 *  Returns the event of the TCB that grants the resource
 *  \param [in] presource Pointer to RCB
 *  \param [in] ptcb      Pointer to TCB
 *  \return Pointer to the ECB granted the resource or NULL if not granted
 *  \relates Task
 *  \sa Resource
 */
GS_ECB *gk_TCB_in_RCBGEL(G_RCB *presource, GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10037)
	GS_ECB *pevent; 

	pevent = get_pointed_field(presource, RCB_NextRCBGEL); 
    while ((pevent != (struct gs_ecb *) 0)) {
        if(get_pointed_field(pevent, ECB_AssocTCB) == ptcb)	break; 
        pevent = get_pointed_field(pevent, ECB_NextECB); 
    }
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
 *  \sa RRDS
 */
G_INT32 gk_RRDSFL_Link(GS_RRDS *prrds)
{
    GK_SAMPLE_FUNCTION_BEGIN(10036)
    // GS_SCB *psignal1;

	// Remove SCB from ASL
	// 15-07-2024 while (get_pointed_field(prrds, RRDS_NextSCB) != (struct gs_scb *) 0)
	// 15-07-2024 {
	// 15-07-2024 	psignal1 = get_pointed_field(prrds, RRDS_NextSCB);
    // 15-07-2024     gk_RRDSASL_UnLink(prrds, psignal1);
	// 15-07-2024 	gk_SCBFL_Link(psignal1);
	// 15-07-2024 }

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

	GS_PCB *ppcb = &g_kcb.G_PCBTbl[gm_ProcessorId -1];
	GS_TCB *ptcb = get_pointed_field(ppcb, PCB_EXECTCB);
	PRINT_ASSERT((gkm_TCB_IsValid(ppcb->PCB_EXECTCB) == G_TRUE),"ERROR TCB is not valid PCB_EXECTCB= %p\n", (void *) ppcb->PCB_EXECTCB);
	
	
    GK_SAMPLE_FUNCTION_END(10023)
	return(ptcb);
}

/**gk_PCB_GetNextTCB
 *  \brief 
 *  Returns the next TCB to be executed by the current processor. If no
 *  task is requesting for execution, then a pointer to the idle task
 *  of the processor is returned
 *  \return Pointer to the next TCB to be executed
 *  \return 
 *  \todo Check if it is necessary or delete this function
 *  \relates Processor
 *  \sa Task
 */
GS_TCB *gk_PCB_GetNextTCB(void)
{
    GK_SAMPLE_FUNCTION_BEGIN(10002)
	GS_TCB *ptcb = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB;
	GS_PCB *ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[gm_ProcessorId -1];
    GS_PCBAssocLCB *ppcbalcb = ppcb->PCB_AssocLCB;
    
    PRINT_ASSERT((gkm_GS_PCBAssocLCB_IsValid(ppcbalcb) == G_TRUE),"ERROR gkm_GS_PCBAssocLCB_IsValid (%p) not valid\n",(void *) ppcbalcb);
    PRINT_ASSERT((gkm_LCB_IsValid(ppcbalcb->PCB_RDY_LCBL) == G_TRUE),"ERROR gkm_LCB_IsValid not valid\n");
	
    if (get_pointed_field(ppcb, PCBState) == GS_PCBState_FREE) { // It is not executing a main list task
        // 24-05-2023 !!!!!!!!!!!!!!
        if ((ptcb != get_pointed_field(ppcb, PCB_IDLETCB)) && (get_pointed_field(ppcbalcb->PCB_RDY_LCBL, LCB_NextTCBRDYL) != (struct gs_tcb *) 0) &&  (get_pointed_field(get_pointed_field(ppcbalcb,PCB_RDY_LCBL), LCBExclusion) > get_pointed_field(get_pointed_field(ppcbalcb,PCB_RDY_LCBL), LCBCurrentRunning))) 
        { // Unlink if it executing a task with ready in foreground
            gk_TCBRUNL_Unlink(ptcb);
            gk_TCBRDYL_Link(ptcb);
            ptcb = get_pointed_field(ppcb, PCB_IDLETCB);
        }
        // Look for next task, otherwise remains the current
        while (ppcbalcb != (struct gs_pcb_rdy_lcbl *) 0)
        {
            PRINT_ASSERT((gkm_LCB_IsValid(ppcbalcb->PCB_RDY_LCBL) == G_TRUE),"ERROR gkm_LCB_IsValid not valid\n");
            if ((get_pointed_field(get_pointed_field(ppcbalcb,PCB_RDY_LCBL), LCB_NextTCBRDYL) != (struct gs_tcb *) 0) &&
                (get_pointed_field(get_pointed_field(ppcbalcb,PCB_RDY_LCBL), LCBExclusion) > get_pointed_field(get_pointed_field(ppcbalcb, PCB_RDY_LCBL), LCBCurrentRunning))) { // main list has a ready task
                ptcb = get_pointed_field(get_pointed_field(ppcbalcb, PCB_RDY_LCBL), LCB_NextTCBRDYL);
                PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB not valid\n");
                break;
            }
            ppcbalcb = get_pointed_field(ppcbalcb,gs_pcb_rdy_lcbl_next);
        }
    }
    else {
        // PRINT_ASSERT((g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_AssocLCB->PCB_RDY_LCBL == ptcb->TCB_RDY_LCB_Index),"ERROR LCB not valid\n");
        if (get_pointed_field(get_pointed_field(ppcbalcb, PCB_RDY_LCBL), LCB_NextTCBRDYL) != (struct gs_tcb *) 0) { // main list has a ready task
        	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ptcb not valid= %p\n", (void *) ptcb);
        	if (ptcb->TCBRunPriority > ppcbalcb->PCB_RDY_LCBL->LCB_NextTCBRDYL->TCBReadyPriority) {
                gk_TCBRUNL_Unlink(ptcb);
                gk_TCBRDYL_Link(ptcb);
                ptcb = get_pointed_field(get_pointed_field(ppcbalcb, PCB_RDY_LCBL), LCB_NextTCBRDYL);
            }
        }        
    }
    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB not valid\n");
    GK_SAMPLE_FUNCTION_END(10002)
    return(ptcb);
}


/**gk_PCBAssocLCBFL_Link
 *  \brief 
 *  Links a PCBAssocLCB to the Free PCBAssocLCB List
 *  \param [in] ppcbalcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \relates Task
 */ 
G_INT32 gk_PCBAssocLCBFL_Link(GS_PCBAssocLCB *ppcbalcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(75)

    PRINT_ASSERT((gkm_GS_PCBAssocLCB_IsValid(ppcbalcb) == G_TRUE),"ERROR gkm_GS_PCBAssocLCB_IsValid (%p) not valid\n",(void *) ppcbalcb);
    PRINT_ASSERT((gkm_LCB_IsValid(ppcbalcb->PCB_RDY_LCBL) == G_TRUE),"ERROR gkm_LCB_IsValid not valid\n");
    
    ppcbalcb->gs_pcb_rdy_lcbl_next = g_kcb.KCB_FREE_RDYs;
    g_kcb.KCB_FREE_RDYs = ppcbalcb;    

    GK_SAMPLE_FUNCTION_END(75)
    return(G_TRUE);
}

/**gk_SetLowestProcessor
 *  \brief 
 *  Sets the Lowest Processor register in GRTOS Controller
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check for error and return G_FALSE
 *  \relates Processor
 */
G_INT32 gk_SetLowestProcessor(void)
{
    GK_SAMPLE_FUNCTION_BEGIN(10003)

    unsigned int i = 1;
    unsigned int lowest_pcb = 0;
	GS_PCB *ppcb = &g_kcb.G_PCBTbl[lowest_pcb];
	GS_TCB *ptcb = get_pointed_field(ppcb, PCB_EXECTCB);
    G_INT64 priority = ptcb->TCBRunPriority;
	G_INT64 priority2;
    
	if (get_pointed_field(ppcb, PCBType)  == (int) GS_PCBType_UNAVAILABLE) 
	{
		priority = 0xFFFFFFFFFFFFFFFF;
	}
	
    if (get_pointed_field(ppcb, PCBState) == GS_PCBState_FREE)
    {
        priority = priority | (G_INT64) G_IDLE_PRIORITY_MASK;
    }
    
    while (i < G_NUMBER_OF_PCB) {
		ppcb = &g_kcb.G_PCBTbl[i];
		if (get_pointed_field(ppcb, PCBType)  == (int) GS_PCBType_AVAILABLE)
		{
			ptcb = get_pointed_field(ppcb, PCB_EXECTCB);
			if (ptcb != (struct gs_tcb *) 0)
			{
				priority2 = ptcb->TCBRunPriority;
				if (get_pointed_field(ppcb, PCBState) == GS_PCBState_FREE)
				{
					priority2 = priority2 | (G_INT64) G_IDLE_PRIORITY_MASK;
				}
				if (priority2 > priority)
				{
					priority = priority2;
					lowest_pcb = i;
				}
			}
		}
        i++;
    }


	// PRINT_ASSERT((priority != 0xFFFFFFFFFFFFFFFF),"No processor left for task execution");
	PRINT_ASSERT((g_kcb.G_PCBTbl[lowest_pcb].PCBID >= 1),"ERROR No valid processor= %d\n", (int) lowest_pcb); 
	PRINT_ASSERT((g_kcb.G_PCBTbl[lowest_pcb].PCBID <= G_NUMBER_OF_PCB),"ERROR No valid processor= %d\n", (int) lowest_pcb);

	ppcb = &g_kcb.G_PCBTbl[lowest_pcb];
	GRTOS_CMD_LOW_PRC_SET(get_pointed_field(ppcb, PCBID));
	GK_SAMPLE_FUNCTION_END(10003)
    return(G_TRUE);
}

/**gk_SetNextTimeProcessor
 *  \brief 
 *  Sets the Next Time Processor register in GRTOS Controller
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Processor
 */ 
G_INT32 gk_SetNextTimeProcessor(void)
{
	GK_SAMPLE_FUNCTION_BEGIN(10016)
	
	GS_ECB * pevent = g_kcb.KCB_NextECBTL;
    
    PRINT_ASSERT((pevent != (struct gs_ecb *) 0),"ERROR No next timed event \n");
    PRINT_ASSERT((pevent->ECB_AssocTCB != (GS_TCB *)0 || pevent->ECBType == (G_INT32) G_ECBType_LASTEST_TIME),"ERROR in timed event list, ECB_AssocTCB= %d\n", (int) pevent->ECB_AssocTCB);    
    PRINT_ASSERT((pevent->ECB_AssocTCB == (GS_TCB *)0 || pevent->ECB_AssocTCB->TCB_AssocPCB <= G_NUMBER_OF_PCB),"ERROR TCB_AssocPCB= %d\n", (int) pevent);
	
	if (get_pointed_field(pevent, ECBType) != (G_INT32) G_ECBType_LASTEST_TIME) {
		PRINT_ASSERT((gkm_TCB_IsValid(pevent->ECB_AssocTCB) == G_TRUE),"ERROR invalid TCB");
		GRTOS_CMD_NXT_TM_PRC_SET(get_pointed_field(get_pointed_field(pevent, ECB_AssocTCB), TCB_AssocPCB));
	} else {
		GRTOS_CMD_NXT_TM_PRC_SET((int) 0);
	}
    GK_SAMPLE_FUNCTION_END(10016)
    return(G_TRUE);
}

/**gk_LCB_CheckInvertion
 *  \brief 
 *  Checks if there is a processor that has to be interrupted to switch task
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Processor
 *  \sa Task
 */
G_INT32 gk_LCB_CheckInvertion(void)
{   
    GK_SAMPLE_FUNCTION_BEGIN(10004)

    GS_PCB *ppcb;
    GS_TCB *ptcb;
    GS_LCB *readylist = g_kcb.KCB_NextLCBL;

    PRINT_ASSERT((readylist != (GS_LCB *) 0),"ERROR readylist\n");
    
    while ((readylist != (GS_LCB *) 0))
    {
        if (get_pointed_field(readylist, LCB_NextTCBRDYL) != (GS_TCB *) 0)
        {        
            PRINT_ASSERT((gkm_TCB_IsValid(readylist->LCB_NextTCBRDYL) == G_TRUE),"ERROR TCB is not valid\n");
            /* There is at least a Ready Task */
            ppcb = get_pointed_field(readylist, LCB_NextLCBFPL);
            if ((ppcb != (struct gs_pcb *) 0))
            {
                if (get_pointed_field(readylist, LCBExclusion) > get_pointed_field(readylist, LCBCurrentRunning))
                {
                    /* There is a Free Processor and exclusion quota, then interrupt it */
                    PRINT_ASSERT((gkm_PCB_IsValid(ppcb) == G_TRUE),"ERROR PCB is not valid\n");
					PRINT_ASSERT((ppcb->PCBState == GS_PCBState_FREE), "ERROR PCB state is not valid=%d\n",(int) ppcb->PCBState);
					
					ptcb = get_pointed_field(ppcb, PCB_EXECTCB);

					gk_TCB_Unlink(ptcb);
					gk_TCBRDYL_Link(ptcb);
                }
            }
            else
            {
                /* There is not Free Processor then check the priority of running task */
                ptcb = get_pointed_field(readylist, LCB_NextTCBRUNL);
                if (ptcb != (struct gs_tcb *) 0)
                {
                    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
                    if (ptcb->TCBRunPriority > readylist->LCB_NextTCBRDYL->TCBReadyPriority)
                    {
						/* Ready priority is greater then processor assigned is interrupted */
						gk_TCB_Unlink(ptcb);           // 24-05-2023 !!!!!!!!!
						gk_TCBRDYL_Link(ptcb);             // 24-05-2023 !!!!!!!!!
                    }
                }
            }
        }
        readylist = get_pointed_field(readylist, LCB_NextLCBL);
    }

    GK_SAMPLE_FUNCTION_END(10004)
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
G_INT32 gk_TCB_Unlink(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10006)
    
    PRINT_ASSERT((struct_test(ptcb) == G_TRUE),"ERROR TCB is not valid\n");    

	switch (ptcb->TCBState)
	{
		case G_TCBState_RUNNING:           gk_TCBRUNL_Unlink(ptcb); break;
		case G_TCBState_READY:             gk_TCBRDYL_Unlink(ptcb); break;
		case G_TCBState_WAITING_COMPLETED: gk_TCBWL_Unlink(ptcb); break;
        case G_TCBState_WAITING :          gk_TCBWL_Unlink(ptcb); break;
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

inline GS_SCB *gk_TCBPSL_GetSCB(GS_TCB *ptcb, G_INT32 SignalType)
{
	return (gk_GetSCB_from_list(ptcb->TCB_NextTCBPSL, SignalType));
}


/**gk_KCBASL_GetSCB
 *  \brief 
 *  Returns a pointer to the SCB linked to the KCBASL with the same SignalType
 *  \param [in] SignalType Signal type to search in the KCB
 *  \return Pointer to the SCB found or NULL if no SCB with such a type
 *  \todo Check if this function is necessary
 *  \relates Kernel
 *  \sa Signal
 */ 
inline GS_SCB *gk_KCBASL_GetSCB(G_INT32 SignalType)
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
inline GS_SCB *gk_TCBASL_GetSCB(GS_TCB *ptcb, G_INT32 SignalType)
{
	return (gk_GetSCB_from_list(ptcb->TCB_NextTCBASL, SignalType));
}


/**gk_ECBASL_GetSCB
 *  \brief 
 *  Returns a pointer to the GS_SCB in the associated GS_SCBs of an event or NULL if any
 *  \param [in] pecb       Pointer to the ECB
 *  \param [in] SignalType Signal type to search
 *  \return Returns a pointer to the GS_SCB in the associated GS_SCBs of a event or NULL if any
 *  \relates Event
 *  \sa Signal
 */ 
inline GS_SCB *gk_ECBASL_GetSCB(GS_ECB *pecb, G_INT32 SignalType)
{
	return (gk_GetSCB_from_list(pecb->ECB_NextECBASL, SignalType));
}
// 10-06-2024 GS_SCB *gk_ECBASL_GetSCB(GS_ECB *pecb, G_INT32 SignalType)
// 10-06-2024 {
// 10-06-2024     GK_SAMPLE_FUNCTION_BEGIN(66)
// 10-06-2024 	GS_SCB *psignal = (struct gs_scb *) 0;
// 10-06-2024 	GS_SCB *psignal1 = pecb->ECB_NextECBASL;
// 10-06-2024 
// 10-06-2024     while (psignal != (struct gs_scb *) 0)
// 10-06-2024     {
// 10-06-2024     	if (psignal->SCBType == SignalType)
// 10-06-2024     	{
// 10-06-2024     		psignal = psignal1;
// 10-06-2024     		psignal1 = (struct gs_scb *) 0;
// 10-06-2024     	}
// 10-06-2024     	else
// 10-06-2024     	{
// 10-06-2024         	psignal1 = psignal->SCB_NextSCB;
// 10-06-2024     	}
// 10-06-2024     }
// 10-06-2024     GK_SAMPLE_FUNCTION_END(66)
// 10-06-2024 	return(psignal);
// 10-06-2024 }

/**gk_RCBASL_GetSCB
 *  \brief 
 *  Returns the pointer to the SCB of a resource or NULL otherwise
 *  \param [in] prcb       Pointer to the RCB of the resource
 *  \param [in] SignalType Signal Type og the signal
 *  \return Pointer to the SCB of NULL if was not found
 *  \relates Signal
 */
inline GS_SCB *gk_RCBASL_GetSCB(G_RCB *prcb, G_INT32 SignalType)
{
	return (gk_GetSCB_from_list(prcb->RCB_NextRCBASL, SignalType));
}
// 10-06-2024 GS_SCB *gk_RCBASL_GetSCB(G_RCB *prcb, G_INT32 SignalType)
// 10-06-2024 {
// 10-06-2024     GK_SAMPLE_FUNCTION_BEGIN(67)
// 10-06-2024 	GS_SCB *psignal = (struct gs_scb *) 0;
// 10-06-2024 	GS_SCB *psignal1 = prcb->RCB_NextRCBASL;
// 10-06-2024 
// 10-06-2024     while (psignal1 != (struct gs_scb *) 0)
// 10-06-2024     {
// 10-06-2024 		if (psignal1->SCBType == SignalType)
// 10-06-2024 		{
// 10-06-2024 			psignal  = psignal1;
// 10-06-2024 			psignal1 = (struct gs_scb *) 0;
// 10-06-2024 		}
// 10-06-2024 		else
// 10-06-2024 		{
// 10-06-2024 			psignal1 = psignal->SCB_NextSCB;
// 10-06-2024 		}
// 10-06-2024     }
// 10-06-2024     GK_SAMPLE_FUNCTION_END(67)
// 10-06-2024 	return(psignal);
// 10-06-2024 }

/**gk_GetSCB_from_list
 *  \brief 
 *  Returns the pointer to the SCB in a SCB linked list or NULL otherwise
 *  \param [in] head_list       Pointer to the root of the list
 *  \param [in] SignalType Signal Type og the signal
 *  \return Pointer to the SCB or NULL if was not found
 *  \relates Signal
 */
GS_SCB *gk_GetSCB_from_list(GS_SCB *head_list, G_INT32 SignalType)
{
    GK_SAMPLE_FUNCTION_BEGIN(10062)
	GS_SCB *psignal = (GS_SCB *) 0;
	GS_SCB *psignal1 = (GS_SCB *) head_list;

    while (psignal1 != (struct gs_scb *) 0)
    {
    	if (psignal1->SCBType == SignalType)
    	{
    		psignal = psignal1;
    		psignal1 = (struct gs_scb *) 0;
    	}
    	else
    	{
    		psignal1 = psignal1->SCB_NextSCB;
    	}
    }
    GK_SAMPLE_FUNCTION_END(10062)
	return(psignal);
}
/***********************************************************************************
*********************** GRTOS COMPLEMENTARY FUNCTIONS  ****************************
***********************************************************************************/

/**gk_TASK_STK_Init
 *  \brief 
 *  Initializes the Task Stack acording to the TCB parameters
 *  \param [in] ptcb Pointer to the TCB of the task to initilise
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Implement validity checks
 *  \relates Task
 */
G_INT32 gk_TASK_STK_Init(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(10009)
	GS_STK *stk;

#ifdef __niosX_arch__
    // not changing the current stack
    void *StackPointer=(void *)0;
    GRTOS_READ_SP(StackPointer);
    PRINT_ASSERT(((void *) ptcb->TCB_StackBottom < StackPointer || (void *) ptcb->TCB_StackTop > StackPointer),"ERROR Initiliasing the current stack, ptcb->TCB_StackBottom= %p, ptcb->TCB_StackTop= %p, StackPointer= %p \n", (void *) ptcb->TCB_StackBottom, (void *) ptcb->TCB_StackTop, (void *) StackPointer);
    StackPointer = (void *) get_pointed_field(ptcb, TCB_StackPointer);
    PRINT_ASSERT(((void *) ptcb->TCB_StackBottom >= StackPointer && (void *) ptcb->TCB_StackTop <= StackPointer),"ERROR Initiliasing the current task stack, ptcb->TCB_StackBottom= %p, ptcb->TCB_StackTop= %p, StackPointer= %p \n", (void *) ptcb->TCB_StackBottom, (void *) ptcb->TCB_StackTop, (void *) StackPointer);
#endif

	stk = (GS_STK *) get_pointed_field(ptcb, TCB_StackBottom) - G_STACK_STATUS_LENGTH;  /* It has to be aligned */
	set_pointed_field(ptcb, TCB_StackPointer, stk);            /* Load Stack pointer in TCB               */

	/* Lleno la pila del primer llamado  */
	stk[OFFSET_REG_0]  = 1;                                                            /* estatus         */
	stk[OFFSET_REG_1]  = (G_INT32) gk_ENTRY_TASK_COMPLETE;                             /* register r31 ra */
	
	stk[OFFSET_REG_2]  = (G_INT32) get_pointed_field(ptcb, TCB_TaskCode) + G_RETURN_OFFSET_FROM_INTERRUPT; /* register r29 ea */

	stk[OFFSET_REG_3]  = (G_INT32) get_pointed_field(ptcb, TCB_StackPointer);    /* register r28 fp */
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
	stk[OFFSET_REG_15] = (G_INT32) get_pointed_field(ptcb, TCB_TaskArg);         /* register r4     */
	stk[OFFSET_REG_16] = G_INITIAL_REGISTER_VALUE;                               /* register r3     */
	stk[OFFSET_REG_17] = (G_INT32) get_pointed_field(ptcb,TCB_TaskArg);          /* register r2     */
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

	
    // 05-07-2024 #ifdef __niosX_arch__
    // 05-07-2024 	memcpy((void *) ptcb->_impure_ptr, (void *) _impure_ptr, sizeof(struct _reent));
    // 05-07-2024 #endif
	// for (unsigned int j = 0 ; j < sizeof(struct _reent) / 4; j++) {
	// 	((unsigned int *) &(ptcb->_impure_ptr))[j] = ((unsigned int *) _impure_ptr)[j];
	// }

#ifndef __niosX_arch__
	getcontext(&ptcb->uctx);
    ptcb->uctx.uc_stack.ss_sp = ptcb->uctx_stack;
    // makecontext(&ptcb->uctx, *(void **) &ptcb->TCB_TaskCode, 1, ptcb->TCB_TaskArg);
    makecontext(&ptcb->uctx, gu_pthread, 1, (int) ptcb);
#endif

    GK_SAMPLE_FUNCTION_END(10009)
    return(G_TRUE);
}

/**
 *  \brief 
 *  Destroys a task and return the data structure to the TCB free list
 *  \param [in] ptcb Pointer to the TCB of the task to destroy
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \details Unlinks all the linked lists and link to the Free TCB list
 *  \todo Implement G_FALSE return
 *  \relates Task
 */ 
G_INT32 gk_TASK_Kill(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(70)
    gk_TCB_Unlink(ptcb);       /* Unlinks the TCB structure               */
    gk_TCB_List_Unlink(ptcb);  /* Unlinks all the linked lists of the TCB */
    gk_TCBFL_Link(ptcb);       /* Links the TCB to the TCBFL              */
    GK_SAMPLE_FUNCTION_END(70)
    return (G_TRUE);
}

/***********************************************************************************
*********************** GRTOS COMPLEMENTARY FUNCTIONS to be added!!!!!!!  ****************************
***********************************************************************************/



/***********************************************************************************
*********************** FINISH COMPLEMENTARY FUNCTIONS  ***************************
***********************************************************************************/

/**gk_TASK_GRANTED_PRIORITY_GET
 *  \brief 
 *  Determine the priority of a tasks according to the resources granted
 *  \param [in] ptcb Pointer to the TCB of the task
 *  \return Priority determined
 *  \todo Check according the type of reource
 *  \relates Task
 */
G_INT64 gk_TASK_GRANTED_PRIORITY_GET(GS_TCB *ptcb)
{
    GK_SAMPLE_FUNCTION_BEGIN(74)
	GS_ECB *pevent;
	G_INT64 priority;

    PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR ptcb invalid= %p\n",ptcb);

	priority = G_LOWEST_PRIORITY;
	pevent = (GS_ECB *) ptcb->TCB_NextTCBAEL;
	while ((pevent != (struct gs_ecb *) 0))
	{
		// Controlar que solo sean recursos granted !!!!!!!!!!!!!!
        if (priority > pevent->ECBValue.i64) priority = pevent->ECBValue.i64;
    	pevent = pevent->ECB_NextECB;
	}
    GK_SAMPLE_FUNCTION_END(74)
	return (priority);
}

// !!!!D 14-10-2024 G_INT32 get_CPU_ID(void)
// !!!!D 14-10-2024 {
// !!!!D 14-10-2024 	return ((G_INT32) (*((volatile unsigned int *) GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1 + ADDR_REG_1)));
// !!!!D 14-10-2024 }

OPTIMEZE_RESTORE

