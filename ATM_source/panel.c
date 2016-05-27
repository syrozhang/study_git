/**************************************************************************
 *		   Copyright(c)	2003 by	Sunplus	Technology Co.,	Ltd.			 *
 *																		  *
 *	This software is copyrighted by	and	is the property	of Sunplus		  *
 *	Technology Co.,	Ltd. All rights	are	reserved by	Sunplus	Technology	  *
 *	Co., Ltd. This software	may	only be	used in	accordance with	the		  *
 *	corresponding license agreement. Any unauthorized use, duplication,	  *
 *	distribution, or disclosure	of this	software is	expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T"	not	be removed or modified without	  *
 *	prior written consent of Sunplus Technology	Co., Ltd.				  *
 *																		  *
 *	Sunplus	Technology Co.,	Ltd. reserves the right	to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	Sunplus	Technology Co.,	Ltd.										  *
 *	19,	Innovation First Road, Science-Based Industrial	Park,			  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *	Author:	Ted	Chiu													  *
 **************************************************************************/
//#define RS_232_DBG

#include	<stdio.h>
#include	<string.h>
#include	"general.h"
#include	"os_api.h"
#include	"sysg_api.h"
#include	"hw_disp_ext.h"
#include	"osd_common.h"
#include	"osd_api.h"
#include	"pub_api.h"
#include	"dsc_init.h"
#include	"panel.h"
#include	"hw_disp.h"
#include	"zoom.h"
#include	"gpio.h"
#include	"xiaoao.h"

#include 	"sysregs.h"

//#define LCD_EN_2_4		0  // 全部为 0 为高清
//#define LCD_EN_TD024	0   // 0 是2.5寸 ，1 是同保

#if GAOQING_A036
#define LCD_EN_2_4		0  // 全部为 0 为高清
#define LCD_EN_TD024	0   // 0 是2.5寸 ，1 是同保
#endif

#if TOPPOLY_A017
#define LCD_EN_2_4		0  // 全部为 0 为高清
#define LCD_EN_TD024	1   // 0 是2.5寸 ，1 是同保
#endif

#if AU_A024
#define LCD_EN_2_4		1  // 全部为 0 为高清
#define LCD_EN_TD024	0   // 0 是2.5寸 ，1 是同保
#endif

#if AU_A025
#define  LCD_EN_2_4		1  // 全部为 0 为高清
#define LCD_EN_TD024	0   // 0 是2.5寸 ，1 是同保
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void	sysMemInit();
void	hwDispPnlFlipVer(UINT32);
void	hwDispRegUpd(UINT32);
void	hwPipAddrSet(UINT32);
void	hwOsdAddrSet(UINT32);
void	hwTgGpioCfgSet(UINT32, UINT32, UINT32, UINT32);
void	osdMenu(void);
void	An02PowerCtrl(UINT32 power);	/* 1=ON, 0=OFF */

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 /*andrew_gu@2005/May/09, 11:28 (*/
//You must NOT use this variable directly!!
 UINT32 currentPanel = 0;
/*andrew_gu@2005/May/09, 11:28 )*/

UINT32	gOSD_A, gOSD_B;
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
UINT8	gOSDBuffFlag=0;
UINT8	gOSDBuffCnt=0;

#if 1	//Paul@2005/09/29
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
COLORREF DefaultRGB[16] = {	0x000000, 0x0000FF,	0x0080ff, 0x00ffff,
							0x00ff00, 0x008000,	0x008080, 0x800000,
							0xff0000, 0xff0080,	0x800080, 0xff00ff,
							0xffff00, 0xffffff,	0x808080, 0x000000 };
#else
COLORREF DefaultRGB[16] = { 0xFFFFFF, 0x9F9F9F, 0x464646, 0xA0A0A0,
							0x000000, 0x00FF00, 0x0046FF, 0x00FFFF,
							0xFF0000, 0x282828, 0x6E6E6E, 0xC8C8C8,
							0xFE0000, 0xFFFF00, 0x820F6E, 0xFEFEFE};

#endif 

#define	PANEL_DEBUG	0

/*#define	PIP_W	(16+8)
#define	PIP_H	(120)*/
void	DispSerialCtrl(UINT32 InputData, UINT32 BitLength, UINT32 LowBitFirst);
extern UINT32 ChgPvSetting;
extern UINT32	 gPB;

extern UINT32 ui32SavePanel;
/**************************************************************************
 *                                                                        *
 *  Function Name: InitLcdGpio                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *      Initial TFT_LCD 1.7 or 2.0 register                               *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 **************************************************************************/

/* LCD ADDR OF REG */
#define LCD_REG_SYSTEM			0x00
#define LCD_REG_DATA			0x20
#define LCD_REG_DRIVER			0x40
#define LCD_REG_TIME			0x60
#define LCD_REG_VCAC			0xC0

/* LCD VALUE OF REG */
#define LCD_VAL_SYSTEM			0x0F
#define LCD_VAL_DATA			0x00
#define LCD_VAL_DRIVER			0x03
#define LCD_VAL_TIME			0x00
#define LCD_VAL_VCAC			0x06
#define LCD_VAL_CLOSE			0x0D

/**
**/
#if (LCD_EN_TD024 == 0)
/**
**/
void delay10us(void)
{
    UINT16 i;
	
    i = 0;
    while (i < 0x0F)
    {
        i++;
    }
}

/**
**/
void WriteLcdIntReg(UINT8 ui8RegAddr, UINT8 ui8Data)
{
	UINT8 i;
	
	gpioConfig(LCD_SCL, 1);
	gpioConfig(LCD_CS, 1);
	gpioConfig(LCD_SDA, 1);
	
	gpioSet(LCD_SCL, 1);			
	gpioSet(LCD_CS, 0);	
	delay10us();        			
	/* Send regAddr */
	for (i=0; i<8; i++)
	{
		gpioSet(LCD_SCL, 0);		
		if (ui8RegAddr & 0x80)
		{
			gpioSet(LCD_SDA, 1);
		}
		else
		{
			gpioSet(LCD_SDA, 0);			
		}

		ui8RegAddr <<= 1;
		gpioSet(LCD_SCL, 1);				
		delay10us();        		
	}
	
	/* Send regData */	
	for (i=0; i<8; i++)
	{
		gpioSet(LCD_SCL, 0);
		if (ui8Data & 0x80)
		{
			gpioSet(LCD_SDA, 1);			
		}
		else
		{
			gpioSet(LCD_SDA, 0);						
		}
		
		ui8Data <<= 1;
		gpioSet(LCD_SCL, 1);
	    delay10us();        		
		
	}
	
	gpioSet(LCD_CS, 1);	
	delay10us();

	return;
}

/* 
*/ 
void WriteLcdReg(UINT8 ui8RegAddr, UINT8 ui8Data)
{
	UINT8 i;
#if LCD_EN_2_4
	gpioConfig(LCD_SCL, 1);
	gpioConfig(LCD_CS, 1);
	gpioConfig(LCD_SDA, 1);
	
	gpioSet(LCD_SCL, 1);			
	gpioSet(LCD_CS, 0);	
	delay10us();        			
	/* Send regAddr */
	for (i=0; i<8; i++)
	{
		gpioSet(LCD_SCL, 0);		
		if (ui8RegAddr & 0x80)
		{
			gpioSet(LCD_SDA, 1);
		}
		else
		{
			gpioSet(LCD_SDA, 0);			
		}

		ui8RegAddr <<= 1;
		gpioSet(LCD_SCL, 1);				
		delay10us();        		
	}
	
	/* Send regData */	
	for (i=0; i<8; i++)
	{
		gpioSet(LCD_SCL, 0);
		if (ui8Data & 0x80)
		{
			gpioSet(LCD_SDA, 1);			
		}
		else
		{
			gpioSet(LCD_SDA, 0);						
		}
		
		ui8Data <<= 1;
		gpioSet(LCD_SCL, 1);
	    delay10us();        		
		
	}

	gpioSet(LCD_CS, 1);	
	delay10us();
#endif

	return;
}

void InitLcdGpio(void)
{

	WriteLcdReg(LCD_REG_SYSTEM, LCD_VAL_SYSTEM);
	WriteLcdReg(LCD_REG_DRIVER, LCD_VAL_DRIVER);
	WriteLcdReg(LCD_REG_TIME, LCD_VAL_TIME);
	WriteLcdReg(LCD_REG_VCAC, LCD_VAL_VCAC);	
}

/*
	flag = 0, Close lcd 
	flag = 1, Open lcd
*/
void OpenLcdDisplay(UINT32 flag)
{
	#if (PANEL_TYPE_1 == A024CN02_UPS051)	
	if (flag)
	{
		WriteLcdReg(LCD_REG_SYSTEM, LCD_VAL_SYSTEM);
		WriteLcdReg(LCD_REG_VCAC, LCD_VAL_VCAC);
	}
	else
	{
		WriteLcdReg(LCD_REG_SYSTEM, LCD_VAL_CLOSE);		
	}		
	#elif (PANEL_TYPE_1 == A025DL01_UPS051)
	if (flag)
	{
		WriteLcdIntReg(0x05, 0x5F);
	}	
	else
	{
		WriteLcdIntReg(0x05, 0x5D);		
	}
	#else
	/* Normal Lcd Control */
	if (flag)
	{		
		gpioSet(GPIO_LCD_SHDB, 1);
	}
	else
	{
		gpioSet(GPIO_LCD_SHDB, 0);		
	}	
	#endif 
}

/**
**/
void LcdInit(void)
{
}

void SetTDLcdParameter()
{
}

#else
/**
**/
#define LCD_CMD_WRITE_MASK			0x01
#define LCD_CMD_READ_MASK			0x03

/**
**/
void WriteLcdReg(UINT8 ui8RegAddr, UINT8 ui8Data)
{
	UINT8 i;

	/*
		Bit[2:7] = (Reg Addr);
		bit[1] = 0 (Write flag);
		bit[0] = 1 (Hiz).
	*/
	ui8RegAddr = (UINT8)((ui8RegAddr<<2) | LCD_CMD_WRITE_MASK);

	gpioSet(LCD_CS, 0);			
	hwWait(1, 10);
	/* Send regAddr */
	for (i=0; i<8; i++)
	{
		gpioSet(LCD_SCL, 0);		
		if (ui8RegAddr & 0x80)
		{
			gpioSet(LCD_SDA, 1);
		}
		else
		{
			gpioSet(LCD_SDA, 0);			
		}
		hwWait(1, 1);
		ui8RegAddr <<= 1;
		gpioSet(LCD_SCL, 1);				
		hwWait(1, 7);
	}
	
	/* Send regData */	
	for (i=0; i<8; i++)
	{
		gpioSet(LCD_SCL, 0);
		if (ui8Data & 0x80)
		{
			gpioSet(LCD_SDA, 1);			
		}
		else
		{
			gpioSet(LCD_SDA, 0);						
		}
		hwWait(1, 1);		
		ui8Data <<= 1;
		gpioSet(LCD_SCL, 1);
		hwWait(1, 7);
	}

	gpioSet(LCD_SCL, 0);			
	gpioSet(LCD_SDA, 0);		
	gpioSet(LCD_CS, 1);	
	hwWait(1, 10);

	return;
}

/**
**/
void LcdInit(void)
{
	
	printf("TD024 Lcd Init\n");
	gpioConfig(LCD_SCL, 1);
	gpioConfig(LCD_CS, 1);
	gpioConfig(LCD_SDA, 1);	

	gpioSet(LCD_SCL, 0);			
	gpioSet(LCD_SDA, 0);	
	gpioSet(LCD_CS, 1);		
	
	WriteLcdReg(0x04, 0x0B);	/* Close screen */	
	WriteLcdReg(0x02, 0x62);	/* UPS051 */	
	WriteLcdReg(0x06, 0x1A);	/* Move right 2 cols */		
	WriteLcdReg(0x07, 0x0C);	/* Move down 2 Lines */	
	WriteLcdReg(0x09, 0x24);	
	WriteLcdReg(0x0c, 0x08);		

	#if 1/*fqdao_add for bug 06.7.5*/
//	WriteLcdReg(0x09, 0x26);	
	WriteLcdReg(0x10, 0x3a);	
	WriteLcdReg(0x11, 0x3f);
	#endif
	
	WriteLcdReg(0x14, 0x40);	/* GAMMA correction */	
	WriteLcdReg(0x15, 0x55);	
	WriteLcdReg(0x16, 0x55);	
	WriteLcdReg(0x17, 0x35);	
	WriteLcdReg(0x18, 0x88);				
	/** Paul@2006/03/25 add start **/	
}

/**
**/
void SetTDLcdParameter()
{
	dispReg_t* pDispReg;

	if (ui32SavePanel == PANEL_TYPE_1)
	{
		hwDispPnlVhSet(659, 262, 95, 13, 577, 256);		
		//hwDispPnlVhSet(820, 262, 117, 13, 757, 256);		
		pDispReg = (dispReg_t*)(0xb000A000);
		printf("Rgb = %d\n", pDispReg->oddRgb);
		pDispReg->oddRgb = 1;
		/** Paul@2006/03/25 add end **/			
		printf("Rgb = %d\n", pDispReg->oddRgb);	
	}
}
/**
**/
void OpenLcdDisplay(UINT32 flag)
{
#if 1	
	if (flag)
	{
		WriteLcdReg(0x04, 0x0F);	/* Open Screen */					}
	else
	{
		WriteLcdReg(0x04, 0x0B);	/* Close screen */	
	}
#endif	
}
#endif

/**************************************************************************
 *                                                                        *
 *  Function Name: panelSelect                                            *
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
extern  UINT8	ui8GameFlag ;
UINT32  ui32SavePanel = 0 ;

#if 1
void panelMainSelect(UINT32 panel)
{	
	UINT32	flag;
	UINT32	err1, err2, err3;
#if	PANEL_DEBUG
	struct	tagPvSetValue	PvSetValue;
	struct	tagPipSetValue	PipSetValue;
	struct	tagOsdSetValue	OsdSetValue;
#endif

#if SUPPORT_PANEL_TOPPOLY	/* callback function for TOPPOLY panel */
	ToppolyLc15004WriteFunSet(LC15004_write);
#endif

#if	SUPPORT_AN02_POWER_CTRL	/* power off LCD panel? */
	An02PowerCtrl(0);
#endif

#if 0
	gpioConfig(GPIO_LCD_POWER, 1);
	gpioSet(GPIO_LCD_POWER, 0);
#endif

	sysgLCDTypeSet(panel);/* See device.h for panel defines */
	#if LCD_EN_2_4
	if (panel == A024CN02_UPS051)
	{
//		hwDispPnlVhSet(617, 262, 101, 16, 586, 251);
		hwDispPnlVhSet(617, 262, 101, 14, 586, 249);
	}
	#endif		
		
#if 1
	if(panel == PANEL_TYPE_2)
	{
	//	WRITE8(0xb000A021, 0x60);	//saturation
		WRITE8(0xb000A021, 0x70);	//tzw add for bug 15 060420

		WRITE8(0xb000A022, 0x00);		
	}
	else if(panel == PANEL_TYPE_3)
	{
		WRITE8(0xb000A021, 0x32);
		WRITE8(0xb000A022, 0x22);	 
	}
#endif
	
#if	DUAL_PANEL
	ui32SavePanel = panel;
	if(panel==PANEL_TYPE_1)
	{
		SetTDLcdParameter();
		
		/*(Gu@ 2004/06/30, 11:18 color correct*/
		if(panel ==A025CN03_UPS051)
			WRITE8(0xb000a024, 0x0f);
		/*)Gu@ 2004/06/30, 11:18 */

		/* Parameters: OffsetX/OffsetY, ImgW/H, PanelW/H */
		err1 = hwDispPvSet( PANEL_1_OFF_X, PANEL_1_OFF_Y, 320, 240, PANEL_1_MAX_X, PANEL_1_MAX_Y);	/* x0/y0, ImgW/H, PanelW/H */
		if(ui8GameFlag)		//for bug 47
		{
			#if (PANEL_TYPE_1==A036QN01_UPS051)
			err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, 480, PANEL_1_OSD_H, 960, PANEL_1_OSD_H);
			#else
			err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, 480, PANEL_1_OSD_H + 6, PANEL_1_MAX_X/*PANEL_1_OSD_W*/, PANEL_1_OSD_H);
			#endif			
		}
		else
		{
			#if (PANEL_TYPE_1==A036QN01_UPS051)
			err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PANEL_1_OSD_W, PANEL_1_OSD_H, 960, PANEL_1_OSD_H);
			#else
			err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_1_MAX_X/*PANEL_1_OSD_W*/, PANEL_1_OSD_H);
			#endif
		}
		err3 = hwDispPipSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PIP_W, PIP_H, PANEL_1_PIP_W, PANEL_1_PIP_H);
	}
	else if(panel==PANEL_TYPE_2)
	{	/* Parameters: OffsetX/OffsetY, ImgW/H, PanelW/H */
		//John 051102	set the luminosity for TV OUT
		err1 = hwDispPvSet( PANEL_2_OFF_X-4, PANEL_2_OFF_Y-10, 320, 240, PANEL_2_MAX_X + 30, PANEL_2_MAX_Y+30);
		if(ui8GameFlag) 			//for bug 47  John 11082005
		{
			#if (PANEL_TYPE_1==A036QN01_UPS051)
			{
				err2 = hwDispOsdSet(PANEL_2_OFF_X, PANEL_2_OFF_Y-10, (480&0xfff0), PANEL_1_OSD_H, PANEL_2_OSD_W+30, PANEL_2_OSD_H+30);
			}	
			#else
			{
				err2 = hwDispOsdSet(PANEL_2_OFF_X, PANEL_2_OFF_Y-10, (480&0xfff0), PANEL_1_OSD_H+6, PANEL_2_OSD_W+30, PANEL_2_OSD_H+30);
			}
			#endif
		}
		else
		{
			err2 = hwDispOsdSet(PANEL_2_OFF_X , PANEL_2_OFF_Y-10, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_2_OSD_W+22, PANEL_2_OSD_H+30);
		}
		err3 = hwDispPipSet(PANEL_2_OFF_X-4 , PANEL_2_OFF_Y-10, PIP_W, PIP_H, PANEL_2_PIP_W+30, PANEL_2_PIP_H+30);
	}
	else if(panel==PANEL_TYPE_3)
	{	/* Parameters: OffsetX/OffsetY, ImgW/H, PanelW/H */
		err1 = hwDispPvSet( PANEL_3_OFF_X-4, PANEL_3_OFF_Y - 10, 320, 240, PANEL_3_MAX_X + 14, PANEL_3_MAX_Y + 32);
		/* Paul@2006/12/27 - 09:47 add start */
		#if GAOQING_A036
			if(ui8GameFlag)
			{
				#if (PANEL_TYPE_1==A036QN01_UPS051)
				{		
					err2 = hwDispOsdSet(PANEL_3_OFF_X+4 , PANEL_3_OFF_Y - 10, (480&0xfff0), PANEL_1_OSD_H, PANEL_3_OSD_W + 14, PANEL_3_OSD_H + 32);
				}
				#else
				{
					err2 = hwDispOsdSet(PANEL_3_OFF_X+4 , PANEL_3_OFF_Y - 10, (480&0xfff0), PANEL_1_OSD_H+ 6, PANEL_3_OSD_W + 14, PANEL_3_OSD_H + 32);
				}
				#endif
			}
			else
			{  
				err2 = hwDispOsdSet(PANEL_3_OFF_X+4, PANEL_3_OFF_Y - 12, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_3_OSD_W + 2, PANEL_3_OSD_H + 32);
			}
		#else		
			if(ui8GameFlag)
			{
				#if (PANEL_TYPE_1==A036QN01_UPS051)
				{		
					err2 = hwDispOsdSet(PANEL_3_OFF_X, PANEL_3_OFF_Y-10, (480&0xfff0), PANEL_1_OSD_H, PANEL_3_OSD_W+14, PANEL_3_OSD_H+32);
				}
				#else
				{
					err2 = hwDispOsdSet(PANEL_3_OFF_X, PANEL_3_OFF_Y-10, (480&0xfff0), PANEL_1_OSD_H+6, PANEL_3_OSD_W+14, PANEL_3_OSD_H+32);
				}
				#endif
			}
			else
			{  
				err2 = hwDispOsdSet(PANEL_3_OFF_X , PANEL_3_OFF_Y-12, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_3_OSD_W+2, PANEL_3_OSD_H+32);
			}
		#endif
		/* Paul@2006/12/27 - 09:47 add end */
		err3 = hwDispPipSet(PANEL_3_OFF_X-4, PANEL_3_OFF_Y-10, PIP_W, PIP_H, PANEL_3_PIP_W+14, PANEL_3_PIP_H+32);
	}
	#else
	/* Parameters: OffsetX/OffsetY, ImgW/H, PanelW/H */
	err1 = hwDispPvSet( PANEL_1_OFF_X, PANEL_1_OFF_Y, 320, 240, PANEL_1_MAX_X, PANEL_1_MAX_Y);	/* x0/y0, ImgW/H, PanelW/H */
	err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_1_OSD_W, PANEL_1_OSD_H);
	err3 = hwDispPipSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PIP_W, PIP_H, PIP_W, PIP_H);
#endif

	#if	PANEL_DEBUG
	if(err1)
	{
		hwDispPvSetRead((UINT8 *)&PvSetValue);
		printf("\n PV: x0=%3d y0=%3d x1=%3d y1=%3d v0=%3d h0=%3d v1=%3d h1=%3d xfac=%3d yfac=%3d \n",
			PvSetValue.pvx0, PvSetValue.pvy0, PvSetValue.pvx1, PvSetValue.pvy1,
			PvSetValue.pvh0, PvSetValue.pvv0, PvSetValue.pvh1, PvSetValue.pvv1,
			PvSetValue.pvxFac, 	PvSetValue.pvyFac);
	}
	if(err2)
	{
		hwDispOsdSetRead((UINT8 *)&OsdSetValue);
		printf("\nOSD: x0=%3d y0=%3d x1=%3d y1=%3d v0=%3d h0=%3d v1=%3d h1=%3d xfac=%3d yfac=%3d ",
			OsdSetValue.osdx0, OsdSetValue.osdy0, OsdSetValue.osdx1, OsdSetValue.osdy1,
			OsdSetValue.osdh0, OsdSetValue.osdv0, OsdSetValue.osdh1, OsdSetValue.osdv1,
			OsdSetValue.osdxFac, OsdSetValue.osdyFac);
	}
	if(err3)
	{
		hwDispPipSetRead((UINT8 *)&PipSetValue);
		printf("\nPIP: x0=%3d y0=%3d x1=%3d y1=%3d v0=%3d h0=%3d v1=%3d h1=%3d xfac=%3d yfac=%3d ",
			PipSetValue.pipx0, PipSetValue.pipy0, PipSetValue.pipx1, PipSetValue.pipy1,
			PipSetValue.piph0, PipSetValue.pipv0, PipSetValue.piph1, PipSetValue.pipv1,
			PipSetValue.pipxFac,PipSetValue.pipyFac);
	}
	#endif

/* keep zoom factor when switch panel and resize the image size from CDSP */
	zoomFactSet(zoomFactGet());

#if	PIP_ENABLE
	UINT32	pipAddr;
	/*UINT32	i=0, j=0;*/
	sysgMemPIPAddrGet(&pipAddr);
	hwPipAddrSet(pipAddr);
	#if	0
	pubFill((UINT8 *)(0xAC000000+pipAddr*2), PIP_W*2, PIP_H,
			0, 0, PIP_W*2, PIP_H, 0x000000);
	imgInit((UINT8 *)(0xAC000000+pipAddr*2), PIP_W, PIP_H, 0);
	#endif
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
	/*hwDispPipEn(1);*/
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
#endif

	/* Turn this on if you want the preview image */
	/* upside-down but keep OSD unchanged         */
	#if 0
	hwDispPnlFlipVer(1);       /* Specify to flip preview image vertically */
								/* In case you need OSD remind upright you  */
	                           /* should call osdChgMapping(OSD_FLIP_V)    */
	                           /* right after hwDispPnlFlipVer(1)          */
	#endif

/* should be executed once only ...*/
	sysgMemDispAddrGet(&gPB);
	sysgMemOSDAddrGet(&gOSD_A);	/* dual buffer for OSD */
	sysgMemOSDAddr2Get(&gOSD_B);
	hwOsdAddrSet(gOSD_A);
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
	/*gOSD_A = 0xac000000	+ gOSD_A * 2;*/
	/*osdAddrSet(gOSD_A);*/
	osdAddrSet (0xac000000 + gOSD_A * 2);
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */

/* should be call again if you want change the color sets for OSD */
	osdBlendColorSet(3,	8, 3, OSD_UPDATE_LATER);
	for(flag=1; flag<16; flag++)
	{
		if(panel==PANEL_NTSC_KOPIN)
			err1 = (DefaultRGB[flag] >> 16) | (DefaultRGB[flag] & 0x00FF00)
					| ((DefaultRGB[flag] & 0x0000FF) << 16);
		else
			err1 = DefaultRGB[flag];
		osdPaletteSet(flag, err1, OSD_UPDATE_LATER);
	}
	if(panel==PANEL_NTSC_KOPIN)
		err1 = (DefaultRGB[0] >> 16) | (DefaultRGB[0] & 0x00FF00)
				| ((DefaultRGB[0] & 0x0000FF) << 16);
	else
		err1 = DefaultRGB[0];
	osdPaletteSet(0, err1, OSD_UPDATE_NOW);

/* should be call again if OSD size was diff. */
	osdInit();

	#if	0	/* flip mode for OSD */
	osdFlipMode(OSD_FLIP_V+OSD_FLIP_H);
	#endif

#if	SUPPORT_AN02_POWER_CTRL	/* power on LCD panel? */
	if((panel==A015AN02_UPS051) || (panel==A015AN02_UPS052))
		An02PowerCtrl(1);
#endif

	if (panel > 7)
	{
		gpioSet(GPIO_LCD_POWER, 1);		
		LcdInit();
	}
	
	currentPanel = panel;
}

#endif
void panelSelect(UINT32 panel)
{
	UINT32	flag;
	UINT32	err1, err2, err3;
#if	PANEL_DEBUG
	struct	tagPvSetValue	PvSetValue;
	struct	tagPipSetValue	PipSetValue;
	struct	tagOsdSetValue	OsdSetValue;
#endif

#if SUPPORT_PANEL_TOPPOLY	/* callback function for TOPPOLY panel */
	ToppolyLc15004WriteFunSet(LC15004_write);
#endif

#if	SUPPORT_AN02_POWER_CTRL	/* power off LCD panel? */
	An02PowerCtrl(0);
#endif

#if 1
	gpioConfig(GPIO_LCD_POWER, 1);
	gpioSet(GPIO_LCD_POWER, 0);
#endif

	sysgLCDTypeSet(panel);/* See device.h for panel defines */
#if LCD_EN_2_4
	if (panel == A024CN02_UPS051)
	{
//		hwDispPnlVhSet(617, 262, 101, 16, 586, 251);
		hwDispPnlVhSet(617, 262, 101, 14, 586, 249);
	}
#endif	

#if 1
	if(panel == PANEL_TYPE_2)
	{
		//WRITE8(0xb000A021, 0x60);	//saturation
		WRITE8(0xb000A021, 0x70);	//for bug 15 tzw add 060420

		WRITE8(0xb000A022, 0x00);		
	}
	else if(panel == PANEL_TYPE_3)
	{
		WRITE8(0xb000A021, 0x32);
		WRITE8(0xb000A022, 0x22);	 
	}
#endif
	
#if	DUAL_PANEL
	ui32SavePanel = panel ;
	if(panel==PANEL_TYPE_1)
	{
		SetTDLcdParameter();
		
		/*(Gu@ 2004/06/30, 11:18 color correct*/
		if(panel ==A025CN03_UPS051)
			WRITE8(0xb000a024, 0x0f);
		/*)Gu@ 2004/06/30, 11:18 */

		/* Parameters: OffsetX/OffsetY, ImgW/H, PanelW/H */
		err1 = hwDispPvSet( PANEL_1_OFF_X, PANEL_1_OFF_Y, 320, 240, PANEL_1_MAX_X, PANEL_1_MAX_Y);	/* x0/y0, ImgW/H, PanelW/H */
		if(ui8GameFlag)		//for bug 47
			{
		#if (PANEL_TYPE_1==A036QN01_UPS051)
		err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, 480, PANEL_1_OSD_H, 960, PANEL_1_OSD_H);
		#else
		err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, 480, PANEL_1_OSD_H + 6, PANEL_1_MAX_X/*PANEL_1_OSD_W*/, PANEL_1_OSD_H);
		#endif			
			}
		else{
		#if (PANEL_TYPE_1==A036QN01_UPS051)
		err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PANEL_1_OSD_W, PANEL_1_OSD_H, 960, PANEL_1_OSD_H);
		#else
		err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_1_MAX_X/*PANEL_1_OSD_W*/, PANEL_1_OSD_H);
		#endif
			}
		err3 = hwDispPipSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PIP_W, PIP_H, PANEL_1_PIP_W, PANEL_1_PIP_H);
	}
	else if(panel==PANEL_TYPE_2)
	{	/* Parameters: OffsetX/OffsetY, ImgW/H, PanelW/H */
		//John 051102	set the luminosity for TV OUT
//		WRITE8(0xb000A021, 0x60);		//John 051102
//		WRITE8(0xb000A022, 0x00);	
		err1 = hwDispPvSet( PANEL_2_OFF_X -4, PANEL_2_OFF_Y - 10, 320, 240, PANEL_2_MAX_X + 30, PANEL_2_MAX_Y + 30);
		if(ui8GameFlag) 			//for bug 47  John 11082005
		{
			#if (PANEL_TYPE_1==A036QN01_UPS051)
			{
				err2 = hwDispOsdSet(PANEL_2_OFF_X , PANEL_2_OFF_Y - 10, (480&0xfff0), PANEL_1_OSD_H, PANEL_2_OSD_W + 30, PANEL_2_OSD_H + 30);
			}	
			#else
			{
				err2 = hwDispOsdSet(PANEL_2_OFF_X, PANEL_2_OFF_Y - 10, (480&0xfff0), PANEL_1_OSD_H+ 6, PANEL_2_OSD_W  + 30, PANEL_2_OSD_H + 30);
			}
			#endif
		}
		else{
			err2 = hwDispOsdSet(PANEL_2_OFF_X, PANEL_2_OFF_Y - 10, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_2_OSD_W + 22, PANEL_2_OSD_H + 30);
		}
		err3 = hwDispPipSet(PANEL_2_OFF_X -4 , PANEL_2_OFF_Y -10, PIP_W, PIP_H, PANEL_2_PIP_W + 30, PANEL_2_PIP_H + 30);
	}
	else if(panel==PANEL_TYPE_3)
	{	/* Parameters: OffsetX/OffsetY, ImgW/H, PanelW/H */
		//John 051102	set the luminosity for TV OUT	
//		WRITE8(0xb000A021, 0x32);
//		WRITE8(0xb000A022, 0x22);	 
		err1 = hwDispPvSet( PANEL_3_OFF_X - 4, PANEL_3_OFF_Y - 10, 320, 240, PANEL_3_MAX_X + 14, PANEL_3_MAX_Y + 32);
		#if GAOQING_A036
			if(ui8GameFlag)
			{
				#if (PANEL_TYPE_1==A036QN01_UPS051)
				{		
					err2 = hwDispOsdSet(PANEL_3_OFF_X+6 , PANEL_3_OFF_Y - 12, (480&0xfff0), PANEL_1_OSD_H, PANEL_3_OSD_W + 14, PANEL_3_OSD_H + 32);
				}
				#else
				{
					err2 = hwDispOsdSet(PANEL_3_OFF_X+6 , PANEL_3_OFF_Y - 12, (480&0xfff0), PANEL_1_OSD_H+ 6, PANEL_3_OSD_W + 14, PANEL_3_OSD_H + 32);
				}
				#endif
			}
			else
			{  
				err2 = hwDispOsdSet(PANEL_3_OFF_X+6 , PANEL_3_OFF_Y - 12, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_3_OSD_W + 2, PANEL_3_OSD_H + 32);
			}
		#else		
			if(ui8GameFlag)
			{
				#if (PANEL_TYPE_1==A036QN01_UPS051)
				{		
					err2 = hwDispOsdSet(PANEL_3_OFF_X, PANEL_3_OFF_Y-10, (480&0xfff0), PANEL_1_OSD_H, PANEL_3_OSD_W+14, PANEL_3_OSD_H+32);
				}
				#else
				{
					err2 = hwDispOsdSet(PANEL_3_OFF_X, PANEL_3_OFF_Y-10, (480&0xfff0), PANEL_1_OSD_H+6, PANEL_3_OSD_W+14, PANEL_3_OSD_H+32);
				}
				#endif
			}
			else
			{  
				err2 = hwDispOsdSet(PANEL_3_OFF_X , PANEL_3_OFF_Y-12, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_3_OSD_W+2, PANEL_3_OSD_H+32);
			}
		#endif
		/* Paul@2006/12/27 - 09:47 add end */
		err3 = hwDispPipSet(PANEL_3_OFF_X - 4, PANEL_3_OFF_Y - 10, PIP_W, PIP_H, PANEL_3_PIP_W + 14, PANEL_3_PIP_H + 32);
	}
	#else
	/* Parameters: OffsetX/OffsetY, ImgW/H, PanelW/H */
	err1 = hwDispPvSet( PANEL_1_OFF_X, PANEL_1_OFF_Y, 320, 240, PANEL_1_MAX_X, PANEL_1_MAX_Y);	/* x0/y0, ImgW/H, PanelW/H */
	err2 = hwDispOsdSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PANEL_1_OSD_W, PANEL_1_OSD_H, PANEL_1_OSD_W, PANEL_1_OSD_H);
	err3 = hwDispPipSet(PANEL_1_OFF_X, PANEL_1_OFF_Y, PIP_W, PIP_H, PIP_W, PIP_H);
#endif

	#if	PANEL_DEBUG
	if(err1)
	{
		hwDispPvSetRead((UINT8 *)&PvSetValue);
		printf("\n PV: x0=%3d y0=%3d x1=%3d y1=%3d v0=%3d h0=%3d v1=%3d h1=%3d xfac=%3d yfac=%3d \n",
			PvSetValue.pvx0, PvSetValue.pvy0, PvSetValue.pvx1, PvSetValue.pvy1,
			PvSetValue.pvh0, PvSetValue.pvv0, PvSetValue.pvh1, PvSetValue.pvv1,
			PvSetValue.pvxFac, 	PvSetValue.pvyFac);
	}
	if(err2)
	{
		hwDispOsdSetRead((UINT8 *)&OsdSetValue);
		printf("\nOSD: x0=%3d y0=%3d x1=%3d y1=%3d v0=%3d h0=%3d v1=%3d h1=%3d xfac=%3d yfac=%3d ",
			OsdSetValue.osdx0, OsdSetValue.osdy0, OsdSetValue.osdx1, OsdSetValue.osdy1,
			OsdSetValue.osdh0, OsdSetValue.osdv0, OsdSetValue.osdh1, OsdSetValue.osdv1,
			OsdSetValue.osdxFac, OsdSetValue.osdyFac);
	}
	if(err3)
	{
		hwDispPipSetRead((UINT8 *)&PipSetValue);
		printf("\nPIP: x0=%3d y0=%3d x1=%3d y1=%3d v0=%3d h0=%3d v1=%3d h1=%3d xfac=%3d yfac=%3d ",
			PipSetValue.pipx0, PipSetValue.pipy0, PipSetValue.pipx1, PipSetValue.pipy1,
			PipSetValue.piph0, PipSetValue.pipv0, PipSetValue.piph1, PipSetValue.pipv1,
			PipSetValue.pipxFac,PipSetValue.pipyFac);
	}
	#endif

/* keep zoom factor when switch panel and resize the image size from CDSP */
	zoomFactSet(zoomFactGet());

#if	PIP_ENABLE
	UINT32	pipAddr;
	/*UINT32	i=0, j=0;*/
	sysgMemPIPAddrGet(&pipAddr);
	hwPipAddrSet(pipAddr);
	#if	0
	pubFill((UINT8 *)(0xAC000000+pipAddr*2), PIP_W*2, PIP_H,
			0, 0, PIP_W*2, PIP_H, 0x000000);
	imgInit((UINT8 *)(0xAC000000+pipAddr*2), PIP_W, PIP_H, 0);
	#endif
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
	/*hwDispPipEn(1);*/
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
#endif

	/* Turn this on if you want the preview image */
	/* upside-down but keep OSD unchanged         */
	#if 0
	hwDispPnlFlipVer(1);       /* Specify to flip preview image vertically */
								/* In case you need OSD remind upright you  */
	                           /* should call osdChgMapping(OSD_FLIP_V)    */
	                           /* right after hwDispPnlFlipVer(1)          */
	#endif

/* should be executed once only ...*/
	sysgMemDispAddrGet(&gPB);
	sysgMemOSDAddrGet(&gOSD_A);	/* dual buffer for OSD */
	sysgMemOSDAddr2Get(&gOSD_B);
      #if 1  //  fqdao_modify for bug TVOut osdcleanScreen  @  06.5.6 
	OsdHWAddrSet();
	OsdFWAddrSet();
	#else //  fqdao_modify for bug TVOut osdcleanScreen  @  06.5.6 
      hwOsdAddrSet(gOSD_A);
	#endif
	/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
	/*gOSD_A = 0xac000000	+ gOSD_A * 2;*/
	/*osdAddrSet(gOSD_A);*/
	#if 0  //  fqdao_modify for bug TVOut osdcleanScreen  @  06.5.6 
      osdAddrSet (0xac000000 + gOSD_A * 2);
      #endif
	  
/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
/* should be call again if you want change the color sets for OSD */
	osdBlendColorSet(3,	8, 3, OSD_UPDATE_LATER);
	for(flag=1; flag<16; flag++)
	{
		if(panel==PANEL_NTSC_KOPIN)
			err1 = (DefaultRGB[flag] >> 16) | (DefaultRGB[flag] & 0x00FF00)
					| ((DefaultRGB[flag] & 0x0000FF) << 16);
		else
			err1 = DefaultRGB[flag];
		osdPaletteSet(flag, err1, OSD_UPDATE_LATER);
	}
	if(panel==PANEL_NTSC_KOPIN)
		err1 = (DefaultRGB[0] >> 16) | (DefaultRGB[0] & 0x00FF00)
				| ((DefaultRGB[0] & 0x0000FF) << 16);
	else
		err1 = DefaultRGB[0];
	osdPaletteSet(0, err1, OSD_UPDATE_NOW);

/* should be call again if OSD size was diff. */
	osdInit();

	#if	0	/* flip mode for OSD */
	osdFlipMode(OSD_FLIP_V+OSD_FLIP_H);
	#endif

#if	SUPPORT_AN02_POWER_CTRL	/* power on LCD panel? */
	if((panel==A015AN02_UPS051) || (panel==A015AN02_UPS052))
		An02PowerCtrl(1);
#endif
	if (panel > 7)
	{
		gpioSet(GPIO_LCD_POWER, 1);
		LcdInit();
		OpenLcdDisplay(1);
	}
	currentPanel = panel;
}

#if	SUPPORT_AN02_POWER_CTRL
void An02PowerCtrl(UINT32 power)	/* 1=ON, 0=OFF */
{
	WRITE8(0xb000a0a6, 0x38);
	WRITE8(0xb000a0a3, 0x00);
	if(power)
	{
		WRITE8(0xb000a0a3, 0x10);	/* standby */
		WRITE8(0xb000a0a3, 0x18);	/* reset */
		WRITE8(0xb000a0a3, 0x38);	/* shut down */
	}
}
#endif

/*andrew_gu@2005/May/09, 11:28 (*/
UINT32 getCurPanel(void)
{
	return currentPanel;
}
/*andrew_gu@2005/May/09, 11:28 )*/

/**
	Paul@2006/04/07-21:53 add 
**/
void PNL_PlaybackSet(void)
{
	playbackSet();
	SetTDLcdParameter();
	if (currentPanel == PANEL_TYPE_1)
	{
//		WriteLcdReg(0x09, 0x26);			
		WriteLcdReg(0x10, 0x3a);	
		WriteLcdReg(0x11, 0x3f);	
	}
}

/**
**/
void PNL_PreviewSet(void)
{
	previewSet();
	SetTDLcdParameter();	
	if (currentPanel == PANEL_TYPE_1)
	{
//		WriteLcdReg(0x09, 0x26);			
		WriteLcdReg(0x10, 0x3a);	
		WriteLcdReg(0x11, 0x3f);	
	}
}

void PanelColorSet()
{
#if 0	
	panelSelect(currentPanel);
#else
	if(currentPanel == PANEL_TYPE_2)
	{
		//WRITE8(0xb000A021, 0x60);	//saturation
		WRITE8(0xb000A021, 0x70);	//for bug 15 tzw add 060420
		WRITE8(0xb000A022, 0x00);		
	}
	else if(currentPanel == PANEL_TYPE_3)
	{
		WRITE8(0xb000A021, 0x32);
		WRITE8(0xb000A022, 0x22);	 
	}
#endif
}


