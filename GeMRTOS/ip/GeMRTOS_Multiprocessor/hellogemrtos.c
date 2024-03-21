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

/* Producer consumer queue for jtag_uart */
G_RCB  *pqueue;

/* Trigger for jtag output server */
G_RCB *jtag_uart_0;


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
		gu_queue_printf(pqueue,"%s by processor %d\n", ptcb->TCB_description, (int) GRTOS_CMD_PRC_ID);
	}
}


/******************************************************************/
/* enable hook for jtag uart write trigger                        */
/******************************************************************/
void *GRTOS_IRQ_WRITE_SERVER_ENABLE(void * pdata)
{
	/* enable jtag-uart write interrupt */
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, 0x00000002);
    return ((void *) 0);
}

/******************************************************************/
/* disable hook for jtag uart write trigger                       */
/******************************************************************/
void *GRTOS_IRQ_WRITE_SERVER_DISABLE(void *pdata)
{
    // Disable interrupt in JTAG UART component
    IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, 0x00000000);
    return ((void *) 0);
}

/* define structure for JTAG information */
struct jtag_uart_info {
	unsigned int trigger;
	G_RCB *pqueue;
	unsigned int buffer_length;
	unsigned int jtag_base;
};


/******************************************************************/
/* JTAG-UART write server                                         */
/* reads from the producer-consumer queue and write into the      */
/* jtag-uart component using write IRQ.                           */
/******************************************************************/
void GRTOS_IRQ_WRITE_SERVER(void *pdata)
{
#ifdef __niosX_arch__
	struct jtag_uart_info * jtag_uart_0_info = (struct jtag_uart_info *) pdata;

	/* Get a buffer to store the messaje to send to the jtag-uart */
    void *mem = malloc(jtag_uart_0_info->buffer_length);
	PRINT_ASSERT(((unsigned int) mem % 4 == 0), "ERROR memory is not alligned");
    char *buffer_msg = (char *) mem;
    
    int size;
    int space_fifo;
    int index_msg;
   
    /* Get the pointer to the TCB of the current task */
    GS_TCB *ptcb = gu_PCB_GetCurrentTCB();
	
    /* Define this task as an ISR associated to trigger jtag_uart_0 */
    gu_trigger_register_task(ptcb, (G_INT32) jtag_uart_0_info->trigger);

    while (1) {
		/* receive next message from producer/consumer queue */
        size = gu_queue_receive(jtag_uart_0_info->pqueue, (void *) buffer_msg, jtag_uart_0_info->buffer_length);

		/* Iterate for each character of the message, but the ending '\0' */
        index_msg = 0;
        while (index_msg < size-1)
        {
			/* Get the available space in jtag uart buffer */
            space_fifo = (((G_INT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(jtag_uart_0_info->jtag_base)) & ((G_INT32) ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_OFST;

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
            
			/* There exist more character to send, so wait to next trigger from jtag device */
            if (index_msg < size-1) {
                gu_trigger_wait();
            }
        }
    };
#endif	
}

/******************************************************************/
/* This function is the ISR for the jtag uart named "jtag_uart_0" */
/******************************************************************/
void jtag_irq_manager(int pdata)
{
    gu_fprintf("\nIRQ=%d\n", (int) pdata);
#ifdef __niosX_arch__
	/* Read the state of interrupt flags from the JTAG UART status register */
    G_INT32 uart_ctrl_register = (G_INT32) IORD_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE);
    
	/* Get the state of the write interrupt (output buffer empty) */
    G_INT32 write_interrupt = (uart_ctrl_register  & ((G_INT32) ALTERA_AVALON_JTAG_UART_CONTROL_WI_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_WI_OFST;
	
    /* Get the state of the read interrupt (input buffer full) */
    G_INT32 read_interrupt = (uart_ctrl_register  & ((G_INT32) ALTERA_AVALON_JTAG_UART_CONTROL_RI_MSK )) >> ALTERA_AVALON_JTAG_UART_CONTROL_RI_OFST;    

	/* If write interrupt is on, the release the output server trigger */
    if (write_interrupt == (G_INT32) 1) gu_trigger_release ((int) jtag_uart_0);
	
	/* Read ISR should be defined for input reading */
#endif
}


int main(void)
{

    /* ################################################### */
    /* main variable definition                            */
	/* ################################################### */
    GS_TCB *ptcb;
	GS_LCB *plcb;
    int i;

    /* Create a semaphore for LEDS */
    leds_sem = gu_sem_create((int) 1);
    
    /* Create a message queue for JTAG-UART jtag_uart_0 */
    pqueue = gu_queue_create();	
	
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


	/* Create a trigger for JTAG UART writting */
	/* -1 because it is not associated with hardware interrupt */
	jtag_uart_0 = gu_trigger_create(-1);
	PRINT_ASSERT((gkm_RCB_IsValid(jtag_uart_0) == G_TRUE),"ERROR ECB not valid\n");
	
	/* create a jtag_uart_info structure to store the information for output server consumer */
	struct jtag_uart_info *jtag_uart_0_info = (struct jtag_uart_info *) malloc(sizeof(struct jtag_uart_info));
	
	/* set the parameter for jtag output server configuration. 
	   The pointer to this structure is pass as reference to the task */
	jtag_uart_0_info->pqueue = pqueue;
	jtag_uart_0_info->trigger = (unsigned int) jtag_uart_0;
	jtag_uart_0_info->buffer_length = 100;
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
	gu_queue_subscribe(ptcb, pqueue);
	
	/* Set the jtag output server hooks */
	gu_trigger_enable_hook((int) jtag_uart_0, (void *) GRTOS_IRQ_WRITE_SERVER_ENABLE, (void *) NULL);
	gu_trigger_disable_hook((int) jtag_uart_0, (void *) GRTOS_IRQ_WRITE_SERVER_DISABLE, (void *) NULL);


	// #################################
	/* Create a ISR task for jtag uart hardware interrupt */
	ptcb = gu_GetTask((void *) jtag_irq_manager,
					   (void *) JTAG_UART_0_IRQ,
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

