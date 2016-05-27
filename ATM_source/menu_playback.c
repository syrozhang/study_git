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
 *  Author: Woods Gu                                                    *
 *                                                                        *
 **************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "stdio.h"
#include "ui_main.h"
#include "gpio.h"
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
#include "hw_cpu.h"
#include "timer.h"
#include "xiaoao.h"
#include "dpof_event.h"
#include "menu_playback.h"
#include "card.h"
#include "dpof_api.h"
#include "uistrings.h"

extern UINT32 dpofFileRead(void);
extern UINT32 dpofFileWrite(void);
extern void	pbPageRefresh(UINT8);
extern void	pbShowM(UINT32, UINT32, UINT32, UINT32);
extern void	pbShowS(UINT32);
extern void    osdCustomMenuDestroy (UINT32);

extern UINT32	fileType;
extern UINT32 keyUnknow[];
//extern UINT32 fbflag;   // caoyong   05-08-23

/*Gu @2003.12.4 11:42static	void *semApp;*/

extern UINT32   pbAbortFlag;
extern UINT32	fileCnt, dirCnt, fileIdx, dirIdx;
extern UINT8	pbDispCol, pbDispRow;
extern UINT16	pbDispStart;
extern UINT32	pbDisp;
extern UINT32	pbZoomFactor;
extern UINT32	asfTime, asfProgress;

extern UINT32	gPB;


extern xdcfAttrElm_t fileattr;
extern xdcfAttrElm_t dirattr;
extern	OS_EVENT *uiKeyMsgQ;
/* 536@patch1.8.1@Joe@2003.11.5 15:27 add end   */

static UINT8 hideMenu=0;/* */
static UINT8 oldpath[20];
static UINT8 pbOrient=EXIF_ORIENT_ROTATE_0;
static UINT32 pbFileIdx=1;/*rotate use only*/

#define	ERR_SUCCESS 0
#define	ERR_READONLY 1
#define	ERR_MEMFULL 2
#define	ERR_DISKFULL 3
#define ERR_NOCARD	4
#define ERR_FILETYPE	5
#define ERR_SCALEUP	6

#define HIDEMENU_COPYONE 1
#define HIDEMENU_DPOF	2
#define HIDEMENU_ROTATE	3

typedef struct pbParam{
	UINT32 err;
	UINT32 w;
	UINT32 h;
	UINT32 fileSize;
} pbParam_t;

extern pbParam_t* pbCbGet(void);

void pbErrMsg(UINT8 errcode);
void pbDPOFSet(UINT32 index);

UINT32 pbFileRotate(
	UINT32 isThumb,
	UINT32 filepos,
	UINT32 scaleAddr,
	UINT32 outWidth,
	UINT32 outHeight,
	UINT32 scaleToFit,
	UINT32 bgColor,
	UINT8 orient
);

UINT32 readOnlyFileErase(UINT8 *pname)
{
	UINT32 r = 1;
	return r;
}


UINT8 menuDelImageSet (UINT32 key, UINT8* sel)
{
#if 0	
/*andrew_gu@2005/May/16, 20:11 (*/
	UINT32 storage;
/*andrew_gu@2005/May/16, 20:11 )*/

	if (key == UI_KEY_FUNC_MENU)
	{
		switch (*sel)
		{
			case 2:
			{
				/*andrew_gu@2005/May/16, 20:11 (*/				
				xdcfActiveDevIdGet(&storage);
				if((storage != DRIVE_NAND) && isCardProtect())
				{
					MsgShow(card_protect_s);
					hwWait(0, 1000);
					//printf("cardprotect!!\n");				
				}
				else
				{				
					if(fileCnt)
					{
						if((xdcfCurFileByPosSet(fileIdx)) != 0)
						{
							#ifdef	OSD_DEBUG
							printf("\nErase	error1...(idx=%d)",	fileIdx);
							#endif
						}
						else if (xdcfCurFileAttrGet(&fileattr)==0)
						{
							#ifdef	OSD_DEBUG
							printf("\n%s ",	fileattr.name);
							#endif
							/*Gu @2003.12.29 11:38 begin
							if((fileattr.attribute) & xDCF_ATTR_READONLY)
								pbErrMsg(ERR_READONLY);
							else*/
							{
								xdcfDelOnlyFileFuncSet(readOnlyFileErase);
								xdcfOneFileErase(fileattr.name);
								fileCnt--;
								if(fileIdx > fileCnt)
									fileIdx = fileCnt;
							}
							/*Gu @2003.12.29 11:38 end*/
						}
					}
				}
				/*andrew_gu@2005/May/16, 20:11 )*/
				break;
			}
			case 3:
			{
				/*andrew_gu@2005/May/16, 20:11 (*/				
				xdcfActiveDevIdGet(&storage);
				if((storage != DRIVE_NAND) && isCardProtect())
				{
				MsgShow(card_protect_s);
				hwWait(0, 1000);				
				}
				else
				{				
					xdcfDelOnlyFileFuncSet(readOnlyFileErase);
					xdcfAllFilesErase();
					xdcfDirChange("..");
					//Paul@2005/08/25
//					xdcfDirRemove(dirattr.name);
					fileCnt	= 0;
					fileIdx = 0;
					#ifdef	OSD_DEBUG
					printf("\nERASE	ALL... ");
					#endif
				}
				/*andrew_gu@2005/May/16, 20:11 )*/
				break;
			}
			case 1:
				break;
		}
	}
#endif
	return CALL_DEFAULT;
}

UINT8 menuSlideIntvl(UINT32 key, UINT8* index)
{
#if 0	
	if(key != UI_KEY_FUNC_MENU)
		return CALL_DEFAULT;
	switch(*index)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			sPara.Interval = (*index) - 1;
			break;
	}
#endif	
	return CALL_DEFAULT;
}

UINT8 menuSlideImg(UINT32 key, UINT8* index)
{
#if 0	
	switch(*index)
	{
		case 1:
			sPara.FolderAll = SlideImageFolder;
			break;
		case 2:
			sPara.FolderAll = SlideImageAll;
			break;
	}
#endif
	return CALL_DEFAULT;
}

UINT8 menuSlideRpt(UINT32 key, UINT8* index)
{
#if 0	
	switch(*index)
	{
		case 1:
			sPara.Repeat = SlideRepeatOn;
			break;
		case 2:
			sPara.Repeat = SlideRepeatOff;
			break;
	}
#endif
	return CALL_DEFAULT;
}

UINT8 menuSlideStart(UINT32 key, UINT8* index)
{
#if 0	
	UINT32 filecount, dircount, i, j;
	UINT32 filetype=-1;
	UINT32 err;
	UINT32 *msg;
	UINT32 intvl;
	UINT32 fbflag = 0xff;          //caoyong
       //   fbflag=0xff; 
    	osTaskSuspend(osTaskFindByName("HISTOGRAM"));
	/*osdCustomMenuDestroy (key);*/
	osdClearScreen(0x00);
	
	ui8ExitSlide = 0;

	switch(sPara.Interval)
	{
		case SlideInterval5sec:
			intvl = 5;
			break;
		case SlideInterval10sec:
			intvl = 10;
			break;
		case SlideInterval30sec:
			intvl = 30;
			break;
		case SlideInterval1min:
			intvl = 60;
			break;
		case SlideInterval3sec:
		default:
			intvl = 3;
			break;
	}
	do
	{
start: if(fbflag)
       { 
		    xdcfDirCountGet(&dircount);
	           j=dirIdx;
		    dircount++;
		    printf("Ly_debug goto j 1\n");
       }
       else
       {
		    j=1;
		    dircount=dirIdx+1;
		    printf("Ly_debug goto j  2\n");
       }
	   
	   for(; j<dircount; j++)
	   {
			xdcfCurDirByPosSet(j);
			xdcfFileCountGet(&filecount);
			printf("Ly_debug fbflag=%d, j=%d\n",fbflag,j);
            if(fbflag && (j==dirIdx))
            {
                 i=fileIdx;
				 filecount++;
				 printf("Ly_debug goto i  1\n");
		    }
			else if((fbflag==0) && (j==dirIdx))
		    { 
			  	 i=1;
			     filecount=fileIdx+1;
				 printf("Ly_debug goto i  2\n");
		    }
			else
			{
				   i=1;
				   filecount++;
				   printf("Ly_debug goto i  3\n");
			}
			for(; i<filecount; i++)
			{
				if (ui8ExitSlide)
				{	//Paul@2005/08/25
					printf("Exit Browse Image\n");
					return 0;
				}
				
				xdcfFileTypeGet(i, &filetype);
				if (filetype !=	xDCF_FILETYPE_JPG)
				{
					continue;
				}
				
				if ((err = xdcfCurFileByPosSet(i)) != SUCCESS)
				{
					printf(" #FILE_ERR j=%d,i=%d \n",j,i);
					continue;
				}
				
				err	= xdcfCurFileAttrGet(&fileattr);
				osdClearScreen(0x00);
				/*osdStrDisp(32, 0, UserFont, 0xc0, fileattr.name);*/
//shuai 2005/11/25				pbShowS(i);

				msg = osQuePend(uiKeyMsgQ, intvl*100, &err);
       /*              //switch(*msg)
				//start:
				if(*msg== UI_KEY_FUNC_UP)
					{
					buttonAudio(1);
					break;
					}
				if(*msg== UI_KEY_FUNC_DOWN)
					{
					buttonAudio(1);
					break;
					}
				 // end
      */				 
				if(*msg == UI_KEY_FUNC_MENU)	//Paul@2005/09/19
				{
				    buttonAudio(3);      
		 		    sPara.Repeat = SlideRepeatOff;
				    ui8ExitSlide = 0;
					return 0;
				}
				
			}
		}/* dir circle */
	   
		fbflag = !fbflag;
		if(!fbflag)
		{
			goto start;
		}
		
	}while(sPara.Repeat == SlideRepeatOn);
	ui8ExitSlide = 1;
	
	osdStrDisp(PANEL_1_OSD_W/2-16*5, PANEL_1_OSD_H/2, UserFont10x20, 0x00, GetString(Slide_Over));//Danny --05/05/23
	msg = osQuePend(uiKeyMsgQ, 0, &err);
	if(*msg == UI_KEY_FUNC_MENU)	//Paul@2005/09/19
	{
 		return 0;
	}
	
    osTaskResume(osTaskFindByName("HISTOGRAM"));
#endif	
	return 0;
}

UINT8 pbFolderRfsh(UINT32 pos)
{
#if 0	
	UINT8 str[25];
	UINT32 filecount;
	xdcfAttrElm_t folderattr;

	xdcfCurDirByPosSet(pos);
	xdcfCurDirAttrGet(&folderattr);
	xdcfFileCountGet(&filecount);
	osdStrDisp(104, 40, UserFont, 0xd7, "            ");   
	osdStrDisp(104, 40, UserFont, 0xd7, folderattr.name);

	sio_psprintf(str, "%04d", filecount);
	osdStrDisp(104, 104, UserFont, 0xd7, str);

	sio_psprintf(str, "%04d.%02d.%02d", folderattr.time.year, folderattr.time.mon, folderattr.time.day);
	osdStrDisp(104, 72, UserFont, 0xd7, str);

	sio_psprintf(str, "%04d/%04d", pos, dirCnt);
	osdStrDisp(148, 144, UserFont, 0xd7, str);
#endif	
	return 0;
}

UINT8 menuFolder(UINT32 key, UINT8* index)
{
#if 0	
	UINT8 okcancel = 0;/*0,cancel; 1, ok*/
	UINT8 flag = 1; /*1,on*/
	UINT32 pos=0;	/*folder index*/

	UINT32	osdW, osdH;
	UINT32	skey;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	xdcfCurDirPosGet(&pos);
	xdcfDirCountGet(&dirCnt);

	if(key != UI_KEY_FUNC_MENU)
		return CALL_DEFAULT;

		{
		osdClearScreen(0x07);

		osdBarDraw(0, 0, osdW, FOSDHeight, 0x07);
		osdBarDraw(0, osdH-40, osdW, FOSDHeight, 0x0f);

		osdStrDisp(0, 0, UserFont, 0x07, "Select Folder");
		osdStrDisp(240, 0, UserIcon, 0x07, "\x53");
		osdStrDisp(224, 8, UserIcon, 0x07, "\x54");
		osdFrameDrawX(0, FOSDHeight, osdW, osdH-FOSDHeight, 0x01, 8);
		osdStrDisp(16, 40, UserFont, 0xd7, "NAME:");
		osdStrDisp(16, 104, UserFont, 0xd7, "FILE:");
		osdStrDisp(16, 72, UserFont, 0xd7, "DATE:");

		osdStrDisp(20, osdH-40, UserFont, 0x0f, "  OK  ");
		osdStrDisp(126, osdH-40, UserFont, 0x7d, "CANCEL");
		osdStrDisp(240, osdH-40, UserIcon, 0x0f, "\x55");
		osdStrDisp(256, osdH-40, UserIcon, 0x0f, "\x56");

		pbFolderRfsh(pos);
		}
		while(flag)
		{
			keyGet(&skey);

			if (skey & UI_KEY_MODE_MASK)
			{
				flag = 0;
				okcancel = 0;
				osQuePost(uiKeyMsgQ, &keyRotary[(uiState & UI_MODE_MASK) >> 24]);
			}
			else
			{
				switch(skey)
				{
					case	UI_KEY_FUNC_MENU:
						flag = 0;
						break;
					case	UI_KEY_DIR_DOWN:
						if(pos <dirCnt)
						{
						 //     	buttonAudio(1);
							pos++;
							pbFolderRfsh(pos);
						
						}
						break;
					case	UI_KEY_DIR_UP:
						if(pos > 1)
						{
						   //    buttonAudio(1);
							pos--;
							pbFolderRfsh(pos);
							
						}
						break;
					case	UI_KEY_DIR_LEFT:
						osdStrDisp(20, osdH-40, UserFont, 0x7d, "  OK  ");
						osdStrDisp(126, osdH-40, UserFont, 0x0f, "CANCEL");
						okcancel = 1;
						break;
					case	UI_KEY_DIR_RIGHT:
						osdStrDisp(20, osdH-40, UserFont, 0x0f, "  OK  ");
						osdStrDisp(126, osdH-40, UserFont, 0x7d, "CANCEL");
						okcancel = 0;
						break;
				}
			}
		}/*end while*/

	osdClearScreen(0x00);
	osdCustomMenuDestroy(UI_KEY_FUNC_MENU);
	if(okcancel)/*OK*/
		dirIdx = pos;
	else
		xdcfCurDirByPosSet(dirIdx);/*restore old folder*/

	/* 536@patch1.8.2@Joe@2003.12.23 10:02 modify begin */
	/*
	return CALL_DEFAULT;
	*/
#endif

	return IGNORE_DEFAULT;
	/* 536@patch1.8.2@Joe@2003.12.23 10:03 modify end   */
}

UINT8 menuResize(UINT32 key, UINT8* index)
{
#if 0	
	UINT32 width, height;
	UINT32 saveflag = 1;
	UINT32 filetype=-1;
	xdcfAttrElm_t attr;
	exifImageInfo_t *pInfo=0;

	if(key != UI_KEY_FUNC_MENU)
		return CALL_DEFAULT;

	switch(*index)
	{
		case 1:
			width = 1600;
			height = 1200;
			break;
		case 2:
			width = 1280;
			height = 1024;
			break;
		case 3:
			width = 1024;
			height = 800;
			break;
		case 4:
			width = 800;
			height = 600;
			break;
		case 5:
			width = 640;
			height = 480;
			break;
		default:
			return CALL_DEFAULT;
	}

	xdcfFileTypeGet(fileIdx, &filetype);
	if (filetype ==	xDCF_FILETYPE_JPG)
	{
		xdcfCurFileAttrGet(&attr);
		if((attr.attribute) & xDCF_ATTR_READONLY)
			pbErrMsg(ERR_READONLY);
		else
		{
			if(pbImageDecode(0, fileIdx, &pInfo) == SUCCESS)
			{
				printf("pInfo.w=%d\n",pInfo->w);
				printf("pInfo.h=%d\n",pInfo->h);
				if((pInfo->w) <= width)
				{
					pbErrMsg(ERR_SCALEUP);
					return CALL_DEFAULT;
				}
			}
			if(SUCCESS == pbJPGResize(fileIdx, width, height, saveflag))
			{
				/*xdcfOneFileErase(attr.name);*/
			}
		}

	}
	else
		pbErrMsg(ERR_FILETYPE);
#endif	
	return CALL_DEFAULT;
}

UINT8 menuRotate(UINT32 key, UINT8* index)
{
#if 0	
	UINT32	dispW, dispH;
	UINT32 filetype=-1;
	UINT32 err = SUCCESS;


	if((key != UI_KEY_FUNC_MENU)||(fileIdx==0))
		return CALL_DEFAULT;
	xdcfFileTypeGet(fileIdx, &fileType);
	if(fileType !=	xDCF_FILETYPE_JPG)
	{
		pbErrMsg(ERR_FILETYPE);
		return CALL_DEFAULT;
	}

	hideMenu = HIDEMENU_ROTATE;
	pbFileIdx = fileIdx;
	switch(*index)
	{
		case 2:
			pbOrient = EXIF_ORIENT_ROTATE_90;
			printf("\ntest90");
			break;
		case 3:
			pbOrient = EXIF_ORIENT_ROTATE_180;
			printf("\ntest180");
			break;
		case 4:
			pbOrient = EXIF_ORIENT_ROTATE_270;
			printf("\ntest270");
			break;

		default:
			pbOrient = EXIF_ORIENT_ROTATE_0;
			pbFileIdx = 0;
			hideMenu = 0;
			return CALL_DEFAULT;

	}

		fileIdx = pbFileIdx;
			/*Gu @2003.12.30 15:01err = xdcfCurFileByPosSet(fileIdx); begin*/
			xdcfFileTypeGet(fileIdx, &filetype);
			if (filetype ==	xDCF_FILETYPE_JPG)
			{
				err	= xdcfFileTypeGet(fileIdx, &fileType);
				hwDispPreviewSizeGet(&dispW, &dispH);

				hwPlayBufSizeSet(dispW,	dispH);
				hwPlayBufAddrSet(gPB);
				hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
				hwCammodeSet(5);	  /* play back mode	*/
				hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/

				pbFileRotate(0, fileIdx, gPB, dispW, dispH, 0, 0x800000, pbOrient); /* 0x800000 is bg color */
			}
			else
			{
				hideMenu = 0;
				pbErrMsg(ERR_FILETYPE);
			}
			pbOrient = EXIF_ORIENT_ROTATE_0;
			hideMenu = 0;
	/*pbDisp = PB_DISP_ONE;*/
#endif	
	return CALL_DEFAULT;
}

void pbProtect(UINT8 flag)
{
#if 0	
	UINT8 value;
	if(0 == xdcfCurFileAttrGet(&fileattr))
	{
		value = (UINT8)(fileattr.attribute);
		if(flag) /* protect */
			value |= xDCF_ATTR_READONLY;
		else
			value &= (~xDCF_ATTR_READONLY);
		xdcfCurFileAttrSet(value);
	}
#endif	
}

UINT8 menuProtect(UINT32 key, UINT8* index)
{
#if 0	
	if(key != UI_KEY_FUNC_MENU)
		return CALL_DEFAULT;

	if((1 == *index)||(2 == *index))
	{
		if(SUCCESS == xdcfCurFileByPosSet(fileIdx))
			pbProtect(2-*index);
	}
#endif	
	return CALL_DEFAULT;
}

UINT8 menuC2C(UINT32 key, UINT8* index)
{
#if 0	
	UINT32 card;
	UINT32 err;
	xdcfAttrElm_t attr;

	if(key != UI_KEY_FUNC_MENU)
		return CALL_DEFAULT;

	if(!isCardExist())
	{
		pbErrMsg(ERR_NOCARD);
		return CALL_DEFAULT;
	}

	if(1 == *index)
	{
		hideMenu = HIDEMENU_COPYONE;
		xdcfCurDirAttrGet(&attr);
		oldpath[0] = 0;
		strncat(oldpath, attr.name, 12);
		xdcfActiveDevIdSet(DRIVE_NAND);
		pbDisp = PB_DISP_NINE;
	}
	else if(2 == *index)
	{
			card = cardSelectionGet();
			xdcfActiveDevIdSet(DRIVE_NAND);
			err = xdcfCopyDisk(card);
			if(err != SUCCESS)/*err msg show*/
				pbErrMsg(ERR_DISKFULL);
			xdcfActiveDevIdSet(card);
	}
#endif
	return CALL_DEFAULT;
}

UINT8 menuDPOF(UINT32 key, UINT8* index)
{
#if 0	
	if(key != UI_KEY_FUNC_MENU)
		return CALL_DEFAULT;

	switch(*index)
	{
		case 1:
			pbDisp = PB_DISP_NINE;
			hideMenu = HIDEMENU_DPOF;
			break;
		case 2:
			pbDPOFSet(0);
			break;
	}
#endif
	return CALL_DEFAULT;
}

UINT8 isHideMenu(void)
{return hideMenu;}

void pbCopyAFile(UINT32 index)
{
#if 0	
	xdcfAttrElm_t attr;
	UINT32 err;
	UINT32 card;
	UINT8 name[60] = "D:\\DCIM\\";

	xdcfCurFileByPosSet(index);
	xdcfCurFileAttrGet(&attr);

	if(isCardExist())
	{
		card = cardSelectionGet()-2;
		name[0] = 'D' + card;
	}
	else
		return;
	strncat(name, oldpath, 12);
	strcat(name, "\\");
	strncat(name, attr.name, 12);
	printf("copy to %s\n", name);
	err = xdcfCopyFile(attr.name, name);
	if(err != SUCCESS)/*err msg show*/
				pbErrMsg(ERR_DISKFULL);
#endif	
}

void pbDPOFSetUI(UINT32 num, UINT8 initFlag)
{
#if 0	
	UINT32	number;
	UINT32	osdW, osdH;
	UINT32	x, y;
	UINT8		str[10];

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;

	number = num;

	x = osdW/2-FOSDWidth*3;
	y = osdH/2-FOSDHeight;

	sio_psprintf(str, " %04d", number);
	osdStrDisp(x, y, UserFont, 0xd7, "      ");
	osdStrDisp(x, y, UserFont, 0xd7, str);
	osdFrameDrawX(x, y, FOSDWidth*6, FOSDHeight, 0x01, 4);
#endif	
}

void pbDPOFSet(UINT32 index)
{
#if 0	
	UINT8 setFlag = 1;
	UINT16 num = 0;
	UINT32 skey;
	dpofStdParam_t dpf_t;

	if(dpofJobQuery(DPOF_PRINT_MODE_STD)==0)
	{
		dpofJobAttrGet(DPOF_PRINT_MODE_STD, &dpf_t);
		printf(dpf_t.filename);
		num = dpf_t.numPrints;
	}

	pbDPOFSetUI(num, 1);
	while(setFlag)
	{
		keyGet(&skey);
		switch(skey)
		{
			case	UI_KEY_DIR_UP:
				num++;
				break;
			case 	UI_KEY_DIR_DOWN:
				if(num > 0)
					num--;
				break;
			case	UI_KEY_FUNC_DISP:
				if(index)
				{
					dpofCurFileSet(&num);
					dpofFileWrite();
				}
				else
				{
					dpofAllFileSet(&num);
					dpofFileWrite();
				}
					setFlag = 0;
				break;
			case	UI_KEY_FUNC_MENU:
				setFlag = 0;
				break;
		}
		if(uiState != UI_MODE_PLAYBACK)
		{
			setFlag = 0;
			osQuePost(uiKeyMsgQ, &keyRotary[(uiState & UI_MODE_MASK) >> 24]);
		}
		pbDPOFSetUI(num, 0);
	}
#endif	
}

void hideMenuFunc(UINT32 key)
{
#if 0	
	UINT32 card;
	UINT32 err = SUCCESS;
	UINT32	dispW, dispH;
	UINT32 filetype=-1;

#if DBG_UI
	printf("hideMenufunc() In\n");
#endif
	if(hideMenu == 3)
	{
		if((key == UI_KEY_DIR_UP)||(key == UI_KEY_DIR_DOWN))
		{
			pbOrient = EXIF_ORIENT_ROTATE_0;
			hideMenu = 0;
			/*printf("hideMenu close.\n");*/
			return;
		}
	}

	if((uiState != UI_MODE_PLAYBACK) || (key == UI_KEY_FUNC_MENU))
	{
		if(isCardExist())
		{
			card = cardSelectionGet();
			xdcfActiveDevIdSet(card);
			xdcfCurDirByPosSet(dirIdx);
		}
		if(key == UI_KEY_FUNC_MENU)
		{
			xdcfFilterSet(xDCF_FILTER_JPG | xDCF_FILTER_ASF | xDCF_FILTER_WAV);
			osQuePost(uiKeyMsgQ, &keyRotary[(uiState & UI_MODE_MASK) >> 24]);
		}
		pbOrient = EXIF_ORIENT_ROTATE_0;
		hideMenu = 0;
		/*printf("hideMenu close.\n");*/
		return;
	}

	if(key == UI_KEY_MODE_PLAYBACK)/*init*/
	{
		printf("PB hidemenu Init.\n");
		if(hideMenu == 2)
		{
			xdcfFilterSet(xDCF_FILTER_JPG);
			xdcfCurDirByPosSet(dirIdx);
			xdcfFileCountGet(&fileCnt);
//shuai 2005/11/25			pbPageRefresh(1);
		}
		else if(hideMenu == 3)/*rotate*/
		{
			fileIdx = pbFileIdx;
			/*Gu @2003.12.30 15:01err = xdcfCurFileByPosSet(fileIdx); begin*/
			xdcfFileTypeGet(fileIdx, &filetype);
			if (filetype ==	xDCF_FILETYPE_JPG)
			{
				err	= xdcfFileTypeGet(fileIdx, &fileType);
				hwDispPreviewSizeGet(&dispW, &dispH);

				hwPlayBufSizeSet(dispW,	dispH);
				hwPlayBufAddrSet(gPB);
				hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
				hwCammodeSet(5);	  /* play back mode	*/
				hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/

				pbFileRotate(0, fileIdx, gPB, dispW, dispH, 0, 0x800000, pbOrient); /* 0x800000 is bg color */

			}
			else
			{
				hideMenu = 0;
				pbErrMsg(ERR_FILETYPE);
			}
		}
		return;
	}
	if(key != UI_KEY_FUNC_DISP)
		return;

	switch(hideMenu)
	{
		case 1: /* copy one to card */
			pbCopyAFile(fileIdx);
			break;
		case 2: /* dpof set*/
			xdcfCurFileByPosSet(fileIdx);
			pbDPOFSet(fileIdx);
			break;
		case 3:
			switch(pbOrient)
			{
				case EXIF_ORIENT_ROTATE_90:
					pbOrient = ORIENT_ROTATE_90;
					break;
				case EXIF_ORIENT_ROTATE_270:
					pbOrient = ORIENT_ROTATE_270;
					break;
				case EXIF_ORIENT_ROTATE_180:
					pbOrient = ORIENT_ROTATE_180;
					break;
				default:
					pbOrient = EXIF_ORIENT_ROTATE_0;
					hideMenu = 0;
					return;
			}
			pbFileFlipRotate(fileIdx, pbOrient);
			xdcfFileCountGet(&fileCnt);
			pbOrient = EXIF_ORIENT_ROTATE_0;
			hideMenu = 0;
			break;
	}
#endif
}

UINT32
pbFileRotate(
	UINT32 isThumb,
	UINT32 filepos,
	UINT32 scaleAddr,
	UINT32 outWidth,
	UINT32 outHeight,
	UINT32 scaleToFit,
	UINT32 bgColor,
	UINT8 orient
)
{
	pbParam_t* pParam;
	
#if 1	
	exifImageInfo_t *pInfo;
	UINT32 decompAddr;
	UINT32 freeBuf;
	UINT32 destBufAddr;
	float xRatio = 1.0, yRatio = 1.0;
	UINT32 scaleWidth, scaleHeight;
	UINT32 xoffset, yoffset;
	UINT32 temp;
	UINT32 maxRotWidth, maxRotHeight;

	osdClearScreen(0);
	pParam = pbCbGet();

	pParam->err = 0;

	if (pbAbortFlag)
		return 1;

	/* retrive image from exif or jfif file */
	if ((pParam->err = pbImageDecode(isThumb, filepos, &pInfo)) != 0) {
		dprint("Decoding failed\n");
		return pParam->err;
	}

	if (pbAbortFlag)
		return 1;

	pbMemDecompBufferGet(&decompAddr);

	/* decompress image retrived from pbImageDecode() */
	if ((pParam->err = pbImageDeCompress(pInfo, decompAddr)) != 0) {
		dprint("Decompression failed\n");
		return pParam->err;
	}

	if (pbAbortFlag)
		return 1;

	/* scale the decompressed images to proper size for playback       */
	/* 536 sram hardware function In case the pixel dimension is not   */
	/* equal to the actual dimension, copy the actual dimension to the */
	/* center of decompresion buffer and fill the remaining area with  */
	/* the specified background color                                  */
	pbMemFreeBufferGet(&freeBuf);
	freeBuf += 4 * outWidth * outHeight;	/* Joe modify temp 20040114 */

	if ((pInfo->pixelX != pInfo->w) || (pInfo->pixelY != pInfo->h)) {
		pbImageFrameFill(
			decompAddr, freeBuf,
			pInfo->pixelX, pInfo->pixelY,
			pInfo->w, pInfo->h,
			bgColor
		);

		pInfo->w = pInfo->pixelX;
		pInfo->h = pInfo->pixelY;
	}

	if (pbAbortFlag)
		return 1;

	/* Rotate image according to the orient tag                  */
	/* Scale down to request dimension first before doing rotate */
	/* so that less free memory space is required                */
	/*if (pInfo->orient != EXIF_ORIENT_ROTATE_0) */{
		if (pInfo->w > outWidth || pInfo->h > outHeight) {
			xRatio = (float)pInfo->w / (float)outWidth;
			yRatio = (float)pInfo->h / (float)outHeight;

			if (xRatio >= yRatio) {
				maxRotWidth = outWidth;
				maxRotHeight = ((UINT32)((float)pInfo->h / xRatio) + 7) >> 3 << 3;
			}
			else {
				maxRotHeight = outHeight;
				maxRotWidth = 	((UINT32)((float)pInfo->w / yRatio) + 7) >> 3 << 3;
			}

			pParam->err = pbImageScale(decompAddr, pInfo->w, pInfo->h, decompAddr, \
				maxRotWidth, maxRotHeight);
			pInfo->w = maxRotWidth;
			pInfo->h = maxRotHeight;
		}
	}

	if (pbAbortFlag)
		return 1;

	if (orient == EXIF_ORIENT_ROTATE_90) {
		printf("orient = 90\n");
		hwImgRotateDo(decompAddr, pInfo->w, pInfo->h, freeBuf, 0, 0);
		temp = pInfo->w;
		pInfo->w = pInfo->h;
		pInfo->h = temp;
	}
	else if (orient == EXIF_ORIENT_ROTATE_180){
		printf("orient = 180\n");
		hwImgRotateDo(decompAddr, pInfo->w, pInfo->h, freeBuf, 0, 0);
		hwImgRotateDo(freeBuf, pInfo->h, pInfo->w, decompAddr, 0, 0);
	}
	else if (orient == EXIF_ORIENT_ROTATE_270) {
		printf("orient = 270\n");
		hwImgRotateDo(decompAddr, pInfo->w, pInfo->h, freeBuf, 1, 0);
		temp = pInfo->w;
		pInfo->w = pInfo->h;
		pInfo->h = temp;
	}

	/* copy image image back to decompression buffer */
	if (orient == EXIF_ORIENT_ROTATE_90 ||
		orient == EXIF_ORIENT_ROTATE_270) {
 		hwImgCopyDo(freeBuf,
			pInfo->w , pInfo->h,
			0, 0,
			pInfo->w, pInfo->h,
			decompAddr,
			pInfo->w, pInfo->h,
			0,  0,
			0
		);
	}

	if (pbAbortFlag)
		return 1;

	/* If scale to fit is not specify */
	if (!scaleToFit) {
		if (pInfo->w <= outWidth && pInfo->h <= outHeight) {/* small pict, do not scale */
			xRatio = 1.0;
			yRatio = 1.0;
			scaleWidth = pInfo->w;
			scaleHeight = pInfo->h;
		}
		else {
			xRatio = (float)pInfo->w / (float)outWidth;
			yRatio = (float)pInfo->h / (float)outHeight;

			if (xRatio >= yRatio) {
				scaleWidth = outWidth;
				scaleHeight = ((UINT32)((float)pInfo->h / xRatio) + 7) >> 3 << 3;
			}
			else {
				scaleHeight = outHeight;
				scaleWidth = ((UINT32)((float)pInfo->w / yRatio) + 7) >> 3 << 3;
			}
		}

		pParam->err = pbImageScale(decompAddr, pInfo->w, pInfo->h, decompAddr, \
			scaleWidth, scaleHeight);

		xoffset = ((outWidth - scaleWidth) / 2) >> 1 << 1;
		yoffset = ((outHeight - scaleHeight) / 2) >> 1 << 1;

		pubFill((UINT8 *)((freeBuf << 1) | 0xac000000), outWidth << 1, \
			outHeight, 0, 0, outWidth << 1, outHeight, bgColor);

		hwImgCopyDo(decompAddr,
			scaleWidth , scaleHeight,
			0, 0,
			scaleWidth, scaleHeight,
			freeBuf,
			outWidth, outHeight,
			xoffset,  yoffset,
			0
		);
		destBufAddr = freeBuf;
	}
	else {/* scale to fit */
		if ((pInfo->w *  pInfo->h) < (outWidth * outHeight)) /* scale up */
			destBufAddr = decompAddr + pInfo->w *  pInfo->h;
		else
			destBufAddr = decompAddr;

		hwImgScaleDo(decompAddr, pInfo->w, pInfo->h, destBufAddr, outWidth, outHeight, freeBuf);
	}


	hwImgCopyDo(
		destBufAddr,
		outWidth , outHeight,
		0, 0,
		outWidth, outHeight,
		scaleAddr,
		outWidth, outHeight,
		0,  0,
		0
	);

#endif
	return pParam->err;
}
UINT8 menuSlideIntvlInit(UINT32 key, UINT8* index)
{
	*index = sPara.Interval + 1;
	return CALL_DEFAULT;
}

UINT8 menuSlideImageInit(UINT32 key, UINT8* index)
{
	*index = sPara.FolderAll + 1;
	return CALL_DEFAULT;
}

UINT8 menuSlideRepeatInit(UINT32 key, UINT8* index)
{
	if(sPara.Repeat)
		*index = 1;
	else
		*index = 2;
	return CALL_DEFAULT;
}

UINT8 menuProtectInit(UINT32 key, UINT8* index)
{
#if 0	
	xdcfAttrElm_t attr;

	xdcfCurFileByPosSet(fileIdx);
	xdcfCurFileAttrGet(&attr);
	if((attr.attribute) & xDCF_ATTR_READONLY)
		*index = 1;
	else
		*index = 2;
#endif	
	return CALL_DEFAULT;
}

extern UINT32 ui32NextState;

UINT8 menuReturn(UINT32 menu, UINT8* index)
{
	UINT32 ui32IrqSts;
	
//	uiState = menu;
	ENTER_CRITICAL(ui32IrqSts);
	if (ui32NextState == uiState)
	{
		ui32NextState = menu;
	}
	EXIT_CRITICAL(ui32IrqSts);
	osQuePost(uiKeyMsgQ, &keyUnknow[0]);
	return 0;
}
UINT8 menuResizeInit(UINT32 key, UINT8* index)
{
#if 0	
	exifImageInfo_t *pInfo=0;

	*index = 6;

	xdcfFileTypeGet(fileIdx, &fileType);
	if (fileType ==	xDCF_FILETYPE_JPG)
	{
		if(pbImageDecode(0, fileIdx, &pInfo) == SUCCESS)
		{
			printf("pInfo.w=%d\n",pInfo->w);
			printf("pInfo.h=%d\n",pInfo->h);
			if(((pInfo->w) == 1600)&&((pInfo->h) == 1200))
				*index = 1;
			else if(((pInfo->w) == 1280)&&((pInfo->h) == 1024))
				*index = 2;
			else if(((pInfo->w) == 1024)&&((pInfo->h) == 800))
				*index = 3;
			else if(((pInfo->w) == 800)&&((pInfo->h) == 600))
				*index = 4;
			else if(((pInfo->w) == 640)&&((pInfo->h) == 480))
				*index = 5;
		}
	}
#endif	
	return CALL_DEFAULT;
}

void pbErrMsg(UINT8 errcode)
{
	UINT8 action = 1;
	switch(errcode)
	{
		case ERR_NOCARD:
			osdClearScreen(0);
			osdStrDisp(96, 96, UserFont, 0x10, "No Card!");
			break;
		case ERR_READONLY:
			osdClearScreen(0);
			osdStrDisp(70, 96, UserFont, 0x10, "File Locked!");
			break;
		case ERR_FILETYPE:
			osdClearScreen(0);
			osdStrDisp(60, 96, UserFont, 0x10, "File Type Err!");
			break;
		case ERR_SCALEUP:
			osdClearScreen(0);
			osdStrDisp(30, 96, UserFont, 0x10, "Scale down only!");
			break;
		default:
			action = 0;
	}

	if(action)
	{
		osTimeDly(300);
		osdClearScreen(0);
	}

}

