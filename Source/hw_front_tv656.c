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
#include "general.h"
#include "hw_front.h"
#include "hw_cdsp.h"
#include "hw_cpu.h"
#include "stdio.h"
#include "device.h"
#include "aaa_api.h"
#include "gpio.h"
#include "hw_gpio.h"
#include "tv656.h"
UINT32 MAXHSIZEM_NOW=MAXHSIZEM;
UINT32 MAXVSIZEM_NOW=MAXVSIZEM;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
AEstr LVtbl60_tv656[101] = {
	{70,200,0x17}, {71,200,0x15}, {72,200,0x14}, {73,200,0x12},
	{74,200,0x11}, {75,200,0x10}, {76,200,0x0e}, {77,200,0x0c},
	{78,200,0x0a}, {79,200,0x08}, {80,200,0x07}, {81,200,0x05},
	{82,200,0x04}, {83,240,0x06}, {84,240,0x04}, {85,299,0x08},
	{86,240,0x02}, {87,240,0x01}, {88,299,0x04}, {89,299,0x02},
   	{90,600,0x11}, {91,600,0x10}, {92,400,0x04}, {93,400,0x02},
    {94,600,0x09}, {95,600,0x08}, {96,600,0x06}, {97,600,0x05},
    {98,600,0x04}, {99,600,0x02}, {100,1196,0x11}, {101,1196,0x10},
    {102,1196,0x0e}, {103,1196,0x0c}, {104,1196,0x0a}, {105,1196,0x08},
    {106,1196,0x06}, {107,1196,0x05}, {108,1196,0x03}, {109,1196,0x02},
	{110,1339,0x03}, {111,1339,0x02}, {112,1540,0x03}, {113,1811,0x05},
	{114,1760,0x03}, {115,1866,0x03}, {116,1925,0x02}, {117,2053,0x02},
	{118,2369,0x03}, {119,2369,0x02}, {120,2566,0x02}, {121,2933,0x03},
	{122,3080,0x03}, {123,3850,0x06}, {124,3850,0x05}, {125,3850,0x03},
	{126,3850,0x02}, {127,4106,0x02}, {128,4400,0x02}, {129,5133,0x04},
	{130,5600,0x04}, {131,8800,0x0d}, {132,7700,0x08}, {133,7700,0x06},
	{134,7700,0x05}, {135,8800,0x06}, {136,7700,0x02}, {137,12320,0x0b},
	{138,8800,0x02},  {139,10266,0x04}, {140,10266,0x02}, {141,12320,0x04},
	{142,12320,0x03}, {143,12320,0x02}, {144,15400,0x05}, {145,15400,0x03},
	{146,15400,0x02}, {147,30800,0x11}, {148,20533,0x05}, {149,20533,0x04},
	{150,20533,0x02}, {151,20533,0x01}, {152,30800,0x08}, {153,30800,0x06},
	{154,30800,0x05}, {155,30800,0x03}, {156,30800,0x02}, {157,61601,0x11},
	{158,61601,0x0f}, {159,61601,0x0d}, {160,61601,0x0b}, {161,61601,0x09},
	{162,61601,0x08}, {163,61601,0x06}, {164,61601,0x05}, {165,61601,0x03},
	{166,61601,0x02}, {167,123203,0x11}, {168,123203,0x0f}, {169,123203,0x0d},
	{170,123203,0x0b}
};

AEstr LVtbl50_tv656[101] = {
	{70,200,0x18}, {71,200,0x16}, {72,200,0x15}, {73,200,0x13},
	{74,200,0x12}, {75,200,0x11}, {76,200,0x0f}, {77,200,0x0d},
	{78,200,0x0b}, {79,200,0x09}, {80,200,0x08}, {81,200,0x06},
	{82,200,0x05}, {83,249,0x08}, {84,332,0x0e}, {85,249,0x05},
	{86,332,0x0a}, {87,249,0x02}, {88,500,0x11}, {89,500,0x10},
	{90,500,0x0e}, {91,500,0x0c}, {92,500,0x0a}, {93,500,0x08},
	{94,500,0x07}, {95,500,0x05}, {96,500,0x04}, {97,500,0x02},
	{98,1001,0x11}, {99,1001,0x10}, {100,1001,0x0e}, {101,1001,0x0c},
	{102,1001,0x0a}, {103,1001,0x08}, {104,1026,0x07}, {105,1001,0x05},
	{106,1026,0x04}, {107,1044,0x03}, {108,1062,0x02}, {109,1184,0x03},
	{110,1283,0x03}, {111,1310,0x02}, {112,1466,0x03}, {113,1502,0x02},
	{114,1621,0x02}, {115,1925,0x04}, {116,1925,0x03}, {117,2200,0x04},
	{118,2200,0x03}, {119,2369,0x03}, {120,2800,0x05}, {121,2566,0x02},
	{122,3080,0x04}, {123,3850,0x07}, {124,3850,0x06}, {125,3850,0x04},
	{126,3850,0x03}, {127,3850,0x02}, {128,7700,0x10}, {129,5133,0x04},
	{130,6844,0x09}, {131,5600,0x03}, {132,6844,0x07}, {133,6844,0x05},
	{134,7700,0x06}, {135,7700,0x04}, {136,8800,0x06}, {137,8000,0x04},
	{138,8800,0x03}, {139,10266,0x04}, {140,10266,0x03}, {141,10266,0x02},
	{142,12320,0x04}, {143,15400,0x07}, {144,15400,0x06}, {145,15400,0x04},
	{146,15400,0x03}, {147,20533,0x07}, {148,20533,0x06}, {149,20533,0x04},
	{150,20533,0x03}, {151,20533,0x02}, {152,30800,0x09}, {153,30800,0x07},
	{154,30800,0x06}, {155,30800,0x04}, {156,30800,0x03}, {157,30800,0x02},
	{158,61601,0x10}, {159,61601,0x0f}, {160,61601,0x0d}, {161,61601,0x0b},
	{162,61601,0x09}, {163,61601,0x07}, {164,61601,0x06}, {165,61601,0x04},
	{166,61601,0x03}, {167,61601,0x02}, {168,123203,0x10}, {169,123203,0x0f},
	{170,123203,0x0d}
};

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
hwFrontInit_tv656(
	UINT32 cammode
)
{
    frontSenReg_t *pfrontSenReg;
   	frontSifReg_t *pfrontSifReg;

    if (cammode != 0) {
        return SUCCESS;
    }
    pfrontSenReg = (frontSenReg_t *)(0xb0000000);
    pfrontSifReg = (frontSifReg_t *)(0xb0000000);

    printf("FRONT enter hwFrontInit TV656 \n");
    WRITE8(0xb0000010, 0x24); /* 1104: TV-in enable */
	hwWait(0, 50);
//    osTimeDly(50) ;
    hwFrontClkCfg(0, 1, 1);
    hwFrontClkSet(0, 1, 2);                  /* Set master clock & pixsel clock */

	/*hwFrontGpioSel(0x07000000, 0x0000);       Select GPIO or sensor */
	/*hwFrontGpioSel(0x070001ff, 0x7440);   */   /* Select GPIO or sensor */

	/*hwFrontGpioCfg(0x00000000, 0xFFFFFFFF, 0x0000, 0xFFFF);  Clk2x output xenable */
	/*hwFrontGpioCfg(0x00000000, 0xFFFFFE00, 0x0000, 0xFFFF);*//* Clk2x output xenable */

	hwFrontGpioSel(0x0700ffff, 0x0000);      /*Select GPIO or sensor */
	hwFrontGpioCfg(0x00000000, 0xFFFFFFFF, 0x0000, 0xFFFF);  /*Clk2x output xenable */
	/*====================================================================*/
	/*Jinch@2004/9/30 For TV656+OV2610*/
	/*hwTgGpioCfgSet(0x00,0x00,0x7400,0x7400);
	hwTgGpioMsbMaskWrite(0x5400,0x7400);
	*/
	/*====================================================================*/
#if 0	
#if 0    //by shuai 2005/10/11
 	gpioConfig(GPIO_KEY_COL_0, 1);
#endif
	gpioConfig(GPIO_KEY_COL_2, 1);
	gpioConfig(GPIO_KEY_COL_2, 1);	//Paul@2005/09/28
	gpioConfig(GPIO_KEY_ROW_0, 0);
	gpioConfig(GPIO_KEY_ROW_1, 0);
	gpioConfig(GPIO_KEY_ROW_2, 0);
#if 0    //by shuai 2005/10/11
//	gpioConfig(GPIO_KEY_ROW_3, 0);
#endif 
 	gpioConfig(GPIO_KEY_ROW_4, 0);
#endif

    hwFrontInputGate(0x0000);
    hwFrontClkPhaseCfg(0, 0, 3, 0);

    pfrontSenReg->senRst = 0x01;             /* Reset Sensor interface */
    pfrontSenReg->senRst = 0x00;
    pfrontSenReg->vdUpdate = 0x01;           /* Enable Vdupdate */

    #ifdef CCIR_601
    pfrontSenReg->tvCtr = 0x00;              /* Enable TV656 */
	#else
	pfrontSenReg->tvCtr = 0x10;              /* Enable TV656 */
	#endif
    pfrontSenReg->yuvS128en = 0x16;          /* Enable TV input */
    pfrontSenReg->capInterval = 0x01;        /* Set capture interval */

#if (TVDECODER == SAA7113)
    pfrontSifReg->slaveAddr = 0x4a;          /* Set sensor slave address */
#endif

#if (TVDECODER == SPV512A)
	pfrontSifReg->slaveAddr = 0x68;          /* Set sensor slave address */
#endif
	pfrontSifReg->frequency = 0x00;          /* Set SSC Frequency */
 
//    tvDecoderInit(0);                         /* Init Spca512 */

    /*hwCdspHscaleYUVSet(720, 320);
    hwCdspVscaleSet(288, 240);*/

	hwFrontProbeSet(1, 2);

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
hwFrontSnap_tv656(
	UINT32 snapnum,
	void *prawAddrFunc,
	void *pbadPixFunc
)
{
    UINT32 badstatus, tmp0;
    frontSenReg_t *pfrontSenReg;
    cdspReg0_t *pcdspReg0;
    ccdReg0_t *pccdReg0;
    ccdReg1_t *pccdReg1;
    cdspWinReg_t *pcdspWinReg;

    pfrontSenReg = (frontSenReg_t *)(0xb0000000);
    pcdspReg0 = (cdspReg0_t *)(0xb0000000);
    pccdReg0 = (ccdReg0_t *)(0xb0000000);
    pccdReg1 = (ccdReg1_t *)(0xb0000000);
    pcdspWinReg = (cdspWinReg_t *)(0xb0000000);
    tmp0 = pfrontSenReg->syncGenen;
    pfrontSenReg->syncGenen = tmp0 | 0x02;     /* Set Set totalsvden */
    pfrontSenReg->sizeSvden = 0x03;            /* Set Size & Offset to sync with VD */
    badstatus = pcdspReg0->badpixen;           /* Read Bad pixel enable or not */
    tmp0 = (snapnum & 0x0F) | 0x10;
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
hwFrontPreviewSet_tv656(
	void
)
{
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
void
hwFrontFullSet_tv656(
	UINT32 field
)
{
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
hwFrontGainSet_tv656(
    UINT32 value,
    UINT32  option
)
{
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
hwFrontExpTimeSet_tv656(
	UINT32 espline,
	UINT32 option
)
{
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
hwFrontFrameRateSet_tv656(
    UINT32 value,
    UINT32 option
)
{
    return SUCCESS;
}


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontSerialInit                                                  *
*                                                                                     *
*  Purposes      :                                                                    *
*    To initialize Front serial interface                                             *
*                                                                                     *
*  Desprictions  :                                                                    *
*    initialize front serial interface for different sensor                           *
*                                                                                     *
*  Arguments     :                                                                    *
*                                                                                     *
*  Returns       :                                                                    *
*    None                                                                             *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
void
hwFrontSerialInit_tv656(
	void
)
{
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
hwFrontSuspend_tv656(
	void
)
{
}


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontCcdAFSet                                                    *
*                                                                                     *
*  Purposes      :                                                                    *
*    Set CCD to Auto focus mode                                                       *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Set CCD Sensor to AF mode                                                        *
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
hwFrontCcdAFSet_tv656(
	void
)
{
}


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontLensInit                                                    *
*                                                                                     *
*  Purposes      :                                                                    *
*    To Initialize Lens Setting                                                       *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Initialize the parameter to meet the lens specification                          *
*                                                                                     *
*  Arguments     : None                                                               *
*                                                                                     *
*  Returns       :                                                                    *
*    Error message                                                                    *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32
hwFrontLensInit_tv656(
	void
)
{
	return SUCCESS;
}


/**************************************************************************************
*                                                                                     *
*  Function Name : hwFrontMSOpen                                                      *
*                                                                                     *
*  Purposes      :                                                                    *
*    To Open Mechanical shutter                                                       *
*                                                                                     *
*  Desprictions  :                                                                    *
*    To Open Mechanical shutter                                                       *
*                                                                                     *
*  Arguments     : None                                                               *
*                                                                                     *
*  Returns       :                                                                    *
*    Error message                                                                    *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32
hwFrontMSOpen_tv656(
	void
)
{
	return SUCCESS;
}

UINT32
hwFrontExpTimeGainSet_tv656(
	UINT32 value,
	UINT32 value1,
	UINT32 option
)
{
	return 0;
}

void hwFrontMaxMonitorSizeGet_tv656(
	UINT32 *pWidth,
	UINT32 *pHeight
)
{
	*pWidth= MAXHSIZEM_NOW;
	*pHeight = MAXVSIZEM_NOW;
}

void hwFrontExtMonitorSizeGet_tv656(
	UINT32 *pWidth,
	UINT32 *pHeight
)
{
	*pWidth= 0;
	*pHeight = 0;
}

void hwFrontSensorSizeGet_tv656(
	UINT32 *pwidth,
	UINT32 *pheight
)
{
	*pwidth = SENSOR_WIDTH;
	*pheight = SENSOR_HEIGHT;
}


void hwFrontSensorTypeGet_tv656(
	UINT32 *pType
)
{
	*pType = SENSOR_CMOS_TV656_TYPE;
}

