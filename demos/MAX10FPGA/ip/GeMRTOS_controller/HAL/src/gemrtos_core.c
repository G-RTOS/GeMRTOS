/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS core management functions
 *  \details This file contains the functions for core management.
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

/* MISRA C:2012 Rule 21.3: malloc() is used only in pool dynamic-fallback for development
 * (G_DEBUG_WHILEFOREVER_ENABLE == 1).  Not included in the production build. */
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
#include <stdlib.h>
#endif

OPTIMIZE_CODE

#include <inttypes.h>

/* MISRA C:2012 Rule 21.3: Static object pools for the embedded target.
 * Pool sizes are configured by the G_NUMBER_OF_* constants in
 * gemrtos_config.h.  Increase those constants if the kernel reports pool
 * exhaustion at run-time. */
static GS_TCB         g_tcb_pool[G_NUMBER_OF_TCB]          __attribute__((aligned(4)));
static G_SSTK         g_stack_pool[G_NUMBER_OF_TCB][G_TASK_STACK_WORDS] __attribute__((aligned(4)));
static GS_ECB         g_ecb_pool[G_NUMBER_OF_ECB]          __attribute__((aligned(4)));
static GS_RCB         g_rcb_pool[G_NUMBER_OF_RCB]          __attribute__((aligned(4)));
static GS_SCB         g_scb_pool[G_NUMBER_OF_SCB]          __attribute__((aligned(4)));
static GS_RRDS        g_rrds_pool[G_NUMBER_OF_RRDS]        __attribute__((aligned(4)));
static GS_LCB         g_lcb_pool[G_NUMBER_OF_LCB]          __attribute__((aligned(4)));
static GS_PCBAssocLCB g_pcbalcb_pool[G_NUMBER_OF_PCBAssocLCB] __attribute__((aligned(4)));

/**
 * \brief Populates all kernel free lists from statically allocated pools.
 *
 * Must be called from gk_INIT_KERNEL() after the KCB free-list pointers are
 * zeroed and before the first gk_Get_XXX() call.  This replaces the
 * malloc-based fallback path, making the embedded build fully compliant with
 * MISRA C:2012 Rule 21.3.
 *
 * Pool sizes are set by the G_NUMBER_OF_* constants in gemrtos_config.h.
 */
void gk_Init_Pools(void)
{
    unsigned int i;

    /* ---- LCB pool ---- */
    for (i = 0U; i < G_NUMBER_OF_LCB; i++) {
        g_lcb_pool[i].BLOCK_HASH   = (unsigned int)&g_lcb_pool[i] + G_LCB_HASH;
        g_lcb_pool[i].LCB_NextLCBL = (i + 1U < G_NUMBER_OF_LCB)
                                      ? &g_lcb_pool[i + 1U]
                                      : (GS_LCB *)0;
    }
    g_kcb.KCB_FREE_LCBs = &g_lcb_pool[0];

    /* ---- PCBAssocLCB pool ---- */
    for (i = 0U; i < G_NUMBER_OF_PCBAssocLCB; i++) {
        g_pcbalcb_pool[i].BLOCK_HASH            = (unsigned int)&g_pcbalcb_pool[i] + G_PCBALCB_HASH;
        g_pcbalcb_pool[i].gs_pcb_rdy_lcbl_next = (i + 1U < G_NUMBER_OF_PCBAssocLCB)
                                                  ? &g_pcbalcb_pool[i + 1U]
                                                  : (GS_PCBAssocLCB *)0;
    }
    g_kcb.KCB_FREE_RDYs = &g_pcbalcb_pool[0];

    /* ---- TCB pool (each TCB paired with its static stack) ---- */
    for (i = 0U; i < G_NUMBER_OF_TCB; i++) {
        g_tcb_pool[i].BLOCK_HASH      = (unsigned int)&g_tcb_pool[i] + G_TCB_HASH;
        g_tcb_pool[i].TCB_StackTop    = &g_stack_pool[i][0];
        g_tcb_pool[i].TCB_StackBottom = &g_stack_pool[i][G_TASK_STACK_WORDS - 1U];
        g_tcb_pool[i].TCB_NextTCB     = (i + 1U < G_NUMBER_OF_TCB)
                                         ? &g_tcb_pool[i + 1U]
                                         : (GS_TCB *)0;
    }
    g_kcb.KCB_FREE_TCBs = &g_tcb_pool[0];

    /* ---- ECB pool ---- */
    /* Initialise the free-list invariant fields so a pool-fresh ECB
     * satisfies the same contract as a recycled one (gk_ECBFL_Link leaves
     * ECBState == GS_ECBState_FREE and cleared associations).  Previously
     * these relied on zeroed BSS and a single boot, leaving stale state on
     * any re-initialisation. */
    for (i = 0U; i < G_NUMBER_OF_ECB; i++) {
        g_ecb_pool[i].BLOCK_HASH     = (unsigned int)&g_ecb_pool[i] + G_ECB_HASH;
        g_ecb_pool[i].ECB_NextECB    = (i + 1U < G_NUMBER_OF_ECB)
                                        ? &g_ecb_pool[i + 1U]
                                        : (GS_ECB *)0;
        g_ecb_pool[i].ECB_PrevECB    = (GS_ECB *)0;
        g_ecb_pool[i].ECB_AssocTCB   = (struct gs_tcb *)0;
        g_ecb_pool[i].ECB_AssocRCB   = (struct g_rcb *)0;
        g_ecb_pool[i].ECB_NextECBAEL = (GS_ECB *)0;
        g_ecb_pool[i].ECBState       = GS_ECBState_FREE;
    }
    g_kcb.KCB_FREE_ECBs = &g_ecb_pool[0];

    /* ---- RCB pool ---- */
    for (i = 0U; i < G_NUMBER_OF_RCB; i++) {
        g_rcb_pool[i].BLOCK_HASH  = (unsigned int)&g_rcb_pool[i] + G_RCB_HASH;
        g_rcb_pool[i].RCB_NextRCB = (i + 1U < G_NUMBER_OF_RCB)
                                     ? &g_rcb_pool[i + 1U]
                                     : (GS_RCB *)0;
    }
    g_kcb.KCB_FREE_RCBs = (struct g_rcb *)&g_rcb_pool[0];

    /* ---- SCB pool ---- */
    for (i = 0U; i < G_NUMBER_OF_SCB; i++) {
        g_scb_pool[i].BLOCK_HASH  = (unsigned int)&g_scb_pool[i] + G_SCB_HASH;
        g_scb_pool[i].SCB_NextSCB = (i + 1U < G_NUMBER_OF_SCB)
                                     ? &g_scb_pool[i + 1U]
                                     : (GS_SCB *)0;
    }
    g_kcb.KCB_FREE_SCBs = &g_scb_pool[0];

    /* ---- RRDS pool ---- */
    for (i = 0U; i < G_NUMBER_OF_RRDS; i++) {
        g_rrds_pool[i].BLOCK_HASH    = (unsigned int)&g_rrds_pool[i] + G_RRDS_HASH;
        g_rrds_pool[i].RRDS_NextRRDS = (i + 1U < G_NUMBER_OF_RRDS)
                                        ? &g_rrds_pool[i + 1U]
                                        : (GS_RRDS *)0;
    }
    g_kcb.KCB_FREE_RRDSs = &g_rrds_pool[0];
}


/**
 *  \brief  Returns a pointer to a free TCB from the static pool (or allocates
 *          dynamically in development mode) and initialises it to a known state.
 *  \param [in] stack_size  Number of G_SSTK words for the task stack (must be > 0).
 *  \return Pointer to an initialised TCB, or NULL if the pool is exhausted.
 *  \relates Task
 */
GS_TCB *gk_TCB_GetFree(unsigned int stack_size)
{
    GK_SAMPLE_FUNCTION_BEGIN(10040);

    PRINT_ASSERT(stack_size > 0, "ERROR stack_size invalid\n");

    GS_TCB *ptcb = g_kcb.KCB_FREE_TCBs;

    if (ptcb != NULL)
    {
        g_kcb.KCB_FREE_TCBs = get_pointed_field(ptcb, TCB_NextTCB);
    }
    else
    {
        // Rationale: development-mode fallback to malloc when the static pool exhausts.
        // This allows testing with dynamically-constrained pools without hard failure,
        // but must be sized correctly in production (via G_NUMBER_OF_TCB). The fallback
        // emits a warning to make exhaustion visible during development.
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        /* DEVELOPMENT MODE: TCB static pool exhausted -- allocating dynamically.
         * ACTION REQUIRED for production: increase G_NUMBER_OF_TCB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_TCB_GetFree: TCB pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_TCB in gemrtos_config.h for production.\n");

        GS_TCB *ptcb_dynamic = (GS_TCB *)malloc(sizeof(GS_TCB));
        if (ptcb_dynamic == NULL) {
            PRINT_ASSERT(G_FALSE, "ERROR gk_TCB_GetFree: malloc failed for dynamic TCB\n");
            return NULL;
        }

            PRINT_ASSERT(((uintptr_t)ptcb_dynamic % 4) == 0, "ERROR TCB alignment\n");

            ptcb_dynamic->BLOCK_HASH = (unsigned int)ptcb_dynamic + G_TCB_HASH;

            /* Debug tracking list */
            GS_TCB *head = g_kcb.KCB_ROOT_TCBs;
            set_pointed_field(ptcb_dynamic, TCB_NEXT_TCBs, head);
            if (head != NULL) set_pointed_field(head, TCB_PREV_TCBs, ptcb_dynamic);
            set_pointed_field(ptcb_dynamic, TCB_PREV_TCBs, NULL);
            g_kcb.KCB_ROOT_TCBs = ptcb_dynamic;
            g_kcb.KCB_NUMBER_OF_TCBs++;

            /* Allocate task stack */
            G_SSTK *top_stack = (G_SSTK *)malloc(sizeof(G_SSTK) * stack_size);
            if (top_stack == NULL) { gk_TCBFL_Link(ptcb_dynamic); return NULL; }

            PRINT_ASSERT(((uintptr_t)top_stack % 4) == 0, "ERROR stack alignment\n");

            G_SSTK *bottom_stack = top_stack + stack_size - 1;
            set_pointed_field(ptcb_dynamic, TCB_StackTop, top_stack);
            set_pointed_field(ptcb_dynamic, TCB_StackBottom, bottom_stack);

#ifndef __niosX_arch__
            /* Host simulation context */
            getcontext(&ptcb_dynamic->uctx);
            ptcb_dynamic->uctx.uc_stack.ss_size = (16U * 1024U); /* host sim stack; keep in sync with G_HOST_CONTEXT_STACK_BYTES in grtos_kernel.c */
            ptcb_dynamic->uctx_stack = malloc(ptcb_dynamic->uctx.uc_stack.ss_size);
            ptcb_dynamic->uctx.uc_stack.ss_sp    = ptcb_dynamic->uctx_stack;
            ptcb_dynamic->uctx.uc_stack.ss_flags = 0;
            makecontext(&ptcb_dynamic->uctx, (void (*)()) gu_pthread, 1, ptcb_dynamic);
#endif
            ptcb = ptcb_dynamic;

#else
    /* PRODUCTION MODE: TCB pool exhausted -- halt. Increase G_NUMBER_OF_TCB in gemrtos_config.h. */
    
#endif /* G_DEBUG_WHILEFOREVER_ENABLE */

        if (ptcb == NULL)
        {
            GK_SAMPLE_FUNCTION_END(10040);
            return NULL;
        }
    }

    set_pointed_field(ptcb, TCB_NextTCBAEL, NULL);
    set_pointed_field(ptcb, TCB_NextTCBASL, NULL);
    set_pointed_field(ptcb, TCB_NextTCBPSL, NULL);
    set_pointed_field(ptcb, TCBType, G_TCBType_OneShot);
    set_pointed_field(ptcb, TCBReadyPriority, G_LOWEST_PRIORITY - 1);
    set_pointed_field(ptcb, TCBRunPriority, G_LOWEST_PRIORITY - 1);
    set_pointed_field(ptcb, TCBCurrentPriority, G_LOWEST_PRIORITY - 1);
    set_pointed_field(ptcb, TCBPeriod, G_LATEST_TIME - 1);
    set_pointed_field(ptcb, TCB_StackPointer, ptcb->TCB_StackBottom);
    set_pointed_field(ptcb, TCB_TaskCode, NULL);
    set_pointed_field(ptcb, TCB_TaskArg,  NULL);
    set_pointed_field(ptcb, TCB_NextTCB, NULL);
    set_pointed_field(ptcb, TCB_PrevTCB, NULL);
    set_pointed_field(ptcb, TCB_AssocPCB, 0);
    set_pointed_field(ptcb, TCB_INTNumber, (GS_RCB *)-1);
    set_pointed_field(ptcb, TCB_RDY_LCB_Index, (GS_LCB *)G_TASK_LCB_DEFAULT);
    set_pointed_field(ptcb, TCBState, G_TCBState_UNLINKED);
    set_pointed_field(ptcb, TCB_MTX_NESTED, 0);
    set_pointed_field(ptcb, TCB_description[0], '\0');    

    GK_SAMPLE_FUNCTION_END(10040);
    return ptcb;
}

/**
 *  \brief  Returns a pointer to a free ECB, or NULL if no ECB is available.
 *  \return Pointer to a new ECB, or NULL if the pool is exhausted.
 */
GS_ECB   *gk_ECB_GetFree(GS_TCB *ptcb)
{
    GS_ECB  *pecb = g_kcb.KCB_FREE_ECBs;
    
    GK_SAMPLE_FUNCTION_BEGIN(10026)
    
    // Check if free structure is available
    if (pecb  != (struct gs_ecb  *) 0) {
        g_kcb.KCB_FREE_ECBs = get_pointed_field(pecb, ECB_NextECB);
    }
    else
    {
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        /* DEVELOPMENT MODE: ECB static pool exhausted -- allocating dynamically.
         * ACTION REQUIRED for production: increase G_NUMBER_OF_ECB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_ECB_GetFree: ECB pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_ECB in gemrtos_config.h for production.\n");
		if ((pecb = (GS_ECB  *) malloc(sizeof(GS_ECB))) != 0) {
			PRINT_ASSERT(((unsigned int) pecb % 4 == 0), "ERROR memory is not alligned");
			PRINT_ASSERT(((unsigned int) pecb != 0), "ERROR no memory available");

			pecb->BLOCK_HASH     = (unsigned int) pecb + G_ECB_HASH;

            // ECBs linked list for debugging
			GS_ECB  *pevent_old_head;
			set_pointed_field(pecb, ECB_NEXT_ECBs, g_kcb.KCB_ROOT_ECBs);
			pevent_old_head = g_kcb.KCB_ROOT_ECBs;
			if (pevent_old_head != (struct gs_ecb *) 0) set_pointed_field(pevent_old_head, ECB_PREV_ECBs, pecb);
			set_pointed_field(pecb, ECB_PREV_ECBs, (struct gs_ecb *) 0);
			g_kcb.KCB_ROOT_ECBs = (struct gs_ecb *) pecb;
			g_kcb.KCB_NUMBER_OF_ECBs++;
		}
#else
        /* PRODUCTION MODE: pool exhausted -- halt. Increase G_NUMBER_OF_ECB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_ECB_GetFree: ECB pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_ECB in gemrtos_config.h for production.\n");    
        G_DEBUG_WHILEFOREVER;
#endif /* G_DEBUG_WHILEFOREVER_ENABLE */
    }

	if (pecb  != (struct gs_ecb  *) 0) {
		set_pointed_field(pecb,ECB_NextECBAEL , (struct gs_ecb *) 0); 
		set_pointed_field(pecb,ECB_NextECBASL , (struct gs_scb *) 0); 
		set_pointed_field(pecb,ECBType        , G_ECBType_NOT_SPECIFIED); 
		set_pointed_field(pecb,ECB_NextECB    , (struct gs_ecb *) 0); 
		set_pointed_field(pecb,ECB_PrevECB    , (struct gs_ecb *) 0); 
		set_pointed_field(pecb,ECB_AssocTCB   , (struct gs_tcb *) 0); 
		set_pointed_field(pecb,ECB_AssocRCB   , (struct g_rcb *) 0); 
		set_pointed_field(pecb,ECB_NextTCBAEL , (struct gs_ecb *) 0); 
		set_pointed_field(pecb,ECB_PrevTCBAEL , (struct gs_ecb *) 0); 
		set_pointed_field(pecb,ECB_RRDS       , (struct gs_rrds *) 0);
		set_pointed_field(pecb,ECBValue.i64   , G_LOWEST_PRIORITY);
		set_pointed_field(pecb,ECBState       , GS_ECBState_UNLINKED);   
		
		if ((ptcb != (GS_TCB *) 0) && (ptcb->TCBType != G_TCBType_IDLE)) {
			PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
			gk_TCBAEL_Link(pecb, ptcb);
		}
		else
		{
			pecb->ECB_AssocTCB = (GS_TCB *) 0; // The event is associated to a RCB (ie Trigger)
		}
	}
	
    GK_SAMPLE_FUNCTION_END(10026)
    return (pecb);
}  

/**
 *  \brief  Unlinks an RCB from the RCBFL list and returns its pointer,
 *          or NULL if no free RCB is available.
 *  \return Pointer to the RCB, or NULL if the pool is exhausted.
 */
GS_RCB *gk_RCB_GetFree(void)
{
    GK_SAMPLE_FUNCTION_BEGIN(10042)
    GS_RCB  *prcb = (GS_RCB  *) g_kcb.KCB_FREE_RCBs;
    
    // Check if free structure is available
    if (prcb  != (GS_RCB  *) 0) {
        g_kcb.KCB_FREE_RCBs = get_pointed_field(prcb, RCB_NextRCB);
    }
    else
    {
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        /* DEVELOPMENT MODE: RCB static pool exhausted -- allocating dynamically.
         * ACTION REQUIRED for production: increase G_NUMBER_OF_RCB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_RCB_GetFree: RCB pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_RCB in gemrtos_config.h for production.\n");
		if ((prcb = (GS_RCB  *) malloc(sizeof(GS_RCB))) != (GS_RCB  *) 0) {
			PRINT_ASSERT(((unsigned int) prcb % 4 == 0), "ERROR memory is not alligned");
			PRINT_ASSERT(((unsigned int) prcb != 0), "ERROR no memory available");

			prcb->BLOCK_HASH     = (unsigned int) prcb + G_RCB_HASH;

            // RCBs linked list for debugging
			GS_RCB  *prcb1 = (GS_RCB *) g_kcb.KCB_ROOT_RCBs;
			set_pointed_field(prcb, RCB_NEXT_RCBs, (GS_RCB *) prcb1);
			if (prcb1 != (GS_RCB  *) 0) {
				set_pointed_field(prcb1, RCB_PREV_RCBs, (GS_RCB *) prcb);
			}
			set_pointed_field(prcb, RCB_PREV_RCBs, (GS_RCB *) 0);
			g_kcb.KCB_ROOT_RCBs = (struct g_rcb *) prcb;
			g_kcb.KCB_NUMBER_OF_RCBs++;
		}
#else
        /* PRODUCTION MODE: pool exhausted -- halt. Increase G_NUMBER_OF_RCB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_RCB_GetFree: RCB pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_RCB in gemrtos_config.h for production.\n");    
        G_DEBUG_WHILEFOREVER;
#endif /* G_DEBUG_WHILEFOREVER_ENABLE */
    }

    if (prcb  != (GS_RCB  *) 0) {
		set_pointed_field(prcb, RCB_NextRCBASL , (struct gs_scb *) 0); 
		set_pointed_field(prcb, RCB_NextRCBGEL , (struct gs_ecb *) 0); 
		set_pointed_field(prcb, RCB_NextRCBWEL , (struct gs_ecb *) 0);
		set_pointed_field(prcb, RCB_NextRCB    , (struct g_rcb *) 0);   
		set_pointed_field(prcb, RCBType , GS_RCBType_FREE);
	}
	
	GK_SAMPLE_FUNCTION_END(10042)	
    return (prcb);
}

/**
 *  \brief  Unlinks a SCB from the free list and returns its pointer.
 *  \return Pointer to the SCB, or NULL if no free SCB is available.
 *  \relates Signal
 */
GS_SCB *gk_SCB_GetFree(void)
{
    GS_SCB *pscb = g_kcb.KCB_FREE_SCBs;

    GK_SAMPLE_FUNCTION_BEGIN(10014)

    // Check if free structure is available
    if (pscb != (GS_SCB *) 0) {
        g_kcb.KCB_FREE_SCBs = get_pointed_field(pscb, SCB_NextSCB);
    }
    else
    {
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        /* DEVELOPMENT MODE: SCB static pool exhausted -- allocating dynamically.
         * ACTION REQUIRED for production: increase G_NUMBER_OF_SCB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_SCB_GetFree: SCB pool exhausted. "
            "Increase G_NUMBER_OF_SCB in gemrtos_config.h for production.\n");
		if ((pscb = (GS_SCB  *) malloc(sizeof(GS_SCB))) != (GS_SCB  *) 0) {
			PRINT_ASSERT(((unsigned int) pscb % 4 == 0), "ERROR memory is not alligned");
			PRINT_ASSERT(((unsigned int) pscb != 0), "ERROR no memory available");

			pscb->BLOCK_HASH     = (unsigned int) pscb + G_SCB_HASH;

            // SCBs linked list for debugging
			GS_SCB *pscb_root;
			pscb_root = g_kcb.KCB_ROOT_SCBs;
			set_pointed_field(pscb, SCB_NEXT_SCBs, pscb_root);
			if (pscb_root != (struct gs_scb *) 0) set_pointed_field(pscb_root, SCB_PREV_SCBs, pscb);
			set_pointed_field(pscb, SCB_PREV_SCBs, (struct gs_scb *) 0);
			g_kcb.KCB_ROOT_SCBs = (struct gs_scb *) pscb;
			g_kcb.KCB_NUMBER_OF_SCBs++;
		}
#else
        /* PRODUCTION MODE: pool exhausted -- halt. Increase G_NUMBER_OF_SCB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_SCB_GetFree: SCB pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_SCB in gemrtos_config.h for production.\n");    
        G_DEBUG_WHILEFOREVER;
#endif /* G_DEBUG_WHILEFOREVER_ENABLE */
    }
 
    if (pscb != (GS_SCB *) 0) {
		set_pointed_field(pscb, SCBState, G_SCBState_UNLINKED);
		set_pointed_field(pscb, SCB_NextSCB, (struct gs_scb  *) 0);
	}

    GK_SAMPLE_FUNCTION_END(10014)
    
    return (pscb);
}    
  
/**
 *  \brief  Gets the pointer of a free RRDS from the free list.
 *  \return Pointer to RRDS, or NULL if no free RRDS are available.
 */
GS_RRDS *gk_RRDS_GetFree(void)
{
	GK_SAMPLE_FUNCTION_BEGIN(10033)
	
    GS_RRDS  *prrds = g_kcb.KCB_FREE_RRDSs;
    
	// Check if free structure is available
    if (prrds  != (GS_RRDS  *) 0) {
        g_kcb.KCB_FREE_RRDSs = get_pointed_field(prrds, RRDS_NextRRDS);
    }
    else
    {
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        /* DEVELOPMENT MODE: RRDS static pool exhausted -- allocating dynamically.
         * ACTION REQUIRED for production: increase G_NUMBER_OF_RRDS in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_RRDS_GetFree: RRDS pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_RRDS in gemrtos_config.h for production.\n");
		if ((prrds = (GS_RRDS  *) malloc(sizeof(GS_RRDS))) != (GS_RRDS  *) 0) {
			PRINT_ASSERT(((unsigned int) prrds % 4 == 0), "ERROR memory is not alligned");
			PRINT_ASSERT(((unsigned int) prrds != 0), "ERROR no memory available");

			prrds->BLOCK_HASH      = (unsigned int) prrds + G_RRDS_HASH;

            // RRDSs linked list for debugging
			GS_RRDS  *prrds1;
			prrds1 = g_kcb.KCB_ROOT_RRDSs;
			set_pointed_field(prrds, RRDS_NEXT_RRDSs, prrds1);
			if (prrds1 != (struct gs_rrds *) 0) set_pointed_field(prrds1, RRDS_PREV_RRDSs, prrds);
			set_pointed_field(prrds, RRDS_PREV_RRDSs, (struct gs_rrds *) 0);
			g_kcb.KCB_ROOT_RRDSs = (struct gs_rrds *) prrds;
			g_kcb.KCB_NUMBER_OF_RRDSs++;
		}
#else
        /* PRODUCTION MODE: pool exhausted -- halt. Increase G_NUMBER_OF_RRDS in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_RRDS_GetFree: RRDS pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_RRDS in gemrtos_config.h for production.\n");    
        G_DEBUG_WHILEFOREVER;
#endif /* G_DEBUG_WHILEFOREVER_ENABLE */
    }
   
    if (prrds  != (GS_RRDS  *) 0) {
		set_pointed_field(prrds, RRDS_NextRRDS, (struct gs_rrds *) 0);
		set_pointed_field(prrds, RRDS_AsocECB, (struct gs_ecb  *) 0);
	}

    GK_SAMPLE_FUNCTION_END(10033)
    return (prrds);
}
 
/**gu_SchedulingListCreate
 *  \brief The gu_SchedulingListCreate function creates a new scheduling list. The type of scheduling discipline used by the list is determined by the lcb_type parameter. 
 *  \exception par_following The function accepts one parameter:
 *  \param [in] lcb_type An enumeration value specifying the type of scheduling list to create. This defines the scheduling discipline that will govern task scheduling within the new scheduling list.
 *  \return The gu_SchedulingListCreate function returns a pointer (GS_LCB *) to the newly created GS_LCB structure. This pointer is essential for all subsequent operations involving this specific scheduling list. A '(GS_LCB *) 0' return value indicates failure to create the scheduling list.
 *  \relates Scheduling_List
 *  \remark gu_get_lcb
 *  \sa gu_SchedulingListExclusionSet, gu_SchedulingListAssociateProcessor, gu_SchedulingListAssociateTask
 */
GS_LCB *gu_SchedulingListCreate(enum lcbtype lcb_type)
{
	gm_GeMRTOSCriticalSectionEnter();
		GS_LCB *plcb = gk_Get_LCB(lcb_type);
	gm_GeMRTOSCriticalSectionExit();
    return (plcb);
}


GS_LCB *gk_Get_LCB(enum lcbtype lcb_type)
{
    GS_LCB *plcb = g_kcb.KCB_FREE_LCBs; 

	GK_SAMPLE_FUNCTION_BEGIN(10087)

	// Check if free structure is available
	if (plcb  != (GS_LCB  *) 0) {
		plcb = g_kcb.KCB_FREE_LCBs;
		g_kcb.KCB_FREE_LCBs = plcb->LCB_NextLCBL;
	}
	else
	{
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        /* DEVELOPMENT MODE: LCB static pool exhausted -- allocating dynamically.
         * ACTION REQUIRED for production: increase G_NUMBER_OF_LCB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_Get_LCB: LCB pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_LCB in gemrtos_config.h for production.\n");
		if ((plcb = (GS_LCB  *) malloc(sizeof(GS_LCB))) != (GS_LCB  *) 0) {
			PRINT_ASSERT(((unsigned int) plcb % 4 == 0), "ERROR memory is not alligned");
			PRINT_ASSERT(((unsigned int) plcb != 0), "ERROR no memory available");

			plcb->BLOCK_HASH      = (unsigned int) plcb + G_LCB_HASH;

            // LCBs linked list for debugging
			plcb->LCB_NEXT_LCBs = g_kcb.KCB_ROOT_LCBs;
			if (g_kcb.KCB_ROOT_LCBs != (struct gs_lcb *) 0) g_kcb.KCB_ROOT_LCBs->LCB_PREV_LCBs = plcb;
			plcb->LCB_PREV_LCBs = (struct gs_lcb *) 0;
			g_kcb.KCB_ROOT_LCBs = (struct gs_lcb *) plcb;
			g_kcb.KCB_NUMBER_OF_LCBs++;
		}
#else
        /* PRODUCTION MODE: pool exhausted -- halt. Increase G_NUMBER_OF_LCB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_Get_LCB: LCB pool exhausted. "
            "Increase G_NUMBER_OF_LCB in gemrtos_config.h for production.\n");
        G_DEBUG_WHILEFOREVER;
#endif /* G_DEBUG_WHILEFOREVER_ENABLE */
	}

	if (plcb  != (GS_LCB  *) 0) {
		plcb->LCB_NextTCBRUNL   = (struct gs_tcb *) 0;       /* Pointer to the TCB list of running tasks */
		plcb->LCB_NextTCBRDYL   = (struct gs_tcb *) 0;       /* Pointer to the TCB of the Highest Priority Task */
		plcb->LCB_NextLCBFPL    = (struct gs_pcb *) 0;       /* Next free processor for this list */
		plcb->LCBState          = GS_LCBState_UNLINKED;
		plcb->LCBType           = lcb_type; // GS_LCBType_UNSPECIFIED;
		plcb->LCBExclusion      = (G_UINT32) G_NUMBER_OF_PCB + 1;
		plcb->LCBCurrentRunning = (G_UINT32) 0;
		
		// Link to the LCBL linked list
		gk_LCBL_Link((GS_LCB *) plcb);		
	}
	
	GK_SAMPLE_FUNCTION_END(10087)
    return (plcb);
}

/** gu_SchedulingListAssociateProcessor
 *  \brief The gu_SchedulingListAssociateProcessor function associates a system processor with a specified scheduling list. The priority is assigned to the association between the processor and the scheduling list. When tasks are ready to execute, the processor will select and execute the task from the highest priority scheduling list that it is associated with. The association with the lowest numerical value indicates the highest priority, ensuring that tasks in the most critical scheduling lists are prioritized for execution.
 *  \exception par_following The following parameters are required for the gu_SchedulingListAssociateProcessor function:
 *  \param [in] plcb A pointer to the GS_LCB structure representing the scheduling list to be associated with the processor.
 *  \param [in] cpu_id The ID of the processor to be associated with the scheduling list.
 *  \param [in] priority The priority level for the association. A lower value indicates a higher priority, and the processor will first search the scheduling lists associated with the highest priority tasks that are ready to execute.
 *  \return The gu_SchedulingListAssociateProcessor function returns G_TRUE if the association is successful. It returns G_FALSE if the association fails.
 *  \sa gu_SchedulingListCreate, gu_SchedulingListExclusionSet, gu_SchedulingListAssociateTask
 *  \relates Scheduling_List
 *  \remark gu_list_associate_processor
 */ 
G_UINT32 gu_SchedulingListAssociateProcessor(GS_LCB *plcb, G_UINT32 cpu_id, G_UINT32 priority)
{
    GS_PCBAssocLCB *ppcbalcb;
    GS_PCBAssocLCB *ppcbalcb_aux;
    enum pcbstate pcb_state =  g_kcb.G_PCBTbl[cpu_id-1].PCBState;

	gm_GeMRTOSCriticalSectionEnter();

	PRINT_ASSERT((gkm_LCB_IsValid((void *)plcb) == G_TRUE),"ERROR LCB is not valid\n");
	PRINT_ASSERT(((cpu_id >= (int) 1) && (cpu_id <= G_NUMBER_OF_PCB)),"ERROR Processor cpu_id= %d\n",(int) cpu_id);

	ppcbalcb = gk_Get_PCBAssocLCB();
	if (ppcbalcb == NULL) {
		PRINT_ASSERT(G_FALSE, "ERROR gu_SchedulingListAssociateProcessor: PCBAssocLCB pool exhausted\n");
		gm_GeMRTOSCriticalSectionExit();
		return G_FALSE;
	}
	PRINT_ASSERT((gkm_PCBAssocLCB_IsValid((void *)ppcbalcb) == G_TRUE),"ERROR GS_PCBAssocLCB is not valid\n");

	ppcbalcb->PCB_RDY_priority     = (G_UINT32) priority;
	ppcbalcb->PCB_RDY_LCBL     = (struct gs_lcb *) plcb;

	if ((g_kcb.G_PCBTbl[cpu_id-1].PCB_AssocLCB == (struct gs_pcb_rdy_lcbl *) 0) ||
		(g_kcb.G_PCBTbl[cpu_id-1].PCB_AssocLCB->PCB_RDY_priority > priority))
	{
		if (pcb_state == GS_PCBState_FREE) {
			gk_LCBFPL_Unlink(cpu_id);
		}
		ppcbalcb->gs_pcb_rdy_lcbl_next = (struct gs_pcb_rdy_lcbl *) g_kcb.G_PCBTbl[cpu_id-1].PCB_AssocLCB;
		ppcbalcb->gs_pcb_rdy_lcbl_prev = (struct gs_pcb_rdy_lcbl *) 0;
		if (g_kcb.G_PCBTbl[cpu_id-1].PCB_AssocLCB != (struct gs_pcb_rdy_lcbl *) 0) {
			g_kcb.G_PCBTbl[cpu_id-1].PCB_AssocLCB->gs_pcb_rdy_lcbl_prev = ppcbalcb;
		}
		g_kcb.G_PCBTbl[cpu_id-1].PCB_AssocLCB = ppcbalcb;
		
		if ((pcb_state != GS_PCBState_NOTRUNNING)) {
			gk_LCBFPL_Link(cpu_id);
		}
	}
	else
	{
		ppcbalcb_aux = g_kcb.G_PCBTbl[cpu_id-1].PCB_AssocLCB;
		while ((ppcbalcb_aux->gs_pcb_rdy_lcbl_next != (struct gs_pcb_rdy_lcbl *) 0) &&
		      (ppcbalcb_aux->gs_pcb_rdy_lcbl_next->PCB_RDY_priority < priority))
		{
			ppcbalcb_aux = ppcbalcb_aux->gs_pcb_rdy_lcbl_next;
		}
		ppcbalcb->gs_pcb_rdy_lcbl_next = ppcbalcb_aux->gs_pcb_rdy_lcbl_next;
		ppcbalcb->gs_pcb_rdy_lcbl_prev = ppcbalcb_aux;
		if (ppcbalcb_aux->gs_pcb_rdy_lcbl_next != (struct gs_pcb_rdy_lcbl *) 0)
			ppcbalcb_aux->gs_pcb_rdy_lcbl_next->gs_pcb_rdy_lcbl_prev = ppcbalcb;
		ppcbalcb_aux->gs_pcb_rdy_lcbl_next = ppcbalcb;
	}

	gm_GeMRTOSCriticalSectionExit();
    
    return(G_TRUE);
}


/**
 *  \brief Creates a PCBAssocLCB structure and returns its pointer.
 *  \return Pointer to the PCBAssocLCB created
 *  \relates Scheduling_List
 *  \sa gu_SchedulingListAssociateProcessor, gu_SchedulingListCreate
 */
GS_PCBAssocLCB *gk_Get_PCBAssocLCB(void)
{
    GS_PCBAssocLCB *ppcbalcb = g_kcb.KCB_FREE_RDYs;
	
 	GK_SAMPLE_FUNCTION_BEGIN(10088)
	
    // Check if free structure is available
    if (ppcbalcb  != (struct gs_pcb_rdy_lcbl *) 0) {
        g_kcb.KCB_FREE_RDYs = ppcbalcb->gs_pcb_rdy_lcbl_next;
    }
    else
    {
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        /* DEVELOPMENT MODE: PCBAssocLCB static pool exhausted -- allocating dynamically.
         * ACTION REQUIRED for production: increase G_NUMBER_OF_PCBAssocLCB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_Get_PCBAssocLCB: PCBAssocLCB pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_PCBAssocLCB in gemrtos_config.h for production.\n");
		if ((ppcbalcb = (GS_PCBAssocLCB  *) malloc(sizeof(GS_PCBAssocLCB))) != (GS_PCBAssocLCB  *) 0) {
			PRINT_ASSERT(((unsigned int) ppcbalcb % 4 == 0), "ERROR memory is not alligned");
			PRINT_ASSERT(((unsigned int) ppcbalcb != 0), "ERROR no memory available");

			ppcbalcb->BLOCK_HASH      = (unsigned int) ppcbalcb + G_PCBALCB_HASH;

            // PCBAssocLCBs linked list for debugging
			ppcbalcb->gs_pcb_lcbl_nexts = g_kcb.KCB_ROOT_RDYs;
			if (g_kcb.KCB_ROOT_RDYs != (struct gs_pcb_rdy_lcbl *) 0) g_kcb.KCB_ROOT_RDYs->gs_pcb_lcbl_prevs = ppcbalcb;
			ppcbalcb->gs_pcb_lcbl_prevs = (struct gs_pcb_rdy_lcbl *) 0;
			g_kcb.KCB_ROOT_RDYs = (struct gs_pcb_rdy_lcbl *) ppcbalcb;
			g_kcb.KCB_NUMBER_OF_PCBAssocLCBs++;
		}
#else
        /* PRODUCTION MODE: pool exhausted -- halt. Increase G_NUMBER_OF_PCBAssocLCB in gemrtos_config.h. */
        gk_fprintf("[ POOL WARNING ] gk_Get_PCBAssocLCB: PCBAssocLCB pool exhausted, allocated dynamically. "
            "Increase G_NUMBER_OF_PCBAssocLCB in gemrtos_config.h for production.\n");    
        G_DEBUG_WHILEFOREVER;
#endif /* G_DEBUG_WHILEFOREVER_ENABLE */
    }
  
    if (ppcbalcb  != (struct gs_pcb_rdy_lcbl *) 0) {  
		// Initialize to zero the structure fields
		ppcbalcb->gs_pcb_rdy_lcbl_next = (struct gs_pcb_rdy_lcbl *) 0;
		ppcbalcb->gs_pcb_rdy_lcbl_prev = (struct gs_pcb_rdy_lcbl *) 0;
		ppcbalcb->PCB_RDY_priority         = (G_UINT32) 0;
		ppcbalcb->PCB_RDY_LCBL         = (struct gs_lcb *) 0;
	}

	GK_SAMPLE_FUNCTION_END(10088)
    return (ppcbalcb);
}

/**
 *  \brief Reserves system memory to store the Processor Control Blocks of the system (PCB).
 *  \return G_TRUE when successful, G_FALSE otherwise.
 *  \relates Processor
 */
G_UINT32 gk_Init_PCBs(void)
{
    int i;
	GS_PCB *ppcb;
	
	GK_SAMPLE_FUNCTION_BEGIN(10089)
	
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    g_kcb.KCB_NUMBER_OF_PCBs = (G_UINT32) G_NUMBER_OF_PCB;
#endif
    
	// Initialize Processor Structures creating IDLE TCBs
	for (i = 0;  i < G_NUMBER_OF_PCB;  i++) {

		ppcb = &g_kcb.G_PCBTbl[i];
		
        ppcb->PCB_AssocLCB  = (struct gs_pcb_rdy_lcbl *) 0;
		ppcb->PCBID         = (G_UINT32) i+1;
		ppcb->BLOCK_HASH    = (unsigned int) ppcb + G_PCB_HASH;
		ppcb->PCBType       = (int) GS_PCBType_AVAILABLE; 
		ppcb->PCB_NextPCB   = (struct gs_pcb *) 0; 
		ppcb->PCB_PrevPCB   = (struct gs_pcb *) 0;
		ppcb->PCB_EXECTCB   = (struct gs_tcb *) 0;
        ppcb->PCBState      = GS_PCBState_NOTRUNNING;
	}
	
    // Assign the default list to the processor with the default priority
	for (i = 0;  i < G_NUMBER_OF_PCB;  i++) {		
        gu_SchedulingListAssociateProcessor((GS_LCB *) G_TASK_LCB_DEFAULT, (G_UINT32) i+1, (G_UINT32) G_TASK_LCB_DEFAULT_PRIORITY); 
        PRINT_ASSERT((g_kcb.G_PCBTbl[i].PCB_AssocLCB != (GS_PCBAssocLCB *) 0),"ERROR ppcb->PCB_AssocLCB\n"); 
	}
	
	PRINT_ASSERT((gkm_PCBAssocLCB_IsValid(g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_AssocLCB) == G_TRUE), "ERROR structure %p", g_kcb.G_PCBTbl[gm_ProcessorId() -1].PCB_AssocLCB);	

	GK_SAMPLE_FUNCTION_END(10089)
	return (G_TRUE);
}

G_UINT32 gk_Create_IDLE_tasks(void)
{
    unsigned int i;
	GS_PCB *ppcb;
	char task_name[G_TCB_DESCRIPTION_LENGTH];

	GK_SAMPLE_FUNCTION_BEGIN(10090)
	
	/* Create IDLE task for each processor */
	for (i = 0;  i < G_NUMBER_OF_PCB;  i++) {

		// Create the IDLE task for each processor
		ppcb = (GS_PCB *) &g_kcb.G_PCBTbl[i];
		PRINT_ASSERT((gkm_PCB_IsValid(ppcb) == G_TRUE),"ERROR PCB is not valid\n");
		
		ppcb->PCB_IDLETCB  = (struct gs_tcb *) gk_TCB_GetFree(G_TASK_STACKSIZE_IDLE);
		PRINT_ASSERT((ppcb->PCB_IDLETCB != (struct gs_tcb *) 0),"ERROR no memory for IDLE ECB\n");
		
		ppcb->PCB_IDLETCB->TCBType = G_TCBType_IDLE;         
		ppcb->PCB_IDLETCB->TCB_TaskCode = gk_CODE_IDLE_TASK; 
		ppcb->PCB_IDLETCB->TCB_TaskArg = (void *) NULL; 
		ppcb->PCB_IDLETCB->TCBReadyPriority = G_IDLETASK_PRIORITY; 
		ppcb->PCB_IDLETCB->TCBRunPriority = G_IDLETASK_PRIORITY;
		ppcb->PCB_IDLETCB->TCBCurrentPriority = G_IDLETASK_PRIORITY;
		ppcb->PCB_IDLETCB->TCB_RDY_LCB_Index = (struct gs_lcb *) G_TASK_LCB_DEFAULT;
		ppcb->PCB_IDLETCB->TCB_NextTCB = (struct gs_tcb *) 0;
		ppcb->PCB_IDLETCB->TCB_PrevTCB = (struct gs_tcb *) 0;
		ppcb->PCB_IDLETCB->TCBState = G_TCBState_UNLINKED;
		ppcb->PCB_IDLETCB->TCB_AssocPCB = 0;

		snprintf(task_name, G_TCB_DESCRIPTION_LENGTH, "idle proc %u", i + 1);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
		strncpy((char *) ppcb->PCB_IDLETCB->TCB_description, (char *) task_name, G_TCB_DESCRIPTION_LENGTH-1);
#pragma GCC diagnostic pop

		gk_TASK_STK_INIT(ppcb->PCB_IDLETCB);
		set_pointed_field(ppcb->PCB_IDLETCB, TCB_MTX_NESTED, 0);	
	}
	
	GK_SAMPLE_FUNCTION_END(10090)
    return (G_TRUE);
}

G_UINT64 gk_SystemTimeGet (void)
{
	TIMEPRIORITY hw_time; 
	PRINT_ASSERT((gm_IsPrcGranted()),"ERROR getting critical section in %s, %d\n", __FUNCTION__, __LINE__);

	hw_time.i32[1] = (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_TM_CNT_HGH); 
	hw_time.i32[0] = (G_UINT32) gm_IORD(GEMRTOS_DRIVER_BASE, ADDR_SMP); 

	return ((G_UINT64) hw_time.i64);
}

/** \def gu_SystemTimeGet
 *  \brief The gu_SystemTimeGet function returns the system time in system time units. This is the time in non frozen mode.
 *  \exception par_following The gu_SystemTimeGet function does not accept any parameters.
 *  \exception class_name System
 *  \exception prototype gu_SystemTimeGet
 *  \return The gu_SystemTimeGet function returns the current system time in system time units (type G_UINT64).
 *  \remark System
 *  \relates Time
 */
G_UINT64 gu_SystemTimeGet(void) 
{ 
	TIMEPRIORITY hw_time; 
	
	gm_GeMRTOSCriticalSectionEnter(); 
		hw_time.i64 = gk_SystemTimeGet();
	gm_GeMRTOSCriticalSectionExit(); 

	return ((G_UINT64) hw_time.i64);
}

OPTIMIZE_RESTORE

