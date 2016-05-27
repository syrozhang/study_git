//#define RS_232_DBG	
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

#include "Vfs_api.h"
#include "dcf_api.h"
#include "Ebook_d.h"
#include "extern.h"
#include "menu_capture.h"

#define    	FILELIST_MAXLINE      			5

#define   	EBOOK_PERLINE_CHARNUM  		30					/*每行多少个字*/
#define   	EBOOK_PERROW_CHARNUM   		9  				/*每列多少个字*/
#define   	EBOOK_PERPAGE_CHARNUM    	(EBOOK_PERLINE_CHARNUM*EBOOK_PERROW_CHARNUM)

#define    	EBOOK_DRAW_PREVIEW      		0					/*画预览区*/
#define    	EBOOK_DRAW_BOOKTEXT   		1					/*显示书的内容*/
#define    	EBOOK_DRAW_BG       			2     				/*画背景 */

#define		EBOOK_ITEM_MAX 		 		3
#define    	EBOOK_SUBITEM_MAX			3

#define		EBOOK_MENU_X					64
#define		EBOOK_MENU_Y					32


#define 	EBOOK_BAR_X				MENUBAR_X
#define 	EBOOK_BAR_Y				MENUBAR_Y
#define 	EBOOK_BAR_WIDTH       		MENUBAR_W
#define 	EBOOK_BAR_HEIGHT     		MENUBAR_H

#define 	EBOOK_TITLE_OFF_X			MENUTITLEX
#define 	EBOOK_TITLE_OFF_Y			MENUTITLEY


#define 	EBOOK_TEXT_OFF_X      		MENUTEXT_X
#define 	EBOOK_TEXT_OFF_Y      		MENUTEXT_Y
#define 	EBOOK_TEXT_HEIGHT     		MENUTEXT_Y_OFF

#define 	EBOOK_ICON_X      			ICON_X
#define 	EBOOK_ICON_Y				ICON_Y
#define 	EBOOK_ICON_OFF_X			ICON_X_OFF


#define 	CurrentIC8751	 				0
#define 	ws9712_HP 						1
#define 	ws9712_OUT2					2
#define 	ws9712_MONOOUT				3
#define 	ws9712_OUT3					4
#define 	ws9712_OUT2HP_VOLMASK 		0xbf3f



static char 		*semApp ;
extern UINT8 		sub_menu_acc;
extern UINT8 		TVFlag; 
extern UINT8  	imgBuf[];
extern UINT8 		SpeakFlag;//wendy for speak
extern void pbMainBack();

/* Paul@2006/05/29 add start */
static UINT8 g_ui8LoadBookmark = 0;
/* Paul@2006/05/29 add end */

UINT8   	ui8EBookReadNow = 	0;
//UINT8     ui8TaskStopFlag = 	1;
UINT8       ui8FirstInEBook =  	1;
UINT32   	ui32FileIdx;
UINT32   	ui32FileCnt;
UINT32   	ui32DirCnt;
UINT32      ui32DirIdx;
xdcfAttrElm_t xdcfFileAttr;
xdcfAttrElm_t xdcfDirAttr;
UINT32 		ui32FileFilter  	=  xDCF_FILETYPE_RESERVE2;	/*将保留2设置其为"txt"类型*/
UINT8 		ui8DispType 		=  PB_DISP_FOUR ;  			/*当前EBook的模式 1：全屏阅读方式   2:列表浏览方式*/
UINT32		ui32Filetype 		=  -1;	
UINT32      ui32CurPageFlag   	=  0 ,  
			ui32PrePageFlag   	=  0 ;
SINT32 		si32EBookMusicVol 	=  9;						/*0---9*/
UINT32      ui32FileIsEnd   	=  0;
UINT32		ui32gPB;
UINT32  	ui32ReadSize;

UINT8 		strEBook[FILELIST_MAXLINE][13];   				/*保存文件名*/
UINT32 		ui32FileSize;

UINT32 		handle = 0;
UINT32      *pui32SavePage 		= NULL ;
UINT8		*pui8ReadBuf = NULL;
// for bug 82 e  268 269 same
/***********************************************************/
UINT8 *EBookStrings[EBOOK_ITEM_MAX][EBOOK_SUBITEM_MAX][LANGUAGES] = 
{
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
		"Unprotect"	,
		"取消保护"	,
		"玂臔"	,
		"Unproteger",//"Cancelar protección",
		"Отменить защиту",
		"  Schutzentfernen",
		"non protégé",//"non protéger ",
		"Elimina Protezione ",
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
	}	
	

} ;

/* Paul@2006/05/29 add start */
enum 
{
	NoBook = 0,
	TitleEBook,
	CurPage,
	OpenFileErr,
	EmptyFile,
	IsLoadmark_s,
	IsLoadmark_s1,
	IsSavemark_s,
	IsSavemark_s1,
	Book_yes_s,
	Book_no_s,
	MaxItem
};
//268 269 same
UINT8 *UiBookStrings[MaxItem][LANGUAGES] =
{	
	{
		"No Files!",
		"无文件",
		"礚ゅン",
		"Sin  Archivo ",
		"Нет файла",
		"kein Datei",
		"pas de fichier",//"pas de dossier",
		"No doc",
		"No Files!",//"N\xe4o há documento",
		"颇老 绝澜",
	},
	{
		"eBook",
		"电子书",
		"筿",
		"Elibro",//"Libro electrónico",
		"файла",//"Электронная книга",
		"Ebuch",
		"eBook",//"livre électronique",// tzw8
		"Elibro",
		"Elivro",
		"傈磊合",
	},
	{
		"Current Page",
		"当前页",
		"讽玡",
		"Actual",
		"Нынче",
		"Aktuell",
		"présent",
		"Attuale",
		"Corrente",
		"泅犁",
	},
	{
		"File err",
		"文件错误",
		"ゅン岿粇",
		"Error de archivo",
		"Ошибка файла",
		"Datei Fehler",
		"Erreur de fichier",//"Erreur de dossier",
		"Doc.errore",
		"Erro de ducomento",
		"颇老 绢矾",
		},
	{
		"No Files!",
		"无文件",
		"礚ゅン",
		"Sin  Archivo ",
		"Нет файла",
		"kein Datei",
		"pas de fichier",//"pas de dossier",
		"No doc",
		"N\xe4o há documento",
		"颇老 绝澜",
	},
	{
		"Is load bookmark?",
		"是否装载书签?",
		"琌杆更帽?",
		"  Cargar el marcador ",//"Cargar el marcador de libro",
		"загрузить закладку?",
		"Lesezeichenladen ",
		"charge de signet",
		"Caricare l'etichetta",
		"Instalar o mercador ",
		"合付农 肺爹　",
		
	},
	{
		"",
		"",
		"",
		"    de libro?",
		"",
		"",
		"",
		"",
		"   de página",
		"",

	},
	{
		"Is save bookmark?",
		"是否保存书签?",
		"琌玂帽?",
		"Conserva el marcador de ",//"Conserva el marcador de libro o no?",
		"сохраните закладку?  ",
		"als Zeichen speichert?",
		"enregistrer signet?",
		"Se conserva l'etichetta?",
		"Se conservar o mercador ",
		"合付农甫 夯粮且啊夸",
	},	
	{
		"",
		"",
		"",
		"    libro o no?",
		"",
		"",
		"",
		"",
		"    de página",
		"",

	},
	{
		"Yes",
		"是",
		"琌",
		"Sí",
		"Да",
		"Ja",
		"oui",
		"Si",
		"Sim",
		"Yes",
	},	
	{
		"No",
		"否",
		"",
		"No",
		"Нет ",
		"Nein",
		"non",
		"No",
		"N\xe3o",
		"No",
	},		
};
#define   EBOOKPICT     0
#define   EBOOKICO        1
#define   EBOOKREAD    2
UINT32 MenupEBook[][4]=
{
#if 1
	{EPict_P1,sizeof(EPict_P1),166,50},    // 电子书图
	{Ebook_I1, sizeof(Ebook_I1),0,0},// 电子书图标
	{ERead_R1, sizeof(ERead_R1),0,0},	// 读电子书背景
#endif
};
void   pbDisplayEbook_Ico(UINT8 ui8_Idx ,UINT8  ui8_X,  UINT8  ui8_y)
{
        pbImagePaste(MenupEBook[ui8_Idx][0],MenupEBook[ui8_Idx][1],ui8_X,ui8_y);
}



UINT8* GetBookString(UINT16 stringId)
{
    UINT8* str;
    UINT32 languageId = sPara.language;

     if (languageId >= LANGUAGES)
    {
		languageId = LanguageEnglish;
    }

    if(stringId >= MaxItem) 
    {
		return 0;
    }

    if(UiBookStrings[stringId][languageId][0]=='\0')
    {
	    str = UiBookStrings[stringId][0];
    }
	else
	{
	    str = UiBookStrings[stringId][languageId];
	}

	return str;
}
/**********************************************************************/

void Pb_EbookTitle(UINT8 *Title)
{
	
	static UINT8   EbookTilie_X ,EbookTilie_Y ;
	
	switch(sPara.language)
	{
		case LanguageEnglish:
			EbookTilie_X= 28 ;
			EbookTilie_Y= 3 ;
			break ;
		case LanguageSpanish:
			EbookTilie_X= 18 ;
			EbookTilie_Y= 3 ;
			break ;
		case LanguageRussian:
			EbookTilie_X= 28 ;
			EbookTilie_Y= 3 ;
			break ;
		default :			/*中文*/
			EbookTilie_X= 28 ;
			EbookTilie_Y= 3 ;
			break ;
	}
	osdStrDisp(EbookTilie_X, EbookTilie_Y, UserFont10x20, 0xd0, Title);
	
}

/*******************************************************************/
enum
{
	EBOOK_MENU_DEL = 0,
	EBOOK_MENU_PROTECT
};

enum
{
	EBOOK_DEL_TITLE = 0,
	EBOOK_DEL_CUR ,
	EBOOK_DEL_ALL
};

void	F_EBookMenuDel(UINT8	idx);
void	F_EBookMenuDelImage();

void	F_EBookMenuProtectImage();
void	F_EBookMenuProtect(UINT8	idx);

void	F_EBookMenuDisProtect(UINT8	idx);
void	F_EBookMenuDisProtectImage();

UINT8	F_EBookDelSingleFile( );	// John @01/22/06
UINT8	F_EBookDelAllFile();
UINT8	F_EBookProtectSingleFile( );
UINT8	F_EBookProtectAllFile();
UINT8	F_EBookDisProtectSingleFile();
UINT8	F_EBookDisProtectAllFile();

typedef	void	(*pEBookSubMenuProc)(UINT8 idx) ;

extern UINT8 IsSDProtect(); // John @01/22/06

pEBookSubMenuProc	EBookRefreshSubitem[EBOOK_ITEM_MAX] = 
{
	//F_EBookMenuSelMusic,
	F_EBookMenuDel,
	F_EBookMenuProtect, 
	F_EBookMenuDisProtect,
};

typedef	void	(*pEBookMenuProc)(void) ;

pEBookMenuProc	EBookRefreshItem[EBOOK_ITEM_MAX] = 
{
	//F_EBookMenuSelMusicImage,
	F_EBookMenuDelImage,
	F_EBookMenuProtectImage,
	F_EBookMenuDisProtectImage
};


typedef	UINT8	(*pEBookMenuFuncProc)(void) ;
pEBookMenuFuncProc	EBookRefreshFunc[EBOOK_ITEM_MAX*2] = 
{
	//F_EBookMenuSelMusicImage,
	//F_EBookMenuSelMusicImage,
	F_EBookDelSingleFile,
	F_EBookDelAllFile,
	F_EBookProtectSingleFile,
	F_EBookProtectAllFile,
	F_EBookDisProtectSingleFile,
	F_EBookDisProtectAllFile
};



//extern UINT32		EBookItemImage[EBOOK_ITEM_MAX][2] ;


/***********************************************************
						选择背景音乐

*************************************************************/
void	F_EBookMenuProtectImage()
{
	UINT32	x,y ;
	x = 100 ;
	y = 196 ;
	UINT32 dispW, dispH;
	UINT32 gpb;
	dispW = PIP_W;
	dispH = PIP_H;	
//	sysgMemDispAddrGet(&gpb);
//	hwDispPreviewSizeGet(&dispW, &dispH);
//	hwPlayBufSizeSet(dispW,	dispH);
//	hwPlayBufAddrSet(gpb);
	//pbDramImgPlay(Tool_menuback, sizeof(Tool_menuback), gpb, dispW, dispH);
	pbIconMenuBG();
#if 0  //fqdao_modify  06.5.19
	pbImagePaste(EBookItemImage[0][0],EBookItemImage[0][1],EBOOK_ICON_X,EBOOK_ICON_Y) ;
	pbImagePaste(EBookItemImage[1][0],EBookItemImage[1][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X,EBOOK_ICON_Y) ;
	pbImagePaste(EBookItemImage[2][0],EBookItemImage[2][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X*2,EBOOK_ICON_Y) ;
#else
	pbImagePaste(EBookItemImage[0][0],EBookItemImage[0][1],EBOOK_ICON_X,EBOOK_ICON_Y) ;
	pbImagePaste(EBookItemImage[1][0],EBookItemImage[1][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X,EBOOK_ICON_Y) ;
	pbImagePaste(EBookItemImage[2][0],EBookItemImage[2][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X*2,EBOOK_ICON_Y) ;
#endif	
	EBookRefreshSubitem[1](1) ;
}

void	F_EBookMenuProtect(UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;
	osdClearScreen(0x00) ;	
	pbDisplayMenuWord();
	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			
			color = 0xd0;		
			osdStrDisp(EBOOK_TITLE_OFF_X, EBOOK_TITLE_OFF_Y+EBOOK_TEXT_HEIGHT*i, UserFont10x20, MENUTITLES, EBookStrings[1][i][sPara.language]) ;
		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw(EBOOK_BAR_X, EBOOK_BAR_Y+EBOOK_TEXT_HEIGHT*(i-1), EBOOK_BAR_WIDTH, EBOOK_BAR_HEIGHT, 0x0e) ;			
			osdStrDisp(EBOOK_TEXT_OFF_X, EBOOK_TEXT_OFF_Y+EBOOK_TEXT_HEIGHT*(i-1), UserFont10x20, color, EBookStrings[1][i][sPara.language]) ;
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(EBOOK_TEXT_OFF_X, EBOOK_TEXT_OFF_Y+EBOOK_TEXT_HEIGHT*(i-1), UserFont10x20, color, EBookStrings[1][i][sPara.language]) ;
		}
	}
}

void	F_EBookMenuDisProtect(UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;
	osdClearScreen(0x00) ;	
	pbDisplayMenuWord();
	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			color = 0xd0;		
			osdStrDisp(EBOOK_TITLE_OFF_X, EBOOK_TITLE_OFF_Y+EBOOK_TEXT_HEIGHT*i, UserFont10x20, MENUTITLES, EBookStrings[2][i][sPara.language]) ;
		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw(EBOOK_BAR_X, EBOOK_BAR_Y+EBOOK_TEXT_HEIGHT*(i-1), EBOOK_BAR_WIDTH, EBOOK_BAR_HEIGHT, 0x0e) ;			
			osdStrDisp(EBOOK_TEXT_OFF_X, EBOOK_TEXT_OFF_Y+EBOOK_TEXT_HEIGHT*(i-1), UserFont10x20, color, EBookStrings[2][i][sPara.language]) ;
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(EBOOK_TEXT_OFF_X, EBOOK_TEXT_OFF_Y+EBOOK_TEXT_HEIGHT*(i-1), UserFont10x20, color, EBookStrings[2][i][sPara.language]) ;
		}
	}
}

void	F_EBookMenuDisProtectImage()
{
	UINT32	i,j ;
	UINT32	x,y ;
	x = 100 ;
	y = 196 ;
	UINT32 dispW, dispH;
	UINT32 gpb;
	dispW = PIP_W;
	dispH = PIP_H;	
//	sysgMemDispAddrGet(&gpb);
//	hwDispPreviewSizeGet(&dispW, &dispH);
//	hwPlayBufSizeSet(dispW,	dispH);
//	hwPlayBufAddrSet(gpb);
//	pbDramImgPlay(Tool_menuback, sizeof(Tool_menuback), gpb, dispW, dispH);
	pbIconMenuBG();
#if  0 //fqdao_modify  06.5.19
	pbImagePaste(EBookItemImage[1][0],EBookItemImage[1][1],EBOOK_ICON_X,EBOOK_ICON_Y) ;
	pbImagePaste(EBookItemImage[2][0],EBookItemImage[2][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X,EBOOK_ICON_Y) ;
#else
	pbImagePaste(EBookItemImage[0][0],EBookItemImage[0][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X*2,EBOOK_ICON_Y) ;
	pbImagePaste(EBookItemImage[1][0],EBookItemImage[1][1],EBOOK_ICON_X,EBOOK_ICON_Y) ;
	pbImagePaste(EBookItemImage[2][0],EBookItemImage[2][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X,EBOOK_ICON_Y) ;
#endif	
	EBookRefreshSubitem[2](1) ;
}






void	F_EBookMenuDel(UINT8	idx)
{
	UINT32	color ;
	UINT8	i ;
	color = 0xf0 ;
	osdClearScreen(0x00) ;
	pbDisplayMenuWord();
	for(i=0 ;i<3;i++)
	{
		if(i==0)
		{
			
			color = 0xd0;
			osdStrDisp(EBOOK_TITLE_OFF_X, EBOOK_TITLE_OFF_Y+EBOOK_TEXT_HEIGHT*i, UserFont10x20, MENUTITLES, EBookStrings[0][i][sPara.language]) ;

		}
		else if(i==idx){
			color = 0xde ;
			osdBarDraw(EBOOK_BAR_X, EBOOK_BAR_Y+EBOOK_TEXT_HEIGHT*(i-1), EBOOK_BAR_WIDTH, EBOOK_BAR_HEIGHT, 0x0e) ;
			osdStrDisp(EBOOK_TEXT_OFF_X, EBOOK_TEXT_OFF_Y+EBOOK_TEXT_HEIGHT*(i-1), UserFont10x20, color, EBookStrings[0][i][sPara.language]) ;
			
		}
		else	{
			color = 0xf0 ;
			osdStrDisp(EBOOK_TEXT_OFF_X, EBOOK_TEXT_OFF_Y+EBOOK_TEXT_HEIGHT*(i-1), UserFont10x20, color, EBookStrings[0][i][sPara.language]) ;
		}
	}
}


void	F_EBookMenuDelImage()
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

	pbIconMenuBG();
//	pbDramImgPlay(Tool_menuback, sizeof(Tool_menuback), gpb, dispW, dispH);
	pbImagePaste(EBookItemImage[0][0],EBookItemImage[0][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X,EBOOK_ICON_Y);//EBOOK_ICON_X,EBOOK_ICON_Y) ;
	pbImagePaste(EBookItemImage[1][0],EBookItemImage[1][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X*2,EBOOK_ICON_Y) ;
#if  0  //fqdao_modify  06.5.19
	pbImagePaste(EBookItemImage[2][0],EBookItemImage[2][1],EBOOK_ICON_X+EBOOK_ICON_OFF_X*3,EBOOK_ICON_Y) ;
#else
	pbImagePaste(EBookItemImage[2][0],EBookItemImage[2][1],EBOOK_ICON_X,EBOOK_ICON_Y) ;
#endif	
	//pbImagePaste(EBookItemImage[3][0],EBookItemImage[3][1],  EBOOK_ICON_X+EBOOK_ICON_OFF_X*3,	EBOOK_ICON_Y) ;
	EBookRefreshSubitem[0](1) ;
}


UINT8	eBookSubMax[EBOOK_ITEM_MAX] = {2,2,2};

UINT8	F_EBookDelSingleFile( )//UINT32	idx)
{
	UINT32	err ;
	
	
	if((xdcfCurFileByPosSet( ui32FileIdx )) != 0)
	{
	}
	if (xdcfCurFileAttrGet(&xdcfFileAttr)==0)
	{
		// John @01/22/06
		if (IsSDProtect ())
		{
			MessageShow (ErrCardProtect);				
		return 0;
		}
			
		if (xdcfFileAttr.attribute & xDCF_ATTR_READONLY)
		{
			MessageShow(ErrfileProtect);
			return 0;
		}	
		
		xdcfOneFileErase(xdcfFileAttr.name);		
		err	= xdcfFileCountGet( &ui32FileCnt );
		if(!ui32FileCnt) 
		{
			xdcfDirCountGet(&ui32DirCnt);
			xdcfCurDirPosGet(&ui32DirIdx);
			xdcfCurDirAttrGet(&xdcfDirAttr);
		}
		else if(ui32FileIdx > ui32FileCnt) 
			ui32FileIdx = ui32FileCnt  ;
	}
				return 1;	
}
UINT8	F_EBookDelAllFile()
{
	UINT32	Idx, Counts;	// John @01/22/06
	
	if (IsSDProtect ())
	{
		MessageShow (ErrCardProtect);				
		return 0;
	}
	// John @01/22/06
	
#if 0   //fqdao_modify  06.5.30
	 xdcfFileCountGet(&Counts);
	for (ui32FileIdx = 1; ui32FileIdx <= Counts; ui32FileIdx++)
	{
		if(!F_EBookDelSingleFile())
		{
		//	return 0; //fqdao_modify for bug del profile nullity 06.5.29 
		}
	}
#endif

	xdcfAllFilesErase();
	xdcfFileCountGet(&ui32FileCnt);
	if(!ui32FileCnt) 
	{
		xdcfDirCountGet(&ui32DirCnt);
		xdcfCurDirPosGet(&ui32DirIdx);
		xdcfCurDirAttrGet(&xdcfDirAttr);				
	}
	else if(ui32FileIdx>ui32FileCnt) 
		ui32FileIdx = ui32FileCnt ;
	xdcfFileCountGet(&ui32FileCnt);

	return 1;
}

UINT8	F_EBookProtectSingleFile( )//UINT32	idx)
{
	// John @01/22/06
	if (IsSDProtect ())
	{
		MessageShow (ErrCardProtect);				
		return 0;
	}
	
	xdcfCurFileByPosSet( ui32FileIdx );
	xdcfCurFileAttrGet(&xdcfFileAttr) ;
	xdcfCurFileAttrSet(xDCF_ATTR_READONLY |xdcfFileAttr.attribute) ;

	return 1;
}
UINT8	F_EBookProtectAllFile()
{
	// John @01/22/06
	if (IsSDProtect ())
	{
		MessageShow (ErrCardProtect);				
		return 0;
	}
	
	xdcfAttrElm_t tempFileAttr;
	UINT32	i;
	xdcfFileCountGet(&ui32FileCnt);
	MessageShow (MenuWaite);	//fqdao
	for(i=1;i<=ui32FileCnt;i++)
	{
		xdcfCurFileByPosSet( i );
		xdcfCurFileAttrGet(&tempFileAttr) ;
		xdcfCurFileAttrSet(xDCF_ATTR_READONLY |tempFileAttr.attribute) ;
	}
	xdcfCurFileByPosSet( ui32FileIdx );
	return 1;
}

UINT8	F_EBookDisProtectSingleFile()
{
	// John @01/22/06
	if (IsSDProtect ())
	{
		MessageShow (ErrCardProtect);				
		return 0;
	}
	
	xdcfCurFileByPosSet( ui32FileIdx );
	xdcfCurFileAttrGet(&xdcfFileAttr) ;
	xdcfCurFileAttrSet((~xDCF_ATTR_READONLY) &xdcfFileAttr.attribute) ;
	return 1;
}

UINT8	F_EBookDisProtectAllFile()
{
	// John @01/22/06
	if (IsSDProtect ())
		{
			MessageShow (ErrCardProtect);				
		return 0;
		}
	
	xdcfAttrElm_t tempFileAttr;
	UINT32	i;
	xdcfFileCountGet(&ui32FileCnt);
	MessageShow (MenuWaite);	//fqdao
	for(i=1;i<=ui32FileCnt;i++)
	{
		xdcfCurFileByPosSet(i);
		xdcfCurFileAttrGet(&tempFileAttr) ;
		xdcfCurFileAttrSet((~xDCF_ATTR_READONLY) &tempFileAttr.attribute) ;
		//F_EBookDisProtectSingleFile(i) ;
	}
	xdcfCurFileByPosSet( ui32FileIdx );

	return 1;
}
void paEBookMenuFunc()
{
	UINT32	key = UI_KEY_MODE_ACCESSORIES;
	UINT32  err;
	UINT8 item=0,	subitem = 1	 ;
	osdClearScreen(0x00);
	UINT8 	flag;
	
	EBookRefreshItem[item]();
	while(1)
	{
		switch(key)
		{
			case	UI_KEY_DIR_UP:
				if(subitem>1)
					subitem -- ;
				
				EBookRefreshSubitem[item](subitem) ;					
				break ;
			case	UI_KEY_DIR_DOWN:
				if(subitem< eBookSubMax[ item ] ) 
						subitem++ ;
				
				EBookRefreshSubitem[item](subitem );
				break ;
				
			case	UI_KEY_DIR_LEFT:
				if(item )	
				{
					item -- ;	
					
				}
				else 
				{
					item = EBOOK_ITEM_MAX - 1;

				}
				EBookRefreshItem[item]();
				break ;
				
			case	UI_KEY_DIR_RIGHT:
				if(item < EBOOK_ITEM_MAX-1) 
				{
					item++ ;
					
				}
				else 
				{
					item = 0 ;
				}
				EBookRefreshItem[item]( );
				break ;
				
			case	UI_KEY_FUNC_OK:
				flag = EBookRefreshFunc[item*2 + subitem - 1 ]();
				if(flag)
				{
					MessageShow (MenuSuccess);
				}
				osdClearScreen(0);//fqdao
				pbEBookShow(EBOOK_DRAW_BG ,0 );
				pbEBookRefresh();
				return;
				break ;
				
			case 	UI_KEY_ACCIDENT:
				pbEBookRefresh();
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);
				return;
				break;
			case 	UI_KEY_FUNC_B:
			case	UI_KEY_FUNC_MENU:
			      osdClearScreen(0);   //fqdao_add 06.5.19
				pbEBookShow(EBOOK_DRAW_BG ,0 );
				pbEBookRefresh();
				return ;
				break ;
		}
		keyGet(&key) ;
	}
}





/***********************************************************
						选择上一页内容显示

*************************************************************/
void pbEBookPrePage( )
{
	if ( ui32CurPageFlag > 0 )
	{
		
		ui32CurPageFlag-- ;
		if ( ui32CurPageFlag > 0 )
		{
			ui32PrePageFlag = ui32CurPageFlag - 1 ;
		}
		else
			ui32PrePageFlag = 0;
		
	}
	else   //if  ui32CurPageFlag == 0 
	{
		ui32PrePageFlag = 0 ;	
		return;
	}
	ui32FileIsEnd = 0;	
	
	pbEBookTextGet();
}

/***********************************************************
						选择下一页内容显示

*************************************************************/
void pbEBookNextPage ( ) 
{
	if ( !ui32FileIsEnd  )
	{
		ui32PrePageFlag = ui32CurPageFlag++ ;
		pbEBookTextGet();
	}
}


/***********************************************************
						当前页内容显示

*************************************************************/


void pbEBookTextGet_O(  )
{
	UINT8 	*pui8EBookBuf ;
	UINT8 	*pbuf;
	UINT8 	ui8Buf[ EBOOK_PERPAGE_CHARNUM + 1 ]; //270个
	UINT32 	x1, y1 = 0;
	UINT32   ui32CurPageLenConter = 0;
	UINT8     *ui8ReadbufTemp;

	//hisIconDisp();
	//hisTimeDisp();
 	ui8ReadbufTemp = osMemAlloc(512);
	pui8EBookBuf = ui8ReadbufTemp;
	osTimeDly(3);
	//osSchedLock();
	vfsLseek(handle, *(pui32SavePage+ui32CurPageFlag), SEEK_SET);
	ui32ReadSize = vfsRead(handle, pui8EBookBuf, 512);
	
	/* Paul@2006/05/29 add start */
	if (g_ui8LoadBookmark == 0)
	{
	 	//osSchedUnlock();
		osTimeDly(3);
		//osTimeDly(50);		
		osdBarDraw(0, 20, 320, 220, 0);	
	}
	//printf("\n ui32ReadSize:%d",ui32ReadSize);
	
	for ( y1=0 ;  y1< EBOOK_PERROW_CHARNUM;  y1++  )  //EBOOK_PERROW_CHARNUM =  9
	{
		pbuf = ui8Buf; 

		for ( x1 = 0; x1< EBOOK_PERLINE_CHARNUM ;   )
		{
			if (  ui32CurPageLenConter + x1  >= ui32ReadSize      )
			{
				*pbuf		= '\0';
				ui32FileIsEnd 	= 1;
				break;
			}
			else if ( ui32CurPageLenConter + x1  >= ui32ReadSize - 1  )
			{	
				*pbuf++ 	= *pui8EBookBuf;
				*pbuf	= '\0';
				ui32FileIsEnd = 1;
				break;
			}
			
			if ( *pui8EBookBuf < 0x80 )
			{
				if ( *pui8EBookBuf == 0x0d && *(pui8EBookBuf + 1)  == 0x0a )  //回车
				{
					*pbuf++ 			= '\0';
					pui8EBookBuf 		+= 2;
					x1 += 2;
					break;
				}
				else
				{
					*pbuf++ = *pui8EBookBuf++ ;
					x1++;					//占用一个字符
				}
			}
			else if ( *pui8EBookBuf >= 0x80 )
			{
				//当最后个一字符为汉字且长度只足够一个
				//英文字符时，不显示
				if ( x1+2 > EBOOK_PERLINE_CHARNUM )
				{	
					*pbuf = '\0';
					break; 
				}
				*pbuf++ = *pui8EBookBuf++;
				*pbuf++ = *pui8EBookBuf++;
				x1 += 2;				//占用两个字符
			}
		}
		pbuf = ui8Buf;
		
		ui32CurPageLenConter += x1;
		if ( strlen( pbuf ) > 30 )
		{
		
				memcpy(pbuf,ui8Buf, 30);
				pbuf[30] = '\0';
		}	
		if ( g_ui8LoadBookmark == 0)
		{
			osdStrDisp( 10, y1 * (20+2) + 20, UserFont10x20, 0xf0, pbuf ); // 显示当前行的数据
		}
		
		if ( ui32FileIsEnd )
			break ;
	}
	
	if ( g_ui8LoadBookmark == 0)
	{
		pbuf = ui8Buf;
		sio_psprintf(pbuf, "%12d", ui32CurPageFlag + 1);
		osdStrDisp(50, 220,UserFont10x20,0xd0, pbuf);
	}

	if ( ui32FileIsEnd )
	{	
		osMemFree(ui8ReadbufTemp);
		return;
	}
	*(pui32SavePage + ui32CurPageFlag + 1 ) = *( pui32SavePage + ui32CurPageFlag ) + ui32CurPageLenConter; 
	osMemFree(ui8ReadbufTemp);

}

void pbEBookTextGet_R(  )
{
	UINT8 	*pui8EBookBuf ;
	UINT8 	*pbuf;
	UINT8 	ui8Buf[ 480 + 1 ]; //270个
	UINT32 	x1, xForBuf=0,y1 = 0;
	UINT32   ui32CurPageLenConter = 0;
	UINT8     *ui8ReadbufTemp;

	//hisIconDisp();
	//hisTimeDisp();
 	ui8ReadbufTemp = osMemAlloc(512);
	pui8EBookBuf = ui8ReadbufTemp;
	osTimeDly(3);
	//osSchedLock();
	vfsLseek(handle, *(pui32SavePage+ui32CurPageFlag), SEEK_SET);
	ui32ReadSize = vfsRead(handle, pui8EBookBuf, 512);
	
	/* Paul@2006/05/29 add start */
	if (g_ui8LoadBookmark == 0)
	{
	 	//osSchedUnlock();
		osTimeDly(3);
		//osTimeDly(50);		
		osdBarDraw(0, 20, 320, 220, 0);	
	}
	//printf("\n ui32ReadSize:%d",ui32ReadSize);
	
	for ( y1=0 ;  y1< EBOOK_PERROW_CHARNUM-1;  y1++  )  //EBOOK_PERROW_CHARNUM =  9
	{
		pbuf = ui8Buf; 
		xForBuf = 0;	
		for ( x1 = 0; x1<( EBOOK_PERLINE_CHARNUM );   )
		{
			if (  ui32CurPageLenConter + xForBuf>= ui32ReadSize      )
			{
				*pbuf		= '\0';
				ui32FileIsEnd 	= 1;
				break;
			}
			else if ( ui32CurPageLenConter + xForBuf>= ui32ReadSize - 1  )
			{	
				*pbuf++ 	= *pui8EBookBuf;
				*pbuf	= '\0';
				ui32FileIsEnd = 1;
				break;
			}
			
			if ( *pui8EBookBuf < 0x80 )
			{
				if ( *pui8EBookBuf == 0x0d && *(pui8EBookBuf + 1)  == 0x0a )  //回车
				{
					*pbuf++ 		= '\0';
					pui8EBookBuf 	+= 2;
					x1 += 2;
					xForBuf+= 2;
					break;
				}
				else
				{
					*pbuf++ = *pui8EBookBuf++ ;
					x1++;					//占用一个字符
					xForBuf++;
				}
			}
			else if ((*pui8EBookBuf == 0xA7)
			&& (*(pui8EBookBuf+1) > 0xA0))
			{
				x1++;	
				xForBuf += 2;
				*pbuf++ = *pui8EBookBuf++ ;
				*pbuf++ = *pui8EBookBuf++ ;
			}
			else if ( *pui8EBookBuf >= 0x80 )
			{
				//当最后个一字符为汉字且长度只足够一个
				//英文字符时，不显示
				if ( x1+2 > EBOOK_PERLINE_CHARNUM )
				{	
					*pbuf = '\0';
					break; 
				}
				*pbuf++ = *pui8EBookBuf++;
				*pbuf++ = *pui8EBookBuf++;
				x1 += 2;				//占用两个字符
				xForBuf+=2;
			}
		}
		/*将pbuf指针地址指向首位*/
		pbuf = ui8Buf;
		
		ui32CurPageLenConter += xForBuf;
		*(pbuf+xForBuf) = '\0';
		
		#if 0
		if ( strlen( pbuf ) > 30 )
		{
		
				memcpy(pbuf,ui8Buf, 30);
				pbuf[30] = '\0';
		}	
		#endif		
				/* Paul@2006/05/29 add start */
		if ( g_ui8LoadBookmark == 0)
		{
			osdStrDisp( 10, y1 * (20+2) + 20, UserFont10x20, 0xf0, pbuf ); // 显示当前行的数据
		}
		
		if ( ui32FileIsEnd )
			break ;
	}
	
	if ( g_ui8LoadBookmark == 0)
	{			
		pbuf = ui8Buf;
		sio_psprintf(pbuf, "%12d", ui32CurPageFlag + 1);
		osdStrDisp(50, 220,UserFont10x20,0xd0, pbuf);
	}

	if ( ui32FileIsEnd )
	{	
		osMemFree(ui8ReadbufTemp);
		return;
	}
	*(pui32SavePage + ui32CurPageFlag + 1 ) = *( pui32SavePage + ui32CurPageFlag ) + ui32CurPageLenConter; 
	osMemFree(ui8ReadbufTemp);

}
//LANGUAGES
void pbEBookTextGet(  )
{
	if( sPara.language == LanguageRussian )
	{
		pbEBookTextGet_R();
	}	
	else
	{
		pbEBookTextGet_O();
	}
}


/***********************************************************
						某书预览内容及文件信息显示

*************************************************************/
#define EBOOK_PREVIEW_BUFSIZE  100
void	   pbEBookInfoShow(UINT32 ui32FileIdx)
{
	UINT8          	str[20];

	sio_psprintf(str, "%04d.%02d.%02d ", fileattr.time.year,fileattr.time.mon,fileattr.time.day);

	osdStrDisp(178, 182, UserFont10x20, 0xf0, str);
//	sio_psprintf(str, "%02d.%02d.%04d", xdcfFileAttr.time.mon, xdcfFileAttr.time.day, xdcfFileAttr.time.year);
//	osdStrDisp( PANEL_1_OSD_W-160, 70+120-4, UserFont10x20, 0xd0, str);
//	sio_psprintf(str, "%02d:%02d:%02d", xdcfFileAttr.time.hour, xdcfFileAttr.time.min, xdcfFileAttr.time.sec);
//	osdStrDisp( PANEL_1_OSD_W-160, 90+120, UserFont10x20, 0xd0, str);

#if 0
	if(xdcfFileAttr.attribute&xDCF_ATTR_READONLY)	//John @01/22/06
	{
		osdStrDisp(295, 210, IconLock, 0x10,"\x1");
	}
	else
	osdStrDisp(295, 210, IconLock, 0x00,"  ");	
#endif
	
}


/***********************************************************
						文件列表显示

*************************************************************/

#define	MP4_FILELIST_X		52
#define	MP4_FILELIST_SX	46
#define	MP4_FILELIST_Y		54

#define	MP4_FILELIST_SIZE	32
void	   pbEBookFileList(UINT32 ui32FileIdx)
{
	UINT8 space[13];
	SINT32 i;
	UINT32 err;
	UINT32	y[FILELIST_MAXLINE] = {MP4_FILELIST_Y,MP4_FILELIST_Y+MP4_FILELIST_SIZE,
		MP4_FILELIST_Y+MP4_FILELIST_SIZE*2,
		MP4_FILELIST_Y+MP4_FILELIST_SIZE*3,MP4_FILELIST_Y+MP4_FILELIST_SIZE*4};

	UINT32	osdW, osdH;
	static UINT8 page_idx = 0;
	static UINT32 active_line = 0;

	xdcfAttrElm_t attr;

	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	
	if(ui32FileIdx == 0)
	{
		/*reset*/
		page_idx = 0;
		active_line = 0;
	}

	/*clear window*/
	memset(space, ' ', 12-4);
	space[12-4] = '\0';

	//if(page_idx != (ui32FileIdx+FILELIST_MAXLINE-1)/FILELIST_MAXLINE)
	if (1)
	{
		page_idx = (ui32FileIdx+FILELIST_MAXLINE-1)/FILELIST_MAXLINE;
		

		pbMainBack();
		pbImagePaste( MenupEBook[EBOOKPICT][0], MenupEBook[EBOOKPICT][1], MenupEBook[EBOOKPICT][2],  MenupEBook[EBOOKPICT][3] );
		Pb_EbookTitle(GetBookString(TitleEBook));
		
		for(i=0; i<FILELIST_MAXLINE; i++)
		{
			if ((page_idx-1)*FILELIST_MAXLINE + i < ui32FileCnt)
			{
				#if 0// tzw060121 for Ico
				pbDisplayEbook_Ico(EBOOKICO,24, y[i]+6);
				
//				pbImagePaste(Menup[1 ][0],  Menup[1][3], Menup[1][1],y[i]-4); // play ico
				#endif
			}	
			osdStrDisp(38, y[i] - 2, UserFont10x20, 0, &space[0]);			/*clear*/
		}
		for(i = (page_idx-1)*FILELIST_MAXLINE; i<(page_idx*FILELIST_MAXLINE); i++)
		{
			if(i < ui32FileCnt)
			{
				xdcfCurFileByPosSet( i + 1 );
				err = xdcfCurFileAttrGet(&attr);
				strncat(attr.name, &space[0], 12-strlen(attr.name));

				if(SUCCESS == err)
					strncpy(&strEBook[i%FILELIST_MAXLINE][0], attr.name,13-4);
				else
					strncpy(&strEBook[i%FILELIST_MAXLINE][0], &space[0], 13-4);
			}
			else
				strncpy(&strEBook[i%FILELIST_MAXLINE][0], &space[0], 13-4);

			strEBook[i%FILELIST_MAXLINE][12-4] = '\0';
		}
	}
	
 	if (ui32FileIdx !=0)
	{
		/*active selected file*/
		for(i=0; i<FILELIST_MAXLINE; i++)
		{
			if ((page_idx-1)*FILELIST_MAXLINE + i < ui32FileCnt)
		     	{  
		     		osdStrDisp(MP4_FILELIST_X, y[i]+2, UserFont10x20, 0xf0, &strEBook[i][0]);
				pbDisplayCleFile_Ico(MP4_FILELIST_SX,y[i]);

				pbDisplayEbook_Ico(EBOOKICO,24, y[i]+6);

				xdcfCurFileByPosSet((page_idx-1)*FILELIST_MAXLINE + i+1);
				err = xdcfCurFileAttrGet(&fileattr);


			//	xdcfCurFileByPosSet(ui32FileIdx);
			//	err = xdcfCurFileAttrGet(&fileattr);
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
			  osdStrDisp(MP4_FILELIST_X, y[i]+2, UserFont10x20, 0xf0, &strEBook[i][0]);
		}
		active_line = (ui32FileIdx-1)%FILELIST_MAXLINE;
	//	osdBarDraw( 34+2,y[active_line]-2 - 2, 106 - 8,  24, 0xf);
		pbDisplaySelFile_Ico(MP4_FILELIST_SX,y[active_line]);
	
		osdStrDisp(MP4_FILELIST_X, y[active_line]+2, UserFont10x20, 0x10, &strEBook[active_line][0]);

		xdcfCurFileByPosSet(ui32FileIdx);
		err = xdcfCurFileAttrGet(&fileattr);
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


/***********************************************************
						电子书显示功能函数


*************************************************************/

void pbEBookShow(UINT8 flag,  UINT32   ui32FileIdx  )
{	
	UINT8	i, j;
	UINT32	err	= SUCCESS;
	UINT32	pPBAddr, phyOsdDBAddr;
	UINT32	tempIdx = 1;
	UINT32	dispW, dispH;
	UINT32	pbThumbW, pbThumbH, pbThumbOffX, pbThumbOffY;
	
	
	pbThumbW =  120 ; // 160-40;
	pbThumbH =  84 ; //120-36;
	pbThumbOffX =150+20+2 ;
	pbThumbOffY = 44+14+14;

	//pImageBuf		= (UINT8*) (((UINT32 )ui8EBookBuf) |	0xac000000);
	//phyOsdDBAddr	= (((UINT32)pEBookBuf) & 0x00FFFFFF) >> 1;

	hwDateFontScalerSet(1);
	pbThumbBufferGet(&pPBAddr);
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwPlayBufSizeSet(dispW,	dispH);
	
	hwPlayBufAddrSet(ui32gPB);

	if( flag == EBOOK_DRAW_PREVIEW )   
	{
		err	= xdcfFileTypeGet(ui32FileIdx, &ui32Filetype);
		
	}
	else if (flag == EBOOK_DRAW_BOOKTEXT)
	{
	
		pbDramImgPlay(MenupEBook[EBOOKREAD][0], MenupEBook[EBOOKREAD][1], ui32gPB, dispW, dispH);

		osdClearScreen(0);
		pbEBookTextGet(); 
		
		if ( (TVFlag == 1 ) && (PANEL_TYPE_1 != getCurPanel()) )
		{
            		gpioSet(GPIO_LCD_POWER, 1);
			hwWait(0,300);
			panelSelect(PANEL_TYPE_1);
			sPara.TvoutFlag=TVOutOff;
			TVFlag=0;
			hwDispPvSetRead(imgBuf);
//			ChgPvSetting = 1;
		}
	}
	else if ( flag == EBOOK_DRAW_BG )
	{
		pbMainBack();
		if (ui32FileCnt == 0)
		{
			#if 0   // tzw add 
			hwImgCopyDo((UINT32)pPBAddr, pbThumbW, pbThumbH, 0, 0, pbThumbW, pbThumbH,
			ui32gPB, dispW,	dispH,pbThumbOffX,pbThumbOffY, 0);
			#endif

		}
		else
		{
			
			pbImagePaste( MenupEBook[EBOOKPICT][0], MenupEBook[EBOOKPICT][1], MenupEBook[EBOOKPICT][2],  MenupEBook[EBOOKPICT][3] );
		}	
	}
	

}



/***********************************************************
					刷新显示

*************************************************************/
//列表在2模式下选中后的一个更新函数
void pbEBookRefresh()  
{
	UINT32 err = SUCCESS;

	osdFWBuffChange();
	osdClearScreen(0x00);
	Pb_EbookTitle(GetBookString(TitleEBook));
	
	if (ui32FileCnt == 0)
	{
		pbEBookShow(EBOOK_DRAW_BG ,0 );
		
		osdStrDisp(170, 90, UserFont10x20, 0xd0, GetBookString(NoBook));
		osdHWBuffChange();
		#if 0  // fqdao_modify for bug press menukey  06.5.18 
	    	 osTimeDly(100);  
		 hwWait(0,1000);
		 osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);
		#else
		osTimeDly(100);  		
		menuReturn(UI_MAINMENU, 0);		
		#endif 
		
		return;
	}
	if ((err = xdcfCurFileByPosSet(ui32FileIdx)) != SUCCESS)
	{
		osdHWBuffChange();
		return;
	}
	err	= xdcfCurFileAttrGet( &xdcfFileAttr );// fqdao_bug for EBook Date  06.5.30
	err	= xdcfCurFileAttrGet( &fileattr );	
	err	= xdcfFileTypeGet( ui32FileIdx,  &ui32Filetype);
	
	
	//hisIconDisp();
	//hisTimeDisp();
	
	if(ui8DispType == PB_DISP_ONE)
	{
		pbEBookShow(1,ui32FileIdx);
	}
	else if ( ui8DispType == PB_DISP_FOUR )
	{
		//pbEBookShow(EBOOK_DRAW_BG ,0 );
		pbEBookInfoShow(ui32FileIdx);
   		pbEBookFileList(ui32FileIdx);
	}
	osdHWBuffChange();
}


/***********************************************************
						电子书主函数

*************************************************************/
void	EBookkeyGet(UINT32 *pKey)
{
	UINT32 err;
	UINT32 *msg;
	
	msg = osQuePend(uiKeyMsgQ,30,&err);
	if(err==OS_TIMEOUT) 
	{
		*pKey = 0xffffffff ;
	}
	else
	{
		*pKey = *msg;
	}
   	extern UINT32 atoCounter;
    atoCounter = 0;
	
	if (*pKey != UI_KEY_FUNC_FOCU)
	{
		if (((sPara.uiBeepFlag==BeepShutter) && (*pKey == UI_KEY_FUNC_SNAP)) || (sPara.uiBeepFlag==BeepOn))
		{
			if ((*pKey == UI_KEY_DIR_UP)||(*pKey == UI_KEY_DIR_DOWN)||(*pKey == UI_KEY_DIR_LEFT)||(*pKey == UI_KEY_DIR_RIGHT))
		   	{
				buttonAudio(1);
			}
			else if (*pKey == UI_KEY_FUNC_OK)
			{
		 	  	if (ui8DispType!= PB_DISP_ONE)
	 	  	 	{
					buttonAudio(2);
	 	  	 	}
			}
//			else if ((*pKey == UI_KEY_FUNC_MENU)||(*pKey == UI_KEY_FUNC_MODE))   /*||(*pKey == UI_KEY_FUNC_B)*/ 
			else if ((*pKey == UI_KEY_FUNC_MENU))//||(*pKey == UI_KEY_FUNC_MODE))   /*||(*pKey == UI_KEY_FUNC_B)*/ 
			{
				buttonAudio(3);
			}
		 }
	}
}

void EBookSetVolum(SINT32 value)
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
#if 1
void uiebook()
{
	UINT32	key = UI_KEY_MODE_EBOOK;
	UINT8     ui8EBookEXPName[4] = "TXT" ;
	UINT8     ui8EBooKTempExp1[4]= "tmp";
	UINT8     ui8EBookTempExp2[4]="emx";
	UINT32    err;
		
	semApp = osEventFindByName("APP_SEM");
	uiKeyMsgQ = osEventFindByName("UI_KEY");
	pbDispStart = 1; 
	ui32DirIdx = 0;
	ui32FileIdx = 0;
	while (  ( uiState & UI_MODE_MASK ) ==UI_MODE_EBOOK)
	{ 
		hisIconDisp();
		hisTimeDisp();
		if (ui32NextState != uiState)
		{
			break;
		}
		switch(key)
		{
			case	UI_KEY_DIR_UP:
				if( ui8DispType == PB_DISP_FOUR )
				{
					if(ui32FileIdx >1)
						ui32FileIdx --;
					else
						ui32FileIdx = ui32FileCnt;
					
					pbEBookRefresh();
				}
				else if ( ui8DispType == PB_DISP_ONE )
				{
					pbEBookPrePage();//上一页
				}
				break;

			case	UI_KEY_DIR_DOWN:
				if( ui8DispType == PB_DISP_FOUR )
				{
					if(ui32FileIdx < ui32FileCnt)
						ui32FileIdx ++;
					else
						ui32FileIdx = 1;
					
					pbEBookRefresh();
				}
				else if  (ui8DispType == PB_DISP_ONE)
				{
					pbEBookNextPage ();//下一页
				}
				break;
				
			case 	UI_KEY_ACCIDENT:
					osTimeDly(20);
					if (pui8ReadBuf)
					{
						osMemFree (pui8ReadBuf);
						pui8ReadBuf = NULL;
					}

					if ( pui32SavePage ) 
					{
						osMemFree (pui32SavePage);
						pui32SavePage = NULL;
					}

					if (handle)
					{
						vfsClose(handle);
						handle = 0;
					}
					
					if (ui8DispType == PB_DISP_ONE)
					{	
						ui8DispType = PB_DISP_FOUR;
						ui8EBookReadNow = 0;
						pbEBookRefresh();
						//fqdao_modify for bug  23    06.4.29  
						osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);					
					}

					break;
			case 	UI_KEY_FUNC_B:
				if( ui8DispType == PB_DISP_FOUR )  //fqdao_add 06.5.16
				{
					buttonAudio(1) ;
					paEBookMenuFunc();
				}
				break ;
				
			case	UI_KEY_DIR_LEFT:					
				break;
			case	UI_KEY_DIR_RIGHT:
				break;
/*
			case 	UI_KEY_FUNC_ZOOMIN:
				break ;
			case 	UI_KEY_FUNC_ZOOMOUT:
				break ;
*/				
			case	UI_KEY_FUNC_MENU:
				if( ui8DispType == PB_DISP_FOUR  )
				{
					if ( pui8ReadBuf )
					{
						osMemFree( pui8ReadBuf );
						pui8ReadBuf = NULL;
					}
					
					if ( pui32SavePage ) 
					{
						osMemFree( pui32SavePage );
						pui32SavePage = NULL;
					}

					if ( handle )
					{
						vfsClose(handle);
						handle = 0;
					}
					sub_menu_acc = 0;
//					UI_OSQFlush(uiKeyMsgQ);
					menuReturn(UI_MAINMENU, 0);															
					osTimeDly(40);
					return;
					break;
					
//					UI_OSQFlush(uiKeyMsgQ);
				}
				else if ( ui8DispType == PB_DISP_ONE )
				{	
					/* Paul@2006/05/29 add start */
					IsSaveBookmark();
					/* Paul@2006/05/29 add end */					

					#if 1
					ui8DispType = PB_DISP_FOUR;
					
					if ( pui8ReadBuf )
					{
						osMemFree(pui8ReadBuf);
						pui8ReadBuf = NULL;
					}
					if ( pui32SavePage ) 
					{	
						osMemFree( pui32SavePage );
						pui32SavePage = NULL;
					}
					ui8EBookReadNow = 0;
					
					if ( handle )
					{	
						vfsClose(handle);
						handle = 0;
					}
					
					//UI_OSQFlush(uiKeyMsgQ);
					pbInit();
					osdClearScreen(0);   //fqdao_add 06.5.19
					pbEBookShow(EBOOK_DRAW_BG ,0 );
					pbEBookRefresh();
					#endif
				}				
				break ;
				
		//		case	UI_KEY_FUNC_MODE:
		//			break;
					
	      		case	UI_KEY_FUNC_OK:
					if (ui32FileCnt != 0 &&  ui8DispType == PB_DISP_FOUR )
					{

						if(pui32SavePage==NULL)
						{
							pui32SavePage 	= osMemAlloc(4096);	
							if ( !pui32SavePage  )
							{
								break;
							}
						}
						
						handle = vfsOpen( xdcfFileAttr.name, O_RDONLY, S_IREAD);
						if ( !handle )
						{	
							break;
						}
						ui32FileSize= 	vfsFileSizeGet(handle);
						
						if ( ui32FileSize == 0 )
						{	
							osdStrDisp(160, 60, UserFont10x20, 0xd0, GetBookString(EmptyFile));
							osTimeDly(100);
							//vfsClose(handle);
							break;
						}
						#if 0
						if ( ui32FileSize < 1024*400 )
						{
							pui8ReadBuf = osMemAlloc( ui32FileSize );
							if ( pui8ReadBuf == NULL )
							{
								osMemFree(pui32SavePage);
								vfsClose(handle);
								break;
							}
							ui32ReadSize =  vfsRead(handle, pui8ReadBuf, ui32FileSize );
						}
						else
						{	
							pui8ReadBuf = osMemAlloc( 1024*400 );/*最大400k*/
							if ( pui8ReadBuf == NULL )
							{
								osMemFree(pui32SavePage);
								vfsClose(handle);
								break;
							}
							ui32ReadSize =  vfsRead(handle, pui8ReadBuf, 1024*400);
						}
						#endif
						ui8EBookReadNow = 1;
						//vfsClose(handle);
					
						ui8DispType 		= PB_DISP_ONE;
						*pui32SavePage 	= 0; 		/*第一页指向的0（开始位置)*/
						ui32FileIsEnd = 0;
						ui32CurPageFlag = 0;
						ui32PrePageFlag = 0;

						/* Paul@2006/05/29  add start */
						CheckCurBookmark();						
						/* Paul@2006/05/29  add end */
						
						osdClearScreen(0);
						pbEBookRefresh();

						osTimeDly(50);
					}
				break;
				
//			case	UI_KEY_FUNC_DISP:
//				break;

			case	UI_KEY_MODE_EBOOK:
				if ( ui8FirstInEBook )
				{	
					xdcfFileTypeAdd(ui8EBooKTempExp1);  // 128
					xdcfFileTypeAdd(ui8EBookTempExp2);  //256
					xdcfFileTypeAdd(ui8EBookEXPName);//512    相当于 1<<9   ( xDCF_FILETYPE_RESERVE2)
					ui8FirstInEBook = 0;
				}
				osTaskSuspend(osTaskFindByName("AAA"));
				sysgMemDispAddrGet(&ui32gPB);
				if ( pui32SavePage )
				{
					osMemFree( pui32SavePage );
					pui32SavePage = NULL;
				}

				if ( pui8ReadBuf )
				{
					osMemFree(pui8ReadBuf);
					pui8ReadBuf = NULL;
				}

				//添加目录名
				xdcfInit(imageDirNameStr, imageRootNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);												

				xdcfCurRootDirSet(otherRootNameStr);
				
				xdcfFilterSet( ui32FileFilter );  			//ui32FileFilter = xDCF_FILETYPE_RESERVE2； 
				hwWait(0,100);
			
				xdcfDirCountGet(&ui32DirCnt);	
				xdcfCurDirAttrGet(&xdcfDirAttr);
				xdcfFileCountGet(&ui32FileCnt);
				ui32FileIdx  = 1;
				if(ui32FileCnt == 0)
				{	
					ui32FileIdx  = 0;
				}
				ui8DispType 	=  PB_DISP_FOUR ;
				pbInit();
				pbEBookShow(EBOOK_DRAW_BG ,0 );

				pbEBookRefresh();
				break;
			default:
				break;
		}
		
		EBookkeyGet(&key);
	}
	
	if ( pui8ReadBuf )
	{
		osMemFree(pui8ReadBuf);
		pui8ReadBuf = NULL;
	}
	if ( pui32SavePage ) 
	{	
		osMemFree( pui32SavePage );
		pui32SavePage = NULL;
	}
	
	if ( handle )
	{	
		vfsClose(handle);
		handle = 0;
	}
	
	uiState = ui32NextState;

}

#endif


/* Paul@2006/05/29  add start */
#define BOOK_PROGRESS_X		(50+60)
#define BOOK_PROGRESS_Y     	60
#define BOOK_PROGRESS_W		4
#define BOOK_PROGRESS_H		100

#define BOOK_DLG_OFF_X			20
#define BOOK_DLG_OFF_Y			40
#define BOOK_DLG_WIDTH			280
#define BOOK_DLG_HEIGHT		100
#define COLOR_BOOK_DLG_BK		0x02
#define COLOR_BOOK_DLG_SEL  	0xd3
#define COLOR_BOOK_DLG_NSEL  	0xd2
/**
**/
UINT32 CheckCurBookmark(void)
{
	UINT32 key;
	UINT32 ui32SelectIdx = 0;
	
	if (sPara.ui8BookmarkFlag == 0xaa)
	{
		if (!memcmp(xdcfFileAttr.name, sPara.ui8BookName, 13))
		{
			key = UI_KEY_MODE_EBOOK;
			while ((uiState & UI_MODE_MASK) == UI_MODE_EBOOK)
			{				
				switch (key)
				{
					case UI_KEY_DIR_LEFT:
					case UI_KEY_DIR_RIGHT:
						ui32SelectIdx = ui32SelectIdx ?  0 : 1;						
						osdStrDisp(BOOK_DLG_OFF_X+40, BOOK_DLG_OFF_Y+70, UserFont10x20, ui32SelectIdx ? COLOR_BOOK_DLG_NSEL : COLOR_BOOK_DLG_SEL, GetBookString(Book_yes_s));
						osdStrDisp(BOOK_DLG_OFF_X+140, BOOK_DLG_OFF_Y+70, UserFont10x20, ui32SelectIdx ? COLOR_BOOK_DLG_SEL : COLOR_BOOK_DLG_NSEL, GetBookString(Book_no_s));
						break;

					case UI_KEY_FUNC_OK:
						if (ui32SelectIdx == 0)
						{
							g_ui8LoadBookmark = 1;						
						}
						sPara.ui8BookmarkFlag = 0xff;
						nandWriteParameters();
						LoadingBookmark();
						return;
					
					case UI_KEY_MODE_EBOOK:
						osdBarDraw(BOOK_DLG_OFF_X, BOOK_DLG_OFF_Y, BOOK_DLG_WIDTH, BOOK_DLG_HEIGHT, COLOR_BOOK_DLG_BK);
						osdStrDisp(BOOK_DLG_OFF_X+10, BOOK_DLG_OFF_Y+20, UserFont10x20, 0xd2, GetBookString(IsLoadmark_s));
						osdStrDisp(BOOK_DLG_OFF_X+40, BOOK_DLG_OFF_Y+44, UserFont10x20, 0xd2, GetBookString(IsLoadmark_s1));
						osdStrDisp(BOOK_DLG_OFF_X+40, BOOK_DLG_OFF_Y+70, UserFont10x20, ui32SelectIdx ? COLOR_BOOK_DLG_NSEL : COLOR_BOOK_DLG_SEL, GetBookString(Book_yes_s));
						osdStrDisp(BOOK_DLG_OFF_X+140, BOOK_DLG_OFF_Y+70, UserFont10x20, ui32SelectIdx ? COLOR_BOOK_DLG_SEL : COLOR_BOOK_DLG_NSEL, GetBookString(Book_no_s));
						break;
						
					default:
						break;
				}
				
				if (ui32NextState != UI_MODE_EBOOK)
				{
					break;					
				}
				
				keyGet(&key);
			}
		}
	}
		
	return 1;
}

/**
**/
void LoadingBookmark()
{
	UINT32* pkey;
	UINT32 err;
	UINT32 i = 0;
	
	if (g_ui8LoadBookmark)
	{
		printf("Next Page = %d\n", sPara.ui32BookCurPage);
		osdBarDraw(BOOK_DLG_OFF_X, BOOK_DLG_OFF_Y, BOOK_DLG_WIDTH, BOOK_DLG_HEIGHT, COLOR_BOOK_DLG_BK);
		
		if (sPara.ui32BookCurPage)
		{
			while (sPara.ui32BookCurPage != ui32CurPageFlag)
			{
				pkey = osQuePend(uiKeyMsgQ, OS_NO_WAIT, &err);
				if ((*pkey & UI_MODE_MASK) 
				  ||(*pkey == UI_KEY_ACCIDENT))
				{
					break;
				}			
				pbEBookTextGet();						
				ui32PrePageFlag = ui32CurPageFlag++;								
				i = ui32CurPageFlag*100/sPara.ui32BookCurPage;
				osdFrameDrawX(BOOK_PROGRESS_X, BOOK_PROGRESS_Y-2, BOOK_PROGRESS_H+4, BOOK_PROGRESS_W+4, 0x0d, 2);
				if (i != 0)
				{
					osdBarDraw(BOOK_PROGRESS_X, BOOK_PROGRESS_Y, i, BOOK_PROGRESS_W, 0x01);	
				}	
			}
		}
		g_ui8LoadBookmark = 0;
	}
}

/**
**/
void IsSaveBookmark()
{
	UINT32 key = UI_KEY_MODE_EBOOK;
	UINT32 ui32SelectIdx = 0;

	while ((uiState & UI_MODE_MASK) == UI_MODE_EBOOK)
	{
		switch (key)
		{
			case UI_KEY_DIR_LEFT:
			case UI_KEY_DIR_RIGHT:
				ui32SelectIdx = ui32SelectIdx ?  0 : 1;						
				osdStrDisp(BOOK_DLG_OFF_X+40, BOOK_DLG_OFF_Y+70, UserFont10x20, ui32SelectIdx ? COLOR_BOOK_DLG_NSEL : COLOR_BOOK_DLG_SEL, GetBookString(Book_yes_s));
				osdStrDisp(BOOK_DLG_OFF_X+140, BOOK_DLG_OFF_Y+70, UserFont10x20, ui32SelectIdx ? COLOR_BOOK_DLG_SEL : COLOR_BOOK_DLG_NSEL, GetBookString(Book_no_s));
				break;

			case UI_KEY_FUNC_OK:
				if (ui32SelectIdx == 0)
				{
					sPara.ui8BookmarkFlag = 0xaa;
					memcpy(sPara.ui8BookName, xdcfFileAttr.name, 13);
					sPara.ui32BookCurPage = ui32CurPageFlag;
					nandWriteParameters();
				}
				return;
				
			case UI_KEY_MODE_EBOOK:
				osdBarDraw(BOOK_DLG_OFF_X, BOOK_DLG_OFF_Y, BOOK_DLG_WIDTH, BOOK_DLG_HEIGHT, COLOR_BOOK_DLG_BK);
				osdStrDisp(BOOK_DLG_OFF_X+10, BOOK_DLG_OFF_Y+20, UserFont10x20, 0xd2, GetBookString(IsSavemark_s));
				osdStrDisp(BOOK_DLG_OFF_X+40, BOOK_DLG_OFF_Y+44, UserFont10x20, 0xd2, GetBookString(IsSavemark_s1));
				osdStrDisp(BOOK_DLG_OFF_X+40, BOOK_DLG_OFF_Y+70, UserFont10x20, ui32SelectIdx ? COLOR_BOOK_DLG_NSEL : COLOR_BOOK_DLG_SEL, GetBookString(Book_yes_s));
				osdStrDisp(BOOK_DLG_OFF_X+140, BOOK_DLG_OFF_Y+70, UserFont10x20, ui32SelectIdx ? COLOR_BOOK_DLG_SEL : COLOR_BOOK_DLG_NSEL, GetBookString(Book_no_s));
				break;

			default: 
				break;				
		}

		if (ui32NextState != UI_MODE_EBOOK)
		{
			break;
		}

		keyGet(&key);
	}	
}
/* Paul@2006/05/29  add end */
