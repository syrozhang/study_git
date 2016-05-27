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
 *  Author: Eric Su                                                    *
 *                                                                        *
 **************************************************************************/
//#define RS_232_DBG 1
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "sysregs.h"
#include "irq.h"
#include "zoom.h"
#include "timer.h"
#include "stdio.h"
#include "snap_api.h"
#include "pb_api.h"
#include "os_api.h"
#include "dcf_api.h"
#include "exif_api.h"
#include "exif_custom.h"
#include "usb_api.h"
#include "osd_common.h"
#include "osd_api.h"
#include "dsc_init.h"
#include "smc_api.h"
#include "hw_cdsp.h"
#include "hw_gpio.h"
#include "sysg_api.h"
#include "aaa_api.h"
#include "sysg_api.h"
#include "dpof_api.h"
#include "utility.h"
#include "badpix.h"
#include "mp3_api.h"
#include "audio_api.h"
#include "asf_api.h"
#include "pub_api.h"
#include "otg_api.h"
#include "panel.h"
#include "hw_disp_ext.h"
#include "hw_disp.h"
#include "hw_sdram.h"
#include "hw_cpu.h"
#include "misc_api.h"
#include "exif_api.h"
#include "sample_Code.h"


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

 /**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void sample_startupscreen(UINT32,UINT8 *);/*custom 3,2,1,0*/
UINT32 sample_pbImgPlay_Form_Flle(UINT32 filepos);
void sample_slideShowTest(UINT32 filepos);/*custom 4*/
void sample_ADCTest(void);        /*custom 5*/
void sample_RTCTest(void);        /*custom 6*/
void sample_ZoomPan(void);        /*custom 7*/ 
void sample_CardIsp(void);        /*custom 8*/
void sample_AccessFlashROM(void); /*custom 9*/ 
void sample_CardDetect(void);     /*custom 10*/ 

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 /**************************************************************************
 *                                                                        *
 *  Function Name: sample_Main                                             *
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
 sample_Main(
	UINT8* param
 )
 {	
 	UINT32 filter;
 	/* (*param) is a string*/
	filter = strtoul(param, 0, 10);
	printf("filter = %d\n",filter);
	
	switch(filter){
		case 0:
			/*start image part of code*/
			sample_startupscreen(0,0);
			break;
		case 1:
			nandRsvBlkWrite(5,sizeof(startup_pic)/512,startup_pic);
			break;
		case 2:
			/*start image from NAND RSV area*/
			sample_startupscreen(1,0);
			break;
		case 3:
			/*start image from file*/
			sample_startupscreen(2,0);
			break;
		case 4:
			/*Load pic into dram and slid show playback*/
			sample_slideShowTest(0);
			break;
		case 5:
			/*ADC test*/
			sample_ADCTest();
			break;
		case 6:
			/*RTC test*/
			sample_RTCTest();
			break;
		case 7:
			/*Zoom and Pan function*/
			sample_ZoomPan();
			break;
		case 8:
			/*CARD isp sample*/
			sample_CardIsp();
			break;
		case 9:
			/*read/write FLASH*/
			sample_AccessFlashROM();
			break;
		case 10:
			/*UI Suspend and Resume by GPIO0 intrrupt*/
			sample_SuspendResume();
			break;
		default :
			printf("Not support !\n");
			break;
	}		
 }
/**************************************************************************
 *                                                                        *
 *  Function Name: sample_startupscreen                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions: 0:image inside code
 *				  1:image inside storage	
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 void
 sample_startupscreen(
	UINT32 method,
	UINT8  *pfile
 )
 {
	UINT32 dispW, dispH;
	UINT32 dispAddr;
	UINT8 *allocBuf;

	switch(method){
		/* The iamge is part of the code */
		case 0:
			hwDispPreviewSizeGet(&dispW, &dispH);
			hwPlayBufSizeSet(dispW, dispH);
			sysgMemDispAddrGet(&dispAddr);	
			hwPlayBufAddrSet(dispAddr);
			hwFrmCnvModeSet(0);   				
			hwCammodeSet(5);      				
			hwCdspDataSource(1);
			pbDramImgPlay(startup_pic,sizeof(startup_pic),dispAddr,dispW,dispH);
			break;	
		/* The iamge is stored in the RSV of NAND */
		case 1:
			allocBuf = osMemAlloc(sizeof(startup_pic));
			if ( allocBuf == NULL ) {
				printf("allocBuf fail !");			
			}
			nandRsvBlkRead(5,sizeof(startup_pic)/512,allocBuf);
			hwDispPreviewSizeGet(&dispW, &dispH);
			hwPlayBufSizeSet(dispW, dispH);
			sysgMemDispAddrGet(&dispAddr);	
			hwPlayBufAddrSet(dispAddr);
			hwFrmCnvModeSet(0);   				
			hwCammodeSet(5);      				
			hwCdspDataSource(1);
			pbDramImgPlay(allocBuf,sizeof(startup_pic),dispAddr,dispW,dispH);
			osMemFree(allocBuf);
			break;	
		/* The iamge is a file */
		case 2:
			sample_pbImgPlay_Form_Flle(0);
			break;	
	}		
 }
/**************************************************************************
 *                                                                        *
 *  Function Name: Sample_pbImgPlay_Form_Flle                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions: 
 *				  	
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 sample_pbImgPlay_Form_Flle(UINT32 filepos)
{
	UINT32 err = SUCCESS;
	UINT32 displayPhyAddr = 0;	/* display buffer address */
	UINT32 decompAddr;		/* decompress buffer address */
	UINT32 dispW = 0, dispH = 0;
	xdcfAttrElm_t fileattr;		/* dcf file attribute */
	SINT32 fd = 0;
	UINT32 pbPhyVlcAddr;		/* Vlc buffer address */
	UINT32 pbLogVlcAddr;		/* Vlc buufer address */
	exifImageInfo_t *pInfo=0;		/* 壓縮檔的資訊 */
	exifAttrDesc_t *pattr;
	UINT32 fileSize;		/* 檔案大小 */
	
	/* 取得Display buffe */	
	sysgMemDispAddrGet(&displayPhyAddr);
	/* 目前取得的都會是320*240 */
	hwDispPreviewSizeGet(&dispW, &dispH);
	#if (PLAY_IMAGE_DEBUG_MSG==1)
	printf("preview width size is %d\n", dispW);	
	printf("preview high size is %d\n", dispH);
	#endif 
	/*hwPlayBufSizeSet(dispW, dispH);*/
	hwPlayBufSizeSet(dispW, dispH);
	hwPlayBufAddrSet(displayPhyAddr);
	hwFrmCnvModeSet(0); /* Disable frame rate conversion */
	hwCdspDataSource(1);/* CDSP Data source from DRAM */
	
	/* decode, decompress and scale img */
	
	/* 設定dcf的來源檔案 */
	if ((err = xdcfCurFileByPosSet(filepos)) != 0)
		return err;
	#if (PLAY_IMAGE_DEBUG_MSG==1)
	printf("file pos is %d \n", filepos);	
	#endif 	
	/* 取得檔案的屬性 */
	err = xdcfCurFileAttrGet(&fileattr);
	#if (PLAY_IMAGE_DEBUG_MSG==1)
 	printf("Decoding file %s, please wait.....\n\n", fileattr.name);
 	printf("file attribute hex value is %x \n",fileattr.attribute);
 	printf("file idx is hex value is %x \n",fileattr.idx);
 	printf("file time is %d:%d:%d:%d:%d:%d \n\n",fileattr.time.year,fileattr.time.mon,fileattr.time.day,fileattr.time.hour,fileattr.time.min,fileattr.time.sec);
 	#endif
 	
 	/* 開啟檔案 */
 	err = xdcfFileOpen(fileattr.name, 0, 0, &fd);
	
	/* 如果開啟檔案成功才執行 */
 	if (!err)
 	{
 		/* 取得Vlc buufer address */
		sysgMemAppAddrGet(&pbPhyVlcAddr);

		pbLogVlcAddr = (pbPhyVlcAddr << 1) | 0xac000000;
		/* 將讀到的jpeg檔案內容存至Vlc buffer */
		err = xdcfFileRead(fd, (UINT8*)pbLogVlcAddr, 0x00200000, &fileSize);
		xdcfFileClose(fd);

		err = exifMainDecode((UINT8*)pbLogVlcAddr, fileSize, &pattr);
		pInfo = &pattr->main;
		#if (PLAY_IMAGE_DEBUG_MSG==1)
		printf("****JPEG File content struct***\n");
		printf("pInfo.len=%d\n",pInfo->len);
		printf("pInfo.thmHdrOffset=%d\n",pInfo->thmHdrOffset);
		printf("pInfo.w=%d\n",pInfo->w);
		printf("pInfo.h=%d\n",pInfo->h);
		printf("pInfo.datafmt=%d\n",pInfo->datafmt);
		/*printf("pInfo.pYQT=%s\n",pInfo->pYQT);
		printf("pInfo.pUVQT=%s\n",pInfo->pUVQT);*/
		printf("pInfo.orient=%d\n",pInfo->orient);
		printf("pInfo.rstMCU=%d\n",pInfo->rstMCU);
		#endif 
 	}
 	/* 取得decompress buffer address */
 	sysgMemFreeAddrGet(APP_STILL_PB, &decompAddr);
	/*sysgMemPBDecompBufferGet(&decompAddr);*/
	
	/* decompress image retrived from pbImageDecode() */
	if ((err = pbImageDeCompress(pInfo, decompAddr)) != 0) {
		#if (PLAY_IMAGE_DEBUG_MSG==1)
		printf("Decompression failed\n");
		#endif
		return err;
	}
	
	/* scale the decompressed images to proper size for playback */
	err = pbImageScale(decompAddr, pInfo->w, pInfo->h, displayPhyAddr, dispW, dispH);
	return err;	
}	


typedef struct pbImgInfo{
	UINT8 *paddr;
	UINT32 fileSize;
} pbImgInfo_t;



/**************************************************************************
 *                                                                        *
 *  Function Name: sample_slideShowTest                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions: 
 *				  	
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void 
sample_slideShowTest(
	UINT32 filepos
)
{
	UINT32 err = SUCCESS;
	UINT32 jpgAddr, scaleAddr;
	xdcfAttrElm_t fileattr;
	UINT32 fileCnt, idx;
	pbImgInfo_t pbImgInfo[10];
	SINT32 fd;
	UINT32 dispW, dispH;
	
	/* Read file in the current folder to DRAM */
	pbMemVlcBufferGet(&jpgAddr);
	jpgAddr = (jpgAddr << 1) | 0xac000000;
	
	xdcfTotalFileCountGet(&fileCnt);
	
	idx = 0;	
	while (idx < fileCnt) {
		if ((err = xdcfCurFileByPosSet(idx + 1)) != 0)
			break;
		
		err = xdcfCurFileAttrGet(&fileattr);
		err = xdcfFileOpen(fileattr.name, 0, 0, &fd);
		err = xdcfFileRead(fd, (UINT8*)jpgAddr, xdcfFileSizeGet(fd), \
			&pbImgInfo[idx].fileSize);
		pbImgInfo[idx].paddr = (UINT8*)jpgAddr;
		jpgAddr += pbImgInfo[idx].fileSize;
			
		xdcfFileClose(fd);
		sysgMemFreeAddrSet(APP_STILL_PB, ((jpgAddr + 1) & 0x00FFFFFF) >> 1);
		idx++;
	}
	
	fileCnt = idx;
	
	/* Slide Show */
	sysgMemDispAddrGet(&scaleAddr); 
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW, dispH);
	hwPlayBufAddrSet(scaleAddr);
	hwFrmCnvModeSet(0); /* Disable frame rate conversion */
	hwCdspDataSource(1);/* CDSP Data source from DRAM */	
	idx = 0;
	while (idx < fileCnt) {
		pbDramImgPlay(
			pbImgInfo[idx].paddr,
			pbImgInfo[idx].fileSize, 
			scaleAddr, 
			dispW, 
			dispH
		);
		idx++;
		osTimeDly(50);
	}
	
}



/**************************************************************************
 *                                                                        *
 *  Function Name: Sample_ADCTest                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions: 
 *				  	
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void 
sample_ADCTest(
	void
)
{	
	hwAdcSet(ADC_ENABLE,ADC_CLK_4MHZ, 0x0b);
    printf("Set adc\n");
    printf("adcSet = %x\n",ADC_ENABLE);    
	printf("adckSel = %x\n",ADC_CLK_4MHZ);
    printf("adcFreq = %x\n",0x0b);

	while(1){
    	printf("C1 adc = %x\n",hwAdc1Data());
    	printf("C2 adc = %x\n",hwAdc2Data());
    	printf("C3 adc = %x\n",hwAdc3Data());
    	printf("C4 adc = %x\n",hwAdc4Data());
		osTimeDly(100);
	}	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: sample_RTCTest                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions: 
 *				  	
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void 
sample_RTCTest(
	void
)
{
 	struct tm RTC_Alarm;
	struct tm RTC_In;
	struct tm *RTC_Out;

	UINT16 temp = 0;
	
	hwRtcEn();/*Enable the RTC*/
	hwRtcReliableWrite(0xaa);/*write the reliable code*/
	printf("Set ReliableCode = %x\n",hwRtcReliableRead());/*check reliable code*/
	
	/*set the alarm time , 2003/4/10/13/1/1*/ 
	RTC_Alarm.tm_sec = 0;
	RTC_Alarm.tm_min = 3;
	RTC_Alarm.tm_hour   = 17;
	RTC_Alarm.tm_mday    = 10;
	RTC_Alarm.tm_mon  = 4;
	RTC_Alarm.tm_year   = 103;
	
	alarmSet(&RTC_Alarm);	
	hwRtcRegWrite(0xd0,0x02);/*enable alarm*/
	hwRtcRegWrite(0xc0,0x00);/*clear alarm*/
		 
	 
	RTC_In.tm_sec = 1;
	RTC_In.tm_min = 1;
	RTC_In.tm_hour   = 17;
	RTC_In.tm_mday    = 10;
	RTC_In.tm_mon  = 4;
	RTC_In.tm_year   = 103;
		
	timeSet(&RTC_In);

		
	while(1){
		timeGet(&RTC_Out);
	
		if(RTC_Out->tm_min != temp){
			printf(" ---> %d/%d/%d/%d/%d/%d\n",1900+RTC_Out->tm_year,RTC_Out->tm_mon,
			RTC_Out->tm_mday,RTC_Out->tm_hour,RTC_Out->tm_min,RTC_Out->tm_sec);
			temp = RTC_Out->tm_min;
			printf("Alarm = %x\n",hwRtcRegRead(0xc0));/*Check alarm*/
			printf("ReliableCode = %x\n",hwRtcReliableRead());/*check reliable code*/
		}

		if(hwRtcRegRead(0xc0)){
			RTC_Alarm.tm_min = RTC_Out->tm_min + 3;/*Every 3 minutes trigger alarm*/
			RTC_Alarm.tm_hour   = RTC_Out->tm_hour;
			RTC_Alarm.tm_mday    = RTC_Out->tm_mday;
			RTC_Alarm.tm_mon  = RTC_Out->tm_mon;
			RTC_Alarm.tm_year   = RTC_Out->tm_year;
			printf("Reset Alarm ---> %d/%d/%d/%d/%d/%d\n",1900+RTC_Alarm.tm_year,RTC_Alarm.tm_mon,
			RTC_Alarm.tm_mday,RTC_Alarm.tm_hour,RTC_Alarm.tm_min,RTC_Alarm.tm_sec);
			alarmSet(&RTC_Alarm);	
			hwRtcRegWrite(0xc0,0x00);/*clear alarm*/		
		}	

	}	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: sample_ZoomPan
                                             							  *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions: Do the zoom and pan function demo under playback mode
 *				  	
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void 
sample_ZoomPan
(
	void
)
{
	UINT32 zoom,pan;
	UINT32 width,hight,h,w;
	UINT32 th,tw;
	UINT32 loop=0;
	UINT32 dir = 1;/*move right or move left*/

	aaaAeawbModeSet(3);/*disable ae*/
	hwDispPreviewSizeGet(&width,&hight);
	hwDispImgSizeSet(width,hight);
	zoomCenterGet(&w,&h);
	
	printf("h = %d, w = %d\n",h,w);
	printf("width = %d, hight = %d\n",width,hight);
	while(loop < 1){
		/*zoom in*/
		for (zoom=2 ; zoom<9 ; zoom++){
			osTimeDly(50);
			zoomFactSet(zoom*100);
			osTimeDly(50);
			zoomCenterGet(&w,&h);
			printf("Zoom = %d\n",zoom);
			printf("h = %d, w = %d\n",h,w);
			printf("center x = %d, center y = %d\n",width/zoom,hight/zoom);
			 
			pan = 0;
			if(dir == 1){
				/*pan left*/
				printf("Pan Right\n");
				dir = 0;
				while(1){
					zoomCenterGet(&tw,&th);
					if(tw >= (width-(width/zoom)/2)-1)/*over the edge of left*/
						break;
					zoomCenterSet(w+pan,h);
					pan ++;
					osTimeDly(1);
				}  
			}
			else{	
				printf("Pan left\n");
				/*pan right*/
				dir = 1;
				while(1){
					zoomCenterGet(&tw,&th);
					if(tw <= (width/zoom)/2)/*over the edge of right*/
						break;
					zoomCenterSet(w-pan,h);
					pan ++;
					osTimeDly(1);
				} 
			}
		}
 
		/*zoom out*/
		for (zoom=8 ; zoom>1 ; zoom--){
			osTimeDly(50);
			zoomFactSet(zoom*100);
			osTimeDly(50);
			zoomCenterGet(&w,&h);
			printf("Zoom = %d\n",zoom);
			printf("h = %d, w = %d\n",h,w);
			printf("center x = %d, center y = %d\n",width/zoom,hight/zoom);
			pan = 0;
			if(dir == 1){
				/*pan left*/
				printf("Pan Right\n");
				dir = 0;
				while(1){
					zoomCenterGet(&tw,&th);
					if(tw >= (width-(width/(zoom-1))/2)-1)/*over the edge of left*/
						break;
					zoomCenterSet(w+pan,h);
					pan ++;
					osTimeDly(1);
				} 
			}
			else{	
				/*pan right*/
				printf("Pan left\n");
				dir = 1;
				while(1){
					zoomCenterGet(&tw,&th);
					if(tw <= (width/(zoom-1))/2)/*over the edge of right*/
						break;
					zoomCenterSet(w-pan,h);
					pan ++;
					osTimeDly(1);
				} 
			}
		}	
		loop ++;
	}
	zoomFactSet(100);/*without ZOOM*/
}
/**************************************************************************
 *                                                                        *
 *  Function Name: sample_CardIsp
                                             							  *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions: Do the isp from storage 
 *				  	
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void 
sample_CardIsp
(
	void
)
{
	UINT32 ststus = 0;
	/*ISP.BIN and SPCA536.BIN file is inside current folder*/
	ststus = cardIsp(0,0,0,0,"ISP.BIN","SPCA536.BIN");
	printf("ststus = %x\n",ststus);
}
/**************************************************************************
 *                                                                        *
 *  Function Name: sample_AccessFlashROM
                                             							  *
 *                                                                        *
 *  Purposes: Write/read some bytes from flash ROM(use ROM store data)
 *                                                                        *
 *  Descriptions: write 0xAA55AA55 into AT49LV040 flash(0x7fff0~0x7fff3) 
 *                and read back   
 *				  	
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void AccessFlashROM(UINT32) __attribute__ ((section (".ram_program")));

void 
sample_AccessFlashROM
(
	void
)
{
	/*Use AT49LV040 as sample*/	
	void (*pf)(UINT32);
	UINT32 readValue,value;
	UINT32 funcAddr,status;
	
	/*test data , save to DRAM first*/
	value = 0x55aa55aa;
	
	/*
	 * The purpose of the variable "funcAddr" is to confuse GCC.
	 * Without funcAddr = functionInRam and funcAddr = funcAddr | 0x80000000,
	 * GCC is too smart and find that it is identical to call functionInRam()
	 * directly.
	 */
	 
	ENTER_CRITICAL(status); 
	funcAddr = (UINT32 )AccessFlashROM;
	funcAddr = funcAddr | 0x8c000000;
	pf = (void(*)(UINT32))funcAddr;
	(*pf)(value);

	readValue = (UINT32)READ8(0xbfc00000 + 0x7fff3) & 0x000000ff;
	readValue |= (UINT32)READ8(0xbfc00000 + 0x7fff2) << 8;
	readValue |= (UINT32)READ8(0xbfc00000 + 0x7fff1) << 16;
	readValue |= (UINT32)READ8(0xbfc00000 + 0x7fff0) << 24;
	EXIT_CRITICAL(status); 

	printf("readValue = %x\n",readValue);
	
}


/* This routine AccessFlashROM() will load to RAM automatically*/
void 
AccessFlashROM
(
	UINT32 value
)
{
	UINT32 accessSize = 4;
	UINT32 ROMindex;
	UINT32 RAMindex;
	UINT8 romWdata[4];
	
	WRITE8(0xb0001044, 0x00);   /* Disable all Timer*/
	WRITE8(0xb0001043, 0x01);   /* Timer4 setting = 1ms*/
	WRITE32(0xb000103C, 0);     /* Timer4 value*/

	WRITE8(0xb0004001, 0x33);   /* ROM speed */
	WRITE8(0xb0001001, 0x30);   /* Reg r/w cycle */

	/* 
	 * If the user's device support block erase 
	 * Here is code for block erase , if not support
	 * block erase , the write area has to be 0xff
	 */
	#if 0
    /* block-Erase */
    WRITE8(0x80005555,0xaa);
    WRITE8(0x80002aaa,0x55);
    WRITE8(0x80005555,0x80);
    WRITE8(0x80005555,0xaa);
    WRITE8(0x80002aaa,0x55);
    WRITE8(0x80005555,0x10);

    /* Wait for chip ready */
    WRITE8(0xb0001044, 0x08);           /* Enable Timer 4*/
    while(READ32(0xb000103C) < 0x2710);  /* Wait for 10s */
 	WRITE8(0xb0001044, 0x00);           /* Disable all Timer*/
	#endif

	WRITE8(0xb0001043, 0x05);           /* Timer4 setting = 10us*/
	WRITE32(0xb000103C, 0);             /* Timer4 value*/
	RAMindex = 0;
	romWdata[0] = (UINT8)(value >> 24);
	romWdata[1] = (UINT8)(value >> 16);	
	romWdata[2] = (UINT8)(value >> 8;	
	romWdata[3] = (UINT8)(value);
	
    for(ROMindex = 0x7fff0;ROMindex < (0x7fff0+accessSize);ROMindex++) {	
		
        WRITE8(0x80005555,0xaa);
        WRITE8(0x8000aaaa,0x55);
        WRITE8(0x80005555,0xa0);
        WRITE8(0x80000000 + ROMindex,romWdata[RAMindex]);

		RAMindex ++;
		
    	WRITE8(0xb0001044, 0x08);           /* Enable Timer 4*/
        while(READ8(0xb000103C) < 0x05);    /* Wait for 50us */
 	    WRITE8(0xb0001044, 0x00);           /* Disable all Timer*/
	    WRITE32(0xb000103C, 0);             /* Timer4 value*/
    }
}


/**************************************************************************
 *                                                                        *
 *  Function Name: sample_SuspendResume                                   *
 *                                                                        *
 *  Purposes: Enter UI suspend abd Resuem by GPIO0 interrupt
 *                                                                        *
 *  Descriptions:    
 *				  	
 *                                                                        *
 *  Arguments:                                                           *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
sample_SuspendResume(
 	void
)
{
	UINT32 status;
	
		/* Enable UI resume state */
		hwUiResumeEnableSet(1);
		/* Clear UI resume flag in case this flag is set */
		hwUiResumeFlagSet(0);

		/* Enable GPIO 0 as resume interrupt source, high and low trigger */
		hwGenGpioIrqInit();
		hwGenGpioIrqReg(0,NULL,0);
		hwGenGpioIrqReg(0,NULL,1);
		/* 
		 * Disable all interrupt source 
		 * User has to carefully aware all interrupt source
		 * under running, maybe it not only below's interrupt 
		*/
		WRITE8(0xB00010d0,0x00);
		WRITE8(0xB00010d1,0x00);	
		WRITE8(0xB00010d2,0x00);	
		WRITE8(0xB00010d3,0x00);	
		WRITE8(0xB00091d0,0x00);
		WRITE8(0xB00091c0,0x00);
		WRITE8(0xB00010c0,0x00);
		WRITE8(0xB00010c1,0x00);	
		WRITE8(0xB00010c2,0x00);	
		WRITE8(0xB00010c3,0x00);

		ENTER_CRITICAL(status);
		/* 
		 * Default set DRAM under self-reflsh 
		 * But for firmware code boot from Nand, 
		 * the DRAM can't set to self-refresh 
		 */
		hwSwSuspendEnableSet(0,1);
		
		/*The system Sleep Here */
		
		hwUiResumeEnableSet(0);
		hwUiResumeFlagSet(0);
	
		/* 
		 * Re-enable all user's interrupt source again 
		*/
		WRITE8(0xB00010d0,0x10);
		WRITE8(0xB00010d1,0x00);	
		WRITE8(0xB00010d2,0x02);	
		WRITE8(0xB00010d3,0x00);	
		WRITE8(0xB00091d0,0x02);
		EXIT_CRITICAL(status);
}
/**************************************************************************
 *                                                                        *
 *  Function Name: hwSwSuspendEnableSet                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:  	Reset
 *						0:resume from suspend
 *						1:resume from reset
 *					SelfRefresh
 *						0:DRAM normal
 *						1:DRAM refresh
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
hwSwSuspendEnableSet(
	UINT32 Reset,
	UINT32 SelfRefresh
)
{}
/**************************************************************************
 *                                                                        *
 *  Function Name: hwUiResumeEnableSet                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:  	Enable
 * 						0:disable UI resume
 *						1:Eable UI resume
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
hwUiResumeEnableSet(
	UINT32 Enable
)
{}
/**************************************************************************
 *                                                                        *
 *  Function Name: hwUiResumeFlagSet                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:  flagSet 
 *						0: clear the flag
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:	   The flag ststus before flag set                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
hwUiResumeFlagSet(
	UINT32 flagSet
)
{}


