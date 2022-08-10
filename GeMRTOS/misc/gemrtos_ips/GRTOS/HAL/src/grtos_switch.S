/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief Switching functions for nios II
 *  \details This file contains the Switching functions for nios II.
 *  \version 0.9a
 *  \date      2015-2020
 *  \bug  None known
 *  \copyright Ricardo Cayssials  (rcayssials@gmail.com)
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


    .global gk_ENTRY_SIGNAL_RETURN
	.global GRTOS_Start_Task
	.global GRTOS_Suspend_Task
	.global gk_ENTRY_BAD_TASK_RETURN
	.global GRTOS_Return_from_Signal
    .global Switch_to_idle
    /// .global NIOS2_ICACHE_SIZE
    /// .global NIOS2_ICACHE_LINE_SIZE
    /// .global NIOS2_DCACHE_SIZE
    /// .global NIOS2_DCACHE_LINE_SIZE


/********************************************************************************/    
    .globl grtos_start_rest_of_processors
    .section rstaux, "xa"
    /// .set noat
grtos_start_rest_of_processors:    
    /// ##########################################
    /// __reset();    
    /// Set the Stack Pointer for each Processor with cpuid greater than 1
    /// equal to __alt_stack_pointer - (cpuid - 1) * 2000
	movhi sp, %hi(__alt_stack_pointer) 
	ori sp, sp, %lo(__alt_stack_pointer) 

    /// Multiplication rewritten for no-multipler processor compatibility
    rdctl r2, cpuid
    addi r2, r2, -1
    mov r3, zero
mul0:
    beq r2, zero, out_mul0
    addi r3, r3, -2000
    addi r2, r2, -1
    jmpi mul0
out_mul0:
    add sp, sp, r3 

    /// From Nios III Software Developer Handbook
    /// Initialize the Instruction cache just in case
    mov r4, r0
    movhi r5, %hi(NIOS2_ICACHE_SIZE)
    ori r5, r5, %lo(NIOS2_ICACHE_SIZE)
icache_init_loop:
    initi r4
    addi r4, r4, NIOS2_ICACHE_LINE_SIZE
    bltu r4, r5, icache_init_loop    
    
    /// Initialize the Data cache just in case
    mov r4, r0
    movhi r5, %hi(NIOS2_DCACHE_SIZE)
    ori r5, r5, %lo(NIOS2_DCACHE_SIZE)
dcache_init_loop:
    initd 0(r4)
    addi r4, r4, NIOS2_DCACHE_LINE_SIZE
    bltu r4, r5, dcache_init_loop    

    
/// /// version sin multiplicador por hardware
///     addi r3,zero, 0
///     /// CPUID is equal to or greater than 2
///     rdctl r2, cpuid
///     addi r2, r2, -1
/// bucle:
///     addi r3, r3, -2000
///     addi r2, r2, -1
///     bne r2, zero, bucle
///     add sp, sp, r3

///    Version con multiplicador por hardware
///    /// change to temporary stack in case multiply is implemented by software
///    addi sp, sp, -2000
///    rdctl r2, cpuid 
///    addi r2, r2, -1 
///    muli r2, r2, -2000 
///    add sp, sp, r2 
    
/// exit_reset:
    /* Set up the global pointer. It is required for gcc compilation */
    /// Otherwise, _gp may give an exception
	movhi gp, %hi(_gp) 
	ori gp, gp, %lo(_gp)
    
    call gk_ENTRY_RST_HANDLER
/********************************************************************************/


/// Interruption in http://www-ug.eecg.toronto.edu/msl/nios_interrupts.html

/// THIS IS CALL FROM gk_KERNEL_TASK_SUSPEND_CURRENT WITH G_TCB_CURRENT
    .set noat
GRTOS_Suspend_Task:
	// Update the new stack pointer
	addi sp, sp, -112

	/// Save the registers of the current task.
    /// https://courses.cs.washington.edu/courses/cse467/08au/labs/Tutorials/tut_nios2_introduction.pdf pp 18
	/* ra is saved as return to gk_ENTRY_BAD_TASK_RETURN bellow */
	addi  r31, r31, 4   /* ra as exception plus 4 */
	stwio r31, 8(sp)   /* register r29 ea */ /* ra as exception return */
	stwio r28, 12(sp)  /* register r28 fp */
	stwio r15, 16(sp)  /* register r15    */
	stwio r14, 20(sp)  /* register r14    */
	stwio r13, 24(sp)  /* register r13    */
	stwio r12, 28(sp)  /* register r12    */
	stwio r11, 32(sp)  /* register r11    */
	stwio r10, 36(sp)  /* register r10    */
	stwio r9, 40(sp)   /* register r9     */
	stwio r8, 44(sp)   /* register r8     */
	stwio r7, 48(sp)   /* register r7     */
	stwio r6, 52(sp)   /* register r6     */
	stwio r5, 56(sp)   /* register r5     */
	stwio r4, 60(sp)   /* register r4     */
	stwio r3, 64(sp)   /* register r3     */
	stwio r2, 68(sp)   /* register r2     */
	stwio r1, 72(sp)   /* register r1     */

	stwio r24, 76(sp)  /* register r24    */
	stwio r23, 80(sp)  /* register r23    */
	stwio r22, 84(sp)  /* register r22    */
	stwio r21, 88(sp)  /* register r21    */
	stwio r20, 92(sp)  /* register r20    */
	stwio r19, 96(sp)  /* register r19    */
	stwio r18, 100(sp) /* register r18    */
	stwio r17, 104(sp) /* register r17    */
	stwio r16, 108(sp) /* register r16    */

	movia r5, gk_ENTRY_BAD_TASK_RETURN
	stwio r5, 4(sp)   /* register r31 ra */

    /// Read status to save as estatus
    rdctl r5, status  /* estatus         */
    /// ori  r5, r5, 1
	stwio r5, 0(sp)

    wrctl status, zero /* Disable Interrupt  */
    
    /// Save the current stack pointer in TCB structure
    movia r4, G_TCB_CURRENT
    ldw	r4, 0(r4)
    stwio sp, (r4)    
    
    jmpi Switch_to_idle


    .globl grtos_irq_entry
    .section .irq, "xa"
/********************************************************************************/
/****   Entry point of NIOS II Interrupt Handler    *****************************/
/** https://forums.intel.com/s/createarticlepage?language=en_US&articleid=a3g0P0000005RdqQAE&artTopicId=0TO0P000000MWKBWA4&action=view **/
/********************************************************************************/
grtos_irq_entry:
/********************************************************************************/
	// Update the new stack pointer
	addi sp, sp, -112

	/// Save the registers of the current task. ea is already incremented in 4
	stwio r31, 4(sp)   /* register r31 ra */
	stwio r29, 8(sp)   /* register r29 ea */
	stwio r28, 12(sp)  /* register r28 fp */
	stwio r15, 16(sp)  /* register r15    */
	stwio r14, 20(sp)  /* register r14    */
	stwio r13, 24(sp)  /* register r13    */
	stwio r12, 28(sp)  /* register r12    */
	stwio r11, 32(sp)  /* register r11    */
	stwio r10, 36(sp)  /* register r10    */
	stwio r9, 40(sp)   /* register r9     */
	stwio r8, 44(sp)   /* register r8     */
	stwio r7, 48(sp)   /* register r7     */
	stwio r6, 52(sp)   /* register r6     */
	stwio r5, 56(sp)   /* register r5     */
	stwio r4, 60(sp)   /* register r4     */
	stwio r3, 64(sp)   /* register r3     */
	stwio r2, 68(sp)   /* register r2     */
	stwio r1, 72(sp)   /* register r1     */

	stwio r24, 76(sp)  /* register r24    */
	stwio r23, 80(sp)  /* register r23    */
	stwio r22, 84(sp)  /* register r22    */
	stwio r21, 88(sp)  /* register r21    */
	stwio r20, 92(sp)  /* register r20    */
	stwio r19, 96(sp)  /* register r19    */
	stwio r18, 100(sp) /* register r18    */
	stwio r17, 104(sp) /* register r17    */
	stwio r16, 108(sp) /* register r16    */


    /// Read estatus to know if it is exception
    rdctl r5, estatus  /* estatus         */
	stwio r5, 0(sp)

    /// Test if it is a exception
    /// .lrestore executes the original exception routine
    rdctl r4, ipending
    andi  r2, r5 , 1
    beq   r2, zero, .Lrestore
    beq   r4, zero, .Lrestore

.Lgrtosrun:
	movia r4, G_Running
	ldwio r4, (r4)
	bne   r4, zero, .Lgrtos
	rdctl r4, ipending
	br .Lrestore

.Lgrtos:
	/// Get the adequate G_TCB_CURRENT
    /// ********************************
    /// (GS_STK) &g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackPointer = sp
    rdctl	r4,cpuid       /* r4 = cpuid     */
    addi	r4,r4,-1       /* r4 = cpuid -1  */

    movia  r5, gs_sizeof_G_PCBTbl
    ldw	r5,0(r5)
    /// mul	r4, r4, r5                        /* r4 = sizeof(g_kcb.G_PCBTbl) * (cpuid-1)             */
    
    /// Multiplication rewritten for no-multipler processor compatibility
    /// mul	r4, r4, r5                     /* r4 = sizeof(g_kcb.G_PCBTbl) * (cpuid-1)                   */
    mov r6, zero
mul2:
    beq r4, zero, out_mul2
    add r6, r6, r5
    addi r4, r4, -1
    jmpi mul2
out_mul2:
    mov r4, r6    
    
    
    
    
    movia	r5,gs_addressof_G_PCBTbl    /* r5 = &g_kcb.G_PCBTbl                                */
    ldw	r5,0(r5)
    add	r4,r4,r5                        /* r4 = &g_kcb.G_PCBTbl + sizeof(g_kcb.G_PCBTbl) * (cpuid-1) */

    movia  r5, gs_offsetof_PCB_EXECTCB
    ldw	r5,0(r5)
    add	r4,r4,r5       /* r4 = &g_kcb.G_PCBTbl + sizeof(g_kcb.G_PCBTbl) * (cpuid-1) + offsetof(struct gs_pcb, PCB_EXECTCB)       */
    ldw	r4,0(r4)       /* r4 = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB                                             */

    movia  r5, gs_offsetof_TCB_StackPointer
    ldw	r5,0(r5)
    add	r4,r4,r5       /* r4 = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB + offsetof(struct gs_tcb, TCB_StackPointer) */ 
    /// Save the current stack pointer in TCB structure
    stwio sp, (r4)

Switch_to_idle:
    /// ********************************
    /// * Switch to idle task stack
    /// ******************************** 
    /// sp = &g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB->TCB_StackPointer
    rdctl	r4,cpuid     /* r4 = cpuid                                  */
    addi	r4,r4,-1     /* r4 = cpuid -1                               */

    movia  r5, gs_sizeof_G_PCBTbl
    ldw	r5,0(r5)
    /// mul	r4,r4,r5         /* r4 = sizeof(g_kcb.G_PCBTbl) * (cpuid-1)     */
    
    /// Multiplication rewritten for no-multipler processor compatibility
    /// mul	r4, r4, r5                     /* r4 = sizeof(g_kcb.G_PCBTbl) * (cpuid-1)                   */
    mov r6, zero
mul3:
    beq r4, zero, out_mul3
    add r6, r6, r5
    addi r4, r4, -1
    jmpi mul3
out_mul3:
    mov r4, r6    
    

    movia	r5,gs_addressof_G_PCBTbl    /* r5 = &g_kcb.G_PCBTbl                                */
    ldw	r5,0(r5)
    add	r4,r4,r5         /* r4 = &g_kcb.G_PCBTbl + sizeof(g_kcb.G_PCBTbl) * (cpuid-1)                                              */

    movia  r5, gs_offsetof_PCB_IDLETCB
    ldw	r5,0(r5)
    add	r4,r4,r5         /* r4 = &g_kcb.G_PCBTbl + sizeof(g_kcb.G_PCBTbl) * (cpuid-1) + offsetof(struct gs_pcb, PCB_IDLETCB)       */
    ldw	r4,0(r4)         /* r4 = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB                                             */

    movia  r5, gs_offsetof_TCB_StackPointer
    ldw	r5,0(r5)
    add	r4,r4,r5         /* r4 = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_IDLETCB + offsetof(struct gs_tcb, TCB_StackPointer) */ 
    ldwio sp, (r4)
    /// ********************************
    // Call the Interrupt Handler Routine
    call gk_ENTRY_IRQ_HANDLER


/********************************************************************************/
/****   Function to start executing a task          *****************************/
/********************************************************************************/
GRTOS_Start_Task:
    //// g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB = (struct gs_tcb *) ptcb;
	/// Restore the Status from the stack as it was saved 
    /// sp = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB->TCB_StackPointer;
    /// *************************************************************************
    rdctl	r4,cpuid                   /* r4 = cpuid                                                    */
    addi	r4,r4,-1                   /* r4 = cpuid -1                                                 */
    
    movia  r5, gs_sizeof_G_PCBTbl
    ldw	r5,0(r5)
    
    /// Multiplication rewritten for no-multipler processor compatibility
    /// mul	r4, r4, r5                     /* r4 = sizeof(g_kcb.G_PCBTbl) * (cpuid-1)                   */
    mov r6, zero
mul1:
    beq r4, zero, out_mul1
    add r6, r6, r5
    addi r4, r4, -1
    jmpi mul1
out_mul1:
    mov r4, r6


    movia	r5,gs_addressof_G_PCBTbl   /* r5 = &g_kcb.G_PCBTbl                                          */
    ldw	r5,0(r5)
    add	r4, r4, r5                     /* r4 = &g_kcb.G_PCBTbl + sizeof(g_kcb.G_PCBTbl) * (cpuid-1)     */
    
    movia r5, gs_offsetof_PCB_EXECTCB
    ldw	r5,0(r5)
    add	r4, r4, r5       /* r4 = &g_kcb.G_PCBTbl + sizeof(g_kcb.G_PCBTbl) * (cpuid-1) + offsetof(struct gs_pcb, PCB_EXECTCB)       */
    ldw	r4, 0(r4)        /* r4 = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB                                             */
    
    movia r5, gs_offsetof_TCB_StackPointer
    ldw	r5, 0(r5)
    add	r4, r4, r5       /* r4 = g_kcb.G_PCBTbl[GRTOS_CMD_PRC_ID -1].PCB_EXECTCB + offsetof(struct gs_tcb, TCB_StackPointer) */ 
    ldwio sp, (r4)    
    /// *************************************************************************
    
    /// movia r4, G_TCB_NEXT
    /// ldwio sp, (r4)
    
    // Jump to Skip_Mutex to avoid get mutex again
	jmpi GRTOS_Skip_Mutex

/********************************************************************************/
/****   Return point from signal execution          *****************************/
/********************************************************************************/
GRTOS_Return_from_Signal:

    /// Get muetx and return wit mutex granted
    call gk_ENTRY_SIGNAL_RETURN

	/* Finish execution of Pending SCB */

GRTOS_Skip_Mutex:

	// Call the GRTOS_Task_GetPendingSCB function to execute the pending signals in task mode
	// This function modifies the stack if a signal has to be executed
	call GRTOS_Task_GetPendingSCB
	// Return [G_SCB_PENDING] = 1 if SCB pending, 0 otherwise
    //		  [G_SCB_CODE]    = SCB.SCB_TaskCode + 4
	//		  [G_SCB_ARG]     = SCB.SCB_TaskArg
	//		  Return_address = GRTOS_Return_from_Signal

    // Check if there is a pending SCB
	movia r4, G_SCB_PENDING
	ldwio r4, (r4)
	beq   r4, zero, L_SCBNoPending

	// Insert the register to execute the signal code
////////////////////////////////////////////////////////////
	// Update the new stack pointer
	addi sp, sp, -112

    /// Read estatus to know if it is exception
    addi r4, zero, 1
	stwio r4, 0(sp)    /* estatus         */

	/// Save the registers of the current task.
	movia r4, GRTOS_Return_from_Signal
	stwio r4, 4(sp)   /* register r31 ra */

	movia r4, G_SCB_CODE
	ldwio r4, (r4)
	stwio r4, 8(sp)   /* register r29 ea */

	subi  r4, sp, 1
	stwio r4, 12(sp)  /* register r28 fp */

	stwio r15, 16(sp)  /* register r15    */
	stwio r14, 20(sp)  /* register r14    */
	stwio r13, 24(sp)  /* register r13    */
	stwio r12, 28(sp)  /* register r12    */
	stwio r11, 32(sp)  /* register r11    */
	stwio r10, 36(sp)  /* register r10    */
	stwio r9, 40(sp)   /* register r9     */
	stwio r8, 44(sp)   /* register r8     */
	stwio r7, 48(sp)   /* register r7     */
	stwio r6, 52(sp)   /* register r6     */
	stwio r5, 56(sp)   /* register r5     */

	movia r4, G_SCB_ARG
	ldwio r4, (r4)
	stwio r4, 60(sp)   /* register r4     */
	stwio r3, 64(sp)   /* register r3     */

	movia r4, G_SCB_ARG
	ldwio r4, (r4)
	stwio r4, 68(sp)   /* register r2     */

	stwio r1, 72(sp)   /* register r1     */

	stwio r24, 76(sp)  /* register r24    */
	stwio r23, 80(sp)  /* register r23    */
	stwio r22, 84(sp)  /* register r22    */
	stwio r21, 88(sp)  /* register r21    */
	stwio r20, 92(sp)  /* register r20    */
	stwio r19, 96(sp)  /* register r19    */
	stwio r18, 100(sp) /* register r18    */
	stwio r17, 104(sp) /* register r17    */
	stwio r16, 108(sp) /* register r16    */


///////////////////////////////////////////////////////

L_SCBNoPending:
	/// Restore the saved registers for the new task.
	ldwio r4,  0(sp)   /* estatus         */
    ori  r4, r4, 1
    wrctl estatus, r4

	ldwio r31, 4(sp)   /* register r31 ra */
	ldwio r29, 8(sp)   /* register r29 ea */
	ldwio r28, 12(sp)  /* register r28 fp */
	ldwio r15, 16(sp)  /* register r14    */
	ldwio r14, 20(sp)  /* register r13    */
	ldwio r13, 24(sp)  /* register r12    */
	ldwio r12, 28(sp)  /* register r11    */
	ldwio r11, 32(sp)  /* register r10    */
	ldwio r10, 36(sp)   /* register r9     */  /* USED TO RELEASE MUTEX     */
	//ldwio r9, 40(sp)   /* register r8     */  /* RESTORE BEFORE ERET       */
	//ldwio r8, 44(sp)   /* register r7     */
	ldwio r7, 48(sp)   /* register r6     */
	ldwio r6, 52(sp)   /* register r5     */
	ldwio r5, 56(sp)   /* register r4     */
	ldwio r4, 60(sp)   /* register r3     */
	ldwio r3, 64(sp)   /* register r2     */
	ldwio r2, 68(sp)   /* register r2     */
	ldwio r1, 72(sp)   /* register r2     */

	ldwio r24, 76(sp)  /* register r24    */
	ldwio r23, 80(sp)  /* register r23    */
	ldwio r22, 84(sp)  /* register r22    */
	ldwio r21, 88(sp)  /* register r21    */
	ldwio r20, 92(sp)  /* register r20    */
	ldwio r19, 96(sp)  /* register r19    */
	ldwio r18, 100(sp) /* register r18    */
	ldwio r17, 104(sp) /* register r17    */
	ldwio r16, 108(sp) /* register r16    */

	///Enable Interrupt current processor and release mutex to execute task
    rdctl r8, cpuid

    /* Release GRTOS Mutex */
    movia r9, GRTOS_MutexReleaseRegisterAddress
    // movhi r9, %hi(GRTOS_DRIVER_GRTOS_BASE + 4 * ADDR_MTX_RLS)
    // ori r9, r9, %lo(GRTOS_DRIVER_GRTOS_BASE + 4 * ADDR_MTX_RLS)
    ldwio r9, 0(r9)
    stwio r8, 0(r9)

	/* Enable the GRTOS Interrupt for current processor */
    movia r9, GRTOS_InterruptEnableRegisterAddress
    ldwio r9, 0(r9)
    stwio r8, 0(r9)

	ldwio r9, 40(sp)   /* register r9     */
	ldwio r8, 44(sp)   /* register r8     */

	// Update the SP register
	addi sp, sp, 112
	/// Return enabling nios interrupt
	addi  ea, ea, -4  /* instruction that caused exception */
	eret


.Lrestore:
	/// Restore the saved registers for the new task.
	ldwio r4,  0(sp)   /* estatus         */
    wrctl estatus, r4

	ldwio r31, 4(sp)   /* register r31 ra */
	ldwio r29, 8(sp)   /* register r29 ea */
	ldwio r28, 12(sp)  /* register r15 fp */
	ldwio r15, 16(sp)  /* register r14    */
	ldwio r14, 20(sp)  /* register r13    */
	ldwio r13, 24(sp)  /* register r12    */
	ldwio r12, 28(sp)  /* register r11    */
	ldwio r11, 32(sp)  /* register r10    */
	ldwio r10, 36(sp)  /* register r9     */
	ldwio r9, 40(sp)   /* register r8     */
	ldwio r8, 44(sp)   /* register r7     */
	ldwio r7, 48(sp)   /* register r6     */
	ldwio r6, 52(sp)   /* register r5     */
	ldwio r5, 56(sp)   /* register r4     */
	ldwio r4, 60(sp)   /* register r3     */
	ldwio r3, 64(sp)   /* register r2     */
	ldwio r2, 68(sp)   /* register r1     */
	ldwio r1, 72(sp)   /* register r1     */

	ldwio r24, 76(sp)  /* register r24    */
	ldwio r23, 80(sp)  /* register r23    */
	ldwio r22, 84(sp)  /* register r22    */
	ldwio r21, 88(sp)  /* register r21    */
	ldwio r20, 92(sp)  /* register r20    */
	ldwio r19, 96(sp)  /* register r19    */
	ldwio r18, 100(sp) /* register r18    */
	ldwio r17, 104(sp) /* register r17    */
	ldwio r16, 108(sp) /* register r16    */


	addi sp, sp, 112
// Continues in exception section with the other kind of exceptions
/********************************************************************************/
