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
 *  Author: Richie Lee                                                    *
 *                                                                        *
 **************************************************************************/
//#define RS_232_DBG		//Paul@2005/10/08 disable debug text

#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "stdio.h"
#include "ui_main.h"
#include "hw_cpu.h"
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
#include "menu_capture.h"
#include "device.h"
#include "pub_api.h"
#include "extern.h"
#include "ws9712.h"
#include "uistrings.h"

#include "shutter.h"		/* Shutter audio */

/* Effect Sound */
#define SOUND_SHUTTER	0
/********************************************************/

extern void panelSelect(UINT32);
extern void movieSpecialEffectSet(UINT8 mode);
extern void WriteLcdReg(UINT8 ui8RegAddr, UINT8 ui8Data);/* sun@060119 */

void DisplayZoomIcon(UINT16 flag, UINT32 value);
UINT32 DisplayPVIcon(UINT8 isClear);
void DisplayFocusIcon(UINT8 flag);
void DisplayFlashIcon(void);
bool selfTimeDownCount(UINT8 ui8TimeIndex);
void dc_PlaybackImage(void);
UINT32 dc_DisplayImage(UINT32 ui32FileIdx);
#if 0	/* Paul@2006/05/30 add for DownLoad picture*/	
void dc_DisplayMultiImage(UINT32 ui32FileIdx);
#else
void dc_DisplayMultiImage(UINT32 ui32FileIdx, UINT32 flag);
#endif
UINT32 dc_BrowseImage(UINT32 ui32FileIdx, UINT8 flag);
UINT32 OnChargeFlash(void);
void PlayEffectSound(UINT32 index);

UINT32	snapAV = OBJ_NONE;
UINT32	pvZoomFactor = 100;
UINT32	pvEvCmpIdx = 0x05;//0x06;

UINT8   ui8DownTimeIdx = 3;	/* Paul@2005/11/01-08:29 for 2s, 5s, 10s, 15s */
UINT8   ui8DownTime[4] = {02, 05, 10, 15};

/* Paul@2005/10/12 */
UINT32 ui32SceneFlag = SceneClose;

/* Paul@2005/11/19 add */
UINT32 g_ui32DispMode;
UINT32 g_ui32DispStart;
UINT32 g_ui32ZoomFactor;
/* Paul@2005/11/19 end */

UINT32 freeflag;
extern UINT8  ui8DV_DCPreviewFlag   ;

extern UINT32 gPB;
extern OS_EVENT *uiKeyMsgQ;
extern xdcfAttrElm_t fileattr;
extern xdcfAttrElm_t dirattr;

extern UINT8 lcdOsdDisp;
extern UINT32 ManualWbFlag;

UINT8 flashmode = FlashForceOff;/* sun@1021 pm 14:55*/

extern UINT8 TestFunc(void);		/* sun@1102 am 10:21 */

#define OSD_DEBUG

#if STILL_FUNC
/**************************************************************************
 *                                                                        *
 *  Function Name: uiPreviewStill                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *    function for Preview Still image mode UI process                    *
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
void uiPreviewStill(void)
{
	UINT32 key = UI_KEY_MODE_PREVIEW;
	UINT32 err;
	UINT32 *msg;
	UINT32	osdW, osdH;
	UINT32 i;			/* Paul@2005/10/12 */
	UINT8 str8[3];
	UINT32 storage;	
	devInfo_t pdevInfofree;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	#ifdef	OSD_DEBUG
	printf("\nP/V   \n ");
	#endif
	aaaInit();					
	uiKeyMsgQ = osEventFindByName("UI_KEY");
	isMenuOn = 0;
	pvZoomFactor = 100;	
	
	freeflag = 1; 	// John 12/23/2005
	isMenuOn = 0;
	flashmode = FlashForceOff;				/* Paul@2005/11/03-11:21 add */
	sPara.snapFlag = SnapSingle;

	captureQTidxInit();
	aeTableChange(0);
	/* Init Sensor */
	gpioConfig(GPIO_SENSOR_RST, 1);
	gpioSet(GPIO_SENSOR_RST, 0);				
	gpioConfig(GPIO_SENSOR_PWDN, 1);
	gpioSet(GPIO_SENSOR_PWDN, 0);	
	WRITE8(0xb0000007,0x7f);		/* ?? */

	gpioConfig(GPIO_SENSOR_POWER, 1);	
	gpioSet(GPIO_SENSOR_POWER, 1);	
//	hwDispPvEn(0);					/* Paul@2005/10/11 */
//	hwDispOsdEn(0);
	uiDispBuffFill(0x000000); // for bug 16  tzw add 

	hwWait(0, 8);			
	hwTvSensorModeSel(0);	

	while ((uiState & UI_MODE_MASK) == UI_KEY_MODE_PREVIEW )
	{
		if (isMenuOn)
		{
			menuPreviewProc(key);			
			if (isMenuOn == 0)
			{
				key = UI_KEY_MODE_PREVIEW;
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
					#if 1  //fqdao_add for clean "memory_full_s"
					osdBarDraw(120, 120, 120, 20, 0x00) ;
					#endif

					lcdOsdDisp = 2;
					osdLcdDisp(5);	/* Paul@2005/10/03 */					
					/* Paul@2005/10/27-10.00AM */
					menuEffectProcInit();
					key = UI_KEY_MODE_MASK;
					while ((uiState & UI_MODE_MASK) == UI_KEY_MODE_PREVIEW )
					{	
						if (menuEffectProc(key))
						{
							break;
						}
						keyGet(&key);												
					}
					/* Paul@2005/10/27-10.00AM */
					break;
					
				case UI_KEY_DIR_UP:	
					
					/* Paul@2006/07/23 */
					gpioConfig(GPIO_SENSOR_PWDN, 1);
					gpioSet(GPIO_SENSOR_PWDN, 1);	
					gpioConfig(GPIO_SENSOR_POWER, 1);
					gpioSet(GPIO_SENSOR_POWER, 1);											
					/* Paul@2006/07/23 */
					
					/* playback */
					lcdOsdDisp = 2;
					osdLcdDisp(5);	/* Paul@2005/10/03 */	
					ui8DV_DCPreviewFlag =0 ;//fqdao_add  for  perview or moive   06.6.2
					dc_PlaybackImage();
					ui8DV_DCPreviewFlag =1 ;
					key = UI_KEY_MODE_PREVIEW;

					/* Paul@2006/07/23 */
					gpioConfig(GPIO_SENSOR_PWDN, 1);
					gpioSet(GPIO_SENSOR_PWDN, 0);	
					gpioConfig(GPIO_SENSOR_POWER, 1);
					gpioSet(GPIO_SENSOR_POWER, 1);											
					/* Paul@2006/07/23 */
					
					continue;
					break;
										
				case UI_KEY_DIR_DOWN:					
					if (sPara.snapFlag == SnapSelf)
					{
						sPara.snapFlag = SnapSingle;
					}
					else if(sPara.snapFlag == SnapSingle) 
					{
						sPara.snapFlag = SnapMulti;
					}					
					else
					{
						sPara.snapFlag = SnapSelf;
					}					
					DisplayPVIcon(0);															
					break;

				case UI_KEY_DIR_LEFT :		
					do
					{
						if (pvZoomFactor > 100)
						{
							pvZoomFactor -= 5;
							hwPVZoom(pvZoomFactor);
							sysgSnapScaleFactorSet(pvZoomFactor);																			
						}
						hisTimeDisp();
						DisplayZoomIcon(1, pvZoomFactor);
						osTimeDly(2);	//~
						msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);
					}while((uiKeyPush) && (!((*msg != UI_KEY_DIR_LEFT) && (err == 0))));
					DisplayZoomIcon(0, pvZoomFactor);
					break;

				case UI_KEY_DIR_RIGHT :
					do
					{
						if (pvZoomFactor < 400)
						{		
							pvZoomFactor += 5;
							hwPVZoom(pvZoomFactor);
							sysgSnapScaleFactorSet(pvZoomFactor);																				
						}
						hisTimeDisp();	
						DisplayZoomIcon(1, pvZoomFactor);
						osTimeDly(1);	//~					
						msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);
					}while((uiKeyPush) && (!((*msg != UI_KEY_DIR_RIGHT) && (err == 0))));
					DisplayZoomIcon(0, pvZoomFactor);
					break;

				case UI_KEY_FUNC_B:
					vcZoomFactor = 100;
					hwPVZoom(vcZoomFactor);
					lcdOsdDisp = 2;
					osdLcdDisp(5);	/* Paul@2005/10/03 */					
					osTaskSuspend(osTaskFindByName("HISTOGRAM"));
//					osTaskSuspend(osTaskFindByName("AAA"));										
					menuPreviewInit(key);
					break;

				case UI_KEY_FUNC_MENU:
					menuReturn(UI_MAINMENU, 0);
					break;

				case UI_KEY_FUNC_A:
					aaaAeawbModeSet(0);
					xdcfActiveDevIdGet(&storage);					
					if((storage != DRIVE_NAND) && isCardProtect())
					{
						MsgShow(card_protect_s); //for bug 10 tzw add 060420
					//	hwWait(0, 1000);		
//						return ;	
						break; 
					}
					if(DisplayPVIcon(1))
					 { 
					    //continue ;  fqdao_bug for 17  06.4.22
					 }		  
				
					//if (cardExist == 1) fqdao_bug for 27  06.4.22
					if(sPara.StorageModeFlag != NandFlash)
				    {
					 	vfsDevInfoGet(DRIVE_SD, 0, &pdevInfofree);
					}
					else
					{
						vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfofree);
                    }
					if((pdevInfofree.nrFreeBytes/1024)<500)
					{
						 
						MsgShow(memory_full_s);
						aaaAeawbModeSet(0);
					//	hwWait(0, 1000);
					//	return;
					//	key = UI_KEY_MODE_PREVIEW;
						
					//	continue; 
						break;
					}
			
					if(sPara.snapFlag == SnapSingle)	/* JPG,	SINGLE */
					{
						osdBarDraw(0, osdH-32, osdW, 32, 0x00);
						snapOne();
						if (snapErrGet() == LIBDCF_MISC_ERR0003)
						{
							printf("Joe: disk is full\n");
						}
					}
					else if(sPara.snapFlag == SnapMulti)	/* JPG,	MULTI */
					{
						UINT8 snapOpt, noShots;

						sysgSnapShotNoGet(&snapOpt, &noShots);
						snapOpt = SNAP_VLC;
						sysgSnapShotNoSet(snapOpt, noShots);
						if (snapOpt == SNAP_S2S)
						{
							osdBarDraw(0, osdH-32, osdW, 32, 0x00);							
							snapOne();
						}
						else
						{
							osdBarDraw(0, osdH-32, osdW, 32, 0x00);
							snapMulti();
						}
					}
					else if(sPara.snapFlag == SnapSelf)	/* Self Snap */
					{
						printf("SnapSelf \n");						
						if (selfTimeDownCount(ui8DownTimeIdx) == true)
						{
							snapOne();
						}
						printf("Snap finish \n");
						gpioSet(GPIO_SELF_LED, 0);						
					}						
					/* Return to SnapSingle */
					sPara.snapFlag = SnapSingle;	/* Paul@200/10/26 */					
                                 DisplayPVIcon(1); 
                                			  
					aaaAeawbModeSet(0);
					/* Clear Key */					
					break;

				case UI_KEY_MODE_PREVIEW:
					hwDispOsdEn(0);							
					hwDispPvEn(0);

					#if 1				
					err	= xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);					
					xdcfCurRootDirSet(imageRootNameStr);					
					err	= xdcfFilterSet(xDCF_FILTER_JPG);
					err = exifInit(exifTagInsCallback, exifTagRmvCallback);
					#endif
					
					osTaskResume(osTaskFindByName("HISTOGRAM"));
					osTaskResume(osTaskFindByName("AAA"));						
					osdClearScreen(0x00);												
					PNL_PreviewSet();

					/* Paul@2006/06/16 add start */
					if (ManualWbFlag == WBAuto)
					{
						hwCdspWbOffsetSet(0xfd,0x01,0xfd,0x00);
						hwCdspWbGainSet(0x53,0x42,0x45,0x40);						
					}
					/* Paul@2006/06/16 add end */					

//					hwCdspManuOBSet(1, 5);
					movieSpecialEffectSet(sPara.EffectFlag);					
					
					zoomFactSet(zoomFactGet());
					hwPVZoom(pvZoomFactor);
					sysgSnapScaleFactorSet(pvZoomFactor);
					aaaEvCmpIdxSet(pvEvCmpIdx);
					SnapImageSizeSet();

					lcdOsdDisp = 0;	
					DisplayPVIcon(1) ;
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
		
		if (((uiState & UI_MODE_MASK ) != UI_KEY_MODE_PREVIEW))
		{			
			break;
		}

		TestFunc();
		
		keyGet(&key);
	}
	
	pvZoomFactor = 100;		
	hwPVZoom(pvZoomFactor);

	osTaskSuspend(osTaskFindByName("AAA"));					
	
//	lcdOsdDisp = 2;
//	osdLcdDisp(5);	/* Paul@2005/10/03 */						
	hwDispPvEn(0);
	PNL_PlaybackSet();
	
#if  SENSOR_PWR_SET
	gpioConfig(GPIO_SENSOR_POWER, 1);
	gpioSet(GPIO_SENSOR_POWER, 0);	
#else	
	gpioConfig(GPIO_SENSOR_PWDN, 1);
	gpioSet(GPIO_SENSOR_PWDN, 1);	
#endif

	flashmode = FlashForceOff;				/* Paul@2005/11/03-11:21 add */
	zoomFactSet(100);				/* Paul@2005/11/07--15:50 add */	
	hwWait(0, 10);	
	PanelColorSet();

	uiState = ui32NextState;

	UI_OSQFlush(uiKeyMsgQ);
		
	return;
}
#endif

#if 0
/**
	Paul@2005/11/24-08:13 add
**/
UINT32 OnChargeFlash(void)
{
	UINT8  flashChargeDelay;	/* sun@1021 PM 14:31*/	
	UINT32* msg;
	UINT32 err;
	
	flashChargeDelay = 0;						
//	osdStrDisp(0, 64, UserIcon, 0xd0, "\x12\x13");
	DisplayFlashIcon();			/* Paul@2005/11/24-08:45 add */
	gpioConfig(GPIO_CHG_PWM,1);
	
	hwFmGpioCfgSet(0,0,0,0x100);	/* Set GPIO_CHG_READY Input */

	gpioSet(GPIO_CHG_PWM,1);
	printf("charge start\n");
	OpenLcdDisplay(0);
	while(gpioGet(GPIO_CHG_READY))
	{	
		gpioSet(GPIO_BUSY_LED,1); 
		DisplayFlashIcon();			/* Paul@2005/11/24-08:45 add */
		osTimeDly(50);
		osdStrDisp(0, 64, UserFont, 0x00, "  ");
		gpioSet(GPIO_BUSY_LED,0);
		msg = osQuePend(uiKeyMsgQ, 50, &err);
		if((*msg == UI_KEY_DIR_LEFT)
		|| (*msg == UI_KEY_ACCIDENT))		
		{
			osQuePost(uiKeyMsgQ, msg);	
			break;
		}
		/* Paul@2005/11/03-10:54 add*/									
		if(*msg & UI_KEY_MODE_MASK)	
		{
			osQuePost(uiKeyMsgQ,&keyRotary[((*msg & UI_KEY_MODE_MASK)>>24)]);	
			break;	
		}
		/* Paul@2005/11/03-10:54 end */				
		if(flashChargeDelay > 14)
		{
			break;/**/
		}
		flashChargeDelay++;						
	}
	gpioSet(GPIO_CHG_PWM,0);	
	
	if ((lcdOsdDisp != 2)  &&  (getCurPanel() == PANEL_TYPE_1))
	{
		OpenLcdDisplay(1);						
	}

//	osdStrDisp(0, 64, UserIcon, 0xd0, "\x12\x13");	      						
	DisplayFlashIcon();			/* Paul@2005/11/24-08:45 add */
	printf("charge end\n");

	return 0;
}
#endif

/**
**/
void DisplayZoomIcon(UINT16 flag, UINT32 value)
{
	UINT32 osdW, osdH;
	UINT8 ui8ZoomStr[8];

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	
	if (flag)
	{	/* Display Zoom Icon */
		#if 1  //fqdao_add for clean "memory_full_s"
		osdBarDraw(120, 120, 120, 20, 0x00) ;
		#endif	
		sio_psprintf(ui8ZoomStr, "%01d.%02d", value/100, value%100);
		osdStrDisp((osdW-6*10)/2, (osdH-16)/2, UserIcon10x20, 0xd0, "\x10\x11");
		osdStrDisp((osdW-6*10)/2+2*10, (osdH-16)/2, UserFont10x20, 0xd0, ui8ZoomStr);		
	}
	else
	{	/* Clear Zoom Icon */
		osdStrDisp((osdW-6*10)/2, (osdH-16)/2, UserFont10x20, 0xd0, "      ");
	}
	return;
}
/**
**/
void DisplayFlashIcon(void)
{
	switch (flashmode)
	{
		case FlashAuto :
			osdStrDisp(0, 64, UserIcon, 0xd0, "\x12\x13");												
			break;

		case FlashForceOff :
			osdStrDisp(0, 64, UserIcon, 0xd0, "\x14\x15");																
			break;

		case FlashForceOn :
			osdStrDisp(0, 64, UserIcon, 0xd0, "\x16\x17");																
			break;

		case FlashRedEye :
			osdStrDisp(0, 64, UserIcon, 0xd0, "\x18\x19");																
			break;				
	}
}



/**
**/

UINT32 DisplayPVIcon(UINT8 isClear)
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
	if (isCardExist() && (sPara.StorageModeFlag == Card))
	{
		printf("card\n");
		vfsDevInfoGet(DRIVE_SD, 0, &pdevInfo);
	}
	else
	{
		printf("flash\n");		
		vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfo);
	}
/*
	if (pdevInfo.fsDepend == 0)
	{
		vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfo);
	}
*/	
	switch (sPara.snapFlag)
	{
		case SnapSingle:
		{
			osdStrDisp(0, 0, UserIcon, 0x10, "\x02\x03");
			osdBarDraw((osdW-64)/2, 32, 64, 32, 0x00);
			break;
		}
		case SnapMulti:
		{
			osdStrDisp(0, 0, UserIcon, 0x10, "\x20\x21");
			break;
		}
		case SnapSelf:
		{
			osdStrDisp(0, 0, UserIcon, 0x10, "\x02\x03");
			osdStrDisp((osdW-64)/2, 32, UserIcon, 0x30, "\x32\x33");
			sio_psprintf(str, "%02d", ui8DownTime[ui8DownTimeIdx]);
			osdStrDisp(((osdW-64)/2)+32, 40, UserFont16x20, 0x30, str);
			break;
		}			
	}		

	printf(" cluster size = %d\n", pdevInfo.fsDepend);
	printf(" free bytes = %x\n", pdevInfo.nrFreeBytes);
	printf(" free cluster = %d\n", pdevInfo.nrFreeBytes/pdevInfo.fsDepend);
	
	ImageSizeSelect(&size);
	printf("size %d \n",size/1024/16);
	/*size = size / 1024;*/
	#if 0   // tzw add 060315
	if(size%pdevInfo.fsDepend)
	{
		size = size/pdevInfo.fsDepend + 1;
	}
	else
	{
		size = size/pdevInfo.fsDepend;
	}

	printf("size/pdevInfo.fsDepend %d \n",size);	
	free = (pdevInfo.nrFreeBytes/pdevInfo.fsDepend)/size;
	#endif
	if((pdevInfo.nrFreeBytes/1024)>500)//fqdao_Add for bug 06.6.2
	{
		free = pdevInfo.nrFreeBytes / size;	// tzw add 060315
	}
	else
	{
		free =0 ;
	}
		
 	freeflag = free;  
	/*herb for image size limitation @03/12/15 end*/
	if (free > 999999)
	{
		free = 999999;
	}
	#if 0  // tzw add 060315
	xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
	xdcfCurRootDirSet(imageRootNameStr);
	xdcfFilterSet(xDCF_FILTER_JPG | xDCF_FILTER_OURTYPE);					
	#endif 
	xdcfFileCountGet(&fileCnt);//machiner add 2006-01-23 for display fileCnt
	
	sio_psprintf(str, "[%d]", free);
	printf("free = %d\n", free);
	osdStrDisp(osdW-8*10, osdH-20, UserFont10x20, 0xd0, str);

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

	sysgSnapSizeGet(&width, &height);			
	if (width == 1600)
	{
		osdStrDisp(8*10+32, osdH-20, UserFont10x20, 0xd0, "1600"/*"6M"*/);
	}		
	else if (width == 1280)
	{
		osdStrDisp(8*10+32, osdH-20, UserFont10x20, 0xd0, "1280"/*"6M"*/);
	}		
	else if (width == 1024)	
	{
		osdStrDisp(8*10+32, osdH-20, UserFont10x20, 0xd0, "1024"/*"3M"*/);
	}
	else if (width == 800) 
	{
		osdStrDisp(8*10+32, osdH-20, UserFont10x20, 0xd0, "800"/*"2M"*/);
	}
	else if (width == 640)
	{
		osdStrDisp(8*10+32, osdH-20, UserFont10x20, 0xd0, "640"/*"0.3M"*/);
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

#if 0
	/* Flash Mode */
	switch(flashmode)
	{
		case FlashAuto :
			osdStrDisp(0, 64, UserIcon, 0xd0, "\x12\x13");												
			break;

		case FlashForceOff :
			osdStrDisp(0, 64, UserIcon, 0xd0, "\x14\x15");																
			break;

		case FlashForceOn :
			osdStrDisp(0, 64, UserIcon, 0xd0, "\x16\x17");																
			break;

		case FlashRedEye :
			osdStrDisp(0, 64, UserIcon, 0xd0, "\x18\x19");																
			break;				
	}
#endif	


#if  0
	static UINT32  savestr = 1 ;
	osdStrDisp(10, 100, UserFont, 0x60, "内存大小");
	osdStrDisp(100, 100, UserFont,0x60," 虚拟大小");
	osdStrDisp(200, 100, UserFont,0x60," 实际大小");
	sio_psprintf(str," %u" , pdevInfo.nrFreeBytes/1024 );
	osdStrDisp(10, 140, UserFont10x20, 0xa0, str);
	sio_psprintf(str," %u *16=%u" , size/1024/16,size/1024);
	osdStrDisp(110, 140, UserFont10x20, 0xa0, str);
	sio_psprintf(str," %u" ,savestr>(pdevInfo.nrFreeBytes/1024 )? (savestr-(pdevInfo.nrFreeBytes/1024 )):((pdevInfo.nrFreeBytes/1024 )-savestr));
	osdStrDisp(210, 140, UserFont10x20, 0xa0, str);
	savestr =pdevInfo.nrFreeBytes/1024  ;
#endif
/*		
	if (ui32SceneFlag == SceneClose)
	{
		osdStrDisp(32, osdH-20, UserFont, 0x00, "  ");
		osdStrDisp(32, osdH-20, UserIcon, 0xd0, "\x52");
	}
	else
	{
		osdStrDisp(32, osdH-20, UserIcon, 0xd0, "\x4e\x4f");
	}
*/
	DisplayCaptureEv(pvEvCmpIdx);
	/*
	osdVProgressDraw(0, 10, 10-pvEvCmpIdx, 0x7c);
	osdHProgressDraw(100, 400, pvZoomFactor, 0x7c);
	*/
	if (isClear)
	{
		osdHWBuffChange();
	}	
	// John @02/06/2006
	if (!free  )
	{
		//MsgShow(memory_full_s);   
		return 1;
	}
	
	return 0 ;
	
}

#define C_FOCUS_ICON_X		(PANEL_1_OSD_W)/2		//(PANEL_1_OSD_W-16)/2		
#define C_FOCUS_ICON_Y		(PANEL_1_OSD_H)/2//(PANEL_1_OSD_H-20)/2
/**
	Paul@2005/11/17-17:19 add 
**/
void DisplayFocusIcon(UINT8 flag)
{
	if (flag)
	{
		osdStrDisp(C_FOCUS_ICON_X-16-20, C_FOCUS_ICON_Y-20 -20, UserIcon16x20, 0x10, "\x38");
		osdStrDisp(C_FOCUS_ICON_X + 20, C_FOCUS_ICON_Y-20 -20, UserIcon16x20, 0x10, "\x39");		
		osdStrDisp(C_FOCUS_ICON_X-16-20, C_FOCUS_ICON_Y+20, UserIcon16x20, 0x10, "\x3A");		
		osdStrDisp(C_FOCUS_ICON_X +20, C_FOCUS_ICON_Y+20, UserIcon16x20, 0x10, "\x3B");		
	}
	else /* Clear */
	{
		osdBarDraw(C_FOCUS_ICON_X-16-30, C_FOCUS_ICON_Y-20-30, 92, 100, 0x00);
	}
}

extern UINT32 atoCounter;
/**
	Paul@2005/11/18-08:39 add 
**/
bool selfTimeDownCount(UINT8 ui8TimeIndex)
{
	UINT32	osdW, osdH;
	UINT8 str8[3];
	UINT32 selfTimeCount;
	UINT32 StartTime, EndTime, err;
	UINT32* msg;
	
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	
	StartTime = EndTime = tmrMsTimeGet();
	selfTimeCount = ui8DownTime[ui8TimeIndex];	

	/* Paul@2005/10/08 for Self Shot */
	gpioConfig(GPIO_SELF_LED, 1);
	gpioSet(GPIO_SELF_LED, 1);
	while (selfTimeCount)
	{		
		if (abs(EndTime-StartTime) > 1000)
		{
			selfTimeCount--;
			sio_psprintf(&str8[0], "%02d", selfTimeCount);
			osdStrDisp(((osdW-64)/2)+32, 40, UserFont16x20, 0x30, &str8[0]);			
			StartTime = EndTime;
		}
		
		EndTime = tmrMsTimeGet();
		if(selfTimeCount > 3)
		{
			msg = osQuePend(uiKeyMsgQ, 50, &err);
		}
		else
		{
			msg = osQuePend(uiKeyMsgQ, 25, &err);
		}
		
		if (gpioSetSts(GPIO_SELF_LED))
		{
			gpioSet(GPIO_SELF_LED, 0);			
		}
		else
		{
			gpioSet(GPIO_SELF_LED, 1);
/*			PlaySoundEffect(SELF_TIME);			*/
			PlayEffectSound(1);   //tzw add for bug 8
		}
		
		atoCounter = 0;		
		if (err == OS_TIMEOUT)
		{ 
			continue;
		}
		
		if ((*msg & UI_MODE_MASK)
		  ||(*msg == UI_KEY_ACCIDENT))
		{
			osQuePost(uiKeyMsgQ, msg);
			break;
		}		
	}
	gpioSet(GPIO_SELF_LED, 0);						
	osdBarDraw((osdW-64)/2, 32, 64, 32, 0x00);	

	if (selfTimeCount != 0)
	{
		return false;
	}

	return true;
}
/*
#define DC_FRAME_W 	(((PANEL_1_OSD_W - (3+1)*8)/3) & 0xFF0) + 4
#define DC_FRAME_H 	((PANEL_1_OSD_H - 8-(3+1)*8)/3)	+ 4	
#define DC_FRAME_X 	(((PANEL_1_OSD_W - 3*DC_FRAME_W) /(3+1)) & 0xFFF8)
#define DC_FRAME_Y 	((PANEL_1_OSD_H - 8 - 3*DC_FRAME_H) /(3+1))
*/

#define DC_FRAME_W 	(((PANEL_1_OSD_W - (3+1)*8)/3) & 0xFF0) + 3
#define DC_FRAME_H 	((PANEL_1_OSD_H - 18-(3+1)*8)/3)	+ 3	
#define DC_FRAME_X 	(((PANEL_1_OSD_W - 3*DC_FRAME_W) /(3+1)) & 0xFFF8) -2
#define DC_FRAME_Y 	((PANEL_1_OSD_H - 18 - 3*DC_FRAME_H) /(3+1)) - 2
/*
	pbThumbW = ((dispW-(3+1)*8)/3) & 0xFF0;
	pbThumbH = (dispH-8-10-(3+1)*8)/3;		
	pbThumbOffX = (((dispW - 3*pbThumbW) /(3+1)) & 0xFFF8);
	pbThumbOffY = ((dispH - 8 - 10 - 3*pbThumbH) /(3+1));
*/
/**
	ui32FileIdx :
	flag        : 0 = Update Multi Image
				  1 = Check Update Multi Image		
**/
UINT32 dc_BrowseImage(UINT32 ui32FileIdx, UINT8 flag)
{
	UINT32 err;
	UINT8  str[24];

	g_ui32ZoomFactor = 100;
	hwPVZoom(g_ui32ZoomFactor);
	sysgSnapScaleFactorSet(g_ui32ZoomFactor);													

	zoomFactSet(g_ui32ZoomFactor);	// David 12/29/2005	
	
	if (g_ui32DispMode == PB_DISP_ONE)
	{
		osdStrDisp(0, 0, UserIcon16x20, 0x40, "\x04\x05");	/* Playback icon */										
		sio_psprintf(str, "%04d", ui32FileIdx);
		osdStrDisp((PANEL_1_OSD_W-10*10), 0/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);
		sio_psprintf(str, "\-%04d", fileCnt);
		osdStrDisp((PANEL_1_OSD_W-6*10), 0/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);			

		return dc_DisplayImage(ui32FileIdx);		
	}
	else if (g_ui32DispMode == PB_DISP_NINE)
	{
#if 0	/* Paul@2006/05/30 add for DownLoad picture*/	
		if (flag == 0)
		{
			dc_DisplayMultiImage(ui32FileIdx);
		}
		else
		{
			if ((ui32FileIdx >= (g_ui32DispStart+9))
			 || (ui32FileIdx < g_ui32DispStart))
			{
				dc_DisplayMultiImage(ui32FileIdx);
			}
		}		
		
		UINT32 i, j;		

		err = ui32FileIdx - g_ui32DispStart;
		i = err/3;
		j = err%3;

		osdBarDraw(0, DC_FRAME_Y+16, PANEL_1_OSD_W, PANEL_1_OSD_H-DC_FRAME_Y-16, 0x00);						
		osdStrDisp(0, 0, UserIcon16x20, 0x40, "\x04\x05");	/* Playback icon */										
		sio_psprintf(str, "%04d", ui32FileIdx);
		osdStrDisp((PANEL_1_OSD_W-10*10), 0/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);
		sio_psprintf(str, "\-%04d", fileCnt);
		osdStrDisp((PANEL_1_OSD_W-6*10), 0/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);			
		
		osdFrameDrawX(DC_FRAME_X+(DC_FRAME_W+DC_FRAME_X)*j, DC_FRAME_Y+(DC_FRAME_Y+DC_FRAME_H)*i+16, DC_FRAME_W - 2, DC_FRAME_H, 0x03, 2);		
#else
		UINT32 i, j;		
		
		osdBarDraw(0, DC_FRAME_Y+16, PANEL_1_OSD_W, PANEL_1_OSD_H-DC_FRAME_Y-16, 0x00);						
		dc_DisplayMultiImage(ui32FileIdx, flag);		

		err = ui32FileIdx - g_ui32DispStart;
		i = err/3;
		j = err%3;

		osdStrDisp(0, 0, UserIcon16x20, 0x40, "\x04\x05");	/* Playback icon */										
		sio_psprintf(str, "%04d", ui32FileIdx);
		osdStrDisp((PANEL_1_OSD_W-10*10), 2/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);
		sio_psprintf(str, "\-%04d", fileCnt);
		osdStrDisp((PANEL_1_OSD_W-6*10), 2/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);					
		
		osdFrameDrawX(DC_FRAME_X+(DC_FRAME_W+DC_FRAME_X)*j, DC_FRAME_Y+(DC_FRAME_Y+DC_FRAME_H)*i+16, DC_FRAME_W - 2, DC_FRAME_H, 0x03, 2);				

#endif		
	}
	
	return 0;
}

/**
	Paul@2005/11/18-13:38 add
**/
void dc_PlaybackImage(void)
{
	UINT32 key = UI_KEY_MODE_PLAYBACK;
	UINT32 err;
	UINT32 dispW, dispH;
	UINT32* msg;
	UINT32 ui32InPlayback = 1;
	
	fileCnt = 1;
	fileIdx = 1;

	g_ui32ZoomFactor = 100;
	
	while ((uiState & UI_MODE_MASK) == UI_MODE_PREVIEW)
	{	
		if (isMenuOn)
		{
			menuPBImageProc(key);
			if (isMenuOn == 0)
			{
				key = UI_KEY_MODE_PLAYBACK;
			}
		}
		if (isMenuOn == 0)
		{
			switch (key)
			{
				case UI_KEY_FUNC_A:
				case UI_KEY_ACCIDENT:
				case UI_KEY_FUNC_MENU:
					/* Exit to still preview */
					pbInit();
					ui32InPlayback = 0;
					break;;
					
				case UI_KEY_DIR_UP:
					if (fileCnt == 0)
					{
						break;
					}
					
					if (g_ui32DispMode == PB_DISP_ONE)
					{						
						if (fileIdx == 1)
						{
							fileIdx = fileCnt;
						}
						else
						{
							fileIdx--;
						}
					}
					else /* PB_DISP_NINE */
					{						
						if (fileCnt <= 3)
						{
							break;
						}						
						
						if (fileIdx <= 3)
						{
							if ((fileCnt-1)%3+1 <= fileIdx)
							{
								fileIdx = fileCnt;								
							}
							else
							{
								fileIdx = (fileCnt-1)/3*3 + fileIdx;
							}
						}
						else
						{
							fileIdx -= 3;
						}
					}
					dc_BrowseImage(fileIdx, 1);
					break;
					
				case UI_KEY_DIR_DOWN:
					if (fileCnt == 0)
					{
						break;
					}
					
					if (g_ui32DispMode == PB_DISP_ONE)
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
					else /* PB_DISP_NINE */
					{
						if (fileCnt <= 3)
						{
							break;
						}						
						
						if ((fileIdx+3) > fileCnt)
						{
							fileIdx = (fileIdx-1)%3+1;
						}
						else
						{
							fileIdx += 3;
						}
					}
					dc_BrowseImage(fileIdx, 1);				
					break;
#if 0					
				case UI_KEY_FUNC_ZOOMIN:
					#if 1
					if ((g_ui32DispMode == PB_DISP_NINE)
					  ||(fileCnt == 0))
					{
						break;					
					}
					do
					{
						if (g_ui32ZoomFactor < 400)
						{		
							g_ui32ZoomFactor += 5;
						}
						hwPVZoom(g_ui32ZoomFactor);
						sysgSnapScaleFactorSet(g_ui32ZoomFactor);													
						DisplayZoomIcon(1, g_ui32ZoomFactor);
						osTimeDly(2);						
						msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);
					}while((uiKeyPush) && (!((*msg != UI_KEY_FUNC_ZOOMIN) && (err == 0))));
					DisplayZoomIcon(0, g_ui32ZoomFactor);
					#endif
					break;

				case UI_KEY_FUNC_ZOOMOUT:
					#if 1
					if ((g_ui32DispMode == PB_DISP_NINE)
					  ||(fileCnt == 0))
					{
						break;					
					}				
					do
					{
						if (g_ui32ZoomFactor > 100)
						{
							g_ui32ZoomFactor -= 5;
						}
						hwPVZoom(g_ui32ZoomFactor);
						sysgSnapScaleFactorSet(g_ui32ZoomFactor);												
						DisplayZoomIcon(1, g_ui32ZoomFactor);
						osTimeDly(2);
						msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);
					}while((uiKeyPush) && (!((*msg != UI_KEY_FUNC_ZOOMOUT) && (err == 0))));
					DisplayZoomIcon(0, g_ui32ZoomFactor);					
					#endif
					break;
#endif
				case UI_KEY_DIR_LEFT:
					if (fileCnt == 0)
					{
						break;
					}
					
					if (g_ui32DispMode == PB_DISP_NINE)
					{
						if (fileIdx == 1)
						{
							fileIdx = fileCnt;
						}
						else
						{
							fileIdx--;
						}
						dc_BrowseImage(fileIdx, 1);
					}										
					else
					{
						do
						{
							if (g_ui32ZoomFactor > 100)
							{
								g_ui32ZoomFactor -= 5;
								hwPVZoom(g_ui32ZoomFactor);
								sysgSnapScaleFactorSet(g_ui32ZoomFactor);																				
							}
							//hisTimeDisp();							
							DisplayZoomIcon(1, g_ui32ZoomFactor);
							osTimeDly(2);
							msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);
						}while((uiKeyPush) && (!((*msg != UI_KEY_DIR_LEFT) && (err == 0))));
						DisplayZoomIcon(0, g_ui32ZoomFactor);											
					}
					break;
					
				case UI_KEY_DIR_RIGHT:
					if (fileCnt == 0)
					{
						break;
					}
					
					if (g_ui32DispMode == PB_DISP_NINE)
					{
						if (fileIdx == fileCnt)
						{
							fileIdx = 1;
						}
						else
						{
							fileIdx++;
						}				
						dc_BrowseImage(fileIdx, 1);
					}					
					else
					{
						do
						{
							if (g_ui32ZoomFactor < 400)
							{		
								g_ui32ZoomFactor += 5;
								hwPVZoom(g_ui32ZoomFactor);
								sysgSnapScaleFactorSet(g_ui32ZoomFactor);																				
								
							}
						//	hisTimeDisp();
							DisplayZoomIcon(1, g_ui32ZoomFactor);
							osTimeDly(2);						
							msg = osQuePend(uiKeyMsgQ, TIME_OUT, &err);
						}while((uiKeyPush) && (!((*msg != UI_KEY_DIR_RIGHT) && (err == 0))));						
						DisplayZoomIcon(0, g_ui32ZoomFactor);						
					}
					break;

				case UI_KEY_FUNC_OK:
					if (fileCnt == 0)
					{
						break;
					}
					
					if (g_ui32DispMode == PB_DISP_ONE)
					{
						pbInit();		/* Must */						
						g_ui32DispMode = PB_DISP_NINE;
						dc_BrowseImage(fileIdx, 0);
					}
					else
					{
						pbInit();		/* Must */
						osdClearScreen(0x00);
						g_ui32DispMode = PB_DISP_ONE;						
						dc_BrowseImage(fileIdx, 1);
					}
					break;				

				case UI_KEY_FUNC_B:
					if (fileCnt == 0)
					{
						break;
					}
					
					if(g_ui32ZoomFactor^100)
					{
						g_ui32ZoomFactor = 100;
						hwPVZoom(g_ui32ZoomFactor);
						sysgSnapScaleFactorSet(g_ui32ZoomFactor);													
					}
					pbInit();
					menuPBImageInit(key);
					break;
					
				case UI_KEY_MODE_PLAYBACK:
					osTaskSuspend(osTaskFindByName("AAA"));
					osTaskSuspend(osTaskFindByName("HISTOGRAM"));				

					g_ui32DispMode = PB_DISP_ONE;
					PNL_PlaybackSet();
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

//					err	= xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
//					xdcfCurRootDirSet(DCIMRootNameStr);										
					err	= xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
					xdcfCurRootDirSet(imageRootNameStr);

					err	= xdcfFilterSet(xDCF_FILTER_JPG | xDCF_FILTER_OURTYPE);				
					
					xdcfFileCountGet(&fileCnt);				
					if (fileCnt == 0)
					{
						uiDispBuffFill(0x0000);
						osdStrDisp((PANEL_1_OSD_W-8*10)/2-1, (PANEL_1_OSD_H-20)/2, UserFont10x20, 0x10, (UINT8 *)dc_GetString(dc_NoImage_s));
						hwDispPvEn(1);				
						hwDispOsdEn(1);

						/* Paul@2006/12/24 - 15:44 add start */
						PanelColorSet();
						/* Paul@2006/12/24 - 15:44 add end */					
						
						fileIdx = 0;
						break;
					}
					else
					fileIdx = fileCnt;						

					zoomFactSet(g_ui32ZoomFactor);
					
					dc_BrowseImage(fileIdx, 0);
					hwDispPvEn(1);				
					hwDispOsdEn(1);	

					/* Paul@2006/12/24 - 15:44 add start */
					PanelColorSet();
					/* Paul@2006/12/24 - 15:44 add end */					
					break;					
			}
		}

		if ((uiState & UI_MODE_MASK) != UI_MODE_PREVIEW
		  ||(ui32InPlayback == 0))
		{
			break;
		}		
		keyGet(&key);
	}

	g_ui32ZoomFactor = 100;
	zoomFactSet(g_ui32ZoomFactor);	
}

/**
**/
UINT32 dc_DisplayImage(UINT32 ui32FileIdx)
{
	UINT32 err;
	UINT32 dispW, dispH;
	UINT32 ui32FileType;
	UINT8  str[24];
	xdcfAttrElm_t xFileAttr;
	xdcfTime_t xTime;
		
	hwDispPreviewSizeGet(&dispW, &dispH);		
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/
	
	if ((err = xdcfCurFileByPosSet(ui32FileIdx)) != SUCCESS)
	{
		return 1;
	}	

	if ((err = xdcfCurFileAttrGet(&xFileAttr)) != SUCCESS)
	{
		return 1;
	}
	xTime = xFileAttr.time;

	// icon of protect file      
	if (xFileAttr.attribute & xDCF_ATTR_READONLY)
	        osdStrDisp(38, 8, IconLock, 0x10,"\x1");      
	else
	   		osdStrDisp(38, 8, IconLock, 0x0," ");
	
	osdBarDraw(0, (PANEL_1_OSD_H-20), PANEL_1_OSD_W, 20, 0x00);
	osdStrDisp(0, (PANEL_1_OSD_H-20), UserFont10x20, 0x30, xFileAttr.name);
	sio_psprintf(str, "%04d\x2F%02d\x2F%02d\x2F%02d\x2F%02d",
				xTime.year,
				xTime.mon,
				xTime.day,
				xTime.hour,
				xTime.min
				);
/*	fqdao_modify for bug 06.5.22	
	osdStrDisp(10*14, (PANEL_1_OSD_H-20), UserFont10x20, 0x30, str);
*/
	err	= xdcfFileTypeGet(ui32FileIdx, &ui32FileType);	
	if (ui32FileType == xDCF_FILETYPE_JPG)
	{
		exifImageInfo_t *pInfo = 0;
		
		pbImageGet(0, ui32FileIdx, gPB, dispW, dispH, 1, 0x800000); /* 0x800000 is bg color */			
		pbImageDecode(0, ui32FileIdx, &pInfo);
		sio_psprintf(str, "%4dx%4d", pInfo->w, pInfo->h);
		osdStrDisp(0, (PANEL_1_OSD_H-40), UserFont10x20, 0x30, str);		
	}
	else
	{
		printf("File error\n");
		return 1;
	}	

	UI_OSQFlush (uiKeyMsgQ);	// David 01/13/2006			
	return 0;	
}

#if 0	/* Paul@2006/05/30 add for DownLoad picture*/	
/**
	Paul@2005/11/11-13:42 add for display nine picture.
**/
void dc_DisplayMultiImage(UINT32 ui32FileIdx)
{
	UINT32 err;
	UINT32 i, j;
	UINT32 dispW, dispH;
	UINT32 tempIdx = 1;	
	UINT32 pbThumbW, pbThumbH, pbThumbOffX, pbThumbOffY;	
	UINT32 pPBAddr;

	g_ui32DispStart = ((ui32FileIdx-1)/(3*3))*(3*3)+1;

	osdClearScreen(0);
	
	hwDispPreviewSizeGet(&dispW, &dispH);		
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);	  
	hwCammodeSet(5);	  
	hwCdspDataSource(1);  
	
	pbThumbBufferGet(&pPBAddr);
	
	pubFill((UINT8 *)(0xAC000000+gPB*2), dispW*2, dispH,
		0, 0, dispW*2, dispH, 0x000000);
	
	pbThumbW = ((dispW-(3+1)*8)/3) & 0xFF0;
	pbThumbH = (dispH-8-10-(3+1)*8)/3;		
	pbThumbOffX = (((dispW - 3*pbThumbW) /(3+1)) & 0xFFF8);
	pbThumbOffY = ((dispH - 8 - 10 - 3*pbThumbH) /(3+1));


	
	for(i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
		{
			tempIdx = g_ui32DispStart + i * 3 + j;
			if(tempIdx <= fileCnt)
			{
				err	= pbImageGet(1,	tempIdx, pPBAddr, pbThumbW, pbThumbH, 0, 0); 
				if (err == SUCCESS)
				{
					hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
								gPB, dispW,	dispH,
								pbThumbOffX+j*(pbThumbOffX+pbThumbW),
								pbThumbOffY+i*(pbThumbOffY+pbThumbH)+20, 0);
				}
			}
		}
	}	
}
#else
/**
	Paul@2005/11/11-13:42 add for display nine picture.
**/
/* Paul@2006/05/30 add for DownLoad picture*/	
void dc_DisplayMultiImage(UINT32 ui32FileIdx, UINT32 flag)
{
	UINT32 err;
	UINT32 i, j;
	UINT32 dispW, dispH;
	UINT32 tempIdx = 1;	
	UINT32 pbThumbW, pbThumbH, pbThumbOffX, pbThumbOffY;	
	UINT32 pPBAddr;
	UINT8  strImgIndex[8];

	if (flag
	 &&(ui32FileIdx < (g_ui32DispStart+9))
	 &&(ui32FileIdx > g_ui32DispStart))		
	{
		for(i=0; i<3; i++)
		{
			for (j=0; j<3; j++)
			{
				tempIdx = g_ui32DispStart + i * 3 + j;
				if(tempIdx <= fileCnt)
				{
					sio_psprintf(strImgIndex, "%04d", tempIdx);
					osdStrDisp(DC_FRAME_X+4+(DC_FRAME_W+DC_FRAME_X)*j, DC_FRAME_Y+4+(DC_FRAME_Y+DC_FRAME_H)*i+16, UserFont10x20, 0xd0, strImgIndex);		
				}
			}
		}
		return;		
	}
	
	g_ui32DispStart = ((ui32FileIdx-1)/(3*3))*(3*3)+1;
	osdClearScreen(0);
	
	hwDispPreviewSizeGet(&dispW, &dispH);		
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);	  
	hwCammodeSet(5);	  
	hwCdspDataSource(1);  
	
	pbThumbBufferGet(&pPBAddr);
	
	pubFill((UINT8 *)(0xAC000000+gPB*2), dispW*2, dispH,
		0, 0, dispW*2, dispH, 0x000000);
	
	pbThumbW = ((dispW-(3+1)*8)/3) & 0xFF0;
	pbThumbH = (dispH-8-10-(3+1)*8)/3;		
	pbThumbOffX = (((dispW - 3*pbThumbW) /(3+1)) & 0xFFF8);
	pbThumbOffY = ((dispH - 8 - 10 - 3*pbThumbH) /(3+1));
	
	for(i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
		{
			tempIdx = g_ui32DispStart + i * 3 + j;
			if(tempIdx <= fileCnt)
			{
				err	= pbImageGet(1,	tempIdx, pPBAddr, pbThumbW, pbThumbH, 0, 0); 
				if (err == SUCCESS)
				{					
					hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
								gPB, dispW,	dispH,
								pbThumbOffX+j*(pbThumbOffX+pbThumbW),
								pbThumbOffY+i*(pbThumbOffY+pbThumbH)+20, 0);
				}
				sio_psprintf(strImgIndex, "%04d", tempIdx);
				osdStrDisp(DC_FRAME_X+4+(DC_FRAME_W+DC_FRAME_X)*j, DC_FRAME_Y+4+(DC_FRAME_Y+DC_FRAME_H)*i+16, UserFont10x20, 0xd0, strImgIndex);		
			}
		}
	}
	
}

#endif

extern UINT8 SpeakFlag;
/**
**/
#if 1
void PlayEffectSound(UINT32 index)
{
	#if (CurrentIC8751==0)	
        gpioConfig(GPIO_MUTE, 1);
        gpioSet(GPIO_MUTE, 0);
	#else	
	if(SpeakFlag)
		wm8751SoundEnable(2,1);
	else
		wm8751SoundEnable(2,1);
	#endif							
	#if (CurrentIC8751==0)	
		#if 1     //Danny   05-06-05
		hwAudAC97RegWrite(0x2c, 48000);		/* 8K Hz*/
		hwAudAC97RegWrite(0x2e, 48000);		/* 8K Hz*/
		hwAudAC97RegWrite(0x32, 48000);		/* 8K Hz*/
		#endif
		if(SpeakFlag)
		{	// Danny--05/06/06
 			pbSetVolume(ws9712_OUT2, 9);
		}
		else
		{
			pbSetVolume(ws9712_HP, 6);
		}
	#else
		wm8751LineOut(SpeakFlag);
		if(SpeakFlag)
		{
			
			pbSet8751Volume(ws9712_OUT2, 5);
		}
		else
		{
			pbSet8751Volume(ws9712_HP, 3);
		}
	#endif
		switch(index)
		{
			case 0:
				audRiffDramOneShotPlay(voicesht, sizeof(voicesht), 1);
				break;
			case 1:   //tzw add for bug 8
				audRiffDramOneShotPlay(Self_time,sizeof(Self_time),1);
				break;
				
			default:
				break;
		}

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
		
		#if (CurrentIC8751==0)	
	        gpioConfig(GPIO_MUTE, 1);
	        gpioSet(GPIO_MUTE, 1);
		#else		
			wm8751SoundEnable(0,0);
		#endif								

	
		#if (CurrentIC8751==0)	
	        gpioConfig(GPIO_MUTE, 1);
	        gpioSet(GPIO_MUTE, 0);
		#else		
			wm8751SoundEnable(0,0);
		#endif							
}
#endif

#if 0
void testFlash()
{
	UINT32 key = UI_KEY_MODE_PREVIEW;
	UINT32 err;
	UINT32 *msg;
	UINT32	osdW, osdH;
	UINT32 i;			/* Paul@2005/10/12 */
	UINT8 str8[3];
	UINT32 storage;	
	devInfo_t pdevInfofree;
	
	flashmode = FlashForceOn;
	OnChargeFlash();
	while(1)
	{
					aaaAeawbModeSet(1);
					/*Display Focus Icon */					
					DisplayFocusIcon(1);

	                   osTimeDly(5);
	
					/* Clear Focus Icon */
					DisplayFocusIcon(0);					
//					osTimeDly(5);			

					xdcfActiveDevIdGet(&storage);
					
					if (cardExist == 1)
					 {
					 	vfsDevInfoGet(DRIVE_SD, 0, &pdevInfofree);
						}
					else
						vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfofree);

					if(sPara.snapFlag == SnapSingle)	/* JPG,	SINGLE */
					{
						osdBarDraw(0, osdH-32, osdW, 32, 0x00);
						snapOne();
						if (snapErrGet() == LIBDCF_MISC_ERR0003)
						{
							printf("Joe: disk is full\n");
						}
					}
					/* Return to SnapSingle */
					sPara.snapFlag = SnapSingle;	/* Paul@200/10/26 */					
					DisplayPVIcon(1);
					aaaAeawbModeSet(0);
					/* Clear Key */
					



					{
						OnChargeFlash();
					}					
		}
}
#endif 
