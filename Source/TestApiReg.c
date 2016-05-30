/*	sun@1030 am 11:25 add
	For the test ap
	DDX  Technology Co., Ltd.                                      
	2 floor R2-B building ShenZhen High-Tech Industria Park Of China.  
	 Author: Sun Chen  
*/
// #define	  RS_232_DBG
 
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "stdio.h"
#include "os_api.h"
#include "ui_main.h"
#include "hw_disp.h"
#include "hw_disp_ext.h"
#include "osd_common.h"
#include "osd_api.h"
#include "xiaoao.h"
#include "panel.h"
#include "gpio.h"
#include "hw_sdram.h"
#include "dsc_init.h"
#include "hw_cdsp.h"
#include "hw_front.h"
#include "zoom.h"
#include "aaa_api.h"
#include "snap_custom.h"
#include "errcode.h"
#include "usb_api.h"
#include "sysg_api.h"
#include "snap_api.H"
#include "timer.h"
#include "dcf_api.h"
#include "vfs_api.h"
#include "misc_api.h"
#include "hw_cpu.h"
#include "Audio_Api.h"
#include "exif_api.h"
#include "Menu_capture.h"
#include "Timer.h"

/* Paul@2006/04/21 add start */
#include "ws9712.h"
#include "asf_api.h"

enum{
	MP4_PROGRESS_VOL = 0,
	MP4_PROGRESS,
	MP4_PLAY_STATUS	
};

enum	
{
	PROGRESS_RST= 0,
	PROGRESS_MOV,
	PROGRESS_VOL,
	MP3_PLAY_STATUS,
	MP3_EQ_DIS,
	MP3_EQ_RST,
	MP3_LRC,
	MP3_LOADING
};


extern UINT8 SpeakFlagMode;
extern UINT8 pressVolume;
extern UINT8 SpeakFlag;
extern UINT8 g_playrecStatus;
extern UINT8 uiOnBusy;

extern UINT8 DCDirNameStr[];
extern UINT8 DVDCRootNameStr[];
extern UINT8 DCNameStr[];
extern UINT8 mp3SubDirMP3Str[];
extern UINT8 mediaRootNameStr[];
extern UINT8 VideoRootNameStr[];
extern UINT32 ChgPvSetting;
extern OS_EVENT* semApp;
/* Paul@2006/04/21 add end */


extern UINT32 exifTagInsCallback(void);
extern UINT32 exifTagRmvCallback(void);

extern UINT8 menuOV3620_OBCalibration(void);


extern OpenLcdDisplay(UINT32 flag);

#define FW_VER_MAIN	    1
#define FW_VER_MEDI		1
#define	FW_VER_MINOR	1

#define frequence60Hz	1
#define frequence50Hz	0

UINT8 G_TestFuncFlag = 0;
UINT8 G_TestFuncIndex = 0;

UINT8  ui8EnterPowerDownVoltage = 0x00;	//xqq add for test voltage   
extern UINT8 atoEnable;
void TestFuncFlagSet( UINT8 testfuncflag,UINT8 testfuncindex )
{
	
	G_TestFuncFlag= testfuncflag;
	G_TestFuncIndex=testfuncindex;
	atoEnable=0;
}


char * testmenu[]=
{
	" Test Voltage(OK) ",
	" Burn In ",
	" Adjust Voltage ",
};

void ShowVersion(void)
{
/*
	UINT8 str[14];
	UINT8 Vermain = FW_VER_MAIN;
	UINT8 Vermedi = FW_VER_MEDI;
	UINT8 Verminor = FW_VER_MINOR;	
	sio_psprintf(str, "DC308 V:%1d.%1d.%1d", Vermain, Vermedi, Verminor);
*/	
	osdBarDraw(0,PANEL_1_OSD_H-32,PANEL_1_OSD_W,64,0x02);
	osdStrDisp(0, PANEL_1_OSD_H-32*1, UserFont16x20, 0xf2, versionID);
}

void TestFuncMenu(UINT8 index)
{
	UINT32 i;

	for(i=0;i<TESTMENUINDEX;i++)
	{
	   if(i==0)
	   {
	   	if(sPara.ui8UserPowerDownVolFlag == 0xA5 )
	   	{
	          osdStrDisp(32, 32+i*32, UserFont16x20, index==i?0xf2: 0xf0, testmenu[i]);
	   	}
	   	else
	   	{
	   	  osdStrDisp(32, 32+i*32, UserFont16x20, index==i?0xf2: 0xf0, " Test Voltage(NO) ");	
	   	}
	   	
	   }
	   else
	   {		
	     osdStrDisp(32, 32+i*32, UserFont16x20, index==i?0xf2: 0xf0, testmenu[i]);
	   }
	}	
}

void dspPowerOff(void)
{
	OpenLcdDisplay(0);
	gpioSet(GPIO_LCD_POWER,0);
	hwWait(0, 10);
	gpioConfig(GPIO_MUTE, 1);
	gpioSet(GPIO_MUTE, 1);
	gpioSet(GPIO_PWR_EN, 0);
}

UINT32 usbCustomWriteVQ_Snap(UINT16 wIndex, UINT16 wValue)
{
	snapOne();
	if (snapErrGet() == LIBDCF_MISC_ERR0003)
	{
		printf("Fanth note : disk is full\n");
		return false;
	}
	return true;
}

#define ZOOM_MODE_H_SHIFT_FLAG_1                           256     /*320*/
#define ZOOM_MODE_H_SHIFT_FLAG_2                           512     /*640*/
#define ZOOM_MODE_H_SHIFT_FLAG_3                           768     /*960*/
#define ZOOM_MODE_H_SHIFT_FLAG_4                           1024     /*1280*/
extern bool   bUSBFocusFlag;
UINT32 usbCustomWriteVQ_fPrivew(UINT16 wIndex, UINT16 wValue);
UINT32 usbCustomWriteVQ_fPrivewInit(UINT16 wIndex, UINT16 wValue)
{
	printf("On Request'0xA2' wIndex = %x, wValue = %x\n", wIndex, wValue);
/*	UINT32  regaddr[1], regdata[2];*/
	UINT32  regaddr[1], regdata[1],senHPos,senZoomPos;
	frontSenReg_t *pfrontSenReg;
	pfrontSenReg = (frontSenReg_t *)(0xb0000000); 
	/* Paul@2005/11/07-16:35 add */
	UINT32 err;
bUSBFocusFlag = true;

	#if 1
	/*dscInit(0);
	PNL_PreviewSet();
	hwFrontInit(0);
	hwFrontPreviewSet();*/
	xdcfCurRootDirSet(imageRootNameStr);
    xdcfInit(imageDirNameStr, imageFileNameStr, xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY);
    xdcfFilterSet(xDCF_FILTER_JPG);
    exifInit(exifTagInsCallback, exifTagRmvCallback);
	#endif
	/* Paul@2005/11/07-16:35 end */	
	
	if(wValue != 0)
	{	
/**		hwFrontClkSet(0, 4, 1); 

		aaaAeawbModeSet(1);**/
	/*	bFocusMode = true;
		hwFrontPreviewSet();
		hwFrontFullSet(0);*/
		regaddr[0] = 0x16; 
		regdata[0] = 0x08;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	}

	if (wIndex == 0)
	{
		wIndex = 0x00;
		wValue = 0x00;	
	}
	else
	{
		wIndex = 0x2D2;
		wValue = 0x2DA;	
	}
	usbCustomWriteVQ_fPrivew(wIndex,wValue);

	return true;

}

UINT32 usbCustomWriteVQ_fPrivew(UINT16 wIndex, UINT16 wValue)
{

	UINT32  regaddr[1], regdata[1],senHPos,senZoomPos;/**/
	frontSenReg_t *pfrontSenReg;
	pfrontSenReg = (frontSenReg_t *)(0xb0000000);  
	printf("On Request'0xA3' wIndex = %x, wValue = %x\n", wIndex, wValue);

	if((wIndex!=0)&&(wValue!=0))
	{
		if(wIndex<ZOOM_MODE_H_SHIFT_FLAG_1)
		{
			senHPos = wIndex/2;
			senZoomPos = 0;
		}
		else if((wIndex>=ZOOM_MODE_H_SHIFT_FLAG_1)&&(wIndex<=ZOOM_MODE_H_SHIFT_FLAG_2))
		{
			senHPos = (wIndex-ZOOM_MODE_H_SHIFT_FLAG_1)/2;
			senZoomPos = 1;		
		}
		else if((wIndex>ZOOM_MODE_H_SHIFT_FLAG_2)&&(wIndex<=ZOOM_MODE_H_SHIFT_FLAG_3))
		{
			senHPos = (wIndex-ZOOM_MODE_H_SHIFT_FLAG_2)/2;
			senZoomPos = 2;			
		}
		else if((wIndex>ZOOM_MODE_H_SHIFT_FLAG_3)&&(wIndex<=ZOOM_MODE_H_SHIFT_FLAG_4))
		{
			senHPos = (wIndex-ZOOM_MODE_H_SHIFT_FLAG_3)/2;
			senZoomPos = 3;			
		}
		else 
		{
			senHPos = (wIndex-ZOOM_MODE_H_SHIFT_FLAG_4)/2;
			senZoomPos = 4;		
		}/**/
		/*printf("senHPos = 0x%x senHPos--0x%x senZoomPos= 0x%x\n",senHPos,(senHPos>>8),senZoomPos);
		printf("wValue--1 =0x%x wValue=0x01=0x%x\n",(wValue>>1),(wValue&0x01));
		senHPos/=4;	 sun@1020 */
		regaddr[0] = 0x30; 
		regdata[0] = senHPos;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
		
		regaddr[0] = 0x2A; 
		regdata[0] = senHPos>>8;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 

		regaddr[0] = 0x39; 
		regdata[0] = senZoomPos;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 

		regaddr[0] = 0x4a; 
		regdata[0] = wValue>>1;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 

		regaddr[0] = 0x4b; 
		regdata[0] = wValue&0x01;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
	/*	regaddr[0] = 0x19; 
		regdata[0] = wIndex;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);  Intergration row numbers */
	}
	else 
	{
		regaddr[0] = 0x30; 
		regdata[0] = 1;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
		
		regaddr[0] = 0x2A; 
		regdata[0] = 0;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 

		regaddr[0] = 0x39; 
		regdata[0] = 0;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 

		regaddr[0] = 0x4a; 
		regdata[0] = 1;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 

		regaddr[0] = 0x4b; 
		regdata[0] = 0;	
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
	}

	return true;

}

UINT32 usbCustomWriteVQ_SetExposure(UINT16 wIndex, UINT16 wValue)
{
	UINT32  regaddr[1], regdata[2];
	
	if(wIndex != 0)
	{
		if(wValue == 0)
		{
			aaaAeawbModeSet(1);
			regaddr[0] = 0x04; 
			regdata[0] = 0x00;	
			hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
			
			regaddr[0] = 0x10; 
			regdata[0] = 0x1e;	
			hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
		}
		else
		{
			regaddr[0] = 0x04; 
			regdata[0] = wValue&0x07;	
			hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
			
			regaddr[0] = 0x10; 
			regdata[0] = wValue>>3;	
			hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
		}
	}
	else
	{
		aaaAeawbModeSet(0);/*'10' disable AE enable AWB*/
			regaddr[0] = 0x04; 
			regdata[0] = 0x00;	
			hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
			
			regaddr[0] = 0x10; 
			regdata[0] = 0x1e;	
			hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
	}
	return true;
}

UINT32 usbCustomWriteVQ_EraseAllPhoto(UINT16 wIndex, UINT16 wValue)
{
		UINT32 err;
		xdcfCurRootDirSet(imageRootNameStr);
		err = xdcfInit(imageDirNameStr, imageFileNameStr, xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY);
		err = xdcfFilterSet(xDCF_FILTER_JPG);
		xdcfAllFilesErase();
	return true;
}

UINT32 usbCustomWriteVQ_PowerOff(UINT16 wIndex, UINT16 wValue)
{
	dspPowerOff();
	return true;
}


/*
	when initial the pc cam mode, please register the usbRegCustomerVendorCommand function at the same time,
		
*/
void usbRegCustomerVendorCommand(void)
{
	usbFuncReg_UsbWriteVendorReq(0xA1, usbCustomWriteVQ_Snap);
	usbFuncReg_UsbWriteVendorReq(0xA2, usbCustomWriteVQ_fPrivewInit);
	usbFuncReg_UsbWriteVendorReq(0xA3, usbCustomWriteVQ_fPrivew);
	usbFuncReg_UsbWriteVendorReq(0xA4, usbCustomWriteVQ_SetExposure);
	usbFuncReg_UsbWriteVendorReq(0xA5, usbCustomWriteVQ_EraseAllPhoto);
	usbFuncReg_UsbWriteVendorReq(0xA9, usbCustomWriteVQ_PowerOff);
}

UINT8 badPixSeek(void)
{
#if 0
	UINT32  countW, countH;

	/*	sun@1030 am 11:27 	when the search the bad pixel, the sensor power must be on.*/
	gpioConfig(GPIO_SENSOR_POWER, 1);
	gpioSet(GPIO_SENSOR_POWER, 1);
	gpioConfig(GPIO_SEN_STANDBY, 1);
	gpioSet(GPIO_SEN_STANDBY, 0);
	/*hwFrontInit(1);*/
    	osTaskResume(osTaskFindByName("HISTOGRAM"));
	osTaskResume(osTaskFindByName("AAA"));
	hwCdspInit(CAM_PREVIEW);
	hwDispPvEn(1);
	hwCammodeSet(CAM_PREVIEW);
	PNL_PreviewSet();
	/*hwFrontInit(2);
	hwDispPipEn(0);
	zoomFactSet(zoomFactGet());
	hwPVZoom(pvZoomFactor);
	aaaEvCmpIdxSet(pvEvCmpIdx);
	hwFrontMaxMonitorSizeGet(&countW, &countH);
#else*/
	aaaInit();					
	uiKeyMsgQ = osEventFindByName("UI_KEY");
	isMenuOn = 0;
	pvZoomFactor = 100;	

	/* Init Sensor */
	WRITE8(0xb0000007,0x7f);		/* ?? */
	gpioSet(GPIO_SENSOR_POWER, 1);
	gpioSet(GPIO_SEN_STANDBY, 0);
	hwDispPvEn(0);					/* Paul@2005/10/11 */
	hwDispOsdEn(0);
	hwWait(0, 8);					
	hwTvSensorModeSel(0);	
	hwDispPvEn(0);
	hwDispOsdEn(0);					
	osTaskResume(osTaskFindByName("HISTOGRAM"));
	osTaskResume(osTaskFindByName("AAA"));						
	osdClearScreen(0x00);												
	
	PNL_PreviewSet();	

	if(sPara.EffectFlag == SephiaEffect)
	{
		hwCdspSatHueSet(0, 0x20, 0x00, 0x40);
		hwCdspColorMSet(0x38, 0x38, 0x38, 0x28, 0x28, 0x28, 0x20, 0x20, 0x20);
	}
	else if(sPara.EffectFlag == NegativeEffect)
	{
		hwCdspSatHueSet(1, 0x00, 0x00, 0x40);
	}
	else if(sPara.EffectFlag == NormalEffect)
	{
		hwCdspSatHueSet(0, 0x20, 0x00, 0x40);
		hwCdspColorMSet(0x86, 0x1AC, 0x0E, 0x1F2, 0x33, 0x1C, 0x1F3, 0x1B0, 0x9D);
	}	
							
	zoomFactSet(zoomFactGet());
	hwPVZoom(pvZoomFactor);
	sysgSnapScaleFactorSet(pvZoomFactor);
	aaaEvCmpIdxSet(pvEvCmpIdx);
	SnapImageSizeSet();
						
	#if 1
	xdcfCurRootDirSet(imageRootNameStr);
	xdcfInit(imageDirNameStr, imageFileNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
	xdcfFilterSet(xDCF_FILTER_JPG | xDCF_FILTER_ASF | xDCF_FILTER_WAV | xDCF_FILTER_OURTYPE);
	exifInit(exifTagInsCallback, exifTagRmvCallback);
	#endif

	osdIconDisp(1);	
	
	hwDispOsdEn(1);
	hwDispPvEn(1);	
#endif
	
	osdClearScreen(0);

	
/*	G_bBadPixSetShutter = true;*/

	badpix_Search();
/*	sun@1030 am 11:30 when finished the bad pixel seek funciton, it must restart and represh the bad pixel buf to enable
	the bad pixel function the capture mode
	G_bBadPixSetShutter = false;*/
	dspPowerOff();
	
	return 0;
}

extern UINT32 Min_Tidx;
extern UINT8 flashmode;
extern UINT32 Oncharge;

UINT8  ui8ExitOBFlag = 1;
/*	sun@1030 
	when run the burn in function, please disable the auto power save.
*/

extern UINT32 vdIrqRefillCnt;		
void BurnPreviewStill(
	void
)
{
   /*	UINT32 startTime,endTime;*/
    	UINT32 timecnt=0;
	UINT32 err;
	UINT32	osdW, osdH;
	UINT8  flashChargeDelay;
	UINT32 key;
	UINT32 snapPhotoNum=0;
	UINT8 str[8];
	UINT32* msg;
	
	semApp = osEventFindByName("APP_SEM");
	uiKeyMsgQ = osEventFindByName("UI_KEY");
	
	printf("burn in \n");
	mp3Vol = 9;
	osTaskSuspend(osTaskFindByName("HISTOGRAM"));	
	
	while (ui8ExitOBFlag)	
	{	
		uiState = UI_MODE_PREVIEW;			
		msg = osQuePend(uiKeyMsgQ, 1, &err);
		if (err == OS_NO_ERR)
		{
			if (*msg == UI_KEY_ACCIDENT)
			{
				ui8ExitOBFlag = 0;
				return;
			}
		}

		zoomFactSet(zoomFactGet());
		hwPVZoom(pvZoomFactor);
		sysgSnapScaleFactorSet(pvZoomFactor);
		aaaEvCmpIdxSet(pvEvCmpIdx);
		SnapImageSizeSet();
		
		osdClearScreen(0x00);
		
		osTimeDly(10);
		xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
		xdcfCurRootDirSet(imageRootNameStr);		
		xdcfFilterSet(xDCF_FILTER_JPG | xDCF_FILTER_ASF);
		exifInit(exifTagInsCallback, exifTagRmvCallback);
		
		sPara.ImageQuality = QualityNormal;
		QTidx = 3;
		
		DisplayPVIcon(1);		
		ChangeQtable();		
		sysgSnapSizeSet(640, 480);	
		xdcfFileCountGet(&fileCnt);
		if(fileCnt > 0)
		{
			xdcfAllFilesErase();
		}
		xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
		xdcfCurRootDirSet(imageRootNameStr);		
		xdcfFilterSet(xDCF_FILTER_JPG | xDCF_FILTER_ASF);		
		xdcfFileCountGet(&fileCnt);
		printf("rtc_out=%d\n");
		snapPhotoNum = 0;			
		while(snapPhotoNum < 10)		
		{	
//			if( snapPhotoNum%2 == 0)
			{
				flashmode = FlashForceOff;	
				snapPhotoNum++;
				osdClearScreen(0x00);						
				sio_psprintf(str, "PHOTO:%4d", snapPhotoNum);
				osdStrDisp(0, 32, UserFont16x20, 0xd0, str);
				osTimeDly(10);
				
				snapOne();
				osTimeDly(1000);
				osTimeDly(1000);
				osTimeDly(1000);
				xdcfAllFilesErase();
				msg = osQuePend(uiKeyMsgQ, 1, &err);
				if (err == OS_NO_ERR)
				{
					if (*msg == UI_KEY_ACCIDENT)
					{
						ui8ExitOBFlag = 0;
						return;
					}
				}	
				
			}
			#if 0
			else 
			{
		//	}
				osTimeDly(10);		
				flashmode = FlashForceOn;
		//	while (snapPhotoNum < 10)
		//	{
				Oncharge = 1;			
				hwFmGpioCfgSet(0,0,0,0x100);	/* Set GPIO_CHG_READY Input */
				flashChargeDelay = 0;
				gpioConfig(GPIO_CHG_PWM,1);/*strobe charge output enable*/
				gpioSet(GPIO_CHG_PWM,1);/*strobe charge*/	  
				OpenLcdDisplay(0);
				while(gpioGet(GPIO_CHG_READY))
				{	
					gpioSet(GPIO_BUSY_LED,1); 
					osTimeDly(50);
					gpioSet(GPIO_BUSY_LED,0);
					msg = osQuePend(uiKeyMsgQ, 50, &err);
					if (err == OS_NO_ERR)
					{
						if (*msg == UI_KEY_ACCIDENT)
						{
							ui8ExitOBFlag = 0;
							return;
						}
					}							
					if(flashChargeDelay > 14)
					{
						break;
					}
					flashChargeDelay++;		
				}	
				gpioConfig(GPIO_CHG_PWM,1);
				gpioSet(GPIO_CHG_PWM,0);/*strobe charge*/
				OpenLcdDisplay(1);						
				Oncharge = 0;
				snapPhotoNum++;
				osdClearScreen(0x00);						
				sio_psprintf(str, "PHOTO:%4d", snapPhotoNum);
				osdStrDisp(0, 32, UserFont16x20, 0xd0, str);						
				osTimeDly(500);			
				snapOne();		
				osTimeDly(500);	
				osTimeDly(500);	
				osTimeDly(300);	
				xdcfAllFilesErase();
				
				msg = osQuePend(uiKeyMsgQ, 1, &err);
				if (err == OS_NO_ERR)
				{
					if (*msg == UI_KEY_ACCIDENT)
					{
						ui8ExitOBFlag = 0;
						return;
					}
				}										
				/*startTime = endTime = tmrMsTimeGet();*/
			}
			#endif
		}			
		ManualWbFlag = WBAuto;
		aaaAeawbModeSet(3);	
		
		PNL_PlaybackSet();		
		gpioConfig(GPIO_SENSOR_POWER, 1);
		gpioSet(GPIO_SENSOR_POWER, 0);	
		hwFrontInterruptCfg(0, 0, 0);				
		hwWait(0, 10);
		osTaskSuspend(osTaskFindByName("AAA"));	
		osTimeDly(10);
		osdClearScreen(0);				
		if (!BurnPreviewMovie())
		{
			return;
		}
		osTimeDly(10);
		if (!BurnMP4Play())
		{
			return;
		}
		osTimeDly(10);
		if (!BurnMP3Play())
		{
			return;
		}		
		
		#if 0		
		ManualWbFlag = AutoWb;
		if(!BurnGamePlay())
		{
			Rsv_FlushGame();
			return;
		}
		#endif
		osTimeDly(10);
		
		aaaInit();	
		captureQTidxInit();	
		WRITE8(0xb0000007,0x7f);		/* ?? */
		gpioSet(GPIO_SENSOR_POWER, 1);  
		uiDispBuffFill(0x000000);	 // for bug 85  tzw add 
		hwWait(0, 8);			//for bug 100003 machiner 2006-02-17				
		hwTvSensorModeSel(0);			
		osTimeDly(10);		
		PNL_PreviewSet();	
		
		hwFrontInterruptCfg(2, 0, vdIrqRefillCnt);				
	}
	printf("while end\n");
}

/**
**/
extern UINT32 asfTime;

UINT32 BurnPreviewMovie(void)
{	
	UINT32 ui32TimeCnt = 0;
	UINT32 err;
	UINT8 asfFilename[13];
	UINT32 *msg;

	uiState = UI_MODE_MOVIE;
	
	aaaInit();
	
	videoQTidxInit();	/* Paul @03/13/2006 */
	WRITE8(0xb0000007,0x7f);		/* ?? */
	gpioSet(GPIO_SENSOR_POWER, 1);  
	uiDispBuffFill(0x000000);	 // for bug 85  tzw add 
	hwWait(0, 8);			//for bug 100003 machiner 2006-02-17					
	hwTvSensorModeSel(0);			
	osTimeDly(10);
	PNL_PreviewSet();

	hwFrontInterruptCfg(2, 0, vdIrqRefillCnt);
	
	xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
	xdcfCurRootDirSet(imageRootNameStr);		
	xdcfFilterSet(xDCF_FILTER_ASF);
	exifInit(exifTagInsCallback, exifTagRmvCallback);

	sPara.VideoClipSize = VideoSize320;
	VideoClipPlaySet();
	hwDispPvSetRead(imgBuf);
	ChgPvSetting = 1;
	xdcfCurRootDirSet(DVDCRootNameStr);
	xdcfNextFileNameGet(xDCF_FILETYPE_ASF, asfFilename);

#if (CurrentIC8751==0)		
		hwAudAC97RegWrite(0x2c, 8000);		/* 8K Hz*/
		hwAudAC97RegWrite(0x2e, 8000);		/* 8K Hz*/
		hwAudAC97RegWrite(0x32, 8000);		/* 8K Hz*/
#else 
		wm8751SampleSet(0);
#endif

	snapAV	= OBJ_ASF;
	asfTime = 0;
	asfAppClipNameSet(asfFilename);
	asfAppClip(); 
	osTimeDly(20);
	/*
		Wait 1 Min for DV
	*/
	while (ui32TimeCnt < 180)
	{
		DisplayMVIcon();
		msg = osQuePend(uiKeyMsgQ, 100, &err);
		if (err == OS_NO_ERR)
		{
			if (*msg == UI_KEY_ACCIDENT)
			{
				ui8ExitOBFlag = 0;
				break;
			}
		}							
		asfTime += 1000;
		ui32TimeCnt++;
	}	
	if (snapAV == OBJ_ASF)
	{
		asfAppClipAbort(); /* stop clipping	*/
	}
	if (ChgPvSetting == 1)
	{
		hwDispPvSetWrite(imgBuf);/**/
		ChgPvSetting = 0;
	}
	
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

	gpioSet(GPIO_BUSY_LED, 0);	
	uiOnBusy = 0;				

	xdcfAllFilesErase();	

	ManualWbFlag = WBAuto;
	aaaAeawbModeSet(3);	
	
	PNL_PlaybackSet();
	
	gpioConfig(GPIO_SENSOR_POWER, 1);
	gpioSet(GPIO_SENSOR_POWER, 0);
	
	hwFrontInterruptCfg(0, 0, 0);	
	hwWait(0, 10);	
	
	osTaskSuspend(osTaskFindByName("AAA"));			

	return ui8ExitOBFlag;
}

/**
**/
extern xdcfAttrElm_t fileattr;
extern xdcfAttrElm_t dirattr;
extern UINT32 gPB;
/**
**/
UINT32 BurnMP4Play(void)
{
	UINT32 err;
	UINT32	dispW, dispH;
	asfFileInfo_t  asfFileInfo;
	UINT32 samplerate = 0;	
	UINT32 ui32TimeCnt = 0;

	uiState = UI_MODE_PLAYBACK;
	
	#if (CurrentIC8751==0)
		ws9712LineOut(SpeakFlag);
	#else
		wm8751LineOut(SpeakFlag);
	#endif
	volSet();	

	PNL_PlaybackSet();
	sysgMemDispAddrGet(&gPB);
	hwDispPvEn(1);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);
	
	semApp = osEventFindByName("APP_SEM");
	uiKeyMsgQ = osEventFindByName("UI_KEY");
	
	osTaskSuspend(osTaskFindByName("AAA"));
	
	pbDisp =  PB_DISP_FOUR;  /*PB_DISP_ONE;*/
	g_playrecStatus = 0;
	ChgPvSetting = 0;
				
	xdcfInit(mediaRootNameStr, mediaRootNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);						
	xdcfCurRootDirSet(mediaRootNameStr);
	xdcfFilterSet(FILTER_VIDEO);
	err	= xdcfFileCountGet(&fileCnt);
	fileIdx = 1;

	pbDisp = PB_DISP_ONE;		
	do	
	{	
		osdClearScreen(0x00);
	
		if ((err = xdcfCurFileByPosSet(fileIdx)) != SUCCESS)
		{
			return;
		}
		err = xdcfCurFileAttrGet(&fileattr);
					
		UINT32 startTime=0, endTime=0;				
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
		
		asfAppFileInfoGet(fileattr.name, &asfFileInfo);
		samplerate = asfFileInfo.audioSampleFreq;
	
		/*andrew_gu@2005/May/10, 19:14 (*/
		UINT8 samplerateIdx ;
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
	
		avPlayNow = OBJ_ASF;
		asfAppPlay(fileattr.name);
	
		g_playrecStatus = STATUS_PLAYING;	
		osTimeDly(20);
		while (1)
		{
			UINT32* msg;
			msg = osQuePend(uiKeyMsgQ, 1, &err);
			if (err == OS_NO_ERR)
			{
				if (*msg == UI_KEY_ACCIDENT)
				{
					ui8ExitOBFlag = 0;
					asfAppPlayAbort();
					avPlayNow = OBJ_NONE;	
					g_playrecStatus = STATUS_STOP;
					pressVolume = 0;
					pbDisp = PB_DISP_FOUR;		
					F_Mp3SetVolum(0);									
					break;
				}
			}										
			osSemPend(semApp, -1, &err);
			if (err == OS_NO_ERR)
			{
				osSemPost(semApp);
				break;
			}
			else
			{
			  	 osTimeDly(20);
			} 
			F_Mp4IconRefresh(MP4_PLAY_STATUS);									
			F_Mp4IconRefresh(MP4_PROGRESS);
			ui32TimeCnt++;
			if (ui32TimeCnt == 300)
			{
				asfAppPlayAbort();
				avPlayNow = OBJ_NONE;	
				g_playrecStatus = STATUS_STOP;
				pressVolume = 0;
				pbDisp = PB_DISP_FOUR;		
				F_Mp3SetVolum(0);									
			}
		}
	
		osSemPend(semApp, 0, &err);
		osSemPost(semApp);		
	
		osTimeDly(20);
		#if (CurrentIC8751==0)	
			gpioConfig(GPIO_MUTE, 1);
			gpioSet(GPIO_MUTE, 1);
		#else		
			wm8751SoundEnable(0,0);
		#endif							
	
		if (ChgPvSetting == 1)
		{
			hwDispPvSetWrite(imgBuf);				
			ChgPvSetting = 0;
		}
		/* Paul@2006/04/02-10:30 add */
		hwDispPreviewSizeGet(&dispW, &dispH);
		hwDispImgSizeSet(dispW, dispH);    /*for zoomFactSet() use*/
	
		uiDispBuffFill(0x00);
		
	#if (CurrentIC8751==0)		
		ws9712LineOut(SpeakFlag);//wendy for speak
	#else
		wm8751LineOut(SpeakFlag);
	#endif		
    }while (avPlayNow != OBJ_NONE);

	return ui8ExitOBFlag;
}

/**
**/
UINT32 BurnMP3Play(void)
{
	UINT32 err;
	UINT32 ui32TimeCnt = 0;
	UINT32 ui32TimeElapsed;
	UINT8 str[20];

	uiState = UI_MODE_MP3;
	
	osdClearScreen(0);
	xdcfInit(mp3SubDirMP3Str, mp3SubDirMP3Str, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
	xdcfCurRootDirSet(mp3SubDirMP3Str);

	printf("Cur dir =%s\n", vfsGetCurrDirName());
	xdcfFilterSet(xDCF_FILTER_MP3);
	xdcfFileCountGet(&fileCnt);
	printf("Cur dir filecnt =%d\n", fileCnt);
						
	fileIdx = 1;
	
#if (CurrentIC8751==0)		
		ws9712LineOut(SpeakFlag);//wendy for speak
#else
		wm8751LineOut(SpeakFlag);
#endif
	volSet();

#if (CurrentIC8751==0)	
        gpioConfig(GPIO_MUTE, 1);
        gpioSet(GPIO_MUTE, 0);
#else		
		if(SpeakFlag)
			wm8751SoundEnable(2,1);
		else
			wm8751SoundEnable(1,1);
#endif		
	if ((err = xdcfCurFileByPosSet(fileIdx)) != SUCCESS)
	{
		return;
	}
	err	= xdcfCurFileAttrGet(&fileattr);

#if (CurrentIC8751==0)		
	hwAudAC97RegWrite(0x2c, 48000);		/* 48K Hz*/
	hwAudAC97RegWrite(0x2e, 48000);		/* 48K Hz*/
	hwAudAC97RegWrite(0x32, 48000);		/* 48K Hz*/
#else 
	wm8751SampleSet(0);
#endif	

	mp3AppPlay(fileattr.name);
	avPlayNow =	OBJ_MP3;
	g_playrecStatus = STATUS_PLAYING;
	
	osTimeDly(30); // John @03/18/2006			
	F_Mp3IconRefresh(MP3_PLAY_STATUS);			
	while (1)
	{
		UINT32* msg;
		msg = osQuePend(uiKeyMsgQ, 1, &err);
		if (err == OS_NO_ERR)
		{
			if (*msg == UI_KEY_ACCIDENT)
			{
				ui8ExitOBFlag = 0;
				mp3AppAbort();
				avPlayNow = OBJ_NONE ;		
				g_playrecStatus = STATUS_STOP;
				pressVolume = 0;
				break;
			}
		}												
		osTimeDly(30);
		ui32TimeElapsed = mp3GetElapsedTime();		
		sio_psprintf(str, "[%2d:%02d:%02d]", ui32TimeElapsed/1000/60/60, (ui32TimeElapsed/1000/60)%60, ui32TimeElapsed/1000%60);		
		osdStrDisp(PANEL_1_OSD_W-11*10, PANEL_1_OSD_H-20, UserFont10x20, 0x10, str);		
		osdStrDisp(PANEL_1_OSD_W-22, PANEL_1_OSD_H-64, UserFont10x20, 0x00, " ");		
		F_Mp3IconRefresh(MP3_PLAY_STATUS);
		ui32TimeCnt++;
		if (ui32TimeCnt == 200)
		{
			mp3AppAbort();
			avPlayNow = OBJ_NONE ;		
			g_playrecStatus = STATUS_STOP;
			pressVolume = 0;
			break;
		}
	}
	osdClearScreen(0x00);	
	g_playrecStatus = STATUS_STOP;
	
	osSemPend(semApp, 0, &err);
	osSemPost(semApp);	
	osdStrDisp(PANEL_1_OSD_W-11*10, PANEL_1_OSD_H-20, UserFont10x20, 0x10, "[00:00:00]");		

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

	#if (CurrentIC8751==0)	
		gpioConfig(GPIO_MUTE, 1);
		gpioSet(GPIO_MUTE, 1);
	#else		
		wm8751SoundEnable(0,0);
	#endif								

	return ui8ExitOBFlag;
}

/****************************************************
* FunctionName: uiDefaultSetup													*
*													*
* Discription: Default Mode include 8 kinds			*
*													*
* Purpose: setup for release						*	
*													*
* retrun: None										*
*													*
* Argument:													*
*													*
* Date:		01/05/2005								*
****************************************************/

UINT8 uiDefaultSetup(UINT8 mode )
{
	UINT8 CMODE = mode + 1;

		switch(CMODE)
		{
         	
			case 1:
				sPara.snapFlag = SnapSingle;
				sPara.language = LanguageEnglish;
				sPara.uiPatternFlag = PatternDefault;
			
				sPara.uiBeepFlag = BeepOn;
				sPara.stampFlag = StampDayTime;
				sPara.USBModeFlag = USBMSDC;
				sPara.SleepTimeFlag = SleepTime3m;
				sPara.VideoModeFlag = TVPAL;
				sPara.VideoClipSize = VideoSize640;
			
				sPara.ImageQuality = QualitySuper;
				sPara.ui8ImageSize = ImageSize1280;
				sPara.ISOFlag = ISOAUTO;
				sPara.EffectFlag = NormalEffect;
				sPara.ASFMICFlag = MICON;
//			    sPara.SelfFlag = SELFON;   /*asaki@0210 for selftimer*/
				sPara.Interval = SlideInterval3sec;
				sPara.Repeat = SlideRepeatOn;
				sPara.FolderAll = SlideImageFolder;
				sPara.frequenceFlag = frequence60Hz;
				sPara.WBFlag = WBAuto;
				if(	uiTVStatus == 1)
	            		panelSelect(sPara.VideoModeFlag);
				break;
			case 2:
				sPara.snapFlag = SnapSingle;
				sPara.language = LanguageChinese_S;
				sPara.uiPatternFlag = PatternDefault;
			
				sPara.uiBeepFlag = BeepOn;
				sPara.stampFlag = StampDayTime;
				sPara.USBModeFlag = USBMSDC;
				sPara.SleepTimeFlag = SleepTime3m;
				sPara.VideoModeFlag = TVPAL;
				sPara.VideoClipSize = VideoSize640;
			
				sPara.ImageQuality = QualitySuper;
				sPara.ui8ImageSize = ImageSize1280;
				sPara.ISOFlag = ISOAUTO;
				sPara.EffectFlag = NormalEffect;
				sPara.ASFMICFlag = MICON;
//			    sPara.SelfFlag = SELFON;   /*asaki@0210 for selftimer*/
				sPara.Interval = SlideInterval3sec;
				sPara.Repeat = SlideRepeatOn;
				sPara.FolderAll = SlideImageFolder;
				sPara.frequenceFlag = frequence50Hz;
				sPara.WBFlag = WBAuto;
				break;	
			case 3:
				sPara.snapFlag = SnapSingle;
				sPara.language = LanguageDeutsch;
				sPara.uiPatternFlag = PatternDefault;
			
				sPara.uiBeepFlag = BeepOn;
				sPara.stampFlag = StampDayTime;
				sPara.USBModeFlag = USBMSDC;
				sPara.SleepTimeFlag = SleepTime3m;
				sPara.VideoModeFlag = TVPAL;
				sPara.VideoClipSize = VideoSize640;
			
				sPara.ImageQuality = QualitySuper;
				sPara.ui8ImageSize = ImageSize1280;
				sPara.ISOFlag = ISOAUTO;
				sPara.EffectFlag = NormalEffect;
				sPara.ASFMICFlag = MICON;
//			    sPara.SelfFlag = SELFON;   /*asaki@0210 for selftimer*/
				sPara.Interval = SlideInterval3sec;
				sPara.Repeat = SlideRepeatOn;
				sPara.FolderAll = SlideImageFolder;
				sPara.frequenceFlag = frequence60Hz;
				sPara.WBFlag = WBAuto;
				break;	
			case 4:
				sPara.snapFlag = SnapSingle;
				sPara.language = LanguageSpanish;
				sPara.uiPatternFlag = PatternDefault;
			
				sPara.uiBeepFlag = BeepOn;
				sPara.stampFlag = StampDayTime;
				sPara.USBModeFlag = USBMSDC;
				sPara.SleepTimeFlag = SleepTime3m;
				sPara.VideoModeFlag = TVPAL;
				sPara.VideoClipSize = VideoSize640;
			
				sPara.ImageQuality = QualitySuper;
				sPara.ui8ImageSize = ImageSize1280;
				sPara.ISOFlag = ISOAUTO;
				sPara.EffectFlag = NormalEffect;
				sPara.ASFMICFlag = MICON;
//			    sPara.SelfFlag = SELFON;   /*asaki@0210 for selftimer*/
				sPara.Interval = SlideInterval3sec;
				sPara.Repeat = SlideRepeatOn;
				sPara.FolderAll = SlideImageFolder;
				sPara.frequenceFlag = frequence60Hz;
				sPara.WBFlag = WBAuto;
				break;	
			case 5:
				sPara.snapFlag = SnapSingle;
				sPara.language = LanguageJapanese;
				sPara.uiPatternFlag = PatternDefault;
			
				sPara.uiBeepFlag = BeepOn;
				sPara.stampFlag = StampDayTime;
				sPara.USBModeFlag = USBMSDC;
				sPara.SleepTimeFlag = SleepTime3m;
				sPara.VideoModeFlag = TVPAL;
				sPara.VideoClipSize = VideoSize640;
			
				sPara.ImageQuality = QualitySuper;
				sPara.ui8ImageSize = ImageSize1280;
				sPara.ISOFlag = ISOAUTO;
				sPara.EffectFlag = NormalEffect;
				sPara.ASFMICFlag = MICON;
//			    sPara.SelfFlag = SELFON;   /*asaki@0210 for selftimer*/
				sPara.Interval = SlideInterval3sec;
				sPara.Repeat = SlideRepeatOn;
				sPara.FolderAll = SlideImageFolder;
				sPara.frequenceFlag = frequence60Hz;
				sPara.WBFlag = WBAuto;
				break;	
			case 6:
				sPara.snapFlag = SnapSingle;
				sPara.language = LanguageFrench;
				sPara.uiPatternFlag = PatternDefault;
			
				sPara.uiBeepFlag = BeepOn;
				sPara.stampFlag = StampDayTime;
				sPara.USBModeFlag = USBMSDC;
				sPara.SleepTimeFlag = SleepTime3m;
				sPara.VideoModeFlag = TVNTSC;
				sPara.VideoClipSize = VideoSize640;
			
				sPara.ImageQuality = QualitySuper;
				sPara.ui8ImageSize = ImageSize1280;
				sPara.ISOFlag = ISOAUTO;
				sPara.EffectFlag = NormalEffect;
				sPara.ASFMICFlag = MICON;
//			    sPara.SelfFlag = SELFON;   /*asaki@0210 for selftimer*/
				sPara.Interval = SlideInterval3sec;
				sPara.Repeat = SlideRepeatOn;
				sPara.FolderAll = SlideImageFolder;
				sPara.frequenceFlag = frequence60Hz;
				sPara.WBFlag = WBAuto;
				break;	
			case 7:
				sPara.snapFlag = SnapSingle;
				sPara.language = LanguageChinese_T;
				sPara.uiPatternFlag = PatternDefault;
			
				sPara.uiBeepFlag = BeepOn;
				sPara.stampFlag = StampDayTime;
				sPara.USBModeFlag = USBMSDC;
				sPara.SleepTimeFlag = SleepTime3m;
				sPara.VideoModeFlag = TVPAL;
				sPara.VideoClipSize = VideoSize640;
			
				sPara.ImageQuality = QualitySuper;
				sPara.ui8ImageSize = ImageSize1280;
				sPara.ISOFlag = ISOAUTO;
				sPara.EffectFlag = NormalEffect;
				sPara.ASFMICFlag = MICON;
//			    sPara.SelfFlag = SELFON;   /*asaki@0210 for selftimer*/
				sPara.Interval = SlideInterval3sec;
				sPara.Repeat = SlideRepeatOn;
				sPara.FolderAll = SlideImageFolder;
				sPara.frequenceFlag = frequence60Hz;
				sPara.WBFlag = WBAuto;
				break;
			case 8:
				sPara.snapFlag = SnapSingle;
				sPara.language = LanguageItalian;
				sPara.uiPatternFlag = PatternDefault;
			
				sPara.uiBeepFlag = BeepOn;
				sPara.stampFlag = StampDayTime;
				sPara.USBModeFlag = USBMSDC;
				sPara.SleepTimeFlag = SleepTime3m;
				sPara.VideoModeFlag = TVPAL;
				sPara.VideoClipSize = VideoSize640;
			
				sPara.ImageQuality = QualitySuper;
				sPara.ui8ImageSize = ImageSize1280;
				sPara.ISOFlag = ISOAUTO;
				sPara.EffectFlag = NormalEffect;
				sPara.ASFMICFlag = MICON;
//			    sPara.SelfFlag = SELFON;   /*asaki@0210 for selftimer*/
				sPara.Interval = SlideInterval3sec;
				sPara.Repeat = SlideRepeatOn;
				sPara.FolderAll = SlideImageFolder;
				sPara.frequenceFlag = frequence60Hz;
				sPara.WBFlag = WBAuto;
				sPara.defaultMode = 8;
				break;						
		}
		extern UINT32 pvEvCmpIdx;
		extern UINT32 vcEvCmpIdx;
		pvEvCmpIdx = 6;
		vcEvCmpIdx = 6;
		struct tm time;
		time.tm_sec  = 0;
		time.tm_min  = 0;
		time.tm_hour = 0;
		time.tm_mday = 1;   /* Day: 1 */
		time.tm_mon  = 1;  /*  Month: 1 */
		time.tm_year = 106; /* Year: since 1900 */
		timeSet(&time);
//		sPara.defaultMode = mode;
		nandWriteParameters();

	return CALL_DEFAULT;	
}


/**
**/
UINT8 TestFunc(void)
{
   	UINT32 startTime,endTime;
	printf("test func\n");
	printf("G_TestFuncFlag=%d G_TestFuncIndex=%d\n",G_TestFuncFlag,G_TestFuncIndex);
	if(G_TestFuncFlag)
	{
		switch(G_TestFuncIndex)
		{
			case OB_CALIBRATION:
				menuOV3620_OBCalibration();
				break;
			case BAD_PIXEL_SEARCH:
				badpix_Search();
				break;				
			case BURN_IN_TEST:
				/*startTime = endTime = tmrMsTimeGet();
				while(1)
				{
					endTime = tmrMsTimeGet();
					if ((endTime-startTime)>29000)
					break;
					osTimeDly(100);
				}
				return 3;*/
				BurnPreviewStill();
				break;
//			case DEFAULT_MODE: 
//				defaultMode();
//				break;
		}
	}
	return SUCCESS;
}

/****************************************************
* FunctionName: defaultMode							*
*													*
* Discription: display 8 kinds Mode for choose		*
*													*
* Purpose: ui of choose mode 						*	
*													*
* retrun: None										*
*													*
* Argument:											*
*													*
* Date:		01/05/2005								*
****************************************************/
UINT8 *languagemode[8]=
{
	"[MODE 1: English  ]",
	"[MODE 2: Chinese_S]",
	"[MODE 3: German   ]",
	"[MODE 4: Spinash  ]",
	"[MODE 5: Japanese ]",
	"[MODE 6: French   ]",
	"[MODE 7: Chinese_T]",
	"[MODE 8: Italian  ]"
};
extern UINT32 gPB;
void defaultMode()
{
	UINT32	key;
	UINT32	index = 0;	
	UINT32	dispW,dispH;

	sysgMemDispAddrGet(&gPB);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);
	pubFill((UINT8 *)(0xAC000000+gPB*2), dispW*2, dispH,
					0, 0, dispW*2, dispH, 0x000000);
	osdClearScreen(0x0f);	
	ModeItem (index);
	
	while(1)
	{
		switch(key)
		{
			case UI_KEY_DIR_UP:
				if (index==0) index= 7;
				else index--;
				ModeItem (index);
				break;
			case UI_KEY_DIR_DOWN:
				if	(index == 7) index = 0;
				else index++;
				ModeItem (index);
				break;
			case UI_KEY_FUNC_OK:
				uiDefaultSetup (index);
				break;
			case UI_KEY_FUNC_MENU:
			case UI_KEY_ACCIDENT:				
					return;
				break;
		}

		keyGet(&key);
	}
}
/*************************************************
* Description: display items of mode			 *										 *	
*												 *	
* return: none												 *	
*												 *	
*************************************************/
void ModeItem(UINT8 index)
{
	UINT32 i;
	osdFWBuffChange();
	osdClearScreen(0x0f);
	for(i=0;i<8;i++)
	osdStrDisp(10, 38+i*24, UserFont16x20, index==i?0xd2: 0xdf, languagemode[i]);
	osdHWBuffChange();
}

//xqq add for test voltage   
void	F_TestBatteryVoltageValue()
{

	UINT8 *pui8LableText[]=
	{
		"3.40v",		/* Paul@2006/05/30 modify */
		"3.55v",
		"3.75v",
		"3.95v",
	};

	UINT16	ui16Cur_Bat_AD_value;
	
	UINT8	TestADValue[4] = {0x00,0x00,0x00,0x00};
	UINT32 	ui32keyold = 0x00,ui32keynew = 0x00;	
	UINT32	ui32i	;
	UINT32	key = UI_KEY_MODE_TEST;
	UINT16	i,j;
	UINT32 	err = SUCCESS;
	UINT32 	* msg		;
	UINT8	ui8ProFlag = 0x00	;
	UINT8	ui8Yes = 0xff	;
	UINT8	str[100] ;

	#define	C_Plus_Value 	7
	enum
	{
		C_Lable_X 	= 100,
		C_Lable_Y 	= 90,
		C_Distance 	= 24,
		C_Test_X 	= C_Lable_X+60,
		C_Test_Y 	= C_Lable_Y,
		C_CurAD_X   = 100,
		C_Yes_X = 20,
		C_Yes_Y = 200,
		C_Yes_Color = 0xd1,
		C_No_Color = 0xf7,

		C_Back_Color 		= 0x07,
		C_SelectBar_Color 	= 0x00,
		C_SelectTex_Color 	= 0x30,
		C_DisSelectTex_Color 	= 0xd7,
		C_CurValtage_Color 	= 0x41,
		
		
	};
	extern UINT32 ui32SysKeyState;
	ui8EnterPowerDownVoltage = 0xff;			
	
	hwDispPvEn(0);
	while((uiState & UI_MODE_MASK) == UI_MODE_TEST)
	{
	    	ui16Cur_Bat_AD_value = F_GetBatteryADValue() ;

	        switch(key)
	        {              
			case	 UI_KEY_MODE_TEST:	
				osdClearScreen( C_Back_Color);				
				osdStrDisp(00, 40, UserFont10x20, C_DisSelectTex_Color, "  If power down Voltage is OK");
				osdStrDisp(00, 60, UserFont10x20, C_DisSelectTex_Color, "please press OK key");
				osdStrDisp(00, 80, UserFont10x20, C_DisSelectTex_Color, "  press ESc key Esc");
				break;
#if 0
			case    UI_KEY_DIR_DOWN:
				if( ui8VoltageIndex < 0x03 )                               
				{
					ui8VoltageIndex++;
					ui8UpDateFlag = 0xff;
				}

				break;

			case    UI_KEY_DIR_UP:
				if( ui8VoltageIndex > 0x00 )                               
				{
					ui8VoltageIndex--;
					ui8UpDateFlag = 0xff;
				}

				break;			
		    
		case	   UI_KEY_FUNC_ESC:
				ui8VoltageFlag = 0x00;				
				break;
#endif
		case	   KEY_BIT_LEFT:
			if(ui8ProFlag != 0x00)
			{
				ui8Yes = 0xff;
				osdStrDisp(C_Yes_X, C_Yes_Y, UserFont10x20, C_Yes_Color, "Yes") ;
				osdStrDisp(C_Yes_X+200, C_Yes_Y, UserFont10x20, C_No_Color, "No") ;
			}
			
			break;
			
		case	   KEY_BIT_RIGHT:
			if(ui8ProFlag != 0x00)
			{
				ui8Yes = 0x00;
				osdStrDisp(C_Yes_X, C_Yes_Y, UserFont10x20, C_No_Color, "Yes") ;
				osdStrDisp(C_Yes_X+200, C_Yes_Y, UserFont10x20, C_Yes_Color, "No") ;
			}			

			break;
			
		//case    KEY_BIT_ESC:
		 case    KEY_BIT_MENU:
			uiState = UI_MAINMENU;			
			break;		
			
		case    KEY_BIT_OK:
			if(ui8ProFlag == 0x00)
			{
				ui8ProFlag = 0xff;
				
				TestADValue[0]= ui16Cur_Bat_AD_value;
				TestADValue[1]= ui16Cur_Bat_AD_value+(11); /* Paul@2006/06/03 22:37 modify */
				TestADValue[2]= TestADValue[1]+(12);
				TestADValue[3]= TestADValue[2]+(12);
				
				osdClearScreen( C_Back_Color);
				
				osdStrDisp(40, 30, UserFont10x20, C_DisSelectTex_Color, "Lefe or Right Key Change, ");
				osdStrDisp(40, 50, UserFont10x20, C_DisSelectTex_Color, "OK key Select,Esc Key Esc.");
				
				
				for(i = 0;i<4;i++)
				{
					sio_psprintf(str, "%s: %03d ",*(pui8LableText+i),TestADValue[i]);
					osdStrDisp(C_Lable_X, C_Lable_Y+C_Distance*i, UserFont10x20, C_DisSelectTex_Color, str) ;
				}

				ui8Yes=0xff;
				{
					osdStrDisp(C_Yes_X, C_Yes_Y, UserFont10x20, C_Yes_Color, "Yes") ;
					osdStrDisp(C_Yes_X+200, C_Yes_Y, UserFont10x20, C_No_Color, "No") ;
				}
			}
			else
			{
				if(ui8Yes)
				{
					sPara.ui8BatPowDownVol = TestADValue[0];
					sPara.ui8BatVol_Low = TestADValue[1];	
					sPara.ui8BatVol_Hight = TestADValue[2];
					sPara.ui8BatVol_Full = TestADValue[3];
					sPara.ui8UserPowerDownVolFlag = 0xA5 ;
					nandWriteParameters();
					uiState = UI_MAINMENU;
				}
				else
				{
					ui8ProFlag = 0x00;
					key = UI_KEY_MODE_TEST; 
					continue;
				}
			}
	       }
#if 1
		key = 0x00;
		ui32keynew = GetSystemKey();
		ui32i = ui32keynew^ui32keyold	;

		if(ui32i)
		{
			ui32keyold = ui32keynew;	
		}
		
		if(ui32i&ui32keynew)
		{
			key = ui32keynew; 
		}
#else
		msg = osQuePend(uiKeyMsgQ,10,&err);
		if(err == OS_NO_ERR) 
		{
			key = *msg	;
			sio_psprintf(str, "%s0x%08x ","Key=",key );
			osdStrDisp(C_CurAD_X, 200, UserFont10x20, C_CurValtage_Color, str) ;
			
		}
		else
		{
			key = 0xffffffff;
		}

#endif
		if(ui8ProFlag == 0x00)
		{
			sio_psprintf(str, "%s: %03d ",*(pui8LableText+0),ui16Cur_Bat_AD_value );
			osdStrDisp(C_CurAD_X, 160, UserFont10x20, C_CurValtage_Color, str) ;
		}
		osTimeDly(10);
	}
	ui8EnterPowerDownVoltage = 0x00;	
	ui32NextState = uiState = UI_MAINMENU;
	hwDispPvEn(1);
}
//xqq add for test voltage   
/************/
/*fqdao_add for high voltage Test 06\7\19 Night 03.54*/
/**************/
void	F_GetHighVoltageValueTest()
{

	UINT8 *pui8LableText[]=
	{
		"3.40 V",
		"3.60 V",
		"3.80 V",
		"4.00 V",
	};

	UINT32	ui32Cur_Bat_AD_value;
	UINT32   ui32G_ad3Vaule ;
	UINT32	TestADValue[4] = {0x00,0x00,0x00,0x00};
	UINT32 	ui32keyold = 0x00,ui32keynew = 0x00;	
	UINT32	ui32i	;
	UINT32	key = UI_KEY_MODE_TEST;
	UINT16	i,j;
	UINT32 	err = SUCCESS;
	UINT32 	* msg		;
	UINT8	ui8ProFlag = 0x00	;
	UINT8	ui8Yes = 0xff	;
	UINT8	str[100] ;

	#define	C_Plus_Value 	7
	enum
	{
		C_Lable_X 	= 100,
		C_Lable_Y 	= 90,
		C_Distance 	= 24,
		C_Test_X 	= C_Lable_X+60,
		C_Test_Y 	= C_Lable_Y,
		C_CurAD_X   = 100,
		C_Yes_X = 20,
		C_Yes_Y = 200,
		C_Yes_Color = 0xd1,
		C_No_Color = 0xf7,

		C_Back_Color 		= 0x07,
		C_SelectBar_Color 	= 0x00,
		C_SelectTex_Color 	= 0x30,
		C_DisSelectTex_Color 	= 0xd7,
		C_CurValtage_Color 	= 0x41,
		
		
	};
	extern UINT32 ui32SysKeyState;

	for(i=0;i<=31;i++)
	{
		 F_GetBatteryADValue() ;	
	}
	
	hwDispPvEn(0);
	while((uiState & UI_MODE_MASK) == UI_MODE_TEST)
	{
	    	ui32Cur_Bat_AD_value = F_GetBatteryADValue() ;
		ui32G_ad3Vaule   =  hwAdc3Data() ;	

	        switch(key)
	        {              
			case	 UI_KEY_MODE_TEST:	
				osdClearScreen( C_Back_Color);				
				osdStrDisp(00, 40, UserFont10x20, C_DisSelectTex_Color, "  If power High Voltage is OK");
				osdStrDisp(00, 60, UserFont10x20, C_DisSelectTex_Color, "  Adjust: Press OK key ");
				osdStrDisp(00, 80, UserFont10x20, C_DisSelectTex_Color, "  Cancel: Press ESc key Exit");
				break;
#if 0
			case    UI_KEY_DIR_DOWN:
				if( ui8VoltageIndex < 0x03 )                               
				{
					ui8VoltageIndex++;
					ui8UpDateFlag = 0xff;
				}

				break;

			case    UI_KEY_DIR_UP:
				if( ui8VoltageIndex > 0x00 )                               
				{
					ui8VoltageIndex--;
					ui8UpDateFlag = 0xff;
				}

				break;			
		    
		case	   UI_KEY_FUNC_ESC:
				ui8VoltageFlag = 0x00;				
				break;
#endif
		case	   KEY_BIT_LEFT:
			if(ui8ProFlag != 0x00)
			{
				ui8Yes = 0xff;
				osdStrDisp(C_Yes_X, C_Yes_Y, UserFont10x20, C_Yes_Color, "Yes") ;
				osdStrDisp(C_Yes_X+200, C_Yes_Y, UserFont10x20, C_No_Color, "No") ;
			}
			
			break;
			
		case	   KEY_BIT_RIGHT:
			if(ui8ProFlag != 0x00)
			{
				ui8Yes = 0x00;
				osdStrDisp(C_Yes_X, C_Yes_Y, UserFont10x20, C_No_Color, "Yes") ;
				osdStrDisp(C_Yes_X+200, C_Yes_Y, UserFont10x20, C_Yes_Color, "No") ;
			}			

			break;
			
		//case    KEY_BIT_ESC:
		 case    KEY_BIT_MENU:
			uiState = UI_MAINMENU;			
			break;		
			
		case    KEY_BIT_OK:
			if(ui8ProFlag == 0x00)
			{
				ui8ProFlag = 0xff;
				
				TestADValue[3]= ui32Cur_Bat_AD_value-C_POWVOLTAGE ;
				TestADValue[2]= TestADValue[3]-C_FULLVOLTAGE  ;
				TestADValue[1]= TestADValue[2]-C_HIGHTVOLTAGE; 
				TestADValue[0]= TestADValue[1]-C_LOWVOLTAGE  ;

			#if  0
				if (TestADValue[0] < (ui32G_ad3Vaule+45))
				{
					TestADValue[0] = ui32G_ad3Vaule+45; 
					TestADValue[1] = TestADValue[0]+C_LOWVOLTAGE; 	
					TestADValue[2] = TestADValue[1]+C_HIGHTVOLTAGE; 	
					TestADValue[3] = TestADValue[2]+C_FULLVOLTAGE; 	
				}
			#endif
			
				osdClearScreen( C_Back_Color);
				
				osdStrDisp(20, 30, UserFont10x20, C_DisSelectTex_Color, "Left or Right Key Select, ");
				osdStrDisp(20, 50, UserFont10x20, C_DisSelectTex_Color, "OK key Confirm,Esc Key Exit");
				
				
				for(i =0;i<4;i++)
				{
					sio_psprintf(str, "%s: %03d ",*(pui8LableText+i),TestADValue[i]);
					osdStrDisp(C_Lable_X, C_Lable_Y+C_Distance*i, UserFont10x20, C_DisSelectTex_Color, str) ;
				}

				ui8Yes=0xff;
				{
					osdStrDisp(C_Yes_X, C_Yes_Y, UserFont10x20, C_Yes_Color, "Yes") ;
					osdStrDisp(C_Yes_X+200, C_Yes_Y, UserFont10x20, C_No_Color, "No") ;
				}
			}
			else
			{
				if(ui8Yes)
				{
					sPara.ui8BatPowDownVol = TestADValue[0];
					sPara.ui8BatVol_Low = TestADValue[1];	
					sPara.ui8BatVol_Hight = TestADValue[2];
					sPara.ui8BatVol_Full = TestADValue[3];
					sPara.ui8UserPowerDownVolFlag = 0xA5 ;
					nandWriteParameters();
					uiState = UI_MAINMENU;
				}
				else
				{
					ui8ProFlag = 0x00;
					key = UI_KEY_MODE_TEST; 
					continue;
				}
			}
	       }
#if 1
		key = 0x00;
		ui32keynew = GetSystemKey();
		ui32i = ui32keynew^ui32keyold	;

		if(ui32i)
		{
			ui32keyold = ui32keynew;	
		}
		
		if(ui32i&ui32keynew)
		{
			key = ui32keynew; 
		}
#else
		msg = osQuePend(uiKeyMsgQ,10,&err);
		if(err == OS_NO_ERR) 
		{
			key = *msg	;
			sio_psprintf(str, "%s0x%08x ","Key=",key );
			osdStrDisp(C_CurAD_X, 200, UserFont10x20, C_CurValtage_Color, str) ;
			
		}
		else
		{
			key = 0xffffffff;
		}

#endif
		if(ui8ProFlag == 0x00)
		{
			sio_psprintf(str, "%s: %03d ","Full Voltage",ui32Cur_Bat_AD_value );
			osdStrDisp(C_CurAD_X-50, 160, UserFont10x20, C_CurValtage_Color, str) ;
		}
		osTimeDly(10);
	}
	ui8EnterPowerDownVoltage = 0x00;	
	ui32NextState = uiState = UI_MAINMENU;
	hwDispPvEn(1);
}





void  AdjustVoltageValueFunc() 
{
	
	UINT8 *pui8LableText[]=
	{
		"3.40 V",
		"3.60 V",
		"3.80 V",
		"4.00 V",
	};

	UINT32	ui32Cur_Bat_AD_value;
	
	UINT32	TestADValue[4] = {0x00,0x00,0x00,0x00};
	UINT32 	ui32keyold = 0x00,ui32keynew = 0x00;	
	UINT32	ui32i	;
	UINT32	key = UI_KEY_MODE_TEST;
	UINT16	i,j;
	UINT32 	err = SUCCESS;
	UINT32 	* msg		;
	UINT8	ui8ProFlag = 0x00	;
	UINT8	ui8Yes = 0xff	;
	UINT8	str[100] ;
	
	UINT8  ui8TabFlag=0 ;
	UINT8  ui8VoltageIndex=0 ;
	
	#define	C_Plus_Value 	7
	enum
	{
		C_Lable_X 	= 50, //100,
		C_Lable_Y 	= 90,
		C_Distance 	= 24,
		C_Test_X 	= C_Lable_X+60,
		C_Test_Y 	= C_Lable_Y,
		C_CurAD_X   = 100,
		C_Yes_X = 50,
		C_Yes_Y = 200,
		C_Yes_Color = 0xd1,
		C_No_Color = 0xf7,

		C_Back_Color 		= 0x07,
		C_SelectBar_Color 	= 0x00,
		C_SelectTex_Color 	= 0x30,
		C_DisSelectTex_Color 	= 0xd7,
		C_CurValtage_Color 	= 0x41,
		
		
	};
	extern UINT32 ui32SysKeyState;


	hwDispPvEn(0);
	
	while((uiState & UI_MODE_MASK) == UI_MODE_TEST)
	{
	    	//ui32Cur_Bat_AD_value = F_GetBatteryADValue() ;

	        switch(key)
	        {              
			case	 UI_KEY_MODE_TEST:	
					osdClearScreen( C_Back_Color);				
					TestADValue[0]=sPara.ui8BatPowDownVol ;
					TestADValue[1]=sPara.ui8BatVol_Low ;	
					TestADValue[2]=sPara.ui8BatVol_Hight ;
					TestADValue[3]=sPara.ui8BatVol_Full ;
					osdStrDisp(00, 40, UserFont10x20, C_DisSelectTex_Color, "  You Have Adjusted Voltage!");
					osdStrDisp(00, 60, UserFont10x20, C_DisSelectTex_Color, "  As Follows :(¡û¡ü¡ý¡ú) ");
					for(i =0;i<4;i++)
					{
						sio_psprintf(str, "%s: %03d ",*(pui8LableText+i),TestADValue[i]);
						osdStrDisp(C_Lable_X, C_Lable_Y+C_Distance*i, UserFont10x20, C_DisSelectTex_Color, str) ;
					}

				break;
				
			case    UI_KEY_DIR_DOWN:
				if(!ui8TabFlag)
				{
					if( ui8VoltageIndex == 0x03 )                               
					{
						ui8VoltageIndex=0 ;
					}
					else
					{
						ui8VoltageIndex++;
					}
			        }
				break;

			case    UI_KEY_DIR_UP:
				if(!ui8TabFlag)
				{
					if( ui8VoltageIndex > 0x00 )                               
					{
						ui8VoltageIndex--;
					}
					else
					{
						ui8VoltageIndex=3 ;
					}
				}
				break;			
		    
		case	 KEY_BIT_A:
				if( ui8TabFlag == 0x02 )                               
				{
					ui8TabFlag=0 ;
				}
				else
				{
					ui8TabFlag++;
				}
				break;
				
		case	   KEY_BIT_LEFT:

					if(!ui8TabFlag)
					{
						TestADValue[ui8VoltageIndex]--;
					}
			
				break;
			
		case	   KEY_BIT_RIGHT:
	
					if(!ui8TabFlag)
					{
						TestADValue[ui8VoltageIndex]++;
					}
				break;
			
		 case    KEY_BIT_MENU:
			 	UI_OSQFlush(uiKeyMsgQ);
				uiState = UI_MAINMENU;			
				break;		
				
		case    KEY_BIT_OK:
				if(ui8TabFlag==1)
				{
					sPara.ui8BatPowDownVol = TestADValue[0];
					sPara.ui8BatVol_Low = TestADValue[1];	
					sPara.ui8BatVol_Hight = TestADValue[2];
					sPara.ui8BatVol_Full = TestADValue[3];
					sPara.ui8UserPowerDownVolFlag = 0xA5 ;
					nandWriteParameters();
					uiState = UI_MAINMENU;
					UI_OSQFlush(uiKeyMsgQ);
				}
				else if(ui8TabFlag==2)
				{
					ui8ProFlag = 0x00;
					ui8TabFlag= 0x00 ;
					ui8VoltageIndex=0x00 ;
					key = UI_KEY_MODE_TEST; 
					continue;
				}
				break ;
			default :
				break ;
	       }
			
		for(i=0;i<4;i++)
		{
				sio_psprintf(str, "- %03d + ",TestADValue[i]);
				if(!ui8TabFlag)
				{
					osdStrDisp(C_Lable_X+150, C_Lable_Y+C_Distance*i, UserFont10x20,( i==ui8VoltageIndex)?C_Yes_Color:0xf7, str) ;
				}
				else
				{
					osdStrDisp(C_Lable_X+150, C_Lable_Y+C_Distance*i, UserFont10x20,0xf7, str) ;
				}
		}

		osdStrDisp(C_Yes_X, C_Yes_Y, UserFont10x20, (ui8TabFlag==1)?C_Yes_Color:C_No_Color, "Yes") ;
		osdStrDisp(C_Yes_X+150, C_Yes_Y, UserFont10x20, (ui8TabFlag==2)?C_Yes_Color:C_No_Color, "No") ;

#if 1
		key = 0x00;
		ui32keynew = GetSystemKey();
		ui32i = ui32keynew^ui32keyold	;

		if(ui32i)
		{
			ui32keyold = ui32keynew;	
		}
		
		if(ui32i&ui32keynew)
		{
			key = ui32keynew; 
		}
#else
		msg = osQuePend(uiKeyMsgQ,10,&err);
		if(err == OS_NO_ERR) 
		{
			key = *msg	;
			sio_psprintf(str, "%s0x%08x ","Key=",key );
			osdStrDisp(C_CurAD_X, 200, UserFont10x20, C_CurValtage_Color, str) ;
			
		}
		else
		{
			key = 0xffffffff;
		}

#endif
	//	if(ui8ProFlag == 0x00)
	//	{
	//		sio_psprintf(str, "%s: %03d ","Full Voltage",ui32Cur_Bat_AD_value );
	//		osdStrDisp(C_CurAD_X-50, 160, UserFont10x20, C_CurValtage_Color, str) ;
	//	}

		osTimeDly(10);
	}
	ui8EnterPowerDownVoltage = 0x00;	
	ui32NextState = uiState = UI_MAINMENU;
	hwDispPvEn(1);
}

