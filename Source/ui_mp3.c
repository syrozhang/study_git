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
//#define  RS_232_DBG
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "stdio.h"
#include "ui_main.h"
/* 536@patch1.8.1@Joe@2003.11.5 15:27 add begin */
#include "general.h"
#include "asf_api.h"
#include "dcf_api.h"
#include "snap_api.h"
#include "pb_api.h"
#include "card.h"
#include "os_api.h"
#include "hw_cammode.h"
#include "hw_cdsp.h"
#include "hw_disp.h"
#include "hw_front.h"
#include "hw_sdram.h"
#include "dsc_init.h"
#include "stdio.h"
#include "vfs_api.h"
#include "usb_api.h"
#include "sysg_api.h"
#include "audio_api.h"
#include "hw_disp_ext.h"
#include "osd_main.h"
#include "osd_common.h"
#include "osd_api.h"
#include "panel.h"
#include "exif_custom.h"
#include "snap_custom.h"
#include "zoom.h"
#include "pub_api.h"
#include "mp3_api.h"
#include "wma_api.h"
#include "gpio.h"
#include "hw_cpu.h"
#include "timer.h"
#include "xiaoao.h"
#include "hw_aud.h"
#include "UiStrings.h"
#include "extern.h"
#include "ws9712.h"
#include "ui_mp3.h"
//#include "mp3_scr.h"
#include "menu_capture.h"

void	pbMP3Refresh(UINT8 isClear);
//void	pbMP3Init(UINT32, UINT32, UINT32, UINT32);
void	pbMP3Play(UINT32);
UINT32 	pbAudioInfoGet(UINT32 idx);
void 	ui_ShowAudioMenuInfo(void);

void osdaudiotimeShow(void );			// display time & progress in mp3 play
UINT32	mp3play;

UINT8 	ui8PlayMp3Zero_Flag = 1 ; //tzwadd135
UINT8   ui8PlayMp3Zero_Num  = 0 ;
static	void *semApp;

UINT8 g_strFileErr[] = "File Err";
SINT32 mp3Vol ;//= 9; //wendy close for record it from nandflash
SINT32 ui32Mp3VolSave ;
UINT32 wmaVol = 0;
UINT8  mp3DispCol, mp3DispRow;
UINT16 mp3DispStart;
UINT8  mp3Repeat = MP3RepeatAll;
UINT32 mp3Disp = PB_DISP_NINE;
UINT8   audioErr = 0;
/*UINT8  wavShortBreak = 0;
UINT8  wavPlayEnd =0;*/
UINT8     ui8SaveVol =0; //for bug 8

UINT8	ui8Mp3ListNum ;
UINT32	ui32CurValumX ;
UINT32	ui32CurProgressX ;
UINT8	ui8FW_REWFlag ;

#if GAOQING_A036
#define MP3_LIST_NAME_X   		68-6+34
#define	MP3_LIST_MAX			3
#define	MP3_LIST_STR_X 			68-6     //    fqdao_modify  06.4.25
#define	MP3_LIST_STR_Y			140+4
#define	MP3_LIST_Y_SIZE			22
#define	MP3_LIST_BAR_X			56  
#define	MP3_LIST_BAR_W			242
#define   LOCK_X    			272+10

#define   MP3_FILECOUNT_X    	20   //24    fqdao_modify  06.4.25
#define   MP3_FILECOUNT_Y    	140
#define   MP3_FILECOUNT_H    	46


#define	MP3_VOLUME_H			6
#define	MP3_VOLUME_W			18
#define	MP3_VOLUME_X			282 
#define	MP3_VOLUME_Y			54-8

#define	MP3_PROGRESS_STR_X		8
#define	MP3_PROGRESS_STR_Y		118   
#define	MP3_PROGRESS_END_X		(300+MP3_PROGRESS_STR_X-28+6)
#define	MP3_PROGRESS_END_Y		88
#define	MP3_EQ_X				40 + 120
#define	MP3_EQ_Y				72+4
#define MP3_EQFlag_X     		160-120        //  流行,古典方式的坐标
#define MP3_EQFlag_Y      		92-4  
#define MP3_FILENAME_X     		40 
#define MP3_FILENAME_Y			60-4

#define C_TitilTex_X			38
#define C_TitilTex_Y			3
#define TitilTexColor			0xd0
#define	C_OtherInf_X			242
#define	C_OtherInf_Y			198         
#define	C_SelctColor			0xdf	
#define	C_NormalColor			0xf0
#else
#define MP3_LIST_NAME_X    	68-6+34 //fqdao_add 
#define	MP3_LIST_MAX		3
#define	MP3_LIST_STR_X 		68-6  //fqdao_modify 06.5.6
#define	MP3_LIST_STR_Y		140+4
#define	MP3_LIST_Y_SIZE		22
#define	MP3_LIST_BAR_X		56
#define	MP3_LIST_BAR_W		242
#define LOCK_X    			272+10

#define MP3_FILECOUNT_X    	22  //fqdao_modify 06.5.6
#define MP3_FILECOUNT_Y    	140
#define MP3_FILECOUNT_H    	46

#define	MP3_VOLUME_H		6
#define	MP3_VOLUME_W		18
#define	MP3_VOLUME_X		284
#define	MP3_VOLUME_Y		54-8

#define	MP3_PROGRESS_STR_X	8
#define	MP3_PROGRESS_STR_Y	117
#define	MP3_PROGRESS_END_X	(300+MP3_PROGRESS_STR_X-28+6)
#define	MP3_PROGRESS_END_Y	88
#define	MP3_EQ_X			40 + 120
#define	MP3_EQ_Y			72+4
#define MP3_EQFlag_X     	160-120        //  流行,古典方式的坐标
#define MP3_EQFlag_Y      	92-4  
#define MP3_FILENAME_X    	40 
#define MP3_FILENAME_Y		60-4

#define C_TitilTex_X		38
#define C_TitilTex_Y		3
#define TitilTexColor		0xd0
#define	C_OtherInf_X		242
#define	C_OtherInf_Y		198         
#define	C_SelctColor		0xdf	
#define	C_NormalColor		0xf0
#endif

UINT8 str[MP3_LIST_MAX][13];

UINT32 timeElapsed = 0 , timeTotal= 0, timeleft=0;

struct mp3TagInfo  mp3_Info;
UINT32 sampleRate;

extern audFileInfo_t audioProfile;

enum	
{
	PROGRESS_RST= 0,
	PROGRESS_MOV,
	PROGRESS_VOL,
	MP3_PLAY_STATUS,
	MP3_EQ_DIS,
	MP3_EQ_RST,
	MP3_LRC,
	MP3_EQFlag
	
};



UINT8 	ui8Mp3StatusSave = 0 ;
UINT8	ui8Mp3EQCounter = 0 ;
UINT8	ui8Mp3EQSizeSave = 0;

#define	MP3EQ_FALLSIZE 	3

extern UINT32  ui32EQFlag;	/* Paul@2006/02/06 add */
extern UINT8  F_osdLRCDisp(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option);

void F_CutLongNameToShortName(UINT8 *pui8str,UINT32 ui32Len); //xqq 
/**************************************************************************
 *                                                                        *
 *  Function Name: vfsLongFileNameGet                                     *
 *                                                                        *
 *  Purposes:                                                             *
 *    Get the corresponding long file name by a 8.3 filename              *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    pname: in, pointer to the name string (8.3)                         *
 *                                                                        *
 *  Returns:                                                              *
 *    NULL: the specified file has no associated long filename            *
 *    otherwise: the pointer to the long filename                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *

**************************************************************************/
extern UINT8* vfsLongFileNameGet(UINT8 *pname);
extern UINT32 file_filter;
extern void  pbVolumWindowRfsh(SINT32 value);
extern UINT8 UI_OSQFlush(OS_EVENT *pevent);
extern void  dirInit();

void pbRepeatWindowRfsh(void)
{
	UINT32	osdW, osdH;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	if (mp3Repeat == MP3RepeatOne)
		osdStrDisp(156, 2, UserIcon10x20, 0x0, "\x5e\x5f");
	else if (mp3Repeat == MP3RepeatAll)
		osdStrDisp(156, 2, UserIcon10x20, 0x0, "\x3e\x3f");
	else /*random*/
		osdStrDisp(156, 2, UserIcon10x20, 0x0, "\x1e\x1f");
}

/***************************************************************
***************************************************************/
void  F_DisplayLoadString(UINT8 *LoadString)
{
	//static UINT8  LoadStr_X , LoadStr_Y ;
	switch(sPara.language)
	{
		case LanguageRussian:
			osdStrDisp((PANEL_1_OSD_W-9*10)/2, 160, UserFont10x20, 0x10, LoadString);  
			break ;
		default :			
			osdStrDisp ( (PANEL_1_MAX_X-20*strlen(LoadString) )/2, 160, UserFont10x20, 0x10, LoadString);//xu
			break ;
	}
}

/***********************/
#define	FILE_MAXCHAR 	23-8

extern UINT8* vfsLongFileNameGet(UINT8 *pname);
extern UINT8 mp3GetVersion(UINT8 *);
UINT32 ui32totalTimeSave[1024] ;
/***************************************************************
bgrefresh==0
bgrefresh==1		refresh bkground
bgrefresh==2		refresh bkground & scan file
***************************************************************/
extern void pbShowMP3RecordBG() ;
void pbShowFileNum(UINT32 filePos1,UINT32 filecnt1 )
{
	UINT32  ui32_y ;
	ui32_y = MP3_FILECOUNT_Y + (MP3_FILECOUNT_H* filePos1)/filecnt1 ;
	osdStrDraw(MP3_FILECOUNT_X,ui32_y, UserFont10x8_4, 0x00, "\x4");	
}


/**
**/
void mp3FileTotalTimeGet(UINT32 startFileIdx, UINT32 endFileIdx)
{
	UINT32 err;
	UINT32 i;
	UINT32 type;
	xdcfAttrElm_t attr;
	audFileInfo_t audio_Profile_t;	

	i = startFileIdx;
	while (i <= endFileIdx)
	{
		xdcfCurFileByPosSet(i);
		err = xdcfCurFileAttrGet(&attr);
		err = xdcfFileTypeGet(i, &type);
		switch(type)
		{
			case xDCF_FILETYPE_MP3:
				ui32totalTimeSave[i] = 0;//mp3GetTotalTime(attr.name);
				break;
				
			case xDCF_FILETYPE_WMA:
				ui32totalTimeSave[i] = 0;//wmaGetTotalTime(attr.name);
				break;
				
			case xDCF_FILETYPE_WAV:
				audAppFileInfoGet(attr.name,&audio_Profile_t);
				ui32totalTimeSave[i] = audio_Profile_t.residualTime;	
				break;
		}
		i++;
	}
}
/**
**/
void pbFileListWindowRfsh(UINT32 filePos,UINT8  bgrefresh)
{
	/*static UINT8 str[FILELIST_MAXLINE][13];*/
	UINT32 y[MP3_LIST_MAX] = {MP3_LIST_STR_Y, MP3_LIST_STR_Y+MP3_LIST_Y_SIZE, 
							MP3_LIST_STR_Y+MP3_LIST_Y_SIZE*2};	
	
	//UINT8 space[29];
	UINT8 space[FILE_MAXCHAR];
	UINT8 filename[FILE_MAXCHAR] ;
	UINT32 i,j;
	UINT32 err;

	UINT32	osdW, osdH;
	static UINT32  page_idx = 0;
	xdcfAttrElm_t attr;
	UINT32 type;
	audFileInfo_t audio_Profile_t;
	UINT32	ui32totalTime = 0 ;

	UINT8  ui8LongName[256];
	UINT8  *pLongNamePtr ;	
	UINT8  aui8Temp[10];
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	
	memset(space, ' ', FILE_MAXCHAR-1);
	space[FILE_MAXCHAR-1] = '\0';

	page_idx = ((filePos-1)/MP3_LIST_MAX)*MP3_LIST_MAX +1;
	
	osdFWBuffChange ();
	osdClearScreen (0);

	hisIconDisp	();
	hisTimeDisp	();
	F_Mp3IconRefresh(PROGRESS_RST) ;
	F_Mp3IconRefresh(PROGRESS_VOL) ;
	ui8Mp3EQCounter = 0  ; 
	F_Mp3IconRefresh(MP3_EQ_DIS) ;
	F_Mp3IconRefresh(MP3_EQFlag) ;
		
	if(bgrefresh)
	{
		UINT32 dispW, dispH;
		UINT32 gpb;
		dispW = PIP_W;
		dispH = PIP_H;
			
		pbShowMP3RecordBG();
		osdStrDisp(C_TitilTex_X, C_TitilTex_Y, UserFont10x20, TitilTexColor, F_Mp3GetString(title_mp3_s));
	}
	
	osdStrDisp(C_TitilTex_X, C_TitilTex_Y, UserFont10x20, TitilTexColor, F_Mp3GetString(title_mp3_s));
	
	if(fileCnt==0)
	{
		osdStrDisp(126,170, UserFont10x20, 0x10/*C_WarringColor*/, F_Mp3GetString(no_audio_s));  
		
		osdHWBuffChange ( );
		menuReturn(UI_MAINMENU, 0);				
   	    	osTimeDly(100);  		
		return	0 ;
	}

	osdHWBuffChange();
	
	if(bgrefresh==2)
	{
		if(fileCnt>4) 
		{
			j=4;
			osdStrDisp ( (PANEL_1_MAX_X-10*strlen(F_Mp3GetString(mp3_Loading)) )/2, 160, UserFont10x20, C_NormalColor, F_Mp3GetString(mp3_Loading));//xu
		}
		else 
		{
			j = fileCnt;
		}		
		mp3FileTotalTimeGet(1, j);
		
		if(fileCnt>4) 
		{
			mp3FileTotalTimeGet(4, fileCnt);
		}
	}

	if (filePos != 0)
	{
		for(i=0; i<MP3_LIST_MAX; i++)
		{
				
				osdBarDraw(MP3_LIST_BAR_X,y[i]-1, MP3_LIST_BAR_W, MP3_LIST_Y_SIZE,0x00) ;
		
				if(i <= fileCnt-page_idx)
				{
					ui32totalTime = 0;					
					xdcfCurFileByPosSet(i+page_idx);
					err = xdcfFileTypeGet(i+page_idx, &type);
					err = xdcfCurFileAttrGet(&attr);
					
					if(SUCCESS == err)
					{
						if (ui32totalTimeSave[i+page_idx] == 0)
						{
							switch(type)
							{
								case xDCF_FILETYPE_MP3:
									/*
									
									if (mp3_CheckFileFormat(attr.name))
									{
										break;
									}		
									*/
									ui32totalTimeSave[i+page_idx] = mp3GetTotalTime(attr.name);
									break;
									
								case xDCF_FILETYPE_WMA:
									ui32totalTimeSave[i+page_idx] = wma_get_total_time(attr.name);//wmaGetTotalTime(attr.name);
									break;
									
								case xDCF_FILETYPE_WAV:
									break;
								
							}
						}
						ui32totalTime = ui32totalTimeSave[i+page_idx];						

						pLongNamePtr = vfsLongFileNameGet(attr.name);	
						printf("=====%s\n",pLongNamePtr);
						if(pLongNamePtr==NULL)
							pLongNamePtr = attr.name;	
						strcpy(ui8LongName,pLongNamePtr);
						pLongNamePtr = ui8LongName ;
						*(pLongNamePtr+strlen(pLongNamePtr)-4) = '\0' ;
						#if 0
						if(strlen(pLongNamePtr)>=FILE_MAXCHAR-1)
						{
							strncpy(filename, pLongNamePtr,FILE_MAXCHAR-1);
							filename[FILE_MAXCHAR-1-4] = '\0' ;
							strcat(filename,"...");
						}
						#endif
						//add by xu for bug 19 2006 4 25
						if(strlen(pLongNamePtr)>FILE_MAXCHAR-1) 
						{
							strncpy(filename, pLongNamePtr,FILE_MAXCHAR-1);
							F_CutLongNameToShortName(filename,FILE_MAXCHAR-1);
						}
						//add by xu for bug 19 2006 4 25
						else
						{
							//strncpy(filename, pLongNamePtr,FILE_MAXCHAR-1);
							strcpy(filename, pLongNamePtr);
						}
					}
					else
					{
						strncpy(filename, space, FILE_MAXCHAR-1);
					}						
					sio_psprintf(str, "%02d:%02d", ui32totalTime/1000/60%60, ui32totalTime/1000%60);
					
					sio_psprintf(aui8Temp,"%03u%c",i+page_idx,'\0');
					if(i+page_idx ==filePos) 
					{

				 	 	osdBarDraw(MP3_LIST_BAR_X,y[i]-2, MP3_LIST_BAR_W, MP3_LIST_Y_SIZE,0x0f) ;
		            			//pbImagePaste(mp3Selectbar[0][0],mp3Selectbar[0][1],MP3_LIST_BAR_X,135+MP3_LIST_Y_SIZE*i);
						osdStrDisp(MP3_LIST_BAR_X+2, y[i], UserFont10x20, C_SelctColor, aui8Temp);		
						osdStrDisp(MP3_LIST_STR_X+42, y[i], UserFont10x20, C_SelctColor, filename);				
                              	      osdStrDisp(C_OtherInf_X, y[i] , UserFont10x20, C_SelctColor, str);   
						// tzw add 060302
						pbShowFileNum(filePos,fileCnt) ;
						osdStrDisp(MP3_FILENAME_X, MP3_FILENAME_Y, UserFont10x20,0x00, space) ;
						osdStrDisp(MP3_FILENAME_X, MP3_FILENAME_Y, UserFont10x20,0xd0, filename) ;
						//end 
						if(attr.attribute&xDCF_ATTR_READONLY)
						{
							osdStrDisp(MP3_LIST_BAR_X+32, y[i]+4 , IconLock, C_SelctColor,"\x1");
						}
						else
						{
							osdStrDisp(MP3_LIST_BAR_X+32, y[i]+4 , IconLock, 0xf," ");
						}
						/*
						*/
					}
					else	 
					{

		                    	osdBarDraw(MP3_LIST_BAR_X,y[i]-2, MP3_LIST_BAR_W, MP3_LIST_Y_SIZE,0x00) ;
		                		//pbImagePaste(mp3Selectbar[1][0],mp3Selectbar[1][1],MP3_LIST_BAR_X,135+MP3_LIST_Y_SIZE*i);					
						osdStrDisp(MP3_LIST_BAR_X+2, y[i], UserFont10x20, C_NormalColor, aui8Temp);				
						osdStrDisp(MP3_LIST_STR_X+42, y[i], UserFont10x20, C_NormalColor, filename);				
			             	osdStrDisp(C_OtherInf_X, y[i], UserFont10x20, C_NormalColor, str);
						//tzw add 060303
						//osdStrDisp(MP3_LIST_STR_X, y[i], UserFont10x20, 0xF0, filename);				
						//osdStrDisp(LOCK_X-50, y[i], UserFont10x20, 0xf0, str);			
						// icon of protect file
						if(attr.attribute&xDCF_ATTR_READONLY)
						{
							//osdStrDisp(LOCK_X, y[i]+3, IconLock, 0xf0,"\x1");
							osdStrDisp(MP3_LIST_BAR_X+32, y[i]+4 , IconLock, C_NormalColor,"\x1");
						}
						else
						{
							osdStrDisp(MP3_LIST_BAR_X+32, y[i]+4, IconLock, 0x00," ");
						}
						
					}
					
				}	
				
		}
 	}	
	UI_OSQFlush(uiKeyMsgQ);
//	osdHWBuffChange(); // tzw add 060225
}


void	F_Mp3IconRefresh(UINT8 ui8Mp3IconFlag)
{
	UINT8	i,j ;
	UINT8	str[8] ;
	UINT32	osdW, osdH;	
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;	
	if( ( uiState & UI_MODE_MASK ) == UI_MODE_MP3 )
	{
		if(mp3Repeat == MP3RepeatOne)
			osdStrDisp(156, 2, UserIcon10x20, 0x0, "\x5e\x5f");
		else if(mp3Repeat == MP3RepeatAll)
			osdStrDisp(156, 2, UserIcon10x20, 0x0, "\x3e\x3f");
		else /*random*/
			osdStrDisp(156, 2, UserIcon10x20, 0x0, "\x1e\x1f");
	}	
		
	if(ui8Mp3IconFlag == PROGRESS_RST)
	{
		osdBarDraw(MP3_PROGRESS_STR_X , MP3_PROGRESS_STR_Y,MP3_PROGRESS_END_X-MP3_PROGRESS_STR_X+40,
					16, 0x00);
		osdStrDraw(MP3_PROGRESS_STR_X , MP3_PROGRESS_STR_Y, UserFont10x8_4, 0x00, "\x1");

	}
	else if(ui8Mp3IconFlag == PROGRESS_MOV )
	{
		F_ProgressDis();
		hisTimeDisp();
		RefurbishBatterGaugeFunc();
	}
	else if(ui8Mp3IconFlag == PROGRESS_VOL)
	{
		osdBarDraw(MP3_VOLUME_X, 42, 20, 56, 0x00) ;
	//	osdStrDisp(MP3_VOLUME_X , MP3_VOLUME_Y, UserFont10x20, 0x00, "       ");
		// set volum > 7 icon red. David 12/22/2005
		for ( i = mp3Vol; i >0;  i --) 
		{
			if(i> 7)
			{
				osdStrDraw(MP3_VOLUME_X, MP3_VOLUME_Y+(9-i)*MP3_VOLUME_H, UserFont10x8_4, 0x00,"\x3");	
			}
			else
			{
				osdStrDraw(MP3_VOLUME_X, MP3_VOLUME_Y+(9-i)*MP3_VOLUME_H, UserFont10x8_4, 0x00, "\x2");	
			}
		}
		
	}
	else if(ui8Mp3IconFlag==MP3_PLAY_STATUS)
	{
		if(g_playrecStatus != ui8Mp3StatusSave)
		{
			ui8Mp3StatusSave = g_playrecStatus;
			switch (g_playrecStatus)
			{
				case	STATUS_STOP:
					osdStrDisp(osdW/2+24 , 2, UserIcon10x20, 0x30, "\x57\x58");
					break;
				case	STATUS_PLAYING:
					osdStrDisp(osdW/2+24 , 2, UserIcon10x20, 0x40, "\x8e\x8f");
					break;
				case	STATUS_PAUSE:
					osdStrDisp(osdW/2+24, 2, UserIcon10x20, 0x30, "\x90\x91");
					break;
				case	STATUS_FF:
					osdStrDisp(osdW/2+24, 2, UserIcon10x20, 0x40, "\x8a\x8b");
					break;
				case	STATUS_REW:
					osdStrDisp(osdW/2+24, 0, UserIcon10x20, 0x40, "\x8c\x8d");
					break;
				default:
					osdStrDisp(osdW/2+24, 0, UserFont10x20, 0x0, "  ");
					break;
			}
		}
	}	
	else if(ui8Mp3IconFlag==MP3_EQ_DIS)
	{
	UINT8	ui8TempSave ;
	UINT8	ui8Temp ;
		if(ui8Mp3EQCounter%2==0)
		{
			ui8TempSave = ui8Mp3EQSizeSave ;

			for(i=0;i<24;i++)
			{
				j = rand()%32 ;
				if(j>=ui8TempSave)
				{
					ui8Temp = j - ui8TempSave ;
					if(ui8Temp>MP3EQ_FALLSIZE) j = ui8TempSave + MP3EQ_FALLSIZE ;
				}
				else
				{
					ui8Temp = ui8TempSave -j ;
					if(ui8Temp>MP3EQ_FALLSIZE) j = ui8TempSave - MP3EQ_FALLSIZE ;					
				}
				ui8TempSave = j ;
				if(i==0) ui8Mp3EQSizeSave = j ;
				str[0] = j ;
				str[1] = '\0' ;
				osdStrDraw(MP3_EQ_X+4*i,MP3_EQ_Y,USERICON_EQ,0x00,str) ;
			}
		}
		ui8Mp3EQCounter++ ;	
	}
	else	 if(ui8Mp3IconFlag==MP3_EQ_RST)
	{
	#if 0
			for(i=0;i<24;i++)
			{
				j = 0 ;
				str[0] = j ;
				str[1] = '\0' ;
				osdStrDraw(MP3_EQ_X+4*i,MP3_EQ_Y,USERICON_EQ,0x00,str) ;
			}
			
	#endif			
	#if 0  //tzw add 060302
 			osdBarDraw(MP3_EQ_X,MP3_EQ_Y, 4*24, 32, 0x00) ;
			osdBarDraw(160,48, 170, 20, 0x00) ;
	#endif 		
	}
	else if(ui8Mp3IconFlag == MP3_LRC)
	{
		F_LRCDis();
	}
	else if(ui8Mp3IconFlag == MP3_EQFlag )
	{
		osdStrDisp(MP3_EQFlag_X, MP3_EQFlag_Y, UserFont10x20, 0x0, "           ");
		osdStrDisp(MP3_EQFlag_X, MP3_EQFlag_Y, UserFont10x20, 0xd0, F_Mp3GetString(ui32EQFlag+EQFlat_s));

	}
}


void	F_ProgressDis()
{
			UINT32	ui32ProgressX_Temp ;
		
			
				timeTotal  = ui32totalTimeSave[fileIdx] ;

				if (avPlayNow ==OBJ_WAV)
				{
					audAppFileInfoGet(NULL, &audioProfile);
					timeTotal = audioProfile.playDuration;
					timeElapsed = audioProfile.playDuration - audioProfile.residualTime;
					timeleft =audioProfile.residualTime;
					if (timeTotal == 0) /* wav<2k timetotal=0,  for osdHProgressDraw*/
					{
						timeTotal = 20;
						timeElapsed =10;
					}
					wavTime = timeElapsed;
				}

				else if (avPlayNow ==OBJ_MP3)
				{
//					timeTotal = mp3GetTotalTime(fileattr.name);
					timeElapsed = mp3GetElapsedTime();
					timeleft = timeTotal-timeElapsed;
				}
				else
				{
//					timeTotal = wmaGetTotalTime(fileattr.name);
					timeElapsed = wmaGetElapsedTime();
					timeleft = timeTotal-timeElapsed;
				}

				
				if (timeElapsed>timeTotal)
				{
					timeElapsed=timeTotal;
					timeleft=0;
				}
				
//				osdHProgressDraw(0, timeTotal/*100*/, timeElapsed/*mp3PlayProgressGet()*/, 0x7c);
//				sio_psprintf(str, "%02d:%02d ", timeElapsed/1000/60, timeElapsed/1000%60);
//				osdStrDisp(osdW/6-20, osdH-62-13, UserFont16x20, 0xd0, str);
//				sio_psprintf(str, "%02d:%02d ", timeleft/1000/60, timeleft/1000%60);
//				osdStrDisp(3*osdW/4-10, osdH-62-13, UserFont16x20, 0xd0, str);
				ui32ProgressX_Temp = (MP3_PROGRESS_END_X-MP3_PROGRESS_STR_X)*timeElapsed/timeTotal ;

                   /*fqdao Add for bug 22 Start   06.4.27*/
			#if 0
			if(ui8FW_REWFlag==1) 
			{
				return ;
			}
			#else
		      if (avPlayNow == OBJ_MP3)
		      	{
			       if(g_playrecStatus == STATUS_FF )
			       {   
			             if(ui32CurProgressX>=ui32ProgressX_Temp)
			             {
			                ui32CurProgressX =ui32ProgressX_Temp ;
						return ;
			             }
					if((ui32ProgressX_Temp-ui32CurProgressX)>8)
					{
						return ;		
					}
			       }
				 if(g_playrecStatus == STATUS_REW)
				 {
					 if((ui32CurProgressX-ui32ProgressX_Temp)>8)
					{
						return ;		
					}
				 }
		      	}
			  #endif 
                   /*fqdao Add for bug 22  End     06.4.27*/

				
				if(ui32CurProgressX!=ui32ProgressX_Temp)
				{
//					osdStrDisp(MP3_PROGRESS_STR_X , MP3_PROGRESS_STR_Y, UserFont10x20, 0x00, "                              ");
					osdBarDraw(MP3_PROGRESS_STR_X , MP3_PROGRESS_STR_Y,MP3_PROGRESS_END_X-MP3_PROGRESS_STR_X+40,
										16, 0x00);

					ui32CurProgressX = ui32ProgressX_Temp ;

					osdStrDraw(MP3_PROGRESS_STR_X+ui32CurProgressX,MP3_PROGRESS_STR_Y,UserFont10x8_4,0x00,"\x1");
				}

}

void	F_LRCDis()
{
			switch(fileType)		
			{
				case	xDCF_FILETYPE_MP3:
					timeElapsed = mp3GetElapsedTime();
					timeleft = timeTotal-timeElapsed;
					break ;
				case	xDCF_FILETYPE_WAV:
					audAppFileInfoGet(NULL, &audioProfile);
					timeElapsed = audioProfile.playDuration - audioProfile.residualTime;
					timeleft =audioProfile.residualTime;					
					break ;
				case	xDCF_FILETYPE_WMA:
					timeElapsed = wmaGetElapsedTime();
					timeleft = timeTotal-timeElapsed;
					break ;
			}

					UINT8 tt;
					
					if  (mp3LRCTotalGet())
					{
						#if 0
						printf("%s\n", mp3LRCAddrTab[i]);
						#endif
						osTimeDly(1);
						mp3LRCGet(timeElapsed, str, 52/*64*/);	
						//xu add for bug 30 2006 05 09
					        if(str[0][0]!='\0')
					        {
								//printf("%sabcd\n",str);
								osdBarDraw(MP3_LIST_STR_X,MP3_LIST_STR_Y+ MP3_LIST_Y_SIZE*1,PANEL_1_OSD_W-MP3_LIST_STR_X,MP3_LIST_Y_SIZE*2,0x00);
					        }
						//xu add for bug 30 2006 05 09
				        	tt = F_osdLRCDisp(MP3_LIST_STR_X, MP3_LIST_STR_Y+ MP3_LIST_Y_SIZE*1, UserFont10x20, 0x40, str, 0,  22,	0 );// tzw add 060420					            		
					   	tt = F_osdLRCDisp(MP3_LIST_STR_X, MP3_LIST_STR_Y+ MP3_LIST_Y_SIZE*2, UserFont10x20, 0x40, str, tt, 22, 0);// tzw add 060420	
					}
			
}

#define 		FONT_HZ_J_PAGES		1000
#define 		FONT_HZ_J_SIZE		(FONT_HZ_J_PAGES*512)
#define 		FONT_HZ_F_PAGES		2000
#define 		FONT_HZ_F_SIZE		(FONT_HZ_F_PAGES*512) 


extern	UINT8	HANZIFlag ;
extern UINT8 		UserFontHZ_J[FONT_HZ_J_SIZE];
extern  UINT8 		UserFontHZ_F[FONT_HZ_F_SIZE];
#if 0 
UINT8  F_osdLRCDisp(UINT32 x, UINT32 y, UINT8* pFont, UINT32 bgColor, UINT8 str[], UINT8 start, UINT8 lgh, UINT8 option)
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
			osTimeDly(2);
		hisTimeDisp();//machiner 2005-10-29
		if (avPlayNow ==OBJ_NONE && uiUSBStatus == 1 ) //for bug 31 2005-10-17
		{
			break;
		}					 
#endif		
		if(str[i]<=0x80)
		{
//			if(HANZIFlag)
//		 		break;	
	
				str8[0] = str[i++];
				osdStrDraw(x+xOffset, y+yOffset, pFont, bgColor, str8);
				xOffset += pFont[2];		
		}
		else if((str[i]>0x80) && (str[i]<=0xa0))
		{	 	 
				str8[0] = str[i++];
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

/*****************************************************************
*****************************************************************/
#define	xDCF_ATTR_PROTECT		xDCF_ATTR_READONLY |fileattr.attribute
#define	xDCF_ATTR_DISPROTECT		(~xDCF_ATTR_READONLY) &fileattr.attribute


#define	MP3_MENU_X	64
#define	MP3_MENU_Y	32


#define MP3_BAR_X				MENUBAR_X
#define MP3_BAR_Y				MENUBAR_Y
#define MP3_BAR_WIDTH     	       MENUBAR_W
#define MP3_BAR_HEIGHT     		MENUBAR_H

#define MP3_TITLE_OFF_X			MENUTITLEX
#define MP3_TITLE_OFF_Y			MENUTITLEY


#define MP3_TEXT_OFF_X      		MENUTEXT_X
#define MP3_TEXT_OFF_Y     		MENUTEXT_Y
#define MP3_TEXT_HEIGHT     		MENUTEXT_Y_OFF

#define MP3_ICON_X      			ICON_X
#define MP3_ICON_Y				ICON_Y
#define MP3_ICON_OFF_X			ICON_X_OFF

enum
{
	MP3_MENU_DEL = 0,
	MP3_MENU_PROTECT
};

enum
{
	MP3_DEL_TITLE = 0,
	MP3_DEL_CUR ,
	MP3_DEL_ALL
};


#define	MP3_ITEM_MAX  4+1
#define	MP3_SUBITEM_MAX  3+2
#define	MP3_ITEM_LIST_MAX 4

UINT8	Mp3SubMax[MP3_ITEM_MAX] = 
{2,2,2,2,4};
#if 0
UINT32	Mp3ItemImage[4][2] = 
{
	{bkground,sizeof(bkground)},
	{icon_rec,sizeof(icon_rec)},
	{icon_pro,sizeof(icon_pro)},
	{icon_dpr,sizeof(icon_dpr)},
};
#endif
/*
UINT32	Mp3SubItemImage[MP3_ITEM_MAX][] = 
{
	
};
*/ //268 269  same
UINT8 *Mp3Strings[MP3_ITEM_MAX][MP3_SUBITEM_MAX][LANGUAGES] = 
{
	#if 1
	{
		{
		"Delete",
		"删除",
		"埃",
		"Borrar",
		"Удалить",
		"Entfernen",
		"supprimer",
		"Elimina",
		"Deletar",
		"昏力",
		},
		{
		"Current",
		"当前",
		"讽玡",
		"Actual",
		"Нынче",
		"Aktuell",
		"présent",
		"Corrente",
		"Atual",//"Corrente",
		"泅犁",
		},
		{
		"All",
		"全部",
		"场",
		"Todo",
		"Все",
		"Alles",
		"tous ",
		"Tutti",
		"Todos",
		"傈何",
		}
	},
	{		 
		{
		"Protect"	,
		"保护"	,
		"玂臔"	,
		"Proteger",
		"Защитить",
		"Schutz",
		"protéger",
		"Proteggi",//"Protezione",
		"Proteger"/*"Protectar"*/,
		"焊龋",
		},
		{
		"Current",
		"当前",
		"讽玡",
		"Actual",
		"Нынче",
		"Aktuell",
		"présent",
		"Corrente",
		"Atual",//"Corrente",
		"泅犁",
		},
		{
		"All",
		"全部",
		"场",
		"Todo",
		"Все",
		"Alles",
		"tous ",
		"Tutti",
		"Todos",
		"傈何",
		}
	},
	{
		{
		"Unprotect"	,
		"取消保护"	,
		"玂臔"	,
		"Unproteger",//"Cancelar protección",
		"Отменить защиту",
		"  Schutzentfernen",
		"non protégé",//"non protéger ",
		"Elimina Protezione	",
		"Fora de protec\xe7\xe4o",
		"攫橇肺咆飘",
		},
		{
		"Current",
		"当前",
		"讽玡",
		"Actual",
		"Нынче",
		"Aktuell",
		"présent",
		"Corrente",
		"Atual"/*"Corrente"*/,
		"泅犁",
		},
		{
		"All",
		"全部",
		"场",
		"Todo",
		"Все",
		"Alles",
		"tous ",
		"Tutti",
		"Todos",
		"傈何",
		}
	},	
	{
		{
		"Play Mode"	,
		"播放模式"	,
		"冀家Α"	,
		"Modo de Repro",//"Modo reproducción",
		"Форма проигрывания",
		"Play Modus",
		"mode lecture",
		"Modalit\xe0 Play",//"Modo Play",
		"Tocar"/*"Modelo de jogar"*/,
		"敲饭捞葛靛",
		},
		{
		"Repeat Current",
//		"Repeat One",
		"单曲循环",
		"虫Ρ碻吏",
		"Repetición una",
		"Оборот Нынче",
	//	"Циркуляция единичной песни",
		"Einyel wdh",
		"répéter 1x",//"répéter une",
		"Ripeti uno",
		"Repetir um cant\xe3o",
		"窜邦府乔飘 ",
		},
		{
		"Repeat All",
		"循环播放",
		"碻吏冀",
		"Repetición toda",
		"Оборот Все",
	//	"Циркуляционное проигрывание",
		"Alles wdh",
		"répéter tout",//"répéter toutes ",
		"Ripeti Tutti",
		"Repetir um ciclo",
		"鉴券犁积",
		},
	},	
	{
		{
		//"Auto Close LCD",tzw8
		"Auto Close LCD",
		"自动关屏",
		"笆闽",
		"Auto Apagamiento",
		//"Авто LCD",
		"Auto Close LCD",//"Auto Apagamiento",
		"Auto Close LCD",
		"Auto Close LCD",
		"Schermo auto",
		"Auto-desligar",
		"胶农赴 磊悼 摧扁",
		},
		{
		"Never",
		"从不 ",
		"眖ぃ ",
		"Nunca",
		"Никогда",
		"nie",
		"jamais",
		"Non attivo",//"Mai",
		"Nunca",
		"傈盔阑 掺瘤 臼绰促",
		},
		{
		"10 S",
		"10 秒",
		"10 ",
		"10 segundos",
		"10 секунд ",
		"10 S",
		"10 S",
		"10 S",
		"10 segundos",
		"10 檬",
		},
		{
		"30 S",
		"30 秒",
		"30 ",
		"30 segundos",
		"30 секунд ",
		"30 S",
		"30 S",
		"30 S",
		"30 segundos",
		"30 檬",
		},
		{
		"60 S",
		"60 秒",
		"60 ",
		"60 segundos",
		"60 секунд ",
		"60 S",
		"60 S",
		"60 S",
		"60 segundos",
		"60 檬",
		}
	}
	#endif
} ;

void	F_Mp3MenuDel(UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;
	osdFWBuffChange ();
	osdClearScreen(0x00) ;
	pbDisplayMenuWord();
	
	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			color = MENUTITLES;
			osdStrDisp(MP3_TITLE_OFF_X, MP3_TITLE_OFF_Y+MP3_TEXT_HEIGHT*i, UserFont10x20, color, Mp3Strings[0][i][sPara.language]) ;

		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw(MP3_BAR_X, MP3_BAR_Y+MP3_TEXT_HEIGHT*(i-1), MP3_BAR_WIDTH, MP3_BAR_HEIGHT, 0x0e) ;
			osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[0][i][sPara.language]) ;
			
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[0][i][sPara.language]) ;
		}
	}
	osdHWBuffChange ();	
}


void	F_Mp3MenuProtect(UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;
	osdFWBuffChange ();	
	osdClearScreen(0x00) ;
	pbDisplayMenuWord();
	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			color = MENUTITLES;		
			osdStrDisp(MP3_TITLE_OFF_X, MP3_TITLE_OFF_Y+MP3_TEXT_HEIGHT*i, UserFont10x20, color, Mp3Strings[1][i][sPara.language]) ;
		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw(MP3_BAR_X, MP3_BAR_Y+MP3_TEXT_HEIGHT*(i-1), MP3_BAR_WIDTH, MP3_BAR_HEIGHT, 0x0e) ;			
			osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[1][i][sPara.language]) ;
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[1][i][sPara.language]) ;
		}
	}
	osdHWBuffChange ();	
}


void	F_Mp3MenuDisProtect(UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;

	osdFWBuffChange ();	
	osdClearScreen(0x00) ;
	pbDisplayMenuWord();

	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			color = MENUTITLES;		
			osdStrDisp(MP3_TITLE_OFF_X, MP3_TITLE_OFF_Y+MP3_TEXT_HEIGHT*i, UserFont10x20, color, Mp3Strings[2][i][sPara.language]) ;
		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw(MP3_BAR_X, MP3_BAR_Y+MP3_TEXT_HEIGHT*(i-1), MP3_BAR_WIDTH, MP3_BAR_HEIGHT, 0x0e) ;			
			osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[2][i][sPara.language]) ;
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[2][i][sPara.language]) ;
		}
	}
	osdHWBuffChange ();	
}



void	F_Mp3MenuMode (UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;

	osdFWBuffChange ();	
	osdClearScreen(0x00) ;	
	pbDisplayMenuWord();

	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			color = MENUTITLES;		
			osdStrDisp(MP3_TITLE_OFF_X, MP3_TITLE_OFF_Y+MP3_TEXT_HEIGHT*i, UserFont10x20, color, Mp3Strings[3][i][sPara.language]) ;
		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw (MP3_BAR_X, MP3_BAR_Y+MP3_TEXT_HEIGHT*(i-1), MP3_BAR_WIDTH, MP3_BAR_HEIGHT, 0x0e) ;			
			osdStrDisp (MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[3][i][sPara.language]) ;
			if((mp3Repeat + 1) == idx)
			osdStrDisp (MP3_TEXT_OFF_X - 10, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserIcon10x20, 0xbe, "\x56");			
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[3][i][sPara.language]) ;
			if((mp3Repeat + 1) != idx)			
			osdStrDisp(MP3_TEXT_OFF_X - 10, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserIcon10x20, 0xb0, "\x56") ;
		}
	}
	osdHWBuffChange ();	
}
void 	F_Mp3AutoCloseLCD( UINT8 idx )
{

	UINT32	color ;
	UINT8	i,j = 0 ;
	color = 0xf0 ;

	osdFWBuffChange ();	
	osdClearScreen(0x00) ;	
	pbDisplayMenuWord();


	for(i= 0;i< 4 ;i++)
	{
		if(idx < 3 )
		{
			if( i== 0 )
			{
 				color = MENUTITLES;		
				osdStrDisp(MP3_TITLE_OFF_X-12, MP3_TITLE_OFF_Y+MP3_TEXT_HEIGHT*i, UserFont10x20, color, Mp3Strings[4][i][sPara.language]) ;
			}
			else if(i==idx)
			{
				color = 0xde ;
				osdBarDraw(MP3_BAR_X, MP3_BAR_Y+MP3_TEXT_HEIGHT*(i-1), MP3_BAR_WIDTH, MP3_BAR_HEIGHT, 0x0e) ;			
				osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[4][i][sPara.language]) ;
			}
			else	{
				color = 0xf0 ;
				osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[4][i][sPara.language]) ;
			}
		}
		else
		{
			if( i== 0 )
			{
				color = MENUTITLES;		
				osdStrDisp(MP3_TITLE_OFF_X-12, MP3_TITLE_OFF_Y+MP3_TEXT_HEIGHT*i, UserFont10x20, color, Mp3Strings[4][i][sPara.language]) ;
			}
			else if(i+1 ==idx)
			{
				color = 0xde ;
				osdBarDraw(MP3_BAR_X, MP3_BAR_Y+MP3_TEXT_HEIGHT*(i-1), MP3_BAR_WIDTH, MP3_BAR_HEIGHT, 0x0e) ;			
				osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[4][i+1][sPara.language]) ;
			}
			else	{
				color = 0xf0 ;
				osdStrDisp(MP3_TEXT_OFF_X, MP3_TEXT_OFF_Y+MP3_TEXT_HEIGHT*(i-1), UserFont10x20, color, Mp3Strings[4][i+1][sPara.language]) ;
			}
		}
		
			
	}
	osdHWBuffChange ();	


}

typedef	UINT32	(*pMp3SubMenuProc)(UINT8 idx) ;

pMp3SubMenuProc	RefreshSubitem[MP3_ITEM_MAX] = 
{
	F_Mp3MenuDel,
	F_Mp3MenuProtect, 
	F_Mp3MenuDisProtect,
	F_Mp3MenuMode,
	F_Mp3AutoCloseLCD,
};

UINT8   ui8_AutoCloseLCD = 0;  //060306  tzw add have auto close lcd  1 not  for Mp4 is not have auto close
void	F_Mp3MenuDelImage()
{
	UINT32	i,j ;
	UINT32	x,y ;
	x = 24 ;
	y = 196 ;
	UINT32 dispW, dispH;
	UINT32 gpb;
	dispW = PIP_W;
	dispH = PIP_H;	
//	sysgMemDispAddrGet(&gpb);
//	hwDispPreviewSizeGet(&dispW, &dispH);
//	hwPlayBufSizeSet(dispW,	dispH);
//	hwPlayBufAddrSet(gpb);
//	pbDramImgPlay(Mp3ItemImage[0][0], Mp3ItemImage[0][1], gpb, dispW, dispH);
	pbImagePaste(Mp3ItemImage[0][0],Mp3ItemImage[0][1],0,0) ;	

	if(ui8_AutoCloseLCD ) 
	{
		pbImagePaste(Mp3ItemImage[4][0],Mp3ItemImage[4][1],MP3_ICON_X,MP3_ICON_Y) ;	
	}
	else 
	{
		pbImagePaste(Mp3ItemImage[5][0],Mp3ItemImage[5][1],MP3_ICON_X,MP3_ICON_Y) ;	

	}
	pbImagePaste(Mp3ItemImage[1][0],Mp3ItemImage[1][1],MP3_ICON_X+MP3_ICON_OFF_X,MP3_ICON_Y) ;
	pbImagePaste(Mp3ItemImage[2][0],Mp3ItemImage[2][1],MP3_ICON_X+MP3_ICON_OFF_X*2,MP3_ICON_Y) ;
	pbImagePaste(Mp3ItemImage[3][0],Mp3ItemImage[3][1],MP3_ICON_X+MP3_ICON_OFF_X*3,MP3_ICON_Y) ;
	RefreshSubitem[0](1) ;
}
void	F_Mp3MenuProtectImage()
{
	UINT32	i,j ;
	UINT32	x,y ;
	x = 100 ;
	y = 196 ;
	UINT32 dispW, dispH;
	UINT32 gpb;
	dispW = PIP_W;
	dispH = PIP_H;	
	sysgMemDispAddrGet(&gpb);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gpb);
	pbDramImgPlay(Mp3ItemImage[0][0], Mp3ItemImage[0][1], gpb, dispW, dispH);
	pbImagePaste(Mp3ItemImage[1][0],Mp3ItemImage[1][1],MP3_ICON_X,MP3_ICON_Y) ;
	pbImagePaste(Mp3ItemImage[2][0],Mp3ItemImage[2][1],MP3_ICON_X+MP3_ICON_OFF_X, MP3_ICON_Y) ;
	pbImagePaste(Mp3ItemImage[3][0],Mp3ItemImage[3][1],MP3_ICON_X+MP3_ICON_OFF_X*2, MP3_ICON_Y) ;
	pbImagePaste(Mp3ItemImage[4][0],Mp3ItemImage[4][1],MP3_ICON_X+MP3_ICON_OFF_X*3, MP3_ICON_Y) ;		
	RefreshSubitem[1](1) ;
}


void	F_Mp3MenuDisProtectImage()
{
	UINT32	i,j ;
	UINT32	x,y ;
	x = 100 ;
	y = 196 ;
	UINT32 dispW, dispH;
	UINT32 gpb;
	dispW = PIP_W;
	dispH = PIP_H;	
	sysgMemDispAddrGet(&gpb);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gpb);
	pbDramImgPlay(Mp3ItemImage[0][0], Mp3ItemImage[0][1], gpb, dispW, dispH);
	pbImagePaste(Mp3ItemImage[2][0],Mp3ItemImage[2][1],MP3_ICON_X,MP3_ICON_Y) ;
	pbImagePaste(Mp3ItemImage[3][0],Mp3ItemImage[3][1],MP3_ICON_X+MP3_ICON_OFF_X,MP3_ICON_Y) ;
	pbImagePaste(Mp3ItemImage[4][0],Mp3ItemImage[4][1],MP3_ICON_X+MP3_ICON_OFF_X*2, MP3_ICON_Y) ;			
	if(ui8_AutoCloseLCD ) 
	{
		pbImagePaste(Mp3ItemImage[1][0],Mp3ItemImage[1][1],MP3_ICON_X+MP3_ICON_OFF_X*3, MP3_ICON_Y) ;		
	}
	else
	{
		pbImagePaste(Mp3ItemImage[5][0],Mp3ItemImage[5][1],MP3_ICON_X+MP3_ICON_OFF_X*3, MP3_ICON_Y) ;		
	
	}
	RefreshSubitem[2](1) ;
}

void	F_Mp3MenuModeImage()
{
	UINT32	i,j ;
	UINT32	x,y ;
	x = 100 ;
	y = 196 ;
	UINT32 dispW, dispH;
	UINT32 gpb;
	dispW = PIP_W;
	dispH = PIP_H;	
	sysgMemDispAddrGet(&gpb);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gpb);
	pbDramImgPlay(Mp3ItemImage[0][0], Mp3ItemImage[0][1], gpb, dispW, dispH);
	pbImagePaste(Mp3ItemImage[3][0],Mp3ItemImage[3][1],MP3_ICON_X,MP3_ICON_Y) ;
	pbImagePaste(Mp3ItemImage[4][0],Mp3ItemImage[4][1],MP3_ICON_X+MP3_ICON_OFF_X,MP3_ICON_Y) ;			
	if(ui8_AutoCloseLCD ) 
	{
		pbImagePaste(Mp3ItemImage[1][0],Mp3ItemImage[1][1],MP3_ICON_X+MP3_ICON_OFF_X*2, MP3_ICON_Y) ;		

	}
	else 
	{
		pbImagePaste(Mp3ItemImage[5][0],Mp3ItemImage[5][1],MP3_ICON_X+MP3_ICON_OFF_X*2, MP3_ICON_Y) ;		

	}
	//pbImagePaste(Mp3ItemImage[5][0],Mp3ItemImage[1][1],MP3_ICON_X+MP3_ICON_OFF_X*2, MP3_ICON_Y) ;	
	pbImagePaste(Mp3ItemImage[2][0],Mp3ItemImage[2][1],MP3_ICON_X + MP3_ICON_OFF_X*3,MP3_ICON_Y) ;
	RefreshSubitem[3](1) ;		
}
void 	F_Mp3MenuAutoCloseLCDImage()
{
	UINT32	i,j ;
	UINT32	x,y ;
	x = 100 ;
	y = 196 ;
	UINT32 dispW, dispH;
	UINT32 gpb;
	dispW = PIP_W;
	dispH = PIP_H;	
	sysgMemDispAddrGet(&gpb);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gpb);
	pbDramImgPlay(Mp3ItemImage[0][0], Mp3ItemImage[0][1], gpb, dispW, dispH);
	pbImagePaste(Mp3ItemImage[4][0],Mp3ItemImage[3][1],MP3_ICON_X,MP3_ICON_Y) ;
	pbImagePaste(Mp3ItemImage[5][0],Mp3ItemImage[4][1],MP3_ICON_X+MP3_ICON_OFF_X,MP3_ICON_Y) ;			
	pbImagePaste(Mp3ItemImage[1][0],Mp3ItemImage[1][1],MP3_ICON_X+MP3_ICON_OFF_X*2, MP3_ICON_Y) ;	
	pbImagePaste(Mp3ItemImage[2][0],Mp3ItemImage[2][1],MP3_ICON_X + MP3_ICON_OFF_X*3,MP3_ICON_Y) ;
	RefreshSubitem[4](sPara.ui8AutoCloseLCDTime+1) ;	/*fqdao mp3 auto close  screen option  06.5.29*/		

}



typedef	void	(*pMp3MenuProc)(void) ;

pMp3MenuProc	RefreshItem[MP3_ITEM_MAX] = 
{
	F_Mp3MenuDelImage,
	F_Mp3MenuProtectImage,
	F_Mp3MenuDisProtectImage,
	F_Mp3MenuModeImage,
	F_Mp3MenuAutoCloseLCDImage,
};
/* john @02/07/06 */
UINT32 	F_Mp3MenuDelDeal(UINT8	subitem)
{
	if(subitem==1) 
		{
		if (F_Mp3DelSingleFile(fileIdx))
			return 1;
		}
	else if(subitem==2)
		{
		if (F_Mp3DelAllFile())
			return 1;
		}
	return 0 ;
}
UINT32	F_Mp3MenuProtectDeal(UINT8	subitem)
{
	if(subitem==1) 
		{
		if (F_Mp3ProtectSingleFile(fileIdx))
			return 1;
		}
	else if(subitem==2)
		{
		if (F_Mp3ProtectAllFile())
			return 1;
		}
	return 0 ;
}
UINT32	F_Mp3MenuDisProtectDeal(UINT8	subitem)
{
	if(subitem==1) 
		{
		if (F_Mp3DisProtectSingleFile(fileIdx))
			return 1;
		}
	else if(subitem==2)
		{
		if (F_Mp3DisProtectAllFile())
			return 1;
		}
	return 0 ;
}

UINT32 	F_Mp3MenuModeDeal(UINT8	subitem)
{
	if(subitem==1) 
		mp3Repeat = MP3RepeatOne;
	else if(subitem==2)
		mp3Repeat = MP3RepeatAll;
	return 0;

}
/* john @02/07/06 */
UINT32	F_Mp3MenuAutoCloseLCD(UINT8 subitem )
{
	switch(subitem )
	{
		case 1:
			sPara.ui8AutoCloseLCDTime = ui8AutoCloseLCDTimeNever ;
			break;
		case 2:
			sPara.ui8AutoCloseLCDTime  = ui8AutoCloseLCDTime10s;
			break;
		case 3:
			sPara.ui8AutoCloseLCDTime = ui8AutoCloseLCDTime30s;
			break;
		case 4 :
			sPara.ui8AutoCloseLCDTime  = ui8AutoCloseLCDTime60s;
			break; 
	}
	nandWriteParameters();
	return 1;
}
pMp3SubMenuProc  F_SelectedDeal[MP3_ITEM_MAX] = 
{
	F_Mp3MenuDelDeal,
	F_Mp3MenuProtectDeal,
	F_Mp3MenuDisProtectDeal,
	F_Mp3MenuModeDeal,	
	F_Mp3MenuAutoCloseLCD,
};

void	F_Mp3MenuFunc()
{
	UINT32	key ;
	UINT8 item=0,	subitem=1	 ;
	xdcfAttrElm_t attr;	
	UINT32	err ;
	UINT32 storage;	
	osdClearScreen(0x00);
	osdStrDisp(38, 3, UserFont10x20, 0xd0, F_Mp3GetString(title_mp3_s));
	ui8_AutoCloseLCD = 0 ;
	RefreshItem[item]();
	while(1)
	{
		keyGet(&key) ;
		switch(key)
		{
			case	UI_KEY_DIR_UP:
				if(subitem>1) subitem -- ;
				RefreshSubitem[item](subitem) ;
				break ;
			case	UI_KEY_DIR_DOWN:
				if(subitem<Mp3SubMax[item]) subitem ++ ;
				RefreshSubitem[item](subitem) ;
				break ;
			case	UI_KEY_DIR_LEFT:
				if(item)	
				{
					item -- ;	
				}else 
				{
					item = MP3_ITEM_MAX-1;
				}	
				/*fqdao mp3 auto close  screen option  06.5.29*/
				if (item == MP3_ITEM_MAX-1)
				{
					subitem = sPara.ui8AutoCloseLCDTime+1;
				}	
				else
				{
					subitem = 1;
				}
				
				RefreshItem[item]();
				break ;
				
			case	UI_KEY_DIR_RIGHT:
				if(item<MP3_ITEM_MAX-1) 
				{
					item++ ;
				}
				else
				{
					item = 0;
				}
				/*fqdao mp3 auto close  screen option  06.5.29*/
				if (item == MP3_ITEM_MAX-1)
				{
					subitem = sPara.ui8AutoCloseLCDTime+1;
				}	
				else
				{
					subitem = 1;
				}
				
				RefreshItem[item]();
				break ;
			case	UI_KEY_FUNC_OK:
				if( F_SelectedDeal[item](subitem))
				{
					MessageShow (MenuSuccess);
				}
				osdClearScreen(0);
				pbFileListWindowRfsh(fileIdx,2);
			//	F_Mp3IconRefresh(PROGRESS_RST) ;
			//	F_Mp3IconRefresh(PROGRESS_VOL) ;				
				return ;
				
				break ;
				
			case	UI_KEY_ACCIDENT:
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);
				return;
				
			case	UI_KEY_FUNC_MENU:
			case	UI_KEY_FUNC_B:
				osdClearScreen(0);
				pbFileListWindowRfsh(fileIdx,2);
			//	F_Mp3IconRefresh(MP3_EQFlag) ;
			//	F_Mp3IconRefresh(MP3_EQ_DIS) ;
				
			//	F_Mp3IconRefresh(PROGRESS_RST) ;
			//	F_Mp3IconRefresh(PROGRESS_VOL) ;				
				return ;
				break ;
		}
	}
}

	
UINT32	F_Mp3DelSingleFile(UINT32	idx)
{
UINT32	err ;
		if((xdcfCurFileByPosSet(idx)) != 0)
		{
			#ifdef	OSD_DEBUG
			printf("\nErase	error1...(idx=%d)",	fileIdx);
			#endif
		}
		// Message of delete readonly file. John 12/23/2005
		else if (xdcfCurFileAttrGet(&fileattr)==1)
		{
	//		MsgShow(ReadOnly);
			MessageShow(ErrfileProtect);
			osTimeDly(10);
			printf("can't delete file");
		}
		else if (xdcfCurFileAttrGet(&fileattr)==0)
		{
//							xdcfDelOnlyFileFuncSet(readOnlyFileErase);
		
			if (IsSDProtect ())
				{
					MessageShow (ErrCardProtect);				
				return 0;
				}
				
			if (fileattr.attribute & xDCF_ATTR_READONLY)
				{
					MessageShow(ErrfileProtect);
					return 0;
				}

			xdcfOneFileErase(fileattr.name);
			err	= xdcfFileCountGet(&fileCnt);
			if(!fileCnt) 
			{
				xdcfDirChange("..");
				xdcfDirCountGet(&dirCnt);
				xdcfCurDirPosGet(&dirIdx);
				xdcfCurDirAttrGet(&dirattr);
			}
			else 	if(idx>fileCnt) 
				fileIdx = fileCnt ;
		}
		return 1; // Delete Success John@02/07/06
}
UINT32	F_Mp3DelAllFile()
{
UINT32	storage ;
UINT32  Idx,Counts;
#if 0
		xdcfActiveDevIdGet(&storage);
		if((storage != DRIVE_NAND) && isCardProtect())
		{
			MsgShow(card_protect_s);
			hwWait(0, 1000);				
		}
		else
#endif			
		if (IsSDProtect ())
		{
			MessageShow (ErrCardProtect);				
			return 0;
		}
		{				
		#if 0   //fqdao_modify  06.5.30
			xdcfFileCountGet (&Counts);
			for (Idx = 1; Idx < Counts; Idx++)
				F_Mp3DelSingleFile(Idx);
		#endif
		
			xdcfAllFilesErase();			
			xdcfFileCountGet(&fileCnt);
			if(!fileCnt) 
			{
				xdcfDirChange("..");
				xdcfDirCountGet(&dirCnt);
				xdcfCurDirPosGet(&dirIdx);
				xdcfCurDirAttrGet(&dirattr);				
			}
			else if(fileIdx>fileCnt) 
				fileIdx = fileCnt ;
			xdcfFileCountGet(&fileCnt);
		}
		return 1; // Delete Success John@02/07/06
}
UINT32	F_Mp3ProtectSingleFile(UINT32	idx)
{
		if (IsSDProtect ())
		{
			MessageShow (ErrCardProtect);	
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);					
			return 0;
		}
		xdcfCurFileByPosSet(idx) ;
		xdcfCurFileAttrGet(&fileattr) ;
		xdcfCurFileAttrSet(xDCF_ATTR_PROTECT) ;
		return 1; // Delete Success John@02/07/06
}
UINT32 F_Mp3ProtectAllFile()
{
	UINT32	i;
	if (IsSDProtect ())
	{
		MessageShow (ErrCardProtect);				
		return 0;
	}
	xdcfFileCountGet(&fileCnt);
	MessageShow (MenuWaite);	//fqdao
	for(i=1;i<=fileCnt;i++)
	{
		F_Mp3ProtectSingleFile(i) ;

	}
	return 1; // Delete Success John@02/07/06
}

UINT32	F_Mp3DisProtectSingleFile(UINT32	idx)
{
	if (IsSDProtect ())
	{
		MessageShow (ErrCardProtect);	
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);					
		return 0;
	}
	xdcfCurFileByPosSet(idx) ;
	xdcfCurFileAttrGet(&fileattr) ;
	xdcfCurFileAttrSet(xDCF_ATTR_DISPROTECT) ;
	return 1; // Delete Success John@02/07/06
}
UINT32	F_Mp3DisProtectAllFile()
{
	UINT32	i;
	if (IsSDProtect ())
	{
		MessageShow (ErrCardProtect);				
		return 0;
	}	
	xdcfFileCountGet(&fileCnt);
	MessageShow (MenuWaite);	//fqdao
	for(i=1;i<=fileCnt;i++)
	{
		F_Mp3DisProtectSingleFile(i) ;
	}
	return 1; // Delete Success John@02/07/06
}


/**************************************************************************
 *                                                                        *
 *  Function Name: uiMP3Play                                              *
 *                                                                        *
 *  Purposes:                                                             *
 *    function for Playback mode UI process                               *
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
void uiMP3Refresh(UINT32 idx )
{
	pbFileListWindowRfsh(idx,0);
//	F_Mp3IconRefresh(MP3_EQFlag) ;
//	ui8Mp3EQCounter = 0  ; 
//	F_Mp3IconRefresh(MP3_EQ_DIS) ;
	
}

 UINT8   Mp3Document;
	
UINT8    ui8Mp3RewEOF = 0 ;
UINT8	ui8UserBreak = 0 ;
UINT8   ui8CloseLCDFlag = 0  ;  // 0 是close 1 Open
UINT8   ui8tmrEnbleFlag = 0 ;   // 0 is  disenble 1 is enble
extern  	UINT8 speed ;
extern UINT8   ui8VolCnt  ;
extern	UINT8 	pressVolume;

void uiMP3Play(void)
{	
	UINT32 key = UI_KEY_MODE_MP3;
	UINT32 err = SUCCESS;
	UINT32	osdW, osdH;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	semApp = osEventFindByName("APP_SEM");
	mp3DispStart = 1;
	dirIdx = 0;
	fileIdx = 0;
//	prefileIdx = 0;

	while ( ( uiState & UI_MODE_MASK ) == UI_MODE_MP3 )
	{
		if (ui32NextState != uiState)		
		{
			break;
		}
		
		switch(key)
		{			
			case	UI_KEY_DIR_UP:
				if(fileCnt&&(fileIdx==1))
				{
					fileIdx = fileCnt ;
				}
				else
				{
					fileIdx -- ;
				}
				uiMP3Refresh(fileIdx);
				break;
				
				
			case	UI_KEY_DIR_DOWN:
				if(fileCnt && fileIdx==fileCnt)
				{
					fileIdx =1 ;
				}
				else
				{
					fileIdx++ ;
				}
				uiMP3Refresh(fileIdx);
				break;

			case	UI_KEY_DIR_LEFT:
				break;
				
			case	UI_KEY_DIR_RIGHT:
				break;
				
			case	UI_KEY_FUNC_A:
				break;
				
			case	UI_KEY_FUNC_B:
				F_Mp3MenuFunc();	/* Paul@006/01/24 add for common */
				break;
				
			case	UI_KEY_FUNC_MENU:
				mp3Disp = PB_DISP_NINE ;	
				avPlayNow = OBJ_NONE ;			
				menuReturn(UI_MAINMENU, 0);
				break;
				
			case	UI_KEY_FUNC_OK:
				if(mp3Disp == PB_DISP_NINE)
				{
					mp3Disp = PB_DISP_ONE;
					g_playrecStatus = STATUS_PLAYING ;
					F_PlayMp3();
				}
				break;
				
			case	UI_KEY_MODE_MP3:
				osTaskSuspend(osTaskFindByName("AAA"));
				mp3DispStart = 1;
				mp3Disp = PB_DISP_NINE;
				Mp3Document=0;//wendy@050309 add for read mp3 from diffrent document end
				xdcfInit(imageDirNameStr, mp3SubDirMP3Str, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_NAME/* | xDCF_CONFIG_DCF_ONLY*/);
				xdcfCurRootDirSet(mp3SubDirMP3Str);

				printf("Cur dir =%s\n", vfsGetCurrDirName());
//					uiFindValidDir(0) ;
				xdcfFilterSet(FILTER_AUDIO);

				xdcfDirCountGet(&dirCnt);
				xdcfCurDirPosGet(&dirIdx);
				xdcfCurDirAttrGet(&dirattr);
				err	= xdcfFileCountGet(&fileCnt);

				printf("Cur dir cnt =%d\n", dirCnt);
				printf("Cur dir filecnt =%d\n", fileCnt);
									
				fileIdx = 1;
				if(fileCnt == 0)
				{
					fileIdx = 0;
				}
				osdLcdDisp(1);
				pbFileListWindowRfsh(1,2);

			//	F_Mp3IconRefresh(PROGRESS_RST);
			//	F_Mp3IconRefresh(PROGRESS_VOL);
			//	ui8Mp3EQCounter = 0  ; 
			//	F_Mp3IconRefresh(MP3_EQ_DIS) ;
				break;
				
			default:
				break;
		}
		keyGet(&key);
	}
	mp3Disp = PB_DISP_NINE;
	uiState = ui32NextState;
}


void F_PlayMp3()
{
	UINT32 Volum;
//	Volum = mp3Vol;
			hisIconDisp();
			SpeakFlagMode=0;//wendy @05025
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
			#if 0
				for (Volum = 0; Volum <= mp3Vol; Volum++)			
				{
					pbSet8751Volume(ws9712_OUT2,Volum);
//					osTimeDly(2);
					hwWait(0, 10);
				}
			#endif
					pbSet8751Volume(ws9712_OUT2,mp3Vol);						
			}
			else
			{
				#if 0
				for (Volum = 0; Volum <= mp3Vol; Volum++)			
				{
					pbSet8751Volume(ws9712_HP,Volum);
//					osTimeDly(2);
					hwWait(0, 10);
				}
				#endif
					pbSet8751Volume(ws9712_HP,mp3Vol);				
			}			
#endif	


#if (CurrentIC8751==0)	
		        gpioConfig(GPIO_MUTE, 1);
		        gpioSet(GPIO_MUTE, 0);
#else		
			if(SpeakFlag)
				wm8751SoundEnable(2,1);
			else
				wm8751SoundEnable(1,1);
#endif	
/* fqdao mp3 auto close screen for next 06.5.29*/
			ui8CloseLCDFlag  = 1;
			ui8tmrEnbleFlag = 1 ;
			ui8PlayMp3Zero_Flag = 1 ; //tzwadd135
			ui8PlayMp3Zero_Num = 0 ;
/* fqdao mp3 auto close screen for next 06.5.29*/
			while(1)
			{
				pbAudioInfoGet(fileIdx);
				pbShowFileNum(fileIdx,fileCnt) ;
				if (!audioErr )
				{
					ui8PlayMp3Zero_Flag = 0 ; //tzwadd135
					g_playrecStatus = STATUS_PLAYING;
					ui8FW_REWFlag = 0 ;
					do {
						pbMP3Play(fileIdx);
					} while(((avPlayNow == OBJ_MP3) ||(avPlayNow == OBJ_WMA)||(avPlayNow == OBJ_WAV))&& (mp3Repeat == MP3RepeatOne));
					osTimeDly (20);
#if 0
					if (((avPlayNow == OBJ_MP3) ||(avPlayNow == OBJ_WMA)||(avPlayNow == OBJ_WAV))
						&& ((mp3Repeat == MP3RepeatAll) ||(fileIdx<fileCnt)/*//wendy update (fileCnt < 5)*/))
					{
						avPlayNow =	OBJ_NONE;
						osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_DIR_DOWN]);
					}
					#if 1		
					else 
					if (((avPlayNow == OBJ_MP3) ||(avPlayNow == OBJ_WMA)||(avPlayNow == OBJ_WAV))
					&& ((mp3Repeat == MP3RepeatRandom)))
					{
						osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_DIR_DOWN]);					
					}
					#endif					
#endif					
				}			
				else
				{
					#if 1//fqdao_add for mp3 format  06.6.10
						UI_OSQFlush(uiKeyMsgQ);
						if(fileCnt && fileIdx==fileCnt)
						{
							fileIdx =1 ;
						}
						else
						{
							fileIdx++ ;
						}
						audioErr = 0;
						avPlayNow =	OBJ_NONE;
						//	continue;
						#if 1  //tzwadd135
						if( ui8PlayMp3Zero_Flag == 1 )
						{
							ui8PlayMp3Zero_Num++ ;
						}

						if( ui8PlayMp3Zero_Num > fileCnt )
						{
							mp3Disp = PB_DISP_NINE ;
						}
						else
						{
							continue;
						}	
						#endif
					#endif

				}

				if (ui32NextState != uiState)
				{
					tmrDisable(1);
					break;
				}
				
				if(mp3Disp==PB_DISP_NINE)
				{
//					osdClearScreen(0x00) ;
					pbFileListWindowRfsh(fileIdx,0);
					g_playrecStatus = 0 ;
					F_Mp3IconRefresh(MP3_PLAY_STATUS) ;
				//	F_Mp3IconRefresh(MP3_EQ_RST) ;
					F_Mp3IconRefresh(PROGRESS_RST) ;

					if( sPara.ui8AutoCloseLCDTime != ui8AutoCloseLCDTimeNever )
					{
						tmrDisable(1);
					}	
					break ;
				}
			}
						
			SpeakFlagMode=0;//wendy @05025
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

		
}


#if 0
void pbMP3Refresh(UINT8 isClear)
{
/*	mp3DispStart = ((fileIdx-1)/(mp3DispCol*mp3DispRow))*(mp3DispCol*mp3DispRow)+1;*/
	audioErr = 0;

	if (fileCnt == 0)
	{
		osdClearScreen(0);
		osdIconDisp(isClear);
		ui_ShowAudioMenuInfo();
		osTimeDly(200);         //Danny --  05/05/26
		menuReturn(UI_MAINMENU, 0);
	}
	else if(mp3Disp == PB_DISP_ONE)
	{
		/*osTaskSuspend(osTaskFindByName("HISTOGRAM"));*/
		osdFWBuffChange();
		osdClearScreen(0);
		pbAudioInfoGet(fileIdx);
		pbRepeatWindowRfsh();
		hisIconDisp();
		hisTimeDisp();//for bug ? 2005-10-26 13:57
	 	ui_ShowAudioMenuInfo();
		osdHWBuffChange();
		SpeakFlagMode=0;//wendy @05025
		tmrEnable(1, 8000, TMR_PERIODIC, osdAutoDisp);//wendy@050205
//		tmrEnable(3, 1000, TMR_PERIODIC, osdaudiotimeShow);//John 051101			
		ws9712LineOut(SpeakFlag);//wendy for speak
		if(SpeakFlag)
			pbSetVolume(ws9712_OUT2, mp3Vol);
		else
			pbSetVolume(ws9712_HP, mp3Vol);

		/*mp3Info.title[0] = '\0';
		osdIconDisp(isClear);*/
		if (audioErr ==1)
		{
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_DIR_DOWN]);
			audioErr = 0;
		}
		else
		{
		
			do
				{

					pbMP3Play(fileIdx);
					

				}while(((avPlayNow == OBJ_MP3) ||(avPlayNow == OBJ_WMA)||(avPlayNow == OBJ_WAV))&& (mp3Repeat == MP3RepeatOne));
				osTimeDly(20);

				if (((avPlayNow == OBJ_MP3) ||(avPlayNow == OBJ_WMA)||(avPlayNow == OBJ_WAV))
					&& ((mp3Repeat == MP3RepeatAll) ||(mp3Repeat == MP3RepeatRandom)||(fileIdx<fileCnt)/*//wendy update (fileCnt < 5)*/))
				{
					avPlayNow =	OBJ_NONE;
					osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_DIR_DOWN]);
				}
		}

	/*	else if ((avPlayNow == OBJ_MP3) && (mp3Repeat == MP3RepeatRandom))
		{
			avPlayNow =	OBJ_NONE;
			fileIdx = tmrUsTimeGet() % fileCnt + 1;
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_DIR_DOWN]);
		}*/
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
		SpeakFlagMode=0;//wendy @05025
		ws9712LineOut(SpeakFlag);//wendy for speak
		if(SpeakFlag)
			pbSetVolume(ws9712_OUT2, mp3Vol);
		else
			pbSetVolume(ws9712_HP, mp3Vol);
		tmrDisable(1);//wendy@050205


	}
	else
	{
		/*osTaskResume(osTaskFindByName("HISTOGRAM"));*/
		gpioSet(GPIO_MUTE, 1);//wendy@050308 for mute
		osdFWBuffChange();
		osdClearScreen(0);
		ui_ShowAudioMenuInfo();
//		pbFileListWindowRfsh(fileIdx);
		pbAudioInfoGet(fileIdx);
		pbRepeatWindowRfsh();
		hisIconDisp();

	//	osSchedLock();
	//	hisTimeDisp();

		osdHWBuffChange();

	}

}
#endif



UINT32 pbAudioInfoGet(UINT32 idx)
{
	UINT32 err;
	xdcfAttrElm_t attr;
	UINT8 *pbuf =NULL;
//	UINT32 totalTime;
	UINT8 str[256];
	UINT8 FileType[4];	
	//UINT32  ret;

	UINT32 i=0, j=0;
	UINT32 offsetX, offsetY;
	UINT32	osdW, osdH;
	SINT32 fd;
	UINT32 filesize = 0;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	audioErr = 0 ;
	if ((err = xdcfCurFileByPosSet(idx)) != SUCCESS)
		return err;
	if ((err = xdcfCurFileAttrGet(&fileattr)) != SUCCESS)
		return err;
	if ((err = xdcfFileTypeGet(idx, &fileType)) != SUCCESS)
		return err;

//	err	= xdcfCurFileAttrGet(&fileattr);

	UINT8* vfsLFN=NULL;
	mp3_Info.title[0] = '\0';
	mp3_Info.album[0] = '\0';
	mp3_Info.artist[0] = '\0';
	sampleRate = 0;
	timeTotal = 0;




	switch(fileType){
	case xDCF_FILETYPE_MP3:
		#if 0
		if (mp3_CheckFileFormat(fileattr.name) == FAIL)
		{
			printf(" \n mp3 file err");
			audioErr = 3;
			break;
		}	
		#endif
		if (ui32totalTimeSave[idx] == 0)
		{
			ui32totalTimeSave[idx] = mp3GetTotalTime(fileattr.name);
		}
		if (ui32totalTimeSave[idx] == 0)
		{
			audioErr = 3;
			break;
		}		

		strcpy (FileType, "MP3");	
		printf(" attr.name =%s \n", fileattr.name);

		if (mp3GetVersion(fileattr.name))
		{
			sampleRate = mp3GetSamplingRate(fileattr.name);
			printf(" mp3GetSamplingRate\n");
			if (sampleRate)
			{

				err = 0;
				pbuf = osMemAlloc( sizeof( mp3_Info));
				if ( pbuf == NULL ) {
				printf("osMemAlloc failed \n");
				return FAIL;
				}
				printf("pbuf = 0x%x\n",pbuf);
				memset( &mp3_Info, 0, sizeof( mp3_Info) );
				err = mp3TagInfoGet(fileattr.name, &mp3_Info);
				printf(" err =%d\n", err);
				if (!err)
				{
					mp3_Info.title[0] = '\0';
					mp3_Info.album[0] = '\0';
					mp3_Info.artist[0] = '\0';
				}
//				timeTotal = mp3GetTotalTime(fileattr.name);

			}
			else
			{
				printf(" \n mp3 file err");
				audioErr = 3;
				vfsCurrDirReset();
			}
		}
		else
		{
			printf(" \n mp3 version err");
			audioErr = 4;

		}
		break;

	case xDCF_FILETYPE_WMA:
		strcpy (FileType, "WMA");
		/*@06.08.30**/
		if (ui32totalTimeSave[idx] == 0)
		{
			ui32totalTimeSave[idx] = wma_get_total_time(fileattr.name);
		}
		if (ui32totalTimeSave[idx] == 0)
		{
			audioErr = 3;
			break;
		}		
		/*@06.08.30**/

		if(wmaGetVersion(fileattr.name))
		{
			sampleRate = wmaGetSamplingRate(fileattr.name);
		}
		else
		{
			audioErr = 6;
			printf(" \n wma version err");
		}
		break;

	case xDCF_FILETYPE_WAV:
			strcpy (FileType, "WAV");

			audAppFileInfoGet(fileattr.name,&audioProfile);
			sampleRate = audioProfile.sampleRate;
			printf("\n sampleRate =%d\n",sampleRate);
//				timeTotal = audioProfile.residualTime;
				printf("\n totalTime =%d\n",timeTotal);

		break;
	default:
		return 0;
	}

	timeTotal  = ui32totalTimeSave[idx] ;
	if(mp3Disp == PB_DISP_ONE)
	{

#if 0
		err = xdcfFileOpen(fileattr.name, 0, 0, &fd);
		if(err!=0)
		{
			return 1;
		}
		else
		{
			filesize =xdcfFileSizeGet(fd);
		}
#endif



	UINT8	*pLongNamePtr ;
	UINT8 	filename[FILE_MAXCHAR] ;
	UINT8	ui8LongName[256] ;

	pLongNamePtr = vfsLongFileNameGet(fileattr.name);	
	//printf("=====%s\n",pLongNamePtr);
	if(pLongNamePtr==NULL)
		pLongNamePtr = fileattr.name;	
		strcpy(ui8LongName,pLongNamePtr);
		pLongNamePtr = ui8LongName ;
	*(pLongNamePtr+strlen(pLongNamePtr)-4) = '\0' ;
       #if 0
	if(strlen(pLongNamePtr)>=FILE_MAXCHAR-1)
	{
		strncpy(filename, pLongNamePtr,FILE_MAXCHAR-1);
		filename[FILE_MAXCHAR-1-4] = '\0' ;
		strcat(filename,"...");
	}
	#endif 
	//add by xu for bug 19 2006 4 25
	if(strlen(pLongNamePtr)>FILE_MAXCHAR-1)//add by xu for bug 19 2006 4 25
	{
		strncpy(filename, pLongNamePtr,FILE_MAXCHAR-1);
		F_CutLongNameToShortName(filename,FILE_MAXCHAR-1);
	}
	//add by xu for bug 19 2006 4 25
	else
	{
		//strncpy(filename, pLongNamePtr,FILE_MAXCHAR-1);
		strcpy(filename, pLongNamePtr);
	}	

		UINT8 space[FILE_MAXCHAR];
		memset(space, ' ', FILE_MAXCHAR-1);
		space[FILE_MAXCHAR-1] = '\0';
//		osdBarDraw(MP3_EQFlag_X,MP3_EQFlag_Y-22,100,24,00);
		osdStrDisp(MP3_FILENAME_X, MP3_FILENAME_Y, UserFont10x20,0x00, space) ;
		osdStrDisp(MP3_FILENAME_X, MP3_FILENAME_Y, UserFont10x20,0xd0, filename) ;

		memset(space, ' ', FILE_MAXCHAR-1);
		space[FILE_MAXCHAR-1] = '\0';
		for(i=0; i<MP3_LIST_MAX; i++)
		{
//			osdStrDisp(MP3_LIST_STR_X, MP3_LIST_STR_Y+MP3_LIST_Y_SIZE*i, UserFont10x20, 0, space);/*clear*/	
			osdBarDraw(MP3_LIST_BAR_X,MP3_LIST_STR_Y+MP3_LIST_Y_SIZE*i-2, MP3_LIST_BAR_W+70, 22,0x00) ;
		}

		sio_psprintf(str, "Artist: %s", mp3_Info.artist);
		printf("mp3 str = %s\n",str);
		osdStrDispExt(MP3_LIST_STR_X, MP3_LIST_STR_Y, UserFont10x20, 0x40, str, 0, osdW/10-9, 0);
//		osdStrDisp(MP3_LIST_STR_X, MP3_LIST_STR_Y, UserFont10x20, 0xf0, str);

	//	sio_psprintf(str, "Album :  %s", mp3_Info.album);
	//	osdStrDispExt(MP3_LIST_STR_X, MP3_LIST_STR_Y+ MP3_LIST_Y_SIZE, UserFont10x20, 0xf0, str, 0, osdW/10-8, 0);
//		osdStrDisp(MP3_LIST_STR_X, MP3_LIST_STR_Y+ MP3_LIST_Y_SIZE, UserFont10x20, 0xf0, str);

	//	sio_psprintf(str, "Samplerate: %dk", sampleRate/1000);
	//	osdStrDisp(MP3_LIST_STR_X, MP3_LIST_STR_Y+ MP3_LIST_Y_SIZE*1, UserFont10x20, 0x40, str);

		sio_psprintf(str, "Format:      %dk", sampleRate/1000);
		osdStrDisp(MP3_LIST_STR_X, MP3_LIST_STR_Y+ MP3_LIST_Y_SIZE*1, UserFont10x20, 0x40, str);
		osdStrDisp(MP3_LIST_STR_X + 80, MP3_LIST_STR_Y+ MP3_LIST_Y_SIZE*1, UserFont10x20, 0x40, FileType);		

		sio_psprintf(str, "TotalTime : %d:%d", timeTotal/1000/60, timeTotal/1000%60);
		osdStrDisp(MP3_LIST_STR_X, MP3_LIST_STR_Y+ MP3_LIST_Y_SIZE*2, UserFont10x20, 0x40, str);
		
		
	}
	
	osMemFree(pbuf);
	return 0;
}


UINT8 pbPlayWAV(UINT32 idx)
	/*Gu@ 2004/07/26, 11:08
play current file*/
{
		//UINT32 startTime=0, endTime=0;
		UINT32 err = SUCCESS;
		//UINT32 sts;

		#ifdef	OSD_DEBUG
		printf(" *WAV* ");
		#endif

		#ifdef	OSD_DEBUG
		printf("\nPLAY WAV... ");
		#endif

		#if 1
		UINT32  codecDev;
//		audFileInfo_t audioProfile;
		sysgAudOutDevGet(&codecDev);
//		audAppFileInfoGet(fileattr.name,&audioProfile);
	//	tmrEnable(3, 500, TMR_PERIODIC, osdaudiotimeShow);//John 051101
#if 1//for bug 41	/* Paul@2005/10/22 for Mp3 words */
		UINT8* pLongName;
		UINT32 size;
		UINT32 handle;
		UINT8   ui8FileName[256];
		
		pLongName = vfsLongFileNameGet(fileattr.name);
		strcpy(ui8FileName, pLongName);
		size = strlen(ui8FileName);

		ui8FileName[size-3] = 'l';
		ui8FileName[size-2] = 'r';
		ui8FileName[size-1] = 'c';
		handle = vfsOpen(ui8FileName, 0, 0);
		if (handle == 0)
		{
			printf("Can not open lrc file\n");
			strcpy(ui8FileName, fileattr.name);	
			ui8FileName[size-3] = 'M';
			ui8FileName[size-2] = 'P';
			ui8FileName[size-1] = '3';			
		}
		else
		{
			extern UINT8 ui8SerialData[];			
			printf("Dir Name = %s\n", ui8SerialData);
			strcpy(ui8FileName, &ui8SerialData[4]);
			ui8FileName[size-3] = 'M';
			ui8FileName[size-2] = 'P';
			ui8FileName[size-1] = '3';
			vfsClose(handle);
		}
		UINT32 mp3LRCAnalyse(UINT8*);
		mp3LRCAnalyse(ui8FileName);		
#endif


#if 0
		if( codecDev == AUDIO_AC97 ) {
			printf("AC97 sample rate %d\n",audioProfile.sampleRate);
			/* depend on file, if not 8K,24K please fixed on 48Khz*/
			if((audioProfile.sampleRate == 8000) || (audioProfile.sampleRate == 24000)){
				hwAudAC97RegWrite(0x2c, audioProfile.sampleRate);
				hwAudAC97RegWrite(0x2e, audioProfile.sampleRate);
				hwAudAC97RegWrite(0x32, audioProfile.sampleRate);
			}
			else{
				hwAudAC97RegWrite(0x2c, 48000);		/* 48K Hz*/
				hwAudAC97RegWrite(0x2e, 48000);		/* 48K Hz*/
				hwAudAC97RegWrite(0x32, 48000);		/* 48K Hz*/
			}
		}
		else if( codecDev == AUDIO_IIS ) {
			printf("IIS sample rate %d\n",audioProfile.sampleRate);
			/* depend on file */
		}
		else {
			printf("PWM sample rate %d\n",audioProfile.sampleRate);
			/* depend on file */
		}
#endif
#endif



				UINT8 samplerateIdx ;
				switch(audioProfile.sampleRate)
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
						default:
							audioProfile.sampleRate = 48000;
				}
				


				#if (CurrentIC8751==0)
					hwAudAC97RegWrite(0x2c, audioProfile.sampleRate);
					hwAudAC97RegWrite(0x2e, audioProfile.sampleRate);
					hwAudAC97RegWrite(0x32, audioProfile.sampleRate);
				#else
					wm8751SampleSet(samplerateIdx) ;
				#endif


		avPlayNow =	OBJ_WAV;
		audPlayFileNameSet(fileattr.name);
		audRiffCtrl(RIFF_PLAY);
		/* when using 1.7.7 code, you could use semApp for audio playing */
		/*startTime = tmrMsTimeGet();*/
		g_playrecStatus = STATUS_PLAYING;
		wavProgress = 0;
		osTimeDly(30);
		/*wavShortBreak = 0;
		wavPlayEnd = 0;*/
		do
		{
			osSemPend(semApp , OS_NO_WAIT, &err);
			if (err == OS_NO_ERR)
			{
				osSemPost(semApp);
//				osdIconDisp(0);
/*				
				while(uiKeyPush)
				{
				  osTimeDly(1);
				}
*/				
				if (mp3Repeat == MP3RepeatOne)
				{

					osSemPend(semApp , 0, &err);
					osSemPost(semApp);			
					g_playrecStatus = STATUS_STOP;
					audRiffCtrl(RIFF_STOP);
					
					return;	
				}
				if(fileCnt && fileIdx==fileCnt)
				{
					fileIdx =1 ;
				}
				else
				{
					fileIdx++ ;
				}
				if(g_playrecStatus==STATUS_FF)  // fqdao_add for bug Mp3_FF 06.5.9
				{
				    F_Mp3SetVolum(mp3Vol);
				}
				avPlayNow = OBJ_NONE;				

			}
			else
			{
				F_Mp3KeyDeal();		
				
//				osTimeDly(20);
				if (avPlayNow == OBJ_NONE)
				{
					break;
				}
				
				F_Mp3IconRefresh(PROGRESS_MOV);
				F_Mp3IconRefresh(MP3_PLAY_STATUS);
				F_Mp3IconRefresh(MP3_LRC) ;
				F_Mp3IconRefresh(MP3_EQFlag) ;
				
				if(g_playrecStatus == STATUS_PLAYING)
				{
					F_Mp3IconRefresh(MP3_EQ_DIS);			
				}
//				osdIconDisp(0);
				if ((g_playrecStatus == STATUS_FF)||(g_playrecStatus == STATUS_REW))
					{
						audRiffCtrl( RIFF_TIMESET );
		           			osTimeDly( 10 );
						if (g_playrecStatus == STATUS_FF)
		            				audRiffCtrl((wavTime/1000) + 5 );/* jump to offset based on current elapse time */
						else
							audRiffCtrl((wavTime/1000) - 5 );/* jump to offset based on current elapse time */
		           			osTimeDly( 10 );
					}
			}
		}while(err);
		/*osTimeDly(20);*/
		osSemPend(semApp , 0, &err);
		osSemPost(semApp);

		g_playrecStatus = STATUS_STOP;
		audRiffCtrl(RIFF_STOP);
		osTimeDly(20);
	/*	avPlayNow =	OBJ_NONE;*/

		#ifdef	OSD_DEBUG
		printf("END	");
		#endif

	return 0;
}


#if 0
void ui_ShowAudioMenuInfo(void)
{
       UINT8   str[]=" ";
	UINT32	osdW, osdH;
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	if (fileCnt == 0)
	{
	//	osdStrDisp(20, 0, UserFont10x20, 0xdf0, GetString(Audio_s));
		osdStrDisp(20, osdH-20, UserIcon10x20, 0x00, "\x80\x81");
		osdStrDisp(44, osdH-20, UserFont10x20, 0x00, GetString(Menu_s));
	}
	else
	{
	   /*osdStrDisp(20, 0, UserIcon10x20, 0x40, "\x9a\x9b"); */
	//   osdStrDisp(20, 0, UserFont10x20, 0xdf0, GetString(Audio_s));
	  if (mp3Disp == PB_DISP_NINE)
	 {
  		osdStrDisp(2, osdH-20, UserIcon10x20, 0x00, "\x14\x15");
		osdStrDisp(22, osdH-20, UserFont10x20, 0x00, GetString(Sel_s));
		osdStrDisp(66, osdH-20, UserIcon10x20, 0x00, "\x16\x17");
		osdStrDisp(92, osdH-20, UserFont10x20, 0x00, GetString(Play_s));
		osdStrDisp(142, osdH-20, UserIcon10x20, 0x00, "\x82\x83");
		osdStrDisp(166, osdH-20, UserFont10x20, 0x00, GetString(Rep_s));
		osdStrDisp(206, osdH-20, UserIcon10x20, 0x00, "\x84\x85");
		osdStrDisp(230, osdH-20, UserFont10x20, 0x00, GetString(Del_s));
	 }
	 else if (mp3Disp == PB_DISP_ONE)
	 {
	 
	 	osdStrDisp(2, osdH-20, UserIcon10x20, 0x00, "\x14\x15");
		osdStrDisp(22, osdH-20, UserFont10x20, 0x00, GetString(PN_s));
		osdStrDisp(62, osdH-20, UserIcon10x20, 0x00, "\x55\x56");
	       osdStrDisp(84, osdH-20, UserFont10x20, 0x00, GetString(FRFF_s));
		osdStrDisp(166, osdH-20, UserIcon10x20, 0x00, "\x16\x17");
			
			if ( g_playrecStatus == STATUS_PAUSE)   // tzw modify 08.23
		       {

				  osdStrDisp(186, osdH-20, UserFont10x20, 0x00, GetString(Play_s));
			        //osdStrDisp(337, osdH-20, UserFont10x20, 0xd0, str); 
		       }	
			else		 	
			      osdStrDisp(186, osdH-20, UserFont10x20, 0x00, GetString(Pause_s));


			//osdStrDisp(288, osdH-20, UserFont10x20, 0xd0, GetString(Pause_s));
			
     		osdStrDisp(236, osdH-20, UserIcon10x20, 0x00,  "\x84\x85");
     		osdStrDisp(254, osdH-20, UserFont10x20, 0x00, GetString(Vol_s));
		#if 1 //tzw add 05-09-23  //for bug 8
		if ((g_playrecStatus == STATUS_FF)||(g_playrecStatus == STATUS_REW))
		{
			if (!	ui8SaveVol)
		      {
            			  ui8SaveVol =1;
		       	  SetVolumValue(0);
			}
		}
		else if(ui8SaveVol)
        {
			SetVolumValue(mp3Vol);
			ui8SaveVol =0 ;
		}	
		#endif
		
	  	}
 }
}
#endif
void F_AutoCloseLCD()
{
	if( ui8CloseLCDFlag == 1)
	{
		ui8CloseLCDFlag = 0 ;
		OpenLcdDisplay(0);
		tmrDisable(1);
	}
}
void F_OpenLCD()
{
	if( ui8CloseLCDFlag == 0 )
	{
		ui8CloseLCDFlag = 1 ;
		OpenLcdDisplay(1);
	}	
}
extern UINT8 ui8SerialData[20];

void pbMP3Play(UINT32 mp3FileIdx)
{
	UINT32  err = SUCCESS;
	UINT32	osdW, osdH;
//	UINT32  SampleRate;
	   
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

//	F_SetSeriesKey(0x00c);
	F_SetSeriesKey(0x00f);		/* Paul@2006/02/06 add */
//	err	= xdcfFileTypeGet(fileIdx, &fileType);
//	tmrEnable(3, 500, TMR_PERIODIC, osdaudiotimeShow);//John 051101			
//	tmrEnable(1, 8000, TMR_PERIODIC, F_AutoCloseLCD);
#if 0  /* fqdao mp3 auto close screen for next 06.5.29*/
	ui8CloseLCDFlag  = 1;
	ui8tmrEnbleFlag = 1 ;
#endif
#if 1  //fqdao_add for mp3 format  06.6.10
	pbAudioInfoGet(fileIdx) ;
	if(audioErr)
	{
		UI_OSQFlush(uiKeyMsgQ);
		if(fileCnt && fileIdx==fileCnt)
		{
			fileIdx =1 ;
		}
		else
		{
			fileIdx++ ;
		}
		avPlayNow =	OBJ_NONE;
		return ;
	}
#endif

	if (fileType == xDCF_FILETYPE_MP3)
	{		
		UINT8* pLongName;
		UINT32 size;
		UINT32 handle;
		UINT8  ui8FileName[256];
		
#if 1//for bug 41	/* Paul@20vfsLongFileNameGet05/10/22 for Mp3 words */		
		pLongName = vfsLongFileNameGet(fileattr.name);
		strcpy(ui8FileName, pLongName);
		size = strlen(ui8FileName);		

		if (ui8FileName[size-4] == '.')
		{
			ui8FileName[size-3] = 'l';
			ui8FileName[size-2] = 'r';
			ui8FileName[size-1] = 'c';				
		}
		else
		{
			ui8FileName[size+0] = '.';
			ui8FileName[size+1] = 'l';
			ui8FileName[size+2] = 'r';
			ui8FileName[size+3] = 'c';							
			ui8FileName[size+4] = 0;										
		}
		
		handle = vfsOpen(ui8FileName, 0, 0);
		if (handle == 0)
		{
			if (sPara.language == LanguageChinese_T)
			{				
				mp3LRCAnalyse(fileattr.name);				 
			}
			else
			{
				if (!pLongName)
				{
					mp3LRCAnalyse(fileattr.name);		
				}
				else
				{
					mp3LRCAnalyse(NULL);					
				}
			}
			printf("Can not open lrc file\n");			
		}
		else
		{		
			printf("Dir Name = %s\n", ui8SerialData);
			strcpy(ui8FileName, &ui8SerialData[4]);
			vfsClose(handle);
			mp3LRCAnalyse(ui8FileName);				
		}
		
#endif

//		SampleRate = mp3GetSamplingRate(fileattr.name);
		/* fixed at 48Khz */
		#if 0
		printf("\nAC97 Sample Rate %d\n",SampleRate);
		#endif


#if (CurrentIC8751==0)		
		hwAudAC97RegWrite(0x2c, 48000);		/* 48K Hz*/
		hwAudAC97RegWrite(0x2e, 48000);		/* 48K Hz*/
		hwAudAC97RegWrite(0x32, 48000);		/* 48K Hz*/
#else 
		wm8751SampleSet(0);
#endif

		if(sampleRate != 0)
		{
			mp3AppPlay(fileattr.name);
			avPlayNow =	OBJ_MP3;
			g_playrecStatus = STATUS_PLAYING;
			osTimeDly(20);
//			tmrEnable(3, 500, TMR_PERIODIC, osdaudiotimeShow);//John 051101		
			while(1)
			{
				F_Mp3KeyDeal();
//				osTimeDly(20);
				if (avPlayNow == OBJ_NONE)
				{
					break;
				}
				 
				F_Mp3IconRefresh(PROGRESS_MOV);
				F_Mp3IconRefresh(MP3_PLAY_STATUS);
				F_Mp3IconRefresh(MP3_LRC) ;
				F_Mp3IconRefresh(MP3_EQFlag) ;
				if(g_playrecStatus==STATUS_PLAYING)
				{
					F_Mp3IconRefresh(MP3_EQ_DIS);
				}
				if(mp3AppStatusGet() == MP3_PENDING)
				{
					if (mp3Repeat == MP3RepeatOne)
					{
						g_playrecStatus = STATUS_STOP;

						osSemPend(semApp , 0, &err);
						osSemPost(semApp);			
						return;	
					}
					if(!ui8Mp3RewEOF) // fqdao_add for bug Mp3_REW 06.5.9
					{
						if(fileCnt && fileIdx==fileCnt)
						{
							fileIdx =1 ;
						}
						else
						{
							fileIdx++ ;
						}
					}
					avPlayNow = OBJ_NONE;
					break;
				}
			}
			g_playrecStatus = STATUS_STOP;

			osSemPend(semApp , 0, &err);
			osSemPost(semApp);
			
		}
	}

	
	#if	WMA_OPT
	else if (fileType == xDCF_FILETYPE_WMA)
	{
		#ifdef	OSD_DEBUG
		//printf("\nPLAY WMA... ");
		#endif
#if 1//for bug 41	/* Paul@2005/10/22 for Mp3 words */
		UINT8* pLongName;
		UINT32 size;
		UINT32 handle;
		UINT8   ui8FileName[256];


#if 1//for bug 41	/* Paul@20vfsLongFileNameGet05/10/22 for Mp3 words */		
		pLongName = vfsLongFileNameGet(fileattr.name);
		strcpy(ui8FileName, pLongName);
		size = strlen(ui8FileName);		

		if (ui8FileName[size-4] == '.')
		{
			ui8FileName[size-3] = 'l';
			ui8FileName[size-2] = 'r';
			ui8FileName[size-1] = 'c';				
		}
		else
		{
			ui8FileName[size+0] = '.';
			ui8FileName[size+1] = 'l';
			ui8FileName[size+2] = 'r';
			ui8FileName[size+3] = 'c';							
			ui8FileName[size+4] = 0;										
		}
		
		handle = vfsOpen(ui8FileName, 0, 0);
		if (handle == 0)
		{
			if (sPara.language == LanguageChinese_T)
			{				
				mp3LRCAnalyse(fileattr.name);				 
			}
			else
			{
				if (!pLongName)
				{
					mp3LRCAnalyse(fileattr.name);		
				}
				else
				{
					mp3LRCAnalyse(NULL);					
				}
			}
			printf("Can not open lrc file\n");			
		}
		else
		{
		
			printf("Dir Name = %s\n", ui8SerialData);
			strcpy(ui8FileName, &ui8SerialData[4]);
			vfsClose(handle);
			mp3LRCAnalyse(ui8FileName);				
		}
		
#endif		
		
#endif
		#if 0
		UINT32  codecDev;//,SampleRate;
		sysgAudOutDevGet(&codecDev);
		if( codecDev == AUDIO_AC97 ) {
			//printf("AC97 Sample Rate %d\n",SampleRate);
			/* fixed at 48Khz */
			hwAudAC97RegWrite(0x2c, 48000);		/* 48K Hz*/
			hwAudAC97RegWrite(0x2e, 48000);		/* 48K Hz*/
			hwAudAC97RegWrite(0x32, 48000);		/* 48K Hz*/
		}
		else if( codecDev == AUDIO_IIS ) {
			//printf("IIS Sample Rate %d\n",SampleRate);
			/* fixed at 48Khz */
			/* audio GPIO */
			hwAudGpioCfg(0xff);
			hwAudGpioSet(0xf9);
		}
		else {
			//printf("PWM Sample Rate %d\n",SampleRate);
			/* fixed at 48Khz */
		}
		#endif


#if (CurrentIC8751==0)		
		hwAudAC97RegWrite(0x2c, 48000);		/* 48K Hz*/
		hwAudAC97RegWrite(0x2e, 48000);		/* 48K Hz*/
		hwAudAC97RegWrite(0x32, 48000);		/* 48K Hz*/
#else 
		wm8751SampleSet(0);
#endif

		
		hwAdcSet(7, 3, 11); /* add this if using ADC as low battery detection */
		//printf("file %s\n",fileattr.name);
		//printf("BitRate %d\n",wmaGetBitrate(fileattr.name));

		if(sampleRate != 0){
			wmaAppPlay(fileattr.name);
			osTimeDly(10);
			avPlayNow =	OBJ_WMA;						
			g_playrecStatus = STATUS_PLAYING;
			osTimeDly(30);			
			while(1){
				F_Mp3KeyDeal();
//				osTimeDly(20);
				if (avPlayNow == OBJ_NONE)
				{
					break;
				}
				
				F_Mp3IconRefresh(PROGRESS_MOV);
				F_Mp3IconRefresh(MP3_PLAY_STATUS);
				F_Mp3IconRefresh(MP3_LRC) ;
				F_Mp3IconRefresh(MP3_EQFlag) ;
				if(g_playrecStatus==STATUS_PLAYING)
				{
					F_Mp3IconRefresh(MP3_EQ_DIS);
				}
				
				if(wmaAppStatusGet() == WMA_PENDING)
				{
					if (mp3Repeat == MP3RepeatOne)
					{
						g_playrecStatus = STATUS_STOP;
						osSemPend(semApp , 0, &err);
						osSemPost(semApp);			
						return;	
					}
					if(fileCnt && fileIdx==fileCnt)
					{
						fileIdx =1 ;
					}
					else
					{
						fileIdx++ ;
					}
					if(g_playrecStatus==STATUS_FF)  // fqdao_add for bug Mp3_FF 06.5.9
					{
					    F_Mp3SetVolum(mp3Vol);
					}
					avPlayNow = OBJ_NONE;				
					break;
				}

			}

			
			g_playrecStatus = STATUS_STOP;
//			osdIconDisp(0);

			osSemPend(semApp , 0, &err);	 /*	wait until current application finish its job*/
			osSemPost(semApp);	   /* wait until current application finish	its	job*/
		}

	}
	#endif
	else if (fileType == xDCF_FILETYPE_WAV)
	{
		pbPlayWAV(fileIdx);
	}
 //      gpioSet(GPIO_MUTE, 1);			//yyq  05-12-7

 
	F_SetSeriesKey(0x00f);
//	tmrDisable(3);
}
void F_OpentrmEnable()
{
	switch (sPara.ui8AutoCloseLCDTime )
	{
		case ui8AutoCloseLCDTimeNever:
			break ;
		case ui8AutoCloseLCDTime10s:	
			tmrEnable(1, 10000, TMR_PERIODIC, F_AutoCloseLCD);
			break;
		case ui8AutoCloseLCDTime30s:
			tmrEnable(1, 30000, TMR_PERIODIC, F_AutoCloseLCD);
			break; 
		case ui8AutoCloseLCDTime60s:	
			tmrEnable(1, 60000, TMR_PERIODIC, F_AutoCloseLCD);
			break; 
	}
}
extern UINT32 g_ui32SaveKeyCode;

void F_Mp3KeyDeal ()
{
	UINT32 key; 
	UINT32 volum;

//-------------------------------------------------
//	keyGet(&key) ;
	F_UserKeyGet(&key, 30);
	#if 1  // tzw add auto close lcd
	if( key < 0xffffff)
	{
		if( sPara.ui8AutoCloseLCDTime != ui8AutoCloseLCDTimeNever )
		{
			if( ui8CloseLCDFlag == 0)
			{
				
				F_OpenLCD();	
				if(key != UI_KEY_ACCIDENT )
				{
					key = 0xffffffff;
				}	
			}		
			else
			{
				tmrDisable(1);
			}
			ui8tmrEnbleFlag = 1 ;
		}
	}
	else
	{
		if( ui8tmrEnbleFlag == 1 )
		{
			ui8tmrEnbleFlag = 0 ;
			F_OpentrmEnable() ;
		}
	}
	#endif
		
	switch(key)
	{	
		case	UI_KEY_DIR_UP:
			if(mp3Vol<9)
			{
				mp3Vol++;
			}
			if (g_playrecStatus != STATUS_PAUSE)					
			{
				F_Mp3SetVolum(mp3Vol);
			}			
			F_Mp3IconRefresh(PROGRESS_VOL);			
			break;
			
		case	UI_KEY_DIR_DOWN:
			if(mp3Vol)
			{
				mp3Vol--;
			}
			if (g_playrecStatus != STATUS_PAUSE)										
			{
				F_Mp3SetVolum(mp3Vol);
			}			
			F_Mp3IconRefresh(PROGRESS_VOL);						
			break;

		case	UI_KEY_DIR_LEFT:
			if (g_playrecStatus != STATUS_REW)
			{
				osTimeDly(20);
				if (!(g_ui32SaveKeyCode & KEY_BIT_LEFT))
				{
					switch(avPlayNow)
					{
						case OBJ_MP3:
							mp3AppAbort();
							break;
						case OBJ_WMA:
							wmaAppAbort();
							break;
						case OBJ_WAV:
							audRiffCtrl(RIFF_STOP);
							break;
					}
					osTimeDly(40);

					UI_OSQFlush(uiKeyMsgQ);
					if(fileCnt && (fileIdx==1))
					{
						fileIdx = fileCnt ;
					}
					else
					{
						fileIdx-- ;
					}
					avPlayNow = OBJ_NONE ;			
					break;					
				}
			}
			switch(avPlayNow)
			{
				case OBJ_MP3:
					if(g_playrecStatus != STATUS_REW)
					{
						mp3AppRew(8);
					}
					break;

				case OBJ_WMA:
					#if (CurrentIC8751==0)	
			        gpioConfig(GPIO_MUTE, 1);
			        gpioSet(GPIO_MUTE, 1);
					ui32Mp3VolSave = mp3Vol;//tzw add for bug 3 060420
					F_Mp3SetVolum(0);//tzw add for bug 3 060420
					#else		
//					wm8751SoundEnable(0,0);
					ui32Mp3VolSave = mp3Vol;
					F_Mp3SetVolum(0);
					#endif								

					if (g_playrecStatus ==STATUS_PAUSE)
					{
						wmaAppPause();
						g_playrecStatus =STATUS_PLAYING;
					}
					printf(" Ly_debug start wma Rew!!!!\n");
					wmaAppRew (4000);
					break;
					
				case OBJ_WAV:
					#if (CurrentIC8751==0)	
			        gpioConfig(GPIO_MUTE, 1);
			        gpioSet(GPIO_MUTE, 1);
					ui32Mp3VolSave = mp3Vol;//tzw add for bug  9 060122
					F_Mp3SetVolum(0);//tzw add for bug  9 060122

					#else		
//					wm8751SoundEnable(0,0);
					ui32Mp3VolSave = mp3Vol;
					F_Mp3SetVolum(0);
					#endif								
					break;
			}
			
			if(g_playrecStatus == STATUS_REW) 
			{
				if(ui8FW_REWFlag && (ui8FW_REWFlag<10))
				{
					ui8FW_REWFlag++;
				}
			}
			else	
			{
				ui8FW_REWFlag = 1;
			}
			g_playrecStatus = STATUS_REW;				
			break;

		case	UI_KEY_DIR_RIGHT:			
			if (g_playrecStatus != STATUS_FF)
			{
				osTimeDly(20);
				if (!(g_ui32SaveKeyCode & KEY_BIT_RIGHT))
				{
					switch(avPlayNow)
					{
						case OBJ_MP3:
							mp3AppAbort();
							break;
						case OBJ_WMA:
							wmaAppAbort();
							break;
						case OBJ_WAV:
							audRiffCtrl(RIFF_STOP);
							break;
					}
					osTimeDly(40);
					UI_OSQFlush(uiKeyMsgQ);
					if(fileCnt && fileIdx==fileCnt)
					{
						fileIdx =1 ;
					}
					else
					{
						fileIdx++ ;
					}
					avPlayNow = OBJ_NONE ;			
					break;					
				}
			}
			
			switch(avPlayNow)
			{
			case OBJ_MP3:
				if(g_playrecStatus != STATUS_FF)
				{
			 		mp3AppFfwd(8);
					ui8Mp3RewEOF = 0; // fqdao_add for bug Mp3_REW 06.5.9
				}
				break;

			case OBJ_WMA:
				#if (CurrentIC8751==0)	
			    gpioConfig(GPIO_MUTE, 1);
			    gpioSet(GPIO_MUTE, 1);
				ui32Mp3VolSave = mp3Vol;//tzw add for bug  3 060420
				F_Mp3SetVolum(0);//tzw add for bug 3 060420

				#else		
//				wm8751SoundEnable(0,0);
				ui32Mp3VolSave = mp3Vol;
				F_Mp3SetVolum(0);
				#endif								
			
				if (g_playrecStatus == STATUS_PAUSE)
				{
					wmaAppPause();
					g_playrecStatus =STATUS_PLAYING;
				}
				wmaAppFfwd(4000);
				break;
				
			case OBJ_WAV:
				#if (CurrentIC8751==0)	
			    gpioConfig(GPIO_MUTE, 1);
			    gpioSet(GPIO_MUTE, 1);
				ui32Mp3VolSave = mp3Vol;//tzw add for bug 3 060420
				F_Mp3SetVolum(0);//tzw add for bug 3 060420
				#else		
//				wm8751SoundEnable(0,0);
				ui32Mp3VolSave = mp3Vol;
				F_Mp3SetVolum(0);
				#endif							
				break;

			}
			
			if(g_playrecStatus==STATUS_FF) 
			{
				if(ui8FW_REWFlag&&(ui8FW_REWFlag<10))
				{
					ui8FW_REWFlag++;
				}
			}
			else	
			{
				ui8FW_REWFlag = 1 ;
			}
			g_playrecStatus = STATUS_FF;
			break;

		case 	UI_KEY_FUNC_A:
			/* Change play mode: random, single, all */			
			if (mp3Repeat == MP3RepeatAll)
				mp3Repeat = MP3RepeatOne;
			else
				mp3Repeat++;
			sPara.Repeat  = mp3Repeat;

			break;

		case	UI_KEY_FUNC_B:
			/* Change sound effect */
			if (ui32EQFlag == EQDance)
			{
				ui32EQFlag = EQFlat;
			}
			else
			{
				ui32EQFlag++;
			}
			//ws9712SetEQ(ui32EQFlag);
			ws9712EqSet(ui32EQFlag);
			printf("EQ = %d\n", ui32EQFlag);				
			break;
			
		case	UI_KEY_ACCIDENT:
			UI_OSQFlush(uiKeyMsgQ);
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);
			
		case	UI_KEY_FUNC_MENU:			
			switch(avPlayNow)
			{
				case OBJ_MP3:
					mp3AppAbort();
					break;
				case OBJ_WMA:
					wmaAppAbort();
					break;
				case OBJ_WAV:
					audRiffCtrl(RIFF_STOP);
					break;
			}
			mp3Disp = PB_DISP_NINE ;	
			avPlayNow = OBJ_NONE ;		
			g_playrecStatus = STATUS_STOP ;	
			break;

	case	UI_KEY_FUNC_OK:
			switch(avPlayNow)
			{				
				case OBJ_MP3:					
					// for mute David 12/22/2005
					  //  fqdao_add for bug 23  06.4.27
					if(mp3AppStatusGet() == MP3_RUNNING||mp3AppStatusGet() == MP3_PAUSE)
					{
						#if (CurrentIC8751==0)	
						gpioConfig(GPIO_MUTE, 1);
						gpioSet(GPIO_MUTE, 1);
						#else		
						wm8751SoundEnable(0,0);					
						#endif
						// ~						
						mp3AppPause() ;
					}
					break;
					
				case OBJ_WMA:
					// mute set wma volum 0. David  12/22/2005				
					// for mute David 12/22/2005						
					#if (CurrentIC8751==0)	
					gpioConfig(GPIO_MUTE, 1);
					gpioSet(GPIO_MUTE, 1);
					#else		
					wm8751SoundEnable(0,0);
					#endif
					// ~					
					wmaAppPause() ;
					break;
					
				case OBJ_WAV:
					// for mute David 12/22/2005						
					#if (CurrentIC8751==0)	
					gpioConfig(GPIO_MUTE, 1);
					gpioSet(GPIO_MUTE, 1);
					#else		
					wm8751SoundEnable(0,0);
					#endif
					// ~												
					audRiffCtrl(RIFF_PAUSE);
					break;
			}	
			if(g_playrecStatus == STATUS_PAUSE)
			{
				if(avPlayNow == OBJ_MP3)
				{
					#if 1  //xqq for bug Pause Sound
						gpioConfig(GPIO_MUTE, 1);
						gpioSet(GPIO_MUTE, 0);	
					#else
					for (volum = 0; volum <= mp3Vol; volum ++)
					{
						F_Mp3SetVolum (volum);
					}		
					#endif
				}	
				else
				{
					 #if 1  //xqq for bug Pause Sound
					gpioConfig(GPIO_MUTE, 1);
					gpioSet(GPIO_MUTE, 0);	
					
					for (volum = 0; volum <= mp3Vol; volum ++)
					{
						F_Mp3SetVolum (volum);
					}				
					#endif
				}			
				g_playrecStatus = STATUS_PLAYING ;
			}
			else if(g_playrecStatus == STATUS_PLAYING)
			{
			#if 0
				for (volum = mp3Vol; volum > 0; volum --)
				{
					F_Mp3SetVolum (volum);
				}					
					F_Mp3SetVolum (0);	
					wm8751SoundEnable(0,0);
			#endif		
				g_playrecStatus = STATUS_PAUSE ;
			}
			break;

#if 1					
		default:
			if(uiKeyPush) 
			{
				break;
			}
			if(ui8Mp3RewEOF)  // fqdao_add for bug Mp3_REW 06.5.9
			{
				ui8Mp3RewEOF-- ;		
			}

			if(g_playrecStatus==STATUS_REW)
			{
				switch(avPlayNow)
				{
					case OBJ_MP3:
						ui8Mp3RewEOF = 3; // fqdao_add for bug Mp3_REW 06.5.9
						/*ui8Mp3RewEOF confirm 3 is the best */
						#if (CurrentIC8751==0)	
						gpioConfig(GPIO_MUTE, 1);
						gpioSet(GPIO_MUTE, 0);
						
						#else		
						if(SpeakFlag)
						{
							wm8751SoundEnable(2,1);
						}
						else
						{
							wm8751SoundEnable(1,1);
						}
						#endif								
						F_Mp3SetVolum(mp3Vol);
						
						mp3AppRew(8);
						break;
						
					case OBJ_WMA:
						#if (CurrentIC8751==0)	
						gpioConfig(GPIO_MUTE, 1);
						gpioSet(GPIO_MUTE, 0);
						F_Mp3SetVolum(mp3Vol);//tzw add for bug 3 060420
						#else		
						if(SpeakFlag)
						{
							wm8751SoundEnable(2,1);
						}
						else
						{
							wm8751SoundEnable(1,1);
						}
						F_Mp3SetVolum(mp3Vol);
						#endif								
						wmaAppRew(8);
						break;
						
					case OBJ_WAV:
						#if (CurrentIC8751==0)	
						gpioConfig(GPIO_MUTE, 1);
						gpioSet(GPIO_MUTE, 0);
						F_Mp3SetVolum(mp3Vol);//tzw add for bug 3 060420
						#else		
						if(SpeakFlag)
						{
							wm8751SoundEnable(2,1);
						}
						else
						{
							wm8751SoundEnable(2,1);
						}
						F_Mp3SetVolum(mp3Vol);
						#endif								
						audAppStatusSet(RIFF_STATUS_CLEAR); /*Julie add */
						break;
				}
				g_playrecStatus =STATUS_PLAYING;
				ui8FW_REWFlag = 0;					
			}
			else if(g_playrecStatus==STATUS_FF)
			{
				switch(avPlayNow)
				{
					case OBJ_MP3:
						#if (CurrentIC8751==0)	
				     		gpioConfig(GPIO_MUTE, 1);
				      		gpioSet(GPIO_MUTE, 0);
						#else		
						if(SpeakFlag)
							wm8751SoundEnable(2,1);
						else
							wm8751SoundEnable(1,1);
						#endif								
						F_Mp3SetVolum(mp3Vol);
						
						mp3AppFfwd(8);
						break;
						
					case OBJ_WMA:
						#if (CurrentIC8751==0)	
				        	gpioConfig(GPIO_MUTE, 1);
				        	gpioSet(GPIO_MUTE, 0);
					 	F_Mp3SetVolum(mp3Vol);//tzw add for bug  9 060122
						#else		
						if(SpeakFlag)
							wm8751SoundEnable(2,1);
						else
							wm8751SoundEnable(1,1);
						F_Mp3SetVolum(mp3Vol);
						#endif														
						wmaAppFfwd(8);
						break;
						
					case OBJ_WAV:
						#if (CurrentIC8751==0)	
					      gpioConfig(GPIO_MUTE, 1);
					      gpioSet(GPIO_MUTE, 0);
					 	F_Mp3SetVolum(mp3Vol);//tzw add for bug  9 060122
						#else		
						if(SpeakFlag)
							wm8751SoundEnable(2,1);
						else
							wm8751SoundEnable(1,1);
						F_Mp3SetVolum(mp3Vol);
						#endif								
						
						audAppStatusSet(RIFF_STATUS_CLEAR); /*Julie add */
						break;
				}
				g_playrecStatus =STATUS_PLAYING;
				ui8FW_REWFlag = 0 ;
				ui8Mp3RewEOF = 0; // fqdao_add for bug Mp3_REW 06.5.9
			}
			break ;
#endif
	}		
}
//-------------------------------------------------

#if 0
void pbMP3Init(UINT32 start, UINT32 curr, UINT32 total, UINT32 key)
{
	UINT32	i;
	UINT32	err	= SUCCESS;
	UINT32	pPBAddr, phyOsdDBAddr;
	UINT8	*pImgBuf;
	UINT32	w, osdFontWidth, osdFontHeight;
	UINT32	osdW, osdH;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	osdFontWidth = UserFont[2];
	osdFontHeight= UserFont[3];


	
	w =	5 *	osdFontWidth;
	

	pImgBuf		= (UINT8*) (((UINT32 )imgBuf) |	0xac000000);
	phyOsdDBAddr= (((UINT32)pImgBuf) & 0x00FFFFFF) >> 1;
	hwDateFontScalerSet(1);

	/*sysgMemThumbAddrGet(&pPBAddr);*/
	/* sysgMemFreeAddrGet(APP_STILL_PB, &pPBAddr);*/ /* ??? */
	pbThumbBufferGet(&pPBAddr);

	#if	0
	printf("\n buffaddr=%x ", pPBAddr);
	printf("\n imgBuf=%x pImgBuf=%0x ",	imgBuf,	pImgBuf);
	#endif

	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/

	for(i=0; i < mp3DispRow; i++)
	{
		if((start + i) <= total)
		{
			err	= xdcfFileTypeGet((start + i), &fileType);
			if ((fileType == xDCF_FILETYPE_MP3) || (fileType == xDCF_FILETYPE_WMA)
				|| (fileType == xDCF_FILETYPE_WAV))
			{
				if ((err = xdcfCurFileByPosSet((start + i))) != SUCCESS)
				{
					#ifdef	OSD_DEBUG
					printf(" #MP3_ERR ");
					#endif
					return;
				}
				xdcfCurFileAttrGet(&fileattr);	/*for filename to disp.*/

				#if (SUPPORT_LFN == 1)
				UINT8* vfsLFN=NULL;
				vfsLFN = vfsLongFileNameGet(fileattr.name);
				if (vfsLFN == NULL)
					vfsLFN = fileattr.name;
				if ((start + i) == fileIdx)
					osdStrDispExt(8, (i+1)*32+8, UserFont, 0x07, vfsLFN, 0, (osdW-32)/16, 0);
				

				else
					osdStrDispExt(8, (i+1)*32+8, UserFont, 0x00, vfsLFN, 0, (osdW-32)/16, 0);


				#else
				if ((start + i) == fileIdx)
					osdStrDisp(8, (i+1)*32+8, UserFont 0x07, fileattr.name);

				else
					osdStrDisp(8, (i+1)*32+8, UserFont, 0x00, fileattr.name);
				#endif
			}
		}
	}
	#ifdef	OSD_DEBUG
	printf(" MP3 Init OK ...\n");
	#endif
	
}

#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: uiFindValidDir                                         *
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
UINT32 uiFindValidDir(UINT32 dir)
{
	UINT32 i, idx, err;

	idx = dirIdx;
	for (i=0; i<dirCnt; i++)
	{
		if (dir==0)
		{
			idx = (idx==1) ? dirCnt:(idx-1);
		}
		else
		{
			idx = (idx==dirCnt) ? 1:(idx+1);
		}
		err	= xdcfCurDirByPosSet(idx);
		err	= xdcfFileCountGet(&fileCnt);
		if (fileCnt)
		{
			dirIdx = idx;
			break;
		}
	}
	printf("dir=%d, idx=%d, fileCnt=%d\n", dirCnt, dirIdx, fileCnt);
	return SUCCESS;
}

/*andrew_gu@2005/Apr/12, 9:46 (*/
#if 0
#define BUF_SIZE  (512 * 256)
#define READ_SIZE  4096
#define RIFF_REFILL_THRESHOLD (8192 / 4)
static UINT8  audBuf[BUF_SIZE + 2048];/* need 2K byte alignment */

testWav(void)
{
		UINT8 *paudBuf, *playbuf, *pdummy;
		UINT32 audFormat, codecFreq, fd, readSize, decodeSize, audPlay;
		UINT32 overFlow, underFlow, cnt, inIdx, outIdx, idxDistance, lastIdx;
		UINT32 nrByteRemain;
		UINT8* pringBufIn;
		UINT8* pringBufStart;
		UINT8* pringBufEnd;
			
		/* set the buffer address and buffer size(max 128KB) */
		audStop();
		paudBuf = (UINT8 *)((UINT32 )audBuf & 0x00fff800) + 2048;
		hwAudAddrSet((UINT32 )paudBuf / 2);
		hwAudBufSizeSet(BUF_SIZE / 2);
		
		pringBufStart = (UINT8 *)(0xac000000 + (UINT32 )paudBuf);
		pringBufEnd = pringBufStart + BUF_SIZE;
		playbuf = pringBufStart;

		/* Create a dummy buffer for changeing the index  */
		pdummy = osMemAlloc(READ_SIZE);/* 0626 */
		if ( pdummy == NULL ) {
			printf("Err: no dummy buffer\n");
			return FAIL;
		}
		
		/* open the raw pcm file inside storage */
		fd = vfsOpen("16BITPCM.PCM", 0, 0);
		if ( fd == 0 ) {
			printf("File not found...(%s)\n", "16BITPCM.PCM");
			return FAIL;
		}
		/* set the play mode and sample rate ready to triger the play */
		audFormat = 0x02;/* sign 16bit,mono , 0x03 is stereo */
		codecFreq = 3;/* 44100 */
		hwAudFromDramStart(audFormat, codecFreq, 0);
		readSize = 0;
		audPlay = 1;
		while( audPlay == 1 ){
			hwAudBufStsGet(&overFlow, &underFlow, &cnt, &inIdx, &outIdx);
			if ( underFlow == 1 ) {
				printf("Underflow...\n");
			}
			idxDistance = (inIdx >= outIdx) ?(inIdx - outIdx):(inIdx + (BUF_SIZE / 2) - outIdx);
			/* Fill data, note that the distance is words */
			if ( idxDistance < RIFF_REFILL_THRESHOLD ) { 
				readSize = vfsRead(fd, pdummy, READ_SIZE);
				printf("readSize[%d]\n",readSize);
				if( readSize < READ_SIZE ){
					audPlay = 0;/* The last packet audio data */
				}
				
				decodeSize = readSize;
				
				nrByteRemain = pringBufEnd - playbuf;
				if ( decodeSize < nrByteRemain ) {
					memcpy(playbuf, pdummy, decodeSize); 
					playbuf = playbuf + decodeSize;
				}
				else {
					printf("Turn around\n");
					memcpy(playbuf, pdummy, nrByteRemain); 
					playbuf = pringBufStart + (decodeSize - nrByteRemain);
					memcpy(pringBufStart, pdummy + (pringBufEnd - playbuf), decodeSize - nrByteRemain); 
				}
				/*Here is simply to increase the index of hardware inter counter*/
				/*But the maxmum is 2048 */ 
				hwAudDmaModeSet(1); /* DMA from DMEM to SDRAM */
				for ( ; decodeSize > 2048; decodeSize -= 2048 ) {
					dmemDmaRead(0, 0, (UINT32 *)pdummy, 2048, 0);
				}
				if ( decodeSize > 0 ) {
					dmemDmaRead(0, 0, (UINT32 *)pdummy, decodeSize, 0);
				}
				hwAudDmaModeSet(0);
			}	
			osTimeDly(10);
		}
		hwAudBufStsGet(&overFlow, &underFlow, &cnt, &inIdx, &outIdx);
		lastIdx = inIdx;
		/* play until buffer null */
		while(1){
			hwAudBufStsGet(&overFlow, &underFlow, &cnt, &inIdx, &outIdx);
			if ( outIdx > inIdx ) { /* case when in is on top while out is at buttom */
				if ( inIdx < 4 ) {
					if ( (outIdx + 4) > BUF_SIZE ) {	
						hwAudPause(1);
						break;
					}	
				}
			}
			else {
				/* The hardware counter may be kept forever when the rest is less than 4 words */
				if ( (outIdx + 4) > lastIdx ) {
					hwAudPause(1);
					break;
				}
			}
			//osTimeDly(40);
		}
		audStop();
		vfsClose(fd);
		osMemFree(pdummy);
		printf("Play End\n");
	}
#endif
/*andrew_gu@2005/Apr/12, 9:46 )*/


#if 1			
// show playtime & progress in audio play
//Update John 05-11-1	
void osdaudiotimeShow( )
{		

			UINT32 timeElapsed = 0 , timeTotal= 0, timeleft=0;
			audFileInfo_t  audFileInfo;	
			UINT32	osdW, osdH;
//			UINT32  SampleRate;
			   
			osdW = PANEL_1_OSD_W;
			osdH = PANEL_1_OSD_H;


		 	if ((avPlayNow ==OBJ_MP3)||(avPlayNow ==OBJ_WMA)||(avPlayNow ==OBJ_WAV))
			{
				if (avPlayNow ==OBJ_WAV)
				{
					/*if (audFileInfo.playDuration > 1000)
					{*/
					audAppFileInfoGet(NULL, &audFileInfo);
					timeTotal = audFileInfo.playDuration;
					timeElapsed = audFileInfo.playDuration - audFileInfo.residualTime;
					timeleft =audFileInfo.residualTime;


					if (timeTotal == 0)
					{
						timeTotal = 20;
						timeElapsed =10;
					}
					wavTime = timeElapsed;

				}

				else if (avPlayNow ==OBJ_MP3)
				{
					timeTotal = mp3GetTotalTime(fileattr.name);
					timeElapsed = mp3GetElapsedTime();
					timeleft = timeTotal-timeElapsed;

		//			printf("\n Mp3timeElapsed%d", timeElapsed);
	
				}
				else		
				
				{
					timeTotal = wmaGetTotalTime(fileattr.name);
					timeElapsed = wmaGetElapsedTime();
					timeleft = timeTotal-timeElapsed;
				}

				if (timeElapsed>timeTotal)
				{
					timeElapsed=timeTotal;
					timeleft=0;
				}

				osTimeDly(3);
				osdHProgressDraw(0, timeTotal/*100*/, timeElapsed/*mp3PlayProgressGet()*/, 0x7c);
				sio_psprintf(str, "%02d:%02d ", timeElapsed/1000/60, timeElapsed/1000%60);
				osdStrDisp(osdW/6-10, osdH-62-13, UserFont10x20, 0xd0, str);
				sio_psprintf(str, "%02d:%02d ", timeleft/1000/60, timeleft/1000%60);
				osdStrDisp(3*osdW/4-10, osdH-62-13, UserFont10x20, 0xd0, str);


					
			}


		
}
#endif
void F_Mp3SetVolum(SINT32 value)
{	
	if ( value > 9 || value < 0 )
		return;
	#if (CurrentIC8751==0)	
		if(SpeakFlag)
		{
			pbSetVolume(ws9712_OUT2, value);
		}
		else
		{
			pbSetVolume(ws9712_HP, value);
		}
	#else
		if(SpeakFlag)
		{
			pbSet8751Volume(ws9712_OUT2,value);
		}
		else
		{
			pbSet8751Volume(ws9712_HP,value);
		}
	#endif	

}

void
F_UserKeyGet(
	UINT32 *pKey,UINT8 wait
)
{
	UINT32 err;
	UINT32 *msg;

	msg = osQuePend(uiKeyMsgQ,wait,&err);
	if(err==OS_TIMEOUT) *pKey = 0xffffffff ;
	else *pKey = *msg;

	
}
//与269不一样有视频，音频
UINT8 *UiMp3Strings[MAX_IDMP3][LANGUAGES] = 
{
	{"Video",
	"视频",
	"跌繵",
	"película",
	"Кинофильм",
	"Film",
	"Film",
	"Film",
	"Filme",
	"厚叼坷",
	},
	{"Audio",
	"音频",
	"繵",
	"Música",
	"Нот",
	"Musik",
	"musique",
	"Musica",
	"Música",
	"坷叼坷",
	},
	{"No Video!",
	"没有视频!",
	"⊿Τ跌繵!",
	"Sin vídeo",
	"Нет Видео",
//	"Отсутствует видео-частота",
	"ohne Video",
	"pas de vidéo",
	"No video",
	"N\xe3o video",
	"厚叼坷 绝澜",
	},
	{"No Song!",
	"没有歌曲!",
	"⊿Τ簈Ρ!",
	"Ninguna canción",
	"Нет песни",
	"keinen Song",
	"pas de chanson",
	"No Canzone",
	"N\xe3o há música",
	"畴贰绝澜",
	},
	{  // Nopaly
	"Flat",
	"正常",
	"タ盽",
	"Plano",
	"плоско",
	"Normal",
	"Normal",
	"Normale",
	"Normal",
	"沥惑",
	},
	{
	"Heavy",
	"金属",
	"妮",
	"pesado",
	"тяжело",	
	"Rock",
	"Rock'n'roll",
	"Metale",
	"Metal",
	"陛加",
	},
	{
	"Pop",
	"流行",
	"瑈︽",
	"estallido",
	"шипучка",
	"POP",
	"pop",//"populaire",
	"Pop",
	"Popular",
	"蜡青",
	},
	{
	"Jazz",
	"爵士",
	"里",
	"Jazz",
	"джаз",
	"Jazz",
	"Jazz",
	"Jazz",
	"Jazz",
	"犁令",
	},
	{
	"Classical",
	"经典",
	"竒ㄥ",
	"Clásico",
	"классическо",
	"Classic",
	"Classique",
	"Classe",
	"Clássico",
	"努贰侥",
	},
	{
	"Soft",
	"柔和",
	"琗㎝",
	"Suave",
	"мягко",	
	"Sanftheit",
	"doux",//	"Douce",
	"Leggero",
	"Doce",
	"何靛反促",
	},
	{"Loading...",
	"装载中...",
	"杆更い...",
	"Cargando...",
	"Идет загрузка",
	"Loading...",
	"chargement...",//"charge en cours...",
	"Caricando...",
	"Está a instalar...",
	"肺爹...",
	},	
};

UINT8 *F_Mp3GetString(UINT16 stringId)
{
    UINT8* str;
    UINT32 languageId = sPara.language;

    if (languageId >= LANGUAGES)
    {
		languageId = LanguageEnglish;
    }

    if(stringId >= MAX_IDMP3) 
    {
		return 0;
    }

    if(UiMp3Strings[stringId][languageId][0]=='\0')
    {
	    str = UiMp3Strings[stringId][0];
    }
	else
	{
	    str = UiMp3Strings[stringId][languageId];
	}

	return str;
}


//xu add
//Function:
//	变更长文件名为指定长度
void F_CutLongNameToShortName(UINT8 *pui8str,UINT32 ui32Len)
{
	UINT32 i,j;
	UINT32 ui32post;

	ui32post = ui32Len - strlen("...")	;
	
	i = strlen(pui8str);
	
	if(i>ui32post)
	{
		for(j=0;j<i;)		
		{
			if( *(pui8str+j) < 0x80 )
			{
				if(j == ui32post)
				{
					*(pui8str+j) = '\0';
					strcat(pui8str,"...");	

					break;
				}
				else
				{
					j++;
				}
			}
			else
			{
				if(j == ui32post)
				{
					*(pui8str+j) = '\0';
					strcat(pui8str,"...");	
					
					break;
				}
				else if((j+1) == ui32post)
				{
					*(pui8str+j+2) = '\0';
					strcat(pui8str,"..");	
					printf("Adjust xu\n");
					break;
				}
				else
				{
					j += 0x02	;
				}
			}
		}
	}
}

/**
**/
void ws9712EqSet(UINT32 EQFlag)
{
	UINT32 ui32RegVal;
	
	hwAudAc97RegRead(0x20, &ui32RegVal);
	ui32RegVal |= (1<<13);
	hwAudAC97RegWrite(0x20, ui32RegVal);
	switch (EQFlag)
	{
		case EQFlat:
			hwAudAC97RegWrite(0x08, 0x0f4f);
			hwAudAC97RegWrite(0x22, 0x0000);
			break;
			
		case EQRock:
			hwAudAC97RegWrite(0x08, 0x8406);
			hwAudAC97RegWrite(0x22, 0x0005);		
			break;
			
		case EQPop:
			hwAudAC97RegWrite(0x08, 0x0946);
			hwAudAC97RegWrite(0x22, 0x0007);		
			break;
			
		case EQJazz:
			hwAudAC97RegWrite(0x08, 0x1354);
			hwAudAC97RegWrite(0x22, 0x0025);		
			break;
			
		case EQClassical:
			hwAudAC97RegWrite(0x08, 0x0f42);
			hwAudAC97RegWrite(0x22, 0x0003);		
			break;		
			
		case EQDance:
			hwAudAC97RegWrite(0x08, 0x8103);
			hwAudAC97RegWrite(0x22, 0x002a);		
			break;
			
		case EQHeavy:
			hwAudAC97RegWrite(0x08, 0x0343);
			hwAudAC97RegWrite(0x22, 0x0000);		
			break;
			
		case EQDisco:
			hwAudAC97RegWrite(0x08, 0x9404);
			hwAudAC97RegWrite(0x22, 0x0000);		
			break;
			
		case EQSoft:
			hwAudAC97RegWrite(0x08, 0x941a);
			hwAudAC97RegWrite(0x22, 0x0000);		
			break;
			
		case EQLive:
			hwAudAC97RegWrite(0x08, 0x0f4f);
			hwAudAC97RegWrite(0x22, 0x0009);				
			break;
			
		case EQHall:
			hwAudAC97RegWrite(0x08, 0x0442);
			hwAudAC97RegWrite(0x22, 0x0009);				
			break;		
						
	}
}


void RefurbishBatterGaugeFunc()
{
	extern void uiShowBatteryLevelIcon(UINT8 BatteryLevel) ;
	extern UINT8 DcPlug ;
	extern  UINT8 BatteryLevel ;
	 if (DcPlug ==1)
	 {
	 	 osdStrDisp (214, 0, UserIcon10x20, 0xd0, "\x92\x93");
	 }
	 else
	 {
		uiShowBatteryLevelIcon( BatteryLevel) ;
 	 }
}


/* Paul@2006/08/11 - 10:46 add start */
UINT32 mp3_CheckFileFormat(UINT8* filename)
{
	UINT32 file;
	UINT8  buf[14];

	file = vfsOpen(filename, 0, 0);
	if (file == 0)
	{
		return FAIL;
	}

	vfsRead(file, buf, 10);		
	vfsClose(file);

    if (buf[0] == 'I' &&
        buf[1] == 'D' &&
        buf[2] == '3' &&
        buf[3] != 0xff &&
        buf[4] != 0xff &&
        (buf[6] & 0x80) == 0 &&
        (buf[7] & 0x80) == 0 &&
        (buf[8] & 0x80) == 0 &&
        (buf[9] & 0x80) == 0)
    {
		return SUCCESS;
    }
	
    UINT32 newhead = 
      buf[0] << 24 |
      buf[1] << 16 |
      buf[2] <<  8 |
      buf[3];
	
    // head_check:
    if( (newhead & 0xffe00000) == 0xffe00000)
    {
		return SUCCESS;
    }
	
	return FAIL;	
}

/* Paul@2006/08/11 - 10:46 add end */
/**
	Asf or Wma File header
**/
typedef struct __attribute__((packed)) 
{
    long        biSize; // sizeof(BITMAPINFOHEADER)
    long        biWidth;
    long        biHeight;
    short       biPlanes; // unused
    short       biBitCount;
    long        biCompression; // fourcc of image
    long        biSizeImage;   // size of image. For uncompressed images
                               // ( biCompression 0 or 3 ) can be zero.


    long        biXPelsPerMeter; // unused
    long        biYPelsPerMeter; // unused
    long        biClrUsed;     // valid only for palettized images.
                               // Number of colors in palette.
    long        biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
  short   wFormatTag; // value that identifies compression format
  short   nChannels;
  long    nSamplesPerSec;
  long    nAvgBytesPerSec;
  short   nBlockAlign; // size of a data sample
  short   wBitsPerSample;
  short   cbSize;    // size of format-specific data
} WAVEFORMATEX;

typedef struct __attribute__((packed)) {
  unsigned char guid[16];
  unsigned long long size;
} ASF_obj_header_t;

typedef struct __attribute__((packed)) {
  ASF_obj_header_t objh;
  unsigned int cno; 				// number of subchunks
  unsigned char v1; 				// unknown (0x01)
  unsigned char v2; 				// unknown (0x02)
} ASF_header_t;

typedef struct __attribute__((packed)) {
  unsigned char client[16]; 		// Client GUID
  unsigned long long file_size;
  unsigned long long creat_time; 	//File creation time FILETIME 8
  unsigned long long packets;    	//Number of packets UINT64 8
  unsigned long long end_timestamp; //Timestamp of the end position UINT64 8	unit of 100ns
  unsigned long long duration;  	//Duration of the playback UINT64 8
  unsigned long start_timestamp; 	//Timestamp of the start position UINT32 4
  unsigned long unk1; 				//Unknown, maybe reserved ( usually contains 0 ) UINT32 4
  unsigned long flags; 				//Unknown, maybe flags ( usually contains 2 ) UINT32 4
  unsigned long packetsize; 		//Size of packet, in bytes UINT32 4 	    -- min_pktsize		
  unsigned long packetsize2; 		//Size of packet ( confirm ) UINT32 4	    -- max_pktsize
  unsigned long frame_size; 		//Size of uncompressed video frame UINT32 4 -- max_bitrate
} ASF_file_header_t;

typedef struct __attribute__((packed)) {
  unsigned char type[16]; 			// Stream type (audio/video) GUID 16
  unsigned char concealment[16]; 	// Audio error concealment type GUID 16
  unsigned long long unk1; 			// Unknown, maybe reserved ( usually contains 0 ) UINT64 8
  unsigned long type_size; 			//Total size of type-specific data UINT32 4
  unsigned long stream_size; 		//Size of stream-specific data UINT32 4
  unsigned short stream_no; 		//Stream number UINT16 2
  unsigned long unk2; 				//Unknown UINT32 4
} ASF_stream_header_t;

typedef struct __attribute__((packed)) {
  unsigned char streamno;
  unsigned char seq;
  unsigned long x;
  unsigned char flag;
} ASF_segmhdr_t;


ASF_header_t asfh;
ASF_obj_header_t objh;
ASF_file_header_t fileh;
ASF_stream_header_t streamh;

/**
**/
#define AV_TIME_BASE		10000

#define C_DECODE_STREAM		0

UINT32 wma_get_total_time(UINT8* filename)
{
	UINT32 asf_file;
	UINT32 pos;
	UINT32 ret_code = 0;
	UINT32 have_time_flag = 0;
	
	if (filename == NULL)
	{
		return ret_code;
	}
	
	asf_file = vfsOpen(filename, 0, 0);
	if (asf_file == 0)
	{
		return ret_code;
	}

	vfsRead(asf_file, &asfh, sizeof(asfh)); // header obj
	if (*((unsigned int*)&asfh.objh.guid) == 0x75B22630)		// guid_header
	{		
		while(vfsRead(asf_file, &objh, sizeof(objh))>0)
		{
			pos = vfsTell(asf_file);				
			switch(*((unsigned int*)&objh.guid))
			{
				case 0xB7DC0791: // guid_stream_header	
					have_time_flag = 1;				
					break;
					
				case 0x8CABDCA1: // guid_file_header
					vfsRead(asf_file, &fileh, sizeof(fileh));
					printf("packets: %d  flags: %d  pack_size: %d  frame_size: %d\n",(int)fileh.packets,(int)fileh.flags,(int)fileh.packetsize,(int)fileh.frame_size);
					printf("end = %x, duration = %x, start = %x\n",
						   (int)fileh.end_timestamp,
						   (int)fileh.duration,
						   (int)fileh.start_timestamp
						   );				
					ret_code = fileh.duration/AV_TIME_BASE
							- fileh.start_timestamp;	
					have_time_flag = 1;								
					break;
		  
				case 0x75b22636: // guid_data_chunk
					have_time_flag = 1;	
					break;
			}
			
			if (have_time_flag)
			{
				break;			
			}
			
			vfsLseek(asf_file, pos+objh.size-sizeof(objh), SEEK_SET);
		}		
	}
	vfsClose(asf_file);
	
	return ret_code;
}

