/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GRTOS general definition
 *  \details Main file for definition
 *  \version 0.9a
 *  \date      2015-2020
 *  \bug  Describe found bugs here.
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
* Copyright (c) 2013,2023 Ricardo L. Cayssials                                *
* All rights reserved.                                                        *
*                                                                             *
******************************************************************************/


#ifndef GRTOSDEFINITIONS_H_
#define GRTOSDEFINITIONS_H_



// Fijarse aca para separar el codigo del switch
// https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html

// Check for nios2 and nios V for validity
// #pragma GCC optimize('mbypass-cache')
// #pragma GCC optimize('mno-cache-volatile')

/****************************************************************************************
 *                   GRTOS SYSTEM GENERAL DEFINITIONS
 ***************************************************************************************/
#define G_ISR_PRIORITY                 0  ///< \brief Priority assigned to an Interrupt task 
#define G_TASK_STACKSIZE            1024  ///< \brief Size of the default stack task
#define G_TASK_STACKSIZE_IDLE        512  ///< \brief Size of the default idle stack task


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
#define G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE 0

/**
 *  G_IDLE_WAIT_WITH_HALT
 *  If equal to 1, the processors are hatled with waitrequest.
 *  If 0, while over waitrequest is implemented to enable Eclipse debugging
 */
#define G_IDLE_WAIT_WITH_HALT 1

/**
 * G_DEBUG_WHILEFOREVER_ENABLE
 * Used to insert code to check during runtime data conistency in several functions
 * when 1: checks are enable
 * when 0: checks are disable
 */
#define G_DEBUG_WHILEFOREVER_ENABLE 0

/**
 * G_DEBUG_GET_POINTED_FIELD_CHECK
 * Used to check structure consistency when reference a field of the structure
 * when 1: checks are enable
 * when 0: checks are disable
 */
#define G_DEBUG_GET_POINTED_FIELD_CHECK 0

/**
 * G_DEBUG_SET_POINTED_FIELD_CHECK
 * Used to check structure consistency when reference a field of the structure to set it
 * when 1: checks are enable
 * when 0: checks are disable
 */
#define G_DEBUG_SET_POINTED_FIELD_CHECK 0

/**
 * G_DEBUG_WAITING_LOOP
 * Used to force a waiting loop instead of ferezing the processor
 * when 1: waiting loop require for working with debugger 
 *         (debugger does not connect when processor is frozen)
 * when 0: processor waits in frozen state 
           (the waitrequest signal is asserted to frozen the processor)
 */
#define G_DEBUG_WAITING_LOOP 0


// from https://stackoverflow.com/questions/2219829/how-to-prevent-gcc-optimizing-some-statements-in-c
// #pragma GCC push_options
// #pragma GCC optimize ("O3")

// from https://stackoverflow.com/questions/64026719/gcc-cli-option-o0-surprisingly-discards-code-after-pragma-gcc-optimize-in-macr
#ifdef __niosX_arch__
#define _STRING(S) #S
#define STRING(S) _STRING(S)
#define OPTIMEZE_CODE(OLEVEL) \
    _Pragma("GCC push_options") \
    _Pragma(STRING(GCC optimize ("O3")))
#endif

#ifndef __niosX_arch__
#define _STRING(S) #S
#define STRING(S) _STRING(S)
#define OPTIMEZE_CODE(OLEVEL) \
    _Pragma("GCC push_options") \
    _Pragma(STRING(GCC optimize ("O3")))
#endif

// _Pragma(STRING(GCC optimize ("O" #OLEVEL)))

#define OPTIMEZE_RESTORE \
    _Pragma("GCC pop_options")  

// from https://stackoverflow.com/questions/58477537/labels-redefined-in-macro-in-c
#define RTSZ(i) RTSZ1(i, __LINE__, __FUNCTION__)
#define RTSZ1(i, l, f) RTSZ2(i, l, f)
#define RTSZ2(i, l, f) RTSZ_##i##_##l##_##f

/*
#define GK_SAMPLE_FUNCTION_BEGIN(number) \
    if ((G_DEBUG_SAMPLE_BEGIN_ENABLE == 0xFFFFFFFF) ||  (G_DEBUG_SAMPLE_BEGIN_ENABLE == number)) { \
        RTSZ(0):  \
        IOWR(GRTOS_DRIVER_GRTOS_MONITOR_BASE, 0, ((unsigned int) (&&RTSZ(0)) | (unsigned int) (0x80000000))); \
    } \
    if (GRTOS_MTX_PRC_GRANTED != GRTOS_CMD_PRC_ID) { \
        printf("ERROR in critical section Proc: %d, grant=%d, function= %s, file=%s, line= %d\n",GRTOS_CMD_PRC_ID,GRTOS_MTX_PRC_GRANTED,__FUNCTION__,__FILE__,__LINE__); \
        while(1); \
    } \
	G_INT64 START_TIME = GRTOS_CMD_SYS_TM_GET; \
	if (number < 10000) g_kcb.function_current_number = number; \


#define GK_SAMPLE_FUNCTION_END(number) \
    if ((G_DEBUG_SAMPLE_END_ENABLE == 0xFFFFFFFF) ||  (G_DEBUG_SAMPLE_END_ENABLE == number)){ \
        RTSZ(0):  \
        IOWR(GRTOS_DRIVER_GRTOS_MONITOR_BASE, 0, (unsigned int) &&RTSZ(0) | 0xC0000000); \
    } \
    if (G_Running == G_TRUE) {\
        PRINT_ASSERT((gkm_PCBState_Valid(g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCBState) == G_TRUE),"ERROR PCBState not valid= %d\n",(int) g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCBState); \
    } \
    if (GRTOS_MTX_PRC_GRANTED != GRTOS_CMD_PRC_ID) { \
        printf("ERROR in critical section Proc: %d, function= %s, file=%s, line= %d\n",GRTOS_CMD_PRC_ID,__FUNCTION__,__FILE__,__LINE__); \
        while(1);\
    } \
	G_INT64 ELAPSED_TIME = GRTOS_CMD_SYS_TM_GET - START_TIME; \
	{ \
	if (number == g_kcb.function_current_number) { \
		if (ELAPSED_TIME >= g_kcb.function_max_interval) { \
			g_kcb.function_max_interval = ELAPSED_TIME; \
			GU_FPRINTF(stderr, "max interval = %llu, in %s\n",  (unsigned long long) ELAPSED_TIME, __FUNCTION__); \
		} \
		g_kcb.function_max_interval = g_kcb.function_max_interval / 2;\
	}} \
*/

#ifdef __niosX_arch__
#define PRINT_FUNCTION \
			{ for (unsigned int character = 0; character < strlen(__FUNCTION__); character++) { \
				IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDOUT_BASE , __FUNCTION__[character]); \
				IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDERR_BASE , __FUNCTION__[character]); \
			} \
			IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDOUT_BASE , '\n'); \
			IOWR_ALTERA_AVALON_JTAG_UART_DATA(ALT_STDERR_BASE , '\n'); \
		}
#else
#define PRINT_FUNCTION   do { while(1); } while(0);
#endif
		
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)

	#define GK_SAMPLE_FUNCTION_BEGIN(number)  \
		PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == GRTOS_CMD_PRC_ID),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) GRTOS_CMD_PRC_ID, __FUNCTION__, __LINE__); \

	#define GK_SAMPLE_FUNCTION_END(number)  \
		PRINT_ASSERT(((G_Running == G_FALSE) || (g_kcb.KCB_NextECBTL != (GS_ECB *) 0)),"ERROR in root time list\n"); \
		PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == GRTOS_CMD_PRC_ID),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) GRTOS_CMD_PRC_ID, __FUNCTION__, __LINE__); \
		if (G_Running == G_TRUE) PRINT_ASSERT((gkm_Check_list_LCBL_IsValid(100000) == G_TRUE),"ERROR in GK_SAMPLE_FUNCTION_END\n"); 
#else
	#define GK_SAMPLE_FUNCTION_BEGIN(number)
	#define GK_SAMPLE_FUNCTION_END(number)  
#endif
// gkm_GS_PCBAssocLCB_IsValid

 /* Test according with the structure type */ 
#define struct_test(x) _Generic((x),  \
        GS_TCB *: gkm_TCB_IsValid,                              \
		GS_PCBAssocLCB *: gkm_PCBAssocLCB_IsValid,              \
        GS_PCB *: gkm_PCB_IsValid,                              \
        GS_ECB *: gkm_ECB_IsValid,                              \
        GS_SCB *: gkm_SCB_IsValid,                              \
        G_RCB *: gkm_RCB_IsValid,                               \
        GS_RRDS *: gkm_RRDS_IsValid,                            \
        GS_LCB *: gkm_LCB_IsValid,                              \
		default: gkm_XCB_NotValid)((void *) x)

 /* Test data according with the structure type */ 
#define data_test(pointer, field, data) _Generic(((pointer)->field),  \
        GS_TCB *: gkm_TCB_IsValid((GS_TCB *) data),                              \
		GS_PCBAssocLCB *: gkm_PCBAssocLCB_IsValid((GS_PCBAssocLCB *) data),      \
        GS_PCB *: gkm_PCB_IsValid((GS_PCB *) data),                              \
        GS_ECB *: gkm_ECB_IsValid((GS_ECB *) data),                              \
        GS_SCB *: gkm_SCB_IsValid((GS_SCB *) data),                              \
        G_RCB *: gkm_RCB_IsValid((G_RCB *) data),                                \
        GS_RRDS *: gkm_RRDS_IsValid((GS_RRDS *) data),                           \
        GS_LCB *: gkm_LCB_IsValid((GS_LCB *) data),                              \
		default: gkm_XCB_NotValid((void *) data))

#if (G_DEBUG_SET_POINTED_FIELD_CHECK == 1)
	#define set_pointed_field(pointer,field,data) \
				({ \
					PRINT_ASSERT((struct_test(pointer) == G_TRUE),"ERROR STRUCTURE pointer is not valid pointer = %p\n", (void *) pointer); \
					(pointer)->field = data ; \
				})

#else
	#define set_pointed_field(pointer,field,data) \
				({ \
					(pointer)->field = (data); \
				})
#endif

#define get_pointed_offset(type,pointer,offset) (*(type *) ((char *) pointer + offset))


#if (G_DEBUG_GET_POINTED_FIELD_CHECK == 1)
	#define get_pointed_field(pointer,field) \
				({ \
					PRINT_ASSERT((struct_test(pointer) == G_TRUE),"ERROR STRUCTURE is not valid\n"); \
					PRINT_ASSERT((((void *) ((pointer)->field) == (void *) 0) || (struct_test(((pointer)->field)) == G_TRUE)),"ERROR STRUCTURE POINTED is not valid\n"); \
					(pointer)->field; \
				})
#else
	#define get_pointed_field(pointer,field) \
				({ \
					(pointer)->field; \
				})	
#endif

#define get_field(pointer,field) \
            ({ \
				PRINT_ASSERT((struct_test(pointer) == G_TRUE),"ERROR STRUCTURE is not valid\n"); \
                (pointer).field; \
            })


/* This is the definition for Nios32.  */

typedef unsigned long long G_INT64;
typedef unsigned int       G_INT32;       /* Unsigned 32 bit quantity      */
typedef unsigned int       GS_STK;      /* Type to Stack Pointers        */

typedef unsigned long long gt_time;
typedef unsigned long long gt_priority;


#define GU_SAMPLE_FUNCTION_BEGIN(number)            


#define GU_SAMPLE_FUNCTION_END(number)            




/************************************************************************************
 *  GRTOS INTERNAL DEFINITIONS
 ************************************************************************************/
/// \brief G_NUMBER_OF_PCB Number of processors defined in Qsys/Platform Designer
/// \todo Define a function to get this value during runtime


// \brief G_IDLE_PRIORITY_MASK mask to or the priority when processor is idle
#define G_IDLE_PRIORITY_MASK      0x8000000000000000


// \brief G_INITIAL_REGISTER_VALUE initial value of the registers initilized into the stack
#define G_INITIAL_REGISTER_VALUE  0

#define G_NUMBER_OF_PCB      GRTOS_DRIVER_NPROCESSORS

/// \brief G_TCB_DESCRIPTION_LENGTH defines the maximum number of characters in the TCB description
/// \brief IT SHOULD BE MULTIPLE OF 4 TO AVOID MEMORY UNALIGMENT
#define G_TCB_DESCRIPTION_LENGTH     48

/// \brief G_NUMBER_OF_IRQ_TCB Number of IRQs defined in Qsys/Platform Designer
/// \todo Define a function to get this value during runtime
#define G_NUMBER_OF_IRQ_TCB			         ALT_NIRQ  

/// \brief G_TICKS_PER_SECOND 
/// \todo Define a function to get this value during runtime
#define G_TICKS_PER_SECOND  (int)(GRTOS_DRIVER_GRTOSFREQUENCY/(GRTOS_DRIVER_PRESCALE))

/// \brief G_TICKS_PER_MSECOND 
/// \todo Define a function to get this value during runtime
#define G_TICKS_PER_MSECOND (G_TICKS_PER_SECOND/1000)

/// \brief G_SECONDS_IN_MINUTE is the number of seconds in a minute 
#define G_SECONDS_IN_MINUTE       60

/// \brief G_SECONDS_IN_HOUR is the number of seconds in an hour
#define G_SECONDS_IN_HOUR       3600


/// \brief Event codes for gk_ENTRY_IRQ_HANDLER
#define EVN_CODE_TIMED  0x21
#define EVN_CODE_FROZEN 0x22


/************************************************************************************
 *  NEWLIB definitions 
 *  from https://www.cs.ccu.edu.tw/~pahsiung/courses/ese/resources/newlib.pdf
 ************************************************************************************/

/**
 * G_DEBUG_WHILEFOREVER
 * Executed when G_DEBUG_WHILEFOREVER_ENABLE is 1 and an inconsistency is found
 */
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	#define G_DEBUG_WHILEFOREVER \
				 do{int dest = 0; \
					printf("ERROR in for Proc %d in function %s, file = %s, line = %d \n",GRTOS_CMD_PRC_ID , __FUNCTION__,__FILE__,__LINE__); \
					while(1){dest++;\
				 }}while(0)
#else
	#define G_DEBUG_WHILEFOREVER  do { while(1); } while(0)
#endif

/**
 * GU_FPRINTF redefines fprintf for use when G_DEBUG_WHILEFOREVER_ENABLE == 1
 * PRINT_ASSERT lets assert a condition, printa a message, the line where it happened and stop execution
 */
// #define GU_FPRINTF(...) do {fprintf(__VA_ARGS__);} while (0)
#if (G_DEBUG_WHILEFOREVER_ENABLE == 1)
	#define GU_FPRINTF(...) do {fprintf(__VA_ARGS__);} while (0) 
	#define PRINT_ASSERT(condition,...) \
        if (!(condition)) { \
			fprintf(stderr, __VA_ARGS__); \
            /* GU_FPRINTF(stderr,  ##__VA_ARGS__); */ \
            G_DEBUG_WHILEFOREVER; \
        }
#else
	#define GU_FPRINTF(...)  
    #define PRINT_ASSERT(condition,...) 
#endif


/************************************************************************************
 *  GRTOS COMMANDS
 ************************************************************************************/

#define IOWR_GRTOS_MTX_NESTED(data)        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_NESTED, data)
#define IORD_GRTOS_RST_CLR                 IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR)

#define gu_write_outputs(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_LEDS, data)


/// \brief GRTOS_now Return the current system time
/// \details ONLY for critical section use
/// \return G_INT64 with the current system time
/// \relates Time
#define GRTOS_now ({ \
TIMEPRIORITY value64; \
do { \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_CNT_HGH); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
} while(0); \
    value64.i64; \
})


/// \brief GRTOS_CMD_MTX_TM_GET Return the time the mutex is granted in system time units
/// \details ONLY for use in critical section
/// \relates Miscellaneous
#define GRTOS_CMD_MTX_TM_GET ({ \
TIMEPRIORITY value64; \
do { \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_MUTEX_TIME_HGH); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
} while(0); \
    value64.i64; \
})

/// \brief GRTOS_CMD_SYS_TM_GET Returns the value of System TIME (R_FRZ_CNT + R_TM_CNT) 
/// \details ONLY for use in critical section
/// \return Returns the value of the System Time (C_SYS_TM)
/// \relates Time
#define GRTOS_CMD_SYS_TM_GET ({ \
TIMEPRIORITY value64; \
do { \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_TM); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
} while(0); \
    value64.i64; \
})

/// \brief GRTOS_CMD_FRZ_TM_GET Returns the value of Frozen time (R_FRZ_CNT) 
/// \details ONLY for use in critical section
/// \return Returns the value of the System Time (C_SYS_TM)
/// \relates Time
#define GRTOS_CMD_FRZ_TM_GET ({ \
TIMEPRIORITY value64; \
do { \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_TM_HGH); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
} while(0); \
    value64.i64; \
})


/// \brief GRTOS_MTX_PRC_GRANTED returns the CPUID of the processor granting the GRTOS MUTEX
/// \todo Describe better and related with GRTOS controller
#define GRTOS_MTX_PRC_GRANTED                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_GRN)

/// GRTOS_GET_INTERVAL
///  \brief GRTOS_GET_INTERVAL Reset the Interval Time Counter, returning the last value
///  \return Return the interval time since the laast reset
///  \relates Time
#define GRTOS_GET_INTERVAL                       IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_ELP_TM_CNT)


/// PROC_INTERRUPT_DISABLE
/// \brief PROC_INTERRUPT_DISABLE disables GeMRTOS processor interrupt in order to enter
/// \brief the processor critical section. 
#define PROC_INTERRUPT_DISABLE      \
		do { \
			IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_PRC_CRTCL_SECTION_START, GRTOS_CMD_PRC_ID); \
		} while (((unsigned int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_PRC_CRTCL_SECTION_START) & (unsigned int) PRC_MASK[GRTOS_CMD_PRC_ID-1]) == (unsigned int) 0);

/// PROC_INTERRUPT_ENABLE
/// \brief PROC_INTERRUPT_ENABLE enables GeMRTOS processor interrupt in order to exit
/// \brief the processor critical section. 
#define PROC_INTERRUPT_ENABLE      \
		IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_PRC_CRTCL_SECTION_END, GRTOS_CMD_PRC_ID);


/// \brief GRTOS_CMD_GET_TIME_PRESCALE gets the system clock prescale
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GET_TIME_PRESCALE                          IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_PSC)

/// \brief GRTOS_CMD_FRZ_TM_THR_GET Returns the value of the Frozen Time Threshold (R_FRZ_TM_THR) register of the GRTOS controller
/// \details ONLY for use in critical section
/// \return Returns the value of the Frozen Time Threshold (R_FRZ_TM_THR)
/// \param [in] None
/// \relates Time
#define GRTOS_CMD_FRZ_TM_THR_GET ({ \
    TIMEPRIORITY value64; \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_THR_HGH); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
    value64.i64; \
})

/// \brief GRTOS_CMD_GET_STATUS_DEBUG_HOLD return the status of the DEBUG_HOLD bit (G_TRUE or G_FALSE)
/// \todo Describe better and related with GRTOS controller
// #define GRTOS_CMD_GET_STATUS_DEBUG_HOLD (((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL) >> 2) & 1) ? G_TRUE : G_FALSE)
#define GRTOS_CMD_GET_STATUS_DEBUG_HOLD (((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL)) & 4) ? G_TRUE : G_FALSE)

/// \brief GRTOS_CMD_GET_FRZ_ENB returns the status of the frozen mode (G_TRUE if enabled, G_FALSE if disabled)
/// \todo Describe better and related with GRTOS controller
// #define GRTOS_CMD_GET_FRZ_ENB (((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL) >> 1) & 1) ? G_TRUE : G_FALSE)
#define GRTOS_CMD_GET_FRZ_ENB (((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL)) & 2) ? G_TRUE : G_FALSE)

/// \brief GRTOS_CMD_GET_FRZ_ACT returns the status of the frozen mode event (G_TRUE if active, G_FALSE if inactive)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GET_FRZ_ACT ((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL)  & 1) ? G_TRUE : G_FALSE)

/// \brief GRTOS_CMD_EVN_OCC returns the event happened from the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_EVN_OCC               IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_EVN_OCC)

/// \brief GRTOS_CMD_IRQ_RQS gets the status of the External IRQ register
/// \todo Describe better and related with GRTOS controller
/// \todo Check if it is required
#define GRTOS_CMD_IRQ_RQS                                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_RQS)

/// \brief GRTOS_CMD_IRQ_ENB_GET(irq) reads the enabled status of the device interrupt request (DIRQ) irq
/// \todo Describe better and related with GRTOS controller
// #define GRTOS_CMD_IRQ_ENB_GET(irq) ((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET) >> (irq-1)) & 1)
#define GRTOS_CMD_IRQ_ENB_GET(irq) ((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET)) & PRC_MASK[(irq-1)])


// Write grtos registers

/// \brief GRTOS_CMD_FRZ_TM_THR_SET Sets the Frozen Time Threshold register of the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_TM_THR_SET(timeset) \
	do { \
		TIMEPRIORITY temp_aux; \
        temp_aux.i64 = (G_INT64) timeset; \
        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP, (unsigned long)temp_aux.i32[0]); \
 		IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_THR_HGH, (unsigned long)temp_aux.i32[1]); \
 	}while(0) 

/// \brief GRTOS_CMD_NXT_OCC_TM_EVN_SET Sets the Next Occurence Time register of the GRTOS controller
/// to produce a timed event when system time reaches the value of timeset
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_NXT_OCC_TM_EVN_SET(timeset) \
	do { \
		TIMEPRIORITY temp_aux; \
        temp_aux.i64 = (G_INT64) timeset; \
        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP, (unsigned long)temp_aux.i32[0]); \
        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_NXT_OCC_TM_HGH, (unsigned long)temp_aux.i32[1]); \
 	}while(0) 

/// \brief GRTOS_MTX_RLS releases the mutex from the current processor
#define GRTOS_MTX_RLS    \
    do { \
        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RLS, GRTOS_CMD_PRC_ID); \
    } while(0)


/// \brief GRTOS_MTX_RSV_SET sets the reserve bit of the current processor to grant the mutex
#define GRTOS_MTX_RSV_SET                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RSV_SET, GRTOS_CMD_PRC_ID)

/// \brief GRTOS_CMD_INT_PRC_PND_CLR clears the pending interrupt of the current processor.
/// The interrupts to processors are always registered to avoid spurious interrupts
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_INT_PRC_PND_CLR        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_PRC_PND_CLR, GRTOS_CMD_PRC_ID);

/// \brief GRTOS_CMD_SET_TIME_PRESCALE sets the system clock prescale to get the system time unitbuf
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_SET_TIME_PRESCALE(scale)                   IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_PSC, scale);

/// \brief GRTOS_CMD_FRZ_ENB_SET enables the frozen mode event
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_ENB_SET             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x2)

/// \brief GRTOS_CMD_FRZ_DSB_SET disables the frozen mode event
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_DSB_SET             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x4)

/// \brief GRTOS_CMD_FRZ_EVN_CLR Clears the C1_FRZ_EVN flags. 
/// It should be executed when the frozen mode strategy is completed.
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_EVN_CLR             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x8) 

/// \brief GRTOS_CMD_TM_CNT_CLR resets the System Time Register
/// \todo Describe better and related with GRTOS controller
/// \todo Check if it is required
#define GRTOS_CMD_TM_CNT_CLR             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x10); 

/// \brief GRTOS_CMD_DEBUG_HOLD_ENB - enables the DEBUG_HOLD mode
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HOLD_ENB         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x40)

/// \brief GRTOS_CMD_DEBUG_HOLD_DIS - disables the DEBUG_HOLD mode
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HOLD_DIS         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x80)

/// \brief GRTOS_CMD_DEBUG_HALT_ENB - enables the HALT mode for better performance (no eclipse support)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HALT_ENB         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x1000)

/// \brief GRTOS_CMD_DEBUG_HALT_DIS - disables the HALT mode for eclipse debugging
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HALT_DIS           IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x2000)

/// \brief GRTOS_CMD_GATED_CLOCK_ENB - disables the GATED_CLK
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GATED_CLOCK_ENB           IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x4000)

/// \brief GRTOS_CMD_GATED_CLOCK_DIS - disables the GATED_CLK
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GATED_CLOCK_DIS           IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x8000)

/// \brief GRTOS_CMD_PRC_INT_ENB enables the processor interrupt request of the current processor in the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_PRC_INT_ENB              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET, GRTOS_CMD_PRC_ID);

/// \brief GRTOS_CMD_PRC_INT_DSB disables the interrupt of the current processor in the GRTOS controller. 
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_PRC_INT_DSB              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADD_INT_ENB_CLR, GRTOS_CMD_PRC_ID);

/// \brief GRTOS_CMD_LOW_PRC_SET 
/// Sets the processor with CPUID equal to processor as the Lowest Priority Processor in the GRTOS Controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_LOW_PRC_SET(processor)    IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_LOW_PRC_SET, (processor));

/// \brief GRTOS_CMD_LOW_PRC_GET
/// Gets the processor with CPUID equal to processor as the Lowest Priority Processor in the GRTOS Controller
/// \todo Describe better and related with GRTOS controller
#ifdef __niosX_arch__
	#define GRTOS_CMD_LOW_PRC_GET    ({ \
			int bin = IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_LOW_PRC_SET); \
			int dec = 0; \
			int i = 1; \
			int rem = 0; \
			while (bin != 0) { \
				rem = bin % 2; \
				bin /= 2; \
				dec += rem * i; \
				if ((dec != 0) && (bin != 0)) dec = 1000;\
				i++; \
			}; \
			dec;})
#else
	#define GRTOS_CMD_LOW_PRC_GET     IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_LOW_PRC_SET)
#endif

/// \brief GRTOS_CMD_NXT_TM_PRC_SET sets the Next Occurrence Time Processor in the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_NXT_TM_PRC_SET(processor)   IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_NXT_TM_PRC_SET, processor);
    
/// \brief GRTOS_CMD_HLT_IDL_PRC set to halt the current processor when it is idle
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_HLT_IDL_PRC         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_ENB, GRTOS_CMD_PRC_ID)

/// \brief GRTOS_CMD_IRQ_ENB_SET(irq) enables the device interrupt request event irq  (DIRQ)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_IRQ_ENB_SET(irq)          IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_ENB_SET, irq)

/// \brief GRTOS_CMD_IRQ_ENB_CLR(irq) disables the device interrupt request event irq  (DIRQ)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_IRQ_ENB_CLR(irq)          IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_ENB_CLR, irq)

/// \brief GRTOS_CMD_TRG_PRC_INT_SET interrupts the processor proc. If proc interrupt is 
/// enabled in the GRTOS controller then the processor is interrupted
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_TRG_PRC_INT_SET(proc)    IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_TRG_INT_PRC, proc);


/// \brief GRTOS_CMD_UNRST_GRTOS - unresets the processors when mask bit is equal to 0
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_UNRST_GRTOS(mask)         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR, mask)


/// \brief GRTOS_CMD_MTX_HGHST_PRC change the highest processor to grant the mutex
#define GRTOS_CMD_MTX_HGHST_PRC(data)            IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_HGHST_PRC, data)


/************************************************************************************
 *  GRTOS COMMANDS
 ************************************************************************************/

/// GRTOS_CMD_PRC_ID  - returns the cpuID of the current processor
// 02-07-2024 #if defined(__nios2_arch__)
// 02-07-2024 	#define GRTOS_CMD_PRC_ID  ({ volatile unsigned int CPUID = __builtin_rdctl(5); CPUID; })
// 02-07-2024 #endif
// 02-07-2024 #if defined(__riscv)
	#define GRTOS_CMD_PRC_ID    IORD(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, ADDR_REG_1)
// 02-07-2024 #endif

#define GRTOS_IRQ_INPUT_STATUS  IORD(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, ADDR_REG_2)

	// #define GRTOS_CMD_PRC_ID  ((G_INT32) (*((volatile unsigned int *) GEMRTOS_MULTIPROCESSOR_0_GRTOS_0_S_PROCESSOR1_BASE + 1)))

#if defined(__nios2_arch__)
#define NIOS2_WRITE_SP(sp) do { __asm volatile ("mov sp, %z0" : : "rM" (sp)); } while (0)
#endif
#if defined(__riscv)
#define NIOS2_WRITE_SP(sp) do { __asm volatile ("addi sp, %z0, 0" : : "rM" (sp)); } while (0)
#endif

/// GRTOS_CMD_PRC_SP  - returns the Stack Pointer register (SP) of the current processor
#define GRTOS_CMD_PRC_SP  ({ \
        void *StackPointer=(void*)0; \
        GRTOS_READ_SP(StackPointer); \
        (G_INT32) StackPointer; \
    })

/// \brief GRTOS_CMD_PRC_INT(proc) 
/// interrupts the processor and waits until it reaches the ISR and disables the interrupt
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_PRC_INT(proc) \
    GRTOS_CMD_TRG_PRC_INT_SET(proc); \
    while (GRTOS_CMD_IRQ_ENB_GET(proc)){ \
        while(0); \
    }

/// \brief GRTOS_CMD_HALT_PROCESSOR puts the processor in halt mode
/// The first command enable the IDLE state for the processor and the second read the first 
/// address of the processor bus because it is in waitrequest until next interrupt
/// \todo Describe better and related with GRTOS controller
#if (G_DEBUG_WAITING_LOOP == 1)
#define GRTOS_CMD_HALT_PROCESSOR  \
            GRTOS_CMD_HLT_IDL_PRC; \
			IOWR(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, ADDR_REG_0, 0);              
#else
#define GRTOS_CMD_HALT_PROCESSOR  \
					gk_WAIT_IDLE_TASK();
					
#endif

#define GRTOS_CMD_IDLE_PROCESSOR GRTOS_CMD_HALT_PROCESSOR


/************************************************************************************
 *  GRTOS CRITICAL SECTION COMMANDS
 ************************************************************************************/

#define NO_OP \
		__asm volatile ("nop");

/**
 *  \brief GRTOS_CMD_CRITICAL_SECTION_GET 
 *  Defines the entry to a critical section to handle system variables and ISR routines.
 *  It asks for critical section and puts the processor in halt mode. 
 *  GRTOS controller enables the processor when section is granted to it.
 *  Only call Kernel functions in a critical section in order to avoid critical section reentrancy
 */
#define GRTOS_CMD_CRITICAL_SECTION_GET \
	do{ \
		do { \
            GRTOS_MTX_RSV_SET; \
            GRTOS_CMD_HALT_PROCESSOR; \
			NO_OP \
        } while (GRTOS_CMD_PRC_ID != GRTOS_MTX_PRC_GRANTED);  \
		PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == GRTOS_CMD_PRC_ID),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) GRTOS_CMD_PRC_ID, __FUNCTION__, __LINE__); \
		gk_CRITICAL_SECTION_HIGHEST_PRIORITY_PROCESSOR_CALLBACK(); \
	}while(0)


/**
 *  \brief GRTOS_USER_CRITICAL_SECTION_GET defines the entry to a critical section to handle system variables.
 *  It is called from outside an interrupt and it may be interrupted while it is waiting for mutex.
 *  It should be used in user functions that execute kernel functions or modify kernal data.
 */
#define GRTOS_USER_CRITICAL_SECTION_GET  \
    GRTOS_CMD_CRITICAL_SECTION_GET; \
	{if (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB == G_FALSE) { \
		g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_MTX_NESTED++; \
	}}

// IOWR_GRTOS_MTX_NESTED(g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_MTX_NESTED); 
// if (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_MTX_NESTED == 0) {
/**
 *  \brief GRTOS_USER_CRITICAL_SECTION_RELEASE releases the critical section from the current processor.
 *  It is called from outside of an interrupt.
 *  It used in all the user function that executes kernel functions or modify kernel data.
 */
#define GRTOS_USER_CRITICAL_SECTION_RELEASE  \
	{{if (g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID-1].PCB_Critical_PCB == G_FALSE) { \
		unsigned int nested = --g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_MTX_NESTED; \
	IOWR_GRTOS_MTX_NESTED(nested); } else { \
	IOWR_GRTOS_MTX_NESTED(1); }}\
	GRTOS_MTX_RLS;}


/**
*********************************************************************************************************
*                            TIME and PRIORITY definitions
*********************************************************************************************************
**/ 
#define G_LOWEST_PRIORITY    0x7FFFFFFFFFFFFFFCLL  ///< Lowest Priority of a Task
#define G_IDLETASK_PRIORITY  0x7FFFFFFFFFFFFFFDLL  ///< Priority of Idle Task
#define G_LATEST_TIME        0x7FFFFFFFFFFFFFFFLL  ///< Lastest Occurence Time of an EVENT
#define G_DEFAULT_LCB_PRIORITY 0x7FFFFFF0



#endif /* GRTOSDEFINITIONS_H_ */
