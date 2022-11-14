#ifndef __GEMRTOS_NEWLIB_SYSCALL_H__
#define __GEMRTOS_NEWLIB_SYSCALL_H__

#include "os/alt_syscall.h"

/*
 * The macros defined in this file are used to provide the function names used
 * for the HAL 'UNIX style' interface, e.g. read(), write() etc. 
 *
 * Operating systems which are ported to the HAL can provide their own 
 * version of this file, which will be used in preference. This allows
 * the operating system to provide it's own implementation of the top level
 * system calls, while retaining the HAL functions under a different name, 
 * for example, alt_read(), alt_write() etc.
 */

#undef ALT_CLOSE        
#undef ALT_ENVIRON      
#undef ALT_EXECVE       
#undef ALT_EXIT         
#undef ALT_FCNTL        
#undef ALT_FORK         
#undef ALT_FSTAT        
#undef ALT_GETPID       
#undef ALT_GETTIMEOFDAY 
#undef ALT_IOCTL        
#undef ALT_ISATTY       
#undef ALT_KILL         
#undef ALT_LINK         
#undef ALT_LSEEK        
#undef ALT_OPEN         
#undef ALT_READ         
#undef ALT_RENAME       
#undef ALT_SBRK         
#undef ALT_SETTIMEOFDAY 
#undef ALT_STAT         
#undef ALT_UNLINK       
#undef ALT_USLEEP       
#undef ALT_WAIT         
#undef ALT_WRITE        
#undef ALT_TIMES        



#define ALT_CLOSE        close
#define ALT_ENVIRON      environ
#define ALT_EXECVE       execve
#define ALT_EXIT         _exit
#define ALT_FCNTL        fcntl
#define ALT_FORK         fork
#define ALT_FSTAT        fstat
#define ALT_GETPID       getpid
#define ALT_GETTIMEOFDAY gettimeofday
#define ALT_IOCTL        ioctl
#define ALT_ISATTY       isatty
#define ALT_KILL         kill
#define ALT_LINK         link
#define ALT_LSEEK        lseek
#define ALT_OPEN         open

#define ALT_READ         newlib_read
#define GEMRTOS_READ     read

#define ALT_RENAME       _rename
#define ALT_SBRK         sbrk
#define ALT_SETTIMEOFDAY settimeofday
#define ALT_STAT         stat
#define ALT_UNLINK       unlink
#define ALT_USLEEP       usleep
#define ALT_WAIT         wait

#define ALT_WRITE        newlib_write
#define GEMRTOS_WRITE    write

#define ALT_TIMES        times

/*
 *
 */

#endif /* __GEMRTOS_NEWLIB_SYSCALL_H__ */
