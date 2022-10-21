/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Wrap for gemrtos_hal functions for GeMRTOS
 *  \details This file contains the hal redefinitions.
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

#include <stdarg.h>
#include <stdio.h>

extern int __real_printf(const char*, ...);

int __wrap_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    __real_printf("[!] ");
    int result = vprintf(format, args);
    va_end(args);
    return result;
}