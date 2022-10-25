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

// from https://stackoverflow.com/questions/1056411/how-to-pass-variable-number-of-arguments-to-printf-sprintf

/**gu_printf
 *  \brief This function is the printf function but for multiprocessor 
 *  \param Same as printf function of stdio.h
 *  \todo Change the mutex to a particular for newlib functions
 */
// void gu_printf(char *format, ...) 
// {
//     // from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
//     va_list args;
//     va_list args1;
//     va_start (args, format);
//     va_copy (args1, args);
//     GRTOS_USER_CRITICAL_SECTION_GET;
//     // printf(format, args);
//     // va_end (args);
//     // #######
//     // from https://stackoverflow.com/questions/12746885/why-use-asprintf-instead-of-sprintf    
//     const int BUF_LEN = 5;
//     char *x = malloc((BUF_LEN + 2) * sizeof(char));
//     // char *x1;
//     if (NULL != x) {
//         // printf("El x es %p\n",(void *) x);
// 
//         int size = snprintf(x, BUF_LEN, format, args);
//         va_end (args);
//         if (size >= BUF_LEN) {
//             x = realloc(x,(size + 2) * sizeof(char));
//             if (NULL != x) {
//                 // printf("El x es %p\n",(void *) x);
//                 snprintf(x, size + 1 , format, args1);
//                 va_end (args1);
//                 // printf("%s",x);
//                 // free(x);
//             }
//             else
//             {
//                 printf("[ ERROR ] Run out of memory for gu_printf");
//                 // free(x);
//             }              
//         } 
//         printf("%s",x);
//         free(x);
//     }
//     else
//     {
//         printf("[ ERROR ] Run out of memory for gu_printf");
//         // free(x);
//     }
//     GRTOS_CMD_CRITICAL_SECTION_RELEASE;
// }


void gu_printf(char *format, ...) 
{
    GRTOS_USER_CRITICAL_SECTION_GET;
    
    // from https://stackoverflow.com/questions/12746885/why-use-asprintf-instead-of-sprintf    
    const int BUF_LEN = 255;
    char *x = malloc((BUF_LEN + 2) * sizeof(char));

    if (NULL != x) {
        // from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
        va_list args;        
        va_start (args, format);
        int size = vsnprintf(x, BUF_LEN, format, args);
        va_end (args);
        printf("%s",x);

    }
    free(x);    
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
}

/**gu_printf
 *  \brief This function is the fprintf function but for multiprocessor 
 *  \param Same as fprintf function of stdio.h
 *  \todo Change the mutex to a particular for newlib functions
 */
void gu_fprintf(FILE *stream, char *format, ...) 
{
    GRTOS_USER_CRITICAL_SECTION_GET;
    
    // from https://stackoverflow.com/questions/12746885/why-use-asprintf-instead-of-sprintf    
    const int BUF_LEN = 255;
    char *x = malloc((BUF_LEN + 2) * sizeof(char));

    if (NULL != x) {
        // from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
        va_list args;        
        va_start (args, format);
        int size = vsnprintf(x, BUF_LEN, format, args);
        va_end (args);
        fprintf(stream, "%s", x);
    }
    free(x);    
    GRTOS_CMD_CRITICAL_SECTION_RELEASE;
}


#include <reent.h>

/*
 * These are the empty malloc lock/unlock stubs required by newlib. These are 
 * used to make newlib's malloc() function thread safe. The default HAL 
 * configuration is single threaded, so there is nothing to do here. Note that 
 * this requires that malloc is never called by an interrupt service routine.
 */

void __malloc_lock ( struct _reent *_r )
{
    // GRTOS_CMD_NEWLIB_MUTEX_GET;
    // unsigned int processor = (1 << (GRTOS_CMD_PRC_ID - 1));
    // IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_NEWLIB_GRN, GRTOS_CMD_PRC_ID);
    // printf("Lock: Processor= %d, mux= %d\n",(int) processor, (int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_NEWLIB_GRN));
	//	do { \
    //        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_NEWLIB_GRN, GRTOS_CMD_PRC_ID); \
    //    } while (processor != IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_NEWLIB_GRN));  \    
    // GRTOS_CMD_NEWLIB_MUTEX_GET;
}

/*
 *
 */

void __malloc_unlock ( struct _reent *_r )
{
    // GRTOS_CMD_NEWLIB_MUTEX_RELEASE;
}
