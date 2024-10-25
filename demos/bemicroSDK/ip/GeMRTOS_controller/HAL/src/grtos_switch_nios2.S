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

.if ALT_CPU_ARCHITECTURE == "altera_nios2_gen2"

    .global gk_ENTRY_SIGNAL_RETURN
	.global GRTOS_Start_Task
	.global GRTOS_Suspend_Task
	.global gk_ENTRY_BAD_TASK_RETURN
	.global GRTOS_Return_from_Signal
    .global Switch_to_idle
    .global grtos_start_rest_of_processors
	.global GRTOS_Change_SP_to_IDLE
	.global GRTOS_jump_to_reset
	.global GRTOS_jump_to_reset_end

// #################################################################################
    /// Get CPU_ID
    /// rout = CPUID
.macro get_cpuid rin1
	movia \rin1, ABSOLUTE_GRTOS_CMD_PRC_ID
	ldwio \rin1, 0(\rin1)
.endm

// #################################################################################
    /// Multiplication rewritten for no-multipler processor compatibility
    /// rout = rin1 * rin2 : adding rin2 times rin1
.macro multiply rin1, rin2, rout      
    mov \rout, zero
    beq \rin2, zero, 2f
1:    
    add \rout, \rout, \rin1
    addi \rin2, \rin2, -1
    bne \rin2, zero, 1b
2:
.endm

// #################################################################################    
    // Read the data in the memory address
.macro read_memory_data rout, address
    movia  \rout, \address
    ldwio	\rout,0(\rout)
.endm

// #################################################################################
	// get &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackPointer
	// where offset_address may be PCB_IDLETCB or PCB_EXECTCB
.macro get_tcb_stackpointer rout, rtemp1, rtemp2, offset_address
    // rdctl	\rtemp2,cpuid
	get_cpuid \rtemp2
    addi	\rtemp2,\rtemp2,-1     /* \rtemp2 = [gm_ProcessorId -1] */

    read_memory_data \rtemp1, gs_sizeof_G_PCBTbl
    multiply \rtemp1, \rtemp2, \rout  /* \rtemp1 = sizeof(G_PCBTbl * [gm_ProcessorId -1]) */
	
    read_memory_data \rtemp1, gs_addressof_G_PCBTbl
    add	\rout,\rout,\rtemp1         /* \rout = &g_kcb.G_PCBTbl[gm_ProcessorId -1] */
	
    read_memory_data \rtemp1, \offset_address
    add	\rout,\rout,\rtemp1  /* \rout = &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB) */
	
    ldwio	\rout,0(\rout)  /* \rout = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB == &TCB of task*/
	
    read_memory_data \rtemp1, gs_offsetof_TCB_StackPointer
    add	\rout,\rout,\rtemp1   /* \rout = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB -> TCB_StackPointer) */
.endm



/********************************************************************************/    

    // .section rstaux, "xa"
	.section .text, "xa"
    /// .set noat
	.org 0
grtos_start_rest_of_processors:

	// 20-09-2023 #######################
	

	mov r4, r0
	movhi r5, %hi(NIOS2_ICACHE_SIZE)
	ori r5, r5, %lo(NIOS2_ICACHE_SIZE)


1:
	initi r4
	addi r4, r4, NIOS2_ICACHE_LINE_SIZE
	bltu r4, r5, 1b
	// 20-09-2023 #######################

    /// Set the Stack Pointer for each Processor with cpuid greater than 1
	movhi sp, %hi(__alt_stack_pointer) 
	ori sp, sp, %lo(__alt_stack_pointer) 

    /* Set up the global pointer. It is required for gcc compilation */
    /// Otherwise, _gp may give an exception
	movhi gp, %hi(_gp) 
	ori gp, gp, %lo(_gp)

// start0:
//     read_memory_data r4, G_Running
//     addi r2, zero, 1
// 	bne   r4, r2, start0

    // Read the reset until the previous processor enables it
    movhi r2, %hi(ABSOLUTE_ADDR_RST_CLR)
    ori r2, r2, %lo(ABSOLUTE_ADDR_RST_CLR)
    // rdctl r3, cpuid
	get_cpuid r3
    addi r3, r3, -1    
start1:
    ldwio	r4,0(r2)
    bne r3, r4, start1

    movia r4, gk_START_KERNEL
	callr r4

    
/********************************************************************************/

/// Interruption in http://www-ug.eecg.toronto.edu/msl/nios_interrupts.html

/// THIS IS CALL FROM gk_KERNEL_TASK_SUSPEND_CURRENT WITH G_TCB_CURRENT
	.section .text, "xa"
    .set noat
	.type GRTOS_Suspend_Task, @function
GRTOS_Suspend_Task:
    // This is executed in critical section
	// Update the new stack pointer
	addi sp, sp, -112

	/// Save the registers of the current task.
    /// https://courses.cs.washington.edu/courses/cse467/08au/labs/Tutorials/tut_nios2_introduction.pdf pp 18
	/* ra is saved as return to gk_ENTRY_BAD_TASK_RETURN bellow */
	addi  r31, r31, 4   /* ra as exception plus 4 */
	stwio r31, (4 * OFFSET_REG_2)(sp)   /* register r29 ea */ /* ra as exception return */
	stwio r28, (4 * OFFSET_REG_3)(sp)  /* register r28 fp */
	stwio r15, (4 * OFFSET_REG_4)(sp)  /* register r15    */
	stwio r14, (4 * OFFSET_REG_5)(sp)  /* register r14    */
	stwio r13, (4 * OFFSET_REG_6)(sp)  /* register r13    */
	stwio r12, (4 * OFFSET_REG_7)(sp)  /* register r12    */
	stwio r11, (4 * OFFSET_REG_8)(sp)  /* register r11    */
	stwio r10, (4 * OFFSET_REG_9)(sp)  /* register r10    */
	stwio r9, (4 * OFFSET_REG_10)(sp)   /* register r9     */
	stwio r8, (4 * OFFSET_REG_11)(sp)   /* register r8     */
	stwio r7, (4 * OFFSET_REG_12)(sp)   /* register r7     */
	stwio r6, (4 * OFFSET_REG_13)(sp)   /* register r6     */
	stwio r5, (4 * OFFSET_REG_14)(sp)   /* register r5     */
	stwio r4, (4 * OFFSET_REG_15)(sp)   /* register r4     */
	stwio r3, (4 * OFFSET_REG_16)(sp)   /* register r3     */
	stwio r2, (4 * OFFSET_REG_17)(sp)   /* register r2     */
	stwio r1, (4 * OFFSET_REG_18)(sp)   /* register r1     */

	stwio r24, (4 * OFFSET_REG_19)(sp)  /* register r24    */
	stwio r23, (4 * OFFSET_REG_20)(sp)  /* register r23    */
	stwio r22, (4 * OFFSET_REG_21)(sp)  /* register r22    */
	stwio r21, (4 * OFFSET_REG_22)(sp)  /* register r21    */
	stwio r20, (4 * OFFSET_REG_23)(sp)  /* register r20    */
	stwio r19, (4 * OFFSET_REG_24)(sp)  /* register r19    */
	stwio r18, (4 * OFFSET_REG_25)(sp) /* register r18    */
	stwio r17, (4 * OFFSET_REG_26)(sp) /* register r17    */
	stwio r16, (4 * OFFSET_REG_27)(sp) /* register r16    */

	movia r5, gk_ENTRY_BAD_TASK_RETURN
	stwio r5, (4 * OFFSET_REG_1)(sp)   /* register r31 ra */

    /// Read status to save as estatus
    rdctl r5, status  /* estatus         */
	stwio r5, (4 * OFFSET_REG_0)(sp)

    /// Save the current stack pointer in TCB structure
    read_memory_data r4, G_TCB_CURRENT
    stwio sp, (r4)


    /// ********************************
    /// * Switch to idle task stack
    /// ******************************** 
    /// sp = &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackPointer
    get_tcb_stackpointer r4, r5, r6, gs_offsetof_PCB_IDLETCB
       
    /// Restore the stack pointer of IDLD TCB structure
    ldwio sp, (r4)

    // Call the Interrupt Handler Routine
    movia r4, gk_IRQ_HANDLER
	callr r4
	
    // call gk_IRQ_HANDLER

    .globl grtos_irq_entry
    .section .irq, "xa"
/********************************************************************************/
/****   Entry point of NIOS II Interrupt Handler    *****************************/
/** https://forums.intel.com/s/createarticlepage?language=en_US&articleid=a3g0P0000005RdqQAE&artTopicId=0TO0P000000MWKBWA4&action=view **/
/********************************************************************************/
grtos_irq_entry:
/********************************************************************************/
	sync
	// Update the new stack pointer
	addi sp, sp, -112

	/// Save the registers of the current task. ea is already incremented in 4
	stwio r31, (4 * OFFSET_REG_1)(sp)   /* register r31 ra */
	stwio r29, (4 * OFFSET_REG_2)(sp)   /* register r29 ea */
	stwio r28, (4 * OFFSET_REG_3)(sp)  /* register r28 fp */
	stwio r15, (4 * OFFSET_REG_4)(sp)  /* register r15    */
	stwio r14, (4 * OFFSET_REG_5)(sp)  /* register r14    */
	stwio r13, (4 * OFFSET_REG_6)(sp)  /* register r13    */
	stwio r12, (4 * OFFSET_REG_7)(sp)  /* register r12    */
	stwio r11, (4 * OFFSET_REG_8)(sp)  /* register r11    */
	stwio r10, (4 * OFFSET_REG_9)(sp)  /* register r10    */
	stwio r9, (4 * OFFSET_REG_10)(sp)   /* register r9     */
	stwio r8, (4 * OFFSET_REG_11)(sp)   /* register r8     */
	stwio r7, (4 * OFFSET_REG_12)(sp)   /* register r7     */
	stwio r6, (4 * OFFSET_REG_13)(sp)   /* register r6     */
	stwio r5, (4 * OFFSET_REG_14)(sp)   /* register r5     */
	stwio r4, (4 * OFFSET_REG_15)(sp)   /* register r4     */
	stwio r3, (4 * OFFSET_REG_16)(sp)   /* register r3     */
	stwio r2, (4 * OFFSET_REG_17)(sp)   /* register r2     */
	stwio r1, (4 * OFFSET_REG_18)(sp)   /* register r1     */

	stwio r24, (4 * OFFSET_REG_19)(sp)  /* register r24    */
	stwio r23, (4 * OFFSET_REG_20)(sp)  /* register r23    */
	stwio r22, (4 * OFFSET_REG_21)(sp)  /* register r22    */
	stwio r21, (4 * OFFSET_REG_22)(sp)  /* register r21    */
	stwio r20, (4 * OFFSET_REG_23)(sp)  /* register r20    */
	stwio r19, (4 * OFFSET_REG_24)(sp)  /* register r19    */
	stwio r18, (4 * OFFSET_REG_25)(sp) /* register r18    */
	stwio r17, (4 * OFFSET_REG_26)(sp) /* register r17    */
	stwio r16, (4 * OFFSET_REG_27)(sp) /* register r16    */
    
    // jmpi .Lgrtos
    
    /// Read estatus to know if it is exception
    rdctl r5, estatus  /* estatus         */
	stwio r5, (4 * OFFSET_REG_0)(sp)


    /// Test if it is a exception
    /// .lrestore executes the original exception routine
    rdctl r4, ipending
    andi  r2, r5 , 1
    beq   r2, zero, .Lrestore
    beq   r4, zero, .Lrestore

.Lgrtosrun:
    read_memory_data r4, G_Running
	bne   r4, zero, .Lgrtos
	rdctl r4, ipending
	br .Lrestore

.Lgrtos:
	/// Get the adequate stackpointer of the current task
    /// ********************************
    /// (GS_STK) &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_StackPointer = sp
    get_tcb_stackpointer r4, r5, r6, gs_offsetof_PCB_EXECTCB

    /// Save the current stack pointer in TCB structure
    stwio sp, (r4)

Switch_to_idle:

    /// ********************************
    /// * Switch to idle task stack
    /// ******************************** 
    /// sp = &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackPointer
    get_tcb_stackpointer r4, r5, r6, gs_offsetof_PCB_IDLETCB

    /// Restore the stack pointer of IDLD TCB structure
    ldwio sp, (r4)
    
    // Call the Interrupt Handler Routine
    movia r4, gk_ENTRY_IRQ_HANDLER
	callr r4	
    // call gk_ENTRY_IRQ_HANDLER


/********************************************************************************/
/****   Function to start executing a task          *****************************/
/********************************************************************************/
GRTOS_Start_Task:
    // g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB = (struct gs_tcb *) ptcb;
	// Restore the Status from the stack as it was saved 
    // sp = g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_EXECTCB->TCB_StackPointer;
    get_tcb_stackpointer r4, r5, r6, gs_offsetof_PCB_EXECTCB
    
    // Restore the sp register
    ldwio sp, (r4)    
    
    // Jump to Skip_Mutex to avoid get mutex again
	jmpi GRTOS_Skip_Mutex

/********************************************************************************/
/****   Return point from signal execution          *****************************/
/********************************************************************************/
GRTOS_Return_from_Signal:


    /// Get muetx and return with mutex granted
    movia r4, gk_ENTRY_SIGNAL_RETURN
	callr r4	
    // call gk_ENTRY_SIGNAL_RETURN

	/* Finish execution of Pending SCB */

GRTOS_Skip_Mutex:

	// Call the gk_TCB_GetPendingSCB function to execute the pending signals in task mode
	// This function modifies the stack if a signal has to be executed
    movia r4, gk_TCB_GetPendingSCB
	callr r4
	// call gk_TCB_GetPendingSCB

	// Return [G_SCB_PENDING] = 1 if SCB pending, 0 otherwise
    //		  [G_SCB_CODE]    = SCB.SCB_TaskCode + 4
	//		  [G_SCB_ARG]     = SCB.SCB_TaskArg
	//		  Return_address  = GRTOS_Return_from_Signal

    // Check if there is a pending SCB
    read_memory_data r4, G_SCB_PENDING
	beq   r4, zero, L_SCBNoPending

/////Create a task status to execute signal/////////////////

	// Update the new stack pointer
	addi sp, sp, -112

	/// Save the registers of the current task.
	movia r4, GRTOS_Return_from_Signal
	stwio r4, (4 * OFFSET_REG_1)(sp)   /* register r31 ra */

    read_memory_data r4, G_SCB_CODE
	stwio r4, (4 * OFFSET_REG_2)(sp)   /* register r29 ea */

	subi  r4, sp, 1
	stwio r4, (4 * OFFSET_REG_3)(sp)  /* register r28 fp */

    read_memory_data r4, G_SCB_ARG
	stwio r4, (4 * OFFSET_REG_15)(sp)   /* register r4     */

	stwio r4, (4 * OFFSET_REG_17)(sp)   /* register r2     */

///////////////////////////////////////////////////////

L_SCBNoPending:
	/// Restore the saved registers for the new task.
    addi r4, zero, 1    
    wrctl estatus, r4
    wrctl status, zero

	// 20-09-2023 #######################
	mov r4, r0
	movhi r5, %hi(NIOS2_ICACHE_SIZE)
	ori r5, r5, %lo(NIOS2_ICACHE_SIZE)
1:
	initi r4
	addi r4, r4, NIOS2_ICACHE_LINE_SIZE
	bltu r4, r5, 1b
	// 20-09-2023 #######################

	ldwio r31, (4 * OFFSET_REG_1)(sp)   /* register r31 ra */
	ldwio r29, (4 * OFFSET_REG_2)(sp)   /* register r29 ea */
	ldwio r28, (4 * OFFSET_REG_3)(sp)   /* register r28 fp */
	ldwio r15, (4 * OFFSET_REG_4)(sp)   /* register r14    */
	ldwio r14, (4 * OFFSET_REG_5)(sp)   /* register r13    */
	ldwio r13, (4 * OFFSET_REG_6)(sp)   /* register r12    */
	ldwio r12, (4 * OFFSET_REG_7)(sp)   /* register r11    */
	ldwio r11, (4 * OFFSET_REG_8)(sp)   /* register r10    */
	ldwio r10, (4 * OFFSET_REG_9)(sp)   /* register r9     */  
	// ldwio r9, (4 * OFFSET_REG_10)(sp)   /* register r8     */  /* USED TO RELEASE MUTEX     */
	// ldwio r8, (4 * OFFSET_REG_11)(sp)   /* register r7     */ /* RESTORE BEFORE ERET       */
	ldwio r7, (4 * OFFSET_REG_12)(sp)   /* register r6     */
	ldwio r6, (4 * OFFSET_REG_13)(sp)   /* register r5     */
	ldwio r5, (4 * OFFSET_REG_14)(sp)   /* register r4     */
	ldwio r4, (4 * OFFSET_REG_15)(sp)   /* register r3     */
	ldwio r3, (4 * OFFSET_REG_16)(sp)   /* register r2     */
	ldwio r2, (4 * OFFSET_REG_17)(sp)   /* register r2     */
	ldwio r1, (4 * OFFSET_REG_18)(sp)   /* register r2     */
	ldwio r24, (4 * OFFSET_REG_19)(sp)  /* register r24    */
	ldwio r23, (4 * OFFSET_REG_20)(sp)  /* register r23    */
	ldwio r22, (4 * OFFSET_REG_21)(sp)  /* register r22    */
	ldwio r21, (4 * OFFSET_REG_22)(sp)  /* register r21    */
	ldwio r20, (4 * OFFSET_REG_23)(sp)  /* register r20    */
	ldwio r19, (4 * OFFSET_REG_24)(sp)  /* register r19    */
	ldwio r18, (4 * OFFSET_REG_25)(sp)  /* register r18    */
	ldwio r17, (4 * OFFSET_REG_26)(sp)  /* register r17    */
	ldwio r16, (4 * OFFSET_REG_27)(sp)  /* register r16    */

	// rdctl r8, cpuid
	get_cpuid r8
	
    movhi r9, %hi(ABSOLUTE_ADDR_INT_ENB_SET)
    ori r9, r9, %lo(ABSOLUTE_ADDR_INT_ENB_SET)
	stwio r8, 0(r9)
	
    movhi r9, %hi(ABSOLUTE_ADDR_MTX_RLS)
    ori r9, r9, %lo(ABSOLUTE_ADDR_MTX_RLS)
	stwio r8, 0(r9)


	addi  ea, ea, -4  /* instruction that caused exception */

	ldwio r9, (4 * OFFSET_REG_10)(sp)   /* register r9     */
	ldwio r8, (4 * OFFSET_REG_11)(sp)   /* register r8     */

	// Update the SP register
	addi sp, sp, 112
    
	sync
	/// Return enabling nios interrupt
    // 26-09-2023 jmpi ABSOLUTE_ADDR_ERET_REG
	eret

// *********************************************
// .section .irq, "xa"
//    .org 1300
//    jmpi grtos_start_rest_of_processors

.Lrestore:
    movia r4, gk_restore
	callr r4
    // call gk_restore
	/// Restore the saved registers for the new task.
	ldwio r4,  (4 * OFFSET_REG_0)(sp)   /* estatus         */
    wrctl estatus, r4

	ldwio r31, (4 * OFFSET_REG_1)(sp)   /* register r31 ra */
	ldwio r29, (4 * OFFSET_REG_2)(sp)   /* register r29 ea */
	ldwio r28, (4 * OFFSET_REG_3)(sp)  /* register r15 fp */
	ldwio r15, (4 * OFFSET_REG_4)(sp)  /* register r14    */
	ldwio r14, (4 * OFFSET_REG_5)(sp)  /* register r13    */
	ldwio r13, (4 * OFFSET_REG_6)(sp)  /* register r12    */
	ldwio r12, (4 * OFFSET_REG_7)(sp)  /* register r11    */
	ldwio r11, (4 * OFFSET_REG_8)(sp)  /* register r10    */
	ldwio r10, (4 * OFFSET_REG_9)(sp)  /* register r9     */
	ldwio r9, (4 * OFFSET_REG_10)(sp)   /* register r8     */
	ldwio r8, (4 * OFFSET_REG_11)(sp)   /* register r7     */
	ldwio r7, (4 * OFFSET_REG_12)(sp)   /* register r6     */
	ldwio r6, (4 * OFFSET_REG_13)(sp)   /* register r5     */
	ldwio r5, (4 * OFFSET_REG_14)(sp)   /* register r4     */
	ldwio r4, (4 * OFFSET_REG_15)(sp)   /* register r3     */
	ldwio r3, (4 * OFFSET_REG_16)(sp)   /* register r2     */
	ldwio r2, (4 * OFFSET_REG_17)(sp)   /* register r1     */
	ldwio r1, (4 * OFFSET_REG_18)(sp)   /* register r1     */

	ldwio r24, (4 * OFFSET_REG_19)(sp)  /* register r24    */
	ldwio r23, (4 * OFFSET_REG_20)(sp)  /* register r23    */
	ldwio r22, (4 * OFFSET_REG_21)(sp)  /* register r22    */
	ldwio r21, (4 * OFFSET_REG_22)(sp)  /* register r21    */
	ldwio r20, (4 * OFFSET_REG_23)(sp)  /* register r20    */
	ldwio r19, (4 * OFFSET_REG_24)(sp)  /* register r19    */
	ldwio r18, (4 * OFFSET_REG_25)(sp) /* register r18    */
	ldwio r17, (4 * OFFSET_REG_26)(sp) /* register r17    */
	ldwio r16, (4 * OFFSET_REG_27)(sp) /* register r16    */


	addi sp, sp, 112
// Continues in exception section with the other kind of exceptions
/********************************************************************************/


	.section .text, "xa"
	.type GRTOS_Change_SP_to_IDLE, @function
GRTOS_Change_SP_to_IDLE:
    /// sp = &g_kcb.G_PCBTbl[gm_ProcessorId -1].PCB_IDLETCB->TCB_StackPointer
    get_tcb_stackpointer r4, r5, r6, gs_offsetof_PCB_IDLETCB
       
    /// Restore the stack pointer of IDLD TCB structure
    ldwio sp, (r4)

    movia r4, gk_KERNEL_TASK_COMPLETE
	callr r4

	.section .text, "xa"
	.align 4
	.type GRTOS_jump_to_reset, @function
GRTOS_jump_to_reset:
    movia r4, grtos_start_rest_of_processors
	callr r4
GRTOS_jump_to_reset_end:


.endif