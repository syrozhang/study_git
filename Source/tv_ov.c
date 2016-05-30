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
 *  Author: Jinch/WorldPlus                                               *
 *                                                                        *
 **************************************************************************/
#include "general.h"
#include "hw_front.h"
#include "tv_ov.h"
#include "aaa_api.h"
#include "stdio.h"
#include "hw_gpio.h"
#include "dsc_init.h"
#include "hw_cpu.h"
#include "hw_sdram.h"
#include "hw_cdsp.h"
#include "tv656.h"
#include "os_api.h"
#include "xiaoao.h"
#include "hw_disp_ext.h"
#include "ui_main.h"
#include "panel.h"
#include "gpio.h"

UINT32 ImgSrc;
extern void panelSelect(UINT32 panel);

UINT32 hwFrontInit(UINT32 cammode)
{
    UINT32 ret_value;
    if (ImgSrc == 0)
    {
//        ret_value = hwFrontInit_ov9620(cammode);
    }
    else
    {
        ret_value = hwFrontInit_tv656(cammode);
    }
    
    return ret_value;
}

void hwFrontSnap(UINT32 snapnum, void *prawAddrFunc, void *pbadPixFunc)
{
    if (ImgSrc == 0)
        hwFrontSnap_ov9620(snapnum,prawAddrFunc,pbadPixFunc);
    else
        hwFrontSnap_tv656(snapnum,prawAddrFunc,pbadPixFunc);
}

void hwFrontPreviewSet(void)
{
    if (ImgSrc == 0)
        hwFrontPreviewSet_ov9620();
    else
        hwFrontPreviewSet_tv656();
}

void hwFrontFullSet(UINT32 field)
{
    if (ImgSrc == 0)
        hwFrontFullSet_ov9620(field);
    else
        hwFrontFullSet_tv656(field);
}

UINT32 hwFrontGainSet(UINT32 value, UINT32 option)
{
    UINT32 ret_value;
    
    if (ImgSrc == 0)
        ret_value = hwFrontGainSet_ov9620(value,option);
    else
        ret_value = hwFrontGainSet_tv656(value,option);
        
    return ret_value;
}

UINT32 hwFrontExpTimeSet(UINT32 value, UINT32 option)
{
    UINT32 ret_value;
	
    if (ImgSrc == 0)
        ret_value = hwFrontExpTimeSet_ov9620(value,option);
    else
        ret_value = hwFrontExpTimeSet_tv656(value,option);
        
    return ret_value;
}

UINT32 hwFrontExpTimeGainSet(UINT32 value, UINT32 value1, UINT32 option)
{
    UINT32 ret_value;
	
    if (ImgSrc == 0)
        ret_value = hwFrontExpTimeGainSet_ov9620(value,value1,option);
    else
        ret_value = hwFrontExpTimeGainSet_tv656(value,value1,option);

    return ret_value;
}

UINT32 hwFrontFrameRateSet(UINT32 value, UINT32 option)
{
    UINT32 ret_value;
	
    if (ImgSrc == 0)
        ret_value = hwFrontFrameRateSet_ov9620(value,option);
    else
        ret_value = hwFrontFrameRateSet_tv656(value,option);
	
    return ret_value;
}

void hwFrontSuspend(void)
{
    if (ImgSrc == 0)
        hwFrontSuspend_ov9620();
    else
        hwFrontSuspend_tv656();
}

void hwFrontMaxMonitorSizeGet(UINT32 *pWidth, UINT32 *pHeight)
{
    if (ImgSrc == 0)
        hwFrontMaxMonitorSizeGet_ov9620(pWidth,pHeight);
    else
        hwFrontMaxMonitorSizeGet_tv656(pWidth,pHeight);
}

void hwFrontSensorSizeGet(UINT32 *pWidth, UINT32 *pHeight)
{
    if (ImgSrc == 0)
        hwFrontSensorSizeGet_ov9620(pWidth,pHeight);
    else
        hwFrontSensorSizeGet_tv656(pWidth,pHeight);
}

void hwFrontSensorTypeGet(UINT32 *pType)
{
    if (ImgSrc == 0)
        hwFrontSensorTypeGet_ov9620(pType);
    else
        hwFrontSensorTypeGet_tv656(pType);
}

void hwTvSensorBusInit(UINT32 src)
{
    ImgSrc = 0;
    hwTgGpioCfgSet(0x00, 0x00, 0x7400, 0x7400);
    
    if (ImgSrc == 0)
        hwTgGpioMsbMaskWrite(0x2000, 0x7400);
    else
        hwTgGpioMsbMaskWrite(0x5400, 0x7400);
}

/**
**/
void hwTvSensorModeSel(UINT32 src)
{
    hwFrmCnvModeSet(0);	                /* Disable frame rate	conversion */
    hwCammodeSet(5);	                /* play back mode	*/
    hwCdspDataSource(1);                /* CDSP Data source from DRAM	*/
    hwTgGpioCfgSet(0x00, 0x00, 0x7400, 0x7400);
    
    ImgSrc = src;
    if (ImgSrc == 0)
    {
        hwTgGpioMsbMaskWrite(0x2000, 0x7400);   /* SCL out, SDA in */
        hwWait(0, 10);
        
//      hwFrontInit(0);                         /* Front Init */
	    hwFrontInit_ov9620(0);

#if 1		
        if(sPara.TvoutFlag == TVOutPAL && 
			gpioGet(GPIO_DET_EAR))
        {
            panelSelect(PANEL_TYPE_3);
        }
        else if(sPara.TvoutFlag == TVOutNTSC && 
				gpioGet(GPIO_DET_EAR))
        {
            panelSelect(PANEL_TYPE_2);
        }
        else if((sPara.TvoutFlag == TVOutOff) && (getCurPanel() != PANEL_TYPE_1))
        {
            panelSelect(PANEL_TYPE_1);
        }
#endif

        /*
        hwDispPvEn(1);
        PNL_PreviewSet();   
        */
        WRITE8(0xb0002044, 0x69);
        WRITE8(0xb0002046, 0x11);
        WRITE8(0xb0002046, 0);
        WRITE8(0xb0002047, 0);

        hwFrontGpioSel(0x3FFFFFFF, 0x750F);     /* Select use of GPIO or sensor */
        hwFrontGpioCfg(0x00000000, 0xC0000000, 0x0200, 0x8AF0); /* Clk2x output enable */
        aaaAeawbModeSet(0);                     /* ae */
    }
    else
    {
        aaaAeawbModeSet(3);
		hwTgGpioMsbMaskWrite(0x5400,0x7400);		

        hwFrontInit(0);                         /* Front Init */
        /*
        if (sPara.TvoutFlag== TVOutPAL)
            panelSelect(PANEL_TYPE_3);
        else if (sPara.TvoutFlag == TVOutNTSC)
            panelSelect(PANEL_TYPE_2);
        hwDispPvEn(1);
        PNL_PreviewSet();
        */
        hwWait(0, 300);
		#if (TVDECODER == TVP5150)
        /* tvp5150 detect color standard and initial tvp5150 */
        UINT32 state;
        state = re_byt_i2c(0xb8, 0x88);
        printf("state = 0x%x\n", state);
        state &= 0x20;
        if (state)
        {
            tvp5150Init(1);                     /* PAL-50Hz */
        }
        else
        {
            tvp5150Init(0);                     /* NTSC-60Hz */
        }
        #endif
		
        #if (TVDECODER == SAA7113)
        tvDecoderInit(1);
        #endif
    }
    /*hwTgGpioCfgSet(0x30, 0x3f, 0, 0);*/
}


