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
//#define  RS_232_DBG

#include "general.h"
#include "os_api.h"
#include "hw_front.h"
#include "hw_cdsp.h"
#include "irq.h"
#include "stdio.h"
#include "aaa_api.h"
#include "aaa.h"
#include "hw_cammode.h"

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 Ylayer, Ylevel, Yreff;
extern UINT32 LightFreq;
extern UINT8  buf[50];
extern UINT32 Max_Tidx,Min_Tidx,Ini_Tidx;

/**************************************************************************
 *                 E X T E R N A L    F U N C T I O N S                 *
 **************************************************************************/
extern void sysgChipVerGet(UINT8 *pmajor, UINT8 *pminor);

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/



/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void aaaTask(void *);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 aaaTaskId;
UINT32 vdIrqRefillCnt; 
UINT32 aaaTaskStack[1536];

UINT32 AeMode,AwbMode;
UINT32 aaaAeFlag, aaaAwbFlag;
UINT32 AvgLumi;

SINT32 PreTidx, Tidx;
UINT32 PreShutter, Shutter, PreGain,Gain;

UINT32 ManualWbFlag;
UINT32 aaaEvCmpIdx;
UINT32 SharpnessFlag;
UINT32 FrameRateFlag;
UINT32 aaaAeEntryFlag;

/**************************************************************************
 *                                                                        *
 *  Function Name: vdIrqServ                                              *
 *                                                                        *
 *  Purposes: Interrupt service routine of VD interrupt.                  *                                                            *
 *                                                                        *
 *  Descriptions:                                                         *
 *    This routine serves as periodic function to wake up the 3A task.    *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None.                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void vdIrqServ(void)
{
//    printf("aaa Vd Task\n");
    hwFrontInterruptClr(2);
    osTaskResume(OS_PRIO_AAA);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaTaskInit                                            *
 *                                                                        *
 *  Purposes: Initialization function of 3A task.                         *
 *                                                                        *
 *  Descriptions: Initial setting of 3A                                   *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None.                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void aaaTaskInit(void *pdata)
{
    vdIrqRefillCnt = 3;
    irqIsrReg(FRONT_IRQ, vdIrqServ);
    hwFrontInterruptCfg(2, 0, vdIrqRefillCnt);

    osTaskCreate("AAA", aaaTask, (void *)1536, (void *)(&aaaTaskStack[1535]), OS_PRIO_AAA);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaTask                                                *
 *                                                                        *
 *  Purposes: 3A task body.                                               *
 *                                                                        *
 *  Descriptions: 3A task control                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void aaaTask(void *pdata)
{
    UINT32 Mode,CamMode;
	
    while (1) 
    {
        /*printf("aaa Task\n");*/
        hwCammodeRead(&CamMode);
        AeMode = !(aaaAeawbModeGet() & 0x01);
        AwbMode = !(aaaAeawbModeGet() & 0x02);
        if (CamMode == 2)
        {
            /* Still Camera */
            Mode = 1;
        }
        else if ((CamMode == 1) || (CamMode == 3) || (CamMode == 4))
        {
            /* Preview, video clip, pc-camera */
            Mode = 0;
        }
        else
        {
            /* idle, playback, data-tran */
            Mode = 2;
        }

        if (ManualWbFlag != AutoWb)
        {
            aaaManualWbSet();
            AwbMode = 0;
        }

        switch (Mode) 
        {
            case 0:
                if (AwbMode) 
                {
                    aaaAwb();
                }
                if (AeMode) 
                {
                    aaaAe();
                }
	        break;
            case 1:
                aaaAeFlag = 0;
                aaaAwbFlag = 0;
                if (AeMode & AwbMode) 
                {
                    while (!(aaaAeFlag & aaaAwbFlag)) 
                    {
                        if (AwbMode) 
                        {
                            aaaAwb();
                        }
                        if (AeMode) 
                        {
                            aaaAe();
                        }
                    }
                }
                buf[0] = (UINT8)aaaAeFlag;
                buf[1] = (UINT8)aaaAwbFlag;
                buf[4] = (UINT8)Tidx;
                buf[5] = (UINT8)PreTidx;
                buf[6] = (UINT8)aaaAeawbModeGet();
                buf[9] = (UINT8)FrameRateFlag;
                break;
            default:
                break;
        }
        osTaskSuspend(OS_PRIO_SELF);
    }
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAe                                                  *
 *                                                                        *
 *  Purposes: AE task body                                                *
 *                                                                        *
 *  Descriptions: AE algorithm                                            *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void aaaAe(void)
{
    UINT8 major, minor;
	
    aaaAvgLumiGet(&AvgLumi);
    printf("AvgLumi=%d, PreTidx=%x, Tidx=%x, %x\n", AvgLumi,  PreTidx, Tidx, Ylayer);
	
    if ((Ylayer >= 120) && (Ylayer <= 136)) 
    {
        aaaAeFlag = 1;
        return;
    }
    else if ((PreTidx == Max_Tidx) && (Tidx == Max_Tidx) && (Ylayer > 136)) 
    {
        aaaAeFlag = 1;
        return;
    }
    else if ((PreTidx == Min_Tidx) && (Tidx == Min_Tidx) && (Ylayer < 120)) 
    {
        aaaAeFlag = 1;
        return;
    }
	
    if (FrameRateFlag == 1) 
    {
        Shutter = aaaAeShutterGet(Tidx);
        Gain = aaaAeGainGet(Tidx);	
        hwFrontExpTimeSet(Shutter, 1);
        hwFrontGainSet(Gain, 1);
        FrameRateFlag = 0;
        return;
    }

    if (1) 
    {	
        PreTidx = Tidx;
        if (Ylayer > 256) 
        {
            Tidx += 6;
        }
        else if (Ylayer > 240) 
        {
            Tidx += 5;
        }
        else if (Ylayer > 210) 
        {
            Tidx += 4;
        }
        else if (Ylayer > 180) 
        {
            Tidx += 3;
        }
        else if (Ylayer > 156) 
        {
            Tidx += 2;
        }
        else if (Ylayer > 136) 
        {
            Tidx += 1;
        }
        else if (Ylayer > 120) 
        {
            Tidx += 0;
        }
        else if (Ylayer > 108) 
        {
            Tidx -= 1;
        }
        else if (Ylayer > 90) 
        {
            Tidx -= 2;
        }
        else if (Ylayer > 70) 
        {
            Tidx -= 3;
        }
        else if (Ylayer > 50) 
        {
            Tidx -= 4;
        }
        else 
        {
            Tidx -= 5;
        }
    }

    if (Tidx > Max_Tidx) 
    {
        Tidx = Max_Tidx;
    }
    if (Tidx < Min_Tidx) 
    {
        Tidx = Min_Tidx;
    }

    if (PreTidx != Tidx) 
    {
        PreShutter = aaaAeShutterGet(PreTidx);
        Shutter = aaaAeShutterGet(Tidx);
        PreGain = aaaAeGainGet(PreTidx);
        Gain = aaaAeGainGet(Tidx);	
    }
	
    sysgChipVerGet(&major, &minor);
    if (major == 1) 
    { 
        /* version B */
        if (LightFreq == 0) 
        {	
            /*50Hz*/
            if ((Shutter >= 250) && (Shutter < 290) && (PreShutter >= 300)) 
            {
                hwFrontFrameRateSet(25, 1);     /*30fps->25fps, sync the value to next Vsync */
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 200) && (Shutter < 240) && (PreShutter >= 250)) 
            {
                hwFrontFrameRateSet(20, 1);     /*25fps->20fps, 30fps->20fps*/
                FrameRateFlag = 1;
                return;				
            }
            else if ((Shutter >= 120) && (Shutter < 160) && (PreShutter >= 200)) 
            {
                hwFrontFrameRateSet(12, 1);     /*20fps->12fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 200) && (Shutter <240) && (PreShutter < 160)) 
            {
                hwFrontFrameRateSet(20, 1);     /*12fps->20fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 250) && (Shutter < 290) && (PreShutter < 240)) 
            {
                hwFrontFrameRateSet(25, 1);     /*20fps->25fps*/
                FrameRateFlag = 1;
                return;				
            }
            else if ((Shutter >= 300) && (PreShutter < 290)) 
            {
                hwFrontFrameRateSet(30, 1);     /*25fps->30fps, 20fps->30fps*/
                FrameRateFlag = 1;
                return;				
            }
            else;
        }
        else 
        {
            /*60Hz*/															/*60Hz*/
            if ((Shutter >= 240) && (Shutter < 290) && (PreShutter >= 300)) 
            {
                hwFrontFrameRateSet(24, 1);     /*30fps->24fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 200) && (Shutter < 230) && (PreShutter >= 240)) 
            {
                hwFrontFrameRateSet(20, 1);     /*24fps->20fps, 30fps->20fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 150) && (Shutter < 190) && (PreShutter >= 200)) 
            {
                hwFrontFrameRateSet(15, 1);     /*20fps->15fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 120) && (Shutter < 140) && (PreShutter >= 200)) 
            {
                hwFrontFrameRateSet(12, 1);     /*20fps->12fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 120) && (Shutter < 140) && (PreShutter >= 150)) 
            {
                hwFrontFrameRateSet(12, 1);     /*15fps->12fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 150) && (Shutter < 190) && (PreShutter < 140)) 
            {
                hwFrontFrameRateSet(15, 1);     /*12fps->15fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >=200) && (Shutter < 230) && (PreShutter < 140)) 
            {
                hwFrontFrameRateSet(20, 1);     /*12fps->20fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >=200) && (Shutter < 230) && (PreShutter >= 150) && (PreShutter < 190)) 
            {
                hwFrontFrameRateSet(20, 1);     /*15fps->20fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 240) && (Shutter < 290) && (PreShutter < 230)) 
            {
                hwFrontFrameRateSet(24, 1);     /*20fps->24fps*/
                FrameRateFlag = 1;
                return;
            }
            else if ((Shutter >= 300) && (PreShutter < 290)) 
            {
                hwFrontFrameRateSet(30, 1);     /*24fps->30fps, 20fps->30fps*/
                FrameRateFlag = 1;
                return;
            }
            else;
        }
    }		

    if ((PreShutter != Shutter) && (PreGain != Gain)) 
    {
	/*hwFrontExpTimeGainSet(Shutter, Gain, 1);*/
        hwFrontExpTimeSet(Shutter, 1);
        hwFrontGainSet(Gain, 1);
    }
    else if (PreShutter != Shutter) 
    {
        hwFrontExpTimeSet(Shutter, 1);
    }
    else if (PreGain != Gain) 
    {
        hwFrontGainSet(Gain, 1);
    }
    else;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaAwb                                                 *
 *                                                                        *
 *  Purposes: AWB task body                                               *
 *                                                                        *
 *  Descriptions: AWB algorithm                                           *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
/*chq add for AWB correct color*/ 
#define RG_MAX_RANGE 3
#define RG_MED_RANGE 2
#define BG_MAX_RANGE 3
#define BG_MED_RANGE 2
/*add end*/

UINT32 PreRGain=0;
UINT32 RGainCnt=0;

#define AWBTEST 0

#if AWBTEST
UINT32 loopcnt=0;
#endif

#define AWBTEST_WU 0

#if AWBTEST_WU
UINT8 strAwb[20];
extern UINT8 UserFont10x20[];
#endif

void
aaaAwb(
	void
)
{
	UINT32 Rgain,GRgain,Bgain,GBgain,Roff,GRoff,Boff,GBoff,MinGain,i;
	UINT32 SumY,SumRG,SumBG,PixCnt;
	UINT32 SpwCnt[4], SpwY[4];
	SINT32 SpwRG[4], SpwBG[4];
	UINT32 Count, YSum;
	SINT32 RGSum, BGSum, TotalCount;
	UINT32 RFlag = 0, BFlag = 0;

	hwCdspWbOffsetRead(&Roff, &GRoff, &Boff, &GBoff);	
	hwCdspWbGainRead(&Rgain, &GRgain, &Bgain, &GBgain);
	
#if AWBTEST_WU	
	sio_psprintf(strAwb, "Rgain %x", Rgain);
	osdStrDisp(120, 40, UserFont10x20, 0xd0, strAwb);
	sio_psprintf(strAwb, "GRgain %x", GRgain);
	osdStrDisp(120, 60, UserFont10x20, 0xd0, strAwb);
	sio_psprintf(strAwb, "Bgain %x", Bgain);
	osdStrDisp(120, 80, UserFont10x20, 0xd0, strAwb);
	sio_psprintf(strAwb, "GBgain %x", GBgain);
	osdStrDisp(120, 100, UserFont10x20, 0xd0, strAwb);	
#endif	

/*		 
	printf("Read Rgain = %x GRgain = %x Bgain = %x GBgain = %x\n", Rgain, GRgain, Bgain, GBgain);	
	osdPrintf("Tidx = %2d",100,132,0xd0,Tidx); 
	*/

	for ( i = 0; i < 4; i++ ) {
		hwCdspSPWinRead(i, &SumY, &SumRG, &SumBG, &PixCnt);
		SpwY[i] = SumY;
		SpwRG[i] = SumRG;
		SpwBG[i] = SumBG;
		SpwCnt[i] = PixCnt;
	}
	
#if AWBTEST
	printf("Read Rgain = %x GRgain = %x Bgain = %x GBgain = %x\n", Rgain, GRgain, Bgain, GBgain);	
	loopcnt++;
	if (loopcnt==16)
	{
		for(i = 0; i < 4; i++)
			printf("spw  Y[%d] = %d RG = %d BG = %d Cnt = %d\n",i,SpwY[i],SpwRG[i],SpwBG[i],SpwCnt[i]);
	}
#endif
	
	Count = YSum = RGSum = BGSum = 0;
	for ( i = 0; i < 4; i++ ) {
		Count += SpwCnt[i];
		YSum += SpwY[i];
		RGSum +=  SpwRG[i];
		BGSum += SpwBG[i];
		
	}
	
	if ( Count > 0 ) {
		TotalCount = Count;
		RGSum /= TotalCount;
		BGSum /= TotalCount;
	}
	
#if AWBTEST
	if (loopcnt == 16)
	{
		printf("Count = %d YSum = %d RGSum = %d BGSum = %d\n",Count,YSum,RGSum,BGSum);
	}
#endif

	if ( 1 ) {
		if ( (RGSum >= WB_RH) || (RGSum <= WB_RL) ) {
			if ( RGSum > (WB_RH+RG_MAX_RANGE) ) {
				Rgain -= 4;
			}
			else if ( RGSum > (WB_RH + RG_MED_RANGE) ) {
				Rgain -= 2;
			}
			else if ( RGSum > WB_RH ) {
				Rgain -= 1;
			}
			else if ( RGSum < (WB_RL - RG_MAX_RANGE) ) {
				Rgain += 4;
			}
			else if ( RGSum < (WB_RL - RG_MED_RANGE) ) {
				Rgain += 2;
			}
			else if ( RGSum < WB_RL ) {
				Rgain += 1;
			}
			else {
				RFlag = 1;
			}
		}
		else {
			RFlag = 1;
		}

		if ( (BGSum >= WB_BH) || (BGSum <= WB_BL) ) {
			if ( BGSum > (WB_BH + BG_MAX_RANGE)) {
				Bgain -= 4;
			}
			else if ( BGSum > (WB_BH + BG_MED_RANGE) ) {
				Bgain -= 2;
			}
			else if ( BGSum > WB_BH ) {
				Bgain -= 1;
			}
			else if ( BGSum < (WB_BL - BG_MAX_RANGE) ) {
				Bgain += 4;
			}
			else if ( BGSum < (WB_BL - BG_MED_RANGE) ) {
				Bgain += 2;
			}
			else if ( BGSum < WB_BL ) {
				Bgain += 1;
			}
			else {
				BFlag = 1;
			}
		}
		else {
			BFlag = 1;
		}
	}

	MinGain = Rgain;
	if ( GRgain < MinGain ) {
		MinGain = GRgain;
	}
	if ( GBgain < MinGain ) {
		MinGain = GBgain;
	}
	if ( Bgain < MinGain ) {
		MinGain = Bgain;
	}
	
	Rgain = Rgain*64/MinGain;	
	GRgain = GRgain*64/MinGain;
	Bgain = Bgain*64/MinGain;
	GBgain = GBgain*64/MinGain;

	if ( (Rgain > MAX_AWB_GAIN)  || (GRgain > MAX_AWB_GAIN) ||
	     (Bgain > MAX_AWB_GAIN)  || (GBgain > MAX_AWB_GAIN) ) {
		aaaAwbFlag = 1;		
	}

	if ( Rgain > MAX_AWB_GAIN ) {
		Rgain = MAX_AWB_GAIN;
	}
	if ( Rgain < MIN_AWB_GAIN ) {
		Rgain = MIN_AWB_GAIN;
	}
	if ( GRgain > 0x40 ) {
		GRgain = 0x40;
	}
	if ( GRgain < MIN_AWB_GAIN ) {
		GRgain = MIN_AWB_GAIN;
	}
	if ( Bgain > MAX_AWB_GAIN-2 ) {
		Bgain = MAX_AWB_GAIN-2;
	}		
	if ( Bgain < MIN_AWB_GAIN ) {
		Bgain = MIN_AWB_GAIN;
	}
	if ( GBgain > 0x40 ) {
		GBgain = 0x40;
	}
	if ( GBgain < MIN_AWB_GAIN ) {
		GBgain = MIN_AWB_GAIN;
	}
/*	osdPrintf("R=%3d B=%3d",0x00,96,0xe0,Rgain, Bgain);
	osdPrintf("GB=%3d GR=%3d",0x00,128,0xe0,GBgain, GRgain);*/

/*	osdPrintf("   ",0,132,0xd0); 
#if AWBTEST
if (loopcnt == 16)
{

	printf("Set  Rgain = %x GRgain = %x Bgain = %x GBgain = %x\n",Rgain, GRgain, Bgain, GBgain);
}
#endif*/
/*
	if (PreRGain!= Rgain)
	{
		PreRGain = Rgain;
		RGainCnt = 0;
	}
	else
	{
		RGainCnt++;
		if (RGainCnt > 12)
		{
			Rgain -= 2;
		}
	}
*/

#if AWBTEST
if (loopcnt == 16)
{
	loopcnt = 0;
	printf("Set  Rgain = %x GRgain = %x Bgain = %x GBgain = %x\n",Rgain, GRgain, Bgain, GBgain);
	hwCdspWbGainSet(Rgain, GRgain, Bgain, GBgain);
}
#else
	hwCdspWbGainSet(Rgain, GRgain, Bgain, GBgain);
#endif
	aaaAwbFlag = RFlag && BFlag;
	buf[11] = (UINT8)((Rgain & 0xff00) >> 8);
	buf[12] = (UINT8)(Rgain & 0xff);
	buf[13] = (UINT8)((GRgain & 0xff00) >> 8);
	buf[14] = (UINT8)(GRgain & 0xff);
	buf[15] = (UINT8)((Bgain & 0xff00) >> 8);
	buf[16] = (UINT8)(Bgain & 0xff);
	buf[17] = (UINT8)((GBgain & 0xff00) >> 8);
	buf[18] = (UINT8)(GBgain & 0xff);	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: aaaSnap                                                *
 *                                                                        *
 *  Purposes: 3A snap flw.                                                *
 *                                                                        *
 *  Descriptions: 3A snap flow                                            *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void aaaSnap(void)
{
}

