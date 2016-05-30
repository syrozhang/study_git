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
//#define RS_232_DBG

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "general.h"
#include "stdio.h"
#include "os_api.h"
#include "sysg_api.h"
#include "ui_main.h"
#include "hw_disp.h"
#include "hw_disp_ext.h"
#include "osd_common.h"
#include "osd_api.h"
#include "panel.h"
#include "dcf_api.h"
#include "xiaoao.h"
#include "mp3en_api.h"
#include "audio_api.h"
#include "hw_sdram.h"
#include "hw_aud.h"
#include "hw_cammode.h"
#include "hw_cdsp.h"
#include "timer.h"
#include "exif_custom.h"
#include "exif_api.h"
#include "asf_api.h"
#include "dsc_init.h"
#include "zoom.h"
#include "gpio.h"
#include "card.h"
#include "hw_front.h"
#include "Tv656.h"
#include "uistrings.h"
#include "ws9712.h"
//#include "mp3_scr.h"
#include "rec_ddx.h"
#include "extern.h"
#include "menu_capture.h"
//**************************************************************************

//**************************************************************************

#define   SelectUp  		0 
#define   SelectDown  		1
#define   RECORD_MODE 		0
#define   Play_mode 		1 
#define   FileInfo_mode 	2 

#define   FILELIST_MAXLINE  3 


//**************************************************************************
static 	UINT8 	strRec[FILELIST_MAXLINE][13];
static 	UINT32 	totalTime[FILELIST_MAXLINE] ;
static	void 	*semApp;

extern void pbShowFileNum(UINT32 filePos1,UINT32 filecnt1 );
extern void	F_Mp3IconRefresh(UINT8 ui8Mp3IconFlag);
extern void F_Mp3SetVolum(SINT32 value);

UINT8 	ui8SaveVolFF= 0;
UINT8 	RecWavPlay;
UINT8 	ui8Record_flag = RECORD_MODE ;   // 1 is playback  0 record 
UINT8 	ui8SelectUpDown = 1;
#define   DisplayVol_N 0 
#define   DisplayVol_T	 1
#define   DisplayVol_NM 2
UINT8     ui8DisplayVol_flag = DisplayVol_NM ;

UINT32 	MenuR[][4]=
{
	{recordp,0,0,sizeof(recordp)},
	{Rec_ico1,32,60,sizeof(Rec_ico1)},
	{Pla_ico,32,60,sizeof(Pla_ico)},	
	{Rec_ico2,32,60,sizeof(Rec_ico2)},
};
enum
{
	   Rec_Title_str= 0, 
	   Rec_Start_str	, 
       Rec_End_str	,
	   Rec_Playback 	,
	   Rec_seltv,
	   Rec_selwav,
	   Rec_selmp3,
	   Rec_NoFile,
	   MAX_REC_ID 	
};

//#define LANGUAGES 3 268 269 录制不一样
UINT8 *Ui_RecordStrings[MAX_REC_ID][LANGUAGES] = {
	{
		"Record",
		"录音",//"录制",
		"魁",//"魁",
		"Grabar",
		"Запись",
		"Aufnahme",
		"Enregister",//"Enregistrement",
		"Reg",
		"Gravar",//"Recorde",
		"踌澜",
	},
	{	"Start ",
		"开始",
		"秨﹍",
		"Iniciar",
		"Старт",//092
		//"Начать запись",
		"Start ",
		"commencer",
		"Inizia ",
		"Inicio",//"Come\xe7a",
		"矫累",
	},
	{	"End   ",
		"结束",
		"挡",
		"Terminar",
		"Стоп",//092
//		"Закончить запись",
		"End   ",
		"fin      ",
		"Finisce",
		"Fim   ",//"Termina",
		"辆丰",
	},	
	{
		"PlayBack",
		"回放"	,
		"",
		"Reproducir",
		"Воспроизведение",
		"Wiedergabe",
		"répéter ",
		"Riprodurre",
		"Tocar",//"Vista atrás",
		"饭内靛 犁积",
	 },
	{	"Av In",
		"视频",
		"跌繵",
		"Vídeo",
		"Видео-частота",
		"Video",
		"vidéo",//"entrée vidéo",
		"Video",
		"Av Entrada",//"Av", 
		"厚叼坷",
	},	
	{	"MIC ",
		"话筒",
		"杠旦",
		"Micrófono",
		"Микрофон",
		"Mikrophone",
		"micro",//"mic",
		"Microfono",
		"Microfone",
		"付捞农",
	},	
	{	"Audio ",
		"音频",
		"繵",
		"Audio",
		"Аудио-частота",
		"Audio",
		"audio",
		"Suono",
		"Audio ",//"Audiofrequência",
		"坷叼坷",
	},	
	{
		"No Video",
		"没有视频",
		"⊿Τ跌繵",
		"Sin vídeo",//spanish
		"Нет Видео",//"Отсутствует видео-частота",	
		"ohne Video",
		"pas de vidéo",
		"No video",
		"N\xe3o video",
		"厚叼坷 绝澜",
	},
};
//**************************************************************************
void MsgShow(UINT16 msgID);


void Pb_RecordPlay(UINT8 *strplay)
{
	
	
	if(sPara.language==LanguageRussian)
	{
		osdStrDisp(116, 61, UserFont10x20, 0xd0, strplay);
	}
	else
	{
		osdStrDisp(156, 61, UserFont10x20, 0xd0, strplay);
	}
	
}
//**************************************************************************
//**************************************************************************
//**************************************************************************
extern  UINT8  flag ;
extern  UINT8 ui8MenuIndex  ;//fqdao_add for bug  record replay 06.6.2
#define      TEXT_X        	68-2
#define      TEXT_H       	22
#define      TEXT_Y       	140	
#define      TEXT_Y1        TEXT_Y+TEXT_H
#define      TEXT_Y2       	TEXT_Y+TEXT_H+TEXT_H

void uiRecord(void)
{
	UINT32 key = UI_KEY_MODE_RECORD;
	UINT32 dispW, dispH;
	ui8Record_flag = RECORD_MODE ;
	isMenuOn = 0;

	semApp = osEventFindByName("APP_SEM");
	
	while ( ( uiState & UI_MODE_MASK ) ==UI_MODE_RECORD )
	{
		if (isMenuOn)
		{
			osdClearScreen(0x00); //tzw add
			menuPBMovieProc(key);
			if (isMenuOn == 0)
			{
				xdcfFileCountGet(&fileCnt);
				if(ui8MenuIndex==0&&fileCnt==0)//fqdao_add for bug  record replay 06.6.2
				{
					MessageShow (ErrNoFile);
					ui8Record_flag = RECORD_MODE ;
				}
				key = UI_KEY_MODE_RECORD;
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
				case UI_KEY_FUNC_MENU:
					menuReturn(UI_MAINMENU, 0);
					break;
				case 	UI_KEY_FUNC_B:
					if((RecWavPlay!=PB_DISP_ONE)&&(ui8Record_flag==Play_mode))
					{
						if (fileCnt == 0)
						{
							break;
						}										
						menuPBMovieInit(key);
					}
					break; 
				case	UI_KEY_FUNC_OK:
					if(ui8Record_flag)
					{
						RecWavPlay = PB_DISP_ONE;
						pbRecordModeSelect( ui8Record_flag);
					}
					break;
					
				case UI_KEY_DIR_UP:
					if (ui8Record_flag)
					{
						if(fileIdx > 1 )
							fileIdx --;
						else
						 	fileIdx = fileCnt;
						pbRecordModeSelect(ui8Record_flag);
					}
					else
					{
					
						xdcfInit(imageDirNameStr, mp3SubDirRecfileName, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
						xdcfCurRootDirSet(otherRootNameStr);
						xdcfFilterSet(FILTER_AUDIO);
						
						xdcfDirCountGet(&dirCnt);
						xdcfCurDirPosGet(&dirIdx);
						xdcfCurDirAttrGet(&dirattr);
						xdcfFileCountGet(&fileCnt);
						if(!fileCnt)
						{
							MessageShow (ErrNoFile);
//								osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);													
//								break;
							key = UI_KEY_MODE_RECORD;
							continue;
						}
						
						if(RecWavPlay == PB_DISP_NINE)
						{
							if(ui8Record_flag == 0 )
							{
								ui8Record_flag =1;
							}
							else
							{
								ui8Record_flag = 0 ;
							}	
							if(fileCnt != 0)
							{
								fileIdx = fileCnt;		// playback the last one john@01/22/2006
								pbRecordShow(ui8Record_flag,0);
							}
						}
					}
					break;
					
				case UI_KEY_DIR_DOWN:
					if (ui8Record_flag)
					{
						if(fileIdx < fileCnt)
							fileIdx ++;
						else
						 	fileIdx = 1;
						pbRecordModeSelect( ui8Record_flag);
					}
#if 0					
					else	
					{
						if(RecWavPlay == PB_DISP_NINE)
						{
							if(ui8Record_flag == 0 )
							{
								ui8Record_flag =1;
							}
							else
							{
								ui8Record_flag = 0 ;
							}	
							xdcfInit(imageDirNameStr, mp3SubDirRecfileName, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
							xdcfCurRootDirSet(otherRootNameStr);

							xdcfDirCountGet(&dirCnt);
							xdcfCurDirPosGet(&dirIdx);
							xdcfCurDirAttrGet(&dirattr);
							xdcfFileCountGet(&fileCnt);
							if(fileCnt != 0)
								fileIdx = 1;
							pbRecordShow(ui8Record_flag,0);
						}
					}
#endif					
					break;


	//			case	UI_KEY_FUNC_SHUTTER:
				case UI_KEY_FUNC_A :
//				case UI_KEY_DIR_LEFT:
//				case UI_KEY_DIR_RIGHT:
					if(ui8Record_flag)
					{
						if(RecWavPlay == PB_DISP_NINE)
						{
							if(ui8Record_flag == 0 )
							{
								ui8Record_flag =1;
							}
							else
							{
								ui8Record_flag = 0 ;
							}	
							xdcfInit(imageDirNameStr, mp3SubDirRecfileName, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
							xdcfCurRootDirSet(otherRootNameStr);
							xdcfFilterSet(FILTER_AUDIO);
							
							xdcfDirCountGet(&dirCnt);
							xdcfCurDirPosGet(&dirIdx);
							xdcfCurDirAttrGet(&dirattr);
							xdcfFileCountGet(&fileCnt);
							if(fileCnt != 0)
								fileIdx = 1;
							pbRecordShow(ui8Record_flag,0);
						}
					}
					else
					{
						RecWavPlay = PB_DISP_ONE;
						pbRecordModeSelect( ui8Record_flag);
					}
					break;	
					
				case	UI_KEY_MODE_RECORD:
		            		osTaskResume(osTaskFindByName("HISTOGRAM"));
					osTaskSuspend(osTaskFindByName("AAA"));

					xdcfInit(imageDirNameStr, mp3SubDirRecfileName, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
					xdcfCurRootDirSet(otherRootNameStr);
					xdcfFilterSet(FILTER_AUDIO);

					printf("Cur dir =%s\n", vfsGetCurrDirName());

					
					printf("Cur dir =%s\n", vfsGetCurrDirName());

					hwWait(0,100);
					xdcfDirCountGet(&dirCnt);
					xdcfCurDirPosGet(&dirIdx);
					xdcfCurDirAttrGet(&dirattr);
					xdcfFileCountGet(&fileCnt);
					printf("Cur Dir = %s\n", dirattr.name);

					printf("Cur file count =%d\n", fileCnt);
					fileIdx = 1;
					if(fileCnt == 0)
					{
						fileIdx = 0;
					}
					
					RecWavPlay = PB_DISP_NINE;
					pbRecordShow(ui8Record_flag,0);

					/**fqdao_add for bug put car clean DATE 06.7.15**/
					if((fileCnt == 0)&&(ui8Record_flag==Play_mode))
					{
						osdBarDraw(156 ,84, 120, 24, 0x00);
						ui8Record_flag = RECORD_MODE ;
						continue ;
					}
					/**fqdao_add for bug put car clean DATE 06.7.15**/
					break;
					
				default:
					break;
			}

		}
	//	if(ui8Record_flag>Play_mode)
	//	{
	//		continue;
	//	}
		keyGet(&key);
	}
	hwDispPipEn(0);

	uiState = ui32NextState;
}
void  pbShowMP3RecordBG()
{
	pbImagePaste(MenuR[0][0], MenuR[0][3], MenuR[0][1], MenuR[0][2]);


}

//**************************************************************************
void pbRecordShow(UINT8 flag ,UINT32 filePos )
{
	UINT32 dispW, dispH;
	UINT32 gpb;

	osdFWBuffChange();

	osdClearScreen(0x00);
	hisIconDisp();
	hisTimeDisp();
 	xdcfCurFileAttrGet(&fileattr);
	xdcfFileTypeGet(fileIdx, &fileType);

	osdStrDisp(34, 4, UserFont10x20, 0xd0, GetRecordString(Rec_Title_str));

	
	if(filePos == 0 )
	{
	//	sysgMemDispAddrGet(&gpb);
	//	hwDispPreviewSizeGet(&dispW, &dispH);
	//	hwPlayBufSizeSet(dispW,	dispH);
	//	hwPlayBufAddrSet(gpb);
		pbShowMP3RecordBG();
		pbImagePaste(MenuR[1+flag][0],  MenuR[1+flag][3], MenuR[1+flag][1],MenuR[1+flag][2]); 
	}

	if(flag == RECORD_MODE)
	{

		
		osdStrDisp(156, 59, UserFont10x20, 0xd0, GetRecordString(Rec_Title_str));
	}
	else if(flag == Play_mode) 
	{
		//osdStrDisp(156, 61, UserFont10x20, 0xd0, GetRecordString( Rec_Playback));
		Pb_RecordPlay(GetRecordString( Rec_Playback));
		pbRecordFileList(fileIdx);
		pbRecordFileInfo(1,0);
	}
	else if (flag == FileInfo_mode)
	{
		//osdStrDisp(156, 61, UserFont10x20, 0xd0, GetRecordString( Rec_Playback));
		Pb_RecordPlay(GetRecordString( Rec_Playback));
		pbRecordFileInfo(1,0);
		pbShowFileNum(fileIdx,fileCnt) ;

	}
	
	osdHWBuffChange();

}

//**************************************************************************
#define   PROGRESS_VOL 		2	 
#define 	PROGRESS_MOV 		1

audFileInfo_t audioProfile;

void pbRecordFileList(UINT32 filePos)
{

	UINT8 space[13];
	SINT32 i;
	UINT32 err;
	UINT32 y[FILELIST_MAXLINE] = {TEXT_Y,TEXT_Y1,TEXT_Y2};
	UINT32	osdW, osdH;
	UINT8 List_idx = 0;
//	static UINT8 List_idx = 0 ;
	static UINT32 active_line = 0;
	UINT8 str[256];
	
	UINT32 fileattr_m[FILELIST_MAXLINE];
	
	xdcfAttrElm_t attr;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

//	F_Mp3IconRefresh(PROGRESS_MOV) ;
	F_Mp3IconRefresh(PROGRESS_VOL) ;
	
	if(filePos == 0)
	{/*reset*/
		List_idx = 0;
		active_line = 0;
	}

	if(fileCnt== 0 || RecWavPlay== PB_DISP_ONE)
		return;
	/*clear window*/
	memset(space, ' ', 12);
	space[12] = '\0';

	if(List_idx != (filePos+FILELIST_MAXLINE-1)/FILELIST_MAXLINE)
	{
		List_idx = (filePos+FILELIST_MAXLINE-1)/FILELIST_MAXLINE;
		
		for(i=0; i<FILELIST_MAXLINE; i++)
			osdStrDisp(TEXT_X, y[i], UserFont10x20, 0, &space[0]);/*clear*/
		for(i = (List_idx-1)*FILELIST_MAXLINE; i<(List_idx*FILELIST_MAXLINE); i++)
		{
			if(i < fileCnt)
			{
				xdcfCurFileByPosSet(i+1);
				err = xdcfCurFileAttrGet(&attr);
				audAppFileInfoGet(attr.name,&audioProfile);	
				totalTime[i%FILELIST_MAXLINE] = audioProfile.residualTime;
				strncat(attr.name, &space[0], 12-strlen(attr.name));
				fileattr_m[i%FILELIST_MAXLINE] = attr.attribute & xDCF_ATTR_READONLY;

				if(SUCCESS == err)
					strncpy(&strRec[i%FILELIST_MAXLINE][0], attr.name,13);
				else
					strncpy(&strRec[i%FILELIST_MAXLINE][0], &space[0], 13);
			}
			else
				strncpy(&strRec[i%FILELIST_MAXLINE][0], &space[0], 13);

			
			strRec[i%FILELIST_MAXLINE][12] = '\0';
			/*osdStrDisp(212, y[i%3], UserFont, 0x00, &str[i%3][0]);*/
		}
	}

 	if (filePos !=0)
	{
		/*active selected file*/
		for(i=0; i<FILELIST_MAXLINE; i++)
		{
			if ((List_idx-1)*FILELIST_MAXLINE + i < fileCnt)
			{
		             osdStrDisp(TEXT_X, y[i], UserFont10x20, 0xf0, &strRec[i][0]);
				sio_psprintf(str, "%d:%d",totalTime[i]/1000/60, totalTime[i]/1000%60);
				osdStrDisp(220, y[i], UserFont10x20, 0xf0, str);

//				if(attr.attribute&xDCF_ATTR_READONLY/*&&((totalTime[i]/1000/60)||(totalTime[i]/1000%60))*/)
				if(fileattr_m[i])
				{
					osdStrDisp(272, y[i]+3 , IconLock, 0xf0,"\x1");
				}
				else
				osdStrDisp(272, y[i]+3, IconLock, 0xf0," ");

				
			}
			else
			{
			  osdStrDisp(TEXT_X, y[i], UserFont10x20, 0xf0, &strRec[i][0]);
				

			}
		}		
		active_line = (filePos-1)%FILELIST_MAXLINE;
		osdBarDraw( TEXT_X-10,y[active_line]-2, 242,22, 0xf);
		osdStrDisp(TEXT_X, y[active_line], UserFont10x20, 0xdf, &strRec[active_line][0]);
		
		sio_psprintf(str, "%d:%d", totalTime[active_line]/1000/60, totalTime[active_line]/1000%60);
		osdStrDisp(220, y[active_line], UserFont10x20, 0xdf, str);

//		if(attr.attribute&xDCF_ATTR_READONLY/*&((totalTime[active_line]/1000/60)||(totalTime[active_line]/1000%60))*/)
		if(fileattr_m[active_line])
		{
			osdStrDisp(272, y[active_line]+3 , IconLock, 0xdf,"\x1");
		}
		else
			osdStrDisp(272, y[active_line]+3, IconLock, 0xf," ");
		printf(" file attribute %d\n", attr.attribute&xDCF_ATTR_READONLY);

		pbShowFileNum(filePos,fileCnt) ;
		xdcfCurFileByPosSet(filePos);
		osdPlayProgressDraw(1,0);
		
//		}// up bracket
	}
}

void pbRecordFileInfo(UINT8 flag,UINT8 flag_Re)
{
	UINT8  	str[25];
	struct	tm	*ptime;
    	UINT8 dateStr[21];
      UINT16 time;
 	UINT32 err;
 	SINT32 fd;
	UINT32 filesize = 0;
	UINT32 sampleRate;

	xdcfAttrElm_t attr;

	//osdFWBuffChange();

	//osdClearScreen(0x00);
	//hisIconDisp();
    //hisTimeDisp();

	totalTime[0] = 0 ;
	if(flag == RECORD_MODE )
	{
 		sio_psprintf(str, "%02d:%02d:%02d ", wavTime/1000/60/60, (wavTime%3600000)/1000/60, wavTime/1000%60);
		osdStrDisp(TEXT_X , TEXT_Y1, UserFont10x20, 0x40, str);
		if(flag_Re == 1)	
		{
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
		
			osdStrDisp (TEXT_X, TEXT_Y2, UserFont10x20, 0x40, dateStr);
		}
		hisTimeDisp();
		if(( wavTime/1000%60)%2)// 录制闪动
		{
			pbImagePaste(MenuR[1][0],  MenuR[1][3], MenuR[1][1],MenuR[1][2]); 
		}
		else
		{
			pbImagePaste(MenuR[3][0],  MenuR[3][3], MenuR[3][1],MenuR[3][2]); 
		
		}			
		
	}
	else if(flag == Play_mode)
	{
		 if(RecWavPlay == PB_DISP_ONE)
		 {

			err	= xdcfCurFileAttrGet(&attr);
			err = xdcfFileOpen(attr.name, 0, 0, &fd);
			if ( err != 0 )
			{
				audioErr = 1;
				printf("File cannot open, err=0x%x\n", err);
				osdStrDisp(26, 70, UserFont10x20, 0x10, g_strFileErr);
				return 1;
			}
			else
			{
				filesize =xdcfFileSizeGet(fd);
				printf(" size =%d\n",filesize);
			}
			xdcfFileClose(fd);

			audAppFileInfoGet(attr.name,&audioProfile);
			sampleRate = audioProfile.sampleRate;
			printf("\n sampleRate =%d\n",sampleRate);
		
			totalTime[0] = audioProfile.residualTime;
			printf("\n totalTime =%d\n",totalTime[0]);
			#if 1
			UINT32 dispAddr;
			UINT32 dispW, dispH;

			hwDispPreviewSizeGet(&dispW, &dispH);
			sysgMemDispAddrGet(&dispAddr);
			hwPlayBufSizeSet(dispW,	dispH);
			hwPlayBufAddrSet(dispAddr);

		//	pubFill((UINT8 *)(0xAC000000 | dispAddr*2), dispW*2, dispH,
		//		30, 140, dispW*2-60, 80, 0x0000);/*eric@5/9/2003*/
		//		16, 32+16, dispW*2-32, dispH-64,0x0000);


			//	sio_psprintf(str, "Artist:  %s", mp3_Info.artist);
			//	osdBarDraw(offsetX, offsetY, 32*12, 20, 0x08);
			//	osdStrDispExt(offsetX, offsetY, UserFont10x20, 0xd0, str, 0, osdW/16-8, 0);

			//	sio_psprintf(str, "Album :  %s", mp3_Info.album);
			//	osdBarDraw(offsetX, offsetY+24, 32*12, 20, 0x08);
			//	osdStrDispExt(offsetX, offsetY+ 24, UserFont10x20, 0xd0, str, 0, osdW/16-8, 0);
				osdStrDisp(TEXT_X,TEXT_Y, UserFont10x20, 0x40, attr.name);

				sio_psprintf(str, "Samplerate: %dk", sampleRate/1000);
			//	osdBarDraw(offsetX, offsetY+24*2, 32*12, 20, 0x08);
				osdStrDisp(TEXT_X,TEXT_Y1, UserFont10x20, 0x40, str);

				sio_psprintf(str, "TotalTime : %d:%d", totalTime[0]/1000/60, totalTime[0]/1000%60);
			//	osdBarDraw(offsetX, offsetY+24*3, 32*12, 20, 0x08);
				osdStrDisp(TEXT_X, TEXT_Y2, UserFont10x20, 0x40, str);

			
				if (audioErr == 1)
				{
					//osdBarDraw(offsetX, offsetY+24*4, 32*12, 20, 0x00);
					//osdStrDisp(offsetX, offsetY+ 24*4, UserFont10x20, 0x18, "     FILE ERROR");
					//osTimeDly(20);
				}
				#ifdef	OSD_DEBUG
				printf("\nPLAY... ");
				printf(fileattr.name);
				printf("\ninfo:\ntitle:%s\n", mp3_Info.title);
				printf("artist:%s\n", mp3_Info.artist);
				printf("album:%s\n", mp3_Info.album);
				#endif
	
		#endif

		 }


	
 		//sio_psprintf(str, "%02d.%02d.%04d", fileattr.time.mon, fileattr.time.day, fileattr.time.year);
 		sio_psprintf(str, "%04d.%02d.%02d",  attr.time.year, attr.time.mon, attr.time.day);
 		osdStrDisp(186-30, 84, UserFont10x20, 0xd0, str);
	//	sio_psprintf(str, "%02d:%02d:%02d", fileattr.time.hour, fileattr.time.min, fileattr.time.sec);
	// 	osdStrDisp(186, 68, UserFont10x20, 0xd0, str);
	}	
	//osdHWBuffChange();
}

/**
**/

#define	MP3_PROGRESS_STR_X	8
#define	MP3_PROGRESS_STR_Y	117
#define	MP3_PROGRESS_END_X	(300+MP3_PROGRESS_STR_X-22+6)
#define	MP3_PROGRESS_END_Y	88
extern  UINT32 ui32CurProgressX;
extern  UINT32 ui32totalTimeSave[] ;
void osdPlayProgressDraw(UINT32 ui32TotalTime,UINT32 ui32timeElapsed )
{
	UINT32 ui32ProgressX_Temp ; 
	if(ui32TotalTime == 0 ) 
		return ;

	ui32ProgressX_Temp = (MP3_PROGRESS_END_X-MP3_PROGRESS_STR_X)*ui32timeElapsed/ui32TotalTime ;

	if(ui32CurProgressX!=ui32ProgressX_Temp||ui32TotalTime== 1||!(ui32timeElapsed/ui32TotalTime))
	{
		osdBarDraw(MP3_PROGRESS_STR_X , MP3_PROGRESS_STR_Y,MP3_PROGRESS_END_X-MP3_PROGRESS_STR_X+40,
							16, 0x00);
		ui32CurProgressX = ui32ProgressX_Temp ;

		osdStrDraw(MP3_PROGRESS_STR_X+ui32CurProgressX,MP3_PROGRESS_STR_Y,UserFont10x8_4,0x00,"\x1");
	}

}
#if 0	
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
	x = 5;//64-14;
	y = osdH - w -105; //tzw modify //90+31
	l = osdW - x - 5;//64+14;

//	osdStrDisp(x-w, y, UserIcon16x16, color&0xf0, "\x03");
//	osdStrDisp(x+l, y, UserIcon16x16, color&0xf0, "\x04");

//	osdFrameDrawX(x, y, l, w, color&0x0f, 3);
	x += 3;
	y += 3;
	w -= 6;
	l -= 6;
	osdBarDraw(x, y, l, w, 0x00);

	stepLgh = l / (end-start+1);

//	if((avPlayNow==OBJ_MP3)&&(mp3AppStatusGet() == MP3_PENDING))
//	{
	 	/*osdBarDraw(x+l-16, y, 16, w, color>>4);*/
//	}
//	else
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
#endif
/**
**/
void pbPlayback_Time()
{
	UINT32 timeElapsed = 0 , timeTotal= 0, timeleft=0;
	audFileInfo_t  audFileInfo;	
	UINT8  	str[25];
	UINT32	osdW, osdH;
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	audAppFileInfoGet(NULL, &audFileInfo);
	timeTotal = audFileInfo.playDuration;
	timeElapsed = audFileInfo.playDuration - audFileInfo.residualTime;
	timeleft =audFileInfo.residualTime;

	if (timeTotal == 0) /* wav<2k timetotal=0,  for osdHProgressDraw*/
	{
		timeTotal = 20;
		timeElapsed =10;
	}
	wavTime = timeElapsed;
	if (timeElapsed>timeTotal)
	{
		timeElapsed=timeTotal;
		timeleft=0;
	}

	if ((g_playrecStatus == STATUS_FF)&&(timeleft< 5000)) 	
	{
		timeElapsed=timeTotal;
		timeleft=0;
	}
	
	osTimeDly(1);
	
//	F_Mp3IconRefresh(PROGRESS_MOV) ;
//	F_Mp3IconRefresh(PROGRESS_VOL) ;
	osdPlayProgressDraw( timeTotal, timeElapsed);

	sio_psprintf(str, "%02d:%02d ", timeElapsed/1000/60, timeElapsed/1000%60);
	osdStrDisp(242,TEXT_Y2, UserFont10x20, 0x30, str);

//	sio_psprintf(str, "%02d:%02d ", timeleft/1000/60, timeleft/1000%60);
//	osdStrDisp(180, 186, UserFont10x20, 0xd0, str);

	osdBarDraw(156 ,84, 100, 24, 0x00);
	sio_psprintf(str, "%04d.%02d.%02d", fileattr.time.year, fileattr.time.mon, fileattr.time.day);
	osdStrDisp(156, 84, UserFont10x20, 0xd0, str);
}

/**
**/
void modifyDisplayVol_flag()
{
	UINT8  	str[25];
	if(ui8DisplayVol_flag != DisplayVol_NM )
	{
		ui8DisplayVol_flag = DisplayVol_NM;
		return 0 ;
	}	
//	osdBarDraw(156 ,84, 100, 50, 0x00);
//	sio_psprintf(str, "%02d.%02d.%04d", fileattr.time.mon, fileattr.time.day, fileattr.time.year);
//	osdStrDisp(156, 84, UserFont10x20, 0xd0, str);
//	sio_psprintf(str, "%02d:%02d:%02d", fileattr.time.hour, fileattr.time.min, fileattr.time.sec);
// 	osdStrDisp(186, 68, UserFont10x20, 0xd0, str);
}

/**
**/
void osdIconDisp_Play(UINT8 isClear)
{
	UINT32	osdW, osdH;
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

//	osdFWBuffChange();
	 switch (g_playrecStatus)
		{

			case	STATUS_STOP:
				osdStrDisp(osdW/2+24, 0, UserIcon10x20, 0x30, "\x57\x58");
				break;
			case	STATUS_PLAYING:
				osdStrDisp(osdW/2+24 , 0, UserIcon10x20, 0x40, "\x8e\x8f");
				break;
			case	STATUS_PAUSE:
				osdStrDisp(osdW/2+24, 0, UserIcon10x20, 0x30, "\x90\x91");
				break;
			case	STATUS_FF:
				osdStrDisp(osdW/2+24, 0, UserIcon10x20, 0x40, "\x8a\x8b");
				break;
			case	STATUS_REW:
				osdStrDisp(osdW/2+24, 0, UserIcon10x20, 0x40, "\x8c\x8d");
				break;
			default:
				osdStrDisp(osdW/2+24, 0, UserFont10x20, 0x0, "  ");
				break;
		}

#if 1 //tzw add 05-09-23  // 快进不能有音
		if ((g_playrecStatus == STATUS_FF)||(g_playrecStatus == STATUS_REW))
		{
			if (!	ui8SaveVolFF)
		      {
            			  ui8SaveVolFF =1;
				#if (CurrentIC8751==0)		  
					F_Mp3SetVolum(0);
				//	pbVolumWindowRfsh(0);
				#else	
			        if(SpeakFlag)
					pbSet8751Volume(ws9712_OUT2,0);
				 else
					pbSet8751Volume(ws9712_HP, 0);
				#endif
			}
		}
		else if(ui8SaveVolFF )
            	{
		#if (CurrentIC8751==0)
			if(SpeakFlag)
			{
//				pbVolumWindowRfsh(mp3Vol);
			}
				F_Mp3SetVolum(mp3Vol);//fqdao_modify for bug ear no voice 06.6.1
		 #else
			wm8751LineOut(SpeakFlag);
			if(SpeakFlag)
			{
				pbSet8751Volume(ws9712_OUT2, mp3Vol);
			}
			else
			{
				pbSet8751Volume(ws9712_HP, mp3Vol);
			}
			
		#endif
		ui8SaveVolFF =0 ;
		}		
		
		#endif
	#if 0  // 显示音量标示
	if(ui8DisplayVol_flag == DisplayVol_T)
	{
		UINT8 i ;
		osdBarDraw(186, 46, 100, 50, 0x00);
		for(   i = 0; i <=mp3Vol; i++)
		{
			osdBarDraw((200+ i*9),74-i*2 , 4, 3+i*2, 0x0d); 
		}
	}	
	#endif	 
//	osdHWBuffChange();
}

/**
**/
void pbRecordModeSelect( UINT8 flag   )
{
	UINT32 	sys;
	UINT32 	dispW, dispH;
	UINT32	gPB;
	UINT32 	freespaceKbytes;

	if(flag == RECORD_MODE) 	 
	{
	//	sysgMemDispAddrGet(&gPB);
	//	hwPlayBufAddrSet(gPB);
	//	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	//	hwCammodeSet(5);	  /* play back mode	*/
	//	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/
	//	gpioSet(GPIO_VIDEO_EN, 0);
	//	osdClearScreen(0);
		pbRecordShow(ui8Record_flag,0);

		freespaceKbytes = 0;
		xdcfFreeSpaceGet(&freespaceKbytes);
		freespaceKbytes /= 1024;
		if(freespaceKbytes > 64)
		{
		      buttonAudio(1);  //fqdao_add 06.5.29
	            osTimeDly(4) ;
			pbRecord_wav();
		}	
		else
		{
			MsgShow(memory_full_s);
			RecWavPlay = PB_DISP_NINE ; //fqdao_add for bug 06.5.22
			//hwWait(0, 1000);  //fqdao_modify for bug 06.5.22			 
		}	
	}
	else if(flag == Play_mode) 
	{
	
		pbRecordShow(ui8Record_flag,1);

		pbPlayback_wav();

	}

}

/**
**/
extern  UINT8 audioErr;
void pbPlayback_wav(void)
{
	audioErr = 0;

	if (fileCnt == 0)
	{
	//	osdClearScreen(0);
	//	osdIconDisp(0);
		osTimeDly(200);         //Danny --  05/05/26
		menuReturn(UI_MAINMENU, 0);			
	}
	else if(RecWavPlay == PB_DISP_ONE)
	{

		pbRecordShow(FileInfo_mode,1);
	//	tmrEnable(1, 8000, TMR_PERIODIC, modifyDisplayVol_flag);//wendy@050205
#if (CurrentIC8751==0)
		ws9712LineOut(SpeakFlag);//wendy for speak
		if(SpeakFlag)
		{
			pbSetVolume(ws9712_OUT2, mp3Vol);
		}
		else
		{
			pbSetVolume(ws9712_HP, mp3Vol);
		}
		F_Mp3IconRefresh(PROGRESS_VOL) ;
#else
	    wm8751LineOut(SpeakFlag);//wendy for speak
		if(SpeakFlag)
		{
			pbSet8751Volume(ws9712_OUT2, mp3Vol);
		}
		else
		{
			pbSet8751Volume(ws9712_HP, mp3Vol);
		}
#endif	
		
		if (audioErr ==1)
		{
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_DIR_DOWN]);
			audioErr = 0;
		}
		else
		{
			do
				{
					xdcfFileTypeGet(fileIdx, &fileType);
					//wm8751DACVolSelect(1);	// David 01102005 
					pbPlayRecord_wav(fileIdx);
					//wm8751DACVolSelect(0);

				}while(0);//(avPlayNow == OBJ_WAV)&&(mp3Repeat == MP3RepeatOne));
				osTimeDly(20);

				if ((avPlayNow == OBJ_WAV)
					&&/* ((mp3Repeat == MP3RepeatAll) ||*/(fileIdx<fileCnt)/*//wendy update (fileCnt < 5)*/)
				{
				      	UI_OSQFlush(uiKeyMsgQ);
					avPlayNow =	OBJ_NONE;
					if(ui8SelectUpDown == SelectUp)
						osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_DIR_UP]);
					else
						osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_DIR_DOWN]);
					ui8SelectUpDown = SelectDown;
				}
				#if 1		
				/* for bug 53 */
				else 
				if (((avPlayNow == OBJ_MP3) ||(avPlayNow == OBJ_WMA)||(avPlayNow == OBJ_WAV)))
			//	&& ((mp3Repeat == MP3RepeatRandom)))
				{
					osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_DIR_DOWN]);					
				}
				#endif				

		}

	
	//	SpeakFlagMode=0;//wendy @05025
#if (CurrentIC8751==0)		
		ws9712LineOut(SpeakFlag);//wendy for speak
		if(SpeakFlag)
		{
			pbSetVolume(ws9712_OUT2, mp3Vol);
		}
		else
		{
			pbSetVolume(ws9712_HP, mp3Vol);
		}
#else
	       wm8751LineOut(SpeakFlag);//wendy for speak
		if(SpeakFlag)
		{
			pbSet8751Volume(ws9712_OUT2, mp3Vol);
		}
		else
		{
			pbSet8751Volume(ws9712_HP, mp3Vol);
		}
#endif		

		tmrDisable(1);//wendy@050205
		pbRecordShow(ui8Record_flag,1);


	}
	else
	{
		pbRecordShow(ui8Record_flag,1);
	}
}

#if 1
void pbRecord_wav(void)
{

	UINT8 name[13];
	UINT32 sts;
	UINT32 startTime=0, endTime=0;
	UINT32 err,key;
	UINT32 storage;

	xdcfActiveDevIdGet(&storage);
	
	if((storage != DRIVE_NAND) && isCardProtect())
	{
		MsgShow(card_protect_s);
		hwWait(0, 1000);		
		return ;		
	}				
	if(snapAV==OBJ_NONE)
	{
		printf("Start record audio\n");
//		xdcfCurRootDirSet(mp3RootNameStr);//wendy@050131 add for audio
		audProfSet(0x00010000 | AUDIO_ADPCM, 16000, 1, 512);


		xdcfFilterSet(FILTER_AUDIO);
		xdcfNextFileNameGet(xDCF_FILETYPE_WAV, name);
		err = audRecordFileNameSet(name);
		osdStrDisp(TEXT_X, TEXT_Y, UserFont10x20, 0x40, name);
		printf("wav nameset:%s\n", name);
		printf("err = %d\n", err);
		
		UINT32  codecDev;
		sysgAudOutDevGet(&codecDev);
		#if (CurrentIC8751==0)	
		if( codecDev == AUDIO_AC97 ) {
			hwAudAC97RegWrite(0x2c, 16000);		/* 8K Hz*///wendy@050303 add for audio
			hwAudAC97RegWrite(0x2e, 16000);		/* 8K Hz*///wendy@050303 add for audio
			hwAudAC97RegWrite(0x32, 16000);		/* 8K Hz*///wendy@050303 add for audio
			/*fqdao_modify for bug recod voice 06.6.5*/
		}	
		wsAc97SetRegValEx(0x0e,0xffff,0x00e0 );

   		gpioConfig(GPIO_MUTE, 1);
     	   	gpioSet(GPIO_MUTE, 0);
		#else		
		if(SpeakFlag)
			wm8751SoundEnable(2,1);
		else
			wm8751SoundEnable(2,1);
		audVolumeSet(0x01);
		#endif					
		/*fqdao_add for bug record souffle 06.6.12*/
			hwAudAC97RegWrite(0x0e, 0x60e0);		 
			if(SpeakFlag)
			{
				pbSetVolume(ws9712_OUT2,0);
			}
			else
			{
				pbSetVolume(ws9712_HP, 0);
			}

		osdBarDraw(184,60+24,28,64,0); // for bug 26 tzw add 060215
		osdStrDisp(184, 60+24, UserFont10x20, 0xd0, GetRecordString(Rec_Start_str));//Danny--05/05/21
		
		wavTime = 0 ;  /* fqdao_add 06.6.7 */
		audRiffCtrl(RIFF_REC); /* kick off clipping	*/
		snapAV	= OBJ_WAV;
		startTime = tmrMsTimeGet();
		wavProgress = 0;
		//osTimeDly(20);		

		pbRecordFileInfo(RECORD_MODE,1);

		do
		{
			osTimeDly(30);
			PlaybackKey_playing(RECORD_MODE);
	
			sts = audAppStatusGet();
			endTime = tmrMsTimeGet();
			wavTime = endTime-startTime;
			pbRecordFileInfo(RECORD_MODE,0);

			
		}while((sts == RIFF_STATUS_CLEAR)&&(snapAV == OBJ_WAV));//wendy@050303

		osTimeDly(30);
		#if (CurrentIC8751==0)	
  	        gpioConfig(GPIO_MUTE, 1);
 	        gpioSet(GPIO_MUTE, 1);
		#else		
			wm8751SoundEnable(0,0);
		#endif	

		/*fqdao_add for bug record souffle 06.6.12*/
			if(SpeakFlag)
			{
				pbSetVolume(ws9712_OUT2,mp3Vol);
			}
			else
			{
				pbSetVolume(ws9712_HP,mp3Vol);
			}

		
		if(snapAV == OBJ_NONE)
       	{
			buttonAudio(ui8SnapTone);
		}
		
		printf("sts=0x%x\n", sts);
		printf("End record audio\n");
		pbImagePaste(MenuR[1][0],  MenuR[1][3], MenuR[1][1],MenuR[1][2]); 

		osdBarDraw(184,60+24,28,64,0); // for bug 26 tzw add 060215

		osdStrDisp(185, 60+24, UserFont10x20, 0xd0, GetRecordString(Rec_End_str)); //Danny --05/05/25
		RecWavPlay = PB_DISP_NINE;
		if (sts == RIFF_STATUS_DISK_FULL)
		{	
			 printf("Joe: disk is full for audio\n");
		     MsgShow(memory_full_s);
			 hwWait(0, 1000);
		}

		snapAV = OBJ_NONE;
	}
	else
	{
		audRiffCtrl(RIFF_STOP);	/* stop	clipping */
		snapAV = OBJ_NONE;
	}
}
#endif
/**
**/
UINT8 pbPlayRecord_wav(UINT32 idx)
{
		//UINT32 startTime=0, endTime=0;
		UINT32 err = SUCCESS;
		//UINT32 sts;
		
		UINT32  codecDev;

		sysgAudOutDevGet(&codecDev);
		audAppFileInfoGet(fileattr.name,&audioProfile);
	#if (CurrentIC8751==0)
		if( codecDev == AUDIO_AC97 ) 
		{
			printf("AC97 sample rate %d\n",audioProfile.sampleRate);

			if((audioProfile.sampleRate == 8000) || (audioProfile.sampleRate == 24000))
			{
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
	#else
	switch(audioProfile.sampleRate)
		{
			case	48000:
					wm8751SampleSet(0);
					break;
			case 	32000:
					wm8751SampleSet(1);
					break;
			case 	24000:
					wm8751SampleSet(2);
					break;
			case 	16000:
					wm8751SampleSet(3);
					break;
			case 	8000:
					wm8751SampleSet(4);
					break;
			default:
					wm8751SampleSet(0);
		}
	#endif

//xu add for era
	    gpioConfig(GPIO_MUTE, 1);	
	    gpioSet(GPIO_MUTE, 0);
//xu add for era

		avPlayNow =	OBJ_WAV;
		audPlayFileNameSet(fileattr.name);
		audRiffCtrl(RIFF_PLAY);
		g_playrecStatus = STATUS_PLAYING;
		wavProgress = 0;
		wavTime = 0 ;  /* fqdao_add 06.6.7 */
		ui8DisplayVol_flag = DisplayVol_NM;

		osTimeDly(20);
		do
		{
			PlaybackKey_playing(Play_mode);
			osSemPend(semApp , OS_NO_WAIT, &err);
			if (err == OS_NO_ERR)
			{
				osSemPost(semApp);
				osdIconDisp_Play(0);
			
			}
			else
			{
				osdIconDisp_Play(0);
				pbPlayback_Time();
				hisTimeDisp();

				if ((g_playrecStatus == STATUS_FF)||(g_playrecStatus == STATUS_REW))
				{
						audRiffCtrl( RIFF_TIMESET );
		           			osTimeDly( 10 );
						if (g_playrecStatus == STATUS_FF)
		            				audRiffCtrl((wavTime/1000) + 5 );/* jump to offset based on current elapse time */
						else
							audRiffCtrl((wavTime/1000) - 5 );/* jump to offset based on current elapse time */
		           			osTimeDly( 10 );
						err = OS_NO_WAIT;
				}
			}
		}while(err);
		/*osTimeDly(20);*/
		osSemPend(semApp , 0, &err);
		osSemPost(semApp);
		g_playrecStatus = STATUS_STOP;
		audRiffCtrl(RIFF_STOP);
		//osdIconDisp(0);
		osTimeDly(20);
		
//xu add for era
	    gpioConfig(GPIO_MUTE, 1);
	    gpioSet(GPIO_MUTE, 1);
//xu add for era

	return 0;
}

/**
**/
extern UINT32 g_ui32SaveKeyCode;
void PlaybackKey_playing(UINT8 flag)
{
	UINT32 key ,err;
	UINT32* msgPlay;
	#if 1
	msgPlay = osQuePend(uiKeyMsgQ, 20, &err);			
	if (err == OS_NO_ERR)
	{
		 key = *msgPlay ;
	}
	else
	{
		key = UI_MODE_MASK;
	}
	#endif	
	switch(key)
	{
		#if 0 
		case	UI_KEY_FUNC_ZOOMIN:
			
				if(flag==Play_mode )
				{
					if(mp3Vol<9)
						mp3Vol++;
					F_Mp3SetVolum(mp3Vol);
					ui8DisplayVol_flag = DisplayVol_T;

				}
				break;
		case	UI_KEY_FUNC_ZOOMOUT:
				if(flag==Play_mode )
				{
					if(mp3Vol)
						mp3Vol--;
					F_Mp3SetVolum(mp3Vol);
					ui8DisplayVol_flag = DisplayVol_T;

				}	
				break;
		#endif	

		case  UI_KEY_FUNC_OK:
			if(flag==Play_mode )
			{
				if(g_playrecStatus == STATUS_PAUSE)
				{
					g_playrecStatus = STATUS_PLAYING;
					RecWavPlay = PB_DISP_ONE;
				}	
				else
				{
					g_playrecStatus =STATUS_PAUSE;
				}	
				audRiffCtrl(RIFF_PAUSE);
			}
#if 0
			else
			{
				audRiffCtrl(RIFF_STOP);
				UI_OSQFlush(uiKeyMsgQ); 
			}
#endif
			break;

		case	UI_KEY_ACCIDENT:
			if(flag==Play_mode )
			{
				g_playrecStatus = STATUS_STOP;
				avPlayNow = OBJ_NONE;
				RecWavPlay= PB_DISP_NINE;
			}
//			else
//			{
				audRiffCtrl(RIFF_STOP);
				UI_OSQFlush(uiKeyMsgQ); 
//			}
			ui8Record_flag = RECORD_MODE ;
//			pbRecordShow(ui8Record_flag,0);
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);	
			break;
		case	UI_KEY_FUNC_A:
			if(flag==Play_mode )
			{
				g_playrecStatus = STATUS_STOP;
				avPlayNow = OBJ_NONE;
				RecWavPlay= PB_DISP_NINE;
			}
//			else
//			{
			      buttonAudio(1);  //fqdao_add 06.5.29
				audRiffCtrl(RIFF_STOP);
				UI_OSQFlush(uiKeyMsgQ); 
//			}
			ui8Record_flag = RECORD_MODE ;
//			pbRecordShow(ui8Record_flag,0);
			break;


		case  UI_KEY_FUNC_MENU:
			if(flag==Play_mode )
			{
				g_playrecStatus = STATUS_STOP;
				avPlayNow = OBJ_NONE;
				RecWavPlay= PB_DISP_NINE;
			}
		//	else
		//	{
				audRiffCtrl(RIFF_STOP);
				UI_OSQFlush(uiKeyMsgQ); 
		//	}	
			break;

		case UI_KEY_DIR_LEFT:
			if( flag == Play_mode )
			{
				if(g_playrecStatus != STATUS_REW)
				{
					osTimeDly(20);
					/*fqdao_modify for bug record_wav REW 06.5.30*/
					if (!(g_ui32SaveKeyCode & KEY_BIT_LEFT))
					{
						audRiffCtrl(RIFF_STOP);
						UI_OSQFlush(uiKeyMsgQ); 
						ui8SelectUpDown = SelectUp;
						break; 
					}	
				}
				g_playrecStatus = STATUS_REW;
			}	
			break;

		case UI_KEY_DIR_RIGHT:
			if(flag==Play_mode )
			{
				if(g_playrecStatus != STATUS_FF)
				{
					osTimeDly(20);
					if (!(g_ui32SaveKeyCode & KEY_BIT_RIGHT))
					{
						audRiffCtrl(RIFF_STOP);
						UI_OSQFlush(uiKeyMsgQ); 
						ui8SelectUpDown = SelectDown;
						break ;
					}	
				}
				g_playrecStatus = STATUS_FF;
			}	
			break;

		case UI_KEY_DIR_DOWN:
			#if 0			
			if(flag==Play_mode )
			{
				audRiffCtrl(RIFF_STOP);
				UI_OSQFlush(uiKeyMsgQ); 
				ui8SelectUpDown = SelectDown;
			}	
			#endif
			if(flag==Play_mode )
			{
				if(mp3Vol)
					mp3Vol--;
				F_Mp3SetVolum(mp3Vol);
				ui8DisplayVol_flag = DisplayVol_T;
				F_Mp3IconRefresh(PROGRESS_VOL) ;

			}
			break;
	
		case UI_KEY_DIR_UP:
			#if 0
			if(flag==Play_mode )
			{
				audRiffCtrl(RIFF_STOP);
				UI_OSQFlush(uiKeyMsgQ); 
				ui8SelectUpDown = SelectUp;

			}	
			#endif
			if(flag==Play_mode )
			{
				if(mp3Vol<9)
					mp3Vol++;
				F_Mp3SetVolum(mp3Vol);
				ui8DisplayVol_flag = DisplayVol_T;
				F_Mp3IconRefresh(PROGRESS_VOL) ;
			}
			break;
		default:
			if(g_playrecStatus != STATUS_PAUSE)
			{
				g_playrecStatus = STATUS_PLAYING;
			}
			break;
	}	
}

//***************************************************************************
void MsgShow(UINT16 msgID)
{
   UINT16 len;
   UINT16 Msg_X,Msg_Y;
   UINT8 *string;
   UINT16 i ;
   string= GetString(msgID);
   len = F_STRCTRL(string);
   Msg_X=180 ;
   Msg_Y=120;
   buttonAudio(1);
   osdStrDisp(Msg_X - len * 5 , Msg_Y, UserFont10x20, 0x10,string);
/* fqdao_add 06.5.17*/
   hwWait(0, 1000);
   osdStrDisp(Msg_X - len * 5 , Msg_Y, UserFont10x20, 0x00,"                                 ");
   /* fqdao_add 06.5.17*/
}

#if 0
//***************************************************************************
#define	xDCF_ATTR_PROTECT		xDCF_ATTR_READONLY |fileattr.attribute
#define	xDCF_ATTR_DISPROTECT	(~xDCF_ATTR_READONLY) &fileattr.attribute


#define	REC_MENU_X	64
#define	REC_MENU_Y	32


#define REC_BAR_X			(REC_MENU_X-4)
#define REC_BAR_Y			(REC_MENU_Y + 48 - 4)
#define REC_BAR_WIDTH       	196
#define REC_BAR_HEIGHT     	 24

#define REC_TITLE_OFF_X	REC_MENU_X+60
#define REC_TITLE_OFF_Y	REC_MENU_Y+2


#define REC_TEXT_OFF_X      (REC_MENU_X + 10)
#define REC_TEXT_OFF_Y      (REC_MENU_Y + 48 )
#define REC_TEXT_HEIGHT     24

#define REC_ICON_X      		24 
#define REC_ICON_Y			196
#define REC_ICON_OFF_X		76

enum
{
	REC_MENU_DEL = 0,
	REC_MENU_PROTECT
};

enum
{
	REC_DEL_TITLE = 0,
	REC_DEL_CUR ,
	REC_DEL_ALL
};


#define	REC_ITEM_MAX  3
#define	REC_SUBITEM_MAX  3
#define	REC_ITEM_LIST_MAX 4

UINT8	RECSubMax[REC_ITEM_MAX] = 
{2,2,2};


UINT32	RECItemImage[REC_ITEM_MAX][2] = 
{
	{icon_rec,sizeof(icon_rec)},
	{icon_pro,sizeof(icon_pro)},
	{icon_dpr,sizeof(icon_dpr)},	
};
/*
UINT32	RECSubItemImage[REC_ITEM_MAX][] = 
{
	
};
*/
UINT8 *RECStrings[REC_ITEM_MAX][REC_SUBITEM_MAX][LANGUAGES] = 
{
	{
		{
		"删除"	,
		"删除"	,
		"删除"	,
		},
		{
		"删除当前",
		"删除当前",
		"删除当前",
		},
		{
		"删除全部",
		"删除全部",
		"删除全部",
		}
	},
	{
		{
		"保护"	,
		"保护"	,
		"保护"	,
		},
		{
		"保护当前",
		"保护当前",
		"保护当前",
		},
		{
		"保护全部",
		"保护全部",
		"保护全部",
		},
	},
	{
		{
		"取消保护"	,
		"取消保护"	,
		"取消保护"	,
		},
		{
		"取消保护当前",
		"取消保护当前",
		"取消保护当前",
		},
		{
		"取消保护全部",
		"取消保护全部",
		"取消保护全部",
		},
	}	
	
} ;



void	F_RecMenuDel(UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;
	osdClearScreen(0x00) ;
	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			color = 0xd0;
			osdStrDisp(REC_TITLE_OFF_X, REC_TITLE_OFF_Y+REC_TEXT_HEIGHT*i, UserFont10x20, color, RECStrings[0][i][sPara.language]) ;

		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw(REC_BAR_X, REC_BAR_Y+REC_TEXT_HEIGHT*(i-1), REC_BAR_WIDTH, REC_BAR_HEIGHT, 0x0e) ;
			osdStrDisp(REC_TEXT_OFF_X, REC_TEXT_OFF_Y+REC_TEXT_HEIGHT*(i-1), UserFont10x20, color, RECStrings[0][i][sPara.language]) ;
			
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(REC_TEXT_OFF_X, REC_TEXT_OFF_Y+REC_TEXT_HEIGHT*(i-1), UserFont10x20, color, RECStrings[0][i][sPara.language]) ;
		}
	}
}
void	F_RecMenuProtect(UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;
	osdClearScreen(0x00) ;	
	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			color = 0xd0;		
			osdStrDisp(REC_TITLE_OFF_X, REC_TITLE_OFF_Y+REC_TEXT_HEIGHT*i, UserFont10x20, color, RECStrings[1][i][sPara.language]) ;
		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw(REC_BAR_X, REC_BAR_Y+REC_TEXT_HEIGHT*(i-1), REC_BAR_WIDTH, REC_BAR_HEIGHT, 0x0e) ;			
			osdStrDisp(REC_TEXT_OFF_X, REC_TEXT_OFF_Y+REC_TEXT_HEIGHT*(i-1), UserFont10x20, color, RECStrings[1][i][sPara.language]) ;
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(REC_TEXT_OFF_X, REC_TEXT_OFF_Y+REC_TEXT_HEIGHT*(i-1), UserFont10x20, color, RECStrings[1][i][sPara.language]) ;
		}
	}
}


void	F_RecMenuDisProtect(UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;
	osdClearScreen(0x00) ;	
	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			color = 0xd0;		
			osdStrDisp(REC_TITLE_OFF_X, REC_TITLE_OFF_Y+REC_TEXT_HEIGHT*i, UserFont10x20, color, RECStrings[2][i][sPara.language]) ;
		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw(REC_BAR_X, REC_BAR_Y+REC_TEXT_HEIGHT*(i-1), REC_BAR_WIDTH, REC_BAR_HEIGHT, 0x0e) ;			
			osdStrDisp(REC_TEXT_OFF_X, REC_TEXT_OFF_Y+REC_TEXT_HEIGHT*(i-1), UserFont10x20, color, RECStrings[2][i][sPara.language]) ;
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(REC_TEXT_OFF_X, REC_TEXT_OFF_Y+REC_TEXT_HEIGHT*(i-1), UserFont10x20, color, RECStrings[2][i][sPara.language]) ;
		}
	}
}

typedef	void	(*pRECSubMenuProc)(UINT8 idx) ;

pRECSubMenuProc	RefreshSubitem[REC_ITEM_MAX] = 
{
	F_RecMenuDel,
	F_RecMenuProtect, 
	F_RecMenuDisProtect
};

void	F_RecMenuDelImage()
{
	UINT32	i,j ;
	UINT32	x,y ;
	x = 24 ;
	y = 196 ;
	UINT32 dispW, dispH;
	UINT32 gpb;
	dispW = PIP_W;
	dispH = PIP_H;	
	sysgMemDispAddrGet(&gpb);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gpb);
	pbDramImgPlay(bkground, sizeof(bkground), gpb, dispW, dispH);
	pbImagePaste(RECItemImage[0][0],RECItemImage[0][1],REC_ICON_X+REC_ICON_OFF_X,REC_ICON_Y) ;
	pbImagePaste(RECItemImage[1][0],RECItemImage[1][1],REC_ICON_X+REC_ICON_OFF_X*2,REC_ICON_Y) ;
	pbImagePaste(RECItemImage[2][0],RECItemImage[2][1],REC_ICON_X+REC_ICON_OFF_X*3,REC_ICON_Y) ;
	RefreshSubitem[0](1) ;
}
void	F_RecMenuProtectImage()
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
	pbDramImgPlay(bkground, sizeof(bkground), gpb, dispW, dispH);
	pbImagePaste(RECItemImage[0][0],RECItemImage[0][1],REC_ICON_X,REC_ICON_Y) ;
	pbImagePaste(RECItemImage[1][0],RECItemImage[1][1],REC_ICON_X+REC_ICON_OFF_X,REC_ICON_Y) ;
	pbImagePaste(RECItemImage[2][0],RECItemImage[2][1],REC_ICON_X+REC_ICON_OFF_X*2,REC_ICON_Y) ;
	RefreshSubitem[1](1) ;
}


void	F_RecMenuDisProtectImage()
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
	pbDramImgPlay(bkground, sizeof(bkground), gpb, dispW, dispH);
	pbImagePaste(RECItemImage[1][0],RECItemImage[1][1],REC_ICON_X,REC_ICON_Y) ;
	pbImagePaste(RECItemImage[2][0],RECItemImage[2][1],REC_ICON_X+REC_ICON_OFF_X,REC_ICON_Y) ;
	RefreshSubitem[2](1) ;
}


typedef	void	(*pRECMenuProc)(void) ;

pRECMenuProc	RefreshItem[REC_ITEM_MAX] = 
{
	F_RecMenuDelImage,
	F_RecMenuProtectImage,
	F_RecMenuDisProtectImage
};




void	F_RecMenuFunc()
{
	UINT32	key ;
	UINT8 item=0,	subitem=1	 ;
	xdcfAttrElm_t attr;	
	UINT32	err ;
	UINT32 storage;	
	osdClearScreen(0x00);

	printf("enter menu!");
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
				if(subitem<RECSubMax[item]) subitem ++ ;
				RefreshSubitem[item](subitem) ;
				break ;
			case	UI_KEY_DIR_LEFT:
				if(item)	
				{
					item -- ;	
					RefreshItem[item]();
				}
				break ;
			case	UI_KEY_DIR_RIGHT:
				if(item<REC_ITEM_MAX-1) 
				{
					item++ ;
					RefreshItem[item]();
				}
				break ;
			case	UI_KEY_FUNC_OK:
				if(item==0)
				{
					if(subitem==1)
					{
						F_RecDelSingleFile(fileIdx);

					}
					else if(subitem==2)
					{
						F_RecDelAllFile();
					}
				}
				else if(item==1)
				{
					if(subitem==1)
					{
						F_RecProtectSingleFile(fileIdx);
					}
					else if(subitem==2)
					{
						F_RecProtectAllFile() ;
					}
				}
				else	 if(item ==2)
				{
					if(subitem==1)
					{
						F_RecDisProtectSingleFile(fileIdx);
					}
					else	 if(subitem ==2)
					{
						F_RecDisProtectAllFile();
					}
				}
				pbFileListWindowRfsh(fileIdx,1);
				return ;
				
				break ;
			case	UI_KEY_FUNC_MENU:
  				isMenuOn=0;
  				return ;
				break ;
		}
	}
}


void	F_RecDelSingleFile(UINT32	idx)
{
UINT32	err ;
		if((xdcfCurFileByPosSet(idx)) != 0)
		{
			#ifdef	OSD_DEBUG
			printf("\nErase	error1...(idx=%d)",	fileIdx);
			#endif
		}
		else if (xdcfCurFileAttrGet(&fileattr)==0)
		{
//							xdcfDelOnlyFileFuncSet(readOnlyFileErase);
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
}
void	F_RecDelAllFile()
{
UINT32	storage ;
#if 0
		xdcfActiveDevIdGet(&storage);
		if((storage != DRIVE_NAND) && isCardProtect())
		{
			MsgShow(card_protect_s);
			hwWait(0, 1000);				
		}
		else
#endif			
		{				
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
}
void	F_RecProtectSingleFile(UINT32	idx)
{
		xdcfCurFileByPosSet(idx) ;
		xdcfCurFileAttrGet(&fileattr) ;
		xdcfCurFileAttrSet(xDCF_ATTR_PROTECT) ;
}
void	F_RecProtectAllFile()
{
	UINT32	i;
	xdcfFileCountGet(&fileCnt);
	for(i=1;i<=fileCnt;i++)
	{
		F_RecProtectSingleFile(i) ;
	}
}

void	F_RecDisProtectSingleFile(UINT32	idx)
{
		xdcfCurFileByPosSet(idx) ;
		xdcfCurFileAttrGet(&fileattr) ;
		xdcfCurFileAttrSet(xDCF_ATTR_DISPROTECT) ;
}
void	F_RecDisProtectAllFile()
{
	UINT32	i;
	xdcfFileCountGet(&fileCnt);
	for(i=1;i<=fileCnt;i++)
	{
		F_RecDisProtectSingleFile(i) ;
	}
}
#endif
//************************************************************************** 
//************************************************************************** 
//************************************************************************** 
UINT8* GetRecordString(UINT16 stringId)
{
    UINT8	*str;
    UINT32 	languageId;

	languageId = sPara.language;
    if (languageId >= LANGUAGES)
    {
		languageId = LanguageEnglish;
    }
    if(Ui_RecordStrings[stringId][languageId][0]=='\0')
    {
	    str = Ui_RecordStrings[stringId][0];
    }
	else
	{
	    str = Ui_RecordStrings[stringId][languageId];
	}

	return str;
}
//************************************************************************** 
//************************************************************************** 
  
