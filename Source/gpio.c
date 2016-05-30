/**************************************************************************
 *                                                                        *
 *        Copyright (c) 2002 by Sunplus Technology Co., Ltd.             *
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
//#define		RS_232_DBG

#include 		<time.h>
#include 		"general.h"
#include 		"gpio.h"
#include 		"os_api.h"
#include 		"timer.h"
#include 		"hw_gpio.h"
#include 		"hw_sdram.h"
#include 		"stdio.h"
#include 		"osd_main.h"
#include 		"ui_main.h"
#include 		"asf_api.h"
#include 		"avi_api.h"
#include 		"mov_api.h"
#include 		"audio_api.h"
#include 		"mp3_api.h"
#include 		"xiaoao.h"
#include 		"aaa_api.h"
#include 		"dsc_init.h"
#include 		"sysg_api.h"
#include 		"wma_api.h"
#include 		"utility.h"
#include		"mp3_api.h"
#include 		"mp3en_api.h"
#include 		"Usb_api.h"
#include 		"hw_cpu.h"
#include 		"panel.h"
#include        "hw_pwrctrl.h"


#include 		"uistrings.h"
#include 		"close.h"
#include 		"parameter.h"

#define 		TASK_KEY_STACK_SIZE		1024

#define 		PLAY_GAME_MODE			0
//#define BATTERY_OSD_X   PANEL_1_OSD_W-130-40
#define 		BATTERY_OSD_X   		PANEL_1_OSD_W-106		//John modify 051020
#define 		BATTERY_OSD_Y   		0
#define 		No_ADamend  			0     //05-05-28--Danny
#define 		AVGCOUNT 				32
/**************************************************************************/
//XOUER0506
//#define SELF_GPIO_DEFINE	

#ifdef SELF_GPIO_DEFINE
#define GPIO_KEY_COL_1	(GPIO_TYPE_FM+22)
#define GPIO_KEY_COL_2	(GPIO_TYPE_FM+28)

#define GPIO_KEY_ROW_1	(GPIO_TYPE_FM+ 26)
#define GPIO_KEY_ROW_2	(GPIO_TYPE_FM+ 25)
#define GPIO_KEY_ROW_3	(GPIO_TYPE_FM+ 24)
#define GPIO_KEY_ROW_4	(GPIO_TYPE_FM+ 23)
#define GPIO_KEY_ROW_5	(GPIO_TYPE_FM+ 27)
#endif
/**************************************************************************/
extern void 	pbVolumWindowRfsh(SINT32 value);
extern void 	pbRepeatWindowRfsh(void);
extern void 	hwDispOsdEn(UINT32 enable);
extern UINT32 	usbChangeMode(UINT32 mode);
extern void 	osdLcdDisp(UINT8 option);

extern UINT16 	uiAccessr_App;
extern UINT32 	wmaVol;
extern struct 	tm clock1_t;
extern UINT8 	pressVolume; 			//Julie@2004.9.17
extern UINT8  	lcdOsdDisp;

extern UINT8 	SpeakFlag;				//wendy for speak
extern UINT8 	SpeakFlagMode;			//wendy for speak
extern UINT8 	TVFlag;					//wendy@050307 for tv detect
//Paul@2005/08/25 select panel for video, audio
extern UINT32 	mp3Disp;
extern UINT8 	imgBuf[];
extern UINT32 	ChgPvSetting;			//1=restore pv setting before process any key 
extern UINT32 	atoCounter;				//wendy@050204 add for auto off
extern UINT8 	sub_menu_setup ;		//wendy@050309 add for tv detect when tv mode
extern UINT32 	osstatus;

//machiner@2005/12/06 
extern UINT8		*pui8ReadBuf;
extern UINT32       *pui32SavePage;
//extern UINT8    	ui8EBookReadNow;


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void 			keyScanTask(void *pdata);
void 			ShowGoodBye(void);

//************************************************************************** 
UINT8 			ui8VolCnt = 0; 			// tzw 05/09/14
UINT8  			Keylong = 0;				//wendy@050301

UINT8 			mp3FastSpeed = 0;
UINT8 			wmaFastSpeed = 0;
UINT8 			ASFFastSpeed = 0; 
UINT8 			mp3Pause = 0 ;
UINT8 			wmaPause = 0;
UINT8 			g_playrecStatus = 0;
UINT8 			speed = 0;				//Paul@2005/08/25 to global
UINT8 			ui8SnapTone;
UINT8 			first_poweron=1;
UINT8 			pwrcount = 0;
UINT8 			Batt_idx=0;
UINT8 			count_EAR=0;
UINT8			PowerLock;
UINT8			uiKeyPush;
UINT8  			swDclast=0xff;
UINT8  			DcPlug = 0;
UINT8  			LastBatteryLevel=BATTERY_LEVEL_FULL, BatteryLevel=BATTERY_LEVEL_FULL, BatLevcount;
UINT8 			RepKeyEn=1;
UINT8 			ConKeyEn=0;
UINT8			FirstOn = 0;
UINT8 			BATTERY_LEVEL_FULL_Osd[] = {8,9,0x00};
UINT8 			BATTERY_LEVEL_HIGH_Osd[] = {10,11,0x00};
UINT8 			BATTERY_LEVEL_LOW_Osd[] = {12,13,0x00};
UINT8 			BATTERY_LEVEL_OUT_Osd[] = {14,15,0x00};

OS_EVENT 		*mqKeyId;         /* Message queue ID for key scan               */
void     		*mqKey[10];       /* Message queue buffers                       */
UINT32 			keyScanTaskStack[TASK_KEY_STACK_SIZE];

static 			UINT32 	ui32SeriesKey;
static 			UINT8 	ui8LcdKeyCnt = 0;
static 			UINT8 	keyCount[3];
static 			UINT32 	swButton[3]; /* Key scan record for column 1 and 2          */
static 			void	*semApp;

//bool G_OV3620_OB_Mode = false;/* sun@1030 pm 19:03 for ov3620 OB calibration*/

UINT16 g_ui16PowerOffFlag = 0;	/* Paul@206/03/20-19:11 add */

//xqq add for test voltage   
#define c_BatLowPowCount	10
UINT8	BatLowPowCount = 0x00;
//xqq add for test voltage   

//************************************************************************** 
UINT8 Bat2Count[32]=
{
		0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
		0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
		0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
		0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,	
};
UINT8 Bat3Count[32]=
{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

/**
	Paul@2006/01/23 add
**/
UINT8 ui8KeyHold = 0;
UINT8 ui8KeyHoldCnt = 0;

UINT8 g_ui8EnterUpdateFlag = 0;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 keyButton[16]=
{
	UI_BUTTON_0,
	UI_BUTTON_1,
	UI_BUTTON_2,
	UI_BUTTON_3,
	UI_BUTTON_4,
	UI_BUTTON_5,
	UI_BUTTON_6,
	UI_BUTTON_7,
	UI_BUTTON_8,
	UI_BUTTON_9,
	UI_BUTTON_10,
	UI_BUTTON_11,
	UI_BUTTON_12,
	UI_BUTTON_13,
	UI_BUTTON_14,	
	UI_BUTTON_15		
};
UINT32 keyRotary[16]=
{
	UI_SWITCH_0,
	UI_SWITCH_1,
	UI_SWITCH_2,
	UI_SWITCH_3,
	UI_SWITCH_4,
	UI_SWITCH_5,
	UI_SWITCH_6,
	UI_SWITCH_7,
	UI_SWITCH_8,
	UI_SWITCH_9,
	UI_SWITCH_A,
	UI_SWITCH_B,
	UI_SWITCH_C,
	UI_SWITCH_D,
	UI_SWITCH_E,
	UI_SWITCH_F,
};
UINT32 keyUnknow[1]=
{
	UI_UNKOWN,
};
UINT32 keyButtonswitch1[ ]=
{
#if  Oem268_Dc260
	0, UI_KEY_FUNC_A, UI_KEY_FUNC_B, UI_KEY_FUNC_A 
#else
	//0, UI_KEY_FUNC_B, UI_KEY_FUNC_A, UI_KEY_FUNC_B 
	0, UI_KEY_FUNC_MENU, UI_KEY_FUNC_A, UI_KEY_FUNC_MENU 
#endif	

/*	
	0,UI_KEY_FUNC_ZOOMIN,UI_KEY_FUNC_ZOOMOUT,UI_KEY_FUNC_ZOOMIN,
	UI_KEY_FUNC_MODE,UI_KEY_FUNC_MODE,UI_KEY_FUNC_MODE,UI_KEY_FUNC_MODE,
	UI_KEY_FUNC_MENU,UI_KEY_FUNC_MENU,UI_KEY_FUNC_MENU,UI_KEY_FUNC_MODE,
	UI_KEY_FUNC_MODE,UI_KEY_FUNC_MODE,UI_KEY_FUNC_MODE,UI_KEY_FUNC_MODE,
*/
};

UINT32  keyButtonswitch2[ ]=
{
	0,UI_KEY_DIR_UP,UI_KEY_DIR_DOWN,UI_KEY_DIR_UP,
	UI_KEY_DIR_LEFT,UI_KEY_DIR_LEFT,UI_KEY_DIR_LEFT,UI_KEY_DIR_LEFT,
	UI_KEY_DIR_RIGHT,UI_KEY_DIR_RIGHT,UI_KEY_DIR_RIGHT,UI_KEY_DIR_RIGHT,
	UI_KEY_DIR_RIGHT,UI_KEY_DIR_RIGHT,UI_KEY_DIR_RIGHT,UI_KEY_DIR_RIGHT,

	UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,
	UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,	
	UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,
	UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,UI_KEY_FUNC_OK,
};	
//*********************************************************************
extern UINT8 uiDCStatus;

static  UINT8 BatteryWranColorFlag =0  ;
void	uiShowBatteryLevelIcon(UINT8 BatteryLevel)
{    
	/*fqdao_add for close  after adjust 06.7.17*/
	UINT32 MainMenuColor ;
	UINT32 NeutralColor;
	/* Paul@2006/12/24 - 16:13 add start */
	#if ((!C_LOWCLOSE_GPIO) && (Oem268_Dc260))
	/* Paul@2006/12/24 - 16:13 add end */
	if (sPara.ui8UserPowerDownVolFlag == 0xA5 )
	{
		MainMenuColor=0xe00;
		NeutralColor=0xd00;
	}
	else
	{
		MainMenuColor=0x100;
		NeutralColor=0x100;		
	}
	#else
	
		MainMenuColor=0xe00;
		NeutralColor=0xd00;
	#endif
	BatteryWranColorFlag++;

	/*fqdao_add for close  after adjust 06.7.17*/
	
	if(uiDCStatus)
	    osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, UserIcon10x20, 0xd0, "\x92\x93");
	else
	{
		if ((uiState & UI_MODE_MASK ) == UI_MAINMENU)
		{

			switch (BatteryLevel)
		    {
		        case BATTERY_LEVEL_FULL:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, UserIcon10x20, MainMenuColor, BATTERY_LEVEL_FULL_Osd);
		            break;

		        case BATTERY_LEVEL_HIGH:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, UserIcon10x20, MainMenuColor, BATTERY_LEVEL_HIGH_Osd);
		            break;

		        case BATTERY_LEVEL_LOW:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, UserIcon10x20, MainMenuColor, BATTERY_LEVEL_LOW_Osd);
		            break;

		        case BATTERY_LEVEL_OUT:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, UserIcon10x20, MainMenuColor, BATTERY_LEVEL_OUT_Osd);
		            break;
					
			  case BATTERY_LEVEL_WRAN:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, BatterWarn10x20, (BatteryWranColorFlag & 0x01) ? 0x100 : 0xe00, "\x02\x03");
				break ;

		        default:
		            break;
		    }
		}
		else
		{
			switch (BatteryLevel)
		    {
		        case BATTERY_LEVEL_FULL:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, UserIcon10x20, NeutralColor, BATTERY_LEVEL_FULL_Osd);
		            break;

		        case BATTERY_LEVEL_HIGH:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, UserIcon10x20, NeutralColor, BATTERY_LEVEL_HIGH_Osd);
		            break;

		        case BATTERY_LEVEL_LOW:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, UserIcon10x20, NeutralColor, BATTERY_LEVEL_LOW_Osd);
		            break;

		        case BATTERY_LEVEL_OUT:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, UserIcon10x20, NeutralColor, BATTERY_LEVEL_OUT_Osd);
		            break;
			  case BATTERY_LEVEL_WRAN:
		            osdStrDisp (BATTERY_OSD_X, BATTERY_OSD_Y, BatterWarn10x20, (BatteryWranColorFlag & 0x01) ? 0x100 : 0xd00,  "\x02\x03");
				break ;

		        default:
		            break;
		    }
		}	
	}
}

//*********************************************************************
#ifdef MMP_GPIO_DEFINE 
void gpioInit(void)
{
	semApp = osEventFindByName("APP_SEM");
	
	hwGenGpioIrqInit();
	hwFmGpioIrqInit();
	/* Select and configure TG_GPIO[0:3]/[4:5] as inputs/outputs */    
	
	/* Paul@2006/01/23 mask start */	
#ifdef SELF_GPIO_DEFINE
	gpioConfig(GPIO_KEY_COL_1, 1);	//xouer0506
	gpioConfig(GPIO_KEY_COL_2, 1);
	gpioConfig(GPIO_KEY_ROW_1, 0);
	gpioConfig(GPIO_KEY_ROW_2, 0);
	gpioConfig(GPIO_KEY_ROW_3, 0);
	gpioConfig(GPIO_KEY_ROW_4, 0);
	gpioConfig(GPIO_KEY_ROW_5, 0);
#endif
	F_SetSeriesKey(0x00f);
	/* Paul@2006/01/23 mask end */		
		
	/* Configure DRAM_GPIO[2:5] as inputs */
	hwSdramGpioSet(0);

	osTaskCreate("KEY SCAN", keyScanTask, (void *)TASK_KEY_STACK_SIZE, (void *)(&keyScanTaskStack[TASK_KEY_STACK_SIZE-1]), OS_PRIO_KEYSCAN);
	mqKeyId = osQueCreate("UI_KEY", mqKey, 10);
	if ( mqKeyId == NULL ) 
	{
		printf("Key que create err!\n");
	}
	
	/* 
	 *Initialize SW button only since rotary switch should be read
	 * after power-on reset. 
	 */
	swButton[0] = 0;
	swButton[1] = 0;
	swButton[2] = 0;
	keyCount[0] = 0;
	keyCount[1] = 0;
	keyCount[2] = 0;	
	uiKeyPush = 0;
	
	gpioConfig(GPIO_LCD_SHDB, 1);
	gpioSet(GPIO_LCD_SHDB, 0);
	gpioConfig(GPIO_LCD_STB, 1);
	gpioSet(GPIO_LCD_STB, 1);

	gpioConfig(GPIO_DET_PWR, 0);
/*	
	gpioConfig(GPIO_PWR_EN, 1);
	
#if POW_EN_HIGH
	gpioSet(GPIO_PWR_EN, 1);
#else 
	gpioSet(GPIO_PWR_EN, 0);
#endif
*/

	gpioConfig(GPIO_SENSOR_RST, 1);
	gpioSet(GPIO_SENSOR_RST, 0);
	
#if SENSOR_PWR_SET
	gpioConfig(GPIO_SENSOR_PWDN, 1);
	gpioSet(GPIO_SENSOR_PWDN, 0);	
	gpioConfig(GPIO_SENSOR_POWER, 1);	/* Paul@2005/09/16 */
	gpioSet(GPIO_SENSOR_POWER, 0);	
#else
	gpioConfig(GPIO_SENSOR_PWDN, 1);
	gpioSet(GPIO_SENSOR_PWDN, 1);	
	gpioConfig(GPIO_SENSOR_POWER, 1);	/* Paul@2005/09/16 */
	gpioSet(GPIO_SENSOR_POWER, 1);	
#endif	

	gpioConfig(GPIO_USB_DET, 0);
    gpioConfig(GPIO_DET_DC, 0);
	gpioConfig(GPIO_DET_EAR, 0);

	gpioConfig(GPIO_SELF_LED, 1);	/* John @12/24/2005 */
	gpioSet(GPIO_SELF_LED, 0);

	gpioConfig(GPIO_CARD_EN, 1);
	gpioSet(GPIO_CARD_EN, 1);    
	
	gpioConfig(GPIO_BUSY_LED, 1);	
	gpioSet(GPIO_BUSY_LED, 0);	

	/* Paul@2006/06/03 22:52 add  */
    hwAdcSet(ADC_ENABLE, ADC_CLK_2MHZ, 0x0b);

	/**low Voltage close 06/08/07**/
	hwFmGpioCfgSet(0,  0, 0x00, 0x80);
	gpioConfig(GPIO_LOWCLOSE, 0);	

	
}

//************************************************************************************************
void keyScanTask(void *pdata)
{
	while(1)
	{
		osTimeDly(3);
		F_ScanPowerKey();
		if(!F_ScanUSBStatus())
		{
			continue;
		}
		//F_ScanCard();
		F_ScanEarStatus();
		F_ScanGeneralKey();
	}
}

//**********************************************************************************
void	F_SetSeriesKey(UINT32 ui32KeyBit)
{
	UINT32 sr;

//	ENTER_CRITICAL(sr);
	ui32SeriesKey = ui32KeyBit;
//	EXIT_CRITICAL(sr);
}

//**********************************************************************************
UINT8 ui8BusyLedCnt = 0;

extern  UINT8 ui8GameFlag ;
extern  UINT8 uiOnBusy;

UINT32 g_ui32SaveKeyCode;

void F_ScanGeneralKey(void)
{
	UINT32 	gpioInput;
	UINT32 	keyIdx;
	UINT32 	i, j;
	UINT8  	DcJack;
	UINT32 	err;
	UINT8   ui8Temp;

/*
	if (uiOnBusy)
	{
		ui8BusyLedCnt++;
		gpioSet(GPIO_BUSY_LED, ((ui8BusyLedCnt & 0x03) == 0x03)); 
	}
*/ 

	DcJack = DcJackDetect();          
	if (DcJack != swDclast)
	{
		if(swDclast == 0xff)
		{
		 	swDclast = 0;
		}
		else
		{ 
			swDclast = 0xff;
		}

		#if Oem268_Dc260
		if (avPlayNow != OBJ_NONE)		//sun@060608
		{
			 gpioConfig(GPIO_MUTE, 1);
		     gpioSet(GPIO_MUTE, 0);
				
			#if 0
			if( (avPlayNow == OBJ_ASF) ||(avPlayNow == OBJ_AVI) ||(avPlayNow == OBJ_MOV))
			{
				hwAudAC97RegWrite(0x2c, g_asfsamplerate);
				hwAudAC97RegWrite(0x2e, g_asfsamplerate);
				hwAudAC97RegWrite(0x32, g_asfsamplerate);		//sun@ open 
			}
			ws9712LineOut(SpeakFlag);//wendy for speak
			if(SpeakFlag)
			{
				pbSetVolume(2, mp3Vol);
			}
			else
			{
				pbSetVolume(1, mp3Vol);
			}

			#else
			if (avPlayNow != OBJ_NONE)
			{
				UI_OSQFlush(uiKeyMsgQ);
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);
			}
			#endif

		}
		hisIconDisp();	//sun@060608
		return;
		#else	
		hisIconDisp();				
		#endif
	}

	if (DcPlug == 0)	
	{
	    LowBatDetect();
	}

#ifdef SELF_GPIO_DEFINE
	extern UINT32 ui32SysKeyState;
	g_ui32SaveKeyCode = 0;

	gpioSet(GPIO_KEY_COL_1, 1);
	gpioSet(GPIO_KEY_COL_2, 0);
	if(gpioGet(GPIO_KEY_ROW_1))	g_ui32SaveKeyCode|=KEY_BIT_UP;
	if(gpioGet(GPIO_KEY_ROW_2))	g_ui32SaveKeyCode|=KEY_BIT_DOWN;
	if(gpioGet(GPIO_KEY_ROW_3))	g_ui32SaveKeyCode|=KEY_BIT_LEFT;
	if(gpioGet(GPIO_KEY_ROW_4))	g_ui32SaveKeyCode|=KEY_BIT_RIGHT;
	if(gpioGet(GPIO_KEY_ROW_5))	g_ui32SaveKeyCode|=KEY_BIT_OK;
			
	gpioSet(GPIO_KEY_COL_1, 0);
	gpioSet(GPIO_KEY_COL_2, 1);
	if(gpioGet(GPIO_KEY_ROW_1))	g_ui32SaveKeyCode|=KEY_BIT_A;
	if(gpioGet(GPIO_KEY_ROW_2))	g_ui32SaveKeyCode|=KEY_BIT_B ;
//	if(gpioGet(GPIO_KEY_ROW_3))	g_ui32SaveKeyCode|=KEY_BIT_MENU;
	if(gpioGet(GPIO_KEY_ROW_4))	g_ui32SaveKeyCode|=KEY_BIT_MENU;
//	if(gpioGet(GPIO_KEY_ROW_5))	g_ui32SaveKeyCode|=KEY_BIT_HOLD;
			
	/* Clear outputs */
	gpioSet(GPIO_KEY_COL_1, 0);
	gpioSet(GPIO_KEY_COL_2, 0);
	ui32SysKeyState = g_ui32SaveKeyCode;
#else
	g_ui32SaveKeyCode = GetSystemKey();	
#endif
	if (g_ui32SaveKeyCode & KEY_BIT_HOLD)
	{
		if (ui8KeyHold == 0)
		{
			ui8KeyHoldCnt++;
			if (ui8KeyHoldCnt == 20)
			{
				ui8KeyHold = 1;
			}			
		}
		else
		{
			ui8KeyHoldCnt = 0;
		}
	}
	else
	{
		if (ui8KeyHold)
		{
			ui8KeyHoldCnt++;
			if (ui8KeyHoldCnt == 20)
			{
				ui8KeyHold = 0;
			}					
		}
		else
		{
			ui8KeyHoldCnt = 0;
		}		
	}
	/* Paul@2006/03/20 add start */
	g_ui32SaveKeyCode &= (~KEY_BIT_HOLD);
	if (g_ui32SaveKeyCode)
	{
		atoCounter = 0;
	}
	/* Paul@2006/03/20 add end */	
	
	if (ui8KeyHold == 1)
	{
		return;
	}

	if ((g_ui32SaveKeyCode & KEY_BIT_A)
	 &&(g_ui32SaveKeyCode & KEY_BIT_B))
	{
		sPara.USBModeFlag = USB_MODE_PCCAM;
		usbChangeMode(sPara.USBModeFlag);		
		g_ui8EnterUpdateFlag = 1;	
	}
	for (i=0; i<3; i++)
	{
		#define KEY_DEDOUND_CNT		0x03
		switch (i)
		{
			case 0:
				gpioInput = g_ui32SaveKeyCode & 0x1f;
				keyIdx = keyButtonswitch2[gpioInput];
				ui8Temp = ui32SeriesKey & 0x1f;
				
				j = swButton[0] ^ gpioInput;
				swButton[0] = gpioInput;				
				if (!j && gpioInput)
				{
					keyCount[0]++;
					if (keyCount[0] == KEY_DEDOUND_CNT)
					{
					}
					else
					{						
						if (keyCount[0] == 10)
						{
							if ((gpioInput & ui8Temp) != gpioInput)
							{
								gpioInput = 0;
							}							
							keyCount[0] = 5;
						}
						else
						{
							gpioInput = 0;
						}
					}
				}				
				else
				{
					gpioInput = 0;
					keyCount[0] = 0;
				}
				break;

			case 1:				
				gpioInput = (g_ui32SaveKeyCode>>5) & 0x03;
				keyIdx = keyButtonswitch1[gpioInput];				
				
				j = swButton[1] ^ gpioInput;
				swButton[1] = gpioInput;				
				if (!j && gpioInput)
				{
					keyCount[1]++;
					if (keyCount[1] == KEY_DEDOUND_CNT)
					{
					}
					else
					{
						gpioInput = 0;
						if (keyCount[1] == 10)
						{
							keyCount[1] = 5;
						}
					}
				}								
				else
				{
					gpioInput = 0;
					keyCount[1] = 0;
				}				
				break;

			case 2:
				gpioInput = (g_ui32SaveKeyCode>>10) & 0x08;				
				
				#if Oem268_Dc260
				keyIdx = UI_KEY_FUNC_MENU;
				#else
				keyIdx = UI_KEY_FUNC_B;
				#endif
				
				j = swButton[2] ^ gpioInput;
				swButton[2] = gpioInput;				
				if (!j && gpioInput)
				{										
					keyCount[2]++;
					if (keyCount[2] == KEY_DEDOUND_CNT)
					{
					}
					else
					{
						gpioInput = 0;
						if (keyCount[2] == 10)
						{
							keyCount[2] = 5;
						}
					}
				}
				else
				{
					gpioInput = 0;
					keyCount[2] = 0;
				}				
				break;
		}
		
		if (gpioInput)
		{
			PostKeyInQue(&keyButton[keyIdx]);			

		}
	}
	
	if ((keyCount[0] | keyCount[1]) >= 1)	/* Paul@2006/02/06 add ">=5" */
	{
		uiKeyPush = 1;
	}
	else
	{
		uiKeyPush = 0;
	}	
	/* Paul@2006/01/23 add end for common */	
}

//**********************************************************************************
void	F_ScanEarStatus(void)
{
//	if (G_OV3620_OB_Mode == false)	/* sun@1030 pm 19:03 for ov3620 OB calibration*/
	{
		if(SpeakFlag)
		{
			if(gpioGet(GPIO_DET_EAR))
			{
				count_EAR++;
			}
			else
			{
				count_EAR = 0;
			}
			if(count_EAR == 10)
			{
				count_EAR = 0;
				SpeakFlag = 0;
#if 0				
				if (sub_menu_setup == 2/* SP_TV_Out*/)
				{
					TVFlag = 2;
					osQuePost(mqKeyId, &keyButton[10]); 
				}	
#endif
				#if 1
				#if (CurrentIC8751==0)
					ws9712LineOut(0);
				#else
					wm8751LineOut(0);
				#endif
				#endif
	  			volSet();	
			}
		}
		else if (!SpeakFlag)
		{
			if(!gpioGet(GPIO_DET_EAR))
			{
				count_EAR++;
			}
			else
			{
				count_EAR = 0;
			}
			if(count_EAR == 10)
			{
				count_EAR = 0;
				SpeakFlag = 1;
			//	if (!avPlayNow)
				{
					TVFlag = 1;
				}
#if 0			
				if (sub_menu_setup == 2/* SP_TV_Out*/)
				{
					osQuePost(mqKeyId, &keyButton[10]); 
				}	
				else 
#endif
					
				if (PANEL_TYPE_1 != getCurPanel())
				{
					osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);  
					osTimeDly(100);     		
					gpioSet(GPIO_LCD_POWER, 1);
					hwWait(0,300);
					panelSelect(PANEL_TYPE_1);
					sPara.TvoutFlag = TVOutOff;
					TVFlag = 0;			
					if (ChgPvSetting == 1)
					{
						hwDispPvSetRead(imgBuf);
					}				
					OpenLcdDisplay(1) ;  // fqdao_modify for bug TVOut osdcleanScreen  @  2006.5.6 
				}	
			#if (CurrentIC8751==0)
				ws9712LineOut(1);
			#else
				wm8751LineOut(1);
			#endif
	 			volSet();	
			}
		}
	}
}	




//**********************************************************************************

extern UINT8 G_Printing;
extern UINT8 G_PrintingNoExit;
extern UINT8 g_ui8InPrintMode;


UINT8	F_ScanUSBStatus(void)
{
	UINT32 gpioInput;
	UINT32 gpioInput1;
	UINT32 status;

	static UINT8  ioCount;
		
#if Oem268_Dc260
	/* DC268A/T */
	gpioConfig(GPIO_USB_DET, 0);
	ENTER_CRITICAL(status);
	gpioInput = gpioGet(GPIO_USB_DET);
	EXIT_CRITICAL(status);

	#if 0
	if((!gpioInput)&&(!gpioInput1))
	{
		gpioConfig(GPIO_DET_UDISK, 1);  
		gpioSet(GPIO_DET_UDISK, GPIODETUDISK);
		uiDCStatus = 1 ;
	}
	else 
	{
//		gpioConfig(GPIO_DET_UDISK,1);  
//		gpioSet(GPIO_DET_UDISK,GPIODETUDISK);	
		uiDCStatus = 0 ;		
	}
	#endif	
/* xouer0507
	if((!gpioInput) && (uiUSBStatus == 0))
	{	
		uiUsbPlugIn();
	}
	else if((gpioInput) && (uiUSBStatus > 0))
	{
		uiUsbPlugOut();
	}		
*/	
	if (uiUSBStatus > 0)
	{	
		return FALSE;
	}
	else
	{
		return TRUE;
	}
#else
	/* DC260A */	
	gpioConfig(GPIO_USB_DET, 0);
	gpioConfig(GPIO_DET_DC, 0);
	ENTER_CRITICAL(status);
	gpioInput = gpioGet(GPIO_USB_DET);
	gpioInput1 = gpioGet(GPIO_DET_DC); 
	EXIT_CRITICAL(status);
	
	#if 1 //fqdao_mopdify 06.7.6
	if((!gpioInput)&&(!gpioInput1))
	{
		//gpioConfig(GPIO_DET_UDISK, 1);  
		//gpioSet(GPIO_DET_UDISK, GPIODETUDISK);
		uiDCStatus = 1 ;
	}
	else 
	{
//		gpioConfig(GPIO_DET_UDISK,1);  
//		gpioSet(GPIO_DET_UDISK,GPIODETUDISK);	
		uiDCStatus = 0 ;		
	}
	#endif	
    ioCount ++ ;

	if((!gpioInput) && (uiUSBStatus == 0)&&(!uiDCStatus))
	{	
		if(ioCount>10)
		{
			uiUsbPlugIn();
			ioCount= 0 ;
		}
	}
	else if((gpioInput) && (uiUSBStatus > 0)&&(!uiDCStatus))
	{
		if(ioCount>10)
		{
			uiUsbPlugOut();
			ioCount= 0 ;
		}
	}		
    else
    {
	  	ioCount= 0 ;
  	}
 	if (uiUSBStatus > 0)
	{	
		return FALSE;
	}
	else
	{
		return TRUE;
	}

#endif
}


//**********************************************************************************
void F_OpenPowerDetect(void)
{
	UINT32 gpioInput;
	UINT32 status;
	UINT32 ui32BatValue ;
	UINT32 ui32BatValueSave ;
	UINT8	i;
	
	ENTER_CRITICAL(status);
	gpioInput = gpioGet(GPIO_USB_DET) ;
	EXIT_CRITICAL(status);

	for(i=0;i<32;i++)
	{
		ui32BatValueSave = F_GetBatteryADValue();
		if(ui32BatValue !=ui32BatValueSave) i=0 ;
		ui32BatValue = ui32BatValueSave ;
		hwWait(0,10 ) ;
	}
	
	if((ui32BatValue < 91)&&(gpioInput))
	{
		gpioConfig(GPIO_PWR_STAT, 1);
		for(i=0 ;i<8;i++){
			gpioSet(GPIO_PWR_STAT,(i&0x01));
			hwWait(0, 500);
		}
		gpioSet(GPIO_AUDIO_EN, 0); 
		hwWait(0, 50);
		#if POW_EN_HIGH
			gpioSet(GPIO_PWR_EN, 0);
		#else 
			gpioSet(GPIO_PWR_EN,1);
		#endif		
	}
}


/***********/
#if 1
/**
**/
UINT32 Bat_AD_value;
UINT32 ADamend_value;
UINT32 F_GetBatteryADValue(void)
{
      	UINT32 Final_Bat_AD_value;
	UINT32 i;
	UINT32 BatLev_temp;

	UINT32 SumRowstatus = 0;
	UINT32 SumADamend = 0;

	hwAdcSet(ADC_ENABLE, ADC_CLK_2MHZ, 0x0b);
	
	/* Paul@2006/06/03 20:12 add start */
//	Bat2Count[Batt_idx] = (hwAdc2Data() < 130) ? 255 : hwAdc2Data();
	Bat2Count[Batt_idx] =  hwAdc2Data();
	/* Paul@2006/06/03 20:12 add end */
	
    	for (i = 0; i < AVGCOUNT; i++)
    	{			
		SumRowstatus += Bat2Count[i];
		if (Bat2Count[i] < 130)
		{
			SumRowstatus += 255;
		}
    	}
//		Bat_AD_value = (SumRowstatus>>5)+128;
		Bat_AD_value = (SumRowstatus>>5);
		Final_Bat_AD_value = Bat_AD_value; 
		
	#if  No_ADamend
		Final_Bat_AD_value = Bat_AD_value;    
	#else
		Bat3Count[Batt_idx]=hwAdc3Data();
			
		 //xqq add for test voltage   
		 #if BatVolDebug
			UINT8	strttt[100] ;
			sio_psprintf(strttt, "%s%04d ","ADC2 = ",hwAdc2Data() );
			osdStrDisp(20, 80, UserFont10x20, 0xdf, strttt) ;
			sio_psprintf(strttt, "%s%04d ","ADC3 = ",hwAdc3Data() );
			osdStrDisp(20, 120, UserFont10x20, 0xdf, strttt) ;
		//	osTimeDly(20);
		#endif	


		//xqq add for test voltage   
			for (i = 0; i < AVGCOUNT; i++)
			{
				SumADamend += Bat3Count[i];
			}
		//	ADamend_value=(SumADamend>>5)+128;
			ADamend_value = (SumADamend>>5);
			//Final_Bat_AD_value = Bat_AD_value - ADamend_value; 
			
		 #if  BatVolDebug
		 	UINT8	strtest[100] ;
			sio_psprintf(strtest, "%s%04d ","AD2/ = ",Bat_AD_value );
			osdStrDisp(20, 190, UserFont10x20, 0xdf, strtest) ;
			sio_psprintf(strtest, "%s%04d ","AD3/ = ",ADamend_value );
			osdStrDisp(20, 170, UserFont10x20, 0xdf, strtest) ;
		#endif	

			/*
			    if(ADamend_value>=128)
			    {
			        Final_Bat_AD_value = (UINT8) (Bat_AD_value-(ADamend_value-128));
			    }
			    else
			    {
			        Final_Bat_AD_value= (UINT8) (Bat_AD_value+(128 - ADamend_value));
			    }
			*/
#endif 

		Batt_idx++;
		Batt_idx%=32;
		
	return	Final_Bat_AD_value ;	
}

#endif
//*********************************************************************
void	F_ScanPowerKey(void)
{
	if(first_poweron == 0)
	{	
		if (ui8KeyHold == 1)
		{	/* Key hold */
			return;
		}
		
		#if POW_EN_HIGH
			if(gpioGet(GPIO_DET_PWR) && (!uiUSBStatus)) 
		#else 
			if((!gpioGet(GPIO_DET_PWR)) && (!uiUSBStatus)) 
		#endif
		{
			 pwrcount++;
			 if(pwrcount > 20)
			 {	
			 	g_ui16PowerOffFlag = 1;
			 	//F_CloseScreenFunc();
			 }	
		}
		else
		{
			pwrcount = 0;		
		}
	}
	else
	{
		#if POW_EN_HIGH
		if(gpioGet(GPIO_DET_PWR)==0)
		#else 
		if(!(gpioGet(GPIO_DET_PWR)))
		#endif
		{
			first_poweron = 0;
		}	
	}

	/* Paul@206/03/20-19:11 add start */
	if (g_ui16PowerOffFlag)
	{
		F_CloseScreenFunc();
	}
	/* Paul@206/03/20-19:11 add end */
}
//*********************************************************************
void	F_CloseScreenFunc(void)
{
	UINT8   ui8tzwVol;  // tzw 05-10-03
    UINT32 	err;
	
	ui8tzwVol = sPara.uiBeepFlag; 
    sPara.uiBeepFlag = BeepOff;  //tzw modify 05-10-03	    

	ui32NextState = UI_MODE_POWEROFF;
	tmrDisable(1);
	if(snapAV == OBJ_ASF)
	{
		snapAV = OBJ_NONE;
		asfAppClipAbort();  
	}
	else if(snapAV == OBJ_MP3)
	{
		snapAV = OBJ_NONE;
		mp3AppRecStop();
	}
	else if (snapAV == OBJ_WAV)
	{
		snapAV = OBJ_NONE;
		audRiffCtrl(RIFF_STOP);
	}
		    
	UI_OSQFlush(uiKeyMsgQ);    
	if (isMenuOn)				
	{
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
	}				
		
	if(ui8GameFlag) 
	{
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);
	}
	else
	{
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
	}

	while (uiState != UI_MODE_POWEROFF)
	{
		osTimeDly(20);
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);			
	}

	sPara.uiBeepFlag = ui8tzwVol;
	ShowGoodBye();
//	hwWait(0, 500);

	hwPowerDisable(MODULE_AUDIO);	/* Paul@2006/04/15 add */
	
	OpenLcdDisplay(0);
	gpioSet(GPIO_LCD_POWER,0);
	/* Paul@2006/12/23 - 17:33 add start */
	uiDispBuffFill(0x00);
	/* Paul@2006/12/23 - 17:33 add end */
	hwWait(0, 50);
	
#if (CurrentIC8751==0)	
    gpioConfig(GPIO_MUTE, 1);
    gpioSet(GPIO_MUTE, 1);
#else		
	wm8751SoundEnable(0,0);
#endif		
	hwWait(0, 50);

	gpioSet(GPIO_PWR_STAT, 1);   

#if POW_EN_HIGH
	gpioSet(GPIO_PWR_EN, 0);
#else 
	gpioSet(GPIO_PWR_EN, 1);
#endif
	hwWait(0, 50);
	printf("keyscan:pwr\n");

}
//*********************************************************************
void ShowGoodBye(void)
{
	UINT32	osdW, osdH;
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	UINT32 dispW, dispH;
	UINT32 dispAddr;
	
	buttonAudio(1);	
	nandWriteParameters();

	sysgMemDispAddrGet(&dispAddr);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(dispAddr);				
	osdClearScreen(0x00);
	#if (!SPLASH_NOCLOSEIMAGE_FLAG)
	pbDramImgPlay(close_cy, sizeof(close_cy), dispAddr, dispW, dispH);	
	#else
	uiDispBuffFill(0);
	OpenLcdDisplay(0);
	#endif
	hwDispPvEn(1);
	osdClearScreen(0x00);	
	#if (!SPLASH_NOCLOSEIMAGE_FLAG)
	hwWait(0, 2500);	
	#endif
}

//*********************************************************************
void 	UI_OSQFlush(OS_EVENT *pevent)
{
   OS_Q *pq;
   ENTER_CRITICAL(osstatus);
   pq = pevent->osEventPtr;
   pq->osQueIn = pq->osQueStart;
   pq->osQueOut = pq->osQueStart;
   pq->osQueEntries = 0;
   EXIT_CRITICAL(osstatus);
}
//*********************************************************************
UINT8	DcJackDetect()
{
	/* Paul@2006/12/24 - 17:09 add start */
	#if Oem268_Dc260
	/* DC268A/T */
	if(gpioGet(GPIO_DET_DC))		
	#else	
	/* DC260A */
	if((!gpioGet(GPIO_DET_DC))&&(!gpioGet(GPIO_USB_DET)))		
	#endif	
	/* Paul@2006/12/24 - 17:09 add end */
	{
		DcPlug = 1;
		return 0;
	}
	else
	{
		DcPlug = 0;
		return 0xff;
	}
	return 0xff;
}
//*********************************************************************

/**
	2006/06/03 20:07 
**/
void InitBatteryValue()
{
	UINT32 i;

	#if 1
	for (i=0; i<32; i++)		
	{
		Bat2Count[i] = hwAdc2Data();		
		Bat3Count[i] = hwAdc3Data();
	}
	#endif
}

/* Paul@2006/12/24 - 16:12 add start */
#if ((C_LOWCLOSE_GPIO) || (!Oem268_Dc260/*260*/))
/* Paul@2006/12/24 - 16:12 add end */
void LowBatDetect()
{
	BatteryValueRead();
}
#else

/**/
extern UINT8	ui8EnterPowerDownVoltage; //xqq add for test voltage   
   // UINT32 ADamend_value;
   // UINT32 Bat_AD_value;
   
UINT32 g_ui32DetectBatCnt = 0;      
   
void LowBatDetect()
{
	
    UINT32 Final_Bat_AD_value;
    UINT32 i;
    UINT8 BatLev_temp;
    UINT32 SumRowstatus = 0;
    UINT32 SumADamend = 0;
//    UINT8  str[3];
	UINT32 err;

	#if 0
	g_ui32DetectBatCnt++;
	if (g_ui32DetectBatCnt != 4)
	{
		return;
	}
	g_ui32DetectBatCnt = 0;
	#endif

	//xqq add for test voltage   
	if( ui8EnterPowerDownVoltage )	
	{
		return;
	}
	
	//xqq add for test voltage   
	Final_Bat_AD_value = F_GetBatteryADValue() ;
	
#if  BatVolDebug
	UINT8	strttt[100] ;
	sio_psprintf(strttt, "%04d ",Final_Bat_AD_value );
	osdStrDisp(20, 50, UserFont10x20, 0x10, strttt) ;
	printf("_AD_value=%u \n",Final_Bat_AD_value);
#endif

   //xqq add for test voltage   
   	if (Final_Bat_AD_value < 130)		// 3.4V
    	{
		Final_Bat_AD_value = 255;
 	}

	#if 0
	sPara.ui8BatPowDownVol = (ADamend_value*1350/1000);
	sPara.ui8BatVol_Low = (sPara.ui8BatPowDownVol + 6);
	sPara.ui8BatVol_Hight = (sPara.ui8BatVol_Low + 11);
	sPara.ui8BatVol_Full = (sPara.ui8BatVol_Hight + 13);	
	#endif

	if (Final_Bat_AD_value < sPara.ui8BatPowDownVol)		// 3.4V
	{
		if(BatLowPowCount < c_BatLowPowCount*2 )
		{
			BatLowPowCount++;
		}
	}
	else
	{
		BatLowPowCount = 0x00;
	}
	/* Test */
	if(BatLowPowCount >= c_BatLowPowCount)	
	{
		if (sPara.ui8UserPowerDownVolFlag == 0xA5 )
		{
			ui32NextState = UI_MODE_POWEROFF;
			tmrDisable(1);
			#if 1	//Return from Snap
			if(snapAV == OBJ_ASF)
			{
				snapAV = OBJ_NONE;
				asfAppClipAbort();  
			}
			else if(snapAV == OBJ_MP3)
			{
				snapAV = OBJ_NONE;
				mp3AppRecStop();   
			}
			else if (snapAV == OBJ_WAV)
			{
				snapAV = OBJ_NONE;
				audRiffCtrl(RIFF_STOP);
			}
			#endif

			UI_OSQFlush(uiKeyMsgQ);
			if (isMenuOn)				
			{
		//		osQuePost(mqKeyId, &keyButton[UI_KEY_FUNC_MENU]);					
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
			}				
				
			if(ui8GameFlag) 
			{
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);
			}
			else
			{
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
			}		
			
	//		osTimeDly(40);		// tzw 05-10-03

	//		osSemPend(semApp , 0, &err);	 /*	wait until current application finish its job*/
	//		osSemPost(semApp);	   /* wait until current application finish	its	job*/				
			while (uiState != UI_MODE_POWEROFF)
			{
				osTimeDly(20);
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
			}
			osdClearScreen(0x07);
      		osdStrDisp(PANEL_1_OSD_W/2-10*(F_STRCTRL(GetString(NO_Battery))/2), PANEL_1_OSD_H/2-20, UserFont10x20,  0x17, GetString(NO_Battery));

	        hwWait(0, 500);
	        hwWait(0, 500);
	        hwWait(0, 500);
	        hwWait(0, 500);
	        hwWait(0, 500);
	        hwWait(0, 500);
				
	        ShowGoodBye();
	        //  hwWait(0, 500);
		   	//gpioSet(GPIO_LCD_SHDB, 0);
		    	
	 		OpenLcdDisplay(0);
			gpioSet(GPIO_LCD_POWER,0);
			hwWait(0, 50);
			#if (CurrentIC8751==0)	
		 	    gpioConfig(GPIO_MUTE, 1);
			    gpioSet(GPIO_MUTE, 1);
			#else		
				wm8751SoundEnable(0,0);
			#endif

			gpioSet(GPIO_PWR_STAT, 1);   
	//		gpioSet(GPIO_AUDIO_EN, 0); 
	  	 	hwWait(0, 50);		
			#if POW_EN_HIGH
				gpioSet(GPIO_PWR_EN, 0);
			#else 
				gpioSet(GPIO_PWR_EN,1);
			#endif
	 	 }
	  	else
	  	{
		   osdStrDisp(20, 150, UserFont10x20, 0x10, "Adjust Voltage Please!") ;
	  	}

	}
	/* Test */
   if(Final_Bat_AD_value<=sPara.ui8BatPowDownVol+3)
   { 
	BatLev_temp=	BATTERY_LEVEL_WRAN ;
   }
   else if (Final_Bat_AD_value < sPara.ui8BatVol_Low)
   {
        BatLev_temp= BATTERY_LEVEL_OUT;
   }
   else if (Final_Bat_AD_value < sPara.ui8BatVol_Hight)
   {
        BatLev_temp= BATTERY_LEVEL_LOW;
   }
   else if (Final_Bat_AD_value < sPara.ui8BatVol_Full )
   {
        BatLev_temp= BATTERY_LEVEL_HIGH;
   }
   else
   {
        BatLev_temp= BATTERY_LEVEL_FULL;
   }
	 
    i = LastBatteryLevel ^ BatLev_temp;
    LastBatteryLevel = BatLev_temp;
//	printf("LastLevel__1=%u \n",LastBatteryLevel);
//	printf("LastLevel__2=%u \n",LastBatteryLevel);
    if(!i)
    {
        BatLevcount++;
        if (BatLevcount == 32/*10*/)
        {
            BatteryLevel=BatLev_temp;
        }
        else if(BatLevcount>32/*10*/)
        {
			BatLevcount=36;
	}
    }
    else
    {
      BatLevcount = 0;
    }
/*=========================================================================*/
}
#endif
//*********************************************************************
//*********************************************************************
//*********************************************************************
/**
**/
void PostKeyInQue(void *pKey)
{	
	struct os_q keyQue;
	UINT32 size;
	void** pQueIn;
	void** pQueStart;
	void** pQueEnd;
	void*  pQueKey;
	UINT32 sr;

	ENTER_CRITICAL(sr);
	keyQue = *(struct os_q *)mqKeyId->osEventPtr;
	size = keyQue.osQueEntries;
	pQueIn = keyQue.osQueIn;
	pQueStart = keyQue.osQueStart;
	pQueEnd = keyQue.osQueEnd;	
	if (size != 0)
	{		
		if (pQueIn == pQueStart)
		{
			pQueIn = pQueEnd-1;
		}
		else
		{
			pQueIn--;
		}
		pQueKey = *pQueIn;		
		EXIT_CRITICAL(sr);
		if (pQueKey != pKey)
		{
			osQuePost(mqKeyId, pKey);
		}
	}
	else
	{	
		EXIT_CRITICAL(sr);
		osQuePost(mqKeyId, pKey);
	}
}


/**
	PostKeyInQue(&keyButton[keyIdx]);
**/

#endif



/****************/
/****************/
/****************/

static  UINT8    ui8LowCloseCount=0 ;
//UINT8 ui8KeyHold = 0;
//UINT8 ui8KeyHoldCnt = 0;
#define BATTERY_EMPTY_MAX  		230
#define BATTERY_EMPTY_MIN  		160
#define SAMPLE_TIMES	    	10
#define BATTERY_AD_RATE	    	0
#define BATTERY_VAL_MAX     	254
#define BATTERY_VAL_MED     	245
#define BATTERY_VAL_MIN			240

#if GAOQING_A036
	/* Paul@2006/12/24 - 16:10 add start */
	#if Oem268_Dc260
	/* 268A */
	#define BATTERY_VALUE_HIGH    	58//35 
	#define BATTERY_VALUE_LOW     	45//26 
	#define BATTERY_VALUE_OUT      	30//15
	#define BATTERY_VALUE_CLOSE  	9//6  
	#else
	/* 260A */
	#define BATTERY_VALUE_HIGH    	41  //49//35 
	#define BATTERY_VALUE_LOW     	30  //37//26 
	#define BATTERY_VALUE_OUT      	17  //23//15
	#define BATTERY_VALUE_CLOSE  	5   //9//6  	
	#endif
	/* Paul@2006/12/24 - 16:11 add end */	
#else
#define BATTERY_VALUE_HIGH    	76//35 
#define BATTERY_VALUE_LOW     	62//26 
#define BATTERY_VALUE_OUT      	49//15
#define BATTERY_VALUE_CLOSE  	9//6  
#endif

#define C_BATTERY_VOLTAGE_NUM   5

UINT8   G_BatValueCnt = 0xff;
UINT8   G_LastBatteryLev = BATTERY_LEVEL_MAX;

UINT32 G_AD3Ref_SampleSum;
UINT32 G_For_FlashCharge_val;
UINT8 G_BatteryStatus = 1;

//extern bool  G_BatteryStatus = 1;
UINT8  BatteryLev = 0;
UINT8  BatteryEmpty = 0;

static UINT32 G_Battery_SampleSum;
static UINT16 G_Battery_SampleTimes;
UINT8 dcincnt =0;
extern UINT32 SleepTime;

bool   G_BatteryADC_Enable = false;
UINT32 BatteryDischargeRateGet(UINT8 *ADBuf);
void BatteryADGet(void);

void BatteryValueRead(void)
{
	UINT32 tempSampleSum;
	UINT32 tmp1,tmp2;

	hwAdcSet(ADC_ENABLE, ADC_CLK_2MHZ, 0x0b);
	
	/* Paul@2006/12/24 - 16:10 add start */
	#if (!Oem268_Dc260)
	if(uiUSBStatus != 0)
		return;
	#endif	
	
	#if C_LOWCLOSE_GPIO	
	if (gpioGet(GPIO_LOWCLOSE)>=1)
	{
		ui8LowCloseCount++ ;
		if(ui8LowCloseCount==10)
		{
			ui8LowCloseCount=0 ;
			#if 1
			LowBatDetectColse();
			#else
			osdStrDisp(20, 180, UserFont10x20, 0x10, "Low Voltage close  ") ;
			#endif
		}
	}
	#endif
	/* Paul@2006/12/24 - 16:10 add end */
	
	if(G_Battery_SampleTimes<SAMPLE_TIMES)
	{
			tempSampleSum = G_Battery_SampleSum;
			G_Battery_SampleSum = G_Battery_SampleSum + ((hwAdc2Data()<130)? (255+hwAdc2Data()):hwAdc2Data());
			if((tempSampleSum == G_Battery_SampleSum)&&(G_Battery_SampleTimes!=0))
			{
				G_Battery_SampleSum += G_Battery_SampleSum/G_Battery_SampleTimes;
			}

			G_AD3Ref_SampleSum = G_AD3Ref_SampleSum +  (hwAdc3Data()<160? (225+hwAdc3Data()):hwAdc3Data());
			
			G_Battery_SampleTimes++;
	}
	else
	{
		G_Battery_SampleSum = G_Battery_SampleSum/SAMPLE_TIMES;
		G_AD3Ref_SampleSum = G_AD3Ref_SampleSum/SAMPLE_TIMES ;//+14;

#if   0	//low voltage
	char	strttt[100] ;
	sio_psprintf(strttt, "ad2 =%04d hwAdc2Data()=%3d", G_Battery_SampleSum,hwAdc2Data());
	osdStrDisp(10, 100, UserFont10x20, 0x4f, strttt);
	
	sio_psprintf(strttt, "ad3 = %04d hwAdc3Data()=%3d", G_AD3Ref_SampleSum ,hwAdc3Data());
	osdStrDisp(10, 120, UserFont10x20, 0x4f, strttt);
	
	sio_psprintf(strttt, "ad2 - ad3 =%04d  ", G_Battery_SampleSum-G_AD3Ref_SampleSum );
	osdStrDisp(10, 140, UserFont10x20, 0x4f, strttt) ;	
#endif
	
		G_Battery_SampleTimes = 0;

		tmp1 = (255-G_AD3Ref_SampleSum)/4;
		tmp2 = (255-G_AD3Ref_SampleSum)%4;
		G_For_FlashCharge_val = G_AD3Ref_SampleSum+10+tmp2;
		if(G_BatteryStatus == 1)//true)
		{
			if(G_Battery_SampleSum > G_AD3Ref_SampleSum+BATTERY_VALUE_HIGH)/*3*(tmp1-tmp2))*//*(tmp1+tmp2)+tmp2)*/
			{
				BatteryLev = BATTERY_LEVEL_FULL;					
			}	
			else if(G_Battery_SampleSum > G_AD3Ref_SampleSum+BATTERY_VALUE_LOW)/*2*(tmp1-tmp2))	*/
			{
				BatteryLev = BATTERY_LEVEL_HIGH;				
			}	
			else if(G_Battery_SampleSum > G_AD3Ref_SampleSum+BATTERY_VALUE_OUT)/*tmp1)+tmp2)*/
			{
				BatteryLev = BATTERY_LEVEL_LOW;				
			}	
			else 
			{
				BatteryLev = BATTERY_LEVEL_OUT;					
			}	

			if (BatteryLev == G_LastBatteryLev)
			{
				G_BatValueCnt++;
				if(G_BatValueCnt >= C_BATTERY_VOLTAGE_NUM)
				{
			    	BatteryLevel = G_LastBatteryLev;
					G_BatValueCnt = C_BATTERY_VOLTAGE_NUM;
				}											
			}
			else
			{
				G_LastBatteryLev = BatteryLev;
				/* For first display battery*/
				if (G_BatValueCnt == 0xff)
				{
					BatteryLevel = G_LastBatteryLev;
				}
				G_BatValueCnt = 0;
			}
		}
		
		if((G_Battery_SampleSum < G_AD3Ref_SampleSum+BATTERY_VALUE_CLOSE)&&(G_Battery_SampleSum > BATTERY_EMPTY_MIN))
		{	
		//	SleepTime = 0;
			BatteryEmpty +=1;
		}
		else if(BatteryEmpty <=3)
		{
			BatteryEmpty = 0;
		}
		G_Battery_SampleSum = 0;
		G_AD3Ref_SampleSum = 0;
	}
	#if(BATTERY_AD_RATE == 1)
	//BatteryADGet();
	#endif
	
 #if  (C_LOWCLOSE_GPIO==0)  //  260A Battery close ***** 
	if(BatteryEmpty > 3)
	{
		BatteryEmpty ++;
		if(BatteryEmpty == 5)
		{
		//	osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT-1]);
		//	osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT-1]);
		}
		if(BatteryEmpty >= 25)
		{
			printf("battery power off\n");
		//	BatteryEmptyIconFlash();
		//	dspPowerOff(); 
		#if   1 //low voltage
			LowBatDetectColse();
		#else
			osdStrDisp(20, 180, UserFont10x20, 0x10, "Low Voltage close  ") ;
		#endif
		
		}
	}
#endif 
}

void LowBatDetectColse()
{
#if  1
		ui32NextState = UI_MODE_POWEROFF;
		tmrDisable(1);
		#if 1	//Return from Snap
		if(snapAV == OBJ_ASF)
		{
			snapAV = OBJ_NONE;
			asfAppClipAbort();  
		}
		else if(snapAV == OBJ_MP3)
		{
			snapAV = OBJ_NONE;
			mp3AppRecStop();   
		}
		else if (snapAV == OBJ_WAV)
		{
			snapAV = OBJ_NONE;
			audRiffCtrl(RIFF_STOP);
		}
		#endif

		UI_OSQFlush(uiKeyMsgQ);
		if (isMenuOn)				
		{
	//		osQuePost(mqKeyId, &keyButton[UI_KEY_FUNC_MENU]);					
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
		}				
			
		if(ui8GameFlag) 
		{
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);
		}
		else
		{
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
		}		
		
//		osTimeDly(40);		// tzw 05-10-03
//	
//		osSemPend(semApp , 0, &err);	 /*	wait until current application finish its job*/
//		osSemPost(semApp);	   /* wait until current application finish	its	job*/				
		while (uiState != UI_MODE_POWEROFF)
		{
			osTimeDly(20);
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
		}		

	 	osdClearScreen(0x07);
      	osdStrDisp(PANEL_1_OSD_W/2-10*4, PANEL_1_OSD_H/2-20, UserFont10x20,  0x17, GetString(NO_Battery));
        hwWait(0, 500);
        hwWait(0, 500);
        hwWait(0, 500);
        hwWait(0, 500);
        hwWait(0, 500);
        hwWait(0, 500);
				
        ShowGoodBye();
   //     hwWait(0, 500);
    	//gpioSet(GPIO_LCD_SHDB, 0);
    	
 		OpenLcdDisplay(0);
		gpioSet(GPIO_LCD_POWER,0);
		hwWait(0, 50);
		#if (CurrentIC8751==0)	
 	    		gpioConfig(GPIO_MUTE, 1);
	    		gpioSet(GPIO_MUTE, 1);
		#else		
			wm8751SoundEnable(0,0);
		#endif

//		gpioSet(GPIO_AUDIO_EN, 0); 
  	 	hwWait(0, 50);		

		gpioSet(GPIO_PWR_STAT, 1);   
		#if POW_EN_HIGH
			gpioSet(GPIO_PWR_EN, 0);
		#else 
			gpioSet(GPIO_PWR_EN,1);
		#endif
		while(1);
	#endif
}

