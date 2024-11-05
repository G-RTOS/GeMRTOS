/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Definition of GRTOS controller address
 *  \details This file contains the Definition of GRTOS controller address.
 *  \version 0.9a
 *  \date      2015-2020
 *  \bug  None known
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

#ifndef __GEMRTOS_H__
#define __GEMRTOS_H__

#if defined(__nios2_arch__) 
#define __niosX_arch__
#endif

#ifdef __riscv
#define __niosX_arch__
#endif


#include "system.h"

#ifdef __nios2_arch__
#include "nios2.h"
#define GRTOS_READ_SP(dest) NIOS2_READ_SP(dest)
#define GRTOS_DISABLE_PROC_INTERRUPTS  NIOS2_WRITE_STATUS(0)
#define GRTOS_DISABLE_DEVICES_IRQs     NIOS2_WRITE_IENABLE(0);
#define GRTOS_ENABLE_GeMRTOS_IRQ       NIOS2_WRITE_IENABLE(1);
// \brief G_RETURN_OFFSET_FROM_INTERRUPT offset to add to the return offset because
// it is returned from interrupt. In NIOS2 processor, returning from
// interrupt decrement in 4 previous to return
#define G_RETURN_OFFSET_FROM_INTERRUPT  4
// \brief G_STACK_STATUS_LENGTH number of addresses required in stack to save the task status
#define G_STACK_STATUS_LENGTH  28
#endif


#ifdef __riscv
#include "intel_niosv.h"
#define GRTOS_READ_SP(dest) NIOSV_READ_SP(dest)

#define GRTOS_DISABLE_DEVICES_IRQs  do { __asm volatile ("csrw mstatus, zero"); \
										 __asm volatile ("csrw mie, zero"); \
										 __asm volatile ("csrw mip, zero"); \
									   } while (0)

#define GRTOS_DISABLE_PROC_INTERRUPTS  GRTOS_DISABLE_DEVICES_IRQs

// it is bit 16 the first external hardware interrupt available
#define GRTOS_ENABLE_GeMRTOS_IRQ  \
		do { __asm volatile ("la t0, grtos_irq_entry; \
						      csrw mtvec, t0; \
							  li t0, 0x80; \
							  csrw mstatus, t0; \
							  li t0 , 0x00010800; \
							  csrw mie, t0"); \
		} while (0)

#define G_RETURN_OFFSET_FROM_INTERRUPT  0
#define G_STACK_STATUS_LENGTH  34
#define NIOSV_READ_MSTATUS(data) do { __asm ("csrr %0, mstatus" : "=r" (data) ); } while (0)
#define NIOSV_READ_MTVEC(data) do { __asm ("csrr %0, mtvec" : "=r" (data) ); } while (0)
#define NIOSV_READ_MCAUSE(data) do { __asm ("csrr %0, mcause" : "=r" (data) ); } while (0)
#define NIOSV_READ_MIE(data) do { __asm ("csrr %0, mie" : "=r" (data) ); } while (0)
#define NIOSV_READ_MIP(data) do { __asm ("csrr %0, mip" : "=r" (data) ); } while (0)
#define NIOSV_READ_MEPC(data) do { __asm ("csrr %0, mepc" : "=r" (data) ); } while (0)
	
#endif

// #include <unistd.h>
// #include <fcntl.h>


#include <stdio.h>
#include <stdarg.h>


// #include <io.h>
// #include <stdlib.h>

#include <malloc.h>
#include <string.h>  // for memcpy

#include <grtos_regs.h>
#include <grtosdefinitions.h>



#ifdef __niosX_arch__
// #include "sys/alt_stdio.h"  // for alt_printf
// #include <reent.h>

// 1 #include <sys/alt_llist.h>
// 1 #include "sys/alt_dev.h"

#include <altera_avalon_jtag_uart_regs.h>
#include "sys/alt_irq.h"
#include "priv/alt_irq_table.h"
// 2 #include <sys/alt_cache.h>
#include <inttypes.h>

#include <sys/alt_exceptions.h>

#include "alt_types.h"
alt_exception_result handler(alt_exception_cause cause, alt_u32 addr, alt_u32 bad_addr );


#endif // __niosX_arch__



#include <gemrtos_core.h>
#include <gemrtos_resources.h>


// #define _REENT (*(struct _reent **) (&_impure_ptr))
// #define _REENT ((struct _reent *) (&impure_data))

// 08-07-2024 #ifdef _REENT
// 08-07-2024 #undef _REENT
// 08-07-2024 #endif
// 08-07-2024 #define _REENT (__getreent_gemrtos())



#endif /* __GEMRTOS_H__ */

