/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Monitor functions for debugging
 *  \details This file contains the Monitor functions for debugging.
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

/** \brief Maximum call-stack frames printed by gkm_PrintFunctionStack().
 *  Bounds the while-loop to prevent infinite traversal on a corrupt
 *  SAMPLINGS chain. */
#define G_PRINT_STACK_MAX_FRAMES  50U

OPTIMIZE_CODE


/*****************************************************************************/
/*****************************************************************************/
/**         DEBUG MONITOR VARIABLES                                          */
/**         COPY MIRROR OF SYSTEM VARIABLES                                  */
/*****************************************************************************/
/*****************************************************************************/
struct GK_SAMPLE_FUNCTION SAMPLINGS[100];
volatile G_UINT32 current_function;
volatile G_UINT64 current_time;
volatile G_UINT64 maximum_time;
volatile G_UINT32 maximum_time_function;

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Checks whether a value is a valid member of an enumeration.
	 *
	 * Delegates to the #ENUM_CHECK macro, which validates \p value against
	 * the compile-time descriptor identified by \p hash.
	 *
	 * \param [in] hash  Hash key identifying the target enum type.
	 * \param [in] value Numeric value to validate.
	 * \return Non-zero if \p value is a valid enum member; zero otherwise.
	 */
	G_UINT32 gkm_ENUM_IsValid(G_UINT32 hash, G_UINT32 value)
	{
		return (ENUM_CHECK(hash, value));  
	}

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Returns the head of the scheduler list currently holding \p ptcb.
	 *
	 * Selects the correct list root by inspecting TCBType and TCBState:
	 * - IDLE task              -> \p ptcb itself
	 * - WAITING / WAITING_COMPLETED -> g_kcb.KCB_NextTCBWL
	 * - READY                  -> LCB_NextTCBRDYL of the task's ready-list
	 * - RUNNING                -> LCB_NextTCBRUNL of the task's ready-list
	 * - UNLINKED               -> \p ptcb itself
	 *
	 * \param [in] ptcb  Task control block to examine.  Must be valid.
	 * \return Pointer to the list root, or NULL if the TCB state is unrecognised.
	 * \relates GS_TCB
	 */
	GS_TCB *gkm_TCB_list_ROOT(const GS_TCB *ptcb) /* const per MISRA C:2012 Rule 8.13 */
	{
		if (ptcb->TCBType == G_TCBType_IDLE) {return ((GS_TCB *) ptcb);}
		if (ptcb->TCBState == G_TCBState_WAITING_COMPLETED) {return ((GS_TCB *) g_kcb.KCB_NextTCBWL);}
		if (ptcb->TCBState == G_TCBState_WAITING)  {return ((GS_TCB *) g_kcb.KCB_NextTCBWL);}
		if (ptcb->TCBState == G_TCBState_READY) {return ((GS_TCB *) ptcb->TCB_RDY_LCB_Index->LCB_NextTCBRDYL);}
		if (ptcb->TCBState == G_TCBState_RUNNING)  {return ((GS_TCB *) ptcb->TCB_RDY_LCB_Index->LCB_NextTCBRUNL);}
		if (ptcb->TCBState == G_TCBState_UNLINKED) {return ((GS_TCB *) ptcb);}

		return((GS_TCB*) 0);
	}

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Returns the head of the kernel list currently holding \p pevent.
	 *
	 * Selects the correct list root by inspecting ECBState and ECBType:
	 * - FREE                               -> KCB_FREE_ECBs
	 * - Timed (OSTimeDly/PERIODIC/LASTEST)  -> KCB_NextECBTL
	 * - SEM_GRANTED / MSG receiver busy/wait/expired -> RCB_NextRCBGEL
	 * - SEM_WAITING / MSG sender wait/delivered/expired/timeout -> RCB_NextRCBWEL
	 * - EXT_ISR_WAITING -> RCB_NextRCBWEL; EXT_ISR_RELEASE -> RCB_NextRCBGEL
	 *
	 * \param [in] pevent  Event control block to examine.  Must be valid.
	 * \return Pointer to the list root, or NULL if the ECB type is unrecognised.
	 * \relates GS_ECB
	 */
	GS_ECB *gkm_ECB_list_ROOT(const GS_ECB *pevent) /* const per MISRA C:2012 Rule 8.13 */
	{
		GS_RCB *presource = (GS_RCB *) pevent->ECB_AssocRCB;
		
		if (pevent->ECBState == GS_ECBState_FREE) {return ((GS_ECB *) g_kcb.KCB_FREE_ECBs);}
		if (pevent->ECBType == G_ECBType_OSTimeDly) {return ((GS_ECB *) g_kcb.KCB_NextECBTL);}
		if (pevent->ECBType == G_ECBType_PERIODIC)  {return ((GS_ECB *) g_kcb.KCB_NextECBTL);}
		if (pevent->ECBType == G_ECBType_LASTEST_TIME) {return ((GS_ECB *) g_kcb.KCB_NextECBTL);}
		if (pevent->ECBType == G_ECBType_SEM_GRANTED)  {return ((GS_ECB *) presource->RCB_NextRCBGEL);}
		if (pevent->ECBType == G_ECBType_SEM_WAITING)  {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_SEM_WAITING_TIMEOUT) {return ((GS_ECB *) g_kcb.KCB_NextECBTL);}
		if (pevent->ECBType == G_ECBType_SEM_WAITING_EXPIRED) {return ((GS_ECB *) pevent);}
		if (pevent->ECBType == G_ECBType_NOT_SPECIFIED) {return ((GS_ECB *) pevent);}
		if (pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_BUSY) {return ((GS_ECB *) presource->RCB_NextRCBGEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_WAIT) {return ((GS_ECB *) presource->RCB_NextRCBGEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_SUBSCRIBED) {return ((GS_ECB *) presource->RCB_NextRCBGEL);} 
		if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_WAIT) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_DELIVERED) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_EXPIRED) {return ((GS_ECB *) presource->RCB_NextRCBGEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_EXPIRED) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_TIMEOUT) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_EXT_ISR_WAITING) {return ((GS_ECB *) presource->RCB_NextRCBWEL);}
		if (pevent->ECBType == G_ECBType_EXT_ISR_RELEASE) {return ((GS_ECB *) presource->RCB_NextRCBGEL);}

		return ((GS_ECB*) 0);
	}

	/* GS_ListOrder is defined in gemrtos_core.h so that the header declaration
	 * and this definition share the same type. */

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Returns the expected sort order of the list that contains \p pevent.
	 *
	 * Used by consistency checks to verify that ECBs appear in the correct order
	 * within their respective kernel list.
	 *
	 * \param [in] pevent  Event control block to classify.  Must be valid.
	 * \return #GS_LIST_ORDER_DECREASING if sorted by decreasing ECBValue (e.g. granted semaphore list);
	 *         #GS_LIST_ORDER_INCREASING if sorted by increasing ECBValue (e.g. timed-event list);
	 *         #GS_LIST_ORDER_NOT_APPLICABLE if ordering is not applicable (free ECB or ISR type);
	 *         #GS_LIST_ORDER_UNRECOGNISED if the ECBType is not recognised.
	 * \relates GS_ECB
	 */
	GS_ListOrder gkm_ECB_list_ORDER(const GS_ECB *pevent) /* const per MISRA C:2012 Rule 8.13 */
	{
		if (pevent->ECBState == GS_ECBState_FREE) {return GS_LIST_ORDER_NOT_APPLICABLE;}
		if (pevent->ECBType == G_ECBType_OSTimeDly) {return GS_LIST_ORDER_INCREASING;}
		if (pevent->ECBType == G_ECBType_PERIODIC)  {return GS_LIST_ORDER_INCREASING;}
		if (pevent->ECBType == G_ECBType_LASTEST_TIME) {return GS_LIST_ORDER_INCREASING;}
		if (pevent->ECBType == G_ECBType_SEM_GRANTED)  {return GS_LIST_ORDER_DECREASING;}
		if (pevent->ECBType == G_ECBType_SEM_WAITING)  {return GS_LIST_ORDER_INCREASING;}
		if (pevent->ECBType == G_ECBType_NOT_SPECIFIED) {return GS_LIST_ORDER_INCREASING;}
		if (pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_BUSY) {return GS_LIST_ORDER_DECREASING;}
		if (pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_WAIT) {return GS_LIST_ORDER_DECREASING;}
		if (pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_SUBSCRIBED) {return GS_LIST_ORDER_DECREASING;}
		if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_WAIT) {return GS_LIST_ORDER_INCREASING;}
		if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_DELIVERED) {return GS_LIST_ORDER_INCREASING;}
		if (pevent->ECBType == G_ECBType_MESSAGE_RECEIVER_EXPIRED) {return GS_LIST_ORDER_DECREASING;}
		if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_EXPIRED) {return GS_LIST_ORDER_INCREASING;}
		if (pevent->ECBType == G_ECBType_MESSAGE_SENDER_TIMEOUT) {return GS_LIST_ORDER_INCREASING;}
		if (pevent->ECBType == G_ECBType_EXT_ISR_WAITING) {return GS_LIST_ORDER_NOT_APPLICABLE;}
		if (pevent->ECBType == G_ECBType_EXT_ISR_RELEASE) {return GS_LIST_ORDER_NOT_APPLICABLE;}

		return GS_LIST_ORDER_UNRECOGNISED;
	}

    /**
     * \ingroup GeMRTOSMonitor
     * \brief   Validates a processor ID value.
     *
     * Checks that \p pcbid lies within the closed interval [1, #G_NUMBER_OF_PCB].
     *
     * \param [in] pcbid  Processor ID to validate.
     * \return #G_TRUE if the ID is in range; #G_FALSE otherwise.
     */
    G_UINT32 gkm_PCBID_Valid(int pcbid)
	{
		if ((pcbid <= G_NUMBER_OF_PCB) && (pcbid >= 1)) {return (G_TRUE);}
		return (G_FALSE);
	}


	/**
	 * \brief Verify that a traversal count matches the KCB expected count.
	 *
	 * Called at the end of each gkm_Check_* function to confirm that the number
	 * of entries found during list traversal equals the count recorded in the KCB.
	 *
	 * \param [in] counted   Number of entries found during traversal.
	 * \param [in] expected  Number of entries recorded in the KCB.
	 * \param [in] name      Structure-type label used in the error message (e.g. "ECB").
	 * \return #G_TRUE if counts match; #G_FALSE otherwise
	 *         (also halts with \c while(1) on mismatch).
	 */
	static G_UINT32 gkm_verify_count(int counted, int expected, const char *name)
	{
		if (counted != expected) {
			gk_fprintf("Invalid %s count: found=%d, expected=%d", name, counted, expected);
			while(1);
			return (G_FALSE);
		}
		return (G_TRUE);
	}


    G_UINT32 gk_CheckTimePriorityLayout(void)
    {
        TIMEPRIORITY test;

        test.i64 = (G_UINT64)0x1122334455667788;

        return (G_UINT32)((test.i32[0] == (G_UINT32)0x55667788) &&
                          (test.i32[1] == (G_UINT32)0x11223344));
    }


	/* gkm_Check_ECBs's per-ECB invariants, extracted 2026-08-16 (split out to
	 * reduce complexity -- see the identical note above
	 * gkm_Check_TCBs's helpers for the full rationale, including why the
	 * inline while(1) calls some checks had are not reproduced. */

	static G_UINT32 gkm_ECB_CheckBlockValid(const GS_ECB *pevent)
	{
		if (gkm_ECB_IsValid(pevent) != G_TRUE) {gk_fprintf("Invalid ECB"); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_ECB_CheckTypeValid(const GS_ECB *pevent)
	{
		if (gkm_ECBType_Valid(pevent->ECBType) != G_TRUE) {gk_fprintf("Invalid ECB Type= %u\n", (unsigned int) pevent->ECBType); return (G_FALSE);}
		return (G_TRUE);
	}

	/* if ECB_PrevECB != 0 then pevent->ECB_PrevECB->ECB_NextECB = pevent */
	static G_UINT32 gkm_ECB_CheckPrevECBBacklink(const GS_ECB *pevent)
	{
		if (!((pevent->ECB_PrevECB == (struct gs_ecb *) 0) || (pevent->ECB_PrevECB->ECB_NextECB == pevent))) {gk_fprintf("Invalid ECB_PrevECB 2, ECBType=%d", pevent->ECBType); return (G_FALSE);}
		return (G_TRUE);
	}

	/* if ECB_PrevTCBAEL = 0 then pevent->ECB_AssocTCB->TCB_NextTCBAEL = pevent */
	static G_UINT32 gkm_ECB_CheckPrevTCBAELListRoot(const GS_ECB *pevent)
	{
		int antecedent = (pevent->ECB_PrevTCBAEL == (GS_ECB *) 0);
		int consequent = ((pevent->ECB_AssocTCB == (GS_TCB *) 0) || (pevent->ECB_AssocTCB->TCB_NextTCBAEL == pevent));
		if (!((!antecedent) || consequent)) {gk_fprintf("Invalid ECB_PrevTCBAEL 1, ECBType=%d", pevent->ECBType); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Previous Associated TCB event */
	static G_UINT32 gkm_ECB_CheckPrevTCBAELBacklink(const GS_ECB *pevent)
	{
		int antecedent = (pevent->ECB_PrevTCBAEL != (GS_ECB *) 0);
		if (!((!antecedent) || (pevent->ECB_PrevTCBAEL->ECB_NextTCBAEL == pevent))) {gk_fprintf("Invalid ECB_PrevTCBAEL 2, ECBType=%d", pevent->ECBType); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Next Associated TCB event */
	static G_UINT32 gkm_ECB_CheckNextTCBAELBacklink(const GS_ECB *pevent)
	{
		int antecedent = (pevent->ECB_NextTCBAEL != (GS_ECB *) 0);
		if (!((!antecedent) || (pevent->ECB_NextTCBAEL->ECB_PrevTCBAEL == pevent))) {gk_fprintf("Invalid ECB_NextTCBAEL, ECBType=%d", pevent->ECBType); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Next Associated signal */
	static G_UINT32 gkm_ECB_CheckNextECBASLAssoc(const GS_ECB *pevent)
	{
		if (((pevent->ECB_NextECBASL != (GS_SCB *) 0) && (pevent->ECB_NextECBASL->SCB_AssocXCB != (void *) pevent))) {gk_fprintf("Invalid ECB_NextECBASL->SCB_AssocXCB, ECBType=%d", pevent->ECBType); return (G_FALSE);}
		return (G_TRUE);
	}

	/* list increasing order */
	static G_UINT32 gkm_ECB_CheckIncreasingOrder(const GS_ECB *pevent)
	{
		int antecedent = ((pevent->ECB_NextECB != (GS_ECB *) 0) && (gkm_ECB_list_ORDER(pevent) == GS_LIST_ORDER_INCREASING));
		if (!((!antecedent) || (pevent->ECBValue.i64 <= pevent->ECB_NextECB->ECBValue.i64))) {gk_fprintf("Invalid increasing ECBValue=0x%llx, next=0x%llx, type=%d", (unsigned long long) pevent->ECBValue.i64, (unsigned long long) pevent->ECB_NextECB->ECBValue.i64, (int) pevent->ECBType); return (G_FALSE);}
		return (G_TRUE);
	}

	/* list decreasing order */
	static G_UINT32 gkm_ECB_CheckDecreasingOrder(const GS_ECB *pevent)
	{
		int antecedent = ((gkm_ECB_list_ORDER(pevent) == GS_LIST_ORDER_DECREASING) && (pevent->ECB_NextECB != (GS_ECB *) 0));
		if (!((!antecedent) || (pevent->ECBValue.i64 >= pevent->ECB_NextECB->ECBValue.i64))) {gk_fprintf("Invalid decreasing ECBValue=0x%llx, next=0x%llx, type=%d", (unsigned long long) pevent->ECBValue.i64, (unsigned long long) pevent->ECB_NextECB->ECBValue.i64, (int) pevent->ECBType); return (G_FALSE);}
		return (G_TRUE);
	}

	/* ISR waiting event -- only checked when partial == G_FALSE */
	static G_UINT32 gkm_ECB_CheckISRWaitingState(const GS_ECB *pevent)
	{
		int antecedent = (pevent->ECBType == G_ECBType_EXT_ISR_WAITING);
		if (!((!antecedent) || ((pevent->ECB_AssocTCB->TCBState == G_TCBState_WAITING) || (pevent->ECB_AssocTCB->TCBState == G_TCBState_WAITING_COMPLETED)))) {gk_fprintf("Invalid ISR waiting event type=%d, task state=%d, proc=%d", (int) pevent->ECBType, (int) pevent->ECB_AssocTCB->TCBState, (int) gm_ProcessorId()); return (G_FALSE);}
		return (G_TRUE);
	}

	/* ISR released event -- only checked when partial == G_FALSE */
	static G_UINT32 gkm_ECB_CheckISRReleasedState(const GS_ECB *pevent)
	{
		int antecedent = (pevent->ECBType == G_ECBType_EXT_ISR_RELEASE);
		if (!((!antecedent) || ((pevent->ECB_AssocTCB->TCBState == G_TCBState_READY) || (pevent->ECB_AssocTCB->TCBState == G_TCBState_RUNNING) || (pevent->ECB_AssocTCB->TCBState == G_TCBState_WAITING)))) {gk_fprintf("Invalid ISR released event type=%d, task state=%d, proc=%d", (int) pevent->ECBType, (int) pevent->ECB_AssocTCB->TCBState, (int) gm_ProcessorId()); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Debug previous structure */
	static G_UINT32 gkm_ECB_CheckPrevStructure(const GS_ECB *pevent, const GS_ECB *pevent_prv)
	{
		if (pevent->ECB_PREV_ECBs != pevent_prv) {gk_fprintf("Invalid ECB_PREV_ECBs"); return (G_FALSE);}
		return (G_TRUE);
	}

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Validate the consistency of every ECB in the system.
	 *
	 * Traverses g_kcb.KCB_ROOT_ECBs and for each ECB asserts: valid BLOCK_HASH,
	 * valid ECBType, correct doubly-linked prev/next pointers, correct associated-TCB
	 * event-list linkage, correct associated-signal-list back-pointer, increasing or
	 * decreasing value ordering, and (when partial == G_FALSE) correct ISR
	 * waiting/released state.  Also verifies that the traversal count matches
	 * KCB_NUMBER_OF_ECBs.
	 *
	 * \param [in] partial  G_TRUE to skip ISR-state cross-checks (safe during IRQ);
	 *                      G_FALSE for the full check.
	 * \return #G_TRUE if all ECBs are consistent; #G_FALSE on first violation.
	 *         gkm_Check_ECBs is only ever reached from inside
	 *         PRINT_ASSERT(gkm_Check_GeMRTOS(...) == G_TRUE, ...), which halts
	 *         with #G_DEBUG_WHILEFOREVER on any G_FALSE reaching it.
	 */
    G_UINT32 gkm_Check_ECBs(G_UINT32 partial)
    {
		GS_ECB *pevent;
		const GS_ECB *pevent_prv = (GS_ECB *) 0;

		int NBR_ECBs = 0;
		for (pevent = g_kcb.KCB_ROOT_ECBs; pevent != (GS_ECB *) 0; pevent = pevent->ECB_NEXT_ECBs)
		{
			NBR_ECBs++;

			if (gkm_ECB_CheckBlockValid(pevent) != G_TRUE) return (G_FALSE);
			if (gkm_ECB_CheckTypeValid(pevent) != G_TRUE) return (G_FALSE);
			if (gkm_ECB_CheckPrevECBBacklink(pevent) != G_TRUE) return (G_FALSE);
			if (gkm_ECB_CheckPrevTCBAELListRoot(pevent) != G_TRUE) return (G_FALSE);
			if (gkm_ECB_CheckPrevTCBAELBacklink(pevent) != G_TRUE) return (G_FALSE);
			if (gkm_ECB_CheckNextTCBAELBacklink(pevent) != G_TRUE) return (G_FALSE);
			if (gkm_ECB_CheckNextECBASLAssoc(pevent) != G_TRUE) return (G_FALSE);
			if (gkm_ECB_CheckIncreasingOrder(pevent) != G_TRUE) return (G_FALSE);
			if (gkm_ECB_CheckDecreasingOrder(pevent) != G_TRUE) return (G_FALSE);

			if (partial == G_FALSE)
			{
				if (gkm_ECB_CheckISRWaitingState(pevent) != G_TRUE) return (G_FALSE);
				if (gkm_ECB_CheckISRReleasedState(pevent) != G_TRUE) return (G_FALSE);
			}

			if (gkm_ECB_CheckPrevStructure(pevent, pevent_prv) != G_TRUE) return (G_FALSE);
			pevent_prv = pevent;
		}
		return gkm_verify_count(NBR_ECBs, g_kcb.KCB_NUMBER_OF_ECBs, "ECB");
	}


	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Validates priority ordering in the ready and run lists.
	 *
	 * Iterates every TCB in the global TCB list and asserts that:
	 * - Tasks in the ready list are sorted with non-decreasing TCBCurrentPriority.
	 * - Tasks in the run list are sorted with non-increasing TCBCurrentPriority.
	 *
	 * \return #G_TRUE if all priorities are consistent; #G_FALSE on first violation.
	 */
	G_UINT32 gkm_Check_Testing(void)
	{
		GS_TCB * ptcb;

		for (ptcb = g_kcb.KCB_ROOT_TCBs; ptcb != (GS_TCB *) 0; ptcb = (GS_TCB *) ptcb->TCB_NEXT_TCBs) {
			int antecedent;

			// priority in ready list
			antecedent = ((ptcb->TCBState == G_TCBState_READY) && (ptcb->TCB_NextTCB != (GS_TCB *) 0));
			if ((antecedent) && (!(ptcb->TCBCurrentPriority <= ptcb->TCB_NextTCB->TCBCurrentPriority))) {gk_fprintf("Invalid Ready TCBCurrentPriority= 0x%llx, task= %s, next= 0x%llx, task_next, %s", (unsigned long long) ptcb->TCBCurrentPriority, ptcb->TCB_description, (unsigned long long) ptcb->TCB_NextTCB->TCBCurrentPriority, ptcb->TCB_description); return (G_FALSE);}
			
			// priority in run list
			antecedent = ((ptcb->TCBState == G_TCBState_RUNNING) && (ptcb->TCB_NextTCB != (GS_TCB *) 0));
			if ((antecedent) && (!(ptcb->TCBCurrentPriority >= ptcb->TCB_NextTCB->TCBCurrentPriority))) 
			{
				gk_fprintf("Invalid Run TCBCurrentPriority= 0x%llx, task= %s, ptcb->TCBState= %u\n next Priority= 0x%llx, task_next, %s, ptcb->TCB_NextTCB->TCBState=%u\n", (unsigned long long) ptcb->TCBCurrentPriority, ptcb->TCB_description, (unsigned int) ptcb->TCBState, (unsigned long long) ptcb->TCB_NextTCB->TCBCurrentPriority, ptcb->TCB_NextTCB->TCB_description, (unsigned int) ptcb->TCB_NextTCB->TCBState);
				GS_LCB *RunList = (GS_LCB *) get_pointed_field(ptcb, TCB_RDY_LCB_Index);
				const GS_TCB *ptcb1 = get_pointed_field(RunList, LCB_NextTCBRUNL);
				while (ptcb1 != (GS_TCB *) 0) {
					gk_fprintf("task= %s, priority= %llu, TCBState= %d\n", ptcb1->TCB_description, ptcb1->TCBCurrentPriority, ptcb1->TCBState);
					ptcb1 = ptcb1->TCB_NextTCB;
				}
				
				
				return (G_FALSE);
			}
		}
		return (G_TRUE);
	}


	/* gkm_Check_TCBs's 20 per-TCB invariants, extracted 2026-08-16 (split out to
	 * reduce complexity). Each function below is exactly
	 * one of the original inline `if` conditions, unchanged, with its exact
	 * original fprintf message. The per-check `while(1)` calls some of the
	 * original checks had are not reproduced here: gkm_Check_TCBs is only
	 * ever reached through gkm_Check_GeMRTOS(), itself only ever called from
	 * inside PRINT_ASSERT(gkm_Check_GeMRTOS(...) == G_TRUE, ...) -- so any
	 * G_FALSE returned here already halts unconditionally one level up.
	 * The inline while(1) calls that existed on some (not all) of the
	 * original checks were therefore already dead code -- the `return`
	 * statement immediately after an executed `while(1)` can never run.
	 * Confirmed with Ricardo before removing them (2026-08-16): the halt is
	 * real either way, just one PRINT_ASSERT layer higher. */

	static G_UINT32 gkm_TCB_CheckBlockValid(const GS_TCB *ptcb)
	{
		if (gkm_TCB_IsValid(ptcb) != G_TRUE) {gk_fprintf("invalid TCB"); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_TCB_CheckStateValid(const GS_TCB *ptcb)
	{
		if (gkm_TCBState_Valid(ptcb->TCBState) != G_TRUE) {gk_fprintf("invalid TCBState"); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_TCB_CheckTypeValid(const GS_TCB *ptcb)
	{
		if (gkm_TCBType_Valid(ptcb->TCBType) != G_TRUE) {gk_fprintf("invalid TCBType"); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_TCB_CheckRdyLCBIndexValid(const GS_TCB *ptcb)
	{
		if (gkm_LCB_IsValid((GS_LCB *) ptcb->TCB_RDY_LCB_Index) != G_TRUE) {gk_fprintf("invalid TCB_RDY_LCB_Index"); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_TCB_CheckNextTCBASLValid(const GS_TCB *ptcb)
	{
		if ((ptcb->TCB_NextTCBASL != (struct gs_scb *) 0) && (gkm_SCB_IsValid((GS_SCB *) ptcb->TCB_NextTCBASL) != G_TRUE)) {gk_fprintf("ERROR No valid TCB_NextTCBASL"); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_TCB_CheckNextTCBPSLValid(const GS_TCB *ptcb)
	{
		if ((ptcb->TCB_NextTCBPSL != (struct gs_scb *) 0) && (gkm_SCB_IsValid((GS_SCB *) ptcb->TCB_NextTCBPSL) != G_TRUE)) {gk_fprintf("ERROR No valid TCB_NextTCBPSL"); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_TCB_CheckAssocPCBRange(const GS_TCB *ptcb)
	{
		if (ptcb->TCB_AssocPCB > G_NUMBER_OF_PCB) {gk_fprintf("ERROR gkm_Check_TCBs No valid processor= %d\n", (int) ptcb->TCB_AssocPCB); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_TCB_CheckNextTCBAELValid(const GS_TCB *ptcb)
	{
		if ((ptcb->TCB_NextTCBAEL != (struct gs_ecb *) 0) && (gkm_ECB_IsValid(ptcb->TCB_NextTCBAEL) != G_TRUE)) {gk_fprintf("Invalid TCB_NextTCBAEL, TCB_NextTCBAEL=%p", ptcb->TCB_NextTCBAEL); return (G_FALSE);}
		return (G_TRUE);
	}

	/* if TCB_PrevTCB = 0 then list_root_value = ptcb */
	static G_UINT32 gkm_TCB_CheckPrevTCBListRoot(const GS_TCB *ptcb, const GS_TCB *list_root_value)
	{
		int antecedent = (ptcb->TCB_PrevTCB == (GS_TCB *) 0);
		int consequent = (list_root_value == ptcb);
		if (!(consequent || (!antecedent))) {gk_fprintf("Invalid TCB_PrevTCB 1, TCBState=%d", ptcb->TCBState); return (G_FALSE);}
		return (G_TRUE);
	}

	/* if TCB_PrevTCB != 0 then TCB_PrevTCB->TCB_NextTCB = ptcb */
	static G_UINT32 gkm_TCB_CheckPrevTCBBacklink(const GS_TCB *ptcb)
	{
		int antecedent = (ptcb->TCB_PrevTCB != (GS_TCB *) 0);
		if (!((!antecedent) || (ptcb->TCB_PrevTCB->TCB_NextTCB == ptcb))) {gk_fprintf("Invalid TCB_PrevTCB 2, TCBState=%d", ptcb->TCBState); return (G_FALSE);}
		return (G_TRUE);
	}

	/* if TCB_NextTCB != 0 then TCB_NextTCB->TCB_PrevTCB = ptcb */
	static G_UINT32 gkm_TCB_CheckNextTCBBacklink(const GS_TCB *ptcb)
	{
		int antecedent = (ptcb->TCB_NextTCB != (GS_TCB *) 0);
		if (!((!antecedent) || (ptcb->TCB_NextTCB->TCB_PrevTCB == ptcb))) {gk_fprintf("Invalid TCB_NextTCB TCBState=%d", ptcb->TCBState); return (G_FALSE);}
		return (G_TRUE);
	}

	/* priority in ready list */
	static G_UINT32 gkm_TCB_CheckReadyListPriorityOrder(const GS_TCB *ptcb)
	{
		int antecedent = ((ptcb->TCBState == G_TCBState_READY) && (ptcb->TCB_NextTCB != (GS_TCB *) 0));
		if ((antecedent) && (ptcb->TCBCurrentPriority > ptcb->TCB_NextTCB->TCBCurrentPriority)) {gk_fprintf("Invalid in ready TCBCurrentPriority=0x%llx, next=0x%llx", (unsigned long long) ptcb->TCBCurrentPriority, (unsigned long long) ptcb->TCB_NextTCB->TCBCurrentPriority); return (G_FALSE);}
		return (G_TRUE);
	}

	/* priority in run list */
	static G_UINT32 gkm_TCB_CheckRunListPriorityOrder(const GS_TCB *ptcb)
	{
		int antecedent = ((ptcb->TCBState == G_TCBState_RUNNING) && (ptcb->TCB_NextTCB != (GS_TCB *) 0));
		if (!((!antecedent) || (ptcb->TCBCurrentPriority >= ptcb->TCB_NextTCB->TCBCurrentPriority))) {gk_fprintf("Invalid in run TCBCurrentPriority=0x%llx, next=0x%llx", (unsigned long long) ptcb->TCBCurrentPriority, (unsigned long long) ptcb->TCB_NextTCB->TCBCurrentPriority); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Associated event list */
	static G_UINT32 gkm_TCB_CheckAssocEventList(const GS_TCB *ptcb)
	{
		int antecedent = ((ptcb->TCB_NextTCBAEL != (GS_ECB *) 0));
		if (!((!antecedent) || (ptcb->TCB_NextTCBAEL->ECB_AssocTCB == ptcb))) {gk_fprintf("Invalid TCB_NextTCBAEL TCBState=%d", ptcb->TCBState); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Associated signal list */
	static G_UINT32 gkm_TCB_CheckAssocSignalList(const GS_TCB *ptcb)
	{
		int antecedent = ((ptcb->TCB_NextTCBASL != (GS_SCB *) 0));
		if (!((!antecedent) || ((GS_TCB *) ptcb->TCB_NextTCBASL->SCB_AssocXCB == ptcb))) {gk_fprintf("Invalid TCB_NextTCBASL TCBState=%d", ptcb->TCBState); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Associated pending signal list */
	static G_UINT32 gkm_TCB_CheckAssocPendingSignalList(const GS_TCB *ptcb)
	{
		int antecedent = ((ptcb->TCB_NextTCBPSL != (GS_SCB *) 0));
		if (!((!antecedent) || ((GS_TCB *) ptcb->TCB_NextTCBPSL->SCB_AssocXCB == ptcb))) {gk_fprintf("Invalid TCB_NextTCBPSL TCBState=%d", ptcb->TCBState); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Stack bottom/top vs stack pointer */
	static G_UINT32 gkm_TCB_CheckStackBounds(const GS_TCB *ptcb)
	{
		int consequent = ((ptcb->TCB_StackBottom >= ptcb->TCB_StackPointer) && (ptcb->TCB_StackTop <= ptcb->TCB_StackPointer));
		if (!(consequent)) {gk_fprintf("Invalid STACK botton or top"); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Associated processor */
	static G_UINT32 gkm_TCB_CheckAssocProcessor(const GS_TCB *ptcb)
	{
		int antecedent = ((ptcb->TCB_AssocPCB != (unsigned int) 0));
		if (!((!antecedent) || (g_kcb.G_PCBTbl[(ptcb->TCB_AssocPCB) - 1].PCB_EXECTCB == (struct gs_tcb *) ptcb))) {gk_fprintf("Invalid TCB_AssocPCB TCB_AssocPCB=%d, g_kcb=%p, ptcb=%p", (int) ptcb->TCB_AssocPCB, (void *) g_kcb.G_PCBTbl[(ptcb->TCB_AssocPCB) - 1].PCB_EXECTCB, (void *) ptcb); return (G_FALSE);}
		return (G_TRUE);
	}

	/* TCB_RDY_LCB_Index 1 */
	static G_UINT32 gkm_TCB_CheckRdyLCBIndex1(const GS_TCB *ptcb)
	{
		int antecedent = (((ptcb->TCBState == G_TCBState_RUNNING) || (ptcb->TCBState == G_TCBState_READY)) && (ptcb->TCB_NextTCB != (GS_TCB *) 0));
		if (!((!antecedent) || ((ptcb->TCB_RDY_LCB_Index == ptcb->TCB_NextTCB->TCB_RDY_LCB_Index) && (ptcb->TCBState == ptcb->TCB_NextTCB->TCBState)))) {gk_fprintf("Invalid TCB_RDY_LCB_Index 1"); return (G_FALSE);}
		return (G_TRUE);
	}

	/* TCB_RDY_LCB_Index 2 */
	static G_UINT32 gkm_TCB_CheckRdyLCBIndex2(const GS_TCB *ptcb)
	{
		int antecedent = ((ptcb->TCBState == G_TCBState_RUNNING) && (ptcb->TCB_PrevTCB == (GS_TCB *) 0));
		if (!((!antecedent) || (ptcb->TCB_RDY_LCB_Index->LCB_NextTCBRUNL == ptcb))) {gk_fprintf("Invalid TCB_RDY_LCB_Index 2"); return (G_FALSE);}
		return (G_TRUE);
	}

	/* TCB_RDY_LCB_Index 3 */
	static G_UINT32 gkm_TCB_CheckRdyLCBIndex3(const GS_TCB *ptcb)
	{
		int antecedent = ((ptcb->TCBState == G_TCBState_READY) && (ptcb->TCB_PrevTCB == (GS_TCB *) 0) && (ptcb->TCBType != G_TCBType_IDLE));
		if (!((!antecedent) || (ptcb->TCB_RDY_LCB_Index->LCB_NextTCBRDYL == ptcb))) {gk_fprintf("Invalid TCB_RDY_LCB_Index 3"); return (G_FALSE);}
		return (G_TRUE);
	}

	/* Debug previous structure */
	static G_UINT32 gkm_TCB_CheckPrevStructure(const GS_TCB *ptcb, const GS_TCB *ptcb_prv)
	{
		if (ptcb->TCB_PREV_TCBs != ptcb_prv) {gk_fprintf("Invalid TCB_PREV_TCBs"); return (G_FALSE);}
		return (G_TRUE);
	}

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Validate the consistency of every TCB in the system.
	 *
	 * Traverses g_kcb.KCB_ROOT_TCBs and for each TCB asserts: valid BLOCK_HASH,
	 * valid TCBState and TCBType, valid LCB index, valid associated-event and
	 * signal-list back-pointers, correct doubly-linked prev/next pointers, correct
	 * stack bounds, priority ordering in ready and run lists, correct processor
	 * association, and list-root consistency.  Also verifies that the traversal
	 * count matches KCB_NUMBER_OF_TCBs.
	 *
	 * \param [in] partial  Accepted for interface consistency with the other
	 *                      gkm_Check_* validators; not currently read by this
	 *                      one (unlike gkm_Check_ECBs) -- every check below
	 *                      always runs. Flagged 2026-08-16, not changed here:
	 *                      a behavioural question, not a structural one.
	 * \return #G_TRUE if all TCBs are consistent; #G_FALSE on first violation.
	 *         gkm_Check_TCBs is only ever reached from inside
	 *         PRINT_ASSERT(gkm_Check_GeMRTOS(...) == G_TRUE, ...), which halts
	 *         with #G_DEBUG_WHILEFOREVER on any G_FALSE reaching it.
	 */
	G_UINT32 gkm_Check_TCBs(G_UINT32 partial)
	{
		GS_TCB * ptcb;
		const GS_TCB * ptcb_prv = (GS_TCB *) 0;
		(void) partial;

		int NBR_TCBs = 0;
		for (ptcb = g_kcb.KCB_ROOT_TCBs; ptcb != (GS_TCB *) 0; ptcb = (GS_TCB *) ptcb->TCB_NEXT_TCBs) {
			const GS_TCB * list_root_value;

			NBR_TCBs++;

			if (gkm_TCB_CheckBlockValid(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckStateValid(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckTypeValid(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckRdyLCBIndexValid(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckNextTCBASLValid(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckNextTCBPSLValid(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckAssocPCBRange(ptcb) != G_TRUE) return (G_FALSE);

			list_root_value = gkm_TCB_list_ROOT(ptcb);

			if (gkm_TCB_CheckNextTCBAELValid(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckPrevTCBListRoot(ptcb, list_root_value) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckPrevTCBBacklink(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckNextTCBBacklink(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckReadyListPriorityOrder(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckRunListPriorityOrder(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckAssocEventList(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckAssocSignalList(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckAssocPendingSignalList(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckStackBounds(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckAssocProcessor(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckRdyLCBIndex1(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckRdyLCBIndex2(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckRdyLCBIndex3(ptcb) != G_TRUE) return (G_FALSE);
			if (gkm_TCB_CheckPrevStructure(ptcb, ptcb_prv) != G_TRUE) return (G_FALSE);

			ptcb_prv = ptcb;
		}
		return gkm_verify_count(NBR_TCBs, g_kcb.KCB_NUMBER_OF_TCBs, "TCB");
	}

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Validate the consistency of every PCBAssocLCB entry in the system.
	 *
	 * Traverses g_kcb.KCB_ROOT_RDYs and for each PCBAssocLCB asserts: valid
	 * BLOCK_HASH, non-decreasing PCB_RDY_priority ordering, correct prev/next
	 * back-linkage, and a valid associated LCB pointer.  Also verifies that the
	 * traversal count matches KCB_NUMBER_OF_PCBAssocLCBs.
	 *
	 * \param [in] partial  Reserved; currently unused.
	 * \return #G_TRUE if all PCBAssocLCB entries are consistent; #G_FALSE on first
	 *         violation (most paths also halt with #G_DEBUG_WHILEFOREVER).
	 */
	G_UINT32 gkm_Check_PCBAssocLCBs(G_UINT32 partial)
	{
		struct gs_pcb_rdy_lcbl * ppcb_assoc_lcb;
		const struct gs_pcb_rdy_lcbl * ppcb_assoc_lcb_prv = (struct gs_pcb_rdy_lcbl *) 0;

		int NBR_PCBAssocLCBs = 0;
		for (ppcb_assoc_lcb = g_kcb.KCB_ROOT_RDYs; ppcb_assoc_lcb != (struct gs_pcb_rdy_lcbl *) 0; ppcb_assoc_lcb = ppcb_assoc_lcb->gs_pcb_lcbl_nexts) {
			int antecedent;
			int consequent;

			NBR_PCBAssocLCBs++;
			if (gkm_PCBAssocLCB_IsValid(ppcb_assoc_lcb) != G_TRUE) {gk_fprintf("invalid gs_pcb_rdy_lcbl"); while(1); return (G_FALSE); }

			// Next pcbassoclcb
			antecedent = ((ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next != (struct gs_pcb_rdy_lcbl *) 0));
			if (!((!antecedent) || ((ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next->gs_pcb_rdy_lcbl_prev == ppcb_assoc_lcb) && (ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next->PCB_RDY_priority >= ppcb_assoc_lcb->PCB_RDY_priority)))) {gk_fprintf("Invalid next pcbassoclcb"); while(1); return (G_FALSE);}

			// LCB previous of next (antecedent unchanged since the identical check above)
			if (!((!antecedent) || ((ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next->gs_pcb_rdy_lcbl_prev == ppcb_assoc_lcb) && (ppcb_assoc_lcb->gs_pcb_rdy_lcbl_next->PCB_RDY_priority >= ppcb_assoc_lcb->PCB_RDY_priority)))) {gk_fprintf("Invalid previous pcbassoclcb of next"); while(1); return (G_FALSE);}

			// LCB associated valid
			antecedent = (1);
			consequent = (gkm_LCB_IsValid(ppcb_assoc_lcb->PCB_RDY_LCBL) == G_TRUE);
			if (!((!antecedent) || consequent)) {gk_fprintf("Invalid LCB associated valid"); while(1); return (G_FALSE);}


			// #############################################
			// Debug previous structure
			if (ppcb_assoc_lcb->gs_pcb_lcbl_prevs != ppcb_assoc_lcb_prv) {gk_fprintf("Invalid gs_pcb_lcbl_prevs"); while(1); return (G_FALSE);}
			ppcb_assoc_lcb_prv = ppcb_assoc_lcb;        
		}
		return gkm_verify_count(NBR_PCBAssocLCBs, g_kcb.KCB_NUMBER_OF_PCBAssocLCBs, "PCBAssocLCB");
	}

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Validate the consistency of every LCB (scheduling list) in the system.
	 *
	 * Traverses g_kcb.KCB_ROOT_LCBs and for each LCB asserts: valid BLOCK_HASH,
	 * correct running-list/LCBCurrentRunning agreement, correct doubly-linked
	 * LCB_NextLCBL/LCB_PrevLCBL pointers, and correct root-LCB back-pointer.
	 * Also verifies that the traversal count matches KCB_NUMBER_OF_LCBs.
	 *
	 * \param [in] partial  Reserved; currently unused.
	 * \return #G_TRUE if all LCBs are consistent; #G_FALSE on first violation
	 *         (most paths also halt with #G_DEBUG_WHILEFOREVER).
	 */
	G_UINT32 gkm_Check_LCBs(G_UINT32 partial)
	{
		GS_LCB *plcb;
		const GS_LCB *plcb_prv = (GS_LCB *) 0;

		int NBR_LCBs = 0;
		for (plcb = g_kcb.KCB_ROOT_LCBs; plcb != (GS_LCB *) 0; plcb = plcb->LCB_NEXT_LCBs) {
			int antecedent;
			int consequent;

			NBR_LCBs++;
			if (gkm_LCB_IsValid(plcb) != G_TRUE) {gk_fprintf("invalid LCB"); while(1); return (G_FALSE); }

			// LCB exclusion
			// 23-10-0224 antecedent = (1);
			// 23-10-0224 consequent = (plcb->LCBExclusion >= plcb->LCBCurrentRunning);
			// 23-10-0224 if (!((!antecedent) || consequent)) {gk_printf("Invalid LCB exclusion"); while(1); return (G_FALSE);}

			// LCB running list
			antecedent = (plcb->LCB_NextTCBRUNL == (GS_TCB *) 0);
			consequent = ((plcb->LCBCurrentRunning == 0));
			if (!((!antecedent) || consequent)) {gk_fprintf("Invalid LCB running list Excl=%d, running=%d", (int) plcb->LCBExclusion, (int) plcb->LCBCurrentRunning); while(1); return (G_FALSE);}

			// LCB next LCB
			antecedent = (plcb->LCB_NextLCBL != (GS_LCB *) 0);
			if (!((!antecedent) || ((plcb->LCB_NextLCBL->LCB_PrevLCBL == plcb)))) {gk_fprintf("Invalid LCB next LCB"); while(1); return (G_FALSE);}

			// LCB root LCB
			antecedent = (plcb->LCB_PrevLCBL == (GS_LCB *) 0);
			if (!((!antecedent) || ((g_kcb.KCB_NextLCBL == plcb)))) {gk_fprintf("Invalid LCB root LCB"); while(1); return (G_FALSE);}
			
			
			// #############################################
			// Debug previous structure
			if (plcb->LCB_PREV_LCBs != plcb_prv) {gk_fprintf("Invalid LCB_PREV_LCBs"); while(1); return (G_FALSE);}
			plcb_prv = plcb;        
		}
		return gkm_verify_count(NBR_LCBs, g_kcb.KCB_NUMBER_OF_LCBs, "LCB");
	}

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Validate the consistency of every RCB (resource control block) in the system.
	 *
	 * Traverses g_kcb.KCB_ROOT_RCBs and for each RCB asserts: valid BLOCK_HASH,
	 * valid RCBType, and correct debug prev-pointer linkage.  Also verifies that
	 * the traversal count matches KCB_NUMBER_OF_RCBs.
	 *
	 * \param [in] partial  Reserved; currently unused.
	 * \return #G_TRUE if all RCBs are consistent; #G_FALSE on first violation.
	 */
	G_UINT32 gkm_Check_RCBs(G_UINT32 partial)
	{
		GS_RCB *prcb;
		const GS_RCB *prcb_prv = (GS_RCB *) 0;

		int NBR_RCBs = 0;
		for (prcb = (GS_RCB *) g_kcb.KCB_ROOT_RCBs; prcb != (GS_RCB *) 0; prcb = (GS_RCB *) prcb->RCB_NEXT_RCBs) {
			NBR_RCBs++;
			if (gkm_RCB_IsValid(prcb) != G_TRUE) {gk_fprintf("invalid RCB"); return (G_FALSE); }
			if (gkm_RCBType_Valid(prcb->RCBType) != G_TRUE) {gk_fprintf("invalid RCB"); return (G_FALSE); }

			// #############################################
			// Debug previous structure
			if (prcb->RCB_PREV_RCBs != prcb_prv) {gk_fprintf("Invalid RCB_PREV_RCBs"); while(1); return (G_FALSE);}
			prcb_prv = prcb;        
		}
		return gkm_verify_count(NBR_RCBs, g_kcb.KCB_NUMBER_OF_RCBs, "RCB");
	}


	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Validate the consistency of every RRDS (resource-request descriptor) in the system.
	 *
	 * Traverses g_kcb.KCB_ROOT_RRDSs and for each RRDS asserts: valid BLOCK_HASH,
	 * and correct RRDS_AsocECB back-pointer (if non-NULL, the referenced ECB's
	 * ECB_RRDS field must point back to this RRDS).  Also verifies that the
	 * traversal count matches KCB_NUMBER_OF_RRDSs.
	 *
	 * \param [in] partial  Reserved; currently unused.
	 * \return #G_TRUE if all RRDSs are consistent; #G_FALSE on first violation
	 *         (most paths also halt with #G_DEBUG_WHILEFOREVER).
	 */
	G_UINT32 gkm_Check_RRDSs(G_UINT32 partial)
	{
		GS_RRDS *prrds;
		const GS_RRDS *prrds_prv = (GS_RRDS *) 0;
		
		int NBR_RRDSs = 0;
		for (prrds = (GS_RRDS *) g_kcb.KCB_ROOT_RRDSs; prrds != (GS_RRDS *) 0; prrds = (GS_RRDS *) prrds->RRDS_NEXT_RRDSs) {
			NBR_RRDSs++;
			if (gkm_RRDS_IsValid(prrds) != G_TRUE) {gk_fprintf("invalid RRDS"); while(1); return (G_FALSE);}

			// RRDS_AsocECB
			if (((prrds != (GS_RRDS *) 0) && (get_pointed_field(prrds, RRDS_AsocECB) != (GS_ECB *) 0)) && (!(get_pointed_field(get_pointed_field(prrds, RRDS_AsocECB), ECB_RRDS) == prrds))) {gk_fprintf("Error in RRDS_AsocECB"); while(1); return (G_FALSE);}
			
			// #############################################
			// Debug previous structure
			if (prrds->RRDS_PREV_RRDSs != prrds_prv) {gk_fprintf("Invalid RRDS_PREV_RRDSs"); while(1); return (G_FALSE);}
			prrds_prv = prrds;        
		}
		return gkm_verify_count(NBR_RRDSs, g_kcb.KCB_NUMBER_OF_RRDSs, "RRDS");
	}

	/* gkm_Check_PCBs's per-PCB invariants, extracted 2026-08-16 (split out to
	 * reduce complexity -- see the identical note above
	 * gkm_Check_TCBs's helpers for the full rationale, including why the
	 * inline while(1) calls some checks had are not reproduced. */

	static G_UINT32 gkm_PCB_CheckIDMatchesIndex(const struct gs_pcb *ppcb, int i)
	{
		if (ppcb->PCBID != i + 1) {gk_fprintf("Invalid PCB PCBID=%d, i=%d", (int) ppcb->PCBID, i); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_PCB_CheckStateValid(const struct gs_pcb *ppcb)
	{
		if (gkm_PCBState_Valid(ppcb->PCBState) != G_TRUE) {gk_fprintf("Invalid PCB State=%d", ppcb->PCBState); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_PCB_CheckTypeValid(const struct gs_pcb *ppcb)
	{
		if (gkm_PCBType_Valid(ppcb->PCBType) != G_TRUE) {gk_fprintf("Invalid PCB Type=%d", ppcb->PCBType); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_PCB_CheckExecTCBValid(const struct gs_pcb *ppcb, int i)
	{
		if (gkm_TCB_IsValid(ppcb->PCB_EXECTCB) != G_TRUE) {gk_fprintf("Error in exec proc=%u, i=%u, task=%p, PCBState= %u\n", gm_ProcessorId(), (unsigned int) i, ppcb->PCB_EXECTCB, (unsigned int) ppcb->PCBState); return (G_FALSE);}
		return (G_TRUE);
	}

	static G_UINT32 gkm_PCB_CheckIdleTCBValid(const struct gs_pcb *ppcb)
	{
		if (gkm_TCB_IsValid(ppcb->PCB_IDLETCB) != G_TRUE) {gk_fprintf("Error in idle"); return (G_FALSE);}
		return (G_TRUE);
	}

	/* PCB Free */
	static G_UINT32 gkm_PCB_CheckFreeListLinkage(const struct gs_pcb *ppcb)
	{
		int antecedent = ((ppcb->PCBState == GS_PCBState_FREE) && (ppcb->PCB_PrevPCB == (struct gs_pcb *) 0));
		int consequent = (ppcb->PCB_AssocLCB != (struct gs_pcb_rdy_lcbl *) 0 && ppcb->PCB_AssocLCB->PCB_RDY_LCBL != (struct gs_lcb *) 0 && ppcb->PCB_AssocLCB->PCB_RDY_LCBL->LCB_NextLCBFPL == (struct gs_pcb *) ppcb);
		if (!((!antecedent) || consequent)) {gk_fprintf("Error in FREE PCB"); return (G_FALSE);}
		return (G_TRUE);
	}

	/* PCB_AssocLCB valid */
	static G_UINT32 gkm_PCB_CheckAssocLCBValid(const struct gs_pcb *ppcb)
	{
		int antecedent = (ppcb->PCB_AssocLCB != (struct gs_pcb_rdy_lcbl *) 0);
		int consequent = (gkm_PCBAssocLCB_IsValid(ppcb->PCB_AssocLCB) == G_TRUE);
		if (!((!antecedent) || consequent)) {gk_fprintf("Error PCB_AssocLCB"); return (G_FALSE);}
		return (G_TRUE);
	}

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Validates the consistency of all processor control blocks.
	 *
	 * For every active PCB verifies: PCBID in range, valid PCBState and PCBType,
	 * valid executing and idle TCB pointers, correct free-list linkage, and
	 * valid PCB_AssocLCB pointer.
	 *
	 * \param [in] partial  Reserved for future partial-check support; currently unused.
	 * \return #G_TRUE if all PCBs are consistent; #G_FALSE on first violation.
	 *         gkm_Check_PCBs is only ever reached from inside
	 *         PRINT_ASSERT(gkm_Check_GeMRTOS(...) == G_TRUE, ...), which halts
	 *         with #G_DEBUG_WHILEFOREVER on any G_FALSE reaching it.
	 */
	G_UINT32 gkm_Check_PCBs(G_UINT32 partial)
	{
		int i;

		// Check processors
		// Assigned task should be different for different processors
		for (i = 0;  i < G_NUMBER_OF_PCB;  i++) {
			if (g_kcb.G_PCBTbl[i].PCBState != GS_PCBState_NOTRUNNING)
			{
				struct gs_pcb *ppcb = &g_kcb.G_PCBTbl[i];

				if (gkm_PCB_CheckIDMatchesIndex(ppcb, i) != G_TRUE) return (G_FALSE);
				if (gkm_PCB_CheckStateValid(ppcb) != G_TRUE) return (G_FALSE);
				if (gkm_PCB_CheckTypeValid(ppcb) != G_TRUE) return (G_FALSE);
				if (gkm_PCB_CheckExecTCBValid(ppcb, i) != G_TRUE) return (G_FALSE);
				if (gkm_PCB_CheckIdleTCBValid(ppcb) != G_TRUE) return (G_FALSE);
				if (gkm_PCB_CheckFreeListLinkage(ppcb) != G_TRUE) return (G_FALSE);
				if (gkm_PCB_CheckAssocLCBValid(ppcb) != G_TRUE) return (G_FALSE);
			}


		}
		return (G_TRUE);
	}

	/**
	 * \ingroup GeMRTOSMonitor
	 * \brief   Runs a full consistency check every \p cycles invocations.
	 *
	 * Maintains an internal invocation counter.  When the counter reaches
	 * \p cycles it resets to zero and executes all sub-checks in sequence:
	 * gkm_Check_PCBs, gkm_Check_ECBs, gkm_Check_TCBs, gkm_Check_PCBAssocLCBs,
	 * gkm_Check_LCBs, gkm_Check_RCBs, gkm_Check_RRDSs.
	 *
	 * Intended to be called from the idle task to spread the monitoring overhead.
	 *
	 * \param [in] cycles  Number of invocations between full checks.  Must be > 0.
	 * \return #G_TRUE if all checks pass or the cycle count has not been reached;
	 *         #G_FALSE if any sub-check fails.
	 */
	G_UINT32 gkm_Check_GeMRTOS(int cycles)
	{
		static int count = 0;


		count++;
		
		if ((count >= cycles))
		{
			count = 0;
		    if (gkm_Check_PCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		    if (gkm_Check_ECBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		    if (gkm_Check_TCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		    if (gkm_Check_PCBAssocLCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		    if (gkm_Check_LCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		    if (gkm_Check_RCBs(G_TRUE) == G_FALSE) {return (G_FALSE);}
		    if (gkm_Check_RRDSs(G_TRUE) == G_FALSE) {return (G_FALSE);}
			
		
		}
		return (G_TRUE);
	}

/**
 * \ingroup GeMRTOSMonitor
 * \brief   Prints the recorded call stack to stderr.
 *
 * Walks the SAMPLINGS array starting from current_function, following the
 * previous_function chain recorded by GK_SAMPLE_FUNCTION_BEGIN/END macros.
 * Prints each function name until the chain ends or 50 frames have been printed
 * (loop guard against corrupted data).
 *
 * Only compiled when #G_DEBUG_WHILEFOREVER_ENABLE == 1.
 *
 * \return #G_TRUE unconditionally.
 */
G_UINT32 gkm_PrintFunctionStack( void )
{
	G_UINT32 number = current_function;
    G_UINT32 iterations = 0;
	gk_fprintf("\n **************************");
	while ( number != 0 && iterations < G_PRINT_STACK_MAX_FRAMES) {
		gk_fprintf("\n function = %s, ", SAMPLINGS[number - G_SAMPLING_BASE_ID].function_name);
		number = SAMPLINGS[number - G_SAMPLING_BASE_ID].previous_function;
        iterations++;
	}
	gk_fprintf("\n ************************** \n");
	return (G_TRUE);
}


#endif /* G_DEBUG_WHILEFOREVER_ENABLE == 1 */

OPTIMIZE_RESTORE

