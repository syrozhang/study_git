/*	sun@1030 am 11:25 add
	For the OB calibration 
	DDX  Technology Co., Ltd.                                      
	2 floor R2-B building ShenZhen High-Tech Industria Park Of China.  
	 Author: Sun Chen  
*/

#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "stdio.h"
#include "os_api.h"
#include "ui_main.h"
#include "hw_disp.h"
#include "hw_disp_ext.h"
#include "osd_common.h"
#include "osd_api.h"
#include "xiaoao.h"
#include "panel.h"
#include "gpio.h"
#include "hw_sdram.h"
#include "dsc_init.h"
#include "hw_cdsp.h"
#include "hw_front.h"
#include "zoom.h"
#include "aaa_api.h"
#include "snap_custom.h"
#include "errcode.h"
#include "usb_api.h"
#include "sysg_api.h"
#include "snap_api.H"
#include "timer.h"
#include "dcf_api.h"
#include "vfs_api.h"
#include "misc_api.h"
#include "hw_cpu.h"
#include "Audio_Api.h"
#include "smc_api.h"

#define SENSORREG_READ(addr,dat,cnt,opt)   hwFrontSscRead(addr,dat,cnt,opt)
#define SENSORREG_WRITE(addr,dat,cnt,opt)  hwFrontSscWrite(addr,dat,cnt,opt)
#define MAX_VAL_DIF 1
#define MIN_VAL_DIF 1
#define MAX_VAL 8
#define MIN_VAL 4
#define B_CHANNEL_rREG  0x05
#define GB_CHANNEL_rREG 0x06
#define GR_CHANNEL_rREG 0x07
#define R_CHANNEL_rREG  0x08
#define PID_NUM_MSB_rREG 0x0A
#define PID_NUM_LSB_rREG 0x0B
#define OFFSET_DIR_REG  0x3F
#define B_CHANNEL_wREG  0x40
#define GB_CHANNEL_wREG 0x42
#define GR_CHANNEL_wREG 0x43
#define R_CHANNEL_wREG  0x41
/***********************************/
	extern UINT8 G_OB_previewRval;
	extern UINT8 G_OB_previewGRval;
	extern UINT8 G_OB_previewGBval;
	extern UINT8 G_OB_previewBval;
	extern UINT8 G_OB_snapRval;
	extern UINT8 G_OB_snapGRval;
	extern UINT8 G_OB_snapGBval;
	extern UINT8 G_OB_snapBval;
/***********************************/
void ov3620_OBcheck(UINT8* r,UINT8* gr,UINT8* gb,UINT8* b)
{
	int wBval[1],wGBval[1],wGRval[1],wRval[1],tempData[1];
	int rBval[1],rGBval[1],rGRval[1],rRval[1];
	int  B_wReg[1],GB_wReg[1],GR_wReg[1],R_wReg[1];
	UINT8 Loop_Flag=0;
	int RGB_avearge=0;

	UINT8 str[16];
	
	B_wReg[0]=B_CHANNEL_wREG;
	GB_wReg[0]=GB_CHANNEL_wREG;
	GR_wReg[0]=GR_CHANNEL_wREG;
	R_wReg[0]=R_CHANNEL_wREG;
	wBval[0] = 0;
	wGBval[0] = 0;
	wGRval[0] = 0;
	wRval[0] = 0;
	SENSORREG_READ(PID_NUM_MSB_rREG,tempData,1,2);
	if(tempData[0] == 0x36)
	{
		SENSORREG_READ(PID_NUM_LSB_rREG,tempData,1,2);	
		if(tempData[0] == 0x20)
	 		osdStrDisp(0, 160, UserFont16x20, 0xe0, "Sensor->3620");
		else 
			osdStrDisp(0, 160, UserFont16x20, 0xe0, "Sensor->Err");
		
	}
	SENSORREG_READ(B_CHANNEL_rREG,rBval,1,2);
	SENSORREG_READ(GB_CHANNEL_rREG,rGBval,1,2);
	SENSORREG_READ(GR_CHANNEL_rREG,rGRval,1,2);
	SENSORREG_READ(R_CHANNEL_rREG,rRval,1,2);
	RGB_avearge = (rGBval[0]+rGRval[0])/2;
	for(;;)         
	{
	       SENSORREG_READ(B_CHANNEL_rREG,rBval,1,2);
	       if(rBval[0] < (RGB_avearge - MAX_VAL_DIF))
	       {
	                wBval[0]++;
	                SENSORREG_WRITE(B_wReg,wBval,1,2);               
	       }
	       else if(rBval[0] > (MAX_VAL_DIF + RGB_avearge))
	       {
	                wBval[0] = (wBval[0]>0)?wBval[0]--:wBval[0];
	                SENSORREG_WRITE(B_wReg,wBval,1,2);
	       }
	       if(rGBval[0] < (RGB_avearge - MAX_VAL_DIF))
	       {
	                wGBval[0]++;
	                SENSORREG_WRITE(GB_wReg,wGBval,1,2);                  
	       }
	       else if(rGBval[0] > (MAX_VAL_DIF + RGB_avearge))
	       {      
	                wGBval[0] = (wGBval[0]>0)?wGBval[0]--:wGBval[0];
	                SENSORREG_WRITE(GB_wReg,wGBval,1,2);
	       }
	       SENSORREG_READ(GR_CHANNEL_rREG,rGRval,1,2);
	       if(rGRval[0] < (RGB_avearge - MAX_VAL_DIF))
	       {
	                wGRval[0]++;
	                SENSORREG_WRITE(GR_wReg,wGRval,1,2);                  
	       }
	       else if(rGRval[0] > (MAX_VAL_DIF + RGB_avearge))
	       {        
	                wGRval[0] = (wGRval[0]>0)?wGRval[0]--:wGRval[0];
	                SENSORREG_WRITE(GR_wReg,wGRval,1,2);
	       }
	       SENSORREG_READ(R_CHANNEL_rREG,rRval,1,2);
	       if(rRval[0] < (RGB_avearge - MAX_VAL_DIF))
	       {
	                wRval[0]++;
	                SENSORREG_WRITE(R_wReg,wRval,1,2);              
	       }
	       else if(rRval[0] > (MAX_VAL_DIF + RGB_avearge))
	       {                 
	                wRval[0] = (wRval[0]>0)?wRval[0]--:wRval[0];
	                SENSORREG_WRITE(R_wReg,wRval,1,2);
	       }
	       RGB_avearge = (rGBval[0]+rGRval[0])/2;

		sio_psprintf(str, "rB=%3d  wB=%3d", rBval[0],wBval[0]);
		osdStrDisp(0, 32, UserFont16x20, 0xd0, str);
		
		sio_psprintf(str, "rGB=%3d wGB=%3d", rGBval[0],wGBval[0]);
		osdStrDisp(0, 64, UserFont16x20, 0xd0, str);
		
		sio_psprintf(str, "rGR=%3d wGR=%3d", rGRval[0],wGRval[0]);
		osdStrDisp(0, 96, UserFont16x20, 0xd0, str);
		
		sio_psprintf(str, "rR=%3d  wR=%3d", rRval[0],wRval[0]);
		osdStrDisp(0, 128, UserFont16x20, 0xd0, str);
		
		
/*	 
	       osdPrintf("rB=%3d  wB=%3d",0,32,0xd0,rBval[0],wBval[0]);
		   
	       osdPrintf("rGB=%3d wGB=%3d",0,64,0xd0,rGBval[0],wGBval[0]);
	       osdPrintf("rGR=%3d wGR=%3d",0,96,0xd0,rGRval[0],wGRval[0]);
	       osdPrintf("rR=%3d  wR=%3d",0,128,0xd0,rRval[0],wRval[0]);
*/
		   
		if((rBval[0]<=(MAX_VAL + RGB_avearge))&&(rBval[0]>=(RGB_avearge - MIN_VAL)))
			Loop_Flag |= 0x01;
		else
			Loop_Flag &= 0xfe;	
	/*		if((rGBval[0]<(MAX_VAL + RGB_avearge))&&(rGBval[0]>(RGB_avearge - MIN_VAL)))
			Loop_Flag |= 0x02;
		else
			Loop_Flag &= 0xfd;
		if((rGRval[0]<(MAX_VAL + RGB_avearge))&&(rGRval[0]>(RGB_avearge - MIN_VAL)))
			Loop_Flag |= 0x04;
		else
			Loop_Flag &= 0xfb;*/
		if((rRval[0]<=(MAX_VAL + RGB_avearge))&&(rRval[0]>=(RGB_avearge - MIN_VAL)))
			Loop_Flag  |=0x08;
		else 
			Loop_Flag  &=0xf7;
		if((Loop_Flag == 0x09)||(wBval[0]>wRval[0]+2)||(wRval[0]>wBval[0]+2))
		{
			*r=wRval[0];
			*gr=wGRval[0];
			*gb=wGBval[0];
			*b=wBval[0];
			/*osdPrintf("Clibration OK",0,160,0x30);	*/
			osdStrDisp(0, 160, UserFont16x20, 0x30, "Clibration OK");
			break;
		}
		osTimeDly(200);
	}
}
void ov3620_OBcheckSnapMode(UINT8* r,UINT8* gr,UINT8* gb,UINT8* b)
{
	int wBval[1],wGBval[1],wGRval[1],wRval[1],tempData[1];
	int rBval[1],rGBval[1],rGRval[1],rRval[1];
	int  B_wReg[1],GB_wReg[1],GR_wReg[1],R_wReg[1];
	UINT8 Loop_Flag=0;
	int RGB_avearge=0;

	UINT8 str[16];
	
	B_wReg[0]=B_CHANNEL_wREG;
	GB_wReg[0]=GB_CHANNEL_wREG;
	GR_wReg[0]=GR_CHANNEL_wREG;
	R_wReg[0]=R_CHANNEL_wREG;
	wBval[0] = 0;
	wGBval[0] = 0;
	wGRval[0] = 0;
	wRval[0] = 0;
	SENSORREG_READ(PID_NUM_MSB_rREG,tempData,1,2);
	if(tempData[0] == 0x36)
	{
		SENSORREG_READ(PID_NUM_LSB_rREG,tempData,1,2);	
		if(tempData[0] == 0x20)
	 		osdStrDisp(0, 160, UserFont16x20, 0xe0, "Sensor->3620");
		else 
			osdStrDisp(0, 160, UserFont16x20, 0xe0, "Sensor->Err");
	}
#if 0

hwFrontVdWait(0, 3);

SENSORREG_READ(B_CHANNEL_rREG,rBval,1,2);
printf("rBval03=%d\n",rBval[0]);
rBval[0]-=5;
SENSORREG_WRITE(B_CHANNEL_rREG,rBval,1,2);	
hwFrontVdWait(0, 3);
SENSORREG_READ(B_CHANNEL_rREG,rBval,1,2);
printf("rBval04=%d\n",rBval[0]);

hwFrontVdWait(0, 3);

SENSORREG_READ(B_wReg,wBval,1,2);
printf("wBval03=%d\n",wBval[0]);
wBval[0]+=5;
SENSORREG_WRITE(B_wReg,wBval,1,2);	
hwFrontVdWait(0, 3);
SENSORREG_READ(B_wReg,wBval,1,2);
printf("wBvall04=%d\n",wBval[0]);

	wBval[0] = 0;
	wGBval[0] = 0;
	wGRval[0] = 0;
	wRval[0] = 0;
	#endif
SENSORREG_WRITE(B_CHANNEL_rREG,wBval,1,2);	
SENSORREG_WRITE(GB_CHANNEL_rREG,wBval,1,2);	
SENSORREG_WRITE(GR_CHANNEL_rREG,wBval,1,2);	
SENSORREG_WRITE(R_CHANNEL_rREG,wBval,1,2);	
hwFrontVdWait(0, 9);


	for(;;)	
	{
		SENSORREG_READ(B_CHANNEL_rREG,rBval,1,2);
		printf("rBval0=%d\n",rBval[0]);
		if(rBval[0] < (MIN_VAL))
		{
			wBval[0]++;
			SENSORREG_WRITE(B_wReg,wBval,1,2);		
		}
		else if(rBval[0] > (MAX_VAL))
		{
			wBval[0] = (wBval[0]>0)?wBval[0]--:wBval[0];
			SENSORREG_WRITE(B_wReg,wBval,1,2);
		}
		SENSORREG_READ(GB_CHANNEL_rREG,rGBval,1,2);
		printf("rGBval0=%d\n",rGBval[0]);
		if(rGBval[0] < (MIN_VAL))
		{
			wGBval[0]++;
			SENSORREG_WRITE(GB_wReg,wGBval,1,2);		
		}
		else if(rGBval[0] > (MAX_VAL))
		{	
			wGBval[0] = (wGBval[0]>0)?wGBval[0]--:wGBval[0];
			SENSORREG_WRITE(GB_wReg,wGBval,1,2);
		}
		SENSORREG_READ(GR_CHANNEL_rREG,rGRval,1,2);
		printf("rGRval0=%d\n",rGRval[0]);
		if(rGRval[0] < (MIN_VAL))
		{
			wGRval[0]++;
			SENSORREG_WRITE(GR_wReg,wGRval,1,2);		
		}
		else if(rGRval[0] > (MAX_VAL))
		{	
			wGRval[0] = (wGRval[0]>0)?wGRval[0]--:wGRval[0];
			SENSORREG_WRITE(GR_wReg,wGRval,1,2);
		}
		SENSORREG_READ(R_CHANNEL_rREG,rRval,1,2);
		printf("rRval0=%d\n",rRval[0]);
		if(rRval[0] < (MIN_VAL))
		{
			wRval[0]++;
			SENSORREG_WRITE(R_wReg,wRval,1,2);		
		}
		else if(rRval[0] > (MAX_VAL))
		{		
			wRval[0] = (wRval[0]>0)?wRval[0]--:wRval[0];
			SENSORREG_WRITE(R_wReg,wRval,1,2);
		}
		RGB_avearge = (rRval[0]+rGBval[0]+rGRval[0]+rBval[0])/4;
		printf("RGB_avearge=%d\n",RGB_avearge);
		sio_psprintf(str, "rB=%3d  wB=%3d", rBval[0],wBval[0]);
		osdStrDisp(0, 32, UserFont16x20, 0xd0, str);
		
		sio_psprintf(str, "rGB=%3d wGB=%3d", rGBval[0],wGBval[0]);
		osdStrDisp(0, 64, UserFont16x20, 0xd0, str);
		
		sio_psprintf(str, "rGR=%3d wGR=%3d", rGRval[0],wGRval[0]);
		osdStrDisp(0, 96, UserFont16x20, 0xd0, str);
		
		sio_psprintf(str, "rR=%3d  wR=%3d", rRval[0],wRval[0]);
		osdStrDisp(0, 128, UserFont16x20, 0xd0, str);
		
/*		
		osdPrintf("rB=%3d  wB=%3d",0,32,0xd0,rBval[0],wBval[0]);
		osdPrintf("rGB=%3d wGB=%3d",0,64,0xd0,rGBval[0],wGBval[0]);
		osdPrintf("rGR=%3d wGR=%3d",0,96,0xd0,rGRval[0],wGRval[0]);
		osdPrintf("rR=%3d  wR=%3d",0,128,0xd0,rRval[0],wRval[0]);
*/
		if((rBval[0]<=(MAX_VAL))&&(rBval[0]>=(MIN_VAL)))
			Loop_Flag |= 0x01;
		else
			Loop_Flag &= 0xfe;	
		if((rGBval[0]<=(MAX_VAL))&&(rGBval[0]>=(MIN_VAL)))
			Loop_Flag |= 0x02;
		else
			Loop_Flag &= 0xfd;
		if((rGRval[0]<=(MAX_VAL))&&(rGRval[0]>=(MIN_VAL)))
			Loop_Flag |= 0x04;
		else
			Loop_Flag &= 0xfb;/**/
		if((rRval[0]<=(MAX_VAL))&&(rRval[0]>=(MIN_VAL)))
			Loop_Flag  |=0x08;
		else 
			Loop_Flag  &=0xf7;
		
		if((Loop_Flag == 0x0F))
		{
			*r=wRval[0];
			*gr=wGRval[0];
			*gb=wGBval[0];
			*b=wBval[0];
			osdStrDisp(0, 160, UserFont16x20, 0x30, "Clibration OK");
			break;
		}
		osTimeDly(200);
	}
}
#define PREVIEW_MODE 1
#define SNAP_MODE    2
void PreviewORSnapModeSet(UINT8 mode)
{	
	UINT32 regaddr[1], regdata[1];
	if(mode == PREVIEW_MODE)
		{
		hwFrontPreviewSet();
		/*David 01132006	*
		regaddr[0] = 0x00; 
		regdata[0] = 0x7f; 
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);                    
		
		regaddr[0] = 0x10; 
		regdata[0] = 0x3d; 
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);                    
		/**/
		}
	else if(mode == SNAP_MODE)
	{
		regaddr[0] = 0x12; 
		regdata[0] = 0x00; 
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);                    
		
		regaddr[0] = 0x03; 
		regdata[0] = 0x44; 
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);  
		regaddr[0] = 0x17; 
		regdata[0] = 0x0e;/*0x0a; */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);  
		regaddr[0] = 0x1a; 
		regdata[0] = 0xc4;/*0x0a; */
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);  

		/*David 01132006	*
		regaddr[0] = 0x00; 
		regdata[0] = 0x10; 
		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);                    

//		regaddr[0] = 0x10; 
//		regdata[0] = 0xc1; 
//		hwFrontSscWrite(regaddr, regdata, 0x01, 0x00);                    
		/**/
	}
}
#define SECTOR_OF_OB_PARAM 0x100
void WriteOBvalToRsvBlk(UINT8 pBval,UINT8 pGBval,UINT8 pGRval,UINT8 pRval,UINT8 sBval,UINT8 sGBval,UINT8 sGRval,UINT8 sRval)
{
	UINT8 *wData;
	wData = osMemAlloc(512);
	wData = (UINT8 *)((0xAC000000 | (UINT32)wData));
	UINT32 err,i;
	memset(wData,0,512);
	wData[500]=pRval;
	wData[501]=pGRval;
	wData[502]=pGBval;
	wData[503]=pBval;	
	wData[504]=sRval;
	wData[505]=sGRval;
	wData[506]=sGBval;
	wData[507]=sBval;
	wData[508]=0x1,wData[509]=0x2;
	for(i=500;i<510;i++)
	{
		wData[510] += wData[i];	
	}
	err = nandRsvBlkWrite(SECTOR_OF_UI_PARAM, 1, wData);
/*	for(i=0;i<512;i++)
			printf("para[%d]=%d\n",i,wData[i]);
	printf("write para to nandflash err =%d\n",err);*/
	osMemFree(wData);
} 
void ReadOBvalFromRsvBlk(UINT8 *pBval,UINT8 *pGBval,UINT8 *pGRval,UINT8 *pRval,UINT8 *sBval,UINT8 *sGBval,UINT8 *sGRval,UINT8 *sRval)
{
	UINT8 *wData;
	UINT8 checksum=0;
	wData = osMemAlloc(512);
	wData = (UINT8 *)((0xAC000000 | (UINT32)wData));
	UINT32 i;
	memset(wData,0,512);
	nandRsvBlkRead(SECTOR_OF_UI_PARAM, 1, wData);
	for(i=500;i<510;i++)
	{
		checksum += wData[i];	
	}
	if((checksum == wData[510])&&(wData[508]==0x1)&&(wData[509]==0x2))
	{
		*pRval  = (wData[500]==0xff)?0:wData[500];
		*pGRval = (wData[501]==0xff)?0:wData[501];
		*pGBval = (wData[502]==0xff)?0:wData[502];
		*pBval  = (wData[503]==0xff)?0:wData[503];
		*sRval  = (wData[504]==0xff)?0:wData[504];
		*sGRval = (wData[505]==0xff)?0:wData[505];
		*sGBval = (wData[506]==0xff)?0:wData[506];
		*sBval  = (wData[507]==0xff)?0:wData[507];
		printf("<read OB para succes>\n");
	/*	for(i=0;i<512;i++)
			printf("para[%d]=%d\n",i,wData[i]);*/
	}
	else
	{
		*pRval  = 0;
		*pGRval = 0;
		*pGBval = 0;
		*pBval  = 0;
		*sRval  = 0;
		*sGRval = 0;
		*sGBval = 0;
		*sBval  = 0;
		printf("<read OB para failed>\n");
	/*	for(i=0;i<512;i++)
			printf("para[%d]=%d\n",i,wData[i]);	*/
	}
	osMemFree(wData);
	printf("pR=%d pGR=%d pGB=%d pB=%d\n",*pRval,*pGRval,*pGBval,*pBval);
	printf("sR=%d sGR=%d sGB=%d sB=%d\n",*sRval,*sGRval ,*sGBval,*sBval);
}
void OV3620_OBCalibration(void)
{
	int i;
	
	int  B_wReg[1],GB_wReg[1],GR_wReg[1],R_wReg[1];
	B_wReg[0]=B_CHANNEL_wREG;
	GB_wReg[0]=GB_CHANNEL_wREG;
	GR_wReg[0]=GR_CHANNEL_wREG;
	R_wReg[0]=R_CHANNEL_wREG;
	#if 0
	/* sun@1030 am 10:26 
	gpioConfig(GPIO_SENSOR_POWER, 1);
	gpioSet(GPIO_SENSOR_POWER, 1);
	gpioConfig(GPIO_SEN_STANDBY, 1);
	gpioSet(GPIO_SEN_STANDBY, 0);
	
	PNL_PreviewSet();
	#else*/
	aaaInit();					
	uiKeyMsgQ = osEventFindByName("UI_KEY");
	isMenuOn = 0;
	pvZoomFactor = 100;	

	/* Init Sensor */
	WRITE8(0xb0000007,0x7f);		/* ?? */
	gpioSet(GPIO_SENSOR_POWER, 1);
	gpioSet(GPIO_SEN_STANDBY, 0);
	hwDispPvEn(0);					/* Paul@2005/10/11 */
	hwDispOsdEn(0);
	hwWait(0, 8);					
	hwTvSensorModeSel(0);	
	hwDispPvEn(0);
	hwDispOsdEn(0);					
	osTaskResume(osTaskFindByName("HISTOGRAM"));
	osTaskResume(osTaskFindByName("AAA"));						
	osdClearScreen(0x00);												
	
	PNL_PreviewSet();	

	if(sPara.EffectFlag == SephiaEffect)
	{
		hwCdspSatHueSet(0, 0x20, 0x00, 0x40);
		hwCdspColorMSet(0x38, 0x38, 0x38, 0x28, 0x28, 0x28, 0x20, 0x20, 0x20);
	}
	else if(sPara.EffectFlag == NegativeEffect)
	{
		hwCdspSatHueSet(1, 0x00, 0x00, 0x40);
	}
	else if(sPara.EffectFlag == NormalEffect)
	{
		hwCdspSatHueSet(0, 0x20, 0x00, 0x40);
		hwCdspColorMSet(0x86, 0x1AC, 0x0E, 0x1F2, 0x33, 0x1C, 0x1F3, 0x1B0, 0x9D);
	}	
							
	zoomFactSet(zoomFactGet());
	hwPVZoom(pvZoomFactor);
	sysgSnapScaleFactorSet(pvZoomFactor);
	aaaEvCmpIdxSet(pvEvCmpIdx);
	SnapImageSizeSet();
						
	#if 1
	xdcfCurRootDirSet(imageRootNameStr);
	xdcfInit(imageDirNameStr, imageFileNameStr, xDCF_CONFIG_KEY_MIN | xDCF_CONFIG_SORT_IDX/* | xDCF_CONFIG_DCF_ONLY*/);
	xdcfFilterSet(xDCF_FILTER_JPG | xDCF_FILTER_ASF | xDCF_FILTER_WAV | xDCF_FILTER_OURTYPE);
	exifInit(exifTagInsCallback, exifTagRmvCallback);
	#endif

	osdIconDisp(1);	
	
	hwDispOsdEn(1);
	hwDispPvEn(1);	
	#endif
    gpioSet(GPIO_LCD_POWER, 0);
	osTimeDly(20);	

	for(i=0;i<1;i++)
	{
		/*hwDispPvEn(1);
		osdClearScreen(0);*/
		aaaAeawbModeSet(3);
		hwCdspManuOBSet(0,0);
		hwCdspWbOffsetSet(0, 0, 0, 0);
		hwCdspBadPixelSet(0,0,0,0);
		osdStrDisp(0, 0, UserFont16x20, 0x30, "SNAP MODE");
		/*osdPrintf("SNAP MODE",0,0,0x30);*/
		PreviewORSnapModeSet(SNAP_MODE);
		hwFrontExpTimeSet(1548, 0);
		hwFrontGainSet(0xff,0);
		osTimeDly(200);
		ov3620_OBcheckSnapMode(&G_OB_snapRval,&G_OB_snapGRval,&G_OB_snapGBval,&G_OB_snapBval);
		osTimeDly(100);
		osdClearScreen(0);
		SENSORREG_WRITE(B_wReg,0,1,2);
		SENSORREG_WRITE(GB_wReg,0,1,2);
		SENSORREG_WRITE(GR_wReg,0,1,2);
		SENSORREG_WRITE(R_wReg,0,1,2);
		osdStrDisp(0, 0, UserFont16x20, 0x30, "PREVIEW MODE");
		/*osdPrintf("PREVIEW MODE",0,0,0x30);*/
		PreviewORSnapModeSet(PREVIEW_MODE);
		hwFrontExpTimeSet(778, 0);
		hwFrontGainSet(0xfe,0);
		osTimeDly(100);	
		ov3620_OBcheck(&G_OB_previewRval,&G_OB_previewGRval,&G_OB_previewGBval,&G_OB_previewBval);
	/*	WriteOBvalToRsvBlk(G_OB_previewRval,G_OB_previewGRval,G_OB_previewGBval,G_OB_previewBval,
						   G_OB_snapRval,   G_OB_snapGRval,   G_OB_snapGBval,   G_OB_snapBval);*/
		osTimeDly(20);
	    gpioSet(GPIO_LCD_POWER, 1);							   
		printf("pR=%d pGR=%d pGB=%d pB=%d\n",G_OB_previewRval,G_OB_previewGRval,G_OB_previewGBval,G_OB_previewBval);
		printf("sR=%d sGR=%d sGB=%d sB=%d\n",G_OB_snapRval,G_OB_snapGRval,G_OB_snapGBval,G_OB_snapBval);
	}	
}

/**************************************************************************
when to OB calibration, It must be disable all audio device, so please set one flag to jump 
the audio device before power on, after OB calibration, please save the R,G,B value to nand flash.

***************************************************************************/
UINT8 onOBCheck = 0;

UINT8 menuOV3620_OBCalibration(void)
{

	OV3620_OBCalibration();	
	return 0;
}

