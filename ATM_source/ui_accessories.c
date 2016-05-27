/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2003 by Sunplus Technology Co., Ltd.             *
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
 *  2 floor R2-B building ShenZhen High-Tech Industria Park Of China.     *
 *  Author: Woods Gu                                                       *
 *                                                                        *
 **************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "general.h"
#include "stdio.h"
#include "os_api.h" 
#include "ui_main.h"
#include "hw_disp.h"
#include "hw_disp_ext.h"
#include "osd_common.h"
#include "osd_api.h"
#include "panel.h"
#include "xiaoao.h"
#include "game.h"
#include "utility.h"
#include "Card.h"
#include "gpio.h"
#include "ui_accessories.h"
#include "Oth_scr.h"
extern UINT32 vfsDevInfoGet(UINT32, UINT32, devInfo_t *);

extern UINT32 cardExist;
extern UINT8  DcPlug;
extern UINT8  PowerLock;
extern struct nandUIPara_s sPara;

UINT8     dateStr[11];
UINT8     *dateCHStr[11];

enum {
AP_IDLE = 0,
AP_CALENDAR,
AP_MENORY,
AP_GAME1,
AP_GAME2,
AP_EBOOK,	
};

UINT16 uiAccessr_App = 0;
UINT8  icon_idx_acc = 0;
UINT8 sub_menu_acc = 0;

ICON_POS icon_pos_acc[4] = {
	{60, 32},
	{262, 32},
	{60, 120},
	{262, 120}};

char *weekTbl[][LANGUAGES] =
 {
   { "Sun","日","ら","Dom","вос",
   	"Son","Dim","Dom","Dom","老",},

   { "Mon","一","","Lun","Пон",
   	"Mon","Lun","Lun","Seg","岿",},

   { "Tue","二","","Mar","вто",
   	"Die","Mar","Mar","Ter","拳",},
   	
   { "Wed","三","","Mié","сре",
   	"Mit","Mer","Mer","Qua","荐",},
   	
   { "Thu","四","","Jue","чет",
   	"Don","Jeu","Gio","Qui","格",},
   	
   {"Fri","五","き","Vie","пят",
   	"Fre","Ven","Ven","Sex","陛",},
   	
   { "Sat","六","せ","Sáb","суб",
   	"Sam","Sam","Sab","Sáb","配",},
   	
};
typedef struct clk_month
{
    UINT8  MonthDays;
    UINT16  MonthDayoffset;                /* Number of days in each month                                 */
//    UINT8  *MonthName_e;                /* Name of the month                                            */
//    UINT8 *MothName_s;		
    UINT8 *MothName[LANGUAGES];			
} CLK_MONTH;

//LANGUAGES  268 269 same
CLK_MONTH  ClkMonthTbl[] = {    /* MONTHS TABLE                                                 */
    {0, 0, {"","","  "}},             /* Invalid month                                                */

	{31, 1, {"January ", "一月", "る","enero","в январе",
		"Januar","Jan","Gennaio","Janeiro","1岿",}},	/* January                                                      */

	{28, 32, {"February ","二月","る","febrero","в феврале",
		"Februar","fév"/*"Fér"*/,"Febbraio","Fevereiro","2岿 ",}},  	/* February (note leap years are handled by code)               */

	{31, 60, {"March     ","三月","る","marzo","в марте",
		"M\xe4rz","Mars","Marzo","Mar\xe7o","3岿 ",}}, 	/* March                                                        */

	{30, 91, {"April     ","四月","る","abril","в апреле",
		"April","Avril","Aprile","Abril","4岿 ",}}, 		/* April                                                        */

    {31, 121, {"May     ","五月","きる","Mayo","в мае ",
		"Mai","Mai","Maggio","Maio","5岿",}}, 	/* May                                                          */

	{30, 152, {"June    ","六月","せる","junio","в июне",
		"Juni","Juin","Giugno","Junho","6岿",}}, 		/* June                                                         */

	{31, 182, {"July    ","七月","る","julio","в июле",
		"Juli","Juil","Luglio","Julho","7岿",}},  /* July                                                         */

	{31, 213, {"August ","八月","る","agosto","в августе ",
		"August","Ao\xfbt","Agosto","Agosto","8岿",}}, 	/* August                                                       */

	{30, 244, {"September ","九月","る","septiembre","в сентябре",
		"September","Sep","Settembre","Setembro","9岿",}},   	/* September                                                    */

	{31, 274, {"October  ","十月","る","octubre","в октябре",
		"Oktober","Oct","Ottobre","Outobro","10岿",}}, 		/* October                                                      */

	{30, 305, {"November ","十一月","る","noviembre","в ноябре ",
		"November","Nov","Novembre","Novembro","11岿",}},  		/* November                                                     */

	{31, 335, {"December ","十二月","る","diciembre","в декабре",
		"Dezember","Déc","Dicembre","Desembro","12岿",}}              /* December                                                     */
};
UINT32 ToolIcon[][4] = {
	{ACale_P1,sizeof(ACale_P1), 166,50,},		
	{AMemo_P1,sizeof(AMemo_P1), 166, 50,},		
	{ASnak_P1,sizeof(ASnak_P1), 166, 50,},		
	{ARuss_P1,sizeof(ARuss_P1), 166, 50,},
	{accee_I1,sizeof(accee_I1), 24, 56,},
};

void ui_ShowToolsBg(void)
{
	pbImagePaste(Bg_Tools, sizeof(Bg_Tools), 0, 0);	

}
/////////////////
void Pb_ToolTitle(UINT8 *Title)
{
	
	static UINT8   PbTilie_X ,PbTilie_Y ;
	
	switch(sPara.language)
	{
		case LanguageEnglish:
			PbTilie_X= 18 ;
			PbTilie_Y= 3 ;
			break ;
		case LanguageSpanish:
			PbTilie_X= 8 ;
			PbTilie_Y= 3 ;
			break ;
		case Languagekorean:	
		case LanguageRussian:
			PbTilie_X= 8 ;
			PbTilie_Y= 3 ;
			break ;
		default :			/*中文*/
			PbTilie_X= 38 ;
			PbTilie_Y= 3 ;
			break ;
	}
	osdStrDisp(PbTilie_X, PbTilie_Y, UserFont10x20, 0xd0, Title);
	
}
/******************/
UINT32  F_StringCtrl(UINT8 *str)
{
	if(sPara.language==LanguageRussian)
	{
		return (strlen(str)/2*10) ;
	}
	return strlen(str)*10 ;
}
/******************/
UINT8 *ToolGetString(UINT16 stringId)
{
    UINT8* str;
    UINT32 languageId = sPara.language;

    if (languageId >= LANGUAGES)
		languageId = LanguageEnglish;

    if(stringId >= MAX_ID) return 0;

    if(ToolStrings[stringId][languageId][0]=='\0')
	    str = ToolStrings[stringId][0];
	else
	    str = ToolStrings[stringId][languageId];

	return str;
}


void app_memory(void)
{
	UINT8     str1[5],str2[5], str3[5],str4[5];
	UINT32   nand_cap = 0, nand_f = 0;
	UINT32   card_cap = 0, card_f = 0;
	UINT8     percent;
	UINT8      verstr[4];     //Danny  --05/06/05
	UINT8	 version[22];
	UINT32   key = UI_KEY_MODE_ACCESSORIES;

	devInfo_t pdevInfo1, pdevInfo2 ;

	osdClearScreen(0);
	ui_ShowToolsBg( );
	Pb_ToolTitle(ToolGetString(Memory_s));

	if ((cardExist == 1)&&(sPara.StorageModeFlag == Card))//wendy update 050122 for memory bug
//	if (cardExist == 1)
	 {
	 	vfsDevInfoGet(DRIVE_SD, 0, &pdevInfo2);
	  	card_cap = pdevInfo2.megaByte;
		card_f = (pdevInfo2.nrFreeBytes)/1024/1024;
		printf("cardcap= %d\n",card_cap);
	 	printf("cardfree= %d\n",card_f);
	 	sio_psprintf(str3, "%d", card_cap - card_f);
		sio_psprintf(str4, "%d", card_f);
		if(card_cap!=0)
		{
			percent = (card_cap-card_f)*100/card_cap;
		}
		else
		{
			percent= 0 ;
		}
		//wendy@050301 begin
		if(percent<10)
		sio_psprintf(str1, "%01d", percent);
		else	
		sio_psprintf(str1, "%02d", percent);
		//wendy@050301 end
	
		osdStrDisp(94, 130,UserFont10x20, 0xd0, ToolGetString(Used_s));
		osdStrDisp(94+F_StringCtrl(ToolGetString(Used_s)), 130,UserFont10x20, 0xd0, str1);
		//osdStrDisp(280, 130, UserIcon10x20, 0xdf0, "\x6c");
		osdStrDisp(94+F_StringCtrl(ToolGetString(Used_s))+strlen(str1)*10, 130,UserFont10x20, 0xd0, "%");	/*machiner add 2005-09-23 14:15*/
//		osdStrDisp(172+(strlen(str1))*10, 130,UserFont10x20, 0xd0, "%");	/*machiner add 2005-09-23 14:15*/
		osdStrDisp(100, 45, UserFont10x20, 0xd0, ToolGetString(SDCARD_s));

		osdStrDisp(94, 170,UserFont10x20, 0xd0, ToolGetString(Unused_s));
//		osdStrDisp(100 + strlen(ToolGetString(Unused_s))*10, 170,UserFont10x20, 0xd0, str4);
//		osdStrDisp(100 + strlen(ToolGetString(Unused_s))*10 +strlen(str4)*10,  170,UserFont10x20, 0xd0, "MB");				
		osdStrDisp(100 + F_StringCtrl(ToolGetString(Unused_s)), 170,UserFont10x20, 0xd0, str4);
		osdStrDisp(100 + F_StringCtrl(ToolGetString(Unused_s))+strlen(str4)*10,  170,UserFont10x20, 0xd0, "MB");				
		
		osdStrDisp(94, 150,UserFont10x20, 0xd0, ToolGetString(use_s));
//		osdStrDisp(100 + strlen(ToolGetString(use_s))*10, 150,UserFont10x20, 0xd0, str3);
//		osdStrDisp(100 + strlen(ToolGetString(use_s))*10+strlen(str3)*10, 150,UserFont10x20, 0xd0, "MB");				
		osdStrDisp(100 + F_StringCtrl(ToolGetString(use_s)), 150,UserFont10x20, 0xd0, str3);
		osdStrDisp(100 + F_StringCtrl(ToolGetString(use_s))+strlen(str3)*10, 150,UserFont10x20, 0xd0, "MB");				

			
		osdFrameDrawX(54, 80, 206, 40, 0x07, 5);
		//osdBarDraw(58, 85, (card_cap-card_f)*200/card_cap, 30, 0x03);
		osdBarDraw(58, 85,percent*2, 30, 0x03);
	}
	else
	{
		 vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfo1);
		 nand_cap = pdevInfo1.megaByte;
		 nand_f = (pdevInfo1.nrFreeBytes)/1024/1024;
		 printf("nandcap= %d\n",nand_cap);
		 printf("nandfree= %d\n",nand_f);
		 percent = (nand_cap-nand_f)*100/nand_cap;
		//wendy@050301 begin
		
	 	sio_psprintf(str3, "%d", nand_cap - nand_f);
		sio_psprintf(str4, "%d", nand_f);
		
		if(percent<10)
		sio_psprintf(str2, "%01d", percent);
		else	
		sio_psprintf(str2, "%02d", percent);
		//wendy@050301 end

		osdStrDisp(94, 130,UserFont10x20, 0xd0, ToolGetString(Used_s));
		osdStrDisp(94+F_StringCtrl(ToolGetString(Used_s)),130,UserFont10x20,0xd0,str2);
		//osdStrDisp(280, 130, UserIcon10x20, 0xdf0, "\x6c");
//		osdStrDisp(172+(strlen(str2))*10, 130,UserFont10x20, 0xd0, "%");	/*machiner add 2005-09-23 14:15*/
		osdStrDisp(94+F_StringCtrl(ToolGetString(Used_s))+strlen(str2)*10, 130,UserFont10x20, 0xd0, "%");	/*machiner add 2005-09-23 14:15*/
		osdStrDisp(100, 45, UserFont10x20, 0xd0, ToolGetString(NandFlash_s));

		osdStrDisp(94, 170,UserFont10x20, 0xd0, ToolGetString(Unused_s));
//		osdStrDisp(100 + strlen(ToolGetString(Unused_s))*10, 170,UserFont10x20, 0xd0, str4);
//		osdStrDisp(100 + strlen(ToolGetString(Unused_s))*10 +strlen(str4)*10,  170,UserFont10x20, 0xd0, "MB");				
		osdStrDisp(100 + F_StringCtrl(ToolGetString(Unused_s)), 170,UserFont10x20, 0xd0, str4);
		osdStrDisp(100 + F_StringCtrl(ToolGetString(Unused_s))+strlen(str4)*10,  170,UserFont10x20, 0xd0, "MB");				
		
		osdStrDisp(94, 150,UserFont10x20, 0xd0, ToolGetString(use_s));
//		osdStrDisp(100 + strlen(ToolGetString(use_s))*10, 150,UserFont10x20, 0xd0, str3);
//		osdStrDisp(100 + strlen(ToolGetString(use_s))*10 +strlen(str3)*10, 150,UserFont10x20, 0xd0, "MB");				
		osdStrDisp(100 + F_StringCtrl(ToolGetString(use_s)), 150,UserFont10x20, 0xd0, str3);
		osdStrDisp(100 + F_StringCtrl(ToolGetString(use_s))+strlen(str3)*10, 150,UserFont10x20, 0xd0, "MB");				


			
		osdFrameDrawX(54, 80, 206, 40, 0x07, 5);
		osdBarDraw(58, 85, (nand_cap-nand_f)*200/nand_cap, 30, 0x03);
	}
	 while ( sub_menu_acc == AP_MENORY){
		switch(key)
		{
			case UI_KEY_MODE_ACCESSORIES:/*in*/
				break;

			case UI_KEY_FUNC_MENU:/* out/cancel */
			case UI_KEY_ACCIDENT:/* out/cancel */				
				sub_menu_acc = 0;
				continue;
				break;


				
		}
		keyGet(&key);
       }

}

UINT8  IsLeapYear(UINT32 year)
{
    if ((!(year % 4) && (year % 100)) || !(year % 400))
     {
        return 1;
     }
    else
     {
        return (0);
     }
}

#if 1
void calendarshow(UINT32 year, UINT8 month, UINT8 day)
{
    UINT32	osdW, osdH;
	UINT8     x , y;
	UINT8     i, j = 0;
	UINT32    countday, yearoffset;
	UINT8     weekoffset;
    UINT8     monthvalueStr[62];
	UINT32   color = 0x00;

	UINT8     monthdays;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	x = 15;
	y =  PANEL_1_OSD_H /10+28;


	osdBarDraw(0, 30, 320, 210, 0x00);	


	ltoa((UINT32)year, &dateStr[0], 10, 4);
	dateStr[4] = '\0';

	osdStrDisp(x+6*10, y+2, UserFont10x20, 0xf0, dateStr);
	

	osdStrDisp(x+18*10 , y+2, UserFont10x20, 0xf0, ClkMonthTbl[month].MothName[sPara.language]);		
	 for( i = 0 ;i < 7; i++)
           osdStrDisp(x+ i*(42) +6 , y+22+2, UserFont10x20, 0xf0, weekTbl[i][sPara.language]);	

	 osdLineDrawX(x-3, y+22, x+ 7*(42) - 1, y+22, 0x0d, 2);
	 osdLineDrawX(x-3, y+22*2, x+ 7*(42) - 1, y+22*2, 0x0d, 2);

	  monthdays = ClkMonthTbl[month].MonthDays;

    /*算出一号为星期几*/
	/* countday= 从这一年的一月一号算起到这一天(包括这一天)的天数*/
	/* s = x-1 +[(x-1)/4]-[(x-1)/100]+[(x-1)/400] +countday;   x=year  */
	 countday = ClkMonthTbl[month].MonthDayoffset;
	 if (IsLeapYear(year))
	    {
	      if (month == 2)
		  monthdays = ClkMonthTbl[month].MonthDays + 1;
	     else if (month > 2)
	        countday++;
	    }
	 printf("\ncountday=%d", countday);
	 yearoffset = year  -1;
	 printf("\nyearoffset=%d", yearoffset);
        weekoffset = (yearoffset + yearoffset /4 -yearoffset/100 + yearoffset/400 + countday) % 7;
	 printf("\nweekoffset=%d", weekoffset);


 //	  j =(monthdays -1 + weekoffset) /7 ; /* 控制换行*/
//	 osdBarDraw(x-3,y + 2*22+ 22* (j) , x+ 7*42 -11,23, 0x0e);	 


	/*draw days*/
	 for( i = 0; i < monthdays; i++ )
	 {	
 		
	    
 		  ltoa((UINT32) i+1, &monthvalueStr[0], 10, 2);
	      j =(i + weekoffset) /7 ; /* 控制换行*/
	      if(( i + 1) ==day)
		 color = 0x7d;
	      else	if ((i + weekoffset) %7 ==0)
		  color = 0x10;
	      else if ((i + weekoffset) %7 ==6)
		  color = 0x10;
	      else
		  color = 0x70;
		osdStrDisp(x+ ((i + weekoffset)%7)*(42) , y + 2*22+ 22* j+2, UserFont10x20, color, " ");
	       osdStrDisp(x+ ((i + weekoffset)%7)*(42)+ 10, y + 2*22+ 22* j+2, UserFont10x20, color, monthvalueStr);
		osdStrDisp(x+ ((i + weekoffset)%7)*(42) + 30, y + 2*22+ 22* j+2, UserFont10x20, color, " ");

	 }
	   /*draw lines*/ 
//	 for (i = 0; i < j+1; i++)
	 for (i = 0; i < 7+1; i++)
	   {
	// 	osdLineDrawX(x-3, y + 2*22+ 22* (i+1) , x+ 7*(42) - 1, y + 2*22+ 22* (i+1) , 0x09, 1);
		
	   }

#if 0
	 for (i = 0; i < 8; i++)
	   {
	       if ( i ==0|| i == 7)
		    osdLineDrawX(x-3+i*(42), y , x-3+i*(42), y+22+22* (j+2), 0x09, 2);
		else
	           osdLineDrawX(x-2+i*(42), y+22 , x-2+i*(42), y+22+22* (j+2), 0x09, 1);
		
	   }
	 
#endif	 

	
}
#endif

void  app_calendar(void)
{
	UINT32    key = UI_KEY_MODE_ACCESSORIES;
	 
 	struct	tm	*ptime;
			
	UINT32   curyear;
	UINT8     curmonth;
	UINT8      curday;
      SINT8     inc = 0;
	SINT8     leapinc = 0;
	 
 	osdClearScreen(0x0);
	ui_ShowToolsBg();

	Pb_ToolTitle(ToolGetString(Calendar_s));

	timeGet(&ptime);
	curday = ptime->tm_mday;
	curmonth = ptime->tm_mon;
	curyear = ptime->tm_year +1900;

	while (sub_menu_acc == AP_CALENDAR)
 	{
		switch(key)
		{
			case UI_KEY_MODE_ACCESSORIES:/*in*/

			/*	 osdClearScreen(0x0);
	   			 uiDrawSubIconFrame(icon_idx_acc, ACTIVECOLOR);
	    			 calendarshow(ptime->tm_year+ 1900, ptime->tm_mon, ptime->tm_mday);*/
	    			/* calendarshow(curyear, curmonth, curday);*/

				break;
			case UI_KEY_FUNC_MENU:/* out/cancel */
			case UI_KEY_ACCIDENT:/* out/cancel */								
				sub_menu_acc = 0;
				continue;
				/*return 1;*/
				break;



				
			case UI_KEY_FUNC_OK:

				break;
			case UI_KEY_DIR_LEFT:
				 inc = -1;
				break;
			case UI_KEY_DIR_RIGHT:
				 inc = 1;
				break;
			case UI_KEY_DIR_UP:
				 inc = -7;
				break;
			case UI_KEY_DIR_DOWN:
				inc = 7;
				break;
		}

            	if ((curday + inc) < 1)
		 {
		   if( curmonth == 1) /*跨年*/
			{
				curyear--;
				curmonth = 12;
				/*curday += ClkMonthTbl[curmonth].MonthDays + inc;	*/
			}
		    else
			{
				if (curmonth == 3)
                               {
                                  if( IsLeapYear(curyear))
					   leapinc = 1;
				   }
                        	curmonth --;
			 }
                  curday += ClkMonthTbl[curmonth].MonthDays + inc + leapinc ;
		 }
	 	else if ((curday + inc) > ClkMonthTbl[curmonth].MonthDays)
		 {
		     if (curmonth == 12) /*跨年*/
			    {
			       curday = curday +  inc - ClkMonthTbl[curmonth].MonthDays ;
                            curyear++;
				curmonth = 1;
			    }
		    else
			   {
				if (curmonth == 2)
			  	 {
			     	    if( IsLeapYear(curyear))
				  	 leapinc = -1;
			 	 }
				 curday = curday +  inc - ClkMonthTbl[curmonth].MonthDays + leapinc;
			    	 curmonth ++;
			   }

		 }

	      else
		    curday += inc;

	 leapinc = 0;
	 inc = 0;
     printf("\ncuryear=%d,curmonth=%d,curday=%d",curyear,curmonth,curday);

//	 osdClearScreen(0x0);
//	 ui_ShowAccMenuInfo();
//	 ui_CustomCalenderShow( );
	 osdFWBuffChange();
	 hisTimeDisp() ;
	 hisIconDisp() ;
	 calendarshow(curyear, curmonth, curday);
 	 Pb_ToolTitle(ToolGetString(Calendar_s));
	 osdHWBuffChange();
      /*up/down select icon   left/right  add/sub value*/

     keyGet(&key);

	}

	/*osdClearScreen(0x0);
	uiDrawSubIconFrame(icon_idx_acc, ACTIVECOLOR);
	curday = ptime->tm_mday;
	curmonth = ptime->tm_mon;
	curyear = ptime->tm_year +1900;
	calendarshow(curyear, curmonth, curday);
	 */
}

void  Pb_ToolsItem(UINT8 idx, UINT8 Items)
{	
	UINT8 i,j = 0;
	UINT8 space[14];	
	memset(space, ' ', 12);
	space[12] = '\0';	

	for(i = idx; i < Items; i++, j++)
	{
		Tools[i].x = 56-6;
		Tools[i].y = 56 + j * 32;
		osdStrDisp(Tools[i].x, Tools[i].y, UserFont10x20, 0xf0, Tools[i].Content[sPara.language]);				
	}
}

void  Pb_ToolChose(UINT8 idx, UINT8 front)
{	
	UINT8 i,j = 0;
	UINT8 space[14];	
	memset(space, ' ', 12);
	space[12] = '\0';	
	i = idx - 1;
	j = front -1;

	UINT8 barx = 42;
	UINT8 barwidth = 93;
	
	pbDisplayCleFile_Ico(barx + 4,Tools[j].y-2);
//	osdBarDraw(barx - 4, Tools[j].y-2,  barwidth, 24, 0x00);	
	osdStrDisp(Tools[j].x, Tools[j].y, UserFont10x20, 0xf0, Tools[j].Content[sPara.language]);					

	pbDisplaySelFile_Ico(barx +4,Tools[i].y-2);

//	osdBarDraw(barx - 4, Tools[i].y-2,  barwidth, 24, 0x0e);	
	osdStrDisp(Tools[i].x, Tools[i].y, UserFont10x20, 0x10, Tools[i].Content[sPara.language]);				
	pbImagePaste(ToolIcon[i][0], ToolIcon[i][1], ToolIcon[i][2], ToolIcon[i][3]);
}

/**************************************************************************
 *                                                                        									*
 *  Function Name: uiAccessories                                               						*
 *                                                                        									*
 *  Purposes:                                                             								*
 *    function for uiAccessories mode UI process                                  					*
 *                                                                        									*
 *  Descriptions:                                                         								*
 *                                                                        									*
 *  Arguments:                                                            								*
 *                                                                        									*
 *  Returns: This function should never return.                           						*
 *                                                                        									*
 *  See also:                                                             								*
 *                                                                        									*
 **************************************************************************/
 UINT8	ui8GameFlag = 0 ;
extern	UINT32  ui32SavePanel ;
void
uiAccessories(
	void
)
{
	UINT32 key = UI_KEY_MODE_ACCESSORIES;
	UINT8  Front_icon_idx_acc=1;
	#ifdef	OSD_DEBUG
	printf("\nmode ACCESSORIE\n");
	#endif

	uiAccessr_App = 0;
	/*hwDispPipEn(1);
	osdIconDisp(1);*/

	while ( ( uiState & UI_MODE_MASK ) ==UI_MODE_ACCESSORIES )
	{
		if (ui32NextState != uiState)
		{
			break;
		}
		
		switch(key)
		{
			case UI_KEY_FUNC_MENU:
					menuReturn(UI_MAINMENU, 0);
					break;

			case UI_KEY_FUNC_OK:
				    sub_menu_acc = icon_idx_acc;
					printf("\ n OK KEY the icon_idx_acc %d",icon_idx_acc);
					switch (sub_menu_acc)
					{
 				        case AP_GAME1:
						   	osTaskSuspend(osTaskFindByName("HISTOGRAM"));
							/*setSnakeColor(1, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c);*/							
							ui8GameFlag = 1 ;					//for bug 47
							panelSelect(ui32SavePanel);							
						   	Snakemain();
							hwDispPvEn(0);
							uiDispBuffFill(0x00);
							osdClearScreen(0);
							ui8GameFlag = 0 ;							
							panelSelect(ui32SavePanel);														
						   	break;
							 
						 case AP_GAME2:
						 	 osTaskSuspend(osTaskFindByName("HISTOGRAM"));
							/* setRussianColor(1, 0x0a, 0x0f, 0x0c);*/
							ui8GameFlag = 1 ;				//for bug 47
							panelSelect(ui32SavePanel) ;							
					   	    Russianmain();							 							
							hwDispPvEn(0);
							uiDispBuffFill(0x00);							
							osdClearScreen(0);							
							ui8GameFlag = 0 ;
							panelSelect(ui32SavePanel) ;
						   	break;
							 
					  	case AP_MENORY:
							app_memory();
						 	break;
							
	   	 				case AP_EBOOK:
							break;
							#if 0
							osTaskSuspend(osTaskFindByName("HISTOGRAM"));
							#endif
			  		//		uiebook();
							break;

	   	 				case AP_CALENDAR:
							printf("\ n AP_CALENDAR %d",icon_idx_acc);
							app_calendar();
							break;								
					}
				    key = UI_KEY_MODE_ACCESSORIES;
					sub_menu_acc = 0;
					continue;
				    break;
					
			case       UI_KEY_DIR_LEFT:
				break;

			case      UI_KEY_DIR_RIGHT:
				break;
				
			case       UI_KEY_DIR_UP:
             	   icon_idx_acc = (icon_idx_acc  <= 1 ) ? (4) : (icon_idx_acc - 1);
				break;

			case      UI_KEY_DIR_DOWN:
			    icon_idx_acc = (icon_idx_acc >= 4 ) ? (1) : (icon_idx_acc + 1);
				break;
				
			case	UI_KEY_MODE_ACCESSORIES:
				osTaskResume(osTaskFindByName("HISTOGRAM"));
				osTaskSuspend(osTaskFindByName("AAA"));
				osdClearScreen(0x00);
				pbMainBack();
				pbImagePaste(ToolIcon[4][0], ToolIcon[4][1], ToolIcon[4][2], ToolIcon[4][3]);

//				pbImagePaste(ACCBG,sizeof(ACCBG), 0, 0);
				if (icon_idx_acc ==0)
				{
					icon_idx_acc = 1;
				}
				Pb_ToolsItem(0, 4);
				Pb_ToolTitle(ToolGetString(Tool_s));
				break;

			default:
				#ifdef	OSD_DEBUG
				/*printf("\nKEY=%0x ", key);*/
				#endif
				break;
		}
		Pb_ToolChose(icon_idx_acc, Front_icon_idx_acc);
		printf("\ n  the icon_idx_acc %d",icon_idx_acc);			
		Front_icon_idx_acc = icon_idx_acc;
		keyGet(&key);
	}
	
	hwDispPipEn(0);
	uiState = ui32NextState;
}



