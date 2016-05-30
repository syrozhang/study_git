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
#include "os_api.h"
#include "ui_main.h"
#include "hw_disp.h"
#include "hw_sdram.h"
#include "hw_disp_ext.h"
#include "osd_common.h"
#include "osd_api.h"
#include "xiaoao.h"
#include "panel.h"
#include "Utility.h"
#include "otg_api.h"
#include "usb_api.h"
#include "Card.h"
#include "gpio.h"
#include "vfs_api.h"
#include "Audio_api.h"
#include "Timer.h"
#include "Dcf_api.h"
#include "Hw_cpu.h"
#include "ui_setup.h"

#include "extern.h"
#include "parameter.h"
#include "menu_capture.h"
struct	tm	*ptime;
struct	tm  stime;
UINT8	timeItem;

extern UINT32    cardExist,cardprotect; /*Julie*/
UINT8 icon_idx_setup = 0;
UINT8 sub_menu_setup = 0;
UINT8 Ckey = 0;	

UINT8 strLenHalf;
UINT32    key;

UINT8	choose;
UINT8	ICONOK = 1;
/*wendy add for chinese */
extern UINT8 HANZIFlag; //if 1;without ziku;0 with ziku
/*wendy add for chinese  */
extern UINT8 TVFlag;//wendy@050307 for tv detect
extern UINT8 SpeakFlag;//wendy for speak

extern void pipCopyPaste(UINT32 srcX, UINT32 srcY, UINT32 srcW, UINT32 srcH, UINT32 dstAddr, UINT32 dstW, UINT32 dstH, UINT32 dstX, UINT32 dstY);
extern void ws9712LineOut(UINT8 onoff);
extern UINT32 StorageSel(UINT32 sts);
extern void   asfAppPlayAbort(void);
extern void mp3AppAbort(void);
extern void panelSelect(UINT32);


UINT8 *SetGetString(UINT16 stringId)
{
    UINT8* str;
    UINT32 languageId = sPara.language;

    if (languageId >= LANGUAGES)
		languageId = LanguageEnglish;

    if(stringId >= MAX_ID) return 0;

    if(SetStrings[stringId][languageId][0]=='\0')
	    str = SetStrings[stringId][0];
	else
	    str = SetStrings[stringId][languageId];

	return str;
}

void Pb_Title(void)
{
	static UINT8   PbTilie_X ,PbTilie_Y ;
	
	switch(sPara.language)
	{
		case LanguageEnglish:
			PbTilie_X= 38 ;
			PbTilie_Y= 2 ;
			break ;
		case LanguageSpanish:
			PbTilie_X= 38 ;
			PbTilie_Y= 2 ;
			break ;
		case LanguageRussian:
			PbTilie_X= 18 ;
			PbTilie_Y= 2 ;
			break ;
		default :			/*ÖÐÎÄ*/
			PbTilie_X= 38 ;
			PbTilie_Y= 2 ;
			break ;
	}
	osdStrDisp(PbTilie_X, PbTilie_Y, UserFont10x20, 0xd0, Title[sPara.language]);
	
}

void  pb_subItem(UINT8 idx,UINT8 items)
{
	UINT8 i, j = 0, n;
	UINT8 space[13];	
	

	memset(space, ' ', 9);
	space[9] = '\0';	
//	osdBarDraw(0,26, 180,190,0x00);

	for(n = 0; n < items;  n++,j++)
	{	

		i = idx;
//		setup[i].x= 68 - strlen(setup[i].Item[sPara.language]) * 5 ;
//		setup[i].y= 50 + j*25;
//		S_Icon[i].x = 26;
//		S_Icon[i].y = 58 + j*37;		
//		osdStrDisp(setup[i].x, setup[i].y,UserFont10x20, 0xd0, setup[i].Item[sPara.language]);		
		SetICON[i][4] = 52 + j * 60;
		SetICON[i][5] = 174;
		pbImagePaste(SetICON[i][2], SetICON[i][3], SetICON[i][4],SetICON[i][5]);

		idx ++;
		if( idx >= 11)
			idx = 0;
		
	}	

}




 void  pb_Item(UINT8 idx, UINT8 front_idx)
{	
	UINT8 i = idx;
	UINT8 space[14];	
	memset(space, ' ', 12);
	space[12] = '\0';	



	
	if(!idx)
	{	
		osdFWBuffChange();		
		strLenHalf = 160 -strlen(setup[i].Item[sPara.language]) * 5;
		osdStrDisp(TITLEX , TITLEY,UserFont10x20, TITLES, "                       ");						
		osdStrDisp(TITLEX, TITLEY,UserFont10x20, TITLES, setup[i].Item[sPara.language]);				
		Pb_Title( );
		
		SystemTimeInit();
		SystemTimeDisp();
		osdHWBuffChange();			
	}

	if(idx + 1 == S_Version)
	{
	Pb_Options(setup[i].OptionStart, setup[i].OptionItems);
	}

}

// ¿ØÖÆÏÔÊ¾×ÓÏî
void Pb_Options(UINT16 idx, UINT16 items){
	UINT8 i;
	UINT8 space[13];	
	memset(space, ' ', 9);
	space[12] = '\0';	

	osdFWBuffChange();
	osdClearScreen(0);

	hisTimeDisp();
	hisIconDisp();
	
	i = icon_idx_setup;
	strLenHalf = 160 -strlen(setup[i].Item[sPara.language]) * 5;
	osdStrDisp(TITLEX, TITLEY, UserFont10x20, TITLES, "                       ");						
	osdStrDisp(TITLEX, TITLEY, UserFont10x20, TITLES, setup[i].Item[sPara.language]);				
	Pb_Title();

	for( i = idx; i < idx + items; i++)
		{
		strLenHalf =Setup_Options[i].x - (strlen(Setup_Options[i].Content[sPara.language]) * 5);
		osdStrDisp(ITEMX, Setup_Options[i].y, UserFont10x20, 0xf0, Setup_Options[i].Content[sPara.language]);
		}
	osdHWBuffChange();	
}


void Pb_OpenModeItems(UINT16 idx, UINT16 items){
	UINT8 i,j = 0;
	UINT8 space[13];	
	memset(space, ' ', 9);
	space[12] = '\0';	

	osdFWBuffChange();
	osdClearScreen(0);

	hisTimeDisp();
	hisIconDisp();
	
	i = icon_idx_setup;
	strLenHalf = 160 -strlen(setup[i].Item[sPara.language]) * 5;
	osdStrDisp(TITLEX, TITLEY, UserFont10x20, TITLES, "                       ");						
	osdStrDisp(TITLEX, TITLEY, UserFont10x20, TITLES, setup[i].Item[sPara.language]);				
	Pb_Title();
	
	for( i = idx; i < idx + items; i++,j++)
		{
		Setup_Options[i].x = ITEMX;
		Setup_Options[i].y = 70 + j * 30;
		strLenHalf =Setup_Options[i].x - (strlen(Setup_Options[i].Content[sPara.language]) * 5);
		osdStrDisp(ITEMX, Setup_Options[i].y, UserFont10x20, 0xf0, Setup_Options[i].Content[sPara.language]);
		}
	osdHWBuffChange();	
}

void  pb_ItemIcon(UINT8 idx)
{

//		osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*(i+1), UserIcon10x20, (ui8SelIdx != ui8SelOKIdx) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");										
}
void  pb_OptionItem(UINT8 idx, UINT8 front_idx)
{	
	UINT8 i = idx;
	UINT8 space[13];	
	memset(space, ' ', 11);
	space[11] = '\0';	
	UINT8 barx = 60;
	UINT8 barwidth = 196;

	osdBarDraw(barx, Setup_Options[front_idx].y-2,  barwidth, 24, 0x00);
	strLenHalf = Setup_Options[front_idx].x - strlen(Setup_Options[front_idx].Content[sPara.language])* 5;
	osdStrDisp(ITEMX, Setup_Options[front_idx].y,UserFont10x20, 0xf0, Setup_Options[front_idx].Content[sPara.language]);				
		
	osdBarDraw(barx, Setup_Options[i].y- 2,  barwidth, 24, 0x0e);	
	strLenHalf =Setup_Options[i].x - (strlen(Setup_Options[i].Content[sPara.language]) * 5);
	osdStrDisp(ITEMX, Setup_Options[i].y,UserFont10x20, 0xde, Setup_Options[i].Content[sPara.language]);				

//	if (ICONOK)
//	{
	if (idx == choose)
	{
		osdStrDisp(ITEMX - 10, Setup_Options[choose].y, UserIcon10x20, 0xbe, "\x56");
	}
	else if (ICONOK)
	{
		osdStrDisp(ITEMX - 10, Setup_Options[choose].y, UserIcon10x20, 0xb0, "\x56");		
			//	ICONOK = 0;
	}
//		else
//		osdStrDisp(ITEMX - 10, Setup_Options[choose].y, UserIcon10x20, 0xb0, " ");		
//	}
}

void SystemTimeInit (void)
{

		timeGet(&ptime);
		stime.tm_year=ptime->tm_year;
		stime.tm_mon=ptime->tm_mon;
		stime.tm_mday=ptime->tm_mday;
		stime.tm_hour=ptime->tm_hour;
		stime.tm_min=ptime->tm_min;
		stime.tm_sec=ptime->tm_sec;

}

void SystemTimeDisp ()
{
	UINT8 yearStr[4];
	UINT8 monStr[2];
	UINT8 dayStr[2];
	UINT8 hourStr[2];
	UINT8 minStr[2];
	UINT8 secStr[2];
	UINT32	osdW, osdH, x,y;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

//  x = osdW/4 + 5;
    x = 80;
	y = osdH/3;

	ltoa((UINT32)(stime.tm_year + 1900), yearStr, 10, 4);
	ltoa((UINT32)stime.tm_mon, monStr, 10, 2);
	ltoa((UINT32)stime.tm_mday, dayStr, 10, 2);
	ltoa((UINT32)stime.tm_hour, hourStr, 10, 2);
	ltoa((UINT32)stime.tm_min, minStr, 10, 2);
	ltoa((UINT32)stime.tm_sec, secStr, 10, 2);
	if(timeItem==1)
		osdStrDisp(x, y, UserFont, 0x30, yearStr);
	else
		osdStrDisp(x, y, UserFont, 0x00, yearStr);
	osdStrDisp(x+16*4, y, UserFont, 0x00, ".");
	if(timeItem==2)
		osdStrDisp(x+16*5, y, UserFont, 0x30, monStr);
	else
		osdStrDisp(x+16*5, y, UserFont, 0x00, monStr);
	osdStrDisp(x+16*7, y, UserFont, 0x00, ".");
	if(timeItem==3)
		osdStrDisp(x+16*8, y, UserFont, 0x30, dayStr);
	else
		osdStrDisp(x+16*8, y, UserFont, 0x00, dayStr);
	if(timeItem==4)
		osdStrDisp(x+16, y+32, UserFont, 0x30, hourStr);
	else
		osdStrDisp(x+16, y+32, UserFont, 0x00, hourStr);
	osdStrDisp(x+16+16*2, y+32, UserFont, 0x00, ":");
	if(timeItem==5)
		osdStrDisp(x+16+16*3, y+32,	UserFont, 0x30, minStr);
	else
		osdStrDisp(x+16+16*3, y+32, UserFont, 0x00, minStr);
	osdStrDisp(x+16+16*5, y+32, UserFont, 0x00, ":");
	if(timeItem==6)
		osdStrDisp(x+16+16*6, y+32, UserFont, 0x30, secStr);
	else
		osdStrDisp(x+16+16*6, y+32, UserFont, 0x00, secStr);
}


void TimeSet(void)
{
       UINT16   Year;   // 05-05-14-15:14
       UINT32   osdW, osdH;
        UINT32  x, y;
        UINT32    key = UI_KEY_MODE_SETUP;
        osdW = PANEL_1_OSD_W;
        osdH = PANEL_1_OSD_H;
        x = osdW/2-FOSDWidth*6;		//John update 051020
        y = osdH/2 + FOSDHeight;

        SystemTimeInit();
       while (sub_menu_setup == S_TimeSet)
       {
           switch(key)
            {
               case UI_KEY_MODE_SETUP:/*in*/
                        timeItem = 1;   
//				osdClearScreen(0);
//				 pbMenu(2,9);
//				 pbMenu_title(Time_title);
//			pb_Item(icon_idx_setup, front_icon_idx_setup);
                        break;
                case UI_KEY_DIR_UP:
                
                        if (timeItem == 1)
                        {                               
                                if(stime.tm_year>=200)//Davis:patch_2005/Apr/25
                                {
                                        stime.tm_year=0;
                                }
                                else 
                                        stime.tm_year++;
                        }
                        else if(timeItem == 2)
                        {
                                if(stime.tm_mon>11)
                                {
                                        stime.tm_mon=1;
                                }
                                else
                                        stime.tm_mon++;
                        }
                        else if(timeItem == 3)
                        {
                                             stime.tm_mday++;
                                if(stime.tm_mon==2)
                                {
                                        if(!(Year%4)&&Year%100||!(Year%400))
                                        {
                                                if(stime.tm_mday>29)//Davis:patch_2005/Apr/25
                                                {
                                                        stime.tm_mday=1;
                                                }
                                        }
                                        else
                                        {
                                                if(stime.tm_mday>28)//Davis:patch_2005/Apr/25
                                                {
                                                        stime.tm_mday=1;
                                                }
                                        }
                                }
                                else if((stime.tm_mon==4)||(stime.tm_mon==6)||(stime.tm_mon==9)||(stime.tm_mon==11))
                                {
                                        if(stime.tm_mday>30)//Davis:patch_2005/Apr/25
                                        {
                                                stime.tm_mday=1;
                                        }
                                }
                                else
                                {
                                        if(stime.tm_mday>31)//Davis:patch_2005/Apr/25
                                        {
                                                stime.tm_mday=1;
                                        }
                                }
                        }
                        else if(timeItem == 4)
                        {
                                if(stime.tm_hour==23)
                                {
                                        stime.tm_hour=0;
                                }
                                else
                                {
                                        stime.tm_hour++;
                                }
                        }
                        else if(timeItem == 5)
                        {
                                if(stime.tm_min==59)
                                {
                                        stime.tm_min=0;
                                }
                                else
                                {
                                        stime.tm_min++;
                                }
                        }
                        else if(timeItem == 6)
                        {
                                if(stime.tm_sec==59)
                                {
                                        stime.tm_sec=0;
                                }
                                else
                                {
                                        stime.tm_sec++;
                                }
                        }
                        break;
                case UI_KEY_DIR_DOWN:
                        if (timeItem == 1)
                        {
                                if(stime.tm_year==0)
                                {
                                        stime.tm_year=200;
                                }
                                else
                                {
                                        stime.tm_year--;
                                }
                        }
                        else if(timeItem == 2)
                        {
                                if(stime.tm_mon==1)
                                {
                                        stime.tm_mon=12;
                                }
                                else
                                {
                                        stime.tm_mon--;
                                }
                        }
                        else if(timeItem == 3)
                        {
                                if(stime.tm_mday==1)
                                {
                                        if(stime.tm_mon==2)
                                        {
                                                if(!(Year%4)&&Year%100||!(Year%400))
                                                {  
                                                              stime.tm_mday=29;
                                                }
                                               else
                                                {
                                                                stime.tm_mday=28;
                                                 }
                                        }
                                        else if((stime.tm_mon==4)||(stime.tm_mon==6)||(stime.tm_mon==9)||(stime.tm_mon==11))
                                        {
                                                stime.tm_mday=30;
                                        }
                                        else
                                        {
                                                stime.tm_mday=31;
                                        }
                                }
                                else
                                {
                                        stime.tm_mday--;
                                }
                        }
                        else if(timeItem == 4)
                        {
                                if(stime.tm_hour==0)
                                {
                                        stime.tm_hour=23;
                                }
                                else
                                {
                                        stime.tm_hour--;
                                }
                        }
                        else if(timeItem == 5)
                        {
                                if(stime.tm_min==0)
                                {
                                        stime.tm_min=59;
                                }
                                else
                                {
                                        stime.tm_min--;
                                }
                        }
                        else if(timeItem == 6)
                        {
                                if(stime.tm_sec==0)
                                {
                                        stime.tm_sec=59;
                                }
                                else
                                {
                                        stime.tm_sec--;
                                }
                        }
                        break;
                case UI_KEY_DIR_LEFT:

                        if (timeItem > 1)
                                timeItem--;

                        break;
                case UI_KEY_DIR_RIGHT:

                        if (timeItem < 6 && timeItem !=0)
                                timeItem++;

                        break;
                case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
                        if (timeItem ==0)
                            timeItem = 1;
                        else
                        {
                                ptime->tm_year=stime.tm_year;
                                ptime->tm_mon=stime.tm_mon;
                                ptime->tm_mday=stime.tm_mday;
                                ptime->tm_hour=stime.tm_hour;
                                ptime->tm_min=stime.tm_min;
                                ptime->tm_sec=stime.tm_sec;
                                timeSet(ptime);

  					strLenHalf = OFFSIDEX - strlen(SetGetString(Finished_s)) * 5;			
//					osdStrDisp(strLenHalf, OFFSIDEY + 110, UserFont10x20, 0x00, SetGetString(Finished_s)/*" OK "*/);//for bug 44

                                timeItem = 0;
                                osTimeDly(100);
                                return;
                        }
                        break;
                case UI_KEY_FUNC_MENU:
						menuReturn(UI_MAINMENU, 0);						
	 	        case UI_KEY_ACCIDENT:/* out/cancel */						
                      /* timeItem = 0;
                        SystemTimeInit();
                        SystemTimeDisp();*/
                        sub_menu_setup = 0;
					    return;
//                        continue;
                        break;

                default :
                         break;
         }
        /***05-05-14--Danny**/     
                       Year=stime.tm_year+1900;
        if((stime.tm_mon==4)||(stime.tm_mon==6)||(stime.tm_mon==9)||(stime.tm_mon==11))
        {
                         if(stime.tm_mday>30)
                                                stime.tm_mday=30;
        }
        else if(stime.tm_mon==2)
        {
                 if(!(Year%4)&&Year%100||!(Year%400))
                 {
                        if(stime.tm_mday>29)
                                        stime.tm_mday=29;
                 }
                 else
                 {
                        if(stime.tm_mday>28)
                                    stime.tm_mday=28;
                 }
         }
         /***05-05-14--Danny     **/
//         osdFWBuffChange();
//         ui_ShowSetupMenuInfo();
 
         SystemTimeDisp();
//         osdHWBuffChange();
         keyGet(&key);
      }
}

void SpeakerSet(void)
{

//	UINT32    key = UI_KEY_MODE_SETUP;
   	key = UI_KEY_MODE_SETUP;
	UINT8      sel = 1;
	UINT8      Front_sel=1;
	while (sub_menu_setup == S_KeyTones)
       {
            switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
		   	sel = sPara.uiBeepFlag;
			choose = setup[icon_idx_setup].OptionStart + sel - 1;
			Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);
			break;
		case UI_KEY_DIR_UP:
			sel = (sel == BeepOn) ? BeepOff : BeepOn;       //ÉèÖÃÎª¿ª	
			break;
		case UI_KEY_DIR_DOWN:
			sel = (sel ==  BeepOff)? BeepOn : BeepOff;          //ÉèÖÃÎª¹Øª	
			break;
			
		case UI_KEY_DIR_LEFT:
			sub_menu_setup = 0;
		
			continue;
			break;
//			return;

		case UI_KEY_DIR_RIGHT:
			sub_menu_setup = 0;
			continue;
			break;
//			return;
			
		case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
		
			sPara.uiBeepFlag = sel ;
			nandWriteParameters();
			
			sub_menu_setup = 0;
			strLenHalf = OFFSIDEX - strlen(SetGetString(Finished_s)) * 5;			
//			osdStrDisp(strLenHalf, OFFSIDEY + 110, UserFont10x20, 0x00, SetGetString(Finished_s)/*" OK "*/);//for bug 44
			osTimeDly(100);
			return;
		case UI_KEY_FUNC_MENU:
			menuReturn(UI_MAINMENU, 0);
		case UI_KEY_ACCIDENT:/* out/cancel */							
			sub_menu_setup = 0;
//			continue;
			return;
			break;
		}
		
		pb_OptionItem(setup[icon_idx_setup].OptionStart + sel - 1,setup[icon_idx_setup].OptionStart + Front_sel -1);
		
		Front_sel=sel;
		keyGet(&key);
	}

}

#if 0
void OpenModeset()
{


	key = UI_KEY_MODE_SETUP;
	UINT8      sel = 0;
	UINT8      Front_sel= 0;
	UINT8 	OpenKey = 22;
	UINT8	Offside;
	UINT8	step;
	while (sub_menu_setup == S_OpenMode)
       {
         switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
			sel = sPara.ui8Mode -1;
			choose = setup[icon_idx_setup].OptionStart + sel;
			Front_sel = sel;
			Offside = ((sel -2) > 0) ? sel -2 : 0;
			Pb_OpenModeItems(OpenKey + Offside, 3);				
			break;
		case UI_KEY_DIR_UP:
			sel = (sel <= 0) ? 0: sel - 1;       //ÉèÖÃÎª¿ª	
			break;
		case UI_KEY_DIR_DOWN:
			sel = (sel >=  7)? 7 :sel + 1;          //ÉèÖÃÎª¹Øª	
			break;

		case UI_KEY_DIR_LEFT:
			sub_menu_setup = 0;
			continue;
			break;

		case UI_KEY_DIR_RIGHT:
			sub_menu_setup = 0;
			continue;
			break;
			
		case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
			sPara.ui8Mode = sel + 1;
			printf("+++++++++ui8Mode = %d\n",sPara.ui8Mode);
			strLenHalf = OFFSIDEX - strlen(SetGetString(Finished_s)) * 5;			
//			osdStrDisp(strLenHalf, OFFSIDEY + 110, UserFont10x20, 0x00, SetGetString(Finished_s)/*" OK "*/);//for bug 44
			osTimeDly(100);
			return;
		case UI_KEY_FUNC_MENU:
		case UI_KEY_ACCIDENT:/* out/cancel */							
			sub_menu_setup = 0;
//			continue;
			menuReturn(UI_MAINMENU, 0);
			break;
		}
			
       		Offside = ((sel -2) > 0) ? sel -2 : 0;
			Pb_OpenModeItems(OpenKey + Offside,3);	

			step = OpenKey + Offside;
			if (((choose - step) <= 2) && 
				(choose >= step))
				ICONOK = 2;
			else
				ICONOK = 0;


			pb_OptionItem(setup[icon_idx_setup].OptionStart + sel,setup[icon_idx_setup].OptionStart + Front_sel);
			
		Front_sel=sel;
		keyGet(&key);
	}

}
#endif

#if 1
void Version(void)
{

	key = UI_KEY_MODE_SETUP;
	UINT8      sel = 0;
	UINT8	*Ver_num = (UINT8 *)versionID;	

	while (sub_menu_setup == S_Version)
       {
            switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
		   	sel = sPara.uiPatternFlag;			
			Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);
//			osdStrDisp(ITEMX, 110, UserFont10x20, 0xf0, Ver_num);
			break;
		case UI_KEY_DIR_UP:
			break;
			
		case UI_KEY_DIR_DOWN:
			break;

		case UI_KEY_DIR_LEFT:
			sub_menu_setup = 0;
			continue;
			break;

		case UI_KEY_DIR_RIGHT:
			sub_menu_setup = 0;
			continue;
			break;
			
		case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
			osTimeDly(100);
			return;
		case UI_KEY_FUNC_MENU:
			menuReturn(UI_MAINMENU, 0);			
		case UI_KEY_ACCIDENT:/* out/cancel */							
			sub_menu_setup = 0;
			return;
			break;
		}
	
		keyGet(&key);
	}

}
#endif

#if 1
void SplashSet()
{

	UINT8      sel = 0;
	UINT8      Front_sel= 0;

	key = UI_KEY_MODE_SETUP;
	
	while (sub_menu_setup == S_Splash)
       {
            switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
		   	sel = sPara.uiPatternFlag;
			if(sel == PatternCurrent)
				sel = 0;

			choose = setup[icon_idx_setup].OptionStart + sel;
			Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);
			break;
			
			case UI_KEY_DIR_UP:
				sel = (sel == 0) ? 1 : 0;       //ÉèÖÃÎª¿ª	
				break;
			case UI_KEY_DIR_DOWN:
				sel = (sel ==  1)? 0 : 1;          //ÉèÖÃÎª¹Øª	
				break;

			case UI_KEY_DIR_LEFT:
				sub_menu_setup = 0;
				continue;
				break;

			case UI_KEY_DIR_RIGHT:
				sub_menu_setup = 0;
				continue;
				break;
				
			case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
				sPara.uiPatternFlag = sel;
				nandWriteParameters();
				strLenHalf = OFFSIDEX - strlen(SetGetString(Finished_s)) * 5;			
//				osdStrDisp(strLenHalf, OFFSIDEY + 110, UserFont10x20, 0x00, SetGetString(Finished_s)/*" OK "*/);//for bug 44
				osTimeDly(100);
				return;
			case UI_KEY_FUNC_MENU:
				menuReturn(UI_MAINMENU, 0);				
			case UI_KEY_ACCIDENT:/* out/cancel */							
				sub_menu_setup = 0;
				return;
				break;
		}
		
		pb_OptionItem(setup[icon_idx_setup].OptionStart + sel,setup[icon_idx_setup].OptionStart + Front_sel);
		Front_sel=sel;
		keyGet(&key);
	}
}
#endif


extern UINT32 LightFreq;

void RayFreqSet(void)
{

	key = UI_KEY_MODE_SETUP;
	UINT8      sel = 0;
	UINT8      Front_sel = 0;
	while (sub_menu_setup == S_RayFreq)
       {
            switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
		 	sel = LightFreq;
			sel = sPara.frequenceFlag;
			choose = setup[icon_idx_setup].OptionStart + sel;
			Front_sel = sel;
			Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);
			break;
		case UI_KEY_DIR_UP:
			sel = (sel == 0) ? 1 : 0;        
			break;
		case UI_KEY_DIR_DOWN:
			sel = (sel ==  1)? 0 : 1;          
			break;

		case UI_KEY_DIR_LEFT:
			sub_menu_setup = 0;
			continue;
			break;

		case UI_KEY_DIR_RIGHT:
			sub_menu_setup = 0;
			continue;
			break;
			
		case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
			sPara.frequenceFlag = sel;
			aeTableChange(0);
			printf("the value of LightFreq %d",LightFreq);
			
//			osdStrDisp(OFFSIDEX - strlen(SetGetString(Finished_s)) * 5, OFFSIDEY + 110,UserFont10x20, 0x00, SetGetString(Finished_s)/*" OK "*/);//for bug 44
			osTimeDly(100);
			return;
		case UI_KEY_FUNC_MENU:
			menuReturn(UI_MAINMENU, 0);			
		case UI_KEY_ACCIDENT:/* out/cancel */							
			sub_menu_setup = 0;
//			continue;
			return;
			break;
		}
		
		pb_OptionItem(setup[icon_idx_setup].OptionStart + sel, setup[icon_idx_setup].OptionStart + Front_sel);
		Front_sel=sel;
		keyGet(&key);
	}

}

/* Paul@2006/12/25 - 13:17 add start */
/**
**/
UINT8 InitLanguageSet(void)
{
	UINT8 ui8SelIndex = 0;
	UINT8 i, j;

	j = 0;
	for (i=0; i<LANGUAGE_MAX_NUM; i++)
	{
		if (LANGUAGE_DEFAUT_SET & (1<<i))
		{
			LanSel[j] = i;
			if (sPara.language == i)
			{
				ui8SelIndex = j;
			}
			j++;
			if (j == totaLANGUAGE)
			{
				break;
			}
		}
	}

	return ui8SelIndex;
}
/* Paul@2006/12/25 - 13:17 add end */

/**
**/
//LANGUAGES
extern UINT8  *UserFont10x20;
extern UINT8* UserFont10x20_RES[9];

void DispMenuText(UINT16 TopIdx, UINT16 SelOk, UINT16 Sel,UINT16 StartSel,UINT8 stotalsel)
{
	UINT8 i = 0,idx_N = 0 ;
	UINT8 barx = 60;
	UINT8 barwidth = 196;


	osdFWBuffChange();
	osdClearScreen(0);

	hisTimeDisp();
	hisIconDisp();
	
	i = icon_idx_setup;
	strLenHalf = 160 -strlen(setup[i].Item[sPara.language]) * 5;
	osdStrDisp(TITLEX, TITLEY, UserFont10x20, 0x00, "                       ");						
	osdStrDisp(TITLEX, TITLEY, UserFont10x20, TITLES, setup[i].Item[sPara.language]);				
	Pb_Title();
	
	for( i = 0; i < stotalsel; i++)
	{
			idx_N = LanSel[TopIdx+i];

			if (Sel == TopIdx+i)
			{
				osdBarDraw(barx, 80 + i * 30 - 2,  barwidth, 24, 0x0e);					
				osdStrDisp(ITEMX, 80 + i * 30, UserFont10x20, 0xfe, Setup_Options [StartSel+idx_N].Content[sPara.language]);
			}
			else 
			{
				osdStrDisp(ITEMX, 80 + i * 30, UserFont10x20, 0xf0, Setup_Options [StartSel+idx_N].Content[sPara.language]);
			}	
	}

	if ((SelOk >= TopIdx) && 
		((SelOk - TopIdx) < stotalsel))
	{
		osdStrDisp (ITEMX - 10, 80 + (SelOk - TopIdx) * 30, UserIcon10x20, (SelOk == Sel)? 0xbe : 0xb0, "\x56");		
	}			
		
	osdHWBuffChange();	
}

/**
**/
void LanguageSet(void)
{
	UINT8	TopIdx;
	UINT8	step,i;
	UINT8 	SelOk;

//	UINT32  
	key = UI_KEY_MODE_SETUP;
	UINT8      sel = 0;
	UINT8      Front_sel=0;

	while (sub_menu_setup == S_Language)
       {
		
         switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
		   	//LANGUAGES
			sel = InitLanguageSet();
		//	sel = sPara.language;
			Front_sel=sel;			
	//		Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);

			SelOk = sel;
			// for bug 44 John @02/16/2006
			choose = setup[icon_idx_setup].OptionStart + sel;
			TopIdx = ((sel -2) > 0) ? sel -2 : 0;
			DispMenuText(TopIdx, SelOk, sel,S_Language_S,S_Language_I);
			break;
			
		case UI_KEY_DIR_UP:
			sel = (sel == LanguageEnglish)? 0: sel - 1;						
			if (sel < TopIdx) TopIdx = TopIdx - 1;
			break;
			
		case UI_KEY_DIR_DOWN://totaLANGUAGE
			sel =  (sel == totaLANGUAGE - 1) ? totaLANGUAGE-1 : sel + 1;
			if ((sel - TopIdx) > 2) TopIdx = TopIdx + 1;
			break;
			//John add 051005 end

		case UI_KEY_DIR_LEFT:
			sub_menu_setup = 0;
			continue;
			break;

		case UI_KEY_DIR_RIGHT:
			sub_menu_setup = 0;
			continue;
			break;


		case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
			//LANGUAGES
			for(i=0; i<totaLANGUAGE; i++) 
		   	{
				if(sel == i)
				{
					sPara.language  = LanSel[i];
					break;
				}
			}
			//LANGUAGES
			if( SelOk != sel )	
			{
				osdFontLoad_K();
			}
			UserFont10x20 =UserFont10x20_RES[sPara.language];
		//	sPara.language = sel;
			nandWriteParameters();
			SelOk = sel;
			choose = setup[icon_idx_setup].OptionStart + sel;
	//		Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);
			DispMenuText(TopIdx, SelOk, sel,S_Language_S,S_Language_I);
			break;
			
		case UI_KEY_FUNC_MENU:
			menuReturn(UI_MAINMENU, 0);			
		case UI_KEY_ACCIDENT:/* out/cancel */							
			sub_menu_setup = 0;
			continue;
			break;
		}
		DispMenuText(TopIdx, SelOk, sel,S_Language_S,S_Language_I);

	//	pb_OptionItem(setup[icon_idx_setup].OptionStart + sel, setup[icon_idx_setup].OptionStart + Front_sel);

		Front_sel=sel;
		

		keyGet(&key);
	}
}

#if 0
void LanguageSet(void)
{

//	UINT32  
	key = UI_KEY_MODE_SETUP;
	UINT8      sel = 0;
	UINT8      Front_sel=0;

	while (sub_menu_setup == S_Language)
       {
		
            switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
		   	sel = sPara.language;
			choose = setup[icon_idx_setup].OptionStart + sel;
			Front_sel=sel;
			
			//osdBarDraw(140, 30, 180, 190, 0x00);
			Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);

			break;
//John add 051005 start
			
		case UI_KEY_DIR_UP:
			sel = (sel == LanguageEnglish)?LanguageChinese_T : sel - 1;						
						break;					

		case UI_KEY_DIR_DOWN:
			sel =  (sel == LanguageChinese_T) ? LanguageEnglish : sel + 1;
						break;				
		
//John add 051005 end


		case UI_KEY_DIR_LEFT:
			sub_menu_setup = 0;
			continue;
			break;

		case UI_KEY_DIR_RIGHT:
			sub_menu_setup = 0;
			continue;
			break;


		case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
			 sPara.language = sel;
			nandWriteParameters();
			
			strLenHalf = OFFSIDEX - strlen(SetGetString(Finished_s)) * 5;							
//			osdStrDisp(strLenHalf, OFFSIDEY + 110, UserFont10x20, 0x00, SetGetString(Finished_s)/*" OK"*/);//for bug 44
			osTimeDly(100);
			return;
		case UI_KEY_FUNC_MENU:
			menuReturn(UI_MAINMENU, 0);
		case UI_KEY_ACCIDENT:/* out/cancel */							
			sub_menu_setup = 0;
//			continue;
			return;
			break;
		}
		
		pb_OptionItem(setup[icon_idx_setup].OptionStart + sel, setup[icon_idx_setup].OptionStart + Front_sel);

		Front_sel=sel;
		

		keyGet(&key);
	}
}
#endif
/*Ly 04/09/20 add start*/
extern UINT16 g_ui16PowerOffFlag; 	/* Paul@206/03/20-19:11 add */

UINT8 atoEnable = 1;
UINT32 atoCounter = 0;

void AutoTimeOff(void)
{
	UINT32 stopStep;
	if(sPara.SleepTimeFlag!=SleepTimeNone && atoEnable)
	{
		switch(sPara.SleepTimeFlag)
		{
		case SleepTime3m:
			stopStep=6*3;
			break;

		case SleepTime5m:
			stopStep=6*5;
			break;
			
		/*		
		case SleepTime15m:
			stopStep=6*15;
			break;
		case SleepTime30m:
			stopStep=6*30;
			break;
		*/
		default:
			stopStep=6*3;
		}

		if((avPlayNow != OBJ_NONE)
		 ||(snapAV != OBJ_NONE)
		 ||(uiUSBStatus > 0))
		{
			atoCounter = 0;
		}
		else
		{
		    atoCounter++;
		}
		
		if(atoCounter == stopStep)
		{
		  	printf(" Auto power off\n");
			g_ui16PowerOffFlag = 1;
		}		
	}
}

void AutoPowerOffSet(void)
{

	UINT8      sel = 0;
	UINT8      Front_sel=0;
	
	key = UI_KEY_MODE_SETUP;
	
	while (sub_menu_setup == S_AutoPwOff)
       {
         switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
		   	sel = sPara.SleepTimeFlag;
			choose = setup[icon_idx_setup].OptionStart + sel;
			Front_sel=sel;
			Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);		
			break;
			
			case UI_KEY_DIR_UP:
			sel = (sel == SleepTimeNone) ? SleepTime5m: sel -1;
			break;
			
			case UI_KEY_DIR_DOWN:
			sel = (sel == SleepTime5m) ? SleepTimeNone: sel +1;
			break;
			
			case UI_KEY_DIR_LEFT:
			sub_menu_setup = 0;
			continue;
			break;

			case UI_KEY_DIR_RIGHT:
			sub_menu_setup = 0;
			continue;
			break;	
			
			case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
			sPara.SleepTimeFlag = sel;
			atoCounter=0;
			atoEnable=1;
			/*tmrPeriodFuncSet(AutoTimeOff, 10000);*/
			nandWriteParameters();
			
			strLenHalf = OFFSIDEX - strlen(SetGetString(Finished_s)) * 5;			
//			osdStrDisp(strLenHalf, OFFSIDEY + 110, UserFont10x20, 0x00, SetGetString(Finished_s)/*" OK "*/);//for bug 44

			osTimeDly(100);
			return;
			
			case UI_KEY_FUNC_MENU:
    			 menuReturn(UI_MAINMENU, 0);				
			case UI_KEY_ACCIDENT:/* out/cancel */							
		         sub_menu_setup = 0;
				 return;
				 break;
		}
		pb_OptionItem(setup[icon_idx_setup].OptionStart + sel,setup[icon_idx_setup].OptionStart + Front_sel);
		Front_sel=sel;
		keyGet(&key);
}

}


#include "vfs_api.h"

void  storageFormat(UINT8 type)
{
 	UINT16 strLength;

    if(type==Card)
    {	
		if (cardExist== TRUE)
		{
			/*start modified by lhm for card protect detect*/
			if(cardprotect)
			{
	       	strLength = strlen(SetGetString(card_protect_s));//for bug 44
	       	//osdStrDisp(OFFSIDEX-strLength*(10/2), OFFSIDEY+60   , UserFont10x20, 0x30, SetGetString(card_protect_s));//for bug 44 2005-10-26 15:14
			MessageShow (ErrCardProtect);  // fqdao_add  06.6.1
			//hwWait(0, 1000);
			hwWait(0, 1000);
	            	osdStrDisp(OFFSIDEX-strLength*(10/2), OFFSIDEY+60  , UserFont10x20, 0x00, "            ");
			printf("ly_debug cardprotect!!\n");
			}
			else
			{
				 strLength = strlen(SetGetString(Format_Info1));
//				 osdStrDisp(OFFSIDEX-strLength*(10/2), OFFSIDEY + 80,UserFont10x20, 0x00, SetGetString(Format_Info1));    //Danny --05/05/24
				 strLength = strlen(SetGetString(Format_Info2));
//				 osdStrDisp(OFFSIDEX-strLength*(10/2), OFFSIDEY + 100,UserFont10x20, 0x00, SetGetString(Format_Info2));

				if (xdcfFormat(2, "FAT", FORMAT_ERASE_ALL) == SUCCESS)
				{

					xdcfInit(imageDirNameStr, DCNameStr, 0 /* | xDCF_CONFIG_DCF_ONLY*/);
					#if 1
						dirInit();
						#if 0		// Update  John 051103
							xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);
							if(FAIL != xdcfActiveDevIdSet(1))
							dirInit();
							xdcfActiveDevIdSet(2);
						#endif
					#endif

					strLength = strlen(SetGetString(Format_Finish_s));//for bug 44
//					osdStrDisp(160-strLength*(10/2), OFFSIDEY + 120, UserFont10x20, 0x00, SetGetString(Format_Finish_s));//for bug 44 2005-10-26 15:22 machiner
					MessageShow (MenuSuccess);  // tzw add for bug 7
				}
				else
				{
					strLength = strlen(SetGetString(Format_Failed));//for bug 44
					osdStrDisp(160-strLength*(10/2), OFFSIDEY + 120, UserFont10x20, 0x00, SetGetString(Format_Failed));//for bug 44 2005-10-26 15:22 machiner
					 
				}
				osTimeDly(100);
			}
		}
	   }
	   else
	  {
		#if 1 //for bug 44 2005-10-26 machiner
		strLength = strlen(SetGetString(Format_Info1));
//		osdStrDisp(160-strLength*(10/2), 155-2,UserFont10x20, 0x00, SetGetString(Format_Info1));    //Danny --05/05/24
		strLength = strlen(SetGetString(Format_Info2));
//       osdStrDisp(160-strLength*(10/2), 175-2,UserFont10x20, 0x00, SetGetString(Format_Info2));
		#endif
		if (xdcfFormat(1, "FAT", FORMAT_FS_ONLY) == SUCCESS)
		{
			#if 1
			xdcfInit(imageDirNameStr, DCNameStr, 0 /* | xDCF_CONFIG_DCF_ONLY*/);				
			dirInit();
			#endif

			#if 0
				if(FAIL != xdcfActiveDevIdSet(1))
					dirInit();
				xdcfActiveDevIdSet(2);
			#endif
			strLength = strlen(SetGetString(Format_Finish_s));
//			osdStrDisp(160-strLength*(10/2), 195-2, UserFont10x20, 0x00, SetGetString(Format_Finish_s));//for bug 44 2005-10-26 15:22 machiner
			MessageShow (MenuSuccess); // tzw add for bug 7
		}
		else
		{
			strLength = strlen(SetGetString(Format_Failed));
			osdStrDisp(160-strLength*(10/2), 195-2, UserFont10x20, 0x00, SetGetString(Format_Failed));//for bug 44 2005-10-26 15:22 machiner
		}
		osTimeDly(100);
	}
}

void Formatset(void)
{

	UINT8      sel = 0;
	UINT8      Front_sel=0;

	key = UI_KEY_MODE_SETUP;
	
	while (sub_menu_setup == S_Format)
       {

         switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
		   	sel = NandFlash;
			choose = setup[icon_idx_setup].OptionStart + sel;
			Front_sel=sel;

			if (cardExist == TRUE)
				Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);
			else
				Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems - 1);			
			break;
			
		case UI_KEY_DIR_UP:
	              if (cardExist == TRUE)
	             {
	 			sel = (sel == NandFlash) ? Card : sel -1;
	             }
			break;
		case UI_KEY_DIR_DOWN:
			if (cardExist == TRUE)
                	{
       			sel = (sel == Card) ? NandFlash : sel +1;
			}
			break;

		case UI_KEY_DIR_LEFT:
		   	sub_menu_setup = 0;
	  		continue;
			break;

		case UI_KEY_DIR_RIGHT:
		 	sub_menu_setup = 0;
			continue;
			break;
			
		case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
      		      storageFormat(sel);
			return;
			
		case UI_KEY_FUNC_MENU:
			menuReturn(UI_MAINMENU, 0);
		case UI_KEY_ACCIDENT:/* out/cancel */							
			sub_menu_setup = 0;
			return;
			break;
		}
		pb_OptionItem(setup[icon_idx_setup].OptionStart + sel,setup[icon_idx_setup].OptionStart + Front_sel);
		Front_sel=sel;
		keyGet(&key);
	}
}

#if 0
/*Ly 04/09/20 add end*/
//Davis:patch_2005/Apr/25 ÐÞ¸ÄÏÔÊ¾£¬Ö»ÏÔÊ¾ÈýÏî
void udiskDetect()
{
 	int  i;
 	UINT8 str[8];

	sPara.OTGModeFlag = OTGUDISK;
	otgJackIdSet(OTG_JACKID_HOST);
	
	/*Julie add 2004.12.2 start
	otgFuncReg_DPCtrl(host_DPCtrl);
    otgFuncReg_DMCtrl(host_DMCtrl);
    otgFuncReg_PowerCtrl(host_PowerCtrl);*/
	/*Julie add 2004.12.2 end*/
	
    otgHostEn(TRUE);
	osdStrDisp(214 , 140 , UserFont10x20, 0x70, "Udisk Host");
	for(i=10; i>0; i--)
		{
			sio_psprintf(str, "%02d", i);
			osdStrDisp(226 , 160 , UserFont10x20, 0x10, str);
			osTimeDly(200);
			if(uiUSBStatus)
			{
				printf("uiUsbstatus=1");
				break;
			}
		}

		if(!uiUSBStatus)
		{
			sPara.OTGModeFlag = OTGPC;
			/*sPara.StorageModeFlag = NandFlash;*/
			otgJackIdSet(OTG_JACKID_SLAVE);
			otgHostEn(FALSE);
			osdStrDisp(214 , 180 , UserFont10x20, 0x07, "Time Out!");
			osTimeDly(100);
			/*osdClearScreen(0);*/
		}
}
#endif
UINT8  ui8SelectDisplay_F = 0 ; // tzw add 060314 
void StorageSet(void)
{

	UINT8  sel = 0;
	UINT8  Front_sel=0;
	UINT32 err = FAIL;
	
	key = UI_KEY_MODE_SETUP;
	
	while (sub_menu_setup == S_Storage)
    {
         switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
		   	sel = sPara.StorageModeFlag;

			 if( sPara.USBModeFlag == USB_MODE_PCCAM )
			 {
			 	if(ui8SelectDisplay_F == 0 )
		   		{
					sel = PCMSDC;
			   	}	
		   	}
			
			choose = setup[icon_idx_setup].OptionStart + sel;			
			Front_sel=sel;
			Pb_Options(setup[icon_idx_setup].OptionStart, 3);
			break;
			
			case UI_KEY_DIR_UP:
			sel = (sel == NandFlash) ? PCMSDC: sel-1;
			break;
			
			case UI_KEY_DIR_DOWN:
			sel = (sel == PCMSDC) ? NandFlash : sel+1;
			break;

			case UI_KEY_DIR_LEFT:
			sub_menu_setup = 0;
			continue;
			break;

			case UI_KEY_DIR_RIGHT:
			sub_menu_setup = 0;
			continue;
			break;

			
			case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
			switch (sel)
			{
			  	case NandFlash:
					/*gpioSet(GPIO_DET_UDISK, 0);*/
					sPara.StorageModeFlag = sel;
					err = StorageSel(DRIVE_NAND);
					ui8SelectDisplay_F = 1 ;
			  		break;
					
			 	 case Card:
				 	/*gpioSet(GPIO_DET_UDISK, 0);*/
					sPara.StorageModeFlag = sel;
				 	err = StorageSel(DRIVE_SD);
					ui8SelectDisplay_F = 1 ;
			  		break;
				case  PCMSDC:
					 sPara.USBModeFlag = USB_MODE_PCCAM ; 
					 usbChangeMode(USB_MODE_PCCAM);
					 ui8SelectDisplay_F = 0 ;
					 err= SUCCESS;
					 break;
					
			}

         	if ( err!= FAIL)
			{
         		nandWriteParameters();
				strLenHalf = OFFSIDEX - strlen(SetGetString(Finished_s)) * 5;
//         			osdStrDisp(strLenHalf, OFFSIDEY + 110, UserFont10x20, 0x00,SetGetString(Finished_s)/* " OK"*/);//for bug 44
			}
			else
	    	{
			 	sPara.StorageModeFlag = NandFlash;
	   	 		StorageSel(DRIVE_NAND);
 				strLenHalf = OFFSIDEX - strlen(SetGetString(Failed_s)) * 5;
//       			osdStrDisp(strLenHalf, OFFSIDEY + 110, UserFont10x20, 0x10, SetGetString(Failed_s)/*"FAILED"*/);//for bug 44
			}
			osTimeDly(100);
       	//	sub_menu_setup = 0;
       	//	continue;
       		return;
			break;
			
			case UI_KEY_FUNC_MENU:
				menuReturn(UI_MAINMENU, 0);				
			case UI_KEY_ACCIDENT:/* out/cancel */							
				sub_menu_setup = 0;
				return;
				break;
		}
		//StorageDisp(sel);
		printf(" sel item [%d]\n",sel);
		printf(" frontsel item [%d]\n",Front_sel);
		pb_OptionItem(setup[icon_idx_setup].OptionStart + sel,setup[icon_idx_setup].OptionStart + Front_sel);
		Front_sel=sel;

		keyGet(&key);
		/*osdStrDisp(100 , 160, UserFont, 0x00, "                  ");*/
	//	osdStrDisp(strLenHalf , 200 , UserFont10x20, 0x00, "       ");
	}
}

/*Danny 05/05/22 Rewrite start*/
void TVoutSet(void)
{
    UINT8   sel = 0;
	UINT8   Front_sel=0;
	UINT8   TempFlag1=0;
	
	key = UI_KEY_MODE_SETUP;
	while (sub_menu_setup == S_TvOut)
    {
        switch(key)
		{
           case UI_KEY_MODE_SETUP:
		   	/* Paul@2006/12/26 - 09:52 add start */
			#if 0
		   	sel = sPara.TvoutFlag;			
			if(sel == TVOutOff)
			{
				sel = TVOutPAL;
			}
			#else
			sel = sPara.TvoutFlag;
			if (sel == TVOutOff)
			{
				if (sPara.VideoModeFlag == TVNTSC)
				{
					sel = TVOutNTSC;
				}
				else
				{
					sel = TVOutPAL;
				}
			}
			#endif			
			/* Paul@2006/12/26 - 09:53 add end */	
			
			choose = setup[icon_idx_setup].OptionStart + sel;		
			Front_sel=sel;
			Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);		
            break;
				
	        case UI_KEY_DIR_UP:
                sel = (sel == TVOutPAL) ? TVOutNTSC: TVOutPAL;
                break;
				
            case UI_KEY_DIR_DOWN:
//              sel = (sel == TVOutOff ||sel == TVOutNTSC) ? TVOutPAL: TVOutNTSC;
                sel = (sel == TVOutNTSC) ? TVOutPAL: TVOutNTSC;
                break;

			case UI_KEY_DIR_LEFT:
//                sPara.TvoutFlag = TVOutOff;				
				sub_menu_setup = 0;
				continue;
				break;

			case UI_KEY_DIR_RIGHT:
//                sPara.TvoutFlag = TVOutOff;								
				sub_menu_setup = 0;
				continue;
				break;				
            case UI_KEY_FUNC_OK:                          //05-05-24--Danny
                sPara.TvoutFlag = sel;
				printf("current sel item: %d\n",sel);
				choose = setup[icon_idx_setup].OptionStart + sel;	
				Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);						
	            nandWriteParameters();
		        if (avPlayNow)
                {
                    if (avPlayNow == OBJ_ASF)
                    {
	                    asfAppPlayAbort();       
                    }
                    else if (avPlayNow == OBJ_MP3)
                    {
                        mp3AppAbort();
                    }
                    else if (avPlayNow == OBJ_WAV)
                    {
                        audRiffCtrl(RIFF_STOP);
                    }
                }
                if(sel == TVOutPAL)
                {
					printf("\n 01");
					TempFlag1=2;            
                }
                else if(sel == TVOutNTSC)
                {
					printf("\n 02");
					TempFlag1=2; 
                }
                else if(sel == TVOutOff)
                {
					printf("\n 03");
					TempFlag1=1; 
                }

//			osdStrDisp(OFFSIDEX - strlen(SetGetString(Finished_s)) * 5, OFFSIDEY + 110,UserFont10x20, 0x00, SetGetString(Finished_s)/*" OK"*/);//for bug 44			
//			osTimeDly(100);
//			osdStrDisp(OFFSIDEX - strlen(SetGetString(Finished_s)) * 5, OFFSIDEY + 110, UserFont10x20, 0x00, "        ");//for bug 44 machiner 2005-10-26  "OK" always display on the TFT, "OK"---->"        "
//			return;
            break;
                
	        case UI_KEY_FUNC_MENU:
				menuReturn(UI_MAINMENU, 0);				
			case UI_KEY_ACCIDENT:/* out/cancel */								
                sub_menu_setup = 0;
				return;
                break;
        }
		
		printf("\n TVFLAG=%d , TempFlag=%d", TVFlag, TempFlag1);
		if( TempFlag1 )                                                            //05-05-24--Danny
		{    
			// if(TempFlag1==2&&TVFlag==2)
			 if(TempFlag1==2 && gpioGet(GPIO_DET_EAR))	
             {    
                gpioSet(GPIO_LCD_POWER, 0);
                printf("tv out on");
				if(sel == TVOutPAL)
	            {   
					panelSelect(PANEL_TYPE_3);
				}
				else if(sel == TVOutNTSC)
                {    
                   panelSelect(PANEL_TYPE_2);
				}

				
				TempFlag1=0;			
		        return;	
             }
             else if(TempFlag1==1)
             {   
                gpioSet(GPIO_LCD_POWER, 1);
                hwWait(0,300);
                Buzzer(5);
                panelSelect(PANEL_TYPE_1);
                printf("tv out off");
                sPara.TvoutFlag=TVOutOff;
			    TempFlag1=0;
		        return;	
            }	
		}		
		pb_OptionItem(setup[icon_idx_setup].OptionStart + sel,setup[icon_idx_setup].OptionStart + Front_sel);
		Front_sel=sel;
        keyGet(&key);
    }
}

void DefaultSet(void)
{
	UINT8      sel = 0;
	UINT8      Front_sel=0;
	UINT8      no = 0, yes = 1;

	key = UI_KEY_MODE_SETUP;	
	
	while (sub_menu_setup == S_Default)
       {

            switch(key)
	     {
	       case UI_KEY_MODE_SETUP:
			   	sel = no;
				Front_sel=sel;
				choose = setup[icon_idx_setup].OptionStart + sel;
				Pb_Options(setup[icon_idx_setup].OptionStart, setup[icon_idx_setup].OptionItems);
			break;
			
			case UI_KEY_DIR_UP:
				sel = (sel == no)?yes : no;
			break;
			
			case UI_KEY_DIR_DOWN:
				sel = (sel == yes)? no : yes;
			break;
			
			case UI_KEY_DIR_LEFT:
				sub_menu_setup = 0;
				continue;
			break;

			case UI_KEY_DIR_RIGHT:
				sub_menu_setup = 0;
				continue;
				break;		
			
			case UI_KEY_FUNC_OK:/*UI_KEY_FUNC_MENU:*/
			if (sel==yes)
			{
				defautParameters();
				nandWriteParameters();
			}
//			osdStrDisp(OFFSIDEX - strlen(SetGetString(Finished_s)) * 5, OFFSIDEY + 110, UserFont10x20, 0x00, SetGetString(Finished_s)/*" OK "*/);//for bug 44
			osTimeDly(100);
			return;
			
			case UI_KEY_FUNC_MENU:
	    		 menuReturn(UI_MAINMENU, 0);				
			case UI_KEY_ACCIDENT:/* out/cancel */							
				 sub_menu_setup = 0;
				 return;
				 break;
		}
		pb_OptionItem(setup[icon_idx_setup].OptionStart + sel,setup[icon_idx_setup].OptionStart + Front_sel);
		Front_sel=sel;
		keyGet(&key);
	}

}
/**************************************************************************
 *                                                                        *
 *  Function Name: uiSetup                                                *
 *                                                                        *
 *  Purposes:                                                             *
 *    function for Setup mode UI process                                  *
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
extern UINT32 gPB;
void uiSetup(void)
{
//	UINT32 
	key = UI_KEY_MODE_SETUP;

	UINT8 i;
	UINT8  front_icon_idx_setup=0;     //Danny 

	UINT32 dispW,dispH;/* sun@1030 pm 16:28 add */
	UINT8 index = 0;/* sun@1030 pm 16:28 add */

	#ifdef	OSD_DEBUG
	printf("\nSETUP ");
	#endif

//	icon_idx_setup = 0;
	isMenuOn = 0;
	for(i = 0; i < OPTIONS; i++)
	{
		setup[i].OptionItems = OptionItems[i];
		setup[i].OptionStart  =  OptnItem_START[i];
	}
		
	while ( (uiState & UI_MODE_MASK) == UI_MODE_SETUP )
	{			
		if (ui32NextState != uiState)
		{
			break;
		}
		switch(key)
		{
			#if 0
			/* sun@1030 pm 16:28 add */
			case UI_KEY_FUNC_ZOOMIN:						
				if(gpioGet(GPIO_SHUTTER1)||gpioGet(GPIO_SHUTTER2))
				{
					sysgMemDispAddrGet(&gPB);
					hwDispPreviewSizeGet(&dispW, &dispH);
					hwPlayBufSizeSet(dispW,	dispH);
					hwPlayBufAddrSet(gPB);
					pubFill((UINT8 *)(0xAC000000+gPB*2), dispW*2, dispH,
									0, 0, dispW*2, dispH, 0x808080);
					osdClearScreen(0x00);
					ShowVersion();
					TestFuncMenu(index);
					while(1)
					{
						switch(key)
						{
							case UI_KEY_DIR_UP:
								if(index==0) index=3;
								else index--;
								TestFuncMenu(index);
								break;
							case UI_KEY_DIR_DOWN:
								if(index==3) index=0;
								else index++;
								TestFuncMenu(index);
								break;
							case UI_KEY_FUNC_OK:
								if(index == 3)
									{
									defaultMode();
									}
								else
								{
									#if 1
									TestFuncFlagSet( 1,index);
									
									/*ui_ModeSwitch(0x07);*/
									
									uiState = UI_MODE_PREVIEW;
									return;
									#else
									if (index==0)
										menuOV3620_OBCalibration();
									else if (index==1)
										badPixSeek();
									else
										BurnPreviewStill();
									#endif
									}
								break;									
						}
						keyGet(&key);
						if(key==UI_KEY_FUNC_MENU) 
							break;
					}
					key = UI_KEY_MODE_SETUP;
					continue;
				}
				break;
			case UI_KEY_FUNC_ZOOMOUT:
				break;
		/* sun@1030 pm 16:28 end */			
			#endif
								
			case UI_KEY_FUNC_MENU:
				menuReturn(UI_MAINMENU, 0);	
			case UI_KEY_ACCIDENT:/* out/cancel */	
				break;
				
			case UI_KEY_FUNC_OK:
				#if 1
			    sub_menu_setup = icon_idx_setup + 1;
				switch (sub_menu_setup)							
				{

				 case S_TimeSet:
				   	 TimeSet();
				   	 break;
				}

		    	key = UI_KEY_MODE_SETUP;
				sub_menu_setup= 0;
				continue;
				#endif
				break;
			case       UI_KEY_DIR_LEFT:
                icon_idx_setup = (icon_idx_setup <= 0) ? SETUPITEM-1:(icon_idx_setup - 1);					
				break;

			case      UI_KEY_DIR_RIGHT:
                icon_idx_setup = (icon_idx_setup >= SETUPITEM -1) ? (0):(icon_idx_setup  + 1);
				break;

			case       UI_KEY_DIR_UP:
				break;

			case      UI_KEY_DIR_DOWN:
				break;
				
			case	UI_KEY_MODE_SETUP:
				osTaskSuspend(osTaskFindByName("AAA"));
//					osTaskSuspend(osTaskFindByName("HISTOGRAM"));
				
				osdClearScreen(0);
				pbImagePaste(SetICON[12][2], SetICON[12][3], SetICON[12][4],SetICON[12][5]);

			//	pbImagePaste(SetBG, sizeof(SetBG), 0,  0); 
				Pb_Title();
				pb_subItem(icon_idx_setup, 4);					
				break;

			default:
				#ifdef	OSD_DEBUG
				printf("\nKEY=%0x ", key);
				#endif
				break;
		}
			
		pb_subItem(icon_idx_setup, 4);				
		pb_Item(icon_idx_setup, front_icon_idx_setup);
		front_icon_idx_setup = icon_idx_setup;	

#if 1
		sub_menu_setup = icon_idx_setup + 1;
		switch (sub_menu_setup)							
		{
		#if 0
		    case S_display:
			   	DisplaySet();
				break;
		#endif		
		 /*   case S_TimeSet:
			   	TimeSet();
			   	 break;
		*/	
			case S_KeyTones:
				SpeakerSet();
				continue;
				break;
					
			 case S_Language:
				LanguageSet();
				continue;					
				   	break;
					
			 case S_Storage:
				StorageSet();
				continue;					
				   	break;
					
			 case S_RayFreq:
				RayFreqSet();
				continue;					
				   	break;
					
			 case S_TvOut:
				TVoutSet();
				continue;						
				   	break;		

			case S_Default:
				DefaultSet();
				continue;						
					break;

			case S_AutoPwOff:
				AutoPowerOffSet();
				continue;						
					break;									
					
			case S_Format:
				Formatset();
				continue;						
					break;

			case S_Version:
				Version();
				continue;
					break;
		//	case S_OpenMode:
		//		OpenModeset();
		//		continue;						
		//			break;
					
			case S_Splash:
				SplashSet();
				continue;						
					break;									
		}

		#endif
		
		keyGet(&key);

	}

	hwDispPipEn(0);	
	uiState = ui32NextState;	
}

/**
	Paul@2006/05/29 add for test mode
**/
extern UINT32 gPB;
extern UINT8 DcPlug;
extern UINT8 ui8EnterPowerDownVoltage ;
void DcTestMode()
{
	UINT32	dispW,dispH;/* sun@1030 pm 16:28 add */
	UINT8	index = 0;/* sun@1030 pm 16:28 add */
	
	UINT32	key = UI_MODE_MASK;

	osTaskSuspend(osTaskFindByName("HISTOGRAM"));
//	osTaskSuspend(osTaskFindByName("AAA"));										
	
	sysgMemDispAddrGet(&gPB);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);
	pubFill((UINT8 *)(0xAC000000+gPB*2), dispW*2, dispH,
					0, 0, dispW*2, dispH, 0x808080);
	osdClearScreen(0x00);
	ShowVersion();
	TestFuncMenu(index);
	ui8EnterPowerDownVoltage = 0xff;
	
	while(uiState == UI_MODE_TEST)
	{
		printf("Enter OB\n");
		switch(key)
		{
		case UI_KEY_ACCIDENT:
			uiState = ui32NextState;
			return;
		
		case UI_KEY_DIR_UP:
			if(index==0) 
			{
				index = TESTMENUINDEX-1;
			}
			else 
			{
				index--;
			}
			TestFuncMenu(index);
			break;
			
		case UI_KEY_DIR_DOWN:
			if(index == TESTMENUINDEX-1) 
			{
				index = 0;
			}
			else 
			{
				index++;
			}
			TestFuncMenu(index);
			break;
			
		case UI_KEY_FUNC_OK:			
			switch (index)
			{
				case 0:
					if (DcPlug == 0)
					{
						//F_TestBatteryVoltageValue();
						F_GetHighVoltageValueTest(); 
						return;															
					}
					break;

				case 1:
					ui8EnterPowerDownVoltage = 0x00;
					TestFuncFlagSet(1, 2); /* BURN_IN_TEST = 2 */
					uiState = UI_MODE_PREVIEW;
					ui32NextState = uiState;
					return;
					break;
				case 2:
					if (sPara.ui8UserPowerDownVolFlag == 0xA5 )
					{
						AdjustVoltageValueFunc();
						return;															
					}
					else
					{
						osdStrDisp(00, 180, UserFont10x20, 0xd7, "  You Have Not Adjusted Voltage!");
					}
					break;
			}
		}
		
		pubFill((UINT8 *)(0xAC000000+gPB*2), dispW*2, dispH,
		0, 0, dispW*2, dispH, 0x808080);
		osdClearScreen(0x00);							
		ShowVersion();
		TestFuncMenu(index);						
		
		keyGet(&key);
	}
}

