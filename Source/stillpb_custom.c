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
#include <stdio.h>
#include "general.h"
#include "exif_api.h"
#include "stillpb_custom.h"
#include "snap_api.h"

/**************************************************************************
 *                                                                        *
 *  Function Name: stillPbQVCallback                                      *
 *                                                                        *
 *  Purposes:  Call back for quickview display                            *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    exifImageInfo_t : exif info of the decoded JPG file                 *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
stillPbQVCallback(
	exifImageInfo_t *pInfo
)
{
	UINT32 err = SUCCESS;
	UINT32 qvHdrByteCnt;

	qvHdrByteCnt = QV_QT_BYTE_CNT + QV_ORIENT_BYTE_CNT;
	
	if ((err = exifAppnRead(0xe4, &pInfo->paddr, &pInfo->len))==SUCCESS) {
		*pInfo->pYQT = *pInfo->paddr;
		pInfo->pUVQT = pInfo->pYQT + (QV_QT_BYTE_CNT / 2);
		pInfo->orient = *(pInfo->paddr + qvHdrByteCnt - 1);
		pInfo->paddr = pInfo->paddr + qvHdrByteCnt;

		pInfo->len = pInfo->len - qvHdrByteCnt;
		pInfo->w=320;
		pInfo->h=240;
		pInfo->pixelX=320;
		pInfo->pixelY=240;
		pInfo->thmHdrOffset = 0;
		pInfo->datafmt=1;
		pInfo->rstMCU=0;
	}
	else
		printf("QuickViw decode failed....\n");	
		
	return err;
}

