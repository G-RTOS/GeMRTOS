/// \file 
/// \author Ricardo Cayssials
/// \brief GRTOS general definition
/// \details Main file for definition
/// \version 0.9a
/// \date      Created on: Sep 16, 2013
/// \bug   None known
/// \copyright Ricardo Cayssials 
/// \copyright All rights reserved.
/// \warning This product was produced by Ricardo Cayssials in the hope that it 
/// will be useful, but WITHOUT ANY WARRANTY; without even the implied 
/// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
/// \warning DISCLAIMER OF WARRANTY. THIS PRODUCT IS LICENSED "AS IS." YOU BEAR THE RISK OF USING IT. 
/// \warning RICARDO CAYSSIALS GIVES NO EXPRESS WARRANTIES, GUARANTEES, OR CONDITIONS. 
/// TO THE EXTENT PERMITTED UNDER APPLICABLE LAWS, RICARDO CAYSSIALS EXCLUDES ALL IMPLIED WARRANTIES,
/// INCLUDING MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.



/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) Ricardo L. Cayssials                                          *
* All rights reserved.                                                        *
*                                                                             *
******************************************************************************/

#ifndef GRTOSDEFINITIONS_H_
#define GRTOSDEFINITIONS_H_

// Fijarse aca para separar el codigo del switch
// https://stackoverflow.com/questions/4550075/can-i-append-to-a-preprocessor-macro
// https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html
// https://gcc.gnu.org/onlinedocs/cpp/Directives-Within-Macro-Arguments.html#Directives-Within-Macro-Arguments

// Check https://stackoverflow.com/questions/1154709/how-can-i-hide-the-declaration-of-a-struct-in-c


// #pragma GCC optimize('mbypass-cache')
// #pragma GCC optimize('mno-cache-volatile')

#include <system.h>
#include <stdio.h>
#include "sys/alt_irq.h"
#include "priv/alt_irq_table.h"
#include <sys/alt_cache.h>
#include <inttypes.h>
#include <assert.h>
#include <grtos_regs.h>

#include "../runtime/rts_gemrtos.h"



/****************************************************************************************
 *                   GRTOS SYSTEM GENERAL DEFINITIONS
 ***************************************************************************************/
#define G_ISR_STACKSIZE                          4096  ///< \brief Size of the stack of an Interrupt task 
#define G_ISR_PRIORITY                              0  ///< \brief Priority assigned to an Interrupt task 
#define G_IDLE_STACKSIZE                         4096  ///< \brief Size of the stack of an Idle task      
#define G_NUMBER_OF_LCBs_FOR_PCB                   2u  ///< \brief Number of LCB list for PCB             


/************************************************************************************
 *  COMPILATION OPTIONS
 ************************************************************************************/
 
// from https://stackoverflow.com/questions/2219829/how-to-prevent-gcc-optimizing-some-statements-in-c
// #pragma GCC push_options
// #pragma GCC optimize ("O3")

// from https://stackoverflow.com/questions/64026719/gcc-cli-option-o0-surprisingly-discards-code-after-pragma-gcc-optimize-in-macr
#define _STRING(S) #S
#define STRING(S) _STRING(S)
#define OPTIMEZE_CODE(OLEVEL) \
    _Pragma("GCC push_options") \
    _Pragma(STRING(GCC optimize ("O3")))

// _Pragma(STRING(GCC optimize ("O" #OLEVEL)))

#define OPTIMEZE_RESTORE \
    _Pragma("GCC pop_options")  

// from https://stackoverflow.com/questions/58477537/labels-redefined-in-macro-in-c
#define RTSZ(i) RTSZ1(i, __LINE__, __FUNCTION__)
#define RTSZ1(i, l, f) RTSZ2(i, l, f)
#define RTSZ2(i, l, f) RTSZ_##i##_##l##_##f

    //  if (G_Running == G_TRUE) { 
    //      fprintf(fpuart[GRTOS_CMD_PRC_ID-1],"Proc %d in func %s, file %s, %d \n",GRTOS_CMD_PRC_ID , __FUNCTION__,__FILE__,__LINE__); 
    //  } 

#define SAMPLE_FUNCTION_BEGIN(number) \
    if ((G_DEBUG_SAMPLE_BEGIN_ENABLE == 0xFFFFFFFF) ||  (G_DEBUG_SAMPLE_BEGIN_ENABLE == number)){ \
        RTSZ(0):  \
        IOWR(GRTOS_DRIVER_GRTOS_MONITOR_BASE, 0, (unsigned int) &&RTSZ(0) | 0x80000000); \
    }

    // if (G_Running == G_TRUE) { 
    //     if ((g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackPointer < g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackTop - 300) || (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackPointer > g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackBottom)) { 
    //         PRINT_TO_DEBUG("ERROR TCB= %p, TCB_StackBottom = %p, TCB_StackPointer = %p, TCB_StackTop = %p\n",g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB, (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackBottom, (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackPointer, (void *)  g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackTop); 
    //         PRINT_TO_DEBUG("ERROR TCB_IDLE= %p\n", (void *) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB ); 
    //         G_DEBUG_WHILEFOREVER; } 
    // } 
    
#define SAMPLE_FUNCTION_END(number) \
    if ((G_DEBUG_SAMPLE_END_ENABLE == 0xFFFFFFFF) ||  (G_DEBUG_SAMPLE_END_ENABLE == number)){ \
        RTSZ(0):  \
        IOWR(GRTOS_DRIVER_GRTOS_MONITOR_BASE, 0, (unsigned int) &&RTSZ(0) | 0xC0000000); \
    }


/************************************************************************************
 *  GRTOS INTERNAL DEFINITIONS
 ************************************************************************************/
/// \brief G_NUMBER_OF_PCB Number of processors defined in Qsys/Platform Designer
/// \todo Define a function to get this value during runtime
#define G_NUMBER_OF_PCB      GRTOS_DRIVER_NPROCESSORS  /**    */

/// \brief G_NUMBER_OF_IRQ_TCB Number of IRQs defined in Qsys/Platform Designer
/// \todo Define a function to get this value during runtime
#define G_NUMBER_OF_IRQ_TCB			         ALT_NIRQ  

/// \brief G_TICKS_PER_SECOND 
/// \todo Define a function to get this value during runtime
#define G_TICKS_PER_SECOND  (int)(GRTOS_DRIVER_GRTOSFREQUENCY/(GRTOS_DRIVER_PRESCALE))

/// \brief G_TICKS_PER_MSECOND 
/// \todo Define a function to get this value during runtime
#define G_TICKS_PER_MSECOND (G_TICKS_PER_SECOND/1000)


/// \Brief Event codes for gk_ENTRY_IRQ_HANDLER
#define EVN_CODE_TIMED  0x21
#define EVN_CODE_FROZEN 0x22
/************************************************************************************
 *  COMPILATION OPTIONS
 ************************************************************************************/
/**
 *  G_GRTOS_PRESERVE_HAL_ISR
 *  If 1, the HAL ISR are preserved in GRTOS (producing overhead)
 *  If 0, no ISR are preserved
 */
#define G_GRTOS_PRESERVE_HAL_ISR 0

/**
 *  G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE
 *  Used to print to the processor console when processor goes to IDLE
 *  when 1: message is printed when proessor goes to IDLE
 *  when 0: no message is printed
 */
#define G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE 1

/**
 * G_DEBUG_WHILEFOREVER_ENABLE
 * Used to insert code to check during runtime data conistency in several functions
 * when 1: checks are enable
 * when 0: checks are disable
 */
#define G_DEBUG_WHILEFOREVER_ENABLE  1


/**
 * G_DEBUG_WHILEFOREVER
 * Executed when G_DEBUG_WHILEFOREVER_ENABLE is 1 and an inconsistency is found
 */
#define G_DEBUG_WHILEFOREVER \
			 do{int dest; \
                 fprintf(fpuart[GRTOS_CMD_PRC_ID-1],"ERROR in for Proc %d in function %s, file = %s, line = %d \n",GRTOS_CMD_PRC_ID , __FUNCTION__,__FILE__,__LINE__); \
				 while(1){dest++;\
			 }}while(0)


/// \todo Check if it required
#define G_DEBUG_RUN_MONITOR_ROUTINE \
    do{ \
        GRTOS_CMD_DEBUG_HOLD_ENB; \
        fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "In %s, %d, proc %x, mtx %x \n",__FUNCTION__,__LINE__, GRTOS_CMD_PRC_ID,GRTOS_MTX_PRC_GRANTED); \
        GRTOS_CMD_DEBUG_HOLD_DIS; } while(0)

/**
 * PRINT_TO_DEBUG
 * print to the stderr console if open, open it otherwise
 */
#define PRINT_TO_DEBUG(...) \
    fprintf(fpuart[GRTOS_CMD_PRC_ID-1], ##__VA_ARGS__);

    
/// \todo Check if it required
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	#define G_DEBUG_VERBOSE_RUNTIME \
        GRTOS_CMD_DEBUG_HOLD_ENB; \
        G_DEBUG_RUN_MONITOR_ROUTINE; \
		fprintf(fpuart[GRTOS_CMD_PRC_ID-1],"Proc %d, func %s \n",GRTOS_CMD_PRC_ID , __FUNCTION__); \
        GRTOS_CMD_DEBUG_HOLD_DIS; 
#else
	#define G_DEBUG_VERBOSE_RUNTIME
#endif

/// \todo Check if it required
//#define G_DEBUG_VERBOSE G_DEBUG_RUN_MONITOR_ROUTINE
//#define G_DEBUG_VERBOSE G_DEBUG_VERBOSE_RUNTIME 
#define G_DEBUG_VERBOSE  

/**
 *  \brief PRINT_DEBUG_LINE can be defined to print a message after the execution of each line of the code.
 *  It is defined as empty when no in debug mode
 *  /// \todo Check if it required
 */
// #define PRINT_DEBUG_LINE  fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "%s, %s, %d, proc %x, mtx %x\n",,__FILE__,__LINE__, GRTOS_CMD_PRC_ID, GRTOS_MTX_PRC_GRANTED);
// #define PRINT_DEBUG_LINE  fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "%s, %d\n",__FUNCTION__,__LINE__);       
#define PRINT_DEBUG_LINE  











/**
*********************************************************************************************************
*                            TIME and PRIORITY definitions
*********************************************************************************************************
**/ 
#define G_LOWEST_PRIORITY    0x7FFFFFFFFFFFFFFCLL  ///< Lowest Priority of a Task
#define G_IDLETASK_PRIORITY  0x7FFFFFFFFFFFFFFDLL  ///< Priority of Idle Task
#define G_LATEST_TIME        0x7FFFFFFFFFFFFFFFLL  ///< Lastest Occurence Time of an EVENT




#endif /* GRTOSDEFINITIONS_H_ */
