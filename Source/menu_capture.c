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
 *  2nd Floor, R2-B Buliding, Hi-tech.S, 7th Road,                        *
 *  High-tech Industrial Park, Shenzhen, China                            *
 *                                                                        *
 *  Author: Wallace Zhang                                                 *
 *                                                                        *
 **************************************************************************/
//#define RS_232_DBG 

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "general.h"
#include "sysg_api.h"
#include "os_api.h"
#include "xiaoao.h"
#include "ui_main.h"
#include "q_table.h"
#include "aaa_api.h"
#include "menu_capture.h"
#include "hw_cdsp.h"
#include "asf_api.h"
#include "card.h"
#include "vfs_api.h"
#include "dcf_api.h"
#include "osd_api.h"
#include "panel.h"
#include "osd_common.h"

#include "dpof_api.h"
#include "dpof_event.h"

#include "usb_api.h"
#include "dps_paramdef.h"
#include "dps_api.h"
#include "dps_custom.h"
#include "gpio.h"

//#include "pict_ddx.h"
#include "extern.h"
//#include "icon_dc.h"


/* QTIdx constant */
#define CAPTURE_SUPER_QTIDX	8//	2
#define CAPTURE_FINE_QTIDX		10//	4
#define CAPTURE_NORMAL_QTIDX	12//	6

#define VIDEO_SUPER_QTIDX			8
#define VIDEO_FINE_QTIDX			7
#define VIDEO_NORMAL_QTIDX		3

/* Menu using color */
#define CR_TITLE_ARROW		0xde//0xd8
#define CR_SELECT_TEXT		0xde//0xd8
#define CR_NORMAL_TEXT		0xf0//0xd0
#define CR_SELECT_ICON		0xde//0xd8
#define CR_NORMAL_ICON		0xd0//0xd0
#define CR_TITLE_SEL_TEXT   0x3e//0xd8
#define CR_TITLE_NOR_TEXT   MENUTITLES//0xd0
#define CR_SELECT_BAR		0x0e//0x08

#define CR_SEL_BUTTON		0xbe//0xB8
#define CR_NOR_BUTTON		0xb0//0xB0

#define CAP_MENU_WIDTH		190
#define CAP_MENU_HEIGHT		100
#define CAP_MENU_X			MENUTEXT_X-10 
#define CAP_MENU_Y			32

#define CAP_TEXT_HEIGHT     MENUTEXT_Y_OFF
#define CAP_TEXT_OFF_X      MENUTEXT_X 
#define CAP_TEXT_OFF_Y      MENUTEXT_Y-30 
#define CAP_BAR_X			MENUBAR_X
#define CAP_BAR_Y			MENUBAR_Y-30
#define CAP_BAR_WIDTH       MENUBAR_W
#define CAP_BAR_HEIGHT      MENUBAR_H

#define CAP_ICON_OFF_X      (CAP_MENU_X + 0)
#define CAP_ICON_OFF_Y      CAP_TEXT_OFF_Y-4

#define CAP_TITLE_ICON_X	MENUTITLEX
#define CAP_TITLE_ICON_Y	MENUTITLEY
#define CAP_TITLE_TEXT_X	MENUTITLEX
#define CAP_TITLE_TEXT_Y	MENUTITLEY

#define MENU_TIP_X			64
#define MENU_TIP_Y			PANEL_1_OSD_H-24
#define MENU_TIP_FONT_W		10
#define MENU_TIP_FONT_H		20

#define MENU_EV_X			(PANEL_1_OSD_W-4*16)/2
#define MENU_EV_Y           (PANEL_1_OSD_H-20)/2


#define OPEN_DLG_OFF_X			60
#define OPEN_DLG_OFF_Y			40
#define OPEN_DLG_WIDTH			220
#define OPEN_DLG_HEIGHT			100
#define COLOR_OPEN_DLG_BK		0x02
#define COLOR_OPEN_DLG_SEL  	0xd3
#define COLOR_OPEN_DLG_NSEL  	0xd2
/**************************************************************************
 *                         Function Declaration                           *
 **************************************************************************/
void DisplayMenuText(UINT16 ui16StartIdx, UINT8 ui8TopIdx, UINT8 ui8SelOKIdx, UINT8 ui8SelIdx, UINT8 ui8TotalCnt);

void menuDeleteInit(UINT32 key);
void menuDcProtectInit(UINT32 key);
void menuNoProtectInit(UINT32 key);
void menuPrintInit(UINT32 key);
void menuDpofInit(UINT32 key);
void menuAutoInit(UINT32 key);
void menuOpenInit(UINT32 key);

UINT32 menuDeleteProc(UINT32 key);
UINT32 menuProtectProc(UINT32 key);
UINT32 menuNoProtectProc(UINT32 key);
//UINT32 menuPrintProc(UINT32 key);
//UINT32 menuDpofProc(UINT32 key);
UINT32 menuAutoProc(UINT32 key);
UINT32 menuOpenProc(UINT32 key);	

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
typedef UINT32 (*PMENUPROC)(UINT32 key);
typedef void (*PMENUINIT)(UINT32 key);
UINT8 flag;

#define C_PREVIEW_MENUS		0x08

PMENUINIT menuPvInit[C_PREVIEW_MENUS] = 
{
	menuAWBInit,
	menuDateStampInit,
	menuImageSizeInit,
	menuPictureQualityInit,
	menuEffectInit,
	menuSelfShotInit,
	menuCaptureModeInit,
	menuEVSetInit
};

PMENUPROC menuPvProc[C_PREVIEW_MENUS] = 
{
	menuAWBSet,
	menuDateStampSet,
	menuImageSizeSet,
	menuPictureQualitySet,	
	menuEffectSet,
	menuSelfShotSet,
	menuCaptureModeSet,	
	menuEVSet
};

#define C_MOVIE_MENUS		0x05
UINT8 ui8MvMenuIdx = 0;

PMENUINIT menuMvInit[C_MOVIE_MENUS] = 
{
	menuAWBInit,	
//	menuVideoSizeInit,
	menuVideoQualityInit,
	menuEffectInit,
	menuMvSelfShotInit,	
	menuMvEVSetInit
};

PMENUPROC menuMvProc[C_MOVIE_MENUS] = 
{
	menuAWBSet,
//	menuVideoSizeSet,
	menuVideoQualitySet,
	menuEffectSet,
	menuMvSelfShotSet,		
	menuMvEVSet
};

#define C_PBIMAGE_MENU_NUM		5
/**
	Image Playback Menu
**/
PMENUINIT osdMenuPhotoInit[C_PBIMAGE_MENU_NUM] = 
{
	menuDeleteInit,
	menuDcProtectInit,
	menuNoProtectInit,
	menuAutoInit,
	menuOpenInit	
};

PMENUPROC osdMenuPhotoProc[C_PBIMAGE_MENU_NUM] = 
{
	menuDeleteProc,
	menuProtectProc,
	menuNoProtectProc,
	menuAutoProc,
	menuOpenProc		
};

#define C_PBMOVIE_MENU_NUM		0x03

PMENUINIT osdMenuMovieInit[C_PBMOVIE_MENU_NUM] =
{
	menuDeleteInit,
	menuDcProtectInit,
	menuNoProtectInit,
};

PMENUPROC osdMenuMovieProc[C_PBMOVIE_MENU_NUM] =
{
	menuDeleteProc,
	menuProtectProc,
	menuNoProtectProc,
};

#define MENU_ICON_IMAGE_OFF		0x00
#define MENU_ICON_MOVIE_OFF		C_PBIMAGE_MENU_NUM
#define MENU_ICON_DC_OFF		(C_PBIMAGE_MENU_NUM+C_PBMOVIE_MENU_NUM)
#define MENU_ICON_DV_OFF		(C_PBIMAGE_MENU_NUM+C_PBMOVIE_MENU_NUM+C_PREVIEW_MENUS)
 
UINT8  QTidx = QTIDX_Q95;
UINT8  *Q_Table;
SINT32 sizedifference = 0;
SINT32 fragment = 0;
UINT32 Qtabledir = 0;
UINT32 delfragment = 0;

static UINT8 ui8PvMenuIdx = 0;
static UINT8 ui8SubPvMenuIdx;

/* Paul@2005/11/03-11:37 add */
//static UINT8 uiEvDisplay[6] = 
//	{20, 16, 13, 10, 6, 3};
static UINT8 uiEvDisplay[5] = 
	{15, 12, 9, 6, 3};

static UINT8 g_ui8TopIdx;
static UINT8 g_ui8SelOKIdx;
static UINT8 g_ui8TotalItem;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 ManualWbFlag;
extern UINT32 SharpnessFlag;

extern xdcfAttrElm_t fileattr;
extern xdcfAttrElm_t dirattr;
extern UINT32 fileIdx;
extern UINT32 fileCnt;
extern UINT32 LightFreq;
extern UINT32 DefaultRGB[];
extern UINT32 gPB;
#include "test.c"
#if 1
#if 0//LANGUAGES  268 269 same
UINT8 *dc_UiStrings[DC_MAX_ID][LANGUAGES] = {

{
"No Video",
"û����Ƶ",
"�S�����W",
"Sin v��deo",//spanish
"���֧� ���ڧէ֧�",//"���������ӧ�֧� �ӧڧէ֧�-��ѧ�����",	
"ohne Video",
"pas de vid��o",
"No video",
"N\xe3o video",
"���� ����",
},
{
"No Image",
"û��ͼƬ",	
"�S���Ϥ�",	
"Sin imagen",
"���֧� �ڧ٧�ҧ�ѧا֧ߧڧ�",	
"Kein Bild",
"pas de image",
"No Immagine",
"N\xe3o h�� imagens",
"���� ����",
},
{
"MENU",
"�˵�",	
"���",	
"Men��",
"���֧ߧ�",	
"Men��",
"menu",
"Menu",
"Menu",
"�޴�",
},
{
"Setting",
"����",	
"�]�m",		
"Ajuste",
"�����ѧߧ�ӧܧ�",
"Spielfestsetzung",//"Setup",
"r��glage",
"Impostazione",
"Instala\xe7\xe3o",
"���� ��ġ",
},
{
"Volumn",
"����",
"���q",	
"Volumen",
"�����ާܧ����",
"Lautst\xe4rk",
"vol",
"Vol",
"Volume",
"����",
},
{
"OFF",
"��",
"��",
"Apagar",
"����ܧ�",
"Zu",//"Nein",
"non",//"off",
"OFF",
"Cerrar",//"N\xe3o",
"OFF",
},
{
"ON",
"��",
"�}",
"Encender",
"���ܧ�",
"Auf",//"Ja",
"oui",//"On",
"On",
"Abrir",//"Sim",
"ON",
},
{
"Capture Mode ",
"���㷽ʽ",	
"����覡",	
"Modo de Capturar",//"Modo de fotografiar",
"����է֧ݧ� �٧ѧ��֧ާܧ� ",//092"�����ާ� �����ԧ�ѧ�ڧ��ӧѧߧڧ�",	
"Aufnahme Modell",
"mode capture",
"Modo foto",
"Capture Mode ",//"Modelo de fotografia",
"�Կ� ���",
},
{
"Single Shot",
"����",
"���", 
"Fotograf��a singular",
"���էߧ�ܧ�ѧ�ߧ�",
"Einzelaufnahme",//"Einzelfotoaufzeichnung",
"une prise",//"une par fois",
"Singolo",
"Um tiro",
"���� ���",
},	 		
{
"Multi Shot",
"����",	
"�s��",	
"Fotograf��a sucesiva",
"����֧�֧է�",
"Fotolaufendaufnahme",//"Fortlaufendfotoaufzeichnung",
"plusieurs prises",//"plusieurs par fois",
"Continuoso",
"Tiro sucessivamente",
"������ ���",
},
{
"Self Shot",
"����",
"�۩�",
"Fotograf��a auto",//autom��tica",
"���ӧ��.�����",
"Automatik",
"retardateur",//"ratardatuer",
"Foto.stesso",
"Tiro para si pr��prio",
"�ڵ����� ���",
},	 		
{
"White Balance ",
"��ƽ��",	
"�ե���",	
"Balance blanco",
"���ѧݧѧߧ� �ҧ֧ݧ�ԧ�",
"Weissabgleich",
"balance du blanc",
"Bilancia bianca",
"Equil��brio branco",
"ȭ��Ʈ �뷱��",
},
{
"Auto",
"�Զ�",	
"�۰�",	
"Autom��tico",
"���ӧ��",
"Auto",
"auto",
"Auto",
"Auto",
"�ڵ�",
},
{
"Daylight",	
"����",	
"����",	
"Luz del d��a",
"���ߧ֧ӧߧ�� ��ӧ֧�",
"Sonnenlicht",
"soleil",
"Luce ",
"Luz do dia",
"�ϱ�",
},
{
"Cloudy",
"����",
"����",	 
"Nublado",
"���ѧ�ާ��ߧ����",
"bew\xf6lkt",
"nuageux",
"Scuro",
"Nublado",
"�帰 ����",
},
{
"Tungsten",
"�׳��",	
"�տK�O",		 
"Tungsteno",
"���ѧާ��",
"Lamp",
"lampe",
"Lampadina",
"Tungst��nio",
"����",
},
{
"Fluorescent",	
"ӫ���",
"�����O",
"Fluorescente",
"����ާڧߧ����",
"Fluoreszenzlicht",
"Fluorescent",
"Lampada",
"Fluorescente",
"������",
},
{
"Resulotion ",
"�ֱ��� ",
"����v ", 
"Resoluci��n",
"���ѧ٧�֧�֧ߧڧ�",
"Aufl\xf6sung",
"r��solution",
"Risoluzione",
"Resulo\xe7\xe3o",
"�ػ�",
},
{
"Size ",
"��С",	
"�j�p",		 
"Tama?o",
"���ѧ٧ާ֧�",
"Gr\xf6\xdfe",
"taille",
"Misura",
"Tamanho",
"������",
},
{
"Flash Mode",
"����ģʽ",	 
"�{���Ҧ�",	 
"Modo de flash",
"����է֧ݧ� �ӧ����ܧ�",
"Blitzmodus",
"mode flash",
"Lampo modo",
"Modelo de flash",
"�÷��� ���",
},
{
"Force On",	 	
"ǿ��",	
"�j��",		 
"Forzar flash",
"����ڧߧ�էڧ�֧ݧ�ߧѧ� �ӧ����ܧ�",
"Zwangsblitz",
"toujours flash",
"Forza on",
"Modelo de flash for\xe7ado",
"���� �÷���",
},
{
"Force Off",
"�ر�",	
"����",		 
"Apagar flash",
"����ܧ�.�ӧ����ܧ�",
"Blitz-Aus",
"pas de flash",
"Forza off",
"Modelo de n\xe3o flash",
"�÷��� ����",
},
{
"Red Eye Redutions",	
"������",	
"������",		 
"Reducci��n de Ojo Rojo",
"���ѧ�ڧ�� ��� �ܧ�ѧ�ߧ�ԧ� �ԧݧѧ٧�",
"Rotaugenschutz",
"r��duction yeux rouge",
"Occhi rossi reduzione",
"Redu\xe7\xe3o de olhos vermelhos",
"������� ����",
},
{
" Quality ",
"ͼ������ ",	
"�Ϲ���q ",		 
"Calidad de imagen",
"�ܧѧ�֧��ӧ�",//"���ѧ�֧��ӧ� �ڧ٧�ҧ�ѧا֧ߧڧ�",
"Qualit\xe4t",
"qualit��",
"Qualit��",
"Qualidade",
"ȭ��ǰ��",
},
{
"Super",
"����",	
"�u��",		 
"Excelente",
"����ݧڧ�ߧ�",
"Super",
"super",
"Superiore",
"Super",
"����",
},
{
"Fine",
"��׼",
"�з�",	 
"Fina",
"����ѧߧէѧ��",
"Standard",
"meilleur",//"standard",
"Standard",
"Bom",
"����",
},
{
"Normal",
"һ��",
"�@��",	 
"Normal",
"�����ާѧݧ�ߧ�",
"Normal",
"normal",
"Normale",
"Normal",
"�븻",
},
{
"Data Stamp",
"����ʱ��",
"����ɶ�",	 
"Fecha/Tiempo",
"���ѧ��/�ӧ�֧ާ�",
"Datum/Zeit",
"date/heure",
"Data/tempo",
"Data",
"����/�ð�",
},
{
"Effect",
"Ч��",
"�ĪG",	 
" Efecto",
"�����֧ܧ�",
"Wirkung",
"effet ",
"Effetto",
"Efeito",
"ȿ��",
},
{
"Normal",
"����",
"���`",	 
"Normal",
"�����ާѧݧ�",
"Normal",
"normal",
"Normale",
"Normal",
"����",
},	 
{
"Sepia",
"����",
"�_�j",
"Sepia",
"���֧�ڧ�",
"Sepia",
"s��pia",//"pass��",
"Classico",
"Foto antiga",
"���Ǿ�",
},
{
"B&W",
"�ڰ�",
"�¥�",	 
"Negro/blanco",
"���֧��/�ҧ֧�",//092	"���֧�ߧ�-�ҧ֧ݧѧ� �����",
"Schwarz&Wei\xdf",//"Preto e branco",
"noir et blanc",//"blanc et noir ",
"B&N",
"Preto e branco",
"���",
},
{
"Negative",
"����",
"�t��"	 ,
"Negativo",
"���֧ԧѧ�ڧ�",
"Negativo",
"N��gatif",//"N��gative",
"Negativa",
"Negativo",
"����",
},
{
"EV Adjust",
"�ع����",	
"�n���վ�",		 
"Ajuste de EV",	//"Ajuste de exposici��n",
"����֧ԧ�ݧڧ��ۧ��",//	"���֧ԧ�ݧڧ��ӧܧ� ��ܧ���٧ڧ�ڧ�",
"Belichtung",//"Belichtungssteuerung",
"ajust exposition",
"EV Aggiustare",
"EV adjustar",
"�ͽ����� ����",
},
{
"EV ",
"�ع�ֵ",	
"�n����",
"EV",//"Valor exposici��n",
"EV",//"���ߧѧ�֧ߧڧ� ��ܧ���٧ڧ�ڧ�",
"EV",//"Belichtungszahl",
"EV",//"valeur exposition",
"EV",
"EV",
"�ͽ����� ",
},
{
"Ray Freq",           //Frequency     6
"��ԴƵ��",
"�����W�v",
"Frecuencia de luz",//"Frecuencia de rayo",
"���ѧ����� ��ӧ֧��",
"Licht Freq",
"Frq Ray ",
"Luce Freq",
"Frequ��ncia de luz", 
"����Ʈ�ҽ� ��������",
},
{
"Exit Game",	
"�˳���Ϸ",	 
"�h�X����",	 	 
"Salir juego",
"������� �ڧ� �ڧԧ��",
"Spiel beendet",
"quit jeux ",
"Usce gioco",
"Sair de jogos",
"��������",
},
{
"Output Mode",
"���ģʽ",
"��X�Ҧ�",	 
"Modo de salida",
"����է֧ݧ� �ӧ�ӧ�է�",
"Ausgabe Modus",
"���ģʽ",//"mode output",
"Modo esporta",
"Modelo de sa��da",
"��� ���",
},
{
"No Game!",	
"û����Ϸ!",	
"�S������!",		 
"Sin juego",
"���֧� �ڧԧ��",
"kein Spiel",
"pas de jeux",
"No gioco",
"N\xe3o h�� jogo",
"���� ����",
},
{
"Lum",		 	
"����",
"�G��",
"Brillo",
"���ӧ֧�ݧ���",
"Helligkeit",
"luminosit��",
"Brillanza",
"luz",
"���",
},
{
"Exit",
"�˳�",
"�h�X",
"Salir juego",
"������� �ڧ� �ڧԧ��",
"beendet",
"quit",
"Usce",
"Sair",
"����",
},
{
"Night Mode",
"ҹ��ģʽ",
"�]���Ҧ�",
"Modo de noche",
"�����ߧѧ� �ާ�է֧ݧ�",
"Nacht Modell",
"mode nuit",
"Modo di notte",
"Modelo de noites",
"�߰� ���",
},
{
"OFF",
"��",
"��",
"Apagar",
"����ܧ�",
"Zu",//"Nein",
"non",//"off",
"OFF",
"Cerrar",//"N\xe3o",
"OFF",
},
{
"ON",
"��",
"�}",
"Encender",
"���ܧ�",
"Auf",//"Ja",
"oui",//"On",
"On",
"Abrir",//"Sim",
"ON",
},
{
"Self Timer",
"����ʱ��",
"�۩�ɶ�"	,	
"Temporizador",//092//	"Contador tiempo ",//"Contador de tiempo fotograf��a autom��tica",
"���ӧ�� �ӧ�֧ާ�",//092//"����֧ާ� �ѧӧ������ԧ�ѧ�ڧ�ӧ�ѧߧڧ�",
"Autoaufnahmezeit",
"retardateur",//"Self Timer",//"temps de ratardateur",
"Auto.foto",
"Self Timer",//"Tempo de instala\xe7\xe3o de auto fotografar",
"�ڵ�����",
},
{
"2s",
"2s",	
"2s",
"2  segundos",
"2  ��֧�.",
"2s",
"2s",
"2s",
"2 segundos",
"2��",
},
{
"5s",
"5s",
"5s",
"5  segundos",
"5  ��֧�.",
"5s",
"5s",
"5s",
"5 segundos",
"5��",
},
{
"10s",
"10s",	
"10s",
"10 segundos",
"10 ��֧�.",
"10s",
"10s",
"10s",
"10 segundos",
"10��",
},
{
"15s",
"15s",	
"15s",
"15 segundos",
"15 ��֧�.",
"15s",
"15s",
"15s",
"15 segundos",
"15��",
},
{
"File Format",
"�ļ���ʽ",	 	
"���榡",	 		 
"Formato de archivo",
"�����ާѧ� ��ѧۧݧ�",
"Dateiformat",
"format du fichier",//"format du dossier",
"Doc.forma",
"Formular de documento",
"��������",
},
{
"Frame Rate",
"֡Ƶ",
"�V�W",	 
"Frecuencia del cap��tulo",
"���ѧ����� ���֧ާܧ�",
"Bildausschnittrate",
"nombre d'images",//"Taux d'armature",
"Freq.Regime",
"Taxa de frame",
"������ ����Ʈ",
},
{
"15 FPS",
"15 ֡/��",	
"15 �V/��",		 
"15 FPS",
"15 �ܧѧէ���",
"15 FPS",
"15 FPS",
"15 FPS",
"15 FPS",
"15������",
},
{
"30 FPS",
"30 ֡/��",	
"30 �V/��",		 
"30 FPS",	
"30 �ܧѧէ���",
"30 FPS",
"30 FPS",
"30 FPS",
"30 FPS",
"30������",
},
{
"Delete",
"ɾ��",
"�R��",
"Borrar",
"���էѧݧڧ��",
"Entfernen",
"supprimer",
"Cancel",
"Deletar",
"����",
},
{
"Protect"	,
"����"	,
"�O�@"	,
"Proteger",
"���ѧ�ڧ�ڧ��",
"Schutz",
"prot��ger",
"Protezione",
"Protectar",
"��ȣ",
},
{
"Unprotect"	,
"ȡ������"	,
"�����O�@"	,
"Unproteger",//"Cancelar protecci��n",
"����ާ֧ߧڧ�� �٧ѧ�ڧ��",
"Schutzentfernen",
"non prot��ger ",
"Non protezione	",
"Fora de protec\xe7\xe4o",
"��������Ʈ",
},
{
"Print",
"ֱ�Ӵ�ӡ",
"�������L",	
"Imprimir",//"Impresi��n directa",
"�����ާ� ��֧�ѧ�ѧ��",
"Abdruck",
"imprimer",
"Stampare",
"Imprimir",
"���� ����Ʈ",
},
{
"DPOF",
"DPOF"	,
"DPOF"	,	
"DPOF",
"DPOF",
"DPOF",
"DPOF",
"DPOF",
"DPOF",
"DPOF",
},
{
"Slide Show",
"�Զ�����",	
"�۰ʼ���",		
"Diapositiva activa",//"Reproducci��n auto",//"Reproducci��n autom��tica",
"���ӧ�� ����ڧԧ��ӧѧߧڧ�",//092//"���ӧ��ާѧ�ڧ�֧�ܧ�� ����ڧԧ��ӧѧߧڧ�",
"Automatik ",
"Slide Show",//"lecture automatique",
"Play auto",
"P\xf5r slide",
"�ڿ� �÷���",
},
{
"Open Picture",
"��������",	
"�}���e��",		
"Imagen de inicio",
"����ܧ����� ��ܧ�ѧ�",//092//"�����ܧ�ӧ�� �ڧ٧�ҧ�ѧا֧ߧڧ�",
"Gru\xdfimage",
"l'��cran d'acceuil",
"Pitt. avv",
"Imagen de liga\xe7\xe3o",
"��ŸƮȭ��",
},
{
"One",
"����"	,
"��i"	,	
"Una",
"���էߧ�",
"Einzel",
"seule",
"Uno",
"Um",
"����",
},
{
"Current",
"��ǰ",
"��e",
"Actual",
"����ߧ��",
"Aktuell",
"pr��sent",
"Attuale",
"Corrente",
"����",
},
{
"All",
"ȫ��",
"����",
"Todo",
"�����",
"Alles",
"tous ",
"Tutti",
"Todos",
"����",
},
{
"Print One",
"��ӡ����",	
"���L��i",		
"Imprimir una",
"�����ާৰ�էߧ�",
"Einzelandruck",
"imprimer une page",
"Stampare singolo",
"Imprimir uma folha",
"���� ����Ʈ�ϱ�",
},
{
"Print All",
"��ӡȫ��",	
"���L����",		
"Imprimir todas",
"�����ާৣ���",
"Allesdruck",
"imprimer tous",
"Stampare tutti",
"Imprimir todas as folhas",
"���θ� ����Ʈ�ϱ�",
},
{
"DPOF Set One",
"DPOF ���õ���",	
"DPOF �]�m��i",		
"DPOF ajustar uno",
"��էڧ�DPOF",
"DPOF Einzeleinstellung",
"DPOF 1x ",//"DPOF r��gle une ",
"DPOF Imposta Uno",
"DPOF instalado um",
"DPOF�� ���� ��ġ",
},
{
"DPOF Set All",
"DPOF ����ȫ��",	
"DPOF �]�m����",		
"DPOF Ajustar todo",
"�ӧ��DPOF",
"DPOF Einstellung",
"DPOF tous",//"DPOF r��gle toutes",
"DPOF Imposta Tutto",
"DPOF para todos",
"DPOF�� ���� ��ġ",
},
{
"OK",
"ȷ��",	
"�T�w",		
"Confirmar",
"����",
"ok",
"OK",//"confirmer",
"OK",
"OK",
"Ȯ��",
},
{
"Cancle",
"ȡ��",	
"����",		
"Cancelar",
"����ާ֧ߧ�",
"Abbrechen",
"abandon",//"supprimer",
"Cancel",
"Cancelar",
"ĵ��",
},
{
"Slide End ",			
"�õ�Ƭ����",
"�ۿO������",	
"Terminaci��n de diapositiva",
"���ѧܧ�ߧ�ڧ�� �էڧѧ�ڧݧ��",
"Dia Enden",
"fin de pr��sentation",
"Finisce Dia",
"Termina a jogar slide",
"�����̵� ����",
},
{
"PC Mode",
"PC ģʽ",	
"PC �Ҧ�",		
"Modo de PC",
"����է֧ݧ� ����",
"PC Modus",
"mode PC",
"PC modo",
"Modelo de pc",
"PC ���",
},
{
"Connect PC",
"���ӵ���",	
"�s���q��",	
"Conectar PC",
"����֧էڧߧ֧ߧڧ� �ܧ�ާ����֧��",
"PC Verbindung",
"conect de PC",
"Connettere PC",
"Donectar PC",
"��ǻ�Ϳ� ����",
},
{
"PC CAMERA",
"����ͷ",	
"�ṳ�Y",		
"Renovar programa",
"���ҧߧ�ӧڧ�� ����ԧ�ѧާާ�",
"Frisch",
"renouvler logiciel",
"PC camera",
"Programa renovado",
"��������",
},
{
"MASS STORAGE",
"�ƶ�Ӳ��",	
"���ʵw�L",		
"MEMORIA DE MASA ",
"���֧�֧ߧ��ߧ�� �էڧ��",
"Mobilfestplatte",
"disque dur mobil",
"Disco mobile",
"Disco m��vel",
"�Ž� ���丮��",
},
{
"Connect Print...",
" ���Ӵ�ӡ��...",	
" �s�����L��...",		
"Conectar impresi��n ...",
"����֧էڧߧ֧ߧڧ� ���ڧߧ�֧�� ...",
"Abdruck verbindet",
"connecter imprimante",
"Connetter Stampante",
"Eonecx\xe3o de impressora",
"�����Ϳ� ����",
},
{
"Printing...",
" ��ӡ��... ",
" ���L��... ",		
"Imprimiendo ...",
"���է֧� ��֧�ѧ�ѧߧڧ� ...",
"drucken",
"impression en cours",
"Stampando",
"Estar a imprimir",
"������... ",
},
{
"Set this picture",
"  ��������ͼƬ  ",
"  �]�m�o�i�Ϥ�  ",
"Ajustar esta imagen",
"���֧ݧѧ�� ���� �ڧ٧�ҧ�ѧا֧ߧڧ�",
"Bildeinstellung",
"r��glage photo",
"Impostar la foto",
"Instalar a imgem como a de ",
"�̱׸��� ��ġ�ϱ�",
},
{
"as start picturer",
"   Ϊ��������   ",
"   ���}���e��   ",	
"como imagen de inicio",
"����ܧ�ӧ�� �ڧ٧�ҧ�ѧا֧ߧڧ֧�",
"als Gru\xdfimage",
"image d'acceuil",
"Pitt.avv. Come",
"liga\xe7\xe3o imagem de abrir",//
"��ŸƮȭ��",
},	
{		
"1600x1200 ",
"1600x1200 ",
"1600x1200 ",
"1600x1200 ",
"1600x1200 ",
"1600x1200 ",
"1600x1200 ",
"1600x1200 ",
"1600x1200 ",
"1600x1200 ",
},
{		
"1280x1024 ",
"1280x1024 ",
"1280x1024 ",
"1280x1024 ",
"1280x1024 ",
"1280x1024 ",
"1280x1024 ",
"1280x1024 ",
"1280x1024 ",
"1280x1024 ",
},	
{
"1024x 768 ",
"1024x 768 ",
"1024x 768 ",
"1024x 768 ",
"1024x 768 ",
"1024x 768 ",
"1024x 768 ",
"1024x 768 ",
"1024x 768 ",
"1024x 768 ",
},	
{
"800 x 600 ",
"800 x 600 ",
"800 x 600 ",
"800 x 600 ",
"800 x 600 ",
"800 x 600 ",
"800 x 600 ",
"800 x 600 ",
"800 x 600 ",
"800 x 600 ",
},	
{
"640 x 480 ",
"640 x 480 ",
"640 x 480 ",
"640 x 480 ",	
"640 x 480 ",
"640 x 480 ",
"640 x 480 ",
"640 x 480 ",	
"640 x 480 ",
"640 x 480 ",
},// Nopaly
{
"GAME",
"��Ϸ",
"����",	
"Juego",
"���ԧ��",
"Spiel",
"jeux",
"Gioco",
"Jogo",
"����",
},
{
"No Game!",	
"û����Ϸ!",	
"�S������!",		 
"Sin juego",
"���֧� �ڧԧ��",
"kein Spiel",
"pas de jeux",
"No gioco",
"N\xe3o h�� jogo",
"���� ����",
},
{
"Loading...",
"װ����...",
"�˸���...",
"Cargando...",
"���է֧� �٧ѧԧ��٧ܧ�",
"Loading...",
"charge en cours...",
"Caricando...",
"Est�� a instalar...",
"�ε�",
},
{
"Load Fail",
"װ��ʧ��",	
"�˸�����",	
"Falla de Carga",
"���ѧԧ��٧ܧ� �ߧ� ��էѧݧѧ��",
"Laden Fehler",
"��chec du chargement",//"��chec de la charge",
"Carica falito",
"Falha de instalar",
"�ε� ����",
},
{
"Open Picture",
"��������",	
"�}���e��",		
"Imagen de inicio",
"����ܧ����� ��ܧ�ѧ�",//092//"�����ܧ�ӧ�� �ڧ٧�ҧ�ѧا֧ߧڧ�",
"Gru\xdfimage",
"l'��cran d'acceuil",
"Pitt. avv",
"Imagen de liga\xe7\xe3o",
"��ŸƮȭ��",
},	
{
"Yes",
"��",
"�O",
"S��",
"����",
"Ja",
"oui",
"Si",
"Sim",
"Yes",
},	
{
"No",
"��",
"�_",
"No",
"���֧� ",
"Nein",
"non",
"No",
"N\xe3o",
"No",
},		
};
#endif

UINT8* dc_GetString(UINT16 stringId)
{
    UINT8* str;
    UINT32 languageId = sPara.language;

    if (languageId >= LANGUAGES)
    {
		languageId = LanguageEnglish;
    }

    if(stringId >= DC_MAX_ID) 
    {
		return 0;
    }

    if(dc_UiStrings[stringId][languageId][0]=='\0')
    {
	    str = dc_UiStrings[stringId][0];
    }
	else
	{
	    str = dc_UiStrings[stringId][languageId];
	}

	return str;
}

/**
**/
//#define ICON_X					24			
//#define ICON_Y					192
//#define ICON_OFF_X				76
#define MENU_ICONS_PER_PAGE		4
#define MENU_SELECT_ICON_IDX	0
#define MENU_DISABLE_ICON_IDX	2

UINT32 MenuIconX[MENU_ICONS_PER_PAGE] =
{
	ICON_X, 
	ICON_X+ICON_X_OFF, 
	ICON_X+2*ICON_X_OFF, 
	ICON_X+3*ICON_X_OFF	
};

/**
**/
void DisplayMenuBack(void)
{
	osdClearScreen(0);	
	sysgMemDispAddrGet(&gPB);	
	PNL_PlaybackSet();
	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/			
	pbImagePaste(SetICON[12][2], SetICON[12][3], SetICON[12][4],SetICON[12][5]);

	//pbImagePaste(menuback, sizeof(menuback), 0, 0);		

	/* Paul@2006/12/24 - 15:44 add start */
	PanelColorSet();
	/* Paul@2006/12/24 - 15:44 add end */						
	
}

/**
**/
UINT32 onMovieMenu = 0;  //tzw add for record and move menu 060306

void MenuIconDisp(UINT8 startIdx, UINT8 selIdx, UINT8 totalCnt)
{
	UINT32 i;
	UINT8 ui8PasteCnt = 2;
	UINT8 ui8IconCount;
	UINT8 ui8ImageIdx;

	ui8ImageIdx = startIdx + selIdx;
	if (selIdx == 0)
	{
		pbImagePaste(MenuIconData[startIdx+totalCnt-1][MENU_DISABLE_ICON_IDX], MenuIconData[startIdx+totalCnt-1][MENU_DISABLE_ICON_IDX+1], MenuIconX[0], ICON_Y);		
	}	
	else
	{
		pbImagePaste(MenuIconData[ui8ImageIdx-1][MENU_DISABLE_ICON_IDX], MenuIconData[ui8ImageIdx-1][MENU_DISABLE_ICON_IDX+1], MenuIconX[0], ICON_Y);		
	}
	pbImagePaste(MenuIconData[ui8ImageIdx][MENU_SELECT_ICON_IDX], MenuIconData[ui8ImageIdx][MENU_SELECT_ICON_IDX+1], MenuIconX[1], ICON_Y);	
	
	ui8IconCount = totalCnt - selIdx - 1;
	if (ui8IconCount > (MENU_ICONS_PER_PAGE-2))
	{
		ui8IconCount = 2;
	}
	
	for (i=1; i<(ui8IconCount+1); i++)
	{
		pbImagePaste(MenuIconData[ui8ImageIdx+i][MENU_DISABLE_ICON_IDX], MenuIconData[ui8ImageIdx+i][MENU_DISABLE_ICON_IDX+1], MenuIconX[ui8PasteCnt], ICON_Y);
		ui8PasteCnt++;	
		
		if ((ui8PasteCnt & 0x03) & onMovieMenu)
			return;		
	}		

	i = 0;
	while (ui8PasteCnt < MENU_ICONS_PER_PAGE)
	{		
		pbImagePaste(MenuIconData[startIdx+i][MENU_DISABLE_ICON_IDX], MenuIconData[startIdx+i][MENU_DISABLE_ICON_IDX+1], MenuIconX[ui8PasteCnt], ICON_Y);	
		ui8PasteCnt++;	
		i++;

		if ((ui8PasteCnt & 0x03) & onMovieMenu)
			return;
	}
}

/**
**/
void ClearMenuContent(void)
{
	pbDisplayMenuWord();
	osdBarDraw(0, 32, PANEL_1_OSD_W, PANEL_1_OSD_H-32, 0x00);
}

#if 1
#define MAX_MENU_ITEMS		3
/**
	Paul@2005/11/28-11:51 add 
**/
void DisplayMenuText(
	UINT16 ui16StartIdx,
	UINT8 ui8TopIdx,
	UINT8 ui8SelOKIdx,
	UINT8 ui8SelIdx,
	UINT8 ui8TotalCnt)
{
	UINT8 ui8DispCnt;
	UINT8 i;
	
	if (ui8TotalCnt <= MAX_MENU_ITEMS)
	{
		ui8DispCnt = ui8TotalCnt;
	}
	else
	{
		ui8DispCnt = MAX_MENU_ITEMS;
		/* Update VScrollBar */
	}

	for (i=0; i<ui8DispCnt; i++)
	{
		if (ui8SelIdx == (ui8TopIdx+i))
		{
			osdBarDraw(CAP_BAR_X, CAP_BAR_Y+(i+1)*CAP_TEXT_HEIGHT, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, 0x0e);
			osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*(i+1), UserFont10x20, CR_SELECT_TEXT, dc_GetString(ui16StartIdx+ui8TopIdx+i));								
		}
		else
		{
			osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*(i+1), UserFont10x20, CR_NORMAL_TEXT, dc_GetString(ui16StartIdx+ui8TopIdx+i));					
		}
	}

	if (ui8SelOKIdx < ui8TopIdx)
	{
		return;
	}

	i = ui8SelOKIdx-ui8TopIdx;
	if (i < MAX_MENU_ITEMS)
	{
		osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*(i+1), UserIcon10x20, (ui8SelIdx != ui8SelOKIdx) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");										
	}	

	return;
}
#endif

/**
**/
void menuPcConnectInit(void)
{
	DisplayMenuBack();
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(ConnectPC_s));
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*1, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(Pccam_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(Msdc_s));
}

/**
**/
void menuPcConnectProc(UINT8 select)
{
	ClearMenuContent();
	if (select == 0)
	{
		osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(ConnectPC_s));
		osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*1, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(Pccam_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(Msdc_s));		
	}
	else
	{
		osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(ConnectPC_s));
		osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*2, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(Pccam_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_SELECT_TEXT, dc_GetString(Msdc_s));				
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: ChangeQtable		                                  *
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
void ChangeQtable(void)
{
	switch (QTidx){
		case QTIDX_Q100:
			Q_Table = (UINT8*)QTable100;
			break;
		case QTIDX_Q99:
			Q_Table = (UINT8*)QTable99;
			break;
		case QTIDX_Q98:
			Q_Table = (UINT8*)QTable98;
			break;
		case QTIDX_Q97:
			Q_Table = (UINT8*)QTable97;
			break;
		case QTIDX_Q96:
			Q_Table = (UINT8*)QTable96;
			break;
		case QTIDX_Q95:
			Q_Table = (UINT8*)QTable95;
			break;
		case QTIDX_Q94:
			Q_Table = (UINT8*)QTable94;
			break;
		case QTIDX_Q93:
			Q_Table = (UINT8*)QTable93;
			break;
		case QTIDX_Q92:
			Q_Table = (UINT8*)QTable92;
			break;
		case QTIDX_Q91:
			Q_Table = (UINT8*)QTable91;
			break;
		case QTIDX_Q90:
			Q_Table = (UINT8*)QTable90;
			break;
		case QTIDX_Q89:
			Q_Table = (UINT8*)QTable89;
			break;
		case QTIDX_Q88:
			Q_Table = (UINT8*)QTable88;
			break;
		case QTIDX_Q87:
			Q_Table = (UINT8*)QTable87;
			break;
		case QTIDX_Q86:
			Q_Table = (UINT8*)QTable86;
			break;
		case QTIDX_Q85:
			Q_Table = (UINT8*)QTable85;
			break;
		case QTIDX_Q84:
			Q_Table = (UINT8*)QTable84;
			break;
		case QTIDX_Q83:
			Q_Table = (UINT8*)QTable83;
			break;
		case QTIDX_Q82:
			Q_Table = (UINT8*)QTable82;
			break;
		case QTIDX_Q81:
			Q_Table = (UINT8*)QTable81;
			break;
		case QTIDX_Q80:
			Q_Table = (UINT8*)QTable80;
			break;
		case QTIDX_Q79:
			Q_Table = (UINT8*)QTable79;
			break;
		case QTIDX_Q78:
			Q_Table = (UINT8*)QTable78;
			break;
		case QTIDX_Q77:
			Q_Table = (UINT8*)QTable77;
			break;
		case QTIDX_Q76:
			Q_Table = (UINT8*)QTable76;
			break;
		case QTIDX_Q75:
			Q_Table = (UINT8*)QTable75;
			break;
		case QTIDX_Q74:
			Q_Table = (UINT8*)QTable74;
			break;
		case QTIDX_Q73:
			Q_Table = (UINT8*)QTable73;
			break;
		case QTIDX_Q72:
			Q_Table = (UINT8*)QTable72;
			break;
		case QTIDX_Q71:
			Q_Table = (UINT8*)QTable71;
			break;
		case QTIDX_Q70:
			Q_Table = (UINT8*)QTable70;
			break;
	}
}

#if 0
/**************************************************************************
 *                                                                        *
 *  Function Name: menuVideoFileType                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 * 	 Paul@2005/11/23-09:07 add                                            *
 **************************************************************************/
extern UINT32 ui32MovieType; 

void menuVideoFileTypeInit(UINT32 key)
{
	if (ui32MovieType == xDCF_FILETYPE_ASF)
	{
		ui8SubPvMenuIdx = 1;
	}
	else 
	{
		ui8SubPvMenuIdx = 2;
	}
	menuVideoFileTypeSet(key);
}

UINT32 menuVideoFileTypeSet(UINT32 key)
{
	if (key == UI_KEY_DIR_UP)
	{
		ui8SubPvMenuIdx = 1;
	}
	else if (key == UI_KEY_DIR_DOWN)
	{
		ui8SubPvMenuIdx = 2;
	}
	else if (key == UI_KEY_FUNC_OK)
	{
		if (ui8SubPvMenuIdx == 1)
		{
			ui32MovieType = xDCF_FILETYPE_ASF;
		}
		else
		{
			ui32MovieType = xDCF_FILETYPE_AVI;
		}
	}
	
	osdFWBuffChange();	
	osdClearScreen(0);
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubPvMenuIdx, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(File_Format_s));							
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 1, UserFont10x20, (ui8SubPvMenuIdx != 1) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, "ASF");
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, "AVI");

	if (ui32MovieType == xDCF_FILETYPE_ASF)	
	{
		osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserIcon10x20, (ui8SubPvMenuIdx != 1) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");	
	}
	else if (ui32MovieType == xDCF_FILETYPE_AVI)
	{
		osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserIcon10x20, (ui8SubPvMenuIdx != 2) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");			
	}	

	osdHWBuffChange();	

	return 0;	
}


/**************************************************************************
 *                                                                        *
 *  Function Name: menuVideoFrameRate                                     *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 * 	 Paul@2005/11/23-09:07 add                                            *
 **************************************************************************/
extern UINT32 ui32MvFrameRate;

void menuVideoFrameRateInit(UINT32 key)
{
	if (ui32MvFrameRate == 15)
	{
		ui8SubPvMenuIdx = 1;
	}
	else
	{
		ui8SubPvMenuIdx = 2;		
	}

	menuVideoFrameRateSet(key);
}

UINT32 menuVideoFrameRateSet(UINT32 key)
{
	if (key == UI_KEY_DIR_UP)
	{
		ui8SubPvMenuIdx = 1;
	}
	else if (key == UI_KEY_DIR_DOWN)
	{
		ui8SubPvMenuIdx = 2;
	}
	else if (key == UI_KEY_FUNC_OK)
	{
		if (ui8SubPvMenuIdx == 1)
		{
			ui32MvFrameRate = 15;
		}
		else if (ui8SubPvMenuIdx == 2)
		{
			ui32MvFrameRate = 30;
		}
	}
	
	osdFWBuffChange();	
	osdClearScreen(0);
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubPvMenuIdx, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(Frame_Rate_s));							
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 1, UserFont10x20, (ui8SubPvMenuIdx != 1) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Frame_15_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Frame_30_s));

	if (ui32MvFrameRate == 15)	
	{
		osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserIcon10x20, (ui8SubPvMenuIdx != 1) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");	
	}
	else if (ui32MvFrameRate == 30)
	{
		osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserIcon10x20, (ui8SubPvMenuIdx != 2) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");			
	}	

	osdHWBuffChange();	

	return 0;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuFlashModeInit                                 *
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
extern UINT8 flashmode;

void menuFlashModeInit(UINT32 key)
{
	printf("Flash Mode Init\n");
	ui8SubPvMenuIdx = flashmode + 1;
	menuFlashModeSet(key);
}

UINT32 menuFlashModeSet(UINT32 key)
{
	if (key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
		}
	}
	else if (key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < 4)
		{
			ui8SubPvMenuIdx++; 
		}
	}
	else if (key == UI_KEY_FUNC_OK)
	{
		flashmode = ui8SubPvMenuIdx - 1;
	}
	
	osdFWBuffChange();	
	osdClearScreen(0);
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubPvMenuIdx, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(Flash_Mode_s));							
	
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 1, UserFont10x20, (ui8SubPvMenuIdx != 1) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Force_off_s));		
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Auto_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 3, UserFont10x20, (ui8SubPvMenuIdx != 3) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Force_on_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 4, UserFont10x20, (ui8SubPvMenuIdx != 4) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Red_eye_s));
	
	osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * (flashmode+1), UserIcon10x20, (ui8SubPvMenuIdx != (flashmode+1)) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");	
#if 0
	UINT16 x;
	UINT16 len;
	
	len = strlen(dc_GetString(dc_Menu_s)) + strlen(dc_GetString(Exit_s))+6;
	x = (PANEL_1_OSD_W-len*MENU_TIP_FONT_W)/2;
	osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd0, "[ ");
	x += 2*MENU_TIP_FONT_W;
	osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd0, dc_GetString(dc_Menu_s));
	len = strlen(dc_GetString(dc_Menu_s));
	x += len*MENU_TIP_FONT_W;
	osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd0, " ]: ");	
	x += 4*MENU_TIP_FONT_W;
	osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd0, dc_GetString(Exit_s));
#endif	
	osdHWBuffChange();	

	return 0;
}
#endif

/**************************************************************************
 *                                                                        *
 *  Function Name: menuCaptureModeInit                                    *
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
void menuCaptureModeInit(UINT32 key)
{
	printf("Capture Mode Init\n");
	ui8SubPvMenuIdx = sPara.snapFlag;
	menuCaptureModeSet(key);
}


UINT32 menuCaptureModeSet(UINT32 key)
{
	if (key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
		}
		else
		{
			ui8SubPvMenuIdx = SnapSelf;
		}
	}
	else if (key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < SnapSelf)
		{
			ui8SubPvMenuIdx++;
		}
		else
		{
			ui8SubPvMenuIdx = 1;			
		}
	}
	else if (key == UI_KEY_FUNC_OK)
	{
		sPara.snapFlag = ui8SubPvMenuIdx;		
		printf("Snap Mode = %d\n", sPara.snapFlag);
	}
	
	osdFWBuffChange();	
	osdClearScreen(0);	
	/* No Arrow */
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubPvMenuIdx, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(Capture_mode_s));							
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 1, UserFont10x20, (ui8SubPvMenuIdx != 1) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Single_Shot_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Multi_shot_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 3, UserFont10x20, (ui8SubPvMenuIdx != 3) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Self_shot_s));
	
	osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * sPara.snapFlag, UserIcon10x20, (ui8SubPvMenuIdx != sPara.snapFlag) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");	
	
	osdHWBuffChange();	

	return 0;
}

extern UINT8 ui8DownTimeIdx;
extern UINT8 ui8DownTimeMvIdx;
/**
	Adjust 2s, 5s, 10s, 15s
**/
void menuSelfShotInit(UINT32 key)
{
	ui8SubPvMenuIdx = ui8DownTimeIdx+1;
	g_ui8TotalItem = 4;	
	g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
	g_ui8SelOKIdx = ui8DownTimeIdx;
	menuSelfShotSet(key);
}

/**
**/
UINT32 menuSelfShotSet(UINT32 key)
{
	if (key == UI_KEY_FUNC_OK)
	{
		ui8DownTimeIdx = ui8SubPvMenuIdx-1;
		g_ui8SelOKIdx = ui8DownTimeIdx;
	}
	else if(key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
			if (ui8SubPvMenuIdx == g_ui8TopIdx)
			{
				g_ui8TopIdx--;
			}			
		}	
		else
		{
			ui8SubPvMenuIdx = g_ui8TotalItem;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
		}				
	}
	else if(key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < g_ui8TotalItem)
		{
			ui8SubPvMenuIdx++;		
			if ((g_ui8TopIdx + MAX_MENU_ITEMS) < ui8SubPvMenuIdx)
			{
				g_ui8TopIdx++;
			}			
		}
		else
		{
			ui8SubPvMenuIdx = 1 ;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
		}				
	}	

	osdFWBuffChange();	
	osdClearScreen(0);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(ShotTime_s));					
	DisplayMenuText(Time_2s_s, g_ui8TopIdx, g_ui8SelOKIdx, ui8SubPvMenuIdx-1, g_ui8TotalItem);	
	osdHWBuffChange();	
	return 0;
}

/**
	Adjust 2s, 5s, 10s, 15s
**/
void menuMvSelfShotInit(UINT32 key)
{
	ui8SubPvMenuIdx = ui8DownTimeMvIdx+1;
	g_ui8TotalItem = 4;	
	g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
	g_ui8SelOKIdx = ui8DownTimeMvIdx;	
	menuMvSelfShotSet(key);
}

/**
**/
UINT32 menuMvSelfShotSet(UINT32 key)
{
	if (key == UI_KEY_FUNC_OK)
	{
		ui8DownTimeMvIdx = ui8SubPvMenuIdx-1;
		g_ui8SelOKIdx = ui8DownTimeMvIdx;
	}
	else if(key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
			if (ui8SubPvMenuIdx == g_ui8TopIdx)
			{
				g_ui8TopIdx--;
			}			
		}	
		else
		{
			ui8SubPvMenuIdx = g_ui8TotalItem;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;			
		}
	}
	else if(key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < g_ui8TotalItem)
		{
			ui8SubPvMenuIdx++;		
			if ((g_ui8TopIdx + MAX_MENU_ITEMS) < ui8SubPvMenuIdx)
			{
				g_ui8TopIdx++;
			}			
		}
		else
		{
			ui8SubPvMenuIdx = 1;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;			
		}
	}	

	osdFWBuffChange();	
	osdClearScreen(0);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(ShotTime_s));					
	DisplayMenuText(Time_2s_s, g_ui8TopIdx, g_ui8SelOKIdx, ui8SubPvMenuIdx-1, g_ui8TotalItem);	
	osdHWBuffChange();	
	
	return 0;
}

UINT8 ui8NightMode = 0;
/**
**/
void menuNightSceneInit(UINT32 key)
{	
	ui8SubPvMenuIdx = ui8NightMode+1;
	menuNightSceneSet(key);
}

UINT32 menuNightSceneSet(UINT32 key)
{
	if (key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
		}
		else
		{
			ui8SubPvMenuIdx = 2;			
		}
	}
	else if (key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < 2)
		{
			ui8SubPvMenuIdx++;
		}		
		else
		{
			ui8SubPvMenuIdx = 1;			
		}
	}
	else if (key == UI_KEY_FUNC_OK)
	{
		ui8NightMode = ui8SubPvMenuIdx-1;
	}

	osdFWBuffChange();	
	osdClearScreen(0);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(NightMode_s));					
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubPvMenuIdx, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);	
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 1, UserFont10x20, (ui8SubPvMenuIdx != 1) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Open_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT* 2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Close_s));
	
	osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * (ui8NightMode+1), UserIcon10x20, (ui8SubPvMenuIdx != (ui8NightMode+1)) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");	
	
	osdHWBuffChange();		
	
	return 0;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuPictureQualityInit                                 *
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
void menuPictureQualityInit (UINT32 key)
{
	printf("Picture Quality Init\n");
	ui8SubPvMenuIdx = QTidx;

	if(ui8SubPvMenuIdx == CAPTURE_SUPER_QTIDX)
	{
		ui8SubPvMenuIdx = 1;
	}
	else if(ui8SubPvMenuIdx == CAPTURE_FINE_QTIDX)
	{
		ui8SubPvMenuIdx = 2;
	}
	else if(ui8SubPvMenuIdx == CAPTURE_NORMAL_QTIDX)
	{
		ui8SubPvMenuIdx = 3;
	}
	menuPictureQualitySet(key);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuPictureQualitySet                                  *
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
UINT32 menuPictureQualitySet(UINT32 key)
{	
	if (key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
		}	
		else
		{
			ui8SubPvMenuIdx = 3;
		}
	}
	else if (key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < 3)
		{
			ui8SubPvMenuIdx++;
		}		
		else
		{
			ui8SubPvMenuIdx = 1;			
		}
	}		
	else if (key == UI_KEY_FUNC_OK)
	{
		if(ui8SubPvMenuIdx == 1)
		{
			QTidx = CAPTURE_SUPER_QTIDX;
			sPara.ImageQuality = QualitySuper;
		}
		else if(ui8SubPvMenuIdx == 2)
		{
			QTidx = CAPTURE_FINE_QTIDX;
			sPara.ImageQuality = QualityFine;
		}
		else if(ui8SubPvMenuIdx == 3)
		{
			QTidx = CAPTURE_NORMAL_QTIDX;
			sPara.ImageQuality = QualityNormal;
		}
		ChangeQtable();
	}
	
	osdFWBuffChange();	
	osdClearScreen(0);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(Quality_s));					
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubPvMenuIdx, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);	
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 1, UserFont10x20, (ui8SubPvMenuIdx != 1) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Super_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Fine_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 3, UserFont10x20, (ui8SubPvMenuIdx != 3) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Normal_s));
	
	osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * (sPara.ImageQuality+1), UserIcon10x20, (ui8SubPvMenuIdx != (sPara.ImageQuality+1)) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");	

	osdHWBuffChange();	

	return 0;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuVideoQualityInit                                 *
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
void menuVideoQualityInit (UINT32 key)
{
	printf("Picture Quality Init\n");
	ui8SubPvMenuIdx = QTidx;

	if(ui8SubPvMenuIdx == VIDEO_SUPER_QTIDX)
	{
		ui8SubPvMenuIdx = 1;
	}
	else if(ui8SubPvMenuIdx == VIDEO_FINE_QTIDX)
	{
		ui8SubPvMenuIdx = 2;
	}
	else if(ui8SubPvMenuIdx == VIDEO_NORMAL_QTIDX)
	{
		ui8SubPvMenuIdx = 3;
	}
	menuPictureQualitySet(key);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuVideoQualitySet                                  *
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
UINT32 menuVideoQualitySet(UINT32 key)
{	
	if (key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
		}		
	}
	else if (key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < 3)
		{
			ui8SubPvMenuIdx++;
		}		
	}		
	else if (key == UI_KEY_FUNC_OK)
	{
		if(ui8SubPvMenuIdx == 1)
		{
			QTidx = VIDEO_SUPER_QTIDX;
			sPara.ImageQuality = QualitySuper;
		}
		else if(ui8SubPvMenuIdx == 2)
		{
			QTidx = VIDEO_FINE_QTIDX;
			sPara.ImageQuality = QualityFine;
		}
		else if(ui8SubPvMenuIdx == 3)
		{
			QTidx = VIDEO_NORMAL_QTIDX;
			sPara.ImageQuality = QualityNormal;
		}
		ChangeQtable();
	}
	
	osdFWBuffChange();	
	osdClearScreen(0);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(Quality_s));					
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubPvMenuIdx, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);	
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 1, UserFont10x20, (ui8SubPvMenuIdx != 1) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Super_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Fine_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * 3, UserFont10x20, (ui8SubPvMenuIdx != 3) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(Normal_s));
	
	osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * (sPara.ImageQuality+1), UserIcon10x20, (ui8SubPvMenuIdx != (sPara.ImageQuality+1)) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");	

	osdHWBuffChange();	

	return 0;
}

/**
**/
void captureQTidxInit()
{
	switch (sPara.ImageQuality)
	{
		case QualitySuper:
			QTidx = CAPTURE_SUPER_QTIDX;
			break;

		case QualityFine:
			QTidx = CAPTURE_FINE_QTIDX;
			break;

		case QualityNormal:
			QTidx = CAPTURE_NORMAL_QTIDX;
			break;
	}
}

/**
**/
void videoQTidxInit()
{
	switch (sPara.ImageQuality)
	{
		case QualitySuper:
			QTidx = VIDEO_SUPER_QTIDX;
			break;

		case QualityFine:
			QTidx = VIDEO_FINE_QTIDX;
			break;

		case QualityNormal:
			QTidx = VIDEO_NORMAL_QTIDX;
			break;
	}
}/**************************************************************************
 *                                                                        *
 *  Function Name: menuAWBInit		                                  *
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
void menuAWBInit(UINT32 key)
{
	ui8SubPvMenuIdx = ManualWbFlag+1;
	g_ui8TotalItem = 5;	
	g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
	g_ui8SelOKIdx = ManualWbFlag;	
	menuAWBSet(key);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuAWBSet                      		                  *
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
UINT32 menuAWBSet(UINT32 key)
{
	if (key == UI_KEY_FUNC_OK)
	{
		ManualWbFlag = ui8SubPvMenuIdx-1;
		sPara.WBFlag = ManualWbFlag;		
		g_ui8SelOKIdx = ManualWbFlag;
	}
	else if(key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
			if (ui8SubPvMenuIdx == g_ui8TopIdx)
			{
				g_ui8TopIdx--;
			}			
		}		
		else
		{
			ui8SubPvMenuIdx = g_ui8TotalItem;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
		}				
	}
	else if(key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < g_ui8TotalItem)
		{
			ui8SubPvMenuIdx++;		
			if ((g_ui8TopIdx + MAX_MENU_ITEMS) < ui8SubPvMenuIdx)
			{
				g_ui8TopIdx++;
			}			
		}
		else
		{
			ui8SubPvMenuIdx = 1;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
		}		
	}	

	osdFWBuffChange();	
	osdClearScreen(0);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(AWB_s));							
	DisplayMenuText(Auto_s, g_ui8TopIdx, g_ui8SelOKIdx, ui8SubPvMenuIdx-1, g_ui8TotalItem);	
	printf("Sel = %d\n", ui8SubPvMenuIdx);
	osdHWBuffChange();	

	return 0;
}

/**
	Paul@2005/11/19-08:23 add 
**/
void movieSpecialEffectSet(UINT8 mode)
{
	extern UINT32 GAMMA_TBL[1024];
	
	if(mode == SephiaEffect)
	{
		hwCdspSatHueSet(1, 0x20, 0x00, 0x30);
		hwCdspColorMSet(0x38, 0x0, 0x0, 0x0, 0x28, 0x0, 0x0, 0x0, 0x20);
//		hwCdspGammaSet(0);   
		hwFrontVdWait(0, 1);   //fqdao for bug Effect 06.5.30
		hwCdspGammaInit(GAMMA_TBL);
		hwFrontVdWait(0, 1);	//fqdao for bug Effect 06.5.30	
//		hwCdspGammaSet(1);	 
	}
	else if(mode == BWEffect){
		hwCdspSatHueSet(1, 0x00, 0x00, 0x30);
//		hwCdspGammaSet(0);
		hwFrontVdWait(0, 1);  //fqdao for bug Effect 06.5.30
		hwCdspGammaInit(GAMMA_TBL);						
		hwFrontVdWait(0, 1);	//fqdao for bug Effect 06.5.30	
//		hwCdspGammaSet(1);		
	}
	else if(mode == NegativeEffect)
	{
		UINT32 NegativeGammaTbl[1024];
		UINT32 i;	
		hwCdspSatHueSet(1, 0x20, 0x00, 0x30);
		for (i=0; i<1024; i++)
			NegativeGammaTbl[i] = 255-GAMMA_TBL[i];
//		hwCdspGammaSet(0);          
		hwFrontVdWait(0, 1);     //fqdao for bug Effect 06.5.30
		hwCdspGammaInit(NegativeGammaTbl);
		hwFrontVdWait(0, 1);		//fqdao for bug Effect 06.5.30
//		hwCdspGammaSet(1);		 
	}
	else if(mode == NormalEffect)
	{
		hwCdspSatHueSet(0, 36, 0x00, 0x48);   //fqdao_modify 06.5.5
//		hwCdspColorMSet(69,508,509,486,108,496,489,496,102);				
//		hwCdspColorMSet(0x86, 0x1AC, 0x0E, 0x1F2, 0x33, 0x1C, 0x1F3, 0x1B0, 0x9D);
//		hwCdspColorMSet(76,502,508,483,115,489,492,499,96);
		hwCdspColorMSet(0x67, 0x1e0, 0x1f9, 0x1eb, 0x62, 0x1f3, 0x1F6, 0x1cb, 0x7f);
//		hwCdspGammaSet(0);   
		hwFrontVdWait(0, 1);   //fqdao for bug Effect 06.5.30
		hwCdspGammaInit(GAMMA_TBL);
		hwFrontVdWait(0, 1);	 //fqdao for bug Effect 06.5.30	
//		hwCdspGammaSet(1);		 
	}
#if 0	
	else if(mode == REDEFFECT)
	{
		hwCdspColorMSet(0x35, 0x0, 0x0, 0x0, 0x14, 0x0, 0x0, 0x0, 0x14);	
		hwCdspGammaInit(GAMMA_TBL);
	}
	else if(mode == PINKEFFECT)
	{
		hwCdspColorMSet(0x3F, 0x0, 0x0, 0x0, 0x19, 0x0, 0x0, 0x0, 0x2A);	
	}
	else if(mode == PURPLEEFFECT)
	{
		hwCdspColorMSet(0x2c, 0x0, 0x0, 0x0, 0x10, 0x0, 0x0, 0x0, 0x2f);	
	}
	else if(mode == BLUEEFFECT)
	{
		hwCdspColorMSet(0x1A, 0x0, 0x0, 0x0, 0x23, 0x0, 0x0, 0x0, 0x38);	
	}
	else if(mode == GREENEFFECT)
	{
		hwCdspColorMSet(0x1c, 0x0, 0x0, 0x0, 0x33, 0x0, 0x0, 0x0, 0x1d);	
	}
	else if(mode == YELLOWEFFECT)
	{
		hwCdspColorMSet(0x3f, 0x0, 0x0, 0x0, 0x39, 0x0, 0x0, 0x0, 0x21);	
	}
#endif	
}



/**************************************************************************
 *                                                                        *
 *  Function Name: menuEffectInit	                                      *
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
void menuEffectInit (UINT32 key)
{
	ui8SubPvMenuIdx = sPara.EffectFlag+1;
	g_ui8TotalItem = 4;	
	g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
	g_ui8SelOKIdx = sPara.EffectFlag;
	menuEffectSet(key);	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuEffectSet	                                  *
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
UINT32 menuEffectSet (UINT32 key)
{
	if (key == UI_KEY_FUNC_OK)
	{
		sPara.EffectFlag = ui8SubPvMenuIdx - 1;
		movieSpecialEffectSet(sPara.EffectFlag);
		g_ui8SelOKIdx = sPara.EffectFlag;
	}
	else if(key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
			if (ui8SubPvMenuIdx == g_ui8TopIdx)
			{
				g_ui8TopIdx--;
			}			
		}	
		else
		{
			ui8SubPvMenuIdx = g_ui8TotalItem;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
		}				
	}
	else if(key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < g_ui8TotalItem)
		{
			ui8SubPvMenuIdx++;		
			if ((g_ui8TopIdx + MAX_MENU_ITEMS) < ui8SubPvMenuIdx)
			{
				g_ui8TopIdx++;
			}			
		}
		else
		{
			ui8SubPvMenuIdx = 1;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
		}
		
	}	

	osdFWBuffChange();	
	osdClearScreen(0);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(Effect_s));								
	DisplayMenuText(Effect_Normal_s, g_ui8TopIdx, g_ui8SelOKIdx, ui8SubPvMenuIdx-1, g_ui8TotalItem);	
	osdHWBuffChange();	
	
	return 0;
}

#define MENU_EFFECT_W		160
#define MENU_EFFECT_H		(24 * 4)
#define MENU_EFFECT_X		(PANEL_1_OSD_W-MENU_EFFECT_W)
#define MENU_EFFECT_Y		100
#define MENU_EFFECT_S_COL	0x0e
#define MENU_EFFECT_T_COL	0x3e
#define MENU_EFFECT_N_COL	0xd2
#define MENU_EFFECT_A_COL	0x32

static UINT8 ui8UpdateEffect;

/**
	Paul@2005/11/01-08:27
**/
void DisplayCaptureEv(UINT32 value)
{
	UINT8 str[8];
	
	if (value > ((C_EVCMP_MAX_INDEX-1)/2))
	{
		sio_psprintf(str, "%c%01d.%01dEV", '-', uiEvDisplay[(C_EVCMP_MAX_INDEX-1-value)]/10, uiEvDisplay[(C_EVCMP_MAX_INDEX-1-value)]%10);
	}
	else if (value < ((C_EVCMP_MAX_INDEX-1)/2))
	{
		sio_psprintf(str, "%c%01d.%01dEV", '+', uiEvDisplay[value]/10, uiEvDisplay[value]%10);
	}
	else
	{
		sio_psprintf(str, " 0.0EV");
	}		
	osdStrDisp(0, PANEL_1_OSD_H-20, UserFont10x20, 0x30, str);
}

/**
**/
void menuEffectProcInit(void)
{
	ui8SubPvMenuIdx = sPara.EffectFlag;
	ui8UpdateEffect = 1;
}

/**
**/
UINT32 menuEffectProc(UINT32 key)
{
	if (key == UI_KEY_ACCIDENT)
	{
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);					
		return 1;
	}
	else if (key == UI_KEY_FUNC_OK)
	{		
		sPara.EffectFlag = ui8SubPvMenuIdx;
		osdBarDraw(MENU_EFFECT_X, MENU_EFFECT_Y, MENU_EFFECT_W, MENU_EFFECT_H, 0x00);		
		return 1;
	}
	else if(key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 0)
		{
			ui8SubPvMenuIdx--;
			movieSpecialEffectSet(ui8SubPvMenuIdx);
			ui8UpdateEffect = 1;
		}		
	}
	else if(key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < 3)
		{
			ui8SubPvMenuIdx++;
			movieSpecialEffectSet(ui8SubPvMenuIdx);			
			ui8UpdateEffect = 1;
		}
	}
	else if (key == UI_KEY_DIR_LEFT)
	{
		if ((uiState & UI_MODE_MASK) == UI_MODE_PREVIEW)
		{
			if (pvEvCmpIdx < (C_EVCMP_MAX_INDEX-1))	//Paul@2005/11/03-08:48 add
			{
				aaaEvCmpIdxSet(++pvEvCmpIdx);
				DisplayCaptureEv(pvEvCmpIdx);				
			}
		}	
		else if ((uiState & UI_MODE_MASK) == UI_MODE_MOVIE)
		{
			if (vcEvCmpIdx < (C_EVCMP_MAX_INDEX-1))	//Paul@2005/11/03-08:48 add
			{
				aaaEvCmpIdxSet(++vcEvCmpIdx);
				DisplayCaptureEv(vcEvCmpIdx);
			}			
		}
	}	
	else if (key == UI_KEY_DIR_RIGHT)	/* Paul@2005/11/01-08:27 */
	{
		if ((uiState & UI_MODE_MASK) == UI_MODE_PREVIEW)		
		{
			if (pvEvCmpIdx > 0)
			{
				aaaEvCmpIdxSet(--pvEvCmpIdx);
				DisplayCaptureEv(pvEvCmpIdx);				
			}
		}
		else if ((uiState & UI_MODE_MASK) == UI_MODE_MOVIE)
		{
			if (vcEvCmpIdx > 0)
			{
				aaaEvCmpIdxSet(--vcEvCmpIdx);
				DisplayCaptureEv(vcEvCmpIdx);				
			}			
		}		
	}

	if (ui8UpdateEffect == 1)
	{
		
		osdBarDraw(MENU_EFFECT_X, MENU_EFFECT_Y, MENU_EFFECT_W, MENU_EFFECT_H, 0x02);
		osdBarDraw(MENU_EFFECT_X, MENU_EFFECT_Y+24*ui8SubPvMenuIdx, MENU_EFFECT_W, 24, MENU_EFFECT_S_COL);	
		osdStrDisp(MENU_EFFECT_X + 10, MENU_EFFECT_Y+24 * 0+2, UserFont10x20, (ui8SubPvMenuIdx != 0) ? MENU_EFFECT_N_COL : MENU_EFFECT_T_COL, dc_GetString(Effect_Normal_s));
		osdStrDisp(MENU_EFFECT_X + 10, MENU_EFFECT_Y+24 * 1+2, UserFont10x20, (ui8SubPvMenuIdx != 1) ? MENU_EFFECT_N_COL : MENU_EFFECT_T_COL, dc_GetString(Sepia_s));	
		osdStrDisp(MENU_EFFECT_X + 10, MENU_EFFECT_Y+24 * 2+2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? MENU_EFFECT_N_COL : MENU_EFFECT_T_COL, dc_GetString(BW_s));
		osdStrDisp(MENU_EFFECT_X + 10, MENU_EFFECT_Y+24 * 3+2, UserFont10x20, (ui8SubPvMenuIdx != 3) ? MENU_EFFECT_N_COL : MENU_EFFECT_T_COL, dc_GetString(Negative_s));		
		ui8UpdateEffect = 0;
	}
	
	return 0;
}


/**************************************************************************
 *                                                                        *
 *  Function Name: menuSharpnessInit	                                  *
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
UINT8 menuSharpnessInit (UINT32 key, UINT8* sel)
{
	/*if (key == UI_KEY_FUNC_MENU)*/
	{
		*sel = SharpnessFlag + 1;
		/*printf("sel = %x\n", *sel);*/
	}

	return CALL_DEFAULT;
}


/**************************************************************************
 *                                                                        *
 *  Function Name: menuSharpnessSet		                                  *
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
UINT8 menuSharpnessSet (UINT32 key, UINT8* sel)
{
	UINT32 flag = 0;

	if (key == UI_KEY_FUNC_MENU)
	{
		flag = *sel - 1;
	}
	else if(key == UI_KEY_DIR_UP)
	{
		flag = *sel - 2;

		if(*sel == 1){
			flag = 4;
		}
	}
	else if(key == UI_KEY_DIR_DOWN)
	{
		flag = *sel;

		if(*sel == 5){
			flag = 0;
		}
	}

	aaaSharpnessFlagSet(flag);
	/*
	printf("sel = %x\n", *sel);
	printf("flag = %x\n", SharpnessFlag);		
	*/

	return CALL_DEFAULT;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuISOInit		                                  *
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
UINT8 menuISOInit (UINT32 key, UINT8* sel)
{
	/*if (key == UI_KEY_FUNC_MENU)*/
	{
		*sel = sPara.ISOFlag+1;
	}

	return CALL_DEFAULT;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuISOSet		                                  *
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
UINT8 menuISOSet (UINT32 key, UINT8* sel)
{

	if (key == UI_KEY_FUNC_MENU)
	{
		sPara.ISOFlag = *sel-1;
		/*printf("isoflag=%x\n", sPara.ISOFlag);*/
	}

	return CALL_DEFAULT;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuASFMICInit	                                  *
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
UINT8 menuASFMICInit (UINT32 key, UINT8* sel)
{
	/*if (key == UI_KEY_FUNC_MENU)*/
	{
		*sel = sPara.ASFMICFlag+1;
	}

	return CALL_DEFAULT;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuASFMICSet 	                                  *
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
UINT8 menuASFMICSet (UINT32 key, UINT8* sel)
{
	UINT32 opt;

	if (key == UI_KEY_FUNC_MENU)
	{
		sPara.ASFMICFlag = *sel-1;

		opt = asfAppClipOptGet();

		if(sPara.ASFMICFlag == MICON){
			asfAppClipOptSet(opt&0xfffffffd, ASF_NO_AUDIO_DATA);
		}
		else if(sPara.ASFMICFlag == MICOFF){
			asfAppClipOptSet(opt|ASF_NO_AUDIO_DATA, ASF_NO_AUDIO_DATA);
		}
	}

	return CALL_DEFAULT;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: ImageSizeSelect                                        *
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
void ImageSizeSelect(UINT32 *size)
{
	UINT32 width, height;

	sysgSnapSizeGet(&width, &height);
	switch (width)
	{
		case 1600:
			if(sPara.ImageQuality == QualitySuper){
				*size = super1600;
			}
			else if(sPara.ImageQuality == QualityFine){
				*size = fine1600;
			}
			else if(sPara.ImageQuality == QualityNormal){
				*size = normal1600;
			}			
			break;
	
		case 1280:
			if(sPara.ImageQuality == QualitySuper){
				*size = super1280;
			}
			else if(sPara.ImageQuality == QualityFine){
				*size = fine1280;
			}
			else if(sPara.ImageQuality == QualityNormal){
				*size = normal1280;
			}			
			break;

		case 1024:
			if(sPara.ImageQuality == QualitySuper){
				*size = super1024;
			}
			else if(sPara.ImageQuality == QualityFine){
				*size = fine1024;
			}
			else if(sPara.ImageQuality == QualityNormal){
				*size = normal1024;
			}			
			break;
			
		case 800:
			if(sPara.ImageQuality == QualitySuper){
				*size = super800;
			}
			else if(sPara.ImageQuality == QualityFine){
				*size = fine800;
			}
			else if(sPara.ImageQuality == QualityNormal){
				*size = normal800;
			}
			break;
			
		case 640:
			if(sPara.ImageQuality == QualitySuper){
				*size = super640;
			}
			else if(sPara.ImageQuality == QualityFine){
				*size = fine640;
			}
			else if(sPara.ImageQuality == QualityNormal){
				*size = normal640;
			}
			break;
	}
}

#if 0
/**
**/	
bool uiSnapFileSizeChk(void)
{
	bool retVal = false;
	devInfo_t pdevInfo;
	
	if(cardExist)
	{
		vfsDevInfoGet(DRIVE_SD, 0, &pdevInfo);
	}
	else
	{
		vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfo);
	}
	
	switch(sPara.ui8ImageSize)
	{
		case ImageSize2976:
			if(sPara.ImageQuality == QualitySuper)
			{	
				retVal = (pdevInfo.nrFreeBytes < (1024 * 1024))?true:false;
			}
			else if(sPara.ImageQuality == QualityFine)
			{
				retVal = (pdevInfo.nrFreeBytes < (1024 * 672))?true:false;
			}
			else if(sPara.ImageQuality == QualityNormal)
			{
				retVal = (pdevInfo.nrFreeBytes < (1024 * 490))?true:false;
			}
			break;
			
		case ImageSize2048:
			if(sPara.ImageQuality == QualitySuper)
			{	
				retVal = (pdevInfo.nrFreeBytes < (1024 * 635))?true:false;
			}
			else if(sPara.ImageQuality == QualityFine)
			{
				retVal = (pdevInfo.nrFreeBytes < (1024 * 377))?true:false;
			}
			else if(sPara.ImageQuality == QualityNormal)
			{
				retVal = (pdevInfo.nrFreeBytes < (1024 * 276))?true:false;
			}
			break;
			
		case ImageSize1600:
			if(sPara.ImageQuality == QualitySuper)
			{	
				retVal = (pdevInfo.nrFreeBytes < (1024 * 409))?true:false;
			}
			else if(sPara.ImageQuality == QualityFine)
			{
				retVal = (pdevInfo.nrFreeBytes < (1024 * 248))?true:false;
			}
			else if(sPara.ImageQuality == QualityNormal)
			{
				retVal = (pdevInfo.nrFreeBytes < (1024 * 183))?true:false;
			}
			break;
			
		case ImageSize640:
			if(sPara.ImageQuality == QualitySuper)
			{	
				retVal = (pdevInfo.nrFreeBytes < (1024 * 100))?true:false;
			}
			else if(sPara.ImageQuality == QualityFine)
			{
				retVal = (pdevInfo.nrFreeBytes < (1024 * 90))?true:false;
			}
			else if(sPara.ImageQuality == QualityNormal)
			{
				retVal = (pdevInfo.nrFreeBytes < (1024 * 80))?true:false;
			}
			break;
	}
	return retVal;
}
#endif

/**************************************************************************
 *                                                                        *
 *  Function Name: QtableMove                                             *
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
void QtableMove(UINT32 direct, UINT32 step)
{
	for( ; step>0; step--){
		switch (QTidx){
			case QTIDX_Q100:
				QTidx = (direct==1) ? QTIDX_Q100 : QTIDX_Q99;
				break;
			case QTIDX_Q99:
				QTidx = (direct==1) ? QTIDX_Q100 : QTIDX_Q98;
				break;
			case QTIDX_Q98:
				QTidx = (direct==1) ? QTIDX_Q99 : QTIDX_Q97;
				break;
			case QTIDX_Q97:
				QTidx = (direct==1) ? QTIDX_Q98 : QTIDX_Q96;
				break;
			case QTIDX_Q96:
				QTidx = (direct==1) ? QTIDX_Q97 : QTIDX_Q95;
				break;
			case QTIDX_Q95:
				QTidx = (direct==1) ? QTIDX_Q96 : QTIDX_Q94;
				break;
			case QTIDX_Q94:
				QTidx = (direct==1) ? QTIDX_Q95 : QTIDX_Q93;
				break;
			case QTIDX_Q93:
				QTidx = (direct==1) ? QTIDX_Q94 : QTIDX_Q92;
				break;
			case QTIDX_Q92:
				QTidx = (direct==1) ? QTIDX_Q93 : QTIDX_Q91;
				break;
			case QTIDX_Q91:
				QTidx = (direct==1) ? QTIDX_Q92 : QTIDX_Q90;
				break;
			case QTIDX_Q90:
				QTidx = (direct==1) ? QTIDX_Q91 : QTIDX_Q89;
				break;
			case QTIDX_Q89:
				QTidx = (direct==1) ? QTIDX_Q90 : QTIDX_Q88;
				break;
			case QTIDX_Q88:
				QTidx = (direct==1) ? QTIDX_Q89 : QTIDX_Q87;
				break;
			case QTIDX_Q87:
				QTidx = (direct==1) ? QTIDX_Q88 : QTIDX_Q86;
				break;
			case QTIDX_Q86:
				QTidx = (direct==1) ? QTIDX_Q87 : QTIDX_Q85;
				break;
			case QTIDX_Q85:
				QTidx = (direct==1) ? QTIDX_Q86 : QTIDX_Q84;
				break;
			case QTIDX_Q84:
				QTidx = (direct==1) ? QTIDX_Q85 : QTIDX_Q83;
				break;
			case QTIDX_Q83:
				QTidx = (direct==1) ? QTIDX_Q84 : QTIDX_Q82;
				break;
			case QTIDX_Q82:
				QTidx = (direct==1) ? QTIDX_Q83 : QTIDX_Q81;
				break;
			case QTIDX_Q81:
				QTidx = (direct==1) ? QTIDX_Q82 : QTIDX_Q80;
				break;
			case QTIDX_Q80:
				QTidx = (direct==1) ? QTIDX_Q81 : QTIDX_Q79;
				break;
			case QTIDX_Q79:
				QTidx = (direct==1) ? QTIDX_Q80 : QTIDX_Q78;
				break;
			case QTIDX_Q78:
				QTidx = (direct==1) ? QTIDX_Q79 : QTIDX_Q77;
				break;
			case QTIDX_Q77:
				QTidx = (direct==1) ? QTIDX_Q78 : QTIDX_Q76;
				break;
			case QTIDX_Q76:
				QTidx = (direct==1) ? QTIDX_Q77 : QTIDX_Q75;
				break;
			case QTIDX_Q75:
				QTidx = (direct==1) ? QTIDX_Q76 : QTIDX_Q74;
				break;
			case QTIDX_Q74:
				QTidx = (direct==1) ? QTIDX_Q75 : QTIDX_Q73;
				break;
			case QTIDX_Q73:
				QTidx = (direct==1) ? QTIDX_Q74 : QTIDX_Q72;
				break;
			case QTIDX_Q72:
				QTidx = (direct==1) ? QTIDX_Q73 : QTIDX_Q71;
				break;
			case QTIDX_Q71:
				QTidx = (direct==1) ? QTIDX_Q72 : QTIDX_Q70;
				break;
			case QTIDX_Q70:
				QTidx = (direct==1) ? QTIDX_Q71 : QTIDX_Q70;
				break;
		}
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: snapFileSizeControl                                    *
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
void snapFileSizeControl(UINT32 *flag, UINT32 compressedsize, UINT32 desiredsize)
{
	SINT32 tempsize = 0;
	UINT32 range;
	devInfo_t pdevInfo;

	vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfo);

	range = desiredsize/2;

	/*if(delfragment == 1){
		tempsize = desiredsize - compressedsize;

		*flag = 1;
		sizedifference = 0;
		delfragment = 0;
		fragment = fragment + tempsize;
		printf("tempsize = %d, sizedifference = %d\n", tempsize, sizedifference);
		printf("del fragment = %d\n", fragment);
		return;
	}

	if(desiredsize >= 204800){
		range = 102400;
	}
	else{
		range = desiredsize/2;
	}*/

	if((compressedsize > desiredsize)&&(sizedifference == 0)){	/*should not larger than desired size*/
		QtableMove(0, 1);
		ChangeQtable();
		return;
	}
	else if(compressedsize < (desiredsize - range)){	/*should not less than (desired size - 4 cluster)*/
		QtableMove(1, 1);
		ChangeQtable();
		return;
	}

	if(compressedsize!=desiredsize){
		tempsize = desiredsize - compressedsize;
	}
	else{
		*flag = 1;
		sizedifference = 0;
		fragment = fragment + tempsize;	/*tempsize = 0*/
		printf("fragment = %d\n", fragment);
		return;
	}
	printf("tempsize = %d, sizedifference = %d\n", tempsize, sizedifference);

	if(sizedifference == 0){
		sizedifference = tempsize;

		if(compressedsize<desiredsize){
			QtableMove(1, 1);
			Qtabledir = 1;	/*small index to large index*/
		}
		else{
			QtableMove(0, 1);
			Qtabledir = 2;	/*large index to small index*/
		}

		ChangeQtable();
		return;
	}
	else{
		if((sizedifference>tempsize)&&(tempsize>0)){
			sizedifference = tempsize;

			QtableMove(1, 1);
			Qtabledir = 1;

			ChangeQtable();

			return;
		}
		else if((sizedifference>tempsize)&&(tempsize<0)){

			if(fragment > (desiredsize/pdevInfo.fsDepend/2)){
				tempsize = fabs(tempsize);

				/*if(tempsize%pdevInfo.fsDepend)*/{
					tempsize = tempsize/pdevInfo.fsDepend+1;
				}
				/*else{
					tempsize = tempsize/pdevInfo.fsDepend;
				}*/

				fragment = fragment - tempsize;

				*flag = 1;
				sizedifference = 0;
				printf("method 1: fragment = %d\n", fragment);
				return;
			}
			else if(fragment > 0){
				tempsize = fabs(tempsize);

				/*if(tempsize%pdevInfo.fsDepend)*/{
					tempsize = tempsize/pdevInfo.fsDepend+1;
				}
				/*else{
					tempsize = tempsize/pdevInfo.fsDepend;
				}*/

				fragment = fragment - tempsize;

				if(fragment<0){
					fragment = fragment + tempsize;

					QtableMove(0, 1);
					ChangeQtable();

					/*if(sizedifference%pdevInfo.fsDepend){
						sizedifference = sizedifference/pdevInfo.fsDepend+1;
					}
					else*/{
						sizedifference = sizedifference/pdevInfo.fsDepend;
					}
					fragment = fragment + sizedifference;
				}

				*flag = 1;
				sizedifference = 0;
				printf("method 2: fragment = %d\n", fragment);
				return;
			}
			else{
				QtableMove(0, 1);
				ChangeQtable();

				*flag = 1;

				/*if(sizedifference%pdevInfo.fsDepend){
					sizedifference = sizedifference/pdevInfo.fsDepend+1;
				}
				else*/{
					sizedifference = sizedifference/pdevInfo.fsDepend;
				}

				fragment = fragment + sizedifference;
				printf("method 3: fragment = %d\n", fragment);
				sizedifference = 0;

				return;
			}
		}

		/*if(fabs(tempsize) > fabs(sizedifference)){

			if(fabs(fragment) > (pdevInfo.fsDepend)){
				if(((fragment>0)&&(tempsize>0))||((fragment<0)&&(tempsize<0))){
					if(Qtabledir == 1){
						QtableMove(0, 2);
					}
					else if(Qtabledir == 2){
						QtableMove(1, 2);
					}
					ChangeQtable();

					delfragment = 1;
					return;
				}
				else{
					*flag = 1;
					sizedifference = 0;
					fragment = fragment + tempsize;
					printf("fragment = %d\n", fragment);
					return;
				}
			}

			if(Qtabledir == 1){
				QtableMove(0, 1);
			}
			else if(Qtabledir == 2){
				QtableMove(1, 1);
			}
			ChangeQtable();
			tempsize = sizedifference;

			*flag = 1;
			sizedifference = 0;
			fragment = fragment + tempsize;
			printf("fragment = %d\n", fragment);
			return;
		}
		else if(fabs(tempsize) < fabs(sizedifference)){
			sizedifference = tempsize;

			if(compressedsize<desiredsize){
				QtableMove(1, 1);
				Qtabledir = 1;
			}
			else{
				QtableMove(0, 1);
				Qtabledir = 2;
			}

			ChangeQtable();
			return;
		}
		else{
			*flag = 1;
			sizedifference = 0;
			fragment = fragment + tempsize;
			printf("fragment = %d\n", fragment);
			return;
		}*/
	}

}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuDateTimeSetInit                                          *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 **************************************************************************/
void menuDateStampInit(UINT32 key)
{
	printf("Date Stamp Init\n");
	ui8SubPvMenuIdx = sPara.stampFlag + 1;
	menuDateStampSet(key);
}

UINT32 menuDateStampSet(UINT32 key)
{
	if (key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
		}
		else
		{
			ui8SubPvMenuIdx = 2;
		}
	}
	else if (key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < 2)
		{
			ui8SubPvMenuIdx++;
		}
		else
		{
			ui8SubPvMenuIdx = 1;
		}		
	}
	else if (key == UI_KEY_FUNC_OK)
	{		
		sPara.stampFlag = ui8SubPvMenuIdx - 1;
	}
	
	osdFWBuffChange();	
	osdClearScreen(0);	
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubPvMenuIdx, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(Date_stamp_s));							
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, (ui8SubPvMenuIdx != 1) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(dc_ON_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, dc_GetString(dc_OFF_s));		
	osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * (sPara.stampFlag+1), UserIcon10x20, (ui8SubPvMenuIdx != (sPara.stampFlag+1)) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");				
	
	osdHWBuffChange();		

	return 0;
}
 
/**************************************************************************
 *                                                                        *
 *  Function Name: menuEVSetInit                                          *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 **************************************************************************/
UINT8 ui8EvSelect;
UINT8 ui8SelEvMode;

void menuEVSetInit(UINT32 key)
{	
	ui8SubPvMenuIdx = 1;
	ui8EvSelect = pvEvCmpIdx;
	ui8SelEvMode = 0;
	menuEVSet(key);
}

UINT32 menuEVSet(UINT32 key)
{
	UINT8 str[10];

	if (key == UI_KEY_DIR_DOWN)
	{
		if (ui8SelEvMode)
		{
			if (ui8EvSelect < (C_EVCMP_MAX_INDEX-1))
			{
				aaaEvCmpIdxSet(++ui8EvSelect);
			}						
		}
		else 
		{
			return 0;
		}
	}
	else if (key == UI_KEY_DIR_UP)
	{
		if (ui8SelEvMode)
		{			
			if (ui8EvSelect > 0)
			{
				aaaEvCmpIdxSet(--ui8EvSelect);
			}				
		
		}
		else 
		{
			return 0;
		}

	}	
	else if (key == UI_KEY_FUNC_OK)
	{	
		if (ui8SelEvMode)
		{
			pvEvCmpIdx = ui8EvSelect;
			ui8SelEvMode = 0;
			DisplayMenuBack();
			MenuIconDisp(MENU_ICON_DC_OFF, ui8PvMenuIdx, C_PREVIEW_MENUS);							
		}
		else 
		{
			ui8SelEvMode = 1;
			PNL_PreviewSet();
		}
	}

	UINT16 x;
	UINT16 len;

	/* 0 - 8 */
	osdFWBuffChange();			
	osdClearScreen(0);
	if (ui8SelEvMode)
	{
		osdBarDraw(0, 0, PANEL_1_OSD_W, 30, 0x02);
		osdStrDisp(CAP_TITLE_TEXT_X, 4, UserFont10x20, 0xd2, dc_GetString(EV_Adjust_s));							
		
		if (ui8EvSelect > ((C_EVCMP_MAX_INDEX-1)/2))
		{
			sio_psprintf(str, "%c%01d.%01d", '-', uiEvDisplay[(C_EVCMP_MAX_INDEX-1-ui8EvSelect)]/10, uiEvDisplay[(C_EVCMP_MAX_INDEX-1-ui8EvSelect)]%10);
		}
		else if (ui8EvSelect < ((C_EVCMP_MAX_INDEX-1)/2))
		{
			sio_psprintf(str, "%c%01d.%01d", '+', uiEvDisplay[ui8EvSelect]/10, uiEvDisplay[ui8EvSelect]%10);
		}
		else
		{
			sio_psprintf(str, " 0.0");
		}		
		osdStrDraw(MENU_EV_X+24, MENU_EV_Y-30, UserIcon16x20, 0x30, "\x53");	
		osdStrDraw(MENU_EV_X+24, MENU_EV_Y+30, UserIcon16x20, 0x30, "\x54");	
		
		osdStrDisp(MENU_EV_X, MENU_EV_Y, UserFont16x20, 0x30, str); 				

		osdBarDraw(0, PANEL_1_OSD_H-24, PANEL_1_OSD_W, 24, 0x02);		
#if 0
		len = strlen(dc_GetString(dc_Menu_s)) + strlen(dc_GetString(Exit_s))+6;
		x = (PANEL_1_OSD_W-len*MENU_TIP_FONT_W)/2;
		osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd2, "[ ");
		x += 2*MENU_TIP_FONT_W;
		osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd2, dc_GetString(dc_Menu_s));
		len = strlen(dc_GetString(dc_Menu_s));
		x += len*MENU_TIP_FONT_W;
		osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd2, " ]: ");	
		x += 4*MENU_TIP_FONT_W;
		osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd2, dc_GetString(Exit_s));
#endif
	}
	else
	{
		osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(EV_Adjust_s));									
		osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*1, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);			
		
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(EV_s));		
		
		if (pvEvCmpIdx > ((C_EVCMP_MAX_INDEX-1)/2))
		{
			sio_psprintf(str, "%c%01d.%01d", '-', uiEvDisplay[(C_EVCMP_MAX_INDEX-1-pvEvCmpIdx)]/10, uiEvDisplay[(C_EVCMP_MAX_INDEX-1-pvEvCmpIdx)]%10);
		}
		else if (pvEvCmpIdx < ((C_EVCMP_MAX_INDEX-1)/2))
		{
			sio_psprintf(str, "%c%01d.%01d", '+', uiEvDisplay[pvEvCmpIdx]/10, uiEvDisplay[pvEvCmpIdx]%10);
		}
		else
		{
			sio_psprintf(str, " 0.0");
		}		

		osdFrameDrawX(CAP_MENU_X+CAP_MENU_WIDTH-10*5-4, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1-6, 10*4+10, 20+10, 0x01, 0x02);
		osdStrDisp(CAP_MENU_X+CAP_MENU_WIDTH-10*5, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, str);

	}
	
	osdHWBuffChange();		
	
}

/**
**/
extern UINT32 vcEvCmpIdx;

void menuMvEVSetInit(UINT32 key)
{	
	ui8SubPvMenuIdx = 1;
	ui8EvSelect = vcEvCmpIdx;
	ui8SelEvMode = 0;
	menuMvEVSet(key);
}

UINT32 menuMvEVSet(UINT32 key)
{
	UINT8 str[10];

	if (key == UI_KEY_DIR_DOWN)
	{
		if (ui8SelEvMode)
		{
			if (ui8EvSelect < (C_EVCMP_MAX_INDEX-1))
			{
				aaaEvCmpIdxSet(++ui8EvSelect);
			}				
		}
		else 
		{
			return 0;
		}
	}
	else if (key == UI_KEY_DIR_UP)
	{
		if (ui8SelEvMode)
		{					
			if (ui8EvSelect > 0)
			{
				aaaEvCmpIdxSet(--ui8EvSelect);
			}						
		}
		else 
		{
			return 0;
		}

	}	
	else if (key == UI_KEY_FUNC_OK)
	{	
		if (ui8SelEvMode)
		{
			vcEvCmpIdx = ui8EvSelect;
			ui8SelEvMode = 0;
			DisplayMenuBack();
			MenuIconDisp(MENU_ICON_DV_OFF, ui8MvMenuIdx, C_MOVIE_MENUS);							
		}
		else 
		{
			ui8SelEvMode = 1;
			PNL_PreviewSet();
		}
	}

	UINT16 x;
	UINT16 len;

	/* 0 - 8 */
	osdFWBuffChange();			
	osdClearScreen(0);
	if (ui8SelEvMode)
	{
		osdBarDraw(0, 0, PANEL_1_OSD_W, 30, 0x02);
		osdStrDisp(CAP_TITLE_TEXT_X, 4, UserFont10x20, 0xd2, dc_GetString(EV_Adjust_s));							
		
		if (ui8EvSelect > ((C_EVCMP_MAX_INDEX-1)/2))
		{
			sio_psprintf(str, "%c%01d.%01d", '-', uiEvDisplay[(C_EVCMP_MAX_INDEX-1-ui8EvSelect)]/10, uiEvDisplay[(C_EVCMP_MAX_INDEX-1-ui8EvSelect)]%10);
		}
		else if (ui8EvSelect < ((C_EVCMP_MAX_INDEX-1)/2))
		{
			sio_psprintf(str, "%c%01d.%01d", '+', uiEvDisplay[ui8EvSelect]/10, uiEvDisplay[ui8EvSelect]%10);
		}
		else
		{
			sio_psprintf(str, " 0.0");
		}		

		osdStrDraw(MENU_EV_X+24, MENU_EV_Y-30, UserIcon16x20, 0x30, "\x53");	
		osdStrDraw(MENU_EV_X+24, MENU_EV_Y+30, UserIcon16x20, 0x30, "\x54");	
		
		osdStrDisp(MENU_EV_X, MENU_EV_Y, UserFont16x20, 0x30, str); 				

		osdBarDraw(0, PANEL_1_OSD_H-24, PANEL_1_OSD_W, 24, 0x02);	
		
#if 0				
		len = strlen(dc_GetString(dc_Menu_s)) + strlen(dc_GetString(Exit_s))+6;
		x = (PANEL_1_OSD_W-len*MENU_TIP_FONT_W)/2;
		osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd2, "[ ");
		x += 2*MENU_TIP_FONT_W;
		osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd2, dc_GetString(dc_Menu_s));
		len = strlen(dc_GetString(dc_Menu_s));
		x += len*MENU_TIP_FONT_W;
		osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd2, " ]: ");	
		x += 4*MENU_TIP_FONT_W;
		osdStrDisp(x, MENU_TIP_Y, UserFont10x20, 0xd2, dc_GetString(Exit_s));
#endif		
	}
	else
	{
		osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(EV_Adjust_s));									
		osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*1, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);			
		
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(EV_s));		
		
		if (vcEvCmpIdx > ((C_EVCMP_MAX_INDEX-1)/2))
		{
			sio_psprintf(str, "%c%01d.%01d", '-', uiEvDisplay[(C_EVCMP_MAX_INDEX-1-vcEvCmpIdx)]/10, uiEvDisplay[(C_EVCMP_MAX_INDEX-1-vcEvCmpIdx)]%10);
		}
		else if (vcEvCmpIdx < ((C_EVCMP_MAX_INDEX-1)/2))
		{
			sio_psprintf(str, "%c%01d.%01d", '+', uiEvDisplay[vcEvCmpIdx]/10, uiEvDisplay[vcEvCmpIdx]%10);
		}
		else
		{
			sio_psprintf(str, " 0.0");
		}		

		osdFrameDrawX(CAP_MENU_X+CAP_MENU_WIDTH-10*5-4, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1-6, 10*4+10, 20+10, 0x01, 0x02);
		osdStrDisp(CAP_MENU_X+CAP_MENU_WIDTH-10*5, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, str);
	}
	
	osdHWBuffChange();		
	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuImageSizeInit                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 **************************************************************************/
void menuImageSizeInit(UINT32 key)
{	
	ui8SubPvMenuIdx = sPara.ui8ImageSize;
	g_ui8TotalItem = 5;	
	g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
	g_ui8SelOKIdx = ui8SubPvMenuIdx-1;
	menuImageSizeSet(key);
}

UINT32 menuImageSizeSet(UINT32 key)
{	
	if (key == UI_KEY_FUNC_OK)
	{						
		sPara.ui8ImageSize = ui8SubPvMenuIdx;
		switch (ui8SubPvMenuIdx)
		{				
			case ImageSize1600:
				sysgSnapSizeSet(1600, 1200);
				break;
		
			case ImageSize1280:
				sysgSnapSizeSet(1280, 1024);
				break;

			case ImageSize1024:
				sysgSnapSizeSet(1024, 768);			
				break;							
				
			case ImageSize800:
				sysgSnapSizeSet(800, 600);
				break;
								
			case ImageSize640:
				sysgSnapSizeSet(640, 480);
				break;
		}		
		g_ui8SelOKIdx = ui8SubPvMenuIdx - 1;
	}
	else if(key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
			if (ui8SubPvMenuIdx == g_ui8TopIdx)
			{
				g_ui8TopIdx--;
			}			
		}	
		else
		{
			ui8SubPvMenuIdx = g_ui8TotalItem;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;			
		}
	}
	else if(key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < g_ui8TotalItem)
		{
			ui8SubPvMenuIdx++;		
			if ((g_ui8TopIdx + MAX_MENU_ITEMS) < ui8SubPvMenuIdx)
			{
				g_ui8TopIdx++;
			}			
		}
		else
		{
			ui8SubPvMenuIdx = 1;
			g_ui8TopIdx = (ui8SubPvMenuIdx+1)/MAX_MENU_ITEMS;
		}		
	}	

	osdFWBuffChange();	
	osdClearScreen(0);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(Resulotion_s));											
	DisplayMenuText(Size_1600_s, g_ui8TopIdx, g_ui8SelOKIdx, ui8SubPvMenuIdx-1, g_ui8TotalItem);	
	osdHWBuffChange();	

	return 0;
}
/**
	Paul@2005/10/19 
**/
void SnapImageSizeSet(void)
{
	switch (sPara.ui8ImageSize)
	{				
		case ImageSize1600:
			sysgSnapSizeSet(1600, 1200);			
			break;			
	
		case ImageSize1024:
			sysgSnapSizeSet(1024, 768);			
			break;			

		case ImageSize1280:
			sysgSnapSizeSet(1280, 1024);
			break;
			
		case ImageSize800:
			sysgSnapSizeSet(800, 600);
			break;
							
		case ImageSize640:
			sysgSnapSizeSet(640, 480);
			break;
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuPreview                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 **************************************************************************/
void menuPreviewInit(UINT32 key)
{
	UINT32 pipAddr;
	isMenuOn = 1;	
	DisplayMenuBack();	
	zoomFactSet(100);
	MenuIconDisp(MENU_ICON_DC_OFF, ui8PvMenuIdx, C_PREVIEW_MENUS);	
	menuPvInit[ui8PvMenuIdx](key);
	
}

void menuPreviewProc(UINT32 key)
{
	switch (key)
	{
		case UI_KEY_ACCIDENT:		
		case UI_KEY_FUNC_MENU:
		case  	UI_KEY_FUNC_B:
			isMenuOn = 0;
			nandWriteParameters();
			osdClearScreen(0);			
			break;

		case UI_KEY_DIR_LEFT:
			if (ui8SelEvMode)
			{
				ui8SelEvMode = 0;
				DisplayMenuBack();
			}
			if (ui8PvMenuIdx > 0)
			{
				ui8PvMenuIdx--;
			}
			else
			{
				ui8PvMenuIdx = C_PREVIEW_MENUS-1;
			}
			MenuIconDisp(MENU_ICON_DC_OFF, ui8PvMenuIdx, C_PREVIEW_MENUS);			
			menuPvInit[ui8PvMenuIdx](key);								
			break;

		case UI_KEY_DIR_RIGHT:
			if (ui8SelEvMode)
			{
				ui8SelEvMode = 0;
				DisplayMenuBack();
			}		
			if (ui8PvMenuIdx < (C_PREVIEW_MENUS-1))
			{
				ui8PvMenuIdx++;
			}
			else
			{
				ui8PvMenuIdx = 0;
			}		
			MenuIconDisp(MENU_ICON_DC_OFF, ui8PvMenuIdx, C_PREVIEW_MENUS);			
			menuPvInit[ui8PvMenuIdx](key);
			break;			

		case UI_KEY_DIR_UP:
		case UI_KEY_DIR_DOWN:	
		case UI_KEY_FUNC_OK:
			menuPvProc[ui8PvMenuIdx](key);
			break;
	}
	
	return;
}

/**************************************************************************
 *					     Movie menu                                       * 
 **************************************************************************/
#if 0 
/**************************************************************************
 *                                                                        *
 *  Function Name: menuVideoSizeInit                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 **************************************************************************/ 
void menuVideoSizeInit(UINT32 key)
{
	ui8SubPvMenuIdx = sPara.VideoClipSize;
	menuVideoSizeSet(key);
}

UINT32 menuVideoSizeSet(UINT32 key)
{
	if (key == UI_KEY_DIR_UP)
	{
		if (ui8SubPvMenuIdx > 1)
		{
			ui8SubPvMenuIdx--;
		}
	}
	else if (key == UI_KEY_DIR_DOWN)
	{
		if (ui8SubPvMenuIdx < VideoSize320)
		{
			ui8SubPvMenuIdx++;
		}		
	}
	else if (key == UI_KEY_FUNC_OK)
	{		
		sPara.VideoClipSize = ui8SubPvMenuIdx;
	}
	
	osdFWBuffChange();	
	osdClearScreen(0);	
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubPvMenuIdx, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(Resulotion_s));													
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, (ui8SubPvMenuIdx != 1) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, "640 x 480 ");
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, (ui8SubPvMenuIdx != 2) ? CR_NORMAL_TEXT : CR_SELECT_TEXT, "320 x 240 ");		
	
	osdStrDisp(CAP_ICON_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT * sPara.VideoClipSize, UserIcon10x20, (ui8SubPvMenuIdx != sPara.VideoClipSize) ? CR_NOR_BUTTON : CR_SEL_BUTTON, "\x56");								

	osdHWBuffChange();		
}
 #endif
 
/**************************************************************************
 *                                                                        *
 *  Function Name: menuMovie                                              *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 **************************************************************************/
void menuMovieInit(UINT32 key)
{
	isMenuOn = 1;
	DisplayMenuBack();
	zoomFactSet(100);
	MenuIconDisp(MENU_ICON_DV_OFF, ui8MvMenuIdx, C_MOVIE_MENUS);
	menuMvInit[ui8MvMenuIdx](key);	
}

void menuMovieProc(UINT32 key)
{
	switch (key)
	{
		case UI_KEY_ACCIDENT:
		case UI_KEY_FUNC_MENU:
		case  	UI_KEY_FUNC_B:
			isMenuOn = 0;
			nandWriteParameters();		
			break;

		case UI_KEY_DIR_LEFT:
			if (ui8SelEvMode)
			{
				ui8SelEvMode = 0;
				DisplayMenuBack();
			}
			
			if (ui8MvMenuIdx > 0)
			{
				ui8MvMenuIdx--;
			}
			else
			{
				ui8MvMenuIdx = C_MOVIE_MENUS-1;
			}
			MenuIconDisp(MENU_ICON_DV_OFF, ui8MvMenuIdx, C_MOVIE_MENUS);
			menuMvInit[ui8MvMenuIdx](key);								
			break;

		case UI_KEY_DIR_RIGHT:
			if (ui8SelEvMode)
			{
				ui8SelEvMode = 0;
				DisplayMenuBack();
			}
			
			if (ui8MvMenuIdx < (C_MOVIE_MENUS-1))
			{
				ui8MvMenuIdx++;
			}
			else
			{
				ui8MvMenuIdx = 0;
			}		
			MenuIconDisp(MENU_ICON_DV_OFF, ui8MvMenuIdx, C_MOVIE_MENUS);
			menuMvInit[ui8MvMenuIdx](key);			
			break;

		case UI_KEY_DIR_UP:
		case UI_KEY_DIR_DOWN:
		case UI_KEY_FUNC_OK:	
			menuMvProc[ui8MvMenuIdx](key);	
			break;			
	}

	return 0;
	
}

UINT32 MenuT[C_PBIMAGE_MENU_NUM]=
{
	2,2,2,2,2,1,1
};

UINT8 ui8TotalMenuNum = 0;
UINT8 ui8MenuIndex = 0;
UINT8 ui8SubMenuIndex = 1;

void menuDeleteInit(UINT32 key)
{

	osdFWBuffChange ();
	ClearMenuContent ();		
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20,CR_TITLE_NOR_TEXT, dc_GetString( dc_Delete_s));
	
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubMenuIndex, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
	if(ui8SubMenuIndex == 1)	
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_Current_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_All_s));
	}
	else
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_Current_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_All_s));		
	}
	osdHWBuffChange ();	
}

void menuDcProtectInit(UINT32 key)
{
	osdFWBuffChange();
	ClearMenuContent();
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20,CR_TITLE_NOR_TEXT, dc_GetString( dc_Protect_s));
	
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubMenuIndex, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
	if(ui8SubMenuIndex == 1)	
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_Current_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_All_s));
	}
	else
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_Current_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_All_s));		
	}
	osdHWBuffChange();
}

void menuNoProtectInit(UINT32 key)
{

	osdFWBuffChange();
	ClearMenuContent();
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20,CR_TITLE_NOR_TEXT, dc_GetString(dc_NotProte_s));
	
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubMenuIndex, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
	if(ui8SubMenuIndex == 1)	
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_Current_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_All_s));
	}
	else
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_Current_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_All_s));		
	}
	osdHWBuffChange ();
}

void menuPrintInit(UINT32 key)
{
	osdFWBuffChange();
	ClearMenuContent();	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20,CR_TITLE_NOR_TEXT, dc_GetString( dc_Print_s));
	
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubMenuIndex, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
	if(ui8SubMenuIndex == 1)	
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_One_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_All_s));
	}
	else
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_One_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_All_s));		
	}
	osdHWBuffChange();	
}

void menuDpofInit(UINT32 key)
{
	osdFWBuffChange();
	ClearMenuContent();	
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20,CR_TITLE_NOR_TEXT, dc_GetString( dc_DPOF_s));	

	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT*ui8SubMenuIndex, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
	if(ui8SubMenuIndex == 1)	
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_One_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_All_s));
	}
	else
	{
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_NORMAL_TEXT, dc_GetString(dc_One_s));
		osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_All_s));		
	}
	osdHWBuffChange ();	
}

void menuAutoInit(UINT32 key)
{	
	osdFWBuffChange();
	ClearMenuContent();	
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20,CR_TITLE_NOR_TEXT, dc_GetString(dc_Slide_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20,CR_SELECT_TEXT, dc_GetString(dc_Slide_s));
	osdHWBuffChange();
}

void menuOpenInit(UINT32 key)
{
	osdFWBuffChange();	
	ClearMenuContent();		
	osdBarDraw(CAP_BAR_X, CAP_BAR_Y+CAP_TEXT_HEIGHT, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, CR_SELECT_BAR);
	osdStrDisp(CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, CR_TITLE_NOR_TEXT, dc_GetString(dc_OpenPic_s));
	osdStrDisp(CAP_TEXT_OFF_X, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*1, UserFont10x20, CR_SELECT_TEXT, dc_GetString(dc_OpenPic_s));
	osdHWBuffChange();
}


#if 1

/* Message display john@01/22/2006 */ //268 269 same
UINT8 *ui32MsgStrings[MAXMSG_ID][LANGUAGES] = {
		{
			"Warning",
			"����",	
			"ĵ�i",
			"Advertencia",
			"����֧է���֧اէ֧ߧڧ�",
			"Warnung",
			"alerte",//"alert",
			"Avviso",
			"Advert��ncia",
			"���",
		},
		{
			"File Protected!",
			"�ļ�����",
			"���O�@",		
			"Protecci��n de Archivo",
			"���ѧ�ڧ�� ��ѧۧݧ�",
			"Dateischutz",
			"dossier prot��g��",
			"Doc. protezione",
			"Protec\xe7\xe3o de documneto",
			"���� ������Ʈ",
		},
		 {
			"Card protect",
			"������",
			"�d�O�@",
			"Protecci��n de tarjeta",
			"���ѧ�ڧ�� ��ݧѧ��",
			"Karteschutz",
			"carte prot��g��e",//"cate prot��g��e",
			"Scheda protezione",
			"Protec\xe7\xe3o de cart\xe3o",
			"ī�� ����",
      },
		{
			"No Files!",
			"�ļ�������",
			"��󤣦s�b",
			"Sin  Archivo ",
			"���֧� ��ѧۧݧ�",
			"keine Datei",
			"dossier exist pas",
			"Non esiste il doc.",
			"Documento n\xe3o existe",
			"���� ����",
		},
		{
			"Success",//for bug 44 2005-10-26 16:14
	        "�������!",
			"�ާ@����!",
	        "Confirmar",
	        "����",
	        "Erfolg",
	        "r��ussit",
	        "Complete",
	        "Sucesso",
	        "����",
		},
		#endif
};


UINT8* GetMsgString(UINT16 stringId)
{
    UINT8* str;
    UINT32 languageId = sPara.language;

    if (languageId >= LANGUAGES)
    {
		languageId = LanguageEnglish;
    }

    if(stringId >= MAXMSG_ID) 
    {
		return 0;
    }

    if(ui32MsgStrings[stringId][languageId][0]=='\0')
    {
	    str = ui32MsgStrings[stringId][0];
    }
	else
	{
	    str = ui32MsgStrings[stringId][languageId];
	}

	return str;
}


void MessageShow(UINT8 index)
{
	UINT32 dispW, dispH;
	UINT32 gpb;
	pbImagePaste(SetICON[12][2], SetICON[12][3], SetICON[12][4],SetICON[12][5]);

//	pbImagePaste(menuback, sizeof(menuback), 0, 0);		
	osdHWBuffChange();
	osdClearScreen (0);

	switch (index)
		{
		case ErrfileProtect:		/* Protect */
			#if 0 //fqdao_modify  06.5.30
			 	osdFWBuffChange();  // tzw add for bug 4 060420
			 	return ; 
			#endif		
				osdStrDisp (CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20,CR_TITLE_NOR_TEXT, GetMsgString(WarnMsg));				
	    		       osdStrDisp (CAP_TEXT_OFF_X + 10, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2 - 10, UserFont10x20, 0x10, GetMsgString(fileProtectMsg));    
				break;
		case ErrCardProtect:			
				osdStrDisp (CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20,CR_TITLE_NOR_TEXT, GetMsgString(WarnMsg));				
				osdStrDisp (CAP_TEXT_OFF_X + 10, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2 - 10, UserFont10x20, 0x10, GetMsgString(CardProtectMsg));		
				break;
		case ErrNoFile:			
				osdStrDisp (CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20,CR_TITLE_NOR_TEXT, GetMsgString(WarnMsg));				
				osdStrDisp (CAP_TEXT_OFF_X + 20, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2 - 10, UserFont10x20, 0x10, GetMsgString(NoFileMsg));		
				break;		
		case MenuSuccess:
				osdStrDisp (CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, 0x00, "           ");			
				osdStrDisp (CAP_TEXT_OFF_X + 30, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2 - 10, UserFont10x20, 0x70, GetMsgString(MenuSuccessMsg));		
				break;		
		case MenuWaite:
				osdStrDisp (CAP_TITLE_TEXT_X, CAP_TITLE_TEXT_Y, UserFont10x20, 0x00, "           ");			
				osdStrDisp (CAP_TEXT_OFF_X + 30, CAP_TEXT_OFF_Y+CAP_TEXT_HEIGHT*2 - 10, UserFont10x20, 0x70, GetMsgString(MenuWaiteMsg));		
				break;	
		}
	
	hwWait(0, 1000);	
	osdFWBuffChange();
}
#endif 
extern UINT8 DeleImage_Flag ;

UINT32 menuDeleteProc(UINT32 key)
{
	if(ui8SubMenuIndex == 1)
	{				
		printf("Delete One\n");
		UINT32	err ;
		if((xdcfCurFileByPosSet(fileIdx)) != 0)
		{
			#ifdef	OSD_DEBUG
			printf("\nErase	error1...(idx=%d)",	fileIdx);
			#endif
		}
		else if (xdcfCurFileAttrGet( &fileattr) == 0)
		{
//			xdcfDelOnlyFileFuncSet(readOnlyFileErase);
			// icon of protect file   
			if (IsSDProtect ())
			{
				MessageShow (ErrCardProtect);				
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);										
				return 0;
			}

			if (fileattr.attribute & xDCF_ATTR_READONLY)
			{
				MessageShow(ErrfileProtect);
				osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);															
				return 0;
			}
		
			#if 1 // for bug 89 tzw add 060218
			if(( uiState & UI_MODE_MASK ) == UI_MODE_PHOTO )
			{
				DeleImage_Flag = 0 ;
			}
			#endif 
			xdcfOneFileErase(fileattr.name);
			err	= xdcfFileCountGet(&fileCnt);
			if(!fileCnt) 
			{
				xdcfDirChange("..");
				xdcfDirCountGet(&fileCnt);
				xdcfCurDirPosGet(&fileCnt);
				xdcfCurDirAttrGet(&dirattr);
			}
			else if(fileIdx>fileCnt) 
			{
				fileIdx = fileCnt;
			}
		}
	}
	else if(ui8SubMenuIndex == 2)
	{
		printf("Delete All\n");
		UINT32	storage;
		UINT32	Idx, Counts;
		
		xdcfActiveDevIdGet(&storage);
		if((storage != DRIVE_NAND) && isCardProtect())
		{
			hwWait(0, 1000);				
		}
		else
		{				

			#if 0 // fqdao _modify  for bug del  profile  06.5.29
			xdcfFileCountGet(&Counts);
			for (Idx = 1; Idx < Counts; Idx++)
			{
				printf("Delete One\n");
				UINT32	err ;
				if((xdcfCurFileByPosSet(fileIdx)) != 0)
				{
					#ifdef	OSD_DEBUG
					printf("\nErase	error1...(idx=%d)",	fileIdx);
					#endif
				}
				else if (xdcfCurFileAttrGet( &fileattr) == 0)
				{
		//			xdcfDelOnlyFileFuncSet(readOnlyFileErase);
					// icon of protect file    

					if (IsSDProtect ())
						{
							MessageShow (ErrCardProtect);				
							osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);										
						return 0;
						}					
					
					if (fileattr.attribute & xDCF_ATTR_READONLY)
						{
							MessageShow(ErrfileProtect);
							osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);																		
							return 0;
						}
				

					xdcfOneFileErase(fileattr.name);
					err	= xdcfFileCountGet(&fileCnt);
					if(!fileCnt) 
					{
						xdcfDirChange("..");
						xdcfDirCountGet(&fileCnt);
						xdcfCurDirPosGet(&fileCnt);
						xdcfCurDirAttrGet(&dirattr);
					}
					else if(fileIdx>fileCnt) 
					{
						fileIdx = fileCnt;
					}
				}				
			}
			#endif
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
			{
				fileIdx = fileCnt;
			}
			xdcfFileCountGet(&fileCnt);
		}
	}

	isMenuOn = 0;		/* Exit menu to noraml */
	
	return 1;
}

UINT32 menuProtectProc(UINT32 key)
{
	if (IsSDProtect ())
	{
		MessageShow (ErrCardProtect);	
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);					
		return 0;
	}
	if(ui8SubMenuIndex == 1)
	{				
		printf("Protect One\n");
		xdcfCurFileByPosSet(fileIdx) ;
		xdcfCurFileAttrGet(&fileattr) ;
		fileattr.attribute |= xDCF_ATTR_READONLY;		
		xdcfCurFileAttrSet(fileattr.attribute) ;
	}
	else if(ui8SubMenuIndex == 2)
	{
		printf("Protect All\n");
		UINT32	i;
		xdcfFileCountGet(&fileCnt);
		MessageShow (MenuWaite);	//fqdao
		for(i=1; i<=fileCnt; i++)
		{
			xdcfCurFileByPosSet(i) ;
			xdcfCurFileAttrGet(&fileattr) ;
			fileattr.attribute |= xDCF_ATTR_READONLY;
			xdcfCurFileAttrSet(fileattr.attribute) ;
		}
	}

	isMenuOn = 0;		/* Exit menu to noraml */
	
	return 1;
}

UINT32 menuNoProtectProc(UINT32 key)
{

	if (IsSDProtect ())
	{
		MessageShow (ErrCardProtect);	
		osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);					
		return 0;
	}
	if(ui8SubMenuIndex == 1)
	{		
		printf("Unprotect One\n");
		xdcfCurFileByPosSet(fileIdx) ;
		xdcfCurFileAttrGet(&fileattr) ;
		fileattr.attribute &= (~xDCF_ATTR_READONLY);
		xdcfCurFileAttrSet(fileattr.attribute) ;
	}	
	else if(ui8SubMenuIndex == 2)
	{
		printf("Unprotect All\n");
		UINT32	i;
		xdcfFileCountGet(&fileCnt);
		MessageShow (MenuWaite);	//fqdao
		for(i=1;i<=fileCnt;i++)
		{
			xdcfCurFileByPosSet(i) ;
			xdcfCurFileAttrGet(&fileattr) ;
			fileattr.attribute &= (~xDCF_ATTR_READONLY);			
			xdcfCurFileAttrSet(fileattr.attribute) ;
		}
	}
	
	isMenuOn = 0;		/* Exit menu to noraml */
	
	return 1;
}

/**
**/
extern UINT32 fileIdx;

extern UINT8 G_PrintingNoExit;
extern UINT8 G_Printing;

UINT8 g_ui8PrintOneIdx;
UINT8 g_ui8InPrintMode = 0;

static UINT32 g_DateStamp;
static UINT32 PaperSize;
static UINT32 qty;
static UINT32 *pCapPaperSizeArray;
static UINT32 capSelectState;
static UINT32 capSize;

#define PRINT_SEL_FILE		0
#define PRINT_SEL_QTY		1
#define PRINT_SEL_PAPER		2
#define PRINT_SEL_DATE		3
#define PRINT_SEL_PRINT     4

#define PRINT_CR_SELECT     0x32
#define PRINT_CR_NORMAL     0xd2

void PictBridgeMultiPrint(UINT32 paperSize, UINT32 qty, int dateStamp)
{
	UINT32 jobCfgHnd;
	UINT32 prtInfoHnd;
	UINT32 fileID;
	UINT32 ret;
	UINT32 *pCapabilityArray;
	UINT32 capabilitySize;
	UINT32 i;
	char * fullPath;
	UINT8 fullFileName[30];	
	xdcfAttrElm_t pattrDirElm;
	xdcfAttrElm_t pattrFileElm;

	ret = dspPrtJobNew(&jobCfgHnd);
	if (ret != SUCCESS) 
	{
		printf("dspPrtJobNew() FAIL, ret = 0x%x\n",ret);
		return;
	}
	
	/*Date Stamp*/
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_datePrints,
		&pCapabilityArray, &capabilitySize,0);	
	if (ret == SUCCESS) {		
		printf("Printer supported datePrints[%d] = \n",capabilitySize);
		for (i = 0;i < capabilitySize;i++) {
			printf(" %x\n", pCapabilityArray[i]);
		}
	}
	else {
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}		
	
	if (dateStamp) {
		printf("dpsJobCfgDataSet()... date print\n");
		ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_datePrint
								,K_DPS_HEX_PARAVAL_DatePrint_On);
		if (ret != SUCCESS) {
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n",ret);
		return;
		}
	}	
	
 	/* Quality */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_qualities,
								&pCapabilityArray, &capabilitySize,0);
	if (ret == SUCCESS) 
	{
		printf("Printer supported paperTypes[%d] = \n",capabilitySize);
		for (i = 0;i < capabilitySize;i++) 
			printf(" %x\n", pCapabilityArray[i]);
	}
	else 
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	/* job config data quality*/
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_quality, pCapabilityArray[0]);
	if (ret != SUCCESS) 
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n",ret);
		return;
	}

 	/* PaperType */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_paperTypes,
								&pCapabilityArray, &capabilitySize,0);
	if (ret == SUCCESS) 
	{
		printf("Printer supported paperTypes[%d] = \n",capabilitySize);
		for (i = 0;i < capabilitySize;i++) 
			printf(" %x\n", pCapabilityArray[i]);
	}
	else 
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);	
	
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_paperType, pCapabilityArray[0]);
	if (ret != SUCCESS) 
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n",ret);
		return;
	}
	
	/* PaperSize */
	printf("dpsJobCfgDataSet()... PAPER Size = %08x\n",
			paperSize);
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_paperSize,
			paperSize);
	if (ret != SUCCESS) 
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n",ret);
		return;
	}

	/*dxg_FileFilter(JPG);*/	
	/*xdcfTotalFileCountGet(&g_TotalCnt);*/									
	/*SetCurrFile(g_TotalCnt, 1);*/ 	
	for( fileIdx = 1; fileIdx <= fileCnt;fileIdx++)	
	{		
		fullPath = vfsGetCurrDirName();
		xdcfCurFileByPosSet(fileIdx);	
		xdcfCurDirAttrGet(&pattrDirElm);
		xdcfCurFileAttrGet(&pattrFileElm);		
		strncpy(fullFileName,fullPath,7);
		fullFileName[7] = '\\';	
		strncpy(fullFileName+8,pattrDirElm.name,8);
		fullFileName[16] = '\\';	
		strncpy(fullFileName+17,pattrFileElm.name,12);	
		fullFileName[29] = '\0';		
		printf("string len %s \n",fullFileName);
		
		/* print info data */
		printf("dpsPrtInfoHndGet()...\n");
		ret = dpsPrtInfoHndGet(jobCfgHnd,&prtInfoHnd);
		if (ret != SUCCESS) 
		{
			printf("dpsPrtInfoHndGet() FAIL, ret = 0x%x\n",ret);
			return;
		}	
		printf("dpsPrtInfoData_filePath2fileIDGet()...\n");
		
		ret = dpsPrtInfoData_filePath2fileIDGet((UINT8 *)fullFileName,&fileID);
	
		if (ret != SUCCESS) 
		{
			printf("dpsPrtInfoData_filePath2fileIDGet() FAIL, ret = 0x%x\n",ret);
			return;
		}
		/* set infoData fileID*/
		printf("dpsPrtInfoData_fileIDSet()...\n");
		ret = dpsPrtInfoData_fileIDSet(prtInfoHnd,fileID);
		if (ret != SUCCESS) 
		{
			printf("dpsPrtInfoData_fileIDSet() FAIL, ret = 0x%x\n",ret);
			return;
		}
		/* set infoData filename*/
		printf("dpsPrtInfoData_fileNameSet()...\n");
		ret = dpsPrtInfoData_fileNameSet(prtInfoHnd,pattrFileElm.name);
		if (ret != SUCCESS) 
		{
			printf("dpsPrtInfoData_fileNameSet() FAIL, ret = 0x%x\n",ret);
			return;
		}
		/* set infoData date*/
		printf("dpsPrtInfoData_dateSet()...\n");
		struct	tm	*ptime;
		/*UINT8 dateStr[11];*/
                UINT16 temp1;
                UINT16 temp2;
                UINT16 temp3;
		timeGet(&ptime);
		temp1=ptime->tm_year+1900;
		temp2=ptime->tm_mon;
		temp3=ptime->tm_mday;
		ret = dpsPrtInfoData_dateSet(prtInfoHnd,temp1,temp2,temp3,K_DPS_DATE_TYPE1);
		if (ret != SUCCESS) 
		{
			printf("dpsPrtInfoData_dateSet() FAIL, ret = 0x%x\n",ret);
			return;
		}
		/* print cnt*/
		dpsPrtInfoData_copiesSet(prtInfoHnd,qty);
	}
		
	/* print Job Cfg */
	dpsDbg_JobListPrint(jobCfgHnd);
	/* execute this job */
	printf("dspPrtJobExec()...\n");
	
	ret = dspPrtJobExec();
	if (ret != K_DPS_HEX_RESULT_Ok) 
	{
		printf("dspPrtJobExec() FAIL, ret = 0x%x\n",ret);
		return;
	}
	printf("dspPrtJobExec() return.....................\n");	
}          

void PBPrintingAllInit()
{
	/*printingFlag = 1;								
	tolCnt = GetTolFileCnt(JPG);
	i = 0;			
	ShowOSDCounter(DPOFCNT_X, DPOFCNT_Y, i + 1, tolCnt);		
	dxg_DPOFImgToPanelPositionXY( 40, 60 , 128, 96);
	SetCurrFile( i + 1, 1);*/			
	PictBridgeMultiPrint(PaperSize, qty, g_DateStamp);									
	osTimeDly(200);	
	/*dxg_EventSet(DXG_PRINTING);*/		
}
/**
**/
void PictBridgeSinglePrint(UINT32 paperSize, UINT32 qty, int dateStamp)
{
	UINT32 jobCfgHnd;
	UINT32 prtInfoHnd;
	UINT32 fileID;
	UINT32 ret;
	UINT32 *pCapabilityArray;
	UINT32 capabilitySize;
	UINT32 i;
	char * fullPath;
	UINT8 fullFileName[30];	
	xdcfAttrElm_t pattrDirElm;
	xdcfAttrElm_t pattrFileElm;

	fullPath = vfsGetCurrDirName();
	xdcfCurDirAttrGet(&pattrDirElm);
	xdcfCurFileAttrGet(&pattrFileElm);		
	strncpy(fullFileName,fullPath,7);
	fullFileName[7] = '\\';	
	strncpy(fullFileName+8,pattrDirElm.name,8);
	fullFileName[16] = '\\';	
	strncpy(fullFileName+17,pattrFileElm.name,12);	
	fullFileName[29] = '\0';		
	printf("string len %s \n",fullFileName);

	ret = dspPrtJobNew(&jobCfgHnd);
	if (ret != SUCCESS) 
	{
		printf("dspPrtJobNew() FAIL, ret = 0x%x\n",ret);
		return;
	}
	
	/*Date Stamp*/
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_datePrints,
		&pCapabilityArray, &capabilitySize,0);	
	if (ret == SUCCESS) {		
		printf("Printer supported datePrints[%d] = \n",capabilitySize);
		for (i = 0;i < capabilitySize;i++) {
			printf(" %x\n", pCapabilityArray[i]);
		}
	}
	else {
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}		
	
	if (dateStamp) {
		printf("dpsJobCfgDataSet()... date print\n");
		ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_datePrint
								,K_DPS_HEX_PARAVAL_DatePrint_On);
		if (ret != SUCCESS) {
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n",ret);
		return;
		}
	}

 	/* Quality */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_qualities,
								&pCapabilityArray, &capabilitySize,0);
	if (ret == SUCCESS) 
	{
		printf("Printer supported quality[%d] = \n",capabilitySize);
		for (i = 0;i < capabilitySize;i++) 
			printf(" %x\n", pCapabilityArray[i]);
	}
	else 
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	/* job config data quality*/
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_quality, pCapabilityArray[0]);
	if (ret != SUCCESS) 
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n",ret);
		return;
	}

 	/* PaperType */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_paperTypes,
								&pCapabilityArray, &capabilitySize,0);
	if (ret == SUCCESS) 
	{
		printf("Printer supported paperTypes[%d] = \n",capabilitySize);
		for (i = 0;i < capabilitySize;i++) 
			printf(" %x\n", pCapabilityArray[i]);
	}
	else 
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);	
	
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_paperType, pCapabilityArray[0]);
	if (ret != SUCCESS) 
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n",ret);
		return;
	}
	
	/* PaperSize */
	printf("dpsJobCfgDataSet()... PAPER Size = %08x\n",
			paperSize);
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_paperSize,
			paperSize);
	if (ret != SUCCESS) 
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n",ret);
		return;
	}

	/* print info data */

	printf("dpsPrtInfoHndGet()...\n");
	ret = dpsPrtInfoHndGet(jobCfgHnd,&prtInfoHnd);
	if (ret != SUCCESS) 
	{
		printf("dpsPrtInfoHndGet() FAIL, ret = 0x%x\n",ret);
		return;
	}
	printf("dpsPrtInfoData_filePath2fileIDGet()...\n");
	
	ret = dpsPrtInfoData_filePath2fileIDGet((UINT8 *)fullFileName,&fileID);

	if (ret != SUCCESS) 
	{
		printf("dpsPrtInfoData_filePath2fileIDGet() FAIL, ret = 0x%x\n",ret);
		return;
	}
	/* set infoData fileID*/
	printf("dpsPrtInfoData_fileIDSet()...\n");
	ret = dpsPrtInfoData_fileIDSet(prtInfoHnd,fileID);
	if (ret != SUCCESS) 
	{
		printf("dpsPrtInfoData_fileIDSet() FAIL, ret = 0x%x\n",ret);
		return;
	}
	/* set infoData filename*/
	printf("dpsPrtInfoData_fileNameSet()...\n");
	ret = dpsPrtInfoData_fileNameSet(prtInfoHnd,pattrFileElm.name);
	if (ret != SUCCESS) 
	{
		printf("dpsPrtInfoData_fileNameSet() FAIL, ret = 0x%x\n",ret);
		return;
	}
	/* set infoData date*/
	printf("dpsPrtInfoData_dateSet()...\n");
	struct	tm	*ptime;
		/*UINT8 dateStr[11];*/
                UINT16 temp1;
                UINT16 temp2;
                UINT16 temp3;
		timeGet(&ptime);
		temp1=ptime->tm_year+1900;
		temp2=ptime->tm_mon;
		temp3=ptime->tm_mday;
	ret = dpsPrtInfoData_dateSet(prtInfoHnd,temp1,temp2,temp3,K_DPS_DATE_TYPE1);
	if (ret != SUCCESS) 
	{
		printf("dpsPrtInfoData_dateSet() FAIL, ret = 0x%x\n",ret);
		return;
	}
	/* print cnt*/
	ret = dpsPrtInfoData_copiesSet(prtInfoHnd, qty);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_copiesSet() FAIL ret = 0x%x\n", ret);
		return;
	}

	/* print Job Cfg */
	dpsDbg_JobListPrint(jobCfgHnd);
	/* execute this job */
	printf("dspPrtJobExec()...\n");
	
	ret = dspPrtJobExec();
	if (ret != K_DPS_HEX_RESULT_Ok) 
	{
		printf("dspPrtJobExec() FAIL, ret = 0x%x\n",ret);
		return;
	}
	printf("dspPrtJobExec() return.....................\n");	
}

/**
**/
void PBPrintingOneInit()
{
	PictBridgeSinglePrint(PaperSize, qty, g_DateStamp);
	osTimeDly(200);			
	/*dxg_EventSet(DXG_PRINTING);*/	
}

void paperSizeSet(void)
{
	switch(capSelectState)
	{
	    case 0:
		 PaperSize = K_DPS_HEX_PARAVAL_PaperSize_4X6in;					
		 break;
		 
	    case 1:
         PaperSize = K_DPS_HEX_PARAVAL_PaperSize_Letter;					
         break;
		 
        case 2:
		 PaperSize = K_DPS_HEX_PARAVAL_PaperSize_A4;					
		 break;
		 
	    case 3:
         PaperSize = K_DPS_HEX_PARAVAL_PaperSize_Default;					
		 break;
	}
}
/**
**/
void PaperSizeOSD(UINT32 x, UINT32 y, UINT32 paperSize, UINT8 OneorAll)
{
	switch(paperSize)
	{
		case K_DPS_HEX_PARAVAL_PaperSize_4X6in:
	        if(OneorAll==0)
	        {
				osdStrDisp(x, y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, "4X6" );
	        }
			else
			{
				osdStrDisp(x, y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, "4X6" );				
			}
			break;
			
		case K_DPS_HEX_PARAVAL_PaperSize_Letter:
	        if(OneorAll==0)
	        {
				osdStrDisp(x, y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, "LTR" );
	        }
			else
			{
				osdStrDisp(x, y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, "LTR" );								
			}
			break;
			
		case K_DPS_HEX_PARAVAL_PaperSize_A4:
			if(OneorAll==0)
			{
		        osdStrDisp(x, y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, "A4 " );
			}
	        else
	        {
				osdStrDisp(x, y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, "A4 " );											
	        }
			break;
			
		case K_DPS_HEX_PARAVAL_PaperSize_Default:
			if(OneorAll==0)
			{
				osdStrDisp(x, y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, "DEF" );
			}
			else
			{
				osdStrDisp(x, y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, "DEF" );										
			}
			break;
	}
}

#define PRINT_MENU_X		64
#define PRINT_MENU_Y        32
#define PRINT_MENU_W        (PANEL_1_OSD_W-PRINT_MENU_X)
#define PRINT_MENU_H		(PANEL_1_OSD_H-PRINT_MENU_Y-50)
/**
**/
void osdPrintOneSetUI(UINT8 flag)
{
	UINT8 string[10];

	if (flag == 1)
	{
		osdBarDraw(0, PRINT_MENU_Y, PRINT_MENU_W+PRINT_MENU_X, PRINT_MENU_H, 0x02);	
	}	
	osdStrDisp(0, 0, UserIcon10x20, 0xd0, "\x66\x67");
	osdStrDisp(30, 0, UserFont10x20, 0xd0, dc_GetString(Print_One_s));

	sio_psprintf(string, "%04d-%04d", fileIdx, fileCnt);
	osdStrDisp(PRINT_MENU_X, PRINT_MENU_Y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_FILE) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, string);

	sio_psprintf(string, "%02d", qty);
//	osdStrDisp(PRINT_MENU_X, PRINT_MENU_Y+24*1, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_QTY) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, 0);			
	osdStrDisp(PRINT_MENU_X+20, PRINT_MENU_Y+24*1, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_QTY) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, string);
	
	paperSizeSet();
//	osdStrDisp(PRINT_MENU_X, PRINT_MENU_Y+24*2, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, 0);		
	PaperSizeOSD(PRINT_MENU_X+20, PRINT_MENU_Y+24*2, PaperSize, 0);
	
//	osdStrDisp(PRINT_MENU_X, PRINT_MENU_Y+24*3, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_DATE) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, 0);	
	osdStrDisp(PRINT_MENU_X+20, PRINT_MENU_Y+24*3, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_DATE) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, dc_GetString(dc_OFF_s+g_DateStamp));	
	sio_psprintf(string, "[%s]", dc_GetString(dc_Print_s));
	osdStrDisp(PRINT_MENU_X, PRINT_MENU_Y+24*4, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PRINT) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, string);
}

/**
**/
void osdPrintAllSetUI(UINT8 flag)
{
	UINT8 string[10];

	if (flag == 1)
	{
		osdBarDraw(0, PRINT_MENU_Y, PRINT_MENU_W+PRINT_MENU_X, PRINT_MENU_H, 0x02);	
	}		
	osdStrDisp(0, 0, UserIcon10x20, 0xd0, "\x66\x67");
	osdStrDisp(30, 0, UserFont10x20, 0xd0, dc_GetString(Print_All_s));

	sio_psprintf(string, "%02d", qty);
//	osdStrDisp(PRINT_MENU_X, PRINT_MENU_Y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_QTY) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, 0);				
	osdStrDisp(PRINT_MENU_X+20, PRINT_MENU_Y, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_QTY) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, string);
	
	paperSizeSet();
//	osdStrDisp(PRINT_MENU_X, PRINT_MENU_Y+24*1, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PAPER) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, 0);			
	PaperSizeOSD(PRINT_MENU_X+20, PRINT_MENU_Y+24*1, PaperSize, 0);
	
//	osdStrDisp(PRINT_MENU_X, PRINT_MENU_Y+24*2, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_DATE) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, 0);	
	osdStrDisp(PRINT_MENU_X+20, PRINT_MENU_Y+24*2, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_DATE) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, dc_GetString(dc_OFF_s+g_DateStamp));	
	sio_psprintf(string, "[%s]", dc_GetString(dc_Print_s));
	osdStrDisp(PRINT_MENU_X, PRINT_MENU_Y+24*3, UserFont10x20, (g_ui8PrintOneIdx == PRINT_SEL_PRINT) ? PRINT_CR_SELECT : PRINT_CR_NORMAL, string);	
}

#define C_PRINT_MENU_NUM		5

extern UINT32 gPB;

static void DisplayImage(UINT32 ui32FileIdx)
{
	UINT32 dispW, dispH;
	UINT32 err;
	UINT32 ui32FileType;
	
	hwDispPreviewSizeGet(&dispW, &dispH);		
	hwPlayBufSizeSet(dispW,	dispH);
	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);	  /* Disable frame rate	conversion */
	hwCammodeSet(5);	  /* play back mode	*/
	hwCdspDataSource(1);  /* CDSP Data source from DRAM	*/
	
	if ((err = xdcfCurFileByPosSet(ui32FileIdx)) != SUCCESS)
	{
		return -1;
	}	

	err	= xdcfFileTypeGet(ui32FileIdx, &ui32FileType);	
	if (ui32FileType == xDCF_FILETYPE_JPG)
	{
		pbImageGet(0, ui32FileIdx, gPB, dispW, dispH, 0, 0x800000); /* 0x800000 is bg color */					
	}	
	
	return;
}

extern UINT32 dc_DisplayImage(UINT32 ui32FileIdx);

/**
**/
UINT32 PrintOneSet(void)
{
	UINT32 ret;
	UINT32 err;
	UINT32* msg;
	UINT32 key;
	UINT8 ui8SetFlag = 1;

	g_DateStamp = 0;
	qty = 0;	

	g_ui8PrintOneIdx = PRINT_SEL_FILE;
	capSelectState = 0;					/* A4 */
	G_Printing = 0;
	
	hwDispPvEn(0);
	DisplayImage(fileIdx);
	hwDispPvEn(1);	
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_paperSizes, &pCapPaperSizeArray, &capSize, 0);
	osdPrintOneSetUI(1);
	while (ui8SetFlag)
	{
		keyGet(&key);
		switch (key)	
		{
			case UI_KEY_DIR_LEFT:
				if (g_ui8PrintOneIdx == PRINT_SEL_FILE)
				{
					if (fileIdx > 1)
					{
						fileIdx--;
					}
					else 
					{
						fileIdx = fileCnt;						
					}
					DisplayImage(fileIdx);
				}
				else if (g_ui8PrintOneIdx == PRINT_SEL_QTY)
				{
					if (qty > 0)
					{
						qty--;
					}
					else 
					{
						qty = 99;
					}
				}
				else if (g_ui8PrintOneIdx == PRINT_SEL_PAPER)
				{
					if (capSelectState > 0)
					{
						capSelectState--;
					}
					else
					{
						capSelectState = 3;
					}					
				}									
				else if (g_ui8PrintOneIdx == PRINT_SEL_DATE)
				{
					if (g_DateStamp)
					{
						g_DateStamp = 0;
					}
					else
					{
						g_DateStamp = 1;						
					}
				}		
				osdPrintOneSetUI(0);								
				break;

			case UI_KEY_DIR_RIGHT:
				if (g_ui8PrintOneIdx == PRINT_SEL_FILE)
				{
					if (fileIdx < fileCnt)
					{
						fileIdx++;
					}
					else 
					{
						fileIdx = 1;						
					}
					DisplayImage(fileIdx);
				}
				else if (g_ui8PrintOneIdx == PRINT_SEL_QTY)
				{
					if (qty < 99)
					{
						qty++;
					}
					else 
					{
						qty = 0;
					}
				}
				else if (g_ui8PrintOneIdx == PRINT_SEL_PAPER)
				{	
					if (capSelectState < 3)
					{
						capSelectState++;
					}
					else
					{
						capSelectState = 0;
					}
				}					
				else if (g_ui8PrintOneIdx == PRINT_SEL_DATE)
				{
					if (g_DateStamp)
					{
						g_DateStamp = 0;
					}
					else
					{
						g_DateStamp = 1;						
					}
				}				
				osdPrintOneSetUI(0);												
				break;
				
			case UI_KEY_DIR_UP:
				if (g_ui8PrintOneIdx > 0)
				{
					g_ui8PrintOneIdx--;
				}
				else
				{
					g_ui8PrintOneIdx = C_PRINT_MENU_NUM-1;
				}				
				osdPrintOneSetUI(0);								
				break;

			case UI_KEY_DIR_DOWN:
				if (g_ui8PrintOneIdx < C_PRINT_MENU_NUM-1)
				{
					g_ui8PrintOneIdx++;
				}
				else
				{
					g_ui8PrintOneIdx = PRINT_SEL_PAPER;
				}				
				osdPrintOneSetUI(0);								
				break;

			case UI_KEY_FUNC_OK:
				if (g_ui8PrintOneIdx == PRINT_SEL_PRINT)
				{
	                if(qty)
	                {
					   osdBarDraw(PRINT_MENU_X, PRINT_MENU_Y, PRINT_MENU_W, PRINT_MENU_H, 0x02);						   
					   osdStrDisp((PANEL_1_OSD_W-10*11)/2, PANEL_1_OSD_H/2, UserFont10x20, 0x32, dc_GetString(Printing_s));						
	                   G_Printing = 1;
	                   PBPrintingOneInit();
	                   do
				       {
	                      msg = osQuePend(uiKeyMsgQ, 10, &err);
				       }while(G_PrintingNoExit == 1);
					   osdPrintOneSetUI(1);
    	            }
				}
				break;

			case UI_KEY_ACCIDENT:
				return 1;
				break;
				
			case UI_KEY_FUNC_MENU:				
				return 0;
				break;
		}
	}

	return 0;
}

/**
**/
UINT32 PrintAllSet(void)
{
	UINT32 key;
	UINT32 ret;
	UINT32 err;
	UINT32 originfileIdx;
	UINT32* msg;
	UINT8 ui8SetFlag = 1;
	
	G_Printing = 0;
	qty = 0;
	g_DateStamp = 0;
	g_ui8PrintOneIdx = PRINT_SEL_QTY;
	capSelectState = 0;					
	
	uiDispBuffFill(0x00);
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_paperSizes,&pCapPaperSizeArray, &capSize,0);	
	osdPrintAllSetUI(1);
	while (ui8SetFlag)
	{
		keyGet(&key);
		switch (key)	
		{
			case UI_KEY_DIR_LEFT:
				if (g_ui8PrintOneIdx == PRINT_SEL_QTY)
				{
					if (qty > 0)
					{
						qty--;
					}
					else 
					{
						qty = 99;
					}
				}
				else if (g_ui8PrintOneIdx == PRINT_SEL_PAPER)
				{
					if (capSelectState > 0)
					{
						capSelectState--;
					}
					else
					{
						capSelectState = 3;
					}					
				}									
				else if (g_ui8PrintOneIdx == PRINT_SEL_DATE)
				{
					if (g_DateStamp)
					{
						g_DateStamp = 0;
					}
					else
					{
						g_DateStamp = 1;						
					}
				}		
				osdPrintAllSetUI(0);								
				break;

			case UI_KEY_DIR_RIGHT:
				if (g_ui8PrintOneIdx == PRINT_SEL_QTY)
				{
					if (qty < 99)
					{
						qty++;
					}
					else 
					{
						qty = 0;
					}
				}
				else if (g_ui8PrintOneIdx == PRINT_SEL_PAPER)
				{	
					if (capSelectState < 3)
					{
						capSelectState++;
					}
					else
					{
						capSelectState = 0;
					}
				}					
				else if (g_ui8PrintOneIdx == PRINT_SEL_DATE)
				{
					if (g_DateStamp)
					{
						g_DateStamp = 0;
					}
					else
					{
						g_DateStamp = 1;						
					}
				}				
				osdPrintAllSetUI(0);												
				break;
				
			case UI_KEY_DIR_UP:
				if (g_ui8PrintOneIdx > 1)	/* From 1 */
				{
					g_ui8PrintOneIdx--;
				}
				else
				{
					g_ui8PrintOneIdx = C_PRINT_MENU_NUM-1;
				}				
				osdPrintAllSetUI(0);								
				break;

			case UI_KEY_DIR_DOWN:
				if (g_ui8PrintOneIdx < C_PRINT_MENU_NUM-1)
				{
					g_ui8PrintOneIdx++;
				}
				else
				{
					g_ui8PrintOneIdx = 1;	/* From 1 */
				}				
				osdPrintAllSetUI(0);								
				break;

			case UI_KEY_FUNC_OK:
				if (g_ui8PrintOneIdx == PRINT_SEL_PRINT)
				{
	                if(qty)
	                {
					   osdBarDraw(PRINT_MENU_X, PRINT_MENU_Y, PRINT_MENU_W, PRINT_MENU_H, 0x02);						   
					   osdStrDisp((PANEL_1_OSD_W-10*11)/2, PANEL_1_OSD_H/2, UserFont10x20, 0x32, dc_GetString(Printing_s));						
	                   G_Printing = 1;
                       originfileIdx = fileIdx;
                       PBPrintingAllInit();
                       fileIdx = originfileIdx;
	                   do
				       {
	                      msg = osQuePend(uiKeyMsgQ, 10, &err);
				       }while(G_PrintingNoExit == 1);
    	            }
					osdPrintAllSetUI(1);
				}
				break;

			case UI_KEY_ACCIDENT:
				return 1;
				break;
				
			case UI_KEY_FUNC_MENU:
				return 0;
				break;
		}
	}

	return 0;
}

/**
**/
UINT32 menuPrintProc(UINT32 key)
{
#if 0	
	UINT32 err;
	UINT32 key;
	UINT32* msg;
	
	/* Check Connect to printer */
	g_ui8InPrintMode = 1;		
	gpioConfig(GPIO_USB_OE, 1);
	gpioSet(GPIO_USB_OE, 0);	
	usbChangeMode(USB_MODE_SIDC);				
	gpioConfig(GPIO_USB_OE, 1);
	gpioSet(GPIO_USB_OE, 1);
	osTimeDly(100);
	usbChangeMode(USB_MODE_SIDC);			
	
//	gpioConfig(GPIO_SEN_STANDBY, 1);
//	gpioSet(GPIO_SEN_STANDBY, 1);
//	gpioConfig(GPIO_SENSOR_POWER, 1);
//	gpioSet(GPIO_SENSOR_POWER, 0);	

	osdClearScreen(0x00);
	uiDispBuffFill(0x00);
	osdStrDisp((PANEL_1_OSD_W-10*16)/2, (PANEL_1_OSD_H-20)/2, UserFont10x20, 0xd0, dc_GetString(ConnectPRT_s));
	while (isMenuOn)
	{		
		if (dpsPrinterConected)	
		{			
			printf("DPS OK\n");
			break;
		}

		msg = osQuePend(uiKeyMsgQ, 20, &err);
		if (err == OS_NO_ERR)
		{
			key = *msg;
		}
		else
		{
			continue;
		}
		
		switch(key)
		{
			case UI_KEY_ACCIDENT:
				isMenuOn = 0;
				uiUSBStatus = 0;	
				g_ui8InPrintMode = 0;
				gpioConfig(GPIO_USB_OE, 1);
	         	gpioSet(GPIO_USB_OE, 0);								
				osQuePost(uiKeyMsgQ, &keyButton[key]);	
				return 0;
				break;
				
			case UI_KEY_FUNC_MENU:
				isMenuOn = 0;
				g_ui8InPrintMode = 0;
				uiUSBStatus = 0;	
				gpioConfig(GPIO_USB_OE, 1);
	         	gpioSet(GPIO_USB_OE, 0);				
				return 0;
				break;
		}
	}	
	
	osdClearScreen(0x00);
	printf("Printer OK\n");
	/* Printer connect OK */
	if (ui8SubMenuIndex == 1)
	{		
		err = PrintOneSet();
	}
	else
	{
		err = PrintAllSet();
	}			
	
	if (err == 0)
	{
		DisplayMenuBack();
		MenuIconDisp(MENU_ICON_IMAGE_OFF, ui8MenuIndex, ui8TotalMenuNum);	
	}	
	g_ui8InPrintMode = 0;
	gpioConfig(GPIO_USB_OE, 1);
 	gpioSet(GPIO_USB_OE, 0);		
	uiUSBStatus = 0;
	
	return 0;
#endif	
}

#if 0
/**
**/
void dc_pbDPOFSetUI(UINT32 num, UINT8 initFlag)
{
	UINT32	number;
	UINT32	osdW, osdH;
	UINT32	x, y;
	osdW = PANEL_1_OSD_W;
	osdH = PANEL_1_OSD_H;
	UINT8 str[10];
	number = num;

	x = osdW/2-FOSDWidth*3;
	y = osdH/2-FOSDHeight;
	
	if(initFlag)
	{
		osdBarDraw(0, 100, PANEL_1_OSD_W, 24, 0x02);
		sio_psprintf(str, "%04d-%04d", fileIdx, fileCnt);
		osdStrDisp((PANEL_1_OSD_W-10*9)/2, 102, UserFont10x20, 0x32, str);
		sio_psprintf(str, "%1d", num);
		osdStrDisp(PANEL_1_OSD_W-10*2, 0, UserFont10x20, 0xd2, str);
		osdStrDisp(PANEL_1_OSD_W-10*4, 0, UserIcon10x20, 0xd2, "\x66\x67");
    }
    else
    {
	}
}

/**
**/
UINT32 dc_pbDPOFSet(UINT32 index)
{
	UINT8 setFlag = 1;
	UINT16 num = 0;
	UINT32 skey;
	UINT32 PrefileIdx = 0;	
	dpofStdParam_t dpf_t;

	osdClearScreen(0x00);
	dc_DisplayImage(fileIdx);
	osdBarDraw(0, 0, PANEL_1_OSD_W, 24, 0x02);
	osdStrDisp(10, 0, UserFont10x20, 0xd2, dc_GetString(DPOF_One_s));
	while(setFlag)
	{
	 	if(index)
	 	{
		     if(dpofJobQuery(DPOF_PRINT_MODE_STD) == 0)
		     {
				dpofJobAttrGet(DPOF_PRINT_MODE_STD, &dpf_t);
				printf("dpf_t.num = %d\n",dpf_t.numPrints);
				num = dpf_t.numPrints;	
		     }
		     else
		     {
		        if(fileIdx != PrefileIdx)
		        {
		        	num = 0;
		        }
	         }			 
	         PrefileIdx = fileIdx;
	         dc_pbDPOFSetUI(num, 1);
        }  
		keyGet(&skey);
		switch(skey)
		{
		        /*case	UI_KEY_DIR_DOWN:*/
			case UI_KEY_DIR_RIGHT:
                if(index)
                {
					if(fileIdx == fileCnt)
					{
						fileIdx = 1;
					}
					else
					{
						fileIdx++;
					}
					dc_DisplayImage(fileIdx);				
	       		}
	          	break;
				
			case UI_KEY_DIR_LEFT:
				if(index)
				{
					if(fileIdx == 1)
					{
						fileIdx = fileCnt;
					}
					else
					{
						fileIdx--;
					}
					dc_DisplayImage(fileIdx);					
				}
	          	break;
				
			case UI_KEY_FUNC_OK:
		        if(index)
		        {
		           num++;
		           if(num == 4)
		           {
			           num = 0;
		           }
				   dpofCurFileSet(&num);
				   dpofFileWrite();		
		           dc_pbDPOFSetUI(num, 1);
				}
		        break;					

		 case UI_KEY_ACCIDENT:
		 	 isMenuOn = 0;
			 printf("ACCENT of exit pdof\n");
			 osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);
			 return -1;
			 
		 case UI_KEY_FUNC_MENU:
		 	 osdClearScreen(0x00);
	         return 0;
	         break;
		}
	}	
}

void dpofAllJPGFileSet(UINT16 *numPrints)
{
   UINT32 PrefileIdx;
   UINT32 err = SUCCESS;
   
   PrefileIdx = fileIdx;
   
   if(fileCnt)
   {
     for(fileIdx=1; fileIdx<fileCnt+1; fileIdx++)
     {
        if (xdcfCurFileByPosSet(fileIdx)!= SUCCESS)
		{
			#ifdef	OSD_DEBUG
			printf(" #FILE_ERR ");
			#endif
			printf(" #FILE_ERR ");
			return;
		}
		err	= xdcfCurFileAttrGet(&fileattr);
		err	= xdcfFileTypeGet(fileIdx, &fileType);
        xdcfCurDirAttrGet(&dirattr);
        dpofCurFileSet(numPrints);		
     }
   }  
   dpofFileWrite();
   fileIdx = PrefileIdx;
}

UINT32 G_Start = 1;

#define DPOF_MENU_X		(64)
#define DPOF_MENU_Y		(64)

/**
**/
UINT32 menuDpofProc(UINT32 key)
{
	UINT8 SetFlag = 1;
	UINT16 num;
	UINT32 key;
	UINT32 index = 0;

	printf("menu DPOF\n");
	if ((sPara.StorageModeFlag == Card) && isCardProtect())
	{
		printf("Card Protect\n");
		return -1;
	}
	
	if (G_Start == 1)
	{
		dpofFileRead();
		G_Start = 0;
	}
	
	if (ui8SubMenuIndex == 1)
	{
		xdcfCurFileByPosSet(fileIdx);
		if (dc_pbDPOFSet(fileIdx) == 0)
		{
		//	DisplayMenuBack();
		//	MenuIconDisp(MENU_ICON_IMAGE_OFF, ui8MenuIndex, ui8TotalMenuNum);
		//	osdMenuPhotoInit[ui8MenuIndex](key);	//for bug 72 tzw add 060317
		}
	}
	else 
	{
		osdClearScreen(0x00);
		uiDispBuffFill(0x00);
		index = 0;
		osdStrDisp(10, 0, UserFont10x20, 0xd0, dc_GetString(DPOF_All_s));	
		osdBarDraw(DPOF_MENU_X, DPOF_MENU_Y, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, 0x0e);
		osdStrDisp(DPOF_MENU_X, DPOF_MENU_Y, UserFont10x20, 0x3e, dc_GetString(dc_OK_s));
		osdStrDisp(DPOF_MENU_X, DPOF_MENU_Y+24*1, UserFont10x20, 0xd0, dc_GetString(dc_Cancel_s));
		
		while (SetFlag)
		{
			keyGet(&key);
			switch (key)
			{
			case UI_KEY_ACCIDENT:
				isMenuOn = 0;
				return 1;
				break;
				
			case UI_KEY_FUNC_MENU:				
				SetFlag = 0;
				break;
				
			case UI_KEY_DIR_UP:
				osdBarDraw(DPOF_MENU_X, DPOF_MENU_Y+24*1, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, 0x00);
				osdBarDraw(DPOF_MENU_X, DPOF_MENU_Y, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, 0x0e);
				osdStrDisp(DPOF_MENU_X, DPOF_MENU_Y, UserFont10x20, 0x3e, dc_GetString(dc_OK_s));
				osdStrDisp(DPOF_MENU_X, DPOF_MENU_Y+24*1, UserFont10x20, 0xd0, dc_GetString(dc_Cancel_s));								
				index = 0;
				break;
				
			case UI_KEY_DIR_DOWN:
				osdBarDraw(DPOF_MENU_X, DPOF_MENU_Y, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, 0x00);
				osdBarDraw(DPOF_MENU_X, DPOF_MENU_Y+24*1, CAP_BAR_WIDTH, CAP_BAR_HEIGHT, 0x0e);
				osdStrDisp(DPOF_MENU_X, DPOF_MENU_Y, UserFont10x20, 0xd0, dc_GetString(dc_OK_s));
				osdStrDisp(DPOF_MENU_X, DPOF_MENU_Y+24*1, UserFont10x20, 0x3e, dc_GetString(dc_Cancel_s));				
				index = 1;
				break;
				
			case UI_KEY_FUNC_OK:				
				if (index == 0)
				{
				    num = 1;		
				}
				else
				{
					num = 0;
				}
			    dpofAllJPGFileSet(&num);			
				SetFlag = 0;
				break;
			}
		}					
	//	DisplayMenuBack();
	//	MenuIconDisp(MENU_ICON_IMAGE_OFF, ui8MenuIndex, ui8TotalMenuNum);						
	//	osdMenuPhotoInit[ui8MenuIndex](key);	//for bug 72 tzw add 060317
	}
	
	return 0;
}
#endif

extern void pbShowErrerFile();
extern UINT32 atoCounter ;
UINT32 menuAutoProc(UINT32 key)
{
	UINT32 err;
	UINT32 *msg;
	UINT32 dispW,dispH;
	UINT32  fileIdx1;
	UINT8   ui8AutoOpen_Flag = 1;  // for bug 69 tzw add 060316
	UINT8  ui8EndOfAccident = 0;
	UINT32 x, y;
	UINT8	ui8FirstIn = 1;
	
	hwDispPreviewSizeGet(&dispW, &dispH);
	hwDispImgSizeSet(dispW, dispH);
	hwPlayBufAddrSet(gPB);
	hwFrmCnvModeSet(0);   				
	hwCammodeSet(5);      				
	hwCdspDataSource(1);  

	if(fileCnt <= 1)
	{
		return 0;
	}	

	#if 1   // ��ʾ����  // tzw add for bug 5 060420
		x = (PANEL_1_OSD_W-10*F_STRCTRL(dc_GetString(dc_Slide_s)))/2;
		y = (PANEL_1_OSD_H-20)/2;
//		hwWait(0, 2000);
//		osdBarDraw(x, y, 100, 20, 0);	
			
	#endif 
	fileIdx1 = fileIdx;

/*
	if (fileIdx1 < fileCnt)
	{
		//fileIdx1++;   // for bug 69 tzw add 060316
	}
	else
	{
		fileIdx1 = 1;
	}
*/	
	
	osdClearScreen(0x00);
	
	while( fileIdx1 != fileIdx  ||ui8AutoOpen_Flag == 1)
	{	
		ui8AutoOpen_Flag = 0 ;  // for bug 69 tzw add 060316
		err = xdcfCurFileByPosSet(fileIdx1);
		atoCounter = 0 ;
		if ( err == SUCCESS)
		{
//			pbImageGet(0, fileIdx1, gPB, dispW, dispH, 0, 0x800000);  
			pbImageGet(0, fileIdx1, gPB, dispW, dispH, 0, 0x000000); // fqdao_modify  for bug  18    06.4.25

			if(ui8FirstIn) // tzw add for bug 5 060420
			{
				osdClearScreen(0x00);				
				osdStrDisp(x, y, UserFont10x20, 0x30, dc_GetString(dc_Slide_s));
				osTimeDly(200);
				osdBarDraw(x, y, 180, 200, 0);
				ui8FirstIn = 0;
			}

		}
		else
		{
			#if 1  // tzw add for bug 88 060217
			pbShowErrerFile();
			#endif
		}
		
		if (fileIdx1 < fileCnt)
		{
			fileIdx1 ++;
		}
		else
		{
			fileIdx1 = 1;
		}
			
		msg = osQuePend(uiKeyMsgQ, 3*100, &err);
		if( *msg == UI_KEY_FUNC_MENU)
		{
			ui8EndOfAccident = 1;			
			printf("Exit of menu key\n");
			fileIdx = fileIdx1;
			buttonAudio(1) ;  //fqdao_add for  bug  06.5.8
			break ;
		}
		else if(*msg == UI_KEY_ACCIDENT)
		{
			ui8EndOfAccident = 1;
			printf("Exit of accident\n");			
			fileIdx = fileIdx1;
			break;
		}
	}	
	
	if (ui8EndOfAccident == 0)
	{
		
		x = (PANEL_1_OSD_W-10*F_STRCTRL(dc_GetString(Slide_End_s)))/2;
		y = (PANEL_1_OSD_H-20)/2;
		osdStrDisp(x, y, UserFont10x20, 0x30, dc_GetString(Slide_End_s));
//		osTimeDly(100);
		hwWait(0, 1000);
	}

	isMenuOn = 0;		/* Exit Menu To Normal */	
	return 0;			/* Exit Menu */
}

UINT32 menuOpenProc(UINT32 key)
{
	UINT8*	pbuf;
	UINT32	dispW, dispH;
	UINT32	dispAddr;
	UINT32  page;	
	UINT32  stringlen;  	

	sysgMemDispAddrGet(&dispAddr);

	pbuf = (UINT8 *)((0xAC000000 | (UINT32)dispAddr*2));

	xdcfFileTypeGet(fileIdx, &fileType);
	if (fileType != xDCF_FILETYPE_JPG)
	{
		return 0;
	}
	
	isMenuOn = 0;	/* Exit menu to normal */

	hwDispPreviewSizeGet(&dispW,&dispH);
	
	pbImageGet(0, fileIdx, dispAddr, dispW, dispH, 0, 0x800000); //0x800000 is bg color - fqdao_modify 06.5.6 "1 for enlarge "

/* fqdao_add for set open picture menu start 06.5.30  */
	if (IsSetOpenProc()) 
	 {
		osdBarDraw(0, 100, 479, 60, 0x00); 
	 	return 0 ;
	 }

	sPara.uiPatternFlag = PatternCurrent;
//	sPara.uiPatternFlag = PatternCustom;
	nandWriteParameters();

	osdClearScreen(0x00);	
	stringlen = F_STRCTRL(dc_GetString(dc_SetOpen1_s)); 		
	osdStrDisp(160-stringlen*5, 100, UserFont10x20, 0x32, dc_GetString(dc_SetOpen1_s));
	stringlen = F_STRCTRL(dc_GetString(dc_SetOpen2_s)); 
	osdStrDisp(160-stringlen*5, 120, UserFont10x20, 0x32, dc_GetString(dc_SetOpen2_s));	

	if (dispW*dispH*2%512 == 0)
	{
		page = dispW*dispH*2/512;
	}
	else
	{
		page = (dispW*dispH*2/512)+1;
	}

	nandRsvBlkWrite(SECTOR_OF_START_PIC, page, pbuf);
	
	hwWait(0, 1000);		
	
	osdBarDraw(0, 100, 479, 60, 0x00);//for bug 20	
	
	
	return 0;
}

void menuPBImageInit(UINT32 key)
{
	isMenuOn = 1;
	ui8MenuIndex = 0;
	onMovieMenu = 0;

	ui8SubMenuIndex = 1;
	ui8TotalMenuNum = C_PBIMAGE_MENU_NUM;
	osdClearScreen(0x00);
	
	DisplayMenuBack();
	MenuIconDisp(MENU_ICON_IMAGE_OFF, ui8MenuIndex, ui8TotalMenuNum);
	osdMenuPhotoInit[ui8MenuIndex](key);	
}

void menuPBImageProc(UINT32 key)
{
	switch (key)
	{
		case UI_KEY_ACCIDENT:
		case UI_KEY_FUNC_MENU:
		case UI_KEY_FUNC_B:
			isMenuOn = 0;
			return;
			break;

		case UI_KEY_DIR_LEFT:
			if (ui8MenuIndex > 0)
			{
				ui8MenuIndex--;
			}
			else
			{
				ui8MenuIndex = ui8TotalMenuNum - 1;
			}
			#if 0
			// for bug 71 @03/17/2006
			if ((ui8MenuIndex == 4) && (cardExist != TRUE))
			{ 
				ui8MenuIndex--;
			}
			#endif   			
			MenuIconDisp(MENU_ICON_IMAGE_OFF, ui8MenuIndex, ui8TotalMenuNum);						
			ui8SubMenuIndex = 1;			
			break;

		case UI_KEY_DIR_RIGHT:
			if (ui8MenuIndex < (ui8TotalMenuNum - 1))
			{
				ui8MenuIndex++;
			}
			else
			{
				ui8MenuIndex = 0;
			}

			#if 0
			// for bug 71 @03/17/2006   
			if ((ui8MenuIndex == 4) && (cardExist != TRUE))
			{ 
				ui8MenuIndex++;
			}   
			#endif		
			MenuIconDisp(MENU_ICON_IMAGE_OFF, ui8MenuIndex, ui8TotalMenuNum);			
			ui8SubMenuIndex = 1;
			break;

		case  UI_KEY_DIR_UP:
			if (ui8SubMenuIndex > 1)
			{
				ui8SubMenuIndex--;
			}
			break;
		case  UI_KEY_DIR_DOWN:
			if (ui8SubMenuIndex < MenuT[ui8MenuIndex])
			{
				ui8SubMenuIndex++;		
			}
			break;

		case UI_KEY_FUNC_OK:
			flag = osdMenuPhotoProc[ui8MenuIndex](key);
			if (flag)
			{
				MessageShow (MenuSuccess); //John @02/07/2006
			}
			return;		
			break;			
	}	

	osdMenuPhotoInit[ui8MenuIndex](key);
}


void menuPBMovieInit(UINT32 key)
{
	isMenuOn = 1;
	ui8MenuIndex = 0;
	onMovieMenu = 1;

	ui8SubMenuIndex = 1;
	ui8TotalMenuNum = C_PBMOVIE_MENU_NUM;
	DisplayMenuBack();
	MenuIconDisp(MENU_ICON_MOVIE_OFF, ui8MenuIndex, ui8TotalMenuNum);
	osdMenuMovieInit[ui8MenuIndex](key);		
}

void menuPBMovieProc(UINT32 key)
{
	switch (key)
	{
		case UI_KEY_ACCIDENT:		
		case UI_KEY_FUNC_MENU:
		/* Paul@2006/12/27 - 13:08 add start */
		case UI_KEY_FUNC_B:
		/* Paul@2006/12/27 - 13:08 add end */
			isMenuOn = 0;
			onMovieMenu = 1;
			return;
			break;

		case UI_KEY_DIR_LEFT:
			if (ui8MenuIndex > 0)
			{
				ui8MenuIndex--;
			}
			else
			{
				ui8MenuIndex = ui8TotalMenuNum - 1;
			}
			MenuIconDisp(MENU_ICON_MOVIE_OFF, ui8MenuIndex, ui8TotalMenuNum);						
			ui8SubMenuIndex = 1;			
			break;

		case UI_KEY_DIR_RIGHT:
			if (ui8MenuIndex < (ui8TotalMenuNum - 1))
			{
				ui8MenuIndex++;
			}
			else
			{
				ui8MenuIndex = 0;
			}
			MenuIconDisp(MENU_ICON_MOVIE_OFF, ui8MenuIndex, ui8TotalMenuNum);			
			ui8SubMenuIndex = 1;
			break;

		case  UI_KEY_DIR_UP:
			if (ui8SubMenuIndex > 1)
			{
				ui8SubMenuIndex--;
			}
			break;
		case  UI_KEY_DIR_DOWN:
			if (ui8SubMenuIndex < MenuT[ui8MenuIndex])
			{
				ui8SubMenuIndex++;		
			}
			break;

		case UI_KEY_FUNC_OK:
			flag = osdMenuMovieProc[ui8MenuIndex](key);
			if (flag)
			{
				MessageShow (MenuSuccess);
			}
			return ;
			break;			
	}	
	osdMenuMovieInit[ui8MenuIndex](key);	
}


/*********************/
/* fqdao_add for set open picture menu start 06.5.30  */
UINT32 IsSetOpenProc()
{
	UINT32 key = UI_MODE_MASK;
	UINT32 ui32SelectIdx = 0;

      osdClearScreen(0 );
	osdBarDraw(OPEN_DLG_OFF_X, OPEN_DLG_OFF_Y,OPEN_DLG_WIDTH, OPEN_DLG_HEIGHT, COLOR_OPEN_DLG_BK);
	osdStrDisp(OPEN_DLG_OFF_X+10, OPEN_DLG_OFF_Y+20, UserFont10x20, 0xd2, dc_GetString(IsSetOpen_s));
	osdStrDisp(OPEN_DLG_OFF_X+40, OPEN_DLG_OFF_Y+60, UserFont10x20, ui32SelectIdx ? COLOR_OPEN_DLG_NSEL : COLOR_OPEN_DLG_SEL, dc_GetString(Open_yes_s));
	osdStrDisp(OPEN_DLG_OFF_X+140, OPEN_DLG_OFF_Y+60, UserFont10x20, ui32SelectIdx ? COLOR_OPEN_DLG_SEL : COLOR_OPEN_DLG_NSEL, dc_GetString(Open_no_s));

	while (uiState == ui32NextState)
	{
		switch (key)
		{
			case UI_KEY_DIR_LEFT:
			case UI_KEY_DIR_RIGHT:
				ui32SelectIdx = ui32SelectIdx ?  0 : 1;						
				osdStrDisp(OPEN_DLG_OFF_X+40, OPEN_DLG_OFF_Y+60, UserFont10x20, ui32SelectIdx ? COLOR_OPEN_DLG_NSEL : COLOR_OPEN_DLG_SEL, dc_GetString(Open_yes_s));
				osdStrDisp(OPEN_DLG_OFF_X+140, OPEN_DLG_OFF_Y+60, UserFont10x20, ui32SelectIdx ? COLOR_OPEN_DLG_SEL : COLOR_OPEN_DLG_NSEL, dc_GetString(Open_no_s));
				break;

			case UI_KEY_FUNC_OK:				
				return ui32SelectIdx;

			case UI_KEY_ACCIDENT:
				return 1;
				
			default: 
				break;				
		}

		keyGet(&key);
	}	

	return 1;
}
/* fqdao_add for set open picture menu End 06.5.30  */
/*********************/	
