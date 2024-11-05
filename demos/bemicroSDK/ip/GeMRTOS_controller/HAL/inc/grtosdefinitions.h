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
#define G_DEBUG_PRINT_WHEN_PROCESSOR_GOES_IDLE 1

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
#define G_DEBUG_WHILEFOREVER_ENABLE  1

/**
 * G_DEBUG_GET_POINTED_FIELD_CHECK
 * Used to check structure consistency when reference a field of the structure
 * when 1: checks are enable
 * when 0: checks are disable
 */
#define G_DEBUG_GET_POINTED_FIELD_CHECK 1

/**
 * G_DEBUG_SET_POINTED_FIELD_CHECK
 * Used to check structure consistency when reference a field of the structure to set it
 * when 1: checks are enable
 * when 0: checks are disable
 */
#define G_DEBUG_SET_POINTED_FIELD_CHECK 1

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
    if (GRTOS_MTX_PRC_GRANTED != gm_ProcessorId) { \
        printf("ERROR in critical section Proc: %d, grant=%d, function= %s, file=%s, line= %d\n",gm_ProcessorId,GRTOS_MTX_PRC_GRANTED,__FUNCTION__,__FILE__,__LINE__); \
        while(1); \
    } \
	G_INT64 START_TIME = gm_SystemTotalTimeGet; \
	if (number < 10000) g_kcb.function_current_number = number; \


#define GK_SAMPLE_FUNCTION_END(number) \
    if ((G_DEBUG_SAMPLE_END_ENABLE == 0xFFFFFFFF) ||  (G_DEBUG_SAMPLE_END_ENABLE == number)){ \
        RTSZ(0):  \
        IOWR(GRTOS_DRIVER_GRTOS_MONITOR_BASE, 0, (unsigned int) &&RTSZ(0) | 0xC0000000); \
    } \
    if (G_Running == G_TRUE) {\
        PRINT_ASSERT((gkm_PCBState_Valid(g_kcb.G_PCBTbl[gm_ProcessorId -1].PCBState) == G_TRUE),"ERROR PCBState not valid= %d\n",(int) g_kcb.G_PCBTbl[gm_ProcessorId -1].PCBState); \
    } \
    if (GRTOS_MTX_PRC_GRANTED != gm_ProcessorId) { \
        printf("ERROR in critical section Proc: %d, function= %s, file=%s, line= %d\n",gm_ProcessorId,__FUNCTION__,__FILE__,__LINE__); \
        while(1);\
    } \
	G_INT64 ELAPSED_TIME = gm_SystemTotalTimeGet - START_TIME; \
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
		PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == gm_ProcessorId),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) gm_ProcessorId, __FUNCTION__, __LINE__); \

	#define GK_SAMPLE_FUNCTION_END(number)  \
		PRINT_ASSERT(((G_Running == G_FALSE) || (g_kcb.KCB_NextECBTL != (GS_ECB *) 0)),"ERROR in root time list\n"); \
		PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == gm_ProcessorId),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) gm_ProcessorId, __FUNCTION__, __LINE__); \
		if (G_Running == G_TRUE) PRINT_ASSERT((gkm_Check_list_LCBL_IsValid(100) == G_TRUE),"ERROR in GK_SAMPLE_FUNCTION_END\n"); 
#else
	#define GK_SAMPLE_FUNCTION_BEGIN(number)
	#define GK_SAMPLE_FUNCTION_END(number)  
#endif


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
					printf("ERROR in for Proc %d in function %s, file = %s, line = %d \n",gm_ProcessorId , __FUNCTION__,__FILE__,__LINE__); \
					while(1){dest++;\
				 }}while(0)
#else
	#define G_DEBUG_WHILEFOREVER  do { while(1); } while(0)
#endif

/**
 * GU_FPRINTF redefines fprintf for use when G_DEBUG_WHILEFOREVER_ENABLE == 1
 * PRINT_ASSERT lets assert a condition, printa a message, the line where it happened and stop execution
 */
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

/** \def gm_WriteOutputs
 *  \brief The gm_WriteOutputs macro transfer the data input to the gemrtos_phy output conduit of GeMRTOS controller.
 *  \exception par_following The gm_WriteOutputs macro requires the following parameter:
 *  \exception class_name System
 *  \exception prototype gm_WriteOutputs
 *  \param [in] data Data to be transfered the the gemrtos_phy output conduit of GeMRTOS controller.
 *  \return The gm_WriteOutputs macro does not return any value..
 *  \remark gm_WriteOutputs
 */
#define gm_WriteOutputs(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_LEDS, data)

/** \def gm_SystemTimeGet
 *  \brief The gm_SystemTimeGet macro returns the current system time in system time units. This is the time in non frozen mode.
 *  \exception par_following The gm_SystemTimeGet macro does not accept any parameters.
 *  \exception class_name System
 *  \exception prototype gm_SystemTimeGet
 *  \return The gm_SystemTimeGet macro returns the current system time in system time units.
 *  \remark System
 */
#define gm_SystemTimeGet ({ \
	TIMEPRIORITY value64; \
	do { \
		gm_GeMRTOSCriticalSectionEnter; \
		value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_CNT_HGH); \
		value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
		gm_GeMRTOSCriticalSectionExit; \
	} while(0); \
		value64.i64; \
	})

/** \def gm_GeMRTOSCriticalSectionGrantedTime
 *  \brief The gm_GeMRTOSCriticalSectionGrantedTime macro returns the time the mutex was granted in system time units.
 *  \exception par_following The gm_GeMRTOSCriticalSectionGrantedTime macro does not accept any parameters.
 *  \exception class_name Critical_Section
 *  \exception prototype gm_GeMRTOSCriticalSectionGrantedTime
 *  \return The gm_GeMRTOSCriticalSectionGrantedTime macro returns the time the mutex was granted in system time units.
 *  \relates Critical_Section
 *  \remark gm_GeMRTOSCriticalSectionGrantedTime
 */
#define gm_GeMRTOSCriticalSectionGrantedTime ({ \
	TIMEPRIORITY value64; \
	do { \
		gm_GeMRTOSCriticalSectionEnter; \
		value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_MUTEX_TIME_HGH); \
		value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
		gm_GeMRTOSCriticalSectionExit; \
	} while(0); \
		value64.i64; \
	})


/** \def gm_SystemTotalTimeGet
 *  \brief gm_SystemTotalTimeGet returns the total system time. It is the time in non frozen mode plus the time in frozen mode.
 *  \exception par_following The gm_SystemTotalTimeGet macro has no parameter. It has to be noted that temporal constraints (deadline, period) are related to system time, obtained using the gm_SystemTimeGet macro, which is the time in non frozen mode.
 *  \exception class_name System
 *  \exception prototype G_INT64 time = gm_SystemTotalTimeGet
 *  \return The gm_SystemTotalTimeGet macro returns the total system time. 
 *  \remark gm_SystemTotalTimeGet
 */
#define gm_SystemTotalTimeGet ({ \
	TIMEPRIORITY value64; \
	do { \
		value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_TM); \
		value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
	} while(0); \
		value64.i64; \
	})

/** \def gm_FrozenModeTimeGet
 *  \brief gm_FrozenModeTimeGet returns the accumulated time the system was in Frozen Mode. This time is hold in the R_FRZ_CNT register of the GeMRTOS controller. 
 *  \exception par_following The gm_FrozenModeTimeGet macro has no parameter.
 *  \exception class_name Frozen_Mode
 *  \exception prototype G_INT64 time = gm_FrozenModeTimeGet
 *  \return The gm_FrozenModeTimeGet macro returns the accumulated time the system was in Frozen Mode, hold in the the R_FRZ_CNT register of the GeMRTOS controller.
 *  \remark gm_FrozenModeTimeGet
 */
#define gm_FrozenModeTimeGet ({ \
TIMEPRIORITY value64; \
do { \
	gm_GeMRTOSCriticalSectionEnter; \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_TM_HGH); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
	gm_GeMRTOSCriticalSectionExit; \
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


/** \def gm_ProcessorInterruptDisable
 *  \brief The gm_ProcessorInterruptDisable macro disables GeMRTOS processor interrupt in the GeMRTOS controller.
 *  \exception par_following The gm_ProcessorInterruptDisable macro does not accept any parameters.
 *  \exception class_name Processor
 *  \exception prototype gm_ProcessorInterruptDisable
 *  \return The gm_ProcessorInterruptDisable macro does not return any value.
 *  \sa gm_ProcessorInterruptEnable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq, gm_ProcessorHalt, gm_ProcessorId
 *  \remark gm_ProcessorInterruptDisable
 */
#define gm_ProcessorInterruptDisable    \
		gm_GeMRTOSCriticalSectionEnter; \
		GRTOS_CMD_PRC_INT_DSB;          \
		gm_GeMRTOSCriticalSectionExit; 
		

/** \def gm_ProcessorInterruptEnable
 *  \brief The gm_ProcessorInterruptEnable macro enables processor interrupts in the GeMRTOS controller.
 *  \exception par_following The gm_ProcessorInterruptEnable macro does not accept any parameters.
 *  \exception class_name Processor
 *  \exception prototype gm_ProcessorInterruptEnable
 *  \return The gm_ProcessorInterruptEnable macro does not return any value.
 *  \sa gm_ProcessorInterruptDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq, gm_ProcessorHalt, gm_ProcessorId
 *  \remark gm_ProcessorInterruptEnable
 */
#define gm_ProcessorInterruptEnable      \
		GRTOS_CMD_PRC_INT_ENB; 


/** \def gm_SystemTimePrescaleGet
 *  \brief The gm_SystemTimePrescaleGet macro returns the time prescale. This prescale is used to obtaine the system time unit from the system clock.
 *  \exception par_following The gm_SystemTimePrescaleGet macro has no parameter.
 *  \exception class_name System
 *  \exception prototype G_INT64 time = gm_SystemTimePrescaleGet
 *  \return The gm_SystemTimePrescaleGet macro returns the time prescale.
 *  \remark gm_SystemTimePrescaleGet
 */
#define gm_SystemTimePrescaleGet                 IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_PSC)

/** \def gm_FrozenModeThresholdSet
 *  \brief The gm_FrozenModeThresholdSet macro sets the value of the Frozen Time Threshold register in the GeMRTOS controller. By default, the frozen mode is disabled, and the frozen threshold is initialized to zero. This macro is essential for configuring the threshold that determines when the frozen mode becomes active when it is enabled.
 *  \exception par_following The gm_FrozenModeThresholdSet macro accepts the following parameter:
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_FrozenModeThresholdSet(timeset)
 *  \param [in] timeset The frozen threshold value specified in system ticks units. This value establishes the interval of delay in processing timed events after which the frozen mode will be triggered when frozen mode is enabled. 
 *  \return The gm_FrozenModeThresholdSet macro does not return any value.
 *  \sa gm_FrozenModeThresholdGet, gm_FrozenModeEnable, gm_FrozenModeDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_FrozenModeThresholdSet
 */
#define gm_FrozenModeThresholdSet(timeset) \
	do { \
		TIMEPRIORITY temp_aux; \
		gm_GeMRTOSCriticalSectionEnter; \
        temp_aux.i64 = (G_INT64) timeset; \
        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP, (unsigned long)temp_aux.i32[0]); \
 		IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_THR_HGH, (unsigned long)temp_aux.i32[1]); \
		gm_GeMRTOSCriticalSectionExit; \
 	}while(0) 

/** \def gm_FrozenModeThresholdGet
 *  \brief The gm_FrozenModeThresholdGet macro retrieves the value of the Frozen Time Threshold register from the GeMRTOS controller (R_FRZ_TM_THR). By default, the frozen mode is disabled, and the frozen threshold is set to zero. This macro is useful for determining the current threshold value, which is critical for managing the activation of frozen mode. 
 *  \exception par_following The gm_FrozenModeThresholdGet macro does not accept any parameters
 *  \exception class_name Frozen_Mode
 *  \exception prototype TIMEPRIORITY interval = gm_FrozenModeThresholdGet
 *  \return The gm_FrozenModeThresholdGet macro returns the current value of the Frozen Time Threshold register in the GeMRTOS controller.
 *  \sa gm_FrozenModeThresholdSet, gm_FrozenModeEnable, gm_FrozenModeDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_FrozenModeThresholdGet
 */
#define gm_FrozenModeThresholdGet ({ \
    TIMEPRIORITY value64; \
	gm_GeMRTOSCriticalSectionEnter; \
    value64.i32[1] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_THR_HGH); \
    value64.i32[0] = (unsigned) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP); \
	gm_GeMRTOSCriticalSectionExit; \
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
        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RLS, gm_ProcessorId); \
    } while(0)


/// \brief GRTOS_MTX_RSV_SET sets the reserve bit of the current processor to grant the mutex
#define GRTOS_MTX_RSV_SET                IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_RSV_SET, gm_ProcessorId)

/// \brief GRTOS_CMD_INT_PRC_PND_CLR clears the pending interrupt of the current processor.
/// The interrupts to processors are always registered to avoid spurious interrupts
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_INT_PRC_PND_CLR        IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_PRC_PND_CLR, gm_ProcessorId);

/// \brief gm_SystemTimePrescaleSet sets the system clock prescale to get the system time unitbuf
/// \todo Describe better and related with GRTOS controller
/** \def gm_SystemTimePrescaleSet
 *  \brief The gm_SystemTimePrescaleSet macro sets the system clock prescale to get the system time unit. By default, the prescale is set to configure a 10MHz frecuency for system time units.
 *  \exception par_following The gm_SystemTimePrescaleSet has one parameter:
 *  \exception class_name System
 *  \exception prototype gm_SystemTimePrescaleSet(scale)
 *  \param [in] scale The prescale configuration.
 *  \return The gm_SystemTimePrescaleSet macro does not return any value.
 *  \remark gm_SystemTimePrescaleSet
 */
#define gm_SystemTimePrescaleSet(scale)                   IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_PSC, scale);


/** \def gm_FrozenModeEnable
 *  \brief The gm_FrozenModeEnable macro enables the frozen mode event. By default, the frozen mode starts in a disabled state. Before invoking this macro, ensure that the frozen threshold is properly configured using the gm_FrozenModeThresholdSet function.  
 *  \exception par_following The gm_FrozenModeEnable macro does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_FrozenModeEnable
 *  \return The gm_FrozenModeEnable macro does not return any value.
 *  \sa gm_FrozenModeDisable, gm_FrozenModeThresholdSet, gm_FrozenModeThresholdGet, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_FrozenModeEnable
 */
#define gm_FrozenModeEnable             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x2)


/** \def gm_FrozenModeDisable
 *  \brief The gm_FrozenModeDisable macro disables the frozen mode event. By default, the frozen mode starts in a disabled state.
 *  \exception par_following The gm_FrozenModeDisable macro does not accept any parameters.
 *  \exception class_name Frozen_Mode
 *  \exception prototype gm_FrozenModeDisable
 *  \return The gm_FrozenModeDisable macro does not return any value.
 *  \sa gm_FrozenModeEnable, gm_FrozenModeThresholdSet, gm_FrozenModeThresholdGet, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_FrozenModeDisable
 */
#define gm_FrozenModeDisable             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 0x4)

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
#define GRTOS_CMD_PRC_INT_ENB              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET, gm_ProcessorId);

/// \brief GRTOS_CMD_PRC_INT_DSB disables the interrupt of the current processor in the GRTOS controller. 
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_PRC_INT_DSB              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADD_INT_ENB_CLR, gm_ProcessorId);

/// \brief GRTOS_CMD_LOW_PRC_SET 
/// Sets the processor with CPUID equal to processor as the Lowest Priority Processor in the GRTOS Controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_LOW_PRC_SET(processor)    IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_LOW_PRC_SET, (processor));

/// \brief GRTOS_CMD_LOW_PRC_GET
/// Gets the processor with CPUID equal to processor as the Lowest Priority Processor in the GRTOS Controller
#define GRTOS_CMD_LOW_PRC_GET     IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_LOW_PRC_SET)


/// \brief GRTOS_CMD_NXT_TM_PRC_SET sets the Next Occurrence Time Processor in the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_NXT_TM_PRC_SET(processor)   IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_NXT_TM_PRC_SET, processor);
    
/// \brief GRTOS_CMD_HLT_IDL_PRC set to halt the current processor when it is idle
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_HLT_IDL_PRC         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_ENB, gm_ProcessorId)

/** \def gm_IrqDisable
 *  \brief The gm_IrqDisable macro disables the specified device interrupt request event (IRQ) in the GeMRTOS controller. This macro is essential for managing interrupt handling and preventing specified DIRQs from triggering.
 *  \exception par_following The gm_IrqDisable macro accepts the following parameter:
 *  \exception class_name IRQ_Management
 *  \exception prototype gm_IrqDisable(irq)
 *  \param [in] irq The number of the IRQ to be disabled.
 *  \return The gm_IrqDisable macro does not return any value.
 *  \sa gm_IrqEnable, gm_IrqStatusGet, gm_IRQ_STATUS_SET, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_IrqDisable
 */
#define gm_IrqDisable(irq)          IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_ENB_SET, irq)

/** \def gm_IrqEnable
 *  \brief The gm_IrqEnable macro enables the specified device interrupt request event (DIRQ) in the GeMRTOS controller. This macro is crucial for allowing specified DIRQ to trigger.
 *  \exception par_following The gm_IrqEnable macro accepts the following parameter:
 *  \exception class_name IRQ_Management
 *  \exception prototype gm_IrqEnable(irq)
 *  \param [in] irq The number of the DIRQ to be enabled.
 *  \return The gm_IrqEnable macro does not return any value.
 *  \sa gm_IrqDisable, gm_IrqStatusGet, gm_IRQ_STATUS_SET, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_IrqEnable
 */
#define gm_IrqEnable(irq)          IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_ENB_CLR, irq)

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


// !!!!D 02-07-2024 #if defined(__nios2_arch__)
// !!!!D 02-07-2024 	#define gm_ProcessorId  ({ volatile unsigned int CPUID = __builtin_rdctl(5); CPUID; })

/** \def gm_ProcessorId
 *  \brief The gm_ProcessorId macro retrieves the ID of the current processor. This macro is useful for identifying the processor.
 *  \exception par_following The gm_ProcessorId macro does not accept any parameters.
 *  \exception class_name Processor
 *  \exception prototype G_INT32 prcID = gm_ProcessorId
 *  \return The gm_ProcessorId macro returns the ID of the current processor.
 *  \sa gm_ProcessorInterruptEnable, gm_ProcessorInterruptDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq, gm_ProcessorHalt
 *  \remark gm_ProcessorId
 */
#define gm_ProcessorId         IORD(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, ADDR_REG_1)

/** \def gm_ProcessorWaitForIrq
 *  \brief The gm_ProcessorWaitForIrq macro halts the processor until an interrupt occurs on one of the specified masked IRQs. This function is useful for enabling the processor to wait for specific interrupt events.
 *  \exception par_following The gm_ProcessorWaitForIrq macro accepts the following parameter:
 *  \exception class_name Processor
 *  \exception prototype gm_ProcessorWaitForIrq(IRQ_mask)
 *  \param [in] IRQ_mask A mask of the DIRQs that the processor will wait for. This mask specifies which interrupts should wake the processor from its halted state. The interrupt should be disabled in order to be used to wake up the processor.
 *  \return The gm_ProcessorWaitForIrq macro does not return any value.
 *  \sa gm_ProcessorInterruptDisable, gm_ProcessorInterrupt, gm_ProcessorHalt, gm_ProcessorId, gm_ProcessorInterruptEnable
 *  \remark gm_ProcessorWaitForIrq
 *  \todo Create another macro to return the processor to available type
 */
#define gm_ProcessorWaitForIrq(IRQ_mask)                                            \
	gm_GeMRTOSCriticalSectionEnter;                                               \
		g_kcb.G_PCBTbl[gm_ProcessorId-1].PCBType  = (int) GS_PCBType_UNAVAILABLE; \
		gk_SetLowestProcessor();                                             \
	gm_GeMRTOSCriticalSectionExit;                                                 \
	IOWR(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, ADDR_REG_1, IRQ_mask)


/** \def gm_IrqStatusGet
 *  \brief The gm_IrqStatusGet macro retrieves the current DIRQ status from the GeMRTOS controller. It reflects the value of the input signals from the device IRQ inputs, prior to the application of any enabling logic. This macro is useful for determining which interrupt requests are currently active.
 *  \exception par_following The gm_IrqStatusGet macro does not accept any parameters.
 *  \exception class_name IRQ_Management
 *  \exception prototype G_INT32 status = gm_IrqStatusGet
 *  \return The macro returns the status of the DIRQ register of the GeMRTOS controller.
 *  \sa gm_IrqEnable, gm_IrqDisable, gm_IRQ_STATUS_SET, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_IrqStatusGet
 */
#define gm_IrqStatusGet  IORD(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, ADDR_REG_2)



#if defined(__nios2_arch__)
#define NIOS2_WRITE_SP(sp) do { __asm volatile ("mov sp, %z0" : : "rM" (sp)); } while (0)
#endif
#if defined(__riscv)
#define NIOS2_WRITE_SP(sp) do { __asm volatile ("addi sp, %z0, 0" : : "rM" (sp)); } while (0)
#endif

// GRTOS_CMD_PRC_SP  - returns the Stack Pointer register (SP) of the current processor
#define GRTOS_CMD_PRC_SP  ({ \
        void *StackPointer=(void*)0; \
        GRTOS_READ_SP(StackPointer); \
        (G_INT32) StackPointer; \
    })


/**
 *  \def gm_ProcessorInterrupt
 *  \brief gm_ProcessorInterrupt issues an interrupt for the processor with the specified ID and waits until it reaches the ISR and disables its interrupt in the GeMRTOS controller.
 *  \exception par_following The gm_ProcessorInterrupt macro accepts the following parameter:
 *  \exception class_name Processor
 *  \exception prototype gm_ProcessorInterrupt(proc)
 *  \param [in] proc The ID of the processor to be interrupted. This specifies which processor will receive the interrupt signal.
 *  \return The gm_ProcessorInterrupt macro returns when the target processor disables its interrupt in the GeMRTOS controller.
 *  \sa gm_ProcessorInterruptEnable, gm_ProcessorInterruptDisable, gm_ProcessorWaitForIrq, gm_ProcessorHalt, gm_ProcessorId
 *  \remark gm_ProcessorInterrupt
 */
#define gm_ProcessorInterrupt(proc) \
    GRTOS_CMD_TRG_PRC_INT_SET(proc); \
    while (GRTOS_CMD_IRQ_ENB_GET(proc)){ \
        while(0); \
    }

/**
 *  \def gm_ProcessorHalt
 *  \brief The gm_ProcessorHalt macro places the processor into halt mode, effectively stopping its execution until an interrupt is issued for this processor by the GeMRTOS controller. This mode is often used to conserve power or to wait for external events before resuming normal operation.
 *  \exception par_following The gm_ProcessorHalt macro does not require any parameters.
 *  \exception class_name Processor
 *  \exception prototype gm_ProcessorHalt
 *  \return The gm_ProcessorHalt macro returns control to the calling function when the processor is interrupted and the ISR routine executed, allowing it to resume execution.
 *  \sa gm_ProcessorInterruptEnable, gm_ProcessorInterruptDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq, gm_ProcessorId
 *  \remark gm_ProcessorHalt
 */
#if (G_DEBUG_WAITING_LOOP == 1)

#define gm_ProcessorHalt  \
            GRTOS_CMD_HLT_IDL_PRC; \
			IOWR(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, ADDR_REG_0, 0);              
#else
	
#define gm_ProcessorHalt  \
			gk_WAIT_IDLE_TASK();
#endif

/**
 *  \def gm_SchedulingListExclusionSectionEnter
 *  \brief The gm_SchedulingListExclusionSectionEnter macro set temporalely the scheduling list exclusion parameter equal to 1 in order to avoid any other task to execute the following critial code. If another processor placed the exclusion before, then the task is suspended until the task executing the critical code restore the scheduling list exclusion parameter to its original value.
 *  \exception par_following The gm_SchedulingListExclusionSectionEnter macro does not require any parameters.
 *  \exception class_name Scheduling_List
 *  \exception prototype gm_SchedulingListExclusionSectionEnter
 *  \return The gm_SchedulingListExclusionSectionEnter macro returns control to the calling function when the exclusion parameter is set to 1.
 *  \sa gm_ProcessorInterruptEnable, gm_ProcessorInterruptDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq, gm_ProcessorId
 *  \remark gm_SchedulingListExclusionSectionEnter
 */
#define gm_SchedulingListExclusionSectionEnter  \
	gm_GeMRTOSCriticalSectionEnter; \
	GS_LCB *GeMRTOSplcb = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_RDY_LCB_Index; \
	while ((GeMRTOSplcb->LCBExclusion == 1) && (GeMRTOSplcb->LCBCurrentRunning != 1)) { \
		gk_KERNEL_TASK_SUSPEND_CURRENT(G_TCBState_READY); \
	} \
	g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_PrevExclusion = GeMRTOSplcb->LCBExclusion; \
	GeMRTOSplcb->LCBExclusion = 1; \
	GRTOS_CMD_PRC_INT_DSB; \
	gm_GeMRTOSCriticalSectionExit; 

	
/**
 *  \def gm_SchedulingListExclusionSectionExit
 *  \brief The gm_SchedulingListExclusionSectionExit macro restores the exclusion parameter of the scheduling list previous to the execution of gm_SchedulingListExclusionSectionEnter. It should executed when the critical code ends. It also enables the processor interrupt in the GeMRTOS controller.
 *  \exception par_following The gm_SchedulingListExclusionSectionExit macro does not require any parameters.
 *  \exception class_name Scheduling_List
 *  \exception prototype gm_SchedulingListExclusionSectionExit
 *  \return The gm_SchedulingListExclusionSectionExit macro returns control to the calling function when the exclusion parameter is set to 1.
 *  \sa gm_ProcessorInterruptEnable, gm_ProcessorInterruptDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq, gm_ProcessorId
 *  \remark gm_SchedulingListExclusionSectionExit
 */
#define gm_SchedulingListExclusionSectionExit  \
	GRTOS_CMD_PRC_INT_ENB; \
	gm_GeMRTOSCriticalSectionEnter; \
	GeMRTOSplcb->LCBExclusion = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_PrevExclusion; \
	g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_PrevExclusion = 0; \
	gm_GeMRTOSCriticalSectionExit; \
	

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
            gm_ProcessorHalt; \
			NO_OP \
        } while (gm_ProcessorId != GRTOS_MTX_PRC_GRANTED);  \
		PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == gm_ProcessorId),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) gm_ProcessorId, __FUNCTION__, __LINE__); \
		gk_CRITICAL_SECTION_HIGHEST_PRIORITY_PROCESSOR_CALLBACK(); \
	}while(0)


/**
 *  \def gm_GeMRTOSCriticalSectionEnter
 *  \brief The gm_GeMRTOSCriticalSectionEnter macro defines the entry point into a critical section for the management of kernel data. It is designed to ensure that modifications to shared kernel resources occur safely, although it may be interrupted while waiting for the GeMRTOS controller mutex. This macro should be utilized whenever there is a need to modify kernel data to prevent data corruption and maintain system stability.
 *  \exception par_following The gm_GeMRTOSCriticalSectionEnter macro does not accept any parameters.
 *  \exception class_name Critical_Section
 *  \exception prototype gm_GeMRTOSCriticalSectionEnter
 *  \return The gm_GeMRTOSCriticalSectionEnter macro does not return any value but blocks the code execution until the GeMRTOS controller mutex is granted.
 *  \sa gm_GeMRTOSCriticalSectionExit, gm_IrqEnable, gm_IrqDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_GeMRTOSCriticalSectionEnter
 */
#define gm_GeMRTOSCriticalSectionEnter  \
    GRTOS_CMD_CRITICAL_SECTION_GET; \
	{if (g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB == G_FALSE) { \
		g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_MTX_NESTED++; \
	}} 


/** \def gm_GeMRTOSCriticalSectionExit
 *  \brief The gm_GeMRTOSCriticalSectionExit macro exits the critical section from the current processor, allowing other processes enter. It is essential to use this macro in all user functions that execute kernel functions or modify kernel data, ensuring that the critical section is properly released and preventing potential deadlocks or resource contention.
 *  \exception par_following The gm_GeMRTOSCriticalSectionExit macro does not accept any parameters.
 *  \exception class_name Critical_Section
 *  \exception prototype gm_GeMRTOSCriticalSectionExit
 *  \return The gm_GeMRTOSCriticalSectionExit macro does not return any value.
 *  \sa gm_GeMRTOSCriticalSectionEnter, gm_IrqEnable, gm_IrqDisable, gm_ProcessorInterrupt, gm_ProcessorWaitForIrq
 *  \remark gm_GeMRTOSCriticalSectionExit
 */
#define gm_GeMRTOSCriticalSectionExit  \
	{{if (g_kcb.G_PCBTbl[gm_ProcessorId-1].PCB_Critical_PCB == G_FALSE) { \
		unsigned int nested = --g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_MTX_NESTED; \
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
