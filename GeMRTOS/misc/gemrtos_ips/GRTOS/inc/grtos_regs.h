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
#define C_SYS_TM                5
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

#define ADDR_HLT_ACT_CNT_SMP    34

// Macros defined and implemented in hardware, but not used
#define IOWR_GRTOS_HLT_IDL_PRC_CLR(data)         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_DSB, data)
#define IOWR_GRTOS_FRZ_TM_HGH(data)              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_TM_HGH,data)


// Read grtos registers
#define IORD_GRTOS_SMP                           IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP)
#define IORD_GRTOS_TM_CNT_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_CNT_HGH)
#define IORD_GRTOS_SYS_MUTEX_TIME_HGH            IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_MUTEX_TIME_HGH)
#define IORD_GRTOS_SYS_TM_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_TM)
#define IORD_GRTOS_MTX_RQS                       IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_GRN)
#define IORD_GRTOS_ELP_TM_CNT                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_ELP_TM_CNT)
#define IORD_GRTOS_FRZ_TM_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_TM_HGH)
#define IORD_GRTOS_TM_PSC                        IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_PSC)
#define IORD_GRTOS_FRZ_THR_HGH                   IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_THR_HGH)
#define IORD_GRTOS_CTRL_SET                      IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL)
#define IORD_GRTOS_EVN_OCC                       IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_EVN_OCC)

#define IORD_GRTOS_MUTEXBLOCKED32                IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MUTEXBLOCKED32)

#define IORD_GRTOS_IRQ_RQS                       IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_RQS)
#define IORD_GRTOS_INT_ENB_SET                   IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET)
#define IORD_GRTOS_RST_CLR                       IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR)

#define IORD_GRTOS_HLT_IDL_PRC_ENB               IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_ENB)
#define IORD_GRTOS_TRG_INT_PRC                   IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TRG_INT_PRC)

#define IORD_GRTOS_HLT_ACT_CNT_SMP               IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_ACT_CNT_SMP)


// Write grtos registers
#define IOWR_GRTOS_SMP(data)                     IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP, data)
#define IOWR_GRTOS_NXT_OCC_TM_RQS(data)          IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_NXT_OCC_TM_HGH, data)
#define IOWR_GRTOS_MTX_RQS(data)                 IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_GRN, data)
#define IOWR_GRTOS_MTX_RLS(data)                 IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RLS, data)
#define IOWR_GRTOS_MTX_RSV_SET(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RSV_SET, data)
#define IOWR_GRTOS_MTX_RSV_CLR(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RSV_CLR, data)
/// #define IOWR_GRTOS_INT_PRC_PND_CLR(data)         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_PRC_PND_CLR, GRTOS_CMD_PRC_ID)


#define IOWR_GRTOS_FRZ_THR_HGH(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_THR_HGH, data)


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










#endif /* __GRTOS_REGS_H__ */
