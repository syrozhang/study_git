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
#include "os_api.h"
#include "sysg_api.h"
#include "stdio.h"

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 *bssEndAddr;

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define NR_MEM_POOLS 8

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct bufDesc_s {
	UINT32 size;
	UINT32 nrBlks;
} bufDesc_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
bufDesc_t bufProf[NR_MEM_POOLS] = {
	{(64        ), 1024}, /* 64   byte x 1024 entries */
	{(512       ), 32},   /* 512  byte x 32   entries */
	{(2048      ), 16},   /* 2K   byte x 16   entries */
	{(16  * 1024), 8},    /* 16K  byte x 8    entries */
//	{(64  * 1024), 4},    /* 64K  byte x 2    entries */
	{(64  * 1024), 4},    /* 64K  byte x 5    entries */		
	{(128 * 1024), 1},    /* 128K byte x 2    entriy  */
	{(400 * 1024), 1},    /* 400K byte x 1    entry   */
//	{(512 * 1024), 1},     /* 512K byte x 1    entry   */
/* fqdao_modify for bug 06.5.30**/
	{(1536 * 1024), 1}     /* 512K byte x 1    entry   */
};
OS_MPCB pool[NR_MEM_POOLS];

/**************************************************************************
 *                                                                        *
 *  Function Name: sysMemInit                                             *
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
sysMemInit(
	void
)
{
	bufDesc_t *pbufProf;
	UINT32 freeMemAddr, poolSize;
	UINT32 i;

	pbufProf = &bufProf[0];
	freeMemAddr = (UINT32 )bssEndAddr;
	for ( i = 0; i < NR_MEM_POOLS; i++ ) {
		dprint2("Size[%d] NRBLKS[%d]\n", pbufProf->size, pbufProf->nrBlks);
		poolSize = (pbufProf->size + sizeof(OS_MBLK)) * pbufProf->nrBlks;
		osMemPoolCreate(pool + i, (UINT8 *)freeMemAddr, poolSize, pbufProf->size);
		freeMemAddr = freeMemAddr + poolSize;
		pbufProf++;
	}
	freeMemAddr = ((freeMemAddr + 2047) / 2048) * 2048; /* 2K alignment */
	freeMemAddr = (freeMemAddr >> 1) & 0x00ffffff;
	sysgMemCommInit(freeMemAddr);
}
