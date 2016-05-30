/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2003 by Worldplus Technology Co., Ltd.           *
 *                                                                        *
 *  This software is copyrighted by and is the property of Worldplus      *
 *  Technology Co., Ltd. All rights are reserved by Worldplus Technology  *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Worldplus Technology Co., Ltd.                     *
 *                                                                        *
 *  Worldplus Technology Co., Ltd. reserves the right to modify this      *
 *  software without notice.                                              *
 *                                                                        *
 *  Worldplus Technology Co., Ltd.                                        *
 *  2nd Floor, R2-B Buliding, Hi-tech.S, 7th Road,                        *
 *  High-tech Industrial Park, Shenzhen, China                            *
 *                                                                        *
 *  Author: Wallace Zhang                                                 *
 *                                                                        *
 **************************************************************************/
#include <stdio.h>
#include "general.h"
#include "dpof_api.h"
#include "dcf_api.h"
#include "errcode.h"

/**************************************************************************************
*                                                                                     *
*  Function Name : dpofCurFileSet                                                     *
*                                                                                     *
*  Purposes      :                                                                    *
*    To set the current file job section                                              *
*                                                                                     *
*  Desprictions  :                                                                    *
*     					                                              *
*  Arguments     :                                                                    *
*                                                                                     *
*  Returns       :                                                                    *
*    no return message                                                                *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32 dpofCurFileSet(UINT16 *numPrints){
	
	UINT32 err	= SUCCESS;
	UINT8  dpofImgfmt = DPOF_IMGFMT_EXIF2_J;
	UINT8  rot = DPOF_ROT_NONE;
	UINT16 trm[4]={0, 0, 640, 480};
	UINT8  dtmOpt = 1;
	UINT32 pos, filetype;
	
	xdcfCurFilePosGet(&pos);
	xdcfFileTypeGet(pos, &filetype);	/*get current file type*/
	if(filetype !=	xDCF_FILETYPE_JPG){
		return err = LIBDPOF_PARSE_ERR0003;
	}
	
	if(dpofJobQuery(DPOF_PRINT_MODE_STD)==0){		/*if job section exist*/
		printf("job section exist\n");
			
		if(*numPrints!=0){
			err = dpofJobAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_PRT_QTY, (void *)numPrints);	/*set print number*/
			printf("attr change err=%x\n", err);
		}
		else{
			err = dpofJobDelete(DPOF_PRINT_MODE_STD);	/*delete job section*/
			printf("delete job, err=%x\n", err);
		}
			
		return err;
	}
		
	
	if((*numPrints)!=0){
		printf("create new job section\n");
		
		err = dpofJobAdd(DPOF_PRINT_MODE_STD);	/*create a new job section*/
		printf("job add err=%x\n", err);
		err = dpofJobAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_PRT_QTY, (void *)numPrints);	/*set print number*/
		err = dpofJobAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_IMG_FMT, (void *)&dpofImgfmt);	
		err = dpofJobAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_CFG_ROT, (void *)&rot);
          	err = dpofJobAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_CFG_TRM, (void *)&trm);
          	err = dpofJobAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_CFG_DSC_AD, (void *)&dtmOpt);
		printf("err=%x\n", err);
	}
	
	return err;	
}

/**************************************************************************************
*                                                                                     *
*  Function Name : dpofAllFileSet                                                     *
*                                                                                     *
*  Purposes      :                                                                    *
*    To set all files' job section                                                    *
*                                                                                     *
*  Desprictions  :                                                                    *
*     					                                              *
*  Arguments     :                                                                    *
*                                                                                     *
*  Returns       :                                                                    *
*    no return message                                                                *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32 dpofAllFileSet(UINT16 *numPrints){
	
	UINT32 err	= SUCCESS;
	UINT32 dirpos, filepos;
				
	xdcfCurDirPosGet(&dirpos);	/*store current dir & file position*/
	xdcfCurFilePosGet(&filepos);
	printf("filepos=%x, dirpos=%x\n", filepos, dirpos);
	
	err = xdcfCurDirByDirectSet(xDCF_MOVE_FIRST);	/*start from the first directory*/
	printf("move to first dir err = %x\n", err);
	
	while(err == SUCCESS){
		
		err = xdcfCurFileByDirectSet(xDCF_MOVE_FIRST);	/*start from the first file*/
		printf("move to first file err = %x\n", err);
		
		while(err == SUCCESS){
			
			err = dpofCurFileSet(numPrints);	/*set current file to protect*/
			
			err = xdcfCurFileByDirectSet(xDCF_MOVE_NEXT);	/*move to next file*/
			printf("move to next file err = %x\n", err);
		}
		
		err = xdcfCurDirByDirectSet(xDCF_MOVE_NEXT);	/*move to the next directory*/
		printf("move to next dir err = %x\n", err);
	}
	
	xdcfCurDirByPosSet(dirpos);
	xdcfCurFileByPosSet(filepos);
	
	return err;	
}

