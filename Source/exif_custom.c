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
#include <string.h>
#include "general.h"
#include "exif_api.h"
#include "exif_custom.h"
#include "aaa_api.h"
#include "snap_api.h"

extern UINT32 Tidx;
extern UINT32  pvEvCmpIdx ;
/**************************************************************************
 *                                                                        *
 *  Function Name: exifTagInsCallback                                     *
 *                                                                        *
 *  Purposes: A call back function to insert custom defined tags.         *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    none :                                                              *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error Code                                                   *
 *                                                                        *
 *  See also: exifInit, exifTagRmvCallback                                *
 *                                                                        *
 **************************************************************************/
UINT32 
exifTagInsCallback(
	void
)
{	
	IFD_t ifd;
	UINT8 buf[32];
	
	ifd.tag = 0x0131; /* Software version */
	ifd.type = 2;     /* IFD_DATATYPE_ASCII */
	#if Version268T_G1_E
	strcpy(buf, "F/W138");
	#else
	strcpy(buf, "F/W138");
	#endif
//	strcpy(buf, "F/W13080B3620Z0120");
	ifd.count = strlen(buf) + 1;
	exifTagInsert(IFD_TYPE_ZEROIFD, &ifd, (void*)buf);	
	
	ifd.tag = 0x010F; /* Make */
	ifd.type = 2;     /*IFD_DATATYPE_ASCII */
//	strcpy(buf, "SUNPLUS TECHNOLOGY CO.,  LTD.");
	strcpy(buf, "Fashion Digital");
	ifd.count = strlen(buf) + 1;
	exifTagInsert(IFD_TYPE_ZEROIFD, &ifd, (void*)buf);

	ifd.tag = 0x0110; /* Model */
	ifd.type = 2;     /*IFD_DATATYPE_ASCII */
	strcpy(buf, "SPCA536");
//	strcpy(buf, "DC308");
	ifd.count = strlen(buf) + 1;
	exifTagInsert(IFD_TYPE_ZEROIFD, &ifd, (void*)buf);
	
	#if 0
	exifAppnInsert(0xe4);
	#endif
	
	return SUCCESS;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: exifTagRmvCallback                                     *
 *                                                                        *
 *  Purposes: A call back function to remove defined tags.                *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    none :                                                              *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error Code                                                   *
 *                                                                        *
 *  See also: exifInit, exifTagInsCallback                                *
 *                                                                        *
 **************************************************************************/
UINT32 
exifTagRmvCallback(
	void
)
{	
	UINT32 err = 0;
		
	err = exifTagRemove(IFD_TYPE_EXIFIFD, EXIF_TAG_AUDIOFILE);/* Remove Audio Tag */	
	
	#ifndef AAA_EXIF_DEBUG
	err = exifTagRemove(IFD_TYPE_EXIFIFD, EXIF_TAG_USRCOMMENT);/* Remove comment Tag */	
	#endif
	
	return SUCCESS;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: exifTagSaveCallback                                    *
 *                                                                        *
 *  Purposes: A call back function to save custom exif tags before        *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    pcustomTag :                                                        *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error Code                                                   *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
exifTagSaveCallback(
	void **pcustomTag
)
{
	UINT32 err = SUCCESS;
	
	return err;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: exifTagModifyCallback                                  *
 *                                                                        *
 *  Purposes: A call back function to modify before each captured is      *
 *            about to save.                                              *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    pcustomTag                                                          *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error Code                                                   *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
exifTagModifyCallback(
	UINT32 idx,
	void *pcustomTag
)
{
	UINT32 err = SUCCESS;
	UINT8 buf[64];
	jpgImageInfo_t jpgQuickViewInfo;
	
	snapQVJPGImageInfoGet(idx, &jpgQuickViewInfo);

	if (pcustomTag) {
	}
	else {
		/* Modify Exposure Time */
		*(UINT32*)buf = 0x00000002;
		*(((UINT32*)buf)+ 1) = 0x00000024;
		err = exifTagModify(IFD_TYPE_EXIFIFD, EXIF_TAG_EXPTIME, buf);
		
		/* Modify Focal length */
		*(UINT32*)buf = 0x00000052;
		*(((UINT32*)buf)+ 1) = 0x0000000b;
		err = exifTagModify(IFD_TYPE_EXIFIFD, EXIF_TAG_FOCALLEN, buf);

		#if 0
		/* ISO Speed rate */
		*(UINT16*)buf = 0x00FF;
		err = exifTagModify(IFD_TYPE_EXIFIFD, EXIF_TAG_ISOSPEEDRATE, buf);

		/* Light Source */
		*(UINT16*)buf = 0x0003;
		err = exifTagModify(IFD_TYPE_EXIFIFD, EXIF_TAG_LIGHTSOURCE, buf);
		
		/* Flash */
		*(UINT16*)buf = 0x0001;
		err = exifTagModify(IFD_TYPE_EXIFIFD, EXIF_TAG_FLASH, buf);
		
		/* Main Image, x resolution */
		*(UINT32*)buf = 0x00000002;
		*(((UINT32*)buf)+ 1) = 0x00000004;
		err = exifTagModify(IFD_TYPE_ZEROIFD, IFD0_TAG_XRES, buf);
		
		/* Main Image , y resolution */
		*(UINT32*)buf = 0x00000002;
		*(((UINT32*)buf)+ 1) = 0x00000004;
		err = exifTagModify(IFD_TYPE_ZEROIFD, IFD0_TAG_YRES,buf);

		/* Main Image, Resolution Unit*/
		*(UINT16*)buf = 0x0001;
		err = exifTagModify(IFD_TYPE_ZEROIFD, IFD0_TAG_RESUNIT, buf);
		
		/* Main Image, orientation */
		*(UINT16*)buf = 0x0008;
		err = exifTagModify(IFD_TYPE_ZEROIFD, IFD0_TAG_ORIENT, buf);
		
		/* Exposure Program */
		*(UINT16*)buf = 0x0007;
		err = exifTagModify(IFD_TYPE_EXIFIFD, EXIF_TAG_EXPPROG, buf);
		#endif
		/* Modify F Number */
		*(UINT32*)buf = 0x00000014;
		*(((UINT32*)buf)+ 1) = 0x0000000a;
		err = exifTagModify(IFD_TYPE_EXIFIFD, EXIF_TAG_FNUM, buf);

		/* Modify Exposure Bias */
//		*(UINT32*)buf = aaaAeEvGet(Tidx);
		*(UINT32*)buf = 15-pvEvCmpIdx*3;
		*(((UINT32*)buf)+ 1) = 0x0000000a;
		err = exifTagModify(IFD_TYPE_EXIFIFD, EXIF_TAG_EXPBIAS, buf);

		#if 0
		exifAppnModify(0xe4, (UINT8*)jpgQuickViewInfo.vlcAddr, jpgQuickViewInfo.size);
		#endif
	}
	
	return err;
}
