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


// from https://stackoverflow.com/questions/1056411/how-to-pass-variable-number-of-arguments-to-printf-sprintf


// 06-09-2023 int __wrap_printf(const char *__format, ...)
// 06-09-2023 {
// 06-09-2023     GEMRTOS_NEWLIB_LOCK;
// 06-09-2023 
// 06-09-2023     // from https://stackoverflow.com/questions/150543/forward-an-invocation-of-a-variadic-function-in-c
// 06-09-2023     va_list args;        
// 06-09-2023     va_start (args, __format);
// 06-09-2023     vprintf(__format, args);
// 06-09-2023     va_end (args);    
// 06-09-2023     // __real_printf("__wrap_printf");
// 06-09-2023     GEMRTOS_NEWLIB_UNLOCK;
// 06-09-2023     return(G_TRUE);
// 06-09-2023 }

/**gu_printf
 *  \brief This function is the printf function but for multiprocessor 
 *  \param format as printf function of stdio.h
 *  \todo Change the mutex to a particular for newlib functions
 */
void gu_printf(char *format, ...) 
{
	
	if (gu_sem_wait(gu_printf_sem, (int) G_TRUE) == G_TRUE) {
		
		// 23-02-2024 GS_PCB *ppcb = &g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1];
		// 23-02-2024 if (ppcb->PCB_Critical_PCB == G_FALSE) {
	// 15-07-2024 GRTOS_USER_CRITICAL_SECTION_GET;
		// 23-02-2024 }
		// GK_SAMPLE_FUNCTION_BEGIN(10055)

		// from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
		
		// 05-007-2024 PROC_CRITICAL_SECTION_START
		// from https://stackoverflow.com/questions/1056411/how-to-pass-variable-number-of-arguments-to-printf-sprintf 
		// from https://stackoverflow.com/questions/150543/forward-an-invocation-of-a-variadic-function-in-c
		va_list args;        
		va_start (args, format);
		vprintf(format, args);
		va_end (args);
		
		// 05-007-2024 PROC_CRITICAL_SECTION_END
		// GK_SAMPLE_FUNCTION_END(10055)

		// 23-02-2024 if (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB == G_FALSE) {
	// 15-07-2024 GRTOS_USER_CRITICAL_SECTION_RELEASE;
		// 23-02-2024 }
		gu_sem_post((G_RCB *) gu_printf_sem);
	}	
}

 
/** gu_fprintf
 *  \brief This function is the fprintf function but for multiprocessor 
 *  \param format Same as fprintf function of stdio.h
 */
void gu_fprintf(char *format, ...) 
{
    // GU_SAMPLE_FUNCTION_BEGIN();
	
	// int sem = (int) gu_sem_wait(gu_fprintf_sem, (gt_priority) 0, (int) G_TRUE);

	/* if semaphore is granted, prints message */
    // 27-06-2024 if (gu_sem_wait(gu_fprintf_sem, (int) G_TRUE) == G_TRUE) {
	if (gu_sem_wait(gu_printf_sem, (int) G_TRUE) == G_TRUE) {

		// 02-01-2024 if (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB == G_FALSE) {
	// 15-07-2024 GRTOS_USER_CRITICAL_SECTION_GET;
		// 02-01-2024 }
		
		// from https://stackoverflow.com/questions/12746885/why-use-asprintf-instead-of-sprintf    
		const unsigned int BUF_LEN = 254;
		char x[BUF_LEN + 2];
		x[BUF_LEN + 1] = '\0';
		
		// 05-007-2024 PROC_CRITICAL_SECTION_START
		
		// from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
		va_list args;        
		va_start (args, format);
		vsnprintf(x, BUF_LEN, format, args);
		va_end (args);
	#ifdef __niosX_arch__
		fprintf(stderr, "%s", x);
		// printf("%s", x);
	#else
		printf("%s", x);
	#endif
		
		// 05-007-2024 PROC_CRITICAL_SECTION_END
		
		// i = gu_queue_send(pqueue, (void *) message_tcb0, (G_INT32) (strlen(message_tcb0) + 1), (gt_time) 0);
		
		// 02-01-2024 if (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB == G_FALSE) {
	// 15-07-2024 GRTOS_USER_CRITICAL_SECTION_RELEASE;
		// 02-01-2024 }
		// GU_SAMPLE_FUNCTION_END();
		
		// 27-06-2024 gu_sem_post((G_RCB *) gu_fprintf_sem);
		gu_sem_post((G_RCB *) gu_printf_sem);
	}
}

#ifdef __niosX_arch__

// #include <reent.h>

/*
 * These are the empty malloc lock/unlock stubs required by newlib. These are 
 * used to make newlib's malloc() function thread safe. The default HAL 
 * configuration is single threaded, so there is nothing to do here. Note that 
 * this requires that malloc is never called by an interrupt service routine.
 */


// struct _reent *_impure_return;

void __malloc_lock ( struct _reent *_r )
{
	GRTOS_USER_CRITICAL_SECTION_GET;
}    

/*   
 *   
 */ 
void  __malloc_unlock ( struct _reent *_r )
{    
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
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
	GRTOS_USER_CRITICAL_SECTION_GET;
}

/*
 *
 */
void __env_unlock ( struct _reent *_r )
{
	GRTOS_USER_CRITICAL_SECTION_RELEASE;
}


// #define __DYNAMIC_REENT__
// #ifdef __SINGLE_THREAD__
// #undef __SINGLE_THREAD__
// #endif
// 
// #include <_ansi.h>
// #include <reent.h>
// #include <errno.h>
// 
// #ifdef __getreent
// #undef __getreent
// #endif
// 
// #ifdef _REENT
// #undef _REENT
// #endif
// 
// #ifdef __errno
// #undef __errno
// #endif

// 15-07-2024 struct _reent *__getreent_gemrtos(void)
// 15-07-2024 {	
// 15-07-2024 	if ( IORD_GRTOS_RST_CLR == G_NUMBER_OF_PCB ) {
// 15-07-2024 		if (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB == G_TRUE)
// 15-07-2024 			return g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->_impure_ptr;
// 15-07-2024 		else
// 15-07-2024 			return g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->_impure_ptr;
// 15-07-2024 	} else {
// 15-07-2024 		return _impure_ptr;
// 15-07-2024 	}
// 15-07-2024 }

// struct _reent *__getreent (void)
// {
//   // while (1);
//   if (GRTOS_CMD_PRC_ID == GRTOS_MTX_PRC_GRANTED) 
//   {
//       return g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->_impure_ptr;
//   }
//   else
//   {
//       return g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->_impure_ptr;
//   }
//   // return _impure_ptr;
// }

// #define _REENT (__getreent())
// 
// #define errno (*___errno())
// extern int *___errno _PARAMS ((void));
// 
// int *
// ___errno ()
// {
// return &_REENT->_errno;
// }


#endif
