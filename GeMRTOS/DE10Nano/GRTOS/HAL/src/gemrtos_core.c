/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS core management functions
 *  \details This file contains the funtion for core management.
 *  \version 0.9a
 *  \date      2015-2020
 *  \bug  Describe found bugs here.
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
#include <gemrtos_core.h>

OPTIMEZE_CODE(3)

// https://gcc.gnu.org/onlinedocs/gcc/Nios-II-Options.html#index-mno-bypass-cache
// #pragma GCC optimize('mno-cache-volatile')

#include <inttypes.h>


GS_TCB *gk_Get_TCB(void)
{
    g_kcb.KCB_NUMBER_OF_TCBs++;
    // from https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library/3430102#3430102
    void *mem = malloc(sizeof(GS_TCB) + 15);
    GS_TCB *ptcb = (GS_TCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
 
    /// INITIALIZE THE TCB STRUCTURE
    ptcb->BLOCK_HASH           = (unsigned int) ptcb + 1;
    ptcb->malloc_address       = mem;
    ptcb->TCBState             = G_TASK_STATE_FREE; 
    ptcb->TCB_NextTCBAEL       = (struct gs_ecb *) 0; 
    ptcb->TCB_NextTCBASL       = (struct gs_scb *) 0; 
    ptcb->TCB_NextTCBPSL       = (struct gs_scb *) 0; 
    ptcb->TCBType              = G_TASK_TYPE_UNDEFINED; 
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
    ptcb->TCB_NextTCBAEL       = (struct gs_ecb *) 0; 
    ptcb->TCB_AssocPCB         = 0; 
    ptcb->TCB_INTNumber        = -1; 
    ptcb->TCB_RDY_LCB_Index    = (GS_LCB *) G_TASK_LCB_DEFAULT; 
    ptcb->TCB_Abort_w_Deadline = G_FALSE;         

    return ptcb;
}

GS_ECB * gk_Get_ECB(void)
{
    g_kcb.KCB_NUMBER_OF_ECBs++;;
    // from https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library/3430102#3430102    
    void *mem = malloc(sizeof(GS_ECB) + 15);
    GS_ECB  *pecb = (GS_ECB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);

    pecb->BLOCK_HASH     = (unsigned int) pecb + 2;
    pecb->malloc_address = mem;    
    pecb->ECBState       = GS_ECB_STATE_FREE; 
    pecb->ECB_NextECBAEL = (struct gs_ecb *) 0; 
    pecb->ECB_NextECBASL = (struct gs_scb *) 0; 
    pecb->ECBType        = G_ECB_TYPE_NOT_SPECIFIED; 
    pecb->ECBValue.i64   = G_LOWEST_PRIORITY -1; 
    pecb->ECB_NextECB    = (struct gs_ecb *) 0; 
    pecb->ECB_PrevECB    = (struct gs_ecb *) 0; 
    pecb->ECB_AssocTCB   = (struct gs_tcb *) 0; 
    pecb->ECB_AssocRCB   = (struct g_rcb *) 0; 
    pecb->ECB_NextTCBAEL = (struct gs_ecb *) 0; 
    pecb->ECB_PrevTCBAEL = (struct gs_ecb *) 0; 
    pecb->ECB_RRDS       = (struct gs_rrds *) 0;     
    
    return pecb;
}  

G_RCB *gk_Get_RCB(void)
{
    g_kcb.KCB_NUMBER_OF_RCBs++;;
    void *mem = malloc(sizeof(G_RCB) + 15);
    G_RCB  *prcb = (G_RCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
    // from https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library/3430102#3430102    

    prcb->BLOCK_HASH     = (unsigned int) prcb + 3;
    prcb->malloc_address = mem;  
    prcb->RCBType        = GK_RCB_TYPE_FREE; 
    prcb->RCBState       = GK_RCB_STATE_UNDEFINED; 
    prcb->RCBCount       = (INT32) 0; 
    prcb->RCB_NextRCBASL = (struct gs_scb *) 0; 
    prcb->RCB_NextRCBGEL = (struct gs_ecb *) 0; 
    prcb->RCB_NextRCBWEL = (struct gs_ecb *) 0;

    return prcb;
}


GS_SCB *gk_Get_SCB(void)
{
    g_kcb.KCB_NUMBER_OF_SCBs++;
    
    void *mem = malloc(sizeof(GS_SCB) + 15);
    GS_SCB *pscb = (GS_SCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
    
    pscb->BLOCK_HASH     = (unsigned int) pscb + 4;
    pscb->malloc_address = mem;  
    pscb->SCBState       = G_SCB_STATE_FREE;

    return pscb;
}    
    

GS_RRDS *gk_Get_RRDS(void)
{
    g_kcb.KCB_NUMBER_OF_RRDSs++;
    void *mem = malloc(sizeof(GS_RRDS) + 15);
    GS_RRDS  *prrds = (GS_RRDS  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
    
    prrds->BLOCK_HASH      = (unsigned int) prrds + 5;
    prrds->malloc_address  = mem;
    
    prrds->RRDS_State = 0;           ///< \todo verify correctness !!!!!
    prrds->RRDS_Type = 0;            ///< \todo verify correctness !!!!!
    prrds->RRDS_NextRRDS = (struct gs_rrds *) 0;
    prrds->RRDS_NextSCB  = (struct gs_scb  *) 0;
    prrds->RRDS_AsocECB  = (struct gs_ecb  *) 0;    
    
    return prrds;
}

GS_LCB *gk_Get_LCB(void)
{
    g_kcb.KCB_NUMBER_OF_LCBs++;
    void *mem = malloc(sizeof(GS_LCB) + 15);
    GS_LCB *plcb = (GS_LCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);    

    plcb->BLOCK_HASH      = (unsigned int) plcb + 6;
    plcb->malloc_address  = mem;    
    
    plcb->LCB_NextTCBRUNL = (struct gs_tcb *) 0;    /* Pointer to the TCB list of running tasks */
    plcb->LCBRunPriority  = (INT64) G_LOWEST_PRIORITY;  /* It is because it is empty*/
    plcb->LCB_NextTCBRDYL = (struct gs_tcb *) 0; 	  /* Pointer to the TCB of the Highest Priority Task */
    plcb->LCB_NextLCBFPL  = (struct gs_pcb *) 0;  /*!< Next free processor for this list */
    plcb->LCBState        = GS_LCB_STATE_UNLINKED;     

    return plcb;
}




/**gk_Create_PCBs
 *  \brief Reservs system memory to store the Processor Control Blocks of the system (PCB)
 *  \param [in] Nmbr_TCB Number of user PCBs desired to create (Interrupt and system PCB are added by the function)
 *  \return G_TRUE when successful, G_FALSE otherwise
 *  \todo Include into the KCB
 *  \relates Core
 */
INT32 gk_Create_PCBs(int Nmbr_PCB)
{
    g_kcb.KCB_NUMBER_OF_PCBs = Nmbr_PCB;
    // from https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library/3430102#3430102
    void *mem = malloc((Nmbr_PCB) * sizeof(GS_PCB) + 15);
    g_kcb.G_PCBTbl = (GS_PCB  *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
#if G_DEBUG_WHILEFOREVER_ENABLE == 1    
    fprintf(stderr,"[ MESSAGE ] mem = %p, g_kcb.G_PCBTbl = %p, size = %d\n", (void *) mem, (void *) g_kcb.G_PCBTbl, (int) G_NUMBER_OF_PCB * sizeof(GS_PCB));
#endif
    return G_TRUE;
}




OPTIMEZE_RESTORE

