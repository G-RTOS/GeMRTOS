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
#include <grtosdefinitions.h>


// #pragma GCC optimize('mno-cache-volatile')


/*************************************************************************************/
/*************************************************************************************/
/***********REAL-TIME SYSTEM DEFINITION***********************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

t_semaphore_resource *semaphore1;

INT32  queue_buffer[4096];
G_RCB  *pqueue;

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       4096

/* Definition of Task Priorities */

/************************************************************************************/
/* TASK 1 ***************************************************************************/
/************************************************************************************/
/* Prints "Hello World" and sleeps for three seconds */
INT32   task1_stk[TASK_STACKSIZE];
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
INT32    task2_stk[TASK_STACKSIZE];
INT32    consumer_buffer2[1024];

#define TASK2_PRIORITY      1
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata)
{
    gt_tm sys_time; 

    GS_ECB  *pbuffer;
    pbuffer = gu_queue_consume((G_RCB *)pqueue, (void *)consumer_buffer2, (INT32) sizeof(consumer_buffer2), (gt_time) 0);
    
    sys_time = gu_Clock(gu_get_now());
    printf("Hi task2: ");
    printf("Proc: %d, ", gu_Get_CPU_ID());
    printf("y= %d, d= %d, ", sys_time.tm_year, sys_time.tm_day);
    printf("h= %d, m= %d, ", sys_time.tm_hour, sys_time.tm_min);
    printf("s= %d, ms= %d\n", sys_time.tm_sec, sys_time.tm_msec);     
}

/************************************************************************************/
/* TASK 3 ***************************************************************************/
/************************************************************************************/
INT32    task3_stk[TASK_STACKSIZE];
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
INT32   task4_stk[TASK_STACKSIZE];
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


int main()
{

    // ###################################################
    // main variable definition
    // ################################################### 
    GS_TCB *ptcb1;
    
    // ###################################################
    // General Resources creation
    // ###################################################     
    // Create semaphore semaphore1
    semaphore1 = gu_sem_create((gt_time) 1,(int) 1,(gt_time) 0,(gt_time) 0);
    // Create message queue
    pqueue = gu_queue_create((void *) queue_buffer, (INT32) sizeof(queue_buffer));
     
    // ###################################################
    // TASKs creation 
    // ###################################################     
 
    /** TASK 1 **/
    ptcb1 = gu_GetTask((void *)        task1,                         ///< Pointer to the beginning of the task code
				       (void *)        NULL,                          ///< Pointer to the argument of the first call
				       (void *)        &task1_stk[TASK_STACKSIZE-4],  ///< Botton of the Stack of the Task
                       (unsigned int)  TASK_STACKSIZE);	              ///< Size of the stack in words
                       
    if (ptcb1 != (void *) 0)
    {
        gu_SetTaskType(ptcb1, G_TASK_TYPE_PERIODIC);                  ///< Size of the stack in words
        gu_SetTaskList(ptcb1, 0);
        gu_SetTaskReadyPriority(ptcb1, TASK1_PRIORITY);
        gu_SetTaskRunPriority(ptcb1, TASK1_PRIORITY);
        gu_SetTaskDeadline(ptcb1, 0, 0, 30, 0);
        gu_SetTaskPeriod(ptcb1, 0, 0, 30, 0);
        gu_SetTaskAbortwhenDeadline(ptcb1, 0);      
        gu_StartTaskwithOffset(ptcb1, 0, 0, 0, 0);
    }    
//    gu_signal_create(G_SCB_TCB_ABORTED, 0, (void *) ptcb1, (void *) sig_aborted_task1);
    							   /// Abort when deadline              ------


    /** TASK 2 **/
    ptcb1 = gu_GetTask((void *)        task2,                          ///< Pointer to the beginning of the task code
				       (void *)        NULL,                          ///< Pointer to the argument of the first call
				       (void *)        &task2_stk[TASK_STACKSIZE-4],  ///< Botton of the Stack of the Task
                       (unsigned int)  TASK_STACKSIZE);	            /// Size of the stack in words
                       
    if (ptcb1 != (void *) 0)
    {
        gu_SetTaskType(ptcb1, G_TASK_TYPE_PERIODIC);
        gu_SetTaskList(ptcb1, 0);
        gu_SetTaskReadyPriority(ptcb1, TASK2_PRIORITY);
        gu_SetTaskRunPriority(ptcb1, TASK2_PRIORITY);
        gu_SetTaskDeadline(ptcb1, 0, 0, 10, 0);
        gu_SetTaskPeriod(ptcb1, 0, 0, 10, 0);
        gu_SetTaskAbortwhenDeadline(ptcb1, 0);       
        gu_StartTaskwithOffset(ptcb1, 0, 0, 20, 0);
    }    


    /** TASK 3 **/
    ptcb1 = gu_GetTask((void *)        task3,                          ///< Pointer to the beginning of the task code
				       (void *)        NULL,                          ///< Pointer to the argument of the first call
				       (void *)        &task3_stk[TASK_STACKSIZE-4],  ///< Botton of the Stack of the Task
                       (unsigned int)  TASK_STACKSIZE);	            /// Size of the stack in words
                       
    if (ptcb1 != (void *) 0)
    {
        gu_SetTaskType(ptcb1, G_TASK_TYPE_PERIODIC);
        gu_SetTaskList(ptcb1, 0);
        gu_SetTaskReadyPriority(ptcb1, TASK3_PRIORITY);
        gu_SetTaskRunPriority(ptcb1, TASK3_PRIORITY);
        gu_SetTaskDeadline(ptcb1, 0, 0, 25, 0);
        gu_SetTaskPeriod(ptcb1, 0, 0, 25, 0);
        gu_SetTaskAbortwhenDeadline(ptcb1, 0);       
        gu_StartTaskwithOffset(ptcb1, 0, 0, 23, 0);
    } 

    /** TASK 4 **/
    ptcb1 = gu_GetTask((void *)        task4,                          ///< Pointer to the beginning of the task code
				       (void *)        NULL,                          ///< Pointer to the argument of the first call
				       (void *)        &task4_stk[TASK_STACKSIZE-4],  ///< Botton of the Stack of the Task
                       (unsigned int)  TASK_STACKSIZE);	            /// Size of the stack in words
                       
    if (ptcb1 != (void *) 0)
    {
        gu_SetTaskType(ptcb1, G_TASK_TYPE_UCOS);
        gu_SetTaskList(ptcb1, 0);
        gu_SetTaskReadyPriority(ptcb1, TASK4_PRIORITY);
        gu_SetTaskRunPriority(ptcb1, TASK4_PRIORITY);
        gu_SetTaskDeadline(ptcb1, 0, 0, 25, 0);
        gu_SetTaskPeriod(ptcb1, 0, 0, 25, 0);
        gu_SetTaskAbortwhenDeadline(ptcb1, 0);      
        gu_StartTaskwithOffset(ptcb1, 0, 0, 5, 0);
    }
    
  return 0;
}
