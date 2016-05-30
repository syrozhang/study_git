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
//#define		RS_232_DBG
#include 		<time.h>
#include		<stdlib.h>
#include 		<string.h>
#include 		"general.h"
#include 		"sysregs.h"
#include 		"irq.h"
#include 		"zoom.h"
#include 		"timer.h"
#include 		"stdio.h"
#include 		"snap_api.h"
#include 		"pb_api.h"
#include 		"os_api.h"
#include 		"dcf_api.h"
#include 		"exif_api.h"
#include 		"exif_custom.h"
#include 		"usb_api.h"
#include 		"osd_common.h"
#include 		"osd_api.h"
#include 		"dsc_init.h"
#include 		"smc_api.h"
#include 		"hw_cdsp.h"
#include 		"hw_gpio.h"
#include 		"sysg_api.h"
#include 		"aaa_api.h"
#include 		"sysg_api.h"
#include 		"dpof_api.h"
#include 		"utility.h"
#include 		"badpix.h"
#include 		"mp3_api.h"
#include 		"audio_api.h"
#include 		"wma_api.h"
#include 		"asf_api.h"
#include 		"avi_api.h"
#include 		"mov_api.h"
#include 		"vfs_api.h"
#include 		"pub_api.h"
#include 		"otg_api.h"
#include 		"panel.h"
#include 		"hw_disp_ext.h"
#include 		"hw_disp.h"
#include 		"hw_sdram.h"
#include 		"hw_cpu.h"
#include 		"misc_api.h"
#include 		"exif_api.h"
#include 		"hw_front.h"
#include 		"smc_api.h"
#include 	 	"uistrings.h"
#include 		"xiaoao.h"
#include 		"ui_main.h"
#include 		"gpio.h"
#include 		"card.h"
#include 		"ws9712.h"
#include 		"voice.h"
/*herb add quatity estimate @03/12/09 start*/
#include	 	"menu_capture.h"
#include 		"q_table.h"
/*herb add quatity estimate @03/12/09 end*/
/*Gu @2003.12.27 18:05 begin*/
#include 		"menu_playback.h"
/*Gu @2003.12.27 18:05 end*/
#include 		"menu_playback.h"  //caoyong

#include		 "start.h"
#include         "Icon_dc.h"
#include         "ESP10x20.C"

//**************************************************************************
#define 		FONT_HZ_J_PAGES		1000
#define 		FONT_HZ_F_PAGES		2000
#define 		FONT_HZ_J_SIZE		(FONT_HZ_J_PAGES*512)
#define 		FONT_HZ_F_SIZE		(FONT_HZ_F_PAGES*512) 

#define 		SECTOR_OF_FONT_K	0xdef
#define 		SECTOR_OF_FONT_J	SECTOR_OF_32X32_FONT
#define 		SECTOR_OF_FONT_F    (SECTOR_OF_32X32_FONT+(FONT_HZ_J_SIZE/512))

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern 		UINT8 			TVFlag;//wendy@050307 for tv detect
extern 		xdcfAttrElm_t 	fileattr;
extern 		xdcfAttrElm_t 	dirattr;
extern 		UINT32 			file_filter;
extern 		UINT8 			g_playrecStatus;
extern 		UINT8 			infoshow;
extern 		UINT32 			videoTotalTime;
extern 		UINT32 			cardExist; /*Julie*/
extern 		UINT8  			DcPlug;
extern 		UINT8 			ui8VolCnt ;
extern 		UINT8 			SpeakFlag;
extern 		UINT32			ChgPvSetting;	/* 1=restore pv setting before process any key */
extern 		UINT8 			ui8ExitSlide;

extern 		struct	mp3TagInfo mp3Info;
extern 		void 	panelSelect(UINT32);
extern 		UINT32 	getCurPanel(void);
extern 		UINT32 	aviPlayProgressGet(void);
extern 		UINT32 	movPlayProgressGet(void);
extern 		void 	dirInit();
extern 		UINT32 	isCardExist(void);
extern 		UINT32 	isCardProtect(void);

typedef struct {
	UINT16 sec;
	UINT16 min;
	UINT16 hour;
}AvPlayedTime_T;

void Playedtime();
void DateDisplay();

typedef void (*regfunc) (UINT32, void*, UINT32);
typedef void (*rmvfunc) (UINT32, UINT32);
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
struct nandUIPara_s sPara;
UINT8 		HANZIFlag; //if 1;without ziku;0 with ziku
//UINT8 		ui8Mode = 4;
UINT8 		ui8StartScreenFlag = 0 ;
//UINT8 		UserFontHZ_J[FONT_HZ_J_SIZE-100*512];
//UINT8 		UserFontHZ_F[FONT_HZ_F_SIZE-500*512];
UINT8 		UserFontHZ_J[FONT_HZ_J_SIZE];
UINT8 		UserFontHZ_F[FONT_HZ_F_SIZE];
UINT8 		uiUSBStatus = 0;
UINT8		uiDCStatus	 = 0 ;
UINT8 		uiOtgStatus = 0;
UINT8 		uiTVStatus = 0;
UINT8  		lcdOsdDisp = 1; /*Lcd and osd diap   0: close lcd ; 1: open lcd osd; 2:open lcd, close osd*/
static 		UINT8 blinkcnt = 0;
static AvPlayedTime_T avPlayedTime;
UINT32 		ui32OldState;

#if 1 //LANGUAGES
typedef void (*OSDSTRDISPPRO)(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[]);
typedef UINT8 (*OSDSTRDISPEXTPRO)(UINT32 x, UINT32 y, UINT8 * pFont, UINT32 bgColor, UINT8 str [ ], UINT8 start, UINT8 lgh, UINT8 option);
typedef UINT8  (*OSDLRCDISPPRO)(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);
UINT8  *UserFont10x20; 
//LANGUAGES
void osdStrDisp_C(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[]);
void osdStrDisp_S(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[]);
void osdStrDisp_K(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[]);
void osdStrDisp_R(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[]);

UINT8  osdStrDispExt_C(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);
UINT8  osdStrDispExt_S(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);
UINT8  osdStrDispExt_R(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);
UINT8  osdStrDispExt_K(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);

UINT8  F_osdLRCDisp_C(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);
UINT8  F_osdLRCDisp_R(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);
UINT8  F_osdLRCDisp_S(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);
UINT8  F_osdLRCDisp_K(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);
/**
**/
//LANGUAGES
UINT8* UserFont10x20_RES[10]=
{
	UserFont10x20ECT,
	UserFont10x20ECT,
	UserFont10x20ECT,
	UserFont10x20C_S,
	UserFont10x20C_ER,
	UserFont10x20C_S,
	UserFont10x20C_S,
	UserFont10x20C_S,
	UserFont10x20C_S,
	UserFont10x20ECT,
};

OSDSTRDISPPRO osdStrDisPro_S[10] =
{
	osdStrDisp_C,
	osdStrDisp_C,
	osdStrDisp_C,
	osdStrDisp_S,
	osdStrDisp_R,
	osdStrDisp_S,
	osdStrDisp_S,
	osdStrDisp_S,
	osdStrDisp_S,
	osdStrDisp_K
};
OSDSTRDISPEXTPRO osdStrDisExt_S[10] =
{
	osdStrDispExt_C,
	osdStrDispExt_C,
	osdStrDispExt_C,
	osdStrDispExt_S,
	osdStrDispExt_R,
	osdStrDispExt_S,
	osdStrDispExt_S,
	osdStrDispExt_S,
	osdStrDispExt_S,
	osdStrDispExt_K,
};
OSDLRCDISPPRO osdLRCDisPro_S[10] =
{
	F_osdLRCDisp_C,
	F_osdLRCDisp_C,
	F_osdLRCDisp_C,
	F_osdLRCDisp_S,
	F_osdLRCDisp_R,
	F_osdLRCDisp_S,
	F_osdLRCDisp_S,
	F_osdLRCDisp_S,
	F_osdLRCDisp_S,
	F_osdLRCDisp_K
};


#endif

#if (SUPPORT_HANZI == 1)
//UINT8		UserFont32x32[((16+32*32*2/8*87*94+511)/512)*512];
#endif
#if (SUPPORT_FANTI== 1)
//UINT8		UserFont32x32[((16+32*32*2/8*87*94+511)/512)*512];
#endif
UINT8 GpioCountMax[]=
{
	GE_GPIO_CT,
	FM_GPIO_CT,
	UT_GPIO_CT,
	AU_GPIO_CT,
	DR_GPIO_CT,
	TG_GPIO_CT,
	DG_GPIO_CT,
};

UINT32 GpioFuncReg[]=
{
	GE_GPIO_SL,
	FM_GPIO_SL,
	UT_GPIO_SL,
	AU_GPIO_SL,
	DR_GPIO_SL,
	TG_GPIO_SL,
	DG_GPIO_SL,
};
UINT32 GpioOutputReg[]=
{
	GE_GPIO_OV,
	FM_GPIO_OV,
	UT_GPIO_OV,
	AU_GPIO_OV,
	DR_GPIO_OV,
	TG_GPIO_OV,
	DG_GPIO_OV,
};
UINT32 GpioOutEnReg[]=
{
	GE_GPIO_OE,
	FM_GPIO_OE,
	UT_GPIO_OE,
	AU_GPIO_OE,
	DR_GPIO_OE,
	TG_GPIO_OE,
	DG_GPIO_OE,
};
UINT32 GpioInputReg[]=
{
	GE_GPIO_IV,
	FM_GPIO_IV,
	UT_GPIO_IV,
	AU_GPIO_IV,
	DR_GPIO_IV,
	TG_GPIO_IV,
	DG_GPIO_IV,
};
regfunc GpioIrqReg[]=
{
	hwGenGpioIrqReg,
	hwFmGpioIrqReg,
	NULL,
	hwAudioGpioIrqReg,
	NULL,
	hwTgGpioIrqReg,
	hwTvGpioIrqReg,
};
rmvfunc GpioIrqRmv[]=
{
	hwGenGpioIrqRmv,
	hwFmGpioIrqRmv,
	NULL,
	hwAudioGpioIrqRmv,
	NULL,
	hwTgGpioIrqRmv,
	hwTvGpioIrqRmv,
};
//***********************************************************************
//dcdbv

/**
**/
UINT32 MenuIconData[][4] =
{		
	/* Playback Image Icon */
	{Icon_rec,sizeof(Icon_rec),Icon_rec,sizeof(Icon_rec)}, 		//Recycle Bin for Delete	
	{Icon_pro,sizeof(Icon_pro),Icon_pro,sizeof(Icon_pro)},		//Pretect
	{Icon_dpr,sizeof(Icon_dpr),Icon_dpr,sizeof(Icon_dpr)},		//Un Pretect
//	{Icon_pri,sizeof(Icon_pri),Icon_pri,sizeof(Icon_pri)},	    //Print
//	{Icon_dpo,sizeof(Icon_dpo),Icon_dpo,sizeof(Icon_dpo)},      //DPOF
	{Icon_aut,sizeof(Icon_aut),Icon_aut,sizeof(Icon_aut)},      //Auto Play
	{Icon_ope,sizeof(Icon_ope),Icon_ope,sizeof(Icon_ope)},   	//Open Picture		

	/* Playback moive icon */
	{Icon_rec,sizeof(Icon_rec),Icon_rec,sizeof(Icon_rec)}, 		//Recycle Bin for Delete	
	{Icon_pro,sizeof(Icon_pro),Icon_pro,sizeof(Icon_pro)},		//Pretect
	{Icon_dpr,sizeof(Icon_dpr),Icon_dpr,sizeof(Icon_dpr)},		//Un Pretect	
	
	/* DC Icon */
	{Icon_wb1, sizeof(Icon_wb1), Icon_wb1, sizeof(Icon_wb1)},				//White Blance
	{Icon_dat, sizeof(Icon_dat), Icon_dat, sizeof(Icon_dat)},		//Date Stamp
	{Icon_siz, sizeof(Icon_siz), Icon_siz, sizeof(Icon_siz)},		//Image Size
	{Icon_qua, sizeof(Icon_qua), Icon_qua, sizeof(Icon_qua)},		//Quanlity
	{Icon_eff, sizeof(Icon_eff), Icon_eff, sizeof(Icon_eff)},		//Effect
	{Icon_tim, sizeof(Icon_tim), Icon_tim, sizeof(Icon_tim)},	//Self Shot Timer
	{Icon_cap, sizeof(Icon_cap), Icon_cap, sizeof(Icon_cap)},	//Capture Mode	
	{Icon_ev1, sizeof(Icon_ev1), Icon_ev1, sizeof(Icon_ev1)},				//EV
	
	/* DV Icon */		
	{Icon_wb1, sizeof(Icon_wb1), Icon_wb1, sizeof(Icon_wb1)},				//White Blance	
//	{Icon_siz, sizeof(Icon_siz), Icon_siz, sizeof(Icon_siz)},		//Image Size	
	{Icon_qua, sizeof(Icon_qua), Icon_qua, sizeof(Icon_qua)},		//Quanlity	
	{Icon_eff, sizeof(Icon_eff), Icon_eff, sizeof(Icon_eff)},		//Effect	
	{Icon_tim, sizeof(Icon_tim), Icon_tim, sizeof(Icon_tim)},	//Self Shot Timer	
	{Icon_ev1, sizeof(Icon_ev1), Icon_ev1, sizeof(Icon_ev1)},				//EV	
};




// setup

UINT32 SetICON[][6] = {

		{Icon_opi, sizeof(Icon_opi), Icon_opi,sizeof(Icon_opi),0,0},
		{Icon_tim, sizeof(Icon_tim), Icon_tim,sizeof(Icon_tim),0,0},
		{Icon_ton, sizeof(Icon_ton), Icon_ton,sizeof(Icon_ton),0,0},
		{Icon_lan, sizeof(Icon_lan), Icon_lan,sizeof(Icon_lan),0,0},
		{Icon_sto, sizeof(Icon_sto), Icon_sto,sizeof(Icon_sto),0,0},		
		{Icon_ray, sizeof(Icon_ray), Icon_ray,sizeof(Icon_ray),0,0},		
		{Icon_tvo, sizeof(Icon_tvo), Icon_tvo,sizeof(Icon_tvo),0,0},
		{Icon_def, sizeof(Icon_def), Icon_def,sizeof(Icon_def),0,0},		
		{Icon_off, sizeof(Icon_off), Icon_off,sizeof(Icon_off),0,0},
		{Icon_for, sizeof(Icon_for), Icon_for,sizeof(Icon_for),0,0},
		{Icon_ver, sizeof(Icon_ver), Icon_ver,sizeof(Icon_ver),0,0},		
//		{MODE1, sizeof(MODE1), MODE1,sizeof(MODE1),0,0},		
		{Icon_opi, sizeof(Icon_opi), Icon_opi,sizeof(Icon_opi),0,0}, // 11
		{menuback, sizeof(menuback), menuback,sizeof(menuback),0,0}, // 12

};
// 显示菜单背景
void pbIconMenuBG()
{
	
		pbImagePaste(SetICON[12][2], SetICON[12][3], SetICON[12][4],SetICON[12][5]);

}

UINT8 *MENU[LANGUAGES] =
{
"MENU",
"菜单",	
"垫虫",	
"Menú",
"Меню",	
"Menü",
"menu",
"Menu",
"Menu",
"皋春",
};
	
void pbDisplayMenuWord() 
{
	osdBarDraw(0, 0, 120, 24, 0x00);
	 osdStrDisp(34,4 , UserFont10x20, 0xd0, MENU[sPara.language]) ;

}

//end
UINT32	Mp3ItemImage[6][2] = 
{
	{menuback,sizeof(menuback)},
	{Icon_rec,sizeof(Icon_rec)},
	{Icon_pro,sizeof(Icon_pro)},
	{Icon_dpr,sizeof(Icon_dpr)},
	{Icon_mod,sizeof(Icon_mod)},	
	{Icon_clo,sizeof(Icon_clo)},
};

#define		EBOOK_ITEM_MAX 		 3
UINT32	EBookItemImage[EBOOK_ITEM_MAX][2] = 
{
//	{icon_mis,sizeof(icon_mis)},
//	{icon_rec,sizeof(icon_rec)},
	{Icon_rec,sizeof(Icon_rec)},
	{Icon_pro,sizeof(Icon_pro)},
	{Icon_dpr,sizeof(Icon_dpr)},
};

//***********************************************************************


UINT8	mediaRootNameStr[6] = 
{
	'V','I','D','E','O','\0',
};

UINT8	DVDCRootNameStr[5] = 
{
	'D','C','I','M','\0','\0',
};


UINT8 	photoRootNameStr[6] = 
{

	'P','H','O','T','O','\0'
};

UINT8	otherRootNameStr[6] = 
{
//	'O','T','H','E','R','\0'
	'D','A','T','A','S','\0'
};


/**************************************************************************
 *                       DCIM    S T R I N G                         *
 **************************************************************************/
UINT8	DCIMRootNameStr[6] = 
 {
	'D','C','I','M','\0','\0',
};

UINT8  DCDirNameStr[6] = 
{
	'M','E','D','I','A','\0',
};

UINT8	DCNameStr[6] = 
{
	'D','C','O','O','\0','\0',
};


UINT8	DVNameStr[6] = 
{
	'D','V','O','O','\0','\0',
};
/**************************************************************************
 *                       V E D I O    S T R I N G                         *
 **************************************************************************/
UINT8 videoRootNameStr[6]=
{
	'M', 'O', 'V', 'I', 'E','\0',
};
UINT8 videoSubDirDVStr[6]=
{
	'D', 'V', '\0', '\0', '\0', '\0',
};
UINT8 videoSubDirMP4Str[6]=
{
	'M', 'P', '4', '\0', '\0', '\0',
};
/**************************************************************************
 *                         M P 3      S T R I N G                         *
 **************************************************************************/
UINT8 mp3RootNameStr[6]=
{
	'M', 'U', 'S', 'I', 'C', '\0',
};
UINT8 mp3SubDirRecStr[6]=
{
	'R', 'E', 'C', '\0', '\0', '\0',
};

#if DIR_MP3_TO_MUSIC
UINT8 mp3SubDirMP3Str[6]=
{
	'M', 'U', 'S', 'I', 'C', '\0' 
};
#else
UINT8 mp3SubDirMP3Str[4]=
{
	'M', 'P', '3', '\0'
};
#endif

UINT8 mp3SubDirRecfileName[6]=
{
	'R', 'E', 'C', 'O', '\0', '\0',
};



/**************************************************************************
 *                       I M A G E    S T R I N G                         *
 **************************************************************************/
UINT8 imageRootNameStr[6]=
{
'D','C','I','M','\0','\0',
};
UINT8 imageSubImageStr[6]=
{
	'I', 'M', 'A', 'G', 'E', '\0',
};
UINT8 imageSubDCStr[6]=
{
	'D', 'C', '\0', '\0', '\0', '\0',
};
/**************************************************************************
 *                         RECORD      S T R I N G                         *
 **************************************************************************/
UINT8 UserRootNameStr[6]=
{
	'U', 'S', 'E', 'R', '\0', '\0',
};
UINT8 UserSubBooksStr[6]=
{
	'B', 'O', 'O', 'K', 'S', '\0',
};
UINT8 UserSubGamesStr[6]=
{
	'G', 'A', 'M', 'E', 'S', '\0',
};
#if 1
UINT8 videoDirNameStr[6]=
{
	'M', 'E', 'D', 'I', 'A', '\0',
};

UINT8 videoFileNameStr[5]=
{
	'P', 'L', 'U', 'S', '\0',
};

UINT8 mp3DirNameStr[6]=
{
	'I', 'M', 'A', 'G', 'E', '\0',
};

UINT8 mp3FileNameStr[5]=
{
	'P', 'L', 'U', 'S', '\0',
};

UINT8 imageDirNameStr[6]=
{
	'M', 'E', 'D', 'I', 'A', '\0',
};

UINT8 imageFileNameStr[5]=
{
	'P', 'L', 'U', 'S', '\0',
};

UINT8 recDirNameStr[6]=
{
	'I', 'M', 'A', 'G', 'E', '\0',
};

UINT8 recFileNameStr[5]=
{
	'P', 'L', 'U', 'S', '\0',
};
#endif






/**************************************************************************
 *                        F O N T    H E A D E R                          *
 **************************************************************************/
#if 0 
struct tagOSDFont
{
	// Total 16 Bytes
	UINT16	Count;
	UINT8	Width;			//+2
	UINT8	High;			//+3
	UINT8	Sign[2];		//+4	//"SP"
	UINT16	Ver;			//+6
	UINT8	CompressMode;	//+8	//00=NO
	UINT8	ColorDepth;		//+9
	UINT8	Reserved0A[2];	//+11
	UINT32	Size;			//+12
};
#endif
/**************************************************************************
 *  Function Name: smcCmdReliableTest                                     *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  1.driveId = DRIVE_NAND/DRIVE_SMC
    2.opt =  0 : Test all blocks including the ones marked as bad
             1 : Test only the good blocks that are not marked                                                                    *
 *  Returns:    SUCCESS: the number of good blocks are enough
                FAIL: the number of good blocks are not enough                                                            *
 *  See also:                                                             *
 **************************************************************************/
extern UINT32 smcCmdReliableTest(UINT32 driveId, UINT32 opt);
/**************************************************************************
 *  Function Name: gpioFuncSet                                            *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
 **************************************************************************/
void gpioFuncSet(UINT16 gpio, UINT8 val)
{
	UINT32 addr;
	UINT32 status;

	if (GpioFuncReg[gpio>>8] == 0)
		return;
	if ((gpio&0xff) >= GpioCountMax[gpio>>8])
		return;
	if ((gpio>>8) >= GPIO_TYPE_TOL)
		return;

	addr = 0xb0000000 + GpioFuncReg[gpio>>8] + ((gpio&0xff)>>3);

	ENTER_CRITICAL(status);
	if (val > 0)
		WRITE8(addr, READ8(addr) | (0x01<<(gpio&0x07)));
	else
		WRITE8(addr, READ8(addr) & (~(0x01<<(gpio&0x07))));
	EXIT_CRITICAL(status);
}
/**************************************************************************
 *  Function Name: gpioConfig                                             *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
void gpioConfig(UINT16 gpio, UINT8 val)
{
	UINT32 addr;
	UINT32 status;

	if ((gpio&0xff) >= GpioCountMax[gpio>>8])
		return;
	if ((gpio>>8) >= GPIO_TYPE_TOL)
		return;
	addr = 0xb0000000 + GpioOutEnReg[gpio>>8] + ((gpio&0xff)>>3);

	ENTER_CRITICAL(status);
	if (val > 0)
		WRITE8(addr, READ8(addr) | (0x01<<(gpio&0x07)));
	else
		WRITE8(addr, READ8(addr) & (~(0x01<<(gpio&0x07))));
	EXIT_CRITICAL(status);
}
/**************************************************************************
 *  Function Name: gpioSet                                                *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
void gpioSet(UINT16 gpio, UINT8 val)
{
	UINT32 addr;
	UINT32 status;

	if ((gpio&0xff) >= GpioCountMax[gpio>>8])
		return;
	if ((gpio>>8) >= GPIO_TYPE_TOL)
		return;

	addr = 0xb0000000 + GpioOutputReg[gpio>>8] + ((gpio&0xff)>>3);

	ENTER_CRITICAL(status);
	if (val > 0)
		WRITE8(addr, READ8(addr) | (0x01<<(gpio&0x07)));
	else
		WRITE8(addr, READ8(addr) & (~(0x01<<(gpio&0x07))));
	EXIT_CRITICAL(status);
}
/**************************************************************************
 *  Function Name: gpioSetSts                                                *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT8 gpioSetSts(UINT16 gpio)
{
	UINT32 addr;

	if ((gpio&0xff) >= GpioCountMax[gpio>>8])
		return 0;
	if ((gpio>>8) >= GPIO_TYPE_TOL)
		return 0;

	addr = 0xb0000000 + GpioOutputReg[gpio>>8] + ((gpio&0xff)>>3);

	return (READ8(addr) & (0x01<<(gpio&0x07)));
}
/**************************************************************************
 *  Function Name: gpioGet                                                *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT8 gpioGet(UINT16 gpio)
{
	UINT32 addr;

	if ((gpio&0xff) >= GpioCountMax[gpio>>8])
		return 0;
	if ((gpio>>8) >= GPIO_TYPE_TOL)
		return 0;

	addr = 0xb0000000 + GpioInputReg[gpio>>8] + ((gpio&0xff)>>3);

	return (READ8(addr) & (0x01<<(gpio&0x07)));
}
//************************************************************************** 
UINT8 gpioGetByte(UINT16 gpio)
{
	UINT32 addr;

	if ((gpio&0xff) >= GpioCountMax[gpio>>8])
		return 0;
	if ((gpio>>8) >= GPIO_TYPE_TOL)
		return 0;

	addr = 0xb0000000 + GpioInputReg[gpio>>8] + ((gpio&0xff)>>3);

	return (READ8(addr));
}
//************************************************************************** 
UINT8 gpioGetKeyByte(UINT16 gpio)
{
	UINT32 addr;
	UINT8  ui8Return; 

	if ((gpio&0xff) >= GpioCountMax[gpio>>8])
		return 0;
	if ((gpio>>8) >= GPIO_TYPE_TOL)
		return 0;

	addr = 0xb0000000 + GpioInputReg[gpio>>8] + ((gpio&0xff)>>3);
	ui8Return=(READ8(addr))>>3;
	return (ui8Return);
}
/**************************************************************************
 *  Function Name: gpioIrqReg                                             *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
void gpioIrqReg(UINT32 gpio, void *pfunc, UINT32 edge)
{
	if ((gpio&0xff) >= GpioCountMax[gpio>>8])
		return;
	if ((gpio>>8) >= GPIO_TYPE_TOL)
		return;

	if (GpioIrqReg[gpio>>8] == NULL)
		return;
	GpioIrqReg[gpio>>8](gpio&0xff, pfunc, edge);

	return;
}
/**************************************************************************
 *  Function Name: gpioIrqRmv                                             *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
void gpioIrqRmv(UINT32 gpio, UINT32 edge)
{
	if ((gpio&0xff) >= GpioCountMax[gpio>>8])
		return;
	if ((gpio>>8) >= GPIO_TYPE_TOL)
		return;

	if (GpioIrqRmv[gpio>>8] == NULL)
		return;
	GpioIrqRmv[gpio>>8](gpio&0xff, edge);

	return;
}
/**************************************************************************
 *  Function Name: Buzzer                                                 *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
void Buzzer(UINT8 freq)
{
#if 0	
	UINT16 i;
	UINT32 start, end;

	freq = 5;
	end = tmrUsTimeGet();
	start = end;
	gpioConfig(GPIO_BUZZER, 1);
	for (i = 0; i < 320; i++)
	{
		gpioSet(GPIO_BUZZER, 1);

		while((end-start) < 1000/2/freq)
		{
			end = tmrUsTimeGet();
		}
		start = end;

		gpioSet(GPIO_BUZZER, 0);

		while((end-start) < 1000/2/freq)
		{
			end = tmrUsTimeGet();
		}
		start = end;
	}
#endif	
}

/**************************************************************************
 *  Function Name: uiUsbPlugIn                                            *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
extern UINT32	rec_mode;
extern UINT8	ui8GameFlag;

//extern UINT8 	EBookPlayMusicFlag; 
UINT32 uiUsbPlugIn(void)
{
	gpioConfig(GPIO_USB_OE, 1);
	gpioSet(GPIO_USB_OE, 0);
	
	ui32NextState = UI_MODE_USB;
	uiUSBStatus = 1;
		
	tmrDisable(1);
	UI_OSQFlush(uiKeyMsgQ);
	if(ui8GameFlag) 
	{
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);
	}
	else
	{
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);
	}	
	#if 0
	if (EBookPlayMusicFlag)
		{
			mp3AppAbort();
			osTimeDly(40);
			//osTaskSuspend(osTaskFindByName("EBMUSIC"));				
		}
	#endif

//	osTimeDly(20);										
//	gpioSet(GPIO_VIDEO_EN, 0);	
//	hwWait(0, 50);
// 	if (snapAV)
//	{
//		snapAV = OBJ_NONE;
//	}
#if 0 	//xouer080907		
	osQuePost(uiKeyMsgQ, &keyRotary[uiState>>24]);		
	while (uiState != UI_MODE_USB)
	{
		osTimeDly(20);
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);			
	}
#else
	uiState = UI_MODE_USB;
#endif
	printf("		uiUsbPlugIn \n");
	
	return SUCCESS;
}
/**************************************************************************
 *  Function Name: uiUsbPlugOut                                           *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT32 uiUsbPlugOut(void)
{	
//	gpioConfig(GPIO_DET_UDISK,1);  
//	gpioSet(GPIO_DET_UDISK,GPIODETUDISK);	
	
	if (uiUSBStatus != 0)
	{
		gpioConfig(GPIO_USB_OE, 1);
		gpioSet(GPIO_USB_OE, 0);
		uiUSBStatus = 0;

		#if 1
		vfsUnmount(DRIVE_NAND);
		nandDetect();
		/*
		vfsUnmount(DRIVE_SD);
		sdDetect();
		*/
	//	if (isCardExist()  && (sPara.StorageModeFlag == Card) ) 
		if (isCardExist())   // tzw 05-10-03 
		{
			/*OS_EVENT* semCard  = osEventFindByName("CARD");
			osSemPost(semCard);*/
			/*vfsUnmount(DRIVE_SD);*/
			sdRemove();
			sdDetect();
		}
		/*
		vfsInit();
		xdcfInit(imageDirNameStr, imageFileNameStr, 0);
		*/

		dirIdx = 0;
		fileIdx = 0;
		/*osQuePost(uiKeyMsgQ, &keyRotary[(uiState & UI_MODE_MASK) >> 24]);*/
		//	if (sPara.StorageModeFlag !=Udisk)//Davis:patch_2005/Apr/25
		//shuai	osdClearScreen(0);

		if (sPara.USBModeFlag == USB_MODE_MSDC)
		{
			usbChangeMode(USB_MODE_PCCAM);
		}
		sPara.USBModeFlag  = USB_MODE_MSDC;//wendy  for msdc 

		#if  1
		PowerOffSystem() ;
		#endif
		menuReturn(UI_MAINMENU, 0);	
		osTimeDly(20);
		#endif

		#if 0
		hwWait(0, 200);	/*500ms*/
		xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);
		dirInit();
		#endif
	}
	printf("uiUsbPlugOut()\n");

	return SUCCESS;
}
/***/
#define MODULE_AUDIO 1
void PowerOffSystem()
{
 		 /* usb copy data flash LED fqdao  06.5.19*/
	 	usbFuncReg_MsdcBeforeRead(uiUSBLedOn);
	   	usbFuncReg_MsdcAfterRead(uiUSBLedOff);
	   	usbFuncReg_MsdcBeforeWrite(uiUSBLedOn);
	  	usbFuncReg_MsdcAfterWrite(uiUSBLedOff);
		/* usb copy data flash LED  fqdao  06.5.19*/
	
		hwPowerDisable(MODULE_AUDIO);	/* Paul@2006/04/15 add */
		
		OpenLcdDisplay(0);
		gpioSet(GPIO_LCD_POWER,0);
		hwWait(0, 50);
		
#if (CurrentIC8751==0)	
	    gpioConfig(GPIO_MUTE, 1);
	    gpioSet(GPIO_MUTE, 1);
#else		
		wm8751SoundEnable(0,0);
#endif		
		hwWait(0, 50);
		
#if POW_EN_HIGH
		gpioSet(GPIO_PWR_EN, 0);
#else 
		gpioSet(GPIO_PWR_EN, 1);
#endif
		hwWait(0, 50);
		printf("Power ok !!\n");
	
}

/**************************************************************************
 *  Function Name: uiUSBMsdcWp                                           *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT32 uiUSBMsdcWp(UINT32 devID)
{
	/*andrew_gu@2005/May/16, 21:45 (*/
	if (devID != DRIVE_NAND)
	{
		if(isCardProtect())
			return TRUE;
	}
	/*andrew_gu@2005/May/16, 21:45 )*/
	#if 0
	if (devID == DRIVE_SD)
	{
		/* check the storage write protect H/W */
		gpioConfig(GPIO_USB_DET, 0);		/* Config I/O port is input mode */
		if (gpioGet(GPIO_USB_DET))	/* Read I/O port status */
		{
			return TRUE;				/* WP */
		}
		else
		{
			return FALSE;			/* NOT WP */
		}
	}
	else if (devID == DRIVE_SMC)
	{
		/* check the storage write protect H/W */
		gpioConfig(GPIO_USB_DET, 0);		/* Config I/O port is input mode */
		if (gpioGet(GPIO_USB_DET) == 0)	/* Read I/O port status */
		{
			return TRUE;				/* WP */
		}
		else
		{
			return FALSE;			/* NOT WP */
		}
	}
	#endif
	return FALSE;
}
/**************************************************************************
 *  Function Name: uiUSBLedOn                                             *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT32 uiUSBLedOn(void)
{
	/*gpioFuncSet(GPIO_USB_LED, 1);*/
	gpioConfig(GPIO_USB_LED, 1);
	gpioSet(GPIO_USB_LED, 1);
	return SUCCESS;
}

/**************************************************************************
 *  Function Name: uiUSBLedOff                                            *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT32 uiUSBLedOff(void)
{
	/*gpioFuncSet(GPIO_USB_LED, 1);*/
	gpioConfig(GPIO_USB_LED, 1);
	gpioSet(GPIO_USB_LED, 0);

	return SUCCESS;
}
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
/**************************************************************************
 *  Function Name:                                                        *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns: None                                                         *
 *  See also:                                                             *
**************************************************************************/
UINT32	host_DPCtrl(UINT32 pull)
{
      UINT32 ret;

      if (pull == OTG_D_PULL_DOWN) 
	  {
           /*Set gpio 15 to output*/
           hwUsbGpioCfgSet(0x8000, 0x8000);
           /*Write gpio 15 to 1*/
           hwUsbGpioMaskWrite(0x8000, 0x8000);
      }
      else 
	  {
           /*Set gpio 15 to output*/
           hwUsbGpioCfgSet(0x8000, 0x8000);
           /*Write gpio 15 to 0*/
           hwUsbGpioMaskWrite(0x8000, 0x0000);
      }
      ret = SUCCESS;
      return ret;
}
/**************************************************************************
 *  Function Name:                                                        *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns: None                                                         *
 *  See also:                                                             *
**************************************************************************/
UINT32	host_DMCtrl(UINT32 pull)
{
      UINT32 ret;

      if (pull == OTG_D_PULL_DOWN)
	  {
          /*Set gpio 14 to output*/
          hwUsbGpioCfgSet(0x4000, 0x4000);
          /*Write gpio 14 to 1*/
          hwUsbGpioMaskWrite(0x4000, 0x4000);
      }
      else
	  {
          /*Set gpio 14 to output*/
          hwUsbGpioCfgSet(0x4000, 0x4000);
          /*Write gpio 14 to 0*/
          hwUsbGpioMaskWrite(0x4000, 0x0000);
      }
      ret = SUCCESS;
      return ret;
}
/**************************************************************************
 *  Function Name:                                                        *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns: None                                                         *
 *  See also:                                                             *
**************************************************************************/
UINT32	host_PowerCtrl(UINT32 mA)
{
      UINT32 ret;

      if (mA > 0) 
	  {
         /*Set gpio 7 to output*/
         hwUsbGpioCfgSet(0x0080, 0x0080);
         /*Write gpio 7 to 1*/
         hwUsbGpioMaskWrite(0x0080, 0x0080);
      }
      else 
	  {
         /*Set gpio 7 to output*/
         hwUsbGpioCfgSet(0x0080, 0x0080);
         /*Write gpio 7 to 0*/
         hwUsbGpioMaskWrite(0x0080, 0x0000);
      }
      ret = SUCCESS;
      return ret;
}
/**************************************************************************
 *  Function Name: otgPowerCtrl                                           *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT32 otgPowerCtrl(UINT32 on)
{
#if 0
	/*gpioFuncSet(GPIO_OTG_POWER, 1);*/
	gpioConfig(GPIO_OTG_POWER, 1);
	if (on)
		gpioSet(GPIO_OTG_POWER, 1);
	else
		gpioSet(GPIO_OTG_POWER, 0);
	printf("OTG Power on=%d\n",on);
	return SUCCESS;
#endif	
}
/**************************************************************************
 *  Function Name: otgDPCtrl                                              *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT32 otgDPCtrl(UINT32 sts)
{
	/*gpioFuncSet(GPIO_OTG_DP, 1);*/

	if (sts == OTG_D_PULL_OFF)
		gpioConfig(GPIO_OTG_DP, 0);
	else
		gpioConfig(GPIO_OTG_DP, 1);

	if (sts == OTG_D_PULL_UP)
		gpioSet(GPIO_OTG_DP, 0);
	else if(sts == OTG_D_PULL_DOWN)
		gpioSet(GPIO_OTG_DP, 1);
	printf("OTG DP sts=%d\n",sts);
	return SUCCESS;
}
/**************************************************************************
 *  Function Name: otgDMCtrl                                              *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT32 otgDMCtrl(UINT32 sts)
{
	/*gpioFuncSet(GPIO_OTG_DM, 1);*/

	if (sts == OTG_D_PULL_OFF)
		gpioConfig(GPIO_OTG_DM, 0);
	else
		gpioConfig(GPIO_OTG_DM, 1);

	if (sts == OTG_D_PULL_UP)
		gpioSet(GPIO_OTG_DM, 0);
	else if(sts == OTG_D_PULL_DOWN)
		gpioSet(GPIO_OTG_DM, 1);
	printf("OTG DM sts=%d\n",sts);
	return SUCCESS;
}
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
//************************************************************************** 
UINT32 StorageSel(UINT32 sts)
{
    UINT32  err;
	err = xdcfActiveDevIdSet(sts);
	if (err != SUCCESS)
		return err;
	#if 1
		dirInit();
	#endif
	xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);
	dirIdx = 0;
	fileIdx = 0;
	/*osQuePost(uiKeyMsgQ, &keyRotary[(uiState & UI_MODE_MASK) >> 24]);*/
	printf("StorageSel\n");
	return SUCCESS;
}
/**************************************************************************
 *  Function Name: otgMount                                              *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT32 otgMount(UINT32 sts)
{
	UINT32 err;

	printf("OTG enter Mount\n");
	err = xdcfActiveDevIdSet(DRIVE_UDISK);
	if(err)
	{
		osTimeDly(30);
		err = xdcfActiveDevIdSet(DRIVE_UDISK);
		if(err == FAIL)
		 {
		 	xdcfActiveDevIdSet(DRIVE_NAND);
		 }
	}
	#if 1
	xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);//wendy 050121 add for  folder

	dirInit();
//	xdcfCurRootDirSet(PhotoRootNameStr);
	//wendy@050203 for five folder begin
//	xdcfChange2Root();
 //    					vfsMkdir(PhotoRootNameStr);
					//wendy@050203 for five folder begin	 
	//wendy@050128 add for four folder end
	#endif
	xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);
	dirIdx = 0;
	fileIdx = 0;

	uiUSBStatus = 1;
	sPara.OTGModeFlag = OTGUDISK;
	//sPara.StorageModeFlag = Udisk;//Davis:patch_2005/Apr/25
	/*osQuePost(uiKeyMsgQ, &keyRotary[(uiState & UI_MODE_MASK) >> 24]);*/

	printf("OTG Mount\n");
	return SUCCESS;
}
/**************************************************************************
 *  Function Name: otgUnmount                                              *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
UINT32 otgUnmount(UINT32 sts)
{
	UINT32 err ;
 	printf("OTG enter Unmount\n");

	err =xdcfActiveDevIdSet(DRIVE_NAND);
	if (err == FAIL)
		printf("otgUnmount-err");

	#if 1
	xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);//wendy 050121 add for  folder

	dirInit();
//	xdcfCurRootDirSet(PhotoRootNameStr);
	//wendy@050203 for five folder begin
//	xdcfChange2Root();
//	vfsMkdir(PhotoRootNameStr);
	//wendy@050203 for five folder begin	 

	//wendy@050128 add for four folder end
	#endif
	xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);
	dirIdx = 0;
	fileIdx = 0;

	/*osQuePost(uiKeyMsgQ, &keyRotary[(uiState & UI_MODE_MASK) >> 24]);*/

	uiUSBStatus = 0;

	/*menuReturn(UI_MAINMENU, 0);*/
	sPara.OTGModeFlag = OTGPC;
	sPara.StorageModeFlag = NandFlash;

	printf("OTG Unmount\n");
	return SUCCESS;
}
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
/* sun@1029 am 11:44 add */
	extern UINT8 G_OB_previewRval;
	extern UINT8 G_OB_previewGRval;
	extern UINT8 G_OB_previewGBval;
	extern UINT8 G_OB_previewBval;
	extern UINT8 G_OB_snapRval;
	extern UINT8 G_OB_snapGRval;
	extern UINT8 G_OB_snapGBval;
	extern UINT8 G_OB_snapBval;
/* sun@1029 am 11:44 end */
/**************************************************************************
 *                                                                        *
 *  Function Name: osdIconDisp                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 nandWriteParameters(void)
{
#if 1
	UINT8 *allocBuf;
	UINT32 i;
	UINT8 * pbData;
	/* sun@1029 am 11:31 add */
	sPara.pOB_R = G_OB_previewRval;
	sPara.pOB_GR = G_OB_previewGRval;
	sPara.pOB_GB = G_OB_previewGBval;
	sPara.pOB_B = G_OB_previewBval;
	sPara.sOB_R = G_OB_snapRval;
	sPara.sOB_GR = G_OB_snapGRval;
	sPara.sOB_GB = G_OB_snapGBval;
	sPara.sOB_B = G_OB_snapBval;
	sPara.OB_ChkSum = G_OB_previewRval+G_OB_previewGRval+G_OB_previewGBval+G_OB_previewBval+
					  G_OB_snapRval+G_OB_snapGRval+G_OB_snapGBval+G_OB_snapBval+1;
	/* sun@1029 am 11:31 end */
	sPara.mp3vol=mp3Vol ;//fqdao_add 06.6.17
	allocBuf = osMemAlloc(512);
	if (allocBuf == NULL)
	{
		printf("allocBuf fail !");
		return FAIL;
	}
	allocBuf = (UINT8 *)((0xAC000000 | (UINT32)allocBuf));
	pbData = (UINT8 *)(0xAC000000 | (UINT32)&sPara);

	memcpy(sPara.asciiName, "Sunplus", 8);
	sPara.checkSum = 0;
	for (i=10; i<sizeof(struct nandUIPara_s); i++)
		sPara.checkSum += pbData[i];

	for (i=0; i<sizeof(struct nandUIPara_s); i++)
		allocBuf[i] = pbData[i];
	nandRsvBlkWrite(SECTOR_OF_UI_PARAM, 1, allocBuf);

	osMemFree(allocBuf);
	return SUCCESS;
#endif
}
/**************************************************************************
 *                                                                        *
 *  Function Name: osdIconDisp                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 nandReadParameters(void)
{
#if 1
	UINT8 *allocBuf;
	UINT32 i;
	UINT16 checkSum=0;
	UINT8 * pbData;

	/* Paul@2006/12/25 - 17:24 add start */
	/* Initial Font Ponter */
	UserFont10x20 = UserFont10x20_RES[DEFAULT_LANGUAGE];
	/* Paul@2006/12/25 - 17:24 add end */

	allocBuf = osMemAlloc(512);
	if (allocBuf == NULL)
	{
		printf("allocBuf fail !");
		return FAIL;
	}
	allocBuf = (UINT8 *)((0xAC000000 | (UINT32)allocBuf));
	nandRsvBlkRead(SECTOR_OF_UI_PARAM, 1, allocBuf);

	pbData = (UINT8 *)(0xAC000000 | (UINT32)&sPara);
	for (i=0; i<sizeof(struct nandUIPara_s); i++)
		pbData[i] = allocBuf[i];

	if(memcmp(sPara.asciiName, "Sunplus", 8))
		return FAIL;
	for (i=10; i<sizeof(struct nandUIPara_s); i++)
		checkSum += allocBuf[i];
	if(sPara.checkSum != checkSum)
		return FAIL;
//LANGUAGES
	sPara.snapFlag = (sPara.snapFlag > SnapSelf) ? SnapSingle : sPara.snapFlag;
	sPara.language = (F_CheckLanguage(sPara.language)==0) ? sPara.language:LanguageEnglish;
	UserFont10x20 =UserFont10x20_RES[sPara.language];

	sPara.uiPatternFlag = (sPara.uiPatternFlag<3) ? sPara.uiPatternFlag:PatternDefault;
	sPara.uiBeepFlag = (sPara.uiBeepFlag<3) ? sPara.uiBeepFlag : BeepOn;
	sPara.stampFlag = (sPara.stampFlag > StampOff) ? StampDayTime : sPara.stampFlag;
	sPara.USBModeFlag = USB_MODE_MSDC;// USBMSDC;/*(sPara.USBModeFlag<3) ? sPara.USBModeFlag : USBMSDC;*/
//	sPara.StorageModeFlag =NandFlash; /*(sPara.StorageModeFlag<4) ? sPara.StorageModeFlag : NandFlash; Julie*/

	 /* For bug 70 John @03/17/2006 */
	 sPara.StorageModeFlag = cardExist? Card:NandFlash; 

	sPara.OTGModeFlag = OTGPC;

	/*Ly 04/09/21 add start*/
	//sPara.TvoutFlag = (sPara.TvoutFlag<3) ? sPara.TvoutFlag  : TVOutNTSC;  /*Julie*/ wendy@050228 close
	/*Ly 04/09/21 add start*/
	sPara.SleepTimeFlag = (sPara.SleepTimeFlag<4) ? sPara.SleepTimeFlag : SleepTime3m;
	sPara.VideoModeFlag = (sPara.VideoModeFlag<2) ? sPara.VideoModeFlag : TVNTSC;

	sPara.ImageQuality = (sPara.ImageQuality<3) ? sPara.ImageQuality:QualitySuper;
	sPara.ISOFlag = (sPara.ISOFlag<4) ? sPara.ISOFlag:ISOAUTO;
	sPara.EffectFlag = (sPara.EffectFlag<4) ? sPara.EffectFlag:NormalEffect;
	sPara.ASFMICFlag = (sPara.ASFMICFlag<2) ? sPara.ASFMICFlag:MICON;
	/* Paul@2005/09/21 */
	sPara.ui8FlashMode = (sPara.ui8FlashMode > FlashForceOff) ? FlashAuto : sPara.ui8FlashMode;
	sPara.ui8ImageSize = (sPara.ui8ImageSize > ImageSize640) ? ImageSize1280 : sPara.ui8ImageSize;
	sPara.VideoClipSize = (sPara.VideoClipSize > VideoSize320) ? VideoSize320 : sPara.VideoClipSize;
	sPara.WBFlag = (sPara.WBFlag > WBFluorescent) ? WBAuto : sPara.WBFlag;
	sPara.frequenceFlag = (sPara.frequenceFlag > LIGHT_60HZ) ? LIGHT_50HZ : sPara.frequenceFlag;

	sPara.Interval = (sPara.Interval<5) ? sPara.Interval : SlideInterval3sec;
	sPara.Repeat = (sPara.Repeat<2) ? sPara.Repeat : SlideRepeatOn;
	sPara.FolderAll = (sPara.FolderAll<2) ? sPara.FolderAll : SlideImageFolder;
	sPara.mp3vol = (sPara.mp3vol<11)? sPara.mp3vol : 7;	//Paul@2005/09/21
    	mp3Vol= sPara.mp3vol;		

	sPara.hour = (sPara.hour<24) ? sPara.hour : 0;
	sPara.min = (sPara.min<60) ? sPara.min : 0;

	sPara.ui8AutoCloseLCDTime = (sPara.ui8AutoCloseLCDTime < ui8AutoCloseLCDTime60s )? sPara.ui8AutoCloseLCDTime : ui8AutoCloseLCDTimeNever;
	sPara.TvoutFlag = TVOutOff;                  /*Julie*/// wendy@050228 add

	sPara.EQFlag = (sPara.EQFlag < EQHall) ? sPara.EQFlag : EQFlat;/*andrew_gu@2005/Apr/04, 11:34 */
	/* sun@1029 am 11:33 add */
	G_OB_previewRval = (sPara.pOB_R > 0x80)?0:sPara.pOB_R;
	G_OB_previewGRval = (sPara.pOB_GR > 0x80)?0:sPara.pOB_GR;
	G_OB_previewGBval = (sPara.pOB_GB > 0x80)?0:sPara.pOB_GB;
	G_OB_previewBval = (sPara.pOB_B > 0x80)?0:sPara.pOB_B;
	G_OB_snapRval = (sPara.pOB_R > 0x80)?0:sPara.sOB_R;
	G_OB_snapGRval = (sPara.pOB_GR > 0x80)?0:sPara.sOB_GR;
	G_OB_snapGBval = (sPara.pOB_GB > 0x80)?0:sPara.sOB_GB;
	G_OB_snapBval = (sPara.pOB_B > 0x80)?0:sPara.sOB_B;
	if(sPara.OB_ChkSum ==( G_OB_previewRval+G_OB_previewGRval+G_OB_previewGBval+G_OB_previewBval+
					  G_OB_snapRval+G_OB_snapGRval+G_OB_snapGBval+G_OB_snapBval+1))
	{
		printf("<read OB para success>\n");
		printf("pR=%d pGR=%d pGB=%d pB=%d\n",G_OB_previewRval,G_OB_previewGRval,G_OB_previewGBval,G_OB_previewBval);
		printf("sR=%d sGR=%d sGB=%d sB=%d\n",G_OB_snapRval,G_OB_snapGRval,G_OB_snapGBval,G_OB_snapBval);	
	}
	else 
	{
		printf("<read OB para failed>\n");
		G_OB_previewRval = 0;
		G_OB_previewGRval = 0;
		G_OB_previewGBval = 0;
		G_OB_previewBval = 0;
		G_OB_snapRval = 0;
		G_OB_snapGRval = 0;
		G_OB_snapGBval = 0;
		G_OB_snapBval = 0;
		printf("pR=%d pGR=%d pGB=%d pB=%d\n",G_OB_previewRval,G_OB_previewGRval,G_OB_previewGBval,G_OB_previewBval);
		printf("sR=%d sGR=%d sGB=%d sB=%d\n",G_OB_snapRval,G_OB_snapGRval,G_OB_snapGBval,G_OB_snapBval);
	}
	/* sun@1029 am 11:33 end */

	//xqq add for test voltage   
	if(sPara.ui8UserPowerDownVolFlag != 0xA5)
	{
		sPara.ui8BatPowDownVol 	= C_BatPowDownVol;
		sPara.ui8BatVol_Low 	= C_BatVol_LOW;
		sPara.ui8BatVol_Hight 	= C_BatVol_HIGHT;
		sPara.ui8BatVol_Full 	= C_BatVol_FULL;
	}
	else
	{
		if(((sPara.ui8BatVol_Low-sPara.ui8BatPowDownVol)!=C_LOWVOLTAGE)
			||((sPara.ui8BatVol_Hight-sPara.ui8BatVol_Low)!=C_HIGHTVOLTAGE)
				||((sPara.ui8BatVol_Full-sPara.ui8BatVol_Hight)!=C_FULLVOLTAGE))
				{
					sPara.ui8UserPowerDownVolFlag = 0x0A ;
				}
	}
	//xqq add for test voltage   
	
	osMemFree(allocBuf);
	
	return SUCCESS;
#endif
}
/**************************************************************************
 *                                                                        *
 *  Function Name: defautParameters                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
extern UINT32 vcEvCmpIdx;
extern UINT32 pvEvCmpIdx;
/**************************************************************************
 *                                                                        *
 *  Function Name: defautParameters                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 defautParameters(void)
{
#if 1
	sPara.snapFlag = SnapSingle;
	sPara.language = DEFAULT_LANGUAGE ; // 语言
	sPara.VideoModeFlag = DEFAULT_VIDEOMODE;	// TV 制式
	sPara.frequenceFlag = DEFAULT_FREQUENCE;// 光源频率

	sPara.uiPatternFlag = PatternDefault;

	sPara.uiBeepFlag = DEFAULT_BEEPKEYSTOKE;//BeepOn;
	sPara.stampFlag = StampDayTime;
	sPara.USBModeFlag =USB_MODE_MSDC;//USB_MODE_MSDC;//wendy @050125 // USB_MODE_PCCAM;
	sPara.StorageModeFlag = NandFlash; /*Julie*/
	sPara.OTGModeFlag = OTGPC;
	sPara.TvoutFlag = TVOutOff;                  /*Julie*/
	sPara.SleepTimeFlag = SleepTime3m;
//	sPara.SleepTimeFlag = SleepTimeNone;
	
	sPara.ImageQuality = QualitySuper;
	sPara.ISOFlag = ISOAUTO;
	sPara.EffectFlag = NormalEffect;
	sPara.ASFMICFlag = MICON;
	
	/* Paul@2005/09/21 */
	sPara.ui8FlashMode = FlashAuto;
	sPara.ui8ImageSize = ImageSize1280;
	sPara.VideoClipSize = VideoSize320;
	sPara.WBFlag = WBAuto;
	sPara.ui8AutoCloseLCDTime = ui8AutoCloseLCDTimeNever ;

	sPara.Interval = SlideInterval3sec;
	sPara.Repeat = SlideRepeatOn;
	sPara.FolderAll = SlideImageFolder;

	sPara.hour = 0;
	sPara.min = 0;

	sPara.SnakeScore = 0;
	sPara.RussianScore = 0;
	mp3Vol=7;//wendy add 
    sPara.mp3vol=20 ;//wendy@050121 add for record vol

	sPara.ui8Mode = 8;	

	sPara.videoLastFlag = 0;
	sPara.videoLastFrame = 0;
	pvEvCmpIdx = 0x05;	
	vcEvCmpIdx = 0x05;	

	/* Paul@2006/12/25 - 17:24 add start */
	/* Initial Font Ponter */
	UserFont10x20 = UserFont10x20_RES[DEFAULT_LANGUAGE];
	/* Paul@2006/12/25 - 17:24 add end */
	
	return SUCCESS;
#endif

}

//************************************************************************** 
extern  UINT8  ui8DV_DCPreviewFlag ;   //fqdao_add  for  perview or moive   06.6.2
void  hisIconDisp(void)
{
	extern void uiShowBatteryLevelIcon(UINT8 BatteryLevel);
	extern UINT8  BatteryLevel;

	 if(!ui8DV_DCPreviewFlag)//fqdao_add  for  perview or moive   06.6.2
	 {
	 	return ;
	 }
//	if(uiUSBStatus&&(sPara.StorageModeFlag == Udisk))//Davis:patch_2005/Apr/25
//		osdStrDisp(160, 0, UserIcon10x20, 0xd80, "\x7a\x7b");
	 if (isCardExist()&& ( uiState != UI_MAINMENU ))
	 {
//		gpioSet(GPIO_CARD_EN, 1);     
		if( isCardProtect())
		{
			osdStrDisp(128, 0, UserIcon10x20, 0xdd0, "\x9c\x9d");//wendy@050307  update for CARD ICON move to right
		}
		else  osdStrDisp(128, 0, UserIcon10x20, 0xd70, "\x88\x89");//wendy@050307  update for CARD ICON move to right
	 }	
	 else
	 {
//		gpioSet(GPIO_CARD_EN, 0);      
		osdStrDisp(128, 0, UserFont10x20, 0x00, "  ");//wendy@050307  update for CARD ICON move to right
	 }
	 if (DcPlug ==1)
	 {
	 	 osdStrDisp (214, 0, UserIcon10x20, 0xd0, "\x92\x93");
	 } 
//	 	uiShowBatteryLevelIcon(BatteryLevel);	 
	 else
	 {
	 	uiShowBatteryLevelIcon(BatteryLevel);	 
	 }	

	
}

void	F_ScanCard()
{
	 if (isCardExist())
	 {
		gpioSet(GPIO_CARD_EN, 1);      
	 }	
	 else
	 {
		gpioSet(GPIO_CARD_EN, 0);      
	 }	
}
//************************************************************************** 
extern UINT8 PowerLock;//wendy@050205
extern UINT8  ui8KeyHold;
void hisTimeDisp(void)
{
	UINT32	osdW, osdH;
	UINT8 	dateStr[11];
	struct	tm	*ptime;
	
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	timeGet(&ptime);

	ltoa((UINT32)ptime->tm_hour, &dateStr[0], 10, 2);
	strcat(dateStr, ":");
	ltoa((UINT32)ptime->tm_min, &dateStr[3], 10, 2);
	strcat(dateStr, ":");
	ltoa((UINT32)ptime->tm_sec, &dateStr[6], 10, 2);

	dateStr[8] = '\0';
	if ((uiState & UI_MODE_MASK ) == UI_MAINMENU)
	{
		osdStrDisp (osdW- 84, 2, UserFont10x20, 0xe0, dateStr);	//John modify 051021
	}
	else
	{
		osdStrDisp (osdW- 84, 2, UserFont10x20, 0xd0, dateStr);	//John modify 051021
	}
	if(ui8KeyHold)// for bug 12 tzw add 060420
	{
		osdStrDisp(128-20,  2, UserIcon10x20, 0xd0, "\x2e\x2f");
	}
	else
	{	//printf("keyhold Off\n");
		osdStrDisp(128-20, 2, UserFont10x20, 0x00, "  ");
	}


	
}
//************************************************************************** 
extern OpenLcdDisplay(UINT32 flag);
void osdLcdDisp(UINT8 option)
{
  switch(option)
  {
   	 case 0:  /* colse osd*/
	    if (lcdOsdDisp == 1)
        {
       		lcdOsdDisp = 2;
       		hwDispOsdEn(0);
        }
	    break;

      case 1:   /*open osd */
	  	if (lcdOsdDisp == 2)
       	{
       		lcdOsdDisp = 1;
       		hwDispOsdEn(1);
       	}
		break;

      case 2: /* open & close osd*/
	 	if (lcdOsdDisp == 2)
       	{
       		lcdOsdDisp = 1;
       		hwDispOsdEn(1);
       	}
	 	else
	   	{
	   		lcdOsdDisp = 2;
	   		hwDispOsdEn(0);
	   	}
		break;
		
	case 3: /* open & close LCD*/
		if (lcdOsdDisp == 2)
       	{
       		lcdOsdDisp = 1;
  		    OpenLcdDisplay(1);
			gpioSet(GPIO_LCD_POWER,1);
		}
	 	else
       	{
       		lcdOsdDisp = 2;
            OpenLcdDisplay(0);
			gpioSet(GPIO_LCD_POWER,0);
       	}
		break;

	case 4: /* open LCD*/
		if (lcdOsdDisp == 0)
        {
	        lcdOsdDisp = 1;
     		OpenLcdDisplay(1);
			gpioSet(GPIO_LCD_POWER,1);
        }
		break;
		
	case 5:	/* Paul@2005/10/03 */
		if (lcdOsdDisp == 0)
		{
       		lcdOsdDisp = 1;
			hwDispOsdEn(0);
		}
		else if (lcdOsdDisp == 1)
		{
			lcdOsdDisp = 2;
 			OpenLcdDisplay(0);
			gpioSet(GPIO_LCD_POWER,0);
		}
		else
		{
			lcdOsdDisp = 0;			
			hwDispOsdEn(1);	
 			OpenLcdDisplay(1);
			gpioSet(GPIO_LCD_POWER,1);
		}
		break;
  }
}
//************************************************************************** 
extern UINT8 	pressVolume; /*Julie@2004.9.17*///wendy@050205 
extern UINT32 g_ui32SaveKeyCode;

void osdAutoDisp(void)
{
 	UINT8 ui8Temp;
	
//wendy@050228 begin
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
				pressVolume = 0;//wendy@050205 
	//			osdStrDisp(PANEL_1_OSD_W/2-40 ,PANEL_1_OSD_H-70+8, UserFont, 0x00, "        ");  //tzw 10/10/10
				osdBarDraw(PANEL_1_OSD_W/2-40 , PANEL_1_OSD_H-66, 110, 30, 0x00);	
				printf("clear volum mp3**************\n");
		}
		else
		{
			if ((avPlayNow)||(snapAV))
			{
				pressVolume = 0;//wendy@050205 
		//		osdStrDisp(PANEL_1_OSD_W/2-40 ,PANEL_1_OSD_H-70+8, UserFont, 0x00, "        ");  //tzw 10/10/10
				osdBarDraw(PANEL_1_OSD_W/2-40, PANEL_1_OSD_H-66, 110, 30, 0x00);	
	              	osdLcdDisp(0);
				printf("clear volum av**************\n");				
	       	}
		}
	}
}
/**************************************************************************
 *  Function Name: osdIconDisp                                            *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments: None                                                       *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/

void osdIconDisp (UINT8 isClear)
{
	UINT8  	str[25];
	UINT32	osdW, osdH;
//	asfFileInfo_t  asfFileInfo;
//	movFileInfo_t movFileInfo;
//	aviFileInfo_t   aviFileInfo;
//	audFileInfo_t  audFileInfo;
    UINT32 sys;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	if (isClear)
	{
		osdFWBuffChange();
		osdClearScreen(0x00);
	}
	
	osdStrDisp(osdW/2+6, 0, UserFont10x20, 0x0, "  ");
	if ((pbDisp == PB_DISP_ONE)||(mp3Disp==PB_DISP_ONE))
	{
		 switch (g_playrecStatus)
		{

			case	STATUS_STOP:
				osdStrDisp(osdW/2+6 , 0, UserIcon10x20, 0x30, "\x57\x58");
				break;
			case	STATUS_PLAYING:
				osdStrDisp(osdW/2+6 , 0, UserIcon10x20, 0x40, "\x8e\x8f");
				break;
			case	STATUS_PAUSE:
				osdStrDisp(osdW/2+6, 0, UserIcon10x20, 0x30, "\x90\x91");
				break;
			case	STATUS_FF:
				osdStrDisp(osdW/2+6, 0, UserIcon10x20, 0x40, "\x8a\x8b");
				break;
			case	STATUS_REW:
				osdStrDisp(osdW/2+6, 0, UserIcon10x20, 0x40, "\x8c\x8d");
				break;
			default:
				osdStrDisp(osdW/2+6, 0, UserFont10x20, 0x0, "  ");
				break;
		}
	}
	
	if ((uiState & UI_MODE_MASK) == UI_MODE_PLAYBACK)
	{
	}
	else if ((uiState & UI_MODE_MASK) == UI_MODE_MP3)
	{
	}
	else if ((uiState & UI_MODE_MASK) == UI_MODE_SETUP)
	{
	}
	else if ((uiState & UI_MODE_MASK) == UI_MODE_ACCESSORIES)
	{
	}
	else if ((uiState & UI_MODE_MASK) == UI_MAINMENU)
	{
	}
	else if ((uiState & UI_MODE_MASK) == UI_MODE_RECORD)
	{
		if(snapAV)
		{
			if(snapAV == OBJ_ASF)
			{
				hwFrontSscRead(0x1f,&sys,0x01,0x02);
				blinkcnt ++;
				if (blinkcnt == 1 )
				{
					osdStrDisp(40, 0, UserIcon10x20, 0x10, "\x28\x29");
					
					if(!(sys&0x01))
					{
					   osdStrDisp((PANEL_1_OSD_W-10*10)/2, (PANEL_1_OSD_H-20)/2, UserFont10x20, 0x40, GetString(NO_Signal));
					   snapAV = OBJ_NONE;
					}
					else
					{
					   osdStrDisp((PANEL_1_OSD_W-10*10)/2, (PANEL_1_OSD_H-20)/2, UserFont10x20, 0x40, "                 ");
					}
				}
				else if (blinkcnt == 5)
				{
					osdStrDisp(40, 0, UserFont10x20, 0x00, "  ");
					osdStrDisp((PANEL_1_OSD_W-10*10)/2, (PANEL_1_OSD_H-20)/2, UserFont10x20, 0x40, "                 ");
				}
				else if (blinkcnt == 8)
					blinkcnt = 0;

				sio_psprintf(str, "%02d:%02d:%02d ", asfTime/1000/60/60, (asfTime%3600000)/1000/60, asfTime/1000%60);
				osdStrDisp(osdW-130, 0, UserFont10x20, 0x10, str);
			}
			else if(snapAV == OBJ_MP3)
			{
				sio_psprintf(str, "%02d:%02d:%02d ", wavTime/1000/60/60, (wavTime%3600000)/1000/60, wavTime/1000%60);
				osdStrDisp(60, 160, UserFont10x20, 0x10, str);
			}
			else if(snapAV == OBJ_WAV)
			{
				sio_psprintf(str, "%02d:%02d:%02d ", wavTime/1000/60/60, (wavTime%3600000)/1000/60, wavTime/1000%60);
				osdStrDisp(60, 160, UserFont10x20, 0x10, str);
			}
			//wendy@050303 end
			else
			{
				str[0] = 0;
				osdStrDisp(60, 160, UserFont10x20, 0x10, str);
			}
			DateDisplay();
		}

	}
	if (isClear)
	{
		osdHWBuffChange();
	}	
}

/**************************************************************************
 *  Function Name: osdFontLoad                                            *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
*************************************************************************/
 #if 1
#if 1 //LANGUAGES
void osdFontLoad_K(void)
{
	UINT32	count, w, h, i;
	UINT8	*ptr;

	if( sPara.language == Languagekorean)
	{
		ptr	= (UINT8 *)((0xAC000000	| (UINT32)UserFontHZ_F));	
		nandRsvBlkRead(SECTOR_OF_FONT_K, 1, ptr);
		count =	ptr[0] + ptr[1]*256;
		w	  =	ptr[2];
		h	  =	ptr[3];

		if(!count || (w != 20) || (h != 20))
		{
			printf("FONT_F ERROR...\n");	
			HANZIFlag |= 0x02;	
		}
		else
		{
			w = (w+7)/8*8;		
			i =	(count*w*h/8+16+511)/512;
			nandRsvBlkRead(SECTOR_OF_FONT_K, i, UserFontHZ_F);
			printf("Font_F loaded");				
			HANZIFlag &= 0xFD;
		}	
	}
	if( sPara.language == LanguageChinese_T)
	{
		ptr	= (UINT8 *)((0xAC000000	| (UINT32)UserFontHZ_F));	
		nandRsvBlkRead(SECTOR_OF_FONT_F, 1, ptr);
		count =	ptr[0] + ptr[1]*256;
		w	  =	ptr[2];
		h	  =	ptr[3];

		if(!count || (w != 20) || (h != 20))
		{
			printf("FONT_F ERROR...\n");	
			HANZIFlag |= 0x02;	
		}
		else
		{
			w = (w+7)/8*8;		
			i =	(count*w*h/8+16+511)/512;
			nandRsvBlkRead(SECTOR_OF_FONT_F, i, UserFontHZ_F);
			printf("Font_F loaded");				
			HANZIFlag &= 0xFD;
		}	
	}	
}
#endif

#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
void osdFontLoad(void)
{
	UINT32	count, w, h, i;
	UINT8	*ptr;
		
	ptr	= (UINT8 *)((0xAC000000	| (UINT32)UserFontHZ_J));
	
	nandRsvBlkRead(SECTOR_OF_FONT_J, 1, ptr);
	count =	ptr[0] + ptr[1]*256;
	w	  =	ptr[2];
	h	  =	ptr[3];

	printf("w = %d, h = %\n", w, h);
	if(!count || (w != 20) || (h != 20))
	{
		printf("FONT_J ERROR...\n");	
		HANZIFlag |= 0x01;	
	}
	else
	{
		w = (w+7)/8*8;
		i =	(count*w*h/8+16+511)/512;
		nandRsvBlkRead(SECTOR_OF_FONT_J, i, UserFontHZ_J);
		printf("Font_J loaded");				
		HANZIFlag &= 0xFE;
	}
	#if 1 //LANGUAGES
	osdFontLoad_K(); 
	#else
	
	ptr	= (UINT8 *)((0xAC000000	| (UINT32)UserFontHZ_F));	
	nandRsvBlkRead(SECTOR_OF_FONT_F, 1, ptr);
	count =	ptr[0] + ptr[1]*256;
	w	  =	ptr[2];
	h	  =	ptr[3];

	if(!count || (w != 20) || (h != 20))
	{
		printf("FONT_F ERROR...\n");	
		HANZIFlag |= 0x02;	
	}
	else
	{
		w = (w+7)/8*8;		
		i =	(count*w*h/8+16+511)/512;
		nandRsvBlkRead(SECTOR_OF_FONT_F, i, UserFontHZ_F);
		printf("Font_F loaded");				
		HANZIFlag &= 0xFD;
	}	
	#endif
}
#endif
#else
#if (SUPPORT_HANZI == 1)
void osdFontLoad(void)
{
	#if	1
	printf("\nRead 32x32 font from NAND ...\n");
	#endif

	UINT32	count, w, h, i;
	UINT8	*ptr;
	UINT8      g_chinese;
	ptr	= (UINT8 *)((0xAC000000	| (UINT32)UserFont32x32));

	if (sPara.language == LanguageChinese_T)
		nandRsvBlkRead(SECTOR_OF_32X32_FONT_T, 1, ptr);
	else /*if (sPara.language == LanguageChinese)*/
	nandRsvBlkRead(SECTOR_OF_32X32_FONT, 1, ptr);
	count =	ptr[0] + ptr[1]*256;
	w	  =	ptr[2];
	h	  =	ptr[3];

	#if	1
	printf("count=%d, w=%d h=%d\n", count, w, h );
	#endif
	/*(Gu@ 2004/012/10, 10:45 */
	if(count == COUNT_OF_32X32_FONT)
		 g_chinese = LanguageChinese_S;
	else if(count == COUNT_OF_32X32_FONT_T)
		 g_chinese = LanguageChinese_T;

	if(sPara.language != LanguageEnglish)
		sPara.language =  g_chinese;
	/*)Gu@ 2004/012/10, 10:45 */

	if(count > COUNT_OF_32X32_FONT_T || !count || !w || !h)
	{
		printf("FONT ERROR...\n");
				/*wendy add*/
		HANZIFlag=1;
		/*wendy add*/

	}
	else
	{
		i =	(count*w*h*2/8+16+511)/512;
		if (sPara.language == LanguageChinese_T)
			nandRsvBlkRead(SECTOR_OF_32X32_FONT_T, i, UserFont32x32);
		else /*if (sPara.language == LanguageChinese)*/
		nandRsvBlkRead(SECTOR_OF_32X32_FONT, i, UserFont32x32);
		#if	1
		printf("Read 32x32 font End\n");
		#endif
	}
}
#endif
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: osdStrDisp                                             *
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
 *     //wendy@050302 update for  tradtional                                                                 *
 **************************************************************************/
 #if 1  //加语言开始  LANGUAGES
 

 void osdStrDisp_S(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[])
{
	
	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI == 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	#else

	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;

	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;

	while(str[i])
	{
		if(str[i]<=0x80)
		{
			if(pFont==UserFont10x20){
		
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;
//				if (ColorOrder[1] == 0x00)
//					ColorOrder[1] = 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
//				if (ColorOrder[3] == 0x00)
//					ColorOrder[3] = 0x0e;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];			
			}
		}
		else if ((str[i] == 0xA8)
			&& (str[i+1] > 0xA0)
			&& (str[i+1] < 0xBB))
		{
			str16[0] = (str[i++] - 0xa8);
			str16[0] += str[i++] - 0x91;			
			osdStrDraw16(x+xOffset, y+yOffset, UserFont10x20_ES, bgColor, str16);			
			xOffset += UserFont10x20_ES[2];
		}
		else //if((str[i]>0x80) && (str[i]<=0xa0))
		{
			if(pFont == UserFont10x20)
			{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;
//				if (ColorOrder[1] == 0x00)
//					ColorOrder[1] = 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
//				if (ColorOrder[3] == 0x00)
//					ColorOrder[3] = 0x0e;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];	
			}

		}
	}
	#endif
}

 
 void osdStrDisp_C(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[])
{
	
	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI == 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	#else

	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;

	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;

	while(str[i])
	{
		if(str[i]<=0x80)
		{
			if(pFont==UserFont10x20){
		
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;
//				if (ColorOrder[1] == 0x00)
//					ColorOrder[1] = 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
//				if (ColorOrder[3] == 0x00)
//					ColorOrder[3] = 0x0e;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];			
			}
		}
		
		#if 1
		else if((str[i]>0x80) && (str[i]<=0xa0))
		{
			if(pFont==UserFont10x20)
			{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;
//				if (ColorOrder[1] == 0x00)
//					ColorOrder[1] = 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
//				if (ColorOrder[3] == 0x00)
//					ColorOrder[3] = 0x0e;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];	
			}

		}
		else if ((str[i]>0xa0) && ((sPara.language == LanguageChinese_S)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x01)
		 		break;
		
			str16[0] = (str[i++]-0xa1)*94;
			if (str[i] == '\0')
				break;
			str16[0] += str[i++] - 0xa1;
			
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_J, bgColor, str16);
			xOffset += UserFontHZ_J[2];
		}
		
		else if (((str[i]>0xa0) && (str[i]<=0xc8)) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;

			str16[0] = (str[i++]-0xa1)*157;
			if (str[i] == '\0')
				break;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
		else if ((str[i]>0xc8) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1-2)*157;
			if (str[i] == '\0')
				break;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
		#endif
	}
	#endif
}

 
 void osdStrDisp_K(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[])
{
	
	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI == 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	#else

	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;

	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;

	while(str[i])
	{
		if(str[i]<=0x80)
		{
			if(pFont==UserFont10x20){
		
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;
//				if (ColorOrder[1] == 0x00)
//					ColorOrder[1] = 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
//				if (ColorOrder[3] == 0x00)
//					ColorOrder[3] = 0x0e;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];			
			}
		}		
		else if((str[i]>0x80) && (str[i]<=0xa0))
		{
			if(pFont==UserFont10x20)
			{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;
//				if (ColorOrder[1] == 0x00)
//					ColorOrder[1] = 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
//				if (ColorOrder[3] == 0x00)
//					ColorOrder[3] = 0x0e;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];	
			}

		}
		else if ((str[i]>0xa0) && ((sPara.language == Languagekorean)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x02)
		 		break;
		
			str16[0] = (str[i++]-0xa1)*94;
			if (str[i] == '\0')
				break;
			str16[0] += str[i++] - 0xa1;
			
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
		
	}
	#endif
}


 void osdStrDisp_R(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[])
{
	
	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI == 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	#else

	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;

	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;

	while(str[i])
	{
		if(str[i]<=0x80)
		{
			if(pFont==UserFont10x20){
		
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;
//				if (ColorOrder[1] == 0x00)
//					ColorOrder[1] = 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
//				if (ColorOrder[3] == 0x00)
//					ColorOrder[3] = 0x0e;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];			
			}
		}
		else if ((str[i] == 0xA7)
			&& (str[i+1] > 0xA0))
		//	&& (str[i+1] < 0xBB))
		{
			str16[0] = (str[i++] - 0xa7);
			str16[0] += str[i++] - 0x91;			
			osdStrDraw16(x+xOffset, y+yOffset, UserFont10x20_ER, bgColor, str16);			
			xOffset += UserFont10x20_ER[2];
		}
		else //if((str[i]>0x80) && (str[i]<=0xa0))
		{
			if(pFont == UserFont10x20)
			{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;
//				if (ColorOrder[1] == 0x00)
//					ColorOrder[1] = 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
//				if (ColorOrder[3] == 0x00)
//					ColorOrder[3] = 0x0e;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];	
			}

		}
	}
	#endif
}

 void osdStrDisp(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[])
{

	osdStrDisPro_S[sPara.language]( x,  y,  pFont,  bgColor, str);
#if 0	
#if VersionLanguage_E

	osdStrDisp_C( x,  y,  pFont,  bgColor, str);

#endif 

#if VersionLanguage_ECT

	osdStrDisp_C( x,  y,  pFont,  bgColor, str);

#endif 

#if VersionLanguage_ER
	if( sPara.language == LanguageRussian )
	{
		osdStrDisp_R( x,  y,  pFont,  bgColor,  str);
	}
	else
	{
		osdStrDisp_C( x,  y,  pFont,  bgColor, str);
	}


#endif

#if VersionLanguage_ES8

	osdStrDisp_S( x,  y,  pFont,  bgColor,  str);

#endif

#endif
}



UINT8  osdStrDispExt_S(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{

	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	return 0;
	#else
	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;


	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;
	i = start;

	while((str[0]) && (str[i]) && ((i-start)<lgh))
	{	
	
		if(str[i]<=0x80)
		{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont,0x00, "      ");			//for bug 48 John 11092005d
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
		}
		else if ((str[i] == 0xA8)
			&& (str[i+1] > 0xA0)
			&& (str[i+1] < 0xBB))
		{
			str16[0] = (str[i++] - 0xa8);
			str16[0] += str[i++] - 0x91;			
			osdStrDraw16(x+xOffset, y+yOffset, UserFont10x20_ES, bgColor, str16);			
			xOffset += UserFont10x20_ES[2];
		}
		else //if((str[i]>0x80) && (str[i]<=0xa0))		
		{	 	 
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, 0x00, "      ");
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
	 
		}
		#if 0
		else if ((str[i]>0xa0) && ((sPara.language == LanguageChinese_S)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x01)
	 			break;
			
			str16[0] = (str[i++]-0xa1)*94;
			str16[0] += str[i++] - 0xa1;
			osdChgImgPatt(ColorOrder);			
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00,"      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_J, bgColor, str16);
		//	xOffset += UserFontHZ_J[2];
			xOffset += UserFontHZ_J[2];
		}
		else if (((str[i]>0xa0) && (str[i]<=0xc8)) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			
			xOffset += UserFontHZ_F[2];
		}
		else if ((str[i]>0xc8) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1-2)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
		#endif	 
	}

	return i;

	#endif
}

UINT8  osdStrDispExt_C(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{

	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	return 0;
	#else
	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;


	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;
	i = start;

	while((str[0]) && (str[i]) && ((i-start)<lgh))
	{	
#if 0		
		if(option == 1)
			osTimeDly(6);		

		if(option == 2)
			osTimeDly(1);
		hisTimeDisp();//machiner 2005-10-29
		if (avPlayNow ==OBJ_NONE && uiUSBStatus == 1 ) //for bug 31 2005-10-17
		{
			break;
		}					 
#endif		
		if(str[i]<=0x80)
		{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont,0x00, "      ");			//for bug 48 John 11092005d
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
		}
		else if((str[i]>0x80) && (str[i]<=0xa0))
		{	 	 
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, 0x00, "      ");
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
	 
		}
		else if ((str[i]>0xa0) && ((sPara.language == LanguageChinese_S)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x01)
	 			break;
			
			str16[0] = (str[i++]-0xa1)*94;
			str16[0] += str[i++] - 0xa1;
			osdChgImgPatt(ColorOrder);			
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00,"      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_J, bgColor, str16);
		//	xOffset += UserFontHZ_J[2];
			xOffset += UserFontHZ_J[2];
		}
		else if (((str[i]>0xa0) && (str[i]<=0xc8)) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			
			xOffset += UserFontHZ_F[2];
		}
		else if ((str[i]>0xc8) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1-2)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
			 
	}

	return i;

	#endif
}

UINT8  osdStrDispExt_K(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{

	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	return 0;
	#else
	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;


	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;
	i = start;

	while((str[0]) && (str[i]) && ((i-start)<lgh))
	{	
#if 0		
		if(option == 1)
			osTimeDly(6);		

		if(option == 2)
			osTimeDly(1);
		hisTimeDisp();//machiner 2005-10-29
		if (avPlayNow ==OBJ_NONE && uiUSBStatus == 1 ) //for bug 31 2005-10-17
		{
			break;
		}					 
#endif		
		if(str[i]<=0x80)
		{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont,0x00, "      ");			//for bug 48 John 11092005d
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
		}
		else if((str[i]>0x80) && (str[i]<=0xa0))
		{	 	 
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, 0x00, "      ");
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
	 
		}
		else if ((str[i]>0xa0) && ((sPara.language == Languagekorean)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x02)
	 			break;
			
			str16[0] = (str[i++]-0xa1)*94;
			str16[0] += str[i++] - 0xa1;
			osdChgImgPatt(ColorOrder);			
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00,"      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
		//	xOffset += UserFontHZ_J[2];
			xOffset += UserFontHZ_F[2];
		}
		
		
			 
	}

	return i;

	#endif
}

UINT8  osdStrDispExt_R(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{

	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	return 0;
	#else
	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;


	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;
	i = start;

	while((str[0]) && (str[i]) && ((i-start)<lgh))
	{	
	
		if(str[i]<=0x80)
		{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont,0x00, "      ");			//for bug 48 John 11092005d
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
		}
		else if ((str[i] == 0xA7)
			&& (str[i+1] > 0xA0))
		//	&& (str[i+1] < 0xBB))
		{
			str16[0] = (str[i++] - 0xa7);
			str16[0] += str[i++] - 0x91;			
			osdStrDraw16(x+xOffset, y+yOffset, UserFont10x20_ER, bgColor, str16);			
			xOffset += UserFont10x20_ER[2];
		}
		else //if((str[i]>0x80) && (str[i]<=0xa0))		
		{	 	 
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, 0x00, "      ");
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
	 
		}
		#if 0
		else if ((str[i]>0xa0) && ((sPara.language == LanguageChinese_S)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x01)
	 			break;
			
			str16[0] = (str[i++]-0xa1)*94;
			str16[0] += str[i++] - 0xa1;
			osdChgImgPatt(ColorOrder);			
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00,"      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_J, bgColor, str16);
		//	xOffset += UserFontHZ_J[2];
			xOffset += UserFontHZ_J[2];
		}
		else if (((str[i]>0xa0) && (str[i]<=0xc8)) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			
			xOffset += UserFontHZ_F[2];
		}
		else if ((str[i]>0xc8) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1-2)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
		#endif	 
	}

	return i;

	#endif
}

UINT8  osdStrDispExt(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{
	osdStrDisExt_S[sPara.language](x,  y,  pFont,  bgColor, str, start,  lgh,  option);
#if 0
#if VersionLanguage_E

	osdStrDispExt_C( x,  y,  pFont,  bgColor, str, start,  lgh,  option);

#endif 


#if VersionLanguage_ECT

	osdStrDispExt_C( x,  y,  pFont,  bgColor, str, start,  lgh,  option);

#endif 

#if VersionLanguage_ER
	if( sPara.language == LanguageRussian )
	{
		osdStrDispExt_R( x,  y,  pFont,  bgColor,  str, start,  lgh,  option);
	}
	else 
	{
		osdStrDispExt_C( x,  y,  pFont,  bgColor, str, start,  lgh,  option);
	}	



#endif

#if VersionLanguage_ES8

	osdStrDispExt_S( x,  y,  pFont,  bgColor,  str, start,  lgh,  option);

#endif
#endif

}


UINT8  F_osdLRCDisp_C(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{

	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	return 0;
	#else
	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;


	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;
	i = start;

	while((str[0]) && (str[i]) && ((i-start)<lgh))
	{	
		if(option == 1)
			osTimeDly(6);		

		if(option == 2)
			osTimeDly(1);
		hisTimeDisp();//machiner 2005-10-29
		if (avPlayNow ==OBJ_NONE && uiUSBStatus == 1 ) //for bug 31 2005-10-17
		{
			break;
		}					 
		if(str[i]<=0x80)
		{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont,0x00, "      ");			//for bug 48 John 11092005d
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
		}
		else if((str[i]>0x80) && (str[i]<=0xa0))
		{	 	 
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, 0x00, "      ");
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
	 
		}
		else if ((str[i]>0xa0) && ((sPara.language == LanguageChinese_S)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x01)
	 			break;
			
			str16[0] = (str[i++]-0xa1)*94;
			str16[0] += str[i++] - 0xa1;
			osdChgImgPatt(ColorOrder);			
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00,"      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_J, bgColor, str16);
		//	xOffset += UserFontHZ_J[2];
			xOffset += UserFontHZ_J[2];
		}
		else if (((str[i]>0xa0) && (str[i]<=0xc8)) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
//			osdChgImgPatt(ColorOrder);
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			
		//	xOffset += UserFontHZ_F[2];
			xOffset += UserFontHZ_F[2];
		}
		else if ((str[i]>0xc8) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1-2)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
//			osdChgImgPatt(ColorOrder);
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
			 
	}

	return i;

	#endif
}



UINT8  F_osdLRCDisp_K(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{

	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	return 0;
	#else
	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;


	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;
	i = start;

	while((str[0]) && (str[i]) && ((i-start)<lgh))
	{	
		if(option == 1)
			osTimeDly(6);		

		if(option == 2)
			osTimeDly(1);
		hisTimeDisp();//machiner 2005-10-29
		if (avPlayNow ==OBJ_NONE && uiUSBStatus == 1 ) //for bug 31 2005-10-17
		{
			break;
		}					 
		if(str[i]<=0x80)
		{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont,0x00, "      ");			//for bug 48 John 11092005d
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
		}
		else if((str[i]>0x80) && (str[i]<=0xa0))
		{	 	 
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, 0x00, "      ");
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
	 
		}
		else if ((str[i]>0xa0) && ((sPara.language == Languagekorean)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x02)
	 			break;
			
			str16[0] = (str[i++]-0xa1)*94;
			str16[0] += str[i++] - 0xa1;
			osdChgImgPatt(ColorOrder);			
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00,"      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
		//	xOffset += UserFontHZ_J[2];
			xOffset += UserFontHZ_F[2];
		}
		
			 
	}

	return i;

	#endif
}

UINT8  F_osdLRCDisp_R(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{

	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	return 0;
	#else
	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;


	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;
	i = start;

	while((str[0]) && (str[i]) && ((i-start)<lgh))
	{	
		if(option == 1)
			osTimeDly(6);		

		if(option == 2)
			osTimeDly(1);
		hisTimeDisp();//machiner 2005-10-29
		if (avPlayNow ==OBJ_NONE && uiUSBStatus == 1 ) //for bug 31 2005-10-17
		{
			break;
		}					 
		if(str[i]<=0x80)
		{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont,0x00, "      ");			//for bug 48 John 11092005d
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
		}
		else if ((str[i] == 0xA7)
				&& (str[i+1] > 0xA0))
		//	&& (str[i+1] < 0xBB))
		{
			str16[0] = (str[i++] - 0xa7);
			str16[0] += str[i++] - 0x91;			
			osdStrDraw16(x+xOffset, y+yOffset, UserFont10x20_ER, bgColor, str16);			
			xOffset += UserFont10x20_ER[2];
		}
		else //if((str[i]>0x80) && (str[i]<=0xa0))		
		{	 	 
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, 0x00, "      ");
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
	 
		}
		#if 0
		else if ((str[i]>0xa0) && ((sPara.language == LanguageChinese_S)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x01)
	 			break;
			
			str16[0] = (str[i++]-0xa1)*94;
			str16[0] += str[i++] - 0xa1;
			osdChgImgPatt(ColorOrder);			
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00,"      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_J, bgColor, str16);
		//	xOffset += UserFontHZ_J[2];
			xOffset += UserFontHZ_J[2];
		}
		else if (((str[i]>0xa0) && (str[i]<=0xc8)) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
//			osdChgImgPatt(ColorOrder);
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			
		//	xOffset += UserFontHZ_F[2];
			xOffset += UserFontHZ_F[2];
		}
		else if ((str[i]>0xc8) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1-2)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
//			osdChgImgPatt(ColorOrder);
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
	 #endif
	}

	return i;

	#endif
}



UINT8  F_osdLRCDisp_S(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{

	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, UserFont, bgColor, str);
	return 0;
	#else
	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;


	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;
	i = start;

	while((str[0]) && (str[i]) && ((i-start)<lgh))
	{	
		if(option == 1)
			osTimeDly(6);		

		if(option == 2)
			osTimeDly(1);
		hisTimeDisp();//machiner 2005-10-29
		if (avPlayNow ==OBJ_NONE && uiUSBStatus == 1 ) //for bug 31 2005-10-17
		{
			break;
		}					 
		if(str[i]<=0x80)
		{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont,0x00, "      ");			//for bug 48 John 11092005d
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
		}
		else if ((str[i] == 0xA8)
			&& (str[i+1] > 0xA0)
			&& (str[i+1] < 0xBB))
		{
			str16[0] = (str[i++] - 0xa8);
			str16[0] += str[i++] - 0x91;			
			osdStrDraw16(x+xOffset, y+yOffset, UserFont10x20_ES, bgColor, str16);			
			xOffset += UserFont10x20_ES[2];
		}
		else //if((str[i]>0x80) && (str[i]<=0xa0))		
		{	 	 
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, 0x00, "      ");
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
	 
		}
		#if 0
		else if ((str[i]>0xa0) && ((sPara.language == LanguageChinese_S)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x01)
	 			break;
			
			str16[0] = (str[i++]-0xa1)*94;
			str16[0] += str[i++] - 0xa1;
			osdChgImgPatt(ColorOrder);			
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00,"      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_J, bgColor, str16);
		//	xOffset += UserFontHZ_J[2];
			xOffset += UserFontHZ_J[2];
		}
		else if (((str[i]>0xa0) && (str[i]<=0xc8)) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
//			osdChgImgPatt(ColorOrder);
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			
		//	xOffset += UserFontHZ_F[2];
			xOffset += UserFontHZ_F[2];
		}
		else if ((str[i]>0xc8) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1-2)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
//			osdChgImgPatt(ColorOrder);
			osdStrDraw(x+xOffset, y+yOffset, UserFont10x20, 0x00, "      ");
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
	 #endif
	}

	return i;

	#endif
}


UINT8  F_osdLRCDisp(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{
	osdLRCDisPro_S[sPara.language]( x,  y,  pFont,  bgColor, str, start,  lgh,  option);
#if 0
#if VersionLanguage_E

	F_osdLRCDisp_C( x,  y,  pFont,  bgColor, str, start,  lgh,  option);

#endif 


#if VersionLanguage_ECT

	F_osdLRCDisp_C( x,  y,  pFont,  bgColor, str, start,  lgh,  option);

#endif 

#if VersionLanguage_ER
	if( sPara.language == LanguageRussian )
	{
		F_osdLRCDisp_R( x,  y,  pFont,  bgColor,  str, start,  lgh,  option);
	}
	else
	{
		F_osdLRCDisp_C( x,  y,  pFont,  bgColor, str, start,  lgh,  option);
	}	


#endif

#if VersionLanguage_ES8

	F_osdLRCDisp_S( x,  y,  pFont,  bgColor,  str, start,  lgh,  option);

#endif	
#endif
}

#if 0
 UINT8 osdStrDispExt(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
{
	#if ((SUPPORT_HANZI == 1)||(SUPPORT_FANTI== 1))
	UINT8	str8[2];
	UINT16	str16[2];
	UINT32	xOffset, yOffset;
	UINT8	i=0;
	UINT8	ColorOrder[16];
	#endif

	#if ((SUPPORT_HANZI == 0)&&(SUPPORT_FANTI== 0))
	osdStrDraw(x, y, pFont, bgColor, str);
	return 0;
	#else
	if((bgColor&0x00f0)==0) bgColor = bgColor&0xff0f|0x00e0 ;
	if((bgColor&0x0f00)==0) bgColor |=0x0f00;

	xOffset=0x00;
	yOffset=0x00;
	str8[1]=0x00;
	str16[1]=0x00;
	i = start;
	while((str[0]) && (str[i]) && ((i-start)<lgh))
	{
		if(option == 1)
			osTimeDly(6);
		
		if(option == 2)
			osTimeDly(2);

		if (avPlayNow ==OBJ_NONE && uiUSBStatus == 1 ) //for bug 31 2005-10-17
		{
			break;
		}						//for bug 31 2005-10-17

		if(str[i]<=0x80)
		{
			if(pFont==UserFont10x20){
		
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];			
			}
			
		}
		else if((str[i]>0x80) && (str[i]<=0xa0))
		{
		
			if(pFont==UserFont10x20)
			{
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];
			}
			else
			{
				ColorOrder[0] = bgColor & 0x0f;

				/*ColorOrder[1] = 0x0f;*/
				ColorOrder[1] = (bgColor>>8) & 0x0f;

				ColorOrder[2] = 0x07;
				ColorOrder[3] = (bgColor>>4) & 0x0f;
				osdChgImgPatt(ColorOrder);

				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, -1, str8);
				xOffset += pFont[2];	
			}
		}
		else if ((str[i]>0xa0) && ((sPara.language == LanguageChinese_S)|(sPara.language == LanguageEnglish)))
		{
			if(HANZIFlag & 0x01)
	 			break;
			
			str16[0] = (str[i++]-0xa1)*94;
			str16[0] += str[i++] - 0xa1;
			osdChgImgPatt(ColorOrder);			
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_J, bgColor, str16);
		//	xOffset += UserFontHZ_J[2];
			xOffset += UserFontHZ_J[2];
		}
		else if (((str[i]>0xa0) && (str[i]<=0xc8)) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdChgImgPatt(ColorOrder);
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
		//	xOffset += UserFontHZ_F[2];
			xOffset += UserFontHZ_F[2];
		}
		else if ((str[i]>0xc8) && (sPara.language == LanguageChinese_T))
		{
			if(HANZIFlag & 0x02)
		 		break;
			
			str16[0] = (str[i++]-0xa1-2)*157;
			if (str[i]<0x7f)
				str16[0] += str[i++] - 0x40;
			else
				str16[0] += str[i++] - 0x40 - 0x22;
			osdChgImgPatt(ColorOrder);
			osdStrDraw16(x+xOffset, y+yOffset, UserFontHZ_F, bgColor, str16);
			xOffset += UserFontHZ_F[2];
		}
	}
	return i;
	#endif
}
#endif
 #endif
/**************************************************************************
 *  Function Name: hisTask                                                *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *    pdata: in, task's replicated argument.                              *
 *  Returns: Never returns.                                               *
 *  See also:                                                             *
**************************************************************************/
extern UINT32 gPB;
void hisTask(void *pdata)
{
	UINT32	osdW, osdH;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	while(1)
	{
		osSchedLock();
		hisTimeDisp();
		hisIconDisp();
		osSchedUnlock();			
		osTimeDly(1);
	}
}
/**************************************************************************
 *  Function Name: osdHistogramDraw                                       *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
void osdHistogramDraw(SINT32 x1, SINT32 y1, SINT32 x2, SINT32 y2, SINT32 color)
{
	UINT16 i;
	UINT32 pcdspHisReg[256], h;
	UINT32 countW, countH;

	if(((( uiState & UI_MODE_MASK ) == UI_MODE_PREVIEW)  ||
	    (( uiState & UI_MODE_MASK ) == UI_MODE_MOVIE)))
	{
		hwFrontMaxMonitorSizeGet(&countW, &countH);
		hwCdspHisRead(pcdspHisReg);
	}
	else if (( uiState & UI_MODE_MASK ) == UI_MODE_PLAYBACK)
	{
		UINT32 x, y;
		UINT8* pDisp;

		hwPlayBufSizeGet(&countW, &countH);

		for (x=0; x<256; x++)
		{
			pcdspHisReg[x] = 0;
		}

		for (y=0; y<countH; y++)
		{
			for (x=0; x<(countW&0xfffffff8); x=x+8*2)	/* "*2" is 1/2 subsample */
			{
				pDisp = (UINT8*)(0xAC000000+gPB*2+2*(y*countW+x));
				if(pcdspHisReg[*pDisp] < 0xffff)
				{
					pcdspHisReg[*pDisp]++;
				}
				pDisp++;
				if(pcdspHisReg[*pDisp] < 0xffff)
				{
					pcdspHisReg[*pDisp]++;
				}
				pDisp++;
				if(pcdspHisReg[*pDisp] < 0xffff)
				{
					pcdspHisReg[*pDisp]++;
				}
				pDisp++;
				if(pcdspHisReg[*pDisp] < 0xffff)
				{
					pcdspHisReg[*pDisp]++;
				}
				pDisp++;
				if(pcdspHisReg[*pDisp] < 0xffff)
				{
					pcdspHisReg[*pDisp]++;
				}
				pDisp++;
				if(pcdspHisReg[*pDisp] < 0xffff)
				{
					pcdspHisReg[*pDisp]++;
				}
				pDisp++;
				if(pcdspHisReg[*pDisp] < 0xffff)
				{
					pcdspHisReg[*pDisp]++;
				}
				pDisp++;
				if(pcdspHisReg[*pDisp] < 0xffff)
				{
					pcdspHisReg[*pDisp]++;
				}
			}
		}
	}

	osdFrameDrawX(x1, y1, x2-x1, y2-y1, (color>>4)&0x0f, 3);
	x1 += 3;
	y1 += 3;
	x2 -= 3;
	y2 -= 3;

	for (i=0; i<(x2-x1); i++)
	{
		h = pcdspHisReg[i*256/(x2-x1)]*(y2-y1)/countW*countH/0x200;
		if (h > (y2-y1))
		{
			h = y2-y1;
		}
		if (h < (y2-y1))
		{
			osdLineDraw(x1+i, y1, x1+i, y2-h, color&0x0f);
		}
		osdLineDraw(x1+i, y2, x1+i, y2-h, (color>>4)&0x0f);
	}
}
/**************************************************************************
 *  Function Name: osdButtonDraw                                          *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
void osdButtonDraw(SINT32 x, SINT32 y, UINT8 *pfont, SINT32 color, UINT8 str[], UINT8 sts)
{
#if 0	
	SINT32 x1, y1, x2, y2;

	x1 = x;
	y1 = y;
	x2 = x1+pfont[2]*strlen(str)+5-1;
	y2 = y1+pfont[3]+5-1;

	if(sts==0)		/* released */
	{

		osdFrameDraw(x1-2, y1-2, x2-x1+4, y2-y1+4, 0x0f);
		/*	
		osdLineDraw(x1, y2, x2, y2, 0x0f);
		osdLineDraw(x2, y1, x2, y2, 0x0f);
		*/

		/*x2--;
		y2--;*/

		osdLineDraw(x1, y1, x1, y2, 0x0d);
		osdLineDraw(x1, y1, x2, y1, 0x0d);

		osdLineDraw(x1, y2, x2, y2, 0x0e);
		osdLineDraw(x2, y1, x2, y2, 0x0e);

		x1++;
		y1++;
		x2--;
		y2--;

		osdLineDraw(x1, y1, x1, y2, 0x0d);
		osdLineDraw(x1, y1, x2, y1, 0x0d);

		osdLineDraw(x1, y2, x2, y2, 0x0e);
		osdLineDraw(x2, y1, x2, y2, 0x0e);

		x1++;
		y1++;

		osdStrDisp(x1, y1, pfont, color, str);
	}
	else			/* pressed */
	{
		/*osdLineDraw(x1, y1, x2, y1, 0x0f);
		osdLineDraw(x1, y1, x1, y2, 0x0f);*/
		osdFrameDraw(x1-2, y1-2, x2-x1+4, y2-y1+4, 0x0f);

		/*x1++;
		y1++;*/

		osdLineDraw(x1, y1, x1, y2, 0x0e);
		osdLineDraw(x1, y1, x2, y1, 0x0e);

		osdLineDraw(x1, y2, x2, y2, 0x0d);
		osdLineDraw(x2, y1, x2, y2, 0x0d);

		x1++;
		y1++;
		x2--;
		y2--;

		osdLineDraw(x1, y1, x1, y2, 0x0e);
		osdLineDraw(x1, y1, x2, y1, 0x0e);

		osdLineDraw(x1, y2, x2, y2, 0x0d);
		osdLineDraw(x2, y1, x2, y2, 0x0d);

		x1++;
		y1++;

		osdStrDisp(x1, y1, pfont, color, str);
	}
#endif
}
/**************************************************************************
 *  Function Name: osdHProgressDraw                                       *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
 **************************************************************************/
void osdHProgressDraw(UINT32 start, UINT32 end, UINT32 pos, UINT32 color)
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
	y = osdH - w - 3 - 20+8;  //tzw 10/10/10
	l = osdW - x - 64;

	osdStrDisp(x-w, y, UserIcon16x16, color&0xf0, "\x03");
	osdStrDisp(x+l, y, UserIcon16x16, color&0xf0, "\x04");

	osdFrameDrawX(x, y, l, w, color&0x0f, 3);
	x += 3;
	y += 3;
	w -= 6;
	l -= 6;
	osdBarDraw(x, y, l, w, 0x00);

	stepLgh = l / (end-start+1);

	if((avPlayNow==OBJ_MP3)&&(mp3AppStatusGet() == MP3_PENDING))
	{
	 	/*osdBarDraw(x+l-16, y, 16, w, color>>4);*/
	}
	else
	 {
	if (stepLgh < 16)
	{
		osdBarDraw(x+(l-(16-stepLgh))*(pos-start)/(end-start+1), y, 16, w, color>>4);
	}
	else
	{
		osdBarDraw(x+l*(pos-start)/(end-start+1), y, stepLgh, w, color>>4);
	}
	}
}

/**************************************************************************
 *  Function Name: osdVProgressDraw                                       *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
**************************************************************************/
void osdVProgressDraw(UINT32 start, UINT32 end, UINT32 pos, UINT32 color)
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
	x = osdW-w-3;
	y = 32;
	l = osdH-y-32;

	osdStrDisp(x, y-16, UserIcon16x16, color&0xf0, "\x01");
	osdStrDisp(x, y+ l, UserIcon16x16, color&0xf0, "\x02");

	osdFrameDrawX(x, y, w, l, color&0x0f, 3);
	x += 3;
	y += 3;
	w -= 6;
	l -= 6;
	osdBarDraw(x, y, w, l, 0x00);

	stepLgh = l / (end-start+1);
	if (stepLgh < 16)
	{
		osdBarDraw(x, y+(l-16)-(l-(16-stepLgh))*(pos-start)/(end-start+1), w, 16, color>>4);
	}
	else
	{
		osdBarDraw(x, y+(l-stepLgh)-l*(pos-start)/(end-start+1), w, stepLgh, color>>4);
	}
}

//**************************************************************************
UINT32 uiDispBuffFill(UINT32 patt)
{
	UINT32 dispAddr;
	UINT32 dispW, dispH;

	hwDispPreviewSizeGet(&dispW, &dispH);
	sysgMemDispAddrGet(&dispAddr);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(dispAddr);

	pubFill((UINT8 *)(0xAC000000 | dispAddr*2), dispW*2, dispH,
		0, 0, dispW*2, dispH, patt);/*eric@5/9/2003*/

	return SUCCESS;
}

/**************************************************************************
 *  Function Name: sample_startupscreen                                   *
 *  Purposes:                                                             *
 *  Descriptions:
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
void openScreen(UINT32 PatternFlag)
{

     UINT32 dispW,dispH;
     UINT32 dispAddr;
     UINT32 page;
     UINT8* pbuf;

     hwDispPreviewSizeGet(&dispW, &dispH);
     sysgMemDispAddrGet(&dispAddr);
     pbuf = (UINT8 *)((0xAC000000 | (UINT32)dispAddr*2));

	 hwFrmCnvModeSet(0);
     hwCammodeSet(5);
     hwCdspDataSource(1);
	 switch(PatternFlag)
     {
          case PatternDefault:
          	   pbDramImgPlay((UINT8 *)startup,sizeof(startup),dispAddr,dispW,dispH);
			   break;
			   
          case PatternCurrent:
		  		printf("dfdsf");
               if (dispW*dispH*2%512 == 0)
                    page = dispW*dispH*2/512;
               else
                    page=dispW*dispH*2/512 +1;
          		nandRsvBlkRead(SECTOR_OF_START_PIC, page, pbuf);		// David 12/28/2005
	     	   break;

		 case PatternCustom:
			 	nandRsvBlkErase(SECTOR_OF_START_PIC);
		 		break;
			#if 0				
          	default:
  		       ui8StartScreenFlag = 1 ; //tzw 05-10-03	
               pbDramImgPlay(startup,sizeof(startup),dispAddr,dispW,dispH);
			   break;
			#endif			   
     }
  	 hwWait(0,100);
  	 OpenLcdDisplay(1);	 
      hwWait(0, 50);              ///:
}

void buttonAudio(UINT32 idx)
{
    gpioConfig(GPIO_MUTE, 1);
    gpioSet(GPIO_MUTE, 0);
	if (sPara.uiBeepFlag == BeepOn)
	{
		#if 1     //Danny   05-06-05
		hwAudAC97RegWrite(0x2c, 48000);		/* 8K Hz*/
		hwAudAC97RegWrite(0x2e, 48000);		/* 8K Hz*/
		hwAudAC97RegWrite(0x32, 48000);		/* 8K Hz*/
		#endif
		if(SpeakFlag)
		{	// Danny--05/06/06
 			pbSetVolume(ws9712_OUT2, 8);
		}
		else
		{
			pbSetVolume(ws9712_HP, 2); //tzw add for bug 14 060420
		}
		
		switch(idx)
		{
			case 1:
				audRiffDramOneShotPlay(voice1, sizeof(voice1), 1);
				break;
			case 2:
				audRiffDramOneShotPlay(voice1, sizeof(voice1), 1);
				break;
			case 3:
				audRiffDramOneShotPlay(voice1, sizeof(voice1), 1);
				break;
				
			default:
				break;	

		}

		if(SpeakFlag)
		{
			pbSetVolume(ws9712_OUT2, mp3Vol);
		}
		else
		{
			pbSetVolume(ws9712_HP, mp3Vol);
		}
	}
    gpioConfig(GPIO_MUTE, 1);
    gpioSet(GPIO_MUTE, 1);
}
	
//**************************************************************************
UINT32 pbImagePaste(UINT32 imgAddr, UINT32 imgSize, UINT32 dstX, UINT32 dstY)
{
    UINT32 displayPhyAddr;
    UINT32 decompAddr;
    UINT32 err;
    exifImageInfo_t *pInfo=0;		
    exifAttrDesc_t *pattr;    
	UINT32 w;
	UINT32 h;
    
	err = exifMainDecode((UINT8 *)imgAddr, imgSize, &pattr);
	pInfo = &pattr->main;
    
 	sysgMemFreeAddrGet(APP_STILL_PB, &decompAddr);
	printf("Decomp Addr = 0x%x, w=%d, h=%d\n", 0x8c000000 | (decompAddr*2), pInfo->w, pInfo->h);
	if ((err = pbImageDeCompress(pInfo, decompAddr)) != 0) 
    {
		return err;
	}    

    sysgMemDispAddrGet(&displayPhyAddr);

	w = (pInfo->w + 0x0F) & 0xFF0;
	h = pInfo->h;
    hwImgCopyDo(decompAddr, w, h, 0, 0, 
        pInfo->w, pInfo->h, displayPhyAddr, 320, 240,
        dstX, dstY, 0);

    return SUCCESS;    
}

//**************************************************************************
void volSet(void)
{
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
	UINT8	temp;
	wm8751LineOut(SpeakFlag);

	#if 1
	//set for mute
	if(avPlayNow == OBJ_NONE)
	{
		wm8751SoundEnable (0, 0);
	}
	//David 01/07/2005	
	else
	#endif	
	if(SpeakFlag)
	{
		pbSet8751Volume(ws9712_OUT2,mp3Vol);
	}
	else
	{
		for (temp = 0; temp <= mp3Vol; temp++)
		{
			pbSet8751Volume(ws9712_HP,temp);
//			osTimeDly(10);
		}
	}
#endif	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: smcDiskScan                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void smcDiskScan(UINT32 opt)
{
	if (smcCmdReliableTest(DRIVE_NAND, opt) == SUCCESS)
	{
		printf("NAND flash test OK\n");
	}
	else
	{
		printf("NAND flash test Bad\n");
	}
}
/**************************************************************************
 *                                                                        *
 *  Function Name: osdFWBuffChange                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void osdFWBuffChange(void)
{
	if (gOSDBuffCnt == 0)
	{
		gOSDBuffCnt++;
		if (gOSDBuffFlag & 0x01)
		{
			gOSDBuffFlag &= ~0x01;
			osdAddrSet(0xac000000 + 2*gOSD_A);
		}
		else
		{
			gOSDBuffFlag |= 0x01;
			osdAddrSet(0xac000000 + 2*gOSD_B);
		}
		osdClearScreen(0x00);
	}
	else
	{
		gOSDBuffCnt++;
		if (gOSDBuffCnt == 0)
		{
			printf("gOSD FW Buffer Error!");
		}
	}
}

/********************************************
*** fqdao_modify for bug TVOut osdcleanScreen  ********
***  @  2006.5.6 @ ******************************
*********************************************/
void  OsdFWAddrSet()
{
     		if (gOSDBuffFlag & 0x01)
		{
			
			osdAddrSet(0xac000000 + 2*gOSD_B);
		}
		else
		{
			
			osdAddrSet(0xac000000 + 2*gOSD_A);
		}

}
/********************************************
*** fqdao_modify for bug TVOut osdcleanScreen  ********
***  @  2006.5.6 @ ******************************
*********************************************/
void  OsdHWAddrSet()
{
     		if (gOSDBuffFlag & 0x02)
		{
			
			hwOsdAddrSet(gOSD_B);
		}
		else
		{
			
			hwOsdAddrSet(gOSD_A);
		}
		
}
/**************************************************************************
 *                                                                        *
 *  Function Name: osdHWBuffChange                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void osdHWBuffChange(void)
{
	if (gOSDBuffCnt == 1)
	{
		gOSDBuffCnt--;
		if (gOSDBuffFlag & 0x02)
		{
			gOSDBuffFlag &= ~0x02;
			hwOsdAddrSet(gOSD_A);
		}
		else
		{
			gOSDBuffFlag |= 0x02;
			hwOsdAddrSet(gOSD_B);
		}
	}
	else
	{
		gOSDBuffCnt--;
		if (gOSDBuffCnt == 0xff)
		{
			printf("gOSD HW Buffer Error!");
		}
	}
}
//**************************************************************************
#if 1
UINT32 osstatus;
UINT32 usbSuspendCallback(void)
{
	UINT8 reg0;
	globalReg_t *pglobalReg;
       sdramReg_t *psdramReg;

	osTimeDly(10);
	ENTER_CRITICAL(osstatus);

	pglobalReg = (globalReg_t *)0xb0000000;
       psdramReg = (sdramReg_t *)(0xb0000000);
	reg0 = 1;
	reg0 = reg0 | 0x01;
	pglobalReg->suspend = 0;/*clear suspend */
	pglobalReg->resumeLen = 0x05;
	psdramReg->srefresh = 0;/*SelfRefresh;*/

	hwUiResumeEnableSet(1);
	hwUiResumeFlagSet(0);

	return 0;
}
//**************************************************************************
UINT32 usbResumeCallback(void)
{
    hwUiResumeEnableSet(0);
	hwUiResumeFlagSet(0);

    osTimeDly(10);
    EXIT_CRITICAL(osstatus);

	return 0;
}
#endif
//**************************************************************************
void DateDisplay()
{
	UINT32	osdW, osdH;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;


	struct	tm	*ptime;
	UINT8 dateStr[21];
    UINT16 time;
	timeGet(&ptime);
    time=ptime->tm_year+1900;
	
	ltoa((UINT32)ptime->tm_mon, &dateStr[0], 10, 2);
	strcat(dateStr, ".");
	ltoa((UINT32)ptime->tm_mday, &dateStr[3], 10, 2);
	strcat(dateStr, ".");
	sio_psprintf(&dateStr[6],"%4d",time);
	//ltoa((UINT32)ptime->tm_year, &dateStr[6], 10, 4);
	strcat(dateStr, "  ");
   
	ltoa((UINT32)ptime->tm_hour, &dateStr[12], 10, 2);
	strcat(dateStr, ":");
	ltoa((UINT32)ptime->tm_min, &dateStr[15], 10, 2);
	strcat(dateStr, ":");
	ltoa((UINT32)ptime->tm_sec, &dateStr[18], 10, 2);

	dateStr[20] = '\0';
	if(snapAV == OBJ_ASF)
		osdStrDisp (osdW-320, osdH-20, UserFont10x20, 0x30, dateStr); 
		osdStrDisp (60, osdH-40, UserFont10x20, 0x30, dateStr); 
}
//**************************************************************************
void Playedtime()
{

	xdcfTime_t  PlayedTime;
	UINT8  	str[25];
	PlayedTime = fileattr.time;
	

	PlayedTime.sec +=  avPlayedTime.sec;
	if(  PlayedTime.sec > 59 ) 
	{
		PlayedTime.sec -= 60;
		PlayedTime.min++;
	}
	
	PlayedTime.min += avPlayedTime.min;
	
	if( PlayedTime.min > 59 )
	{
		PlayedTime.min -= 60;
		PlayedTime.hour++;
	}
	
	PlayedTime.hour += avPlayedTime.hour;
	
	if( PlayedTime.hour > 23 )
	{
		PlayedTime.hour -= 24;
		PlayedTime.day++;
	}

	  if(PlayedTime.mon==2)
        {
                if((!(PlayedTime.year % 4) && PlayedTime.year % 100 ) || !(PlayedTime.year % 400))
                {
                        if(PlayedTime.day > 29)
                        {
                                PlayedTime.day -=29;
					PlayedTime.mon++;
                        }
                }
                else
                {
                        if(PlayedTime.day>28)
                        {
                                PlayedTime.day -=28;
					PlayedTime.mon++;
                        }
                }
        }
        else if((PlayedTime.mon==4)||(PlayedTime.mon==6)||(PlayedTime.mon==9)||(PlayedTime.mon==11))
        {
                if(PlayedTime.day>30)
                {
                        PlayedTime.day -=30;
			     PlayedTime.mon++;
                }
        }
        else
        {
                if(PlayedTime.day>31)
                {
                        PlayedTime.day -=31;
			     PlayedTime.mon++;
                }
        }
	if( PlayedTime.mon > 12 )
	{
		PlayedTime.mon -=12;
		PlayedTime.year ++;
	}

	//07/27/2005  Danny
	sio_psprintf(str, "%02d.%02d.%04d", PlayedTime.mon, PlayedTime.day, PlayedTime.year);
	osdStrDisp(0, 0, UserFont10x20, 0x30, str);
	sio_psprintf(str, "%02d:%02d:%02d", PlayedTime.hour, PlayedTime.min, PlayedTime.sec);
	osdStrDisp(0, 20, UserFont10x20, 0x30, str);

}
//**************************************************************************
#define TV_DETECT 1
#if TV_DETECT
/**************************************************************************
 *  Function Name: uiTVPlugDet                                            *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
//wendy@050307 update  for tv  detect
void uiTVPlugDet(void)
{//Gu@ 2004/08/03, 17:56
#if 0
	gpioConfig(GPIO_TV_DET, 0);
	hwGenGpioIrqInit();
	if (gpioGet(GPIO_TV_DET))
	{
		if (sPara.TvoutFlag==TVNTSC)
		panelSelect(PANEL_TYPE_2);/*Gu@ 2004/08/03, 17:56*/
		else if (sPara.TvoutFlag==TVPAL)
		panelSelect(PANEL_TYPE_3);/*Gu@ 2004/08/03, 17:56*/
		else 
		panelSelect(PANEL_TYPE_1);/*Gu@ 2004/08/03, 17:56*/
		gpioIrqReg(GPIO_TV_DET, uiTVPlugOut, FALL);
		gpioIrqRmv(GPIO_TV_DET, RISE);
	}
	else
	{
		gpioIrqReg(GPIO_TV_DET, uiTVPlugIn, RISE);
		gpioIrqRmv(GPIO_TV_DET, FALL);
	}
#endif	
		panelSelect(PANEL_TYPE_1);/*Gu@ 2004/08/03, 17:56*/
}
/**************************************************************************
 *  Function Name: uiTVPlugIn                                             *
 *  Purposes:                                                             *
 *  Descriptions:                                                         *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *  See also:                                                             *
**************************************************************************/
//wendy@050307 update  for tv  detect
void uiTVPlugIn(void)
{ 
#if 0
	printf("TV In\n");
	if (sPara.TvoutFlag==TVNTSC)
	panelSelect(PANEL_TYPE_2);/*Gu@ 2004/08/03, 17:56*/
	else if (sPara.TvoutFlag==TVPAL)
	panelSelect(PANEL_TYPE_3);/*Gu@ 2004/08/03, 17:56*/
	else 
	panelSelect(PANEL_TYPE_1);/*Gu@ 2004/08/03, 17:56*/
	gpioIrqReg(GPIO_TV_DET, uiTVPlugOut, FALL);
	gpioIrqRmv(GPIO_TV_DET, RISE);
	uiTVStatus = 1;
	Buzzer(5);

#endif	
}
/**************************************************************************
 *                                                                        *
 *  Function Name: uiTVPlugOut                                            *
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
 //wendy@050307 update  for tv  detect
void  uiTVPlugOut(void)
{
	printf("TV Out\n");

	panelSelect(PANEL_TYPE_1/*sPara.VideoModeFlag*/);
#if 0	
//Gu@ 2004/08/03, 17:57
	gpioIrqReg(GPIO_TV_DET, uiTVPlugIn, RISE);
	gpioIrqRmv(GPIO_TV_DET, FALL);
	uiTVStatus = 0;
	Buzzer(5); 
#endif	
}
#endif
UINT8 IsSDProtect()
{
		UINT32 storage;
		xdcfActiveDevIdGet(&storage);
						
		if((storage != DRIVE_NAND) && isCardProtect())
		{	
			return 1;		
		}

		return 0;

}

//**************************************************************************
UINT8    F_CheckLanguage(UINT8  LanguageCode)
{
	
	if( ((1<<LanguageCode) & LANGUAGE_DEFAUT_SET) !=0x00)
	{
		return  SUCCESS;
	}
	
	return FAIL ;
	
}
//**************************************************************************
UINT32 F_STRCTRL(UINT8 *str )
{
	if(sPara.language == LanguageRussian)
	{
		return (strlen(str)/2);
	}		
	else
	{
		return strlen(str);
	}		
}
//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************

