#include <gemrtos.h>
#include <stdio.h>
#include <stdlib.h>


/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GEmRTOS CORE definitions
 *  \details This file contains the Message queue definitions.
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
#include <stdio.h>
#include <stdlib.h>


// ####################################################################################
// Defining system parameters
#define number_EDF_lists    2
#define number_FP_lists     2
#define tasks_per_list      15

// ####################################################################################
// Global structure for task information storage
// Declaring structures to hold data for easy displaying
struct task_information {
    G_INT32  list_index; // Index to the scheduling list pointer in scheduling list array
    G_INT32  task_index; // Index to the task pointer in task array
};

/* Define the number of TASK equal to ten times the number of processors */
#define G_MAX_NUMBER_OF_USER_TCB (1 * G_NUMBER_OF_PCB)

/* Semaphore to exclude LEDs variable access */
t_semaphore_resource *leds_sem;

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
    gu_fprintf("\nTASK %d ABORTED\n", pdata);
}

/******************************************************************/
/* generic task function                                          */
/******************************************************************/
void task_generic(void* pdata)
{
	struct task_information *ptask_info = (struct task_information *) pdata;
	
	/* get the pointer to the TCB of the current task */
	GS_TCB *ptcb = gu_GetCurrentTCB();
	
	/* get the led_sem semaphore to exclude reading leds variable */
    int sem = (int) gu_sem_wait(leds_sem, (int) G_TRUE);
	
	/* if semaphore is granted, change the leds value             */
    if (sem == G_TRUE) {
        leds = leds ^ (unsigned int) ptask_info->task_index;
		
		/* write the value in leds variable to the GeMRTOS output port  */
        gu_write_outputs(leds);
		
		/* release the led_sem semaphore                                */
        gu_sem_post((G_RCB *) leds_sem);
    }
	/* print to STDOUT the task description and the processor */
    gu_printf("%s by processor %d\n", ptcb->TCB_description, (int) GRTOS_CMD_PRC_ID);

	if (ptask_info->task_index > 30) {
		/* Print to the system JTAG-UART as through the consumer-producer queue */
		gu_queue_printf(pqueue_out,"%s by processor %d\n", ptcb->TCB_description, (int) GRTOS_CMD_PRC_ID);
	}
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


void *GRTOS_IRQ_READ_SERVER_ENABLE(void * pdata)
{
    PRINT_ASSERT((GRTOS_CMD_PRC_ID == GRTOS_MTX_PRC_GRANTED), "ERROR MTX not proc=%d, mtx=%d\n",(int) GRTOS_CMD_PRC_ID, (int) GRTOS_MTX_PRC_GRANTED);    
    
#ifdef __niosX_arch__
	// Enable interrupt in JTAG UART component
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, status | ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK);
#endif
    return ((void *) 0);
}

void *GRTOS_IRQ_READ_SERVER_DISABLE(void *pdata)
{
   PRINT_ASSERT((GRTOS_CMD_PRC_ID == GRTOS_MTX_PRC_GRANTED), "ERROR MTX not proc=%d, mtx=%d\n",(int) GRTOS_CMD_PRC_ID, (int) GRTOS_MTX_PRC_GRANTED);
    
#ifdef __niosX_arch__
	// Disable interrupt in JTAG UART component
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, status & ~(ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK));
#endif
    // Dummy read to take effect
    // IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
    return ((void *) 0);
}

void GRTOS_IRQ_READ_SERVER(void *pdata)
{
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pdata;

	unsigned int index_msg = 0;
	
	/* Get a buffer to store the messaje to send to the jtag-uart queue_in */
    void *mem = malloc(jtag_uart_0_info->buffer_length_in);
	PRINT_ASSERT(((unsigned int) mem % 4 == 0), "ERROR memory is not alligned");
    char *buffer_msg = (char *) mem;	
	
    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_PCB_GetCurrentTCB();	
	
    /* Define this task as an ISR associated to trigger_out jtag_uart_0_in */
    gu_trigger_register_task(ptcb, (G_INT32) jtag_uart_0_info->trigger_in);
	
	while (1) {
		
		/* wait for trigger to read the jtag buffer */
		gu_trigger_wait();
	
		unsigned int uart_data; // = IORD_ALTERA_AVALON_JTAG_UART_DATA(JTAG_UART_0_BASE);
		while ((uart_data = IORD_ALTERA_AVALON_JTAG_UART_DATA(JTAG_UART_0_BASE)) & ALTERA_AVALON_JTAG_UART_DATA_RVALID_MSK) {

			char data = (char) (uart_data & ALTERA_AVALON_JTAG_UART_DATA_DATA_MSK) >> ALTERA_AVALON_JTAG_UART_DATA_DATA_OFST;

			buffer_msg[index_msg] = data;
			index_msg++;
			
			if ((data == '\r') || (data == '\n') || (index_msg >= jtag_uart_0_info->buffer_length_in - 2)){
				buffer_msg[index_msg] = 0;
				/* Produce the message received to the queue_in */
				fprintf(stderr, "\nINPUT FROM JTAG-UART = %s\n", buffer_msg);
				gu_queue_printf(jtag_uart_0_info->pqueue_in,"%s", buffer_msg);
				index_msg = 0;
			}
		
			// uart_data = IORD_ALTERA_AVALON_JTAG_UART_DATA(JTAG_UART_0_BASE);
		}
	}
	
}

void *GRTOS_IRQ_WRITE_SERVER_ENABLE(void * pdata)
{
    // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    PRINT_ASSERT((GRTOS_CMD_PRC_ID == GRTOS_MTX_PRC_GRANTED), "ERROR MTX not proc=%d, mtx=%d\n",(int) GRTOS_CMD_PRC_ID, (int) GRTOS_MTX_PRC_GRANTED);    
    
#ifdef __niosX_arch__
	// Enable interrupt in JTAG UART component
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, status | ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK);
#endif
    return ((void *) 0);
}

void *GRTOS_IRQ_WRITE_SERVER_DISABLE(void *pdata)
{
    // gu_fprintf(stderr,"[ MESSAGE ] Executing  %s, %d, Proc: %d\n",__FUNCTION__,__LINE__,GRTOS_CMD_PRC_ID);
    PRINT_ASSERT((GRTOS_CMD_PRC_ID == GRTOS_MTX_PRC_GRANTED), "ERROR MTX not proc=%d, mtx=%d\n",(int) GRTOS_CMD_PRC_ID, (int) GRTOS_MTX_PRC_GRANTED);
    
#ifdef __niosX_arch__
	// Disable interrupt in JTAG UART component
	unsigned int status = IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, status & ~(ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK));
#endif
    // Dummy read to take effect
    // IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
    return ((void *) 0);
}


void GRTOS_IRQ_WRITE_SERVER(void *pdata)
{

	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pdata;
	
	/* Get a buffer to store the messaje to send to the jtag-uart */
    void *mem = malloc(jtag_uart_0_info->buffer_length_out);
	PRINT_ASSERT(((unsigned int) mem % 4 == 0), "ERROR memory is not alligned");
    char *buffer_msg = (char *) mem;
    
    int size;
    int space_fifo;
    unsigned int index_msg;
	
    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_PCB_GetCurrentTCB();


    /* Define this task as an ISR associated to trigger_out jtag_uart_0_out */
    gu_trigger_register_task(ptcb, (G_INT32) jtag_uart_0_info->trigger_out);

    // Enable interrupt in JTAG UART component
    // 07-09-2023 IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, 0x00000000);
    // Dummy read to take effect
    // 07-09-2023 IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);    


    while (1) {
		/* receive next message from producer/consumer queue */
        size = gu_queue_receive(jtag_uart_0_info->pqueue_out, (void *) buffer_msg, jtag_uart_0_info->buffer_length_out);
		
		/* Iterate for each character of the message, but the ending '\0' */
        index_msg = 0;
        while (index_msg < size-1)
        {
			/* Get the available space in jtag uart buffer */
            space_fifo = (((G_INT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base)) & ((G_INT32) ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_OFST;

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
                gu_trigger_wait();
            }
        }
    };
}


/* This function is the ISR for the jtag uart named "jtag_uart_0_out" */
void jtag_irq_manager(int pdata)
{
    gu_fprintf("\nIRQ=%d\n", (int) pdata);

	/* Read the state of interrupt flags from the JTAG UART status register */
    G_INT32 uart_ctrl_register = (G_INT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
    
	/* Get the state of the write interrupt (output buffer empty) */
    G_INT32 write_interrupt = (uart_ctrl_register  & ((G_INT32) ALTERA_AVALON_JTAG_UART_CONTROL_WI_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_WI_OFST;
	
    /* Get the state of the read interrupt (input buffer full) */
    G_INT32 read_interrupt = (uart_ctrl_register  & ((G_INT32) ALTERA_AVALON_JTAG_UART_CONTROL_RI_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_RI_OFST;    

	/* If write interrupt is on, the release the output server trigger_out */
    if (write_interrupt == (G_INT32) 1) gu_trigger_release ((int) jtag_uart_0_out);
	
	/* Read ISR should be defined for input reading */
	/* If write interrupt is on, the release the output server trigger_in */
    if (read_interrupt == (G_INT32) 1) {
		gu_fprintf("\n read_interrupt JTAG-UART READ SERVER \n");
		gu_trigger_release ((int) jtag_uart_0_in);
	}
}


// Declaring global array for easy access to task and scheduling list information
// Array with pointers to GS_TCB of tasks
// GS_TCB *tasks[tasks_per_list * (number_EDF_lists + number_FP_lists)];    
// Array with pointers to GS_LCBs of scheduling lists
// GS_LCB *scheduling_lists[number_EDF_lists + number_FP_lists];
// ####################################################################################



int main(void)
{

#ifndef __niosX_arch__
    printf("Hello GeMRTOS running in desktop environment\n");
#endif

    /* ################################################### */
    /* main variable definition                            */
	/* ################################################### */
    GS_TCB *ptcb;
	GS_LCB *plcb;
    int i;


    /* Create a semaphore for LEDS */
    leds_sem = gu_sem_create((int) 1);
    
    /* Create a message queue for JTAG-UART jtag_uart_0 output */
    pqueue_out = gu_queue_create();	
	
    /* Create a message queue for JTAG-UART jtag_uart_0 input */
    pqueue_in = gu_queue_create();		
	
    G_INT32 task_index;
    G_INT32 list_index;                         // index for array elements
    struct task_information *ptask_info;      // pointer to task information structure

    for (list_index = 0; list_index < ((G_INT32) number_FP_lists + (G_INT32) number_EDF_lists); list_index++)
    {
        if (list_index < number_FP_lists) {
            plcb = gu_Get_LCB((G_INT32) GS_LCBType_FP);
        } else {
            plcb = gu_Get_LCB((G_INT32) GS_LCBType_EDF);
        }
        PRINT_ASSERT((plcb != (GS_LCB *) 0),"ERROR");

		// Assigning all the processors to the list
		for (i = 1;  i <= G_NUMBER_OF_PCB;  i++) {
				gu_LCB_Associate_PCB((GS_LCB *) plcb, (G_INT32) i, (G_INT32) ((list_index+i+G_NUMBER_OF_PCB-1)%G_NUMBER_OF_PCB));
		}


		// Create all the tasks
		for (task_index=0; task_index < tasks_per_list; task_index++)
		{
			/* Get structure to hold task information */
			ptask_info = malloc(sizeof(struct task_information));
			// ptask_info->task_index = task_index;
			ptask_info->task_index = (list_index * tasks_per_list) + task_index;
			
			/* Create a new Task */
			ptcb = gu_GetTask((void *) task_generic,    ///< Pointer to the task code
										   (void *) ptask_info,      ///< Pointer to the argument of first call
										   "task %d", ptask_info->task_index);
										   
			if (ptcb != (void *) 0) { /* Task was created successfully */
				
				// Assigning the task to the scheduling list with the index equal to: task_index/tasks_per_list
				gu_SetTaskList(ptcb,(struct gs_lcb *) plcb);
				
				/* Set the task as periodic */
				gu_SetTaskType(ptcb, G_TCBType_PERIODIC);
				
				/* Set the task priority equal to the task index plus one */
				gu_SetTaskReadyPriority(ptcb, (long long) ptask_info->task_index+1);
				gu_SetTaskRunPriority(ptcb,(long long) ptask_info->task_index+1);

				/* Set the period and deadline equal to the task index plus one in seconds */
				gu_SetTaskDeadline(ptcb, 0, 0, 1, ptask_info->task_index * 100);
				gu_SetTaskPeriod(ptcb, 0, 0, 1, ptask_info->task_index * 100);
				
				/* Set the task with abort when deadline */
				gu_SetTaskAbortwhenDeadline(ptcb, 0);
				
				/* Start execution of task with an offset equal to five seconds */
				gu_StartTaskwithOffset(ptcb, 0, 0, 5, 0);
				gu_signal_create(G_SCBType_TCB_ABORTED, 0, (void *) ptcb, (void *) sig_aborted_task_generic, (void *) task_index);  /// Abort when deadline
			}
			else {
				gu_printf("Error when creating task");
				while(1);
			}
		}
	}

 

	/* Create a trigger_out for JTAG UART writting */
	/* -1 because it is not associated with hardware interrupt */
	jtag_uart_0_out = gu_trigger_create(-1);
	PRINT_ASSERT((gkm_RCB_IsValid(jtag_uart_0_out) == G_TRUE),"ERROR ECB not valid\n");
	
	/* Create a trigger_in for JTAG UART reading */
	/* -1 because it is not associated with hardware interrupt */
	jtag_uart_0_in = gu_trigger_create(-1);
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
	ptcb = gu_GetTask((void *) GRTOS_IRQ_WRITE_SERVER,
					   (void *) jtag_uart_0_info,
					   "GRTOS_IRQ_WRITE_SERVER");
	if (ptcb != (void *) 0)
	{
		gu_SetTaskType(ptcb, G_TCBType_UCOS);

		// gu_SetTaskList(ptcb, (struct gs_lcb *) pedf_list);
		gu_SetTaskReadyPriority(ptcb, 0);
		gu_SetTaskRunPriority(ptcb, 0);

		gu_SetTaskDeadline(ptcb, 0, 0, 1, 1);
		gu_SetTaskPeriod(ptcb, 0, 0,  1, 1);
		gu_SetTaskAbortwhenDeadline(ptcb, 0);
		/* Start execution of task with an offset */
		gu_StartTaskwithOffset(ptcb, 0, 0, 5, 0);
	}
	/* suscribe task as consumer to the consumer/producer queue */
	gu_queue_subscribe(ptcb, pqueue_out);
	
	/* Set the jtag output server hooks */
	gu_trigger_enable_hook((int) jtag_uart_0_out, (void *) GRTOS_IRQ_WRITE_SERVER_ENABLE, (void *) NULL);
	gu_trigger_disable_hook((int) jtag_uart_0_out, (void *) GRTOS_IRQ_WRITE_SERVER_DISABLE, (void *) NULL);

	// 22-03-2024 ####################################
	/* Creatse task for jtag output server */
	ptcb = gu_GetTask((void *) GRTOS_IRQ_READ_SERVER,
					   (void *) jtag_uart_0_info,
					   "GRTOS_IRQ_WRITE_SERVER");
	if (ptcb != (void *) 0)
	{
		gu_SetTaskType(ptcb, G_TCBType_UCOS);

		// gu_SetTaskList(ptcb, (struct gs_lcb *) pedf_list);
		gu_SetTaskReadyPriority(ptcb, 0);
		gu_SetTaskRunPriority(ptcb, 0);

		gu_SetTaskDeadline(ptcb, 0, 0, 1, 1);
		gu_SetTaskPeriod(ptcb, 0, 0,  1, 1);
		gu_SetTaskAbortwhenDeadline(ptcb, 0);
		/* Start execution of task with an offset */
		gu_StartTaskwithOffset(ptcb, 0, 0, 5, 0);
	}
	/* suscribe task as consumer to the consumer/producer queue */
	// gu_queue_subscribe(ptcb, pqueue_out);
	
	/* Set the jtag output server hooks */
	gu_trigger_enable_hook((int) jtag_uart_0_in, (void *) GRTOS_IRQ_READ_SERVER_ENABLE, (void *) NULL);
	gu_trigger_disable_hook((int) jtag_uart_0_in, (void *) GRTOS_IRQ_READ_SERVER_DISABLE, (void *) NULL);	
	// 22-03-2024 ####################################	

	// #################################
	/* Create a ISR task for jtag uart hardware interrupt */
	ptcb = gu_GetTask((void *) jtag_irq_manager,
					   (void *) 5,
					   "jtag_irq_manager");
	if (ptcb != (void *) 0)
	{
		gu_SetTaskType(ptcb, G_TCBType_UCOS);

		// gu_SetTaskList(ptcb, (struct gs_lcb *) pedf_list);
		
		gu_SetTaskReadyPriority(ptcb, 0);
		gu_SetTaskRunPriority(ptcb, 0);

		gu_SetTaskDeadline(ptcb, 0, 0, 1, 1);
		gu_SetTaskPeriod(ptcb, 0, 0,  1, 1);
		gu_SetTaskAbortwhenDeadline(ptcb, 0);
	}        
	
	/* Associate the task to be triggered with the jtag uart interrupt */
	gu_trigger_register_task(ptcb, (int) JTAG_UART_0_IRQ);
	gu_trigger_enable((int) JTAG_UART_0_IRQ);
	
    return (0);
}

