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

/**gu_printf
 *  \brief This function is the printf function but for multiprocessor 
 *  \param format as printf function of stdio.h
 *  \todo Change the mutex to a particular for newlib functions
 */
void gu_printf(char *format, ...) 
{
	
	if (gu_SemaphoreWait(gu_printf_sem, (int) G_TRUE) == G_TRUE) {

		
		va_list args;        
		va_start (args, format);
		vprintf(format, args);
		va_end (args);
		
		gu_SemaphorePost((G_RCB *) gu_printf_sem);
	}	
}

 
/** gu_fprintf
 *  \brief This function is the fprintf function but for multiprocessor 
 *  \param format Same as fprintf function of stdio.h
 */
void gu_fprintf(char *format, ...) 
{

	/* if semaphore is granted, prints message */
	if (gu_SemaphoreWait(gu_printf_sem, (int) G_TRUE) == G_TRUE) {

		const unsigned int BUF_LEN = 254;
		char x[BUF_LEN + 2];
		x[BUF_LEN + 1] = '\0';
		
		va_list args;        
		va_start (args, format);
		vsnprintf(x, BUF_LEN, format, args);
		va_end (args);
		
		fprintf(stderr, "%s", x);
		
		gu_SemaphorePost((G_RCB *) gu_printf_sem);
	}
}


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




