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
        int size = vsnprintf(x, BUF_LEN, format, args);
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

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/

#include <unistd.h>
#include <fcntl.h>

#include <sys/alt_errno.h>

#include "sys/alt_errno.h"
#include "sys/alt_warning.h"
#include "priv/alt_file.h"
#include "os/alt_syscall.h"

#include "sys/alt_log_printf.h"

/*
 * The write() system call is used to write a block of data to a file or 
 * device. This function simply vectors the request to the device driver 
 * associated with the input file descriptor "file". 
 *
 * ALT_WRITE is mapped onto the write() system call in alt_syscall.h
 */



#ifdef ALT_USE_DIRECT_DRIVERS

#include "system.h"
#include "sys/alt_driver.h"

/*
 * Provide minimal version that just writes to the stdout/stderr devices
 * when provided.
 */

int ALT_WRITE (int file, const void *ptr, size_t len)
{
#ifdef ALT_STDOUT_PRESENT
    ALT_DRIVER_WRITE_EXTERNS(ALT_STDOUT_DEV);
#endif
#ifdef ALT_STDERR_PRESENT
    ALT_DRIVER_WRITE_EXTERNS(ALT_STDERR_DEV);
#endif

#if !defined(ALT_STDOUT_PRESENT) && !defined(ALT_STDERR_PRESENT)
    /* Generate a link time warning, should this function ever be called. */
    ALT_STUB_WARNING(write);
#endif

    switch (file) {
#ifdef ALT_STDOUT_PRESENT
    case 1: /* stdout file descriptor */
        return ALT_DRIVER_WRITE(ALT_STDOUT_DEV, ptr, len, 0);
#endif /* ALT_STDOUT_PRESENT */
#ifdef ALT_STDERR_PRESENT
    case 2: /* stderr file descriptor */
        return ALT_DRIVER_WRITE(ALT_STDERR_DEV, ptr, len, 0);
#endif /* ALT_STDERR_PRESENT */
    default:
        ALT_ERRNO = EBADFD;
        return -1;
    }
}

#else /* !ALT_USE_DIRECT_DRIVERS */

int ALT_WRITE (int file, const void *ptr, size_t len)
{
  alt_fd*  fd;
  int      rval;

    GEMRTOS_NEWLIB_LOCK;

  /*
   * A common error case is that when the file descriptor was created, the call
   * to open() failed resulting in a negative file descriptor. This is trapped
   * below so that we don't try and process an invalid file descriptor.
   */

  fd = (file < 0) ? NULL : &alt_fd_list[file];
  
  if (fd)
  {
    /*
     * If the file has not been opened with write access, or if the driver does
     * not provide an implementation of write(), generate an error. Otherwise
     * call the drivers write() function to process the request.
     */

    if (((fd->fd_flags & O_ACCMODE) != O_RDONLY) && fd->dev->write)
    {
      
      /* ALT_LOG - see altera_hal/HAL/inc/sys/alt_log_printf.h */
      ALT_LOG_WRITE_FUNCTION(ptr,len);

      if ((rval = fd->dev->write(fd, ptr, len)) < 0)
      {
        ALT_ERRNO = -rval;
        GEMRTOS_NEWLIB_UNLOCK;
        return -1;
      }
      GEMRTOS_NEWLIB_UNLOCK;
      return rval;
    }
    else
    {
      ALT_ERRNO = EACCES;
    }
  }
  else  
  {
    ALT_ERRNO = EBADFD;
  }
  GEMRTOS_NEWLIB_UNLOCK;
  return -1;
}

#endif /* ALT_USE_DIRECT_DRIVERS */
