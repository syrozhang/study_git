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
//#define RS_232_DBG

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "irq.h"
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
#include "hw_gpio.h"
#include "sysg_api.h"
#include "aaa_api.h"
#include "sysg_api.h"
#include "dpof_api.h"
#include "hw_cpu.h"
#include "utility.h"
#include "badpix.h"/*badpix*/
#include "mp3_api.h"
#include "audio_api.h"
#include "asf_api.h"
#include "avi_api.h"
#include "hw_disp_ext.h"
#include "pub_api.h"
#include "otg_api.h"
#include "panel.h"
#include "hw_uart.h"
#include "dps_api.h"
#include "hw_sdram.h"
#include "snap_custom.h"
#include "xiaoao.h"
#include "hw_aud.h"
#include "vfs_api.h"
#include "gpio.h"
#include "card.h"

/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
#include "hw_front.h"
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
#include "wma_api.h"
#include "Hw_cdsp.h"
//wendy@050204 for dps begin
#include "dps_api.h"
#include "dps_custom.h" 
//wendy@050204 for dps end

/*CDSP_TOOL*/
#define CDSPTOOL_OPT	1

#if (CDSPTOOL_OPT)
/*CDSP_TOOL*/
#include "hw_cdsp_para.h"
#include "hw_cdspTool_api.h"
#endif

#include "sysregs.h"
#include "extern.h"

#include "ui_main.h"


//#define  UART0_RECEIVE_ENABLE
//#define  TVIN_CHECK_ENABLE

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* In this version, if the video clip MOV option is enabled               */
/* then ASF will be disabled                                              */
#define CLIP_OPTION_MOV		0
#define CLIP_OPTION_ASF		1
#define CLIP_OPTION_AVI     1

#define K_CUSTOMER_VERSION		((UINT8)11)
#define K_CUSTOMER_RELEASE		((UINT8)50)
#define K_CUSTOMER_MONTH		((UINT8)11)
#define K_CUSTOMER_DATE			((UINT8)30)

#define C_CMD_STK_SIZE			1024
#define C_CST_STK_SIZE          	512
#define C_HIS_STK_SIZE			512

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void cmdTask(void *);
void cstddetTask(void *pdata);
void UI_PccamStart(void);
void ledBlink(void);

void tmrTest(void);
void tmrTest1(void);
void tmrTest2(void);
void panelSelect(UINT32);
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
UINT32 uiCustomVersionGet(UINT16, UINT16, UINT8*, UINT16);
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
UINT32 uiDispBuffFill(UINT32 patt);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 hisTaskStack[C_HIS_STK_SIZE];
#ifdef UART0_RECEIVE_ENABLE
UINT32 cmdTaskStack[C_CMD_STK_SIZE];
#endif 
#ifdef TVIN_CHECK_ENABLE
UINT32 cstddetTaskStack[C_CST_STK_SIZE]; 
#endif

//#define LED_BLINK 0
UINT32 ledBlinkIdx;
#if   (LED_BLINK == 1)
UINT8  ledPattern[] = {0x81, 0x42, 0x24, 0x18, 0x24, 0x42, 0x81};
#elif (LED_BLINK == 2)
UINT8  ledPattern[] = {0xff, 0xe7, 0xc3, 0x81, 0x00, 0x81, 0xc3, 0xe7};
#endif
OS_EVENT* semApp;

#if	SUPPORT_PANEL_TOPPOLY
void	LC15004_write(UINT32 nData);
#endif

#if (CDSPTOOL_OPT)
/*CDSP_TOOL*/
UINT32	*pfirstCdspToolPara = &cdspToolPara[0];
UINT32	*psecondCdspToolPara = &cdspToolPara[1];
UINT32	cdspParaArraySize;
#endif

UINT8 SpeakFlag;//wendy for speak
UINT8 TVFlag = 0;//wendy@050307 for tv detect
UINT8 SpeakFlagMode;//wendy for speak about mp3 or video
UINT32 g_uiOldSdramClk;

//xqq add for test voltage   
extern	UINT32	key;
extern	UINT8  	DcPlug	;
extern	UINT32	ui32SysKeyState;
extern	UINT32	uiState;
extern 	 UINT8 	cardValid[];
extern 	 UINT32 	cardSelection;
//xqq add for test voltage   
/**************************************************************************
 *                                                                        *
 *  Function Name: __main                                                 *
 *                                                                        *
 *  Purposes: C entry function.                                           *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: This function should never return.                           *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
__main(
	void
)
{
	UINT8 rtcVal[6];
	UINT32 volum;
	struct tm time;
	
	#if POW_EN_HIGH
	gpioConfig(GPIO_PWR_EN, 1);
	gpioSet(GPIO_PWR_EN, 1);
	#else 
	gpioConfig(GPIO_PWR_EN, 1);
	gpioSet(GPIO_PWR_EN,0);
	#endif
#if 0	//xouer080616
	#if 1
	gpioConfig(GPIO_LCD_STB, 1);
	gpioSet(GPIO_LCD_STB, 1);			
	gpioConfig(GPIO_LCD_SHDB, 1);
	gpioSet(GPIO_LCD_SHDB, 0);		
	gpioConfig(GPIO_LCD_POWER, 1);
	gpioSet(GPIO_LCD_POWER, 1);
	#endif	
#endif	
	hwUartInit(0, 115200); /* Initialize UART registers (uartPort, baud) */	
	hwRtcEn();             /* Enable RTC */
	hwUart1Init(1, 115200);	//xouer080704
#if 0	//xouer080616
	#if (CurrentIC8751==0)	
    gpioConfig(GPIO_MUTE, 1);
    gpioSet(GPIO_MUTE, 1);
	#else		
	wm8751SoundEnable(0,0);
	F_Mp3SetVolum(0);		
	#endif								
#endif 	//xouer080616
	gpioConfig(GPIO_PWR_STAT, 1);     
	gpioSet(GPIO_PWR_STAT, 0);   
    hwWait(0, 30); 

#if 1	//xouer090520
	hwRtcTimerRead(rtcVal);	
	srand(*(UINT32 *)rtcVal);	
	/* Read RTC and check its validity */
	if ( hwRtcReliableRead() != 0x5a ) {
		hwRtcReliableWrite(0x5a);
		time.tm_sec  = 0;
		time.tm_min  = 0;
		time.tm_hour = 12;
		time.tm_mday = 1;   /* Day: 1 */
		time.tm_mon  = 1;   /* Month: 1 */
		time.tm_year = 100; /* Year: since 1900 */
		timeSet(&time);
	}
#endif
	irqInit();
	osInit();
	uartInit(0);
	uart1Init(1);		//xouer080704
	
	printf("\nTURNKEY PMP V1.3\n");
	printf("_____________________________________\n");
	printf("F/W compiled at %s, %s\n", __TIME__, __DATE__);
	semApp = osSemCreate("APP_SEM", 1);
	sysgInit();

	sysgPreviewMaxDimSet(BUFFER_MAX_PV_W, BUFFER_MAX_PV_H);	/* MUST >= 320x240 */
	sysgOSDMaxDimSet(BUFFER_MAX_OSD_W, BUFFER_MAX_OSD_H);
	#if	PIP_ENABLE
	sysgPIPMaxDimSet(BUFFER_MAX_PIP_W, BUFFER_MAX_PIP_H);
	#else
	sysgPIPMaxDimSet(0, 0);
	#endif
 	sysgBadPixMaxCntSet(3072);		/* sun@1030 pm 16:09 */	
	sysMemInit();	
	
	/* Do not change the order of the following function calls. */
	tmrInit();
	#if 1 //MY_USE_USB
	tmrEnable(0, 10, TMR_PERIODIC, NULL); /* timer0, 10 ms@ 48MHz */
	#endif

	/*Julie add 20040911 start*/
//	gpioConfig(GPIO_VIDEO_EN, 1);
//	gpioSet(GPIO_VIDEO_EN, 0);
//	hwWait(3,0);
	/*Julie add 20040911 end*/	
	dscInit(0);
	
	/* set sdram clock to 96 */
	#if 0
	hwSdramClkGet(&g_uiOldSdramClk);
	hwSdramClkSet(96);
	#endif
	#if MY_USE_USB
	gpioInit();
	#endif
	PNL_PlaybackSet();	
	
	#if 1
	UINT32 dispAddr;
	sysgMemDispAddrGet(&dispAddr);
	hwPlayBufAddrSet(dispAddr);
	#endif

	uiDispBuffFill(0x00);
	hwDispPvEn(0);	
	hwDispPipEn(0);	
	panelMainSelect(PANEL_TYPE_3);
	//shuai 2005/10/12
	//gpioSet(GPIO_LCD_POWER,0);
	OpenLcdDisplay(0);//fqdao_modify for bug 先亮背光06.6.14 
	//shuai 2005/10/12

	#ifdef UART0_RECEIVE_ENABLE
	osTaskCreate("CMD", cmdTask, (void *)C_CMD_STK_SIZE, (void *)(&cmdTaskStack[C_CST_STK_SIZE-1]), 20);
	#endif
	
	vfsInit();
	printf("Memory card detecting, please wait\n");
	nandDetect();

 	#if 1
	if (nandReadParameters() == FAIL)
	{
		printf("Nand Parameters error!\n");
		defautParameters();
	}
	#endif
	
	#if 0
	dpofInit();
	dpofHdrAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_GEN_CRT, "\"Model Name\"");
	dpofHdrAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_USR_NAM, "\"Huiling\", \"Chou\"");
	#endif

	xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);
	dirInit();
	
	cardDetect();	
	xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);	
	dirInit();
	
	#if 0
	xdcfFilterSet(xDCF_FILTER_MP3 | xDCF_FILTER_ASF);
	#endif

	exifInit(exifTagInsCallback, exifTagRmvCallback);
	
	pbTaskInit();
	#if 1
//	uiTVPlugDet();
//	F_OpenPowerDetect();
	#endif

	openScreen(sPara.uiPatternFlag);
 	hwDispPipEn(0);
 	hwDispPvEn(1);
#if 0	//xouer080907a
    hwWait(0, 50);              ///:	    
	DcJackDetect();
	if( (ui32SysKeyState&(KEY_BIT_A|KEY_BIT_B)) == (KEY_BIT_A|KEY_BIT_B))
	{
 		uiState = UI_MODE_TEST;
	}
	else
	{
		uiState = UI_MAINMENU;			
	}
	hwWait(0, 400);
	/*hwDispPvEn(1);*/
	#if STILL_FUNC
	snapTaskInit();
	snap3AFuncSet(aaaPreviewToSnap, aaaSnapToPreview); /* register 3A callback */
	snapDateStampFuncSet(snapDateStampCallback); /* Register date stamp callback */
	snapExifModifyFuncSet(exifTagModifyCallback); /* Register Exif tag modification call back */
	snaprgbDSPFuncSet(snaprgbEnhanceCallback); /* Register RGB color enhance callback */
	snapyuvDSPFuncSet(snapyuvEnhanceCallback); /* Register YUV color enhance callback */

	#if 1
	snapQTblFuncSet(snapQTblCallback); /* Register your own compression call back */
	/* Specify the progress bar call back rountine, the function would be called
	aftet each 16384 bytes data was written */
	
	snapSaveProgressFuncSet(snapWriteFileCallback, 16384); /* Paul@2005/12/26 mask */

	/*snapQVLengthFuncSet(snapQVLengthCallback);*/ /* Register QV length callback if
	want to do you own UI work while quick view is shown */
	#endif	
	#endif
#else
	DcPlug = 1;
#endif	
	
	/*pbTaskInit();*/
	usbInit();
   	usbFuncReg_PccamStart(UI_PccamStart);
	
	#if 1
	otgInit();
	#if 1
	otgFuncReg_DPCtrl(otgDPCtrl);
	otgFuncReg_DMCtrl(otgDMCtrl);
	otgFuncReg_PowerCtrl(otgPowerCtrl);
	otgReg_MsdcMnt(otgMount);
	otgReg_MsdcUnmnt(otgUnmount);
	/*
	otgJackIdSet(OTG_JACKID_HOST);
	otgHostEn(TRUE);
	*/
	otgJackIdSet(OTG_JACKID_SLAVE);
	otgHostEn(FALSE);	
	#endif
	#endif

	#if 0
	/* for PictBridge function */
	//wendy@050204 update for dps begin
	dpsInit();
	dpsFuncReg_dpsDiscovered(dpsDiscovered, TRUE);//是否连接好
	dpsFuncReg_notifyDeviceStatus(dpsDeviceStatus, TRUE);//得到设备属性
	dpsFuncReg_notifyJobStatus(dpsJobStatus,TRUE);
	dpsEnable(TRUE);
	//wendy@050204 update for dps end
	#endif

	#if 1   		
	/* Set the usb detect gpio for usb plug in/off event */
	/* for Example: GPIO 8 is low when USB Power is low (Please reference release note V1.7.7) */
	usbDetectGpioReg(8,0);
	usbFuncReg_PlugIn(uiUsbPlugIn, 1);
	usbFuncReg_PlugOff(uiUsbPlugOut,1);
		/* usb copy data flash LED fqdao 06.5.16*/
 	usbFuncReg_MsdcBeforeRead(uiUSBLedOn);
 	usbFuncReg_MsdcAfterRead(uiUSBLedOff);
 	usbFuncReg_MsdcBeforeWrite(uiUSBLedOn);
 	usbFuncReg_MsdcAfterWrite(uiUSBLedOff);
		/* usb copy data flash LED fqdao 06.5.16*/
	usbFuncReg_MsdcWp(uiUSBMsdcWp);
	usbFuncReg_Suspend(usbSuspendCallback);
	usbFuncReg_Resume(usbResumeCallback);
	#endif	

	#if 1
	/*usbFuncReg_UsbReadVendorReq(0xa0, uiCustomVersionGet);*/
	{
		UINT8 str[5];
		str[0]=K_CUSTOMER_VERSION;
		str[1]=K_CUSTOMER_RELEASE;
		str[2]=K_CUSTOMER_MONTH;
		str[3]=K_CUSTOMER_DATE;
		usbCustomVersionSet(str);
	}
	#endif

#if 0 //xouer080907
	audTaskInit();	/* dlchiou - 03/04/30 17:48 */

	#if CLIP_OPTION_ASF
	asfTaskInit();
	#endif
	#if CLIP_OPTION_MOV
	movTaskInit(OS_PRIO_MOV_CLIP,OS_PRIO_MOV_PLAY);
	#endif
	#if CLIP_OPTION_AVI
	aviTaskInit(OS_PRIO_AVI_CLIP,OS_PRIO_AVI_PLAY);
	#endif
	#if MP3_OPT
	mp3TaskInit();
	#endif
	#if WMA_OPT
	wmaTaskInit();
	wmaDebugSet(1);
	#endif
#endif	
#if MY_USE_USB //xouer080909
	aaaTaskInit();
#endif	

	printf("\nPress enter to continue...\n");
	tmrPeriodFuncSet(osTimeTick, 10);

	#ifdef LED_BLINK
	tmrPeriodFuncSet(ledBlink, 8); /* 100 ms */
	#endif

#if 0	//xouer080616
	mp3RecTaskInit();
#endif
	#if STILL_FUNC
	/*PNL_PreviewSet();*/
	#endif
	#if MY_USE_USB //xouer080909
	osTaskCreate("HISTOGRAM", hisTask, (void *)C_HIS_STK_SIZE, (void *)(&hisTaskStack[C_CST_STK_SIZE-1]), 60);
	#endif
	/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
	#if 0
	hwProbeEn(9);
	#endif
	/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
	
	#if 1
	uiInit();
	osdCustomInit();
	#endif

	sysInfoPrint();
	tmrPeriodFuncSet(AutoTimeOff, 10000);
	#if 0
	hwFrontSiggenSet(1,1,15); /*15 is Moving horizontal color bar */
	#endif
	#if 0
	badPixelInit();/*badpix*/
	#endif
	
	#if 0
	asfAppBitRateCtrlEn(1, 500000); /* use this to set bit rate of mpeg 4 to 500K */
	#endif
	
	/*sysgAudOutDevSet(AUDIO_PWM);
	 * Here below is the sample code
	 * for audio device setting
	 */
	SpeakFlag = 0;
#if 0	//xouer080907a
	#if 1	 
	if (gpioGet(GPIO_DET_EAR))
	{	
		SpeakFlag = 0;
		TVFlag = 2;
	}
	else
	{
		SpeakFlag = 1;
		TVFlag = 0; 
	}
	#endif

	UINT32  codecDev;	
	#if (CurrentIC8751 == 0)
	sysgAudOutDevSet(AUDIO_AC97);
	sysgAudOutDevGet(&codecDev);
	if( codecDev == AUDIO_AC97 ) 
	{
		ws9712LineInAndPhoneOutSet();
		ws9712LineOut(SpeakFlag);
		ws9712LineIn(0);
		volSet();	
	}
	else if( codecDev == AUDIO_IIS ) 
	{
		//wm8751LineOutSet();
	}
	#else	
	sysgAudOutDevSet(AUDIO_IIS);
	if (G_OV3620_OB_Mode == false)	/* sun@1030 pm 19:03 for ov3620 OB calibration*/
	{
		wm8751L_Initial ();
	}
	#endif	
#endif	

	#if MY_USE_USB
	#if (SUPPORT_LFN == 1)
	vfsNlsCp936Init();
	/*ret = */vfsNlsSet("cp936");
	#endif
	#endif
#if 0	//xouer080907a
	#if (CDSPTOOL_OPT)
	/*CDSP_TOOL*/
	cdspParaArraySize = hwCdspTool_GetParaArrySize(cdspToolPara);
	hwCdspTool_Init();
	cdspToolRgbDSPFuncSet(snaprgbEnhanceCallback);
	cdspToolYuvDSPFuncSet(snapyuvEnhanceCallback);
	#endif
#endif	

	#ifdef TVIN_CHECK_ENABLE
	osTaskCreate("CSTDDET", cstddetTask, (void *)C_CST_STK_SIZE, (void *)(&cstddetTaskStack[C_CST_STK_SIZE-1]), 39);				
	#endif
	
    extern UINT8 first_poweron;   	
	gpioConfig(GPIO_DET_PWR, 0);
	#if POW_EN_HIGH
	if(!gpioGet(GPIO_DET_PWR))
	#else
	if(gpioGet(GPIO_DET_PWR))
	#endif
	{
		first_poweron=0;
	}

	InitBatteryValue();	

	/* Paul@2006/01/04-19:28 add start */
//	extern UINT8 icon_idx;
//	icon_idx = sPara.ui8Mode;
//	ui_ModeSwitch(sPara.ui8Mode);	
#if 0  //xqq add for test voltage 
	uiState = UI_MAINMENU;
#endif
	ui32NextState = uiState;
	sio_printf("Power on = %d\n", first_poweron);	

	osStart();
}

#ifdef UART0_RECEIVE_ENABLE
/**************************************************************************
 *                                                                        *
 *  Function Name: cmdTask                                                *
 *                                                                        *
 *  Purposes:                                                             *
 *    Task for command parser (commnands are read from RS-232).           *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    pdata: in, task's replicated argument.                              *
 *                                                                        *
 *  Returns: Never returns.                                               *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
cmdTask(
	void *pdata
)
{
	cmdInit();
	cmdMemInit();   /* commands for dump/fill/search/xsend(XMODEM) */
	cmdSmcInit();   /* commands for read/write SMC and NAND        */
	#if 0
	cmdSdInit();
	#endif

	#if 0
	cmdDcfInit();   /* commands for DCF file system                */
	cmdDpofInit();  /* commands for DPOF print jobs                */
	#endif
	cmdDcfInit();

	cmdOsInit();    /* commands for displaying OS information      */
	cmdUsbInit();   /* commands for USB applications               */
	cmdStillInit();
	#if CLIP_OPTION_ASF
	cmdMpeg4Init(); /* commands for clip/playback ASF files        */
	#endif
	#if CLIP_OPTION_MOV
	cmdMovInit();   /* commands for clip/playback MOV files        */
	#endif
	#if CLIP_OPTION_AVI
	cmdAviInit();   /* commands for clip/playback AVI files        */
	#endif
	#if MP3_OPT
	cmdMp3Init();   /* commands for MP3 files                      */
	#endif
	#if WMA_OPT
	cmdWmaInit();
	#endif
	cmdCdspInit();	/* commands for CDSP APIs						*/
	#if 0
	cmdUiInit();    /* commands for UI testing                     */
	#endif
	cmdZoomInit();
	while ( 1 ) {
		cmdMonitor();
	}
}
#endif	

/**
**/
UINT32 state[] = {0x20,1};
UINT8 Tvmode = 1;
extern UINT32 MAXVSIZEM_NOW;

#ifdef TVIN_CHECK_ENABLE
void cstddetTask(void *pdata)
{
#if 1
	while(1)
	{
		if (gpioSetSts(GPIO_VIDEO_EN) != 0)
		{
			hwFrontSscRead(0x1f,state,0x01,0x02);
			state[0] &= 0x20;
			if(state[0]!=state[1])
			{
				/*
				printf("ly_debug go to cstddetTask\n");
			  	hwFrontVValidWait(0, 1);
			  	*/
				if(state[0])
				/*saa7113Init(0);*/
				{ 
					MAXVSIZEM_NOW=244;
				  	/* WRITE8(0xb0002068,0x72);*/
				    if(snapAV==OBJ_ASF)
	                { 
						hwCdspHscaleYUVSet(720, 324);
    	                hwCdspVscaleSet(244,245);/*(244, 244);*/
					}
	  			    else
					{
					    hwCdspHscaleYUVSet(720, 320);
    	                hwCdspVscaleSet(244,244);/*(244, 244);*/
								
					}					
				    Tvmode=0;
				}
				else
				{  
					MAXVSIZEM_NOW=288;
				   /*saa7113Init(1);*/
				   /*WRITE8(0xb0002068,0x70);*/
				    if(snapAV==OBJ_ASF)
	                { 
				   		hwCdspHscaleYUVSet(720, 324);
    	                hwCdspVscaleSet(300,252);/*(288, 240);*/
					}
				    else
			  		{  
						hwCdspHscaleYUVSet(720, 322);
			     		hwCdspVscaleSet(288,242);/*(288, 240);*/
	  			    }
                    Tvmode=1;
				}
				hwFrontVValidWait(0, 1);                   /* Wait Vertical Valid falling edge for field 0 */
			}
			state[1] = state[0];
		}
		osTimeDly(50);
	}
#endif
}
#endif

void UI_PccamStart(void)
{
  state[1] = 1;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: customerCmd                                            *
 *                                                                        *
 *  Purposes: Define user's commands.                                     *
 *                                                                        *
 *  Descriptions:                                                         *
 *    This function is left for users to define their own commands. The   *
 *    default one will echo user's input of arguments.                    *
 *                                                                        *
 *  Arguments:                                                            *
 *    ]: in, pointer to command's token.                              *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
customerCmd(
	UINT8* param
)
{
	#if 0
	UINT8 *pParam[8];
	UINT8 pStr[64];
	UINT32 i,j;

	/*** parse the parameter ****/
	for (i = 0; i < 8; i++) {
		pParam[i] = NULL;
	}
	strcpy(pStr,param);
	pParam[0] = pStr;
	for (i = 0, j = 1;i < strlen(param); i++)
	{
		if ((pStr[i] == '-') || (pStr[i] == '/') || (pStr[i] == ' '))
		{
			pStr[i] = 0;
			pParam[j] = &pStr[i + 1];
			j++;
			if (j > 8) break;
		}
	}
	
	printf("param=%s\n", param);
	if (strcmp(pParam[0], "tvout") == 0)
	{
		dispReg_t* pDispReg;
		UINT32 rgbValue;
		
		/** Paul@2006/03/25 add start **/
		pDispReg = (dispReg_t*)(0xb000A000);
		printf("Rgb = %d\n", pDispReg->oddRgb);
		rgbValue = atoi(pParam[1]);		
		pDispReg->oddRgb = rgbValue;
		printf("after Rgb = %d\n", pDispReg->oddRgb);		
		/** Paul@2006/03/25 add end **/		
	}
	/*for (i = 0; i < j;i++)
	{
		printf("param[%d]=[%s]\n",i,pParam[i]);
	}
	*****

	if (strcmp(pParam[0], "tvout") == 0)
	{
		UINT32 hueValue, satValue;
		if (strcmp(pParam[1], "w") == 0)
		{
			satValue = atoi(pParam[2]);
			hueValue = atoi(pParam[3]);			
			WRITE8(0xb000A021, satValue);	//saturation
			WRITE8(0xb000A022, hueValue);				
			printf("Write Hue = %x, sat = %x\n", hueValue, satValue);
		}
		else
		{
			satValue = READ8(0xb000A021);
			hueValue = READ8(0xb000A022);
			printf("Read Hue = %x, sat = %x\n", hueValue, satValue);
		}
	}
	
	if(strcmp(pParam[0], "panel1")==0)
	{
		panelSelect(PANEL_TYPE_1);
		printf("\n chg to panel 1 ... \n");
	}
	if(strcmp(pParam[0], "panel2")==0)
	{
		panelSelect(PANEL_TYPE_2);
		printf("\n chg to panel 2 ... \n");
	}
	if(strcmp(pParam[0], "panel3")==0)
	{
		panelSelect(PANEL_TYPE_3);
		printf("\n chg to panel 3 ... \n");
	}
	if(strcmp(pParam[0], "smcDiskScan")==0)
	{
		if(strcmp(pParam[1], "0")==0)
			smcDiskScan(0);
		else if(strcmp(pParam[1], "1")==0)
			smcDiskScan(1);
		else
			printf("nand test param wrong\n");
	}*/
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
	/*if(strcmp(pParam[0], "pinyin")==0)
	{
		UINT8 strout[1024];
		if(pinyin(pParam[1], strout))
		{
			printf("Type in OK!!\n");
			printf(strout);
			printf("\n");
		}
		else
			printf("Type in wrong!\n");
	}
	if(strcmp(pParam[0], "flash")==0)
	{
		gpioFuncSet(GPIO_USB_FLASH, 0);
		gpioConfig(GPIO_USB_FLASH, 1);
		hwFrontFlashSet(0x100, 0, 0x0);
		WRITE8(0xb0009104, 0x10);
	}
	if(strcmp(pParam[0], "image")==0)
	{
		openScreen(0);
	}*/
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
/*(Gu@ 2004/07/06, 9:54 */



	if(strncmp(pParam[0], "AC_RD", 5) == 0)
	{
		UINT32 value;
#if (CurrentIC8751==0)		
		printf("AC_RD\n");
		hwAudAc97RegRead(0x24, &value);
		printf("0x24:%08x\n", value);
		hwAudAc97RegRead(0x26, &value);
		printf("0x26:%08x\n", value);
#else

#endif
	}
	if(strcmp(pParam[0], "crtlfn")==0)
	{
		printf("\n create lfn folder ... \n");
		UINT32 ret;
		ret = !SUCCESS;
		if(SUCCESS == vfsChdir("C:"))
		{printf("C:\\");
			if(SUCCESS == vfsChdir("DCIM"))
			{printf("DCIM\\");
				if(SUCCESS == vfsChdir("100MEDIA"))
				{printf("100MEDIA\n");
					if(SUCCESS != vfsChdir("abcd1234.a.32"))
						ret = vfsMkdir("abcd1234.a.32");
					if(SUCCESS != ret)
						printf("mk dir failed\n");
				}
			}
		}
	}

	if(strncmp(pParam[0], "VK_", 3) == 0)
	{
	}

/*)Gu@ 2004/07/06, 9:55 */
	#endif
}

#ifdef LED_BLINK
/**************************************************************************
 *                                                                        *
 *  Function Name: ledBlink                                               *
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
ledBlink(
	void
)
{
	hwGenGpioWrite(~ledPattern[ledBlinkIdx]);
	ledBlinkIdx++;
	if ( ledBlinkIdx == sizeof(ledPattern) ) {
		ledBlinkIdx = 0;
	}
}
#endif

/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
#define K_CUSTOMER_VERSION		((UINT8)0x01)
#if 1
#define K_CUSTOMER_RELEASE		((UINT8)0x00)
#define K_CUSTOMER_SEQUENCE		((UINT8)0x00)
#define K_CUSTOMER_CHIPNUMBER	((UINT8)0x08)
#endif

UINT32 uiCustomVersionGet(UINT16 index, UINT16 value, UINT8* str, UINT16 length)
{
	str[0]=K_CUSTOMER_VERSION;//
	str[1]=K_CUSTOMER_RELEASE;
	str[2]=K_CUSTOMER_SEQUENCE;
	str[3]=K_CUSTOMER_CHIPNUMBER;
	printf("index=0x%04x, value=0x%04x, length=0x%04x\n", index, value, length);
	return SUCCESS;
}

void dirInit()
{
	#if 0
// 	xdcfRootDirAdd(".", 0);     
	xdcfRootDirAdd(DVDCRootNameStr,0); 	
	xdcfRootDirAdd(mediaRootNameStr,0);
	xdcfRootDirAdd(mp3SubDirMP3Str,0);	
	xdcfRootDirAdd(otherRootNameStr,0);   

	xdcfCurRootDirSet(DVDCRootNameStr);	
	xdcfCurRootDirSet(mediaRootNameStr);
	xdcfCurRootDirSet(mp3SubDirMP3Str);	
	xdcfCurRootDirSet(otherRootNameStr);	
	#endif
}
/*andrew_gu@2005/May/11, 13:31 )*/

