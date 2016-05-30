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
	.extern __init
	.extern interrupt
	.extern __main

	.globl  start
	.ent    start
start:
	/*_____________________________________________________________________
	
	  Fill in stack pointer $29 and global pointer $28
	  _____________________________________________________________________
	*/
	la		sp, __stack
	la		gp, _gp

	/*_____________________________________________________________________
	
	  Cache invalidate:
	    If you have to invalidate the cache, execute the function before
	    you make any function calls
	  _____________________________________________________________________
	*/
	#if 0
	li      k0, 0x00000003
	li      k1, 512
	mtc0    k0, $20
__cacheInv:
	subu    k1, k1, 1
	bne     k1, $0, __cacheInv
	nop
	#endif

	/*_____________________________________________________________________
	
	  Jump to reset handler. Please do not insert program before .exceptRam
	  and .exceptRom sections.
	  _____________________________________________________________________
	*/
	j       __resetTrampoline
	nop
	.end    start

	.section .exceptRam, "ax"
	la		$26, interrupt
	nop
	jr		$26
	nop

	.section .exceptRom, "ax"
	la      $26, interrupt
	nop
	jr		$26
	nop

	/*_____________________________________________________________________
	
	  Reset Handler:
	    1. Detect and initialize DRAM (by reading I/O trap)
	    2. Initialize address varialbes (referenced by __init())
	    3. Disable timers and clear timer's interrupt
	  _____________________________________________________________________
	*/
	.ent    __resetTrampoline
__resetTrampoline:
	/* SDRAM initialization */

	lui     t0, 0xb000
	li      t1, 0xfd
	sb      t1, (0xa000)(t0)    /* Disable video DAC to reduce the power */
	sb      zero, (0x1000)(t0)	/* Disable All intr to CPU */
	sb      zero, (0x7009)(t0)  /* make sure DRAM is not self-refresh state */

	jal		sdramType
	nop
	jal		sdramInit
	nop

	/* Initialize address reference variables */
	jal		addrInit
	nop

	/* Move and clear data area */
	la      t3, __init
	nop
	jalr	t3
	nop
	la      t4, __main

	lui     t0, 0xb000         /* Stop all timers */
	sw      $0, (0x1044)(t0)
	li      t1, 0xff           /* Clear all interrupts, sw does not work? */
	sb      t1, (0x10c0)(t0)
	sb      t1, (0x10c1)(t0)
	sb      t1, (0x10c2)(t0)
	sb      t1, (0x10c3)(t0)
	sb      zero, (0x0001)(t0) /* Unlock all modules */

	/*_____________________________________________________________________
	
	  Enable interrupt mask and jump to main() (never back again)
	  _____________________________________________________________________
	*/
	mfc0    s0, $12
	ori     s1, s0, 0xfc01   /* Enable interrupt [5:0] */
	mtc0    s1, $12
	jr      t4               /* Never return */
	nop
	.end    __resetTrampoline

	.ent    sdramType
sdramType:
	lui		t0, 0xb000       /* Configure SDRAM Type*/

	#ifndef RAM_PROGRAM
	/* DRAM S/W reset */
	li      t1, 0x10
	sb      t1, (0x0001)(t0)
	sb      zero, (0x0001)(t0)
	#endif

	lb      t2, (0x00b1)(t0)
	li      t3, 0x1C
	and     t2, t2,t3

	li      t1, 0x00
	beq     t2, t1, sdr16M
	li      t1, 0x04
	beq     t2, t1, sdr64M
	li      t1, 0x08
	beq     t2, t1, sdr128M
	li      t1, 0x0C
	beq     t2, t1, sdr256M
	li      t1, 0x10
	beq     t2, t1, sdr512M
	li      t1, 0x14
	beq     t2, t1, ddr128M
	li      t1, 0x18
	beq     t2, t1, ddr256M
	li      t1, 0x1C
	beq     t2, t1, ddr512M

sdr16M:
	li      t1, 0x01
	sb      t1, (0x7000)(t0)
	jr		ra
	nop
sdr64M:
	li      t1, 0x02
	sb      t1, (0x7000)(t0)
	jr		ra
	nop
sdr128M:
	li      t1, 0x03
	sb      t1, (0x7000)(t0)
	jr		ra
	nop
sdr256M:
	li      t1, 0x04
	sb      t1, (0x7000)(t0)
	jr		ra
	nop
sdr512M:
	li      t1, 0x05
	sb      t1, (0x7000)(t0)
	jr		ra
	nop
ddr128M:
	li      t1, 0x0b
	sb      t1, (0x7000)(t0)
	jr		ra
	nop
ddr256M:
	li      t1, 0x0c
	sb      t1, (0x7000)(t0)
	jr		ra
	nop
ddr512M:
	li      t1, 0x0d
	sb      t1, (0x7000)(t0)
	jr		ra
	nop
	.end	sdramType

	.ent    sdramInit
sdramInit:
	lui		t0, 0xb000
	li      t1, 0x01            /* Was 0x01: 48MHz, 3: 12MHz */
	sb      t1, (0x0012)(t0)    /* Configure CPU Freq = 48MHz */
	li      t1, 0xD000FFFF  
	sw      t1, (0x1154)(t0)    /* Configure CPU D-Mem */
	li      t1, 0x000007FF
	sw      t1, (0x115c)(t0)
	li      t1, 0x06           /* Configure SDRAM */
	sb      t1, (0x7001)(t0)
	#ifndef RAM_PROGRAM
	li      t2, 0x01
	sb      t2, (0x700f)(t0)
	#endif
	jr		ra
	nop
	.end	sdramInit

	.ent    addrInit
addrInit:
	la		k0, _erom
	la		k1, romCodeEndAddr
	sw		k0, 0x0(k1)
	la		k0, _text
	la		k1, ramCodeStartAddr
	sw		k0, 0x0(k1)
	la		k0, _dataStart
	la		k1, romDataStartAddr
	sw		k0, 0x0(k1)
	la		k0, _data
	la		k1, ramDataStartAddr
	sw		k0, 0x0(k1)
	la		k0, _bssStart
	la		k1, bssStartAddr
	sw		k0, 0x0(k1)
	la		k0, _bssEnd
	la		k1, bssEndAddr
	sw		k0, 0x0(k1)
	jr		ra
	nop
	.end	addrInit
