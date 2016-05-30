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

//#define		RS_232_DBG
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
#include "gpio.h"
#include "extern.h"


static 	UINT32  FileSize;
static	void *semApp;
UINT32	gPB_p;
UINT8 uipbDisp = PB_DISP_FOUR;
UINT8 orient;
UINT8 DeleImage_Flag = 1;
#if 1
UINT32 Menup[][4]=
{
//	{picture,picture,picture,sizeof(picture)}, //0
#if 0	/* Paul@2005/12/02-12:00 mask */	
	{menuback,0,0,sizeof(menuback)},  // 1

	{Icon_ope,0,0,sizeof(Icon_ope)},   	//选中删除 2
	{Icon_rec,0,0,sizeof(Icon_rec)}, 		//选中保护3
	{Icon_pro,0,0,sizeof(Icon_pro)},		//选中取消保护 4 
	{Icon_dpr,0,0,sizeof(Icon_dpr)},	//选中打印5

	{Icon_pri,0,0,sizeof(Icon_pri)},	  //选中DPOF  6
	{Icon_dpo,0,0,sizeof(Icon_dpo)},      //选中自动播放 7
	{Icon_aut,0,0,sizeof(Icon_aut)},     //选中开机8
	{Icon_ope,0,0,sizeof(Icon_ope)},
	{Icon_rec,0,0,sizeof(Icon_rec)},
		
	{Icon_pro,0,0,sizeof(Icon_pro)},      // 到这里结束
#endif	
};
#endif

enum
{
		Menu_sp= 0,
		Photo_sp ,
		FileError_sp, // tzw add for bug 88 060217	
		PhotoNo_sp ,
		MAX_IDP
			
};	

//#define LANGUAGES 3 与269不同是图片
UINT8 *UiStrings_P[MAX_IDP][LANGUAGES] = 
{
	{
	"Menu",
	"菜单",
	"垫虫",
	"Menú",
	"Меню",
	"Menü",
	"menu",
	"Menu",
	"Menu",
	"皋春",
	},
	{"Photo",
	"图片",
	"瓜",
	"Imagen",
	"Меню",
	"Bild",
	"image",
	"Foto",
	"Foto",
	"荤柳",
	},
	{"File error !",
	"文件错误!"	,
	"ゅン岿粇!",
	"Error de archivo",
	"Ошибка файла",
	"Datei Fehler",
	"Erreur de dossier",
	"Doc.errore",
	"Erro de ducomento",
	"颇老 绢矾",
	},
	{"No Image!",
	"没有图片!",
	"⊿Τ瓜!",
	"Sin imagen",
	"Нет изображения",
	"Kein Bild",
	"pas de image",
	"No Immagine",
	"N\xe3o há imagens",
	"荤柳 绝澜",
	},
};


UINT8* GetString_P(UINT16 stringId)
{
    UINT8* str;
    UINT32 languageId = sPara.language;

    if (languageId >= LANGUAGES)
    {
		languageId = LanguageEnglish;
    }

    if(stringId >= MAX_IDP) 
    {
		return 0;
    }

    if(UiStrings_P[stringId][languageId][0]=='\0')
    {
	    str = UiStrings_P[stringId][0];
    }
	else
	{
	    str = UiStrings_P[stringId][languageId];
	}

	return str;
}

/* menu  end  string */
#if 0
#define	MP3_MENU_X	64
#define	MP3_MENU_Y	32


#define MP3_BAR_X			(MP3_MENU_X-4)
#define MP3_BAR_Y			(MP3_MENU_Y + 48 - 4)
#define MP3_BAR_WIDTH       	196
#define MP3_BAR_HEIGHT     	 24

#define MP3_TITLE_OFF_X	MP3_MENU_X+60
#define MP3_TITLE_OFF_Y	MP3_MENU_Y+2


#define MP3_TEXT_OFF_X      (MP3_MENU_X + 10)
#define MP3_TEXT_OFF_Y      (MP3_MENU_Y + 48 )
#define MP3_TEXT_HEIGHT     24

#define MP3_ICON_X      		24 
#define MP3_ICON_Y			196
#define MP3_ICON_OFF_X		76

#endif
#if 1
extern UINT8  TVFlag;
UINT8	ui8PhMenuIdx = 0 ;
UINT8     ui8SubPhMenuIdx= 1;

#define   onePhoto   1
#define   numPhoto  0
#define   backPhoto  2 
#define  C_PHOTO_MENUN 7

#define   TitlTool_X    62 +2
#define   TitlTool_Y   34 -2

#define   SubTool_X1   84
#define   SubTool_Y1  68

#define   SubTool_X2   84
#define   SubTool_Y2   92

//#define ICON_X      		24 
//#define ICON_Y			192
//#define ICON_OFF_X		76


#define   TitlColor   0xd0
#define   SubColor  0xf0
#define   SubSelCl  0xde

#define   DrowColor 0xe
#define   DrowWithe 120
#define   DrowHigh  24 

#define	xDCF_ATTR_PROTECT		xDCF_ATTR_READONLY |fileattr.attribute
#define	xDCF_ATTR_DISPROTECT		(~xDCF_ATTR_READONLY) &fileattr.attribute

#if 0
UINT32 MenuT[C_PHOTO_MENUN]=
{
	2,2,2,2,1,1,1
};
UINT32 MenuSubX[3]=
{
	SubTool_X1,SubTool_X1,SubTool_X2
};
UINT32 MenuSubY[3]=
{
	SubTool_Y1,SubTool_Y1,SubTool_Y2
};

UINT32 IconX[4] =
{
	ICON_X,ICON_X+ICON_OFF_X,ICON_X+2*ICON_OFF_X,ICON_X+3*ICON_OFF_X	
};
UINT32 IconY[1] =
{
	ICON_Y
};

/*************MNUE*******************/
typedef void (*PMENUPROC)(UINT32 key);

void IconDisp(UINT8 Idx )
{
  	UINT8 i;
	for (i = 0 ; i<4 ; i++) 	
	{
		pbImagePaste(Menup[Idx + i ][0],  Menup[Idx +i][3], IconX[i],IconY[0]); 

	}

}

void menuDeleInit()
{
	
		osdStrDisp(TitlTool_X, TitlTool_Y, UserFont10x20,TitlColor, GetString_P( Dele_sp));
		osdStrDisp(SubTool_X1, SubTool_Y1, UserFont10x20,SubColor, GetString_P( One_sp));
		osdStrDisp(SubTool_X2, SubTool_Y2, UserFont10x20,SubColor, GetString_P( All_sp));
	
		osdBarDraw(MenuSubX[ui8SubPhMenuIdx],MenuSubY[ui8SubPhMenuIdx]-2, DrowWithe,DrowHigh, DrowColor);
		osdStrDisp(MenuSubX[ui8SubPhMenuIdx],MenuSubY[ui8SubPhMenuIdx], UserFont10x20,SubSelCl, GetString_P( One_sp+ui8SubPhMenuIdx-1));
		IconDisp(2);

}
void	menuProtInit()
{
	
		osdStrDisp(TitlTool_X, TitlTool_Y, UserFont10x20,TitlColor, GetString_P( Prot_sp));
		osdStrDisp(SubTool_X1, SubTool_Y1, UserFont10x20,SubColor, GetString_P( One_sp));
		osdStrDisp(SubTool_X2, SubTool_Y2, UserFont10x20,SubColor, GetString_P( All_sp));
		osdBarDraw(MenuSubX[ui8SubPhMenuIdx],MenuSubY[ui8SubPhMenuIdx]-2, DrowWithe,DrowHigh, DrowColor);
		osdStrDisp(MenuSubX[ui8SubPhMenuIdx],MenuSubY[ui8SubPhMenuIdx], UserFont10x20,SubSelCl, GetString_P( One_sp+ui8SubPhMenuIdx-1));
		IconDisp(3);

}
void	menuCancInit()
{

		osdStrDisp(TitlTool_X, TitlTool_Y, UserFont10x20,TitlColor, GetString_P( Canc_sp));
		osdStrDisp(SubTool_X1, SubTool_Y1, UserFont10x20,SubColor, GetString_P( One_sp));
		osdStrDisp(SubTool_X2, SubTool_Y2, UserFont10x20,SubColor, GetString_P( All_sp));

		osdBarDraw(MenuSubX[ui8SubPhMenuIdx],MenuSubY[ui8SubPhMenuIdx]-2, DrowWithe,DrowHigh, DrowColor);
		osdStrDisp(MenuSubX[ui8SubPhMenuIdx],MenuSubY[ui8SubPhMenuIdx], UserFont10x20,SubSelCl, GetString_P( One_sp+ui8SubPhMenuIdx-1));
		IconDisp(4);

}
void	menuPrinInit()
{
	
		osdStrDisp(TitlTool_X, TitlTool_Y, UserFont10x20,TitlColor, GetString_P( Prin_sp));
		osdStrDisp(SubTool_X1, SubTool_Y1, UserFont10x20,SubColor, GetString_P( One_sp));
		osdStrDisp(SubTool_X2, SubTool_Y2, UserFont10x20,SubColor, GetString_P( All_sp));

		osdBarDraw(MenuSubX[ui8SubPhMenuIdx],MenuSubY[ui8SubPhMenuIdx]-2, DrowWithe,DrowHigh, DrowColor);
		osdStrDisp(MenuSubX[ui8SubPhMenuIdx],MenuSubY[ui8SubPhMenuIdx], UserFont10x20,SubSelCl, GetString_P( One_sp+ui8SubPhMenuIdx-1));
		IconDisp(5);

}
void	menuDpofInit()
{

		osdBarDraw( SubTool_X1,SubTool_Y1-2, DrowWithe,DrowHigh, DrowColor);
		osdStrDisp(TitlTool_X, TitlTool_Y, UserFont10x20,TitlColor, GetString_P( Dpof_sp));
		osdStrDisp(SubTool_X1, SubTool_Y1, UserFont10x20,SubSelCl, GetString_P(Dpof_sp));
		IconDisp(6);
}
void	menuAutoInit()
{	
	
		osdBarDraw( SubTool_X1,SubTool_Y1-2, DrowWithe,DrowHigh, DrowColor);
		osdStrDisp(TitlTool_X, TitlTool_Y, UserFont10x20,TitlColor, GetString_P( Auto_sp));
		osdStrDisp(SubTool_X1, SubTool_Y1, UserFont10x20,SubSelCl, GetString_P( Auto_sp));
		IconDisp(7);
}
void	menuOpenInit()
{
	
		osdBarDraw( SubTool_X1,SubTool_Y1-2, DrowWithe,DrowHigh, DrowColor);
		osdStrDisp(TitlTool_X, TitlTool_Y, UserFont10x20,TitlColor, GetString_P( Open_sp));
		osdStrDisp(SubTool_X1, SubTool_Y1, UserFont10x20,SubSelCl, GetString_P( Open_sp));
		IconDisp(8);
}

void	menuDeleProc()
{
	
	
		if( ui8SubPhMenuIdx==1  )
		{				
			UINT32	err ;
			if((xdcfCurFileByPosSet(fileIdx)) != 0)
			{
				#ifdef	OSD_DEBUG
				printf("\nErase	error1...(idx=%d)",	fileIdx);
				#endif
			}
			else if (xdcfCurFileAttrGet(&fileattr)==0)
			{
				
				xdcfOneFileErase(fileattr.name);
				err	= xdcfFileCountGet(&fileCnt);
				if(!fileCnt) 
				{
					xdcfDirChange("..");
					xdcfDirCountGet(&fileCnt);
					xdcfCurDirPosGet(&fileCnt);
					xdcfCurDirAttrGet(&dirattr);
				}
				else 	if(fileIdx>fileCnt) 
					fileIdx = fileCnt ;
			}
		}
		else if( ui8SubPhMenuIdx==2  )
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
					xdcfDirCountGet(&fileCnt);
					xdcfCurDirPosGet(&fileCnt);
					xdcfCurDirAttrGet(&dirattr);				
				}
				else if(fileIdx>fileCnt) 
					fileIdx = fileCnt ;
				xdcfFileCountGet(&fileCnt);
			}


		}
}

void	menuProtProc()
{
		
				
		if( ui8SubPhMenuIdx==1  )
		{				
			xdcfCurFileByPosSet(fileIdx) ;
			xdcfCurFileAttrGet(&fileattr) ;
			xdcfCurFileAttrSet(xDCF_ATTR_PROTECT) ;
		}
		else if( ui8SubPhMenuIdx==2  )
		{
			UINT32	i;
			xdcfFileCountGet(&fileCnt);
			for(i=1;i<=fileCnt;i++)
			{
				xdcfCurFileByPosSet(i) ;
				xdcfCurFileAttrGet(&fileattr) ;
				xdcfCurFileAttrSet(xDCF_ATTR_PROTECT) ;
			}
		}

}
void	menuCancProc()
{
		
	if( ui8SubPhMenuIdx==1  )
	{		
		xdcfCurFileByPosSet(fileIdx) ;
		xdcfCurFileAttrGet(&fileattr) ;
		xdcfCurFileAttrSet(xDCF_ATTR_DISPROTECT) ;
	}	
	else if( ui8SubPhMenuIdx==2  )
	{
		UINT32	i;
		xdcfFileCountGet(&fileCnt);
		for(i=1;i<=fileCnt;i++)
		{
			xdcfCurFileByPosSet(i) ;
			xdcfCurFileAttrGet(&fileattr) ;
			xdcfCurFileAttrSet(xDCF_ATTR_DISPROTECT) ;
		}
	}

}
void	menuPrinProc()
{
	if( ui8SubPhMenuIdx==1  )
	{		

	}	
	else if( ui8SubPhMenuIdx==2  )
	{

	}

}
void	menuDpoProc()
{

}
void	menuAutoProc()
{

	UINT32 err;
	UINT32 *msg;
	UINT32   dispW,dispH;
	UINT32  ui32_Key;
	xdcfAttrElm_t fileattr1;
	UINT8 fileIdx1=1;
	UINT32 * pos1;
	UINT8  fileName1[8]="00";
	UINT32 storage1;
	UINT32 starttime =80 ;
	
	sysgMemDispAddrGet(&gPB_p);
	hwDispPvEn(0);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwDispImgSizeSet(dispW, dispH);
	hwPlayBufAddrSet(gPB_p);
	hwFrmCnvModeSet(0);   				
	hwCammodeSet(5);      				
	hwCdspDataSource(1);  
	hwDispPvEn(1);
	osdClearScreen(0x00);

	if( !fileCnt )
	{
		return;
	}	


	fileIdx1 = fileIdx;
	if (fileIdx1 < fileCnt)
		fileIdx1 ++;
	else
		fileIdx = 1;
	while( fileIdx1 != fileIdx)
	{	
		xdcfCurFileByPosSet(fileIdx1);
		err =xdcfCurFilePosGet(&pos1);

		if ( err == SUCCESS)
			pbImageGet(0, pos1, gPB_p, dispW, dispH, 0, 0x800000); 

		if (fileIdx1 < fileCnt)
			fileIdx1 ++;
		else
		{
			fileIdx1 = 1;
		}
		msg = osQuePend(uiKeyMsgQ,starttime,&err);
		ui32_Key = *msg;
		if( ui32_Key== UI_KEY_FUNC_MENU )
		{
			printf("*msg = %08x\n",*msg);
			fileIdx = fileIdx1;
			break ;
		}
	}
	osTimeDly(50);


}
void	menuOpenProc()
{

		UINT32	err = SUCCESS;	
		UINT32	filetype;	
		UINT8*	pbuf;
		UINT32	dispW, dispH;
		UINT32	dispAddr;
		pbPictureShow(onePhoto ,fileIdx );
		hwWait(0,1000);

		 sysgMemDispAddrGet(&dispAddr);

		pbuf = (UINT8 *)((0xAC000000 | (UINT32)dispAddr*2));


		xdcfFileTypeGet(fileIdx, &fileType);
		
		UINT32 page;

		 sPara.uiPatternFlag = PatternCurrent;
		nandWriteParameters();
		UINT32 stringlen;  

		stringlen = strlen(GetString(Set_StartP1)); 
		osdStrDisp(160-stringlen*5, 100, UserFont10x20, 0x02, GetString(Set_StartP1));
		stringlen = strlen(GetString(Set_StartP2)); 
		osdStrDisp(160-stringlen*5, 120, UserFont10x20, 0x02, GetString(Set_StartP2));
		hwWait(0,1000);										

		hwDispPreviewSizeGet(&dispW,&dispH);
			
		pbImageGet(0, fileIdx,dispAddr, dispW, dispH, 0, 0x800000); //0x800000 is bg color 


		if (dispW*dispH*2%512 == 0)
		page = dispW*dispH*2/512;
		else
		page = (dispW*dispH*2/512)+1;

		nandRsvBlkWrite(SECTOR_OF_START_PIC, page, pbuf);

		

		osdBarDraw(0, 100, 479, 50, 0x00);//for bug 20
		hwWait(0,100);		


}

PMENUPROC osdMenuPhotoInit[C_PHOTO_MENUN] = 
{
	menuDeleInit,
	menuProtInit,
	menuCancInit,
	menuPrinInit,
	menuDpofInit,
	menuAutoInit,
	menuOpenInit

};
PMENUPROC osdMenuPhotoProc[C_PHOTO_MENUN] = 
{
	menuDeleProc,
	menuProtProc,
	menuCancProc,
	menuPrinProc,
	menuDpoProc,
	menuAutoProc,
	menuOpenProc
};
void	osdPhotoMenuInit(UINT32 key)
{
	isMenuOn = 1;	
	ui8PhMenuIdx = 0 ;
	ui8SubPhMenuIdx = 1 ;
	osdClearScreen(0);	
	pbImagePaste(Menup[1][0], Menup[1][3],Menup[1][1],Menup[1][2]);
	osdMenuPhotoInit[ui8PhMenuIdx](key);
}


void osdPhotoMenuKey(UINT32 key)
{


	osdClearScreen(0x00);
	switch(key)
	{
		case  UI_KEY_FUNC_MENU:
	
			ui8PhMenuIdx = 0;
			isMenuOn = 0;
			ui8SubPhMenuIdx= 1 ;
			pbPictureShow(backPhoto,1);
			pbPictureRefresh();
			break;
		case  UI_KEY_DIR_LEFT:
			if (ui8PhMenuIdx > 0)
			{
				ui8PhMenuIdx--;
			}
			else
			{
				ui8PhMenuIdx = C_PHOTO_MENUN-1;
			}
			break;
		case  UI_KEY_DIR_RIGHT:
		
			if (ui8PhMenuIdx < (C_PHOTO_MENUN-1))
			{
				ui8PhMenuIdx++;
			}
			else
			{
				ui8PhMenuIdx = 0;
			}	
			break;
		 case  UI_KEY_DIR_UP:
			if (ui8SubPhMenuIdx > 1)
			{
				ui8SubPhMenuIdx--;
			}
			break;
		case  UI_KEY_DIR_DOWN:
			if (ui8SubPhMenuIdx < MenuT[ui8PhMenuIdx])
			{
				ui8SubPhMenuIdx++;		
			}
			break;
			
		case  UI_KEY_FUNC_OK:
			printf("ui8SubPhMenuIdx= %d\n",ui8SubPhMenuIdx);
			osdMenuPhotoProc[ui8PhMenuIdx](key);
			osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);
			break;	
	}

	if(isMenuOn)
		osdMenuPhotoInit[ui8PhMenuIdx](key);
	return;

}
#endif

/************MNUE END ***********/
#endif
#if 1
/****************************************/
/* picture  record DC308  Start					*/
/*						*/
/***************************************/

void uiPhoto(void)
{
	UINT32	key = UI_KEY_MODE_PHOTO;	
//	SINT32	i;
//	UINT32	err = SUCCESS;	
//	UINT32	filetype;	
//	UINT8*	pbuf;
//	UINT32	dispW, dispH;
//	UINT32	dispAddr;

	file_filter = FILTER_PHOTO ;
	semApp = osEventFindByName("APP_SEM");
	uiKeyMsgQ = osEventFindByName("UI_KEY");

	/*ui8PhMenuIdx = 0;*/
	pbDispStart = 1;
	dirIdx = 0;
	fileIdx = 0;
	DeleImage_Flag= 0 ; // tzw060121 for Ico

	isMenuOn = 0;
	orient = 3;

	while ( ( uiState & UI_MODE_MASK ) == UI_MODE_PHOTO )
	{

		if (isMenuOn)
		{
			menuPBImageProc(key);
			if (ui32NextState != uiState)
			{
				break;
			}			
			
			if (isMenuOn == 0)
			{				
				key = UI_KEY_MODE_PLAYBACK;
				osdClearScreen(0);
				pbPictureShow(backPhoto,1); //tzw add
				DeleImage_Flag = 0 ; 
				pbPictureRefresh();//tzw add
				
			}
		}
	
		if (isMenuOn == 0)
		{			
			if (ui32NextState != uiState)
			{
				break;
			}
			
			switch(key)
			{
				case	UI_KEY_DIR_UP:
					orient = 0;

					if(fileIdx >1)
						fileIdx --;
					else
						fileIdx = fileCnt;
					pbPictureRefresh();
					break;

				case	UI_KEY_DIR_DOWN:
					orient = 0;
					if(fileIdx < fileCnt)
					{
						fileIdx ++;
					}
					else
					{
						fileIdx = 1;
					}
					pbPictureRefresh();
					break;

				case	UI_KEY_DIR_LEFT:
					if(uipbDisp == PB_DISP_ONE)
					{
						if (orient == 0)	orient = 3;							
						else
						orient --;
						printf("orient %d\n", orient);
						pbPictureRefresh();								
						
					}
					break;
				case	UI_KEY_DIR_RIGHT:
					if(uipbDisp == PB_DISP_ONE)
					{
						
						orient++;
						if (orient > 3)	
						{
							orient = 0;
						}
						printf("orient %d\n", orient);						
						pbPictureRefresh();							
					}
					break;
				case    UI_KEY_FUNC_A:
					if(uipbDisp == PB_DISP_ONE)
					{// 灯片
						menuAutoProc();
                                      pbPictureRefresh(); // fqdao_add for bug 06.5.8
					}
					break; 

				case  	UI_KEY_FUNC_B:
					if(uipbDisp == PB_DISP_FOUR)
					{
						osTaskSuspend(osTaskFindByName("AAA"));		
						menuPBImageInit(key);
					}
					break; 
				case	UI_KEY_FUNC_MENU:
					if(uipbDisp == PB_DISP_FOUR)
					{
					//	osTaskSuspend(osTaskFindByName("AAA"));		
					//	menuPBImageInit(key);
						menuReturn(UI_MAINMENU, 0);
					}
					else if(uipbDisp == PB_DISP_ONE)
					{
						osdLcdDisp(1);
						uipbDisp = PB_DISP_FOUR;
						DeleImage_Flag = 0 ; // tzw060121 for Ico

						pbInit();
						pbPictureShow(backPhoto,1);
						pbPictureRefresh();
					}
					break;
					
           		case	UI_KEY_FUNC_OK:
					if (fileCnt != 0)
					{
						if(uipbDisp == PB_DISP_FOUR)
						{
						       orient = 0;    // fqdao_add  for bug 
					 		uipbDisp = PB_DISP_ONE;
						}
						else if (uipbDisp == PB_DISP_ONE) 
						{
							// 设置为开机画面
							menuOpenProc() ;
							osTimeDly(10);
						}
					}
					pbPictureRefresh();
					break;
					
	//			case	UI_KEY_FUNC_DISP:
	//				break;					

				case	UI_KEY_MODE_PHOTO:
					/*osTaskSuspend(osTaskFindByName("HISTOGRAM"));*/
					osTaskSuspend(osTaskFindByName("AAA"));
					sysgMemDispAddrGet(&gPB_p);
					xdcfInit(imageDirNameStr, imageFileNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);												

					xdcfRootDirAdd(otherRootNameStr, 0);    
					xdcfCurRootDirSet(otherRootNameStr);
					hwWait(0,100);
//					xdcfCurDirAttrGet(&dirattr);
//
//					printf("1dirattr.name= %s  ",dirattr.name);
//					xdcfDirCountGet(&dirCnt);	
//					xdcfCurDirAttrGet(&dirattr);
					xdcfFilterSet(file_filter);
					xdcfFileCountGet(&fileCnt);

//					printf("2dirattr.name= %s  ",dirattr.name);					
					fileIdx  = 1;
					if(fileCnt == 0)
					{
						fileIdx  = 0;
					}					
					pbPictureShow(backPhoto,1);
					pbPictureRefresh();
					break;

				default:					
					break;
			}
		}			
		keyGet(&key);		
	}

	uiState = ui32NextState;
}

/**
**/
void pbDisPlayNoFilestr()
{
	UINT8 len;
	
	if(sPara.language==LanguageRussian)
	{	//tzw0918	
		osdStrDisp((PANEL_1_OSD_W-9*10)/2+80, (PANEL_1_OSD_H-20)/2, UserFont10x20, 0xd0, GetString_P(PhotoNo_sp));
	}
	else
	{
		/* Paul@2006/12/23 - 17:13 add start */
		len = strlen(GetString_P(PhotoNo_sp));
		osdStrDisp((PANEL_1_OSD_W-len*10)/2+80, (PANEL_1_OSD_H-20)/2-20, UserFont10x20, 0xd0, GetString_P(PhotoNo_sp));
		/* Paul@2006/12/23 - 17:13 add end */
	}	
}

void pbPictureRefresh()
{
	UINT32 err = SUCCESS;
	UINT32	osdW, osdH;	
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	osdFWBuffChange();
	osdClearScreen(0x00);
	hisIconDisp();
	hisTimeDisp();
	
	if(uipbDisp == PB_DISP_FOUR)
	{
		osdStrDisp(34, 4, UserFont10x20, 0xd0, GetString_P(Photo_sp));	
	}
	
	if (fileCnt == 0)
	{
		pbDisPlayNoFilestr();
		osdHWBuffChange();	
		osTimeDly(100);		
		menuReturn(UI_MAINMENU, 0);			
		return ;

	}

	if ((err = xdcfCurFileByPosSet(fileIdx)) != SUCCESS)
	{
		osdHWBuffChange();
		return;
	}
	err	= xdcfCurFileAttrGet(&fileattr);
	err	= xdcfFileTypeGet(fileIdx, &fileType);

	osdHWBuffChange();  // tzw add 060302
	if(uipbDisp == PB_DISP_ONE)
	{
		pbPictureShow(onePhoto, fileIdx);
	}
	else
	{
	//	osdIconDisp(1);
		pbPictureInfoGet(fileIdx);
   		pbPictureFileList(fileIdx);
		pbPictureShow(numPhoto, fileIdx );
				
	}
	
}

/**
**/
void pbPictureInfoGet(UINT32 fileIdx)
{
	UINT8  	str[25];
	UINT32	osdW, osdH;
	osdW = PANEL_1_OSD_W-160;
	osdH = PANEL_1_OSD_H;
	UINT32 err ;
	if ((err = xdcfCurFileByPosSet(fileIdx)) != SUCCESS)
	{
		return;
	}
	err	= xdcfCurFileAttrGet(&fileattr);
	err	= xdcfFileTypeGet(fileIdx, &fileType);
	#if 0
	if(fileattr.attribute&xDCF_ATTR_READONLY)
	{
		osdStrDisp(295, 212, IconLock, 0xf0,"\x1");
	}
	else
	{
		osdStrDisp(295, 212, IconLock, 0xd0,"  ");
	}	
	#endif
	sio_psprintf(str, "%04d.%02d.%02d ", fileattr.time.year,fileattr.time.mon,fileattr.time.day);

	osdStrDisp(178, 182, UserFont10x20, 0xf0, str);
//	sio_psprintf(str, "%02d:%02d:%02d", fileattr.time.hour, fileattr.time.min, fileattr.time.sec);
//	osdStrDisp(osdW, 90+120-4, UserFont10x20, 0xf0, str);
//	sio_psprintf(str, "[%04d/%04d]", fileIdx, fileCnt);
	//osdStrDisp(osdW, 120+120, UserFont10x20, 0x00, str);
}

/**
**/
#define FILELIST_MAXLINE  	5 
#define	MP4_FILELIST_X		52
#define	MP4_FILELIST_SX		46
#define	MP4_FILELIST_Y		54

#define	MP4_FILELIST_SIZE	32
UINT8 strPic[FILELIST_MAXLINE][13];

void pbPictureFileList(UINT32 filePos)
{

	UINT8 space[13];
	SINT32 i;
	UINT32 err;
	UINT32	y[FILELIST_MAXLINE] = {MP4_FILELIST_Y,MP4_FILELIST_Y+MP4_FILELIST_SIZE,
		MP4_FILELIST_Y+MP4_FILELIST_SIZE*2,
		MP4_FILELIST_Y+MP4_FILELIST_SIZE*3,MP4_FILELIST_Y+MP4_FILELIST_SIZE*4};

	UINT32	osdW, osdH;
	static UINT8 page_idx = 0;
	UINT32	dispW, dispH,pPBAddr;
//	UINT8 page_idx = 0;
	static UINT32 active_line = 0;

	xdcfAttrElm_t attr;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	//xdcfFileCountGet(&fileCnt);
	
	if(filePos == 0)
	{/*reset*/
		page_idx = 0;
		active_line = 0;
	}

	/*clear window*/
	memset(space, ' ', 12-4);
	space[12-4] = '\0';
	if((page_idx != (filePos+FILELIST_MAXLINE-1)/FILELIST_MAXLINE)||(DeleImage_Flag == 0 ))

//	if(page_idx != (filePos+FILELIST_MAXLINE-1)/FILELIST_MAXLINE)
	{
		
		page_idx = (filePos+FILELIST_MAXLINE-1)/FILELIST_MAXLINE;
		
		#if  1 // tzw060121 for Ico
	
		pbMainBack();
		osdStrDisp(34, 4, UserFont10x20, 0xd0, GetString_P( Photo_sp));	

		#endif // tzw060121 for Ico
		
		#if 1 // for bug 89 tzw add 060218
		if (DeleImage_Flag == 0)
		{
			DeleImage_Flag = 1 ;
		}	
		#endif
		for(i=0; i<FILELIST_MAXLINE; i++)
		{
			if ((page_idx-1)*FILELIST_MAXLINE + i < fileCnt)
			{
				#if 1 // tzw060121 for Ico
				pbDisplayMp4_Ico(14,24, y[i]+6);
				
//				pbImagePaste(Menup[1 ][0],  Menup[1][3], Menup[1][1],y[i]-4); // play ico
				#endif
			}	

			osdStrDisp(30, y[i], UserFont10x20, 0, &space[0]);/*clear*/

		}	
		for(i = (page_idx-1)*FILELIST_MAXLINE; i<(page_idx*FILELIST_MAXLINE); i++)
		{
			if(i < fileCnt)
			{
				xdcfCurFileByPosSet(i+1);
				err = xdcfCurFileAttrGet(&attr);
				/*strcat(attr.name, "        ");*/
				strncat(attr.name, &space[0], 12-strlen(attr.name));

				if(SUCCESS == err)
					strncpy(&strPic[i%FILELIST_MAXLINE][0], attr.name,13-4);
				else
					strncpy(&strPic[i%FILELIST_MAXLINE][0], &space[0], 13-4);
			}
			else
				strncpy(&strPic[i%FILELIST_MAXLINE][0], &space[0], 13-4);

			strPic[i%FILELIST_MAXLINE][12-4] = '\0';
			/*osdStrDisp(212, y[i%3], UserFont, 0x00, &str[i%3][0]);*/
		}
	}
	
 	if (filePos !=0)
	{
		/*active selected file*/
		for(i=0; i<FILELIST_MAXLINE; i++)
		{
			if ((page_idx-1)*FILELIST_MAXLINE + i < fileCnt)
			{
				xdcfCurFileByPosSet((page_idx-1)*FILELIST_MAXLINE + i+1);
				err = xdcfCurFileAttrGet(&fileattr);

				osdStrDisp(MP4_FILELIST_X, y[i]+4, UserFont10x20, 0xf0, &strPic[i][0]);
				pbDisplayCleFile_Ico(MP4_FILELIST_SX,y[i]);

				if(fileattr.attribute&xDCF_ATTR_READONLY)	//John @01/22/06
				{
					osdStrDisp(MP4_FILELIST_SX+86,y[i]+5, IconLock, 0xf0,"\x1");
				}
				else
				{
					osdStrDisp(MP4_FILELIST_SX+86, y[i]+5, IconLock, 0x0,"  ");	
				}	

			}	
			else
			{
			 	 osdStrDisp(MP4_FILELIST_X, y[i]+4, UserFont10x20, 0xf0, &strPic[i][0]);
			}	 
 			
		}
		xdcfCurFileByPosSet(filePos);
		err = xdcfCurFileAttrGet(&fileattr);

		active_line = (filePos-1)%FILELIST_MAXLINE;
	//	osdBarDraw( 32,y[active_line]-2, 100,24, 0xe);
		pbDisplaySelFile_Ico(MP4_FILELIST_SX,y[active_line]);
		osdStrDisp(MP4_FILELIST_X, y[active_line]+4, UserFont10x20, 0x10, &strPic[active_line][0]);

		if(fileattr.attribute&xDCF_ATTR_READONLY)	//John @01/22/06
		{
			osdStrDisp(MP4_FILELIST_SX+86,y[active_line]+5, IconLock, 0x10,"\x1");
		}
		else
		{
			osdStrDisp(MP4_FILELIST_SX+86, y[active_line]+5, IconLock, 0x0,"  ");	
		}	
	}
}

/**
**/
extern UINT32	gPB;
void pbThumb1(UINT32 offsetX, UINT32 offsetY, UINT32 w, UINT32 h)
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

	
	hwDispPreviewSizeGet(&dispW, &dispH);
//	hwPlayBufSizeSet(dispW,	dispH);
//	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/


	pbThumbW = w & 0xFF0;
	pbThumbH = h;
	pbThumbOffX = (offsetX);
	pbThumbOffY = (offsetY);

	tempIdx = fileIdx;
	pubFill((UINT8 *)(0xAC000000+pPBAddr*2), pbThumbW*2, pbThumbH,
			0, 0, pbThumbW*2, pbThumbH, 0x800000);
	/**fqdao_add for bug over 16k 06.7.10 start ***/
	if(FileSize<(1024+600)*1024)
	{
		err	= pbImageGet(0,	tempIdx, pPBAddr,	pbThumbW, pbThumbH, 0, 0);//wendy@050202 for fit image /* scale from primary image	*/
	}
	/**fqdao_add for bug over 16k 06.7.10 end  ***/
	hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
				gPB, dispW,	dispH,
				pbThumbOffX,
				pbThumbOffY, 0);

}

/**
**/
UINT32 pbImagePaste1(UINT32 imgAddr, UINT32 imgSize)
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
	printf("decompAddr= %x,pInfo= %x\n",decompAddr,pInfo);
	if(pInfo<0xff)
		return pInfo;
	else
		return err;
	
	
}

/**
**/

void pbpictureshowOne(UINT32 filePos)
{
	UINT32 err;
	UINT32 dispW, dispH;
	UINT32 ui32FileType;
	UINT8  str[24];
	xdcfAttrElm_t xFileAttr;
	xdcfTime_t xTime;

	osdStrDisp(0, 0, UserIcon16x20, 0x40, "\x04\x05");	/* Playback icon */										
	sio_psprintf(str, "%04d", filePos);
	osdStrDisp(0/*(PANEL_1_OSD_W-10*10)*/, 2/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);
	sio_psprintf(str, "\-%04d", fileCnt);
	osdStrDisp(40/*(PANEL_1_OSD_W-6*10)*/, 2/*(PANEL_1_OSD_H-20)*/, UserFont10x20, 0xd0, str);			
	
	if ((err = xdcfCurFileAttrGet(&xFileAttr)) != SUCCESS)
	{
		return ;
	}
	
	xTime = xFileAttr.time;

	#if 1
	// icon of protect file John@01/22/06     
	if (xFileAttr.attribute & xDCF_ATTR_READONLY)
	        osdStrDisp(1, 38, IconLock, 0x10,"\x1");      
	else
	   		osdStrDisp(1, 38, IconLock, 0x0," ");
	#endif
	osdBarDraw(6, (PANEL_1_OSD_H-20), PANEL_1_OSD_W, 20, 0x00);
	osdStrDisp(6, (PANEL_1_OSD_H-20), UserFont10x20, 0x30, xFileAttr.name);
	sio_psprintf(str, "%04d\x2F%02d\x2F%02d\x2F%02d\x2F%02d",
				xTime.year,
				xTime.mon,
				xTime.day,
				xTime.hour,
				xTime.min
				);
	
	osdStrDisp(10*14, (PANEL_1_OSD_H-20), UserFont10x20, 0x30, str);

	err	= xdcfFileTypeGet(filePos, &ui32FileType);	
	if (ui32FileType == xDCF_FILETYPE_JPG)
	{
		exifImageInfo_t *pInfo = 0;
		
		//pbImageGet(0, filePos, gPB, dispW, dispH, 0, 0x800000); /* 0x800000 is bg color */			
		pbImageDecode(0, filePos, &pInfo);
		sio_psprintf(str, "%4dx%4d", pInfo->w, pInfo->h);
		osdStrDisp(4, (PANEL_1_OSD_H-40), UserFont10x20, 0x30, str);		
	}
	else
	{
		printf("File error\n");
		
	}	

	
} 
void  pbShowErrerFile()
{
	uiDispBuffFill(0x0000);
	osdStrDisp(120, 100, UserFont10x20, 0xd0, GetString_P( FileError_sp));	
}

void pbPictureShow(UINT8 flag ,UINT32 filePos )
{		
//	UINT8	i, j;
	UINT32	err	= SUCCESS;
	UINT32	pPBAddr, phyOsdDBAddr;
	UINT8	*pImgBuf;	
	
	UINT32	w, osdFontWidth, osdFontHeight;
//	UINT32	tempIdx = 1;
	UINT32	dispW, dispH;
	UINT32	pbThumbW, pbThumbH, pbThumbOffX, pbThumbOffY;

	UINT32  FileHandl;
	UINT32  FileBuf;
	exifImageInfo_t *pInfo = 0;
      exifAttrDesc_t *fpattr;    
	/*osTaskResume(osTaskFindByName("HISTOGRAM"));*/
	#if 1


	pImgBuf		= (UINT8*) (((UINT32 )imgBuf) |	0xac000000);
	phyOsdDBAddr= (((UINT32)pImgBuf) & 0x00FFFFFF) >> 1;
	hwDateFontScalerSet(1);
	pbThumbBufferGet(&pPBAddr);
	hwDispPreviewSizeGet(&dispW, &dispH);

	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB_p);
	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/

//	pubFill((UINT8 *)(0xAC000000+gPB_p*2), dispW*2, dispH,
//		16, 32+16, dispW*2-32, dispH-66+3, 0x000000);/*eric@5/9/2003*/
	#endif
	if(flag == numPhoto||flag == onePhoto)
	{
	  	#if 1    // fqdao add for bug 88 06.4.25
		xdcfCurFileAttrGet(&fileattr);
 		FileHandl = vfsOpen(fileattr.name, 0, 0);
		printf("fileattr.name = %s\n ",fileattr.name);
		if (!FileHandl)
		{
			printf("fail00000\n");
			return;
		}
		sysgMemFreeAddrGet(APP_STILL_PB, &FileBuf);

		FileBuf =( FileBuf<<1)|0x8c000000;
		
		FileSize =vfsFileSizeGet(FileHandl);
		vfsRead(FileHandl, (UINT8*)FileBuf, FileSize);
		vfsClose(FileHandl);
		/*
		if(err = pbImagePaste1(FileBuf,FileSize)!= 0 )
		{
			printf("err file= %d\n",err);
			return err;
		}
		*/
		#endif	
		if(flag == numPhoto)
		{
			pbThumbW =  120 ; // 160-40;
			pbThumbH =  84 ; //120-36;
			pbThumbOffX =150+20+2 ;
			pbThumbOffY = 44+14+14;
             
			#if  1   // fqdao add for bug 88 06.4.25
			if(err = pbImagePaste1(FileBuf,FileSize)!= 0 )
			{
				pbThumbW =  120-8 ;  
				pubFill((UINT8 *)(0xAC000000+pPBAddr*2), pbThumbW*2, pbThumbH,
				0, 0, pbThumbW*2, pbThumbH, 0x000000);
				printf("0000000001 filePos = %d,dispw= %d,dispH= %d\n",filePos,dispW,dispH);

				hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
				gPB, dispW,	dispH,
				pbThumbOffX,
				pbThumbOffY, 0);
				
				osdStrDisp(172, 90, UserFont10x20, 0xd0, GetString_P( FileError_sp));	
				return ;
			}	
			#else  	  // tzw add for bug 88 060217
			pubFill((UINT8 *)(0xAC000000+pPBAddr*2), pbThumbW*2, pbThumbH,
				0, 0, pbThumbW*2, pbThumbH, 0x000000);
			printf("0000000001 filePos = %d,dispw= %d,dispH= %d\n",filePos,dispW,dispH);

			err	= pbImageGet(0,	filePos, pPBAddr,	pbThumbW, pbThumbH, 0, 0); 
			printf("err = %x\n",err);
			if(err != SUCCESS)	
				//return  ;
			   //if(err==1)
			{
				hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
				gPB, dispW,	dispH,
				pbThumbOffX,
				pbThumbOffY, 0);
				
				osdStrDisp(172, 90, UserFont10x20, 0xd0, GetString_P( FileError_sp));	
				return ;
			}	
			#endif	
			
			//	hwWait(0,20);
			//	hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
			//	gPB_p, dispW,dispH,pbThumbOffX,pbThumbOffY, 0);

				hwWait(0,20);
			// ??  图片出现品字形处理
				pbThumb1( pbThumbOffX, pbThumbOffY,pbThumbW, pbThumbH);
				hwWait(0,20);
			 

		}
		else if (flag == onePhoto)
		{

		#if 1  //for bug 22 tzw add 060215
			
			#if 1  // fqdao add for bug 88 06.4.25
			//if(err = pbImagePaste1(FileBuf,FileSize)!= 0 )
            if(err = exifMainDecode((UINT8 *)FileBuf, FileSize, &fpattr)!=SUCCESS)
			{
				pbShowErrerFile();
				return ;
			}
			 pbImageDecode(0, filePos, &pInfo);  
			 
			#else   // tzw add for bug 88 060217
			err = pbImageDecode(0, filePos, &pInfo);
			if(err != 0) 	
			{
				pbShowErrerFile();
				return ;
			}	
			#endif
			 
			UINT32 a[]={1, 6, 3, 8}; /* 6 - 90
										3 - 180
										8 - 270
										*/
			if (orient )
			{				
				pbFileRotate(0, filePos, gPB_p, dispW, dispH, 0, 0x000000, a[orient]); /* 0x800000 is bg color */
				//osdStrDisp(34, 4, UserFont10x20, 0xd0, GetString_P( Photo_sp));					
			}
			else
			{

				if( pInfo->h < 240 )
				{
			
					pbImageGet(0, filePos, gPB_p, dispW,dispH+8, 0, 0x000000); /* 0x800000 is bg color */
				}	
				else
				{
					pbImageGet(0, filePos, gPB_p, dispW, dispH, 0, 0x000000); /* 0x800000 is bg color */
				}
			}
			osTimeDly(1); //for bug 24  tzw add 060215
			pbpictureshowOne(filePos); // for bug 23 tzw add 060215


		#endif

			#if 0  // tzw add  060224
			/* flip photo David 01/03/2005 */
			UINT32 a[]={6, 3, 8};
			if (orient < 3)
			{				
				pbFileRotate(0, filePos, gPB_p, dispW, dispH, 0, 0x800000, a[orient]); /* 0x800000 is bg color */
				osdStrDisp(34, 4, UserFont10x20, 0xd0, GetString_P( Photo_sp));					
			}
			else
			{
				pbImageGet(0, filePos, gPB_p, dispW, dispH, 0, 0x800000); /* 0x800000 is bg color */								
			}
			#endif
			

				if((TVFlag==1) && (PANEL_TYPE_1 != getCurPanel()))
				{
				      gpioSet(GPIO_LCD_POWER, 1);
					hwWait(0,300);
					panelSelect(PANEL_TYPE_1);
					printf("tv out off");
					sPara.TvoutFlag=TVOutOff;
					TVFlag=0;
					hwDispPvSetRead(imgBuf);
	//				ChgPvSetting = 1;
				}
			
		}
	}	
	else if ( flag ==  backPhoto )
	{
		pbMainBack();
		//pbDramImgPlay(Menup[0][0], Menup[0][3], gPB_p, dispW, dispH);
	}
}

/****************************************/
/* picture  record DC308	End					*/
/*											*/
/***************************************/
#endif

