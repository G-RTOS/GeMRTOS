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




