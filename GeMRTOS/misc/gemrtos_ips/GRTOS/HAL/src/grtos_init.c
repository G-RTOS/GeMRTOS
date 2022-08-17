/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Initialization for ISR functions
 *  \details This file contains the Initialization for ISR functions.
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

// /**GRTOS_CMD_TM_CNT_GET
//  *  \brief Read the GRTOS System Time Register
//  *  \param [in] ptime Pointer to an array of 2 integers to return the system time
//  *  \details Used in GRTOS_now
//  *  \relates Time
//  */
// void GRTOS_CMD_TM_CNT_GET(int *ptime)
// {
// 	ptime[1] = (int) IORD_GRTOS_TM_CNT_HGH;
// 	ptime[0] = (int) IORD_GRTOS_SMP;
// }

/**GRTOS_CMD_SYS_MUTEX_TIME
 *  \brief Read the GRTOS Mutex System Time Register
 *  \param [in] ptime Pointer to an array of 2 integers to return the system time
 *  \details Used to read the accumulated time with granted mutex of the system
 *  \relates Time
 */
INT64 GRTOS_CMD_SYS_MUTEX_TIME(void)
{
    union {
        unsigned long long i64;
        unsigned i32[2];
    } value64;
    
    value64.i32[1] = (unsigned) IORD_GRTOS_SYS_MUTEX_TIME_HGH;
    value64.i32[0] = (unsigned) IORD_GRTOS_SMP;
    
    return ((INT64) value64.i64);
}


/**GRTOS_CMD_FRZ_TM_THR_GET
 *  \brief Returns the value of the Frozen Time Threshold (R_FRZ_TM_THR) register of the GRTOS controller
 *  \details ONLY for use in critical section
 *  \return Returns the value of the Frozen Time Threshold (R_FRZ_TM_THR)
 *  \param [in] None
 *  \relates Time
 */
INT64 GRTOS_CMD_FRZ_TM_THR_GET(void)
{
    union {
        unsigned long long i64;
        unsigned i32[2];
    } value64;
    
    value64.i32[1] = (unsigned) IORD_GRTOS_FRZ_THR_HGH;
    value64.i32[0] = (unsigned) IORD_GRTOS_SMP;
    
    return ((INT64) value64.i64);
    
	// ptime[1] = (int) IORD_GRTOS_FRZ_THR_HGH;
	// ptime[0] = (int) IORD_GRTOS_SMP;
}


/**GRTOS_now
 *  \brief Return the current system time
 *  \details ONLY for critical section use
 *  \return INT64 with the current system time
 *  \relates Time
 */
INT64 GRTOS_now(void)
{
    union {
        unsigned long long i64;
        unsigned i32[2];
    } value64;
    
    value64.i32[1] = (unsigned) IORD_GRTOS_TM_CNT_HGH;
    value64.i32[0] = (unsigned) IORD_GRTOS_SMP;
    
    return ((INT64) value64.i64);
    
	// INT64 timenow;
	// GRTOS_CMD_TM_CNT_GET((int *) &timenow);
	// return(timenow);
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
        #if G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE == 1
            fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "I ");
        #endif
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


OPTIMEZE_RESTORE