/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS core management functions
 *  \details This file contains the funtion for core management.
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

// Check https://stackoverflow.com/questions/1154709/how-can-i-hide-the-declaration-of-a-struct-in-c
// https://gcc.gnu.org/onlinedocs/gcc/Nios-II-Options.html#index-mno-bypass-cache
// #pragma GCC optimize('mno-cache-volatile')

#include <inttypes.h>

/**gk_TCB_GetFree
 *  \brief Creates a TCB and returns its pointer
 *  Returns a pointer to a Free TCB, NULL if it is not a TCB available
 *  \return A pointer to the TCB or NULL if no TCB is available
 *  \relates Task
 */ 
GS_TCB *gk_TCB_GetFree(unsigned int stack_size)
{
    GS_TCB *ptcb = g_kcb.KCB_FREE_TCBs;
	GS_STK *top_stack;

    
    GK_SAMPLE_FUNCTION_BEGIN(10040)
    
    // Check if free structure is available
    if (ptcb  != (struct gs_tcb  *) 0) {
        g_kcb.KCB_FREE_TCBs = (struct gs_tcb *) get_pointed_field(ptcb, TCB_NextTCB);
    }
    else
    {
		
		if ((ptcb = (GS_TCB *) malloc(sizeof(GS_TCB))) == 0) PRINT_FUNCTION;
        PRINT_ASSERT(((unsigned int) ptcb % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) ptcb != 0), "ERROR no memory available");
			
        // INITIALIZE THE TCB STRUCTURE
        ptcb->BLOCK_HASH           = (unsigned int) ptcb + G_TCB_HASH;
		
        // TCBs linked list for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
		GS_TCB *ptcb1;
		ptcb1 = g_kcb.KCB_ROOT_TCBs;
        set_pointed_field(ptcb, TCB_NEXT_TCBs, ptcb1);
        if (ptcb1 != (struct gs_tcb *) 0) set_pointed_field(ptcb1, TCB_PREV_TCBs, ptcb);
        set_pointed_field(ptcb, TCB_PREV_TCBs, (struct gs_tcb *) 0);
        g_kcb.KCB_ROOT_TCBs = (struct gs_tcb *) ptcb;
        g_kcb.KCB_NUMBER_OF_TCBs++;
#endif
		
		if ((top_stack = (GS_STK *) malloc(sizeof(GS_STK) * stack_size)) == 0) PRINT_FUNCTION;
        PRINT_ASSERT(((unsigned int) top_stack % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) top_stack != 0), "ERROR no memory available");		
		
        GS_STK *btn_stack = top_stack + stack_size - 1;
        PRINT_ASSERT(((unsigned int) btn_stack % 4 == 0), "ERROR memory is not alligned");
        
        
        set_pointed_field(ptcb, TCB_StackTop, (GS_STK *) top_stack);
		set_pointed_field(ptcb, TCB_StackBottom, (GS_STK *) btn_stack); 		
        
		set_pointed_field(ptcb, TCB_description[0], '\0');


#ifndef __niosX_arch__
	    getcontext(&ptcb->uctx);
		ptcb->uctx.uc_stack.ss_size = 16 * 1024;
		ptcb->uctx_stack = malloc(ptcb->uctx.uc_stack.ss_size);
	    ptcb->uctx.uc_stack.ss_sp = ptcb->uctx_stack;
	    ptcb->uctx.uc_stack.ss_flags = 0;
	    // makecontext(&ptcb->uctx, *(void **) &ptcb->TCB_TaskCode, 1, ptcb->TCB_TaskArg);
	    makecontext(&ptcb->uctx, gu_pthread, 1, ptcb);
#endif
       
    }
 
    set_pointed_field(ptcb, TCB_NextTCBAEL      , (struct gs_ecb *) 0); 
    set_pointed_field(ptcb, TCB_NextTCBASL      , (struct gs_scb *) 0); 
    set_pointed_field(ptcb, TCB_NextTCBPSL      , (struct gs_scb *) 0); 
    set_pointed_field(ptcb, TCBType             , G_TCBType_OneShot); 
    set_pointed_field(ptcb, TCBReadyPriority    , G_LOWEST_PRIORITY -1); 
    set_pointed_field(ptcb, TCBRunPriority      , G_LOWEST_PRIORITY -1); 
    set_pointed_field(ptcb, TCBPeriod           , G_LATEST_TIME - 1);
    set_pointed_field(ptcb, TCB_StackPointer    , (GS_STK *) ptcb->TCB_StackBottom);
    set_pointed_field(ptcb, TCB_TaskCode        , (void *) 0); 
    set_pointed_field(ptcb, TCB_TaskArg         , (void   *) 0); 
    set_pointed_field(ptcb, TCB_NextTCB         , (struct gs_tcb *) 0); 
    set_pointed_field(ptcb, TCB_PrevTCB         , (struct gs_tcb *) 0); 
    set_pointed_field(ptcb, TCB_AssocPCB        , 0); 
    set_pointed_field(ptcb, TCB_INTNumber       , -1); 
    set_pointed_field(ptcb, TCB_RDY_LCB_Index   , (GS_LCB *) G_TASK_LCB_DEFAULT); 
    set_pointed_field(ptcb, TCBState            , G_TCBState_UNLINKED);
	set_pointed_field(ptcb, TCB_MTX_NESTED      , 0);
	set_pointed_field(ptcb, TCB_PrevExclusion   , 0);
	

    GK_SAMPLE_FUNCTION_END(10040)
    return (ptcb);
}


/**gk_ECB_GetFree
 *  \brief  Returns a pointer to a Free ECB, NULL if there is not ECB available
 *  \return Returns the pointer to a new ECB, NULL if there is not ECB available
 *  \todo System signal should be implmented when no free ECB is available
 *  \relates Event
 */ 
GS_ECB *gk_ECB_GetFree(void)
{
    GS_ECB  *pecb = g_kcb.KCB_FREE_ECBs;
    
    GK_SAMPLE_FUNCTION_BEGIN(10026)
    
    // Check if free structure is available
    if (pecb  != (struct gs_ecb  *) 0) {
        g_kcb.KCB_FREE_ECBs = get_pointed_field(pecb, ECB_NextECB);
    }
    else
    {    
        // from https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library/3430102#3430102    
        // mem = malloc(sizeof(GS_ECB) + 15); // Adding 15 to make sure there exist an address module 16 to align the block
        // pecb = (GS_ECB  *) (((uintptr_t)mem+7) & ~ (uintptr_t)0x03);

		if ((pecb = (GS_ECB  *) malloc(sizeof(GS_ECB))) == 0) PRINT_FUNCTION;
        PRINT_ASSERT(((unsigned int) pecb % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) pecb != 0), "ERROR no memory available");
		
		// fprintf(stderr,"diff malloc= %u\n", (unsigned int) __alt_stack_pointer - (unsigned int) pecb);
		
        pecb->BLOCK_HASH     = (unsigned int) pecb + G_ECB_HASH;
        // 15-07-2024 set_pointed_field(pecb, malloc_address, pecb);
    
        // TCBs linked list for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	    GS_ECB  *pevent1;
        set_pointed_field(pecb, ECB_NEXT_ECBs, g_kcb.KCB_ROOT_ECBs);
		pevent1 = g_kcb.KCB_ROOT_ECBs;
        if (pevent1 != (struct gs_ecb *) 0) set_pointed_field(pevent1, ECB_PREV_ECBs, pecb);
        set_pointed_field(pecb, ECB_PREV_ECBs, (struct gs_ecb *) 0);
        g_kcb.KCB_ROOT_ECBs = (struct gs_ecb *) pecb;
        g_kcb.KCB_NUMBER_OF_ECBs++;
#endif
    }

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
    
    GK_SAMPLE_FUNCTION_END(10026)

	PRINT_ASSERT((gkm_ECB_IsValid(pecb) == G_TRUE),"ERROR ECB is not valid\n");

    return (pecb);
}  

/**gk_RCB_GetFree
 *  \brief 
 *  Unlinks an RCB from the RCBFL list and returns its pointer or NULL if no free RCB is available
 *  \return Pointer to the RCB or NULL when no RCB available
 *  \todo system signal should be implemented when no free RCB is available
 *  \todo link all the currently used resources with RCB_NextRCB
 *  \relates Resource
 */
G_RCB *gk_RCB_GetFree(void)
{
    GK_SAMPLE_FUNCTION_BEGIN(10042)
    G_RCB  *prcb = (G_RCB  *) g_kcb.KCB_FREE_RCBs;
    
    // Check if free structure is available
    if (prcb  != (G_RCB  *) 0) {
        g_kcb.KCB_FREE_RCBs = get_pointed_field(prcb, RCB_NextRCB);
    }
    else
    {      
		if ((prcb = (G_RCB  *) malloc(sizeof(G_RCB))) == 0) PRINT_FUNCTION;		
        PRINT_ASSERT(((unsigned int) prcb % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) prcb != 0), "ERROR no memory available");
		
        prcb->BLOCK_HASH     = (unsigned int) prcb + G_RCB_HASH;
        
        // TCBs linked list for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
		G_RCB  *prcb1 = (G_RCB *) g_kcb.KCB_ROOT_RCBs;
        set_pointed_field(prcb, RCB_NEXT_RCBs, (G_RCB *) prcb1);
        if (prcb1 != (G_RCB  *) 0) {
            set_pointed_field(prcb1, RCB_PREV_RCBs, (G_RCB *) prcb);
        }
        set_pointed_field(prcb, RCB_PREV_RCBs, (G_RCB *) 0);
        g_kcb.KCB_ROOT_RCBs = (struct g_rcb *) prcb;
        g_kcb.KCB_NUMBER_OF_RCBs++;
#endif
    }

    // set_pointed_field(prcb, RCBState       , GK_RCBState_FREE); 
    set_pointed_field(prcb, RCB_NextRCBASL , (struct gs_scb *) 0); 
    set_pointed_field(prcb, RCB_NextRCBGEL , (struct gs_ecb *) 0); 
    set_pointed_field(prcb, RCB_NextRCBWEL , (struct gs_ecb *) 0);
    set_pointed_field(prcb, RCB_NextRCB    , (struct g_rcb *) 0);   // Link all the system resources currently used !!!!!!!!
    set_pointed_field(prcb, RCBType , GK_RCBType_FREE);

    GK_SAMPLE_FUNCTION_END(10042)

    return (prcb);
}

/**gk_SCB_GetFree
 *  \brief 
 *  Unlinks a SCB from free list and return its pointer
 *  \return Pointer to the SCB or NULL if no free SCB is available 
 *  \relates Signal
 */
GS_SCB *gk_SCB_GetFree(void)
{
    GS_SCB *pscb = g_kcb.KCB_FREE_SCBs;

    
    GK_SAMPLE_FUNCTION_BEGIN(10014)
	
    // Check if free structure is available
    if (pscb != (struct gs_scb  *) 0) {
        g_kcb.KCB_FREE_SCBs = get_pointed_field(pscb, SCB_NextSCB);
    }
    else
    {

		if ((pscb = (GS_SCB  *) malloc(sizeof(GS_SCB))) == 0) PRINT_FUNCTION;		
        PRINT_ASSERT(((unsigned int) pscb % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) pscb != 0), "ERROR no memory available");
	
		
        pscb->BLOCK_HASH     = (unsigned int) pscb + G_SCB_HASH;
        // 15-07-2024 set_pointed_field(pscb, malloc_address, pscb);
        
        // SCBs linked list for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
		GS_SCB *pscb_root;
		pscb_root = g_kcb.KCB_ROOT_SCBs;
        set_pointed_field(pscb, SCB_NEXT_SCBs, pscb_root);
        if (pscb_root != (struct gs_scb *) 0) set_pointed_field(pscb_root, SCB_PREV_SCBs, pscb);
        set_pointed_field(pscb, SCB_PREV_SCBs, (struct gs_scb *) 0);
        g_kcb.KCB_ROOT_SCBs = (struct gs_scb *) pscb;
		g_kcb.KCB_NUMBER_OF_SCBs++;
#endif
    }
        
    set_pointed_field(pscb, SCBState, G_SCBState_FREE);
    set_pointed_field(pscb, SCBState, G_SCBState_UNLINKED);
    set_pointed_field(pscb, SCB_NextSCB, (struct gs_scb  *) 0);

    GK_SAMPLE_FUNCTION_END(10014)
    
    return (pscb);
}    
  
/**gk_RRDS_GetFree
 *  \brief 
 *  Gets the pointer of a free RRDS from the free list
 *  \return Pointer to RRDS, or NULL if no free RRDS are available
 *  \todo System signal should be implmented when no free RRDS is available
 *  \relates RRDS
 */ 
GS_RRDS *gk_RRDS_GetFree(void)
{
	GK_SAMPLE_FUNCTION_BEGIN(10033)
	
    GS_RRDS  *prrds = g_kcb.KCB_FREE_RRDSs;
    
	// Check if free structure is available
    if (prrds  != (struct gs_rrds  *) 0) {
        g_kcb.KCB_FREE_RRDSs = get_pointed_field(prrds, RRDS_NextRRDS);
    }
    else
    {    
		if ((prrds = (GS_RRDS  *) malloc(sizeof(GS_RRDS))) == 0) PRINT_FUNCTION;
        PRINT_ASSERT(((unsigned int) prrds % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) prrds != 0), "ERROR no memory available");
        
        prrds->BLOCK_HASH      = (unsigned int) prrds + G_RRDS_HASH;
		
        // 15-07-2024 set_pointed_field(prrds, malloc_address, prrds);
    
        // TCBs linked list for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
		GS_RRDS  *prrds1;
		prrds1 = g_kcb.KCB_ROOT_RRDSs;
        set_pointed_field(prrds, RRDS_NEXT_RRDSs, prrds1);
        if (prrds1 != (struct gs_rrds *) 0) set_pointed_field(prrds1, RRDS_PREV_RRDSs, prrds);
        set_pointed_field(prrds, RRDS_PREV_RRDSs, (struct gs_rrds *) 0);
        g_kcb.KCB_ROOT_RRDSs = (struct gs_rrds *) prrds;
        g_kcb.KCB_NUMBER_OF_RRDSs++;
#endif
    }
    
    // 18-10-2024 set_pointed_field(prrds, RRDS_State, 0);           ///< \todo verify correctness !!!!!
    // 18-10-2024 set_pointed_field(prrds, RRDS_Type, 0);            ///< \todo verify correctness !!!!!
    set_pointed_field(prrds, RRDS_NextRRDS, (struct gs_rrds *) 0);
    // 15-07-2024 set_pointed_field(prrds, RRDS_NextSCB, (struct gs_scb  *) 0);
    set_pointed_field(prrds, RRDS_AsocECB, (struct gs_ecb  *) 0);    

    GK_SAMPLE_FUNCTION_END(10033)
    return (prrds);
}
 
/**gu_SchedulingListCreate
 *  \brief The gu_SchedulingListCreate function creates a new scheduling list. The type of scheduling discipline used by the list is determined by the lcbtype parameter. 
 *  \exception par_following The function accepts one parameter:
 *  \param [in] lcbtype An enumeration value specifying the type of scheduling list to create. This defines the scheduling discipline that will govern task scheduling within the new scheduling list.
 *  \return The gu_SchedulingListCreate function returns a pointer (GS_LCB *) to the newly created GS_LCB structure. This pointer is essential for all subsequent operations involving this specific scheduling list. A '(GS_LCB *) 0' return value indicates failure to create the scheduling list.
 *  \relates Scheduling_List
 *  \remark gu_get_lcb
 *  \sa gu_SchedulingListExclusionSet, gu_SchedulingListAssociateProcessor, gu_SchedulingListAssociateTask
 *  \todo Rewrite all the LCB support
 *  \todo Write the destroy LCB structure
 *  \todo remove the priority of the scheduling list. The priority is given by the order of the pcb_associated_lcb links in pcb structures.
 */
GS_LCB *gu_SchedulingListCreate(enum lcbtype lcbtype)
{
	gm_GeMRTOSCriticalSectionEnter;
		GS_LCB *plcb = gk_Get_LCB(lcbtype);
	gm_GeMRTOSCriticalSectionExit;
    return (plcb);
}


GS_LCB *gk_Get_LCB(enum lcbtype lcbtype)
{
    GS_LCB *plcb; 

	// Check if free structure is available
	if (g_kcb.KCB_FREE_LCBs  != (struct gs_lcb  *) 0) {
		plcb = g_kcb.KCB_FREE_LCBs;
		g_kcb.KCB_FREE_LCBs = plcb->LCB_NextLCBL;
	}
	else
	{
		if ((plcb = (GS_LCB  *) malloc(sizeof(GS_LCB))) == 0) PRINT_FUNCTION;			
		PRINT_ASSERT(((unsigned int) plcb % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) plcb != 0), "ERROR no memory available");

		plcb->BLOCK_HASH      = (unsigned int) plcb + G_LCB_HASH;
		// 15-07-2024 plcb->malloc_address  = plcb;

		// LCBs linked list for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
		plcb->LCB_NEXT_LCBs = g_kcb.KCB_ROOT_LCBs;
		if (g_kcb.KCB_ROOT_LCBs != (struct gs_lcb *) 0) g_kcb.KCB_ROOT_LCBs->LCB_PREV_LCBs = plcb;
		plcb->LCB_PREV_LCBs = (struct gs_lcb *) 0;
		g_kcb.KCB_ROOT_LCBs = (struct gs_lcb *) plcb;
		g_kcb.KCB_NUMBER_OF_LCBs++;
#endif
	}

	plcb->LCB_NextTCBRUNL   = (struct gs_tcb *) 0;        /* Pointer to the TCB list of running tasks */
	plcb->LCB_NextTCBRDYL   = (struct gs_tcb *) 0; 	   /* Pointer to the TCB of the Highest Priority Task */
	plcb->LCB_NextLCBFPL    = (struct gs_pcb *) 0;       /*Next free processor for this list */
	plcb->LCBState          = GS_LCBState_UNLINKED;
	plcb->LCBType           = lcbtype; // GS_LCBType_UNSPECIFIED;
	plcb->LCBExclusion      = (G_INT32) G_NUMBER_OF_PCB + 1;
	plcb->LCBCurrentRunning = (G_INT32) 0;

	// Link to the LCBL linked list
	gk_LCBL_Link((GS_LCB *) plcb);
    
    return (plcb);
}

/** gu_SchedulingListAssociateProcessor
 *  \brief The gu_SchedulingListAssociateProcessor function associates a system processor with a specified scheduling list. The priority is assigned to the association between the processor and the scheduling list. When tasks are ready to execute, the processor will select and execute the task from the highest priority scheduling list that it is associated with. The association with the lowest numerical value indicates the highest priority, ensuring that tasks in the most critical scheduling lists are prioritized for execution.
 *  \exception par_following The following parameters are required for the gu_SchedulingListAssociateProcessor function:
 *  \param [in] plcb A pointer to the GS_LCB structure representing the scheduling list to be associated with the processor.
 *  \param [in] CPUID The ID of the processor to be associated with the scheduling list.
 *  \param [in] priority The priority level for the association. A lower value indicates a higher priority, and the processor will first search the scheduling lists associated with the highest priority tasks that are ready to execute.
 *  \return The gu_SchedulingListAssociateProcessor function returns G_TRUE if the association is successful. It returns G_FALSE if the association fails.
 *  \sa gu_SchedulingListCreate, gu_SchedulingListExclusionSet, gu_SchedulingListAssociateTask
 *  \relates Scheduling_List
 *  \remark gu_list_associate_processor
 */ 
G_INT32 gu_SchedulingListAssociateProcessor(GS_LCB *plcb, G_INT32 CPUID, G_INT32 priority)
{
    GS_PCBAssocLCB *ppcbalcb;
    GS_PCBAssocLCB *ppcbalcb_aux;
    enum pcbstate pcb_state =  g_kcb.G_PCBTbl[CPUID-1].PCBState;

    if (G_Running == G_TRUE) {gm_GeMRTOSCriticalSectionEnter;}

	PRINT_ASSERT((gkm_LCB_IsValid((void *)plcb) == G_TRUE),"ERROR LCB is not valid\n");
	PRINT_ASSERT(((CPUID >= (int) 1) && (CPUID <= G_NUMBER_OF_PCB)),"ERROR Processor CPUID= %d\n",(int) CPUID);

	ppcbalcb = gk_Get_PCBAssocLCB();
	PRINT_ASSERT((gkm_PCBAssocLCB_IsValid((void *)ppcbalcb) == G_TRUE),"ERROR GS_PCBAssocLCB is not valid\n");
	
	ppcbalcb->PCB_RDY_priority     = (G_INT32) priority;
	ppcbalcb->PCB_RDY_LCBL     = (struct gs_lcb *) plcb;
	// 16-09-2024 ppcbalcb->gs_pcb_assoc_pcb = CPUID;

	if ((g_kcb.G_PCBTbl[CPUID-1].PCB_AssocLCB == (struct gs_pcb_rdy_lcbl *) 0) ||
		(g_kcb.G_PCBTbl[CPUID-1].PCB_AssocLCB->PCB_RDY_priority > priority))
	{
		if (pcb_state == GS_PCBState_FREE) {
			gk_LCBFPL_Unlink(CPUID);
		}
		ppcbalcb->gs_pcb_rdy_lcbl_next = (struct gs_pcb_rdy_lcbl *) g_kcb.G_PCBTbl[CPUID-1].PCB_AssocLCB;
		ppcbalcb->gs_pcb_rdy_lcbl_prev = (struct gs_pcb_rdy_lcbl *) 0;
		if (g_kcb.G_PCBTbl[CPUID-1].PCB_AssocLCB != (struct gs_pcb_rdy_lcbl *) 0) {
			g_kcb.G_PCBTbl[CPUID-1].PCB_AssocLCB->gs_pcb_rdy_lcbl_prev = ppcbalcb;
		}
		g_kcb.G_PCBTbl[CPUID-1].PCB_AssocLCB = ppcbalcb;
		
		if ((pcb_state != GS_PCBState_NOTRUNNING)) {
			gk_LCBFPL_Link(CPUID);
		}
		
	}
	else
	{
		ppcbalcb_aux = g_kcb.G_PCBTbl[CPUID-1].PCB_AssocLCB;
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
   
    if (G_Running == G_TRUE) {gm_GeMRTOSCriticalSectionExit;}
    
    return(G_TRUE);
}


/**gk_Get_PCBAssocLCB
 *  \brief Creates a PCBAssocLCB structure and returns its pointer
 *  \return Pointer to the PCBAssocLCB created
 *  \relates Scheduling_List
 *  \sa gu_SchedulingListAssociateProcessor, gu_SchedulingListCreate
 */
GS_PCBAssocLCB *gk_Get_PCBAssocLCB(void)
{
    GS_PCBAssocLCB *ppcbalcb; 
    
    // Check if free structure is available
    if (g_kcb.KCB_FREE_RDYs  != (struct gs_pcb_rdy_lcbl *) 0) {
        ppcbalcb = g_kcb.KCB_FREE_RDYs;
        g_kcb.KCB_FREE_RDYs = ppcbalcb->gs_pcb_rdy_lcbl_next;
    }
    else
    {        
		if ((ppcbalcb = (GS_PCBAssocLCB  *) malloc(sizeof(GS_PCBAssocLCB))) == 0) PRINT_FUNCTION;
        PRINT_ASSERT(((unsigned int) ppcbalcb % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) ppcbalcb != 0), "ERROR no memory available");
        
        ppcbalcb->BLOCK_HASH      = (unsigned int) ppcbalcb + G_PCBALCB_HASH;
        // 15-07-2024 ppcbalcb->malloc_address  = ppcbalcb;

        // LCBs linked list for debugging
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
        ppcbalcb->gs_pcb_lcbl_nexts = g_kcb.KCB_ROOT_RDYs;
        if (g_kcb.KCB_ROOT_RDYs != (struct gs_pcb_rdy_lcbl *) 0) g_kcb.KCB_ROOT_RDYs->gs_pcb_lcbl_prevs = ppcbalcb;
        ppcbalcb->gs_pcb_lcbl_prevs = (struct gs_pcb_rdy_lcbl *) 0;
        g_kcb.KCB_ROOT_RDYs = (struct gs_pcb_rdy_lcbl *) ppcbalcb;
		g_kcb.KCB_NUMBER_OF_PCBAssocLCBs++;
#endif
    }
    
    // Initialize to zero the structure fields
    ppcbalcb->gs_pcb_rdy_lcbl_next = (struct gs_pcb_rdy_lcbl *) 0;
    ppcbalcb->gs_pcb_rdy_lcbl_prev = (struct gs_pcb_rdy_lcbl *) 0;
    ppcbalcb->PCB_RDY_priority         = (G_INT32) 0;
    ppcbalcb->PCB_RDY_LCBL         = (struct gs_lcb *) 0;

    PRINT_ASSERT((gkm_PCBAssocLCB_IsValid((void *)ppcbalcb) == G_TRUE),"ERROR GS_PCBAssocLCB is not valid\n");

    return (ppcbalcb);
}

/**gk_Init_PCBs
 *  \brief Reserves system memory to store the Processor Control Blocks of the system (PCB)
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo System signal should be implmented when no free PCB is available
 *  \relates Processor
 */
G_INT32 gk_Init_PCBs(void)
{
    int i;
	GS_PCB *ppcb;

#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
    g_kcb.KCB_NUMBER_OF_PCBs = (G_INT32) G_NUMBER_OF_PCB;
#endif
    
	// Initialize Processor Structures creating IDLE TCBs
	for (i = 0;  i < G_NUMBER_OF_PCB;  i++) {

		ppcb = &g_kcb.G_PCBTbl[i];
		
        ppcb->PCB_AssocLCB  = (struct gs_pcb_rdy_lcbl *) 0;
		ppcb->PCBID         = (G_INT32) i+1;
		ppcb->BLOCK_HASH    = (unsigned int) ppcb + G_PCB_HASH;
		ppcb->PCBType       = (int) GS_PCBType_AVAILABLE; 
		ppcb->PCB_NextPCB   = (struct gs_pcb *) 0; 
		ppcb->PCB_PrevPCB   = (struct gs_pcb *) 0;
		ppcb->PCB_EXECTCB   = (struct gs_tcb *) 0;
        ppcb->PCBState      = GS_PCBState_NOTRUNNING;
	}
	
    // Assign the default list to the processor with the default priority
	for (i = 0;  i < G_NUMBER_OF_PCB;  i++) {		
        gu_SchedulingListAssociateProcessor((GS_LCB *) G_TASK_LCB_DEFAULT, (G_INT32) i+1, (G_INT32) G_TASK_LCB_DEFAULT_PRIORITY); 
        PRINT_ASSERT((g_kcb.G_PCBTbl[i].PCB_AssocLCB != (GS_PCBAssocLCB *) 0),"ERROR ppcb->PCB_AssocLCB\n"); 
	}
	
	PRINT_ASSERT((gkm_PCBAssocLCB_IsValid(g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_AssocLCB) == G_TRUE), "ERROR structure %p", g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_AssocLCB);	
	
	return (G_TRUE);
}

G_INT32 gk_Create_IDLE_tasks(void)
{
    unsigned int i;
	GS_PCB *ppcb;
	char task_name[G_TCB_DESCRIPTION_LENGTH];
	
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
		ppcb->PCB_IDLETCB->TCB_RDY_LCB_Index = (struct gs_lcb *) G_TASK_LCB_DEFAULT;
		ppcb->PCB_IDLETCB->TCB_NextTCB = (struct gs_tcb *) 0;
		ppcb->PCB_IDLETCB->TCB_PrevTCB = (struct gs_tcb *) 0;
		ppcb->PCB_IDLETCB->TCBState = G_TCBState_UNLINKED;
		ppcb->PCB_IDLETCB->TCB_AssocPCB = 0;

		snprintf(task_name, G_TCB_DESCRIPTION_LENGTH, "idle proc %u\n", i + 1);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
		strncpy((char *) ppcb->PCB_IDLETCB->TCB_description, (char *) task_name, G_TCB_DESCRIPTION_LENGTH-1);
#pragma GCC diagnostic push

		gk_TASK_STK_Init(ppcb->PCB_IDLETCB); 
	}

    return (G_TRUE);
}


OPTIMEZE_RESTORE

