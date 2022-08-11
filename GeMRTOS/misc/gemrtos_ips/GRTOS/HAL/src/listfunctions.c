/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS structure list management functions
 *  \details This file contains thefuntion for list management.
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

#include <grtosdefinitions.h>

OPTIMEZE_CODE(3)

// https://gcc.gnu.org/onlinedocs/gcc/Nios-II-Options.html#index-mno-bypass-cache
// #pragma GCC optimize('mno-cache-volatile')



/***********************************************************************************
 * *********************************************************************************
 *                    gRTOS SUPPORT FUNCTIONS
 * *********************************************************************************
 ***********************************************************************************/
 
// /**gk_ECB_GetFree
//  *  \brief  Returns a pointer to a Free ECB, NULL if there is not ECB available
//   * \return pointer to a Free ECB, NULL if there is not ECB available
//  *  \todo System signal should be implmented when no free ECB is available
//  *  \relates Event
//  */ 
// GS_ECB *gk_ECB_GetFree(void)
// {
//     SAMPLE_FUNCTION_BEGIN(1)
//     GS_ECB *pevent =  gk_Get_ECB();
// 
// 	// if (pevent != (struct gs_ecb *) 0){
// 		// pevent->ECB_NextECBAEL = (struct gs_ecb *) 0;
// 		// pevent->ECB_AssocTCB = (struct gs_tcb *) 0;
// 		// pevent->ECB_NextECBASL = (struct gs_scb *) 0;
// 		// pevent->ECB_RRDS = (struct gs_rrds *) 0;
// 	    // pevent->ECB_PrevTCBAEL = (struct gs_ecb *) 0;
// 	    // pevent->ECB_NextTCBAEL = (struct gs_ecb *) 0;
// 	    // pevent->ECB_PrevECB = (struct gs_ecb *) 0;
// 	    // pevent->ECB_NextECB = (struct gs_ecb *) 0;
// 	    // pevent->ECBValue.i64 = G_LOWEST_PRIORITY;
// 		// pevent->ECBState = GS_ECB_STATE_UNLINKED;
// 	// }
//     
// #if G_DEBUG_WHILEFOREVER_ENABLE == 1
// 	if (pevent == (struct gs_ecb *) 0) G_DEBUG_WHILEFOREVER;
// #endif
//     SAMPLE_FUNCTION_END(1)
// 	return(pevent);
// }

/**gk_ECBAEL_Link
 *  \brief 
 *  Links event2 to the Associated list of event1
 *  \param [in] pevent1 pointer to the ECB of event 1
 *  \param [in] pevent2 pointer to the ECB of event 2
 *  \return G_TRUE if successful
 *  \relates Event
 */ 
INT32 gk_ECBAEL_Link(GS_ECB *pevent1, GS_ECB *pevent2)
{
    SAMPLE_FUNCTION_BEGIN(2)
	GS_ECB *pevent;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (ECB_IsValid(pevent1) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (ECB_IsValid(pevent2) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

    if (pevent1->ECB_NextECBAEL == (struct gs_ecb *) 00) 
    //if ((GS_ECB *) *((char *) pevent1 + offsetof(struct gs_ecb, ECB_NextECBAEL)) == (struct gs_ecb *) 00) 
    {
        if (pevent2->ECB_NextECBAEL == (struct gs_ecb *) 00)
        {
            pevent1->ECB_NextECBAEL = (struct gs_ecb *) pevent2;
            pevent2->ECB_NextECBAEL = (struct gs_ecb *) pevent1;
        }
        else
        {
            pevent1->ECB_NextECBAEL = pevent2->ECB_NextECBAEL;
            pevent2->ECB_NextECBAEL = (struct gs_ecb *) pevent1;        
        }
    }
    else
    {
        if (pevent2->ECB_NextECBAEL == (struct gs_ecb *) 00)
        {
            pevent2->ECB_NextECBAEL = pevent1->ECB_NextECBAEL;
            pevent1->ECB_NextECBAEL = (struct gs_ecb *) pevent2;
        }
        else
        {
            pevent = pevent1->ECB_NextECBAEL;
            pevent1->ECB_NextECBAEL = pevent2->ECB_NextECBAEL;
            pevent2->ECB_NextECBAEL = (struct gs_ecb *) pevent;
        }
    }
    SAMPLE_FUNCTION_END(2)
	return(G_TRUE);
}
 
/**gk_ECBAEL_Remove
 *  \brief 
 *  Unlinks the pevent from the associated ECB list
 *  \param [in] pevent pointer to ECB of the event
 *  \return G_TRUE if event was linked to a Associated list, G_FALSE otherwise
 *  \relates Event
 */ 
INT32 gk_ECBAEL_Remove(GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(3)
    GS_ECB *pevent1;
    INT32 retorno = G_FALSE;

	#if G_DEBUG_WHILEFOREVER_ENABLE == 1
		if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
	#endif

    if (pevent->ECB_NextECBAEL != (struct gs_ecb *) 0)
    {
        // Looks the previous ECB that points to pevent
        pevent1 = pevent->ECB_NextECBAEL;
        while (pevent1->ECB_NextECBAEL != (struct gs_ecb *) pevent)
        {
            #if G_DEBUG_WHILEFOREVER_ENABLE == 1
                if (ECB_IsValid(pevent1) != G_TRUE) G_DEBUG_WHILEFOREVER;
            #endif            
            pevent1 = (GS_ECB *) pevent1->ECB_NextECBAEL;
        }
        if (pevent->ECB_NextECBAEL != pevent1) pevent1->ECB_NextECBAEL = pevent->ECB_NextECBAEL; // there is more ECB
        else pevent1->ECB_NextECBAEL = (struct gs_ecb *) 0; 
        pevent->ECB_NextECBAEL = (struct gs_ecb *) 0;
        retorno = G_TRUE;
    }
    SAMPLE_FUNCTION_END(3)
	return(retorno);
}
 

 
/**gk_ECBASL_Link
 *  \brief 
 *  Links a SCB to the ECB Associated Signal List
 *  \param [in] pevent  Pointer to the ECB of the event
 *  \param [in] psignal Pointer to the SCB of the signal
 *  \relates Signal
 */ 
void  gk_ECBASL_Link(GS_ECB *pevent, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(4)
    INT32 priority;
    GS_SCB *psignal2;

	priority = psignal->SCBType;
    if (pevent->ECB_NextECBASL == (struct gs_scb *) 0) 
    {
        // it is the only SCB associated to pevent
    	psignal->SCB_NextSCB   = (struct gs_scb *) 0;
    	pevent->ECB_NextECBASL = (struct gs_scb *) psignal;
    }
    else 
    {
        if(pevent->ECB_NextECBASL->SCBType > priority)
        {
            // it is the first SCB associated to pevent
            psignal->SCB_NextSCB    = pevent->ECB_NextECBASL;
            pevent->ECB_NextECBASL  = (struct gs_scb *) psignal;        
        }
        else
        {
            // Sort by priotiy
            psignal2 = pevent->ECB_NextECBASL;
            while (psignal2 != (struct gs_scb *) 0)
            {
                if (psignal2->SCB_NextSCB == (struct gs_scb *) 0)
                {
                    // It is inserted as the last SCB
                    psignal->SCB_NextSCB  = (struct gs_scb *) 0;
                    psignal2->SCB_NextSCB = (struct gs_scb *) psignal;
                    psignal2 = (struct gs_scb *) 0;
                }
                else
                {
                    if (psignal2->SCB_NextSCB->SCBType > priority)
                    {
                        psignal->SCB_NextSCB  = psignal2->SCB_NextSCB;
                        psignal2->SCB_NextSCB = (struct gs_scb *) psignal; 
                        psignal2 = (struct gs_scb *) 0;
                    }
                    else
                    {
                        // Keep searching
                        psignal2 = psignal2->SCB_NextSCB;                        
                    }
                }
            }
        }
        psignal->SCBState = G_SCB_STATE_IN_ECB;
    }
    SAMPLE_FUNCTION_END(4)
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
INT32  gk_ECBASL_Unlink(GS_ECB *pevent, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(5)
    int retorno = G_FALSE;
	GS_SCB *psignal1;
	GS_SCB *psignal2;

    if (pevent->ECB_NextECBASL == psignal)
    {
        // It is the fist ESB of the list
    	pevent->ECB_NextECBASL = psignal->SCB_NextSCB;
		psignal->SCBState = G_SCB_STATE_UNLINKED;
		retorno = G_TRUE;
    }
    else
    {
    	psignal2 = pevent->ECB_NextECBASL;
    	while (psignal2 != psignal && psignal2 != (struct gs_scb *) 0)
    	{
    		psignal1 = psignal2;
    		psignal2 = psignal2->SCB_NextSCB;
    	}
    	if (psignal2 == (struct gs_scb *) 0)
    	{
    		retorno = G_FALSE;
    	}
    	else
    	{
    		psignal1->SCB_NextSCB = psignal->SCB_NextSCB;
    		psignal->SCBState = G_SCB_STATE_UNLINKED;
    		retorno = G_TRUE;
    	}
    }
    SAMPLE_FUNCTION_END(5)
    return(retorno);
}

/**gk_ECB_List_Unlink
 *  \brief 
 *  Unlinks all the structures linked to a ECB
 *  \param [in] pevent Pointer to the ECB
 *  \return G_TRUE
 *  \todo Verify when a resource is unlinked (semaphore and queue message)
 *  \relates Event
 */ 
INT32 gk_ECB_List_Unlink(GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(6)
	GS_SCB *psignal1;
	
	/// GS_ECB  Event Control Block structure
	/// typedef struct gs_ecb {
	//	 INT32         ECBState;          /** Granted, Waiting, Free                                 */
	//	 INT32         ECBType;           /** Type of event control block (see OS_EVENT_TYPE_xxxx)   */
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
	/// } GS_ECB;
	
	// Remove ECB from waiting list ECB_NextECB and ECB_PrevECB pointed in timed or resource list
	// according the pevent->ECBState field
    switch (pevent->ECBState)
    {
        case GS_ECB_STATE_WAITING_TIME:
            gk_ECBTL_Unlink(pevent);
            break;
        case GS_ECB_STATE_GRANTED_RESOURCE:    
            gk_RCBGEL_Unlink(pevent);            
            break;
        case GS_ECB_STATE_WAITING_RESOURCE:  
            gk_RCBWEL_Unlink(pevent);
            break;
        case GS_ECB_STATE_UNLINKED:
            break;
        default:
            G_DEBUG_WHILEFOREVER;
            break;
    }
	
    // Remove from the task list
	gk_TCBAEL_Unlink(pevent);
    
    // Remove from the ECB Associated list
	gk_ECBAEL_Remove(pevent);
	
	// Remove SCB from ASL
	while (pevent->ECB_NextECBASL != (struct gs_scb *) 0)
	{
		psignal1 = pevent->ECB_NextECBASL;
		gk_ECBASL_Unlink(pevent, psignal1);
		gk_SCBFL_Link(psignal1);
	}	

    // Remove RRDS
    struct gs_rrds *prrds;
    struct gs_rrds *prrds1;
    prrds = (struct gs_rrds *) pevent->ECB_RRDS;
    pevent->ECB_RRDS = (struct gs_rrds *) 0; 
    while (prrds != (struct gs_rrds *) 0) {
        prrds1 = (struct gs_rrds *) prrds->RRDS_NextRRDS;
        gk_RRDSFL_Link(prrds);
        prrds = prrds1;
    }
    SAMPLE_FUNCTION_END(6)
    return(G_TRUE);
}

/**gk_ECBFL_Link
 *  \brief 
 *  Link ECB to Free List. Removes the signals from ECB
 *  \param [in] pevent Pointer to the ECB to link
 *  \return G_TRUE
 *  \relates Event
 */ 
INT32 gk_ECBFL_Link(GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(7)
    // Unlink the ECB
    gk_ECB_List_Unlink(pevent);
    
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (pevent->ECBState != GS_ECB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif

    free(pevent->malloc_address);
    g_kcb.KCB_NUMBER_OF_ECBs-- ;
    
    SAMPLE_FUNCTION_END(7)
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
inline INT32  gk_ECBTL_Link (GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(8)
	GS_ECB *pevent2;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (pevent->ECBState != GS_ECB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif
    //printf("pevent = %p, %p", (void *)*((char *) pevent + 16), pevent->ECB_NextECB);
	
    if (g_kcb.KCB_NextECBTL->ECBValue.i64 > pevent->ECBValue.i64)
		{/* Event inserted is the next occurence   */
		pevent->ECB_NextECB = g_kcb.KCB_NextECBTL;
		pevent->ECB_PrevECB = (struct gs_ecb *) 00;
		g_kcb.KCB_NextECBTL->ECB_PrevECB = (struct gs_ecb *) pevent;
		g_kcb.KCB_NextECBTL = (struct gs_ecb *) pevent;
		GRTOS_CMD_NXT_OCC_TM_EVN_SET(pevent->ECBValue.i64);
	} else {
		/* Recorro la lista para buscar el punto de insercion del evento */
        /* siempre habra un ultimo elemento */
		pevent2 = g_kcb.KCB_NextECBTL;
		while (pevent2->ECBValue.i64 <= pevent->ECBValue.i64){
			pevent2 = pevent2->ECB_NextECB;
		}        

		pevent->ECB_NextECB = (struct gs_ecb *) pevent2;   /* Insert the task in the linked list        */
		pevent->ECB_PrevECB = pevent2->ECB_PrevECB;
		pevent2->ECB_PrevECB->ECB_NextECB = (struct gs_ecb *) pevent;
		pevent2->ECB_PrevECB = (struct gs_ecb *) pevent;
	}
    /* Change Status of the Event */
    pevent->ECBState = GS_ECB_STATE_WAITING_TIME;
    SAMPLE_FUNCTION_END(8)
    return(G_TRUE);
}

/**gk_ECBTL_Unlink
 *  \brief 
 *  Unlinks the ECB from the Time Event List
 *  \param [in] pevent Pointer to the ECB of the event to unlink
 *  \return G_TRUE if successful, G_FALSE if was not a time event
 *  \details
 *  When starts:
 *     pevent->ECBState = GS_ECB_STATE_WAITING_TIME
 *  When finishes:
 *     pevent->ECBState = GS_ECB_STATE_UNLINKED
 *  \todo Implement G_FALSE when no a time event
 *  \relates Event
 */
inline INT32 gk_ECBTL_Unlink(GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(9)
	#if G_DEBUG_WHILEFOREVER_ENABLE == 1
		if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
		if (pevent->ECBState != GS_ECB_STATE_WAITING_TIME) G_DEBUG_WHILEFOREVER;
	#endif

	/* Remove from the time linked list */
	if (g_kcb.KCB_NextECBTL == (struct gs_ecb *) pevent)
	{
		g_kcb.KCB_NextECBTL = pevent->ECB_NextECB;   
        // g_kcb.KCB_NextECBTL is never NULL becuase the latest ECB is linked
        
        if (g_kcb.KCB_NextECBTL == (struct gs_ecb *) 0) G_DEBUG_WHILEFOREVER;
   
		g_kcb.KCB_NextECBTL->ECB_PrevECB = (struct gs_ecb *) 00;
		GRTOS_CMD_NXT_OCC_TM_EVN_SET(g_kcb.KCB_NextECBTL->ECBValue.i64);
	} else
	{
        pevent->ECB_PrevECB->ECB_NextECB = pevent->ECB_NextECB;
        pevent->ECB_NextECB->ECB_PrevECB = pevent->ECB_PrevECB;
	}
	/* Set the pointer to unlinked */
	pevent->ECB_PrevECB = (struct gs_ecb *) 0;
	pevent->ECB_NextECB = (struct gs_ecb *) 0;
	pevent->ECBState = GS_ECB_STATE_UNLINKED;
    SAMPLE_FUNCTION_END(9)
    return(G_TRUE);
}

/**gk_KCBASL_Link
 *  \brief 
 *  Links a SCG to the Associated Signal list of KCB
 *  \param [in] psignal Pointer to the SCB of the signal to link
 *  \return G_TRUE if successful
 *  \relates Signal
 */
INT32  gk_KCBASL_Link(GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(10)
    INT32 priority;
    GS_SCB *psignal2;

	priority = psignal->SCBType;
    if (g_kcb.KCB_NextKCBASL == (struct gs_scb *) 0)
    {
        // it is the only SCB associated to pevent
    	psignal->SCB_NextSCB = (struct gs_scb *) 0;
    	g_kcb.KCB_NextKCBASL = (struct gs_scb *)psignal;
    }
    else
    {
        if(g_kcb.KCB_NextKCBASL->SCBType > priority)
        {
            // it is the first SCB associated to pevent
            psignal->SCB_NextSCB = g_kcb.KCB_NextKCBASL;
            g_kcb.KCB_NextKCBASL = (struct gs_scb *) psignal;
        }
        else
        {
            // Sort by priotiy
            psignal2 = g_kcb.KCB_NextKCBASL;
            while (psignal2 != (struct gs_scb *) 0)
            {
                if (psignal2->SCB_NextSCB == (struct gs_scb *) 0)
                {
                    // It is inserted as the last SCB
                    psignal->SCB_NextSCB = (struct gs_scb *) 0;
                    psignal2->SCB_NextSCB = (struct gs_scb *) psignal;
                    psignal2 = (struct gs_scb *) 0;
                }
                else
                {
                    if (psignal2->SCB_NextSCB->SCBType > priority)
                    {
                        psignal->SCB_NextSCB  = psignal2->SCB_NextSCB;
                        psignal2->SCB_NextSCB = (struct gs_scb *) psignal;
                        psignal2 = (struct gs_scb *) 0;
                    }
                    else
                    {
                        // Keep searching
                        psignal2 = psignal2->SCB_NextSCB;
                    }
                }
            }
        }
        psignal->SCBState = G_SCB_STATE_IN_KCB;
    }
    SAMPLE_FUNCTION_END(10)
    return(G_TRUE);
}

/**gk_KCBASL_Unlink
 *  \brief 
 *  Unlinks a SCG from the Associated Signal list of KCB
 *  \param [in] psignal Pointer to the SCB of the signal to unlink
 *  \return G_TRUE if successful, G_FALSE if not such signal linked to KCBASL
 *  \relates Signal
 *  \sa Kernel
 */ 
INT32  gk_KCBASL_Unlink(GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(11)
    int retorno = G_FALSE;
	GS_SCB *psignal1;
	GS_SCB *psignal2;

    if (g_kcb.KCB_NextKCBASL == psignal)
    {
        // It is the fist ESB of the list
    	g_kcb.KCB_NextKCBASL = psignal->SCB_NextSCB;
		psignal->SCBState = G_SCB_STATE_UNLINKED;
		retorno = G_TRUE;
    }
    else
    {
    	psignal2 = g_kcb.KCB_NextKCBASL;
    	while (psignal2 != psignal && psignal2 != (struct gs_scb *) 0)
    	{
    		psignal1 = psignal2;
    		psignal2 = psignal2->SCB_NextSCB;
    	}
    	if (psignal2 == (struct gs_scb *) 0)
    	{
    		retorno = G_FALSE;
    	}
    	else
    	{
    		psignal1->SCB_NextSCB = psignal->SCB_NextSCB;
    		psignal->SCBState = G_SCB_STATE_UNLINKED;
    		retorno = G_TRUE;
    	}
    }
    SAMPLE_FUNCTION_END(11)
    return(retorno);
}

/**gk_LCBL_Link
 *  \brief 
 *  Inserts a RunList in LCB Runining list ordered by background list and then priority
 *  \param [in] plcb Pointer to the LCB list
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Check everything about this function
 *  \todo Define the G_FALSE return
 *  \relates List
 */
INT32 gk_LCBL_Link(GS_LCB *plcb)
{
    SAMPLE_FUNCTION_BEGIN(12)
	GS_LCB *plcb1 = g_kcb.KCB_NextLCBL;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (LCB_IsValid(plcb) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

	if (g_kcb.KCB_NextLCBL == (struct gs_lcb *) 0)
	{/* it is the first list to be added */
		g_kcb.KCB_NextLCBL = (struct gs_lcb *) plcb;
		plcb->LCB_NextLCBL = (struct gs_lcb *) 0;
		plcb->LCB_PrevLCBL = (struct gs_lcb *) 0;
	}
	else
	{/* The list it is not empty */
		if (gk_LCBLowerPriorityThanLCB(plcb, g_kcb.KCB_NextLCBL) == G_TRUE)
		{
			/* It should be inserted as first element */
			plcb->LCB_NextLCBL = g_kcb.KCB_NextLCBL;
			plcb->LCB_PrevLCBL = (struct gs_lcb *) 0;
			plcb->LCB_NextLCBL->LCB_PrevLCBL = (struct gs_lcb *) plcb;
			g_kcb.KCB_NextLCBL = (struct gs_lcb *) plcb;
		}
		else
		{
			while (plcb1 != (struct gs_lcb *) 0) {
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (LCB_IsValid(plcb1) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif
				if (plcb1->LCB_NextLCBL == (struct gs_lcb *) 0){
					/* It has to be inserted as the last element */
					plcb->LCB_NextLCBL  = (struct gs_lcb *) 0;
					plcb->LCB_PrevLCBL  = (struct gs_lcb *) plcb1;
					plcb1->LCB_NextLCBL = (struct gs_lcb *) plcb;
					plcb1 = (struct gs_lcb *) 0; /* to exit the while */
				}
				else
				{
					/* Chheck if it has to be inserted before next */
					if (gk_LCBLowerPriorityThanLCB(plcb, plcb1->LCB_NextLCBL) == G_TRUE){
						plcb->LCB_NextLCBL  = plcb1->LCB_NextLCBL;
						plcb->LCB_PrevLCBL  = (struct gs_lcb *) plcb1;
						plcb->LCB_NextLCBL->LCB_PrevLCBL = (struct gs_lcb *) plcb;
						plcb1->LCB_NextLCBL = (struct gs_lcb *) plcb;
						plcb1 = (struct gs_lcb *) 0; /* to exit the while */
					}
					else
					{
						plcb1 = plcb1->LCB_NextLCBL;
					}
				}
			}
		}
	}
	plcb->LCBState = GS_LCB_STATE_LINKED;
    SAMPLE_FUNCTION_END(12)
    return(G_TRUE);
}
 
 
/**gk_LCBL_UnLink
 *  \brief 
 *  Unlinks the LCB from the LCB List
 *  \param [in] plcb Pointer to the LCB to unlink
 *  \return G_TRUE if successful, G_FALSE otherwise 
 *  \todo Implement G_FALSE returns
 *  \relates List
 */ 
INT32 gk_LCBL_UnLink(GS_LCB *plcb)
{
    SAMPLE_FUNCTION_BEGIN(13) 

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (LCB_IsValid(plcb) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (LCB_IsValid(g_kcb.KCB_NextLCBL) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

	/// Check if it is the first element in the list
	if (g_kcb.KCB_NextLCBL == plcb)
	{/* Remove as first element  */
		g_kcb.KCB_NextLCBL = plcb->LCB_NextLCBL;
		if (g_kcb.KCB_NextLCBL != (struct gs_lcb *) 0) g_kcb.KCB_NextLCBL->LCB_PrevLCBL =(struct gs_lcb *) 0;
	}
	else
	{/* It is not the firstLCB in the list */
		plcb->LCB_PrevLCBL->LCB_NextLCBL = plcb->LCB_NextLCBL;
		if (plcb->LCB_NextLCBL != (struct gs_lcb *) 0) plcb->LCB_NextLCBL->LCB_PrevLCBL = plcb->LCB_PrevLCBL;
	}
	plcb->LCBState = GS_LCB_STATE_UNLINKED;
    
    SAMPLE_FUNCTION_END(13) 
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
 *  \sa List
 */
INT32 gk_LCBFPL_Link(int processorID)
{
    SAMPLE_FUNCTION_BEGIN(14)
	GS_PCB *ppcb = &g_kcb.G_PCBTbl[processorID-1];
    GS_LCB *plcb = ppcb->PCB_RDY_LCBL[0];

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (processorID < 1 || processorID > G_NUMBER_OF_PCB) G_DEBUG_WHILEFOREVER;
#endif

    if (ppcb->PCBState != GS_PCB_STATE_FREE) {
        if (plcb->LCB_NextLCBFPL == (struct gs_pcb *) 0) {
            plcb->LCB_NextLCBFPL = (struct gs_pcb *) ppcb;
            ppcb->PCB_NextPCB = (struct gs_pcb *) 0;
            ppcb->PCB_PrevPCB = (struct gs_pcb *) 0;
        }
        else
        {
            plcb->LCB_NextLCBFPL->PCB_PrevPCB = (struct gs_pcb *) ppcb;
            ppcb->PCB_NextPCB = plcb->LCB_NextLCBFPL;
            ppcb->PCB_PrevPCB = (struct gs_pcb *) 0;
            plcb->LCB_NextLCBFPL = (struct gs_pcb *) ppcb;
        }
    }
	ppcb->PCBState = GS_PCB_STATE_FREE;


#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (plcb->LCB_NextLCBFPL == (struct gs_pcb *) 0) G_DEBUG_WHILEFOREVER;
#endif
    SAMPLE_FUNCTION_END(14)
    return(G_TRUE);
} 


/**gk_LCBFPL_Unlink
 *  \brief 
 *  Unlinks a PCB from the LCB Free Processor List
 *  \param [in] processorID Processor ID
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Check if Processor ID is valid and return G_FALSE
 *  \relates Processor
 *  \sa List
 */
INT32 gk_LCBFPL_Unlink(int processorID)
{
    SAMPLE_FUNCTION_BEGIN(15)
	GS_PCB *ppcb = &g_kcb.G_PCBTbl[processorID-1];
    GS_LCB *plcb = ppcb->PCB_RDY_LCBL[0];    

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (processorID < 1 || processorID > G_NUMBER_OF_PCB) G_DEBUG_WHILEFOREVER;
#endif


    if (ppcb->PCBState != GS_PCB_STATE_RUNNING) {
        if (plcb->LCB_NextLCBFPL == ppcb) { /* It is the first element */
            plcb->LCB_NextLCBFPL = ppcb->PCB_NextPCB;
        }
        else {
            ppcb->PCB_PrevPCB->PCB_NextPCB = ppcb->PCB_NextPCB;
        }
        if (ppcb->PCB_NextPCB != (struct gs_pcb *) 0) ppcb->PCB_NextPCB->PCB_PrevPCB = ppcb->PCB_PrevPCB;
    }

	ppcb->PCBState = GS_PCB_STATE_RUNNING;
    SAMPLE_FUNCTION_END(15)
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
INT32 gk_RCBASL_Link(G_RCB *presource, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(16)
    INT32 priority = psignal->SCBType;
    GS_SCB *psignal1;

    if (presource->RCB_NextRCBASL == (struct gs_scb *) 0)
    {
        // it is the only SCB associated to pevent
    	psignal->SCB_NextSCB = (struct gs_scb *) 0;
    	presource->RCB_NextRCBASL = (struct gs_scb *) psignal;
    }
    else
    {
        if(presource->RCB_NextRCBASL->SCBType < priority)
        {
            // it is the first SCB associated to pevent
            psignal->SCB_NextSCB = presource->RCB_NextRCBASL;
            presource->RCB_NextRCBASL = (struct gs_scb *) psignal;
        }
        else
        {
            // Sort by priotiy
            psignal1 = presource->RCB_NextRCBASL;
            while (psignal1 != (struct gs_scb *) 0)
            {
                if (psignal1->SCB_NextSCB == (struct gs_scb *) 0)
                {
                    // It is inserted as the last SCB
                    psignal->SCB_NextSCB = (struct gs_scb *) 0;
                    psignal1->SCB_NextSCB = (struct gs_scb *) psignal;
                    psignal1 = (struct gs_scb *) 0;
                }
                else
                {
                    if (psignal1->SCB_NextSCB->SCBType < priority)
                    {
                        psignal->SCB_NextSCB = psignal1->SCB_NextSCB;
                        psignal1->SCB_NextSCB = (struct gs_scb *) psignal;
                        psignal1 = (struct gs_scb *) 0;
                    }
                    else
                    {
                        // Keep searching
                        psignal1 = psignal1->SCB_NextSCB;
                    }
                }
            }
        }
    }
    psignal->SCBState = G_SCB_STATE_IN_RCB;
    SAMPLE_FUNCTION_END(16)
    return(G_TRUE);
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
INT32 gk_RCBASL_Unlink(G_RCB *presource, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(17)
	GS_SCB *psignal1;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal->SCBState == G_SCB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif

	if (presource->RCB_NextRCBASL != (struct gs_scb *) 0)
	{
		if (presource->RCB_NextRCBASL == psignal)
		{
			presource->RCB_NextRCBASL = psignal->SCB_NextSCB;
		}
		else
		{
			psignal1 = presource->RCB_NextRCBASL;
			while (psignal1 != (struct gs_scb *) 0)
			{
				if (psignal1->SCB_NextSCB == psignal)
				{
					psignal1->SCB_NextSCB = psignal->SCB_NextSCB;
					psignal1 = (struct gs_scb *) 0;
				}
				else
				{
					psignal1 = psignal1->SCB_NextSCB;
				}
			}
		}
		psignal->SCBState = G_SCB_STATE_UNLINKED;
	}

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal->SCBState != G_SCB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif
    SAMPLE_FUNCTION_END(17)
    return(G_TRUE);
}

/**gk_RCBFL_Link
 *  \brief 
 *  Links the RCB of a resource to Free List
 *  \param [in] presource Pointer to the RCB of the resource
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Return G_FALSE when it is not unused
 *  \relates Resource
 */
INT32 gk_RCBFL_Link(G_RCB *presource)
{
    SAMPLE_FUNCTION_BEGIN(18)
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (presource->RCBType != GK_RCB_TYPE_UNUSED) G_DEBUG_WHILEFOREVER;
#endif

    free(presource->malloc_address);
    g_kcb.KCB_NUMBER_OF_RCBs--; 
    
    SAMPLE_FUNCTION_END(18)
    return(G_TRUE);
}

// /**gk_RCB_GetFree
//  *  \brief 
//  *  Unlinks an RCB from the RCBFL list and returns its pointer or NULL if no free RCB is available
//  *  \return Pointer to the RCB or NULL when no RCB available
//  *  \relates Resource
//  */
// G_RCB *gk_RCB_GetFree(void)
// {
//     SAMPLE_FUNCTION_BEGIN(19)
// 
//     G_RCB *presource = gk_Get_RCB();
// 	if (presource != (G_RCB *) 0) {
//         presource->RCBType = GK_RCB_TYPE_UNUSED;
//     }
//     SAMPLE_FUNCTION_END(19)
// 	return(presource);
// }

/**gk_TASK_RELEASE
 *  \brief 
 *  Release a new invocation of the task. Preserves the Associated signal list.
 *  If task is not completed, then the Abort signal is triggered
 *  \param [in] ptcb Pointer to the TCB of the task to release
 *  \return G_TRUE if successful 
 *  \todo Check Ready and Running state to trigger the abort signal
 *  \relates Task
 */
INT32 gk_TASK_RELEASE(GS_TCB *ptcb) 
{
    SAMPLE_FUNCTION_BEGIN(20)
    GS_SCB *psignal = ptcb->TCB_NextTCBASL;             // Preserve the Associated Signal list
    unsigned int state = ptcb->TCBState;                // Preserve the currente state of task
    
    gk_TCB_Unlink(ptcb); 
    // Debo dejar la tarea como si recien se ejecutara
    ptcb->TCB_NextTCBASL = (struct gs_scb *) 0;
    gk_TCB_List_Unlink(ptcb);                  // Remove all the lists from the TCB
    gk_TASK_STK_Init(ptcb);                    // Initiates the stack of the task

    gk_TCBRDYL_Link(ptcb); PRINT_DEBUG_LINE
    ptcb->TCB_NextTCBASL = (struct gs_scb *) psignal;   // Put back the Associated Signal list

    // Check if there is G_SCB_TCB_ABORTED signal
    if (state == G_TASK_STATE_WAITING) {  // It is waiting not completed    
        psignal = gk_TCBASL_GetSCB(ptcb, G_SCB_TCB_ABORTED); PRINT_DEBUG_LINE
        if (psignal != (GS_SCB *) 0) {
            psignal = gk_SCB_Copy((GS_SCB *) psignal); PRINT_DEBUG_LINE
            gk_TCBPSL_Link(ptcb, (GS_SCB *) psignal); PRINT_DEBUG_LINE
            // fprintf(stderr,"[ MESSAGE ] SIGNAL next ASL of task 1 is %p copy is %p \n", (void *) ptcb->TCB_NextTCBASL, (void *) psignal);
        }
    }
    SAMPLE_FUNCTION_END(20)
    return(G_TRUE);
}

/**gk_RCBGEL_Link
 *  \brief 
 *  Links ECB to RCB granted list and return pointer to the ECB linked
 *  \param [in] presource Pointer to the RCB of the resource
 *  \param [in] pevent    Pointer to the ECB of the event. If NULL the a free ECB is chosen
 *  \return Pointer to ECB, NULL if no ECB is available
 *  \todo Check priority for different structures
 *  \relates Resource
 *  \sa Event
 */
GS_ECB *gk_RCBGEL_Link(G_RCB *presource, GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(21)
	GS_ECB *pevent1;

	if (pevent == (struct gs_ecb *) 0) pevent = gk_ECB_GetFree();
	if (pevent == (struct gs_ecb *) 0) {
        SAMPLE_FUNCTION_END(21)
        return(pevent);
    }

	#if G_DEBUG_WHILEFOREVER_ENABLE == 1
		if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
	#endif

    /* Set the priority of the event to link */ 
    // !!!! cambiar para prioridad de diferetnes structures
	if (pevent->ECB_RRDS != (struct gs_rrds *) 0) pevent->ECBValue.i64 = pevent->ECB_RRDS->RRDSGrantedPriority.i64;
	else pevent->ECBValue.i64 = (INT64) G_LOWEST_PRIORITY - (INT64) 100;

	/* Get the priority of the Event of highest priority */
	pevent1 = presource->RCB_NextRCBGEL;

	if (pevent1 == (struct gs_ecb *) 0)
	{
	    pevent->ECB_NextECB = (struct gs_ecb *)0;
	    pevent->ECB_PrevECB = (struct gs_ecb *)0;
	    presource->RCB_NextRCBGEL = (struct gs_ecb *) pevent;
	}
	else
	{
		if (pevent1->ECBValue.i64 > pevent->ECBValue.i64)
		{
			pevent->ECB_NextECB = (struct gs_ecb *) pevent1;
			pevent->ECB_NextECB->ECB_PrevECB = (struct gs_ecb *) pevent;
		    pevent->ECB_PrevECB = (struct gs_ecb *)0;
			presource->RCB_NextRCBGEL = (struct gs_ecb *) pevent;
		} else {
			while (pevent1 != (struct gs_ecb *) 0)
			{
				if (pevent1->ECBValue.i64 > pevent->ECBValue.i64)
				{
					pevent->ECB_NextECB = (struct gs_ecb *) pevent1;
					pevent->ECB_PrevECB = pevent1->ECB_PrevECB;
					pevent1->ECB_PrevECB->ECB_NextECB = (struct gs_ecb *) pevent;
					pevent1->ECB_PrevECB = (struct gs_ecb *) pevent;
					pevent1 = (GS_ECB *) 0;
				}
				else
				{
					if (pevent1->ECB_NextECB == (struct gs_ecb *) 0)
					{
						pevent->ECB_NextECB = (struct gs_ecb *) 0;
						pevent->ECB_PrevECB  = (struct gs_ecb *)pevent1;
						pevent1->ECB_NextECB = (struct gs_ecb *)pevent;
						pevent1 = (struct gs_ecb *) 0;
					}
					else
					{
						pevent1 = pevent1->ECB_NextECB;
					}
				}
			}
		}
	}
	pevent->ECB_AssocRCB = (struct g_rcb *) presource;
	pevent->ECBState = GS_ECB_STATE_GRANTED_RESOURCE;
    SAMPLE_FUNCTION_END(21)
    return((GS_ECB *)pevent);
}

/**gk_RCBGEL_Unlink
 *  \brief 
 *  Unlinks ECB from RCB granted list
 *  \param [in] pevent Pointer to the ECB ef the event
 *  \relates Event
 */ 
void  gk_RCBGEL_Unlink(GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(22)

	G_RCB *presource = (G_RCB *) pevent->ECB_AssocRCB;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (pevent->ECBState != GS_ECB_STATE_GRANTED_RESOURCE) G_DEBUG_WHILEFOREVER;
	if (ECB_IsValid(presource->RCB_NextRCBGEL) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

	/* Remove from the resource linked list */
	if (presource->RCB_NextRCBGEL == pevent)
	{
		presource->RCB_NextRCBGEL = pevent->ECB_NextECB;
	}
	else
	{
        pevent->ECB_PrevECB->ECB_NextECB = pevent->ECB_NextECB;
	}
    if (pevent->ECB_NextECB != (struct gs_ecb *) 0) pevent->ECB_NextECB->ECB_PrevECB = pevent->ECB_PrevECB;

	/* Set the pointer to unlinked */
	pevent->ECBState = GS_ECB_STATE_UNLINKED;
	pevent->ECB_PrevECB = (struct gs_ecb *) 0;
	pevent->ECB_NextECB = (struct gs_ecb *) 0;
    SAMPLE_FUNCTION_END(22)
}


/**gk_RCBWEL_Link
 *  \brief 
 *  Links an event ECB to the resource waiting list of RCB. If no ECB is given then a new ECB is obtained
 *  and the priority of the resource is given as the priority of the event
 *  \param [in] presource Pointer to the RCB of the resource
 *  \param [in] pevent    Pointer to the ECB of the event, NULL if an event should be return
 *  \return Return a pointer to the ECB of the event
 *  \todo Considered when no free ECB are available
 *  \relates Resource
 *  \sa Event
 */ 
GS_ECB *gk_RCBWEL_Link(G_RCB *presource, GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(23)
	GS_ECB *pevent1;

	// Get event if it is NULL
	if (pevent == (struct gs_ecb *) 0){
		pevent = gk_ECB_GetFree();
		if (pevent != (struct gs_ecb *) 0) pevent->ECB_RRDS = (struct gs_rrds *) 0;
	}
	if (pevent == (struct gs_ecb *) 0) G_DEBUG_WHILEFOREVER;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

	// Set the priority of the event
	if (pevent->ECB_RRDS != (struct gs_rrds *) 0) pevent->ECBValue.i64 = pevent->ECB_RRDS->RRDSWaitingPriority.i64;
	else pevent->ECBValue.i64 = presource->RCBPriority.i64;

	/* Get the priority of the Event of highest priority */
	//pevent1 = GetFirstEventWaitingForResource(presource);
	pevent1 = presource->RCB_NextRCBWEL;

	if (pevent1 == (struct gs_ecb *) 00)
	{
	    pevent->ECB_NextECB = (struct gs_ecb *) 00;
	    pevent->ECB_PrevECB = (struct gs_ecb *) 00;
	    presource->RCB_NextRCBWEL = (struct gs_ecb *) pevent;
	}
	else
	{
		if (pevent1->ECBValue.i64 > pevent->ECBValue.i64){
		    pevent->ECB_NextECB = presource->RCB_NextRCBWEL;
		    pevent->ECB_PrevECB = (struct gs_ecb *) 00;
		    presource->RCB_NextRCBWEL = (struct gs_ecb *) pevent;
		    pevent1->ECB_PrevECB = (struct gs_ecb *) pevent;
		}
		else
		{
			while (pevent1 != (struct gs_ecb *) 00){
				if (pevent1->ECB_NextECB == (struct gs_ecb *) 00) {
					pevent->ECB_NextECB = (struct gs_ecb *) 0;
					pevent->ECB_PrevECB = (struct gs_ecb *) pevent1;
					pevent1->ECB_NextECB = (struct gs_ecb *) pevent;
					pevent1 = (struct gs_ecb *) 0;
				}
				else
				{
					if (pevent1->ECB_NextECB->ECBValue.i64 > pevent->ECBValue.i64){
						pevent->ECB_NextECB = pevent1->ECB_NextECB;
						pevent->ECB_PrevECB = (struct gs_ecb *) pevent1;
						pevent->ECB_NextECB->ECB_PrevECB = (struct gs_ecb *) pevent;
						pevent1->ECB_NextECB = (struct gs_ecb *) pevent;
						pevent1 = (struct gs_ecb *) 0;
					}
					else
					{
						pevent1 = pevent1->ECB_NextECB;
					}
				}
			}
		}
	}
	pevent->ECBState = GS_ECB_STATE_WAITING_RESOURCE;
	pevent->ECB_AssocRCB = (struct g_rcb *) presource;
    SAMPLE_FUNCTION_END(23)
    return(pevent);
}

/**gk_RCBWEL_Unlink
 *  \brief 
 *  Unlinks ECB from RCB waiting list
 *  \param [in] pevent Pointer to the ECB of the event
 *  \details 
 *  When starts:
 *  - pevent->ECBState != GS_ECB_STATE_UNLINKED
 *  When finishes:
 *  - pevent->ECBState = GS_ECB_STATE_UNLINKED 
 *  \relates Event
 */ 
void gk_RCBWEL_Unlink(GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(24)
	G_RCB *presource = (G_RCB *) pevent->ECB_AssocRCB;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (pevent->ECBState == GS_ECB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif

	/* Remove from the resource linked list */
	if (presource->RCB_NextRCBWEL == pevent)
	{
		presource->RCB_NextRCBWEL = pevent->ECB_NextECB;
		presource->RCB_NextRCBWEL->ECB_PrevECB = (struct gs_ecb *) 0;
	} else
	{
        pevent->ECB_PrevECB->ECB_NextECB = pevent->ECB_NextECB;
        if (pevent->ECB_NextECB != (struct gs_ecb *) 0) {
            pevent->ECB_NextECB->ECB_PrevECB = pevent->ECB_PrevECB;
        }
	}
	/* Set the pointer to unlinked */
	pevent->ECBState = GS_ECB_STATE_UNLINKED;
	pevent->ECB_PrevECB = (struct gs_ecb *) 0;
	pevent->ECB_NextECB = (struct gs_ecb *) 0;
    SAMPLE_FUNCTION_END(24)
}


/**gk_SCBFL_Link
 *  \brief 
 *  Links a SCB to the Free List
 *  \param [in] psignal Pointer to the SCB to be linked
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Check is ECB signal is ready for linking to free list without links.
 *  \relates Signal
 */ 
INT32 gk_SCBFL_Link(GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(25)

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal->SCBState != G_SCB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif

    free(psignal->malloc_address);
    g_kcb.KCB_NUMBER_OF_SCBs--; 
    
    SAMPLE_FUNCTION_END(25)
    return(G_TRUE);
}


/**gk_SCB_GetFree
 *  \brief 
 *  Unlinks a SCB from free list and return its pointer
 *  \return Pointer to the SCB or NULL if no free SCB is available 
 *  \relates Signal
 */
GS_SCB *gk_SCB_GetFree(void)
{
    SAMPLE_FUNCTION_BEGIN(26)
	GS_SCB *psignal;

    psignal = gk_Get_SCB();

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal == (struct gs_scb *) 0) G_DEBUG_WHILEFOREVER;
#endif

	if (psignal != (struct gs_scb *) 0){
		psignal->SCBState = G_SCB_STATE_UNLINKED;
	}
    SAMPLE_FUNCTION_END(26)
	return(psignal);
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
    SAMPLE_FUNCTION_BEGIN(27)
    GS_SCB *psignal1 = gk_SCB_GetFree();
    
    if (psignal1 != (struct gs_scb *) 0){
        psignal1->SCBState         = psignal->SCBState;
        psignal1->SCBType          = psignal->SCBType;
        psignal1->SCBPriority      = psignal->SCBPriority;
        psignal1->SCB_TaskCode     = psignal->SCB_TaskCode;
        psignal1->SCB_TaskArg      = psignal->SCB_TaskArg;
        psignal1->SCB_NextSCB      = (struct gs_scb *) 0;
        psignal1->SCB_NextSCBAPSL  = (struct gs_scb *) 0;
        psignal1->SCB_AssocXCB     = psignal->SCB_AssocXCB;
    }
    SAMPLE_FUNCTION_END(27)
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
    SAMPLE_FUNCTION_BEGIN(28)
	pscb_pending->SCB_NextSCBAPSL = pscb_root->SCB_NextSCBAPSL;
	pscb_root = pscb_pending;
    SAMPLE_FUNCTION_END(28)
}

/**gk_SCBAPSL_UnLink
 *  \brief 
 *  Unlinks a pending signal from the root signal
 *  \param [in] pscb_root    Pointer to the ROOT SCB
 *  \param [in] pscb_pending Pointer to the Pending SCB
 *  \return G_TRUE if successfull, G_FALSE if pending is executing
 *  \relates Signal
 */
INT32 gk_SCBAPSL_UnLink(GS_SCB *pscb_root, GS_SCB *pscb_pending)
{
    SAMPLE_FUNCTION_BEGIN(29)
	GS_SCB * pscb;
	if (pscb_pending->SCBState == G_SCB_STATE_EXECUTING) return(G_FALSE);

	if (pscb_root->SCB_NextSCBAPSL == pscb_pending)
	{
		pscb_root->SCB_NextSCBAPSL = pscb_pending->SCB_NextSCBAPSL;
	}
	else
	{
		pscb = pscb_root->SCB_NextSCBAPSL;
		while (pscb != (struct gs_scb *) 0)
		{
			if (pscb->SCB_NextSCBAPSL == pscb_pending)
			{
				pscb->SCB_NextSCBAPSL = pscb_pending->SCB_NextSCBAPSL;
				pscb = (struct gs_scb *) 0;
			}
			else
			{
				pscb = pscb->SCB_NextSCBAPSL;
			}
		}
	}
    SAMPLE_FUNCTION_END(29)
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
INT32 gk_TCBAEL_Link(GS_ECB *pevent, GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(30)
	#if G_DEBUG_WHILEFOREVER_ENABLE == 1
		if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
		if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER;
	#endif

    pevent->ECB_AssocTCB = (struct gs_tcb *) ptcb; /* Associate EVENT to the Task            */

    pevent->ECB_NextTCBAEL = ptcb->TCB_NextTCBAEL; /* Associate TASK to EVENT         */
    pevent->ECB_PrevTCBAEL = (struct gs_ecb *) 0;
    if (ptcb->TCB_NextTCBAEL != (struct gs_ecb *) 0) ptcb->TCB_NextTCBAEL->ECB_PrevTCBAEL = (struct gs_ecb *) pevent;
    ptcb->TCB_NextTCBAEL = (struct gs_ecb *) pevent;
    SAMPLE_FUNCTION_END(30)
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
INT32 gk_TCBAEL_Unlink(GS_ECB *pevent)
{
    SAMPLE_FUNCTION_BEGIN(31)
    int retorno = G_FALSE;
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (ECB_IsValid(pevent) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif
    if (pevent->ECB_AssocTCB != (GS_TCB *)0)
    {
        if (pevent->ECB_PrevTCBAEL == (struct gs_ecb *) 0) pevent->ECB_AssocTCB->TCB_NextTCBAEL = pevent->ECB_NextTCBAEL;
        else pevent->ECB_PrevTCBAEL->ECB_NextTCBAEL = pevent->ECB_NextTCBAEL;
        if (pevent->ECB_NextTCBAEL != (struct gs_ecb *) 00) pevent->ECB_NextTCBAEL->ECB_PrevTCBAEL = pevent->ECB_PrevTCBAEL;

        /* Set the event unlink from task list */
        pevent->ECB_PrevTCBAEL = (struct gs_ecb *) 0;
        pevent->ECB_NextTCBAEL = (struct gs_ecb *) 0;
        pevent->ECB_AssocTCB   = (struct gs_tcb *) 0;
        retorno = G_TRUE;
    }
    SAMPLE_FUNCTION_END(31)
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
INT32 gk_TCBASL_Link(GS_TCB *ptcb, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(32)
    INT32 priority = psignal->SCBType;
    GS_SCB *psignal1;

    if (ptcb->TCB_NextTCBASL == (struct gs_scb *) 0)
    {
        // it is the only SCB associated to pevent
    	psignal->SCB_NextSCB  = (struct gs_scb *) 0;
    	ptcb->TCB_NextTCBASL  = (struct gs_scb *) psignal;
    }
    else
    {
        if(ptcb->TCB_NextTCBASL->SCBType < priority)
        {
            // it is the first SCB associated to pevent
            psignal->SCB_NextSCB = ptcb->TCB_NextTCBASL;
            ptcb->TCB_NextTCBASL = (struct gs_scb *) psignal;
        }
        else
        {
            // Sort by priotiy
            psignal1 = ptcb->TCB_NextTCBASL;
            while (psignal1 != (struct gs_scb *) 0)
            {
                if (psignal1->SCB_NextSCB == (struct gs_scb *) 0)
                {
                    // It is inserted as the last SCB
                    psignal->SCB_NextSCB = (struct gs_scb *) 0;
                    psignal1->SCB_NextSCB = (struct gs_scb *) psignal;
                    psignal1 = (struct gs_scb *) 0;
                }
                else
                {
                    if (psignal1->SCB_NextSCB->SCBType > priority)
                    {
                        psignal->SCB_NextSCB  = psignal1->SCB_NextSCB;
                        psignal1->SCB_NextSCB = (struct gs_scb *) psignal;
                        psignal1 = (struct gs_scb *) 0;
                    }
                    else
                    {
                        // Keep searching
                        psignal1 = psignal1->SCB_NextSCB;
                    }
                }
            }
        }
    }
	psignal->SCBState = G_SCB_STATE_IN_TCB;
    SAMPLE_FUNCTION_END(32)
    return(G_TRUE);
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
INT32  gk_TCBASL_Unlink(GS_TCB *ptcb, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(33)
	GS_SCB *psignal1;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal->SCBState == G_SCB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif

	if (ptcb->TCB_NextTCBASL != (struct gs_scb *) 0)
	{
		if (ptcb->TCB_NextTCBASL == psignal)
		{
			ptcb->TCB_NextTCBASL = psignal->SCB_NextSCB;
		}
		else
		{
			psignal1 = ptcb->TCB_NextTCBASL;
			while (psignal1 != (struct gs_scb *) 0)
			{
				if (psignal1->SCB_NextSCB == psignal)
				{
					psignal1->SCB_NextSCB = psignal->SCB_NextSCB;
					psignal1 = (struct gs_scb *) 0;
				}
				else
				{
					psignal1 = psignal1->SCB_NextSCB;
				}
			}
		}
		psignal->SCBState = G_SCB_STATE_UNLINKED;
	}

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal->SCBState != G_SCB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif
    SAMPLE_FUNCTION_END(33)
    return G_TRUE;
}

/**gk_TCB_List_Unlink
 *  \brief 
 *  Unlinks the TCB from events and signals and links them to the free lists respectively
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successfull, G_FALSE otherwise
 *  \todo Check unlink for different resources (semaphore and messages) when they are granted
 *  \relates Task
 */
INT32 gk_TCB_List_Unlink(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(34)
	GS_SCB *psignal1;
	GS_ECB *pevent1;
    
    // GS_ECB *pevent;
    // GS_ECB *peventime;
	
	// Remove SCB from ASL
	while (ptcb->TCB_NextTCBASL != (struct gs_scb *) 0)
	{
		psignal1 = ptcb->TCB_NextTCBASL;
		gk_TCBASL_Unlink(ptcb, psignal1);
		gk_SCBFL_Link(psignal1);
	}	

	// Remove SCB from PSL
	while (ptcb->TCB_NextTCBPSL != (struct gs_scb *) 0)
	{
		psignal1 = ptcb->TCB_NextTCBPSL;
		gk_TCBPSL_Unlink(ptcb, psignal1);
		gk_SCBFL_Link(psignal1);
	}
	
    // Remove ECB from AEL
    // UNGRANT and UNWAIT resouce related event
    pevent1 = ptcb->TCB_NextTCBAEL;
	while (pevent1 != (struct gs_ecb *) 0)
	{
        gk_RESOURCE_ECB_KILL_CALLBACK((GS_ECB *) pevent1);
        pevent1 = ptcb->TCB_NextTCBAEL;
    }
    SAMPLE_FUNCTION_END(34)
    return(G_TRUE);
}

/**gk_TCBFL_Link
 *  \brief 
 *  Links a TCB to the Free TCB List removing associated and pending SCBs and ECBs
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \relates Task
 */ 
INT32 gk_TCBFL_Link(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(35)
	// GS_SCB *psignal1;
    
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (ptcb->TCBState != G_TASK_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif

	/* Remove the links from TCB        */
    gk_TCB_List_Unlink(ptcb);

    free(ptcb->malloc_address);
    g_kcb.KCB_NUMBER_OF_TCBs--; 
    
	G_DEBUG_VERBOSE

    SAMPLE_FUNCTION_END(35)
    return(G_TRUE);
}

/**gk_TCB_GetFree
 *  \brief 
 *  Returns a pointer to a Free TCB, NULL if it is not a TCB available
 *  \return A pointer to the TCB or NULL if no TCB is available
 *  \relates Task
 */ 
GS_TCB *gk_TCB_GetFree(void)
{
    SAMPLE_FUNCTION_BEGIN(36)
	GS_TCB *ptcb;

	G_DEBUG_VERBOSE

	// ptcb = g_kcb.KCB_NextTCBFL;
    ptcb = gk_Get_TCB();

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (ptcb->TCBState != G_TASK_STATE_FREE || ptcb == (struct gs_tcb *) 0) G_DEBUG_WHILEFOREVER;
#endif

	if (ptcb != (struct gs_tcb *) 0){
	    ptcb->TCBState = G_TASK_STATE_UNLINKED;
	    // g_kcb.KCB_NextTCBFL = (struct gs_tcb *) ptcb->TCB_NextTCB;
        
        ptcb->TCB_AssocPCB   = 0;                      /// It is not assigned at the beginnig

        ptcb->TCB_NextTCBAEL = (struct gs_ecb *) 0;    /* No associated events  */
        ptcb->TCB_NextTCB    = (struct gs_tcb *) 0;    /* Task not linked       */
        ptcb->TCB_PrevTCB    = (struct gs_tcb *) 0;
        ptcb->TCB_NextTCBPSL = (struct gs_scb *) 0;    /* No pending signal     */
	}
    SAMPLE_FUNCTION_END(36)
	return(ptcb);
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
INT32  gk_TCBPSL_Link(GS_TCB *ptcb, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(37)
    INT32 priority;
    GS_SCB *psignal1;

	priority = psignal->SCBPriority;
    if ((ptcb->TCB_NextTCBPSL == (struct gs_scb *) 0))
    {
    	psignal->SCB_NextSCB = (struct gs_scb *) 0;
    	ptcb->TCB_NextTCBPSL = (struct gs_scb *) psignal;
    }
    else
    {
        if (ptcb->TCB_NextTCBPSL->SCBPriority > priority)
        {
        	psignal->SCB_NextSCB = ptcb->TCB_NextTCBPSL;
        	ptcb->TCB_NextTCBPSL = (struct gs_scb *) psignal;
        }
		else
		{
            psignal1 = ptcb->TCB_NextTCBPSL;
            while (psignal1 != (struct gs_scb *) 0)
            {
                if (psignal1->SCB_NextSCB == (struct gs_scb *) 0)
                {
                    // It is inserted as the last SCB
                    psignal->SCB_NextSCB = (struct gs_scb *) 0;
                    psignal1->SCB_NextSCB = (struct gs_scb *) psignal;
                    psignal1 = (struct gs_scb *) 0;
                }
                else
                {
                    if (psignal1->SCB_NextSCB->SCBPriority > priority)
                    {
                        psignal->SCB_NextSCB = psignal1->SCB_NextSCB;
                        psignal1->SCB_NextSCB = (struct gs_scb *) psignal;
                        psignal1 = (struct gs_scb *) 0;
                    }
                    else
                    {
                        // Keep searching
                        psignal1 = psignal1->SCB_NextSCB;
                    }
                }
            }
		}
    }
	psignal->SCBState = G_SCB_STATE_PENDING;
    SAMPLE_FUNCTION_END(37)
    return(G_TRUE);
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
INT32  gk_TCBPSL_Unlink(GS_TCB *ptcb, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(38)
	GS_SCB *psignal1;
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal->SCBState == G_SCB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif
	if (ptcb->TCB_NextTCBPSL != (struct gs_scb *) 0)
	{
		if (ptcb->TCB_NextTCBPSL == psignal)
		{
			ptcb->TCB_NextTCBPSL = psignal->SCB_NextSCB;
		}
		else
		{
			psignal1 = ptcb->TCB_NextTCBPSL;
			while (psignal1 != (struct gs_scb *) 0)
			{
				if (psignal1->SCB_NextSCB == psignal)
				{
					psignal1->SCB_NextSCB = psignal->SCB_NextSCB;
					psignal1 = (struct gs_scb *) 0;
				}
				else
				{
					psignal1 = psignal1->SCB_NextSCB;
				}
			}
		}
		psignal->SCBState = G_SCB_STATE_UNLINKED;
	}
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal->SCBState != G_SCB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif
    SAMPLE_FUNCTION_END(38)
    return G_TRUE;

}

/**gk_TCBRDYL_Link
 *  \brief 
 *  Links the TCB in the Ready Task List sorted by its priority
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Implement G_FALSE return
 *  \relates Task
 */ 
INT32 gk_TCBRDYL_Link(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(39)
	GS_TCB    *ptcb1;

    GS_LCB *readylist = ptcb->TCB_RDY_LCB_Index; PRINT_DEBUG_LINE
    // int i; PRINT_DEBUG_LINE

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
	if (ptcb->TCBState != G_TASK_STATE_UNLINKED) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
#endif
    G_DEBUG_VERBOSE
	gk_TASK_PRIORITY_SET(ptcb, G_TASK_STATE_READY); PRINT_DEBUG_LINE

	if (ptcb->TCBType != G_TASK_TYPE_IDLE)   /* Idle task are not inserted in Ready Lists */
	{
		if (readylist->LCB_NextTCBRDYL == (struct gs_tcb *) 0)
		{// It is the first TCB to insert
			ptcb->TCB_NextTCB = (struct gs_tcb *) 0; PRINT_DEBUG_LINE
			ptcb->TCB_PrevTCB = (struct gs_tcb *) 0; PRINT_DEBUG_LINE
			readylist->LCB_NextTCBRDYL = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
		}
		else
		{
			if (readylist->LCB_NextTCBRDYL->TCBCurrentPriority > ptcb->TCBCurrentPriority) {   /* Task inserted is the highest priority one */
				ptcb->TCB_NextTCB = readylist->LCB_NextTCBRDYL ; PRINT_DEBUG_LINE
				ptcb->TCB_PrevTCB = (struct gs_tcb *) 0; PRINT_DEBUG_LINE
				ptcb->TCB_NextTCB->TCB_PrevTCB = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
				readylist->LCB_NextTCBRDYL = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
			} else {
				ptcb1 = readylist->LCB_NextTCBRDYL; PRINT_DEBUG_LINE
				while (ptcb1 != (struct gs_tcb *) 0)
				{
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb1) != G_TRUE) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
#endif
					if (ptcb1->TCB_NextTCB == (struct gs_tcb *) 0){    /* It has to be inserted as the last element */
						ptcb->TCB_NextTCB  = (struct gs_tcb *) 0; PRINT_DEBUG_LINE
						ptcb->TCB_PrevTCB  = (struct gs_tcb *) ptcb1; PRINT_DEBUG_LINE
						ptcb1->TCB_NextTCB = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
						ptcb1 = (struct gs_tcb *) 0; PRINT_DEBUG_LINE /* to exit the while */
					}
					else
					{
						if (ptcb1->TCB_NextTCB->TCBCurrentPriority > ptcb->TCBCurrentPriority){
							/* It has to be inserted before next */
							ptcb->TCB_NextTCB = ptcb1->TCB_NextTCB; PRINT_DEBUG_LINE
							ptcb->TCB_PrevTCB = (struct gs_tcb *) ptcb1; PRINT_DEBUG_LINE
							ptcb->TCB_NextTCB->TCB_PrevTCB = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
							ptcb1->TCB_NextTCB = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
							ptcb1 = (struct gs_tcb *) 0; PRINT_DEBUG_LINE /* to exit the while */
						}
						else
						{
							ptcb1 = (GS_TCB *) ptcb1->TCB_NextTCB; PRINT_DEBUG_LINE
						}
					}
				}
			}
		}
	}
	ptcb->TCB_AssocPCB = (INT32) 0; PRINT_DEBUG_LINE
	ptcb->TCBState = G_TASK_STATE_READY; PRINT_DEBUG_LINE
	// Check if Inversion occurs !!!!!!!!!!!
    /// If there exist a free processor, then trigger its interrupt
    if (readylist->LCB_NextLCBFPL != (struct gs_pcb *) 0)
    {
        if (readylist->LCB_NextLCBFPL->PCBID != GRTOS_CMD_PRC_ID )	{
            // fprintf(stderr, "[ OK ] Processor %d trigger proc %d in %s, %d\n", GRTOS_CMD_PRC_ID, i, __FUNCTION__,__LINE__);
            GRTOS_CMD_PRC_INT(readylist->LCB_NextLCBFPL->PCBID); 
        }
    }
	//gk_LCB_CheckInvertion(readylist); PRINT_DEBUG_LINE
	G_DEBUG_VERBOSE; PRINT_DEBUG_LINE // Print Debug information 
    SAMPLE_FUNCTION_END(39)
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
INT32 gk_TCBRDYL_Unlink(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(40)
	GS_LCB *readylist = ptcb->TCB_RDY_LCB_Index; PRINT_DEBUG_LINE

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
	if ((TCB_IsValid(readylist->LCB_NextTCBRDYL) != G_TRUE && readylist->LCB_NextTCBRDYL != 0) ||
			(readylist->LCB_NextTCBRDYL == (struct gs_tcb *) 0 && ptcb->TCBType != G_TASK_TYPE_IDLE)) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
    if (ptcb->TCBState != G_TASK_STATE_READY) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
#endif

    if (ptcb->TCBType != G_TASK_TYPE_IDLE)  /* Task Idle is not inserted in ready lists */
    {
		if (readylist->LCB_NextTCBRDYL == ptcb) /* It is the first ready task */
		{
			readylist->LCB_NextTCBRDYL = (struct gs_tcb *) ptcb->TCB_NextTCB; PRINT_DEBUG_LINE
			if (ptcb->TCB_NextTCB != (GS_TCB *)0) ptcb->TCB_NextTCB->TCB_PrevTCB = (struct gs_tcb *) 0; PRINT_DEBUG_LINE
		}
		else
		{/* It is not the first one */
			if (ptcb->TCB_NextTCB != (GS_TCB *)0) ptcb->TCB_NextTCB->TCB_PrevTCB = ptcb->TCB_PrevTCB; PRINT_DEBUG_LINE
			ptcb->TCB_PrevTCB->TCB_NextTCB = ptcb->TCB_NextTCB; PRINT_DEBUG_LINE
		}
    }

	ptcb->TCBState = G_TASK_STATE_UNLINKED; PRINT_DEBUG_LINE

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(readylist->LCB_NextTCBRDYL) != G_TRUE &&
			readylist->LCB_NextTCBRDYL != (struct gs_tcb *) 0) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
#endif
	// Check if Inversion occurs  !!!!
	//gk_LCB_CheckInvertion(readylist); PRINT_DEBUG_LINE
    SAMPLE_FUNCTION_END(40)
    return(G_TRUE);
}

/**gk_TCBRUNL_Link
 *  \brief 
 *  Links the TCB to the Run Task List
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE if successful, G_FALSE otherwise
 *  \todo Implement G_FALSE return
 *  \relates Task
 */ 
INT32  gk_TCBRUNL_Link(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(41)
	GS_TCB *ptcb1;
    GS_LCB *RunList = ptcb->TCB_RDY_LCB_Index; PRINT_DEBUG_LINE
    ptcb1 = RunList->LCB_NextTCBRUNL; PRINT_DEBUG_LINE


#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb1) != G_TRUE && ptcb1 != (struct gs_tcb *) 0) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
	if ((TCB_IsValid(ptcb) != G_TRUE) && (ptcb != (struct gs_tcb *) 0))G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
    G_DEBUG_VERBOSE
	if (ptcb->TCBState != G_TASK_STATE_UNLINKED) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
#endif
    
    
	gk_TASK_PRIORITY_SET(ptcb, G_TASK_STATE_RUNNING); PRINT_DEBUG_LINE
    
	/* Set the current processor as the processor associated  of the task */
	ptcb->TCB_AssocPCB = (INT32) GRTOS_CMD_PRC_ID; PRINT_DEBUG_LINE

	if (RunList->LCB_NextTCBRUNL == (struct gs_tcb *) 0)  /* It is the first element in the running list */
	{
		RunList->LCBRunPriority = ptcb->TCBCurrentPriority; PRINT_DEBUG_LINE
		RunList->LCB_NextTCBRUNL = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
		ptcb->TCB_NextTCB = (struct gs_tcb *) 0; PRINT_DEBUG_LINE
		ptcb->TCB_PrevTCB = (struct gs_tcb *) 0; PRINT_DEBUG_LINE 
		gk_LCBL_UnLink(RunList); PRINT_DEBUG_LINE  /* Unlink the LCB and link again to update the priority */
		gk_LCBL_Link(RunList); PRINT_DEBUG_LINE
	}
	else
	{
		ptcb1 = RunList->LCB_NextTCBRUNL; PRINT_DEBUG_LINE
		if (gk_TCBLowerPriorityThanTCB(ptcb, ptcb1) == G_TRUE)    /* It has to be inserted as first element */
		{
			RunList->LCBRunPriority  = ptcb->TCBCurrentPriority; PRINT_DEBUG_LINE
			ptcb->TCB_NextTCB = RunList->LCB_NextTCBRUNL; PRINT_DEBUG_LINE
			ptcb->TCB_PrevTCB = (struct gs_tcb *) 0; PRINT_DEBUG_LINE
			ptcb->TCB_NextTCB->TCB_PrevTCB = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
			RunList->LCB_NextTCBRUNL = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
			gk_LCBL_UnLink(RunList); PRINT_DEBUG_LINE   /* Unlink the LCB and link again to update the priority */
			gk_LCBL_Link(RunList); PRINT_DEBUG_LINE
		}
		else
		{
			/* Search the place in the list to insert ptcb */
			while (ptcb1 != (struct gs_tcb *) 0)
			{
				if (ptcb1->TCB_NextTCB == (struct gs_tcb *) 0) {    /* It has to be inserted as the last element */
					ptcb->TCB_NextTCB = (struct gs_tcb *) 0; PRINT_DEBUG_LINE
					ptcb->TCB_PrevTCB  = (struct gs_tcb *)  ptcb1; PRINT_DEBUG_LINE
					ptcb1->TCB_NextTCB = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
					ptcb1 = (struct gs_tcb *) 0; PRINT_DEBUG_LINE /* to exit the while */
				}
				else
				{
					if (gk_TCBLowerPriorityThanTCB(ptcb, (GS_TCB *) ptcb1->TCB_NextTCB) == G_TRUE) {  /* It has to be inserted before next */
						ptcb->TCB_NextTCB = ptcb1->TCB_NextTCB; PRINT_DEBUG_LINE
						ptcb->TCB_PrevTCB = (struct gs_tcb *) ptcb1; PRINT_DEBUG_LINE
						ptcb->TCB_NextTCB->TCB_PrevTCB = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
						ptcb1->TCB_NextTCB = (struct gs_tcb *) ptcb; PRINT_DEBUG_LINE
						ptcb1 = (struct gs_tcb *) 0; PRINT_DEBUG_LINE /* to exit the while */
					}
					else
					{
						ptcb1 = (GS_TCB *) ptcb1->TCB_NextTCB; PRINT_DEBUG_LINE
					}
				}
			}
		}
	}

    ptcb->TCBState = G_TASK_STATE_RUNNING; PRINT_DEBUG_LINE
	// Check if Inversion occurs !!!!!!!!!
	//gk_LCB_CheckInvertion(RunList); PRINT_DEBUG_LINE
    // Si no esta en la lista principal del procesador o es tarea idle, el procesador es puesto como libre
	if (ptcb->TCB_RDY_LCB_Index != g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCB_RDY_LCBL[0] || ptcb->TCBType == G_TASK_TYPE_IDLE)
	{
		if (g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCBState != GS_PCB_STATE_FREE)
	        gk_LCBFPL_Link(ptcb->TCB_AssocPCB); PRINT_DEBUG_LINE // Link the processor to the free list
	}

    // Si esta en la lista de ready principal del procesador, entocnes es sacado de la lista de libres
	if (ptcb->TCB_RDY_LCB_Index == g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCB_RDY_LCBL[0])
	{
		if (g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCBState == GS_PCB_STATE_FREE)
		    gk_LCBFPL_Unlink(ptcb->TCB_AssocPCB); PRINT_DEBUG_LINE // Unlink the processor from the free list
	}
    
    /***********************************************/
    /* Set the processor to ptcb                   */
    	G_DEBUG_VERBOSE
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	int i;
	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB = (struct gs_tcb *) 0; /* just to check no duplication */
	if (TCB_IsValid(ptcb) != G_TRUE) {PRINT_TO_DEBUG("ERROR TCB= %p\n",ptcb); G_DEBUG_WHILEFOREVER;}
	for (i=0; i<G_NUMBER_OF_PCB;i++){
		if (g_kcb.G_PCBTbl[i].PCB_EXECTCB == ptcb) {
            PRINT_TO_DEBUG("ERROR %s, %s, %d, pi %x, tcb %p, ptcb %p, proc %x, mtx %x\n",__FUNCTION__,__FILE__,__LINE__, i, g_kcb.G_PCBTbl[i].PCB_EXECTCB, ptcb, GRTOS_CMD_PRC_ID, IORD_GRTOS_MTX_RQS);            
            PRINT_TO_DEBUG("ERROR %s, %s, %d, pi %x, tcb %p, ptcb %p, proc %x, mtx %x\n",__FUNCTION__,__FILE__,__LINE__, i, g_kcb.G_PCBTbl[i].PCB_EXECTCB, ptcb, GRTOS_CMD_PRC_ID, IORD_GRTOS_MTX_RQS);           
            G_DEBUG_WHILEFOREVER;
        }
	}
	if ((ptcb->TCB_StackPointer < ptcb->TCB_StackTop - 300) || (ptcb->TCB_StackPointer > ptcb->TCB_StackBottom)) {
        PRINT_TO_DEBUG("ERROR TCB= %p, TCB_StackBottom = %p, TCB_StackPointer = %p, TCB_StackTop = %p\n",ptcb, (void *) ptcb->TCB_StackBottom, (void *) ptcb->TCB_StackPointer, (void *)  ptcb->TCB_StackTop); 
        PRINT_TO_DEBUG("ERROR TCB_IDLE= %p\n", (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB ); 
        G_DEBUG_WHILEFOREVER; }
#endif

	g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB = (struct gs_tcb *) ptcb;
    ptcb->TCB_AssocPCB = GRTOS_CMD_PRC_ID;
    if (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_RDY_LCBL[0] == ptcb->TCB_RDY_LCB_Index) { /* processor is not idle */
        gk_LCBFPL_Unlink(GRTOS_CMD_PRC_ID);
    }
    else { /* processor is idle */
        gk_LCBFPL_Link(GRTOS_CMD_PRC_ID);
    }
    SAMPLE_FUNCTION_END(41)
    return(G_TRUE);
}

/**gk_TCBRUNL_Unlink
 *  \brief 
 *  Unlinks TCB from Running List
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Include G_FALSE when TCB is not in RUN list
 *  \relates Task
 */
INT32  gk_TCBRUNL_Unlink(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(42)
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
	if (ptcb->TCBState != G_TASK_STATE_RUNNING) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
	if (ptcb->TCBState == G_TASK_STATE_UNLINKED) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
#endif

    GS_LCB *RunList = ptcb->TCB_RDY_LCB_Index; PRINT_DEBUG_LINE

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
    if (TCB_IsValid(RunList->LCB_NextTCBRUNL) != G_TRUE) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
	if (ptcb->TCB_AssocPCB < 1 || ptcb->TCB_AssocPCB > G_NUMBER_OF_PCB) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
#endif


    /// Stop the processor executing running task triggering its interrupt
    if (ptcb->TCB_AssocPCB != GRTOS_CMD_PRC_ID ) {
        // fprintf(stderr, "[ OK ] Processor %d trigger proc %d in %s, %d\n", GRTOS_CMD_PRC_ID, ptcb->TCB_AssocPCB, __FUNCTION__,__LINE__);
        GRTOS_CMD_PRC_INT(ptcb->TCB_AssocPCB);
    }
    if (g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCBState != GS_PCB_STATE_FREE)
    	gk_LCBFPL_Link(ptcb->TCB_AssocPCB); PRINT_DEBUG_LINE // Link processor to free list


	/* Runs in background or it is an IDLE TCB, then unlink processor from Free List */
	//if (ptcb->TCB_RDY_LCB_Index != g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCB_RDY_LCBL[0] ||
	//		ptcb->TCBType == G_TASK_TYPE_IDLE) gk_LCBFPL_Unlink(ptcb->TCB_AssocPCB); PRINT_DEBUG_LINE


    if (RunList->LCB_NextTCBRUNL == ptcb){ // checks if it is the first TCP in RUNNING LIST
    	RunList->LCB_NextTCBRUNL = (GS_TCB *) ptcb->TCB_NextTCB; PRINT_DEBUG_LINE
    	if (RunList->LCB_NextTCBRUNL != (struct gs_tcb *) 0) /* It is not the only element */
    	{
    		RunList->LCB_NextTCBRUNL->TCB_PrevTCB = (struct gs_tcb *) 0; PRINT_DEBUG_LINE
    		RunList->LCBRunPriority = ptcb->TCBCurrentPriority; PRINT_DEBUG_LINE
    	}
    	else
    	{/* The list will be empty therefore lowest priority */
    		RunList->LCBRunPriority = G_LOWEST_PRIORITY; PRINT_DEBUG_LINE
    	}
		/* Unlink the LCB and link again to update the priority */
    	gk_LCBL_UnLink(RunList); PRINT_DEBUG_LINE
    	gk_LCBL_Link(RunList); PRINT_DEBUG_LINE
    }
    else
    {
    	/* It is not the first element in the list */
		ptcb->TCB_PrevTCB->TCB_NextTCB = ptcb->TCB_NextTCB; PRINT_DEBUG_LINE
		if (ptcb->TCB_NextTCB != (GS_TCB *)0) ptcb->TCB_NextTCB->TCB_PrevTCB = ptcb->TCB_PrevTCB; PRINT_DEBUG_LINE
    }

    // g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCB_EXECTCB = g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCB_IDLETCB; PRINT_DEBUG_LINE 
    // g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCB_IDLETCB->TCBState = G_TASK_STATE_RUNNING; PRINT_DEBUG_LINE 
    
    /* Desassociate the task from the processor */    
    g_kcb.G_PCBTbl[ptcb->TCB_AssocPCB-1].PCB_EXECTCB = (struct gs_tcb *) 0;
	ptcb->TCB_AssocPCB = (INT32) 0; PRINT_DEBUG_LINE
	ptcb->TCBState = G_TASK_STATE_UNLINKED; PRINT_DEBUG_LINE
	// Check if Inversion occurs
	//gk_LCB_CheckInvertion(RunList); PRINT_DEBUG_LINE
	SAMPLE_FUNCTION_END(42)
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
INT32  gk_TCBWL_Link(GS_TCB *ptcb, unsigned int state)
{
    SAMPLE_FUNCTION_BEGIN(43)
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (ptcb->TCBState != G_TASK_STATE_UNLINKED)G_DEBUG_WHILEFOREVER;
#endif

    // int i; 

    /* Insert the task in the Waiting List   */
    ptcb->TCB_NextTCB = g_kcb.KCB_NextTCBWL;
    ptcb->TCB_PrevTCB = (struct gs_tcb *) 0;

    if (g_kcb.KCB_NextTCBWL != (struct gs_tcb *) 0) g_kcb.KCB_NextTCBWL->TCB_PrevTCB = (struct gs_tcb *) ptcb;

    g_kcb.KCB_NextTCBWL = (struct gs_tcb *) ptcb;
    ptcb->TCBState = state;
    SAMPLE_FUNCTION_END(43)
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
INT32 gk_TCBWL_Unlink(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(44)
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (!GRTOS_TASK_STATE_WAITING(ptcb)) G_DEBUG_WHILEFOREVER;
#endif

    if (g_kcb.KCB_NextTCBWL == ptcb) g_kcb.KCB_NextTCBWL = (GS_TCB *) ptcb->TCB_NextTCB;
    else ptcb->TCB_PrevTCB->TCB_NextTCB = ptcb->TCB_NextTCB;

    if (ptcb->TCB_NextTCB != (struct gs_tcb *) 0) ptcb->TCB_NextTCB->TCB_PrevTCB = ptcb->TCB_PrevTCB;

    ptcb->TCBState = G_TASK_STATE_UNLINKED;
    SAMPLE_FUNCTION_END(44)
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
INT32 gk_TASK_IS_BLOCKED(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(45)
	GS_ECB *pevent; PRINT_DEBUG_LINE
	INT32 return_int; PRINT_DEBUG_LINE

	return_int = G_FALSE; PRINT_DEBUG_LINE

	pevent = ptcb->TCB_NextTCBAEL; PRINT_DEBUG_LINE

	while (pevent != (struct gs_ecb *) 0)
	{
        /* GET THE TYPE OF THE EVENT     */
	    switch (pevent->ECBState)
	    {
	        /* It is a Waiting for resource Event *****************************/
	        case GS_ECB_STATE_WAITING_RESOURCE:
	        	return_int = G_TRUE; PRINT_DEBUG_LINE
	    	    break; 
	    }
    	pevent = pevent->ECB_NextECB; PRINT_DEBUG_LINE
	}
    SAMPLE_FUNCTION_END(45)
	return return_int; 
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
    SAMPLE_FUNCTION_BEGIN(46)
	GS_ECB *pevent; PRINT_DEBUG_LINE

	pevent = presource->RCB_NextRCBGEL; PRINT_DEBUG_LINE
    while ((pevent != (struct gs_ecb *) 0)) {
    	// if(pevent->ECB_AssocTCB == ptcb)	return(pevent); 
        if(pevent->ECB_AssocTCB == ptcb)	break; 
        pevent = pevent->ECB_NextECB; PRINT_DEBUG_LINE
    }
    SAMPLE_FUNCTION_END(46)
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
INT32 gk_RRDSFL_Link(GS_RRDS *prrds)
{
    SAMPLE_FUNCTION_BEGIN(47)
    GS_SCB *psignal1;
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
    fprintf(stderr,"[ MESSAGE ] Remove SCB from ASL\n");    
#endif  
	// Remove SCB from ASL
	while (prrds->RRDS_NextSCB != (struct gs_scb *) 0)
	{
		psignal1 = prrds->RRDS_NextSCB;
        gk_RRDSASL_UnLink(prrds, psignal1);
		gk_SCBFL_Link(psignal1);
	}
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
    fprintf(stderr,"[ MESSAGE ] Insert the RRDS in the Free RRDS List\n");    
#endif     
    
    free(prrds->malloc_address);
    g_kcb.KCB_NUMBER_OF_RRDSs-- ;
    
    SAMPLE_FUNCTION_END(47)
    return(G_TRUE);
}

/**gk_RRDS_GetFree
 *  \brief 
 *  Gets the pointer of a free RRDS from the free list
 *  \return Pointer to RRDS, or NULL if no free RRDS are available
 *  \relates RRDS
 */ 
GS_RRDS *gk_RRDS_GetFree(void)
{
    SAMPLE_FUNCTION_BEGIN(48)
	GS_RRDS *prrds;

    prrds = gk_Get_RRDS();
    
	if (prrds != (struct gs_rrds *) 0)	{
        prrds->RRDS_NextSCB = (struct gs_scb *) 0;
        prrds->RRDS_NextRRDS = (struct gs_rrds *) 0;
        // prrds->RRDS_RCB_Data = (void *) 0;
    }
    SAMPLE_FUNCTION_END(48)
	return(prrds);
}

/**gk_RRDSASL_Link
 *  \brief 
 *  Links SCB to RRDS Associated Signal List
 *  \param [in] prrds   Pointer to the RRDS
 *  \param [in] psignal Pointer to the SCB of the signal
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Signal
 */ 
INT32 gk_RRDSASL_Link(GS_RRDS *prrds, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(49)
    INT32 priority = psignal->SCBType;
    GS_SCB *psignal1;

    if (prrds->RRDS_NextSCB == (struct gs_scb *) 0)
    {
        // it is the only SCB associated to pevent
    	psignal->SCB_NextSCB = (struct gs_scb *) 0;
    	prrds->RRDS_NextSCB  = (struct gs_scb *) psignal;
    }
    else
    {
        if(prrds->RRDS_NextSCB->SCBType < priority)
        {
            // it is the first SCB associated to pevent
            psignal->SCB_NextSCB = prrds->RRDS_NextSCB;
            prrds->RRDS_NextSCB  = (struct gs_scb *) psignal;
        }
        else
        {
            // Sort by priotiy
            psignal1 = prrds->RRDS_NextSCB;
            while (psignal1 != (struct gs_scb *) 0)
            {
                if (psignal1->SCB_NextSCB == (struct gs_scb *) 0)
                {
                    // It is inserted as the last SCB
                    psignal->SCB_NextSCB  = (struct gs_scb *) 0;
                    psignal1->SCB_NextSCB = (struct gs_scb *) psignal;
                    psignal1 = (struct gs_scb *) 0;
                }
                else
                {
                    if (psignal1->SCB_NextSCB->SCBType > priority)
                    {
                        psignal->SCB_NextSCB  = psignal1->SCB_NextSCB;
                        psignal1->SCB_NextSCB = (struct gs_scb *) psignal;
                        psignal1 = (struct gs_scb *) 0;
                    }
                    else
                    {
                        // Keep searching
                        psignal1 = psignal1->SCB_NextSCB;
                    }
                }
            }
        }
    }
	psignal->SCBState = G_SCB_STATE_IN_TCB;
    SAMPLE_FUNCTION_END(49)
    return(G_TRUE);
}

/**gk_RRDSASL_UnLink
 *  \brief 
 *  Unlinks SCB from RRDS Associated Signal List
 *  \param [in] prrds   Pointer to RRDS
 *  \param [in] psignal Pointer to SCB
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if it is linked and return G_FALSE if it is not
 *  \relates RRSD
 */ 
INT32  gk_RRDSASL_UnLink(GS_RRDS *prrds, GS_SCB *psignal)
{
    SAMPLE_FUNCTION_BEGIN(50)
	GS_SCB *psignal1;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal->SCBState == G_SCB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif

	if (prrds->RRDS_NextSCB != (struct gs_scb *) 0)
	{
		if (prrds->RRDS_NextSCB == psignal)
		{
			prrds->RRDS_NextSCB = psignal->SCB_NextSCB;
		}
		else
		{
			psignal1 = prrds->RRDS_NextSCB;
			while (psignal1 != (struct gs_scb *) 0)
			{
				if (psignal1->SCB_NextSCB == psignal)
				{
					psignal1->SCB_NextSCB = psignal->SCB_NextSCB;
					psignal1 = (struct gs_scb *) 0;
				}
				else
				{
					psignal1 = psignal1->SCB_NextSCB;
				}
			}
		}
		psignal->SCBState = G_SCB_STATE_UNLINKED;
	}

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (psignal->SCBState != G_SCB_STATE_UNLINKED) G_DEBUG_WHILEFOREVER;
#endif
    SAMPLE_FUNCTION_END(50)
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
 *  \sa Processor
 */
GS_TCB *gk_PCB_GetCurrentTCB(void)
{
	SAMPLE_FUNCTION_BEGIN(51)
	#if G_DEBUG_WHILEFOREVER_ENABLE == 1
		if (TCB_IsValid(g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB) != G_TRUE && g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB != (struct gs_tcb *) 0) {
            PRINT_TO_DEBUG("g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB = %p", (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB)
            G_DEBUG_WHILEFOREVER;
        }
	#endif
    SAMPLE_FUNCTION_END(51)
	return(g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB);
}


/**gk_LCBLowerPriorityThanLCB
 *  \brief 
 *  Return G_TRUE if plct1 is greater than plcb2, otherwise returns G_FALSE
 *  \param [in] plcb1 first pointer to GS_LCB
 *  \param [in] plcb2 second pointer to GS_LCB
 *  \return G_TRUE if priority of plcb1 is lower than priority of plcb2
 *  \relates List
 */
INT32 gk_LCBLowerPriorityThanLCB(GS_LCB *plcb1, GS_LCB *plcb2)
{
    SAMPLE_FUNCTION_BEGIN(52)
    int retorno = G_FALSE;
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (LCB_IsValid(plcb1) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (LCB_IsValid(plcb2) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif
	if ((plcb1->LCB_NextTCBRUNL != (struct gs_tcb *) 0) && (plcb2->LCB_NextTCBRUNL == (struct gs_tcb *) 0)) retorno = G_TRUE;
	if (((plcb1->LCB_NextTCBRUNL != (struct gs_tcb *) 0) && (plcb2->LCB_NextTCBRUNL != (struct gs_tcb *) 0)) &&
		(plcb1->LCBRunPriority > plcb2->LCBRunPriority)) retorno = G_TRUE;
    SAMPLE_FUNCTION_END(52)
	return(retorno);
}

/**gk_TCBLowerPriorityThanTCB
 *  \brief 
 *  Return G_TRUE if current priority of ptct1 is greater than the current priority of ptcb2, otherwise returns G_FALSE
 *  \param [in] ptcb1 Pointer to the TCB of task 1
 *  \param [in] ptcb2 Pointer to the TCB of task 2
 *  \return G_TRUE if current priority of ptct1 is greater than the current priority of ptcb2, otherwise returns G_FALSE
 *  \relates Task
 */ 
INT32 gk_TCBLowerPriorityThanTCB(GS_TCB *ptcb1, GS_TCB *ptcb2)
{
    SAMPLE_FUNCTION_BEGIN(53)
    int retorno = G_FALSE;
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb1) != G_TRUE) G_DEBUG_WHILEFOREVER;
	if (TCB_IsValid(ptcb2) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

	if (ptcb1->TCBCurrentPriority > ptcb2->TCBCurrentPriority) retorno = G_TRUE;
    SAMPLE_FUNCTION_END(53)
	return(retorno);
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
    SAMPLE_FUNCTION_BEGIN(54)
	GS_TCB *ptcb = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB;
	GS_PCB *ppcb = &g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1]; 
    int i;
    // int run_main;

    // fprintf(stderr, "[ OK1 ] ptcb %p, IDLE %p, PCBState %d, ptcb->TCB_RDY_LCB_Index %d PCB_RDY_LCBL[0] %d in %s, %d\n", ptcb, ppcb->PCB_IDLETCB, ppcb->PCBState, ptcb->TCB_RDY_LCB_Index, g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_RDY_LCBL[0], __FUNCTION__,__LINE__);

    if (ppcb->PCBState == GS_PCB_STATE_FREE) { // It is not executing a main list task
        // Unlink if it executing a task
        if ((ptcb != ppcb->PCB_IDLETCB) && (ptcb != (struct gs_tcb *) 0)) { // It is executing a task (no from main list)
            gk_TCBRUNL_Unlink(ptcb);
            gk_TCBRDYL_Link(ptcb);
        }    
        if (ppcb->PCB_RDY_LCBL[0]->LCB_NextTCBRDYL != (struct gs_tcb *) 0) { // main list has a ready task
            ptcb = ppcb->PCB_RDY_LCBL[0]->LCB_NextTCBRDYL;
        }            
        else { // No task to execute from main list
            ptcb = ppcb->PCB_IDLETCB;
            for (i = 1; i < G_NUMBER_OF_LCBs_FOR_PCB; i++) {
                if (ppcb->PCB_RDY_LCBL[i]->LCB_NextTCBRDYL != (struct gs_tcb *) 0) {
                    ptcb = ppcb->PCB_RDY_LCBL[i]->LCB_NextTCBRDYL;
                    break;
                }
                i++;
            }
        }
    }
    else {
        if (ppcb->PCB_RDY_LCBL[0]->LCB_NextTCBRDYL != (struct gs_tcb *) 0) { // main list has a ready task
        	if (ptcb->TCBCurrentPriority > ppcb->PCB_RDY_LCBL[0]->LCB_NextTCBRDYL->TCBCurrentPriority) {
                gk_TCBRUNL_Unlink(ptcb);
                gk_TCBRDYL_Link(ptcb);
                ptcb = ppcb->PCB_RDY_LCBL[0]->LCB_NextTCBRDYL;
            }
        } 
        else {
            ptcb = ptcb;
        }
    }
    SAMPLE_FUNCTION_END(54)
    return(ptcb);
}

/**gk_SetLowestProcessor
 *  \brief 
 *  Sets the Lowest Processor register in GRTOS Controller
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check for error and return G_FALSE
 *  \relates Processor
 */
INT32 gk_SetLowestProcessor(void)
{
    SAMPLE_FUNCTION_BEGIN(55)
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        if (g_kcb.KCB_NextLCBL == (GS_LCB *)0) G_DEBUG_WHILEFOREVER;
        if (g_kcb.KCB_NextLCBL->LCB_NextTCBRUNL == (GS_TCB *)0) G_DEBUG_WHILEFOREVER;
        if (g_kcb.KCB_NextLCBL->LCB_NextTCBRUNL->TCB_AssocPCB <=  0) G_DEBUG_WHILEFOREVER;
        if (g_kcb.KCB_NextLCBL->LCB_NextTCBRUNL->TCB_AssocPCB == 0) G_DEBUG_WHILEFOREVER;
        if (g_kcb.KCB_NextLCBL->LCB_NextTCBRUNL->TCB_AssocPCB > G_NUMBER_OF_PCB) G_DEBUG_WHILEFOREVER;
    #endif

	GRTOS_CMD_LOW_PRC_SET((int)g_kcb.KCB_NextLCBL->LCB_NextTCBRUNL->TCB_AssocPCB);
	SAMPLE_FUNCTION_END(55)
    return(G_TRUE);
}

/**gk_SetNextTimeProcessor
 *  \brief 
 *  Sets the Next Time Processor register in GRTOS Controller
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Processor
 */ 
INT32 gk_SetNextTimeProcessor(void)
{
	SAMPLE_FUNCTION_BEGIN(56)
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        if (g_kcb.KCB_NextECBTL == (struct gs_ecb *) 00) G_DEBUG_WHILEFOREVER;
        if (g_kcb.KCB_NextECBTL->ECB_AssocTCB == (GS_TCB *)0 &&
            g_kcb.KCB_NextECBTL->ECBType != (INT32) G_ECB_TYPE_LASTEST_TIME) G_DEBUG_WHILEFOREVER;
        if (g_kcb.KCB_NextECBTL->ECB_AssocTCB->TCB_AssocPCB > G_NUMBER_OF_PCB) G_DEBUG_WHILEFOREVER;
    #endif

	if (g_kcb.KCB_NextECBTL->ECBType != (INT32) G_ECB_TYPE_LASTEST_TIME) {
		GRTOS_CMD_NXT_TM_PRC_SET((int) g_kcb.KCB_NextECBTL->ECB_AssocTCB->TCB_AssocPCB);
	} else {
		GRTOS_CMD_NXT_TM_PRC_SET((int) 0);
	}
    SAMPLE_FUNCTION_END(56)
    return(G_TRUE);
}

/**gk_TASK_INHERENCE_PRIORITY_SET
 *  \brief 
 *  Sets the inherence priority of TCB according the priority of granted resources
 *  \param [in] ptcb Pointer to the TCB
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Verify kind of resource
 *  \relates Task
 */
INT32 gk_TASK_INHERENCE_PRIORITY_SET(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(57)
	GS_ECB *pevent;
	INT64 priority;
	INT64 priority2;
	G_RCB *presource;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

	priority = ptcb->TCBReadyPriority;
	pevent = ptcb->TCB_NextTCBAEL;

	while (pevent != (struct gs_ecb *) 0) {
		if (pevent->ECBState == GS_ECB_STATE_GRANTED_RESOURCE){
			presource = (G_RCB *) pevent->ECB_AssocRCB;
			priority2 = presource->RCBPriority.i64;
			if (priority2 < priority){
				priority = priority2;
			}
		}
		pevent = pevent->ECB_NextTCBAEL;
	}
	ptcb->TCBInherPriority = priority;
    SAMPLE_FUNCTION_END(57)
    return(G_TRUE);
}

/**gk_TASK_PRIORITY_SET
 *  \brief 
 *  Computes the current priority of the task
 *  \param [in] ptcb       Pointer to the TCB
 *  \param [in] task_state State of the task (G_TASK_STATE_READY, G_TASK_STATE_RUNNING)
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if state is valid
 *  \relates Task
 */
INT32 gk_TASK_PRIORITY_SET(GS_TCB *ptcb, INT32 task_state)
{
    SAMPLE_FUNCTION_BEGIN(58)
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE || ptcb == (struct gs_tcb *) 0) G_DEBUG_WHILEFOREVER;
#endif

	if (ptcb->TCBReadyPriority < ptcb->TCBInherPriority) {ptcb->TCBCurrentPriority = ptcb->TCBReadyPriority;}
	else {ptcb->TCBCurrentPriority = ptcb->TCBInherPriority;}
	if (task_state == G_TASK_STATE_RUNNING && ptcb->TCBCurrentPriority > ptcb->TCBRunPriority) {
		ptcb->TCBCurrentPriority = ptcb->TCBRunPriority;}
    SAMPLE_FUNCTION_END(58)
    return(G_TRUE);
}

/**gk_LCB_CheckInvertion
 *  \brief 
 *  Checks if there is a processor that has to be interrupted to switch task and interruupt it all
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \relates Processor
 *  \sa Task
 */
INT32 gk_LCB_CheckInvertion(void)
{
    SAMPLE_FUNCTION_BEGIN(59)
	GS_LCB *readylist = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_RDY_LCBL[0];
	int i = GRTOS_CMD_PRC_ID ;
    // int j;


	if (readylist->LCB_NextTCBRDYL != (struct gs_tcb *) 0)
	{/* There is at least a Ready Task */
		if (readylist->LCB_NextLCBFPL != (struct gs_pcb *) 0)
		{/* There is a Free Processor then interrupt it */


			if (i != readylist->LCB_NextLCBFPL->PCBID) {
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
    fprintf(stderr, "[ OK ] Processor %d trigger proc %d in %s, %d\n", GRTOS_CMD_PRC_ID, readylist->LCB_NextLCBFPL->PCBID, __FUNCTION__,__LINE__);
#endif    
                GRTOS_CMD_PRC_INT(readylist->LCB_NextLCBFPL->PCBID);
            }
		}
		else
		{
			if (readylist->LCB_NextTCBRUNL != (struct gs_tcb *) 0)
			{/*�There is not Free Processor then check the priority of running task */
				if (readylist->LCB_NextTCBRUNL->TCBCurrentPriority > readylist->LCB_NextTCBRDYL->TCBCurrentPriority){
					/* Ready priority is greater then processor assigned is interrupted */

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (readylist->LCB_NextTCBRUNL->TCB_AssocPCB < 1 || readylist->LCB_NextTCBRUNL->TCB_AssocPCB > G_NUMBER_OF_PCB) G_DEBUG_WHILEFOREVER;
#endif

					if (i != readylist->LCB_NextTCBRUNL->TCB_AssocPCB) {
#if G_DEBUG_WHILEFOREVER_ENABLE == 1                        
    fprintf(stderr, "[ OK ] Processor %d trigger proc %d in %s, %d\n", GRTOS_CMD_PRC_ID, readylist->LCB_NextTCBRUNL->TCB_AssocPCB, __FUNCTION__,__LINE__);
#endif                        
                        GRTOS_CMD_PRC_INT(readylist->LCB_NextTCBRUNL->TCB_AssocPCB);
                    }
				}
			}
		}
	}
    SAMPLE_FUNCTION_END(59)
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
INT32 gk_TCB_Unlink(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(60)
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (TCB_IsValid(ptcb) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

	switch (ptcb->TCBState)
	{
		case G_TASK_STATE_RUNNING:           gk_TCBRUNL_Unlink(ptcb); PRINT_DEBUG_LINE break;
		case G_TASK_STATE_READY:             gk_TCBRDYL_Unlink(ptcb); PRINT_DEBUG_LINE break;
		case G_TASK_STATE_WAITING_COMPLETED: gk_TCBWL_Unlink(ptcb); PRINT_DEBUG_LINE   break;
        case G_TASK_STATE_WAITING :          gk_TCBWL_Unlink(ptcb); PRINT_DEBUG_LINE   break;
		case G_TASK_STATE_UNLINKED:
			if (ptcb->TCBType != G_TASK_TYPE_IDLE) G_DEBUG_WHILEFOREVER;
			break;
		default:
			G_DEBUG_WHILEFOREVER;
			break;
	}
    SAMPLE_FUNCTION_END(60)
    return(G_TRUE);
}

/**gk_ECB_Unlink
 *  \brief 
 *  Unlinks the ECB according to the list it is linked
 *  \param [in] pecb Pointer to the ECB
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if it useful, otherwise delete the function
 *  \todo Implement G_FALSE when no linked
 *  \relates Event
 */ 
INT32 gk_ECB_Unlink(GS_ECB *pecb)
{
    SAMPLE_FUNCTION_BEGIN(61)
	G_DEBUG_VERBOSE

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (ECB_IsValid(pecb) != G_TRUE) G_DEBUG_WHILEFOREVER;
#endif

	if (pecb != (struct gs_ecb *) 0)
	{
		switch (pecb->ECBState)
		{
			case GS_ECB_STATE_WAITING_TIME:     gk_ECBTL_Unlink(pecb); break;
			case GS_ECB_STATE_WAITING_RESOURCE: gk_RCBWEL_Unlink(pecb); break; ///!!! Poner resource
			case GS_ECB_STATE_GRANTED_RESOURCE: gk_RCBGEL_Unlink(pecb); break;
			default: G_DEBUG_WHILEFOREVER; break;
		}
	}
    SAMPLE_FUNCTION_END(61)
    return(G_TRUE);
}

/**gk_TCBPSL_GetSCB
 *  \brief 
 *  Returns pointer to the SCB in te TCBPSL
 *  \param [in] ptcb       Pointer to the TCB
 *  \param [in] SignalType Type of the signal to search in the TCB
 *  \return Pointer to the SCB or NULL if no signal type associated with the task
 *  \relates Task
 *  \sa Signal
 */ 
GS_SCB *gk_TCBPSL_GetSCB(GS_TCB *ptcb, INT32 SignalType)
{
    SAMPLE_FUNCTION_BEGIN(62)
	GS_SCB *psignal = (struct gs_scb *) 0;
	GS_SCB *psignal1 = ptcb->TCB_NextTCBPSL;

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
    SAMPLE_FUNCTION_END(62)
	return(psignal);
}

/**gk_RRDSASL_GetSCB
 *  \brief 
 *  Returns pointer to the SCB linked in the RRDSASL list
 *  \param [in] prrds      Pointer to the RRDS
 *  \param [in] SignalType Type of signal to search in the list
 *  \return Pointer to the SCB with the type searched
 *  \todo Check if this function is necessary and valid for the union RRDS
 *  \relates Signal
 *  \sa RRDS
 */
GS_SCB *gk_RRDSASL_GetSCB(GS_RRDS *prrds, INT32 SignalType)
{
    SAMPLE_FUNCTION_BEGIN(63)
	GS_SCB *psignal = (struct gs_scb *) 0;
	GS_SCB *psignal1 = prrds->RRDS_NextSCB;

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
    SAMPLE_FUNCTION_END(63)
	return(psignal);
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
GS_SCB *gk_KCBASL_GetSCB(INT32 SignalType)
{
    SAMPLE_FUNCTION_BEGIN(64)
	GS_SCB *psignal = (struct gs_scb *) 0;
	GS_SCB *psignal1 = g_kcb.KCB_NextKCBASL;

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
    SAMPLE_FUNCTION_END(64)
	return(psignal);
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
GS_SCB *gk_TCBASL_GetSCB(GS_TCB *ptcb, INT32 SignalType)
{
    SAMPLE_FUNCTION_BEGIN(65)
	GS_SCB *psignal = (struct gs_scb *) 0;
	GS_SCB *psignal1 = ptcb->TCB_NextTCBASL;

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
    SAMPLE_FUNCTION_END(65)
	return(psignal);
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
GS_SCB *gk_ECBASL_GetSCB(GS_ECB *pecb, INT32 SignalType)
{
    SAMPLE_FUNCTION_BEGIN(66)
	GS_SCB *psignal = (struct gs_scb *) 0;
	GS_SCB *psignal1 = pecb->ECB_NextECBASL;

    while (psignal != (struct gs_scb *) 0)
    {
    	if (psignal->SCBType == SignalType)
    	{
    		psignal = psignal1;
    		psignal1 = (struct gs_scb *) 0;
    	}
    	else
    	{
        	psignal1 = psignal->SCB_NextSCB;
    	}
    }
    SAMPLE_FUNCTION_END(66)
	return(psignal);
}

/**gk_RCBASL_GetSCB
 *  \brief 
 *  Returns the pointer to the SCB of a resource or NULL otherwise
 *  \param [in] prcb       Pointer to the RCB of the resource
 *  \param [in] SignalType Signal Type og the signal
 *  \return Pointer to the SCB of NULL if was not found
 *  \relates Signal
 */ 
GS_SCB *gk_RCBASL_GetSCB(G_RCB *prcb, INT32 SignalType)
{
    SAMPLE_FUNCTION_BEGIN(67)
	GS_SCB *psignal = (struct gs_scb *) 0;
	GS_SCB *psignal1 = prcb->RCB_NextRCBASL;

    while (psignal1 != (struct gs_scb *) 0)
    {
		if (psignal1->SCBType == SignalType)
		{
			psignal  = psignal1;
			psignal1 = (struct gs_scb *) 0;
		}
		else
		{
			psignal1 = psignal->SCB_NextSCB;
		}
    }
    SAMPLE_FUNCTION_END(67)
	return(psignal);
}

/**GRTOS_Task_GetPendingSCB
 *  \brief 
 *  Configures system to execute next pending signal. Call from switch routine
 *  \relates Task
 */
void GRTOS_Task_GetPendingSCB(void)
{
    SAMPLE_FUNCTION_BEGIN(68)
	GS_TCB *ptcb =gk_PCB_GetCurrentTCB();   /* Get Current Task          */
	GS_SCB *psignal = ptcb->TCB_NextTCBPSL;  /* Get Next Pending Signal   */

	G_SCB_PENDING = 0;               /* Default is no pending SCB */

	if (psignal != (struct gs_scb *) 0)    /* There is a pending SCB    */
	{
		if (psignal->SCBState == G_SCB_STATE_PENDING) { /* Signal is pending      */
			psignal->SCBState = G_SCB_STATE_EXECUTING; /* Signal to execution     */
		    G_SCB_CODE    = (INT32) psignal->SCB_TaskCode + 4; /* Task Code                     */
			G_SCB_ARG     = (INT32) psignal->SCB_TaskArg;       /* Task Argument                 */
		    gk_TCBPSL_Unlink(ptcb, psignal);            /* Unlink SCB from TCB       */
		    gk_SCBFL_Link(psignal);                     /* Link SCB to free list     */
			G_SCB_PENDING = 1;                       /* Set the Pending Signal status */
		}
	}
    alt_dcache_flush_all();
    SAMPLE_FUNCTION_END(68)
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
INT32 gk_TASK_STK_Init(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(69)
	GS_STK *stk;

	stk = (GS_STK *) ptcb->TCB_StackBottom - 80;  /* It has to be aligned */
	ptcb->TCB_StackPointer = stk;            /* Load Stack pointer in TCB               */

	/* Lleno la pila del primer llamado que llamar� a OSStartTsk  */
	stk[0]  = 1;                                      /* estatus         */
	stk[1]  = (INT32) gk_KERNEL_TASK_COMPLETE;           /* register r31 ra */
	stk[2]  = (INT32)ptcb->TCB_TaskCode + 4;  /* register r29 ea */
	stk[3]  = (INT32)ptcb->TCB_StackPointer-1;        /* register r28 fp */
	stk[4]  = 0;                                      /* register r15    */
	stk[5]  = 0;                                      /* register r14    */
	stk[6]  = 0;                                      /* register r13    */
	stk[7]  = 0;                                      /* register r12    */
	stk[8]  = 0;                                      /* register r11    */
	stk[9]  = 0;                                      /* register r10    */
	stk[10] = 0;                                      /* register r9     */
	stk[11] = 0;                                      /* register r8     */
	stk[12] = 0;                                      /* register r7     */
	stk[13] = 0;                                      /* register r6     */
	stk[14] = 0;                                      /* register r5     */
	stk[15] = (INT32) ptcb->TCB_TaskArg;             /* register r4     */
	stk[16] = 0;                                      /* register r3     */
	stk[17] = (INT32) ptcb->TCB_TaskArg;             /* register r2     */
	stk[18] = 0;                                      /* register r1     */
	stk[19] = 0;                                      /* register r24    */
	stk[20] = 0;                                      /* register r23    */
	stk[21] = 0;                                      /* register r22    */
	stk[22] = 0;                                      /* register r21    */
	stk[23] = 0;                                      /* register r20    */
	stk[24] = 0;                                      /* register r19    */
	stk[25] = 0;                                      /* register r18    */
	stk[26] = 0;                                      /* register r17    */
	stk[27] = 0;                                      /* register r16    */
    SAMPLE_FUNCTION_END(69)
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
INT32 gk_TASK_Kill(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(70)
    gk_TCB_Unlink(ptcb);       /* Unlinks the TCB structure               */
    gk_TCB_List_Unlink(ptcb);  /* Unlinks all the linked lists of the TCB */
    gk_TCBFL_Link(ptcb);       /* Links the TCB to the TCBFL              */
    SAMPLE_FUNCTION_END(70)
    return G_TRUE;
}

/***********************************************************************************
*********************** GRTOS COMPLEMENTARY FUNCTIONS to be added!!!!!!!  ****************************
***********************************************************************************/

/**gu_Get_IRQ_Register
 *  \brief 
 *  Returns the status or the IRQ inputs of the GRTOS Controller
 *  \return integer with the status of the External IRQ Register
 *  \todo Check if return can be changed to INT32
 *  \todo Change to user API files  
 *  \relates Interrupt
 */
int gu_Get_IRQ_Register(void)
{
    SAMPLE_FUNCTION_BEGIN(71)
    SAMPLE_FUNCTION_END(71)
	return(IORD_GRTOS_IRQ_RQS);
}

/**gu_Get_CPU_ID
 *  \brief 
 *  Returns the ID of the current processor
 *  \return Returns the ID of the current processor
 *  \todo Check if return can be changed to INT32
 *  \todo Change to user API files
 *  \relates Processor
 */
int gu_Get_CPU_ID(void)
{
    SAMPLE_FUNCTION_BEGIN(72)
    SAMPLE_FUNCTION_END(72)
	return(GRTOS_CMD_PRC_ID );
}


/**gu_Get_Next_Occurrence_Time
 *  \brief 
 *  Returns the Next Occurrence Time
 *  \return Returns the Next Occurrence Time
 *  \todo Change to user API files
 *  \relates Time
 */
gt_time gu_Get_Next_Occurrence_Time(void)
{
    SAMPLE_FUNCTION_BEGIN(73)
    SAMPLE_FUNCTION_END(73)
    return(g_kcb.KCB_NextECBTL->ECBValue.i64);
}


/***********************************************************************************
*********************** FINISH COMPLEMENTARY FUNCTIONS  ***************************
***********************************************************************************/

/**gk_TASK_GRANTED_PRIORITY_GET
 *  \brief 
 *  Determine the priority of a taks according to the resources granted
 *  \param [in] ptcb Pointer to the TCB of the task
 *  \return Priority determined
 *  \todo Check according the type of reource
 *  \relates Task
 */
INT64 gk_TASK_GRANTED_PRIORITY_GET(GS_TCB *ptcb)
{
    SAMPLE_FUNCTION_BEGIN(74)
	GS_ECB *pevent;
	INT64 priority;

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if ((TCB_IsValid(ptcb) != G_TRUE)) G_DEBUG_WHILEFOREVER;
#endif

	priority = G_LOWEST_PRIORITY;
	pevent = ptcb->TCB_NextTCBAEL;
	while ((pevent != (struct gs_ecb *) 0))
	{
		// Controlar que solo sean recursos granted !!!!!!!!!!!!!!
        if (priority > pevent->ECBValue.i64) priority = pevent->ECBValue.i64;
    	pevent = pevent->ECB_NextECB;
	}
    SAMPLE_FUNCTION_END(74)
	return priority;
}


OPTIMEZE_RESTORE

