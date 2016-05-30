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
 *  Author:                                                               *
 *                                                                        *
 **************************************************************************/

/* hw_cdsp_custom :    hwCdspWbGainSet(0x155, 0x40, 0x153, 0x40);*/

//#define RS_232_DBG
#include "general.h"
#include "hw_front.h"
#include "hw_cdsp.h"
#include "hw_cpu.h"
#include "stdio.h"
#include "device.h"
#include "os_api.h"
#include "aaa_api.h"
#include "hw_gpio.h"
#include "timer.h"		/* sun@1021 pm 14:55*/
#include "..\hwinclude\hw_tmr.h"		/* sun@1021 pm 14:55*/
#include "gpio.h"	/* sun@1022 am 10:05*/
#include "Ui_main.h " /* sun@1101 pm 16:51 */
#define DV_MODE 0


#define MAXHSIZEM      1024
#if DV_MODE
#define MAXVSIZEM      510
#else
#define MAXVSIZEM      768
#endif	

#define SENSOR_WIDTH   2048
#define SENSOR_HEIGHT  1536

#define EXT_HSIZE 0
#define EXT_VSIZE 0

UINT32  ESP_LINE;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 aaaAeEntryFlag;
extern UINT8 flashmode;	/* sun@1021 pm 15:06 */
UINT8	uiOnBusy = 0;
void FlashTriggerPulse(void);  	/* sun@1021 pm 15:11 */

/* sun@1029 am 11:46 add */
	UINT8 G_OB_previewRval=0;
	UINT8 G_OB_previewGRval=0;
	UINT8 G_OB_previewGBval=0;
	UINT8 G_OB_previewBval=0;
	UINT8 G_OB_snapRval=0;
	UINT8 G_OB_snapGRval=0;
	UINT8 G_OB_snapGBval=0;
	UINT8 G_OB_snapBval=0;

extern SINT32 PreTidx, Tidx;	
extern UINT32 G_AvgLumi;
extern UINT32 Ev7_Index,Ev8_Index,Ev9_Index,Ev10_Index,Ev11_Index,Ev12_Index,Max_Tidx,Min_Tidx,Ini_Tidx;
extern UINT32 Flag3620;
bool  bUSBFocusFlag = false;

/* sun@1029 am 11:47 end */
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if 0	/* sun@1101 pm 16: 27 mask */
AEstr LVtbl60[121] = {
	{50,100,0x3c},{51,100,0x3a},{52,120,0x3d},{53,109,0x39},
	{54,109,0x37},{55,100,0x34},{56,109,0x34},{57,109,0x33},
	{58,100,0x30},{59,100,0x1e},{60,100,0x1c},{61,100,0x1a},
	{62,120,0x1d},{63,109,0x19},{64,109,0x17},{65,120,0x18},
	{66,120,0x16},{67,133,0x17},{68,149,0x18},{69,200,0x1e},
	{70,200,0x1c},{71,200,0x1a},{72,200,0x18},{73,200,0x17},
	{74,200,0x15},{75,200,0x14},{76,200,0x12},{77,200,0x11},
	{78,200,0x10},{79,200,0x0e},{80,200,0x0c},{81,200,0x0a},
	{82,200,0x08},{83,200,0x07},{84,240,0x09},{85,240,0x08},
	{86,240,0x06},{87,240,0x05},{88,240,0x03},{89,240,0x02},
	{90,299,0x05},{91,299,0x03},{92,299,0x02},{93,299,0x01},
	{94,399,0x05},{95,399,0x04},{96,600,0x0c},{97,600,0x0a},
	{98,399,0x00},{99,600,0x07},{100,600,0x05},{101,600,0x04},
	{102,1201,0x12},{103,1201,0x11},{104,1201,0x10},{105,1201,0x0e},
	{106,1201,0x0c},{107,1201,0x0a},{108,1201,0x08},{109,1211,0x07},
	{110,1201,0x05},{111,1211,0x04},{112,1211,0x03},{113,1211,0x02},
	{114,1201,0x00},{115,1370,0x01},{116,1370,0x00},{117,1488,0x00},
	{118,1795,0x02},{119,1929,0x02},{120,2170,0x03},{121,2083,0x01},
	{122,2367,0x02},{123,2264,0x00},{124,3472,0x07},{125,3720,0x07},
	{126,3472,0x04},{127,3720,0x04},{128,4006,0x04},{129,4734,0x06},
	{130,4340,0x03},{131,5208,0x05},{132,4734,0x02},{133,4734,0x01},
	{134,5787,0x03},{135,7440,0x07},{136,8680,0x09},{137,10416,0x0c},
	{138,10416,0x0a},{139,8680,0x04},{140,8680,0x03},{141,10416,0x05},
	{142,10416,0x04},{143,13020,0x07},{144,17361,0x0d},{145,10416,0x00},
	{146,17361,0x09},{147,17361,0x07},{148,17361,0x06},{149,17361,0x04},
	{150,17361,0x03},{151,17361,0x02},{152,26041,0x09},{153,26041,0x07},
	{154,26041,0x06},{155,26041,0x04},{156,26041,0x03},{157,26041,0x02},
	{158,26041,0x00},{159,52083,0x0f},{160,52083,0x0d},{161,52083,0x0b},
	{162,52083,0x09},{163,52083,0x07},{164,52083,0x06},{165,52083,0x04},
	{166,52083,0x03},{167,52083,0x02},{168,52083,0x00},{169,78125,0x07},
	{170,78125,0x05}
	};

AEstr LVtbl50[121] = {
	{50,100,0x3c},{51,100,0x3a},{52,111,0x3b},{53,111,0x39},
	{54,100,0x35},{55,111,0x36},{56,125,0x37},{57,111,0x33},
	{58,142,0x37},{59,166,0x39},{60,100,0x1c},{61,111,0x1d},
	{62,111,0x1b},{63,166,0x33},{64,142,0x1e},{65,111,0x16},
	{66,125,0x17},{67,111,0x13},{68,166,0x1b},{69,200,0x1e},
	{70,200,0x1c},{71,200,0x1a},{72,200,0x18},{73,200,0x17},
	{74,200,0x15},{75,200,0x14},{76,200,0x12},{77,200,0x11},
	{78,200,0x10},{79,200,0x0e},{80,200,0x0c},{81,200,0x0a},
	{82,200,0x08},{83,250,0x0c},{84,250,0x0a},{85,250,0x08},
	{86,250,0x07},{87,250,0x06},{88,250,0x04},{89,333,0x09},
	{90,333,0x07},{91,333,0x06},{92,499,0x0e},{93,333,0x03},
	{94,333,0x02},{95,499,0x09},{96,499,0x07},{97,499,0x06},
	{98,499,0x04},{99,500,0x00},{100,499,0x01},{101,499,0x00},
	{102,1001,0x0e},{103,1001,0x0c},{104,1001,0x0b},{105,1001,0x09},
	{106,1001,0x07},{107,1021,0x06},{108,1001,0x04},{109,1001,0x03},
	{110,1021,0x02},{111,1041,0x01},{112,1041,0x00},{113,1132,0x00},
	{114,1211,0x00},{115,1370,0x01},{116,1736,0x04},{117,1578,0x01},
	{118,1795,0x02},{119,1929,0x02},{120,2170,0x03},{121,2083,0x01},
	{122,2367,0x02},{123,2264,0x00},{124,3472,0x07},{125,3720,0x07},
	{126,3472,0x04},{127,3720,0x04},{128,4006,0x04},{129,4734,0x06},
	{130,4340,0x03},{131,5208,0x05},{132,4734,0x02},{133,4734,0x01},
	{134,5787,0x03},{135,7440,0x07},{136,8680,0x09},{137,10416,0x0c},
	{138,10416,0x0a},{139,8680,0x04},{140,8680,0x03},{141,10416,0x05},
	{142,10416,0x04},{143,13020,0x07},{144,17361,0x0d},{145,10416,0x00},
	{146,17361,0x09},{147,17361,0x07},{148,17361,0x06},{149,17361,0x04},
	{150,17361,0x03},{151,17361,0x02},{152,26041,0x09},{153,26041,0x07},
	{154,26041,0x06},{155,26041,0x04},{156,26041,0x03},{157,26041,0x02},
	{158,26041,0x00},{159,52083,0x0f},{160,52083,0x0d},{161,52083,0x0b},
	{162,52083,0x09},{163,52083,0x07},{164,52083,0x06},{165,52083,0x04},
	{166,52083,0x03},{167,52083,0x02},{168,52083,0x00},{169,78125,0x07},
	{170,78125,0x05}
	};
#else
AEstr LVtbl60C[128] ={
{50 , 0x050D, 0x0E  }, 
{51 , 0x050D, 0x0D  }, 
{52 , 0x050D, 0x0C  }, 
{53 , 0x050D, 0x0B  }, 
{54 , 0x050D, 0x0A  }, 
{55 , 0x050D, 0x09  }, 
{56 , 0x050D, 0x08  }, 
{57 , 0x050D, 0x07  }, 
{58 , 0x050D, 0x06  }, 
{59 , 0x050D, 0x05  }, 
{60 , 0x050D, 0x04  }, 
{61 , 0x0460, 0x07  }, 
{62 , 0x0460, 0x06  }, 
{63 , 0x0460, 0x06  }, 
{64 , 0x0460, 0x05  }, 
{65 , 0x0460, 0x04  }, 
{66 , 0x040A, 0x05  }, 
{67 , 0x040A, 0x04  }, 
{68 , 0x040A, 0x04  }, 
{69 , 0x03B4, 0x05  }, 
{70 , 0x03B4, 0x04  }, 
{71 , 0x03B4, 0x04  }, 
{72 , 0x035E, 0x06  }, 
{73 , 0x035E, 0x05  }, 
{74 , 0x035E, 0x04  }, 
{75 , 0x035E, 0x04  }, 
{76 , 0x0307, 0x05  }, 
{77 , 0x0307, 0x04  }, 
{78 , 0x0307, 0x04  }, 
{79 , 0x02B1, 0x06  }, 
{80 , 0x02B1, 0x05  }, 
{81 , 0x02B1, 0x04  }, 
{82 , 0x02B1, 0x04  }, 
{83 , 0x0205, 0x0C  }, 
{84 , 0x0205, 0x0B  }, 
{85 , 0x0205, 0x0A  }, 
{86 , 0x0205, 0x09  }, 
{87 , 0x0205, 0x08  }, 
{88 , 0x0205, 0x07  }, 
{89 , 0x0205, 0x06  }, 
{90 , 0x0205, 0x05  }, 
{91 , 0x0205, 0x04  }, 
{92 , 0x0205, 0x09  }, 
{93 , 0x0205, 0x08  }, 
{94 , 0x0205, 0x07  }, 
{95 , 0x0205, 0x06  }, 
{96 , 0x0205, 0x05  }, 
{97 , 0x0205, 0x04  }, 
{98 , 0x0158, 0x0A  }, 
{99 , 0x0158, 0x09  }, 
{100, 0x0158, 0x08  }, 
{101, 0x0158, 0x07  }, 
{102, 0x0158, 0x06  }, 
{103, 0x0158, 0x05  }, 
{104, 0x0158, 0x04  }, 
{105, 0x0102, 0x0E  }, 
{106, 0x0102, 0x0D  }, 
{107, 0x0102, 0x0C  }, 
{108, 0x0102, 0x0B  }, 
{109, 0x0102, 0x0A  }, 
{110, 0x0102, 0x09  }, 
{111, 0x0102, 0x08  }, 
{112, 0x0102, 0x07  }, 
{113, 0x0102, 0x06  }, 
{114, 0x0102, 0x05  }, 
{115, 0x0102, 0x04  }, 
{116, 0x00AC, 0x0D  }, 
{117, 0x00AC, 0x0C  }, 
{118, 0x00AC, 0x0B  }, 
{119, 0x00AC, 0x0A  }, 
{120, 0x00AC, 0x09  }, 
{121, 0x00AC, 0x08  }, 
{122, 0x00AC, 0x07  }, 
{123, 0x00AC, 0x06  }, 
{124, 0x00AC, 0x05  }, 
{125, 0x0056, 0x0F  }, 
{126, 0x0056, 0x0E  }, 
{127, 0x0056, 0x0D  }, 
{128, 0x0056, 0x0C  }, 
{129, 0x0056, 0x0B  }, 
{130, 0x0056, 0x0A  }, 
{131, 0x0056, 0x09  }, 
{132, 0x0056, 0x08  }, 
{133, 0x0056,  0x07 }, 
{134, 0x0056,  0x06 }, 
{135, 0x0056,  0x05 }, 
{136, 0x0056,  0x04 }, 
{137, 0x0054,  0x04 }, 
{138, 0x0052,  0x04 }, 
{139, 0x0050,  0x04 }, 
{140, 0x004A,  0x04 }, 
{141, 0x0043,  0x04 }, 
{142, 0x003E,  0x04 }, 
{143, 0x0039,  0x04 }, 
{144, 0x0034,  0x04 }, 
{145, 0x0030,  0x04 }, 
{146, 0x002C,  0x04 }, 
{147, 0x0028,  0x04 }, 
{148, 0x0025,  0x04 }, 
{149, 0x0021,  0x04 }, 
{150, 0x001F,  0x04 }, 
{151, 0x001C,  0x04 }, 
{152, 0x001A,  0x04 }, 
{153, 0x0018,  0x04 }, 
{154, 0x0016,  0x04 }, 
{155, 0x0014,  0x04 }, 
{156, 0x0012,  0x04 }, 
{157, 0x0010,  0x04 }, 
{158, 0x000F,  0x04 }, 
{159, 0x000E,  0x04 }, 
{160, 0x000D,  0x04 }, 
{161, 0x000C,  0x04 }, 
{162, 0x000B,  0x04 }, 
{163, 0x000A,  0x04 }, 
{164, 0x0009,  0x04 }, 
{165, 0x0008,  0x04 }, 
{166, 0x0007,  0x04 }, 
{167, 0x0006,  0x04 }, 
{168, 0x0006,  0x04 }, 
{169, 0x0005,  0x04 }, 
{170, 0x0004,  0x04 }, 
{171, 0x0003,  0x04 }, 
{172, 0x0003,  0x04 }, 
{173, 0x0002,  0x04 }, 
{174, 0x0002,  0x04 }, 
{175, 0x0002,  0x04 }, 
{176, 0x0002,  0x04 }, 
{177, 0x0002,  0x04 }  

};
AEstr LVtbl60[128] ={
 {50 , 0x0186, 0x3F},
 {51 , 0x0186, 0x3E},
 {52 , 0x0186, 0x3D},
 {53 , 0x0186, 0x3B},
 {54 , 0x0186, 0x3A},
 {55 , 0x0186, 0x39},
 {56 , 0x0186, 0x38},
 {57 , 0x0186, 0x37},
 {58 , 0x0186, 0x36},
 {59 , 0x0186, 0x35},
 {60 , 0x0186, 0x34},
 {61 , 0x0186, 0x33},
 {62 , 0x0186, 0x32},
 {63 , 0x0186, 0x31},
 {64 , 0x0186, 0x30},
 {65 , 0x0104, 0x3D},
 {66 , 0x0104, 0x3C},
 {67 , 0x0104, 0x3B},
 {68 , 0x0104, 0x3A},
 {69 , 0x0104, 0x39},
 {70 , 0x0104, 0x38},
 {71 , 0x0104, 0x37},
 {72 , 0x0104, 0x36},
 {73 , 0x0104, 0x35},
 {74 , 0x0186, 0x1F},
 {75 , 0x0186, 0x1E},
 {76 , 0x0186, 0x1D},
 {77 , 0x0186, 0x1C},
 {78 , 0x0186, 0x1B},
 {79 , 0x0186, 0x1A},
 {80 , 0x0186, 0x19},
 {81 , 0x0186, 0x18},
 {82 , 0x0186, 0x17},
 {83 , 0x0186, 0x16},
 {84 , 0x0186, 0x15},
 {85 , 0x0186, 0x14},
 {86 , 0x0186,0x13},
 {87 , 0x0186,0x12},
 {88 , 0x0186,0x11},
 {89 , 0x0186,0x10},
 {90 , 0x0104,0x1D},
 {91 , 0x0104,0x1C},
 {92 , 0x0104,0x1B},
 {93 , 0x0104,0x1A},
 {94 , 0x0104,0x19},
 {95 , 0x0104,0x18},
 {96 , 0x0104,0x17},
 {97 , 0x0104,0x16},
 {98 , 0x0186,0x0F},
 {99 , 0x0186,0x0E},
 {100, 0x0186,0x0D},
 {101, 0x0186,0x0C},
 {102, 0x0186,0x0B},
 {103, 0x0186,0x0A},
 {104, 0x0186,0x09},
 {105, 0x0186,0x08},
 {106, 0x0104,0x12},
 {107, 0x0104,0x11},
 {108, 0x0104,0x10},
 {109, 0x0104,0x0F},
 {110, 0x0104,0x0E},
 {111, 0x0104,0x0D},
 {112, 0x0104,0x0C},
 {113, 0x0104,0x0B},
 {114, 0x0104,0x0A},
 {115, 0x0104,0x09},
 {116, 0x0104,0x08},
 {117, 0x0082,0x18},
 {118, 0x0082,0x17},
 {119, 0x0082,0x16},
 {120, 0x0082,0x15},
 {121, 0x0082,0x14},
 {122, 0x0082,0x13},
 {123, 0x0082,0x12},
 {124, 0x0082,0x11},
 {125, 0x0082,0x10},
 {126, 0x0082,0x0F},
 {127, 0x0082,0x0E},
 {128, 0x0082,0x0D},
 {129, 0x0082,0x0C},
 {130, 0x0082,0x0B},
 {131, 0x0082,0x0A},
 {132, 0x0082,0x09},
 {133, 0x0082,0x08},
 {134, 0x007F,0x08},
 {135, 0x007A,0x08},
 {136, 0x006E,0x08},
 {137, 0x0066,0x08},
 {138, 0x005E,0x08},
 {139, 0x0057,0x08},
 {140, 0x004A,0x08},
 {141, 0x0048,0x08},
 {142, 0x0045,0x08},
 {143, 0x003F,0x08},
 {144, 0x0037,0x08},
 {145, 0x0033,0x08},
 {146, 0x002F,0x08},
 {147, 0x002B,0x08},
 {148, 0x0028,0x08},
 {149, 0x0024,0x08},
 {150, 0x0020,0x08},
 {151, 0x001B,0x08},
 {152, 0x0019,0x08},
 {153, 0x0018,0x08},
 {154, 0x0017,0x08},
 {155, 0x0016,0x08},
 {156, 0x0015,0x08},
 {157, 0x0014,0x08},
 {158, 0x0013,0x08},
 {159, 0x0012,0x08},
 {160, 0x0011,0x08},
 {161, 0x0010,0x08},
 {162, 0x000F,0x08},
 {163, 0x000E,0x08},
 {164, 0x000D,0x08},
 {165, 0x000C,0x08},
 {166, 0x000B,0x08},
 {167, 0x000A,0x08},
 {168, 0x0009,0x08},
 {169, 0x0008,0x08},
 {170, 0x0007,0x08},
 {171, 0x0006,0x08},
 {172, 0x0006,0x06},
 {173, 0x0006,0x04},
 {174, 0x0006,0x02},
 {175, 0x0006,0x01},
 {176, 0x0006,0x00},
 {177, 0x0006,0x00},
};                
             
AEstr LVtbl50[128] = {
{50 ,  0x01D4, 0x3F},	
{51 ,  0x01D4, 0x3E },
{52 ,  0x01D4, 0x3D },
{53 ,  0x01D4, 0x3B },
{54 ,  0x01D4, 0x3A },
{55 ,  0x01D4, 0x39 },
{56 ,  0x01D4, 0x38 },
{57 ,  0x01D4, 0x37 },
{58 ,  0x01D4, 0x36 },
{59 ,  0x01D4, 0x35 },
{60 ,  0x01D4, 0x34 },
{61 ,  0x01D4, 0x33 },
{62 ,  0x01D4, 0x32 },
{63 ,  0x01D4, 0x31 },
{64 ,  0x01D0, 0x30 },
{65 ,  0x0138, 0x3D },
{66 ,  0x0138, 0x3C },
{67 ,  0x0138, 0x3B },
{68 ,  0x0138, 0x3A },
{69 ,  0x0138, 0x39 },
{70 ,  0x0138, 0x38 },
{71 ,  0x0138, 0x37 },
{72 ,  0x0138, 0x36 },
{73 ,  0x0138, 0x35 },
{74 ,  0x01D4, 0x1F },
{75 ,  0x01D4, 0x1E },
{76 ,  0x01D4, 0x1D },
{77 ,  0x01D4, 0x1C },
{78 ,  0x01D4, 0x1B },
{79 ,  0x01D4, 0x1A },
{80 ,  0x01D4, 0x19 },
{81 ,  0x01D4, 0x18 },
{82 ,  0x01D4, 0x17 },
{83 ,  0x01D4, 0x16 },
{84 ,  0x01D4, 0x15 },
{85 ,  0x01D4, 0x14 },
{86 ,  0x01D4, 0x13 },
{87 ,  0x01D4, 0x12 },
{88 ,  0x01D4, 0x11 },
{89 ,  0x01D0, 0x10 },
{90 ,  0x0138, 0x1D },
{91 ,  0x0138, 0x1C },
{92 ,  0x0138, 0x1B },
{93 ,  0x0138, 0x1A },
{94 ,  0x0138, 0x19 },
{95 ,  0x0138, 0x18 },
{96 ,  0x0138, 0x17 },
{97 ,  0x0138, 0x16 },
{98 ,  0x01D4, 0x0F },
{99 ,  0x01D4, 0x0E },
{100,  0x01D4, 0x0D },
{101,  0x01D4, 0x0C },
{102,  0x01D4, 0x0B },
{103,  0x01D4, 0x0A },
{104,  0x01D4, 0x09 },
{105,  0x01D4, 0x08 },
{106,  0x0138, 0x12 },
{107,  0x0138, 0x11 },
{108,  0x0138, 0x10 },
{109,  0x0138, 0x0F },
{110,  0x0138, 0x0E },
{111,  0x0138, 0x0D },
{112,  0x0138, 0x0C },
{113,  0x0138, 0x0B },
{114,  0x0138, 0x0A },
{115,  0x0138, 0x09 },
{116,  0x0138, 0x08 },
{117,  0x009C, 0x18 },
{118,  0x009C, 0x17 },
{119,  0x009C, 0x16 },
{120,  0x009C, 0x15 },
{121,  0x009C, 0x14 },
{122,  0x009C, 0x13 },
{123,  0x009C, 0x12 },
{124,  0x009C, 0x11 },
{125,  0x009C, 0x10 },
{126,  0x009C, 0x0F },
{127,  0x009C, 0x0E },
{128,  0x009C, 0x0D },
{129,  0x009C, 0x0C },
{130,  0x009C, 0x0B },
{131,  0x009C, 0x0A },
{132,  0x009C, 0x09 },
{133,  0x009C, 0x08 },
{134,  0x0099, 0x08 },
{135,  0x0095, 0x08 },
{136,  0x0082, 0x08 },
{137,  0x007F, 0x08 },
{138,  0x006E, 0x08 },
{139,  0x0066, 0x08 },
{140,  0x005E, 0x08 },
{141,  0x0057, 0x08 },
{142,  0x0048, 0x08 },
{143,  0x004A, 0x08 },
{144,  0x0045, 0x08 },
{145,  0x003F, 0x08 },
{146,  0x0037, 0x08 },
{147,  0x0033, 0x08 },
{148,  0x002F, 0x08 },
{149,  0x002A, 0x08 },
{150,  0x0026, 0x08 },
{151,  0x0024, 0x08 },
{152,  0x0021, 0x08 },
{153,  0x0020, 0x08 },
{154,  0x001D, 0x08 },
{155,  0x001B, 0x08 },
{156,  0x0019, 0x08 },
{157,  0x0017, 0x08 },
{158,  0x0015, 0x08 },
{159,  0x0013, 0x08 },
{160,  0x0011, 0x08 },
{161,  0x0010, 0x08 },
{162,  0x000F, 0x08 },
{163,  0x000E, 0x08 },
{164,  0x000D, 0x08 },
{165,  0x000C, 0x08 },
{166,  0x000B, 0x08 },
{167,  0x000A, 0x08 },
{168,  0x0009, 0x08 },
{169,  0x0008, 0x08 },
{170,  0x0008, 0x07 },
{171,  0x0008, 0x06 },
{172,  0x0008, 0x05 },
{173,  0x0008, 0x04 },
{174,  0x0008, 0x03 },
{175,  0x0008, 0x02 },
{176,  0x0008, 0x01 },
{177,  0x0008, 0x00 } 
};
AEstr LVtbl50C[128] = {
{ 50 ,        0x05A8,	   0x0E     },                   
{ 51 ,        0x05A8,     0x0E    },                       
{ 52 ,        0x05A8,     0x0D    },                     
{ 53 ,        0x05A8,     0x0C    },                       
{ 54 ,        0x05A8,     0x0B    },                       
{ 55 ,        0x05A8,     0x0A    },                     
{ 56 ,        0x05A8,     0x09    },                       
{ 57 ,        0x05A8,     0x08   },                      
{ 58 ,        0x05A8,     0x07   },                      
{ 59 ,        0x05A8,     0x06   },                      
{ 60 ,        0x05A8,     0x05   },                      
{ 61 ,        0x05A8,     0x04   },                      
{ 62 ,        0x0540,     0x05    },                     
{ 63 ,        0x0540,     0x04    },                     
{ 64 ,        0x04D9,     0x05    },                     
{ 65 ,        0x04D9,     0x04    },                     
{ 66 ,        0x0471,     0x05    },                     
{ 67 ,        0x0471,     0x04    },                     
{ 68 ,        0x0471,     0x04    },                     
{ 69 ,        0x03A2,     0x07    },                     
{ 70 ,        0x03A2,     0x06    },                     
{ 71 ,        0x03A2,     0x06    },                     
{ 72 ,        0x03A2,     0x05    },                     
{ 73 ,        0x03A2,     0x04    },                     
{ 74 ,        0x03A2,     0x04    },                     
{ 75 ,        0x033B,     0x06    },                     
{ 76 ,        0x033B,     0x05    },                     
{ 77 ,        0x033B,     0x04    },                    
{ 78 ,        0x033B,     0x04    },                     
{ 79 ,        0x02D4,     0x06    },                     
{ 80 ,        0x02D4,     0x05    },                     
{ 81 ,        0x02D4,     0x05    },                     
{ 82 ,        0x02D4,     0x04    },                     
{ 83 ,        0x0205,     0x0C    },                     
{ 84 ,        0x0205,     0x0B    },                     
{ 85 ,        0x0205,     0x0A  },                         
{ 86 ,        0x0205,     0x09  },                         
{ 87 ,        0x0205,     0x08  },                         
{ 88 ,        0x0205,     0x07  },                         
{ 89 ,        0x0205,     0x06  },                         
{ 90 ,        0x0205,     0x05  },                         
{ 91 ,        0x0205,     0x04  },                         
{ 92 ,        0x019D,     0x09  },                         
{ 93 ,        0x019D,     0x08  },                         
{ 94 ,        0x019D,     0x07  },                         
{ 95 ,        0x019D,     0x06  },                         
{ 96 ,        0x019D,     0x05  },                         
{ 97 ,        0x019D,     0x04  },                         
{ 98 ,        0x0136,     0x0A  },                         
{ 99 ,        0x0136,     0x09  },                         
{ 100,        0x0136,     0x08  },                         
{ 101,        0x0136,     0x07  },                         
{ 102,        0x0136,     0x06  },                         
{ 103,        0x0136,     0x05  },                         
{ 104,        0x0136,     0x04   },                      
{ 105,        0x00CE,     0x0E   },                      
{ 106,        0x00CE,     0x0D   },                      
{ 107,        0x00CE,     0x0C   },                      
{ 108,        0x00CE,     0x0B   },                      
{ 109,        0x00CE,     0x0A   },                      
{ 110,        0x00CE,     0x09   },                      
{ 111,        0x00CE,     0x08   },                      
{ 112,        0x00CE,     0x07   },                      
{ 113,        0x00CE,     0x06   },                      
{ 114,        0x00CE,     0x05   },                      
{ 115,        0x00CE,     0x04   },                      
{ 116,        0x00CE,     0x04   },                      
{ 117,        0x00CE,     0x04   },                      
{ 118,        0x00CE,     0x04   },                      
{ 119,        0x00CE,     0x04   },                      
{ 120,        0x00CE,     0x04   },                      
{ 121,        0x00CE,     0x04   },                      
{ 122,        0x00CE,     0x04   },                      
{ 123,        0x00CE,     0x04   },                      
{ 124,        0x00CE,     0x04   },                      
{ 125,        0x0067,     0x0F   },                      
{ 126,        0x0067,     0x0E   },                      
{ 127,        0x0067,     0x0D   },                      
{ 128,        0x0067,     0x0C   },                      
{ 129,        0x0067,     0x0B   },                      
{ 130,        0x0067,     0x0A   },                      
{ 131,        0x0067,     0x09   },                      
{ 132,        0x0067,     0x08   },                      
{ 133,        0x0067,     0x07   },                      
{ 134,        0x0067,     0x06   },                      
{ 135,        0x0067,     0x05   },                      
{ 136,        0x0067,     0x04   },                      
{ 137,        0x0067,     0x04   },                      
{ 138,        0x0060,     0x04   },                      
{ 139,        0x0058,     0x04   },                      
{ 140,        0x0050,     0x04   },                      
{ 141,        0x004A,     0x04   },                      
{ 142,        0x0043,     0x04   },                      
{ 143,        0x003E,     0x04   },                      
{ 144,        0x0039,     0x04   },                      
{ 145,        0x0034,     0x04   },                      
{ 146,        0x0030,     0x04   },                      
{ 147,        0x002C,     0x04   },                      
{ 148,        0x0028,     0x04   },                      
{ 149,        0x0025,     0x04   },                      
{ 150,        0x0021,     0x04   },                      
{ 151,        0x001F,     0x04   },                      
{ 152,        0x001C,     0x04   },                      
{ 153,        0x001A,     0x04   },                      
{ 154,        0x0018,     0x04   },                      
{ 155,        0x0016,     0x04   },                      
{ 156,        0x0014,     0x04   },                      
{ 157,        0x0012,     0x04   },                      
{ 158,        0x0010,     0x04 },                        
{ 159,        0x000F,     0x04 },                        
{ 160,        0x000E,     0x04 },                        
{ 161,        0x000D,     0x04 },                        
{ 162,        0x000C,     0x04 },                        
{ 163,        0x000B,     0x04 },                        
{ 164,        0x000A,     0x04 },                        
{ 165,        0x0009,     0x04 },                        
{ 166,        0x0008,     0x04 },                        
{ 167,        0x0007,     0x04 },                        
{ 168,        0x0006,     0x04 },                        
{ 169,        0x0006,     0x04 },                        
{ 170,        0x0005,     0x04 },                        
{ 171,        0x0004,     0x04 },                        
{ 172,        0x0003,     0x04 },                        
{ 173,        0x0003,     0x04 },                        
{ 174,        0x0002,     0x04 },                        
{ 175,        0x0002,     0x04 },                        
{ 176,        0x0002,     0x04 },                        
{177,         0x0002,     0x04 }                                                                               
};
#endif


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontInit                                                        *
*                                                                                     *
*  Purposes      :                                                                    *
*    To Initialize Front Setting                                                      *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Initialize sensor setting about front                                            *
*                                                                                     *
*  Arguments     :                                                                    *
*    cammode : in, the camera operation mode                                          *
*                  0: idle mode                                                       *
*                  1: preview mode                                                    *
*                  2: still camera mode                                               *
*                  3: video clip mode                                                 *
*                  4: PC-camera mode                                                  *
*                  5: playback mode                                                   *
*                                                                                     *
*  Returns       :                                                                    *
*    Error message                                                                    *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32 
hwFrontInit_ov3620(
	UINT32 cammode
)
{
	UINT32  regaddr[1], regdata[1];
	UINT32 i;
	frontSenReg_t *pfrontSenReg;
	frontSifReg_t *pfrontSifReg;

	if (cammode != 0) {
		return SUCCESS;
	}	

	pfrontSenReg = (frontSenReg_t *)(0xb0000000);  
	pfrontSifReg = (frontSifReg_t *)(0xb0000000); 

/*	dprint("FRONT enter hwFrontInit OV3620 \n");      */

	hwFrontSiEnable(1, 0);			/* Paul@2005/11/03-08:11 add */
	hwFrontGpioSel(0x3FFFFFFF, 0xF50F);	/* Select GPIO or sensor sun@1022 am 9:19 */
	hwFrontGpioCfg(0x00000000, 0xC0000000, 0x8200, 0x8AF0); /* Clk2x output enable sun@1022 am 9:19 */
	/*====================================================================*/
	/*Jinch@2004/9/30 For TV656+OV2610*/
//	WRITE8(0xb0000010, 0x04);		//glbReg->pixelClkSel
	
	hwTgGpioCfgSet(0x00,0x00,0x7400,0x7400);
	hwTgGpioMsbMaskWrite(0x2000,0x7400);
	/*====================================================================*/
	
	hwFrontInputGate(0x0000);

	hwFrontClkCfg(1, 0, 1);
	hwFrontClkSet(0, 4, 1);                   /* Set master clock & pixsel clock */
	hwFrontClkPhaseCfg(0, 0, 0x03, 0);

	pfrontSenReg->senRst = 0x01;             /* Reset Sensor interface */
	pfrontSenReg->senRst = 0x00;       
	pfrontSenReg->vdUpdate = 0x01;           /* Enable Vdupdate */     

	pfrontSenReg->hOffset = 0x01f7;          /* Set Hoffset */

	pfrontSenReg->lineTotal  = 0x0600;       /* Set Linetotal, for frame rate control */	

 /*	pfrontSenReg->hSize  = 0x400;            Set hsize */
	if((uiState == UI_MODE_USB)&&(bUSBFocusFlag==true))
		pfrontSenReg->hSize  = 668;           /* Set hsize */
	else
		pfrontSenReg->hSize  = 1024;           /* Set hsize */
	
#if 1

	if(uiState != UI_MODE_PREVIEW)
	{
		pfrontSenReg->vOffset = 0x000b;          /* Set Voffset */
		pfrontSenReg->vSize  = 0x01fe;           /* Set Vsize */
		pfrontSenReg->frameTotal = 0x0208;       /* Set Frametotal */
	}
	else
	{
		pfrontSenReg->vOffset = 0x000b;          /* Set Voffset */
		pfrontSenReg->vSize  = 0x0300;           /* Set Vsize */
		pfrontSenReg->frameTotal = 0x030d;       /* Set Frametotal */
	}
#else
#if DV_MODE	
	pfrontSenReg->vOffset = 0x000b;          /* Set Voffset */
	pfrontSenReg->vSize  = 0x01fe;           /* Set Vsize */
	pfrontSenReg->frameTotal = 0x0208;       /* Set Frametotal */
#else
	pfrontSenReg->vOffset = 0x000b;          /* Set Voffset */
	pfrontSenReg->vSize  = 0x0300;           /* Set Vsize */
	pfrontSenReg->frameTotal = 0x030d;       /* Set Frametotal */
#endif	
#endif 
	/*====================================================================*/
	/*Jinch@2004/9/30 For TV656+OV2610*/
	pfrontSenReg->tvCtr = 0;
	pfrontSenReg->yuvS128en = 0;
	pfrontSenReg->capInterval = 0;
	/*====================================================================*/

	hwFrontDummyWr(0, 0x0600);               /* Save linetotal to dummy reg*/
	hwFrontDummyWr(2, 0x0400);               /* Save hsize to dummy reg*/
	hwFrontDummyWr(3, 0x01f7);               /* Save hoffset to dummy reg*/
	hwFrontDummyWr(4, 0x01f7);               /* Save hoffset default to dummy reg*/
#if 1
	if(uiState != UI_MODE_PREVIEW)
	{
		hwFrontDummyWr(1, 0x0208);               /* Save frametotal to dummy reg*/
		hwFrontDummyWr(5, 0x01fe);               /* Save vsize to dummy reg*/
		hwFrontDummyWr(6, 0x000b);               /* Save voffset to dummy reg*/
		hwFrontDummyWr(7, 0x000b);               /* Save voffset default to dummy reg*/ 
	}
	else
	{	
		hwFrontDummyWr(1, 0x030d);               /* Save frametotal to dummy reg*/
		hwFrontDummyWr(5, 0x0300);               /* Save vsize to dummy reg*/
		hwFrontDummyWr(6, 0x000b);               /* Save voffset to dummy reg*/
		hwFrontDummyWr(7, 0x000b);               /* Save voffset default to dummy reg*/ 
	}

#else
#if DV_MODE	
	hwFrontDummyWr(1, 0x0208);               /* Save frametotal to dummy reg*/
	hwFrontDummyWr(5, 0x01fe);               /* Save vsize to dummy reg*/
	hwFrontDummyWr(6, 0x000b);               /* Save voffset to dummy reg*/
	hwFrontDummyWr(7, 0x000b);               /* Save voffset default to dummy reg*/ 
#else	
	hwFrontDummyWr(1, 0x030d);               /* Save frametotal to dummy reg*/
	hwFrontDummyWr(5, 0x0300);               /* Save vsize to dummy reg*/
	hwFrontDummyWr(6, 0x000b);               /* Save voffset to dummy reg*/
	hwFrontDummyWr(7, 0x000b);               /* Save voffset default to dummy reg*/ 
#endif
#endif
	/* Use SSC to initializes OV3610 registers */
	pfrontSifReg->slaveAddr = 0x60;          /* Set sensor slave address */     
	pfrontSifReg->frequency = 0x03;          /* Set SSC Frequency */    
	regaddr[0] = 0x12; 
	regdata[0] = 0x80;                       /* Rst Ov2610 */
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	for (i=0;i<512;i++) {
	};
#if 1
	if(uiState != UI_MODE_PREVIEW)
	{
		regaddr[0] = 0x12; 
		regdata[0] = 0x20;                       /* Set DV Mode */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
	}
	else
	{
		regaddr[0] = 0x12; 
		regdata[0] = 0x40;                       /* Set XGA Mode */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
	}	
#else
#if DV_MODE
	regaddr[0] = 0x12; 
	regdata[0] = 0x20;                       /* Set DV Mode */
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
#else
	regaddr[0] = 0x12; 
	regdata[0] = 0x40;                       /* Set XGA Mode */
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
#endif	
#endif
	for (i=0;i<512;i++) {
	};

		regaddr[0] = 0x01; 
		regdata[0] = 0x7f;						 /*lca	Blue, Red bow bits Gain */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); /* Normal mode */

		regaddr[0] = 0x02; 
		regdata[0] = 0x7f;						 /*lca	Blue, Red bow bits Gain */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); /* Normal mode */

        regaddr[0] = 0x03; 
		regdata[0] = 0x44;						 /*lca	Blue, Red bow bits Gain */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); /* Normal mode */
		
		regaddr[0]	= 0x17; 			
		regdata[0]	= 0x16; 
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	/*lca  Extend h window */
	
		regaddr[0] = 0x19; 
		regdata[0] = 0x01;	  
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);		
	
		regaddr[0] = 0x1a; 
		regdata[0] = 0xe0;	  
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);		

		regaddr[0]	= 0x13; 				
		regdata[0]	= 0x80;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x09; 				
		regdata[0]	= 0x08;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
	    regaddr[0]	= 0x0c; 				
	    regdata[0]	= 0x08;
	    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x0d; 				
		regdata[0]	= 0x81;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
#if 1	
		regaddr[0] = 0x0e; 
		regdata[0] = 0x00;/*0x70-Zoran set;*/ 					  /* Set COMF, May change when real chip */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); /* lcaNormal mode */  
#endif	
		regaddr[0]	= 0x0f; 				
		regdata[0]	= 0x42;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); /*lca */

		regaddr[0]	= 0x20; 				
		regdata[0]	= 0x00;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); /*lca */

		regaddr[0]	= 0x23; 				
		regdata[0]	= 0x00;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); /*lca */

		regaddr[0] = 0x11; 
		regdata[0] = 0x80;	  
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
		regaddr[0] = 0x12; 
		regdata[0] = 0x20;	  
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
		
		regaddr[0] = 0x14; 
		regdata[0] = 0xc6;					  /* Set COMJ */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); /* Normal mode */
#if 0
		regaddr[0] = 0x15; 
		regdata[0] = 0x02;	/*0x03-Zoran set;*/ 	 
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
#endif
		regaddr[0]	= 0x33; 				
		regdata[0]	= 0x09;/*0x0c;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);/*lca */
	
		regaddr[0]	= 0x34; 				
		regdata[0]	= 0x50;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);/*lca */
		
		regaddr[0]	= 0x36; 				
		regdata[0]	= 0x00;/*0x07;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x37; 				
		regdata[0]	= 0x04;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x38; 				
		regdata[0]	= 0x52;/*0x72;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x3a; 				
		regdata[0]	= 0x00;/*0x00;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x3c; 				
		regdata[0]	= 0x1f;/*0x01;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
		
		regaddr[0]	= 0x44; 				
		regdata[0]	= 0x00;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);

		regaddr[0]	= 0x3f; 				
		regdata[0]	= 0x0f;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	/*chq add OB calibration*/
		regaddr[0]	= 0x40; 				
		regdata[0]	= 0x00;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
		
		regaddr[0]	= 0x41; 				
		regdata[0]	= 0x00;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
		
		regaddr[0]	= 0x42; 				
		regdata[0]	= 0x00;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
		
		regaddr[0]	= 0x43; 				
		regdata[0]	= 0x00;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
		
		regaddr[0]	= 0x45; 				
		regdata[0]	= 0x80;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
		
		
		regaddr[0]	= 0x48; 				
		regdata[0]	= 0xc0;/*0xc0;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
	
		regaddr[0]	= 0x49; 				
		regdata[0]	= 0x89;/*0x7f;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x4b; 				
		regdata[0]	= 0x80;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x4d; 				
		regdata[0]	= 0xc4;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
        regaddr[0] = 0x35; 
	    regdata[0] = 0x4c;       
	    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x3d; 				
		regdata[0]	= 0x00;/*0x00;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x3e; 				
		regdata[0]	= 0x00;/*0x00;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x13; 				
		regdata[0]	= 0xc0;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
	regaddr[0]	= 0x00; 				
	regdata[0]	= 0x00;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0]	= 0x01; 				
	regdata[0]	= 0x80;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0]	= 0x02; 				
	regdata[0]	= 0x80;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);


	regaddr[0] = 0x10; 
	regdata[0] = 0x61;       
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);

	regaddr[0] = 0x3b; 
	regdata[0] = 0x18;       
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);

       regaddr[0] = 0x33; 
	regdata[0] = 0x19;
      hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);

       regaddr[0] = 0x34; 
	regdata[0] = 0x58;       
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);

	regaddr[0] = 0x27; 
	regdata[0] = 0x80;       
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0] = 0x28; 
	regdata[0] = 0x80;       
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0] = 0x29; 
	regdata[0] = 0x80;       
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0] = 0x2c; 
	regdata[0] = 0x80;       
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0] = 0x1b; 
	regdata[0] = 0x01;       
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	

	
	aaaAeawbStatusSet(1);  /*Force AEAWB not ready: 0 -> not ready, 1 -> ready*/

	if(uiState != UI_MODE_PREVIEW)
	{
	aaaMaxFrameRateSet(30);
	hwFrontExpTimeSet(300, 0);
	hwFrontFrameRateSet(30, 0);	/*was 30 */
	}
	else
	{
	aaaMaxFrameRateSet(20);
	hwFrontExpTimeSet(300, 0);
	hwFrontFrameRateSet(20, 0); /**/
	}	
	hwFrontGainSet(0,0);
	
	hwFrontProbeSet(1, 2);     
	
	aaaAeEntryFlag = 0;
	    
	/* hwFrontFlashSet(9000, 10, 3);        
	dprint("FRONT exit hwFrontInit OV3610 \n");*/  

	return SUCCESS;
}


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontSnap                                                        *
*                                                                                     *
*  Purposes      :                                                                    *
*    To snap raw data to dram                                                         *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Snap raw data from sensor to dram                                                *
*                                                                                     *
*  Arguments     :                                                                    *
*    snapnum : in, The number of images to capture                                    *
*                                                                                     *
*  Returns       :                                                                    *
*    None                                                                             *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
void
hwFrontSnap_ov3620(
	UINT32 snapnum,
	void *prawAddrFunc,
	void *pbadPixFunc
)
{
	UINT32 badstatus, tmp0;
	frontSenReg_t *pfrontSenReg;
	cdspReg0_t *pcdspReg0;
	cdspReg1_t *pcdspReg1;
	ccdReg0_t *pccdReg0;
	ccdReg1_t *pccdReg1;
	cdspWinReg_t *pcdspWinReg;
	snapCallback praFunc = 0; /* cont shot customize */
	snapCallback pbpFunc = 0; /* cont shot customize */
	UINT32 idx = 1; /* cont shot customize */
	UINT32  rgain, grgain, bgain, gbgain, ggain, hscaleen;	
	UINT32 temp1;	/* sun@1021 pm 15:08 */
		
	praFunc = prawAddrFunc; /* cont shot customize */
	pbpFunc = pbadPixFunc; /* cont shot customize */
		
	pfrontSenReg = (frontSenReg_t *)(0xb0000000);
	pcdspReg0 = (cdspReg0_t *)(0xb0000000);  
	pcdspReg1 = (cdspReg1_t *)(0xb0000000);
	pccdReg0 = (ccdReg0_t *)(0xb0000000); 
	pccdReg1 = (ccdReg1_t *)(0xb0000000);   
	pcdspWinReg = (cdspWinReg_t *)(0xb0000000);   

	tmp0 = pfrontSenReg->syncGenen;
	pfrontSenReg->syncGenen = tmp0 | 0x02;/* Set Set totalsvden */  
	pfrontSenReg->sizeSvden = 0x03;      /* Set Size & Offset to sync with VD */
	badstatus = pcdspReg0->badpixen;     /* Read Bad pixel enable or not */
	pcdspReg0->gainSvden = 0x11;         /* Disable sub OB &n offset */

	rgain   = pcdspReg0->rGain;           /* Save R gain */
	grgain  = pcdspReg0->grGain;          /* Save Gr gain */
	bgain   = pcdspReg0->bGain;           /* Save B gain */
	gbgain  = pcdspReg0->gbGain;          /* Save Gb gain */
	ggain   = pcdspReg0->globalGain;      /* Save global gain */
	hscaleen = pcdspReg0->hScaleCtr;
	
	pcdspReg1->rawMode    = 0x02;         /* Set to rawmode 1, pass through gain & hscale */
	pcdspReg0->rGain      = 0x40;         /* Set unit gain */
	pcdspReg0->grGain     = 0x40;
	pcdspReg0->bGain      = 0x40;
	pcdspReg0->gbGain     = 0x40;
	pcdspReg0->globalGain = 0x20;
	

	tmp0 = (snapnum & 0x0F) | 0x10;

	hwFrontFullSet(0);
	pcdspReg0->badpixen = 0x00;            /* Disable Bad pixel median filter */
	


	/* hwFrontGpioSet(0, 0xFF); */
	hwFrontGpioRead(5,&temp1);		/* sun@1021 pm15:09 */ 
	pccdReg0->snapCtr = tmp0;             /* Set snap trigger */ 
	gpioConfig(GPIO_CHG_TRIGE,1);	/* sun@1022 am 10:21 */

	/* Paul@2005/11/24-08:27 add */	
	/* sun@1021 start pm15:09 */

	if (flashmode == FlashForceOn)	/* sun@060119 */
	{
		hwWait(0,146);
		hwFrontFlashSet(9000, 0, 0x00);		
	}
	else if ((Tidx == Min_Tidx)  && (flashmode<FlashForceOff) && (G_AvgLumi<OV3620_FLASH_LUM))
	{
			hwWait(0,146);
			hwFrontFlashSet(9000, 0, 0x00);	
	}
		/* Paul@2005/11/24-08:27 end */	
	
	for (idx = 1; idx <= snapnum; idx++) {
		if (praFunc)
			(*praFunc)(idx);
		hwFrontVdWait(0, 1);
		pcdspReg0->hScaleCtr  = 0x00;     /* Disable h scale */
		hwFrontVValidWait(0, 1);
		
		if (pbpFunc)
			(*pbpFunc)(idx);
	}

	/* hwFrontGpioSet(0, 0x00); */   
	hwFrontGpioSet(5,temp1 & 0x7F); /*asaki add strobe trigger end*/

	pcdspReg0->badpixen  = badstatus;    /* Save back Bad pixel enable or not */
	PlayEffectSound(0);
	uiOnBusy = 1;	
	/*pcdspReg0->gainSvden = 0x11;*/
	hwCdspDataSource(1);
	
	pcdspReg1->rawMode    = 0x00;         /* Set to rawmode 1, pass through gain & hscale */
	pcdspReg0->rGain      = rgain;         /* Set unit gain */
	pcdspReg0->grGain     = grgain;
	pcdspReg0->bGain      = bgain;
	pcdspReg0->gbGain     = gbgain;
	pcdspReg0->globalGain = ggain;
	pcdspReg0->hScaleCtr = hscaleen;
	hwFrontPreviewSet();                 /* Turn back to preview mode */ 
	hwFrontVdWait(0, 1);                 /* Wait a VD to out the function */  
	
}

/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontPreviewSet                                                  *
*                                                                                     *
*  Purposes      :                                                                    *
*    To preview mode                                                                  *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Set parameters to preview mode                                                   *
*                                                                                     *
*  Arguments     : none                                                               *
*                                                                                     *
*  Returns       :                                                                    *
*    None                                                                             *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
void
hwFrontPreviewSet_ov3620(
	void
)
{
	UINT32 tmp0;
	UINT32 tmp2, tmp3;
	frontSenReg_t *pfrontSenReg;
	UINT32  regaddr[1], regdata[1];
	  
	pfrontSenReg = (frontSenReg_t *)(0xb0000000);
	pfrontSenReg->sizeSvden = 0x03;          /* Offset & Size sync VD */ 

	tmp0 = pfrontSenReg->syncGenen;
	pfrontSenReg->syncGenen = tmp0 | 0x02;   /* Set Set totalsvden */  
	hwFrontDummyRd(2, &tmp2);                /* Read back preview Hoffset, Hsize */
	hwFrontDummyRd(3, &tmp3);
	pfrontSenReg->hOffset = tmp3;            /* Set Sensor Hoffset */
	pfrontSenReg->hSize   = tmp2;            /* Set Sensor Hsize */    

/*	dprint2("Hoffset = %x, Hsize = %x \n", tmp3, tmp2); */

	hwFrontDummyRd(5, &tmp2);                /* Read back preview Voffset, Vsize */
	hwFrontDummyRd(6, &tmp3);
	pfrontSenReg->vOffset = tmp3;            /* Set Sensor Voffset */
	pfrontSenReg->vSize   = tmp2;            /* Set Sensor Vsize */  

/*	dprint2("Voffset = %x, Vsize = %x \n", tmp3, tmp2);*/

	hwFrontDummyRd(1, &tmp2);                /* Read back frametotal */
	pfrontSenReg->frameTotal = tmp2;         /* Write frametotal */

	hwFrontDummyRd(0, &tmp2);                /* Read back linetotal */
	pfrontSenReg->lineTotal = tmp2;          /* Write linetotal */ 

	pfrontSenReg->sizeSvden = 0x00;  

	if(uiState != UI_MODE_PREVIEW)
	{
	regaddr[0] = 0x12; 
	regdata[0] = 0x20;    
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	}
	else
	{
	regaddr[0] = 0x12; 
	regdata[0] = 0x40;    
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	}
	regaddr[0] = 0x03; 
	regdata[0] = 0x44;    
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0] = 0x17; 
	regdata[0] = 0x16;    
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0] = 0x19; 
	regdata[0] = 0x01;    
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
	
	regaddr[0]	= 0x3f; 				
	regdata[0]	= 0x0f;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	/*chq add OB calibration*/

	regaddr[0]	= 0x40; 				
	regdata[0]	= G_OB_previewBval;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0]	= 0x41; 				
	regdata[0]	= G_OB_previewRval;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
	regaddr[0]	= 0x42; 				
	regdata[0]	= G_OB_previewGBval;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	regaddr[0]	= 0x43; 				
	regdata[0]	= G_OB_previewGRval;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
	pfrontSenReg->sizeSvden = 0x03;
	Flag3620=0;
	/* sun@1029 am 11:48 end */
 }


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontFullSet                                                     *
*                                                                                     *
*  Purposes      :                                                                    *
*    To full frame mode                                                               *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Set parameters to full frame mode                                                *
*                                                                                     *
*  Arguments     :                                                                    *
*    field : in, the field number of sensor                                           *
*                0 -> field 0 (CMOS sensor has field 0 only)                          *
*                1 -> field 1 for CCD                                                 *
*                2 -> field 2 for CCD                                                 *
*                3 -> field 3 for CCD                                                 *
*                                                                                     *
*  Returns       :                                                                    *
*    None                                                                             *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
void 
hwFrontFullSet_ov3620(
	UINT32 field
)
{
	frontSenReg_t *pfrontSenReg;

	if (field == 0) {
		UINT32 regaddr[1], regdata[1];
		UINT32 i;
		pfrontSenReg = (frontSenReg_t *)(0xb0000000);        
		regaddr[0] = 0x12; 
		regdata[0] = 0x00; 
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);  

		hwFrontVdWait(0, 3);  
		pfrontSenReg->hSize = 0x0800;            /* Set Sensor Hsize */
		pfrontSenReg->hOffset = 0x00f9;          /* Set Sensor Hoffset */
		pfrontSenReg->vSize = 0x0600;            /* Set Sensor Vsize */
		pfrontSenReg->vOffset = 0x5;            /* Set Sensor Voffset */      
                     
		regaddr[0] = 0x03; 
		regdata[0] = 0x44; 
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);  
		regaddr[0] = 0x17; 
		regdata[0] = 0x0e;/*0x0a; */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);  
		regaddr[0] = 0x1a; 
		regdata[0] = 0xc4;/*0x0a; */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);  

	/* sun@1129 am 11:51 add */
		regaddr[0] = 0x1b; 
		regdata[0] = 0x01;/*0x0a; */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);  
		
		regaddr[0]	= 0x40; 				
		regdata[0]	= G_OB_snapBval;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
		regaddr[0]	= 0x41; 				
		regdata[0]	= G_OB_snapRval;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
		regaddr[0]	= 0x42; 				
		regdata[0]	= G_OB_snapGBval;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
		regaddr[0]	= 0x43; 				
		regdata[0]	= G_OB_snapGRval;
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	/* sun@1129 am 11:51 end */	
	
		for (i = 0; i < 20000; i++);               /* Just for a delay */             
	}
}


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontGainSet                                                     *
*                                                                                     *
*  Purposes      :                                                                    *
*    To control Sensor gain                                                           *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Adjust gain value for sensor                                                     *
*                                                                                     *
*  Arguments     :                                                                    *
*    value : in, gain value to set                                                    *
*    option : in, 0 -> immediate change value                                         *
*                 1 -> synchronize the value to the next Vsync                        *
*                                                                                     *
*  Returns       :                                                                    *
*    Error message                                                                    *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32
hwFrontGainSet_ov3620(
    UINT32 value,
    UINT32  option 
)
{
	UINT32 tmp0;
	UINT32 regaddr[1], regdata[1];

	tmp0 = option & 0x01;                 /* Sync VD ? */
	regaddr[0] = 0x00; 
	regdata[0] = value;
	hwFrontSscWrite(regaddr, regdata, 0x01, tmp0); 
/*	dprint1("Gain = %x\n",value);*/
	
	return SUCCESS;
}


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontExpTimeSet                                                  *
*                                                                                     *
*  Purposes      :                                                                    *
*    To control exposure time                                                         *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Adjust exposure time value                                                       *
*                                                                                     *
*  Arguments     :                                                                    *
*    value : in, expossure time value to set, unit 0.1ms                              *
*    option : in, 0 -> immediate change value                                         *
*                 1 -> synchronize the value to the next Vsync                        *
*                                                                                     *
*  Returns       :                                                                    *
*    Error message                                                                    *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32 
hwFrontExpTimeSet_ov3620(
	UINT32 value,
	UINT32 option
)
{
	UINT32 ltotal, ftotal, espline, tmp1;
	UINT32 pixclk, tmp0;
	frontSifReg_t *pfrontSifReg;    
	frontSenReg_t *pfrontSenReg;
	ccdReg0_t *pccdReg0;
	UINT32  regaddr[1], regdata[1];

	pfrontSifReg = (frontSifReg_t *)(0xb0000000);   
	pfrontSenReg = (frontSenReg_t *)(0xb0000000); 
	pccdReg0 = (ccdReg0_t *)(0xb0000000);

	ltotal = pfrontSenReg->lineTotal;                        /* Read line total */
	ftotal = pfrontSenReg->frameTotal;                       /* Read frame total */    
	tmp1   = pfrontSifReg->pixClkHi;                         /* Read pixel clock */
	tmp0   = (tmp1 << 16); 
	tmp1   = pfrontSifReg->pixClkLow;                        /* Read pixel clock */   
	pixclk = tmp0 | tmp1;                         
	espline = (pixclk * 10) / (ltotal * value);

/*	if (espline > ftotal*2) {
		return FAIL;
	}  */
	
#if 1
	if(uiState == UI_MODE_PREVIEW)
	{
		if((espline>779)&&(Flag3620!=1))
			espline=778;
	}
		/*tmp1 = ftotal - 520;*/
	else
	{
		if((espline>519)&&(Flag3620!=1))
			espline=518;
		/*tmp1 = ftotal - 781;*/
	}
    ESP_LINE = espline;
    espline = value;
#endif    
#if 0
      printf("*************\n");
       printf("espline = %d \n", espline);
	printf("ftotal = %d \n", ftotal);
	printf("shut=%d\n",value);
       printf("linetotal=%d\n",ltotal);
	    printf("*************\n");
#endif
	tmp1 = (option & 0x01); 
	regaddr[0] = 0x04;
	regdata[0] = ( (espline & 0x07));
	hwFrontSscWrite(regaddr, regdata, 0x01, 0);

	regaddr[0] = 0x10;
	regdata[0] = espline >> 3;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0);
#if 0
	regaddr[0] = 0x45;/*chq add espline High byte*/
	regdata[0] = (espline >> 11) & 0x3f;
	hwFrontSscWrite(regaddr, regdata, 0x01, 0);
#endif
Flag3620=0;
	return SUCCESS;
}

/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontExpTimeGainSet                                              *
*                                                                                     *
*  Purposes      :                                                                    *
*    To control exposure time and gain in burst write                                 *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Adjust exposure time value and gain value at the same time                       *
*                                                                                     *
*  Arguments     :                                                                    *
*    value : in, expossure time value to set, unit 0.1ms                              *
*    value1: in, gain value to set	      					      *
*    option : in, 0 -> immediate change value                                         *
*                 1 -> synchronize the value to the next Vsync                        *
*                                                                                     *
*  Returns       :                                                                    *
*    Error message                                                                    *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32 
hwFrontExpTimeGainSet_ov3620(
    UINT32 value,
    UINT32 value1,
    UINT32 option
)
{
    UINT32 ltotal, ftotal, espline, tmp1;
    UINT32 pixclk, tmp0;
    frontSifReg_t *pfrontSifReg;    
    frontSenReg_t *pfrontSenReg;
    ccdReg0_t *pccdReg0;
    UINT32  regaddr[3], regdata[3];

    pfrontSifReg = (frontSifReg_t *)(0xb0000000);   
    pfrontSenReg = (frontSenReg_t *)(0xb0000000); 
    pccdReg0 = (ccdReg0_t *)(0xb0000000);
    
    ltotal = pfrontSenReg->lineTotal;                        /* Read line total */
    ftotal = pfrontSenReg->frameTotal;                       /* Read frame total */    
    tmp1   = pfrontSifReg->pixClkHi;                         /* Read pixel clock */
    tmp0   = (tmp1 << 16); 
    tmp1   = pfrontSifReg->pixClkLow;                        /* Read pixel clock */   
    pixclk = tmp0 | tmp1;                         
    espline = (pixclk * 10) / (ltotal * value);
 
    dprint1("espline = %x \n", espline);
    dprint1("ftotal = %x \n", ftotal);
    
    if (espline > ftotal) {
        return FAIL;
    }  

#if 1    
	#if DV_MODE
	tmp1 = ftotal - 520;
	#else
	tmp1 = ftotal - 781;
	#endif
#endif 		
    espline = espline - tmp1;
    
    tmp1 = (option & 0x01) | 0x02; 
    regaddr[0] = 0x04;
    regdata[0] = espline & 0x07;
    regaddr[1] = 0x10;
    regdata[1] = espline >> 3;
    regaddr[2] = 0x00;
    regdata[2] = value1;
    hwFrontSscWrite(regaddr, regdata, 0x03, tmp1);

    return SUCCESS;
}

/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontFrameRateSet                                                *
*                                                                                     *
*  Purposes      :                                                                    *
*    To control Frame Rate                                                            *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Adjust Frame rate for sensor                                                     *
*                                                                                     *
*  Arguments     :                                                                    *
*    value : in, frame rate value to set                                              *
*    option : in, 0 -> immediate change value                                         *
*                 1 -> synchronize the value to the next Vsync                        *
*                                                                                     *
*  Returns       :                                                                    *
*    Error message                                                                    *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32
hwFrontFrameRateSet_ov3620(
    UINT32 value,
    UINT32 option
)
{
	UINT32 ltotal, ftotal, tmp1, newftotal;
	UINT32 pixclk, tmp0;
	UINT32  sync;
	SINT32 espline;
	frontSifReg_t *pfrontSifReg;    
	frontSenReg_t *pfrontSenReg;
	ccdReg0_t *pccdReg0;

	UINT32  regaddr[4], regdata[4];
	UINT32 videomode;

	pfrontSifReg = (frontSifReg_t *)(0xb0000000);   
	pfrontSenReg = (frontSenReg_t *)(0xb0000000); 
	pccdReg0 = (ccdReg0_t *)(0xb0000000);

	videomode = videoClipModeGet();
	
	ltotal = pfrontSenReg->lineTotal;                        /* Read line total */
	ftotal = pfrontSenReg->frameTotal;                       /* Read frame total */    
	tmp1   = pfrontSifReg->pixClkHi;                         /* Read pixel clock */
	tmp0   = (tmp1 << 16); 
	tmp1   = pfrontSifReg->pixClkLow;                        /* Read pixel clock */   
	pixclk = tmp0 | tmp1;                             
	sync = option & 0x01; 

/*	dprint("    FRONT enter hwfrontFramRateSet OV3610 \n");*/
    
	newftotal = pixclk / (value * ltotal);
	pfrontSenReg->frameTotal = newftotal;
	hwFrontDummyWr(1, newftotal);
#if 0
	espline = (ESP_LINE + ftotal) - newftotal;

	if ( espline < 1 ) {
		espline = 1;
	}
	dprint1("	   ESP_LINE = %d\n",ESP_LINE);
	dprint1("    pixclk = %d \n", pixclk);
	dprint1("    newftotal = %x \n", newftotal);
	dprint1("    ftotal = %x \n", ftotal);       
	dprint1("espline = %d\n",espline);
	ESP_LINE = espline;
#endif


	if(uiState != UI_MODE_PREVIEW)
		tmp0 = newftotal - 520;                   /* Line to extend */           
	else
	 	tmp0 = newftotal - 781;                   /* Line to extend */           

#if 0
        regaddr[0] = 0x04;
        regdata[0] = espline & 0x07;
        regaddr[1] = 0x10;
        regdata[1] = espline >> 3;  
        hwFrontSscWrite(regaddr, regdata, 2, 0x02);          
#endif        
        regaddr[0] = 0x2D;
        regdata[0] = tmp0;    
        regaddr[1] = 0x2E;
        regdata[1] = tmp0 >> 8;
        hwFrontSscWrite(regaddr, regdata, 2, sync | 0x02);                   

	
	
	
/*	dprint("    FRONT Exit hwfrontFramRateSet OV3610 \n");  */

	return SUCCESS;
}


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontSuspend                                                     *
*                                                                                     *
*  Purposes      :                                                                    *
*    Suspend Front                                                                    *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Suspend Front                                                                    *
*                                                                                     *
*  Arguments     :                                                                    *
*    None                                                                             *
*                                                                                     *
*  Returns       :                                                                    *
*    None                                                                             *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
void   
hwFrontSuspend_ov3620(
	void
)
{
	UINT32 regaddr[1], regdata[1];	

	hwFrontGpioSet(0, 0x00);
	hwFrontGpioSet(1, 0x00);
	hwFrontGpioSet(2, 0x00);
	hwFrontGpioSet(3, 0x00);
	hwFrontGpioSet(4, 0xE0);                  /* Only Sen & Sck, Sd = 1 */
	hwFrontGpioSet(5, 0x00);
	hwFrontGpioSel(0xFFFFFFFF, 0xFFFF);       /* Select GPIO */
	hwFrontInputGate(0x0000);

	hwFrontGpioCfg(0x3FFFFFFF, 0xFFFFFFFF,  0xF73F, 0xFFFF);      /* Only V1, V2, SCK, SDO input */                  
	regaddr[0] = 0x09; 
	regdata[0] = 0x10;                       /* Set sensor to sleep mode */
	hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);     
}


void hwFrontMaxMonitorSizeGet_ov3620(
	UINT32 *pWidth, 
	UINT32 *pHeight
)
{
	if((uiState == UI_MODE_USB)&&(bUSBFocusFlag==true))
	{
		*pWidth  = 668;
		*pHeight = 510;	
	}
	else if(uiState != UI_MODE_PREVIEW)
	{
		*pWidth= MAXHSIZEM;
		*pHeight = 510;
	/*	printf("MAXVSIZEM = 510\n");*/
	}
	else
	{
		*pWidth= MAXHSIZEM;
		*pHeight = 768;	
	/*	printf("MAXVSIZEM = 768\n");*/
	}
}

void hwFrontExtMonitorSizeGet(
	UINT32 *pWidth, 
	UINT32 *pHeight
)
{
	*pWidth = EXT_HSIZE;
	*pHeight = EXT_VSIZE;
}

void hwFrontSensorSizeGet_ov3620(
	UINT32 *pwidth, 
	UINT32 *pheight
)
{
	*pwidth = SENSOR_WIDTH;
	*pheight = SENSOR_HEIGHT;
}

void hwFrontSensorTypeGet_ov3620(
	UINT32 *pType
)
{
	*pType = SENSOR_CMOS_OV3620_TYPE;
}

/* sun@1021 start pm 14:55*/
void FlashTriggerPulse(void)
{
	UINT32 temp1;
	printf(" flash pulse\n");
	hwFrontGpioRead(5,&temp1);
	hwFrontGpioSet(5,temp1 | 0x80);
}
#if 0
void FlashTrigger(void)
{
	tmrEnable(2, 146, TMR_ONE_SHOT, FlashTriggerPulse);
	hwTmrEnable(2);
}
#endif
/* sun@1021 end pm 14:55*/

