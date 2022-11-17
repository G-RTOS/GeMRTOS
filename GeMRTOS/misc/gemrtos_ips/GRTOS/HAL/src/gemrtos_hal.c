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

/// void gu_printf(char *format, ...) 
/// {
///     GRTOS_USER_CRITICAL_SECTION_GET;
///    //     va_list args;
//     // va_list args1;
//     // va_start (args, format);
//     // va_copy (args1, args);
///     // from https://stackoverflow.com/questions/12746885/why-use-asprintf-instead-of-sprintf    
///     const int BUF_LEN = 255;
///     char *x = malloc((BUF_LEN + 2) * sizeof(char));
/// 
///     if (NULL != x) {
///         // from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
///         va_list args;        
///         va_start (args, format);
///         int size = vsnprintf(x, BUF_LEN, format, args);
///         va_end (args);
///         printf("%s",x);
/// 
///     }
///     free(x);    
///     GRTOS_CMD_CRITICAL_SECTION_RELEASE;
/// }

// int __real_printf (const char *__format, ...);

int __wrap_printf(const char *__format, ...)
{
    GEMRTOS_NEWLIB_LOCK;

    // from https://stackoverflow.com/questions/150543/forward-an-invocation-of-a-variadic-function-in-c
    va_list args;        
    va_start (args, __format);
    vprintf(__format, args);
    va_end (args);    
    // __real_printf("__wrap_printf");
    GEMRTOS_NEWLIB_UNLOCK;
    return(G_TRUE);
}

void gu_printf(char *format, ...) 
{
    GEMRTOS_NEWLIB_LOCK;
    
    // from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
    /// va_list args;        
    /// va_start (args, format);
    /// printf(format, args);
    /// va_end (args);
    
    // from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
    // from https://stackoverflow.com/questions/1056411/how-to-pass-variable-number-of-arguments-to-printf-sprintf    
    /// const int BUF_LEN = 255;
    /// char x[BUF_LEN+1];
    /// va_list args;        
    /// va_start (args, format);
    /// int size = vsnprintf(x, BUF_LEN, format, args);
    /// va_end (args);
    /// printf(x);

    // from https://stackoverflow.com/questions/150543/forward-an-invocation-of-a-variadic-function-in-c
    va_list args;        
    va_start (args, format);
    vprintf(format, args);
    va_end (args);    

    GEMRTOS_NEWLIB_UNLOCK;
}


/**gu_fprintf
 *  \brief This function is the fprintf function but for multiprocessor 
 *  \param Same as fprintf function of stdio.h
 *  \todo Change the mutex to a particular for newlib functions
 */
void gu_fprintf(FILE *stream, char *format, ...) 
{
    GEMRTOS_NEWLIB_LOCK;
    
    // from https://stackoverflow.com/questions/12746885/why-use-asprintf-instead-of-sprintf    
    const int BUF_LEN = 255;
    char *x = malloc((BUF_LEN + 2) * sizeof(char));

    if (NULL != x) {
        // from https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
        va_list args;        
        va_start (args, format);
        vsnprintf(x, BUF_LEN, format, args);
        va_end (args);
        fprintf(stream, "%s", x);
    }
    free(x);    
    GEMRTOS_NEWLIB_UNLOCK;
}



#include <reent.h>

/*
 * These are the empty malloc lock/unlock stubs required by newlib. These are 
 * used to make newlib's malloc() function thread safe. The default HAL 
 * configuration is single threaded, so there is nothing to do here. Note that 
 * this requires that malloc is never called by an interrupt service routine.
 */

volatile unsigned int new_lib_grant;
volatile unsigned int new_lib_counter;



void __malloc_lock ( struct _reent *_r )
{
    GEMRTOS_NEWLIB_LOCK;
}    

/*   
 *   
 */  


void  __malloc_unlock ( struct _reent *_r )
{    
    GEMRTOS_NEWLIB_UNLOCK;
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
    GEMRTOS_NEWLIB_LOCK;    
}

/*
 *
 */

void __env_unlock ( struct _reent *_r )
{
    GEMRTOS_NEWLIB_UNLOCK;
}


/// Interception of newlib syscall write
int GEMRTOS_WRITE (int file, const void *ptr, size_t len)
{
    GEMRTOS_NEWLIB_LOCK;
    int a = ALT_WRITE (file, ptr, len);
    GEMRTOS_NEWLIB_UNLOCK;
    return a;
}

/// Interception of newlib syscall read
int GEMRTOS_READ (int file, void *ptr, size_t len)
{
    GEMRTOS_NEWLIB_LOCK;
    int a = ALT_READ (file, ptr, len);
    GEMRTOS_NEWLIB_UNLOCK;
    return a;
}