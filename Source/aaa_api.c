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
 *  Author: Jen Hung Yu                                                   *
 *                                                                        *
 **************************************************************************/
//#define RS_232_DBG

#include <stdio.h>
#include <string.h>
#include "general.h"
#include "hw_cdsp.h"
#include "aaa_api.h"
#include "hw_front.h"
#include "device.h"
#include "aaa.h"
#include "exif_api.h"
#include "ui_main.h"
#include "os_api.h"
#include "xiaoao.h"

/*herb add ISO @03/12/09 start*/
#include "menu_capture.h"
/*herb add ISO @03/12/09 end*/
#define FrameExposure 1
#define Snap48MHz 0

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 ManualWbFlag;
extern UINT32 aaaEvCmpIdx;
extern UINT32 SharpnessFlag;
extern UINT32 Shutter,Gain,Tidx,PreTidx;
extern AEstr LVtbl60[],LVtbl50[];
extern UINT32 aaaAeFlag,aaaAwbFlag;
extern UINT32 FrameRateFlag;
/*herb add ISO gain @03/12/09 start*/
UINT32 ISO50_Gain=0x03, ISO100_Gain=0x13, ISO200_Gain=0x33;
/*herb add ISO gain @03/12/09 end*/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
AEstr *G_LVtbl;
WBstr outdoorWB; /*outdoor fixed WB*/
UINT32 Ylayer,Ylevel,Yreff;

UINT32 aaaStdLumi;
UINT32 AeawbMode;
UINT32 LightFreq; /* 0:50Hz, 1:60Hz */

UINT32 Max_Tidx,Min_Tidx,Ini_Tidx;
UINT32 Ev7_Index,Ev9_Index,Ev10_Index,Ev11_Index,Ev12_Index;
UINT32 VideoClipMode;
UINT32 MaxFrameRate;

UINT32 EvCmp[11]={282,  230,  187,  152,  123, 100  , 81,   66,    53,    44,     35};
/*                1.5   1.2   0.9   0.6    0.3   0   -0.3  -0.6  -0.9    -1.2   -1.5 */

UINT8 buf[50];
UINT32 AeawbStatus;

UINT32 G_AvgLumi;
/**************************************************************************
 *                                                                        *
 *  Function Name: aaaTableChange                                         *
 *                                                                        *
 *  Purposes:change AE table for 50/60Hz light source                     *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: idx                                                        *
 *             0:50Hz, 1:60Hz                                             *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
aaaTableChange(
	UINT8 idx
)
{
	if ( idx ) {
		G_LVtbl = LVtbl60;
		LightFreq = 1;
	}
	else {
		G_LVtbl = LVtbl50;
		LightFreq = 0;
    	}
	buf[7] = (UINT8)LightFreq;
}

/**
**/
void
aeTableChange(
	UINT8 idx
)
{
//	Tidx = Min_Tidx+10;
	switch(sPara.frequenceFlag+1)
	{
		case 1: aaaTableChange(0);
			break;	
		case 2:	aaaTableChange(1);
			break;
		default:aaaTableChange(1);
			break;
	}
}
/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAeShutterGet                                        *
 *                                                                        *
 *  Purposes: Get shutter value of exposure index                         *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: idx, exposure index                                        *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
aaaAeShutterGet(
	UINT16 idx
)
{
	return G_LVtbl[idx].shutter;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAeGainGet                                           *
 *                                                                        *
 *  Purposes: Get exposure gain of exposure index                         *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: idx, exposure index                                        *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
aaaAeGainGet(
	UINT16 idx
)
{
	return G_LVtbl[idx].gain;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAeEvGet                                             *
 *                                                                        *
 *  Purposes: Get EV of exposure index                                    *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: idx, exposure index                                        *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
aaaAeEvGet(
	UINT16 idx
)
{
	return G_LVtbl[idx].ev;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAeIrisGet                                           *
 *                                                                        *
 *  Purposes: Get Iris of exposure index                                  *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: idx, exposure index                                        *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
aaaAeIrisGet(
	UINT16 idx
)
{
	return G_LVtbl[idx].iris;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAvgLumiGet                                          *
 *                                                                        *
 *  Purposes: Get luminance of RGB window by algorithm                    *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: AvgLumi, pointer to luminance result                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
aaaAvgLumiGet(
	UINT32 *AvgLumi
)
{
	UINT32 pRval[1],pGval[1],pBval[1],pPixcnt[1];
	UINT32 Y[8][8],YPixCnt[8][8];
	UINT32 Row,Col;
	UINT32 StdLumi;



	for ( Row = 0; Row < 8; Row++ )  {
		for ( Col = 0; Col < 8; Col++ ) {
			hwCdspRGBWinRead((Row * 8 + Col), 0, pRval, pGval, pBval, pPixcnt);
			Y[Row][Col] = (299 * pRval[0] + 587 * pGval[0] + 114 * pBval[0])/1000;
			YPixCnt[Row][Col] = pPixcnt[0];

		}
	}

	/*   Y00  Y01  Y02  Y03	 Y04  Y05  Y06  Y07
	 *   Y10  Y11  Y12  Y13  Y14  Y15  Y16  Y17
	 *   Y20  Y21  Y22  Y23  Y24  Y25  Y26  Y27
	 *   Y30  Y31  Y32  Y33  Y34  Y35  Y36  Y37
	 *   Y40  Y41  Y42  Y43  Y44  Y45  Y46  Y47
	 *   Y50  Y51  Y52  Y53  Y54  Y55  Y56  Y57
	 *   Y60  Y61  Y62  Y63  Y64  Y65  Y66  Y67
	 *   Y70  Y71  Y72  Y73  Y74  Y75  Y76  Y77
	 */
	StdLumi	= aaaStdLumiGet();
	Yreff = StdLumi*EvCmp[aaaEvCmpIdx]/100;
	
	Ylevel = (Y[0][0]+Y[0][1]+Y[0][2]+Y[0][3]+Y[0][4]+Y[0][5]+Y[0][6]+Y[0][7]+
			  Y[1][0]+Y[1][1]+Y[1][2]+Y[1][3]+Y[1][4]+Y[1][5]+Y[1][6]+Y[1][7]+
			  Y[2][0]+Y[2][1]+Y[2][2]+Y[2][3]+Y[2][4]+Y[2][5]+Y[2][6]+Y[2][7]+
			  Y[3][0]+Y[3][1]+Y[3][2]+Y[3][3]+Y[3][4]+Y[3][5]+Y[3][6]+Y[3][7]+
			  Y[4][0]+Y[4][1]+Y[4][2]+Y[4][3]+Y[4][4]+Y[4][5]+Y[4][6]+Y[4][7]+
			  Y[5][0]+Y[5][1]+Y[5][2]+Y[5][3]+Y[5][4]+Y[5][5]+Y[5][6]+Y[5][7]+
			  Y[6][0]+Y[6][1]+Y[6][2]+Y[6][3]+Y[6][4]+Y[6][5]+Y[6][6]+Y[6][7]+
			  Y[7][0]+Y[7][1]+Y[7][2]+Y[7][3]+Y[7][4]+Y[7][5]+Y[7][6]+Y[7][7])/64;
	
	/*Ylevel = (Y[2][2]+Y[2][3]+Y[2][4]+Y[2][5]+
			Y[3][2]+Y[3][3]+Y[3][4]+Y[3][5]+
			Y[4][2]+Y[4][3]+Y[4][4]+Y[4][5]+
			Y[5][2]+Y[5][3]+Y[5][4]+Y[5][5])/16;*/
			
	Ylayer = Ylevel * 128 / Yreff;
	*AvgLumi = Ylayer;
	
	G_AvgLumi = Ylayer;
	buf[2] = (UINT8)((Ylayer & 0xff00) >> 8);
	buf[3] = (UINT8)(Ylayer & 0xff);
	printf("Ylayer = %d Tidx = %d\n",Ylayer,Tidx);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaHandleIris                                          *
 *                                                                        *
 *  Purposes: Setting Iris                                                *
 *                                                                        *
 *  Descriptions: Setting Iris in AE contro                               *
 *                                                                        *
 *  Arguments: AE Table Index                                             *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
aaaHandleIris(
	UINT8 idx
)
{

}



/**************************************************************************
 *                                                                        *
 *  Function Name: aaaStdLumiSet                                          *
 *                                                                        *
 *  Purposes: Setting Standard Luminance Target                           *
 *                                                                        *
 *  Descriptions: Setting luminance target                                *
 *                                                                        *
 *  Arguments: standard luminancn target                                  *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
aaaStdLumiSet(
	UINT32 StdLumi
)
{
	aaaStdLumi = StdLumi;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaStdLumiGet                                          *
 *                                                                        *
 *  Purposes: Getting Standard Luminance Target                           *
 *                                                                        *
 *  Descriptions: Getting luminance target                                *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: Luminance target                                             *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
aaaStdLumiGet(
	void
)
{
	return aaaStdLumi;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAeawbStatusSet                                        *
 *                                                                        *
 *  Purposes: Setting AEAWB Ready or not                              *
 *                                                                        *
 *  Descriptions: If AEAWB status is not ready, AeawbMode won't be set by driver or others
 *                      If status is 0, then AeawbMode is forced to be 3.(Disable AE/AWB)
 *                                                                        *
 *  Arguments: AEAWB  status 0 -> not ready, 1 -> ready                                      *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
aaaAeawbStatusSet(
	UINT32 Status
)
{
	AeawbStatus = Status;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAeawbStatusGet                                        *
 *                                                                        *
 *  Purposes: Getting AEAWB development status of sensor                              *
 *                                                                        *
 *  Descriptions: Getting AE/AWB status                                     *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: AEAWB development status, 0 -> not ready, 1 -> ready                                           *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
aaaAeawbStatusGet(
	void
)
{
	return AeawbStatus;
}



/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAeawbModeSet                                        *
 *                                                                        *
 *  Purposes: Setting AE and AWB Mode on/off                              *
 *                                                                        *
 *  Descriptions: Enable/Disable AE and AWB                               *
 *                                                                        *
 *  Arguments: AE/AWB working mode                                        *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
aaaAeawbModeSet(
	UINT32 Mode
)
{
	UINT32 aeawbstatus;
	aeawbstatus = aaaAeawbStatusGet();
	if ( aeawbstatus ) {
		AeawbMode = Mode;
	}
	else {
		AeawbMode = 0x03;	/*Disable AE/AWB*/
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAeawbModeGet                                        *
 *                                                                        *
 *  Purposes: Getting AE and AWB Mode on/off                              *
 *                                                                        *
 *  Descriptions: Getting AE/AWB mode                                     *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: AE/AWB working mode                                          *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
aaaAeawbModeGet(
	void
)
{
	return AeawbMode;
}





/**************************************************************************
 *                                                                        *
 *  Function Name: aaaOutdoorWbSet                                        *
 *                                                                        *
 *  Purposes: Setting outdoor white balance                               *
 *                                                                        *
 *  Descriptions: Setting fixed white balance                             *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
aaaOutdoorWbSet(
	void
)
{
	hwCdspWbOffsetSet(outdoorWB.outRoff, outdoorWB.outGroff, outdoorWB.outBoff, outdoorWB.outGboff);
	hwCdspWbGainSet(outdoorWB.outRgain, outdoorWB.outGrgain, outdoorWB.outBgain, outdoorWB.outGbgain);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaManualWbFlagSet                                     *
 *                                                                        *
 *  Purposes: Setting manual white balance mode by UI                     *
 *                                                                        *
 *  Descriptions: Setting fixed manual white balance                      *
 *                                                                        *
 *  Arguments: manual white balance index                                 *
 *             0:AutoWb                                                   *
 *             1:DaylightWb                                               *
 *             2:CloudyWb                                                 *
 *             3:IncaWb                                                   *
 *             4:FluorescentWb                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:aaa_api.h                                                    *
 *                                                                        *
 **************************************************************************/
void
aaaManualWbFlagSet(
	UINT32 flag
)
{
	switch ( flag ) {
	case 0:
		ManualWbFlag = AutoWb;
		break;
	case 1:
		ManualWbFlag = DaylightWb;
		break;
	case 2:
		ManualWbFlag = CloudyWb;
		break;
	case 3:
		ManualWbFlag = IncaWb;
		break;
	case 4:
		ManualWbFlag = FluorescentWb;
		break;
	}
}



/**************************************************************************
 *                                                                        *
 *  Function Name: aaaManualWbSet                                         *
 *                                                                        *
 *  Purposes: Setting manual white balance parameter according to         *
 *            ManualWbFlag                                                *
 *                                                                        *
 *  Descriptions: Setting fixed manual white balance                      *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:aaa_api.h                                                    *
 *                                                                        *
 **************************************************************************/
void
aaaManualWbSet(
	void
)
{
	/*herb add awb set @03/12/09 start*/
	UINT32 type;

	hwFrontSensorTypeGet(&type);

	if (1){
		switch ( ManualWbFlag ) {
			case AutoWb:
//				aaaOutdoorWbSet();
				break;

			case DaylightWb:
				hwCdspWbOffsetSet(0xfd,0x01,0xfd,0x00);
				hwCdspWbGainSet(0x53,0x42,0x45,0x40);
				break;
				
			case CloudyWb:/*???*/
				hwCdspWbOffsetSet(0x04,0x04,0xfc,0x00);
				hwCdspWbGainSet(0x2f,0x3c,0x71,0x40);
				break;
				
			case IncaWb:
				hwCdspWbOffsetSet(0x0e,0xf9,0xd6,0x00);
				hwCdspWbGainSet(0x36,0x44,0x6b,0x40);
				break;
				
			case FluorescentWb:/*cwf*/
				hwCdspWbOffsetSet(0x00,0xfc,0xff,0x00);
				hwCdspWbGainSet(0x47,0x43,0x5e,0x40);
				break;
		}
	}
	else{
		switch ( ManualWbFlag ) {
			case AutoWb:
			/*hwCdspWbOffsetSet(0x0, 0x0, 0x0, 0x0);*/
			hwCdspWbGainSet(0x50, 0x3f, 0x50, 0x40);
			break;
		case DaylightWb:
			/*hwCdspWbOffsetSet(0x0, 0x0, 0x0, 0x0);*/
			hwCdspWbGainSet(0x50, 0x3f, 0x50, 0x40);
			break;
		case CloudyWb:
			/*hwCdspWbOffsetSet(0x0, 0x0, 0x0, 0x0);*/
			hwCdspWbGainSet(0x50, 0x3f, 0x50, 0x40);
			break;
		case IncaWb:
			/*hwCdspWbOffsetSet(0x0, 0x0, 0x0, 0x0);*/
			hwCdspWbGainSet(0x50, 0x3f, 0x50, 0x40);
			break;
		case FluorescentWb:
			/*hwCdspWbOffsetSet(0x0, 0x0, 0x0, 0x0);*/
			hwCdspWbGainSet(0x50, 0x3f, 0x50, 0x40);
			break;
	}
}
	/*herb add awb set @03/12/09 end*/
}



/**************************************************************************
 *                                                                        *
 *  Function Name: aaaEvCmpIdxSet                                         *
 *                                                                        *
 *  Purposes: Ev compensate adjustment                                    *
 *                                                                        *
 *  Descriptions: Adjust Ev by UI                                         *
 *                                                                        *
 *  Arguments: idx                                                        *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also: None                                                        *
 *                                                                        *
 **************************************************************************/
void
aaaEvCmpIdxSet(
	UINT32 idx
)
{
	aaaEvCmpIdx = idx;
}


/**************************************************************************
 *                                                                        *
 *  Function Name: aaaSharpnessFlagSet                                    *
 *                                                                        *
 *  Purposes: Setting sharpness flag by UI                                *
 *                                                                        *
 *  Descriptions: Setting different sharpness flag                        *
 *                                                                        *
 *  Arguments: manual sharpness flag                                      *
 *             0:NormalEdge                                               *
 *             1:SoftEdge                                                 *
 *             2:StrongEdge                                               *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:aaa_api.h                                                    *
 *                                                                        *
 **************************************************************************/
void
aaaSharpnessFlagSet(
	UINT32 flag
)
{
	/*herb add sharpness @03/12/09 start*/
	/*switch ( flag ) {
	case 0:
		SharpnessFlag = NormalEdge;
		break;
	case 1:
		SharpnessFlag = SoftEdge;
		break;
	case 2:
		SharpnessFlag = StrongEdge;
		break;
	}*/
	switch ( flag ) {
		case 0:
			SharpnessFlag = ExtremeSharpEdge;
			break;
		case 1:
			SharpnessFlag = SharpEdge;
			break;
		case 2:
			SharpnessFlag = NormalEdge;
			break;
		case 3:
			SharpnessFlag = SoftEdge;
			break;
		case 4:
			SharpnessFlag = ExtremeSoftEdge;
			break;
	}
	/*herb add sharpness @03/12/09 end*/
}



/**************************************************************************
 *                                                                        *
 *  Function Name: aaaManualSharpSet                                      *
 *                                                                        *
 *  Purposes: Setting manual sharpness  by UI                             *
 *                                                                        *
 *  Descriptions: Setting different sharnpess                             *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:aaa_api.h                                                    *
 *                                                                        *
 **************************************************************************/
void
aaaManualSharpSet(
	void
)
{
	/*herb add sharpness @03/12/09 start*/
	/*switch ( SharpnessFlag ) {
	case 0:
		hwCdspYEdgeCfg(1, 0x40, 0x60, 0x0A, 0x06, 0x04);*/	/*NormalEdge*/
	/*	break;
	case 1:
		hwCdspYEdgeCfg(1, 0x20, 0x40, 0x10, 0x06, 0x04);*/	/*SoftEdge*/
	/*	break;
	case 2:
		hwCdspYEdgeCfg(1, 0x40, 0x60, 0x06, 0x04, 0x04);*/	/*StrongEdge*/
	/*	break;
	}*/
	switch ( SharpnessFlag ) {
		case 0:
			hwCdspYEdgeCfg(1, 0x40, 0x60, 0x06, 0x04, 0x04);/*ExtremeSharp*/
			break;
		case 1:
			hwCdspYEdgeCfg(1, 0x40, 0x60, 0x08, 0x06, 0x04);/*Sharp*/
			break;
		case 2:
			hwCdspYEdgeCfg(1, 0x40, 0x60, 0x0A, 0x06, 0x04);/*Normal*/
			break;
		case 3:
			hwCdspYEdgeCfg(1, 0x20, 0x40, 0x0c, 0x06, 0x04);/*Soft*/
			break;
		case 4:
			hwCdspYEdgeCfg(1, 0x20, 0x40, 0x10, 0x06, 0x04);/*ExtremeSoft*/
			break;
	}
	/*herb add sharpness @03/12/09 end*/
}


/**************************************************************************
 *                                                                        *
 *  Function Name: aaaPreviewToSnap                                       *
 *                                                                        *
 *  Purposes: Transfer exposure while preview switch to snap mode         *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:		                                                  *
 *                                                                        *
 **************************************************************************/
void
aaaPreviewToSnap(
	void
)
{
	UINT32 FullShutter,FullGain;
	UINT32 type;
	UINT32 regdata[2],DummyLine;
	float  Const;
	/*herb add ISO @03/12/10 start*/
	UINT32 ltotal=0, ftotal=0, espline=0, tmp1=0, pixclk=0, tmp0=0, Gain;
	float  sen_gain, isogain;
	frontSifReg_t *pfrontSifReg;
	frontSenReg_t *pfrontSenReg;

	pfrontSifReg = (frontSifReg_t *)(0xb0000000);
	pfrontSenReg = (frontSenReg_t *)(0xb0000000);
	ftotal = pfrontSenReg->frameTotal;
	ltotal = pfrontSenReg->lineTotal;
	tmp1   = pfrontSifReg->pixClkHi;
	tmp0   = (tmp1 << 16);
	tmp1   = pfrontSifReg->pixClkLow;
	pixclk = tmp0 | tmp1;
	/*herb add ISO @03/12/10 end*/

	#ifdef AAA_EXIF_DEBUG
	exifTagModify(IFD_TYPE_EXIFIFD, EXIF_TAG_USRCOMMENT, buf); /*save 3A related information to exit*/
	#endif

	hwFrontSensorTypeGet(&type);

	if ( (type == SENSOR_CMOS_OV2610_TYPE) || (type == SENSOR_CMOS_SOI268_TYPE) ) {

		hwFrontSscRead(0x2a, regdata, 0x02, 0x02);
		DummyLine = ((( regdata[0] & 0x60 ) << 3 ) | regdata[1])*2;

		Const = ((float)(OV_2610_FULL_LINE + DummyLine)) /((float)(OV_2610_PREVIEW_LINE + DummyLine));
		printf("const=%d\n", (UINT32)(Const*1000));

		if ( !(aaaAeawbModeGet() & 0x01) ) {
			Shutter = aaaAeShutterGet(Tidx);
			Gain = aaaAeGainGet(Tidx);
			if( Gain >= 0x30 ) {
				FullGain = Gain - 32;
				FullShutter = ((float)Shutter/2)*(Const);
			}
			else if ( Gain >= 0x10 ) {
				FullGain = Gain - 16;
				FullShutter = ((float)Shutter/2)*(Const);
			}
			else {
				FullGain = Gain;
				FullShutter = ((float)Shutter)*(Const);
			}
				/*herb add ISO @03/12/09 start*/
				if(sPara.ISOFlag != ISOAUTO){
					printf("original gain=%x\n", FullGain);

					espline = (pixclk * 10) / (ltotal * FullShutter);
					if(ftotal > 616){
						espline = espline-(ftotal-616);
					}
					printf("original expline=%x\n", espline);

					sen_gain = (((FullGain>>5)&0x00000001)+1)*(((FullGain>>4)&0x00000001)+1)*(16+(FullGain&0x0f))/(float)16;

					if(sPara.ISOFlag == ISO50){
						Gain = ISO50_Gain;
					}
					else if(sPara.ISOFlag == ISO100){
						Gain = ISO100_Gain;
					}
					else if(sPara.ISOFlag == ISO200){
						Gain = ISO200_Gain;
					}

					isogain = (((Gain&0x20)>>5)+1)*(((Gain&0x10)>>4)+1)*(16+(Gain&0x0f))/(float)16;
					espline = (float)espline*sen_gain/isogain;
					printf("desired gain=%x\n", Gain);
					printf("desired expline=%x\n", espline);

					if(ftotal > 616){
						espline = espline + (ftotal - 616);
					}
					FullShutter = (pixclk * 10) / (ltotal * espline);

					FullGain = Gain;
				}
				printf("shutter=%d, gain=%x\n", FullShutter, FullGain);
				/*herb add ISO @03/12/09 end*/

			hwFrontExpTimeSet(FullShutter, 1);
			hwFrontGainSet(FullGain, 0);
			hwFrontVdWait(0, 2);
		}

	}
	else if ( type == SENSOR_CMOS_OV3610_TYPE ) {

		hwFrontSscRead(0x2a, regdata, 0x02, 0x02);
		DummyLine = ((( regdata[0] & 0x60 ) << 3 ) | regdata[1])*2;

		Const = ((float)(OV_3610_FULL_LINE + DummyLine)) /((float)(OV_3610_PREVIEW_LINE + DummyLine));

		if ( !(aaaAeawbModeGet() & 0x01) ) {
			Shutter = aaaAeShutterGet(Tidx);
			Gain = aaaAeGainGet(Tidx);
			if( Gain >= 0x30 ) {
				FullGain = Gain - 32;
				FullShutter = ((float)Shutter/2)*(Const);
			}
			else if ( Gain >= 0x10 ) {
				FullGain = Gain - 16;
				FullShutter = ((float)Shutter/2)*(Const);
			}
			else {
				FullGain = Gain;
				FullShutter = ((float)Shutter)*(Const);
			}

			hwFrontExpTimeSet(FullShutter, 1);
			hwFrontGainSet(FullGain, 0);
			hwFrontVdWait(0, 2);
		}

	}
	#if 0
	else if ( type == SENSOR_CMOS_OV9620_TYPE ) {

		hwFrontSscRead(0x2a, regdata, 0x02, 0x02);
		DummyLine = ((( regdata[0] & 0x60 ) << 3 ) | regdata[1])*2;

		Const = ((float)(OV_9620_FULL_LINE + DummyLine)) /((float)(OV_9620_PREVIEW_LINE + DummyLine));

		if ( !(aaaAeawbModeGet() & 0x01) ) {
			Shutter = aaaAeShutterGet(Tidx);
			Gain = aaaAeGainGet(Tidx);
			if( Gain >= 0x30 ) {
				FullGain = Gain - 32;
				FullShutter = ((float)Shutter/2)*(Const);
			}
			else if ( Gain >= 0x10 ) {
				FullGain = Gain - 16;
				FullShutter = ((float)Shutter/2)*(Const);
			}
			else {
				FullGain = Gain;
				FullShutter = ((float)Shutter)*(Const);
			}
			hwFrontExpTimeSet(FullShutter, 1);
			hwFrontGainSet(FullGain, 0);
			hwFrontVdWait(0, 2);
		}

	}
	#else
	else if ( type == SENSOR_CMOS_OV9620_TYPE ) {
		hwFrontSscRead(0x2a, regdata, 0x02, 0x02);
		DummyLine = ((( regdata[0] & 0xf0 ) << 4 ) | regdata[1]);
		
		Const = ((float)(OV_9620_FULL_LINE + DummyLine)) /((float)(OV_9620_PREVIEW_LINE + DummyLine));

		if ( !(aaaAeawbModeGet() & 0x01) ) {
			Shutter = aaaAeShutterGet(Tidx);
			Gain = aaaAeGainGet(Tidx);
			if( Gain >= 0x30 ) {
				FullGain = Gain - 32;
				FullShutter = ((float)Shutter/2)*(Const)/2;
			}
			else if ( Gain >= 0x10 ) {
				FullGain = Gain - 16;
				FullShutter = ((float)Shutter/2)*(Const)/2;
			}
			else {
				FullGain = Gain;
				FullShutter = ((float)Shutter)*(Const)/2;
			}
			printf("liumr debug!\n");
			printf("DummyLine = %d\n",DummyLine);
			hwFrontExpTimeSet(FullShutter, 1);
			hwFrontGainSet(FullGain, 0);
			hwFrontVdWait(0, 2);
		}

	}
	#endif
	else if ( type == SENSOR_CMOS_MI2000_TYPE ) {
		Shutter = aaaAeShutterGet(Tidx);
		Gain = aaaAeGainGet(Tidx);
		if ( !(aaaAeawbModeGet() & 0x01) ) {
			if ( Gain > 0x20 ) {
				if ( Gain >= 0x60 ) {
					Gain = ( 8 + (Gain & 0x07) ) * 8;
				}
				else {
					Gain = (( (Gain & 0x40) >> 6) + 1 ) * ( Gain & 0x3f );
				}
				FullGain = Gain / 2;
				FullShutter = Shutter ;
			}
			else {
				FullGain = Gain;
				FullShutter = Shutter *2;
			}

			hwFrontExpTimeSet(FullShutter, 1);
			hwFrontGainSet(FullGain, 0);
			hwFrontVdWait(0, 2);
		}
	}
		else if ( type == SENSOR_CMOS_MI1310soc_TYPE ) {

		Shutter = aaaAeShutterGet(Tidx);
		Gain = aaaAeGainGet(Tidx);
/*		if ( !(aaaAeawbModeGet() & 0x01) ) {*/
				FullGain = Gain;
				FullShutter = Shutter *2;


			hwFrontExpTimeSet(FullShutter, 1);


			hwFrontGainSet(FullGain, 1);

			hwFrontVdWait(0, 2);
/*		}*/
	}

	else if ( type == SENSOR_CMOS_MI3100_TYPE ) {
#if (!FrameExposure)
		Tidx=Tidx+0;
		printf("Tidx for snap=%d !\n",Tidx);
		Shutter = aaaAeShutterGet(Tidx);
		Gain = aaaAeGainGet(Tidx);
	#if (Snap48MHz==0)
		if ( !(aaaAeawbModeGet() & 0x01) ) {
			if ( Gain > 0x20 ) {
				if ( Gain >= 0x60 ) {
					Gain = ( 8 + (Gain & 0x07) ) * 8;
				}
				else {
					Gain = (( (Gain & 0x40) >> 6) + 1 ) * ( Gain & 0x3f );
				}
				FullGain = Gain / 2;
				FullShutter = Shutter*1 ;
			}
			else {
				FullGain = Gain;
				FullShutter = Shutter *2;
			}

			hwFrontExpTimeSet(FullShutter, 1);
			hwFrontGainSet(FullGain, 0);
			hwFrontVdWait(0, 2);
		}
	#else

			FullGain = Gain;
			FullShutter = Shutter;
			hwFrontExpTimeSet(FullShutter, 1);
			hwFrontGainSet(FullGain, 0);
			hwFrontVdWait(0, 2);
	#endif
#endif
	}
else if ( type == SENSOR_CMOS_MI3100_1024_TYPE){
#if (!FrameExposure)
		Tidx=Tidx+0;

	if ( !(aaaAeawbModeGet() & 0x01) ) {

		Shutter = aaaAeShutterGet(Tidx);
		Gain = aaaAeGainGet(Tidx);

	#if (Snap48MHz==0)

				FullGain = Gain;
				FullShutter = Shutter *3;
	#else
				FullGain = Gain;
				FullShutter = Shutter *1.5;


#endif

			hwFrontExpTimeSet(FullShutter, 1);
			hwFrontGainSet(FullGain, 0);
			hwFrontVdWait(0, 2);
					}
#endif
	}
}
/**************************************************************************
 *                                                                        *
 *  Function Name: aaaSnapToPreview                                       *
 *                                                                        *
 *  Purposes: Restore back to original exposure after snaping             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:		                                                      *
 *                                                                        *
 **************************************************************************/
void
aaaSnapToPreview(
	void
)
{

	Shutter = aaaAeShutterGet(Tidx);
	Gain = aaaAeGainGet(Tidx);
	hwFrontExpTimeSet(Shutter, 1);
	hwFrontGainSet(Gain, 1);


}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaInit                                                *
 *                                                                        *
 *  Purposes: Initialization of 3A related                                *
 *                                                                        *
 *  Descriptions: Initial setting of 3A related                           *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:		                                      		          *
 *                                                                        *
 **************************************************************************/
void
aaaInit(
	void
)
{
	UINT32 i;

	/*Maximum, Minimum and Initial Tidx can be customerized but AE table must follow architecture of EV5~EV17*/

	Ev7_Index = 20;
	Ev9_Index = Ev7_Index + 20;
	Ev10_Index = Ev7_Index + 30;
	Ev11_Index = Ev7_Index + 40;
	Ev12_Index = Ev7_Index + 50;

	Max_Tidx = /*Ev7_Index + 100*/115	/*liumr modify*/;
	Min_Tidx = 14;
	Ini_Tidx = Ev9_Index;

	/*Initial outdoor daylight WB parameter*/
	outdoorWB.outRoff = 0xfd;
	outdoorWB.outGroff = 0x01;
	outdoorWB.outBoff = 0xfd;
	outdoorWB.outGboff = 0x0;
	outdoorWB.outRgain = 0x53;
	outdoorWB.outGrgain = 0x42;
	outdoorWB.outBgain = 0x45;
	outdoorWB.outGbgain = 0x40;

	aaaStdLumiSet(250);	/* 230 */
	aaaAeawbModeSet(0);
	videoClipModeSet(0); /* For future use */
	PreTidx = Tidx = Ini_Tidx;
	ManualWbFlag = sPara.WBFlag;//AutoWb; /* Paul@2006/06/16 add start */
	SharpnessFlag = NormalEdge;
	aaaEvCmpIdx = 5;
	aaaTableChange(0);
	aaaAeFlag = 0;
	aaaAwbFlag = 0;
	FrameRateFlag = 0;

	memset(buf, 0x00, 50);
	strcpy(buf, "Add your 3A setting here...\n");
	for ( i=0; i<50; i++) {
		buf[i] = 0xFF;
	}
	buf[10] = 0xAA;
	buf[20] = 0xBB;
	buf[30] = 0xCC;
	buf[40] = 0xDD;
	buf[21] = Min_Tidx;
	buf[22] = Max_Tidx;
	buf[23] = Ev7_Index;
	buf[24] = Ev12_Index;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: videoClipModeSet                                        *
 *                                                                        *
 *  Purposes: Setting video clip mode  for internal use                       *
 *                                                                        *
 *  Descriptions: Declare video clip size                               *
 *                                                                        *
 *  Arguments: Video clip size, 0 -> preview mode, 1 -> VGA VideoClip, 2 -> VGA VideoClip to preview        *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
videoClipModeSet(
	UINT32 Mode
)
{
	VideoClipMode = Mode;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: videoClipModeGet                                        *
 *                                                                        *
 *  Purposes: Getting video clip mode   for internal use                           *
 *                                                                        *
 *  Descriptions: Getting video clip mode                                     *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: video clip mode, 0 -> preview mode, 1 -> VGA VideoClip, 2 -> VGA VideoClip to preview         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
videoClipModeGet(
	void
)
{
	return VideoClipMode;
}


void
aaaMaxFrameRateSet(
	UINT32 FrameRate
)
{
	MaxFrameRate = FrameRate;
}


UINT32
aaaMaxFrameRateGet(
	void
)
{
	return MaxFrameRate;
}




