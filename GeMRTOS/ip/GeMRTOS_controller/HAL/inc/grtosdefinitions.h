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
#define G_TASK_STACKSIZE            4096  ///< \brief Size of the default stack task


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
    }


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
    if (gkm_Check_list_LCBL_IsValid(100000) != G_TRUE) {\
        printf("ERROR in Proc: %d, function= %s, file=%s, line= %d\n",GRTOS_CMD_PRC_ID,__FUNCTION__,__FILE__,__LINE__); \
        GRTOS_CMD_DEBUG_HALT_DIS; \
        PRINT_ASSERT((gkm_Check_list_LCBL_IsValid(100000) == G_TRUE),"ERROR LCBL CHECK not valid\n"); \
        while(1); \
    }
*/

#define Check_EBCs \
	{ GS_ECB *pevent; \
	for (pevent = g_kcb.KCB_ROOT_ECBs; pevent != (GS_ECB *) 0; pevent = pevent->ECB_NEXT_ECBs) \
	{ \
		PRINT_ASSERT((gkm_ECB_IsValid(pevent)),"ERROR in pevent= %p",pevent); \
	};}; \

#if G_DEBUG_WHILEFOREVER_ENABLE == 1

	#define GK_SAMPLE_FUNCTION_BEGIN(number)  \
		/* fprintf(stderr, "proc=%d, in %s, %d\n", (int) GRTOS_CMD_PRC_ID, __FUNCTION__, __LINE__); */ \
		PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == GRTOS_CMD_PRC_ID),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) GRTOS_CMD_PRC_ID, __FUNCTION__, __LINE__); \
		G_INT64 START_TIME = GRTOS_CMD_SYS_TM_GET; \
		if (number < 10000) { \
			g_kcb.function_current_number = number; \
		}

		// PRINT_ASSERT((gkm_Check_TCBs(G_FALSE) == G_TRUE), "ERROR in TCBs"); 
	#define GK_SAMPLE_FUNCTION_END(number)  \
		PRINT_ASSERT((GRTOS_MTX_PRC_GRANTED == GRTOS_CMD_PRC_ID),"ERROR getting critical section= %d, proc=%d, in %s, %d\n", (int) GRTOS_MTX_PRC_GRANTED, (int) GRTOS_CMD_PRC_ID, __FUNCTION__, __LINE__); \
		G_INT64 ELAPSED_TIME = GRTOS_CMD_SYS_TM_GET - START_TIME; \
		{ \
		if (number == g_kcb.function_current_number) { \
			if (ELAPSED_TIME >= g_kcb.function_max_interval) { \
				g_kcb.function_max_interval = ELAPSED_TIME; \
				fprintf(stderr, "max interval = %llu, in %s\n",  ELAPSED_TIME, __FUNCTION__); \
			} \
		}} \
		if (G_Running == G_TRUE) PRINT_ASSERT((gkm_Check_list_LCBL_IsValid(100000) == G_TRUE),"ERROR in GK_SAMPLE_FUNCTION_END\n"); \
		g_kcb.function_max_interval = g_kcb.function_max_interval / 2;

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

/* #define field_test(pointer, field, data) _Generic((pointer),  \
        GS_TCB *: gkm_TCB_IsFieldValid(offsetof(struct gs_tcb, field), data),                              \
		default: gkm_XCB_IsFieldValid(data))
		
		GS_PCBAssocLCB *: gkm_PCBAssocLCB_IsFieldValid(offsetof(struct gs_pcb_rdy_lcbl, field), data),     \
        GS_PCB *: gkm_PCB_IsFieldValid(offsetof(struct gs_pcb, field), data),                              \
        GS_ECB *: gkm_ECB_IsFieldValid(offsetof(struct gs_ecb, field), data),                              \
        GS_SCB *: gkm_SCB_IsFieldValid(offsetof(struct gs_scb, field), data),                              \
        G_RCB *: gkm_RCB_IsFieldValid(offsetof(struct g_rcb, field), data),                                \
        GS_RRDS *: gkm_RRDS_IsFieldValid(offsetof(struct gs_rrds, field), data),                           \
        GS_LCB *: gkm_LCB_IsFieldValid(offsetof(struct gs_lcb, field), data),                              \
		default: gkm_XCB_IsFieldValid(data))
*/

		/* enum tcbtype: gkm_TCBType_Valid,                  */ 
		/* enum tcbstate: gkm_TCBState_Valid,                */ 
		/* enum ecbstate: gkm_ECBState_Valid,                */ 
		/* enum ecbtype: gkm_ECBType_Valid,                  */ 
		/* enum pcbstate: gkm_PCBState_Valid,                */ 
		/* enum pcbtype: gkm_PCBType_Valid,                  */ 
		/* enum lcbstate: gkm_LCBState_IsValid,              */ 
		/* enum lcbtype: gkm_LCBType_IsValid,                */ 


#if G_DEBUG_SET_POINTED_FIELD_CHECK == 1
	#define set_pointed_field(pointer,field,data) \
				({ \
					/* g_kcb.function_prescaler++; */ \
					/* if (g_kcb.function_prescaler >= 100) { */ \
					/* 	fprintf(stderr, "i= %u, in %s\n",  g_kcb.function_max_interval , __FUNCTION__); */ \
					/* 	g_kcb.function_prescaler = 0; } */ \
					PRINT_ASSERT((struct_test(pointer) == G_TRUE),"ERROR STRUCTURE pointer is not valid pointer = %p\n", (void *) pointer); \
					/* PRINT_ASSERT((((void *) (data) == (void *) 0) || (data_test(pointer, field, data) == G_TRUE)),"ERROR STRUCTURE POINTED is not valid\n"); */ \
					(pointer)->field = data ; \
				})

#else
	#define set_pointed_field(pointer,field,data) \
				({ \
					(pointer)->field = (data); \
				})
#endif

#if G_DEBUG_GET_POINTED_FIELD_CHECK == 1
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

#define get_PCB_field(pointer,field) \
            ({ \
				PRINT_ASSERT((gkm_PCB_IsValid(&pointer) == G_TRUE),"ERROR PCB is not valid\n"); \
                (pointer).field; \
            })
			
#define set_ECB_field(pointer,field,data) \
            ({ \
				PRINT_ASSERT((gkm_ECB_IsValid(&pointer) == G_TRUE),"ERROR ECB is not valid\n"); \
                (pointer).field = (data); \
            })
			
#define get_ECB_field(pointer,field) \
            ({ \
				PRINT_ASSERT((gkm_ECB_IsValid(&pointer) == G_TRUE),"ERROR ECB is not valid\n"); \
                (pointer).field; \
            })

#define set_RCB_field(pointer,field,data) \
            ({ \
				PRINT_ASSERT((gkm_RCB_IsValid(&pointer) == G_TRUE),"ERROR RCB is not valid\n"); \
                (pointer).field = (data); \
            })
			
#define get_RCB_field(pointer,field) \
            ({ \
				PRINT_ASSERT((gkm_RCB_IsValid(&pointer) == G_TRUE),"ERROR RCB is not valid\n"); \
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
#define G_TCB_DESCRIPTION_LENGTH     50

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

/// \brief G_HOUR_IN_DAY is the number of hours in a day
#define G_HOURS_IN_DAY       24

/// \brief G_HOUR_IN_DAY is the number of hours in a day
#define G_DAYS_IN_YEAR       365

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
#define G_DEBUG_WHILEFOREVER \
			 do{int dest = 0; \
                printf("ERROR in for Proc %d in function %s, file = %s, line = %d \n",GRTOS_CMD_PRC_ID , __FUNCTION__,__FILE__,__LINE__); \
				while(1){dest++;\
			 }}while(0)

/**
 * PRINT_ASSERT lets assert a condition, printa a message, the line where it happened and stop execution
 */
// fprintf(fpuart[GRTOS_CMD_PRC_ID-1], ##__VA_ARGS__); 

#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	#define PRINT_ASSERT(condition,...) \
        if (!(condition)) { \
            fprintf(stderr,  ##__VA_ARGS__); \
            G_DEBUG_WHILEFOREVER; \
        }
#else
    #define PRINT_ASSERT(condition,...) 
#endif




/// \todo Check if it required
#define G_DEBUG_RUN_MONITOR_ROUTINE \
    do{ \
        GRTOS_CMD_DEBUG_HOLD_ENB; \
        fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "In %s, %d, proc %x, mtx %x \n",__FUNCTION__,__LINE__, GRTOS_CMD_PRC_ID,GRTOS_MTX_PRC_GRANTED); \
        GRTOS_CMD_DEBUG_HOLD_DIS; } while(0)

/**
 * PRINT_TO_DEBUG
 * print to the stderr console if open, open it otherwise
 */
#define PRINT_TO_DEBUG(...) \
    fprintf(fpuart[GRTOS_CMD_PRC_ID-1], ##__VA_ARGS__);

    
/// \todo Check if it required
#if G_DEBUG_WHILEFOREVER_ENABLE == 1
	#define G_DEBUG_VERBOSE_RUNTIME \
        GRTOS_CMD_DEBUG_HOLD_ENB; \
        G_DEBUG_RUN_MONITOR_ROUTINE; \
		fprintf(fpuart[GRTOS_CMD_PRC_ID-1],"Proc %d, func %s \n",GRTOS_CMD_PRC_ID , __FUNCTION__); \
        GRTOS_CMD_DEBUG_HOLD_DIS; 
#else
	#define G_DEBUG_VERBOSE_RUNTIME
#endif

/// \todo Check if it required
//#define G_DEBUG_VERBOSE G_DEBUG_RUN_MONITOR_ROUTINE
//#define G_DEBUG_VERBOSE G_DEBUG_VERBOSE_RUNTIME 
#define G_DEBUG_VERBOSE  

/**
 *  \brief PRINT_DEBUG_LINE can be defined to print a message after the execution of each line of the code.
 *  It is defined as empty when no in debug mode
 *  /// \todo Check if it required
 */
// #define PRINT_DEBUG_LINE  fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "%s, %s, %d, proc %x, mtx %x\n",,__FILE__,__LINE__, GRTOS_CMD_PRC_ID, GRTOS_MTX_PRC_GRANTED);
// #define PRINT_DEBUG_LINE  fprintf(fpuart[GRTOS_CMD_PRC_ID-1], "%s, %d\n",__FUNCTION__,__LINE__);       
#define PRINT_DEBUG_LINE  

/************************************************************************************
 *  GRTOS COMMANDS
 ************************************************************************************/


// 22/11/2022 #define ADDR_HLT_ACT_CNT_SMP    34

// Macros defined and implemented in hardware, but not used
// 22/11/2022 #define IORD_GRTOS_HLT_ACT_CNT_SMP               IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_ACT_CNT_SMP)
// 22/11/2022 #define IORD_GRTOS_TRG_INT_PRC                   IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TRG_INT_PRC)
// 22/11/2022 #define IORD_GRTOS_HLT_IDL_PRC_ENB               IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_ENB)
#define IORD_GRTOS_RST_CLR                       IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR)
// 22/11/2022 #define IORD_GRTOS_FRZ_TM_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_TM_HGH)

// 22/11/2022 #define IOWR_GRTOS_HLT_IDL_PRC_CLR(data)         IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_HLT_IDL_PRC_DSB, data)
// 22/11/2022 #define IOWR_GRTOS_FRZ_TM_HGH(data)              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_FRZ_TM_HGH,data)
#define IOWR_GRTOS_MTX_NESTED(data)             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_NESTED, data)
// 22/11/2022 #define IOWR_GRTOS_MTX_RQS(data)                 IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_GRN, data)


// Read grtos registers
// #define IORD_GRTOS_SMP                           IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SMP)
// #define IORD_GRTOS_TM_CNT_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_TM_CNT_HGH)
// #define IORD_GRTOS_SYS_MUTEX_TIME_HGH            IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_MUTEX_TIME_HGH)
// #define IORD_GRTOS_SYS_TM_HGH                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_SYS_TM)

#define gu_write_outputs(data)                 IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_LEDS, data)


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


/// \brief GRTOS_MTX_PRC_GRANTED returns the CPUID of the processor granting the GRTOS MUTEX
/// \todo Describe better and related with GRTOS controller
#define GRTOS_MTX_PRC_GRANTED                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_GRN)

/// GRTOS_GET_INTERVAL
///  \brief GRTOS_GET_INTERVAL Reset the Interval Time Counter, returning the last value
///  \return Return the interval time since the laast reset
///  \relates Time
#define GRTOS_GET_INTERVAL                       IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_ELP_TM_CNT)

#define PRC_MASK (unsigned int[32]){(unsigned int) 0x1, (unsigned int) 0x2, (unsigned int) 0x4, (unsigned int) 0x8, (unsigned int) 0x10, (unsigned int) 0x20, (unsigned int) 0x40, (unsigned int) 0x80, (unsigned int) 0x100, (unsigned int) 0x200, (unsigned int) 0x400, (unsigned int) 0x800, (unsigned int) 0x1000, (unsigned int) 0x2000, (unsigned int) 0x4000, (unsigned int) 0x8000, (unsigned int) 0x10000, (unsigned int) 0x20000, (unsigned int) 0x40000, (unsigned int) 0x80000, (unsigned int) 0x100000, (unsigned int) 0x200000, (unsigned int) 0x400000, (unsigned int) 0x800000, (unsigned int) 0x1000000, (unsigned int) 0x2000000, (unsigned int) 0x4000000, (unsigned int) 0x8000000,(unsigned int) 0x10000000, (unsigned int) 0x20000000, (unsigned int) 0x40000000, (unsigned int) 0x80000000}

/// PROC_CRITICAL_SECTION_START
/// \brief PROC_CRITICAL_SECTION_START disables GeMRTOS processor interrupt in order to enter
/// \bried the processor critical section. 
#define PROC_CRITICAL_SECTION_START      \
		do { \
			IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_PRC_CRTCL_SECTION_START, GRTOS_CMD_PRC_ID); \
		} while (((unsigned int) IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_PRC_CRTCL_SECTION_START) & (unsigned int) PRC_MASK[GRTOS_CMD_PRC_ID-1]) == (unsigned int) 0);

/// PROC_CRITICAL_SECTION_END
/// \brief PROC_CRITICAL_SECTION_END enables GeMRTOS processor interrupt in order to exit
/// \bried the processor critical section. 
#define PROC_CRITICAL_SECTION_END      \
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
#define GRTOS_CMD_GET_STATUS_DEBUG_HOLD (((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL) >> 2) & 1) ? G_TRUE : G_FALSE)

/// \brief GRTOS_CMD_GET_FRZ_ENB returns the status of the frozen mode (G_TRUE if enabled, G_FALSE if disabled)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GET_FRZ_ENB (((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL) >> 1) & 1) ? G_TRUE : G_FALSE)

/// \brief GRTOS_CMD_GET_FRZ_ACT returns the status of the frozen mode event (G_TRUE if active, G_FALSE if inactive)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GET_FRZ_ACT ((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL)  & 1) ? G_TRUE : G_FALSE)

/// \brief GRTOS_CMD_EVN_OCC returns the event happened from the GRTOS controller
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_EVN_OCC                                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_EVN_OCC)

/// \brief GRTOS_CMD_IRQ_RQS gets the status of the External IRQ register
/// \todo Describe better and related with GRTOS controller
/// \todo Check if it is required
#define GRTOS_CMD_IRQ_RQS                                    IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_IRQ_RQS)

/// \brief GRTOS_CMD_IRQ_ENB_GET(irq) reads the enabled status of the device interrupt request (DIRQ) irq
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_IRQ_ENB_GET(irq) ((IORD(GRTOS_DRIVER_GRTOS_BASE, ADDR_INT_ENB_SET) >> (irq-1)) & 1)


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
#define GRTOS_CMD_FRZ_ENB_SET             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 1)

/// \brief GRTOS_CMD_FRZ_DSB_SET disables the frozen mode event
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_DSB_SET              IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 2)

/// \brief GRTOS_CMD_FRZ_EVN_CLR Clears the C1_FRZ_EVN flags. 
/// It should be executed when the frozen mode strategy is completed.
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_FRZ_EVN_CLR             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 3) 

/// \brief GRTOS_CMD_TM_CNT_CLR resets the System Time Register
/// \todo Describe better and related with GRTOS controller
/// \todo Check if it is required
#define GRTOS_CMD_TM_CNT_CLR             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 4); 

/// \brief GRTOS_CMD_DEBUG_HOLD_ENB - enables the DEBUG_HOLD mode
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HOLD_ENB                             IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 6)

/// \brief GRTOS_CMD_DEBUG_HOLD_DIS - disables the DEBUG_HOLD mode
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HOLD_DIS          IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 7)

/// \brief GRTOS_CMD_DEBUG_HALT_ENB - enables the HALT mode for better performance (no eclipse support)
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HALT_ENB          IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 12)

/// \brief GRTOS_CMD_DEBUG_HALT_DIS - disables the HALT mode for eclipse debugging
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_DEBUG_HALT_DIS           IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 13)

/// \brief GRTOS_CMD_GATED_CLOCK_ENB - disables the GATED_CLK
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GATED_CLOCK_ENB           IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 14)

/// \brief GRTOS_CMD_GATED_CLOCK_DIS - disables the GATED_CLK
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_GATED_CLOCK_DIS           IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_CTRL, 15)

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


/// \brief GRTOS_CMD_RST_GRTOS - resets the GRTOS controller to its initialization values
/// and the processors with CPUID different from 1
/// \todo Describe better and related with GRTOS controller
#define GRTOS_CMD_RST_GRTOS           IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR, 0)


/// \brief GRTOS_CMD_RLS_NXT_PRC - unreset the next processor
#define GRTOS_CMD_RLS_NXT_PRC            IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_RST_CLR, GRTOS_CMD_PRC_ID)

/// \brief GRTOS_CMD_MTX_HGHST_PRC change the highest processor to grant the mutex
#define GRTOS_CMD_MTX_HGHST_PRC(data)            IOWR(GRTOS_DRIVER_GRTOS_BASE, ADDR_MTX_HGHST_PRC, data)


/************************************************************************************
 *  GRTOS COMMANDS
 ************************************************************************************/

/// GRTOS_CMD_PRC_ID  - returns the cpuID of the current processor
#if defined(__nios2_arch__)
	#define GRTOS_CMD_PRC_ID  ({ volatile unsigned int CPUID = __builtin_rdctl(5); CPUID; })
#endif
#if defined(__riscv)
	#define GRTOS_CMD_PRC_ID  IORD(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, 1)
#endif

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
#define GRTOS_CMD_HALT_PROCESSOR  \
            GRTOS_CMD_HLT_IDL_PRC; \
			IOWR(GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1, 0, 0);              

#if G_DEBUG_WAITING_LOOP == 1
#undef GRTOS_CMD_HALT_PROCESSOR

#define GRTOS_CMD_HALT_PROCESSOR  \
					gk_WAIT_IDLE_TASK();
					
#endif




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
