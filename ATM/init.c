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
#include "general.h"
#include "sysregs.h"
#include "hw_cpu.h"
#include "stdio.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*#define RAM_PROGRAM 1*/
/*#define MIDE 1*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void start(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 *romCodeEndAddr;
UINT32 *ramCodeStartAddr;
UINT32 *romDataStartAddr;
UINT32 *ramDataStartAddr;
UINT32 *bssStartAddr;
UINT32 *bssEndAddr;

/**************************************************************************
 *                                                                        *
 *  Function Name: __init                                                 *
 *                                                                        *
 *  Purposes: Low level initialization function.                          *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None.                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
__init(
	void
)
{
	flashReg_t *pflashReg;
	audReg_t *paudReg;
	UINT32 *src;
	UINT32 *dst;

	pflashReg = (flashReg_t *)0xb0000000;
	paudReg = (audReg_t *)0xb0006000;
	pflashReg->ndCtrl[0] = 0x03;
	pflashReg->romCyc = 0x22;    /* ROM speed */
	paudReg->aInt = 0;
	paudReg->aGpioiRiseInt = 0;
	paudReg->aGpioiFallInt = 0;

	/* Sp, Gp remap setting */
	WRITE32(0xb0001070, 0x0C810000);
	WRITE32(0xb0001074, 0x0C880000);

	/*
	 *  Global varaible initialization:
	 *  Unpack the initial data from rom to dram region.
	 */
	#ifdef RAM_PROGRAM
	src = romCodeEndAddr;
	dst = ramCodeStartAddr;
	#else
	src = romDataStartAddr;
	dst = ramDataStartAddr;
	#endif

	#ifndef MIDE
	while ( dst < bssStartAddr ) {
		*dst = *src;
		dst++;
		src++;
	}

	/* Zero the uninitialize data .bss section */
	/* The tricky part starts from here: if you set dst = bssStartAddr, it will
	 * erase itself and bssEndAddr because they all belong to the .bss section.
	 * If you give them intial values and they will be copied to zero after we
	 * set them to the proper values, that also does not work.
	 * Let's make the assumption that this file is the first one to contain
	 * bss data, and &bssEndAddr should be the address of bss + sizeof(bss data
	 * in this file) - sizeof(UINT32 *).
	 */
	dst = (UINT32 *)(&bssEndAddr);
	dst++;
	while ( dst < bssEndAddr ) {
		*dst = 0;
		dst++;
	}
	#endif /* #ifndef MIDE */
	#ifdef NAND_BOOT_WORKAROUND
	src = (UINT32 *)start;
	dst = (UINT32 *)0x8c000000;
	while ( src < romCodeEndAddr ) {
		*dst = *src;
		dst++;
		src++;
	}
	#endif
}
