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
 *  2nd Floor,R2-B,Hi-tech.S,7th Road, Shenzhen High-tech Industrial Park *
 *  Shenzhen, China                                                       *
 *                                                                        *
 *  Author: Joe Zhao                                                      *
 *                                                                        *
 **************************************************************************/
//#define RS_232_DBG		//Paul@2005/10/08 disable debug text

#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "stdio.h"
#include "ui_main.h"
#include "hw_cpu.h"

#include "avi_api.h"
#include "asf_api.h"

#include "dcf_api.h"
#include "snap_api.h"
#include "pb_api.h"
#include "card.h"
#include "os_api.h"
#include "hw_cammode.h"
#include "hw_cdsp.h"
#include "hw_disp.h"
#include "timer.h"
#include "hw_front.h"
#include "hw_sdram.h"
#include "dsc_init.h"
#include "stdio.h"
#include "vfs_api.h"
#include "usb_api.h"
#include "sysg_api.h"
#include "audio_api.h"
#include "hw_disp_ext.h"
#include "osd_common.h"
#include "osd_main.h"
#include "osd_api.h"
#include "aaa_api.h"
#include "panel.h"
#include "exif_custom.h"
#include "snap_custom.h"
#include "zoom.h"
#include "xiaoao.h"
#include "errcode.h"
#include "hw_aud.h"
#include "gpio.h"
#include "tv_ov.h"
#include "extern.h"
#include "ws9712.h"
#include "uistrings.h"
#include "menu_capture.h"

#define CLIP_NOW	0x01
#define PAUSE_NOW	0x02

extern void wm8751RegSet(UINT32,UINT32);
extern void panelSelect(UINT32);
extern void DisplayZoomIcon(UINT16 flag, UINT32 value);
extern bool selfTimeDownCount(UINT8 ui8TimeIndex);
extern void movieSpecialEffectSet(UINT8 mode);
extern void WriteLcdReg(UINT8 ui8RegAddr, UINT8 ui8Data);	/* sun@060119 */

UINT32 VideoClipPlaySet(void);
void DisplayMVIcon(UINT8 isClear);
void dv_Playback(void);

UINT8  ui8DownTimeMvIdx = 3;
UINT32 vcZoomFactor = 100;
UINT32 vcEvCmpIdx = 0x06;
UINT16 ui16SnapCount = 0;	/* Paul@2005/10/02 */

UINT32 ui32MovieType = xDCF_FILETYPE_ASF;	/* Paul@2005/11/23-08:33 add */
UINT32 ui32MvFrameRate = 15;
UINT32 memoryflag;
	
static UINT8 pauseFlag = CLIP_NOW;	

extern OS_EVENT* uiKeyMsgQ;
extern OS_EVENT* semApp;
extern UINT8 lcdOsdDisp;
extern UINT8 SpeakFlag;
extern UINT8 TVFlag;
extern UINT8 SpeakFlagMode;
extern UINT32 ChgPvSetting;
extern OS_EVENT* semApp;
extern UINT8 g_playrecStatus;
extern UINT8 ui8DownTime[];
extern UINT32 onMovieMenu;

/**************************************************************************
 *                                                                        *
 *  Function Name: uiPreviewMovie                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *    function for Preview Movie mode UI process                          *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: This function should never return.                           *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
uiPreviewMovie(
	void
)
{
	UINT32 key = UI_KEY_MODE_MOVIE;
	UINT32 err;
	UINT32 *msg;
	UINT32	osdW, osdH;
	UINT32 i;			/* Paul@2005/10/12 */
	UINT8 str8[3];
	
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;	

	#ifdef	OSD_DEBUG
	printf("\nVideo\n");
	#endif
    aaaInit();		
	
	uiKeyMsgQ = osEventFindByName("UI_KEY");
	isMenuOn = 0;
	vcZoomFactor = 100;

	videoQTidxInit();
	sPara.snapFlag = SnapSingle;

	/* Paul@2005/09/30 */
	gpioSet(GPIO_SENSOR_PWDN, 0);	
	gpioSet(GPIO_SENSOR_RST, 0);				
	WRITE8(0xb0000007,0x7f);
	gpioSet(GPIO_SENSOR_POWER, 1);
//	hwDispOsdEn(0);
//	hwDispPvEn(0);
	hwWait(0, 8);
	hwTvSensorModeSel(0);
					
	while ( ( uiState & UI_MODE_MASK ) == UI_KEY_MODE_MOVIE )
	{
		if (isMenuOn)
		{
			menuMovieProc(key);
			if (isMenuOn == 0)
			{
				key = UI_KEY_MODE_MOVIE;
			}
		}
		
		if (ui32NextState != uiState)
		{
			break;
		}					
		
		if (isMenuOn == 0)
		{
			switch(key)
			{
				case UI_KEY_FUNC_OK:	
					lcdOsdDisp = 2;
					osdLcdDisp(5);	/* Paul@2005/10/03 */
					/* Paul@2005/10/27-10.00AM */
					menuEffectProcInit();
					key = UI_KEY_MODE_MASK;
					while ((uiState & UI_MODE_MASK) == UI_KEY_MODE_MOVIE)
					{	
						if (menuEffectProc(key))
						{
							break;
						}
						keyGet(&key);												
					}
					/* Paul@2005/10/27-10.00AM */
					break;
					
				case UI_KEY_DIR_DOWN:					
					if (sPara.snapFlag == SnapSelf)
					{	/* Paul@2005/11/17_14:12 add */
						sPara.snapFlag = SnapSingle;
					}
					else
					{
						sPara.snapFlag = SnapSelf;
					}
					DisplayMVIcon(0);
					break;
					
				case UI_KEY_DIR_UP:
					lcdOsdDisp = 2;
					osdLcdDisp(5);	/* Paul@2005/10/03 */
					dv_Playback();					
					key = UI_KEY_MODE_MOVIE;
					continue;
					break;

				case UI_KEY_DIR_LEFT :
					do
					{
						if (vcZoomFactor > 100)
						{
							vcZoomFactor -= 5;
							hwPVZoom(vcZoomFactor);
							sysgSnapScaleFactorSet(vcZoomFactor);													
						}
						DisplayZoomIcon(1, vcZoomFactor);
						osTimeDly(2);						
						msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);
					}while((uiKeyPush) && (!((*msg != UI_KEY_DIR_LEFT) && (err==0))));
					DisplayZoomIcon(0, vcZoomFactor);

					break;

				case UI_KEY_DIR_RIGHT :
					do
					{
						if (vcZoomFactor < 400)
						{
							vcZoomFactor += 5;
							hwPVZoom(vcZoomFactor);
							sysgSnapScaleFactorSet(vcZoomFactor);													
						}
						DisplayZoomIcon(1, vcZoomFactor);
						osTimeDly(2);						
						msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);
					}while((uiKeyPush) && (!((*msg != UI_KEY_DIR_RIGHT) && (err==0))));
					DisplayZoomIcon(0, vcZoomFactor);
					break;

				case UI_KEY_FUNC_MENU :
					menuReturn(UI_MAINMENU, 0);
					break;
					
				case UI_KEY_FUNC_B :
					osTaskSuspend(osTaskFindByName("HISTOGRAM"));		
							
					vcZoomFactor = 100 ;
					hwPVZoom(vcZoomFactor);
						//yyq 05-12-12
					lcdOsdDisp = 2;
					osdLcdDisp(5);	/* Paul@2005/10/03 */
					menuMovieInit(key);
					break;		

				case UI_KEY_FUNC_A:
					printf("Start snap movie\n");
					if(snapAV == OBJ_NONE)
					{
						#if 0
						/* turn off OSD to fix the noise when record 640x480 using TV */
						if (VideoClipPlaySet())
						{	/* Paul@2005/11/05-16:17 add */
							break;
						}
						#endif

						if (sPara.snapFlag == SnapSelf)
						{
							sPara.snapFlag = SnapSingle;								
							if (selfTimeDownCount(ui8DownTimeMvIdx) == false)
							{
								break;
							}
							/* Paul@2005/11/17_14:12 mask */							
						}						
						scene_ASFClip();
					}
					else
					{
						#ifdef	OSD_DEBUG
						printf("\n abort asf...");
						#endif
						asfAppClipAbort(); /* stop clipping	*/
						if (ChgPvSetting)
						{
							hwDispPvSetWrite(imgBuf);
							ChgPvSetting = 0;
						}						/* turn on OSD to fix the noise when record 640x480 using TV */
						#if 1
						hwDispOsdEn(1);
						hwDispRegUpd(0x01);
						printf("turn on osd\n");
						#endif
						snapAV	= OBJ_NONE;
					}					
					key = UI_KEY_MODE_MOVIE;
					continue;
					break;					

				case UI_KEY_MODE_MOVIE:
					hwDispPvEn(0);
					osTaskResume(osTaskFindByName("HISTOGRAM"));
					osTaskResume(osTaskFindByName("AAA"));
					osdClearScreen(0);										

					lcdOsdDisp = 2;
					osdLcdDisp(5);
					hwCdspManuOBSet(1, 5);
					
					PNL_PreviewSet();	
					
					movieSpecialEffectSet(sPara.EffectFlag);					
					
					/* hwDispPipEn(0); */
					zoomFactSet(zoomFactGet());
					hwPVZoom(vcZoomFactor);
					aaaEvCmpIdxSet(vcEvCmpIdx);
					
					err	= xdcfInit(imageDirNameStr, DVNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
					xdcfCurRootDirSet(imageRootNameStr);					
					err	= xdcfFilterSet(xDCF_FILTER_JPG | xDCF_FILTER_ASF | xDCF_FILTER_WAV | xDCF_FILTER_OURTYPE);
					err = exifInit(exifTagInsCallback, exifTagRmvCallback);
					
					DisplayMVIcon(1);

					hwDispOsdEn(1);
					hwDispPvEn(1);
					break;

				default:
					#ifdef	OSD_DEBUG
					printf("\nKEY=%0x ", key);
					#endif
					break;
			}
		}
		
		if (((uiState & UI_MODE_MASK ) != UI_KEY_MODE_MOVIE))
		{
			break;
		}				
		
		keyGet(&key);
	}	
	vcZoomFactor = 100;		
	hwPVZoom(vcZoomFactor);
	
	osTaskSuspend(osTaskFindByName("AAA"));		
	lcdOsdDisp = 2;
	osdLcdDisp(5);	/* Paul@2005/10/03 */					

	PNL_PlaybackSet();	
	
	hwDispPvEn(0);
	gpioSet(GPIO_SENSOR_POWER, 0);
	hwWait(0, 10);
//	gpioSet(GPIO_SEN_STANDBY, 0);	

	zoomFactSet(100);				/* Paul@2005/11/07--15:50 add */		

	uiState = ui32NextState;		/* Paul@2006/02/18-15:12 add */
	
	return;
}


/**
	Paul@2005/11/05-16:04 add

	return : 0 = success ;
			 1 = fail    ;	
**/
UINT32 VideoClipPlaySet(void)
{
//	UINT8 asfFilename[13];/* after and include 1.8.0, name array must at least 13 */
	/*UINT32 autFuncID = UNKNOWN;*/
	UINT8  mid;
	UINT8 devid;
//	UINT32 *msg = 0;
//	UINT32 err = SUCCESS;
//	UINT32 sts;
	UINT32 freeMem;
	devInfo_t pdevInfo;
	audProf_t *paudProf;
	audProf_t audProf;

//	paudProf = audProfGet(2);
//	audProf = *paudProf;
//	audProf.hwAdcResetCnt = 0x02;
//	audHwCfgCal(&audProf);

	if((isCardExist()) && (sPara.StorageModeFlag == Card))
	{
		vfsDevInfoGet(DRIVE_SD, 0, &pdevInfo);
	}
	else
	{
		vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfo);
	}
	
    mid = pdevInfo.makerId;
    devid = pdevInfo.hwDevId; 
	freeMem = pdevInfo.nrFreeBytes;
	if(freeMem < 1024*100)
	{
		return 1;
	}
	/* turn off OSD to fix the noise when record 640x480 using TV */
	/*	hwCdspDataSource(0);   CDSP Data source from Sensor	*/
	#if 0
	hwDispOsdEn(0);
	hwDispRegUpd(0x0D);/*chq mask*/
	#endif	

    if(devid==117)
	{
		if (ui32MovieType == xDCF_FILETYPE_ASF)
		{
			asfAppResolutionSet(320, 240);
			if(sPara.ImageQuality == QualitySuper)
				asfAppBitRateCtrlEn(1, 1444000); 
				//asfAppBitRateCtrlEn(1, 1000000); 
			else if(sPara.ImageQuality == QualityFine)
				asfAppBitRateCtrlEn(1, 1000000); 
				//asfAppBitRateCtrlEn(1, 900000);
			else if(sPara.ImageQuality == QualityNormal)
				asfAppBitRateCtrlEn(1, 700000);
				//asfAppBitRateCtrlEn(1, 700000);
			else 
				asfAppBitRateCtrlEn(1, 700000); 			
		}
	}
	else if(devid==115)
	{
		if (ui32MovieType == xDCF_FILETYPE_ASF)
		{
			asfAppResolutionSet(320, 240);
			if(sPara.ImageQuality == QualitySuper)
				//asfAppBitRateCtrlEn(1, 1444000); 
				asfAppBitRateCtrlEn(1, 900000);
			else if(sPara.ImageQuality == QualityFine)
				//asfAppBitRateCtrlEn(1, 1000000); 
				asfAppBitRateCtrlEn(1, 700000);
			else if(sPara.ImageQuality == QualityNormal)
				//asfAppBitRateCtrlEn(1, 700000); 
				asfAppBitRateCtrlEn(1, 600000);
			else 
				asfAppBitRateCtrlEn(1, 1000000); 				
		}
	}
    else
	{				
		if (ui32MovieType == xDCF_FILETYPE_ASF)				
		{
			asfAppResolutionSet(320, 240);
			if(sPara.ImageQuality == QualitySuper)
				//asfAppBitRateCtrlEn(1, 1444000); 
				asfAppBitRateCtrlEn(1, 1000000); 
			else if(sPara.ImageQuality == QualityFine)
				//asfAppBitRateCtrlEn(1, 1000000); 
				asfAppBitRateCtrlEn(1, 900000);
			else if(sPara.ImageQuality == QualityNormal)
				//asfAppBitRateCtrlEn(1, 700000); 
				asfAppBitRateCtrlEn(1, 700000);
			else 
				asfAppBitRateCtrlEn(1, 700000); 			
		}
	}

	return 0;	
}

/**
**/
UINT32 RecordLengthGet(void)
{
	UINT32 free;
    UINT8 mid;
    UINT8 devid;
	devInfo_t pdevInfo;
	
	if((isCardExist()) && (sPara.StorageModeFlag == Card))
	{
		vfsDevInfoGet(DRIVE_SD, 0, &pdevInfo);
	}
	else
	{
		vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfo);
	}
	
    mid=pdevInfo.makerId;
    devid=pdevInfo.hwDevId; 
	/*
	osdPrintf("MID:%4d",0,64,0xD2,mid);
	osdPrintf("DEVID:%4d",0,96,0xD2,devid);
	*/
	free = pdevInfo.nrFreeBytes;
    if(devid==117)
    {
		if(sPara.VideoClipSize == VideoSize320)
		{
			if(sPara.ImageQuality == QualitySuper)
				free = free/(127*1024);/*31Kbyte one second*/ 
			else if(sPara.ImageQuality == QualityFine)
				free = free/(115*1024);/*31Kbyte one second*/ 
			else 
				free = free/(90*1024);/*31Kbyte one second*/ 
		}
		else if(sPara.VideoClipSize == VideoSize640)
		{
			if(sPara.ImageQuality == QualitySuper)
				free = free/(166*1024);/*62Kbyte one second*/
			else if(sPara.ImageQuality == QualityFine)
				free = free/(119*1024);/*31Kbyte one second*/ 
			else 
				free = free/(97*1024);/*31Kbyte one second*/
		}
		else 
		{
			if(sPara.ImageQuality == QualitySuper)
				free = free/(127*1024);/*31Kbyte one second*/ 
			else if(sPara.ImageQuality == QualityFine)
				free = free/(115*1024);/*31Kbyte one second*/ 
			else 
				free = free/(90*1024);/*31Kbyte one second*/ 
		}
    }
    else if(devid==115)
    {
		if(sPara.VideoClipSize == VideoSize320)
		{
			if(sPara.ImageQuality == QualitySuper)
				free = free/(115*1024);/*31Kbyte one second*/ 
			else if(sPara.ImageQuality == QualityFine)
				free = free/(91*1024);/*31Kbyte one second*/ 
			else 
				free = free/(60*1024);/*31Kbyte one second*/ 
		}
		else if(sPara.VideoClipSize == VideoSize640)
		{
			if(sPara.ImageQuality == QualitySuper)
				free = free/(158*1024);/*62Kbyte one second*/
			else if(sPara.ImageQuality == QualityFine)
				free = free/(127*1024);/*31Kbyte one second*/ 
			else 
				free = free/(108*1024);/*31Kbyte one second*/
		}
		else 
		{
			if(sPara.ImageQuality == QualitySuper)
				free = free/(115*1024);/*31Kbyte one second*/ 
			else if(sPara.ImageQuality == QualityFine)
				free = free/(91*1024);/*31Kbyte one second*/ 
			else 
				free = free/(60*1024);/*31Kbyte one second*/ 
		}
			
    }
    else
    {
		if(sPara.VideoClipSize == VideoSize320)
		{
			if(sPara.ImageQuality == QualitySuper)
				free = free/(127*1024);/*31Kbyte one second*/ 
			else if(sPara.ImageQuality == QualityFine)
				free = free/(115*1024);/*31Kbyte one second*/ 
			else 
				free = free/(90*1024);/*31Kbyte one second*/ 
		}
		else if(sPara.VideoClipSize == VideoSize640)
		{
			if(sPara.ImageQuality == QualitySuper)
				free = free/(166*1024);/*62Kbyte one second*/
			else if(sPara.ImageQuality == QualityFine)
				free = free/(119*1024);/*31Kbyte one second*/ 
			else 
				free = free/(97*1024);/*31Kbyte one second*/
		}
		else 
		{
			if(sPara.ImageQuality == QualitySuper)
				free = free/(127*1024);/*31Kbyte one second*/ 
			else if(sPara.ImageQuality == QualityFine)
				free = free/(115*1024);/*31Kbyte one second*/ 
			else 
				free = free/(90*1024);/*31Kbyte one second*/ 
		}
    }    		

	return free;
}

/**
**/
void DisplayMVIcon(UINT8 isClear)
{
	UINT32 osdW, osdH; 
	UINT32 width, height;
	UINT32 free;
	UINT32	size;
	devInfo_t pdevInfo;
	UINT8 str[20];
	
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	
	if (isClear)
	{
		osdFWBuffChange();
		osdClearScreen(0x00);
	}
	
	hwDispPipEn(0);
	
	if (snapAV == OBJ_NONE) /* Movie Icon */
	{
		osdStrDisp(0, 0, UserIcon, 0x40, "\x28\x29");
	}
	else
	{
		osdStrDisp(0, 0, UserIcon, 0x10, "\x28\x29");			
	}
	
	switch (sPara.snapFlag)
	{	/* Paul@2005/11/17-14:22 add */
		case SnapSingle:
		{
			osdBarDraw((osdW-64)/2, 32, 64, 32, 0x00);
			break;
		}
		case SnapMulti:
		{
			break;
		}
		case SnapSelf:
		{
			osdStrDisp((osdW-64)/2, 32, UserIcon, 0x30, "\x32\x33");
			sio_psprintf(str, "%02d", ui8DownTime[ui8DownTimeMvIdx]);
			osdStrDisp(((osdW-64)/2)+32, 40, UserFont16x20, 0x30, str);
			break;
		}			
		/* Paul@2005/11/17-14:22 end */
	}			
	switch(ManualWbFlag)
	{
		case WBAuto :
			osdStrDisp(0, 32, UserIcon, 0xd0, "\x5B\x5C");
			break;
			
		case WBDaylight :
			osdStrDisp(0, 32, UserIcon, 0xd0, "\x5E\x5F");				
			break;
			
		case WBCloudy :
			osdStrDisp(0, 32, UserIcon, 0xd0, "\x3E\x3F");								
			break;
			
		case WBInca :		/* 白*/
			osdStrDisp(0, 32, UserIcon, 0xd0, "\x7B\x7C");								
			break;
			
		case WBFluorescent : /* 荧光灯 */
			osdStrDisp(0, 32, UserIcon, 0xd0, "\x7E\x7F");								
			break;				
	}
	
	/* Paul@2005/10/26 add Quanlity Icon */
	switch (sPara.ImageQuality)
	{
		case QualitySuper :
			osdStrDisp(8*10, osdH-32, UserIcon, 0xd0, "\x22\x23");
			break;
			
		case QualityFine :
			osdStrDisp(8*10, osdH-32, UserIcon, 0xd0, "\x24\x25");
			break;

		case QualityNormal :
			osdStrDisp(8*10, osdH-32, UserIcon, 0xd0, "\x26\x27");
			break;
	}	
	
//	if (snapAV == OBJ_ASF)
	if (snapAV != OBJ_NONE)
	{
		/*if (asfPauseFlag == 0)*/
		{
			sio_psprintf(str, "[%2d:%02d:%02d]", asfTime/1000/60/60, (asfTime/1000/60)%60, asfTime/1000%60);
			
			memoryflag = (asfTime/1000/60/60)|((asfTime/1000/60)%60)|(asfTime/1000%60);
			osdStrDisp(osdW-11*10, osdH-20, UserFont10x20, 0x10, str);		//John update 051025

			osdStrDisp(osdW-22, osdH-64, UserFont10x20, 0x00, " ");
		}

		if (pauseFlag == PAUSE_NOW)
		{
			osdStrDisp(80, 0, UserIcon10x20, 0x30, "\x90\x91");			
		}
		else
		{
			if (ui16SnapCount == 0)
			{
				ui16SnapCount = 1;
				osdStrDisp(80, 0, UserIcon, 0x10, "\x9C");
			}
			else
			{
				ui16SnapCount = 0;
				osdStrDisp(80, 0, UserFont, 0x10, "  ");				
			}
		}
	}
	else
	{
		osdStrDisp(80, 0, UserFont, 0x10, " ");
		free = RecordLengthGet();	/* Paul@2005/11/05-16:50 add */
		sio_psprintf(str, "[%2d:%02d:%02d]", free/3600, (free/60)%60, free%60);
		osdStrDisp(osdW-11*10, osdH-20, UserFont10x20, 0x40, str);
	}

	if (sPara.VideoClipSize == VideoSize640)			/* Movie Size */
	{
		osdStrDisp(8*10+32, osdH-20, UserFont10x20, 0xd0, "640");
	}
	else if (sPara.VideoClipSize == VideoSize320)			/* Movie Size */
	{
		osdStrDisp(8*10+32, osdH-20, UserFont10x20, 0xd0, "320");
	}
	
	DisplayCaptureEv(vcEvCmpIdx);
	/*
	osdVProgressDraw(0, 10, 10-vcEvCmpIdx, 0x7c);
	osdHProgressDraw(100, 400, vcZoomFactor, 0x7c);
	*/	
	
	if (isClear)
	{
		osdHWBuffChange();
	}		
}

extern UINT32 gPB;
extern UINT32 fileCnt;
extern UINT32 fileIdx;
/**
**/
UINT32 DisplayMovieInfo(UINT32 ui32FileIdx)
{
	asfFileInfo_t asfFileInfo;
	aviFileInfo_t  aviFileInfo;	
	
	UINT32 err;
	UINT32 dispW, dispH;
	UINT32 pPBAddr;
	xdcfAttrElm_t xFileAttr;
	xdcfTime_t xTime;
	UINT8 str[20];
	UINT32 ui32FileType;
	UINT32 ui32PlayTime;

	osdStrDisp(0, 0, UserIcon16x20, 0x40, "\x04\x05");	/* Playback icon */				
	err	= xdcfFileTypeGet(ui32FileIdx, &ui32FileType);	
	if ((err = xdcfCurFileByPosSet(ui32FileIdx)) != SUCCESS)
	{
		return -1;
	}
	err	= xdcfCurFileAttrGet(&xFileAttr);
	xTime = xFileAttr.time;

	// icon of protect file      
	if (xFileAttr.attribute & xDCF_ATTR_READONLY)
	        osdStrDisp(38, 8, IconLock, 0x10,"\x1");      
	else
	   		osdStrDisp(38, 8, IconLock, 0x0," ");
	
	osdBarDraw(0, (PANEL_1_OSD_H-20), PANEL_1_OSD_W, 20, 0x00);
	osdStrDisp(0, (PANEL_1_OSD_H-20), UserFont10x20, 0x30, xFileAttr.name);
	
	sio_psprintf(str, "%04d", ui32FileIdx);
	osdStrDisp((PANEL_1_OSD_W-10*10), 0/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);
	sio_psprintf(str, "\-%04d", fileCnt);
	osdStrDisp((PANEL_1_OSD_W-6*10), 0/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);	

	hwDispPreviewSizeGet(&dispW, &dispH);
	pbThumbBufferGet(&pPBAddr);
	
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/
	
	printf("File Type = %x\n", ui32FileType);
	if (ui32FileType == xDCF_FILETYPE_ASF)
	{
		err = asfThumbGet(xFileAttr.name, gPB, dispW, dispH);
		printf("ThumbGet = %d\n", err);		
		asfAppFileInfoGet(xFileAttr.name, &asfFileInfo);	
		ui32PlayTime = asfFileInfo.playDuration;
	}
	else if (ui32FileType = xDCF_FILETYPE_AVI)
	{
		aviThumbGet(xFileAttr.name, gPB, dispW, dispH);
		aviAppFileInfoGet(xFileAttr.name, &aviFileInfo);		
		ui32PlayTime = aviFileInfo.playDuration;
	}
	else
	{
		ui32PlayTime = 0;
	}
	
	sio_psprintf(str, "[%2d:%02d:%02d]", 
		ui32PlayTime/1000/60/60, 
		(ui32PlayTime/1000/60)%60,
		(ui32PlayTime/1000)%60
		);

	osdStrDisp(10*13, (PANEL_1_OSD_H-20), UserFont10x20, 0x30, str);	
	
	return 0;
}

void dv_osdHProgressDraw(UINT32 start, UINT32 end, UINT32 pos, UINT32 color)
{
	SINT32 x, y, w, l;
	UINT32 osdW, osdH;
	UINT32 stepLgh;
	UINT32 tmp;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	if (end < start)
	{
		tmp = start;
		start = end;
		end = tmp;
	}
	if (pos > end)
		pos = end;
	if (pos < start)
		pos = start;

	w = 16;
	x = 64;
	y = osdH - w - 3 - 20;  
	l = osdW - x - 64;

#if 0
	osdStrDisp(x-w, y, UserIcon16x16, color&0xf0, "\x03");
	osdStrDisp(x+l, y, UserIcon16x16, color&0xf0, "\x04");
#endif	

	osdFrameDrawX(x, y, l, w, color&0x0f, 3);
	x += 3;
	y += 3;
	w -= 6;
	l -= 6;
	osdBarDraw(x, y, l, w, 0x00);

	stepLgh = l / (end-start+1);

	if (stepLgh < 16)
	{
		osdBarDraw(x+(l-(16-stepLgh))*(pos-start)/(end-start+1), y, 16, w, color>>4);
	}
	else
	{
		osdBarDraw(x+l*(pos-start)/(end-start+1), y, stepLgh, w, color>>4);
	}
}

#define PLAY_ICON_X		(PANEL_1_OSD_W / 2 +6)	
#define PLAY_ICON_Y		(0)	

/**
**/
void DispMovieProgress(UINT8 isClear)
{
	UINT32 osdW, osdH;
	UINT32 tempProgress;
	UINT8  str[20];
	asfFileInfo_t  asfFileInfo;
	aviFileInfo_t  aviFileInfo;
	
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	
	if (isClear)
	{
		osdFWBuffChange();
		osdClearScreen(0x00);		
	}
	
	osdStrDisp(PLAY_ICON_X, PLAY_ICON_Y, UserFont10x20, 0x0, "  ");
	if (pbDisp == PB_DISP_ONE)
	{
		switch (g_playrecStatus)
		{
			case STATUS_STOP:
				osdStrDisp(PLAY_ICON_X , PLAY_ICON_Y, UserIcon10x20, 0x30, "\x57\x58");
				break;
			case STATUS_PLAYING:
				osdStrDisp(PLAY_ICON_X, PLAY_ICON_Y, UserIcon10x20, 0x40, "\x8e\x8f");
				break;
			case STATUS_PAUSE:
				osdStrDisp(PLAY_ICON_X, PLAY_ICON_Y, UserIcon10x20, 0x30, "\x90\x91");
				break;
			case STATUS_FF:
				osdStrDisp(PLAY_ICON_X, PLAY_ICON_Y, UserIcon10x20, 0x40, "\x8a\x8b");
				break;
			case STATUS_REW:
				osdStrDisp(PLAY_ICON_X, PLAY_ICON_Y, UserIcon10x20, 0x40, "\x8c\x8d");
				break;
			default:
				osdStrDisp(PLAY_ICON_X, PLAY_ICON_Y, UserFont10x20, 0x0, "  ");
				break;
		}

		if(avPlayNow == OBJ_ASF)
		{
			if (asfAppStatusGet() == ASF_STATUS_PLAY_TO_EOF)
			{
				tempProgress = 100;
			}
			else
			{
				tempProgress = asfPlayProgressGet();
			}

			asfAppFileInfoGet(NULL,&asfFileInfo);
			if(asfFileInfo.playDuration >= asfFileInfo.residualTime )
			{
				asfTime = asfFileInfo.playDuration - asfFileInfo.residualTime;
			}
			else
			{
				asfTime = 0;
			}
		}				 
		else if(avPlayNow == OBJ_AVI)
		{
			tempProgress = aviPlayProgressGet();
			aviAppFileInfoGet(NULL, &aviFileInfo);
			asfTime = aviFileInfo.playDuration - aviFileInfo.residualTime;
		}
		
//		dv_osdHProgressDraw(0, 100, tempProgress, 0x7c);
		dv_osdHProgressDraw(0, 100, tempProgress, 0x4c);
		sio_psprintf(str, "%01d:%02d:%02d", 
					asfTime/1000/60/60, 
					(asfTime/1000/60)%60, 
					asfTime/1000%60);
		
		osdStrDisp(22, osdH-20*3-2, UserFont10x20, 0x00, str);
		
	}
	
	if (isClear)
	{
		osdHWBuffChange();
	}
	
}

/**
**/
extern UINT8  ui8VolCnt;
extern UINT8  pressVolume; 
extern UINT32 g_ui32SaveKeyCode;

void dv_osdAutoDisp(void)
{
 	UINT8 ui8Temp;
	
	if ((!(g_ui32SaveKeyCode & KEY_BIT_DOWN)) 
	 &&(!(g_ui32SaveKeyCode & KEY_BIT_UP)) )
	{	
	 	if(ui8VolCnt != 0)
		{
      	    ui8VolCnt = 0;
		 	return;  
		}
		
		if ((avPlayNow == OBJ_MP3) ||(avPlayNow == OBJ_WMA)||(avPlayNow == OBJ_WAV))
		{
			pressVolume = 0; 
			osdBarDraw(PANEL_1_OSD_W/2-40, PANEL_1_OSD_H-54-10*2+6, 60, 3+10*2, 0x00); 
		}
		else
		{
			if ((avPlayNow)||(snapAV))
			{
				pressVolume = 0;
				osdBarDraw(PANEL_1_OSD_W/2-40, PANEL_1_OSD_H-54-10*2+6, 60, 3+10*2, 0x00); 
	       	}		
		}
	}
}

extern UINT8 speed;
/**
**/
static UINT32 PlayFlag;
#define NEXT_ASF 2
#define LAST_ASF 1
// tzw add 060315 modify 
UINT8   ui8PlayDownUpLife = 1  ;  //播放时 按上下键时 

UINT32 PlaybackMovie(UINT32 ui32FileIdx)
{
	UINT32 err = SUCCESS;
	UINT32 dispW, dispH;
	asfFileInfo_t asfFileInfo;
	aviFileInfo_t  aviFileInfo;
	xdcfAttrElm_t xFileAttr;
	UINT32 samplerate = 0;
	UINT32 ui32FileType;
	UINT32 codecDev;	
	UINT32 startTime = 0, endTime = 0;	
	UINT32* msg;
	UINT32 ui32Message;	
	UINT32 sts;
	xdcfTime_t xTime;	
	UINT8 str[10];
	
	PlayFlag = 1;

	osdStrDisp(0, 0, UserIcon16x20, 0x40, "\x04\x05");	/* Playback icon */				
	
	if ((err = xdcfCurFileByPosSet(ui32FileIdx)) != SUCCESS)
	{
		#ifdef	OSD_DEBUG
		printf(" #FILE_ERR ");
		#endif
		return -1;
	}	
	if ((err = xdcfCurFileAttrGet(&xFileAttr)) != SUCCESS)
	{
		return -1;
	}	
	if ((err = xdcfFileTypeGet(ui32FileIdx, &ui32FileType)) != SUCCESS)
	{
		return -1;
	}


	xTime = xFileAttr.time;
	
	osdBarDraw(0, (PANEL_1_OSD_H-20), PANEL_1_OSD_W, 20, 0x00);
	osdStrDisp(0, (PANEL_1_OSD_H-20), UserFont10x20, 0x30, xFileAttr.name);
	
	sio_psprintf(str, "%04d", ui32FileIdx);
	osdStrDisp((PANEL_1_OSD_W-10*10), 0/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);
	sio_psprintf(str, "\-%04d", fileCnt);
	osdStrDisp((PANEL_1_OSD_W-6*10), 0/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);	

	printf("Start of play asf\n");	
	hwDispPvSetRead(imgBuf);
	ChgPvSetting = 1;
	
#if (CurrentIC8751==0)	
        gpioConfig(GPIO_MUTE, 1);
        gpioSet(GPIO_MUTE, 0);
#else		
	if(SpeakFlag)
		wm8751SoundEnable(2,1);
	else
		wm8751SoundEnable(1,1);
#endif	
	
	sysgAudOutDevGet(&codecDev);
	if (ui32FileType == xDCF_FILETYPE_ASF)
	{
		asfAppFileInfoGet(xFileAttr.name, &asfFileInfo);
		samplerate = asfFileInfo.audioSampleFreq;
	}
	else if (ui32FileType == xDCF_FILETYPE_AVI)
	{
		aviAppFileInfoGet(xFileAttr.name, &aviFileInfo);
		samplerate = aviFileInfo.audioSampleFreq;

		printf("Qtable = %d\n", aviAppClipQtableGet());
	}
	else
	{
		samplerate = 8000;
	}
	printf("samplerate:%d\n", samplerate);

	UINT8 samplerateIdx = 0 ;
	switch(samplerate)
	{
			case 48000:
				samplerateIdx = 0 ;
				break ;
			case 0xac44:
				samplerateIdx = 1 ;
				break ;							
			case 0x7d00:
				samplerateIdx = 2 ;
				break ;							
			case 0x5dc0:
				samplerateIdx = 3 ;
				break ;							
			case 0x5622:		
				samplerateIdx = 4 ;
				break ;							
			case 16000:
				samplerateIdx = 5 ;
				break ;	
			case 0x2ee0:
				samplerateIdx = 6 ;
				break ;								
			case 0x2b11:	
				samplerateIdx = 7 ;
				break ;							
			
			case 8000:
				samplerateIdx = 8 ;
				break ;							
			break;
		default:
			samplerate = 8000;
	}
	

#if (CurrentIC8751==0)
	hwAudAC97RegWrite(0x2c, samplerate);
	hwAudAC97RegWrite(0x2e, samplerate);
	//hwAudAC97RegWrite(0x32, samplerate);
#else
	wm8751SampleSet(samplerateIdx) ;
#endif

	if (ui32FileType == xDCF_FILETYPE_ASF)
	{
		avPlayNow =	OBJ_ASF;
		asfAppPlay(xFileAttr.name);
	}
	else if (ui32FileType == xDCF_FILETYPE_AVI)
	{
		avPlayNow =	OBJ_AVI;
		aviAppPlay(xFileAttr.name);
	}
	
	pbDisp = PB_DISP_ONE;
	g_playrecStatus = STATUS_PLAYING;
	asfTime = 0;
	tmrEnable(1, 8000, TMR_PERIODIC, dv_osdAutoDisp);
	SpeakFlagMode = 1;
//	ws9712LineOut(SpeakFlag);
	osdBarDraw(0, PANEL_1_OSD_H-20*2, PANEL_1_OSD_W, 20, 0x00);
	DispMovieProgress(0);	
//	volSet();

#if (CurrentIC8751==0)		
				ws9712LineOut(SpeakFlag);//wendy for speak
#else
				wm8751LineOut(SpeakFlag);
#endif

#if (CurrentIC8751==0)	
			if(SpeakFlag)
			{
				pbSetVolume(ws9712_OUT2, mp3Vol);
			}
			else
			{
				pbSetVolume(ws9712_HP, mp3Vol);
			}
#else
			if(SpeakFlag)
			{
				pbSet8751Volume(ws9712_OUT2,mp3Vol);
			}
			else
			{
				pbSet8751Volume(ws9712_HP,mp3Vol);
			}
#endif	
	
	ui32Message = 0xFFFFFFFF;	
	printf("=====================");
	
	osTimeDly(20);
	while ((uiState & UI_MODE_MASK) == UI_MODE_MOVIE)
	{
		switch(ui32Message)
		{
		case UI_KEY_ACCIDENT:
			if (avPlayNow == OBJ_AVI)
			{
				aviAppPlayAbort();
			}
			else if (avPlayNow == OBJ_ASF)
			{
    			asfAppPlayAbort();
			}
			g_playrecStatus = STATUS_STOP;
			if (ChgPvSetting)
			{
				hwDispPvSetWrite(imgBuf);
				ChgPvSetting = 0;
			}
			avPlayNow = OBJ_NONE;
			osSemPend(semApp, 0, &err); /* wait */
		    osSemPost(semApp);          /* free */
			avPlayNow = OBJ_NONE;
			osSemPend(semApp, 20, &err);
			osSemPost(semApp);
			tmrDisable(1);
			osdClearScreen(0);
			if (uiUSBStatus )   //tzw add 060315  
			{
				PlayFlag = 0;
			}
			
			return 0;
			break;

		case UI_KEY_FUNC_MENU:
			if (avPlayNow == OBJ_AVI)
			{
				aviAppPlayAbort();
			}
			else if (avPlayNow == OBJ_ASF)
			{
    			asfAppPlayAbort();
			}
			g_playrecStatus = STATUS_STOP;			
			if (ChgPvSetting)
			{
				hwDispPvSetWrite(imgBuf);
				ChgPvSetting = 0;
			}
			avPlayNow = OBJ_NONE;
			osSemPend(semApp, 0, &err); /* wait */
		    osSemPost(semApp);          /* free */
			avPlayNow = OBJ_NONE;
			osSemPend(semApp, 20, &err);
			osSemPost(semApp);				
			tmrDisable(1);			
			osdClearScreen(0);	
			PlayFlag = 0;
			return 0;
			break;
						
		case UI_KEY_DIR_UP:
			pressVolume = 1;			
			if (mp3Vol < C_Max_Vol)
			{
				ui8VolCnt = 1;
				mp3Vol ++;
				pbVolumWindowRfsh(mp3Vol);
			}
			break;

		case UI_KEY_DIR_DOWN:
			pressVolume = 1;
			if (mp3Vol>C_Min_Vol)
			{
				ui8VolCnt = 1;	
				mp3Vol --;
				pbVolumWindowRfsh(mp3Vol);
			}						
			break;

		case UI_KEY_FUNC_OK:
			if (g_playrecStatus == STATUS_PLAYING)
			{
				g_playrecStatus = STATUS_PAUSE;
			}
			else
			{
				g_playrecStatus = STATUS_PLAYING;
			}
			
			if (avPlayNow == OBJ_AVI)
			{
				aviAppPause();
			}
			else if (avPlayNow == OBJ_ASF)
			{
    			asfAppPause();
			}
			break;
			
		case UI_KEY_DIR_LEFT:
			if (g_playrecStatus != STATUS_REW)
			{
				osTimeDly(20);			
				if (!(g_ui32SaveKeyCode & KEY_BIT_LEFT))
				{
					if (fileCnt < 2)
					{
						break;
					}		
					if (fileIdx == 1)
					{
						fileIdx = fileCnt;
					}
					else
					{
						fileIdx--;
					}
					ui8PlayDownUpLife = 0 ;
					UI_OSQFlush(uiKeyMsgQ);
					ui32Message = UI_KEY_ACCIDENT;
					continue;					
				}
			}
			
			g_playrecStatus = STATUS_REW;			
			if (avPlayNow == OBJ_ASF)
			{	
				asfAppSpeedSet(-4);
			}
			else if (avPlayNow == OBJ_AVI)
			{
				aviAppSpeedSet(-4);
			}
			break;

		case UI_KEY_DIR_RIGHT:
			if (g_playrecStatus != STATUS_FF)
			{	
				osTimeDly(20);
				if (!(g_ui32SaveKeyCode & KEY_BIT_RIGHT))
				{
					if (fileCnt < 2)
					{
						break;
					}			
					
					if (fileIdx == fileCnt)
					{
						fileIdx = 1;
					}
					else
					{
						fileIdx++;
					}								
					ui8PlayDownUpLife = 0 ;
					UI_OSQFlush(uiKeyMsgQ);
					ui32Message = UI_KEY_ACCIDENT;
					continue;			
				}
			}
			
			g_playrecStatus = STATUS_FF;
			if (avPlayNow == OBJ_ASF)
			{
				asfAppSpeedSet(4);
			}
			else if (avPlayNow == OBJ_AVI)
			{
				aviAppSpeedSet(4);
			}			
			break;
				
		default:
			break;
		}
		DispMovieProgress(0);

		msg = osQuePend(uiKeyMsgQ, 30, &err);
		
		if (err == OS_NO_ERR)
		{
			if((*msg & UI_KEY_MODE_MASK) != 0)
			{
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);
				ui32Message = UI_KEY_ACCIDENT;
				continue;
			}
			else if(*msg == UI_KEY_ACCIDENT)
			{
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
				ui32Message = UI_KEY_ACCIDENT;
				continue;
			}
			else
			{
				ui32Message = *msg;
			}
		}
		else
		{
			ui32Message = UI_KEY_MODE_MASK;
		}
		
		if (g_playrecStatus == STATUS_FF)
		{
			if (ui32Message != UI_KEY_DIR_RIGHT)
			{
				g_playrecStatus = STATUS_PLAYING;						
				if (avPlayNow == OBJ_ASF)
				{
					asfAppSpeedSet(1);
				}
				else if (avPlayNow == OBJ_AVI)
				{
					aviAppSpeedSet(1);
				}									
			}
		}
		else if (g_playrecStatus == STATUS_REW)
		{
			if (ui32Message != UI_KEY_DIR_LEFT)
			{
				g_playrecStatus = STATUS_PLAYING;						
				if (avPlayNow == OBJ_ASF)
				{
					asfAppSpeedSet(1);
				}
				else if (avPlayNow == OBJ_AVI)
				{
					aviAppSpeedSet(1);
				}															
			}					
		}					
		
		if (avPlayNow == OBJ_ASF)			
		{
			sts = asfAppStatusGet();											
			if((sts != ASF_STATUS_CLEAR))
		    {
				ui32Message = UI_KEY_ACCIDENT;
				continue;
		    }
		}
		else if (avPlayNow == OBJ_AVI)
		{
			sts = aviAppStatusGet();
			if(sts != AVI_STATUS_CLEAR)
		    {
				ui32Message = UI_KEY_ACCIDENT;
				continue;
		    }			
		}
	}
#if (CurrentIC8751==0)	
        gpioConfig(GPIO_MUTE, 1);
        gpioSet(GPIO_MUTE, 1);
#else		
	wm8751SoundEnable(0,0);
#endif				
	return 0;
}

/**
**/
void dv_Playback(void)
{
	UINT32 key = UI_KEY_MODE_PLAYBACK;
	UINT32 err;
	UINT32 dispW, dispH;
	UINT32 ui32InPlayback = 1;


	fileCnt = 1;
	fileIdx = 1;
	while ((uiState & UI_MODE_MASK) == UI_MODE_MOVIE)
	{
		if (isMenuOn)
		{
			menuPBMovieProc(key);
			if (isMenuOn == 0)
			{
				key = UI_KEY_MODE_PLAYBACK;
			}
		}
		
		if (uiState != ui32NextState)
		{
			break;
		}			
		
		if (isMenuOn == 0)
		{
			switch (key)
			{
				case UI_KEY_ACCIDENT:
				case UI_KEY_FUNC_MENU:
					/* Exit to movie preview */
					ui32InPlayback = 0;
					break;;
					
				case UI_KEY_DIR_LEFT:
					break;

				case UI_KEY_DIR_RIGHT:				
					break;

				case UI_KEY_DIR_UP:
					if (fileCnt < 2)
					{
						break;
					}		
					if (fileIdx == 1)
					{
						fileIdx = fileCnt;
					}
					else
					{
						fileIdx--;
					}
					DisplayMovieInfo(fileIdx);				
					break;

				case UI_KEY_DIR_DOWN:
					if (fileCnt < 2)
					{
						break;
					}			
					
					if (fileIdx == fileCnt)
					{
						fileIdx = 1;
					}
					else
					{
						fileIdx++;
					}				
					DisplayMovieInfo(fileIdx);				
					break;

				case UI_KEY_FUNC_B:
					if (fileCnt == 0)
					{
						break;
					}	
					onMovieMenu = 1;
					menuPBMovieInit(key);
					break;

				case UI_KEY_FUNC_OK:
					if (fileCnt == 0)
					{
						break;
					}	
					
					do {

						ui8PlayDownUpLife = 1 ;
						PlaybackMovie(fileIdx);
						#if 1  // for bug 52 tzw add 060216
						if(	ui8PlayDownUpLife ==  1)
						{
							if (fileIdx == fileCnt)
							{
								fileIdx = 1;
							}
							else
							{
								fileIdx++;
							}	
						}	
						#endif
						
					}while(PlayFlag == 1);
					
					if (ui32NextState != uiState)
					{
						break;
					}	
					
					DisplayMovieInfo(fileIdx);
					break;
					
				case UI_KEY_MODE_PLAYBACK:
					osTaskSuspend(osTaskFindByName("AAA"));
					osTaskSuspend(osTaskFindByName("HISTOGRAM"));				

					pbDisp = PB_DISP_FOUR;
					hwDispPvEn(0);				
					hwDispOsdEn(0);
					osdClearScreen(0);
					hwDispPreviewSizeGet(&dispW, &dispH);
					hwPlayBufSizeSet(dispW,	dispH);
					hwPlayBufAddrSet(gPB);
					hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
					hwCammodeSet(5);	  /* play back mode	*/
					hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/	

					osdStrDisp(0, 0, UserIcon16x20, 0x40, "\x04\x05");	/* Playback icon */			
					
					err	= xdcfInit(imageDirNameStr, DVNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
					xdcfCurRootDirSet(imageRootNameStr);					
					err	= xdcfFilterSet(xDCF_FILTER_ASF | xDCF_FILTER_AVI);
					xdcfFileCountGet(&fileCnt);	
					
					printf("Enter movie playback %d\n", fileCnt);					
					if (fileCnt == 0)
					{
						uiDispBuffFill(0x0000);
						osdStrDisp((PANEL_1_OSD_W-8*10)/2, (PANEL_1_OSD_H-20)/2, UserFont10x20, 0x10, dc_GetString(dc_NoVideo_s));
						hwDispPvEn(1);				
						hwDispOsdEn(1);
						fileIdx = 0;
						break;
					}						
					
					DisplayMovieInfo(fileIdx);
					hwDispOsdEn(1);
					hwDispPvEn(1);
					break;
			}
		}

		if (((uiState & UI_MODE_MASK) != UI_MODE_MOVIE)
		 || (ui32InPlayback == 0))
		{
			break;
		}

		keyGet(&key);
	}

	pbDisp = PB_DISP_FOUR;	
	uiState = ui32NextState;
}

UINT32 g_PauseTime;
UINT32 g_PausePeriod;
/**
	Paul@2005/11/22-11:24 add 
**/

void scene_ASFClip(void)
{
	UINT8 asfFilename[13];/* after and include 1.8.0, name array must at least 13 */
	/*UINT32 autFuncID = UNKNOWN;*/
	UINT8  mid;
	UINT8 devid;
	UINT32 *msg;	
	UINT32 ui32Message;
	UINT32 err = SUCCESS;
	UINT32 sts;
	UINT32 freeMem;
	devInfo_t pdevInfo;
	UINT32 StartTime, EndTime;
	UINT32 storage,freespaceKbytes;

	xdcfActiveDevIdGet(&storage);
	
	if((storage != DRIVE_NAND) && isCardProtect())
	{
//		MsgShow(card_protect_s);
		hwWait(0, 1000);		
		return ;		
	}	

	freespaceKbytes = 0;
	xdcfFreeSpaceGet(&freespaceKbytes);
	freespaceKbytes /= 1024;

	if(freespaceKbytes<64)
	{
//		MsgShow(memory_full_s);
		hwWait(0, 1000);
		return;
		}

	pauseFlag = CLIP_NOW;	
	
	#if 1
	/*hwDispOsdEn(0);*/
	hwDispRegUpd(0x01);
	printf("turn off osd\n");
	#endif

	hwDispPvSetRead(imgBuf);
	ChgPvSetting = 1;	
	xdcfCurRootDirSet(DVDCRootNameStr);
	xdcfNextFileNameGet(ui32MovieType, asfFilename);
	
	#if 1
	UINT32  codecDev;
	sysgAudOutDevGet(&codecDev);
	if( codecDev == AUDIO_AC97 ) 
	{
		hwAudAC97RegWrite(0x2c, 8000);		/* 8K Hz*/
		hwAudAC97RegWrite(0x2e, 8000);		/* 8K Hz*/
		hwAudAC97RegWrite(0x32, 8000);		/* 8K Hz*/
	}
	else if( codecDev == AUDIO_IIS )
	{
		/* wm8751RegSet(0x10, 0x00);			8k hz*/
	}
	else {
	}
	#endif	
	
	if (ui32MovieType == xDCF_FILETYPE_ASF)
	{
		snapAV	= OBJ_ASF;
		asfAppClipNameSet(asfFilename);
		asfAppClip(); 
	}
	else if (ui32MovieType == xDCF_FILETYPE_AVI)
	{
		snapAV = OBJ_AVI;
		aviAppClipNameSet(asfFilename);
		aviAppClip();
	}
	
	StartTime = EndTime = tmrMsTimeGet();
	asfTime = 0;
	g_PausePeriod = 0;	
	ui32Message = UI_KEY_MODE_MASK;	
	osTimeDly(20);
    while((uiState & UI_MODE_MASK) == UI_MODE_MOVIE)	
    {
        switch(ui32Message)
        {		
		case UI_KEY_ACCIDENT:
			if (snapAV == OBJ_ASF)
			{
	    		asfAppClipAbort(); /* stop clipping	*/
			}
			else if (snapAV == OBJ_AVI)
			{
				aviAppClipAbort();
			}			
			if (ChgPvSetting)
			{
				hwDispPvSetWrite(imgBuf);
				ChgPvSetting = 0;
			}
			//    		osdBarDraw(32, 0, 16*8, 32, 0x00); /*Clear clip time, liuyan@2004/2/23 11:21AM*/
			snapAV	= OBJ_NONE;
			osSemPend(semApp, 0, &err); /* wait */
		       osSemPost(semApp);          /* free */
			snapAV	= OBJ_NONE;
			vcZoomFactor = 100;
			zoomFactSet(vcZoomFactor);
			/*hwPVZoom(vcZoomFactor);*/
			sysgSnapScaleFactorSet(vcZoomFactor);
			osSemPend(semApp, 20, &err);
			osSemPost(semApp);
			hwCdspWbOffsetSet(0xfa, 0xfe, 0xf8, 0xfe);
			printf("exit asf clip1\n");				
    		return;
			break;

        case UI_KEY_FUNC_A:
			if(asfTime > 1000)
			{
				printf("exit asf clip2\n");
				if (snapAV == OBJ_ASF)
				{
		    		asfAppClipAbort(); /* stop clipping	*/
				}
				else if (snapAV == OBJ_AVI)
				{
					aviAppClipAbort();
				}				
				if (ChgPvSetting)
				{
					hwDispPvSetWrite(imgBuf);
					ChgPvSetting = 0;
				}	
	    		/* turn on OSD to fix the noise when record 640x480 using TV */
//	    		osdBarDraw(32, 0, 16*8, 32, 0x00); /*Clear clip time, liuyan@2004/2/23 11:21AM*/
				snapAV	= OBJ_NONE;
				osSemPend(semApp, 0, &err); /* wait */
			    osSemPost(semApp);          /* free */
				snapAV	= OBJ_NONE;
				vcZoomFactor = 100;
				zoomFactSet(vcZoomFactor);
				/*hwPVZoom(vcZoomFactor);*/
				sysgSnapScaleFactorSet(vcZoomFactor);
				osSemPend(semApp, 20, &err);
				osSemPost(semApp);
	    		return;
			}
        	break;
			
        case UI_KEY_DIR_UP:
			if (vcEvCmpIdx > 0)
			{
				aaaEvCmpIdxSet(--vcEvCmpIdx);
				DisplayCaptureEv(vcEvCmpIdx);
			}
            break;
			
        case UI_KEY_DIR_DOWN:
			if (vcEvCmpIdx < 12)
			{
				aaaEvCmpIdxSet(++vcEvCmpIdx);
				DisplayCaptureEv(vcEvCmpIdx);				
			}
            break;
			
		case UI_KEY_FUNC_OK:
			if(pauseFlag == CLIP_NOW)
			{
				g_PauseTime = tmrMsTimeGet();				
				pauseFlag = PAUSE_NOW;
			}
			else
			{
				g_PausePeriod = g_PausePeriod + tmrMsTimeGet() - g_PauseTime;				
				pauseFlag = CLIP_NOW;
				zoomFactSet(vcZoomFactor);
				sysgSnapScaleFactorSet(vcZoomFactor);	
			}
			if (snapAV == OBJ_ASF)
			{
	    		asfAppPause(); 
			}
			else if (snapAV == OBJ_AVI)
			{
				aviAppPause();
			}			
			break;
			
        case UI_KEY_DIR_LEFT:
			do
			{
				if (vcZoomFactor > 100)
				{
					vcZoomFactor -= 1;
					hwPVZoom(vcZoomFactor);
					sysgSnapScaleFactorSet(vcZoomFactor);									
				}
				DisplayZoomIcon(1, vcZoomFactor);				
				osTimeDly(2);
				msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);				
			}while((uiKeyPush) && (!((*msg != UI_KEY_DIR_LEFT) && (err==0))));
			DisplayZoomIcon(0, vcZoomFactor);			
            break;
			
        case UI_KEY_DIR_RIGHT:	
			do
			{
				if (vcZoomFactor < 400)
				{
					vcZoomFactor += 1;
					hwPVZoom(vcZoomFactor);
					sysgSnapScaleFactorSet(vcZoomFactor);					
				}
				DisplayZoomIcon(1, vcZoomFactor);				
				osTimeDly(2);
				msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);								
			}while((uiKeyPush) && (!((*msg != UI_KEY_DIR_RIGHT) && (err==0))));
			DisplayZoomIcon(0, vcZoomFactor);
            break;
			
		default:
			break;
    	}		
		DisplayMVIcon(0);
		hisTimeDisp();	/* Paul@2005/10/10 */							
		hisIconDisp();					
		
		msg = osQuePend(uiKeyMsgQ, 30, &err);

		if (err == OS_NO_ERR)
		{
			if((*msg & UI_KEY_MODE_MASK) != 0)
			{
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);
				ui32Message = UI_KEY_ACCIDENT;
				continue;
			}
			else if(*msg == UI_KEY_ACCIDENT)
			{
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
				ui32Message = UI_KEY_ACCIDENT;
				continue;
			}
			else
			{
				ui32Message = *msg;
			}
		}
		else
		{
			ui32Message = UI_KEY_MODE_MASK;
		}
		
		if(sPara.StorageModeFlag == Card)
		{
			vfsDevInfoGet(DRIVE_SD, 0, &pdevInfo);
		}
		else
		{
			vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfo);
		}
		freeMem = pdevInfo.nrFreeBytes;

		if (snapAV == OBJ_ASF)			
		{
			sts = asfAppStatusGet();											
			if(sts != ASF_STATUS_CLEAR)
		    {
				ui32Message = UI_KEY_FUNC_SNAP;
				continue;
		    }
			else if ((sts == ASF_STATUS_DISK_FULL)||(freeMem < 1024*100))
			{
				ui32Message = UI_KEY_FUNC_SNAP;
			}		
		}
		else if (snapAV == OBJ_AVI)
		{
			sts = aviAppStatusGet();
			if(sts != AVI_STATUS_CLEAR)
		    {
				ui32Message = UI_KEY_FUNC_SNAP;
				continue;
		    }
			else if ((sts == AVI_STATUS_DISK_FULL)||(freeMem < 1024*100))
			{
				ui32Message = UI_KEY_FUNC_SNAP;
			}		
			
		}
		
		if(pauseFlag == CLIP_NOW)
		{
			EndTime = tmrMsTimeGet();
			asfTime = EndTime-StartTime-g_PausePeriod;
		}		
    }
}


