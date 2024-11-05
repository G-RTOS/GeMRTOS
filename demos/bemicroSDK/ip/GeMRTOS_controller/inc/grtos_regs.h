/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GRTOS register mapping functions
 *  \details This file contains the GRTOS register mapping functions.
 *  \version 0.9a
 *  \date      2015-2020
 *  \bug  Describe found bugs here.
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


#ifndef __GRTOS_REGS_H__
#define __GRTOS_REGS_H__


// extern void *GRTOS_DRIVER_GRTOS_BASE;

#define ADDR_SMP                30
#define ADDR_TM_CNT_HGH         16
#define ADDR_MTX_HGHST_PRC      53
#define ADDR_RST_CLR            51
#define ADDR_ERET_REG           62
#define ADDR_NXT_OCC_TM_HGH     54
#define ADDR_SYS_MUTEX_TIME_HGH 18
#define ADDR_SYS_TM             17
#define ADDR_MTX_GRN            57
#define ADDR_MTX_RLS            58
#define ADDR_LEDS               59
#define ADDR_MTX_RSV_SET        60
#define ADDR_MTX_NESTED         61
#define ADDR_INT_PRC_PND_CLR    63
#define ADDR_ELP_TM_CNT         35
#define ADDR_PRC_CRTCL_SECTION_START  36
#define ADDR_PRC_CRTCL_SECTION_END    37


#define ADDR_FRZ_TM_HGH         20
#define ADDR_TM_PSC             38
#define ADDR_FRZ_THR_HGH        19
#define ADDR_CTRL               40
#define ADDR_EVN_OCC            41
// #define ADDR_MUTEXBLOCKED32     20
#define ADDR_INT_ENB_SET        26
#define ADD_INT_ENB_CLR         32
#define ADDR_LOW_PRC_SET   		42
#define ADDR_NXT_TM_PRC_SET    	24

#define ADDR_HLT_IDL_PRC_ENB    25
#define ADDR_HLT_IDL_PRC_DSB    26

#define ADDR_IRQ_ENB_SET        27
#define ADDR_IRQ_ENB_CLR        28
#define ADDR_IRQ_RQS            29

#define ADDR_TRG_INT_PRC	    31


// 27-05-2024 #define ADDR_INT_RQS               32
// 27-05-2024 #define ADDR_DIRQI               38
#define ADDR_MTX_PRC_GRN_BIT     33


// #########################################
#define ADDR_PRC_CDE_ADDR0   0
#define ADDR_PRC_CDE_ADDR1   1
#define ADDR_PRC_CDE_ADDR2   2
#define ADDR_PRC_CDE_ADDR3   3
#define ADDR_PRC_CDE_ADDR4   4
#define ADDR_PRC_CDE_ADDR5   5
#define ADDR_PRC_CDE_ADDR6   6
#define ADDR_PRC_CDE_ADDR7   7
#define ADDR_PRC_CDE_ADDR8   8
#define ADDR_PRC_CDE_ADDR9   9
#define ADDR_PRC_CDE_ADDR10  10
#define ADDR_PRC_CDE_ADDR11  11
#define ADDR_PRC_CDE_ADDR12  12
#define ADDR_PRC_CDE_ADDR13  13
#define ADDR_PRC_CDE_ADDR14  14
#define ADDR_PRC_CDE_ADDR15  15


#define ADDR_REG_0  0
#define ADDR_REG_1  1 
#define ADDR_REG_2  2 	
#define ADDR_REG_3  3 
#define ADDR_REG_4  4 
#define ADDR_REG_5  5 
#define ADDR_REG_6  6 
#define ADDR_REG_7  7 
#define ADDR_REG_8  8 
#define ADDR_REG_9  9 
#define ADDR_REG_10 10
#define ADDR_REG_11 11
#define ADDR_REG_12 12
#define ADDR_REG_13 13
#define ADDR_REG_14 14
#define ADDR_REG_15 15	
// #########################################


/************************************************************************************
 *  STACK OFFSET FOR STATUS SAVE AND RESTORE
 ************************************************************************************/
#define BYTE_BY_ADDRESS     4
 
#define OFFSET_REG_0        0
#define OFFSET_REG_1        1
#define OFFSET_REG_2        2
#define OFFSET_REG_3        3
#define OFFSET_REG_4        4
#define OFFSET_REG_5        5
#define OFFSET_REG_6        6
#define OFFSET_REG_7        7
#define OFFSET_REG_8        8
#define OFFSET_REG_9        9
#define OFFSET_REG_10      10
#define OFFSET_REG_11      11
#define OFFSET_REG_12      12
#define OFFSET_REG_13      13
#define OFFSET_REG_14      14
#define OFFSET_REG_15      15
#define OFFSET_REG_16      16
#define OFFSET_REG_17      17
#define OFFSET_REG_18      18
#define OFFSET_REG_19      19
#define OFFSET_REG_20      20
#define OFFSET_REG_21      21
#define OFFSET_REG_22      22
#define OFFSET_REG_23      23
#define OFFSET_REG_24      24
#define OFFSET_REG_25      25
#define OFFSET_REG_26      26
#define OFFSET_REG_27      27
#define OFFSET_REG_28      28
#define OFFSET_REG_29      29
#define OFFSET_REG_30      30
#define OFFSET_REG_31      31
#define OFFSET_REG_32      32
#define OFFSET_REG_33      33
#define OFFSET_REG_34      34
#define OFFSET_REG_35      35
#define OFFSET_REG_36      36
#define OFFSET_REG_37      37
#define OFFSET_REG_38      38
#define OFFSET_REG_39      39

#endif /* __GRTOS_REGS_H__ */
