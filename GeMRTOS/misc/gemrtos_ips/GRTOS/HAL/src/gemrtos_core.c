/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS core management functions
 *  \details This file contains the funtion for core management.
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

#include <gemrtos.h>

OPTIMEZE_CODE(3)

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
GS_TCB *gk_TCB_GetFree(void)
{
    GS_TCB *ptcb;
    void *mem;
    
    SAMPLE_FUNCTION_BEGIN(36)
    
    g_kcb.KCB_NUMBER_OF_TCBs++;
    
    /// Check if free structure is available
    if (g_kcb.KCB_FREE_TCBs  != (struct gs_tcb  *) 0) {
        ptcb = g_kcb.KCB_FREE_TCBs;
        g_kcb.KCB_FREE_TCBs = (struct gs_tcb *) ptcb->TCB_NextTCB;
    }
    else
    {
        // from https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library/3430102#3430102
        mem = malloc(sizeof(GS_TCB) + 15); // Adding 15 to make sure there exist an address module 16 to align the block
        ptcb = (GS_TCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
        /// INITIALIZE THE TCB STRUCTURE
        ptcb->BLOCK_HASH           = (unsigned int) ptcb + 1;        
        ptcb->malloc_address       = mem;
    }
 
    ptcb->TCB_NextTCBAEL       = (struct gs_ecb *) 0; 
    ptcb->TCB_NextTCBASL       = (struct gs_scb *) 0; 
    ptcb->TCB_NextTCBPSL       = (struct gs_scb *) 0; 
    ptcb->TCBType              = G_TCBType_UNDEFINED; 
    ptcb->TCBReadyPriority     = G_LOWEST_PRIORITY -1; 
    ptcb->TCBInherPriority     = G_LOWEST_PRIORITY -1; 
    ptcb->TCBRunPriority       = G_LOWEST_PRIORITY -1; 
    ptcb->TCBCurrentPriority   = G_LOWEST_PRIORITY -1; 
    ptcb->TCBPeriod            = 0x7FFFFFFFFFFFFFFFLL -1; 
    ptcb->TCBDeadline          = 0x7FFFFFFFFFFFFFFFLL -1; 
    ptcb->TCB_StackPointer     = (GS_STK *) 0; 
    ptcb->TCB_StackBottom      = (GS_STK *) 0; 
    ptcb->TCB_StackTop         = (GS_STK *) 0; 
    ptcb->TCB_TaskCode         = (void *) 0; 
    ptcb->TCB_TaskArg          = (void   *) 0; 
    ptcb->TCB_NextTCB          = (struct gs_tcb *) 0; 
    ptcb->TCB_PrevTCB          = (struct gs_tcb *) 0; 
    ptcb->TCB_AssocPCB         = 0; 
    ptcb->TCB_INTNumber        = -1; 
    ptcb->TCB_RDY_LCB_Index    = (GS_LCB *) G_TASK_LCB_DEFAULT; 
    ptcb->TCB_Abort_w_Deadline = G_FALSE;
    ptcb->TCBState             = G_TCBState_UNLINKED;
    ptcb->TCB_NextISRTCB       = (struct gs_tcb *) 0;
    ptcb->TCB_PrevISRTCB       = (struct gs_tcb *) 0;
    
    
    /// TCBs linked list for debugging
    ptcb->TCB_NEXT_TCBs = g_kcb.KCB_ROOT_TCBs;
    if (g_kcb.KCB_ROOT_TCBs != (struct gs_tcb *) 0) g_kcb.KCB_ROOT_TCBs->TCB_PREV_TCBs = ptcb;
    ptcb->TCB_PREV_TCBs = (struct gs_tcb *) 0;
    g_kcb.KCB_ROOT_TCBs = (struct gs_tcb *) ptcb;

    PRINT_ASSERT((TCB_IsValid(ptcb) == G_TRUE),"ERROR G_Running= %d\n",G_Running);

    SAMPLE_FUNCTION_END(36)
    return ptcb;
}


/**gk_ECB_GetFree
 *  \brief  Returns a pointer to a Free ECB, NULL if there is not ECB available
  * \return pointer to a Free ECB, NULL if there is not ECB available
 *  \todo System signal should be implmented when no free ECB is available
 *  \relates Event
 */ 
GS_ECB *gk_ECB_GetFree(void)
{
    GS_ECB  *pecb;
    void *mem;
    
    SAMPLE_FUNCTION_BEGIN(1)
    
    g_kcb.KCB_NUMBER_OF_ECBs++;;
    
    /// Check if free structure is available
    if (g_kcb.KCB_FREE_ECBs  != (struct gs_ecb  *) 0) {
        pecb = g_kcb.KCB_FREE_ECBs;
        g_kcb.KCB_FREE_ECBs = pecb->ECB_NextECB;
    }
    else
    {    
        // from https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library/3430102#3430102    
        mem = malloc(sizeof(GS_ECB) + 15); // Adding 15 to make sure there exist an address module 16 to align the block
        pecb = (GS_ECB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
        pecb->BLOCK_HASH     = (unsigned int) pecb + 2;
        pecb->malloc_address = mem;          
    }

  
    // pecb->ECBState       = GS_ECBState_FREE; 
    pecb->ECB_NextECBAEL = (struct gs_ecb *) 0; 
    pecb->ECB_NextECBASL = (struct gs_scb *) 0; 
    pecb->ECBType        = G_ECBType_NOT_SPECIFIED; 
    // pecb->ECBValue.i64   = G_LOWEST_PRIORITY -1; 
    pecb->ECB_NextECB    = (struct gs_ecb *) 0; 
    pecb->ECB_PrevECB    = (struct gs_ecb *) 0; 
    pecb->ECB_AssocTCB   = (struct gs_tcb *) 0; 
    pecb->ECB_AssocRCB   = (struct g_rcb *) 0; 
    pecb->ECB_NextTCBAEL = (struct gs_ecb *) 0; 
    pecb->ECB_PrevTCBAEL = (struct gs_ecb *) 0; 
    pecb->ECB_RRDS       = (struct gs_rrds *) 0;
    pecb->ECBValue.i64   = G_LOWEST_PRIORITY;
    pecb->ECBState       = GS_ECBState_UNLINKED;
    
    /// TCBs linked list for debugging
    pecb->ECB_NEXT_ECBs = g_kcb.KCB_ROOT_ECBs;
    if (g_kcb.KCB_ROOT_ECBs != (struct gs_ecb *) 0) g_kcb.KCB_ROOT_ECBs->ECB_PREV_ECBs = pecb;
    pecb->ECB_PREV_ECBs = (struct gs_ecb *) 0;
    g_kcb.KCB_ROOT_ECBs = (struct gs_ecb *) pecb;    
    
    SAMPLE_FUNCTION_END(1)

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (pecb == (struct gs_ecb *) 0) G_DEBUG_WHILEFOREVER;
#endif
    
    return pecb;
}  

/**gk_RCB_GetFree
 *  \brief 
 *  Unlinks an RCB from the RCBFL list and returns its pointer or NULL if no free RCB is available
 *  \return Pointer to the RCB or NULL when no RCB available
 *  \todo ystem signal should be implmented when no free RCB is available
 *  \relates Resource
 */
G_RCB *gk_RCB_GetFree(void)
{
    SAMPLE_FUNCTION_BEGIN(19)
    G_RCB  *prcb;
    void *mem;
    
    g_kcb.KCB_NUMBER_OF_RCBs++;
    
    /// Check if free structure is available
    if (g_kcb.KCB_FREE_RCBs  != (struct g_rcb  *) 0) {
        prcb = (G_RCB  *) g_kcb.KCB_FREE_RCBs;
        g_kcb.KCB_FREE_RCBs = prcb->RCB_NextRCB;
    }
    else
    {      
        // from https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library/3430102#3430102    
        mem = malloc(sizeof(G_RCB) + 15); // Adding 15 to make sure there exist an address module 16 to align the block
        prcb = (G_RCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
        prcb->BLOCK_HASH     = (unsigned int) prcb + 3;
        prcb->malloc_address = mem;         
    }

 
    // prcb->RCBType        = GK_RCBType_FREE; 
    prcb->RCBState       = GK_RCBState_UNDEFINED; 
    prcb->RCBCount       = (INT32) 0; 
    prcb->RCB_NextRCBASL = (struct gs_scb *) 0; 
    prcb->RCB_NextRCBGEL = (struct gs_ecb *) 0; 
    prcb->RCB_NextRCBWEL = (struct gs_ecb *) 0;
    prcb->RCB_NextRCB    = (struct g_rcb *) 0;
    prcb->RCBType = GK_RCBType_UNUSED;
    
    /// TCBs linked list for debugging
    prcb->RCB_NEXT_RCBs = (G_RCB *) g_kcb.KCB_ROOT_RCBs;
    if (g_kcb.KCB_ROOT_RCBs != (struct g_rcb *) 0) {
        G_RCB *presource = (G_RCB *) g_kcb.KCB_ROOT_RCBs;
        presource->RCB_PREV_RCBs = (G_RCB *) prcb;
    }
    prcb->RCB_PREV_RCBs = (G_RCB *) 0;
    g_kcb.KCB_ROOT_RCBs = (struct g_rcb *) prcb;    
    
    SAMPLE_FUNCTION_END(19)

    return prcb;
}

/**gk_SCB_GetFree
 *  \brief 
 *  Unlinks a SCB from free list and return its pointer
 *  \return Pointer to the SCB or NULL if no free SCB is available 
 *  \relates Signal
 */
GS_SCB *gk_SCB_GetFree(void)
{
    void *mem;
    GS_SCB *pscb;
    
    SAMPLE_FUNCTION_BEGIN(26)
    
    g_kcb.KCB_NUMBER_OF_SCBs++;
    
    /// Check if free structure is available
    if (g_kcb.KCB_FREE_SCBs  != (struct gs_scb  *) 0) {
        pscb = g_kcb.KCB_FREE_SCBs;
        g_kcb.KCB_FREE_SCBs = pscb->SCB_NextSCB;
    }
    else
    {
        mem = malloc(sizeof(GS_SCB) + 15); // Adding 15 to make sure there exist an address module 16 to align the block
        pscb = (GS_SCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
        pscb->BLOCK_HASH     = (unsigned int) pscb + 4;
        pscb->malloc_address = mem;  
    }
        
    pscb->SCBState       = G_SCBState_FREE;
    pscb->SCBState       = G_SCBState_UNLINKED;
    pscb->SCB_NextSCB    = (struct gs_scb  *) 0;
    
    /// TCBs linked list for debugging
    pscb->SCB_NEXT_SCBs = g_kcb.KCB_ROOT_SCBs;
    if (g_kcb.KCB_ROOT_SCBs != (struct gs_scb *) 0) g_kcb.KCB_ROOT_SCBs->SCB_PREV_SCBs = pscb;
    pscb->SCB_PREV_SCBs = (struct gs_scb *) 0;
    g_kcb.KCB_ROOT_SCBs = (struct gs_scb *) pscb;

    
    SAMPLE_FUNCTION_END(26)
    
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (pscb == (struct gs_scb *) 0) G_DEBUG_WHILEFOREVER;
#endif    
    
    return pscb;
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
    void *mem;
    GS_RRDS  *prrds;
    
    SAMPLE_FUNCTION_BEGIN(48)
    
    g_kcb.KCB_NUMBER_OF_RRDSs++;
    
    /// Check if free structure is available
    if (g_kcb.KCB_FREE_RRDSs  != (struct gs_rrds  *) 0) {
        prrds = g_kcb.KCB_FREE_RRDSs;
        g_kcb.KCB_FREE_RRDSs = prrds->RRDS_NextRRDS;
    }
    else
    {    
        mem = malloc(sizeof(GS_RRDS) + 15); // Adding 15 to make sure there exist an address module 16 to align the block
        prrds = (GS_RRDS  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
        prrds->BLOCK_HASH      = (unsigned int) prrds + 5;
        prrds->malloc_address  = mem;
    }
    
    prrds->RRDS_State = 0;           ///< \todo verify correctness !!!!!
    prrds->RRDS_Type = 0;            ///< \todo verify correctness !!!!!
    prrds->RRDS_NextRRDS = (struct gs_rrds *) 0;
    prrds->RRDS_NextSCB  = (struct gs_scb  *) 0;
    prrds->RRDS_AsocECB  = (struct gs_ecb  *) 0;    
    
    /// TCBs linked list for debugging
    prrds->RRDS_NEXT_RRDSs = g_kcb.KCB_ROOT_RRDSs;
    if (g_kcb.KCB_ROOT_RRDSs != (struct gs_rrds *) 0) g_kcb.KCB_ROOT_RRDSs->RRDS_PREV_RRDSs = prrds;
    prrds->RRDS_PREV_RRDSs = (struct gs_rrds *) 0;
    g_kcb.KCB_ROOT_RRDSs = (struct gs_rrds *) prrds;    
    
    SAMPLE_FUNCTION_END(48)
    return prrds;
}

/**gk_Get_LCB
 *  \brief Creates a LCB and returns its pointer
 *  \return Ponter to the LCB created
 *  \todo System signal should be implmented when no free LCB is available
 *  \todo Rewrite all the LCB support
 *  \todo Write the destroy LCB structure
 *  \relates Core
 */
GS_LCB *gk_Get_LCB(INT32 lcbtype)
{
    void *mem;
    GS_LCB *plcb; 
    
    g_kcb.KCB_NUMBER_OF_LCBs++;
    
    /// Check if free structure is available
    if (g_kcb.KCB_FREE_LCBs  != (struct gs_lcb  *) 0) {
        plcb = g_kcb.KCB_FREE_LCBs;
        g_kcb.KCB_FREE_LCBs = plcb->LCB_NextLCBL;
    }
    else
    {        
        mem = malloc(sizeof(GS_LCB) + 15); // Adding 15 to make sure there exist an address module 16 to align the block
        plcb = (GS_LCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);    
        plcb->BLOCK_HASH      = (unsigned int) plcb + 6;
        plcb->malloc_address  = mem;
    }
    
    plcb->LCB_NextTCBRUNL   = (struct gs_tcb *) 0;        /* Pointer to the TCB list of running tasks */
    plcb->LCBRunPriority    = (INT64) G_LOWEST_PRIORITY;  /* It is because it is empty*/
    plcb->LCB_NextTCBRDYL   = (struct gs_tcb *) 0; 	   /* Pointer to the TCB of the Highest Priority Task */
    plcb->LCB_NextLCBFPL    = (struct gs_pcb *) 0;       /*!< Next free processor for this list */
    plcb->LCBState          = GS_LCBState_UNLINKED;
    plcb->LCBType           = lcbtype; // GS_LCBType_UNSPECIFIED;
    plcb->LCBExclusion      = (INT32) G_NUMBER_OF_PCB + 1;
    plcb->LCBCurrentRunning = (INT32) 0;


    /// LCBs linked list for debugging
    plcb->LCB_NEXT_LCBs = g_kcb.KCB_ROOT_LCBs;
    if (g_kcb.KCB_ROOT_LCBs != (struct gs_lcb *) 0) g_kcb.KCB_ROOT_LCBs->LCB_PREV_LCBs = plcb;
    plcb->LCB_PREV_LCBs = (struct gs_lcb *) 0;
    g_kcb.KCB_ROOT_LCBs = (struct gs_lcb *) plcb;

    
    // Link to the LCBL linked list
    gk_LCBL_Link((GS_LCB *) plcb);

    return plcb;
}

/**gk_Get_PCBAssocLCB
 *  \brief Creates a PCBAssocLCB structure and returns its pointer
 *  \return Pointer to the PCBAssocLCB created
 *  \relates Core
 */
GS_PCBAssocLCB *gk_Get_PCBAssocLCB(void)
{
    void *mem;
    GS_PCBAssocLCB *ppcbalcb; 
    
    g_kcb.KCB_NUMBER_OF_PCBAssocLCBs++;
    
    /// Check if free structure is available
    if (g_kcb.KCB_FREE_RDYs  != (struct gs_pcb_rdy_lcbl *) 0) {
        ppcbalcb = g_kcb.KCB_FREE_RDYs;
        g_kcb.KCB_FREE_RDYs = ppcbalcb->gs_pcb_rdy_lcbl_next;
    }
    else
    {        
        mem = malloc(sizeof(GS_PCBAssocLCB) + 15); // Adding 15 to make sure there exist an address module 16 to align the block
        ppcbalcb = (GS_PCBAssocLCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);    
        ppcbalcb->BLOCK_HASH      = (unsigned int) ppcbalcb + 7;
        ppcbalcb->malloc_address  = mem;
    }
    
    // Initialize to zero the structure fields
    ppcbalcb->gs_pcb_rdy_lcbl_next = (struct gs_pcb_rdy_lcbl *) 0;
    ppcbalcb->gs_pcb_rdy_lcbl_prev = (struct gs_pcb_rdy_lcbl *) 0;
    ppcbalcb->priority             = (INT32) 0;
    ppcbalcb->PCB_RDY_LCBL         = (struct gs_lcb *) 0;

    /// LCBs linked list for debugging
    ppcbalcb->gs_pcb_lcbl_nexts = g_kcb.KCB_ROOT_RDYs;
    if (g_kcb.KCB_ROOT_RDYs != (struct gs_pcb_rdy_lcbl *) 0) g_kcb.KCB_ROOT_RDYs->gs_pcb_lcbl_prevs = ppcbalcb;
    ppcbalcb->gs_pcb_lcbl_prevs = (struct gs_pcb_rdy_lcbl *) 0;
    g_kcb.KCB_ROOT_RDYs = (struct gs_pcb_rdy_lcbl *) ppcbalcb;


    return ppcbalcb;
}



/**gk_Create_PCBs
 *  \brief Reservs system memory to store the Processor Control Blocks of the system (PCB)
 *  \param [in] Nmbr_TCB Number of user PCBs desired to create (Interrupt and system PCB are added by the function)
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo System signal should be implmented when no free PCB is available
 *  \relates Core
 */
INT32 gk_Create_PCBs(int Nmbr_PCB)
{
    g_kcb.KCB_NUMBER_OF_PCBs = Nmbr_PCB;
    // from https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library/3430102#3430102
    void *mem = malloc((Nmbr_PCB) * sizeof(GS_PCB) + 15); // Adding 15 to make sure there exist an address module 16 to align the block
    g_kcb.G_PCBTbl = (GS_PCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
#if G_DEBUG_WHILEFOREVER_ENABLE == 1    
    fprintf(stderr,"[ MESSAGE ] mem = %p, g_kcb.G_PCBTbl = %p, size = %d\n", (void *) mem, (void *) g_kcb.G_PCBTbl, (int) G_NUMBER_OF_PCB * sizeof(GS_PCB));
#endif
    return G_TRUE;
}

/**gk_CreateTask
 *  \brief 
 *  Kernel Function: creates a new task initializing TCB features
 *  \param [in] TaskCode             Pointer to the beginning of the task code
 *  \param [in] p_arg                Pointer to the argument of the first call
 *  \param [in] StkBotton            Botton of the Stack of the Task
 *  \param [in] StkSize              Size of the stack in words
 *  \param [in] TCBType              Type of the Task
 *  \param [in] TCBReadyPriority     Initial Priority of the task
 *  \param [in] TCBRunPriority       Priority of the task when it is executing
 *  \param [in] TCBDeadline          Deadline of the task
 *  \param [in] TCBPeriod            Period of the task
 *  \param [in] TCB_RDY_LCB_Index    Ready list that should be inserted
 *  \param [in] TCB_Abort_w_Deadline True when task is aborted when deadline
 *  \param [in] TCBInherPriority     Inhereted priority
 *  \param [in] TCB_INTNumber        Interrupt Number if task is ISR
 *  \return Pointer to the TCB
 *  \relates Task
 */
GS_TCB *gk_CreateTask(void *TaskCode,              
                     void *p_arg,                  
                     void *StkBotton,              
                     unsigned int StkSize,         
                     unsigned int TCBType,         
                     INT64 TCBReadyPriority,       
                     INT64 TCBRunPriority,         
                     INT64 TCBDeadline,            
                     INT64 TCBPeriod,              
                     GS_LCB *TCB_RDY_LCB_Index,        
                     int TCB_Abort_w_Deadline,     
                     int TCB_INTNumber)       
{
    GS_TCB  *ptcb;
    
    ptcb = gk_TCB_GetFree();

    // Initialize the gs_tcb structure of the task
    if (ptcb != (struct gs_tcb *) 0) {
        ptcb->TCB_StackBottom      = (GS_STK *) ((((INT32) StkBotton)-8)& ~0x3); ///< Botton of the Stack of the Task
        ptcb->TCB_StackTop         = (GS_STK *)((INT32) ptcb->TCB_StackBottom - ((INT32)(StkSize)& ~0x3)); ///< Top of the Stack of the task
        ptcb->TCB_TaskCode         = TaskCode;           ///< Pointer to tha begining of the task code
        ptcb->TCB_TaskArg          = p_arg;              ///< Pointer to the argument of the first call

        ptcb->TCBType              = TCBType;      ///< Type of the Task    
        ptcb->TCBReadyPriority     = TCBReadyPriority;  ///< Initial Priority of the task
        ptcb->TCBRunPriority       = TCBRunPriority;  ///< Priority of the task when it is executing
        ptcb->TCBDeadline          = TCBDeadline;    ///< Deadline of the task
        ptcb->TCBPeriod            = TCBPeriod;    ///< Period of the task
        ptcb->TCB_RDY_LCB_Index    = TCB_RDY_LCB_Index;       /*!< number of the ready list that should be inserted */
        ptcb->TCB_Abort_w_Deadline = TCB_Abort_w_Deadline;   /*!< True when task is aborted when deadline, false otherwise */
        ptcb->TCBInherPriority     = TCBRunPriority;  /* Load task priority into TCB              */
        
        // Initilize the stack of the task
        gk_TASK_STK_Init(ptcb);
        
        // Set the number of associated interrupt (valid only when TCBType == G_TCBType_ISR)
        // and link to the ISR linked list
        ptcb->TCB_INTNumber = TCB_INTNumber;
        ptcb->TCB_PrevISRTCB = (struct gs_tcb *) 0;
        if (TCBType == G_TCBType_ISR) {
            ptcb->TCB_NextISRTCB = g_kcb.KCB_ExtISR[TCB_INTNumber].G_TCB_ISR;
            if (g_kcb.KCB_ExtISR[TCB_INTNumber].G_TCB_ISR != (struct gs_tcb *) 0) {
                g_kcb.KCB_ExtISR[TCB_INTNumber].G_TCB_ISR->TCB_PrevISRTCB = (struct gs_tcb *) ptcb;
            }
            g_kcb.KCB_ExtISR[TCB_INTNumber].G_TCB_ISR = (struct gs_tcb *) ptcb;
        } else {
            ptcb->TCB_NextISRTCB = (struct gs_tcb *) 0;
        }
    }    

	return (ptcb);
}


/**gk_INIT_IRQ
 *  \brief 
 *  Initializes the ISRs according the ISRs set by Nios HAL. It creates a TCB for each
 *  ISR and link it to the g_kcb.KCB_ExtISR[i].G_TCB_ISR accordingly.
 *  \details 
 *  It is called from gk_INIT_KERNEL()
 *  \todo Include Disabled but ready interrupt Type
 *  \todo Utilize the gk_SetTaskISR in order to be coherent if the user define a ISR in the main function
 *  \relates Interrupt
 */ 
void  gk_INIT_IRQ (void)
{
	int i; PRINT_DEBUG_LINE
	G_DEBUG_VERBOSE; PRINT_DEBUG_LINE // Print Debug information
	for (i=0 ;  i<ALT_NIRQ ;  i++)
	{
		///Create ISR TCB if IRQ is enabled
        if ((G_IRQ_ENABLED >> i) & 1) {
            
            printf("Redirecting IRQ = %d, handler in %p\n", (int) i, (void *) alt_irq[i].handler);
            
            g_kcb.KCB_ExtISR[i].G_TCB_ISR = gk_CreateTask((void *) alt_irq[i].handler,  ///< Pointer to the beginning of isr
                                 (void *) alt_irq[i].context,               ///< Pointer to the argument of the first call
                                 // (void *) &g_kcb.G_ISR_STACK[i][G_ISR_STACKSIZE-4],              ///< Botton of the Stack of the Task
                                 (void *) g_kcb.G_ISR_STACK + (i * G_ISR_STACKSIZE) + G_ISR_STACKSIZE-4,              ///< Botton of the Stack of the Task
                                 (unsigned int) G_ISR_STACKSIZE,         ///< Size of the stack in words
                                 (unsigned int) G_TCBType_ISR,         ///< Type of the Task
                                 (INT64) G_ISR_PRIORITY,       ///< Initial Priority of the task
                                 (INT64) G_ISR_PRIORITY,         ///< Priority of the task when it is executing
                                 (INT64) G_TASK_PERIOD_DEFAULT,            ///< Deadline of the task
                                 (INT64) G_TASK_PERIOD_DEFAULT,              ///< Period of the task
                                 (GS_LCB *) G_TASK_LCB_DEFAULT,        /*!< pointer of the ready list that should be inserted */
                                 (int) G_TRUE,     /*!< True when task is aborted when deadline */
                                 (int) i);  
                                     
			// g_kcb.KCB_ExtISR[i].G_TCB_ISR->TCB_INTNumber = i; 
            
            if ((G_IRQ_PENDING >> i) & 1) {                
				/* The interrupt was pending then insert in ready list and disable it */
				GRTOS_CMD_IRQ_ENB_CLR(i); 
                g_kcb.KCB_ExtISR[i].G_EXT_INT_Count++; // = (int) 1;
				gk_TCBRDYL_Link(g_kcb.KCB_ExtISR[i].G_TCB_ISR);   /* Insert Task in Ready List                */
			} else {
                gk_TCBWL_Link(g_kcb.KCB_ExtISR[i].G_TCB_ISR, G_TCBState_WAITING_COMPLETED); 
                // g_kcb.KCB_ExtISR[i].G_EXT_INT_Count = (int) 0;
				GRTOS_CMD_IRQ_ENB_SET(i); 
			}
		}
	}    
}

/**gk_ISR_COMPLETE
 *  \brief 
 *  Executed when Task finishes as ISR. If remains ready, then it will be not released next Interrupt event
 *  \param [in] ptcb Pointer to the TCB of the ISR task
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Include completes but task remains ready.
 *  \relates Interrupt
 */
INT32 gk_ISR_COMPLETE (GS_TCB *ptcb) {
    
    int irq_nbr = ptcb->TCB_INTNumber;
    
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	if (g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count == (int) 0) G_DEBUG_WHILEFOREVER; PRINT_DEBUG_LINE
#endif    
    
    if (ptcb->TCBType == G_TCBType_ISR) g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count--;
    if (g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count == (INT32) 0) {
        GRTOS_CMD_IRQ_ENB_SET(irq_nbr);
    }
    
    return G_TRUE;
}

/**gk_ISR_RELEASE
 *  \brief 
 *  Executed when an interrupt is triggered. It is called from gk_ENTRY_IRQ_HANDLER()
 *  \param [in] irq_nbr Number index of the interrupt
 *  \todo Verify if task is waiting to enable the interrupt, otherwise change the Type of the ISR
 *  \relates Interrupt
 */
void gk_ISR_RELEASE (int irq_nbr) {

    GS_TCB *ptcb = g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR;
    
    GRTOS_CMD_IRQ_ENB_CLR(irq_nbr); PRINT_DEBUG_LINE
    
    while (ptcb != (GS_TCB *) 0) {
        // !!!! verificar si esta en estado waiting
        /* Limpio el registro de interrupcion porque ya fue activada  */
        if (ptcb->TCBType == G_TCBType_ISR) {
            if (ptcb->TCBState == G_TCBState_WAITING_COMPLETED) {     // if task is waiting
                /* Insert Task in Ready List                */                
                gk_TCB_Unlink((GS_TCB *) ptcb); 
                gk_TASK_STK_Init((GS_TCB *) ptcb); 
                gk_TCBRDYL_Link((GS_TCB *) ptcb); 
                
                g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count++;
            }
        }
        ptcb = (GS_TCB *) ptcb->TCB_NextISRTCB;
    }

}

/**gu_SetTaskISR
 *  \brief 
 *  Convert a task to a ISR associated to a Interrupt Number
 *  \param [in] ptcb    Pointer to the TCB of the task to be ISR
 *  \param [in] irq_nbr Number of the IRQ to be associated to.
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if IRQ number or TCB pointer al valid.
 *  \todo Implement the reverse function to convert a ISR into a task
 *  \relates Interrupt
 */
INT32 gu_SetTaskISR(struct gs_tcb *ptcb, unsigned int irq_nbr)
{
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_GET;
        gk_SetTaskISR(ptcb, irq_nbr);
    if (G_Running == G_TRUE) GRTOS_USER_CRITICAL_SECTION_RELEASE; 
    return G_TRUE;
}

/**gk_SetTaskISR
 *  \brief 
 *  Convert a task to a ISR associated to a Interrupt Number in Kernal mode
 *  \param [in] ptcb    Pointer to the TCB of the task to be ISR
 *  \param [in] irq_nbr Number of the IRQ to be associated to.
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Check if IRQ number or TCB pointer al valid.
 *  \todo Implement the reverse function to convert a ISR into a task
 *  \relates Interrupt
 */
INT32 gk_SetTaskISR(struct gs_tcb *ptcb, unsigned int irq_nbr)
{
    // Set the number of associated interrupt (valid only when TCBType == G_TCBType_ISR)
    // and link to the ISR linked list !!! check if task is running for consistency of Count
    ptcb->TCBType = G_TCBType_ISR;
    ptcb->TCB_INTNumber = irq_nbr;
    ptcb->TCB_PrevISRTCB = (struct gs_tcb *) 0;
    ptcb->TCB_NextISRTCB = g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR;
    if (g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR != (struct gs_tcb *) 0) {
        g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR->TCB_PrevISRTCB = (struct gs_tcb *) ptcb;
    }
    g_kcb.KCB_ExtISR[irq_nbr].G_TCB_ISR = (struct gs_tcb *) ptcb;
    
    if (ptcb->TCBState != G_TCBState_WAITING_COMPLETED) {
        g_kcb.KCB_ExtISR[irq_nbr].G_EXT_INT_Count++;
    }
    
    return G_TRUE;
}


OPTIMEZE_RESTORE

