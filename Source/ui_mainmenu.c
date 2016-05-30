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
//#define          RS_232_DBG
#include                        <time.h>
#include                        <stdlib.h>
#include                        <string.h>
#include                        "general.h"
#include                        "stdio.h"
#include                        "os_api.h"
#include                        "ui_main.h"
#include                        "hw_disp.h"
#include                        "hw_disp_ext.h"
#include                        "osd_common.h"
#include                        "osd_api.h"
#include                        "xiaoao.h"
#include                        "panel.h"
#include                        "vfs_api.h"
#include                        "dcf_api.h"
#include                        "hw_sdram.h"
#include                        "hw_cammode.h"
#include                        "hw_cdsp.h"
#include                        "sysg_api.h"
#include                        "gpio.h"
#include                        "otg_api.h"
#include                        "UiStrings.h"
#include                        "sysregs.h"
#include                        "mainmenu.h"
#include                        "extern.h"
#include "timer.h"
#include "mp4_scr.h"                    
//**************************************************************************
#define         MAX_ICON                9

UINT8           icon_idx                = 1;
UINT16          sub_menu                = 0;
UINT32          file_filter     = FILTER_PHOTO;
extern          UINT32                  gPB;
//**************************************************************************
UINT32 MAIN_MENU_TEXT[][13]=    
{                   // 文字//  文字 LANGUAGES //tzw0918	
/*英文,简体中文,繁体中文,西班牙,俄文, 德文, 法文, 意大利, 葡萄牙, 韩文*/	
 {mainE1,mainC1,mainT1,mainS1,mainR1,mainE1,mainE1,mainE1,mainP01,mainK1,sizeof(mainE1),104,184}, 
 {mainE2,mainC2,mainT2,mainS2,mainR2,mainE2,mainE2,mainE2,mainP02,mainK2,sizeof(mainE2),104,184}, 
 {mainE3,mainC3,mainT3,mainS3,mainR3,mainE3,mainE3,mainI3,mainP03,mainK3,sizeof(mainE3),104,184}, 
 {mainE4,mainC4,mainT4,mainS4,mainR4,mainE4,mainE4,mainI4,mainP04,mainK4,sizeof(mainE4),104,184}, 
 {mainE5,mainC5,mainT5,mainS5,mainR5,mainE5,mainE5,mainI5,mainP05,mainK5,sizeof(mainE5),104,184}, 
 {mainE6,mainC6,mainT6,mainS6,mainR6,mainE6,mainE6,mainI6,mainP06,mainK6,sizeof(mainE6),104,184}, 
 {mainE7,mainC7,mainT7,mainS7,mainR7,mainE7,mainE7,mainI7,mainP07,mainK7,sizeof(mainE7),104,184}, 
 {mainE8,mainC8,mainT8,mainS8,mainR8,mainE8,mainE8,mainI8,mainP08,mainK8,sizeof(mainE8),104,184}, 
 {mainE9,mainC9,mainT9,mainS9,mainR9,mainE9,mainE9,mainI9,mainP09,mainK9,sizeof(mainE9),104,184}, 
};	

#define    MAINMAP_FLAG    9 
UINT32 MAIN_MENU_MAP[][4]=      
{
        {main1,sizeof(main1),18,52}, // 选中时图标
        {main2,sizeof(main2),18,52},
        {main3,sizeof(main3),18,52},
        {main4,sizeof(main4),18,52},
        {main5,sizeof(main5),18,52},
        {main6,sizeof(main6),18,52},
        {main7,sizeof(main7),18,52},
        {main8,sizeof(main8),18,52},
        {main9,sizeof(main9),18,52},

        {MainMap,sizeof(MainMap),0,0},  // 9  MAINMAP_FLAG
        {PlayBack,sizeof(PlayBack),0,0},
        {PlayB_I1,sizeof(PlayB_I1),0,0}, //  11 MP4  Ico
        {PlayB_S1,sizeof(PlayB_S1),0,0},        //文件选中时的图标
        {PlayB_C1,sizeof(PlayB_C1),0,0},        //文件选中时的图标
        {Photo_I1,sizeof(Photo_I1),0,0}, //  14 photo  Ico
};
UINT32 MAIN_MENU_MOVE[][8]=
{
        {maind10,maind11,maind12,maind10,sizeof(maind12),116,62}, // 选中时动画
        {maind20,maind21,maind22,maind20,sizeof(maind22),116,62},
        {maind31,maind32,maind33,maind34,sizeof(maind32),116,62},
        {maind40,maind41,maind42,maind40,sizeof(maind42),116,62},
        {maind50,maind51,maind52,maind50,sizeof(maind52),116,62},
        {maind60,maind61,maind62,maind60,sizeof(maind62),116,62},
        {maind70,maind71,maind72,maind73,sizeof(maind72),116,62},
        {maind80,maind81,maind82,maind80,sizeof(maind82),116,62},
        {maind90,maind91,maind92,maind91,sizeof(maind92),116,62},
};



//************************************************************************* 
//*****************************************************************************************************************************
// 显示主菜单文字
void    pbMainMenu_Text(UINT8 idx)
{
        pbImagePaste(MAIN_MENU_TEXT[idx][sPara.language],MAIN_MENU_TEXT[idx][10],MAIN_MENU_TEXT[idx][11],MAIN_MENU_TEXT[idx][12]);
}
//*****************************************************************************************************************************
//显示选中图标
UINT8 ui_OpenMoveFlag = 0  ;
UINT8 ui_OpenMoveIdx = 0 ;
void    pbMainMenu_Move(UINT8 idx)
{
          pbImagePaste(MAIN_MENU_MOVE[idx][ui_OpenMoveFlag],MAIN_MENU_MOVE[idx][4],MAIN_MENU_MOVE[idx][5],MAIN_MENU_MOVE[idx][6]);
}
// 显示选取中图标的动画
UINT8  MoveCount[]= {3,3,4,3,3,3,4,3,3} ;  // 各种主菜单动画数量
void   pbOpenMove(UINT8 idx )
{
        if(ui_OpenMoveFlag < MoveCount[ui_OpenMoveIdx]- 1)
        {
                ui_OpenMoveFlag++ ;
        }
        else 
        {
                ui_OpenMoveFlag = 0 ;
        }       
        pbImagePaste(MAIN_MENU_MOVE[ui_OpenMoveIdx][ui_OpenMoveFlag],MAIN_MENU_MOVE[ui_OpenMoveIdx][4],MAIN_MENU_MOVE[ui_OpenMoveIdx][5],MAIN_MENU_MOVE[ui_OpenMoveIdx][6]);

}
//*****************************************************************************************************************************
//显示没有选中的图标
void    pbMainMenu_ICON(UINT8 idx)
{
        pbImagePaste(MAIN_MENU_MAP[idx][0],MAIN_MENU_MAP[idx][1],MAIN_MENU_MAP[idx][2],MAIN_MENU_MAP[idx][3]);
}
//*****************************************************************************************************************************
// 显示背景公用
#define   MAINBACK      10      // 背景的索引号
void    pbMainBack()
{
        pbImagePaste(MAIN_MENU_MAP[MAINBACK][0],MAIN_MENU_MAP[MAINBACK][1],MAIN_MENU_MAP[MAINBACK][2],MAIN_MENU_MAP[MAINBACK][3]);
}
//*****************************************************************************************************************************
// 显示MP4  图标
#define  Mp4ICO     11
void   pbDisplayMp4_Ico(UINT8 ui8_Idx ,UINT8  ui8_X,  UINT8  ui8_y)
{
        pbImagePaste(MAIN_MENU_MAP[ui8_Idx][0],MAIN_MENU_MAP[ui8_Idx][1],ui8_X,ui8_y);
}

//*****************************************************************************************************************************
// 显示MP4  选中图标
#define  FILESELICO     (Mp4ICO+1) 
#define  FILECLEICO   	(FILESELICO+1)

void   pbDisplaySelFile_Ico(UINT8  ui8_X,  UINT8  ui8_y)
{
        pbImagePaste(MAIN_MENU_MAP[FILESELICO][0],MAIN_MENU_MAP[FILESELICO][1],ui8_X,ui8_y);

}

//清除不选中的图标！
void   pbDisplayCleFile_Ico(UINT8  ui8_X,  UINT8  ui8_y)
{
        pbImagePaste(MAIN_MENU_MAP[FILECLEICO][0],MAIN_MENU_MAP[FILECLEICO][1],ui8_X,ui8_y);

}
//*****************************************************************************************************************************

void uiMainMenu(void)
{
    UINT32          key = UI_KEY_MODE_MAINMENU;
    UINT32          osdW, osdH;
    UINT32          dispW, dispH;
    UINT8           icon_front_idx;
    UINT32          uiCount = 0;
	UINT32          uiColor = 0;
				
    osdW = PANEL_1_OSD_W;
    osdH = PANEL_1_OSD_H;
    devInfo_t pdevInfo;

    UINT8   ui8ModeChange = 0 ;      //yyq
	
    icon_front_idx=0;
    F_SetSeriesKey(0x000f);
    while (( uiState & UI_MODE_MASK ) == UI_MAINMENU )
    {
		if (ui32NextState != uiState)
		{
			break;
		}
		
        switch(key)
        {
/*			
			case    UI_KEY_FUNC_A:				
            case    UI_KEY_DIR_UP:				
				uiColor++;
				pDispReg->oddRgb = uiColor;				
				printf("Rgb = %d\n", pDispReg->oddRgb);					
				break;
*/				
            case    UI_KEY_DIR_DOWN:
				gpioConfig(GPIO_LCD_POWER, 1);
				gpioSet(GPIO_LCD_POWER, 1);
				break;
				
			case	UI_KEY_DIR_UP:
				gpioConfig(GPIO_LCD_POWER, 1);
				gpioSet(GPIO_LCD_POWER, 0);				
				break;
				
            case    UI_KEY_FUNC_MENU:       
				break;                    
					
            case    UI_KEY_DIR_LEFT:       
				if(icon_idx > 1)                               
				{
				icon_idx--;
				}
				else 
				{
				    icon_idx = 9 ;
				} 

				ui_OpenMoveIdx = icon_idx;
				ui8ModeChange =1 ;
				tmrDisable(1);
				break;
                    
            case    UI_KEY_DIR_RIGHT:
				if(icon_idx < 9)                         
				{
					icon_idx++;
				}
				else 
				{
					icon_idx = 1;
				}
				ui8ModeChange =1 ;
				tmrDisable(1);
				ui_OpenMoveIdx = icon_idx;
				break;
				 
            case    UI_KEY_FUNC_OK:
				sub_menu = icon_idx;
				tmrDisable(1);				
				ui_ModeSwitch(sub_menu);
				break;
                    
			case    UI_KEY_MODE_MAINMENU:
                osTaskResume(osTaskFindByName("HISTOGRAM"));
                osTaskSuspend(osTaskFindByName("AAA"));
                osTimeDly(1);
                // 05-9-1  音频退出USB时无按键音                           
                mp3Disp = PB_DISP_NINE;
                pbDisp    = PB_DISP_FOUR;
                // 05-9-1
                sysgMemDispAddrGet(&gPB);
                hwDispPreviewSizeGet(&dispW, &dispH);
                hwDispImgSizeSet(dispW, dispH);
                hwPlayBufSizeSet(dispW, dispH);
                hwPlayBufAddrSet(gPB);
                hwFrmCnvModeSet(0);       /* Disable frame rate conversion */
                hwCammodeSet(5);          /* play back mode     */
                hwCdspDataSource(1);  /* CDSP Data source from DRAM     */
#if 0                                   
                if(sPara.OTGModeFlag == OTGPC)
                {
                        if(otgJackIdGet() == OTG_JACKID_HOST)
                        {
                                osTimeDly(100);
                                if(sPara.OTGModeFlag == OTGPC)
                                {
                                        otgJackIdSet(OTG_JACKID_SLAVE);
                                        otgHostEn(FALSE);
                                        usbChangeMode(sPara.USBModeFlag);
                                }
                        }
                }
#endif

				hwDispOsdEn(1);
				osdClearScreen(0);

				pbMainMenu_ICON(9); 
				hwDispPvEn(1);                                      

				sub_menu = 0;
				volSet();

				/* Paul@006/05/30 add start */
				gpioConfig(GPIO_MUTE, 1);
				gpioSet(GPIO_MUTE, 1);
				/* Paul@006/05/30 add end */				
				
				/* Paul@2006/01/24 add start */
				pbMainMenu_Move(icon_idx-1);
				pbMainMenu_Text(icon_idx-1);
				pbMainMenu_ICON(icon_idx-1);

				ui_OpenMoveFlag = 0;
				ui_OpenMoveIdx = icon_idx - 1 ;
				tmrEnable(1, 500, TMR_PERIODIC, pbOpenMove);

				/* Paul@2006/01/24 add end */
				break;
				
			case UI_KEY_ACCIDENT:
				tmrDisable(1);
				break; 
				
			default:
				break;
        }
        
        if(( uiState & UI_MODE_MASK ) == UI_MAINMENU)
        {
			if(ui8ModeChange)
			{
			    ui8ModeChange = 0;             			   
				ui_OpenMoveFlag = 0;
			    ui_OpenMoveIdx = icon_idx - 1 ;
			    tmrEnable(1, 500, TMR_PERIODIC, pbOpenMove);			   
			//	pbMainMenu_Move(icon_idx-1);
			    pbMainMenu_ICON(MAINMAP_FLAG); 
			    pbMainMenu_Text(icon_idx-1);  
			    pbMainMenu_ICON(icon_idx-1);
				/*
			    hwWait(0,260);
			    if((key != UI_KEY_FUNC_MODE) 
				&& (key != UI_KEY_MODE_UP))
			    {
		            UI_OSQFlush (uiKeyMsgQ);        // David 01/03/2006                     
			    } 
			    */
			}			                             
		}                       
		else
		{
		    break;
		}
		
		keyGet(&key);
    }

	tmrDisable(1);
    hwDispPipEn(0);

    uiState = ui32NextState;	
}

//*****************************************************************************************************************************
void    ui_ModeSwitch(UINT16 idx)
{
        switch(idx)
        {
                case 0x01:/*Gu@ 2004/06/18, 13:45 playback*/
	                    uiState = UI_MODE_MOVIE;                                                         
                        break;

                case 0x02:/*Gu@ 2004/06/18, 13:46 MP3&WMA*/
                        uiState = UI_MODE_PREVIEW;    
                        break;
						
                case 0x03:
                        uiState = UI_MODE_PLAYBACK;
                        file_filter = FILTER_VIDEO;
                        break;

                case 0x04:/*Gu@ record video */
                        uiState = UI_MODE_PHOTO;
                        file_filter = FILTER_PHOTO;
                        break;

                case 0x05:
                        uiState = UI_MODE_MP3;
                        file_filter = FILTER_AUDIO;
                        break;

                case 0x06:
                        uiState = UI_MODE_ACCESSORIES;
                        break;
                        
                case 0x07:
                        uiState = UI_MODE_SETUP;
                        break;

                case 0x08:
                        uiState = UI_MODE_RECORD;
                        break;
						
                case 0x09:
                        uiState = UI_MODE_EBOOK;
                        break;                          
        }

        ui32NextState = uiState;        /* Paul@2006/02/18-15:15 add */
        
}

