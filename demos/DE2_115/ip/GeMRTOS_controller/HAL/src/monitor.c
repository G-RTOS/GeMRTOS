/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Monitor functions for debugging
 *  \details This file contains the Monitor functions for debugging.
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


// #include <grtos.h>

#include <gemrtos.h>

OPTIMEZE_CODE(0)

/*****************************************************************************/
/*****************************************************************************/
/**         DEBUG MONITOR VARIABLES                                          */
/**         COPY MIRROR OF SYSTEM VARIABLES                                  */
/*****************************************************************************/
/*****************************************************************************/

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)

	/**gk_MONITOR_FIFO_SAMPLE
	 *  \brief This function stores the data in he monitor FIFO wth the time stamp 
	 *  
	 *  \param [in] data Value to be stored into the FIFO
	 *  \relates Monitor
	 */
	void gk_MONITOR_FIFO_SAMPLE (int data)
	{
		IOWR(GRTOS_DRIVER_GRTOS_MONITOR_BASE, 0, (G_INT32) data);
	}

	G_INT32 gkm_XCB_NotValid(void *ptcb)
	{
		return(G_TRUE);
	}

	G_INT32 gkm_XCB_IsFieldValid(void *data)
	{
		return(G_TRUE);
	}

	/**
	 * Returns G_TRUE if points to a TCB
	 * @param ptcb pointer to ECB structure to check
	 * @return G_TRUE if pointer to ECB, G_FALSE otherwise
	 */
	G_INT32 gkm_TCB_IsValid(GS_TCB *ptcb)
	{
		if (ptcb->BLOCK_HASH == (unsigned int) ptcb + G_TCB_HASH) return(G_TRUE);
		return(G_FALSE);
	}

	/**
	 * Returns G_TRUE if points to a GS_PCBAssocLCB
	 * @param ppcbalcb pointer to GS_PCBAssocLCB structure to check
	 * @return G_TRUE if pointer to GS_PCBAssocLCB, G_FALSE otherwise
	 */
	G_INT32 gkm_GS_PCBAssocLCB_IsValid(GS_PCBAssocLCB *ppcbalcb)
	{
		if (ppcbalcb->BLOCK_HASH == (unsigned int) ppcbalcb + G_PCBALCB_HASH) return(G_TRUE);
		return(G_FALSE);
	}

	G_INT32 gkm_PCB_IsValid(GS_PCB *ppcb)
	{
		if (ppcb->BLOCK_HASH == (unsigned int) ppcb + G_PCB_HASH) return(G_TRUE);
		return(G_FALSE);	
	}


	/**
	 *  \brief Returns G_TRUE is pointer points to ECB
	 *  
	 *  \param [in] pevent pointer to ECB
	 *  \return G_TRUE if pointer to ECB, G_FALSE otherwise 
	 */
	// G_INT32 gkm_ECB_IsValid(GS_ECB *pevent)  __attribute__ ((section ("onchipmem")));
	G_INT32 gkm_ECB_IsValid(GS_ECB *pevent)
	{
		if (pevent->BLOCK_HASH == (unsigned int) pevent + G_ECB_HASH) return(G_TRUE);
		return(G_FALSE);
	}

	/**
	 *  \brief 
	 *  Returns G_TRUE is pointer points to SCB
	 *  \param [in] pscb Pointer to SCB to validate
	 *  \return G_TRUE when points to a valid SCB, G_FALSE otherwise
	 *  \details 
	 *  \todo
	 *  \relates Validation
	 */ 
	G_INT32 gkm_SCB_IsValid(GS_SCB *pscb)
	{
		if (pscb->BLOCK_HASH == (unsigned int) pscb + G_SCB_HASH) return(G_TRUE);
		return(G_FALSE);
	}

	 
	/**gkm_RCB_IsValid
	 *  \brief 
	 *  Returns G_TRUE is pointer points to RCB
	 *  \param [in] prcb Pointer to the RCB of the resource
	 *  \return G_TRUE if pointer to SCB, G_FALSE otherwise
	 *  \todo Find a way to determine validity when it is not an array
	 *  \relates Resource
	 */ 
	G_INT32 gkm_RCB_IsValid(void *prcb)
	{
		if (((G_RCB *) prcb)->BLOCK_HASH == (unsigned int) prcb + G_RCB_HASH) return(G_TRUE);
		return(G_FALSE);
	}

	/**gkm_PCBAssocLCB_IsValid
	 *  \brief 
	 *  Returns G_TRUE is pointer points to GS_PCBAssocLCB
	 *  \param ppcbalcb Pointer to the GS_PCBAssocLCB of the resource
	 *  \return G_TRUE if pointer to GS_PCBAssocLCB, G_FALSE otherwise
	 *  \relates Resource
	 */ 
	G_INT32 gkm_PCBAssocLCB_IsValid(void *ppcbalcb)
	{
		if (((GS_PCBAssocLCB *) ppcbalcb)->BLOCK_HASH == (unsigned int) ppcbalcb + G_PCBALCB_HASH) return(G_TRUE);
		return(G_FALSE);
	}

	/**gkm_RRDS_IsValid
	 *  \brief 
	 *  Returns G_TRUE is pointer points to GS_RRDS
	 *  \param [in] prrds Pointer to the RRDS
	 *  \return G_TRUE if pointer to RRDS, G_FALSE otherwise
	 *  \todo Find a way when it is not an array of control blocks
	 *  \relates RRDS
	 */ 
	G_INT32 gkm_RRDS_IsValid(GS_RRDS *prrds)
	{
		if (prrds->BLOCK_HASH == (unsigned int) prrds + G_RRDS_HASH) return(G_TRUE);
		return(G_FALSE);
	}



	G_INT32 gkm_LCB_IsValid(GS_LCB *plcb)
	{
		if (plcb->BLOCK_HASH == (unsigned int) plcb + G_LCB_HASH) return(G_TRUE);
		return(G_FALSE);    
	}

	G_INT32 gkm_IsAListorNull(GS_LCB *plcb)
	{
		/* si es puntero nulo retorno G_TRUE */
		if (plcb == (struct gs_lcb *) 0) return(G_TRUE);
		if (plcb->BLOCK_HASH == (unsigned int) plcb + G_LCB_HASH) return(G_TRUE);
		return(G_FALSE);     
	}

	G_INT32 gkm_TCBState_Valid(enum tcbstate tcbstate)
	{
		if (tcbstate == G_TCBState_WAITING_COMPLETED) {return (G_TRUE);}
		if (tcbstate == G_TCBState_READY) {return (G_TRUE);}
		if (tcbstate == G_TCBState_RUNNING) {return (G_TRUE);}
		if (tcbstate == G_TCBState_UNLINKED) {return (G_TRUE);}
		if (tcbstate == G_TCBState_WAITING) {return (G_TRUE);}
		return (G_FALSE);
	}

	GS_TCB *gkm_TCB_list_ROOT(GS_TCB *ptcb)
	{
		if (ptcb->TCBType == G_TCBType_IDLE) {return ((GS_TCB *) ptcb);}
		if (ptcb->TCBState == G_TCBState_WAITING_COMPLETED) {return ((GS_TCB *) g_kcb.KCB_NextTCBWL);}
		if (ptcb->TCBState == G_TCBState_WAITING)  {return ((GS_TCB *) g_kcb.KCB_NextTCBWL);}
		if (ptcb->TCBState == G_TCBState_READY) {return ((GS_TCB *) ptcb->TCB_RDY_LCB_Index->LCB_NextTCBRDYL);}
		if (ptcb->TCBState == G_TCBState_RUNNING)  {return ((GS_TCB *) ptcb->TCB_RDY_LCB_Index->LCB_NextTCBRUNL);}
		if (ptcb->TCBState == G_TCBState_UNLINKED) {return ((GS_TCB *) ptcb);}

		return((GS_TCB*) 0);
	}

	G_INT32 gkm_TCBType_Valid(enum tcbtype tcbtype)
	{
		if (tcbtype == G_TCBType_OneShot) {return (G_TRUE);}
		if (tcbtype == G_TCBType_PERIODIC) {return (G_TRUE);}
		if (tcbtype == G_TCBType_ISR) {return (G_TRUE);}
		if (tcbtype == G_TCBType_IDLE) {return (G_TRUE);}
		return (G_FALSE);
	}

	G_INT32 gkm_ECBState_Valid(enum ecbstate ecbstate)
	{
		if (ecbstate == GS_ECBState_GRANTED_RESOURCE) {return (G_TRUE);}
		if (ecbstate == GS_ECBState_WAITING_RESOURCE) {return (G_TRUE);}
		if (ecbstate == GS_ECBState_FREE) {return (G_TRUE);}
		if (ecbstate == GS_ECBState_WAITING_TIME) {return (G_TRUE);}
		if (ecbstate == GS_ECBState_UNLINKED) {return (G_TRUE);}
		return (G_FALSE);
	}

	G_INT32 gkm_ECBType_Valid(enum ecbtype ecbtype)
	{
		if (ecbtype == G_ECBType_OSTimeDly) {return (G_TRUE);}
		if (ecbtype == G_ECBType_PERIODIC) {return (G_TRUE);}
		if (ecbtype == G_ECBType_LASTEST_TIME) {return (G_TRUE);}
		if (ecbtype == G_ECBType_SEM_GRANTED) {return (G_TRUE);}
		if (ecbtype == G_ECBType_SEM_WAITING) {return (G_TRUE);}
		if (ecbtype == G_ECBType_NOT_SPECIFIED) {return (G_TRUE);}
		if (ecbtype == G_ECBType_QUEUE_CONSUME) {return (G_TRUE);}
		if (ecbtype == G_ECBType_MESSAGE_CONSUME_WAIT) {return (G_TRUE);}
		if (ecbtype == G_ECBType_MESSAGE_PRODUCE_WAIT) {return (G_TRUE);}
		if (ecbtype == G_ECBType_MESSAGE_PRODUCER_DELIVERED) {return (G_TRUE);}
		if (ecbtype == G_ECBType_MESSAGE_CONSUME_EXPIRED) {return (G_TRUE);}
		if (ecbtype == G_ECBType_MESSAGE_SEND_EXPIRED) {return (G_TRUE);}
		if (ecbtype == G_ECBType_MESSAGE_SEND_WAIT) {return (G_TRUE);}
		if (ecbtype == G_ECBType_EXT_ISR_WAITING) {return (G_TRUE);}
		if (ecbtype == G_ECBType_EXT_ISR_RELEASE) {return (G_TRUE);}
		return (G_FALSE);
	}

	GS_ECB *gkm_ECB_list_ROOT(GS_ECB *pevent)
	{
		G_RCB *presource = (G_RCB *) pevent->ECB_AssocRCB;
		
		if (pevent->ECBState == GS_ECBState_FREE) {return ((GS_ECB *) g_kcb.KCB_FREE_ECBs);}
		if (pevent->ECBType == G_ECBType_OSTimeDly) {return ((GS_ECB *) g_kcb.KCB_NextECBTL);}
		if (pevent->ECBType == G_ECBType_PERIODIC)  {return ((GS_ECB *) g_kcb.KCB_NextECBTL);}
		if (pevent->ECBType == G_ECBType_LASTEST_TIME) {return ((GS_ECB *) g_kcb.KCB_NextECBTL);}
		if (pevent->ECBType == G_ECBType_SEM_GRANTED)  {return ((GS_ECB *) presource->RCB_NextRCBGEL);}
		if (pevent->ECBType == G_ECBType_SEM_WAITING)  {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_NOT_SPECIFIED) {return ((GS_ECB *) pevent);}
		if (pevent->ECBType == G_ECBType_QUEUE_CONSUME) {return ((GS_ECB *) presource->RCB_NextRCBGEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_CONSUME_WAIT) {return ((GS_ECB *) presource->RCB_NextRCBGEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_PRODUCE_WAIT) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_PRODUCER_DELIVERED) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_CONSUME_EXPIRED) {return ((GS_ECB *) presource->RCB_NextRCBGEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_SEND_EXPIRED) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_SEND_WAIT) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_EXT_ISR_WAITING) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_EXT_ISR_RELEASE) {return ((GS_ECB *) presource->RCB_NextRCBGEL);}

		return ((GS_ECB*) 0);
	}

	// return 1 for incresing, 0 decreasing and 2 not suitable
	G_INT32 gkm_ECB_list_ORDER(GS_ECB *pevent)
	{

		if (pevent->ECBState == GS_ECBState_FREE) {return ((G_INT32) 2);}
		if (pevent->ECBType == G_ECBType_OSTimeDly) {return ((G_INT32) 1);}
		if (pevent->ECBType == G_ECBType_PERIODIC)  {return ((G_INT32) 1);}
		if (pevent->ECBType == G_ECBType_LASTEST_TIME) {return ((G_INT32) 1);}
		if (pevent->ECBType == G_ECBType_SEM_GRANTED)  {return ((G_INT32) 0);}
		if (pevent->ECBType == G_ECBType_SEM_WAITING)  {return ((G_INT32) 1);}
		if (pevent->ECBType == G_ECBType_NOT_SPECIFIED) {return ((G_INT32) 1);}
		if (pevent->ECBType == G_ECBType_QUEUE_CONSUME) {return ((G_INT32) 0);}
		if (pevent->ECBType == G_ECBType_MESSAGE_CONSUME_WAIT) {return ((G_INT32) 0);}
		if (pevent->ECBType == G_ECBType_MESSAGE_PRODUCE_WAIT) {return ((G_INT32) 1);}
		if (pevent->ECBType == G_ECBType_MESSAGE_PRODUCER_DELIVERED) {return ((G_INT32) 1);}
		if (pevent->ECBType == G_ECBType_MESSAGE_CONSUME_EXPIRED) {return ((G_INT32) 0);}
		if (pevent->ECBType == G_ECBType_MESSAGE_SEND_EXPIRED) {return ((G_INT32) 1);}
		if (pevent->ECBType == G_ECBType_MESSAGE_SEND_WAIT) {return ((G_INT32) 1);}
		if (pevent->ECBType == G_ECBType_EXT_ISR_WAITING) {return ((G_INT32) 2);}
		if (pevent->ECBType == G_ECBType_EXT_ISR_RELEASE) {return ((G_INT32) 2);}

		return (3);
	}

	G_INT32 gkm_PCBID_Valid(int pcbid)
	{
		if ((pcbid <= G_NUMBER_OF_PCB) && (pcbid >= 1)) {return (G_TRUE);}
		return (G_FALSE);
	}

	G_INT32 gkm_PCBState_Valid(enum pcbstate pcbstate)
	{
		if (pcbstate == GS_PCBState_NOTRUNNING) {return (G_TRUE);}
		if (pcbstate == GS_PCBState_RUNNING) {return (G_TRUE);}
		if (pcbstate == GS_PCBState_FREE) {return (G_TRUE);}
		return (G_FALSE);
	}

	G_INT32 gkm_PCBType_Valid(enum pcbtype pcbtype)
	{ 
		return (G_TRUE);
	}


	G_INT32 gkm_LCBState_IsValid(enum lcbstate lcbstate)
	{
		if (lcbstate == GS_LCBState_LINKED) {return (G_TRUE);}
		if (lcbstate == GS_LCBState_UNLINKED) {return (G_TRUE);}
		return (G_FALSE);
	}

	G_INT32 gkm_LCBType_IsValid(enum lcbtype lcbtype)
	{ 
		return (G_TRUE);
	}

	/// Checks the ECB consistency
	G_INT32 gkm_Check_ECBs(G_INT32 partial)
	{
		GS_ECB * pevent;
		GS_ECB * pevent_prv = (GS_ECB *) 0;
		GS_ECB * list_root_value;
		int antecedent;
		int consequent;
		
		int NBR_ECBs = 0;
		for (pevent = g_kcb.KCB_ROOT_ECBs; pevent != (GS_ECB *) 0; pevent = pevent->ECB_NEXT_ECBs)
		{
			NBR_ECBs++;
			if (gkm_ECB_IsValid(pevent) != G_TRUE) {printf("Invalid ECB"); while(1); return (G_FALSE);}
			if (gkm_ECBType_Valid(pevent->ECBType) != G_TRUE) {printf("Invalid ECB Type"); while(1); return (G_FALSE);}
			
			list_root_value = gkm_ECB_list_ROOT(pevent);
			
			// if ECB_PrevECB = 0 then list_root_value == pevent or pevent->ECBState = GS_ECBState_UNLINKED;
			if (!(((list_root_value == pevent) || (pevent->ECBState == GS_ECBState_UNLINKED)) || (pevent->ECBState == GS_ECBState_FREE) || (pevent->ECB_PrevECB != (struct gs_ecb *) 0))) {printf("Invalid ECB_PrevECB, ECBType=%d", pevent->ECBType); while(1); return (G_FALSE);}
			
			// if ECB_PrevECB != 0 then pevent->ECB_PrevECB->ECB_NextECB = pevent
			if (!((pevent->ECB_PrevECB == (struct gs_ecb *) 0) || (pevent->ECB_PrevECB->ECB_NextECB == pevent))) {printf("Invalid ECB_PrevECB 2, ECBType=%d", pevent->ECBType); while(1); return (G_FALSE);}
			
			// if ECB_NextECB != 0 then gkm_ECB_list_ROOT(pevent) = gkm_ECB_list_ROOT(pevent->ECB_NextECB)
			if (!((pevent->ECB_NextECB == (struct gs_ecb *) 0) || (gkm_ECB_list_ROOT(pevent) == gkm_ECB_list_ROOT(pevent->ECB_NextECB)))) {printf("Invalid ECB_NextECB, ECBType=%d", pevent->ECBType); while(1); return (G_FALSE);}
			
			// if ECB_PrevTCBAEL = 0 then pevent->ECB_AssocTCB->TCB_NextTCBAEL = pevent
			antecedent = (pevent->ECB_PrevTCBAEL == (GS_ECB *) 0);
			consequent = ((pevent->ECB_AssocTCB == (GS_TCB *) 0) || (pevent->ECB_AssocTCB->TCB_NextTCBAEL == pevent));
			if (!((!antecedent) || consequent)) {printf("Invalid ECB_PrevTCBAEL 1, ECBType=%d", pevent->ECBType); while(1); return (G_FALSE);}

			// Previous Associated TCB event
			antecedent = (pevent->ECB_PrevTCBAEL != (GS_ECB *) 0);
			if (!((!antecedent) || (pevent->ECB_PrevTCBAEL->ECB_NextTCBAEL == pevent))) {printf("Invalid ECB_PrevTCBAEL 2, ECBType=%d", pevent->ECBType); while(1); return (G_FALSE);}

			// Next Associated TCB event
			antecedent = (pevent->ECB_NextTCBAEL != (GS_ECB *) 0);
			if (!((!antecedent) || (pevent->ECB_NextTCBAEL->ECB_PrevTCBAEL == pevent))) {printf("Invalid ECB_NextTCBAEL, ECBType=%d", pevent->ECBType); while(1); return (G_FALSE);}
			
			// Next Associated event
			antecedent = (pevent->ECB_NextECBAEL != (GS_ECB *) 0);
			if (!((!antecedent) || (pevent->ECB_NextECBAEL->ECB_AssocTCB == pevent->ECB_AssocTCB))) {printf("Invalid ECB_AssocTCB, ECBType=%d", pevent->ECBType); while(1); return (G_FALSE);}
			
			// Next Associated signal
			antecedent = (pevent->ECB_NextECBASL != (GS_SCB *) 0);
			if (!((!antecedent) || (pevent->ECB_NextECBASL->SCB_AssocXCB == (void *) pevent))) {printf("Invalid ECB_AssocTCB, ECBType=%d", pevent->ECBType); while(1); return (G_FALSE);}

			// list increasing order
			antecedent = ((pevent->ECB_NextECB != (GS_ECB *) 0) && (gkm_ECB_list_ORDER(pevent) == 1));
			if (!((!antecedent) || (pevent->ECBValue.i64 <= pevent->ECB_NextECB->ECBValue.i64))) {printf("Invalid increasing ECBValue=0x%llx, next=0x%llx, type=%d", (unsigned long long) pevent->ECBValue.i64, (unsigned long long) pevent->ECB_NextECB->ECBValue.i64, (int) pevent->ECBType); while(1); return (G_FALSE);}

			// list decreasing order
			antecedent = ((gkm_ECB_list_ORDER(pevent) == 0) && (pevent->ECB_NextECB != (GS_ECB *) 0));
			if (!((!antecedent) || (pevent->ECBValue.i64 >= pevent->ECB_NextECB->ECBValue.i64))) {printf("Invalid decreasing ECBValue=0x%llx, next=0x%llx, type=%d", (unsigned long long) pevent->ECBValue.i64, (unsigned long long) pevent->ECB_NextECB->ECBValue.i64, (int) pevent->ECBType); while(1); return (G_FALSE);}

			if (partial == G_FALSE)
			{
				// ISR waiting event
				antecedent = (pevent->ECBType == G_ECBType_EXT_ISR_WAITING);
				if (!((!antecedent) || ((pevent->ECB_AssocTCB->TCBState == G_TCBState_WAITING) || (pevent->ECB_AssocTCB->TCBState == G_TCBState_WAITING_COMPLETED)))) {printf("Invalid ISR waiting event type=%d, task state=%d, proc=%d", (int) pevent->ECBType, (int) pevent->ECB_AssocTCB->TCBState, (int) gm_ProcessorId); while(1); return (G_FALSE);}
				
				// ISR waiting event 2
				// antecedent = (((pevent->ECB_AssocTCB->TCBState == G_TCBState_READY) || (pevent->ECB_AssocTCB->TCBState == G_TCBState_RUNNING)) && (pevent->ECB_AssocTCB->TCB_INTNumber != -1));
				// consequent = (pevent->ECBType == G_ECBType_EXT_ISR_RELEASE);
				// if (!((!antecedent) || consequent)) {printf("Invalid ISR waiting event 2 type=%d, task state=%d, proc=%d", (int) pevent->ECBType, (int) pevent->ECB_AssocTCB->TCBState, (int) gm_ProcessorId); return G_FALSE;}
				
				 
				// ISR released event
				antecedent = (pevent->ECBType == G_ECBType_EXT_ISR_RELEASE);
				if (!((!antecedent) || ((pevent->ECB_AssocTCB->TCBState == G_TCBState_READY) || (pevent->ECB_AssocTCB->TCBState == G_TCBState_RUNNING) || (pevent->ECB_AssocTCB->TCBState == G_TCBState_WAITING)))) {printf("Invalid ISR released event type=%d, task state=%d, proc=%d", (int) pevent->ECBType, (int) pevent->ECB_AssocTCB->TCBState, (int) gm_ProcessorId); while(1); return (G_FALSE);}
			}

			
			// ##############################################
			// Debug previous structure
			if (pevent->ECB_PREV_ECBs != pevent_prv) {printf("Invalid ECB_PREV_ECBs"); while(1); return (G_FALSE);}
			pevent_prv = pevent;
		}
		if (NBR_ECBs != g_kcb.KCB_NUMBER_OF_ECBs) {printf("Invalid KCB_NUMBER_OF_ECBs NBR_ECBs=%d, KCB_NUMBER_OF_ECBs=%d", (int) NBR_ECBs, (int) g_kcb.KCB_NUMBER_OF_ECBs); while(1); return (G_FALSE);}
		return (G_TRUE);
	}


	G_INT32 gkm_Check_Testing(G_INT32 partial)
	{
		return (G_TRUE);
	}


	/// Checks the TCB consistency
	G_INT32 gkm_Check_TCBs(G_INT32 partial)
	{
		GS_TCB * ptcb;
		GS_TCB * ptcb_prv = (GS_TCB *) 0;
		GS_TCB * list_root_value;
		int antecedent;
		int consequent;
		
		int NBR_TCBs = 0;
		for (ptcb = g_kcb.KCB_ROOT_TCBs; ptcb != (GS_TCB *) 0; ptcb = (GS_TCB *) ptcb->TCB_NEXT_TCBs) {
			NBR_TCBs++;
					
			if (gkm_TCB_IsValid(ptcb) != G_TRUE) {printf("invalid TCB"); while(1); return (G_FALSE); }
			if (gkm_TCBState_Valid(ptcb->TCBState) != G_TRUE) {printf("invalid TCBState"); while(1); return (G_FALSE);}
			if (gkm_TCBType_Valid(ptcb->TCBType) != G_TRUE) {printf("invalid TCBType"); while(1); return (G_FALSE);}
			if (gkm_LCB_IsValid((GS_LCB *) ptcb->TCB_RDY_LCB_Index) != G_TRUE) {printf("invalid TCB_RDY_LCB_Index"); while(1); return (G_FALSE); }
			if ((ptcb->TCB_NextTCBASL != (struct gs_scb *) 0) && (gkm_SCB_IsValid((GS_SCB *) ptcb->TCB_NextTCBASL) != G_TRUE)) {printf("ERROR No valid TCB_NextTCBASL"); while(1); return (G_FALSE);}
			if ((ptcb->TCB_NextTCBPSL != (struct gs_scb *) 0) && (gkm_SCB_IsValid((GS_SCB *) ptcb->TCB_NextTCBPSL) != G_TRUE)) {printf("ERROR No valid TCB_NextTCBPSL"); while(1); return (G_FALSE);}
			
			if ((ptcb->TCB_AssocPCB < 0) || (ptcb->TCB_AssocPCB > G_NUMBER_OF_PCB)) {printf("ERROR No valid processor= %d\n", (int) ptcb->TCB_AssocPCB); return (G_FALSE);}
			
			list_root_value = gkm_TCB_list_ROOT(ptcb);
			
			if ((ptcb->TCB_NextTCBAEL != (struct gs_ecb *) 0) && (gkm_ECB_IsValid(ptcb->TCB_NextTCBAEL) != G_TRUE)) {printf("Invalid TCB_NextTCBAEL, TCB_NextTCBAEL=%p", ptcb->TCB_NextTCBAEL); while(1); return (G_FALSE);}
				
			
			if (!(consequent || (!antecedent))) {printf("Invalid TCB_PrevTCB 1, TCBState=%d", ptcb->TCBState); while(1); return (G_FALSE);}
			
			// if TCB_PrevTCB = 0 then list_root_value = ptcb
			antecedent = (ptcb->TCB_PrevTCB == (GS_TCB *) 0);
			consequent = (list_root_value == ptcb);
			if (!(consequent || (!antecedent))) {printf("Invalid TCB_PrevTCB 1, TCBState=%d", ptcb->TCBState); while(1); return (G_FALSE);}
			
			// if TCB_PrevTCB != 0 then TCB_PrevTCB->TCB_NextTCB = ptcb
			antecedent = (ptcb->TCB_PrevTCB != (GS_TCB *) 0);
			if (!((!antecedent) || (ptcb->TCB_PrevTCB->TCB_NextTCB == ptcb))) {printf("Invalid TCB_PrevTCB 2, TCBState=%d", ptcb->TCBState); while(1); return (G_FALSE);}
			
			// if TCB_NextTCB != 0 then TCB_NextTCB->TCB_PrevTCB = ptcb
			antecedent = (ptcb->TCB_NextTCB != (GS_TCB *) 0);
			if (!((!antecedent) || (ptcb->TCB_NextTCB->TCB_PrevTCB == ptcb))) {printf("Invalid TCB_NextTCB TCBState=%d", ptcb->TCBState); while(1); return (G_FALSE);}
			
			// priority in ready list
			antecedent = ((ptcb->TCBState == G_TCBState_READY) && (ptcb->TCB_NextTCB != (GS_TCB *) 0));
			if (!((!antecedent) || (ptcb->TCBReadyPriority <= ptcb->TCB_NextTCB->TCBReadyPriority))) {printf("Invalid TCBReadyPriority=0x%llx, next=0x%llx", (unsigned long long) ptcb->TCBReadyPriority, (unsigned long long) ptcb->TCB_NextTCB->TCBReadyPriority); while(1); return (G_FALSE);}
			
			// priority in run list
			antecedent = ((ptcb->TCBState == G_TCBState_RUNNING) && (ptcb->TCB_NextTCB != (GS_TCB *) 0));
			if (!((!antecedent) || (ptcb->TCBRunPriority >= ptcb->TCB_NextTCB->TCBRunPriority))) {printf("Invalid TCBRunPriority=0x%llx, next=0x%llx", (unsigned long long) ptcb->TCBRunPriority, (unsigned long long) ptcb->TCB_NextTCB->TCBRunPriority); while(1); return (G_FALSE);}
			
			// Associated event list
			antecedent = ((ptcb->TCB_NextTCBAEL != (GS_ECB *) 0));
			if (!((!antecedent) || (ptcb->TCB_NextTCBAEL->ECB_AssocTCB == ptcb))) {printf("Invalid TCB_NextTCBAEL TCBState=%d", ptcb->TCBState); while(1); return (G_FALSE);}
			
			// Associated signal list
			antecedent = ((ptcb->TCB_NextTCBASL != (GS_SCB *) 0));
			if (!((!antecedent) || ((GS_TCB *) ptcb->TCB_NextTCBASL->SCB_AssocXCB == ptcb))) {printf("Invalid TCB_NextTCBASL TCBState=%d", ptcb->TCBState); while(1); return (G_FALSE);}
			
			// Associated pending signal list
			antecedent = ((ptcb->TCB_NextTCBPSL != (GS_SCB *) 0));
			if (!((!antecedent) || ((GS_TCB *) ptcb->TCB_NextTCBPSL->SCB_AssocXCB == ptcb))) {printf("Invalid TCB_NextTCBPSL TCBState=%d", ptcb->TCBState); while(1); return (G_FALSE);}
			
			// Stack botton
			antecedent = (1);
			consequent = ((ptcb->TCB_StackBottom >= ptcb->TCB_StackPointer) && (ptcb->TCB_StackTop <= ptcb->TCB_StackPointer));
			if (!(consequent || (!antecedent))) {printf("Invalid STACK botton or top"); while(1); return (G_FALSE);}
			
			// Associated processor
			antecedent = ((ptcb->TCB_AssocPCB != (unsigned int) 0));
			if (!((!antecedent) || (g_kcb.G_PCBTbl[(ptcb->TCB_AssocPCB) - 1].PCB_EXECTCB == (struct gs_tcb *) ptcb))) {printf("Invalid TCB_AssocPCB TCB_AssocPCB=%d, g_kcb=%p, ptcb=%p", ptcb->TCB_AssocPCB, (void *) g_kcb.G_PCBTbl[(ptcb->TCB_AssocPCB) - 1].PCB_EXECTCB, (void *) ptcb); while(1); while(1); return (G_FALSE);}
			
			// Interrupt ISR 1
			antecedent = ((ptcb->TCB_INTNumber >= 0) && (ptcb->TCB_INTNumber <= G_NUMBER_OF_IRQ_TCB));
			if (!((!antecedent) || (gkm_RCB_IsValid(g_kcb.KCB_NextRCBTRGL[ptcb->TCB_INTNumber]) == G_TRUE))) {printf("Invalid Interrupt ISR 1 TCB_INTNumber=%d", ptcb->TCB_INTNumber); while(1); return (G_FALSE);}
			
			// Interrupt ISR 1
			antecedent = ((ptcb->TCB_INTNumber > G_NUMBER_OF_IRQ_TCB));
			if (!((!antecedent) || (gkm_RCB_IsValid((G_RCB *) ptcb->TCB_INTNumber) == G_TRUE))) {printf("Invalid Interrupt ISR 2 TCB_INTNumber=%d", ptcb->TCB_INTNumber); while(1); return (G_FALSE);}
			
			// TCB_RDY_LCB_Index 1
			antecedent = (((ptcb->TCBState == G_TCBState_RUNNING) || (ptcb->TCBState == G_TCBState_READY)) && (ptcb->TCB_NextTCB != (GS_TCB *) 0));
			if (!((!antecedent) || ((ptcb->TCB_RDY_LCB_Index == ptcb->TCB_NextTCB->TCB_RDY_LCB_Index) && (ptcb->TCBState == ptcb->TCB_NextTCB->TCBState)))) {printf("Invalid TCB_RDY_LCB_Index 1"); while(1); return (G_FALSE);}
			
			// TCB_RDY_LCB_Index 2
			antecedent = ((ptcb->TCBState == G_TCBState_RUNNING) && (ptcb->TCB_PrevTCB == (GS_TCB *) 0));
			if (!((!antecedent) || (ptcb->TCB_RDY_LCB_Index->LCB_NextTCBRUNL == ptcb))) {printf("Invalid TCB_RDY_LCB_Index 2"); while(1); return (G_FALSE);}
			
			// TCB_RDY_LCB_Index 3
			antecedent = ((ptcb->TCBState == G_TCBState_READY) && (ptcb->TCB_PrevTCB == (GS_TCB *) 0) && (ptcb->TCBType != G_TCBType_IDLE));
			if (!((!antecedent) || (ptcb->TCB_RDY_LCB_Index->LCB_NextTCBRDYL == ptcb))) {printf("Invalid TCB_RDY_LCB_Index 3"); while(1); while(1); return (G_FALSE);}
			

			// #############################################
			// Debug previous structure
			if (ptcb->TCB_PREV_TCBs != ptcb_prv) {printf("Invalid TCB_PREV_TCBs"); while(1); return (G_FALSE);}
			ptcb_prv = ptcb;
			
		}
		if (NBR_TCBs != g_kcb.KCB_NUMBER_OF_TCBs) {printf("Invalid KCB_NUMBER_OF_TCBs NBR_TCBs=%d, KCB_NUMBER_OF_TCBs=%d", (int) NBR_TCBs, (int) g_kcb.KCB_NUMBER_OF_TCBs); while(1); return (G_FALSE);}
		return (G_TRUE);
	}

	/// Checks the PCB associated LCBs consistency
	G_INT32 gkm_Check_PCBAssocLCBs(G_INT32 partial)
	{
		struct gs_pcb_rdy_lcbl * ppcb_assoc_lcb;
		struct gs_pcb_rdy_lcbl * ppcb_assoc_lcb_prv = (struct gs_pcb_rdy_lcbl *) 0;

		int antecedent;
		int consequent;
		
		int NBR_PCBAssocLCBs = 0;
		for (ppcb_assoc_lcb = g_kcb.KCB_ROOT_RDYs; ppcb_assoc_lcb != (struct gs_pcb_rdy_lcbl *) 0; ppcb_assoc_lcb = ppcb_assoc_lcb->gs_pcb_lcbl_nexts) {
			NBR_PCBAssocLCBs++;
			if (gkm_PCBAssocLCB_IsValid(ppcb_assoc_lcb) != G_TRUE) {printf("invalid gs_pcb_rdy_lcbl"); while(1); return (G_FALSE); }

			// Next pcbassoclcb
			antecedent = ((ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next != (struct gs_pcb_rdy_lcbl *) 0));
			if (!((!antecedent) || ((ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next->gs_pcb_rdy_lcbl_prev == ppcb_assoc_lcb) && (ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next->PCB_RDY_priority >= ppcb_assoc_lcb->PCB_RDY_priority)))) {printf("Invalid next pcbassoclcb"); while(1); return (G_FALSE);}

			// LCB previous of next
			antecedent = ((ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next != (struct gs_pcb_rdy_lcbl *) 0));
			if (!((!antecedent) || ((ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next->gs_pcb_rdy_lcbl_prev == ppcb_assoc_lcb) && (ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next->PCB_RDY_priority >= ppcb_assoc_lcb->PCB_RDY_priority)))) {printf("Invalid previous pcbassoclcb of next"); while(1); return (G_FALSE);}

			// LCB associated valid
			antecedent = (1);
			consequent = (gkm_LCB_IsValid(ppcb_assoc_lcb->PCB_RDY_LCBL) == G_TRUE);
			if (!((!antecedent) || consequent)) {printf("Invalid LCB associated valid"); while(1); return (G_FALSE);}


			// #############################################
			// Debug previous structure
			if (ppcb_assoc_lcb->gs_pcb_lcbl_prevs != ppcb_assoc_lcb_prv) {printf("Invalid gs_pcb_lcbl_prevs"); while(1); return (G_FALSE);}
			ppcb_assoc_lcb_prv = ppcb_assoc_lcb;        
		}
		if (NBR_PCBAssocLCBs != g_kcb.KCB_NUMBER_OF_PCBAssocLCBs) {printf("Invalid NBR_PCBAssocLCBs NBR_TCBs=%d, KCB_NUMBER_OF_PCBAssocLCBs=%d", (int) NBR_PCBAssocLCBs, (int) g_kcb.KCB_NUMBER_OF_PCBAssocLCBs); while(1); return (G_FALSE);}
		return (G_TRUE);
	}

	/// Checks the PCB associated LCBs consistency
	G_INT32 gkm_Check_LCBs(G_INT32 partial)
	{
		GS_LCB *plcb;
		GS_LCB *plcb_prv = (GS_LCB *) 0;

		int antecedent;
		int consequent;
		
		int NBR_LCBs = 0;
		for (plcb = g_kcb.KCB_ROOT_LCBs; plcb != (GS_LCB *) 0; plcb = plcb->LCB_NEXT_LCBs) {
			NBR_LCBs++;
			if (gkm_LCB_IsValid(plcb) != G_TRUE) {printf("invalid LCB"); while(1); return (G_FALSE); }

			// LCB exclusion
			// 23-10-0224 antecedent = (1);
			// 23-10-0224 consequent = (plcb->LCBExclusion >= plcb->LCBCurrentRunning);
			// 23-10-0224 if (!((!antecedent) || consequent)) {printf("Invalid LCB exclusion"); while(1); return (G_FALSE);}

			// LCB running list
			antecedent = (plcb->LCB_NextTCBRUNL == (GS_TCB *) 0);
			consequent = ((plcb->LCBCurrentRunning == 0));
			if (!((!antecedent) || consequent)) {printf("Invalid LCB running list Excl=%d, running=%d", plcb->LCBExclusion, plcb->LCBCurrentRunning); while(1); return (G_FALSE);}

			// LCB next LCB
			antecedent = (plcb->LCB_NextLCBL != (GS_LCB *) 0);
			if (!((!antecedent) || ((plcb->LCB_NextLCBL->LCB_PrevLCBL == plcb)))) {printf("Invalid LCB next LCB"); while(1); return (G_FALSE);}

			// LCB root LCB
			antecedent = (plcb->LCB_PrevLCBL == (GS_LCB *) 0);
			if (!((!antecedent) || ((g_kcb.KCB_NextLCBL == plcb)))) {printf("Invalid LCB root LCB"); while(1); return (G_FALSE);}
			
			
			// #############################################
			// Debug previous structure
			if (plcb->LCB_PREV_LCBs != plcb_prv) {printf("Invalid LCB_PREV_LCBs"); while(1); return (G_FALSE);}
			plcb_prv = plcb;        
		}
		if (NBR_LCBs != g_kcb.KCB_NUMBER_OF_LCBs) {printf("Invalid NBR_LCBs NBR_TCBs=%d, KCB_NUMBER_OF_LCBs=%d", (int) NBR_LCBs, (int) g_kcb.KCB_NUMBER_OF_LCBs); while(1); return (G_FALSE);}
		return (G_TRUE);
	}

	/// Checks the PCB associated LCBs consistency
	G_INT32 gkm_Check_RCBs(G_INT32 partial)
	{
		G_RCB *prcb;
		G_RCB *prcb_prv = (G_RCB *) 0;

		int NBR_RCBs = 0;
		for (prcb = (G_RCB *) g_kcb.KCB_ROOT_RCBs; prcb != (G_RCB *) 0; prcb = (G_RCB *) prcb->RCB_NEXT_RCBs) {
			NBR_RCBs++;
			if (gkm_RCB_IsValid(prcb) != G_TRUE) {printf("invalid RCB"); while(1); return (G_FALSE); }

			// #############################################
			// Debug previous structure
			if (prcb->RCB_PREV_RCBs != prcb_prv) {printf("Invalid RCB_PREV_RCBs"); while(1); return (G_FALSE);}
			prcb_prv = prcb;        
		}
		if (NBR_RCBs != g_kcb.KCB_NUMBER_OF_RCBs) {printf("Invalid NBR_RCBs NBR_RCBs=%d, KCB_NUMBER_OF_RCBs=%d", (int) NBR_RCBs, (int) g_kcb.KCB_NUMBER_OF_RCBs); while(1); return (G_FALSE);}
		return (G_TRUE);
	}


	/// Checks the PCB associated LCBs consistency
	G_INT32 gkm_Check_RRDSs(G_INT32 partial)
	{
		GS_RRDS *prrds;
		GS_RRDS *prrds_prv = (GS_RRDS *) 0;
		
		int NBR_RRDSs = 0;
		for (prrds = (GS_RRDS *) g_kcb.KCB_ROOT_RRDSs; prrds != (GS_RRDS *) 0; prrds = (GS_RRDS *) prrds->RRDS_NEXT_RRDSs) {
			NBR_RRDSs++;
			if (gkm_RRDS_IsValid(prrds) != G_TRUE) {printf("invalid RRDS"); while(1); return (G_FALSE);}

			// RRDS_AsocECB
			// antecedent = ((prrds != (G_RCB *) 0) && (get_pointed_field(prrds, RRDS_AsocECB) != (GS_ECB *) 0));
			// consequent = (get_pointed_field(get_pointed_field(prrds, RRDS_AsocECB), ECB_RRDS) == prrds);
			if (((prrds != (GS_RRDS *) 0) && (get_pointed_field(prrds, RRDS_AsocECB) != (GS_ECB *) 0)) && (!(get_pointed_field(get_pointed_field(prrds, RRDS_AsocECB), ECB_RRDS) == prrds))) {printf("Error in RRDS_AsocECB"); while(1); return (G_FALSE);}
			
			// #############################################
			// Debug previous structure
			if (prrds->RRDS_PREV_RRDSs != prrds_prv) {printf("Invalid RRDS_PREV_RRDSs"); while(1); return (G_FALSE);}
			prrds_prv = prrds;        
		}
		if (NBR_RRDSs != g_kcb.KCB_NUMBER_OF_RRDSs) {printf("Invalid NBR_RRDSs NBR_RRDSs=%d, KCB_NUMBER_OF_RRDSs=%d", (int) NBR_RRDSs, (int) g_kcb.KCB_NUMBER_OF_RRDSs); while(1); return (G_FALSE);}
		return (G_TRUE);
	}

	G_INT32 gkm_Check_PCBs(G_INT32 partial)
	{
		int antecedent;
		int consequent;
		int i;
		
		// Check processors
		// Assigned task should be different for different processors
		for (i = 0;  i < G_NUMBER_OF_PCB;  i++) {
			if (g_kcb.G_PCBTbl[i].PCBState != GS_PCBState_NOTRUNNING)
			{
				if (g_kcb.G_PCBTbl[i].PCBID != i+1) {printf("Invalid PCB PCBID=%d, i=%d",g_kcb.G_PCBTbl[i].PCBID,i); return (G_FALSE);}
				if (gkm_PCBState_Valid(g_kcb.G_PCBTbl[i].PCBState) != G_TRUE) {printf("Invalid PCB State=%d",g_kcb.G_PCBTbl[i].PCBState); while(1); return (G_FALSE);}
				if (gkm_PCBType_Valid(g_kcb.G_PCBTbl[i].PCBType) != G_TRUE) {printf("Invalid PCB Type=%d",g_kcb.G_PCBTbl[i].PCBType); while(1); return (G_FALSE);}
				if (gkm_TCB_IsValid(g_kcb.G_PCBTbl[i].PCB_EXECTCB) != G_TRUE) {printf("Error in exec proc=%u, i=%u, task=%p, PCBState= %u\n", gm_ProcessorId, i, g_kcb.G_PCBTbl[i].PCB_EXECTCB, g_kcb.G_PCBTbl[i].PCBState); return (G_FALSE);}
				if (gkm_TCB_IsValid(g_kcb.G_PCBTbl[i].PCB_IDLETCB) != G_TRUE) {printf("Error in idle"); while(1); return (G_FALSE);}


				// PCB Free
				antecedent = ((g_kcb.G_PCBTbl[i].PCBState == GS_PCBState_FREE) && (g_kcb.G_PCBTbl[i].PCB_PrevPCB == (struct gs_pcb *) 0));
				consequent = (g_kcb.G_PCBTbl[i].PCB_AssocLCB != (struct gs_pcb_rdy_lcbl *) 0 && g_kcb.G_PCBTbl[i].PCB_AssocLCB->PCB_RDY_LCBL != (struct gs_lcb *) 0 && g_kcb.G_PCBTbl[i].PCB_AssocLCB->PCB_RDY_LCBL->LCB_NextLCBFPL == (struct gs_pcb *) &g_kcb.G_PCBTbl[i]);
				if (!((!antecedent) || consequent)) {printf("Error in FREE PCB"); while(1); return (G_FALSE);}

				// Next free PCB valid SEGMENTATION FAULT CHECK CERO POINTER !!!!!!!!!!!!
				// 16-07-2024 antecedent = (g_kcb.G_PCBTbl[i].PCB_NextPCB != (struct gs_pcb *) 0);
				// 16-07-2024 consequent = ((gkm_PCB_IsValid(g_kcb.G_PCBTbl[i].PCB_NextPCB) == G_TRUE) && (g_kcb.G_PCBTbl[i].PCB_NextPCB->PCBState == GS_PCBState_FREE));
				// 16-07-2024 if (!((!antecedent) || consequent)) {printf("Error Next free PCB valid"); while(1); return (G_FALSE);}

				// Previous PCB valid SEGMENTATION FAULT CHECK CERO POINTER !!!!!!!!!!!!
				// 16-07-2024 antecedent = (g_kcb.G_PCBTbl[i].PCB_PrevPCB != (struct gs_pcb *) 0);
				// 16-07-2024 consequent = ((gkm_PCB_IsValid(g_kcb.G_PCBTbl[i].PCB_PrevPCB) == G_TRUE) && (g_kcb.G_PCBTbl[i].PCB_PrevPCB->PCBState == GS_PCBState_FREE));
				// 16-07-2024 if (!((!antecedent) || consequent)) {printf("Error Previous free PCB valid"); while(1); return (G_FALSE);}

				// PCB_AssocLCB valid
				antecedent = (g_kcb.G_PCBTbl[i].PCB_AssocLCB != (struct gs_pcb_rdy_lcbl *) 0);
				consequent = (gkm_PCBAssocLCB_IsValid(g_kcb.G_PCBTbl[i].PCB_AssocLCB) == G_TRUE);
				if (!((!antecedent) || consequent)) {printf("Error PCB_AssocLCB"); while(1); return (G_FALSE);}
			}


			// check the list of associated lcb of the processor
			// struct gs_pcb_rdy_lcbl *ppcdb_assoc_lcb = g_kcb.G_PCBTbl[i].PCB_AssocLCB;
			// struct gs_pcb_rdy_lcbl *ppcdb_assoc_lcb_prev = (struct gs_pcb_rdy_lcbl *) 0;
			// while (ppcdb_assoc_lcb != (struct gs_pcb_rdy_lcbl *) 0) {
			//     if (gkm_PCBAssocLCB_IsValid(ppcdb_assoc_lcb) != G_TRUE) {printf("ERROR ppcdb_assoc_lcb"); while(1); return G_FALSE;}
			//     if (ppcdb_assoc_lcb->gs_pcb_rdy_lcbl_prev != ppcdb_assoc_lcb_prev) {printf("ERROR ppcdb_assoc_lcb_prev"); while(1); return G_FALSE;}
			//     if (gkm_LCB_IsValid(ppcdb_assoc_lcb->PCB_RDY_LCBL) != G_TRUE) {printf("Error in PCB_RDY_LCBL"); while(1); return G_FALSE;}
			//     ppcdb_assoc_lcb_prev = ppcdb_assoc_lcb;
			//     ppcdb_assoc_lcb = ppcdb_assoc_lcb->gs_pcb_rdy_lcbl_next;
			// }
		}
		// for (i = 0;  i < G_NUMBER_OF_PCB-1;  i++) {            
		//     for (j = i+1;  j < G_NUMBER_OF_PCB;  j++) {
		//         if (g_kcb.G_PCBTbl[i].PCB_EXECTCB == g_kcb.G_PCBTbl[j].PCB_EXECTCB) {printf("duplicated exec"); while(1); return G_FALSE;}
		//         if (g_kcb.G_PCBTbl[i].PCB_IDLETCB == g_kcb.G_PCBTbl[j].PCB_IDLETCB) {printf("duplicated idle"); while(1); return G_FALSE;}
		//     }
		// }
		return (G_TRUE);
	}


	G_INT32 gkm_Check_GeMRTOS(void)
	{
		// 03-07-2024 if (gk_trg_monitor(G_FALSE) == G_FALSE) {return (G_FALSE);}
		if (gkm_Check_PCBs(G_FALSE) == G_FALSE) {return (G_FALSE);}
		if (gkm_Check_ECBs(G_FALSE) == G_FALSE) {return (G_FALSE);}
		if (gkm_Check_TCBs(G_FALSE) == G_FALSE) {return (G_FALSE);}
		if (gkm_Check_PCBAssocLCBs(G_FALSE) == G_FALSE) {return (G_FALSE);}
		if (gkm_Check_LCBs(G_FALSE) == G_FALSE) {return (G_FALSE);}
		if (gkm_Check_RCBs(G_FALSE) == G_FALSE) {return (G_FALSE);}
		if (gkm_Check_RRDSs(G_FALSE) == G_FALSE) {return (G_FALSE);}
		return (G_TRUE);
	}

	G_INT32 gkm_Check_list_LCBL_IsValid(int cycles)
	{

		static int count = 0;
		GS_LCB *element = g_kcb.KCB_NextLCBL;
		GS_LCB *previous_should = (GS_LCB *) 0;
		int i;
		count++;
		
		// if ((G_Running == G_TRUE) && (count >= cycles))
		if ((count >= cycles))
		{
			count = 0;
			// Check trigger
		   // 03-07-2024 if (gk_trg_monitor(G_TRUE) == G_FALSE) {return (G_FALSE);}
		   if (gkm_Check_PCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		   if (gkm_Check_ECBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		   if (gkm_Check_TCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		   if (gkm_Check_PCBAssocLCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		   if (gkm_Check_LCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		   if (gkm_Check_RCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		 if (gkm_Check_RRDSs(G_TRUE) == G_FALSE) {return (G_FALSE);}
			
			// Check lowest priority processor
			i = (int) GRTOS_CMD_LOW_PRC_GET;
			if ((i < 1) || (i > G_NUMBER_OF_PCB)) {printf("ERROR No valid processor= %d\n", (int) i); return (G_FALSE);}
			
			// Check scheduling lists
			while (element != (GS_LCB *) 0) {
				if (gkm_LCB_IsValid(element) != G_TRUE) {printf("invalid element"); while(1); return (G_FALSE); }
				if (element->LCB_PrevLCBL != previous_should) {
					printf("invalid previous element= %p, prev=%p",(void *) element->LCB_PrevLCBL, (void *)previous_should);
					return (G_FALSE);
				}
				
				// Check run list
				struct gs_tcb *ptcb = element->LCB_NextTCBRUNL;
				struct gs_tcb *ptcb_prev = (struct gs_tcb *) 0;
				while (ptcb != (struct gs_tcb *) 0 ) {
					if (gkm_TCB_IsValid(ptcb) != G_TRUE) {printf("invalid ptcb in lcb "); while(1); return (G_FALSE);}
					if (ptcb_prev != ptcb->TCB_PrevTCB) {printf("invalid TCB_PrevTCB in lcb "); while(1); return (G_FALSE);}
					if (ptcb->TCB_AssocPCB == 0) {printf("invalid running TCB_AssocPCB in tcb "); while(1); return (G_FALSE);}
					if (ptcb->TCB_RDY_LCB_Index != element) {printf("invalid running TCB_RDY_LCB_Index"); while(1); return (G_FALSE);}
					ptcb_prev = ptcb;
					ptcb = (struct gs_tcb *) ptcb->TCB_NextTCB;
				}
				
				// Check ready list
				ptcb = element->LCB_NextTCBRDYL;
				ptcb_prev = (struct gs_tcb *) 0;
				while (ptcb != (struct gs_tcb *) 0 ) {
					if (gkm_TCB_IsValid(ptcb) != G_TRUE) {printf("invalid ptcb in lcb "); while(1); return (G_FALSE);}
					if (ptcb_prev != ptcb->TCB_PrevTCB) {printf("invalid TCB_PrevTCB in lcb "); while(1); return (G_FALSE);}
					if (ptcb->TCB_AssocPCB != 0) {printf("invalid ready TCB_AssocPCB in tcb "); while(1); return (G_FALSE);}
					if (ptcb->TCB_RDY_LCB_Index != element) {printf("invalid ready TCB_RDY_LCB_Index"); while(1); return (G_FALSE);}
					ptcb_prev = ptcb;
					ptcb = (struct gs_tcb *) ptcb->TCB_NextTCB;
				}
				previous_should = element;
				element = element->LCB_NextLCBL;
			}
		}
		return (G_TRUE);
	}
	
#endif /* G_DEBUG_WHILEFOREVER_ENABLE == 1 */

OPTIMEZE_RESTORE
