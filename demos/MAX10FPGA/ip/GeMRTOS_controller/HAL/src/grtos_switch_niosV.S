/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Switching functions for nios II
 *  \details This file contains the Switching functions for nios II.
 *  \version 0.9a
 *  \date      2015-2020
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

#include <system.h>

#include <grtos_regs.h>


.equ ABSOLUTE_ADDR_RST_CLR, GRTOS_DRIVER_GRTOS_BASE + 4 * ADDR_RST_CLR
.equ ABSOLUTE_ADDR_ERET_REG, GRTOS_DRIVER_GRTOS_BASE + 4 * ADDR_ERET_REG
.equ ABSOLUTE_ADDR_MTX_RLS, GRTOS_DRIVER_GRTOS_BASE + 4 * ADDR_MTX_RLS
.equ ABSOLUTE_ADDR_INT_ENB_SET, GRTOS_DRIVER_GRTOS_BASE + 4 * ADDR_INT_ENB_SET;
.equ ABSOLUTE_GRTOS_CMD_PRC_ID, GRTOS_DRIVER_GRTOS_PROCESSOR_BASE1 + 4 * ADDR_REG_1;
.equ G_STACK_STATUS_LENGTH, 34


.if ALT_CPU_ARCHITECTURE == "intel_niosv_g" || ALT_CPU_ARCHITECTURE == "intel_niosv_m"

	.global main
    .global gk_ENTRY_SIGNAL_RETURN
	.global GRTOS_Start_Task
	.global GRTOS_Suspend_Task
	.global gk_ENTRY_BAD_TASK_RETURN
	.global GRTOS_Return_from_Signal
    .global Switch_to_idle
    .global grtos_start_rest_of_processors
	.global grtos_irq_entry
	.global GRTOS_Change_SP_to_IDLE
	.global GRTOS_jump_to_reset
	.global GRTOS_jump_to_reset_end

// #################################################################################
    /// Multiplication rewritten for no-multipler processor compatibility
    /// rout = rin1 * rin2 : adding rin2 times rin1
.macro multiply rin1, rin2, rout      
    addi \rout, zero, 0   // nios 2 mov \rout, zero
    beq \rin2, zero, 2f   // nios 2 beq \rin2, zero, 2f
1:    
    add \rout, \rout, \rin1  // nios 2
    addi \rin2, \rin2, -1    // nios 2
    bne \rin2, zero, 1b      // nios 2
2:
.endm


// #################################################################################
    /// Get CPU_ID
    /// rout = CPUID
.macro get_cpuid rin1
	la \rin1, ABSOLUTE_GRTOS_CMD_PRC_ID
	lw \rin1, 0(\rin1)
.endm


// #################################################################################    
    // Read the data in the memory address
.macro read_memory_data rout, address
    la  \rout, \address      // nios 2 movia \rout, \address
    lw  \rout,0(\rout)       // nios 2 ldwio \rout,0(\rout)
.endm

// #################################################################################
	// get &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackPointer
	// where offset_address may be PCB_IDLETCB or PCB_EXECTCB
.macro get_tcb_stackpointer rout, rtemp1, rtemp2, offset_address
	/* \rtemp2 = [gm_ProcessorId -1] */
    get_cpuid \rtemp2              // nios 2 rdctl \rtemp2,cpuid
    addi \rtemp2,\rtemp2,-1     

	/* \rtemp1 = sizeof(G_PCBTbl * [gm_ProcessorId -1]) */
    read_memory_data \rtemp1, gs_sizeof_G_PCBTbl
    multiply \rtemp1, \rtemp2, \rout     // nios 2
	
	/* \rout = &g_kcb.G_PCBTbl[gm_ProcessorId -1] */
    read_memory_data \rtemp1, gs_addressof_G_PCBTbl
    add	\rout,\rout,\rtemp1    // nios 2  
	
	/* \rout = &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB) */
    read_memory_data \rtemp1, \offset_address
    add	\rout,\rout,\rtemp1    // nios 2 
	
	/* \rout = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB == &TCB of task*/
    lw \rout,0(\rout)    // nios 2  ldwio	\rout,0(\rout)  
	
	/* \rout = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB -> TCB_StackPointer) */
    read_memory_data \rtemp1, gs_offsetof_TCB_StackPointer
    add	\rout,\rout,\rtemp1   
.endm



/********************************************************************************/    
    // .section rstaux, "xa"
	.section .text, "xa"
	.align 4
    .type grtos_start_rest_of_processors, @function	
	// .org 0

grtos_start_rest_of_processors:

	// Copied from crt0.S ###############################
    /* assume that we are in machine mode on startup */
    /* setup trap to catch anything that goes wrong early in the boot process */
	la t0, grtos_irq_entry
    csrw mtvec, t0

    /* disable all interrupt */
    li t0, 8
    csrc mstatus, t0
    csrw mie, zero
    csrw mip, zero

    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /*
     * Now that the caches are initialized, set up the stack pointer and global pointer.
     * The values provided by the linker are assumed to be correctly aligned.
     */
    la sp, __alt_stack_pointer
	// end of copy from crt0.S #####################################


    // Read the reset until the previous processor enables it
    la t2, ABSOLUTE_ADDR_RST_CLR
	get_cpuid t3
    addi t3, t3, -1    
start1:
    lw	t4,0(t2)
    bne t3, t4, start1

	la t0, gk_START_KERNEL
    jalr ra, t0, 0

/********************************************************************************/

/// Interruption in http://www-ug.eecg.toronto.edu/msl/nios_interrupts.html

/// THIS IS CALL FROM gk_KERNEL_TASK_SUSPEND_CURRENT WITH G_TCB_CURRENT
	.section .text, "xa"
	.align 4
	.type GRTOS_Suspend_Task, @function
GRTOS_Suspend_Task:
    // This is executed in critical section
	// Update the new stack pointer
	addi sp, sp, -(G_STACK_STATUS_LENGTH * 4)

	/// Save the registers of the current task.
    /// https://courses.cs.washington.edu/courses/cse467/08au/labs/Tutorials/tut_nios2_introduction.pdf pp 18
	/* current ra is saved as mepc because it does not comes from interrupt
	   but it returns from one with mret                                      */
	/* ra is saved as return to gk_ENTRY_BAD_TASK_RETURN bellow               */
	
	sw fp, (4 * OFFSET_REG_3)(sp)  /* register r28 fp */
	sw tp, (4 * OFFSET_REG_4)(sp)  /* register r15    */
	sw t2, (4 * OFFSET_REG_5)(sp)  /* register r14    */
	sw t1, (4 * OFFSET_REG_6)(sp)  /* register r13    */
	sw t0, (4 * OFFSET_REG_7)(sp)  /* register r12    */
	sw a7, (4 * OFFSET_REG_8)(sp)  /* register r11    */
	sw a6, (4 * OFFSET_REG_9)(sp)  /* register r10    */
	sw a5, (4 * OFFSET_REG_10)(sp)   /* register r9     */
	sw a4, (4 * OFFSET_REG_11)(sp)   /* register r8     */
	sw a3, (4 * OFFSET_REG_12)(sp)   /* register r7     */
	sw a2, (4 * OFFSET_REG_13)(sp)   /* register r6     */
	sw a1, (4 * OFFSET_REG_14)(sp)   /* register r5     */
	sw a0, (4 * OFFSET_REG_15)(sp)   /* register r4     */
	sw s11, (4 * OFFSET_REG_16)(sp)   /* register r3     */
	sw s10, (4 * OFFSET_REG_17)(sp)   /* register r2     */
	sw s9, (4 * OFFSET_REG_18)(sp)   /* register r1     */

	sw s8, (4 * OFFSET_REG_19)(sp)  /* register r24    */
	sw s7, (4 * OFFSET_REG_20)(sp)  /* register r23    */
	sw s6, (4 * OFFSET_REG_21)(sp)  /* register r22    */
	sw s5, (4 * OFFSET_REG_22)(sp)  /* register r21    */
	sw s4, (4 * OFFSET_REG_23)(sp)  /* register r20    */
	sw s3, (4 * OFFSET_REG_24)(sp)  /* register r19    */
	sw s2, (4 * OFFSET_REG_25)(sp) /* register r18    */
	sw s1, (4 * OFFSET_REG_26)(sp) /* register r17    */
	sw s0, (4 * OFFSET_REG_27)(sp) /* register r16    */
	sw t6, (4 * OFFSET_REG_28)(sp) /* register r16    */
	sw t5, (4 * OFFSET_REG_29)(sp) /* register r16    */
	sw t4, (4 * OFFSET_REG_30)(sp) /* register r16    */
	sw t3, (4 * OFFSET_REG_31)(sp) /* register r16    */

	
	// addi  t0, ra, 0   /* ra as exception plus 4 */
	sw ra, (4 * OFFSET_REG_2)(sp)   /* register r29 ea */ /* ra as exception return */

	la t0, gk_ENTRY_BAD_TASK_RETURN
	sw t0, (4 * OFFSET_REG_1)(sp)   /* register r31 ra */

    /// Save the current stack pointer in TCB structure
    read_memory_data t0, G_TCB_CURRENT
    sw sp, 0(t0)
	

    /// ********************************
    /// * Switch to idle task stack
    /// ******************************** 
    /// sp = &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackPointer
    get_tcb_stackpointer t0, t1, t2, gs_offsetof_PCB_IDLETCB
       
    /// Restore the stack pointer of IDLD TCB structure
    lw sp, 0(t0)

    // Call the Interrupt Handler Routine
	la t0, gk_IRQ_HANDLER
    jalr t0, t0, 0


	.section .text, "xa"
	.align 4
	.type grtos_irq_entry, @function
/********************************************************************************/
/****   Entry point of NIOS II Interrupt Handler    *****************************/
/** https://forums.intel.com/s/createarticlepage?language=en_US&articleid=a3g0P0000005RdqQAE&artTopicId=0TO0P000000MWKBWA4&action=view **/
/********************************************************************************/
grtos_irq_entry:
/********************************************************************************/
	// Update the new stack pointer
	csrw mstatus, zero
	
	addi sp, sp, -(G_STACK_STATUS_LENGTH * 4)

	/// Save the registers of the current task. ea is already incremented in 4
	sw ra, (4 * OFFSET_REG_1)(sp)   /* register r31 ra */

	sw fp, (4 * OFFSET_REG_3)(sp)  /* register r28 fp */
	sw tp, (4 * OFFSET_REG_4)(sp)  /* register r15    */
	sw t2, (4 * OFFSET_REG_5)(sp)  /* register r14    */
	sw t1, (4 * OFFSET_REG_6)(sp)  /* register r13    */
	sw t0, (4 * OFFSET_REG_7)(sp)  /* register r12    */
	sw a7, (4 * OFFSET_REG_8)(sp)  /* register r11    */
	sw a6, (4 * OFFSET_REG_9)(sp)  /* register r10    */
	sw a5, (4 * OFFSET_REG_10)(sp)   /* register r9     */
	sw a4, (4 * OFFSET_REG_11)(sp)   /* register r8     */
	sw a3, (4 * OFFSET_REG_12)(sp)   /* register r7     */
	sw a2, (4 * OFFSET_REG_13)(sp)   /* register r6     */
	sw a1, (4 * OFFSET_REG_14)(sp)   /* register r5     */
	sw a0, (4 * OFFSET_REG_15)(sp)   /* register r4     */
	sw s11, (4 * OFFSET_REG_16)(sp)   /* register r3     */
	sw s10, (4 * OFFSET_REG_17)(sp)   /* register r2     */
	sw s9, (4 * OFFSET_REG_18)(sp)   /* register r1     */

	sw s8, (4 * OFFSET_REG_19)(sp)  /* register r24    */
	sw s7, (4 * OFFSET_REG_20)(sp)  /* register r23    */
	sw s6, (4 * OFFSET_REG_21)(sp)  /* register r22    */
	sw s5, (4 * OFFSET_REG_22)(sp)  /* register r21    */
	sw s4, (4 * OFFSET_REG_23)(sp)  /* register r20    */
	sw s3, (4 * OFFSET_REG_24)(sp)  /* register r19    */
	sw s2, (4 * OFFSET_REG_25)(sp) /* register r18    */
	sw s1, (4 * OFFSET_REG_26)(sp) /* register r17    */
	sw s0, (4 * OFFSET_REG_27)(sp) /* register r16    */
	sw t6, (4 * OFFSET_REG_28)(sp) /* register r16    */
	sw t5, (4 * OFFSET_REG_29)(sp) /* register r16    */
	sw t4, (4 * OFFSET_REG_30)(sp) /* register r16    */
	sw t3, (4 * OFFSET_REG_31)(sp) /* register r16    */	
    
	csrr t0, mepc
	sw t0, (4 * OFFSET_REG_2)(sp)   /* register r29 ea */


.Lgrtos:
	/// Get the adequate stackpointer of the current task
    /// ********************************
    /// (GS_STK) &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_StackPointer = sp
    get_tcb_stackpointer t0, t1, t2, gs_offsetof_PCB_EXECTCB

    /// Save the current stack pointer in TCB structure
    sw sp, (t0)

Switch_to_idle:

    /// ********************************
    /// * Switch to idle task stack
    /// ******************************** 
    /// sp = &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackPointer
    get_tcb_stackpointer t0, t1, t2, gs_offsetof_PCB_IDLETCB
    /// Restore the stack pointer of IDLD TCB structure
    lw sp, (t0)
    
    // Call the Interrupt Handler Routine
	la t0, gk_ENTRY_IRQ_HANDLER
    jalr t0, t0, 0

	.section .text, "xa"
	.align 4
	.type GRTOS_Start_Task, @function
/********************************************************************************/
/****   Function to start executing a task          *****************************/
/********************************************************************************/
GRTOS_Start_Task:
	// Restore the Status from the stack as it was saved 
    // sp = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_StackPointer;
	
    get_tcb_stackpointer t0, t1, t2, gs_offsetof_PCB_EXECTCB
	
    // Restore the sp register
    lw sp, 0 (t0)    

    // Jump to Skip_Mutex to avoid get mutex again
	la t0, GRTOS_Skip_Mutex
	jalr t0, t0, 0


/********************************************************************************/
/****   Return point from signal execution          *****************************/
/********************************************************************************/
	.section .text, "xa"
	.align 4
	.type GRTOS_Return_from_Signal, @function
GRTOS_Return_from_Signal:

    /// Get mutex and return with mutex granted
	la ra, gk_ENTRY_SIGNAL_RETURN
    jalr ra, ra, 0

    // Jump to Skip_Mutex
	la t0, GRTOS_Skip_Mutex
	jalr t0, t0, 0

	/* Finish execution of Pending SCB */
	.section .text, "xa"
	.align 4
	.type GRTOS_Skip_Mutex, @function
GRTOS_Skip_Mutex:

	// Call the gk_TCB_GetPendingSCB function to execute the pending signals in task mode
	// This function modifies the stack if a signal has to be executed
    // 19-12-2023 movia r4, gk_TCB_GetPendingSCB
	// 19-12-2023 callr r4
	
	la t0, gk_TCB_GetPendingSCB
    jalr ra, t0, 0

	// Return [G_SCB_PENDING] = 1 if SCB pending, 0 otherwise
    //		  [G_SCB_CODE]    = SCB.SCB_TaskCode + 4
	//		  [G_SCB_ARG]     = SCB.SCB_TaskArg
	//		  Return_address  = GRTOS_Return_from_Signal

    // Check if there is a pending SCB
    read_memory_data t0, G_SCB_PENDING
	beq   t0, zero, L_SCBNoPending       // nios 2 beq r4, zero, L_SCBNoPending

	
/////Create a task status to execute signal/////////////////

	// Update the new stack pointer
	addi sp, sp, -(G_STACK_STATUS_LENGTH * 4)

	/// Save the registers of the current task. 
	la t0, GRTOS_Return_from_Signal
	sw t0, (4 * OFFSET_REG_1)(sp)      /* register r31 ra */

    read_memory_data t0, G_SCB_CODE
	sw t0, (4 * OFFSET_REG_2)(sp)      /* register r29 ea */

	// addi  t0, sp, -1
	sw fp, (4 * OFFSET_REG_3)(sp)  /* register r28 fp */

    read_memory_data t0, G_SCB_ARG
	sw t0, (4 * OFFSET_REG_15)(sp)   /* register r4     */
    
	sw t0, (4 * OFFSET_REG_17)(sp)   /* register r2     */


///////////////////////////////////////////////////////

L_SCBNoPending:
	/// Restore the saved registers for the new task.

	li t2, 0x80      // field mstatus.MPIE (bit 7) with value 1
	csrw mstatus, t2
	
	// Enable external interrupt and IRQ 1 
	li t1 , 0x00010800  // set the MEIE field (bits 16 and 11)
	csrw mie, t1        // update mie register	

	lw ra, (4 * OFFSET_REG_1)(sp)       /* register r31 ra */
	
	lw t0, (4 * OFFSET_REG_2)(sp)   /* register r29 ea */
	csrw  mepc, t0

	lw fp, (4 * OFFSET_REG_3)(sp)   /* register r28 fp */
	lw tp, (4 * OFFSET_REG_4)(sp)   /* register r14    */

	lw a7, (4 * OFFSET_REG_8)(sp)   /* register r10    */
	lw a6, (4 * OFFSET_REG_9)(sp)   /* register r9     */  
	lw a5, (4 * OFFSET_REG_10)(sp)   /* register r8     */  
	lw a4, (4 * OFFSET_REG_11)(sp)   /* register r7     */ 
	lw a3, (4 * OFFSET_REG_12)(sp)   /* register r6     */
	lw a2, (4 * OFFSET_REG_13)(sp)   /* register r5     */
	lw a1, (4 * OFFSET_REG_14)(sp)   /* register r4     */
	lw a0, (4 * OFFSET_REG_15)(sp)   /* register r3     */
	lw s11, (4 * OFFSET_REG_16)(sp)   /* register r2     */
	lw s10, (4 * OFFSET_REG_17)(sp)   /* register r2     */
	lw s9, (4 * OFFSET_REG_18)(sp)   /* register r2     */
	lw s8, (4 * OFFSET_REG_19)(sp)  /* register r24    */
	lw s7, (4 * OFFSET_REG_20)(sp)  /* register r23    */
	lw s6, (4 * OFFSET_REG_21)(sp)  /* register r22    */
	lw s5, (4 * OFFSET_REG_22)(sp)  /* register r21    */
	lw s4, (4 * OFFSET_REG_23)(sp)  /* register r20    */
	lw s3, (4 * OFFSET_REG_24)(sp)  /* register r19    */
	lw s2, (4 * OFFSET_REG_25)(sp)  /* register r18    */
	lw s1, (4 * OFFSET_REG_26)(sp)  /* register r17    */
	lw s0, (4 * OFFSET_REG_27)(sp)  /* register r16    */
	lw t6, (4 * OFFSET_REG_28)(sp) /* register r16    */
	lw t5, (4 * OFFSET_REG_29)(sp) /* register r16    */
	lw t4, (4 * OFFSET_REG_30)(sp) /* register r16    */
	lw t3, (4 * OFFSET_REG_31)(sp) /* register r16    */
	
	lw t2, (4 * OFFSET_REG_5)(sp)   /* register r13    */
	
	// rdctl r8, cpuid
	// Enable interrupts
	get_cpuid t0
    la t1, ABSOLUTE_ADDR_INT_ENB_SET
	sw t0, 0(t1)
	
    la t1, ABSOLUTE_ADDR_MTX_RLS
	sw t0, 0(t1)

	lw t1, (4 * OFFSET_REG_6)(sp)   /* register r12    */
	lw t0, (4 * OFFSET_REG_7)(sp)   /* register r11    */	

	// Update the SP register
	addi sp, sp, (G_STACK_STATUS_LENGTH * 4)
    
	/// Return enabling nios interrupt
    // 26-09-2023 jmpi ABSOLUTE_ADDR_ERET_REG
	mret
	

	.section .text, "xa"
	.align 4
	.type GRTOS_Change_SP_to_IDLE, @function
GRTOS_Change_SP_to_IDLE:
	get_tcb_stackpointer t0, t1, t2, gs_offsetof_PCB_IDLETCB
	lw sp, (t0)
	la t0, gk_KERNEL_TASK_COMPLETE
	jalr t0, t0, 0


	.section .text, "xa"
	.align 4
	.type GRTOS_jump_to_reset, @function
GRTOS_jump_to_reset:
	// it has to be absolute jump because it is relocated
	lui t0, %hi(grtos_start_rest_of_processors)
	addi t0, t0, %lo(grtos_start_rest_of_processors)
    jalr t0, t0, 0

GRTOS_jump_to_reset_end:




.endif
