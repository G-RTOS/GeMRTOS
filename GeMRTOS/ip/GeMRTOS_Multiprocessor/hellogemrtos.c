/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS dining philosophers example
 *  \version 0.9a
 *  \date      2015-2023
 *  \bug  None known
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
* Copyright (c) Ricardo L. Cayssials                                          *
* All rights reserved.                                                        *
*                                                                             *
******************************************************************************/

/* Include the GeMRTOS library */
#include <gemrtos.h>


// ####################################################################################
// Defining system parameters
#define NUMBER_PHILOSOPHERS 30

// ####################################################################################
// Global structure for task information storage

t_semaphore_resource *leds_sem;

/* Semaphore to exclude forks variable access */
t_semaphore_resource *forks[NUMBER_PHILOSOPHERS];
#define left_fork(i)  (i)
#define right_fork(i) ((i + 1) % NUMBER_PHILOSOPHERS)

/* leds variable retains the current value written to GeMRTOS outputs */
volatile unsigned int leds;

/* Producer consumer queue for jtag_uart output*/
G_RCB  *pqueue_out;

/* Producer consumer queue for jtag_uart input*/
G_RCB  *pqueue_in;

/* Trigger for jtag output server */
G_RCB *jtag_uart_0_out;

/* Trigger for jtag input server */
G_RCB *jtag_uart_0_in;

/******************************************************************/
/* Signal function when task is aborted (the deadline is missed)  */
/******************************************************************/
void sig_aborted_task_generic(int pdata)
{
    gu_printf("\nTASK %u ABORTED\n", (unsigned int) pdata);
}

/******************************************************************/
/* Signal function when system goes to frozen mode                */
/******************************************************************/
void sig_frozen_mode(int pdata)
{
    gu_printf("\nSYSTEM IN FROZEN MODE %u\n", (unsigned int) pdata);
}

/******************************************************************/
/* generic task function                                          */
/******************************************************************/
void task_philosopher(void* pdata)
{
	unsigned int i = (unsigned int) pdata;

	/* get the pointer to the TCB of the current task */
	GS_TCB *ptcb = gu_TaskGetCurrentTCB();
	
	/* get the semaphores in order */
	if (( i & 1) == 0) {  // philosopher is even
		gu_SemaphoreWait(forks[left_fork(i)], (int) G_TRUE);
		gu_SemaphoreWait(forks[right_fork(i)], (int) G_TRUE);
	} else {              // philosopher is odd
		gu_SemaphoreWait(forks[right_fork(i)], (int) G_TRUE);
		gu_SemaphoreWait(forks[left_fork(i)], (int) G_TRUE);
	}

	/* change the leds value using exclusion */
	gm_SchedulingListExclusionSectionEnter;
		leds = leds + 1; 
		/* write the value in leds variable to the GeMRTOS output port  */
		gm_WriteOutputs(leds);
	gm_SchedulingListExclusionSectionExit;
 
	gu_printf("%s is eating in processor %u\n", ptcb->TCB_description, (unsigned int) gm_ProcessorId);
	if (i > NUMBER_PHILOSOPHERS / 2)
		gu_MessageQueuePrintf(pqueue_out,"%s is eating in processor %d\n", ptcb->TCB_description, (int) gm_ProcessorId);	

	/* release the semaphore in order */
	gu_SemaphorePost((G_RCB *) forks[left_fork(i)]);
	gu_SemaphorePost((G_RCB *) forks[right_fork(i)]);	
}


struct jtag_uart_info {
	unsigned int trigger_out;
	unsigned int trigger_in;
	G_RCB *pqueue_out;
	G_RCB *pqueue_in;
	unsigned int buffer_length_out;
	unsigned int buffer_length_in;
	unsigned int jtag_base;
};

#ifdef JTAG_UART_0_BASE
void *GRTOS_IRQ_READ_SERVER_ENABLE(void * pdata)
{
	PRINT_ASSERT((gm_ProcessorId == GRTOS_MTX_PRC_GRANTED), "ERROR MTX not proc=%d, mtx=%d\n",(int) gm_ProcessorId, (int) GRTOS_MTX_PRC_GRANTED);

	// Enable interrupt in JTAG UART component
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
	IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, status | ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK);

	return ((void *) 0);
}

void *GRTOS_IRQ_READ_SERVER_DISABLE(void *pdata)
{
   PRINT_ASSERT((gm_ProcessorId == GRTOS_MTX_PRC_GRANTED), "ERROR MTX not proc=%d, mtx=%d\n",(int) gm_ProcessorId, (int) GRTOS_MTX_PRC_GRANTED);

	// Disable interrupt in JTAG UART component
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
	IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, status & ~(ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK));

    return ((void *) 0);
}

void GRTOS_IRQ_READ_SERVER(void *pdata)
{

	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pdata;

	unsigned int index_msg = 0;
	unsigned int uart_data;

	char *buffer_msg = (char *) malloc(jtag_uart_0_info->buffer_length_in);	
		PRINT_ASSERT(((unsigned int) buffer_msg % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) buffer_msg != 0), "ERROR no memory available");

    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();	
	
    /* Define this task as an ISR associated to trigger_out jtag_uart_0_in */
    gu_TriggerRegisterTask(ptcb, (G_INT32) jtag_uart_0_info->trigger_in);
	
	while (1) {
		
		/* wait for trigger to read the jtag buffer */
		gu_TriggerWait();


		while ((uart_data = IORD_ALTERA_AVALON_JTAG_UART_DATA(JTAG_UART_0_BASE)) & ALTERA_AVALON_JTAG_UART_DATA_RVALID_MSK) {

			char data = (char) (uart_data & ALTERA_AVALON_JTAG_UART_DATA_DATA_MSK) >> ALTERA_AVALON_JTAG_UART_DATA_DATA_OFST;

			buffer_msg[index_msg] = data;
			index_msg++;
			
			if ((data == '\r') || (data == '\n') || (index_msg >= jtag_uart_0_info->buffer_length_in - 2)) {
				buffer_msg[index_msg] = '\0'; // end of string
				/* Produce the message received to the queue_in */
				gu_fprintf("\nINPUT FROM JTAG-UART = %s\n", buffer_msg);
				gu_MessageQueuePrintf(jtag_uart_0_info->pqueue_in,"%s", buffer_msg);
				index_msg = 0;
				break;
			}
		}
	}

}

void *GRTOS_IRQ_WRITE_SERVER_ENABLE(void * pdata)
{
	PRINT_ASSERT((gm_ProcessorId == GRTOS_MTX_PRC_GRANTED), "ERROR MTX not proc=%d, mtx=%d\n",(int) gm_ProcessorId, (int) GRTOS_MTX_PRC_GRANTED);


	// Enable interrupt in JTAG UART component
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
	IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, status | ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK);

    return ((void *) 0);
}

void *GRTOS_IRQ_WRITE_SERVER_DISABLE(void *pdata)
{
	PRINT_ASSERT((gm_ProcessorId == GRTOS_MTX_PRC_GRANTED), "ERROR MTX not proc=%d, mtx=%d\n",(int) gm_ProcessorId, (int) GRTOS_MTX_PRC_GRANTED);

	// Disable interrupt in JTAG UART component
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
	IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, status & ~(ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK));

    return ((void *) 0);
}


void GRTOS_IRQ_WRITE_SERVER(void *pdata)
{

	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pdata;
	
	/* Get a buffer to store the messaje to send to the jtag-uart */
    char *buffer_msg = (char *) malloc(jtag_uart_0_info->buffer_length_out);
		PRINT_ASSERT(((unsigned int) buffer_msg % 4 == 0), "ERROR memory is not alligned");
		PRINT_ASSERT(((unsigned int) buffer_msg != 0), "ERROR no memory available");
    
    int size;
    unsigned int index_msg;
	
    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_TaskGetCurrentTCB();

    /* Define this task as an ISR associated to trigger_out jtag_uart_0_out */
    gu_TriggerRegisterTask(ptcb, (G_INT32) jtag_uart_0_info->trigger_out);

    while (1) {
		/* receive next message from producer/consumer queue */
        size = gu_MessageQueueReceive(jtag_uart_0_info->pqueue_out, (void *) buffer_msg, jtag_uart_0_info->buffer_length_out);
		
		/* Iterate for each character of the message, but the ending '\0' */
        index_msg = 0;
        while (index_msg < size-1)
        {
            // int space_fifo;
			/* Get the available space in jtag uart buffer */
			int space_fifo = (((G_INT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base)) & ((G_INT32) ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_OFST;

			// if (space_fifo > size) space_fifo = size - 2;
			/* Send as many characters up to the end of message or as possible to jtag uart */
			while (space_fifo > 0 && index_msg < size-1)
			{
				PRINT_ASSERT((buffer_msg[index_msg] != '\0'), "Error char zero=%d", (int) buffer_msg[index_msg]);

				/* write next message character to jtag uart output fifo */
				IOWR_ALTERA_AVALON_JTAG_UART_DATA(jtag_uart_0_info->jtag_base, buffer_msg[index_msg]);

				/* Update iteration variables */
				index_msg++;
				space_fifo--;
			}
			/* There exist more character to send, so wait to next trigger_out from jtag device */
            if (index_msg < size-1) {
                gu_TriggerWait();
            }
        }
    };
}


/* This function is the ISR for the jtag uart named "jtag_uart_0_out" */
void jtag_irq_manager(int pdata)
{

	/* Read the state of interrupt flags from the JTAG UART status register */
    volatile G_INT32 uart_ctrl_register = (G_INT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
    
	/* Get the state of the write interrupt (output buffer empty) */
    volatile G_INT32 write_interrupt = (uart_ctrl_register  & ((G_INT32) ALTERA_AVALON_JTAG_UART_CONTROL_WI_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_WI_OFST;
	
    /* Get the state of the read interrupt (input buffer full) */
    volatile G_INT32 read_interrupt = (uart_ctrl_register  & ((G_INT32) ALTERA_AVALON_JTAG_UART_CONTROL_RI_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_RI_OFST;    

	/* If write interrupt is on, the release the output server trigger_out */
    if ((write_interrupt == (G_INT32) 1) && (jtag_uart_0_out->trigger.TRG_Enabled == G_TRUE)) {
		PRINT_ASSERT((jtag_uart_0_out->trigger.TRG_Enabled == G_TRUE),"ERROR trigger write is not enabled\n");
		gu_TriggerRelease ((int) jtag_uart_0_out);
	}
	
	/* If write interrupt is on, the release the output server trigger_in */
    if ((read_interrupt == (G_INT32) 1) && (jtag_uart_0_in->trigger.TRG_Enabled == G_TRUE)) {
		PRINT_ASSERT((jtag_uart_0_in->trigger.TRG_Enabled == G_TRUE),"ERROR trigger read is not enabled\n");
		gu_TriggerRelease ((int) jtag_uart_0_in);
	}
	PRINT_ASSERT(((write_interrupt == (G_INT32) 1) || (read_interrupt == (G_INT32) 1)),"ERROR not interrupt triggering");
}
#endif

int main(void)
{
    /* ################################################### */
    /* main variable definition                            */
	/* ################################################### */
    GS_TCB *ptcb;

    /* Create a semaphore for LEDS */
    leds_sem = gu_SemaphoreCreate((int) 1);
	
	/* Create semaphores for forks */
	for (int i = 0; i < NUMBER_PHILOSOPHERS; i++) {
		forks[i] = gu_SemaphoreCreate((int) 1);
	}		
	
	// Create the philosopher
	for (unsigned int i = 0; i < NUMBER_PHILOSOPHERS; i++)
	{
		/* Create a new Task */
		ptcb = gu_TaskCreate((void *) task_philosopher,    ///< Pointer to the task code
						  (void *) i,                   ///< Pointer to the argument of first call
						  "task %d", i);
									   
		if (ptcb != (void *) 0) { /* Task was created successfully */
			/* Set the task as periodic */
			gu_TaskTypeSet(ptcb, G_TCBType_PERIODIC);
			
			/* Set the task priority equal to the task index plus one */
			gu_TaskReadyPrioritySet(ptcb, (G_INT64) i+1);
			gu_TaskRunPrioritySet(ptcb,(G_INT64) i+1);

			/* Set the period equal to the task index plus one in seconds */
			gu_TaskPeriodSet(ptcb, 0, 0, 1, (i+1) * 100);
			
			/* Start execution of task with an offset equal to five seconds */
			gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);
			
			// Signal when task is aborted because of deadline
			gu_SignalCreate(G_SCBType_TCB_ABORTED, 0, (void *) ptcb, (void *) sig_aborted_task_generic, (void *) i);
		}
		else {
			gu_printf("Error when creating task\n");
			while(1);
		}
	}

	gu_SignalCreate(G_SCBType_FROZEN_MODE, 0, (void *) 0, (void *) sig_frozen_mode, (void *) 0);


#ifdef JTAG_UART_0_BASE

    /* Create a message queue for JTAG-UART jtag_uart_0 output */
    pqueue_out = gu_MessageQueueCreate();	
	
    /* Create a message queue for JTAG-UART jtag_uart_0 input */
    pqueue_in = gu_MessageQueueCreate();

	/* Create a trigger_out for JTAG UART writting */
	/* -1 because it is not associated with hardware interrupt */
	jtag_uart_0_out = gu_TriggerCreate(-1);
	PRINT_ASSERT((gkm_RCB_IsValid(jtag_uart_0_out) == G_TRUE),"ERROR ECB not valid\n");
	
	/* Create a trigger_in for JTAG UART reading */
	/* -1 because it is not associated with hardware interrupt */
	jtag_uart_0_in = gu_TriggerCreate(-1);
	PRINT_ASSERT((gkm_RCB_IsValid(jtag_uart_0_in) == G_TRUE),"ERROR ECB not valid\n");

	/* create a jtag_uart_info structure to store the information for output server consumer */
	struct jtag_uart_info *jtag_uart_0_info = (struct jtag_uart_info *) malloc(sizeof(struct jtag_uart_info));
	
	/* set the parameter for jtag output server configuration. 
	   The pointer to this structure is pass as reference to the task */
	jtag_uart_0_info->pqueue_out = pqueue_out;
	jtag_uart_0_info->pqueue_in = pqueue_in;
	jtag_uart_0_info->trigger_out = (unsigned int) jtag_uart_0_out;
	jtag_uart_0_info->trigger_in = (unsigned int) jtag_uart_0_in;
	jtag_uart_0_info->buffer_length_in = 100;
	jtag_uart_0_info->buffer_length_out = 100;
	jtag_uart_0_info->jtag_base = JTAG_UART_0_BASE;

	/* Creatse task for jtag output server */
	ptcb = gu_TaskCreate((void *) GRTOS_IRQ_WRITE_SERVER,
					   (void *) jtag_uart_0_info,
					   "GRTOS_IRQ_WRITE_SERVER");
	if (ptcb != (void *) 0)
	{
		gu_TaskTypeSet(ptcb, G_TCBType_OneShot);

		// gu_SchedulingListAssociateTask(ptcb, (struct gs_lcb *) pedf_list);
		gu_TaskReadyPrioritySet(ptcb, (G_INT64) 0);
		gu_TaskRunPrioritySet(ptcb, (G_INT64) 0);

		gu_TaskPeriodSet(ptcb, 0, 0,  1, 1);

		/* Start execution of task with an offset */
		gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);
	}
	/* suscribe task as consumer to the consumer/producer queue */
	gu_MessageQueueSubscribe(ptcb, pqueue_out);
	
	/* Set the jtag output server hooks */
	gu_TriggerEnableHook((int) jtag_uart_0_out, (void *) GRTOS_IRQ_WRITE_SERVER_ENABLE, (void *) NULL);
	gu_TriggerDisableHook((int) jtag_uart_0_out, (void *) GRTOS_IRQ_WRITE_SERVER_DISABLE, (void *) NULL);

	/* Creatse task for jtag output server */
	ptcb = gu_TaskCreate((void *) GRTOS_IRQ_READ_SERVER,
					   (void *) jtag_uart_0_info,
					   "GRTOS_IRQ_READ_SERVER");
	if (ptcb != (void *) 0)
	{
		gu_TaskTypeSet(ptcb, G_TCBType_OneShot);
	
		// gu_SchedulingListAssociateTask(ptcb, (struct gs_lcb *) pedf_list);
		gu_TaskReadyPrioritySet(ptcb, (G_INT64) 0);
		gu_TaskRunPrioritySet(ptcb, (G_INT64) 0);
	
		gu_TaskPeriodSet(ptcb, 0, 0,  1, 1);
		
		/* Start execution of task with an offset */
		gu_TaskStartWithOffset(ptcb, 0, 0, 5, 0);
	}
	
	/* Set the jtag output server hooks */
	gu_TriggerEnableHook((int) jtag_uart_0_in, (void *) GRTOS_IRQ_READ_SERVER_ENABLE, (void *) NULL);
	gu_TriggerDisableHook((int) jtag_uart_0_in, (void *) GRTOS_IRQ_READ_SERVER_DISABLE, (void *) NULL);	
	// 22-03-2024 ####################################	

	// #################################
	/* Create a ISR task for jtag uart hardware interrupt */
	ptcb = gu_TaskCreate((void *) jtag_irq_manager,
					   (void *) 5, 
					   "jtag_irq_manager");
					   
	PRINT_ASSERT((gkm_TCB_IsValid(ptcb) == G_TRUE),"ERROR TCB is not valid\n");
	if (ptcb != (void *) 0)
	{
		gu_TaskTypeSet(ptcb, G_TCBType_OneShot);

		// gu_SchedulingListAssociateTask(ptcb, (struct gs_lcb *) pedf_list);
		
		gu_TaskReadyPrioritySet(ptcb, (G_INT64) 0);
		gu_TaskRunPrioritySet(ptcb, (G_INT64) 0);

		gu_TaskPeriodSet(ptcb, 0, 0,  1, 1);
	}        
	
	/* Associate the task to be triggered with the jtag uart interrupt */
	gu_TriggerRegisterTask(ptcb, (int) JTAG_UART_0_IRQ);
	gu_TriggerEnable((int) JTAG_UART_0_IRQ);
#endif
	
    return (0);
}

