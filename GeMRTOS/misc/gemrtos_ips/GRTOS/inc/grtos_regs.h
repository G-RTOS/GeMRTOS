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


// Read grtos registers
#define IORD_GRTOS_SMP                           IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP)
#define IORD_GRTOS_TM_CNT_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_CNT_HGH)
#define IORD_GRTOS_SYS_MUTEX_TIME_HGH            IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_MUTEX_TIME_HGH)

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
#define IOWR_GRTOS_INT_PRC_PND_CLR(data)         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_PRC_PND_CLR, data)
#define IOWR_GRTOS_FRZ_TM_HGH(data)              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_TM_HGH,data)
#define IOWR_GRTOS_TM_PSC(data)                  IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_PSC, data)
#define IOWR_GRTOS_FRZ_THR_HGH(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_THR_HGH, data)
#define IOWR_GRTOS_CTRL_SET(data)                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, data)
#define IOWR_GRTOS_INT_ENB_SET(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET, data)
#define IOWR_GRTOS_INT_ENB_CLR(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADD_INT_ENB_CLR, data)
#define IOWR_GRTOS_LOW_PRC_SET(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_LOW_PRC_SET, data)
#define IOWR_GRTOS_NXT_TM_PRC_SET(data)          IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_NXT_TM_PRC_SET, data)
#define IOWR_GRTOS_MUTEXBLOCKED32(data)          IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MUTEXBLOCKED32, data)


#define IOWR_GRTOS_HLT_IDL_PRC_SET(data)         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_ENB, data)
#define IOWR_GRTOS_HLT_IDL_PRC_CLR(data)         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_DSB, data)

#define IOWR_GRTOS_IRQ_ENB_SET(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_ENB_SET, data)
#define IOWR_GRTOS_IRQ_ENB_CLR(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_ENB_CLR, data)
#define IOWR_GRTOS_TRG_INT_PRC(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_TRG_INT_PRC, data)
#define IOWR_GRTOS_MTX_SET_TM(data)              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_SET_TM, data)

#define IOWR_GRTOS_RST_CLR(data)                 IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR, data)

#endif /* __GRTOS_REGS_H__ */
