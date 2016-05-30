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
/*#define RS_232_DBG add by asen*/


#include "general.h"
#include "hw_front.h"
#include "hw_cdsp.h"
#include "hw_cpu.h"
#include "stdio.h"
#include "device.h"

#include "aaa_api.h"
#include "tv_ov.h"
#include "hw_gpio.h"
#include "gpio.h"
#include "os_api.h"
#include "xiaoao.h"


#define MAXHSIZEM 			658    /*640*/
#define MAXVSIZEM 			480  

#define SENSOR_WIDTH   		1280
#define SENSOR_HEIGHT  		1024

#if Oem268_Dc260
#define HOFFSET_P 			0x80     
#define VOFFSET_P 			0x0e     

#define HOFFSET_F 			0xc6  
#define VOFFSET_F 			0x10
#define REG_1E_VALUE		0x30
#else
#define HOFFSET_P 			0x80     
#define VOFFSET_P 			0x0f     

#define HOFFSET_F 			0xc6
#define VOFFSET_F 			0x11
#define REG_1E_VALUE		0x00
#endif

#define EXT_HSIZE 0
#define EXT_VSIZE 0

UINT32 Reg2D=0,Reg2E=0;
extern UINT32 Shutter,Gain;

UINT32  ESP_LINE;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 aaaAeEntryFlag;

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
UINT8	uiOnBusy = 0;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 #if 0
AEstr LVtbl50[121]={/* for preview(Default) */
{0,100,496,0},{1,100,124,0},{2,100,56,0},{3,100,20,0},{4,100,15,0},
{5,100,13,0},{6,100,11,0},{7,111,12,0},{8,111,10,0},{9,125,12,0},
{10,125,10,0},{11,144,12,0},{12,166,14,0},{13,166,12,0},{14,200,15,0},
{15,200,13,0},{16,200,11,0},{17,201,10,0},{18,250,14,0},{19,250,12,0},
{20,250,10,0},{21,333,16,0},{22,333,14,0},{23,333,12,0},{24,335,11,0},
{25,333,9,0},{26,333,7,0},{27,335,6,0},{28,333,4,0},{29,333,3,0},
{30,335,2,0},{31,501,9,0},{32,501,7,0},{33,503,6,0},{34,501,4,0},
{35,501,3,0},{36,503,2,0},{37,503,1,0},{38,1002,15,0},{39,1002,13,0},
{40,1002,11,0},{41,1002,9,0},{42,1010,8,0},{43,1002,6,0},{44,1010,5,0},
{45,1002,3,0},{46,1002,2,0},{47,1002,1,0},{48,1010,0,0},{49,1084,0,0},
{50,1159,0,0},{51,1234,0,0},{52,1320,0,0},{53,1418,0,0},{54,1515,0,0},
{55,1626,0,0},{56,1731,0,0},{57,1851,0,0},{58,1990,0,0},{59,2150,0,0},
{60,2298,0,0},{61,2469,0,0},{62,2614,0,0},{63,2777,0,0},{64,3030,0,0},
{65,3252,0,0},{66,3418,0,0},{67,3703,0,0},{68,3921,0,0},{69,4444,1,0},
{70,4761,1,0},{71,5128,1,0},{72,5128,0,0},{73,5555,0,0},{74,6349,1,0},
{75,6349,0,0},{76,8888,5,0},{77,9523,5,0},{78,7843,0,0},{79,8333,0,0},
{80,8888,0,0},{81,9523,0,0},{82,10256,0,0},{83,12121,2,0},{84,16666,7,0},
{85,13333,1,0},{86,13333,0,0},{87,19047,5,0},{88,19047,4,0},{89,22222,6,0},
{90,22222,4,0},{91,22222,3,0},{92,26666,5,0},{93,26666,4,0},{94,33333,7,0},
{95,33333,6,0},{96,26666,0,0},{97,33333,3,0},{98,33333,2,0},{99,44444,6,0},
{100,66666,15,0},{101,44444,3,0},{102,44444,2,0},{103,44444,1,0},{104,44444,0,0},
{105,66666,6,0},{106,66666,5,0},{107,66666,3,0},{108,66666,2,0},{109,66666,1,0},
{110,133333,15,0},{111,133333,13,0},{112,133333,11,0},{113,133333,10,0},{114,133333,8,0},
{115,133333,6,0},{116,133333,5,0},{117,133333,4,0},{118,133333,2,0},{119,133333,1,0},
{120,133333,0,0}
};


/*  Output_AETable_60_for_53x  */
AEstr LVtbl60[121]={/* for Preview(Default) */
{0,100,496,0},{1,100,124,0},{2,100,56,0},{3,100,20,0},
{0,100,15,0},{1,100,13,0},{2,100,11,0},{3,109,11,0},{4,109,10,0},
{5,120,10,0},{6,150,15,0},{7,150,13,0},{8,150,11,0},{9,150,9,0},
{10,200,15,0},{11,200,13,0},{12,200,11,0},{13,240,15,0},{14,240,13,0},
{15,240,11,0},{16,300,15,0},{17,300,13,0},{18,300,11,0},{19,301,10,0},
{20,300,8,0},{21,300,6,0},{22,300,5,0},{23,400,10,0},{24,300,2,0},
{25,300,1,0},{26,300,0,0},{27,400,4,0},{28,600,12,0},{29,600,10,0},
{30,400,0,0},{31,600,7,0},{32,600,5,0},{33,600,4,0},{34,606,3,0},
{35,600,1,0},{36,600,0,0},{37,1201,14,0},{38,1201,12,0},{39,1201,10,0},
{40,1201,9,0},{41,1201,7,0},{42,1201,6,0},{43,1201,4,0},{44,1201,3,0},
{45,1212,2,0},{46,1234,1,0},{47,1234,0,0},{48,1320,0,0},{49,1418,0,0},
{50,1515,0,0},{51,1626,0,0},{52,1731,0,0},{53,1851,0,0},{54,1990,0,0},
{55,2150,0,0},{56,2298,0,0},{57,2469,0,0},{58,2614,0,0},{59,2777,0,0},
{60,3030,0,0},{61,3252,0,0},{62,3418,0,0},{63,3703,0,0},{64,3921,0,0},
{65,4444,1,0},{66,4761,1,0},{67,5128,1,0},{68,5128,0,0},{69,5555,0,0},
{70,6349,1,0},{71,6349,0,0},{72,8888,5,0},{73,9523,5,0},{74,7843,0,0},
{75,8333,0,0},{76,8888,0,0},{77,9523,0,0},{78,10256,0,0},{79,12121,2,0},
{80,16666,7,0},{81,13333,1,0},{82,13333,0,0},{83,19047,5,0},{84,19047,4,0},
{85,22222,6,0},{86,22222,4,0},{87,22222,3,0},{88,26666,5,0},{89,26666,4,0},
{90,33333,7,0},{91,33333,6,0},{92,26666,0,0},{93,33333,3,0},{94,33333,2,0},
{95,44444,6,0},{96,66666,15,0},{97,44444,3,0},{98,44444,2,0},{99,44444,1,0},
{100,44444,0,0},{101,66666,6,0},{102,66666,5,0},{103,66666,3,0},{104,66666,2,0},
{105,66666,1,0},{106,133333,15,0},{107,133333,13,0},{108,133333,11,0},{109,133333,10,0},
{110,133333,8,0},{111,133333,6,0},{112,133333,5,0},{113,133333,4,0},{114,133333,2,0},
{115,133333,1,0},{116,133333,0,0}
};
#endif


#if 1
AEstr LVtbl50[116] = {
{0,100,240,0},{1,100,60,0},{2,100,24,0},{3,100,15,0},{4,100,13,0},
{5,100,11,0},{6,100,10,0},{7,111,11,0},{8,125,12,0},{9,125,10,0},
{10,142,12,0},{11,166,14,0},{12,166,12,0},{13,167,11,0},{14,200,14,0},
{15,200,12,0},{16,200,10,0},{17,250,14,0},{18,250,12,0},{19,251,11,0},
{20,250,9,0},{21,334,15,0},{22,334,13,0},{23,334,11,0},{24,334,9,0},
{25,335,8,0},{26,334,6,0},{27,336,5,0},{28,334,3,0},{29,334,2,0},
{30,334,1,0},{31,335,0,0},{32,500,6,0},{33,505,5,0},{34,500,3,0},
{35,500,2,0},{36,500,1,0},{37,505,0,0},{38,1000,13,0},{39,1000,11,0},
{40,1010,10,0},{41,1000,8,0},{42,1000,6,0},{43,1000,5,0},{44,1010,4,0},
{45,1000,2,0},{46,1000,1,0},{47,1000,0,0},{48,1063,0,0},{49,1136,0,0},
{50,1219,0,0},{51,1298,0,0},{52,1388,0,0},{53,1492,0,0},{54,1612,0,0},
{55,1724,0,0},{56,1851,0,0},{57,1960,0,0},{58,2083,0,0},{59,2272,0,0},
{60,2439,0,0},{61,2564,0,0},{62,2777,0,0},{63,2941,0,0},{64,3333,1,0},
{65,3571,1,0},{66,3846,1,0},{67,3846,0,0},{68,4166,0,0},{69,4761,1,0},
{70,4761,0,0},{71,6666,5,0},{72,7142,5,0},{73,5882,0,0},{74,6250,0,0},
{75,6666,0,0},{76,7142,0,0},{77,7692,0,0},{78,9090,2,0},{79,12500,7,0},
{80,10000,1,0},{81,10000,0,0},{82,14285,5,0},{83,14285,4,0},{84,16666,6,0},
{85,16666,4,0},{86,16666,3,0},{87,20000,5,0},{88,20000,4,0},{89,25000,7,0},
{90,25000,6,0},{91,20000,0,0},{92,25000,3,0},{93,25000,2,0},{94,33333,6,0},
{95,50000,15,0},{96,33333,3,0},{97,33333,2,0},{98,33333,1,0},{99,33333,0,0},
{100,50000,6,0},{101,50000,5,0},{102,50000,3,0},{103,50000,2,0},{104,50000,1,0},
{105,100000,15,0},{106,100000,13,0},{107,100000,11,0},{108,100000,10,0},{109,100000,8,0},
{110,100000,6,0},{111,100000,5,0},{112,100000,4,0},{113,100000,2,0},{114,100000,1,0},
{115,100000,0,0}
};

AEstr LVtbl60[116] = {
{0,100,240,0},{1,100,60,0},{2,100,24,0},
{0,100,15,0},{1,100,13,0},{2,100,11,0},{3,100,10,0},{4,109,10,0},
{5,120,11,0},{6,150,15,0},{7,150,13,0},{8,150,11,0},{9,150,10,0},
{10,200,16,0},{11,200,14,0},{12,200,12,0},{13,240,15,0},{14,240,13,0},
{15,240,11,0},{16,242,10,0},{17,300,14,0},{18,300,12,0},{19,300,10,0},
{20,300,8,0},{21,300,7,0},{22,300,5,0},{23,300,4,0},{24,400,9,0},
{25,400,7,0},{26,300,0,0},{27,400,4,0},{28,400,3,0},{29,606,11,0},
{30,602,9,0},{31,602,7,0},{32,606,6,0},{33,602,4,0},{34,602,3,0},
{35,606,2,0},{36,606,1,0},{37,1204,15,0},{38,1204,13,0},{39,1204,11,0},
{40,1204,9,0},{41,1204,8,0},{42,1204,6,0},{43,1204,5,0},{44,1204,3,0},
{45,1204,2,0},{46,1204,1,0},{47,1219,0,0},{48,1298,0,0},{49,1388,0,0},
{50,1492,0,0},{51,1612,0,0},{52,1724,0,0},{53,1851,0,0},{54,1960,0,0},
{55,2083,0,0},{56,2272,0,0},{57,2439,0,0},{58,2564,0,0},{59,2777,0,0},
{60,2941,0,0},{61,3333,1,0},{62,3571,1,0},{63,3846,1,0},{64,3846,0,0},
{65,4166,0,0},{66,4761,1,0},{67,4761,0,0},{68,6666,5,0},{69,7142,5,0},
{70,5882,0,0},{71,6250,0,0},{72,6666,0,0},{73,7142,0,0},{74,7692,0,0},
{75,9090,2,0},{76,12500,7,0},{77,10000,1,0},{78,10000,0,0},{79,14285,5,0},
{80,14285,4,0},{81,16666,6,0},{82,16666,4,0},{83,16666,3,0},{84,20000,5,0},
{85,20000,4,0},{86,25000,7,0},{87,25000,6,0},{88,20000,0,0},{89,25000,3,0},
{90,25000,2,0},{91,33333,6,0},{92,50000,15,0},{93,33333,3,0},{94,33333,2,0},
{95,33333,1,0},{96,33333,0,0},{97,50000,6,0},{98,50000,5,0},{99,50000,3,0},
{100,50000,2,0},{101,50000,1,0},{102,100000,15,0},{103,100000,13,0},{104,100000,11,0},
{105,100000,10,0},{106,100000,8,0},{107,100000,6,0},{108,100000,5,0},{109,100000,4,0},
{110,100000,2,0},{111,100000,1,0},{112,100000,0,0}
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
hwFrontInit_ov9620(
	UINT32 cammode
)
{
    UINT32  regaddr[1], regdata[1];
    UINT32 i;
    frontSenReg_t *pfrontSenReg;
    frontSifReg_t *pfrontSifReg;
    cdspReg0_t *pcdspReg0;
    UINT32 monitorH, monitorV;	

    if (cammode != 0) {
        return SUCCESS;
    }

    pfrontSenReg = (frontSenReg_t *)(0xb0000000);
    pfrontSifReg = (frontSifReg_t *)(0xb0000000);
	pcdspReg0 =(cdspReg0_t *)(0xb0000000); 

    dprint("FRONT enter hwFrontInit OV9650 step1\n");

//    hwFrontGpioSel(0x3FFFFFFF, 0x750F);	/* Select GPIO or sensor */
//    hwFrontGpioCfg(0x00000000, 0xC0000000, 0x0200, 0x8AF0); /* Clk2x output enable */
	hwFrontGpioSel(0x3FFFFFFF, 0xF50F);	/* Select GPIO or sensor sun@1022 am 9:19 */
	hwFrontGpioCfg(0x00000000, 0xC0000000, 0x8200, 0x8AF0); /* Clk2x output enable sun@1022 am 9:19 */
	/*====================================================================*/
	/*Jinch@2004/9/30 For TV656+OV2610*/
//	WRITE8(0xb0000010, 0x04);	
//	hwTgGpioCfgSet(0x00,0x00,0x7400,0x7400);
//	hwTgGpioMsbMaskWrite(0x2000,0x7400);
	/*====================================================================*/
    hwFrontInputGate(0x0000);

    hwFrontClkCfg(1, 0, 1);
	/*hwFrontClkSet(0, 4, 2); */                 /* Set master clock & pixsel clock */
	hwFrontClkSet(0, 2, 2);		/*24MHz PCLK   Jinch*//*xiaobao  for bug DV desultory  06.6.5*/
    /*hwFrontClkPhaseCfg(0, 0, 3, 0 );*/
	hwFrontClkPhaseCfg(0, 1, 3, 0 );	/*Jinch*/
	
    hwFrontMaxMonitorSizeGet(&monitorH, &monitorV);
    pfrontSenReg->senRst = 0x01;             /* Reset Sensor interface */
    pfrontSenReg->senRst = 0x00;

    pfrontSenReg->vdUpdate = 0x01;           /* Enable Vdupdate */
    pfrontSenReg->reshapeCtr = 0x00;         /* Reshape vertical enable */
    pfrontSenReg->hOffset = /*0x098*/HOFFSET_P;          /* Set Hoffset */
    pfrontSenReg->hSize  = monitorH;           /* Set hsize */
    pfrontSenReg->vOffset = /*0x0014*/VOFFSET_P;          /* 0x14  Set Voffset */
    pfrontSenReg->vSize  = monitorV;           /* Set Vsize */
    pfrontSenReg->lineTotal  = 0x0320;       /* Set Linetotal, for frame rate control */
    pfrontSenReg->frameTotal = 0x01F2;       /* Set Frametotal */

	/*====================================================================*/
	/*Jinch@2004/9/30 For TV656+OV2610*/
	pfrontSenReg->tvCtr = 0;
	pfrontSenReg->yuvS128en = 0;
	pfrontSenReg->capInterval = 0;
	/*====================================================================*/

    hwFrontDummyWr(0, 0x0320);               /* Save linetotal to dummy reg*/
    hwFrontDummyWr(1, 0x01F2);               /* Save frametotal to dummy reg*/
    hwFrontDummyWr(2, monitorH);               /* Save hsize to dummy reg*/
    hwFrontDummyWr(3, /*0x098*/HOFFSET_P);               /* Save hoffset to dummy reg*/
    hwFrontDummyWr(4, /*0x098*/HOFFSET_P);               /* Save hoffset default to dummy reg*/
    hwFrontDummyWr(5, monitorV);               /* Save vsize to dummy reg*/
    hwFrontDummyWr(6, /*0x0014*/VOFFSET_P);               /* Save voffset to dummy reg*/
    hwFrontDummyWr(7, /*0x0014*/VOFFSET_P);               /* Save voffset default to dummy reg*/

    /* Use SSC to initializes OV9620 registers */
    pfrontSifReg->slaveAddr = 0x60;          /* Set sensor slave address */
    pfrontSifReg->frequency = 0x03;          /* Set SSC Frequency */
    regaddr[0] = 0x12;
    regdata[0] = 0x80;                       /* Rst Ov9650 */
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    for (i = 0; i < 65535; i ++) {             /* Just for Delay */
    }
    regaddr[0] = 0x12;
    regdata[0] = 0x45;/*0x60;                       Set VGA Mode */
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    for (i = 0; i < 65535; i ++) {             /* Just for Delay */
    }
    regaddr[0] = 0x39;
    regdata[0] = 0x40;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x0e;
    regdata[0] = 0x00;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x13;
    regdata[0] = 0xc0; 
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x26;
    regdata[0] = 0xe2;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x38;
    regdata[0] = 0x12;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
    regaddr[0] = 0x11;
    /*regdata[0] = 0x80;*/
	regdata[0] = 0x82;	/*Jinch  81*/
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x01;
    regdata[0] = 0x80;                     
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
    regaddr[0] = 0x02;
    regdata[0] = 0x80;                     
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
    regaddr[0] = 0x00;
    regdata[0] = 0x00;                       
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
    regaddr[0] = 0x10;
    regdata[0] = 0xf0;                      
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
//    regaddr[0] = 0x1e;
//    regdata[0] = 0x00;
//    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x69;
    regdata[0] = /*0x80*/0x40;      /*Jinch modified 05/04/26*/
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x1b;
    regdata[0] = 0x00;	/* 0x00*/
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x16;
    regdata[0] = 0x06;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x33;
    regdata[0] = 0x10;//0x0c;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x34;
    regdata[0] = 0xbf;/*0xbf;*/
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
#if 0	
	/**********/		
		regaddr[0]	= 0x36; 				
		regdata[0]	= 0x17;/*0x07;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x37; 				
		regdata[0]	= 0x05;	/* 0x04 */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	
		regaddr[0]	= 0x38; 				
		regdata[0]	= 0x1e;/*0x52;0x72;*/
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	/*********/
	#endif
    regaddr[0] = 0xa8;
    regdata[0] = 0x81;                     
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 	
    regaddr[0] = 0x41;
    regdata[0] = 0x10;                     
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 
    regaddr[0] = 0x96;
    regdata[0] = 0x04;                       
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 	
    regaddr[0] = 0x3d;
    regdata[0] = 0x19;                      
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
    regaddr[0] = 0x3a;
    regdata[0] = 0x0d;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
    regaddr[0] = 0x8e;
    regdata[0] = 0x00;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x3c;
    regdata[0] = 0x60;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x8f;
    regdata[0] = 0xcf;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x8b;
    regdata[0] = 0x06;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x35;
    regdata[0] = 0x91;/*0x91;*/
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x94;
    regdata[0] = 0x99;                      
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
    regaddr[0] = 0x95;
    regdata[0] = 0x99;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
    regaddr[0] = 0x40;
    regdata[0] = 0xc1;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x29;
    regdata[0] = 0x2f;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x0f;
    regdata[0] = 0x42;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x3a;
    regdata[0] = 0x0d;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0xa5;
    regdata[0] = 0x80;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x1e;
    regdata[0] = REG_1E_VALUE;                      
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
/*Jinch modified 05/04/26*/
   /* 
    regaddr[0] = 0xa9;
    regdata[0] = 0x97;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);	
    regaddr[0] = 0x3a;
    regdata[0] = 0x0c;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    */
   
    regaddr[0] = 0x8f;
    regdata[0] = 0xcf;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x90;
    regdata[0] = 0x00;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x91;
    regdata[0] = 0x00;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x9f;
    regdata[0] = 0x00;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0xa0;
    regdata[0] = 0x00;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
     /**/
    regaddr[0] = 0x15;
    regdata[0] = 0x4a;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x14;
    regdata[0] = 0x4e;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);

    /*
    regaddr[0] = 0x20;
    regdata[0] = 0x90;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x21;
    regdata[0] = 0x88;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x22;
    regdata[0] = 0x88;                     
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 	
      regaddr[0] = 0x23;
    regdata[0] = 0x90;                     
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00); 	
    */	

    /*Jinch add 05/04/26 start*/
    regaddr[0] = 0x30;
    regdata[0] = 0x08;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0xa9;
    regdata[0] = 0xb8;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0xaa;
    regdata[0] = 0x92;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0xab;
    regdata[0] = 0x0a;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    /*Jinch add 05/04/26 end*/

    regaddr[0] = 0x18;
    regdata[0] = 0xd0;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    hwFrontVdWait(0 ,1);	
    regaddr[0] = 0x17;
    regdata[0] = 0x0e;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    hwFrontVdWait(0 ,1);    	

    regaddr[0] = 0x19;
    regdata[0] = 0x00;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    hwFrontVdWait(0 ,1);	 
	
    hwFrontSscRead(0x03, regdata, 0x01, 0x02);
    regaddr[0] = 0x03;
    regdata[0]&=0xfc;
    regdata[0] = regdata[0]|(0x02);
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    hwFrontVdWait(0 ,1);
	 
    aaaMaxFrameRateSet(30);
    aaaAeawbStatusSet(1);  /*Force AEAWB not ready: 0 -> not ready, 1 -> ready*/
    hwFrontFrameRateSet_ov9620(30, 0);
    hwFrontExpTimeSet_ov9620(300, 0);
	
    hwFrontProbeSet(1, 2);
	pcdspReg0->pixelSel = 2; /*0,1,2,3*/
    /* hwFrontFlashSet(9000, 10, 3); */
    aaaAeEntryFlag = 0;

    dprint("FRONT exit hwFrontInit OV9650 step2\n");

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
hwFrontSnap_ov9620(
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

	praFunc = prawAddrFunc; /* cont shot customize */
	pbpFunc = pbadPixFunc; /* cont shot customize */

    pfrontSenReg = (frontSenReg_t *)(0xb0000000);
    pcdspReg0 = (cdspReg0_t *)(0xb0000000);
    pcdspReg1 = (cdspReg1_t *)(0xb0000000);
    pccdReg0 = (ccdReg0_t *)(0xb0000000);
    pccdReg1 = (ccdReg1_t *)(0xb0000000);
    pcdspWinReg = (cdspWinReg_t *)(0xb0000000);
    tmp0 = pfrontSenReg->syncGenen;
    pfrontSenReg->syncGenen = tmp0 | 0x02;     /* Set Set totalsvden */
    pfrontSenReg->sizeSvden = 0x03;            /* Set Size & Offset to sync with VD */
    badstatus = pcdspReg0->badpixen;           /* Read Bad pixel enable or not */
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

    hwFrontFullSet_ov9620(0);
	/* hwFrontGpioSet(0, 0xFF); */
    pcdspReg0->badpixen = 0x00;                /* Disable Bad pixel median filter */
    pccdReg0->snapCtr = tmp0;                  /* Set snap trigger */
	for (idx = 1; idx <= snapnum; idx++) {
		if (praFunc)
			(*praFunc)(idx);

		hwFrontVdWait(0, 1);
		pcdspReg0->hScaleCtr  = 0x00;     /* Disable h scale */
		hwFrontVValidWait(0, 1);

		if (pbpFunc)
			(*pbpFunc)(idx);
	}
#if (0)
    do {
    	tmp0 = pccdReg0->snapCtr;              /* Wait snap trigger low */
    }
    while ((tmp0 & 0x10) == 0x10);
#endif

	/* hwFrontGpioSet(0, 0x00); */
	PlayEffectSound(0); //tzw add for bug 8
	uiOnBusy = 1;	
    hwCdspDataSource(1);
    pcdspReg0->badpixen  = badstatus;          /* Save back Bad pixel enable or not */
    /*pcdspReg0->gainSvden = 0x11;*/
	
   	pcdspReg1->rawMode    = 0x00;         /* Set to rawmode 1, pass through gain & hscale */
	pcdspReg0->rGain      = rgain;         /* Set unit gain */
	pcdspReg0->grGain     = grgain;
	pcdspReg0->bGain      = bgain;
	pcdspReg0->gbGain     = gbgain;
	pcdspReg0->globalGain = ggain;
	pcdspReg0->hScaleCtr = hscaleen;

    hwFrontPreviewSet_ov9620();                       /* Turn back to preview mode */
    hwFrontVdWait(0, 1);                       /* Wait a VD to out the function */
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
hwFrontPreviewSet_ov9620(
	void
)
{
    UINT32 tmp0,i;
    UINT32 tmp2, tmp3;
    UINT32  regaddr[1], regdata[1];
	frontSenReg_t *pfrontSenReg;
	cdspReg0_t *pcdspReg0;
    UINT32 monitorH,monitorV;	

 
     hwFrontMaxMonitorSizeGet(&monitorH, &monitorV);  
     dprint("FRONT hwFrontPreviewSet OV9650 step3\n");/*add by asen*/
	pcdspReg0 =(cdspReg0_t *)(0xb0000000); 
    pfrontSenReg = (frontSenReg_t *)(0xb0000000);
    pfrontSenReg->sizeSvden = 0x03;          /* Offset & Size sync VD */
    tmp0 = pfrontSenReg->syncGenen;
    pfrontSenReg->syncGenen = tmp0 | 0x02;   /* Set Set totalsvden */
    hwFrontDummyRd(2, &tmp2);                /* Read back preview Hoffset, Hsize */
    hwFrontDummyRd(3, &tmp3);
	/*tmp3=0x90;add by asen*/
    pfrontSenReg->hOffset = tmp3;            /*Set Sensor Hoffset */
    pfrontSenReg->hSize   = tmp2;            /* Set Sensor Hsize */

    dprint2("Hoffset = %x, Hsize = %x \n", tmp3, tmp2);

    hwFrontDummyRd(5, &tmp2);                /* Read back preview Voffset, Vsize */
    hwFrontDummyRd(6, &tmp3);
    /*tmp3=0x10;add by asen*/	
    pfrontSenReg->vOffset = tmp3;        /* Set Sensor Voffset */
    pfrontSenReg->vSize   = tmp2;            /* Set Sensor Vsize */
    pcdspReg0->pixelSel =2;  /*0,1,2,3 control the colour*/
    dprint2("Voffset = %x, Vsize = %x \n", tmp3, tmp2);

    hwFrontDummyRd(1, &tmp2);                /* Read back frametotal */
    pfrontSenReg->frameTotal = tmp2;         /* Write frametotal */
    dprint1("frameTotal = %x \n", tmp2);
    hwFrontDummyRd(0, &tmp2);                /* Read back linetotal */
    pfrontSenReg->lineTotal = tmp2;          /* Write linetotal */

    pfrontSenReg->sizeSvden = 0x00;        /*set to VGA mode*/
    regaddr[0] = 0x12;
    regdata[0] = 0x45;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    regaddr[0] = 0x11;
    regdata[0] = 0x82;      /*Jinch modified05/04/26*/
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    for (i = 0; i < 65535; i ++) {             /* Just for Delay */
    }
	
    regaddr[0] = 0x18;
    regdata[0] = 0xd0;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    hwFrontVdWait(0 ,1);
    regaddr[0] = 0x17;
    regdata[0] = 0x0e;
    hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
    hwFrontVdWait(0 ,1);
    pfrontSenReg->sizeSvden = 0x03;
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
hwFrontFullSet_ov9620(
	UINT32 field
)
{
    UINT32 regaddr[1], regdata[1], i;
    frontSenReg_t *pfrontSenReg;
    cdspReg0_t *pcdspReg0;
    pcdspReg0 =(cdspReg0_t *)(0xb0000000);          /*Jinch modified05/04/26*/
    pfrontSenReg = (frontSenReg_t *)(0xb0000000);

	if (field == 0) {
        pfrontSenReg->sizeSvden = 0x00;        
        regaddr[0] = 0x12;
        regdata[0] = 0x05;
        hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	 hwFrontVdWait(0 ,1);


	 regaddr[0] = 0x19;
        regdata[0] = 0x00;
        hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	 hwFrontVdWait(0 ,1);	
	 
	 hwFrontSscRead(0x03, regdata, 0x01, 0x02);
    	regaddr[0] = 0x03;
    	regdata[0]&=0xfc;
    	regdata[0] = regdata[0]|(0x02);
        hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	 hwFrontVdWait(0 ,1);
	 
        regaddr[0] = 0x17;
        regdata[0] = 0x1A;
        hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);
	 hwFrontVdWait(0 ,1);      

	/*Jinch modify start*/	
        pfrontSenReg->hSize = /*0x500*/SENSOR_WIDTH;            /* Set Sensor Hsize */
        pfrontSenReg->hOffset = /*0x00f0*/HOFFSET_F;          /* Set Sensor Hoffset */
        pfrontSenReg->vSize = /*0x3C0*/SENSOR_HEIGHT;            /* Set Sensor Vsize */
        pfrontSenReg->vOffset = /*0x0016*/VOFFSET_F;            /* Set Sensor Voffset */
	/*Jinch modify end*/

	pcdspReg0->pixelSel =2;  /*0,1,2,3 control the colour*/
	dprint2("hwFrontFullSet hSize=%x; vSize=%x\n",0x500,	0x3C0);
        /*regaddr[0] = 0x1A;
        regdata[0] = 0x81;
        hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);*/
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
hwFrontGainSet_ov9620(
    UINT32 value,
    UINT32  option
)
{
    UINT32 tmp0;
    UINT32 regaddr[1], regdata[1];

    tmp0 = option & 0x01;                 /* Sync VD ? */

    regaddr[0] = 0x00;
    regdata[0] = value&0xff;
    hwFrontSscWrite(regaddr, regdata, 0x01, tmp0);

	/*Jinch*/
    hwFrontSscRead(0x03, regdata, 0x01, 0x02);
    regaddr[0] = 0x03;
    regdata[0]&=0x3f;
    regdata[0] = regdata[0]|((value&0x300)>>2);
    hwFrontSscWrite(regaddr, regdata, 0x01, tmp0);

	printf("Set gain value = %d\n",value);

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
hwFrontExpTimeSet_ov9620(
	UINT32 value,
	UINT32 option
)
{
	UINT32 ltotal, ftotal, espline, tmp1;
	UINT32 pixclk, tmp0;
	frontSifReg_t *pfrontSifReg;
	frontSenReg_t *pfrontSenReg;
	ccdReg0_t *pccdReg0;
	UINT32  regaddr[2], regdata[2],readdata;

	pfrontSifReg = (frontSifReg_t *)(0xb0000000);
	pfrontSenReg = (frontSenReg_t *)(0xb0000000);
	pccdReg0 = (ccdReg0_t *)(0xb0000000);

	ltotal = pfrontSenReg->lineTotal;                         /* Read line total */
	ftotal = pfrontSenReg->frameTotal;                     /* Read frame total */
	tmp1   = pfrontSifReg->pixClkHi;                         /* Read pixel clock */
	tmp0   = (tmp1 << 16);
	tmp1   = pfrontSifReg->pixClkLow;                        /* Read pixel clock */
	pixclk = tmp0 | tmp1;
	pixclk = pixclk/3;
	espline = (pixclk * 10) / (ltotal * value);
	if(value==200)
	    espline = 498;
	printf("espline = %x \n", espline);
	printf("ftotal = %x \n", ftotal);
	printf("pixclk = %d\n",pixclk);
	printf("hwFrontExpTimeSet value=%d\n",value);
	printf("ltotal=%x\n",ltotal);
	/*if (espline > ftotal) {
		printf("espline error!\n");
		return FAIL;
	}*/


#if 0
      if ( espline < 1 )
		espline =1;   /*Cynthia 20040531*/	
	 
        tmp1 = ftotal - 612;
         espline = espline - tmp1;
         /*if(espline>500)    espline-=3;*/
		 
        ESP_LINE = espline;
	tmp1 = (option & 0x01);
	regaddr[0] = 0x04;
	regdata[0] = espline & 0x03;
	regaddr[1] = 0x10;
	regdata[1] = espline >> 2;
	hwFrontSscWrite(regaddr, regdata, 0x02, tmp1 | 0x02);
	dprint1("ESP_LINE = %d\n",ESP_LINE);
	return SUCCESS;
#endif


        hwFrontSscRead(0x04, &readdata, 0x01, 0x02); 
      /*   printf("readdata 0x04  = %x\n",readdata); */
        

     if ( espline < 1 )
		espline =1;   /*Cynthia 20040531*/
 
    ESP_LINE = espline; 
	tmp0 = (option & 0x01); 
	regaddr[0] = 0x04;
	regdata[0] = (espline & 0x03) /*| (readdata & 0xFC)*/;
	regaddr[1] = 0x10;
	regdata[1] = espline >> 2;
	/*regaddr[1] = 0x10;
	regdata[1] = espline >> 2;*/
	hwFrontSscWrite(regaddr, regdata, 0x02, tmp0 | 0x02);
	
	return SUCCESS;
}



UINT32
hwFrontFrameRateSet_ov9620(
    UINT32 value,
    UINT32 option
)
{
	UINT32 ltotal, ftotal, tmp1, newftotal;
	UINT32 pixclk, tmp0;
	UINT32 sync;
	SINT32  espline;
	frontSifReg_t *pfrontSifReg;
	frontSenReg_t *pfrontSenReg;
	ccdReg0_t *pccdReg0;
	UINT32  regaddr[4], regdata[4];
	UINT32 videomode;

	pfrontSifReg = (frontSifReg_t *)(0xb0000000);   
	pfrontSenReg = (frontSenReg_t *)(0xb0000000); 
	pccdReg0 = (ccdReg0_t *)(0xb0000000);

	videomode = videoClipModeGet();

	if ( ( videomode == 1 ) && ( value <= 10 ) ) {
		hwFrontClkSet(1, 12, 2);				/*Set pixel clock to 6MHz when VGA video clip at 10fps*/
		aaaMaxFrameRateSet(value);
	}
	else if ( (videomode == 1 ) && ( value <= 20 ) ) {
		hwFrontClkSet(1, 6, 2);
		aaaMaxFrameRateSet(value);
	}
	else if ( (videomode == 1) && (value <= 30) ) {
		hwFrontClkSet(1, 4, 2);
		aaaMaxFrameRateSet(value);
	}
	else if( videomode == 2 ) {
		hwFrontClkSet(1, 4, 2);
		aaaMaxFrameRateSet(30);	
	}

	ltotal = pfrontSenReg->lineTotal;                        /* Read line total */
	ftotal = pfrontSenReg->frameTotal;                       /* Read frame total */    
	tmp1   = pfrontSifReg->pixClkHi;                         /* Read pixel clock */
	tmp0   = (tmp1 << 16); 
	tmp1   = pfrontSifReg->pixClkLow;                        /* Read pixel clock */   
	pixclk = tmp0 | tmp1;         
	pixclk = pixclk/3;                    
	sync = option & 0x01; 
	dprint("    FRONT enter hwfrontFramRateSet OV9650 step4\n");

	newftotal = pixclk / (value * ltotal);

	if(newftotal>498)
		newftotal = 498;
	
        pfrontSenReg->frameTotal = newftotal;     /* Write new frame total */
        hwFrontDummyWr(1, newftotal);             /* Save new frametotal to dummy reg*/

        espline = (ESP_LINE + ftotal) - newftotal;
        if (espline < 1)  espline = 1;
        ESP_LINE = espline;


        printf("    pixclk = %d \n", pixclk);
		printf("    newftotal = %x \n", newftotal);
		printf("    ftotal = %x \n", ftotal);
        printf(" hwFrontFrameRateSet vaule=%x\n",value);
        printf("    videomode = %x \n", videomode);  
        tmp0 = newftotal - 498;                   /* Line to extend */

        regaddr[0] = 0x04;
        regdata[0] = espline & 0x03;
        regaddr[1] = 0x10;
        regdata[1] = espline >> 2;
        /*hwFrontSscWrite(regaddr, regdata, 2, 0x02);deleted by asen*/
        regaddr[0] = 0x2D;
        regdata[0] = tmp0;
        regaddr[1] = 0x2E;
        regdata[1] = tmp0 >> 8;
        /*hwFrontSscWrite(regaddr, regdata, 2, sync | 0x02);deleted by asen*/
		dprint("    FRONT Exit hwfrontFramRateSet OV9650 \n");

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
hwFrontSuspend_ov9620(
	void
)
{
    hwFrontGpioSet(0, 0x00);
    hwFrontGpioSet(1, 0x00);
    hwFrontGpioSet(2, 0x00);
    hwFrontGpioSet(3, 0x00);
    hwFrontGpioSet(4, 0xE0);                  /* Only Sen & Sck, Sd = 1 */
    hwFrontGpioSet(5, 0x00);
    hwFrontGpioCfg(0x3FFFFFFF, 0xFFFFFFFF,  0xF73F, 0xFFFF);  /* Select GPIO */
    hwFrontInputGate(0x0000);
}

UINT32
hwFrontExpTimeGainSet_ov9620(
	UINT32 value,
	UINT32 value1,
	UINT32 option
)
{
	return 0;
}

void hwFrontMaxMonitorSizeGet_ov9620(
	UINT32 *pWidth,
	UINT32 *pHeight
)
{
	*pWidth= MAXHSIZEM;
	*pHeight = MAXVSIZEM;
}

void hwFrontExtMonitorSizeGet(
	UINT32 *pWidth, 
	UINT32 *pHeight
)
{
	*pWidth = EXT_HSIZE;
	*pHeight = EXT_VSIZE;
}

void hwFrontSensorSizeGet_ov9620(
	UINT32 *pwidth,
	UINT32 *pheight
)
{
	*pwidth = SENSOR_WIDTH;
	*pheight = SENSOR_HEIGHT;
}


void hwFrontSensorTypeGet_ov9620(
	UINT32 *pType
)
{
	*pType = SENSOR_CMOS_OV9620_TYPE;
}

