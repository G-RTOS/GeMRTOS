/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Wrap for gemrtos_hal functions for GeMRTOS
 *  \details This file contains the hal redefinitions.
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

#include <gemrtos.h>


/**
 *  \brief The gu_printf function formats text and writes it to standard output.
 *  \exception par_following The following parameter is required for the gu_printf function:
 *  \param [in] format A string that may contain format specifiers like %d, %s, etc., which control the formatting of subsequent arguments.
 *  \return The gu_printf function does not return any value.
 *  \relates System 
 *  \remark gu_printf
 */
void gu_printf(char *format, ...) 
{
	
	if (gu_SemaphoreWait(gu_printf_sem, (int) G_TRUE) == G_TRUE) {

		// from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170 // !!!!D
		
		// from https://stackoverflow.com/questions/1056411/how-to-pass-variable-number-of-arguments-to-printf-sprintf  // !!!!D
		// from https://stackoverflow.com/questions/150543/forward-an-invocation-of-a-variadic-function-in-c // !!!!D
		va_list args;        
		va_start (args, format);
		vprintf(format, args);
		va_end (args);
		
		gu_SemaphorePost((G_RCB *) gu_printf_sem);
	}	
}

 
/**
 *  \brief The gu_fprintf function formats text and writes it to standard error output (stderr).
 *  \exception par_following The following parameter is required for the gu_fprintf function:
 *  \param [in] format A string that may contain format specifiers like %d, %s, etc., which control the formatting of subsequent arguments.
 *  \return The gu_fprintf function does not return any value.
 *  \relates System 
 *  \remark gu_fprintf
 */
void gu_fprintf(char *format, ...) 
{

	/* if semaphore is granted, prints message */
	if (gu_SemaphoreWait(gu_printf_sem, (int) G_TRUE) == G_TRUE) {

		// from https://stackoverflow.com/questions/12746885/why-use-asprintf-instead-of-sprintf    // !!!!D
		const unsigned int BUF_LEN = 254;
		char x[BUF_LEN + 2];
		x[BUF_LEN + 1] = '\0';
		
		// from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170 // !!!!D
		va_list args;        
		va_start (args, format);
		vsnprintf(x, BUF_LEN, format, args);
		va_end (args);
		
	#ifdef __niosX_arch__                                                         // !!!!D
		fprintf(stderr, "%s", x);
		// printf("%s", x);                                                       // !!!!D
	#else                                                                         // !!!!D
		printf("%s", x);                                                          // !!!!D
	#endif                                                                        // !!!!D
		
		gu_SemaphorePost((G_RCB *) gu_printf_sem);
	}
}

#ifdef __niosX_arch__          // !!!!D
							   // !!!!D
// #include <reent.h>          // !!!!D

/*
 * These are the malloc lock/unlock stubs required by newlib. These are 
 * used to make newlib's malloc() function thread safe. 
 */

void __malloc_lock ( struct _reent *_r )
{
	gm_GeMRTOSCriticalSectionEnter;
}    

/*   
 *   
 */ 
void  __malloc_unlock ( struct _reent *_r )
{    
	gm_GeMRTOSCriticalSectionExit;
}

/*
 * These are the empty env lock/unlock stubs required by newlib. These are 
 * used to make accesses to environment variables thread safe. The default HAL 
 * configuration is single threaded, so there is nothing to do here. Note that 
 * this requires that environment variables are never manipulated by an interrupt 
 * service routine.
 */
void __env_lock ( struct _reent *_r )
{
	gm_GeMRTOSCriticalSectionEnter;
}

/*
 *
 */
void __env_unlock ( struct _reent *_r )
{
	gm_GeMRTOSCriticalSectionExit;
}


// #define __DYNAMIC_REENT__                                                                          // !!!!D
// #ifdef __SINGLE_THREAD__                                                                           // !!!!D
// #undef __SINGLE_THREAD__                                                                           // !!!!D
// #endif                                                                                             // !!!!D
//                                                                                                    // !!!!D
// #include <_ansi.h>                                                                                 // !!!!D
// #include <reent.h>                                                                                 // !!!!D
// #include <errno.h>                                                                                 // !!!!D
//                                                                                                    // !!!!D
// #ifdef __getreent                                                                                  // !!!!D
// #undef __getreent                                                                                  // !!!!D
// #endif                                                                                             // !!!!D
//                                                                                                    // !!!!D
// #ifdef _REENT                                                                                      // !!!!D
// #undef _REENT                                                                                      // !!!!D
// #endif                                                                                             // !!!!D
//                                                                                                    // !!!!D
// #ifdef __errno                                                                                     // !!!!D
// #undef __errno                                                                                     // !!!!D
// #endif                                                                                             // !!!!D
																									  // !!!!D
// 15-07-2024 struct _reent *__getreent_gemrtos(void)                                                 // !!!!D
// 15-07-2024 {	                                                                                      // !!!!D
// 15-07-2024 	if ( IORD_GRTOS_RST_CLR == G_NUMBER_OF_PCB ) {                                        // !!!!D
// 15-07-2024 		if (g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB == G_TRUE)                // !!!!D
// 15-07-2024 			return g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->_impure_ptr;          // !!!!D
// 15-07-2024 		else                                                                              // !!!!D
// 15-07-2024 			return g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->_impure_ptr;          // !!!!D
// 15-07-2024 	} else {                                                                              // !!!!D
// 15-07-2024 		return _impure_ptr;                                                               // !!!!D
// 15-07-2024 	}                                                                                     // !!!!D
// 15-07-2024 }                                                                                       // !!!!D
																									  // !!!!D
// struct _reent *__getreent (void)                                                                   // !!!!D
// {                                                                                                  // !!!!D
//   // while (1);                                                                                    // !!!!D
//   if (gm_ProcessorId == GRTOS_MTX_PRC_GRANTED)                                                   // !!!!D
//   {                                                                                                // !!!!D
//       return g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->_impure_ptr;                         // !!!!D
//   }                                                                                                // !!!!D
//   else                                                                                             // !!!!D
//   {                                                                                                // !!!!D
//       return g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->_impure_ptr;                         // !!!!D
//   }                                                                                                // !!!!D
//   // return _impure_ptr;                                                                           // !!!!D
// }                                                                                                  // !!!!D
																									  // !!!!D
// #define _REENT (__getreent())                                                                      // !!!!D
//                                                                                                    // !!!!D
// #define errno (*___errno())                                                                        // !!!!D
// extern int *___errno _PARAMS ((void));                                                             // !!!!D
//                                                                                                    // !!!!D
// int *                                                                                              // !!!!D
// ___errno ()                                                                                        // !!!!D
// {                                                                                                  // !!!!D
// return &_REENT->_errno;                                                                            // !!!!D
// }                                                                                                  // !!!!D


#endif              // !!!!D
