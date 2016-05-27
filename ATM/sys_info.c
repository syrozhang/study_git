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
 *  Author:Huiling Chou                                                   *
 *                                                                        *
 **************************************************************************/
//#define RS_232_DBG

#include "general.h"
#include "snap_api.h"
#include "exif_api.h"
#include "dcf_api.h"
#include "sysg_api.h"
#include "dsc_init.h"
#include "stdio.h"

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 *romCodeEndAddr;
extern UINT32 *ramCodeStartAddr;
extern UINT32 *romDataStartAddr;
extern UINT32 *ramDataStartAddr;
extern UINT32 *bssStartAddr;
extern UINT32 *bssEndAddr;
extern void sysgLibVersionGet(UINT8 *pbuf);
extern void exifLibVersionGet(UINT8 *pbuf);
extern void xdcfLibVersionGet(UINT8 *pbuf);
extern void snapLibVersionGet(UINT8 *pbuf);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void sysInfoPrint(void);
void sysMemInfoPrint(void);

/**************************************************************************
 *                                                                        *
 *  Function Name: sysInfoPrint                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void 
sysInfoPrint(
	void
)
{
	UINT8 str[0x20];
	UINT32 dramSize;
	
	dprint("\n");
	dprint("_______________________________________________\n");
	
	sysgDRAMSizeGet(&dramSize);
	dprint1("Spca536 DRAM SIZE (MBit)               %3d\n", dramSize);
	sysgFWVersionGet(str);
	dprint1("Spca536 Firmware Version               %s\n", str);

	#if 0
	sysMemInfoPrint();
	hwLibVersionGet(str);
	sysgLibVersionGet(str);
	dprint1("System Global Library Version          %s\n", str);
	
	exifLibVersionGet(str);
	dprint1("EXIF Library Version                   %s\n", str);
	
	xdcfLibVersionGet(str);
	dprint1("xDCF Library Version                   %s\n", str);

	snapLibVersionGet(str);
	dprint1("SNAP Library Version                   %s\n", str);

	sysgLibVersionGet(str);
	dprint1("System Gloabal Library Version         %s\n", str);
	#endif
	
	dprint("_______________________________________________\n");
	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: sysMemInfoPrint                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
sysMemInfoPrint(
	void
)
{
	UINT32 addr1, addr2, addr3;
	UINT32 w, h;
	
	printf("____________________________________________________________________\n");
	printf("                            System Memory Display                   \n");
	printf("____________________________________________________________________\n");
	
	printf("romCodeEndAddr                      = 0x%8x, size = 0x%x\n", (UINT32)romCodeEndAddr, (UINT32)ramCodeStartAddr - (UINT32)romCodeEndAddr);
	printf("ramCodeStartAddr                    = 0x%8x, size = 0x%x\n", (UINT32)ramCodeStartAddr, (UINT32)romDataStartAddr - (UINT32)ramCodeStartAddr);
	printf("romDataStartAddr                    = 0x%8x\n", (UINT32)romDataStartAddr);
	printf("ramDataStartAddr                    = 0x%8x, size = 0x%x\n", (UINT32)ramDataStartAddr, (UINT32)bssStartAddr - (UINT32)ramDataStartAddr);
	printf("bssStartAddr                        = 0x%8x, size = 0x%x\n", (UINT32)bssStartAddr, (UINT32)bssEndAddr - (UINT32)bssStartAddr);
	sysgMemBadPixCoordAddrGet(&addr1);
	addr1 = (addr1 << 1) | 0x8c000000;

	printf("bssEndAddr/MemoryPool Start         = 0x%8x, size = 0x%x\n", (UINT32)bssEndAddr, addr1 - (UINT32)bssEndAddr);

	
	sysgMemWinValueAddrGet(&addr2, &addr3);
	addr2 = (addr2 << 1) | 0x8c000000;
	addr3 = (addr3 << 1) | 0x8c000000;
	printf("BadPixel start/MemoryPool End(Byte) = 0x%8x, size = 0x%x\n", addr1, addr2 - addr1);
	
	sysgMemOSDAddrGet(&addr1);
	addr1 = (addr1 << 1) | 0x8c000000;
	printf("CDSP win val A starting addr (Byte) = 0x%8x, size = 0x%x\n", addr2, addr3 - addr2);
	printf("CDSP win val B starting addr (Byte) = 0x%8x, size = 0x%x\n", addr3, addr1 - addr3);

	sysgMemPIPAddrGet(&addr2);
	addr2 = (addr2 << 1) | 0x8c000000;
	printf("OSD starting address(Byte)          = 0x%8x, size = 0x%x\n", addr1, addr2 - addr1);

	sysgMemDispAddrGet(&addr1);
	addr1 = (addr1 << 1) | 0x8c000000;
	printf("PIP starting address(Byte)          = 0x%8x, size = 0x%x\n", addr2, addr1 - addr2);

	sysgLCDSizeGet(&w, &h);
	sysgMemAppAddrGet(&addr2);
	addr2 = (addr2 << 1) | 0x8c000000;
	printf("DISP(%dx%d) starting addr(Byte)   = 0x%8x, size = 0x%x\n", w, h, addr1, addr2 - addr1);

	printf("Application starting address(Byte)  = 0x%8x\n", addr2);
	printf("\n");
}

