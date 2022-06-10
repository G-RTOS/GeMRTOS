/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Initialization for ISR functions
 *  \details This file contains the Initialization for ISR functions.
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

OPTIMEZE_CODE(3)

/**GRTOS_CMD_HLT_ACT_CNT_GET
 *  \brief Return the idle time of the current processor
 *  \return Idle time of the processor in INT64
 *  \todo Verify correctness
 *  \relates Time
 */
INT64 GRTOS_CMD_HLT_ACT_CNT_GET()
{
    TIMEPRIORITY idle_time;

    GRTOS_USER_CRITICAL_SECTION_GET;
    // fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "In %s, %d \n", __FUNCTION__, __LINE__);
	idle_time.i32[1] = (int) IORD_GRTOS_HLT_ACT_CNT; PRINT_DEBUG_LINE
	idle_time.i32[0] = (int) IORD_GRTOS_HLT_ACT_CNT_SMP; PRINT_DEBUG_LINE

    GRTOS_USER_CRITICAL_SECTION_RELEASE; 
    return idle_time.i64;
}

/**GRTOS_CMD_TM_CNT_GET
 *  \brief Read the GRTOS System Time Register
 *  \param [in] ptime Pointer to an array of 2 integers to return the system time
 *  \details Used in GRTOS_now
 *  \relates Time
 */
void GRTOS_CMD_TM_CNT_GET(int *ptime)
{
	ptime[1] = (int) IORD_GRTOS_TM_CNT_HGH;
	ptime[0] = (int) IORD_GRTOS_SMP;
}

/**GRTOS_CMD_FRZ_TM_THR_GET
 *  \brief Returns the value of the Frozen Time Threshold (R_FRZ_TM_THR) register of the GRTOS controller
 *  
 *  \param [in] ptime Variable in which the value is returned
 *  \relates Time
 */
void GRTOS_CMD_FRZ_TM_THR_GET(int *ptime)
{
	ptime[1] = (int) IORD_GRTOS_FRZ_THR_HGH;
	ptime[0] = (int) IORD_GRTOS_SMP;
}


/**GRTOS_now
 *  \brief Return the current system time
 *  \return INT64 with the current system time
 *  \relates Time
 */
INT64 GRTOS_now(void)
{
	INT64 timenow;
	GRTOS_CMD_TM_CNT_GET((int *) &timenow);
	return(timenow);
}

/**GetMaximumMutexExecutionTime
 *  \brief Reset the Interval Time Counter, returning the last value
 *  \return Return the interval time since the laast reset
 *  \relates Time
 */
int GetMaximumMutexExecutionTime(void)
{
	return(IORD_GRTOS_ELP_TM_CNT);
}


/**********************************************************************************
 *                        	IDLE TASK DOING NOTHING
 *********************************************************************************/
/**gk_CODE_IDLE_TASK
 *  \brief IDLE task. This code is executed when no task is ready. The processor is halted.
 *  
 *  \param [in] pdata Pointer to data
 *  \details Task can be changed. The processors run this task with different stacks
 *  \relates Task
 */
void gk_CODE_IDLE_TASK(void* pdata)
{
	while(1)
    {
        // #if G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE == 1
            // fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "IDLE time elapsed: %llx \n",(unsigned long long) GRTOS_CMD_HLT_ACT_CNT_GET());
            fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "I ");
        // #endif
        GRTOS_CMD_HALT_PROCESSOR; // Put the procesor in HALT mode 
    }
}

// #####################################################################

// /**
//  * ISR for IRQ0
//  * @param pdata
//  */
// void taskISR0(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[0].handler(alt_irq[0].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ1
//  * @param pdata
//  */
// void taskISR1(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[1].handler(alt_irq[1].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ2
//  * @param pdata
//  */
// void taskISR2(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[2].handler(alt_irq[2].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ3
//  * @param pdata
//  */
// void taskISR3(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[3].handler(alt_irq[3].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ4
//  * @param pdata
//  */
// void taskISR4(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[4].handler(alt_irq[4].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ5
//  * @param pdata
//  */
// void taskISR5(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[5].handler(alt_irq[5].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ6
//  * @param pdata
//  */
// void taskISR6(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[6].handler(alt_irq[6].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ7
//  * @param pdata
//  */
// void taskISR7(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[7].handler(alt_irq[7].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ8
//  * @param pdata
//  */
// void taskISR8(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[8].handler(alt_irq[8].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ9
//  * @param pdata
//  */
// void taskISR9(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[9].handler(alt_irq[9].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ10
//  * @param pdata
//  */
// void taskISR10(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[10].handler(alt_irq[10].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ11
//  * @param pdata
//  */
// void taskISR11(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[11].handler(alt_irq[11].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ12
//  * @param pdata
//  */
// void taskISR12(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[12].handler(alt_irq[12].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ13
//  * @param pdata
//  */
// void taskISR13(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[13].handler(alt_irq[13].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ14
//  * @param pdata
//  */
// void taskISR14(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[14].handler(alt_irq[14].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ15
//  * @param pdata
//  */
// void taskISR15(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[15].handler(alt_irq[15].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ16
//  * @param pdata
//  */
// void taskISR16(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[16].handler(alt_irq[16].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ17
//  * @param pdata
//  */
// void taskISR17(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[17].handler(alt_irq[17].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ18
//  * @param pdata
//  */
// void taskISR18(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[18].handler(alt_irq[18].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ19
//  * @param pdata
//  */
// void taskISR19(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[19].handler(alt_irq[19].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ20
//  * @param pdata
//  */
// void taskISR20(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[20].handler(alt_irq[20].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ21
//  * @param pdata
//  */
// void taskISR21(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[21].handler(alt_irq[21].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ22
//  * @param pdata
//  */
// void taskISR22(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[22].handler(alt_irq[22].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ23
//  * @param pdata
//  */
// void taskISR23(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[23].handler(alt_irq[23].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ24
//  * @param pdata
//  */
// void taskISR24(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[24].handler(alt_irq[24].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ25
//  * @param pdata
//  */
// void taskISR25(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[25].handler(alt_irq[25].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ26
//  * @param pdata
//  */
// void taskISR26(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[26].handler(alt_irq[26].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ27
//  * @param pdata
//  */
// void taskISR27(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[27].handler(alt_irq[27].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ28
//  * @param pdata
//  */
// void taskISR28(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[28].handler(alt_irq[28].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ29
//  * @param pdata
//  */
// void taskISR29(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[29].handler(alt_irq[29].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ30
//  * @param pdata
//  */
// void taskISR30(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[30].handler(alt_irq[30].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }
// 
// /**
//  * ISR for IRQ31
//  * @param pdata
//  */
// void taskISR31(void* pdata)
// {
// GRTOS_CMD_CRITICAL_SECTION_GET;
//     alt_irq[31].handler(alt_irq[31].context);
// GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }

// void taskISRERROR(void* pdata)
// {
// 	G_DEBUG_VERBOSE
//     while(1);
// }

OPTIMEZE_RESTORE