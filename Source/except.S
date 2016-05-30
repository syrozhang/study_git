/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2002 by Sunplus Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Timothy Wu                                                    *
 *                                                                        *
 **************************************************************************/
	#include "mips.h"

	.text
	.globl   interrupt
	.ent     interrupt
interrupt:
	/*
	 * At this points IRQs are disabled, so we know that the EPC
	 * register will not be trashed by another interrupt
	 */
	.set    noat
	addiu   sp, sp, -STACK_SIZE
	sw      AT, STACK_AT(sp)
	.set    at
	sw      v0, STACK_V0(sp)
	sw      v1, STACK_V1(sp)
	sw      a0, STACK_A0(sp)
	sw      a1, STACK_A1(sp)
	sw      a2, STACK_A2(sp)
	sw      a3, STACK_A3(sp)
	sw      t0, STACK_T0(sp)
	sw      t1, STACK_T1(sp)
	sw      t2, STACK_T2(sp)
	sw      t3, STACK_T3(sp)
	sw      t4, STACK_T4(sp)
	sw      t5, STACK_T5(sp)
	sw      t6, STACK_T6(sp)
	sw      t7, STACK_T7(sp)
	sw      t8, STACK_T8(sp)
	sw      t9, STACK_T9(sp)
	sw      ra, STACK_RA(sp)
	mfhi    t0
	sw      t0, STACK_HI(sp)
	mflo    t0
	sw      t0, STACK_LO(sp)

	/* All registers are saved now         */
	/* push EPC to allow nested interrupts */
	mfc0    t0, C0_EPC
	nop
	sw      t0, STACK_EPC(sp)

	la      t0, osRunning
	lw      k0, 0x0(t0)     /* k0 = osRunning, 1 is TRUE */
	nop
	beq     k0, zero, $$osIrqNestingFinish
	la      t0, osIrqNesting
	lw      k1, 0x0(t0)
	nop
	addiu   k1, k1, 1
	sw      k1, 0x0(t0)
	nop
$$osIrqNestingFinish:

	/* Pass cause to high-level handler */
	mfc0    a0, C0_CAUSE
	mfc0    a1, C0_EPC
	#if 1 /* Timothy 0325 */
	move    a2, sp
	#endif
	jal     irqProcess
	nop

	/**********************************************************************/
	/* Restore registers (context) from stack                             */
	/**********************************************************************/
$$contextRestore:
	/* Timothy: we do not allow nested intr, so k0 is kept as osRunning */
	beq     k0, zero, $$osCtxSwCheckEnd
	nop
	jal     osIrqExit
	nop
	beq     v0, zero, $$osCtxSwCheckEnd
	nop
	/* context switch required */
	/* When interrupt happens, s0 ~ s8 must be saved if context switch will occur... */
	sw      s0, STACK_S0(sp)
	sw      s1, STACK_S1(sp)
	sw      s2, STACK_S2(sp)
	sw      s3, STACK_S3(sp)
	sw      s4, STACK_S4(sp)
	sw      s5, STACK_S5(sp)
	sw      s6, STACK_S6(sp)
	sw      s7, STACK_S7(sp)
	sw      s8, STACK_S8(sp)

	/* Get current task TCB address and save sp in the TCB */
	lw      t0, osTcbCur			/* get TCB addr */
	nop                             /* Delay for R3000 */
	sw      sp, (t0)				/* store sp in preempted tasks TCB */

	/* Get highest priority task TCB address */
	lw      t0, osTcbHighRdy		/* get TCB addr */
	nop                             /* Delay for R3000 */
	lw      sp, 0x0(t0)				/* get new tasks stack pointer */
	sw      t0, osTcbCur			/* set new current task TCB address */
	nop
	lw      s0, STACK_S0(sp)
	lw      s1, STACK_S1(sp)
	lw      s2, STACK_S2(sp)
	lw      s3, STACK_S3(sp)
	lw      s4, STACK_S4(sp)
	lw      s5, STACK_S5(sp)
	lw      s6, STACK_S6(sp)
	lw      s7, STACK_S7(sp)
	lw      s8, STACK_S8(sp)
$$osCtxSwCheckEnd:

	.set 	reorder
	/* Restore registers */
	lw      t0, STACK_HI(sp)
	mthi    t0
	lw      t0, STACK_LO(sp)
	mtlo    t0
	lw      v1, STACK_V1(sp)
	lw      a0, STACK_A0(sp)
	lw      a1, STACK_A1(sp)
	lw      a2, STACK_A2(sp)
	lw      a3, STACK_A3(sp)
	lw      t0, STACK_T0(sp)
	lw      t1, STACK_T1(sp)
	lw      t2, STACK_T2(sp)
	lw      t3, STACK_T3(sp)
	lw      t4, STACK_T4(sp)
	lw      t5, STACK_T5(sp)
	lw      t6, STACK_T6(sp)
	lw      t7, STACK_T7(sp)
	lw      t8, STACK_T8(sp)
	lw      t9, STACK_T9(sp)
	lw      ra, STACK_RA(sp)
	lw      v0, STACK_V0(sp)
	.set    noat
	lw      AT, STACK_AT(sp)
	.set    at

	/* Return to the interrupted thread */
	.set    noreorder
	lw      k0, STACK_EPC(sp)
	addiu   sp, sp, STACK_SIZE
	j       k0
	rfe
	.set    reorder
	.end    interrupt
