/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS register mapping functions
 *  \details This file contains the GRTOS register mapping functions.
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


#ifndef __GRTOS_REGS_H__
#define __GRTOS_REGS_H__

#include <io.h>
// extern void *GRTOS_DRIVER_GRTOS_BASE;

#define ADDR_SMP                0
#define ADDR_TM_CNT_HGH         1

#define ADDR_NXT_OCC_TM_HGH     3
#define ADDR_SYS_MUTEX_TIME_HGH 4
#define ADDR_SYS_TM             5
#define ADDR_MTX_GRN            6
#define ADDR_MTX_RLS            7

#define ADDR_MTX_RSV_SET        9
#define ADDR_MTX_RSV_CLR        10
#define ADDR_INT_PRC_PND_CLR    11
#define ADDR_ELP_TM_CNT         12
#define ADDR_MTX_SET_TM         13
#define ADDR_FRZ_TM_HGH         14
#define ADDR_TM_PSC             15
#define ADDR_FRZ_THR_HGH        16
#define ADDR_CTRL               17
#define ADDR_EVN_OCC            19
#define ADDR_MUTEXBLOCKED32     20
#define ADDR_INT_ENB_SET        21
#define ADD_INT_ENB_CLR         22
#define ADDR_LOW_PRC_SET   		23
#define ADDR_NXT_TM_PRC_SET    	24

#define ADDR_HLT_IDL_PRC_ENB    25
#define ADDR_HLT_IDL_PRC_DSB    26

#define ADDR_IRQ_ENB_SET        27
#define ADDR_IRQ_ENB_CLR        28
#define ADDR_IRQ_RQS            29
#define ADDR_RST_CLR            30
#define ADDR_TRG_INT_PRC	    31

#define ADDR_MTX_NEWLIB_GRN     32
#define ADDR_MTX_NEWLIB_RLS     33

#define ADDR_HLT_ACT_CNT_SMP    34

// Macros defined and implemented in hardware, but not used
#define IORD_GRTOS_HLT_ACT_CNT_SMP               IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_ACT_CNT_SMP)
#define IORD_GRTOS_TRG_INT_PRC                   IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TRG_INT_PRC)
#define IORD_GRTOS_HLT_IDL_PRC_ENB               IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_ENB)
#define IORD_GRTOS_RST_CLR                       IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR)
#define IORD_GRTOS_FRZ_TM_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_TM_HGH)

#define IOWR_GRTOS_HLT_IDL_PRC_CLR(data)         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_DSB, data)
#define IOWR_GRTOS_FRZ_TM_HGH(data)              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_TM_HGH,data)
#define IOWR_GRTOS_MTX_RSV_CLR(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RSV_CLR, data)
#define IOWR_GRTOS_MTX_RQS(data)                 IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_GRN, data)


// Read grtos registers
// #define IORD_GRTOS_SMP                           IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP)
// #define IORD_GRTOS_TM_CNT_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_CNT_HGH)
// #define IORD_GRTOS_SYS_MUTEX_TIME_HGH            IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_MUTEX_TIME_HGH)
// #define IORD_GRTOS_SYS_TM_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_TM)



/// \brief GRTOS_now Return the current system time
/// \details ONLY for critical section use
/// \return INT64 with the current system time
/// \relates Time
#define GRTOS_now ({ \
TIMEPRIORITY value64; \
do { \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_CNT_HGH); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
} while(0); \
    value64.i64; \
})


/// \brief GRTOS_CMD_MTX_TM_GET Return the time the mutex is granted in system time units
/// \details ONLY for use in critical section
/// \relates Miscellaneous
#define GRTOS_CMD_MTX_TM_GET ({ \
TIMEPRIORITY value64; \
do { \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_MUTEX_TIME_HGH); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
} while(0); \
    value64.i64; \
})

/// \brief GRTOS_CMD_SYS_TM_GET Returns the value of System TIME (R_FRZ_CNT + R_TM_CNT) 
/// \details ONLY for use in critical section
/// \return Returns the value of the System Time (C_SYS_TM)
/// \relates Time
#define GRTOS_CMD_SYS_TM_GET ({ \
TIMEPRIORITY value64; \
do { \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_TM); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
} while(0); \
    value64.i64; \
})


/// \brief GRTOS_MTX_PRC_GRANTED returns the CPUID of the processor granting the GRTOS MUTEX
/// \todo Describe better and related with GRTOS controller
#define GRTOS_MTX_PRC_GRANTED                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_GRN)

/// GRTOS_GET_INTERVAL
///  \brief GRTOS_GET_INTERVAL Reset the Interval Time Counter, returning the last value
///  \return Return the interval time since the laast reset
///  \relates Time
#define GRTOS_GET_INTERVAL                       IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_ELP_TM_CNT)

/// \brief GRTOS_CMD_GET_TIME_PRESCALE gets the system clock prescale
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GET_TIME_PRESCALE                          IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_PSC)

/// \brief GRTOS_CMD_FRZ_TM_THR_GET Returns the value of the Frozen Time Threshold (R_FRZ_TM_THR) register of the GRTOS controller
/// \details ONLY for use in critical section
/// \return Returns the value of the Frozen Time Threshold (R_FRZ_TM_THR)
/// \param [in] None
/// \relates Time
#define GRTOS_CMD_FRZ_TM_THR_GET ({ \
    TIMEPRIORITY value64; \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_THR_HGH); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
    value64.i64; \
})

/// \brief GRTOS_CMD_GET_STATUS_DEBUG_HOLD return the status of the DEBUG_HOLD bit (G_TRUE or G_FALSE)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GET_STATUS_DEBUG_HOLD (((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL) >> 2) & 1) ? G_TRUE : G_FALSE)

/// \brief GRTOS_CMD_GET_FRZ_ENB returns the status of the frozen mode (G_TRUE if enabled, G_FALSE if disabled)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GET_FRZ_ENB (((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL) >> 1) & 1) ? G_TRUE : G_FALSE)

/// \brief GRTOS_CMD_GET_FRZ_ACT returns the status of the frozen mode event (G_TRUE if active, G_FALSE if inactive)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GET_FRZ_ACT ((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL)  & 1) ? G_TRUE : G_FALSE)

/// \brief GRTOS_CMD_EVN_OCC returns the event happened from the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_EVN_OCC                                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_EVN_OCC)

/// \brief GRTOS_MUTEX_BLOCKED_GET returns the time the mutex will be blocked until next granted
#define GRTOS_MUTEX_BLOCKED_GET                              IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MUTEXBLOCKED32)

/// \brief GRTOS_CMD_IRQ_RQS gets the status of the External IRQ register
/// \todo Describe better and related with GRTOS controller
/// \todo Check if it is required
#define GRTOS_CMD_IRQ_RQS                                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_RQS)

/// \brief GRTOS_CMD_IRQ_ENB_GET(irq) reads the enabled status of the device interrupt request (DIRQ) irq
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_IRQ_ENB_GET(irq) ((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET) >> (irq-1)) & 1)


// Write grtos registers

/// \brief GRTOS_CMD_FRZ_TM_THR_SET Sets the Frozen Time Threshold register of the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_TM_THR_SET(timeset) \
	do { \
		TIMEPRIORITY temp_aux; \
        temp_aux.i64 = (INT64) timeset; \
        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP, (unsigned long)temp_aux.i32[0]); \
 		IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_THR_HGH, (unsigned long)temp_aux.i32[1]); \
 	}while(0) 

/// \brief GRTOS_CMD_NXT_OCC_TM_EVN_SET Sets the Next Occurence Time register of the GRTOS controller
/// to produce a timed event when system time reaches the value of timeset
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_NXT_OCC_TM_EVN_SET(timeset) \
	do { \
		TIMEPRIORITY temp_aux; \
        temp_aux.i64 = (INT64) timeset; \
        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP, (unsigned long)temp_aux.i32[0]); \
        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_NXT_OCC_TM_HGH, (unsigned long)temp_aux.i32[1]); \
 	}while(0) 

/// \brief GRTOS_MTX_RLS releases the mutex from the current processor
#define GRTOS_MTX_RLS                                        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RLS, GRTOS_CMD_PRC_ID)

/// \brief GRTOS_MTX_RSV_SET sets the reserve bit of the current processor to grant the mutex
#define GRTOS_MTX_RSV_SET                                    IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RSV_SET, GRTOS_CMD_PRC_ID)

/// \brief GRTOS_CMD_INT_PRC_PND_CLR clears the pending interrupt of the current processor.
/// The interrupts to processors are always registered to avoid spurious interrupts
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_INT_PRC_PND_CLR                            IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_PRC_PND_CLR, GRTOS_CMD_PRC_ID);

/// \brief GRTOS_CMD_SET_TIME_PRESCALE sets the system clock prescale to get the system time unitbuf
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_SET_TIME_PRESCALE(scale)                   IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_PSC, scale);

/// \brief GRTOS_CMD_FRZ_ENB_SET enables the frozen mode event
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_ENB_SET                                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 1)

/// \brief GRTOS_CMD_FRZ_DSB_SET disables the frozen mode event
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_DSB_SET                                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 2)

/// \brief GRTOS_CMD_FRZ_EVN_CLR Clears the C1_FRZ_EVN flags. 
/// It should be executed when the frozen mode strategy is completed.
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_EVN_CLR                                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 3) 

/// \brief GRTOS_CMD_TM_CNT_CLR resets the System Time Register
/// \todo Describe better and related with GRTOS controller
/// \todo Check if it is required
#define GRTOS_CMD_TM_CNT_CLR                                 IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 4); 

/// \brief GRTOS_CMD_DEBUG_HOLD_ENB - enables the DEBUG_HOLD mode
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HOLD_ENB                             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 6)

/// \brief GRTOS_CMD_DEBUG_HOLD_DIS - disables the DEBUG_HOLD mode
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HOLD_DIS                             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 7)

/// \brief GRTOS_CMD_PRC_INT_ENB enables the processor interrupt request of the current processor in the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_PRC_INT_ENB                                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET, GRTOS_CMD_PRC_ID);

/// \brief GRTOS_CMD_PRC_INT_DSB disables the interrupt of the current processor in the GRTOS controller. 
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_PRC_INT_DSB                                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADD_INT_ENB_CLR, GRTOS_CMD_PRC_ID);

/// \brief GRTOS_CMD_LOW_PRC_SET 
/// Sets the processor with CPUID equal to processor as the Lowest Priority Processor in the GRTOS Controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_LOW_PRC_SET(processor)                     IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_LOW_PRC_SET, processor);

/// \brief GRTOS_CMD_NXT_TM_PRC_SET sets the Next Occurrence Time Processor in the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_NXT_TM_PRC_SET(processor)                  IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_NXT_TM_PRC_SET, processor);

/// \brief GRTOS_MUTEX_BLOCKED set the time the mutex will be blocked until next granted
#define GRTOS_MUTEX_BLOCKED(data)                            IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MUTEXBLOCKED32, data)
    
/// \brief GRTOS_CMD_HLT_IDL_PRC set to halt the current processor when it is idle
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_HLT_IDL_PRC                                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_ENB, GRTOS_CMD_PRC_ID)

/// \brief GRTOS_CMD_IRQ_ENB_SET(irq) enables the device interrupt request event irq  (DIRQ)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_IRQ_ENB_SET(irq)                           IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_ENB_SET, irq)

/// \brief GRTOS_CMD_IRQ_ENB_CLR(irq) disables the device interrupt request event irq  (DIRQ)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_IRQ_ENB_CLR(irq)                           IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_ENB_CLR, irq)

/// \brief GRTOS_CMD_TRG_PRC_INT_SET interrupts the processor proc. If proc interrupt is 
/// enabled in the GRTOS controller then the processor is interrupted
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_TRG_PRC_INT_SET(proc)                      IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_TRG_INT_PRC, proc);

/// \brief GRTOS_CMD_RST_GRTOS - resets the GRTOS controller to its initialization values
/// and the processors with CPUID different from 1
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_RST_GRTOS                                  IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR, 0)

/// \brief GRTOS_CMD_RLS_NXT_PRC - unreset the next processor
#define GRTOS_CMD_RLS_NXT_PRC                                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR, GRTOS_CMD_PRC_ID)

/// \brief GRTOS_USER_CRITICAL_SECTION_SET_RELEASE_TIME 
/// Sets the register ADDR_MTX_SET_TM which holds the number of clocks cycles the
/// Mutex remains granted after it is released. This time is defined in order to allow
/// leaving the section critical after releasing the mutex
#define GRTOS_USER_CRITICAL_SECTION_SET_RELEASE_TIME(time)   IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_SET_TM, time);


/************************************************************************************
 *  GRTOS COMMANDS
 ************************************************************************************/

/// GRTOS_CMD_PRC_ID  - returns the cpuID of the current processor
#define GRTOS_CMD_PRC_ID  __builtin_rdctl(5)

/// GRTOS_CMD_PRC_SP  - returns the Stack Pointer register (SP) of the current processor
#define GRTOS_CMD_PRC_SP  ({ \
        void *StackPointer; \
        NIOS2_READ_SP(StackPointer); \
        (INT32) StackPointer; \
    })

/// \brief GRTOS_CMD_PRC_INT(proc) 
/// interrupts the processor and waits until it reaches the ISR and disables the interrupt
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_PRC_INT(proc) \
    GRTOS_CMD_TRG_PRC_INT_SET(proc); \
    while (GRTOS_CMD_IRQ_ENB_GET(proc)){ \
        while(0); \
    }


/// \brief GRTOS_CMD_HALT_PROCESSOR puts the processor in halt mode
/// The first command enable the IDLE state for the processor and the second read the first 
/// address of the processor bus because it is in waitrequest until next interrupt
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_HALT_PROCESSOR \
        GRTOS_CMD_HLT_IDL_PRC; \
        g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].GRTOS_PROCESSOR_BASE[0] = (int) 0;

/************************************************************************************
 *  GRTOS CRITICAL SECTION COMMANDS
 ************************************************************************************/

/**
 *  \brief GRTOS_CMD_CRITICAL_SECTION_GET 
 *  Defines the entry to a critical section to handle system variables and ISR routines.
 *  It asks for critical section and puts the processor in halt mode. 
 *  GRTOS controller enables the processor when section is granted to it.
 *  Only call Kernel functions in a critical section in order to avoid critical section reentrancy
 */
#define GRTOS_CMD_CRITICAL_SECTION_GET \
	do{ \
		do { \
            GRTOS_MTX_RSV_SET; \
            GRTOS_CMD_HALT_PROCESSOR \
        } while (GRTOS_CMD_PRC_ID != GRTOS_MTX_PRC_GRANTED);  \
	}while(0)


/**
 *  \brief GRTOS_CMD_CRITICAL_SECTION_RELEASE 
 *  Releases the critical section from the current processor.
 *  The final released is delayed by the controller to let the processor finishes executing 
 *  the return from the critical section.
 *  It should be executed from the interrupt routine.
 */
#define GRTOS_CMD_CRITICAL_SECTION_RELEASE \
	do{ GRTOS_CMD_PRC_INT_ENB; \
        alt_dcache_flush_all(); \
        GRTOS_MTX_RLS; \
	}while(0)
  
/**
 *  \brief GRTOS_CMD_MTX_RQS_GET 
 *  Returns the current value of the Mutex.
 */
#define GRTOS_CMD_MTX_RQS_GET GRTOS_MTX_PRC_GRANTED;

/**
 *  \brief GRTOS_USER_CRITICAL_SECTION_GET defines the entry to a critical section to handle system variables.
 *  It is called from outside an interrupt and it may be interrupted while it is waiting for mutex.
 *  It should be used in user functions that execute kernel functions or modify kernal data.
 */
#define GRTOS_USER_CRITICAL_SECTION_GET  GRTOS_CMD_CRITICAL_SECTION_GET


/**
 *  \brief GRTOS_USER_CRITICAL_SECTION_RELEASE releases the critical section from the current processor.
 *  It is called from outside of an interrupt.
 *  It used in all the user function that executes kernel functions or modify kernel data.
 */
#define GRTOS_USER_CRITICAL_SECTION_RELEASE  GRTOS_CMD_CRITICAL_SECTION_RELEASE


/**
 *  \brief GRTOS_CMD_NEWLIB_MUTEX_GET 
 *  Defines the stub for the newlib critical section.
 *  It asks for mutex and checks if it is granted. When already granted, 
 *  the nested counter is incremented.
 */
#define GRTOS_CMD_NEWLIB_MUTEX_GET \
	do{ \
        unsigned int processor; \
		do { \
            processor = (1 << (GRTOS_CMD_PRC_ID - 1)); \
            IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_NEWLIB_GRN, GRTOS_CMD_PRC_ID); \
        } while (processor != (unsigned int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_NEWLIB_GRN));  \
	}while(0)


/**
 *  \brief GRTOS_CMD_NEWLIB_MUTEX_RELEASE 
 *  Defines the stub for the realise of the newlib critical section.
 */
#define GRTOS_CMD_NEWLIB_MUTEX_RELEASE \
	do{ IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_NEWLIB_RLS, GRTOS_CMD_PRC_ID); \
	}while(0)	



#endif /* __GRTOS_REGS_H__ */
