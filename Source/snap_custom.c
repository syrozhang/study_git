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

#include <string.h>
#include <time.h>
#include "general.h"
#include "sysg_api.h"
#include "snap_custom.h"
#include "snap_api.h"
#include "os_api.h"
#include "stdio.h"
#include "dcf_api.h"
#include "hw_front.h"
#include "dsc_init.h"
#include "aaa.h"
#include "osd_common.h"
#include "osd_api.h"
#include "ui_main.h"
#include "hw_sdram.h"
#include "utility.h"
#include "vfs_api.h"
#include "pub_api.h"
#include "hw_disp.h"
#include "hw_cdsp.h"
#include "hw_cpu.h"
#include "panel.h"
#include "xiaoao.h"
/*herb add image quality @03/12/08 start*/
#include "menu_capture.h"
#include "q_table.h"
/*herb add image quality @03/12/08 end*/
#include "aaa_api.h"

#include "aaa.h"

#include "gpio.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define CDSP_EDGE         0x00
#define CDSP_DENOISE      0x01
#define CDSP_EnLOWPASS    0x02
#define CDSP_BYPASS_EnD   0x03

#define REDO_CDSP_DENOISE      0x00

#define REDO_CDSP_EDGE         0x01

#define REDO_CDSP_EnLOWPASS    0x02

#define REDO_CDSP_BYPASS_EnD   0x03

#define SAVE_RAW 				0

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void ltoa (UINT32 val, UINT8 *pbuf, UINT32 radix, UINT32 digitno);
extern struct nandUIPara_s sPara;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void snapCDSPEdgeFilterSet(UINT32 mode);
void snapCDSPEnhance(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
const UINT8 customQTbl[] =
{
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
   
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
   1,   1,   1,   1,   1,   1,   1,   1, 
};
/*
const UINT8 customQTbl[] =
{
 80H Bytes 
	5,   3,   3,   5,   7,   12,  15,  18,
	4,   4,   4,   6,   8,   17,  18,  17,
	4,   4,   5,   7,   12,  17,  18,  17,
	4,   5,   7,   9,   15,  26,  24,  19,
	5,   7,   11,  17,  20,  33,  31,  23,
	7,   11,  17,  19,  24,  31,  34,  28,
	15,  19,  23,  26,  31,  36,  36,  30,
	22,  28,  29,  29,  34,  30,  31,  30,
	5,   5,   7,   14,  30,  30,  30,  30,
	5,   6,   8,   20,  30,  30,  30,  30,
	7,   8,   17,  30,  30,  30,  30,  30,
	14,  20,  30,  30,  30,  30,  30,  30,
	30,  30,  30,  30,  30,  30,  30,  30,
	30,  30,  30,  30,  30,  30,  30,  30,
	30,  30,  30,  30,  30,  30,  30,  30,
	30,  30,  30,  30,  30,  30,  30,  30
};
*/

extern UINT8 DCNameStr[];
/**************************************************************************
 *                                                                        *
 *  Function Name: snapOne()                                              *
 *                                                                        *
 *  Purposes: This is a entry from osd menu to snap a singlr image        *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    none :                                                              *
 *                                                                        *
 *                                                                        *
 *  Returns:                                                              *
 *    none :                                                              *
 *                                                                        *
 *  See also: snapMulti()                                                 *
 *                                                                        *
 **************************************************************************/
void
snapOne(
	void
)
{
	UINT32 err = SUCCESS;
	void *semApp;
	UINT8 snapOpt, shotNo;
	UINT32 aaaId;

	semApp = osEventFindByName("APP_SEM");
	aaaId = osTaskFindByName("AAA"); /* find 3A task */
	
	Q_Table = (UINT8*)QTIDX_Q100;	
	#if 0
	UINT32 zoomFactor = 100; /* 100 ~ 400 */
	sysgSnapScaleFactorSet(zoomFactor);
	#endif
	sysgSnapScaleFactorSet(pvZoomFactor);
	/*herb add image quality @03/12/08 start*/
	sysgQTableSet(PRIMARY_IMAGE, QTIDX_Q100, Q_Table);/**/ /* QTIDX_Q80, QTIDX_Q70, QTIDX_Q50*/
	/*sysgQTableSet(THUMBNAIL_IMAGE, QTIDX_Q95, 0);*/ /* QTIDX_Q80, QTIDX_Q70, QTIDX_Q50*/
	/*sysgQTableSet(PRIMARY_IMAGE, QTidx, 0);*/
	sysgQTableSet(THUMBNAIL_IMAGE, QTIDX_Q100, Q_Table);
	/*herb add image quality @03/12/08 end*/
	sysgSnapShotNoGet(&snapOpt, &shotNo);
	sysgSnapShotNoSet(snapOpt, 1);
	sysgSnapQVLenSet(0);/* Delay for Quick View. dly time = 10 * 10 ms */

	       /*Jane@040803*/
/*	hwCdspIntplModeSet(1);  1: 9X9 */        /* Fine mode */  
    /*hwCdspFineIntplCfg(1, 0x54, 0x320, 0x50);*/

	#if SAVE_RAW
	sysgSnapSaveRawImgSet(1);
	#endif
	/*printf("snap one ready\n");*/
	xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);	
	xdcfCurRootDirSet(imageRootNameStr);	
	/*	xdcfInit(imageRootNameStr, 0,xDCF_CONFIG_SORT_IDX);*/
 	if (aaaId <= 63) { /* Find AAA Task Id */
		hwFrontInterruptCfg(0, 0, 0); /* Before snap, VD interrupt should be stop
		to suspend 3A task that was waking up every 3 VD. Designer should manual
		resume 3A Task after snap is completed */
		aaaSnap(); /* Apply fine mode 3A */
	}
/*	printf("snapOpt =%d\n",snapOpt);*/
	if (0/*snapOpt == SNAP_S2S*/) {		/* sun@1102 mask sovle the snap issue */
		UINT32 availSlot = 0;

		snapRingBufCntGet(&availSlot);
		/*printf("snapOne, availSlot = %d\n", availSlot);*/
		while (availSlot--) {
	
		snap();
		snapPreviewSet();
		}
	}
	else 
	{
	/*printf("snap start\n");*/
		snap();
		osSemPend(semApp ,0, &err); /* wait */
	 	osSemPost(semApp);          /* free */
	 	snapPreviewSet();  		/*	chq add*/		
		/*printf("view reset\n");*/
	}

    if (aaaId <= 63) /* Enable VD if 3A task is existing */
    	hwFrontInterruptCfg(2, 0, 3); /* Enable VD interrupt
    	to resume 3A task every 3VDs */

}

bool poweronsnap = 0;

void
snapOne1(
	void
)
{
	UINT32 err = SUCCESS;
	void *semApp;
	UINT8 snapOpt, shotNo;
	UINT32 aaaId;

	if (poweronsnap) return;
	if (!gpioGet(GPIO_USB_DET)) return;
	semApp = osEventFindByName("APP_SEM");
	aaaId = osTaskFindByName("AAA"); /* find 3A task */

	sysgSnapScaleFactorSet(pvZoomFactor);
	/*herb add image quality @03/12/08 start*/
	/*sysgQTableSet(PRIMARY_IMAGE, QTIDX_Q95, 0);*/ /* QTIDX_Q80, QTIDX_Q70, QTIDX_Q50*/
	/*sysgQTableSet(THUMBNAIL_IMAGE, QTIDX_Q95, 0);*/ /* QTIDX_Q80, QTIDX_Q70, QTIDX_Q50*/
	sysgQTableSet(PRIMARY_IMAGE, QTidx, 0);
	sysgQTableSet(THUMBNAIL_IMAGE, QTidx, 0);
	/*herb add image quality @03/12/08 end*/
	sysgSnapShotNoGet(&snapOpt, &shotNo);
	sysgSnapShotNoSet(snapOpt, 0);
	sysgSnapQVLenSet(5); /* Delay for Quick View. dly time = 10 * 10 ms */

	xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);	
	xdcfCurRootDirSet(imageRootNameStr);
/*	xdcfInit(imageRootNameStr, 0,xDCF_CONFIG_SORT_IDX);*/
 	if (aaaId <= 63) { /* Find AAA Task Id */
		hwFrontInterruptCfg(0, 0, 0); /* Before snap, VD interrupt should be stop
		to suspend 3A task that was waking up every 3 VD. Designer should manual
		resume 3A Task after snap is completed */

		aaaSnap(); /* Apply fine mode 3A */
	}
		snap();
		osSemPend(semApp ,0, &err); /* wait */
	 	osSemPost(semApp);          /* free */
	 	snapPreviewSet();  			/*chq add*/


    if (aaaId <= 63) /* Enable VD if 3A task is existing */
    	hwFrontInterruptCfg(2, 0, 3); /* Enable VD interrupt
    	to resume 3A task every 3VDs */

}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapMulti()                                            *
 *                                                                        *
 *  Purposes: This is a entry to snap multiple image from osd snap menu   *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    none :                                                              *
 *                                                                        *
 *  Returns:                                                              *
 *    none :                                                              *
 *                                                                        *
 *  See also: snapOne()                                                   *
 *                                                                        *
 **************************************************************************/
void
snapMulti(
	void
)
{
	UINT32 err = SUCCESS;
	void *semApp;
	UINT8 snapOpt, shotNo;
	UINT32 aaaId;

	semApp = osEventFindByName("APP_SEM");
	aaaId = osTaskFindByName("AAA"); /* find 3A task */

	#if 0
	UINT32 zoomFactor = 100; /* 100 ~ 400 */
	sysgSnapScaleFactorSet(zoomFactor);
	#endif

	/*herb add image quality @03/12/08 start*/
	/*sysgQTableSet(PRIMARY_IMAGE, QTIDX_Q95, 0);*/ /* QTIDX_Q80, QTIDX_Q70, QTIDX_Q50*/
	/*sysgQTableSet(THUMBNAIL_IMAGE, QTIDX_Q95, 0);*/ /* QTIDX_Q80, QTIDX_Q70, QTIDX_Q50*/
	sysgQTableSet(PRIMARY_IMAGE, QTidx, 0);
	sysgQTableSet(THUMBNAIL_IMAGE, QTidx, 0);
	/*herb add image quality @03/12/08 end*/
	sysgSnapShotNoGet(&snapOpt, &shotNo);
	sysgSnapShotNoSet(snapOpt, 3);	/* sun@1005 */
	sysgSnapQVLenSet(5); /* Delay for Quick View. dly time = 10 * 10 ms */

	xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);	
	xdcfCurRootDirSet(imageRootNameStr);

 	if (aaaId <= 63) { /* Find AAA Task Id */
		hwFrontInterruptCfg(0, 0, 0); /* Before snap, VD interrupt should be stop
		to suspend 3A task that was waking up every 3 VD. Designer should manual
		resume 3A Task after snap is completed */

		aaaSnap(); /* Apply fine mode 3A */
	}

	snap();

	osSemPend(semApp , 0, &err); /* wait */
	osSemPost(semApp);           /* free */
	snapPreviewSet();  /* sun@1005 */
	if (aaaId <= 63) /* Enable VD if 3A task is existing */
    	hwFrontInterruptCfg(2, 0, 3); /* Enable VD interrupt
    	to resume 3A task every 3VDs */

}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapQTblCallback                                       *
 *                                                                        *
 *  Purposes: A call back function for customers to specify different     *
 *            QTable until the taget file size is reached.                *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    none :                                                              *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error Code                                                   *
 *                                                                        *
 *  See also: none                                                        *
 *                                                                        *
 **************************************************************************/
UINT32
snapQTblCallback(
	UINT32 shotpos
)
{
	UINT32 err = SUCCESS;
	/*UINT32 fileSize;*/
	UINT32 mainSize, thumbnailSize;
	/*jpgImageInfo_t mainInfo;
	jpgImageInfo_t thumbInfo;*/
	/*herb for image size limitation @03/12/15 start*/
	UINT32 flag = 0, index;
	UINT32 Temp_Qidx = QTidx;
	UINT8 *Temp_QTable = Q_Table;
	UINT32 size;
	UINT32 i=2;/**/

	/*printf("start compress QTidx=%x\n", QTidx);
	herb for image size limitation @03/12/15 end*/
	#if 0
	while () /* set your criteria here to determine how many times the */
	         /* compression should be applied */
	#endif
	/*for(i=0; i<31; i++)*/
	/*herb for image size limitation @03/12/15 start*/
	ImageSizeSelect(&size);
	i = QTidx;
	while (flag == 0)
	/*herb for image size limitation @03/12/15 end*/
	{
		if(i==0){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable100;
		}
		else if(i==1){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable99;
		}
		else if(i==2){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable98;
		}
		else if(i==3){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable97;
		}
		else if(i==4){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable96;
		}
		else if(i==5){
			QTidx = QTIDX_Q95;
			Q_Table = (UINT8*)QTable95;
		}
		else if(i==6){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable94;
		}
		else if(i==7){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable93;
		}
		else if(i==8){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable92;
		}
		else if(i==9){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable91;
		}
		else if(i==10){
			QTidx = QTIDX_Q90;
			Q_Table = (UINT8*)QTable90;
		}
		else if(i==11){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable89;
		}
		else if(i==12){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable88;
		}
		else if(i==13){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable87;
		}
		else if(i==14){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable86;
		}
		else if(i==15){
			QTidx = QTIDX_Q85;
			Q_Table = (UINT8*)QTable85;
		}
		else if(i==16){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable84;
		}
		else if(i==17){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable83;
		}
		else if(i==18){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable82;
		}
		else if(i==19){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable81;
		}
		else if(i==20){
			QTidx = QTIDX_Q80;
			Q_Table = (UINT8*)QTable80;
		}
		else if(i==21){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable79;
		}
		else if(i==22){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable78;
		}
		else if(i==23){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable77;
		}
		else if(i==24){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable76;
		}
		else if(i==25){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable75;
		}
		else if(i==26){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable74;
		}
		else if(i==27){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable73;
		}
		else if(i==28){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable72;
		}
		else if(i==29){
			QTidx = QTIDX_CUSTOM_RASTER;
			Q_Table = (UINT8*)QTable71;
		}
		else if(i==30){
			QTidx = QTIDX_Q70;
			Q_Table = (UINT8*)QTable70;
		}
		/*
		printf(" i=%d,indx=%x, Q_Table%d,table=%x\n", i,QTidx,100-i, Q_Table);
		UINT8 j ;
		for( j=0; j<128; j++){
			printf(" %d ", Q_Table[j]);

			if(((j!=0)&&((j+1)%8==0))||(j==127)){
				printf("\n");
			}
		} 
		*/
		#if 1	/* Test */
		/*herb add image quality @03/12/08 start*/
		if((QTidx == 0x03)||(QTidx == 0x04)||(QTidx == 0x05)||(QTidx == 0x06)||(QTidx == 0x07)||(QTidx == 0x08)){
			index = QTidx;
		}
		else{
			index = QTIDX_CUSTOM_RASTER;
		}

		/* set Primary and thumbnail Q Table, Constants are defined in sysg_api.h" */
		/*sysgQTableSet(PRIMARY_IMAGE, QTIDX_CUSTOM_RASTER, (UINT8*)customQTbl);*//* Primary Q Table*/
		/*sysgQTableSet(THUMBNAIL_IMAGE, QTIDX_Q85, 0);*//* Thumbnail  Q Table*/
		sysgQTableSet(PRIMARY_IMAGE, index, Q_Table);
		sysgQTableSet(THUMBNAIL_IMAGE, index, Q_Table);
		/*herb add image quality @03/12/08 end*/

		/* apply compression once */
		err = snapImgCompress(shotpos);

		/* Check if the file size is reached your target. If not, try */
		/* other set of QTable until the your ideal file size is reached */
		hwVlcSizeGet(PRIMARY_IMAGE, &mainSize);    /* Get main image VLC size */
		hwVlcSizeGet(THUMBNAIL_IMAGE, &thumbnailSize); /* Get thumbnail image VLC size*/
		/*snapFileSizeGet(shotpos, &mainInfo, &thumbInfo, &fileSize);*/ /*chq mark*/
		/*printf("Qindex=%x  mainSize = %d size = %d\n", QTidx, mainSize, size); */

		/*herb for image size limitation @03/12/15 start*/
		snapFileSizeControl(&flag, mainSize, size);
		#endif	/* Test */
		flag = 1;
		/*herb for image size limitation @03/12/15 end*/
		/*printf("  %d  filesize = %d\n", i, fileSize);*/
	}

	/*herb for image size limitation @03/12/15 start*/
	if((QTidx == 0x03)||(QTidx == 0x04)||(QTidx == 0x05)||(QTidx == 0x06)||(QTidx == 0x07)||(QTidx == 0x08)){
		index = QTidx;
	}
	else{
		index = QTIDX_CUSTOM_RASTER;
	}

	sysgQTableSet(PRIMARY_IMAGE, index, Q_Table);
	sysgQTableSet(THUMBNAIL_IMAGE, index, Q_Table);

/*	PlayEffectSound(0);*/
	err = snapImgCompress(shotpos);

	QTidx = Temp_Qidx;
	Q_Table = Temp_QTable;
	/*herb for image size limitation @03/12/15 end*/
	hwVlcSizeGet(PRIMARY_IMAGE, &mainSize);    /* Get main image VLC size */
	hwVlcSizeGet(THUMBNAIL_IMAGE, &thumbnailSize); /* Get thumbnail image VLC size*/
	/*snapFileSizeGet(shotpos, &mainInfo, &thumbInfo, &fileSize);
	printf(" end mainSize = %d\n", mainSize);
	if(mainSize%16384){
		printf(" end used cluster = %d\n", mainSize/16384+1);
	}
	else{
		printf(" end used cluster = %d\n", mainSize/16384);
	}*/
	/*while(1);*/
	return err;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapWriteFileCallback                                  *
 *                                                                        *
 *  Purposes: Progress bar call back function                             *
 *                                                                        *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    totalCnt : Total file size                                          *
 *    wroteCnt : Size already writte to file                              *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error Code                                                   *
 *                                                                        *
 *  See also: none                                                        *
 *                                                                        *
 **************************************************************************/
extern	UINT8 uiOnBusy;

UINT32
snapWriteFileCallback(
	UINT32 totalCnt,
	UINT32 wroteCnt
)
{
	UINT32 err = SUCCESS;
	UINT32	osdW, osdH;
	/* printf("totalCnt = %d, wroteCnt = %d\n", totalCnt, wroteCnt); */
	/* Draw progress bar here */
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	/*printf("Write File %d\n", wroteCnt);
	osdStrDisp(128, osdH-52, UserFont, 0x07, "Save...");*/
	osdHProgressDraw(0, totalCnt, wroteCnt, 0xA9);/**/
	uiOnBusy = 0;
	gpioSet(GPIO_BUSY_LED, 0); 		
	return err;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapDateStampCallback                                  *
 *                                                                        *
 *  Purposes:  Stamp date to image                                        *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    shotPos :                                                           *
 *    font color:                                                         *
 *    0: white                                                            *
 *    1: yellow                                                           *
 *    2: cyan                                                             *
 *    3: green                                                            *
 *    4: magenta                                                          *
 *    5: red                                                              *
 *    6: blue                                                             *
 *    7: black *  Returns: SUCCESS                                        *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
snapDateStampCallback(
	UINT32 rawAddr
)
{
	UINT32 err = SUCCESS;
	UINT32 freeAddr;
	UINT32 yuvW, yuvH;
	UINT32 phyOsdDBAddr, dspAddr;
	UINT8 dateStr[18];	/*AF modify @3-12-24 17:40*/
	UINT32 idx = 0;
	UINT8 *pImgBuf;
	UINT8 osdStr[2];
	UINT32 osdDBWidth, osdDBHeight;
	UINT32 osdFontWidth, osdFontHeight;
	UINT32 imageStampPosW, imageStampPosH;
	UINT32 totalStampW;
	UINT32 scaleFactor;
	struct tm *ptime;
	UINT8 imgBuf[0x200];
/*	UINT8 company[30]={'D','a','i','D','a','i','X','i','n','g',' ','T','E','C','.',',','E','l','e','c',' ','C','O','.',',','L','T','D','\0',};*/
	timeGet(&ptime);
	ltoa((UINT32)(ptime->tm_year + 1900), dateStr, 10, 4);
	strcat(dateStr, ".");
	ltoa((UINT32)ptime->tm_mon, &dateStr[5], 10, 2);
	strcat(dateStr, ".");
	ltoa((UINT32)ptime->tm_mday, &dateStr[8], 10, 2);
	/*AF add begin @ 3-12-24 17:41*/
	strcat(dateStr, ".");
	ltoa((UINT32)ptime->tm_hour, &dateStr[11], 10, 2);
	strcat(dateStr, ":");
	ltoa((UINT32)ptime->tm_min, &dateStr[14], 10, 2);
	strcat(dateStr, ":");
	ltoa((UINT32)ptime->tm_sec, &dateStr[17], 10, 2);
	/*AF add end @3-12-24 17:42 

	printf("Data Stamp\n");*/
	
	osdDBWidth = 16;
	osdDBHeight = 32;

	osdFontWidth = 16;
	osdFontHeight = 32;

	pImgBuf = (UINT8*) (((UINT32 )imgBuf) | 0xac000000);

	snapYuvImgSizeGet(&yuvW, &yuvH);

	sysgMemDispAddrGet(&dspAddr);
	sysgMemFreeAddrGet(APP_STILL_SNAP, &freeAddr);

	phyOsdDBAddr = (((UINT32)pImgBuf) & 0x03FFFFFF) >> 1;

	totalStampW = osdFontWidth * strlen(dateStr); /* date from RTC */

	scaleFactor = (yuvW >> 2) / totalStampW;
	if (scaleFactor < 1)
		scaleFactor = 1;

	totalStampW *= scaleFactor;
	if((yuvW >> 2) < totalStampW)/*chq modify in 640x480 mode imageStampPosW*/
		imageStampPosW = yuvW - totalStampW - osdFontWidth;
	else
	{
		imageStampPosW = yuvW - (totalStampW<<1) - (osdFontWidth<<1);
		scaleFactor = scaleFactor<<1;
	}
	imageStampPosH = yuvH - (yuvH / 10);
	hwDateFontScalerSet(scaleFactor); /* 1,2,3,4 */
	hwDateFontColorSet(DATE_STAMP_WHITE, 0x7FFE); /* 0x7FFE means color other than 0
	and 15 are treated as font color */
#if 0
	/*sun@1011 start */
	for (idx = 0; idx < strlen(company); idx++) {
		strncpy(osdStr, &company[idx], 1);
		osdStr[1] = '\0';
		osdStr2Img(osdStr, UserFont, pImgBuf);
		hwDateFontStampDo( /* stamp character one by one */
			phyOsdDBAddr,  /* starting of OSD database address */
			osdDBWidth,    /* width of OSD database */
			osdDBHeight,   /* height of OSD database */
			0,             /* horizonal offset in OSD database */
			0,             /* vertical offset in OSD database */
			osdFontWidth,  /* width of font */
			osdFontHeight, /* hight of font */
			rawAddr,       /* image starting address */
			yuvW,       /* image width */
			yuvH,       /* image height */
			yuvW-29*32+idx * osdDBWidth * scaleFactor, /* the
			x offset of the image to stamp */
			imageStampPosH-56,/* the y offset of the image to stamp */
			freeAddr       /* temp buffer */
		);
	}
	/*sun@1011 end */
	#endif
	/*AF modify begin@ 3-12-24 17:42*/
	if(sPara.stampFlag == StampDayTime)
	{
		/*printf("Write Data\n");*/
		for (idx = 0; idx < strlen(dateStr); idx++) {
			strncpy(osdStr, &dateStr[idx], 1);
			osdStr[1] = '\0';
			osdStr2Img(osdStr, UserFont, pImgBuf);
			hwDateFontStampDo( /* stamp character one by one */
				phyOsdDBAddr,  /* starting of OSD database address */
				osdDBWidth,    /* width of OSD database */
				osdDBHeight,   /* height of OSD database */
				0,             /* horizonal offset in OSD database */
				0,             /* vertical offset in OSD database */
				osdFontWidth,  /* width of font */
				osdFontHeight, /* hight of font */
				rawAddr,       /* image starting address */
				yuvW,       /* image width */
				yuvH,       /* image height */
				(imageStampPosW + idx * osdDBWidth * scaleFactor), /* the
				x offset of the image to stamp */
				imageStampPosH,/* the y offset of the image to stamp */
				freeAddr       /* temp buffer */
			);
 		}
 	}
#if 0		/* Paul@2005/11/05-13:47 mask */	
	else if(sPara.stampFlag == Max_StampFlag)
	{
		for (idx = 0; idx < (strlen(dateStr)-9); idx++) {
			strncpy(osdStr, &dateStr[idx], 1);
			osdStr[1] = '\0';
			osdStr2Img(osdStr, UserFont, pImgBuf);
			hwDateFontStampDo( /* stamp character one by one */
				phyOsdDBAddr,  /* starting of OSD database address */
				osdDBWidth,    /* width of OSD database */
				osdDBHeight,   /* height of OSD database */
				0,             /* horizonal offset in OSD database */
				0,             /* vertical offset in OSD database */
				osdFontWidth,  /* width of font */
				osdFontHeight, /* hight of font */
				rawAddr,       /* image starting address */
				yuvW,       /* image width */
				yuvH,       /* image height */
				(imageStampPosW+(9 * osdDBWidth * scaleFactor) + idx * osdDBWidth * scaleFactor), /* the
				x offset of the image to stamp */
				imageStampPosH,/* the y offset of the image to stamp */
				freeAddr       /* temp buffer */
			);
 		}
 	}
#endif	
	else
	{
		;
	}
	/*AF modify end @3-12-24 17:42*/
	return err;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapPhotoFrameCallback                                 *
 *                                                                        *
 *  Purposes:  Call back function photo frame                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    rawAddr :  the starting of YUV raw image buffer                     *
 *    imageWidth: the horizonal dimension of rawAddt                      *
 *    imageHeight: the vertical dimension of rawAddt                      *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
snapPhotoFrameCallback(
	UINT32 rawAddr,
	UINT32 imageWidth,
	UINT32 imageHeight
)
{
	UINT32 err = SUCCESS;
#if 0
	printf("Photo Frame\n");
 /* for reference only */
	SINT32 fd;
	UINT32 fileSize;
	UINT32 tempAddr;
	UINT32 vlcLogFreeAddr;
	UINT32 keyThreshold = 30;
	UINT32 QTIdx = QTIDX_Q85;
	UINT32 frameJPGWidth = 800;
	UINT32 frameJPGHeight = 200;
	UINT32 zoomFactor;
	UINT32 freeAddr;

	zoomFactor = imageWidth / frameJPGWidth;

	snapMemFreeBufferGet(&freeAddr);
 	vlcLogFreeAddr = freeAddr << 1 | 0xac000000;

	fd = vfsOpen("1.JPG", 0, 0);
	if (fd) {
		fileSize = vfsFileSizeGet(fd);
		vfsRead(fd, (UINT8*)vlcLogFreeAddr, fileSize);
		vfsClose(fd);
		tempAddr = freeAddr + (((fileSize + 1)/2 + 1024) >> 10 << 10);
		snapPhotoFrame(keyThreshold, rawAddr, frameJPGWidth, frameJPGHeight, \
			vlcLogFreeAddr,QTIdx, 0, tempAddr);
	}
 	rawAddr += (imageWidth * frameJPGHeight * zoomFactor);
	fd = vfsOpen("2.JPG", 0, 0);
	if (fd) {
		fileSize = vfsFileSizeGet(fd);
		vfsRead(fd, (UINT8*)vlcLogFreeAddr, fileSize);
		vfsClose(fd);
		tempAddr = freeAddr + (((fileSize + 1)/2 + 1024) >> 10 << 10);
		snapPhotoFrame(keyThreshold, rawAddr, frameJPGWidth, frameJPGHeight, \
			vlcLogFreeAddr, QTIdx, 0, tempAddr);
	}

 	rawAddr += (imageWidth * frameJPGHeight * zoomFactor);
	fd = vfsOpen("3.JPG", 0, 0);
	if (fd) {
		fileSize = vfsFileSizeGet(fd);
		vfsRead(fd, (UINT8*)vlcLogFreeAddr, fileSize);
		vfsClose(fd);
		tempAddr = freeAddr + (((fileSize + 1)/2 + 1024) >> 10 << 10);
		snapPhotoFrame(keyThreshold, rawAddr, frameJPGWidth, frameJPGHeight, \
			vlcLogFreeAddr, QTIdx, 0, tempAddr);
	}
#endif
	return err;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapAFQuickViewCallback                                *
 *                                                                        *
 *  Purposes:  Call back right after QuickView is shown                   *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    rawAddr :  the starting of YUV raw image buffer                     *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
snapAFQuickViewCallback(
	UINT32 rawAddr
)
{
	UINT32 err = SUCCESS;

	printf("Quick View\n");
	
	return err;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapClickFillCallback                                  *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    rawAddr :  the starting of YUV raw image buffer                     *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
snapClickFillCallback(
	void
)
{
	UINT32 dispW, dispH;
	UINT32 dspAddr;

	printf("Click Fill\n");
	hwDispPreviewSizeGet(&dispW, &dispH);
	sysgMemDispAddrGet(&dspAddr);

	pubFill((UINT8 *)(0xac000000 + dspAddr*2), dispW * 2, dispH,
		0, 0, dispW * 2, dispH, 0x000000);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapRGBEnhanceCallback                                 *
 *                                                                        *
 *  Purposes: 1. Control Color enhancement in RGB Domain after image is   *
 *            captured.                                                   *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *     rawAddr :                                                          *
 *     freeAddr:                                                          *
 *                                                                        *
 *  Returns: none                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
extern UINT8   flashmode;/*asaki add*/
extern UINT32 Ev7_Index,Ev8_Index,Ev9_Index,Ev10_Index,Ev11_Index,Ev12_Index,Max_Tidx,Min_Tidx,Ini_Tidx;
extern SINT32 PreTidx, Tidx;
extern UINT32 G_AvgLumi;
void
snaprgbEnhanceCallback(
	UINT32 rawAddr,
	UINT32 freeAddr
)
{
	cdspReg0_t *pcdspReg0;
	UINT32 sensorW, sensorH;
	UINT32 snapW, snapH;
	UINT32 cdspEnhanceMode;
	UINT8 saveScaleCtl;
	UINT8 saveScaleFact;

	pcdspReg0 = (cdspReg0_t *)(0xb0000000);

/*	printf("Rgb Enhance\n");*/
	
	snapCDSPEdgeFilterSet(0); /* 0:: fine mode, 1: fast mode */
	/* Disable OB/WB  here, if Ob/Wb are already done in hwFrontSnap */
	#if 1 /* off OB/WB */
		pcdspReg0 = (cdspReg0_t *)(0xb0000000);

		pcdspReg0->lenStep &= 0xEF;
	/*	hwCdspManuOBSet(1, 20);
		hwCdspWbOffsetSet(0, 0, 0, 0);*/
	/*	hwCdspWbOffsetSet(0x00, 0x0, 0x00, 0x0);chq add for flash AWB*/
		UINT32 R,GR,B,GB;
		hwCdspWbGainRead(&R, &GR, &B, &GB);	/**/					
		if((Tidx == Min_Tidx)&&(flashmode == FlashAuto))
		{	
			/*hwCdspWbGainSet(0x4e,0x40,0x4d,0x40);*/
			/*hwCdspWbGainSet(0x55, 0x40, 0x58, 0x40);*/
			if(G_AvgLumi <= 20)
				hwCdspWbGainSet(0x58, 0x40, 0x54, 0x40);
			else
				hwCdspWbGainSet(R-8, GR, B-6, GB);/**/

		} 
	#else /* enable and set OB/WB values */
		hwCdspManuOBSet(1, 0);                    /* just an example */
		hwCdspWbOffsetSet(0xFF, 0x02, 0xFC, 0x0); /* just an example */
	#endif

	sysgSensorSizeGet(&sensorW, &sensorH);
	sysgSnapSizeGet(&snapW, &snapH);
	/*hwCdspImgDo(sensorW, sensorH, 0);  RGB Bayer to YUV      */
	                                  /* 0: Do Edge            */
	                                  /* 1: Do Denoise         */
	                                  /* 2: Do Edge + Low Pass */
	/* Apply vscale if requested size is samller than sensor size to speed up 
 	capture. Note that if vscale is enable redo denoise and edge should be specifed 
 	applied at the time RGB is converted to YUV(hwCdspImgDo). */

	/* save scale control and factor value */
	saveScaleCtl = pcdspReg0->vScaleCtr; 
	saveScaleFact = pcdspReg0->vScaleFactor;

 	if (snapH < sensorH) {
		hwCdspVscaleSet(sensorH, snapH); /* enable v scale */
		cdspEnhanceMode = CDSP_BYPASS_EnD;/*REDO_CDSP_EnLOWPASS;*//*CDSP_BYPASS_EnD; *//* specify not to apply edge&denosie */
		snapCDSPEdgeFilterSet(1); /* set at fast mode to enable vertical scale */
	}
	else {
    	hwCdspVscaleSet(sensorH, sensorH);
		cdspEnhanceMode = CDSP_DENOISE;
	}
	
	/* 0x00:CDSP_EDGE       : Do edge                   */
	/* 0x01:CDSP_DENOISE    : Do de-noise               */
	/* 0x02:CDSP_EnLOWPASS  : Do edge + low pass        */                  
	/* 0x03:CDSP_BYPASS_EnD : By pass edge and de-noise                 
	printf("Into hwCdspImgDo!\n"); */ 

	hwCdspImgDo(sensorW, sensorH, cdspEnhanceMode);		
	/*printf("Out from hwCdspImgDo!\n");
	 restore  scale control and factor value */
	pcdspReg0->vScaleCtr = saveScaleCtl;
	pcdspReg0->vScaleFactor = saveScaleFact;                                 


}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapYUVEnhanceCallback                                 *
 *                                                                        *
 *  Purposes: 1. Redo Color enhancement                                   *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *     rawAddr :                                                          *
 *     freeAddr:                                                          *
 *                                                                        *
 *  Returns: none                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
snapyuvEnhanceCallback(
	UINT32 rawAddr,
	UINT32 freeAddr
)
{
	/*printf("YUV Enhance\n");*/
	snapCDSPEdgeFilterSet(0);
	snapCDSPEnhance();
	snapCDSPEdgeFilterSet(1); /* set edge filter back to fast mode */
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapCDSPEnhance                                        *
 *                                                                        *
 *  Purposes: 1. Redo Color enhancement                                   *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: none                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
snapCDSPEnhance(
	void
)
{
	UINT32 yuvW, yuvH;
	UINT32 freeAddr;
	UINT32 sensorW, sensorH;
	
	sysgSensorSizeGet(&sensorW, &sensorH);	
	snapMemFreeBufferGet(&freeAddr);
	hwCdspDenoiseCfg(8, 6); /* 8: Y denoise low threshold */
	                        /* 6: Y denoise bandwidth     */	                        
	snapYuvImgSizeGet(&yuvW, &yuvH);
	if (yuvH < sensorH) {
		/* Since denoise is not applied when hwCdspImgDo() is applied
		caller may need to redo denoise and edge in YUV domain */
		/* 0x00:REDO_CDSP_DENOISE    : Redo de-noise             */
		/* 0x01:REDO_CDSP_EDGE       : Redo edge                 */
		/* 0x02:REDO_CDSP_EnLOWPASS  : Redo edge + low pass      */ 

		hwCdspYUVRedo(yuvW, yuvH, REDO_CDSP_DENOISE, freeAddr);
		hwCdspYUVRedo(yuvW, yuvH, REDO_CDSP_EDGE, freeAddr);
	}
	else {
		/* De-noise is already applied in the case, only redo edge 
		is necessary */
		hwCdspYUVRedo(yuvW, yuvH, REDO_CDSP_EDGE, freeAddr);
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapCDSPFilterSet                                      *
 *                                                                        *
 *  Purposes: 1. Setting edge filter                                      *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    mode :                                                              *
 *         0: fine mode                                                   *
 *         1: fast mode                                                   *
 *                                                                        *
 *  Returns: none                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
snapCDSPEdgeFilterSet(
	UINT32 mode
)
{
	if (mode == 0) { /* fine mode */
		hwCdspEdgeFilterSet(
			EDGE_FINE_F00, EDGE_FINE_F01, EDGE_FINE_F02, EDGE_FINE_F03, \
			EDGE_FINE_F04, EDGE_FINE_F10, EDGE_FINE_F11, EDGE_FINE_F12, \
			EDGE_FINE_F13, EDGE_FINE_F14, EDGE_FINE_F20, EDGE_FINE_F21, \
			EDGE_FINE_F22, EDGE_FINE_F23, EDGE_FINE_F24, EDGE_FINE_F30, \
			EDGE_FINE_F31, EDGE_FINE_F32, EDGE_FINE_F33, EDGE_FINE_F34, \
			EDGE_FINE_F40, EDGE_FINE_F41, EDGE_FINE_F42, EDGE_FINE_F43, \
			EDGE_FINE_F44
	    );
	}
	else { /* fast mode  */
		hwCdspEdgeFilterSet(
			EDGE_FAST_F00, EDGE_FAST_F01, EDGE_FAST_F02, EDGE_FAST_F03, \
			EDGE_FAST_F04, EDGE_FINE_F10, EDGE_FINE_F11, EDGE_FINE_F12, \
			EDGE_FINE_F13, EDGE_FINE_F14, EDGE_FINE_F20, EDGE_FINE_F21, \
			EDGE_FINE_F22, EDGE_FINE_F23, EDGE_FINE_F24, EDGE_FINE_F30, \
			EDGE_FINE_F31, EDGE_FINE_F32, EDGE_FINE_F33, EDGE_FINE_F34, \
			EDGE_FINE_F40, EDGE_FINE_F41, EDGE_FINE_F42, EDGE_FINE_F43, \
			EDGE_FINE_F44
		);
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapQVLengthCallback                                   *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *     rawAddr :                                                          *
 *     freeAddr:                                                          *
 *                                                                        *
 *  Returns: none                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
snapQVLengthCallback(
	UINT32 timeElapse /* ms */
)
{
	UINT16 qvlength;
	UINT32 time;
	
	printf("QVLength = %d\n", timeElapse);
	
	sysgSnapQVLenGet(&qvlength);
	if (timeElapse < qvlength * 10) {
		time = (qvlength * 10) - timeElapse;
		if (time)
			hwWait(0, time);
	}
}

