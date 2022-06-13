#ifndef RTS_H_ 
#define RTS_H_
#define G_MAX_NUMBER_OF_USER_TCB         5
volatile int USER_TCB_dummy[G_MAX_NUMBER_OF_USER_TCB] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
volatile int USER_TCB_execution_time[G_MAX_NUMBER_OF_USER_TCB] = {1,1,1,1,1};
volatile int USER_TCB_period_ms[G_MAX_NUMBER_OF_USER_TCB] = {0,0,0,0,0};
volatile int USER_TCB_period_s[G_MAX_NUMBER_OF_USER_TCB] = {6,51600,51600,51600,51600};
volatile int USER_TCB_priority[G_MAX_NUMBER_OF_USER_TCB] = {1,2,3,4,5};
#endif
