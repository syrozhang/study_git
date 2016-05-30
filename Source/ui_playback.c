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
//#define RS_232_DBG
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
#include "avi_api.h"
#include "mov_api.h"
#include "hw_disp_ext.h"
#include "osd_main.h"
#include "osd_common.h"
#include "osd_api.h"
#include "panel.h"
#include "exif_custom.h"
#include "snap_custom.h"
#include "zoom.h"
#include "pub_api.h"
#include "hw_cpu.h"
#include "timer.h"
#include "xiaoao.h"
#include "hw_aud.h"
#include "UiStrings.h"
#include "ws9712.h"
#include "gpio.h"


#include "extern.h"
#include "ui_mp3.h"
#include "menu_capture.h"


extern UINT32 wmaVol;
extern void   wm8751RegSet(UINT32,UINT32);
extern UINT32 pbAudioInfoGet(UINT32 idx);
extern void   pbInit();
extern UINT8  g_playrecStatus;
extern UINT8  lcdOsdDisp;
extern UINT8  mp3Repeat;

/*extern void osdMenuInfo(void);*/

UINT32 nandRsvBlkWrite(UINT32, UINT32, UINT8 *);


//void pbThumb(UINT32 offsetX, UINT32 offsetY, UINT32 w, UINT32 h);
void	pbPageRefresh(UINT8 isClear);
void pbVolumWindowRfsh(SINT32 value);
//void	pbShowM(UINT32, UINT32, UINT32, UINT32);
//void	pbShowS(UINT32);

UINT32  ui32EQFlag = EQFlat;

UINT32	pbFileType[9];
UINT32	avPlayNow=OBJ_NONE;
static	void *semApp;
UINT8	pbDispCol, pbDispRow;
UINT16	pbDispStart;
UINT32	pbDisp = PB_DISP_FOUR;
UINT32	pbZoomFactor = 100;
UINT32	asfTime=0;
UINT32	wavTime=0, wavProgress=0;
UINT8 	pressVolume=0; /*Julie@2004.9.17*/
UINT8     infoshow = 0;
UINT32     TmrFunc;
UINT32	ChgPvSetting=0;	/* 1=restore pv setting before process any key */
UINT8	imgBuf[0x500];		/* dlchiou - 03/05/22 13:55	*/
UINT32	gPB;
UINT32   videoTotalTime;
//UINT32	totalTime = 0;		//yyqq
//static UINT32	videoLastFlag;
//static UINT32	videoLastFrame;


xdcfAttrElm_t fileattr;
xdcfAttrElm_t dirattr;
extern	OS_EVENT *uiKeyMsgQ;
extern UINT16 sub_menu;/*Gu@ 2004/06/21, 14:08 */
extern UINT32 file_filter;
extern UINT8 SpeakFlag;//wendy for speak
extern UINT8 SpeakFlagMode;//wendy for speak
extern UINT32 ui32SysKeyState ;

extern	UINT32	fileType;
extern	UINT32	fileCnt;
extern	UINT32	dirCnt;
extern	UINT32	fileIdx;
extern	UINT32	dirIdx;

#define	OSD_DEBUG


#define	xDCF_ATTR_PROTECT		xDCF_ATTR_READONLY |fileattr.attribute
#define	xDCF_ATTR_DISPROTECT		(~xDCF_ATTR_READONLY) &fileattr.attribute

void	uiPlayMP4(void)
{
	UINT32	key = UI_KEY_MODE_PLAYBACK;
	SINT32	i;
	UINT32	err = SUCCESS;	
	UINT32	filetype;	
	UINT8*	pbuf;
	UINT32	dispW, dispH;
	UINT32	dispAddr;


	#ifdef	OSD_DEBUG
	printf("\nP/B ");
	#endif

	semApp = osEventFindByName("APP_SEM");
	uiKeyMsgQ = osEventFindByName("UI_KEY");

	/*isMenuOn = 0;*/
	pbDispStart = 1;
	dirIdx = 0;
	fileIdx = 0;

	while ( ( uiState & UI_MODE_MASK ) == UI_MODE_PLAYBACK )
	{
		if(ChgPvSetting)
		{
			hwDispPvSetWrite(imgBuf);
			ChgPvSetting = 0;
		}
		
		if (ui32NextState != uiState)
		{
			break;
		}		
	/*
		if (isMenuOn)
		{
			osdCustomMenu (key);
			if (isMenuOn == 0)
			{
				key = UI_KEY_MODE_PLAYBACK;
			}
		}

		if (isMenuOn == 0)
		{*/
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
					F_Mp4FileListRefresh(fileIdx,0);
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
					F_Mp4FileListRefresh(fileIdx,0);
					
					break;

				case	UI_KEY_DIR_LEFT:
					break;
					
				case	UI_KEY_DIR_RIGHT:
					break;

				case	UI_KEY_FUNC_MENU:
					infoshow = 0;
					if(pbDisp == PB_DISP_FOUR)
					{
						pbInit();
						osdLcdDisp(1);
						menuReturn(UI_MAINMENU, 0);
					}
					break;

				case	UI_KEY_FUNC_OK:
					infoshow = 0;
					if (fileCnt != 0)
					{
						if(pbDisp == PB_DISP_FOUR)
					 		pbDisp = PB_DISP_ONE;
					}
					F_PlayMp4() ;
//					pbPageRefresh(1);
					break;

				case	UI_KEY_FUNC_B:
					F_Mp4MenuFunc();
					break;
					
				case	UI_KEY_FUNC_A:
					
					break;

				case	UI_KEY_MODE_PLAYBACK:

					osTaskSuspend(osTaskFindByName("AAA"));
					/*hwDispPvEn(0);*/
					pbDisp =  PB_DISP_FOUR;  /*PB_DISP_ONE;*/
					g_playrecStatus = 0;
					ChgPvSetting = 0;
					sysgMemDispAddrGet(&gPB);
					//wendy@050131 add for video beign

  					xdcfInit(mediaRootNameStr, mediaRootNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);						
					xdcfCurRootDirSet(mediaRootNameStr);
	   				printf("Play Back\n");						
					//wendy@050131 add for video end

					//wendy@050131 add for video end
					file_filter = 	FILTER_VIDEO ;
					xdcfFilterSet(file_filter);
		#if 0			
					xdcfDirCountGet(&dirCnt);	/* get total dir count under DCIM */
					/*Gu @2003.12.9 16:38 begin*/
					if((dirIdx<1) || (dirIdx >dirCnt))
					/*Gu @2003.12.9 16:38 end*/
					{
						dirIdx = 1;
						uiFindValidDir(0);
					}

					err	= xdcfCurDirByPosSet(dirIdx);
		#endif
					
					err	= xdcfFileCountGet(&fileCnt);
					if((fileIdx<1) || (fileIdx >fileCnt))
					{
						fileIdx = 1;
					}
					xdcfCurDirAttrGet(&dirattr);
//					osdLcdDisp(1);
					F_Mp4OpenOsd();
					F_Mp4FileListRefresh(1,1) ;
					break;

				default:
					#ifdef	OSD_DEBUG
					printf("\nKEY=%0x ", key);
					#endif
					break;
			}
		keyGet(&key);
	}
	
	uiState = ui32NextState;	
}

#define VEDIO_FILE_MAX	    5
#define	VEDIO_FILENAME_MAX	13
#define	MP4_FILELIST_X		52
#define MP4_FILELIST_SX     46
#define	MP4_FILELIST_Y		54

#define	MP4_FILELIST_SIZE	32

/***********************************
bgrefresh==1  background refresh

***********************************/
UINT32	F_Mp4FileListRefresh(UINT32 idx,UINT8 bgrefresh)
{

	UINT32	i, j;
	UINT32	err	= SUCCESS;
	UINT32	pPBAddr, phyOsdDBAddr;
	UINT8	*pImgBuf, osdStr[6]	= {	"*WAV*"	};;
	UINT32	w, osdFontWidth, osdFontHeight;
	UINT32	tempIdx = 1;
	UINT32	ui32dispW, ui32dispH;
	UINT32	osdW, osdH;
	static UINT8 page_idxI = 0;

	UINT32 dispW, dispH;
	UINT32 gpb;
	dispW = PIP_W;
	dispH = PIP_H;

	UINT32	pbThumbW, pbThumbH, pbThumbOffX, pbThumbOffY;

	xdcfAttrElm_t attr;
	UINT8 	space[VEDIO_FILENAME_MAX];
	UINT8	str[VEDIO_FILENAME_MAX] ;
	UINT32	y[VEDIO_FILE_MAX] = {MP4_FILELIST_Y,MP4_FILELIST_Y+MP4_FILELIST_SIZE,
		MP4_FILELIST_Y+MP4_FILELIST_SIZE*2,
		MP4_FILELIST_Y+MP4_FILELIST_SIZE*3,MP4_FILELIST_Y+MP4_FILELIST_SIZE*4};
	UINT8	page_idx ;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;	

	pbThumbW = 160-40;
	pbThumbH = 120;
	pbThumbOffX =150 +14+4;
	pbThumbOffY = 44+10;
	
//	osdClearScreen(0x00);

	if(idx ==0) idx =1 ;
	
	if(bgrefresh)
	{
		
		osdClearScreen(0) ;
		sysgMemDispAddrGet(&gpb);

		hwDispPreviewSizeGet(&dispW, &dispH);
		hwPlayBufSizeSet(dispW,	dispH);
		hwPlayBufAddrSet(gpb);
	
	//	pbDramImgPlay(mp4, sizeof(mp4), gpb, dispW, dispH);
		pbMainBack();
		osdStrDisp(28, 3, UserFont10x20, 0xd0, F_Mp3GetString(title_mp4_s));		
	}
#if 0
	if(bgrefresh)
	{
		UINT32 gpb;
		ui32dispW = PIP_W;
		ui32dispH = PIP_H;
		osdClearScreen(0x00);

		hwDispPreviewSizeGet(&ui32dispW, &ui32dispH);
		sysgMemDispAddrGet(&gpb);
		hwPlayBufSizeSet(ui32dispW,	ui32dispH);
		hwPlayBufAddrSet(gpb);
//      	hwFrmCnvModeSet(0);
//   	       hwCammodeSet(5);
//     	 hwCdspDataSource(1);
		pbDramImgPlay(mp4, sizeof(mp4), gpb, ui32dispW, ui32dispH);
	}
#endif	
//	osdLcdDisp(1);
	F_Mp4OpenOsd();

	if(fileCnt==0)
	{
		osdStrDisp((osdW-9*10)/2+80-20, (osdH-20)/2-20, UserFont10x20, 0xd0, F_Mp3GetString(no_vedio_s));
   	   	osTimeDly(100);       		
		menuReturn(UI_MAINMENU, 0);				
		return	0 ;
	}
	#if 1 // tzw060121 for Ico
	if(page_idxI != (idx+VEDIO_FILE_MAX-1)/VEDIO_FILE_MAX)
	{
		page_idxI = (idx+VEDIO_FILE_MAX-1)/VEDIO_FILE_MAX;

		sysgMemDispAddrGet(&gpb);

		hwDispPreviewSizeGet(&dispW, &dispH);
		hwPlayBufSizeSet(dispW,	dispH);
		hwPlayBufAddrSet(gpb);
		pbMainBack();
	//	pbDramImgPlay(mp4, sizeof(mp4),gpb, dispW, dispH);
		osdStrDisp(28, 3, UserFont10x20, 0xd0, F_Mp3GetString(title_mp4_s));		

	}	
	#endif
	
	memset(space, ' ', VEDIO_FILENAME_MAX-1);
	space[VEDIO_FILENAME_MAX-1] = '\0';


	page_idx = ((idx-1)/VEDIO_FILE_MAX)*VEDIO_FILE_MAX +1;

 	if (idx !=0)
	{
		for(i=0; i<VEDIO_FILE_MAX; i++)
			{
				osdStrDisp(MP4_FILELIST_X, y[i]+4, UserFont10x20, 0x00, space);/*clear*/
					printf("==========index = %d\n",page_idx);	

				#if 1 // tzw060121 for Ico
				if ((page_idxI-1)*VEDIO_FILE_MAX + i < fileCnt)
				{
					pbDisplayMp4_Ico(11,24, y[i]+6);
				}	
				#endif
		
				if(i <= fileCnt-page_idx)
				{
					xdcfCurFileByPosSet(i+page_idx);
					err = xdcfCurFileAttrGet(&attr);
					if(SUCCESS == err)
					{
							strncpy(str, attr.name,VEDIO_FILENAME_MAX);					
							str[strlen(str)-4] = '\0' ;
							
					}
					else
					{
						strncpy(str, space, VEDIO_FILENAME_MAX);
					}						
					printf("=======name = %s\n",attr.name);

					if(i+page_idx == idx) 
					{
					//	space[VEDIO_FILENAME_MAX-4] = '\0' ;
					//	osdStrDisp(MP4_FILELIST_X, y[i], UserFont10x20, 0x0f, space);/*clear*/					
						pbDisplaySelFile_Ico(MP4_FILELIST_SX,y[i]);
						osdStrDisp(MP4_FILELIST_X, y[i]+4, UserFont10x20, 0x10, str);						

						if(attr.attribute&xDCF_ATTR_READONLY)	//John @01/22/06
						{
							osdStrDisp(MP4_FILELIST_SX+86,y[i]+5, IconLock, 0x10,"\x1");
						}
						else
						{
							osdStrDisp(MP4_FILELIST_SX+86, y[i]+5, IconLock, 0x0,"  ");	
						}	
					}
					else	 
					{
						pbDisplayCleFile_Ico(MP4_FILELIST_SX,y[i]);

						osdStrDisp(MP4_FILELIST_X, y[i]+4, UserFont10x20, 0xF0, str);			
						
						if(attr.attribute&xDCF_ATTR_READONLY)	//John @01/22/06
						{
							osdStrDisp(MP4_FILELIST_SX+86,y[i]+5, IconLock, 0xf0,"\x1");
						}
						else
						{
							osdStrDisp(MP4_FILELIST_SX+86, y[i]+5, IconLock, 0x0,"  ");	
						}	
					}
					
				}	
				
			}
		//David		
		#if 0
					UINT8 str1[50];
					UINT8 str2[50];		
					videoLastFlag = asfAppStatusGet();
					videoLastFrame =  asfAppKeyPktGet ();
					sio_psprintf(str1, "%d ",videoLastFlag);					
					sio_psprintf(str2, "%d ",videoLastFrame);										
					osdStrDisp(50, 32, UserFont16x20, 0x10,str1);
					osdStrDisp(50, 62, UserFont16x20, 0x10,str2);
		#endif					

 		}	

//---------------------------------------------
#if 1

	osdFontWidth = UserFont[2];
	osdFontHeight= UserFont[3];

//	pImgBuf		= (UINT8*) (((UINT32 )imgBuf) |	0xac000000);
//	phyOsdDBAddr= (((UINT32)pImgBuf) & 0x00FFFFFF) >> 1;
//	hwDateFontScalerSet(1);

	/*sysgMemThumbAddrGet(&pPBAddr);*/
	/* sysgMemFreeAddrGet(APP_STILL_PB, &pPBAddr);*/ /* ??? */
	pbThumbBufferGet(&pPBAddr);

	hwDispPreviewSizeGet(&ui32dispW, &ui32dispH);

	hwPlayBufSizeSet(ui32dispW,	ui32dispH);
	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/

	err	= xdcfFileTypeGet(idx, &fileType);
	if ((err = xdcfCurFileByPosSet(idx)) != SUCCESS)
	{
		return;
	}
	err	= xdcfCurFileAttrGet(&fileattr);
	printf("======%s====\n",fileattr.name);
	/* sysgMemPBDecompBufferGet(&pPBAddr); */
	if(fileType == xDCF_FILETYPE_AVI)
		err = aviThumbGet(fileattr.name, pPBAddr,	pbThumbW, pbThumbH);
	else if(fileType == xDCF_FILETYPE_ASF)
		err = asfThumbGet(fileattr.name, pPBAddr,	pbThumbW, pbThumbH);
	else
		err = movThumbGet(fileattr.name, pPBAddr,	pbThumbW, pbThumbH);					
//	asfThumbGet(fileattr.name, pPBAddr,	pbThumbW, pbThumbH);
	hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
			gPB, ui32dispW,	ui32dispH,
			pbThumbOffX,	pbThumbOffY, 0);					

	UINT8 timestr[32] ;
//	sio_psprintf(timestr, "%04d:%02d:%02d:%02d ", fileattr.time.year,fileattr.time.mon,fileattr.time.hour, fileattr.time.min);
	sio_psprintf(timestr, "%04d.%02d.%02d ", fileattr.time.year,fileattr.time.mon,fileattr.time.day);

	
//	osdStrDisp(164, 180, UserFont10x20, 0xf0, timestr) ;
//	osdStrDisp(164, 204, UserFont10x20, 0xf0, timestr) ;
	osdStrDisp(178, 182, UserFont10x20, 0xf0, timestr) ;
	printf("file attribute %x",fileattr.attribute);
	#if 0
	if(fileattr.attribute&xDCF_ATTR_READONLY)
	{
		osdStrDisp(295, 212, IconLock, 0xf0,"\x1");
	}
	else
	osdStrDisp(295, 212, IconLock, 0xd0,"  ");
	#endif

#endif
}

/**
**/
#define	MP3_ITEM_MAX  4
#define	MP3_SUBITEM_MAX  3
typedef	UINT32 (*pMp3SubMenuProc)(UINT8 idx) ;
typedef	UINT32	(*pMp3MenuProc)(void) ;

extern pMp3MenuProc	RefreshItem[MP3_ITEM_MAX] ;
extern pMp3SubMenuProc	RefreshSubitem[MP3_ITEM_MAX] ;
extern pMp3SubMenuProc  F_SelectedDeal[MP3_ITEM_MAX] ;
extern	UINT8	Mp3SubMax[MP3_ITEM_MAX] ;
extern    	UINT8    ui8_AutoCloseLCD ;
void	F_Mp4MenuFunc()
{
	UINT32	key ;
	UINT8 item=0,	subitem=1;
	xdcfAttrElm_t attr;	
	UINT32	err ;
	UINT32 storage;	
	osdClearScreen(0x00);
	osdStrDisp(38, 3, UserFont10x20, 0xd0, F_Mp3GetString(title_mp3_s));

	ui8_AutoCloseLCD = 1 ;		
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
					item = MP3_ITEM_MAX-1;
				
				RefreshItem[item]();
				break ;
			case	UI_KEY_DIR_RIGHT:
				if(item<MP3_ITEM_MAX-1) 
				{
					item++ ;
				}
				else 
					item = 0;
				RefreshItem[item]();
				break ;
			case	UI_KEY_FUNC_OK:
				/* john @02/07/06 */
				if (F_SelectedDeal[item](subitem))	
				{
						MessageShow (MenuSuccess);
				}
				F_Mp4FileListRefresh(fileIdx,1);
				return ;				
				break ;
				
			case	UI_KEY_ACCIDENT:
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);
				return;
				
			case	UI_KEY_FUNC_MENU:
			case	UI_KEY_FUNC_B:
				F_Mp4FileListRefresh(fileIdx,1);
				return ;
				break ;
		}
	}
}



void pbSetVolume(UINT8 device, SINT32 value)
{
	UINT32 vol;
	
	if(value == -1||value==0)
	{
		vol = 0x8000;//mute
	}
	else
	{
		value = (value+1)*3+1;		/* value max = 9 */
		vol = (31 - value) | ((31 - value) << 8);
	}
	#if Oem268_Dc260	
		/* DC268 */
		hwAudAC97RegWrite(0x18, 0x2606);//tzw add for bug 13 060420
	#else
		/**apart about track fqdao_add for 260A 06.7.3****/
		if(device==ws9712_OUT2)
		{
			wsAc97SetRegValEx(0x16, (1<<8),(1<<8));
		}
		/**apart about track fqdao_add for 260A 06.7.3****/
		hwAudAC97RegWrite(0x18, 0x2404);//tzw add for bug 13 060420			
	#endif
	
	ws9712SetVolume(device, vol);
}

//-------------------Davis:patch_2005/Apr/25 begin
//add function to set volum value,copy pbVolumWindowRfsh 
void SetVolumValue(SINT32 value)
{
#if 0	
//	UINT32	osdW, osdH;
//	UINT8 str[4];
	UINT32 vol;
//	UINT8  i, grade;

//	osdW = PANEL_1_OSD_W;
//	osdH = PANEL_1_OSD_H;
//	grade = value/3;
	//if(pressVolume)
	if(1)
	{	
		if(value == -1||value==0)
		vol = 0x8000;
		else
		//wendy@050124 add begin 
		{
			if(SpeakFlag)
			vol =(31 -value) | ((31 - value) << 8);
			else
			vol = (31 - value) | ((31 - value) << 8);
		}
		//wendy@050124 add end 		
		//andrew(
		 
		if(SpeakFlag)
			ws9712SetVolume(ws9712_OUT2, vol);
		else
			ws9712SetVolume(ws9712_HP, vol);
		//andrew)
          	sPara.mp3vol=mp3Vol ;//wendy@050121 add for record vol
		}	
	else{
		if(value == -1)
		{
			vol = 0x8000;
			hwAudAC97RegWrite(0x04, 0x8000);	/* volume */   
			hwAudAC97RegWrite(0x02, 0x8000);	/* volume */
		}
	}
#endif	
}
//-------------------Davis:patch_2005/Apr/25 end
void pbVolumWindowRfsh(SINT32 value)
{
	UINT32	osdW, osdH;
	UINT8 str[4];
	UINT32 vol;
	UINT8  i, grade;

	
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	grade = value;
	if(pressVolume)
	{	
		if(pressVolume==1)
		{
			osdBarDraw((osdW/2-40), osdH-70, 100, 30, 0x00); //wendy update 050125
			for( i = 1; i <=grade; i++)
			{
			    if(!grade)  //fqdao_add for bug 06.5.22 
			    {
				osdBarDraw((osdW/2-40), osdH-70, 100, 30, 0x00); //wendy update 050125
			    }
			    else
			    {
				osdBarDraw((osdW/2-40+ i*9), osdH-48-i*2 , 4, 3+i*2, (i > 7) ? 0x01 : 0x03);  //wendy update 050125
			    }
			}
		}


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



#if 0		

		if(value == -1||!value)
			vol = 0x8000;
		else
		//wendy@050124 add begin 
		{
			if(SpeakFlag)
			vol =(31 -value) | ((31 - value) << 8);
			else
			vol = (31 - value) | ((31 - value) << 8);
		}

		//wendy@050124 add end 
		
		//andrew(
		#if 0
           	if(SpeakFlag)
		{ 
			 if(SpeakFlagMode)	
				hwAudAC97RegWrite(0x02, 0x00c0|vol);
			 else
				hwAudAC97RegWrite(0x02, 0x0080|vol);
           	}
	       else
			hwAudAC97RegWrite(0x04, 0x0080|vol);	/* volume */  
		  #endif
#if 0
		  
		if(SpeakFlag)
			ws9712SetVolume(ws9712_OUT2, vol);
		else
			ws9712SetVolume(ws9712_HP, vol);
		//andrew)
#endif
		sPara.mp3vol=mp3Vol ;//wendy@050121 add for record vol

#endif		


		}	
	
	//wendy@050204 add for clear Volume begin
	else

	{
	//	osdStrDisp(osdW/2-40 ,osdH-70, UserFont, 0x00, "       "); //wendy update 050125
		osdBarDraw((osdW/2-40), osdH-70, 100, 30, 0x00);
		//wendy@050301 begin
		if(value == -1)
		{
			vol = 0x8000;
			hwAudAC97RegWrite(0x04, 0x8000);	/* volume */   
			hwAudAC97RegWrite(0x02, 0x8000);	/* volume */
		}
		//wendy@050301 end
	}
	//wendy@050204 add for clear Volume end
	/*sio_psprintf(str, "%d", value);
	osdStrDisp(400, osdH-65, UserFont, 0x0, "  ");
	osdStrDisp(400, osdH-65, UserFont, 0x0, str);	*/
}
/**************************************************************************
 *                                                                        *
 *  Function Name: pbPageRefresh                                          *
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
 #if 0
void pbPageRefresh(UINT8 isClear)
{
	UINT32 err = SUCCESS;

	if(pbDisp == PB_DISP_ONE)
	{
		pbDispCol = 1;
		pbDispRow = 1;
	}
	else if(pbDisp == PB_DISP_FOUR)
	{
		pbDispCol = 2;
		pbDispRow = 2;
	}

	else if(pbDisp == PB_DISP_NINE)
	{
		pbDispCol = 3;
		pbDispRow = 3;
	}
	else/* if(pbDisp == PB_DISP_PAN)*/
	{
		pbDispCol = 1;
		pbDispRow = 1;
		pbDisp = PB_DISP_ONE;
	}
	pbDispStart = ((fileIdx-1)/(pbDispCol*pbDispRow))*(pbDispCol*pbDispRow)+1;

	/*pbZoomFactor = 100;
	zoomFactSet(pbZoomFactor);*/

	if (fileCnt == 0)
	{
		//pbLoadBG(1);
		if (file_filter == FILTER_VIDEO)    //Danny
			pbMenu(2,2);
		else
			pbMenu(2,4);
		
		osdIconDisp(isClear);
		/*ui_ShowPbMenuInfo(); Julie*/
		printf(" \n fileCnt == 0");
        osTimeDly(100);       //Danny   --05/05/26
		menuReturn(UI_MAINMENU, 0);
	}
	/*else
		hwDispPvEn(1);*/

	if ((err = xdcfCurFileByPosSet(fileIdx)) != SUCCESS)
	{
		#ifdef	OSD_DEBUG
		printf(" #FILE_ERR ");
		printf("fileIdx=%d\n", fileIdx);
		#endif
		return;
	}
	err	= xdcfCurFileAttrGet(&fileattr);
	err	= xdcfFileTypeGet(fileIdx, &fileType);
	if(pbDisp == PB_DISP_ONE)
	{
	
        /*************************05-05-21*/
		if ((fileType == xDCF_FILETYPE_ASF)||(fileType == xDCF_FILETYPE_AVI)||(fileType == xDCF_FILETYPE_MOV))
		{
			while(1)
			{		
				osdClearScreen(0x00);
				
				pbShowS(fileIdx);
				
				if(avPlayNow == OBJ_NONE)
				{
					break;
				}
				#if 34  //for bug 34 
				else if (	g_playrecStatus == STATUS_UP)
				{
					if(fileIdx==1)
					{
						fileIdx =  fileCnt;			
					}
					else
					{
						fileIdx -- ;
					}
				}
				#endif 
				else
				{	
					if(fileIdx>=fileCnt)
					{
						fileIdx = 1 ;			
					}
					else
					{
						fileIdx ++ ;
					}
				}
			}
		}
		else
		{
				osdClearScreen(0x00);
				pbShowS(fileIdx);
				osdIconDisp(0);
		}
	
	/*****************************/
	}
	else
	{
		osdIconDisp(isClear);
		pbShowM(pbDispStart, fileIdx, fileCnt, 0);
	}


}

#endif

void	F_Mp4OpenOsd()
{
  	if (lcdOsdDisp != 1)
   	{
   		lcdOsdDisp = 1;
   		hwDispOsdEn(1);
   	}
}

enum{
	MP4_PROGRESS_VOL=0,
	MP4_PROGRESS,
	MP4_PLAY_STATUS,
	MP4_EQFlag
};

extern UINT32 g_ui32SaveKeyCode;
#define  MP4_NAME_LENG  16
void	F_PlayMp4()
{
	UINT32 err = SUCCESS;
	UINT32	dispW, dispH;
	asfFileInfo_t  asfFileInfo;
	movFileInfo_t movFileInfo;
	aviFileInfo_t   aviFileInfo;
	UINT32 samplerate = 0;
	UINT32	key,volum;
      UINT8   Namestr[MP4_NAME_LENG] ;
	
	err	= xdcfFileTypeGet(fileIdx, &fileType);
	if(pbDisp == PB_DISP_ONE)
	{
		if ((fileType == xDCF_FILETYPE_ASF)||(fileType == xDCF_FILETYPE_AVI)||(fileType == xDCF_FILETYPE_MOV))
		{
			while(1)
			{		
				osdClearScreen(0x00);
				
				if ((err = xdcfCurFileByPosSet(fileIdx)) != SUCCESS)
				{
					return;
				}
				err	= xdcfCurFileAttrGet(&fileattr);

				#if 1
				hwDispPreviewSizeGet(&dispW, &dispH);
				extern UINT32 hwDispImgSizeSetEnable(UINT32);
				hwDispImgSizeSet(dispW, dispH);    /*for zoomFactSet() use*/
				#endif
				
				UINT32 startTime=0, endTime=0;
				
				hwDispPvSetRead(imgBuf);
				ChgPvSetting = 1;
#if 1
				#if (CurrentIC8751==0)	
		        gpioConfig(GPIO_MUTE, 1);
		        gpioSet(GPIO_MUTE, 0);
				#else		
				if(SpeakFlag)
					wm8751SoundEnable(2,1);
				else
					wm8751SoundEnable(1,1);
				#endif	
#endif	
				
				UINT32  codecDev;
				sysgAudOutDevGet(&codecDev);
				if (fileType == xDCF_FILETYPE_ASF)
				{
					asfAppFileInfoGet(fileattr.name, &asfFileInfo);
					samplerate = asfFileInfo.audioSampleFreq;
				}
				else if (fileType == xDCF_FILETYPE_AVI)
				{
					aviAppFileInfoGet(fileattr.name, &aviFileInfo);
					samplerate = aviFileInfo.audioSampleFreq;
				}
				else
				{
					samplerate = 8000;
				}

				printf("samplerate:%d\n", samplerate);

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
							break ;							
						default:
							samplerate = 8000;
				}
				/*andrew_gu@2005/May/10, 19:14 )*/
				
#if (CurrentIC8751==0)
				hwAudAC97RegWrite(0x2c, samplerate);
				hwAudAC97RegWrite(0x2e, samplerate);
				//hwAudAC97RegWrite(0x32, samplerate);
#else
				wm8751SampleSet(samplerateIdx) ;
#endif
				
				if (fileType == xDCF_FILETYPE_ASF)
				{
					avPlayNow =	OBJ_ASF;
					asfAppPlay(fileattr.name);

					#if 1  // fqdao_modify for bug  restart play mp4 06.5.9
                                strncpy(Namestr, fileattr.name,MP4_NAME_LENG);					
					Namestr[strlen(Namestr)-4] = '\0' ;
					if(sPara.videoLastFlag ==1)
					{
				 		if( strcmp(sPara.ui8SaveMP4Name, Namestr )==0)
				 		{
							printf("\nSaveNamestr == Namestr\n") ;
							/* Paul@2006/04/06 add start */
							if (asfFileInfo.totalPktNr > sPara.videoLastFrame)
							{
								asfAppStartPktSet(sPara.videoLastFrame);
							}
							/* Paul@2006/04/06 add end */
							sPara.videoLastFlag = 0;
							nandWriteParameters();
					 	}
					}
					#endif 
				}
				else if (fileType == xDCF_FILETYPE_AVI)
				{
					avPlayNow =	OBJ_AVI;
					aviAppPlay(fileattr.name);
				}
				else if (fileType == xDCF_FILETYPE_MOV)
				{
					avPlayNow =	OBJ_MOV;
					movAppPlay(fileattr.name);
				}
				osTimeDly(20);//fqdao_add for bug play pause 06.6.4
				g_playrecStatus = STATUS_PLAYING;
				startTime = tmrMsTimeGet();
				tmrEnable(1, 8000, TMR_PERIODIC, osdAutoDisp);//wendy@050205
				SpeakFlagMode=1;//wendy for speak
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
//				volSet();
				g_playrecStatus = STATUS_PLAYING;	
				F_Mp4OpenOsd();
				
				//UINT8	str1[100];
				//UINT8	str2[100];
				do
				{
					F_UserKeyGet(&key, 30);
					
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
							F_Mp4IconRefresh(MP4_PROGRESS_VOL) ;
							F_Mp4IconRefresh(MP4_EQFlag) ;

							F_Mp4OpenOsd();							
							#if 0
							if(fileCnt&&(fileIdx==1))
							{
								fileIdx = fileCnt ;
							}
							else
							{
								fileIdx -- ;
							}		
							switch(avPlayNow)
							{
								case OBJ_ASF:
									if (g_playrecStatus ==STATUS_PAUSE)
									asfAppPause();
									asfAppPlayAbort();
									break;
								case OBJ_AVI:
									aviAppPlayAbort();
									break;
								case OBJ_MOV:
									movAppPlayAbort();
									break;								
							}							
							osTimeDly(20) ;
							g_playrecStatus = STATUS_STOP;
							pressVolume = 0;
							UI_OSQFlush(uiKeyMsgQ);								
							avPlayNow = OBJ_NONE ;
							F_Mp4OpenOsd();
							#endif
							break ;
							
						case	UI_KEY_DIR_DOWN:							
							if(mp3Vol)
							{
								mp3Vol--;
							}
							if (g_playrecStatus != STATUS_PAUSE)												
							{
								F_Mp3SetVolum(mp3Vol);
							}
							F_Mp4IconRefresh(MP4_PROGRESS_VOL) ;							
							F_Mp4IconRefresh(MP4_EQFlag) ;
							F_Mp4OpenOsd();							
							#if 0
							if(fileCnt && fileIdx==fileCnt)
							{
								fileIdx =1 ;
							}
							else
							{
								fileIdx++ ;
							}
							switch(avPlayNow)
							{
								case OBJ_ASF:
									if (g_playrecStatus ==STATUS_PAUSE)
									asfAppPause();
									asfAppPlayAbort();
									break;
								case OBJ_AVI:
									aviAppPlayAbort();
									break;
								case OBJ_MOV:
									movAppPlayAbort();
									break;								
							}							
							osTimeDly(20) ;
							g_playrecStatus = STATUS_STOP;
							pressVolume = 0;
							UI_OSQFlush(uiKeyMsgQ);								
							avPlayNow = OBJ_NONE ;
							F_Mp4OpenOsd();							
							#endif
							break ;
							
						case	UI_KEY_DIR_LEFT:
							if (g_playrecStatus != STATUS_REW)
							{
								osTimeDly(20);
								if (!(g_ui32SaveKeyCode & KEY_BIT_LEFT))
								{
									if(fileCnt&&(fileIdx==1))
									{
										fileIdx = fileCnt ;
									}
									else
									{
										fileIdx -- ;
									}		
									switch(avPlayNow)
									{
										case OBJ_ASF:
											if (g_playrecStatus ==STATUS_PAUSE)
											{
												asfAppPause();
											}
											asfAppPlayAbort();
											break;
										case OBJ_AVI:
											aviAppPlayAbort();
											break;
										case OBJ_MOV:
											movAppPlayAbort();
											break;								
									}							
									osTimeDly(20) ;
									g_playrecStatus = STATUS_STOP;
									pressVolume = 0;
									UI_OSQFlush(uiKeyMsgQ);								
									avPlayNow = OBJ_NONE ;
									F_Mp4OpenOsd();
								}
							}							
//							if(g_playrecStatus != STATUS_REW)
							{
								switch(avPlayNow)
								{
									case OBJ_ASF:
									asfAppSpeedSet(-4);
									break;

									case OBJ_AVI:
									aviAppSpeedSet(-4);
									break;

									case OBJ_MOV:
									movAppSpeedSet(-4);
									break;
								}
								g_playrecStatus = STATUS_REW ;
							}							
							F_Mp4OpenOsd();
							break ;
							
						case	UI_KEY_DIR_RIGHT:
							if (g_playrecStatus != STATUS_FF)
							{
								osTimeDly(20);
								if (!(g_ui32SaveKeyCode & KEY_BIT_RIGHT))
								{
									if(fileCnt && fileIdx==fileCnt)
									{
										fileIdx =1 ;
									}
									else
									{
										fileIdx++ ;
									}
									switch(avPlayNow)
									{
										case OBJ_ASF:
											if (g_playrecStatus ==STATUS_PAUSE)
											asfAppPause();
											asfAppPlayAbort();
											break;
										case OBJ_AVI:
											aviAppPlayAbort();
											break;
										case OBJ_MOV:
											movAppPlayAbort();
											break;								
									}							
									osTimeDly(20) ;
									g_playrecStatus = STATUS_STOP;
									pressVolume = 0;
									UI_OSQFlush(uiKeyMsgQ);								
									avPlayNow = OBJ_NONE ;
									F_Mp4OpenOsd();																
								}
							}
							
//							if(g_playrecStatus != STATUS_FF)
							{
								switch(avPlayNow)
								{
									case OBJ_ASF:
									asfAppSpeedSet(4);
									break;

									case OBJ_AVI:
									aviAppSpeedSet(4);
									break;

									case OBJ_MOV:
									movAppSpeedSet(4);
									break;
								}				
								g_playrecStatus = STATUS_FF ;
							}
							F_Mp4OpenOsd();
							break ;

						case	UI_KEY_FUNC_ZOOMIN:
							#if 0
							if(mp3Vol<9)
							{
								mp3Vol++;
							}
							if (g_playrecStatus != STATUS_PAUSE)												
							{
								F_Mp3SetVolum(mp3Vol);
							}
							F_Mp4IconRefresh(MP4_PROGRESS_VOL) ;
							F_Mp4OpenOsd();
							#endif
							break;
							
						case	UI_KEY_FUNC_ZOOMOUT:
							#if 0
							if(mp3Vol)
							{
								mp3Vol--;
							}
							if (g_playrecStatus != STATUS_PAUSE)												
							{
								F_Mp3SetVolum(mp3Vol);
							}
							F_Mp4IconRefresh(MP4_PROGRESS_VOL) ;							
							F_Mp4OpenOsd();
							#endif
							break;							
							
						case	UI_KEY_FUNC_A:
							/* Change play mode: random, single, all */
							if (mp3Repeat == MP3RepeatAll)
								mp3Repeat = MP3RepeatOne;
							else
								mp3Repeat ++;
							sPara.Repeat  = mp3Repeat;
							
							F_Mp4OpenOsd();
							break ;
							
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
							F_Mp4OpenOsd();
							printf("EQ = %d\n", ui32EQFlag);							
							break ;
							
						case	UI_KEY_FUNC_OK:
							if((g_playrecStatus==STATUS_FF)||(g_playrecStatus==STATUS_REW))
							{
								UI_OSQFlush(uiKeyMsgQ);
								break ;
								#if 0
								involuteplaymp4();
								#endif 					
							}
							else 
							{
								if(g_playrecStatus == STATUS_PAUSE)
								{
									for (volum = 0; volum <= mp3Vol; volum ++)
									{
										F_Mp3SetVolum (volum);
									}
									g_playrecStatus = STATUS_PLAYING;
								}
								else
								{
									
									g_playrecStatus = STATUS_PAUSE;
								}
								
								switch(avPlayNow)
								{
									case OBJ_ASF:
									asfAppPause();
									break;
									case OBJ_AVI:
									aviAppPause();
									break;
									case OBJ_MOV:
									movAppPause();
									break;
								}
							}										
							F_Mp4OpenOsd();
							break ;

						case	UI_KEY_ACCIDENT:
//							sPara.videoLastFlag = 1;
//							sPara.videoLastFrame =  asfAppKeyPktGet ();
//							nandWriteParameters();										
							UI_OSQFlush(uiKeyMsgQ);
							osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
								
						case	UI_KEY_FUNC_MENU:
							switch(avPlayNow)
							{
								case OBJ_ASF:
									memcpy(sPara.ui8SaveMP4Name,Namestr,MP4_NAME_LENG);// fqdao_modify for bug  restart play mp4 06.5.9
									sPara.videoLastFlag = 1;    
									sPara.videoLastFrame =  asfAppKeyPktGet ();
									nandWriteParameters();	
								
									if (g_playrecStatus ==STATUS_PAUSE)
									{
										asfAppPause();									
									}	
									asfAppPlayAbort();
									break;
								case OBJ_AVI:
									aviAppPlayAbort();
									break;
								case OBJ_MOV:
									movAppPlayAbort();
									break;								
							}							
							avPlayNow = OBJ_NONE ;	
							g_playrecStatus = STATUS_STOP;
							pressVolume = 0;
							pbDisp = PB_DISP_FOUR ;
							F_Mp4OpenOsd();
							break ;	
							
						default:
							#if 0
							if((g_playrecStatus==STATUS_FF)||(g_playrecStatus==STATUS_REW))
							{
								involuteplaymp4();
								printf("==============fefefefffff---1111111---");
							}
							#endif
							break ;
					}

					if(g_playrecStatus==STATUS_FF)
					{
						if(!(ui32SysKeyState & KEY_BIT_RIGHT))
						{
							involuteplaymp4();
						}
					}
					if(g_playrecStatus==STATUS_REW)
					{
						if(!(ui32SysKeyState & KEY_BIT_LEFT))
						{
							involuteplaymp4();
						}
						else
						{
							if(asfTime==0)
							{
							    osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);									
							}
						}
					}

					
					if (avPlayNow == OBJ_NONE)
					{
						break;
					}
					
					if (lcdOsdDisp == 1)
					{
						if (ui32NextState == uiState)
						{
							F_Mp4IconRefresh(MP4_PLAY_STATUS);
							F_Mp4IconRefresh(MP4_EQFlag) ;
							F_Mp4IconRefresh(MP4_PROGRESS) ;
							hisIconDisp();
						}
					}
					osSemPend(semApp, OS_NO_WAIT, &err);	/* -1 */
					if (err == OS_NO_ERR)
					{
						osSemPost(semApp);
					}
					else
					{
					  	osTimeDly(20);
					}

				}while(err);

				printf("==============fefefefffff---222222---");					
				if(!err)
				{
					if (mp3Repeat != MP3RepeatOne)
					if (fileCnt && (fileIdx==fileCnt))
					{
						fileIdx =1 ;
					}
					else
					{
						fileIdx++ ;
					}
				}
				else
				{
					#if (CurrentIC8751==0)	
				       	 gpioConfig(GPIO_MUTE, 1);
				     	  	 gpioSet(GPIO_MUTE, 1);
					#else		
						 wm8751SoundEnable(0,0);
					#endif						
				}

				osSemPend(semApp , 0, &err);
				osSemPost(semApp);		
				osTimeDly(20);
              #if  0  //fqdao 06.5.31
				#if (CurrentIC8751==0)	
			       	 gpioConfig(GPIO_MUTE, 1);
		     	  	 gpioSet(GPIO_MUTE, 1);
				#else		
					wm8751SoundEnable(0,0);
				#endif	
		#endif			
				if (ChgPvSetting)
				{
					hwDispPvSetWrite(imgBuf);
					ChgPvSetting = 0;
				}

				SpeakFlagMode=0;//wendy for speak
#if (CurrentIC8751==0)		
				ws9712LineOut(SpeakFlag);//wendy for speak
#else
				wm8751LineOut(SpeakFlag);
#endif			

				if (ui32NextState != uiState)
				{
					break;
				}
				
				uiDispBuffFill(0x00);
				if(pbDisp == PB_DISP_FOUR)
				{
					F_Mp4FileListRefresh(fileIdx,1);
					break ;
				}
			}
		}
	}
}

/**/
void	 involuteplaymp4()
{
	switch(avPlayNow)
	{
		case OBJ_ASF:
			asfAppSpeedSet(1);
			break;

		case OBJ_AVI:
			aviAppSpeedSet(1);
			break;

		case OBJ_MOV:
			movAppSpeedSet(1);
			break;
	}
	g_playrecStatus = STATUS_PLAYING;

}
/**/

// UINT8	ui8Mp4StatusSave ;




void	F_Mp4IconRefresh(UINT8 ui8Mp4IconFlag)
{
	UINT32	osdW, osdH;	
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	UINT32	i ;

	
	if(mp3Repeat == MP3RepeatOne)
		osdStrDisp(156, 2, UserIcon10x20, 0x0, "\x5e\x5f");
	else if(mp3Repeat == MP3RepeatAll)
		osdStrDisp(156, 2, UserIcon10x20, 0x0, "\x3e\x3f");
	else /*random*/
		osdStrDisp(156, 2, UserIcon10x20, 0x0, "\x1e\x1f");


	if(ui8Mp4IconFlag==MP4_PLAY_STATUS)	
	{
//		if(g_playrecStatus!=ui8Mp4StatusSave)
//		{
//			ui8Mp4StatusSave = g_playrecStatus ;
			 switch (g_playrecStatus)
			{

				case	STATUS_STOP:
					osdStrDisp(osdW/2+20 , 0, UserIcon10x20, 0x30, "\x57\x58");
					break;
				case	STATUS_PLAYING:
					osdStrDisp(osdW/2+20 , 0, UserIcon10x20, 0x40, "\x8e\x8f");
					break;
				case	STATUS_PAUSE:
					osdStrDisp(osdW/2+20, 0, UserIcon10x20, 0x30, "\x90\x91");
					break;
				case	STATUS_FF:
					osdStrDisp(osdW/2+20, 0, UserIcon10x20, 0x40, "\x8a\x8b");
					break;
				case	STATUS_REW:
					osdStrDisp(osdW/2+20, 0, UserIcon10x20, 0x40, "\x8c\x8d");
					break;
				default:
					osdStrDisp(osdW/2+20, 0, UserFont10x20, 0x0, "  ");
					break;
			}
//		}
	}
	else 	if(ui8Mp4IconFlag==MP4_PROGRESS)
	{
		F_Mp4ProgressDraw();
	}
	else  if(ui8Mp4IconFlag == MP4_PROGRESS_VOL)
	{
		osdBarDraw((osdW/2-40), osdH-70, 100, 30, 0x00);
		for( i = 1; i <=mp3Vol; i++)
		{
	        if(!mp3Vol)  //fqdao_add for bug 06.5.22 
          	{
            	osdBarDraw((osdW/2-40), osdH-70, 100, 30, 0x00);
          	}
			else
			{
				osdBarDraw((osdW/2-40+ i*9), osdH-48-i*2 , 4, 3+i*2, (i > 7) ? 0x01 : 0x03);  //wendy update 050125
			}
		}		
		
	}
	else if(ui8Mp4IconFlag == MP4_EQFlag )
	{	
		osdStrDisp(osdW/2+20, 24, UserFont10x20, 0x0, "           ");
		osdStrDisp(osdW/2+20, 24, UserFont10x20, 0xd0, F_Mp3GetString(ui32EQFlag+EQFlat_s));

	}
}


/**************************************************************************
 *                                                                        *
 *  Function Name: osdHProgressDraw                                       *
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
typedef struct {
	UINT16 sec;
	UINT16 min;
	UINT16 hour;
}AvPlayedTime_T;

AvPlayedTime_T avPlayedTime;

void F_Mp4ProgressDraw()
{
	SINT32 x, y, w, l;

	UINT32 osdW, osdH;
	UINT32 stepLgh;
	UINT32 tmp;
	UINT8 str[25] ;
	
	asfFileInfo_t  asfFileInfo;
	movFileInfo_t movFileInfo;
	aviFileInfo_t   aviFileInfo;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

				UINT32 tempProgress;
				hisTimeDisp();
				if(avPlayNow == OBJ_ASF)
				{
					if (asfAppStatusGet() == ASF_STATUS_PLAY_TO_EOF)
					{
						tempProgress = 100;
						printf("\ntempProgress100\n");
					}
					else
					{
						tempProgress = asfPlayProgressGet();
						printf("\ntempProgress=%d\n",tempProgress);
					}

					asfAppFileInfoGet(NULL,&asfFileInfo);
					//  caoyong   05-08-23  begin
					if(asfFileInfo.playDuration >= asfFileInfo.residualTime )
	 				{
						asfTime = asfFileInfo.playDuration - asfFileInfo.residualTime ; 
						//asfTime=(asfFileInfo.playDuration*tempProgress)/100 ;	
					}
					else
					{
						asfTime = 0;
					}
					//caoyong     05-08-23   end	
				}
				 
				else if(avPlayNow == OBJ_AVI)
				{
					tempProgress = aviPlayProgressGet();
       				aviAppFileInfoGet(NULL, &aviFileInfo);
       				asfTime =aviFileInfo.playDuration-aviFileInfo.residualTime;

       			}
				else
				{
					tempProgress = movPlayProgressGet();
					movAppFileInfoGet(NULL,&movFileInfo);
  		                     asfTime =movFileInfo.playDuration-movFileInfo.residualTime;
				}


//				osdHProgressDraw(0, 100, tempProgress, 0x7c);

				avPlayedTime.sec = asfTime/1000%60;
				avPlayedTime.min = (asfTime/1000/60)%60;
				avPlayedTime.hour = asfTime/1000/60/60;

				sio_psprintf(str, "%01d:%02d:%02d ", avPlayedTime.hour, avPlayedTime.min, avPlayedTime.sec);
				osdStrDisp(32, osdH-60, UserFont10x20, 0x00, str);

UINT32 start=0;
UINT32 end=100;
UINT32 pos=tempProgress;
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
#if 1

	w = 16;
	x = 64;
	y = osdH - w - 3 - 20; //tzw modify
	l = osdW - x - 64;


#if 0
	osdStrDisp(x-w, y, UserIcon16x16, 0x7c&0xf0, "\x03");
	osdStrDisp(x+l, y, UserIcon16x16, 0x7c&0xf0, "\x04");
#endif

	osdFrameDrawX(x, y, l, w, 0x7c&0x0f, 3);
	x += 3;
	y += 3;
	w -= 6;
	l -= 6;
	osdBarDraw(x, y, l, w, 0x00);

	stepLgh = l / (end-start+1);

	if (stepLgh < 16)
	{
//		osdBarDraw(x+(l-(16-stepLgh))*(pos-start)/(end-start+1), y, 16, w, 0x7c>>4);
		osdBarDraw(x+(l-(16-stepLgh))*(pos-start)/(end-start+1), y, 16, w, 0x05);
		
	}
	else
	{
//		osdBarDraw(x+l*(pos-start)/(end-start+1), y, stepLgh, w, 0x7c>>4);
		osdBarDraw(x+l*(pos-start)/(end-start+1), y, stepLgh, w, 0x05);
	}
#endif	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: pbShowS                                                *
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
#if  0
extern  UINT8 TVFlag;
 extern void pipCopyPaste(UINT32 srcX, UINT32 srcY, UINT32 srcW, UINT32 srcH, UINT32 dstAddr, UINT32 dstW, UINT32 dstH, UINT32 dstX, UINT32 dstY);
void pbShowS(UINT32 fileIdx)
{
	UINT8  	str[25];

	UINT32 err = SUCCESS;
	UINT32	dispW, dispH;
	asfFileInfo_t  asfFileInfo;
	movFileInfo_t movFileInfo;
	aviFileInfo_t   aviFileInfo;
	UINT32 samplerate = 0;


	#ifdef	OSD_DEBUG
	printf("\n PB/1	-> fileIdx=%d	file type=",	fileIdx);
	#endif
	if ((err = xdcfCurFileByPosSet(fileIdx)) != SUCCESS)
	{
		return;
	}
	err	= xdcfCurFileAttrGet(&fileattr);
	err	= xdcfFileTypeGet(fileIdx, &fileType);
	
	hwDispPreviewSizeGet(&dispW, &dispH);

	extern UINT32 hwDispImgSizeSetEnable(UINT32);
	hwDispImgSizeSet(dispW, dispH);    /*for zoomFactSet() use*/

	if (fileType ==	xDCF_FILETYPE_JPG)
	{
		#ifdef	OSD_DEBUG
		printf("JPG avPlayNow = %d\n", avPlayNow);
		#endif

		hwPlayBufSizeSet(dispW,	dispH);
		hwPlayBufAddrSet(gPB);
		hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
		hwCammodeSet(5);	  /* play back mode	*/
		hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/
		//wendy@050126 for fit image begin
		pbImageGet(0, fileIdx, gPB, dispW, dispH, 0, 0x800000); /* 0x800000 is bg color */
		//wendy@050126 for fit image end
		#if 1
		//=====================Danny --05/06/08
		if((TVFlag==1) && (PANEL_TYPE_1 != getCurPanel()))
		{
			//asfAppPlayAbort();
		    gpioSet(GPIO_LCD_POWER, 1);
			hwWait(0,300);
			//Buzzer(5);
			panelSelect(PANEL_TYPE_1);
			printf("tv out off");
			sPara.TvoutFlag=TVOutOff;
			TVFlag=0;
			hwDispPvSetRead(imgBuf);
			ChgPvSetting = 1;		
		}
		//=====================Danny --05/06/08
        #endif


	}
	else if ((fileType == xDCF_FILETYPE_ASF)||(fileType == xDCF_FILETYPE_AVI)||(fileType == xDCF_FILETYPE_MOV))
	{
		UINT32 startTime=0, endTime=0;
		#ifdef	OSD_DEBUG
		printf("ASF");
		#endif
		/*PNL_PreviewSet();
		hwDispOsdEn(1);
		hwDispRegUpd(0x01);*/
		hwDispPvSetRead(imgBuf);
		ChgPvSetting = 1;
		#ifdef	OSD_DEBUG
		UINT8 i;
		printf("\nPLAY ASF... ");
		printf("\nBEFORE:\n");
		for(i=0; i<20; i+=2)
			printf("%4d ", imgBuf[i]+imgBuf[i+1]*256);
		#endif
              gpioSet(GPIO_MUTE, 0);

		#if 1
		UINT32  codecDev;
		sysgAudOutDevGet(&codecDev);
		if (fileType == xDCF_FILETYPE_ASF)
		{
			asfAppFileInfoGet(fileattr.name, &asfFileInfo);
			samplerate = asfFileInfo.audioSampleFreq;
		}
		else if (fileType == xDCF_FILETYPE_AVI)
		{
			aviAppFileInfoGet(fileattr.name, &aviFileInfo);
			samplerate = aviFileInfo.audioSampleFreq;
		}
		else
		{
			samplerate = 8000;
		}

		printf("samplerate:%d\n", samplerate);

		/*andrew_gu@2005/May/10, 19:14 (*/
		switch(samplerate)
		{
			case 48000:
			case 16000:
			case 8000:
				break;
			default:
				samplerate = 8000;
		}
		/*andrew_gu@2005/May/10, 19:14 )*/
		
			hwAudAC97RegWrite(0x2c, samplerate);
			hwAudAC97RegWrite(0x2e, samplerate);
			//hwAudAC97RegWrite(0x32, samplerate);
		
		#endif

		if (fileType == xDCF_FILETYPE_ASF)
		{
			avPlayNow =	OBJ_ASF;
			/*asfAppFileInfoGet(fileattr.name,&asfFileInfo);
		        printf(" \n ly_debug asf total time=%d\n",asfFileInfo.playDuration);*/
			asfAppPlay(fileattr.name);
		}
		else if (fileType == xDCF_FILETYPE_AVI)
		{
			avPlayNow =	OBJ_AVI;
			/*aviAppFileInfoGet(fileattr.name, &aviFileInfo);*/
			aviAppPlay(fileattr.name);
		}
		else if (fileType == xDCF_FILETYPE_MOV)
		{
			avPlayNow =	OBJ_MOV;
			/*movAppFileInfoGet(fileattr.name, &movFileInfo);*/
			movAppPlay(fileattr.name);
		}
		/*avPlayNow =	OBJ_ASF;
		asfAppPlay(fileattr.name);*/
		g_playrecStatus = STATUS_PLAYING;
		startTime = tmrMsTimeGet();
		asfProgress = 0;
		tmrEnable(1, 8000, TMR_PERIODIC, osdAutoDisp);//wendy@050205
		SpeakFlagMode=1;//wendy for speak
		ws9712LineOut(SpeakFlag);//wendy for speak
		volSet();
		do
		{
			osSemPend(semApp , 20, &err);
			osSemPost(semApp);
			 if (avPlayNow == OBJ_NONE)
			   break;
			
			if (lcdOsdDisp == 1)
				osdIconDisp(0);
			#if 1
			//=====================Danny --05/06/08
			if((TVFlag==1) && (PANEL_TYPE_1 != getCurPanel()))
			{
		         gpioSet(GPIO_LCD_POWER, 1);
				hwWait(0,300);
				panelSelect(PANEL_TYPE_1);
				printf("tv out off");
				sPara.TvoutFlag=TVOutOff;
				TVFlag=0;
				hwDispPvSetRead(imgBuf);
				ChgPvSetting = 1;		
			}
			//=====================Danny --05/06/08
        	   #endif
		   osTimeDly(1);
		}while(err);
		printf("  break ");
		osTimeDly(20);

		gpioSet(GPIO_MUTE, 1);		//Paul@2005/09/05

		// g_playrecStatus = STATUS_STOP;
		/*
		printf("tmeFunc=%d\n",TmrFunc);*/
		SpeakFlagMode=0;//wendy for speak
		ws9712LineOut(SpeakFlag);//wendy for speak
		tmrDisable(1);
		osdLcdDisp(1);
		osdIconDisp(0);
// 		PayNow =	OBJ_NONE;       //05-05-24--Danny

		#ifdef	OSD_DEBUG
		printf("END	");
		#endif
	}
	#if 0
	else if (fileType == xDCF_FILETYPE_WAV)
	{
		UINT32 startTime=0, endTime=0;

		#ifdef	OSD_DEBUG
		printf(" *WAV* ");
		#endif

		#ifdef	OSD_DEBUG
		printf("\nPLAY WAV... ");
		#endif

		#if 1
		UINT32  codecDev;
		audFileInfo_t audioProfile;
		sysgAudOutDevGet(&codecDev);
		audAppFileInfoGet(fileattr.name,&audioProfile);
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

		avPlayNow =	OBJ_WAV;
		audPlayFileNameSet(fileattr.name);
		audRiffCtrl(RIFF_PLAY);

		/* when using 1.7.7 code, you could use semApp for audio playing */
		startTime = tmrMsTimeGet();
		wavProgress = 0;
		do
		{
			osSemPend(semApp , 2, &err);
			osSemPost(semApp);
			endTime = tmrMsTimeGet();
			wavTime = endTime-startTime;
			osdIconDisp(0);
		}while(err);
		osTimeDly(20);
		avPlayNow =	OBJ_NONE;

		#ifdef	OSD_DEBUG
		printf("END	");
		#endif
	}
	#endif
	else
	{
		#ifdef	OSD_DEBUG
		printf("#??TYPE=%d??#",	fileType);
		#endif
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: pbShowM                                                *
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
void pbShowM(UINT32 start, UINT32 curr, UINT32 total, UINT32 key)
{
	UINT8	i, j;
	UINT32	err	= SUCCESS;
	UINT32	pPBAddr, phyOsdDBAddr;
	UINT8	*pImgBuf, osdStr[6]	= {	"*WAV*"	};;
	UINT32	w, osdFontWidth, osdFontHeight;
	UINT32	tempIdx = 1;
	UINT32	dispW, dispH;
	UINT32	pbThumbW, pbThumbH, pbThumbOffX, pbThumbOffY;


	/*osTaskResume(osTaskFindByName("HISTOGRAM"));*/

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

	hwDispPreviewSizeGet(&dispW, &dispH);

	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/

	pubFill((UINT8 *)(0xAC000000+gPB*2), dispW*2, dispH,
		0, 27, dispW*2, dispH-46, 0x000000);/*eric@5/9/2003*/
       dispH = dispH- 40; /*Julie add 20040901*/
	pbThumbW = ((dispW-10-(pbDispCol+1)*8)/pbDispCol) & 0xFF0;
	pbThumbH = (dispH-(pbDispRow+1)*8)/pbDispRow;
	pbThumbOffX = (((dispW -10- pbDispCol*pbThumbW) /(pbDispCol+1)) & 0xFFF8);
	pbThumbOffY = ((dispH - pbDispRow*pbThumbH) /(pbDispRow+1));

	for(i=0; i<pbDispCol; i++)
	{
		for (j=0; j<pbDispRow; j++)
		{
			tempIdx = start + i * pbDispCol + j;
			pbFileType[tempIdx-start] =	OBJ_NONE;
			if(tempIdx <= total)
			{
				err	= xdcfFileTypeGet(tempIdx, &fileType);
				if (fileType ==	xDCF_FILETYPE_JPG) {

					//wendy@050126 for fit image begin

					//pbFileFit(1, tempIdx, pPBAddr, pbThumbW,pbThumbH, 1, 0, 0); /* 0x800000 is bg color */
				
					#if 1
					pubFill((UINT8 *)(0xAC000000+pPBAddr*2), pbThumbW*2, pbThumbH,
						0, 0, pbThumbW*2, pbThumbH, 0x000000);
					
					/*andrew_gu@2005/May/16, 17:51 ( the API can't display thumbnail sometimes*/
					
					//err	= pbImageGet(1,	tempIdx, pPBAddr,	pbThumbW, pbThumbH, 0, 0);	//wendy@050202 for fit image	/* scale from thumbnail	image */
					//if(err)
					/*andrew_gu@2005/May/16, 17:51 )*/
					
					err	= pbImageGet(0,	tempIdx, pPBAddr,	pbThumbW, pbThumbH, 0, 0); //wendy@050202 for fit image/* scale from primary image	*/
					#if 1
					hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
							gPB, dispW,	dispH,
							pbThumbOffX+j*(pbThumbOffX+pbThumbW+10),
							pbThumbOffY+20+i*(pbThumbOffY+pbThumbH), 0);
					#endif
					#endif
					//wendy@050126 for fit image end

					pbFileType[tempIdx-start] =	OBJ_S_JPG;

				}
				else if (fileType == xDCF_FILETYPE_ASF)
				{
					if ((err = xdcfCurFileByPosSet(tempIdx)) != SUCCESS)
					{
						#ifdef	OSD_DEBUG
						printf("#ASF_ERR %d#", fileType);
						#endif
						return;
					}
					err	= xdcfCurFileAttrGet(&fileattr);
					#ifdef	OSD_DEBUG
					printf("\nPLAY ASF... ");
					#endif
					/* sysgMemPBDecompBufferGet(&pPBAddr); */
					asfThumbGet(fileattr.name, pPBAddr,	pbThumbW, pbThumbH);

					/*osdStr[1] =	'A';
					osdStr[2] =	'S';
					osdStr[3] =	'F';
					pbFileType[tempIdx-start] =	OBJ_ASF;
					hwDateFontColorSet(COLOR_ASF, 0xFFFE); Color	other than 0 are treated as	font color */
				}

				else if (fileType == xDCF_FILETYPE_MP3)
				{
					pbFileType[tempIdx-start] =	OBJ_MP3;
					/* osdStr[1] =	'M';
					osdStr[2] =	'P';
					osdStr[3] =	'3';
					hwDateFontColorSet(COLOR_MP3, 0xFFFE); Color	other than 0 are treated as	font color */
				}
				else if (fileType == xDCF_FILETYPE_WMA)
				{
					pbFileType[i] =	OBJ_WMA;
					/* osdStr[1] =	'W';
					osdStr[2] =	'M';
					osdStr[3] =	'A';
					hwDateFontColorSet(COLOR_WMA, 0xFFFE); Color	other than 0 are treated as	font color */
				}
				else if(fileType == xDCF_FILETYPE_WAV)
				{
					pbFileType[tempIdx-start] =	OBJ_WAV;
					/*osdStr[1] =	'W';
					osdStr[2] =	'A';
					osdStr[3] =	'V';
					hwDateFontColorSet(COLOR_WAV, 0xFFFE);  Color	other than 0 are treated as	font color */
				}
				if(	(fileType == xDCF_FILETYPE_ASF)	||
					(fileType == xDCF_FILETYPE_WAV)	||
					(fileType == xDCF_FILETYPE_MP3) ||
					(fileType == xDCF_FILETYPE_WMA))
				{
					/*osdStr2Img(osdStr, UserFont, pImgBuf);*/
					if(fileType	== xDCF_FILETYPE_ASF)
					{
					   #if 0
						hwDateFontStampDo( /* stamp	character on by	one	*/
							phyOsdDBAddr, w, osdFontHeight,	0, 0,
							w, osdFontHeight,
							pPBAddr, pbThumbW, pbThumbH, 0, 0, pPBAddr);/*eric@5/9/2003*/
					 #endif
					 	hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
							gPB, dispW,	dispH,
							pbThumbOffX+j*(pbThumbOffX+pbThumbW+10),
							pbThumbOffY+20+i*(pbThumbOffY+pbThumbH), 0);
					}
					else
					{
					#if 0
						hwDateFontStampDo( /* stamp	character on by	one	*/
							phyOsdDBAddr, w, osdFontHeight,	0, 0,
							w, osdFontHeight,
							gPB, dispW,	dispH,
							pbThumbOffX+j*(pbThumbOffX+pbThumbW),
							pbThumbOffY+i*(pbThumbOffY+pbThumbH),
							pPBAddr);
					#endif
					}
				}
			}
		}
	}
	#ifdef	OSD_DEBUG
	printf(" P/B 9 OK ...");
	#endif
}
void pbThumb(UINT32 offsetX, UINT32 offsetY, UINT32 w, UINT32 h)
/**************************************************************************
 *                                                                        *
 *  Function : playback thumbnail on specified position, and scale size to fit                       *
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
{
	UINT32	err	= SUCCESS;
	UINT32	pPBAddr, phyOsdDBAddr;
	UINT8	*pImgBuf;

	UINT32	tempIdx = 1;
	UINT32	dispW, dispH;
	UINT32	pbThumbW, pbThumbH, pbThumbOffX, pbThumbOffY;

	pImgBuf		= (UINT8*) (((UINT32 )imgBuf) |	0xac000000);
	phyOsdDBAddr= (((UINT32)pImgBuf) & 0x00FFFFFF) >> 1;
	hwDateFontScalerSet(1);
	printf("pbThumb in\n");

	pbThumbBufferGet(&pPBAddr);

	#if	0
	printf("\n buffaddr=%x ", pPBAddr);
	printf("\n imgBuf=%x pImgBuf=%0x ",	imgBuf,	pImgBuf);
	#endif

	hwDispPreviewSizeGet(&dispW, &dispH);
	/*hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);*/
	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/

	/*pubFill((UINT8 *)(0xAC000000+gPB*2), dispW*2, dispH,
		0, 0, dispW*2, dispH, 0x000000);eric@5/9/2003*/

	pbThumbW = w & 0xFF0;
	pbThumbH = h;
	pbThumbOffX = (offsetX);
	pbThumbOffY = (offsetY);

	{
	tempIdx = fileIdx;
	err	= xdcfFileTypeGet(tempIdx, &fileType);
	if (fileType ==	xDCF_FILETYPE_JPG) {

		pubFill((UINT8 *)(0xAC000000+pPBAddr*2), pbThumbW*2, pbThumbH,
			0, 0, pbThumbW*2, pbThumbH, 0x800000);
		err	= pbImageGet(1,	tempIdx, pPBAddr,	pbThumbW, pbThumbH, 0, 0);	//wendy@050202 for fit image	/* scale from thumbnail	image */
		if(err)
			err	= pbImageGet(0,	tempIdx, pPBAddr,	pbThumbW, pbThumbH, 0, 0);//wendy@050202 for fit image /* scale from primary image	*/
		#if 1
		hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
				gPB, dispW,	dispH,
				pbThumbOffX,
				pbThumbOffY, 0);
		#endif

		/*pbFileType[tempIdx-start] =	OBJ_S_JPG;*/
	}

	else if((fileType == xDCF_FILETYPE_ASF)||(fileType == xDCF_FILETYPE_AVI)||(fileType == xDCF_FILETYPE_MOV))
	{
		if ((err = xdcfCurFileByPosSet(tempIdx)) != SUCCESS)
		{
			#ifdef	OSD_DEBUG
			printf("#ASF_ERR %d#", fileType);
			#endif
			return;
		}
		err	= xdcfCurFileAttrGet(&fileattr);
		#ifdef	OSD_DEBUG
		printf("\nPLAY ASF... ");
		printf("%s\n", fileattr.name);
		#endif
		/* sysgMemPBDecompBufferGet(&pPBAddr); */

		if(fileType == xDCF_FILETYPE_AVI)
			err = aviThumbGet(fileattr.name, pPBAddr,	pbThumbW, pbThumbH);
		else if(fileType == xDCF_FILETYPE_ASF)
			err = asfThumbGet(fileattr.name, pPBAddr,	pbThumbW, pbThumbH);
		else
			err = movThumbGet(fileattr.name, pPBAddr,	pbThumbW, pbThumbH);

		printf("err = %d\n", err);
		/*pbFileType[tempIdx-start] =	OBJ_ASF;*/
		hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
				gPB, dispW,	dispH,
				pbThumbOffX,
				pbThumbOffY, 0);
		hwDateFontColorSet(COLOR_ASF, 0xFFFE); /* Color	other than 0 are treated as	font color */
	}


	}
	#ifdef	OSD_DEBUG
	printf(" P/B 9 OK ...");
	#endif
}


#endif


