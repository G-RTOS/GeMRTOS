/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Definition of GRTOS controller address
 *  \details This file contains the Definition of GRTOS controller address.
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


#ifndef __GRTOS_H__
#define __GRTOS_H__

#include <system.h>
#include <stddef.h>

#include "sys/alt_alarm.h"
#include "sys/alt_warning.h"

#include "os/alt_sem.h"
#include "os/alt_flag.h"

#include "grtos_regs.h"
#include <grtosdefinitions.h>


#define INT64U long long

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* 
 * If the user wants all drivers to be small rather than fast then make sure
 * this one is marked as needing to be small.
 */
#if defined ALT_USE_SMALL_DRIVERS && !defined GRTOS_SMALL
#define GRTOS_SMALL
#endif

/*
* Definition of GRTOS variables
*/
// void *GRTOS_BASE;
// void *GRTOS_MONITOR_BASE;

/*
 * Macros used by alt_sys_init when the ALT file descriptor facility isn't used.
 */


#define GRTOS_INIT(name, dev)  

#define GRTOS_INSTANCE(name, grtosstate)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ALT_AVALON_JTAG_UART_H__ */
