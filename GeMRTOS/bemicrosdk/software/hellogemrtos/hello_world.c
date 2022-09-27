/*
 * *Hello World* example for multitasking operting system.
 *
 * This example executes the real-time tasks according the way they are configured
 *
 * Author: Ricardo Cayssials
 * All rights reserved.
 */

#include <stdio.h>
#include <grtosuserapi.h>
// #include <mq.h>
// #include <sem.h>
#include <grtosdefinitions.h>

// Change this for final version. This is just to make real-time tasks for runtime testing
// #include "C:/genericDE10/corrida/rts.h"
#include "../runtime/rts.h"


// #pragma GCC optimize('mno-cache-volatile')


/*************************************************************************************/
/*************************************************************************************/
/***********REAL-TIME SYSTEM DEFINITION***********************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

/**
 * GeMRTOS structure definitions
 */



t_semaphore_resource *semaphore1;

// INT32  queue_buffer[4096];
G_RCB  *pqueue;

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       4096


/* Definition of Invokation number */
int NUMBER_OF_USER_TASKS = G_MAX_NUMBER_OF_USER_TCB;
int task_invocation_number[G_MAX_NUMBER_OF_USER_TCB];
volatile int task_sampling_enable[G_MAX_NUMBER_OF_USER_TCB];
volatile void *generic_task_tcbs[G_MAX_NUMBER_OF_USER_TCB];
volatile int generic_task_index[G_MAX_NUMBER_OF_USER_TCB];
volatile int execution_auxiliar_counts[G_MAX_NUMBER_OF_USER_TCB];




/************************************************************************************/
/* TASK 1 ***************************************************************************/
/************************************************************************************/
/* Prints "Hello World" and sleeps for three seconds */
#define TASK1_PRIORITY      1

void sig_aborted_task1(int pdata)
{
    printf("\nTASK 1 ABORTED %d \n", pdata);
}

void task1(void* pdata)
{
  gt_tm sys_time;  
  while (1)
  {
    sys_time = gu_Clock(gu_get_now());
    printf("Hi task1: ");
    printf("Proc: %d, ", gu_Get_CPU_ID());
    printf("y= %d, d= %d, ", sys_time.tm_year, sys_time.tm_day);
    printf("h= %d, m= %d, ", sys_time.tm_hour, sys_time.tm_min);
    printf("s= %d, ms= %d\n", sys_time.tm_sec, sys_time.tm_msec); 
    gu_TASK_Sleep(0, 0, 15, 0);
  }
}

/************************************************************************************/
/* TASK 2 ***************************************************************************/
/************************************************************************************/
// INT32    consumer_buffer2[1024];

#define TASK2_PRIORITY      1
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata)
{
    gt_tm sys_time; 
    GS_ECB  *pbuffer;
    
    task_invocation_number[(int) pdata]++;    
    // GRTOS_MULTIPROCESSOR_0_GRTOS_0_S_PROCESSOR_MONITOR_BASE
    
    // IOWR(GRTOS_MONITOR_BASE, 0, (int) pdata | 0x8000);
    gk_MONITOR_FIFO_SAMPLE ((int) pdata | 0x8000);
    
//    pbuffer = (GS_ECB  *) gu_queue_consume((G_RCB *)pqueue, (void *)consumer_buffer2, (INT32) sizeof(consumer_buffer2), (gt_time) 0);
    
    sys_time = gu_Clock(gu_get_now());
    printf("Hi task2: data is %d, invocation %d\n",(int) ((int) pdata & 0xFFFF) , (int) task_invocation_number[(int) pdata]);
    printf("Proc: %d, ", gu_Get_CPU_ID());
    printf("y= %d, d= %d, ", sys_time.tm_year, sys_time.tm_day);
    printf("h= %d, m= %d, ", sys_time.tm_hour, sys_time.tm_min);
    printf("s= %d, ms= %d\n", sys_time.tm_sec, sys_time.tm_msec);
    // task_invocation_number[G_MAX_NUMBER_OF_USER_TCB];
    
    // IOWR(GRTOS_MONITOR_BASE, 0, (int) pdata | 0x0000);    
    gk_MONITOR_FIFO_SAMPLE ((int) pdata | 0x0000);
    
}

/************************************************************************************/
/* TASK 3 ***************************************************************************/
/************************************************************************************/
#define TASK3_PRIORITY      3
/* Prints "Hello World" and sleeps for three seconds */
void task3(void* pdata)
{
    gt_tm sys_time;
    
    int sem;
    sem = (int) gu_sem_wait(semaphore1, (gt_priority) 0, (gt_priority) 0, (gt_time) 0, (gt_time) 0,(int) G_TRUE);

    sys_time = gu_Clock(gu_get_now());
    printf("Hi task3: semaphore = %d, ",sem);
    printf("Proc: %d, ", gu_Get_CPU_ID());
    printf("y= %d, d= %d, ", sys_time.tm_year, sys_time.tm_day);
    printf("h= %d, m= %d, ", sys_time.tm_hour, sys_time.tm_min);
    printf("s= %d, ms= %d\n", sys_time.tm_sec, sys_time.tm_msec);
}

/************************************************************************************/
/* TASK 4 ***************************************************************************/
/************************************************************************************/


/* Prints "Hello World" and sleeps for three seconds */

#define TASK4_PRIORITY      1

void task4(void* pdata)
{
    gt_tm sys_time;

    while (1)
    {
        int sem = (int) gu_sem_wait(semaphore1, (gt_priority) 0, 
                                (gt_priority) 0, 
                                (gt_time) gu_Convert_Time((INT32) 0, (INT32) 0, (INT32) 0, (INT32) 5, (INT32) 0), 
                                (gt_time) 0,(int) G_TRUE);
                                
        sys_time = gu_Clock(gu_get_now());
        printf("Hi task4: semaphore = %d, ",sem);
        printf("Proc: %d, ", gu_Get_CPU_ID());
        printf("y= %d, d= %d, ", sys_time.tm_year, sys_time.tm_day);
        printf("h= %d, m= %d, ", sys_time.tm_hour, sys_time.tm_min);
        printf("s= %d, ms= %d\n", sys_time.tm_sec, sys_time.tm_msec);

        gu_sem_post((t_semaphore_resource *) semaphore1);        
        gu_TASK_Sleep(0, 0, 4, 0);
    }
}

#define MODE_PRINTING_TASK 0
#define MODE_FREQUENTIAL_TASK 1




/************************************************************************************/
/* TASK GENERIC for Frequential analysis*********************************************/
/************************************************************************************/
// INT32    generic_stk[G_MAX_NUMBER_OF_USER_TCB][TASK_STACKSIZE];
INT32    *generic_stk;
void sig_aborted_task_generic(int pdata)
{
    printf("\nTASK 1 ABORTED %d \n", pdata);
}


void task_generic(void* pdata)
{
    // gt_tm sys_time; 
    GS_ECB  *pbuffer;
    int i;
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
        
    // GRTOS_MULTIPROCESSOR_0_GRTOS_0_S_PROCESSOR_MONITOR_BASE
     // Write the sampling time
    if ((task_sampling_enable[(int) pdata] == 1) || (task_sampling_enable[(int) pdata] == 2)) {
        // IOWR(GRTOS_MONITOR_BASE, 0, (int) pdata | 0x800000);
        gk_MONITOR_FIFO_SAMPLE ((int) pdata | 0x800000);
    }
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
        
    task_invocation_number[(int) pdata]++;        
    execution_auxiliar_counts[(int) pdata] = 0;
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
        
    for (i=0; i < USER_TCB_execution_time[(int) pdata]; i++ ) {
        execution_auxiliar_counts[(int) pdata]++;
    }
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
    // sys_time = gu_Clock(gu_get_now());
    if (USER_TCB_execution_time[(int) pdata] == (int) 0) {
        // printf("T %d, P %d, I %d\n",(int) ((int) pdata & 0xFFFF) , gu_Get_CPU_ID(),(int) task_invocation_number[(int) pdata]);
        printf("T %d", ((int) pdata) );
        printf("; Mutex time      = %llx\n", (unsigned long long) gu_get_mutex_time()); 
    }
    // printf("Proc: %d, ", gu_Get_CPU_ID());
    // printf("y= %d, d= %d, ", sys_time.tm_year, sys_time.tm_day);
    // printf("h= %d, m= %d, ", sys_time.tm_hour, sys_time.tm_min);
    // printf("s= %d, ms= %d\n", sys_time.tm_sec, sys_time.tm_msec);
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
     // Write the actuation time
    if ((task_sampling_enable[(int) pdata] == 1) || (task_sampling_enable[(int) pdata] == 3)) {     
        // IOWR(GRTOS_MONITOR_BASE, 0, (int) pdata | 0xC00000);
        gk_MONITOR_FIFO_SAMPLE ((int) pdata | 0xC00000);
    }
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    #endif
    
}

GS_TCB *ptcb_array[G_MAX_NUMBER_OF_USER_TCB];

int main()
{
    printf("GeMRTOS\n");
    printf("Processors      = %d\n", (int)GRTOS_DRIVER_NPROCESSORS);
    printf("Clock frequency = %d MHz\n", (int)GRTOS_DRIVER_GRTOSFREQUENCY);
    printf("Clock prescaler = %d\n", (int)GRTOS_DRIVER_PRESCALE);
    printf("Interrupts      = %#010x\n", (int)GRTOS_DRIVER_GRTOSINTERRUPTS);  
    printf("Generation ID   = %d\n", (int)GRTOS_DRIVER_GRTOSGENERATIONID);
    printf("Processor type  = %s\n", GRTOS_DRIVER_PROCESSOR_TYPE);
    printf("ICache size     = %s\n", GRTOS_DRIVER_INSTRUCTION_CACHE_SIZE);
    printf("ICache Burst    = %s\n", GRTOS_DRIVER_INSTRUCTION_CACHE_BURST);
    printf("HSP Access      = %s\n", GRTOS_DRIVER_ENABLE_HPS_MAP_ACCESS);
    printf("Processor rate  = %d\n", (int)GRTOS_DRIVER_NIOS_CLOCK_FREQUENCY);
    printf("Bus rate        = %d\n", (int)GRTOS_DRIVER_BUS_CLOCK_FREQUENCY);
    printf("Address width   = %d\n", (int)GRTOS_DRIVER_BUS_WIDTH);
    printf("Program memory  = %d %s\n", (int)GRTOS_DRIVER_EXTERNAL_MEMORY_SPAN, GRTOS_DRIVER_EXTERNAL_MEMORY_UNIT);
    printf("GeMRTOS controller information ###########\n");
    IOWR_GRTOS_MUTEXBLOCKED32(500);    // Set the time the mutex is blocked after release
    GRTOS_CMD_FRZ_TM_THR_SET(100000);  // Set the frozen threshold
    GRTOS_CMD_FRZ_ENB_SET;            // Enable the frozen mode
    printf("Mutex Block release time                 = 0x%x\n", (int) IORD_GRTOS_MUTEXBLOCKED32);
    printf("Frozen threshold register (R_FRZ_TM_THR) = 0x%llx\n", (unsigned long long) gu_get_frozen_threshold());
    printf("GCC information ###########\n");
    printf("sizeof(unsigned)   = %d\n", (int) sizeof(unsigned));
    printf("sizeof(unsigned long)   = %d\n", (int) sizeof(unsigned long));
    printf("sizeof(unsigned long long)   = %d\n", (int) sizeof(unsigned long long));
    
    
    
    
    
    void   *mem = malloc(sizeof(INT32) * G_MAX_NUMBER_OF_USER_TCB * TASK_STACKSIZE + 31);
    generic_stk = (INT32 *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
    
    // ###################################################
    // main variable definition
    // ################################################### 
    GS_TCB *ptcb1;
    int i;

    fprintf(stderr,"[ MESSAGE ] GRTOS_DRIVER_GRTOSFREQUENCY %d\n", (int) GRTOS_DRIVER_GRTOSFREQUENCY);
    fprintf(stderr,"[ MESSAGE ] GRTOS_DRIVER_PRESCALE %d\n", (int) GRTOS_DRIVER_PRESCALE);
    fprintf(stderr,"[ MESSAGE ] G_TICKS_PER_SECOND %d\n", (int) G_TICKS_PER_SECOND);

    #if MODE_FREQUENTIAL_TASK == 1
        for ( i = 0; i < G_MAX_NUMBER_OF_USER_TCB; i++ ) {
            // ###################################################
            // inicialize the invocations numbers
            // ###################################################
            task_invocation_number[i] = 0;
            task_sampling_enable[i] = 0;
            
            /** TASK 2 **/
            ptcb_array[i] = gu_GetTask((void *) task_generic,                       ///< Pointer to the beginning of the task code
                                       (void *) i,                                  ///< Pointer to the argument of the first call
                                       (void *) generic_stk + sizeof(INT32) * ((i * TASK_STACKSIZE) + (TASK_STACKSIZE-4)),  ///< Botton of the Stack of the Task
                                       (unsigned int)  TASK_STACKSIZE);             /// Size of the stack in words
                               
            if (ptcb_array[i] != (void *) 0)
            {
                generic_task_tcbs[i] = ptcb_array[i];
                generic_task_index[i] = i;
                
                USER_TCB_execution_time[i] = 0;
                
                gu_SetTaskType(ptcb_array[i], G_TASK_TYPE_PERIODIC);
                // gu_SetTaskList(ptcb_array[i], (struct gs_lcb *) 0);
                gu_SetTaskReadyPriority(ptcb_array[i], USER_TCB_priority[i]);
                gu_SetTaskRunPriority(ptcb_array[i], USER_TCB_priority[i]);
                gu_SetTaskDeadline(ptcb_array[i], 0, 0, USER_TCB_period_s[i], USER_TCB_period_ms[i]);
                gu_SetTaskPeriod(ptcb_array[i], 0, 0,  USER_TCB_period_s[i], USER_TCB_period_ms[i]);
                gu_SetTaskAbortwhenDeadline(ptcb_array[i], 0);       
                gu_StartTaskwithOffset(ptcb_array[i], 0, 0, 5, 0);
            }
            gu_signal_create(G_SCBType_TCB_ABORTED, 0, (void *) ptcb_array[i], (void *) sig_aborted_task_generic,  (void *) 1);  /// Abort when deadline
            #if G_DEBUG_WHILEFOREVER_ENABLE == 1
                fprintf(stderr,"[ MESSAGE ] TASK %d CREATED\n", (int) i);
            #endif
        }
        // task_sampling_enable[0] = 2;
    #endif


    #if MODE_PRINTING_TASK == 1    
        // ###################################################
        // inicialize the invocations numbers
        // ###################################################
        for ( i = 0; i < G_MAX_NUMBER_OF_USER_TCB; i++ ) {
            task_invocation_number[i] = 0;
            task_sampling_enable[i] = 0;
        }
        
        // ###################################################
        // General Resources creation
        // ###################################################     
        // Create semaphore semaphore1
        semaphore1 = gu_sem_create((gt_time) 1,(int) 1,(gt_time) 0,(gt_time) 0);
        // Create message queue
        // pqueue = gu_queue_create((void *) queue_buffer, (INT32) sizeof(queue_buffer));
         
        // ###################################################
        // TASKs creation 
        // ###################################################     

    
        *mem = malloc(sizeof(INT32) * TASK_STACKSIZE + 31);
        INT32   *task1_stk = (INT32 *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
     
        /** TASK 1 **/
        ptcb1 = gu_GetTask((void *)        task1,                         ///< Pointer to the beginning of the task code
                           (void *)        NULL,                          ///< Pointer to the argument of the first call
                           (void *)        task1_stk + (sizeof(INT32) * TASK_STACKSIZE) -4,  ///< Botton of the Stack of the Task
                           (unsigned int)  TASK_STACKSIZE);	              ///< Size of the stack in words
                           
        if (ptcb1 != (void *) 0)
        {
            gu_SetTaskType(ptcb1, G_TASK_TYPE_PERIODIC);                  ///< Size of the stack in words
            // gu_SetTaskList(ptcb1, 0);
            gu_SetTaskReadyPriority(ptcb1, TASK1_PRIORITY);
            gu_SetTaskRunPriority(ptcb1, TASK1_PRIORITY);
            gu_SetTaskDeadline(ptcb1, 0, 0, 30, 0);
            gu_SetTaskPeriod(ptcb1, 0, 0, 30, 0);
            gu_SetTaskAbortwhenDeadline(ptcb1, 0);      
            gu_StartTaskwithOffset(ptcb1, 0, 0, 0, 0);
        }    
        gu_signal_create(G_SCBType_TCB_ABORTED, 0, (void *) ptcb1, (void *) sig_aborted_task1,  (void *) 1);  /// Abort when deadline

        *mem = malloc(sizeof(INT32) * TASK_STACKSIZE + 31);
        INT32   *task2_stk = (INT32 *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);

        /** TASK 2 **/
        ptcb1 = gu_GetTask((void *)        task2,                          ///< Pointer to the beginning of the task code
                           (void *)        2,                          ///< Pointer to the argument of the first call
                           (void *)        task2_stk + sizeof(INT32) * TASK_STACKSIZE-4,  ///< Botton of the Stack of the Task
                           (unsigned int)  TASK_STACKSIZE);	            /// Size of the stack in words
                           
        if (ptcb1 != (void *) 0)
        {
            gu_SetTaskType(ptcb1, G_TASK_TYPE_PERIODIC);
            // gu_SetTaskList(ptcb1, 0);
            gu_SetTaskReadyPriority(ptcb1, TASK2_PRIORITY);
            gu_SetTaskRunPriority(ptcb1, TASK2_PRIORITY);
            gu_SetTaskDeadline(ptcb1, 0, 0, 10, 0);
            gu_SetTaskPeriod(ptcb1, 0, 0, 10, 0);
            gu_SetTaskAbortwhenDeadline(ptcb1, 0);       
            gu_StartTaskwithOffset(ptcb1, 0, 0, 20, 0);
        }    

        *mem = malloc(sizeof(INT32) * TASK_STACKSIZE + 31);
        INT32   *task3_stk = (INT32 *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);

        /** TASK 3 **/
        ptcb1 = gu_GetTask((void *)        task3,                          ///< Pointer to the beginning of the task code
                           (void *)        NULL,                          ///< Pointer to the argument of the first call
                           (void *)        task3_stk + sizeof(INT32) * TASK_STACKSIZE-4,  ///< Botton of the Stack of the Task
                           (unsigned int)  TASK_STACKSIZE);	            /// Size of the stack in words
                           
        if (ptcb1 != (void *) 0)
        {
            gu_SetTaskType(ptcb1, G_TASK_TYPE_PERIODIC);
            // gu_SetTaskList(ptcb1, 0);
            gu_SetTaskReadyPriority(ptcb1, TASK3_PRIORITY);
            gu_SetTaskRunPriority(ptcb1, TASK3_PRIORITY);
            gu_SetTaskDeadline(ptcb1, 0, 0, 25, 0);
            gu_SetTaskPeriod(ptcb1, 0, 0, 25, 0);
            gu_SetTaskAbortwhenDeadline(ptcb1, 0);       
            gu_StartTaskwithOffset(ptcb1, 0, 0, 23, 0);
        } 
        
        /** TASK 4 **/
        *mem = malloc(sizeof(INT32) * TASK_STACKSIZE + 31);
        INT32   *task4_stk = (INT32 *) (((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);

        ptcb1 = gu_GetTask((void *)        task4,                          ///< Pointer to the beginning of the task code
                           (void *)        NULL,                          ///< Pointer to the argument of the first call
                           (void *)        task4_stk + sizeof(INT32) * TASK_STACKSIZE-4,  ///< Botton of the Stack of the Task
                           (unsigned int)  TASK_STACKSIZE);	            /// Size of the stack in words
                           
        if (ptcb1 != (void *) 0)
        {
            gu_SetTaskType(ptcb1, G_TASK_TYPE_UCOS);
            // gu_SetTaskList(ptcb1, 0);
            gu_SetTaskReadyPriority(ptcb1, TASK4_PRIORITY);
            gu_SetTaskRunPriority(ptcb1, TASK4_PRIORITY);
            gu_SetTaskDeadline(ptcb1, 0, 0, 25, 0);
            gu_SetTaskPeriod(ptcb1, 0, 0, 25, 0);
            gu_SetTaskAbortwhenDeadline(ptcb1, 0);      
            gu_StartTaskwithOffset(ptcb1, 0, 0, 5, 0);
        }
    #endif  
    
    #if G_DEBUG_WHILEFOREVER_ENABLE == 1
        fprintf(stderr,"[ MESSAGE ] FINISHING EXECUTION OF MAIN\n");
    #endif
  return 0;
}
