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
 *  Author: Richie Lee                                                    *
 *                                                                        *
 **************************************************************************/
#define  RS_232_DBG

#include  "stdlib.h"
#include  "string.h"
#include  "general.h"
#include  "stdio.h"
#include  "ui_main.h"
#include  "ui_func.h"
#include  "dcf_api.h"
#include  "os_api.h"
#include  "xiaoao.h"
#include  "gpio.h"
#include  "usb_api.h"
#include  "extern.h"
#include  "parameter.h"
#include  "extern.c"
#include  "utility.h"

#include  "uistrings.h"
#include  "sysregs.h"
#include  "card.h"
#include  "vfs_api.h"

#include  "menu_capture.h"
#include  "panel.h"
/*****************************************************************************/
extern  void  usbRegCustomerVendorCommand(void);  // sun@1030 am 11:45 add

extern  UINT8  g_ui8EnterUpdateFlag;

extern  UINT32  file_filter;
/****** ********************************************************************
 *               C O N S T A N T    D E F I N I T I O N                   *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void  uiTask(void *);
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#define  C_4458_DEBUG    0
#define  C_FILE_DEBUG    0
#define  C_LEVI_DEBUG    0
#define  C_RS232_DEBUG   0
#define  C_SWITCH_DEBUG  0
#define  C_TEST_DEBUG    0
#define  C_USB_DEBUG     0

#define  C_AUTO       0
#define  C_MANUAL     1
#define  C_TEST_MODE  C_AUTO

#define  MY_CHANNEL  2

#define  MY_HIGH  0
#define  MY_LOW   1

#define  C_COM0  0
#define  C_COM1  1
#define  C_COM2  2
#define  C_COM3  3
#define  C_DEFAULT_COM  C_COM3

#define  PLC_MODULE_NO  300
#define  PLC_CLASS_NO   301

#define  PLC_BAKCMD   310
#define  PLC_BAKINFO  311

#define  succeessclear 460
#define  failclear     461

#define  PLC_REST_ROOM  320
#define  PLC_REST_DAY   321

#define  PLC_TIME  330

#define  PLCA_ALL_RESULT01  390

#define  PLC_PARA_BUF  800
#define  delay_time  700
#define  LoadCCSwitch  904
#define  next_delaytime  901
#define  Success_Count   900
#define  Test_Count      902
#define  C_TEST_DEBUG    903

#define  C_DL_HEADER_3_LEN  7
#define  TESTSTEP0  12
#define  TESTSTEP1  13
#define  TestMode  14

struct  PLC_DEVICE
{	
	UINT16  emptyVoltMin;
	UINT16  emptyVoltMax;
	UINT16  emptyPowerMin;
	UINT16  emptyPowerMax;
	UINT16  loadVoltMin;
	UINT16  loadVoltMax;
	UINT16  loadVAMin;
	UINT16  loadVAMax;
	UINT16  loadPowerMin;
	UINT16  loadPowerMax;
	UINT16  overloadVolMin;
	UINT16  overloadVolMax;	
}curDevice;

#define  C_PLC_PARA_NUM  (sizeof(struct PLC_DEVICE)/(sizeof(UINT16)))

struct  DELAY_TIME
{
	UINT16  step1delytime;
	UINT16  step2delytime;
	UINT16  step3delytime;
	UINT16  step4delytime;
	UINT16  step5delytime;	
	UINT16  step6delytime;
		
}delay;

#define  C_delay_NUM  (sizeof(struct DELAY_TIME)/(sizeof(UINT16)))

UINT32  uiTaskStack[2048];

UINT16  ui16CurModule = 0;
UINT16  ui16CurClass = 0;
UINT8    resultgetkey;

UINT8  ui8TermModule[] = "31-12103a";
UINT8  ui8TermName[] = "FA01";
static  UINT8  ui8DLHeader1[] = "31-1023完成品自动测试记录,";
static  UINT8  ui8DLHeader2[] = "时间,机号#,";
static  UINT8  ui8DLHeader3[C_DL_HEADER_3_LEN][15] = {  
	                                                    "带载电压(V),\0",
	                                                    "视在功率(VA),\0",
	                                                    "有效功率(W),\0",
	                                                    "过载电压(V),\0",	
	                                                    "空载电压(V),\0",
	                                                    "空载功率(W),\0",	                                                                                                       
	                                                    "结果,\0"
	                                                 };
static  UINT8  ui8DLHeader3_29_0004[C_DL_HEADER_3_LEN-1][15] = {  
	                                                    "充电电流(A),\0",
	                                                    "视在功率(VA),\0",
	                                                    "有效功率(W),\0",	                                                    
	                                                    "空载电压(V),\0",
	                                                    "空载功率(W),\0",	                                                                                                       
	                                                    "结果,\0"
	                                                 };	  
	                                                                                                
static  UINT8  ui8DLHeader3_31_05052[C_DL_HEADER_3_LEN][15] = {  
	                                                    "空载电压(V),\0",	                                                    
	                                                    "视在功率(VA),\0",
	                                                    "有效功率(W),\0",
	                                                    "带载电压(V),\0",
	                                                    "过载电流(A),\0",		                                                    
	                                                    "空载功率(W),\0",	                                                                                                       
	                                                    "结果,\0"
	                                                 };	
static  UINT8  ui8DLHeader3_31_12104[C_DL_HEADER_3_LEN][25] = {  
	                                                    "空载电压(V),\0",	
	                                                    "空载功率(W),\0", 
	                                                    "带载电压(V),\0",  
	                                                    "有效功率(W),\0",                                                 
	                                                    "视在功率(VA),\0",
	                                                    "输出电压(V),\0",	                                                                                          	                                                                                                       
	                                                     "结果,\0"
	                                                 };		                                                 
UINT8  ui8TimeFlag = 0;
UINT8  strbuf1[1024];
UINT8  *ui8DataBufDL = NULL;

UINT32  ui32SuccessNum = 0;

UINT32  ui32Time;
UINT32  ui32DelayTime1;
UINT32  ui32DelayTime2;
UINT32  ui32DelayTime3[MY_CHANNEL];
UINT32  ui32DelayTime4[MY_CHANNEL];
UINT32  ui32DelayTime5[MY_CHANNEL];
UINT8   Jump_case=0;
UINT32  ui32QueWaitTime = 0;


#define	C_MODULE_31_12103 	0      //31-12103A
#define	C_MODULE_31_12100 	1      //31-12103B
#define	C_MODULE_31_05052 	2
#define	C_MODULE_31_05082 	3
#define	C_MODULE_29_0004  	4
#define	C_MODULE_31_05053 	5
#define	C_MODULE_31_05104 	6
#define	C_MODULE_31_12104 	7
#define	C_MODULE_31_05211 	8
//==========================================================================
#include  "my4458.c"
#include  "myHmi.c"
#include  "mySub.c"
/**************************************************************************/

/**************************************************************************/
void  getModuleName(UINT8 model, UINT8* name)
{
	switch(model)
	{
		case  C_MODULE_31_12103:          //31-12103A
			strcpy(name, "31-12103");
			break;	
		case  C_MODULE_31_12100:          //31-12103B
			strcpy(name, "31-12100");
			break;		
		case  C_MODULE_31_05052:
			strcpy(name, "31-05052");
			break;
		case  C_MODULE_31_05082:
			strcpy(name, "31-05082");
			break;		
		case  C_MODULE_29_0004:
			strcpy(name, "29-0004");
			break;	
		case  C_MODULE_31_05053:
			strcpy(name, "31-05053");
			break;	
		case  C_MODULE_31_05104:
			strcpy(name, "31-05104");
			break;	
		case  C_MODULE_31_12104:
			strcpy(name, "31-12104");
			break;
		case  C_MODULE_31_05211:
			strcpy(name, "31-05211");
			break;		
		default:
			name[0] = '\0';
			break;
	}
}

UINT32  decStrToi(UINT8 *str)
{
	UINT8  ch;
	
	UINT16  i;
	
	UINT32  val;
	
	val = 0;
	for(i=0;i<strlen(str);i++)
	{
		ch = str[i];
		if(ch==' ')
			continue;
		val *= 10;
		val += ch-'0';
	}
	return  val;
}

void  readPara()
{
	UINT8  file[20];
	UINT8  num[8];
	UINT8  *buf;
	
	UINT16  i,j,k;
	UINT16  *ptr;
	
	UINT32  pos,size;
	UINT32  err = SUCCESS;
	UINT32  data[C_PLC_PARA_NUM];
	
	SINT32  fd = 0;
	
	xdcfAttrElm_t  xdcfFileAttr1;
	
	ptr = (UINT16 *)&curDevice;
	
	xdcfCurRootDirSet(ui8TermModule);
	sio_psprintf(file, "%s.CSV", ui8TermModule);
	pos = myFindFile(file);		
	
	if(pos==0)
	{
		#if  C_FILE_DEBUG
		    printf("not find module file  ");
		#endif
		if(ui16CurModule==C_MODULE_31_12103)
		{
      curDevice.emptyVoltMin = 11637;     
      curDevice.emptyVoltMax = 12862;     
      curDevice.emptyPowerMin = 0;        
      curDevice.emptyPowerMax = 100;      
      curDevice.loadVoltMin = 11400;      
      curDevice.loadVoltMax = 12600;      
      curDevice.loadVAMin = 19200;        
      curDevice.loadVAMax = 28800;        
      curDevice.loadPowerMin = 8000;     
      curDevice.loadPowerMax = 12000;     
      curDevice.overloadVolMin = 0;       
      curDevice.overloadVolMax = 2000;	
      //printf("readparauiHMIWord[LoadCCSwitch]=%d   ",uiHMIWord[LoadCCSwitch]); 
		}
		if(ui16CurModule==C_MODULE_31_12100)        
		{                                   
		  curDevice.emptyVoltMin = 11637;   
		  curDevice.emptyVoltMax = 12862;   
		  curDevice.emptyPowerMin = 0;      
		  curDevice.emptyPowerMax = 300;    
		  curDevice.loadVoltMin = 11400;    
		  curDevice.loadVoltMax = 12600;    
		  curDevice.loadVAMin = 24000;      
		  curDevice.loadVAMax = 36000;      
		  curDevice.loadPowerMin = 12000;   
		  curDevice.loadPowerMax = 18000;	  	
      curDevice.overloadVolMin = 0;     
      curDevice.overloadVolMax = 2000;		      
    }	
    if(ui16CurModule==C_MODULE_31_12104)        
		{                                   
		  curDevice.emptyVoltMin = 11637;   
		  curDevice.emptyVoltMax = 12862;   
		  curDevice.emptyPowerMin = 0;      
		  curDevice.emptyPowerMax = 170;    
		  curDevice.loadVoltMin = 11400;    
		  curDevice.loadVoltMax = 12600;    
		  curDevice.loadVAMin = 19200;      
		  curDevice.loadVAMax = 28800;      
		  curDevice.loadPowerMin = 8000;   
		  curDevice.loadPowerMax = 12000;	  	
      curDevice.overloadVolMin = 11637;          //OUTPUT VOLTAGE
      curDevice.overloadVolMax = 12862;		      
    }	
    if(ui16CurModule==C_MODULE_31_05082)        
		{                                   
		  curDevice.emptyVoltMin =  4750;
      curDevice.emptyVoltMax =  5250;
      curDevice.emptyPowerMin = 0;
      curDevice.emptyPowerMax = 300;
      curDevice.loadVoltMin = 4750;
      curDevice.loadVoltMax = 5250;
      curDevice.loadVAMin = 5500;
      curDevice.loadVAMax = 15000;
      curDevice.loadPowerMin= 4750;
      curDevice.loadPowerMax= 7000;
      curDevice.overloadVolMin= 730;
      curDevice.overloadVolMax= 850;		      
    }	
    if(ui16CurModule==C_MODULE_31_05052)        
		{                                   
		  curDevice.emptyVoltMin =  4750;
      curDevice.emptyVoltMax =  5250;
      curDevice.emptyPowerMin = 0;
      curDevice.emptyPowerMax = 300;
      curDevice.loadVoltMin = 4750;
      curDevice.loadVoltMax = 5250;
      curDevice.loadVAMin = 5500;
      curDevice.loadVAMax = 15000;
      curDevice.loadPowerMin= 3000;
      curDevice.loadPowerMax= 6000;
      curDevice.overloadVolMin= 510;
      curDevice.overloadVolMax= 700;		      
    }
    if(ui16CurModule==C_MODULE_31_05053)        
		{                                   
		  curDevice.emptyVoltMin =  4750;
      curDevice.emptyVoltMax =  5250;
      curDevice.emptyPowerMin = 0;
      curDevice.emptyPowerMax = 300;
      curDevice.loadVoltMin = 4750;
      curDevice.loadVoltMax = 5250;
      curDevice.loadVAMin = 5500;
      curDevice.loadVAMax = 15000;
      curDevice.loadPowerMin= 3000;
      curDevice.loadPowerMax= 6000;
      curDevice.overloadVolMin= 510;
      curDevice.overloadVolMax= 700;		      
    }
    if(ui16CurModule==C_MODULE_31_05104)        
		{                                   
		  curDevice.emptyVoltMin =  4750;
      curDevice.emptyVoltMax =  5250;
      curDevice.emptyPowerMin = 0;
      curDevice.emptyPowerMax = 300;
      curDevice.loadVoltMin = 4750;
      curDevice.loadVoltMax = 5250;
      curDevice.loadVAMin = 9600;
      curDevice.loadVAMax = 14400;
      curDevice.loadPowerMin= 7000;
      curDevice.loadPowerMax= 8000;
      curDevice.overloadVolMin= 1100;
      curDevice.overloadVolMax= 1400;		      
    }
    if(ui16CurModule==C_MODULE_31_05211)        
		{                                   
		  curDevice.emptyVoltMin =  4750;
      curDevice.emptyVoltMax =  5250;
      curDevice.emptyPowerMin = 0;
      curDevice.emptyPowerMax = 300;
      curDevice.loadVoltMin = 4750;
      curDevice.loadVoltMax = 5250;
      curDevice.loadVAMin = 9600;
      curDevice.loadVAMax = 36400;
      curDevice.loadPowerMin= 12000;
      curDevice.loadPowerMax= 18000;
      curDevice.overloadVolMin= 2200;
      curDevice.overloadVolMax= 2600;		      
    }
    if(ui16CurModule==C_MODULE_29_0004)        
		{                                   
		  curDevice.emptyVoltMin =  850;//charge cur
      curDevice.emptyVoltMax =  950;
      curDevice.emptyPowerMin = 0;
      curDevice.emptyPowerMax = 300;
      curDevice.loadVoltMin = 8380;
      curDevice.loadVoltMax = 8420;
      curDevice.loadVAMin = 19200;
      curDevice.loadVAMax = 28800;
      curDevice.loadPowerMin= 9000;
      curDevice.loadPowerMax= 11000;
      curDevice.overloadVolMin= 510;
      curDevice.overloadVolMax= 700;		      
    }	
	}
	else
	{
		#if  C_FILE_DEBUG
		    printf("find module fine %s  ",file);
		#endif
		xdcfCurFileByPosSet(pos);
		err = xdcfCurFileAttrGet(&xdcfFileAttr1);
		fd = vfsOpen(xdcfFileAttr1.name, O_RDWR, S_IREAD);
		if(!fd)
		{
			#if  C_FILE_DEBUG
			    printf("open file %s error  ",xdcfFileAttr1.name);
			#endif
			return;
		}
		else
		{
			size = vfsFileSizeGet(fd);
			buf = osMemAlloc(size);
			if(!buf)
			{
				#if  C_FILE_DEBUG
				    printf("alloc DL buf error  ");
				#endif
				return;
			}
			vfsRead(fd, buf, size+1);
			k = 0;
			for(i=0;i<size;i++)
			{
				if(buf[i]=='=')
				{
					for(j=0,i++;buf[i]!=0x0d;i++,j++)
					    num[j] = buf[i];
					num[j] = '\0';
					data[k] = decStrToi(num);
					#if  C_FILE_DEBUG
					    printf("read parameter %d=%s=%d,  ",k+1,num,ptr[k]);
					#endif
					k++;
					if(C_MODULE_29_0004!=ui16CurModule)
					{
						if(k>=C_PLC_PARA_NUM)
						break;
					}
					else
					{
						if(k>=C_PLC_PARA_NUM-2)
						break;
					}
				}
			}
			if(C_MODULE_29_0004!=ui16CurModule)
			{
				for(i=0;i<C_PLC_PARA_NUM;i++)
			    ptr[i] = data[i];
			}
			else
			{
				for(i=0;i<C_PLC_PARA_NUM;i++)
			    ptr[i] = data[i];
			}
			vfsClose(fd);
		}
	}
	memcpy((UINT8 *)&uiHMIWord[PLC_PARA_BUF], (UINT8 *)&curDevice, C_PLC_PARA_NUM*sizeof(UINT16));
	
	#if  C_FILE_DEBUG
	    printf("form file %s read parameter ok  ",xdcfFileAttr1.name);
	#endif
}

void  writePara()
{
	//writeInfo[C_PLC_PARA_NUM][30]
	//if(C_MODULE_29_0004!=ui16CurModule)
	//{
		UINT8  writeInfo[C_PLC_PARA_NUM][30] = {  "空载电压下限(mV)=",
		                                      "空载电压上限(mV)=",
		                                      "空载功率下限(mW)=",
		                                      "空载功率上限(mW)=",
		                                      "带载电压下限(mV)=",
		                                      "带载电压上限(mV)=",
		                                      "视在功率下限(mW)=",
		                                      "视在功率上限(mW)=",
		                                      "有效功率下限(mW)=",
		                                      "有效功率上限(mW)=",
		                                      "过载电压下限(mV)=",
		                                      "过载电压上限(mV)=",		                                      
		                                   };
		UINT8  writeInfo2[C_PLC_PARA_NUM][30] = {  "空载电压下限(mV)=",
		                                      "空载电压上限(mV)=",
		                                      "空载功率下限(mW)=",
		                                      "空载功率上限(mW)=",
		                                      "带载电压下限(mV)=",
		                                      "带载电压上限(mV)=",
		                                      "视在功率下限(mW)=",
		                                      "视在功率上限(mW)=",
		                                      "有效功率下限(mW)=",
		                                      "有效功率上限(mW)=",
		                                      "过载电流下限(mA)=",
		                                      "过载电流上限(mA)=",		                                      
		                                   };                                   
		                                   
	//}
//	else
//	{
		UINT8  writeInfo1[C_PLC_PARA_NUM-2][30] = {  "充电电流下限(mA)=",
		                                      "充电电流上限(mA)=",
		                                      "空载功率下限(mW)=",
		                                      "空载功率上限(mW)=",
		                                      "带载电压下限(mV)=",
		                                      "带载电压上限(mV)=",
		                                      "视在功率下限(mW)=",
		                                      "视在功率上限(mW)=",
		                                      "有效功率下限(mW)=",
		                                      "有效功率上限(mW)=",		                                      	                                      
		                                   };
	//}
	
	UINT8  file[20];
	
	UINT16  i;
	UINT16  *ptr;
	
	UINT32  pos;
	UINT32  err = SUCCESS;
	
	SINT32  fd = 0;

	xdcfAttrElm_t  xdcfFileAttr1;
	
	
	xdcfCurRootDirSet(ui8TermModule);
	sio_psprintf(file, "%s.CSV", ui8TermModule);
	
	pos = myFindFile(file);		
	
	if(pos)
	{
		err = xdcfCurFileByPosSet(pos);
		err = xdcfCurFileAttrGet(&xdcfFileAttr1);
		xdcfOneFileErase(xdcfFileAttr1.name);
	}
	
	fd = vfsOpen(file, O_CREATE, 0);
	if(!fd)
	{
		#if  C_FILE_DEBUG
		    printf("create file %s fail  ",file);
		#endif
		return;
	}
	else
	{
		ptr = (UINT16 *)&curDevice;
		if(C_MODULE_29_0004!=ui16CurModule)
		{
			if((C_MODULE_31_12100==ui16CurModule)||(C_MODULE_31_12103==ui16CurModule)||(C_MODULE_31_12104==ui16CurModule))
			{
				for(i=0;i<C_PLC_PARA_NUM;i++)
		    {		  	
		  	  sio_psprintf(strbuf1, "%s%d\r\n", writeInfo[i], *ptr++);
		  	  vfsWrite(fd, strbuf1, strlen(strbuf1));
		    }
			}
			else
			{
				for(i=0;i<C_PLC_PARA_NUM;i++)
		    {		  	
		  	  sio_psprintf(strbuf1, "%s%d\r\n", writeInfo2[i], *ptr++);
		  	  vfsWrite(fd, strbuf1, strlen(strbuf1));
		    }
			}			
		}
		else
		{
			for(i=0;i<C_PLC_PARA_NUM-2;i++)
		  {
		  	sio_psprintf(strbuf1, "%s%d\r\n", writeInfo1[i], *ptr++);
		  	vfsWrite(fd, strbuf1, strlen(strbuf1));
		  }
		}
		
		vfsClose(fd);
	}	
	
	#if  C_FILE_DEBUG
	    printf("write parameter to file %s ok  ",file);
	#endif
}

//**********************************************************************************
//DelayTime save parame
void  readParadelaytime()
{
	UINT8  file[20];
	UINT8  num[8];
	UINT8  *buf;
	
	UINT16  i,j,k;
	UINT16  *ptr;
	
	UINT32  pos,size;
	UINT32  err = SUCCESS;
	UINT32  data[C_delay_NUM];
	
	SINT32  fd = 0;
	
	xdcfAttrElm_t  xdcfFileAttr1;
	
	ptr = (UINT16 *)&delay;
	xdcfCurRootDirSet(ui8TermModule);
		
	//UINT8  ui8Term[] = "StepDelayTime";
	sio_psprintf(file, "%sTM.CSV", ui8TermModule);
	
	pos = myFindFile(file);
	#if  C_FILE_DEBUG
	printf(" ==readpara:%s,pos=%d== ",ui8TermModule,pos);
	#endif
	if(pos==0)
	{
		#if  C_FILE_DEBUG
		    printf("not find module file  ");
		#endif
	  if(ui16CurModule==C_MODULE_31_12103)
	  {
	  	delay.step1delytime=250;
	    delay.step2delytime=10;
	    delay.step3delytime=0;
	    delay.step4delytime=200;
	    delay.step5delytime=100;
	    delay.step6delytime=600;
	  }
		if(ui16CurModule==C_MODULE_31_12100)
	  {
	  	delay.step1delytime=250;
	    delay.step2delytime=10;
	    delay.step3delytime=0;
	    delay.step4delytime=200;
	    delay.step5delytime=100;
	    delay.step6delytime=600;
	  }	
	  if(ui16CurModule==C_MODULE_31_12104)
	  {
	  	delay.step1delytime=250;
	    delay.step2delytime=10;
	    delay.step3delytime=0;
	    delay.step4delytime=200;
	    delay.step5delytime=100;
	    delay.step6delytime=1000;
	  }	
	  if(ui16CurModule==C_MODULE_31_05052)
	  {
	  	delay.step1delytime=500;
	    delay.step2delytime=500;
	    delay.step3delytime=0;
	    delay.step4delytime=50;
	    delay.step5delytime=500;
	    delay.step6delytime=50;
	  }
	  if(ui16CurModule==C_MODULE_31_05082)
	  {
	  	delay.step1delytime=500;
	    delay.step2delytime=500;
	    delay.step3delytime=0;
	    delay.step4delytime=50;
	    delay.step5delytime=500;
	    delay.step6delytime=50;
	  }
	  if(ui16CurModule==C_MODULE_29_0004)
	  {
	  	delay.step1delytime=600;
	    delay.step2delytime=10;
	    delay.step3delytime=0;
	    delay.step4delytime=200;
	    delay.step5delytime=600;
	    delay.step6delytime=600;
	  }
	  if(ui16CurModule==C_MODULE_31_05053)
	  {
	  	delay.step1delytime=500;
	    delay.step2delytime=500;
	    delay.step3delytime=0;
	    delay.step4delytime=50;
	    delay.step5delytime=500;
	    delay.step6delytime=50;
	  }
	  if(ui16CurModule==C_MODULE_31_05104)
	  {
	  	delay.step1delytime=500;
	    delay.step2delytime=500;
	    delay.step3delytime=0;
	    delay.step4delytime=50;
	    delay.step5delytime=500;
	    delay.step6delytime=50;
	  }
	  if(ui16CurModule==C_MODULE_31_05211)
	  {
	  	delay.step1delytime=500;
	    delay.step2delytime=500;
	    delay.step3delytime=0;
	    delay.step4delytime=50;
	    delay.step5delytime=500;
	    delay.step6delytime=50;
	  }
	 //printf("delay.step1delytime   ");
	}
	else
	{
		#if  C_FILE_DEBUG
		    printf("find module fine %s  ",file);
		#endif
		xdcfCurFileByPosSet(pos);
		err = xdcfCurFileAttrGet(&xdcfFileAttr1);
		fd = vfsOpen(xdcfFileAttr1.name, O_RDWR, S_IREAD);
		if(!fd)
		{
			#if  C_FILE_DEBUG
			    printf("open file %s error  ",xdcfFileAttr1.name);
			#endif
			return;
		}
		else
		{
			size = vfsFileSizeGet(fd);
			buf = osMemAlloc(size);
			if(!buf)
			{
				#if  C_FILE_DEBUG
				    printf("alloc DL buf error  ");
				#endif
				return;
			}
			vfsRead(fd, buf, size+1);
			k = 0;
			for(i=0;i<size;i++)
			{
				if(buf[i]=='=')
				{
					for(j=0,i++;buf[i]!=0x0d;i++,j++)
					    num[j] = buf[i];
					num[j] = '\0';
					data[k] = decStrToi(num);
					#if  C_FILE_DEBUG
					    printf("read parameter %d=%s=%d,  ",k+1,num,ptr[k]);
					#endif
					k++;
					if(k>=C_delay_NUM)
						break;
				}
			}
			for(i=0;i<C_delay_NUM;i++)
			    ptr[i] = data[i];
			vfsClose(fd);
		}
	}
	memcpy((UINT8 *)&uiHMIWord[delay_time], (UINT8 *)&delay, C_delay_NUM*sizeof(UINT16));
/*	printf("structdelyatime1=%d     ",delay.step1delytime);
	printf("structdelyatime2=%d     ",delay.step2delytime);
  printf("structdelyatime3=%d     ",delay.step3delytime);
	printf("structdelyatime4=%d     ",delay.step4delytime);
	printf("structdelyatime5=%d     ",delay.step5delytime);
	printf("structdelyatime6=%d     ",delay.step6delytime);*/
	
	#if  C_FILE_DEBUG
	    printf(" ==end readpara %,UIHMI[700]=%d == ",ui8TermModule,uiHMIWord[700]);
	    printf("form file %s read parameter ok  ",xdcfFileAttr1.name);
	#endif
}

void  writeParadelaytime()
{
	UINT8  writeInfo[C_delay_NUM][30] = {  
		                                      "STEP1延时(ms)=",
		                                      "STEP2延时(ms)=",		                                      
		                                      "STEP3延时(ms)=",		                                     
		                                      "STEP4延时(ms)=",
		                                      "STEP5延时(ms)=",		                                      
		                                      "STEP6延时(ms)=", 	                                      
		                                   };
	UINT8  file[20];
	
	UINT16  i;
	UINT16  *ptr;
	
	UINT32  pos;
	UINT32  err = SUCCESS;
	
	SINT32  fd = 0;
	
	xdcfAttrElm_t  xdcfFileAttr1;
	
	xdcfCurRootDirSet(ui8TermModule);
	
	//UINT8  ui8Term[] = "StepDelayTime";
	sio_psprintf(file, "%sTM.CSV", ui8TermModule);
	
	pos = myFindFile(file);
	#if  C_FILE_DEBUG
	printf(" ==writepara:%s,pos=%d == ",ui8TermModule,pos);
	#endif
	if(pos)
	{
		err = xdcfCurFileByPosSet(pos);
		err = xdcfCurFileAttrGet(&xdcfFileAttr1);
		xdcfOneFileErase(xdcfFileAttr1.name);
	}
	
	fd = vfsOpen(file, O_CREATE, 0);
	if(!fd)
	{
		#if  C_FILE_DEBUG
		    printf("create file %s fail  ",file);
		#endif
		return;
	}
	else
	{
		ptr = (UINT16 *)&delay;
		for(i=0;i<C_delay_NUM;i++)
		{
			sio_psprintf(strbuf1, "%s%d\r\n", writeInfo[i], *ptr++);
			vfsWrite(fd, strbuf1, strlen(strbuf1));
		}
		vfsClose(fd);
	}			
	#if  C_FILE_DEBUG
	    printf(" ==end wirtepara %,UIHMI[700]=%d == ",ui8TermModule,uiHMIWord[700]);
	    printf("write parameter to file %s ok  ",file);
	#endif
}


//**********************************************************************************
/**************************************************************************
 *                                                                        *
 *  Function Name: keyGet                                                 *
 *                                                                        *
 *  Purposes: get the key button function.                                *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    pKey: out, key button value                                         *
 *                                                                        *
 *  Returns: This function should never return.                           *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void  keyGet(UINT32 *pKey)
{
	UINT32  err;
	UINT32  *msg;
	
	msg = osQuePend(uiKeyMsgQ, ui32QueWaitTime, &err);
	
	*pKey = *msg;
	#if  C_USB_DEBUG
	    #if  DBG_UI
	        printf("*msg=%08x  ",*msg);
	    #endif
	#endif
	
	#if  0  // xouer080907
	    if(*pKey!=UI_KEY_FUNC_FOCU)
	    {
	    	if(/*((sPara.uiBeepFlag==BeepShutter)&&(*pKey==UI_KEY_FUNC_SNAP))||*/(sPara.uiBeepFlag==BeepOn))
	    	{
	    		if((*pKey==UI_KEY_DIR_UP)||(*pKey==UI_KEY_DIR_DOWN)||(*pKey==UI_KEY_DIR_LEFT)||(*pKey==UI_KEY_DIR_RIGHT))
	    		{
	    			if((mp3Disp!=PB_DISP_ONE)&&(pbDisp!=PB_DISP_ONE))
	    			{
	    				buttonAudio(1);
	    			}
	    			else if((pbDisp==PB_DISP_ONE)&&(file_filter==FILTER_PHOTO))
	    			{
	    				buttonAudio(1);
	    			}
	    		}
	    		else if(*pKey==UI_KEY_FUNC_OK)
	    		{
	    			buttonAudio(2);
	    		}
	    		else if((*pKey==UI_KEY_FUNC_MENU)||(*pKey==UI_KEY_FUNC_MODE))
	    		{
	    			buttonAudio(3);
	    		}
	    	}
	    }
	#endif
}

void  uiUsbMode(void)
{
	UINT32  key;
	
	#if  1  // fqdao_add for del dir 06.5.6
	    xdcfInit(imageDirNameStr, DCNameStr, xDCF_CONFIG_KEY_MIN|xDCF_CONFIG_SORT_IDX);
	    dirInit();
	#endif
	usbRegCustomerVendorCommand();  // sun@1030 am 11:45 add
	
	#if  C_USB_DEBUG
	    printf("select USB mode  ");
	#endif
	lcdOsdDisp = 0;  // Open lcd
	OpenLcdDisplay(1);
	
	UI_OSQFlush(uiKeyMsgQ);  // Pual@20051201
	
	osTaskSuspend(osTaskFindByName("HISTOGRAM"));
	osTaskSuspend(osTaskFindByName("AAA"));
	hwDispOsdEn(1);
	
	osdClearScreen(0x07);
	
	uiUSBStatus = USB_FORCE_MODE;
	usbChangeMode(sPara.USBModeFlag);
	
	osdClearScreen(0x07);
	if(uiUSBStatus==USB_FORCE_MODE)
	{
		#if  C_USB_DEBUG
		    printf("enter USB mode = %x  ",sPara.USBModeFlag);
		#endif
		gpioConfig(GPIO_USB_OE, 1);
		gpioSet(GPIO_USB_OE, 1);
		usbChangeMode(sPara.USBModeFlag);
		
		if(sPara.USBModeFlag==USB_MODE_PCCAM)
		{
			if(!g_ui8EnterUpdateFlag)
			{
				#if  C_USB_DEBUG
				    printf("pc camera  ");
				#endif
				gpioConfig(GPIO_SENSOR_PWDN, 1);
				gpioSet(GPIO_SENSOR_PWDN, 0);
				gpioConfig(GPIO_SENSOR_RST, 1);
				gpioSet(GPIO_SENSOR_RST, 0);
				aaaInit();
				WRITE8(0xb0000007, 0x7f);
				gpioConfig(GPIO_SENSOR_POWER, 1);  // open sensor power
				gpioSet(GPIO_SENSOR_POWER, 1);
				osdStrDisp(94, 100, UserFont10x20, 0x07, dc_GetString(Pccam_s));
				
				hwTvSensorModeSel(0);
				osTaskResume(osTaskFindByName("AAA"));
				hwDispPvEn(1);
				PNL_PreviewSet();
				hwDispPipEn(0);
				zoomFactSet(zoomFactGet());
				hwPVZoom(pvZoomFactor);
				aaaEvCmpIdxSet(pvEvCmpIdx);
			}
			else
			{
				#if  C_USB_DEBUG
				    printf("software update  ");
				#endif
				osdStrDisp(94, 100, UserFont10x20, 0x07, GetString(MSDC_S));  // tzw0918
			}
		}
		else if(sPara.USBModeFlag==USB_MODE_MSDC)
		{
			osdStrDisp(134, 120, UserFont10x20, 0x07, dc_GetString(Msdc_s));
			osdStrDisp(100, 100, UserIcon10x20, 0xd47, "\x78\x79");
		}
	}
	
	while((ui32NextState&UI_MODE_MASK)==UI_MODE_USB)
	{
		keyGet(&key);  // wait exit message
	}
	
	#if  C_USB_DEBUG
	    printf("exit USB  ");
	#endif
	g_ui8EnterUpdateFlag = 0;  // 2006.05.13
	
	osTaskSuspend(osTaskFindByName("AAA"));
	PNL_PlaybackSet();
	
	#if  SENSOR_PWR_SET
	    gpioConfig(GPIO_SENSOR_POWER, 1);
	    gpioSet(GPIO_SENSOR_POWER, 0);
	#else
	    gpioConfig(GPIO_SENSOR_PWDN, 1);
	    gpioSet(GPIO_SENSOR_PWDN, 1);
	#endif
	
	hwDispEn(1);
	
	UI_OSQFlush(uiKeyMsgQ);  // Paul@2005.12.01
	
	SetTDLcdParameter();
	
	uiState = ui32NextState;
	
	return;
}

void  initTgGpio()
{
	UINT32  addr;
	UINT32  status;
	
	addr = 0xb0000000+TG_GPIO_SL;
	
	ENTER_CRITICAL(status);
	WRITE32(addr, 0xffffffff);
	WRITE32(addr+4, 0xffff);
	EXIT_CRITICAL(status);
}
/**************************************************************************
 *                                                                        *
 *  Function Name: uiInit                                                 *
 *                                                                        *
 *  Purposes: UI initialize function.                                     *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: This function should never return.                           *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void  uiInit(void)
{
	UINT16  i;
	
	//UINT8  ui8Kinit[2] = {1,0};
	UINT8  ui8Kinit[4] = {0,0,0,0};
	for(i=0;i<C_WORD_LENGHT;i++)
	    uiHMIWord[i] = 0;
	for(i=0;i<1024;i++)
	    ucHMIBit[i] = 0;
	
	uiUart1RxPtr = 0;
	
	gpioConfig(GPIO_HA, 1);
	gpioConfig(GPIO_HB, 1);
	F_Set232Port(C_DEFAULT_COM, 115200);
	for(i=0;i<4*8;i+=4)
	{
		gpioSet(uisGpioKey[i+2], MY_HIGH);
		gpioSet(uisGpioKey[i+3], MY_HIGH);
		gpioConfig(uisGpioKey[i+2], 1);
		gpioConfig(uisGpioKey[i+3], 1);
		gpioConfig(uisGpioKey[i+0], 0);
		gpioConfig(uisGpioKey[i+1], 0);
	}
	for(i=0;i<sizeof(uisGpioK)/sizeof(UINT16);i++)
	{
		mySetGpio(uisGpioK[i], ui8Kinit[i]);
		gpioConfig(uisGpioK[i], 1);
	}
	for(i=0;i<sizeof(uisGpioQ)/sizeof(UINT16);i++)
	{
		mySetGpio(uisGpioQ[i], 1);
		gpioConfig(uisGpioQ[i], 0);
	}
	
	uiHMIWord[Success_Count]=3;
	uiHMIWord[next_delaytime]=20;
	uiHMIWord[Test_Count]=45;
	uiHMIWord[C_TEST_DEBUG]=0;
	/*delay.step1delytime=250;
	delay.step2delytime=250;
	delay.step3delytime=250;
	delay.step4delytime=200;
	delay.step5delytime=100;
	delay.step6delytime=800;*/
	memcpy((UINT8 *)&uiHMIWord[delay_time], (UINT8 *)&delay, C_delay_NUM*sizeof(UINT16));
	initTgGpio();
	
	UINT8  ui8EBookEXPName[4] = "CSV";
	UINT8  ui8EBookTempExp1[4] = "tmp";
	UINT8  ui8EBookTempExp2[4] = "emx";
	
	UINT32  ui32FileFilter = xDCF_FILETYPE_RESERVE2;  // 将保留2设置其为"txt"类型
	
	xdcfActiveDevIdSet(DRIVE_NAND);
	xdcfFileTypeAdd(ui8EBookTempExp1);  //128
	xdcfFileTypeAdd(ui8EBookTempExp2);  //256
	xdcfFileTypeAdd(ui8EBookEXPName);  //512  相当于 1<<9 (xDCF_FILETYPE_RESERVE2)
	xdcfInit(imageDirNameStr, imageRootNameStr, 0/*|xDCF_CONFIG_DCF_ONLY*/);
	xdcfFilterSet(ui32FileFilter);  //ui32FileFilter = xDCF_FILETYPE_RESERVE2
	
	myGetDevFreeSize();
	ui16CurModule = 0;
	ui16CurClass = 0;
	getModuleName(ui16CurModule, ui8TermModule);
	readPara();
	
	ui8DataBufDL = osMemAlloc(4*1024);
	if(!ui8DataBufDL)
	{
		#if  C_FILE_DEBUG
		    printf("alloc DL buf error  ");
		#endif
		ui8DataBufDL = NULL;
	}
	else
	{
		ui8DataBufDL[0] = '\0';
	}
	
	uiKeyMsgQ = osEventFindByName("UI_KEY");
	osTaskCreate("UITASK", uiTask, (void *)2048, (void *)(&uiTaskStack[2047]), 16);
}

UINT8 MANUAL_TESTMODE(UINT8 step)
{	
	UINT16 j;
	UINT8 switchteststep,i;
	Jump_case=0;
	if(test_mode==1)
		{                                                                			
			if((test_step0!=0)&&(test_step1==0))                                              			
			 {                                                             			
			 	j=test_step0;		 			 	                                                			
			 	for(switchteststep=1;(j=j/2)!=0;switchteststep++);           			
			 	if(switchteststep==step)  Jump_case=1;             			
			 	if(switchteststep<step)                      			
			 		{                                                          			
			 			uis8TestStep[ui8Chnl]=0;
			 			F_DefaultQ(0);
			 			for(i=0;i<PLCA_DATA_CNT;i++)
		    		    uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;
		    		for(i=0;i<PLCS_RESULT_CNT;i++)
		    		    uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;
		    		uis8ErrCount[ui8Chnl] = 0;
		    		uis8DataCnt[ui8Chnl] = 1;                                 			
			 			Jump_case=1;                                                   			
			 		}                                                          			
			 }			                                                     			
			if((test_step1!=0)&&(test_step0==0))                                              			
			 {                                                             			
			 	j=test_step1;		 			 	                                                			
			 	for(switchteststep=17;(j=j/2)!=0;switchteststep++);           			
			 	if(switchteststep==step)  Jump_case=1;             			
			 	if(switchteststep<step)                      			
			 		{                                                          			
			 			uis8TestStep[ui8Chnl]=0;
			 			F_DefaultQ(0);
			 			for(i=0;i<PLCA_DATA_CNT;i++)
		    		    uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;
		    		for(i=0;i<PLCS_RESULT_CNT;i++)
		    		    uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;
		    		uis8ErrCount[ui8Chnl] = 0;
		    		uis8DataCnt[ui8Chnl] = 1;                                 			
			 			Jump_case=1;                                                   			
			 		}                                                          			
			 }
			if((test_step0==0)&&(test_step1==0))
			 {
			 	uis8TestStep[ui8Chnl]=0;
			 	F_DefaultQ(0);
			 	for(i=0;i<PLCA_DATA_CNT;i++)
		    		uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;
		    for(i=0;i<PLCS_RESULT_CNT;i++)
		    		uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;
		    uis8ErrCount[ui8Chnl] = 0;
		    uis8DataCnt[ui8Chnl] = 1;                             						 			                                 			
			 	Jump_case=1;
			 }                                                             			
		}
	return Jump_case;                                                                			
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uiTask                                                 *
 *                                                                        *
 *  Purposes:                                                             *
 *    Task for UI process                                                 *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    pdata: in, task's replicated argument.                              *
 *                                                                        *
 *  Returns: This function should never return.                           *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 
void  uiTask(void *pdata)
{
	UINT16  ui32Nibble[MY_CHANNEL];
	
	UINT32  key;
	UINT32  ui32Value;	
	UINT16   j;
	UINT8    switchteststep,i;
	UINT8    successcount[MY_CHANNEL];
	UINT8    testcount[MY_CHANNEL];
	UINT8    result;	
	UINT8    channel=0;
	while(1)
	{
		ui32Time = tmrUsTimeGet();
		if(ui32Time>=5000000)
			break;
	}
	//uiHMIWord[LoadCCSwitch]=0;
	ui8Chnl = 0;
	ui32DelayTime1 = 0;
	ui32DelayTime2 = 0;
	for(ui8Chnl=0;ui8Chnl<MY_CHANNEL;ui8Chnl++)
	{
		bChnlFlag[ui8Chnl] = 0;
		uis8TestStep[ui8Chnl] = 255;
		ui32DelayTime3[ui8Chnl] = 0;
		ui32DelayTime4[ui8Chnl] = 0;
		ui32DelayTime5[ui8Chnl] = 0;
	}
	
	//for(ui8Chnl=0;ui8Chnl<MY_CHANNEL;ui8Chnl++)
	//{
		sendEl232Cmd(C_IT8500_REMOTE);
		sendEl232Cmd(C_IT8500_CLOSE);
		//F_SetSW2(0);//ADD.LIU@101119@8:17
		//sendTti232Cmd(C_TTI1906_REMOTE);//ADD.LIU@101119@8:17
		//sendTti232Cmd(C_TTI1906_RANGE_3);//ADD.LIU@101119@8:17
		//sendTti232Cmd(C_TTI1906_MODE_DCV);//ADD.LIU@101119@8:17
	//}
	uiHMIWord[Success_Count]=2;
	uiHMIWord[next_delaytime]=10000;
	uiHMIWord[Test_Count]=45;
	uiHMIWord[C_TEST_DEBUG]=0;
	while(1)
	{
		if(flagsource==0)
		{
			F_DealUrgent0();
			if((resultgetkey=F_GetKey(1))==0)
				{
					F_DealUrgent1();
				}
			ui8Chnl=0;
		}		
		if(flagsource==1)
		{
			F_DealUrgent1();
			if((resultgetkey=F_GetKey(0))==0)
				{
					F_DealUrgent0();
				}
			ui8Chnl=1;
		}
		if((uis8TestStep[0]==255)&&(uis8TestStep[1]==255))
		{
		   sendEl232Cmd(C_IT8500_REMOTE);		
			 sendEl232Cmd(C_IT8500_CLOSE);
		}
		commLevi();	
		//for(ui8Chnl=0;ui8Chnl<MY_CHANNEL;ui8Chnl++)
		//{
		if(C_MODULE_31_12104!=ui16CurModule)
		{
			switch(uis8TestStep[ui8Chnl])
			{
				case  0://设置状态
					sendEl232Cmd(C_IT8500_REMOTE);		
					sendEl232Cmd(C_IT8500_CLOSE);
		      if(test_mode==1)                                                    
			    {                                                                		
		      	if((test_step0!=0)&&(test_step1==0))                                        			
			    	 {                                                             		
		      	 	j=test_step0;		 			 	                                          			
			    	 	for(switchteststep=1;(j=j/2)!=0;switchteststep++);           		
		      	 	if(switchteststep==0)  break;                                   
						 		printf("switchteststep=%d, " ,switchteststep);            		 	                                                    			
		   			 }			                                                          	
		      	if((test_step1!=0)&&(test_step0==0))                                         			
			    	 {                                                             		
		      	 	j=test_step1;		 			 	                                          			
				  	 	for(switchteststep=17;(j=j/2)!=0;switchteststep++);           	
		      	 	if(switchteststep==0)  break;          						 	                                                   			
		      	 }                                                                
		      	if((test_step0==0)&&(test_step1==0))                              
		      	 {                                                                
		      	 	uis8TestStep[ui8Chnl]=0;                                        
		      	 	F_DefaultQ(0);                                                  
		      	 	for(i=0;i<PLCA_DATA_CNT;i++)                                    
		          		uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;         
		          for(i=0;i<PLCS_RESULT_CNT;i++)                                  
		          		uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;     
		          uis8ErrCount[ui8Chnl] = 0;                                      
			 		    uis8DataCnt[ui8Chnl] = 1;                             						                                 			
		      	 	break;                                                          
		      	 }                                                                
		      }	                                                                  
					F_SetAcIn1(1);
					F_SetSW1(1);				
					uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT] = 1;
					if(C_MODULE_29_0004!=ui16CurModule)
					{
						if(C_MODULE_31_12103==ui16CurModule)	//1A 31-12103A
					  {
					  	sendEl232Cmd(C_IT8500_CC0A1);						  
					    sendEl232Cmd(C_IT8500_CC0A1);
					    sendEl232Cmd(C_IT8500_MODECC);
					    sendEl232Cmd(C_IT8500_OPEN);
					  }		
					 else if(C_MODULE_31_12104==ui16CurModule)	//0.6A 31-12103B
					  {
					  	sendEl232Cmd(C_IT8500_CC0A06);						  
					    sendEl232Cmd(C_IT8500_CC0A06);
					    sendEl232Cmd(C_IT8500_MODECC);
					    sendEl232Cmd(C_IT8500_OPEN);
					  }
					  else
					  {
					  	sendEl232Cmd(C_IT8500_CLOSE);
					  }	  					  	
					}
					else
					{
						sendEl232Cmd(C_IT8500_CV7V8);						  
					  sendEl232Cmd(C_IT8500_CV7V8);
					  sendEl232Cmd(C_IT8500_MODECV);
					  sendEl232Cmd(C_IT8500_OPEN);
					}	
					successcount[ui8Chnl]=0;
					testcount[ui8Chnl]=0;
					ui32Time = tmrUsTimeGet();
					ui32DelayTime3[ui8Chnl] = ui32Time+delay.step1delytime*1000;					
					nextStep(C_WAIT);
					break;
				case  1://31-12103测带载电压 .29-0004 test charge cur.31-05系列测试空载电压
					ui32Time = tmrUsTimeGet();
					if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
					{	        
				    //while(testcount<=uiHMIWord[Test_Count])  
						//{
							  if(C_MODULE_29_0004!=ui16CurModule)
							  {
							  	sendEl232Cmd(C_IT8500_READV);
						      ui32Value = ui32ElVol;						    
						    }  
						    else
							  {
							  	sendEl232Cmd(C_IT8500_READA);
                  ui32Value = ui32ElCur;                
							  }   
						    result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
						    if(uiHMIWord[C_TEST_DEBUG]==1) 
						      printf("channel%d DATA%02d loadvol=%dmV  ",ui8Chnl,uis8DataCnt[ui8Chnl],ui32Value);						  
						    if(result==1)  successcount[ui8Chnl]=0;
						    if(result==0)  successcount[ui8Chnl]++; 
					//	    if(successcount>=uiHMIWord[Success_Count])  break;
					//	    myHwWait2(uiHMIWord[next_delaytime]);
						    testcount[ui8Chnl]++;						
				  	//}				  	
						  				    						
						//saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);		
						
						MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
					  if(Jump_case==1)
					  	break;
											
						if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
						{
							ui32Time = tmrUsTimeGet();
					    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
					    break;
						}
						if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
						{
							nextData(0, 0);
						  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
						  {
						 	 testcount[ui8Chnl]=0; 
						   successcount[ui8Chnl]=0;
						   if(C_MODULE_31_05082==ui16CurModule)	//0.75A 31-05082
					     {
					     	 sendEl232Cmd(C_IT8500_CC0A075);						  
					       sendEl232Cmd(C_IT8500_CC0A075);
					       sendEl232Cmd(C_IT8500_MODECC);
					       sendEl232Cmd(C_IT8500_OPEN);
					     }
					     if(C_MODULE_31_05104==ui16CurModule)	//1A 31-05104
					     {
					     	 sendEl232Cmd(C_IT8500_CC0A1);						  
					       sendEl232Cmd(C_IT8500_CC0A1);
					       sendEl232Cmd(C_IT8500_MODECC);
					       sendEl232Cmd(C_IT8500_OPEN);
					     }
					     if(C_MODULE_31_05211==ui16CurModule)	//2.1A 31-05211
					     {
					     	 sendEl232Cmd(C_IT8500_CC2A1);						  
					       sendEl232Cmd(C_IT8500_CC2A1);
					       sendEl232Cmd(C_IT8500_MODECC);
					       sendEl232Cmd(C_IT8500_OPEN);
					     }
					     if((C_MODULE_31_05052==ui16CurModule)||(C_MODULE_31_05053==ui16CurModule))//0.7A
					     {
					     	 sendEl232Cmd(C_IT8500_CC0A050);						  
					       sendEl232Cmd(C_IT8500_CC0A050);
					       sendEl232Cmd(C_IT8500_MODECC);
					       sendEl232Cmd(C_IT8500_OPEN);
					     }				  					     
						 	 ui32Time = tmrUsTimeGet();
					     ui32DelayTime3[ui8Chnl] = ui32Time+delay.step2delytime*1000;
						   nextStep(C_WAIT);
						   break;	
						  }
						}  						                		                                            															
					}	
					break;				
				case  2://测视在功率					
					ui32Time = tmrUsTimeGet();
					if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
					{
						//while(testcount<=uiHMIWord[Test_Count])  
						//{
							sendPow232Cmd();
						  ui32Value= ui32PowVa;
						  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
						  if(uiHMIWord[C_TEST_DEBUG]==1) 
						    printf("channel%d DATA%02d loadva=%02d  ",ui8Chnl,uis8DataCnt[ui8Chnl],ui32Nibble[ui8Chnl]);						  
						  if(result==1)  successcount[ui8Chnl]=0;
						  if(result==0)  successcount[ui8Chnl]++; 
						 // if(successcount>=uiHMIWord[Success_Count])  break;
						 // myHwWait2(uiHMIWord[next_delaytime]);
						 testcount[ui8Chnl]++;
						//} 
						//testcount=0; 
						//successcount=0;  				    						
						//saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);		
						
						MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
					  if(Jump_case==1)
					  	break;
														
						if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
						{
							ui32Time = tmrUsTimeGet();
					    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
					    break;
						}
						if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
						{
							nextData(0, 0);
						  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
						  {
						 	 testcount[ui8Chnl]=0; 
						   successcount[ui8Chnl]=0;
						 	 ui32Time = tmrUsTimeGet();
					     ui32DelayTime3[ui8Chnl] = ui32Time+delay.step3delytime*1000;
						   nextStep(C_WAIT);
						   break;	
						  }
						} 
					}
					break;
				case  3://测有效功率
					ui32Time = tmrUsTimeGet();
					if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
					{
						//while(testcount<=uiHMIWord[Test_Count])  
						//{
							sendPow232Cmd();						
						  ui32Value= ui32PowWatt;								
						  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
						  if(uiHMIWord[C_TEST_DEBUG]==1) 
						    printf("channel%d DATA%02d loadpowed=%02d  ",ui8Chnl,uis8DataCnt[ui8Chnl],ui32Value);						  
						  if(result==1)  successcount[ui8Chnl]=0;
						  if(result==0)  successcount[ui8Chnl]++; 
						 // if(successcount>=uiHMIWord[Success_Count])  break;
						 // myHwWait2(uiHMIWord[next_delaytime]);
						  testcount[ui8Chnl]++;
						//} 
						//testcount=0; 
						//successcount=0;  				    						
						//saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);		
						
						MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
					  if(Jump_case==1)
					  	break;
					  	
						if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
						{
							ui32Time = tmrUsTimeGet();
					    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
					    break;
						}
						if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
						{
							nextData(0, 0);	
						  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
						  {
						  	if(C_MODULE_29_0004!=ui16CurModule)
						  	{
						  		if((C_MODULE_31_12103==ui16CurModule)||(C_MODULE_31_12104==ui16CurModule))	//1.5A 31-12103A
					        {
					        	sendEl232Cmd(C_IT8500_CC01A5);						  
						  	    sendEl232Cmd(C_IT8500_CC01A5);
						  	    sendEl232Cmd(C_IT8500_MODECC);
						  	    sendEl232Cmd(C_IT8500_OPEN);
					        }		
						  	/*  if(C_MODULE_31_05082==ui16CurModule) //4.2V
						  	  {
						  	  	sendEl232Cmd(C_IT8500_CV4V2);						  
						  	    sendEl232Cmd(C_IT8500_CV4V2);
						  	    sendEl232Cmd(C_IT8500_MODECV);
						  	    sendEl232Cmd(C_IT8500_OPEN);
						  	  }				
						  	  if((C_MODULE_31_05052==ui16CurModule)||(C_MODULE_31_05053==ui16CurModule))	//4.0V
						  	  {
						  	  	sendEl232Cmd(C_IT8500_CV4V0);						  
						  	    sendEl232Cmd(C_IT8500_CV4V0);
						  	    sendEl232Cmd(C_IT8500_MODECV);
						  	    sendEl232Cmd(C_IT8500_OPEN);
						  	  }*/									  
						  	}	
						  	else
						  	{
						  		sendEl232Cmd(C_IT8500_CLOSE);
						  		nextStep(C_WAIT);   //29-0004跳过Case4
						  	}		
						  	testcount[ui8Chnl]=0; 
						    successcount[ui8Chnl]=0;					 											
						  	ui32Time = tmrUsTimeGet();
						  	ui32DelayTime3[ui8Chnl] = ui32Time+delay.step4delytime*1000;
						  	nextStep(C_WAIT);
						  }
						}      																			
					}
					break;
				case  4://31-12103测过载电压//31-05系列测试带载电压
					ui32Time = tmrUsTimeGet();
					if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
					{						
						//while(testcount<=uiHMIWord[Test_Count])  
						//{
						//  if((C_MODULE_31_12104==ui16CurModule)||(C_MODULE_31_12103==ui16CurModule))
						//  {
						  	sendEl232Cmd(C_IT8500_READV);
						    ui32Value = ui32ElVol;
						//  }
						/*  else
						  {
						  	sendEl232Cmd(C_IT8500_READA);
                ui32Value = ui32ElCur;
						  }*/							
						  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
						  if(uiHMIWord[C_TEST_DEBUG]==1) 
						    printf("channel%d DATA%02d overloadvoltge=%02d  ",ui8Chnl,uis8DataCnt[ui8Chnl],ui32Value);						  
						  //if(result==1)  successcount=0;
						  //if(result==0)  successcount++; 
						  //if(successcount>=uiHMIWord[Success_Count])  break;
						 // myHwWait2(uiHMIWord[next_delaytime]);
						  //testcount++;
						//} 
						if(result==1)  successcount[ui8Chnl]=0;
						if(result==0)  successcount[ui8Chnl]++; 
						 // if(successcount>=uiHMIWord[Success_Count])  break;
						 // myHwWait2(uiHMIWord[next_delaytime]);
						 testcount[ui8Chnl]++;
						//} 
						//testcount=0; 
						//successcount=0;  				    						
						//saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);		
						
						MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
					  if(Jump_case==1)
					  	break;
														
						if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
						{
							ui32Time = tmrUsTimeGet();
					    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
					    break;
						}
						if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
						{
							nextData(0, 0);
						  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
						  {
						 	 testcount[ui8Chnl]=0; 
						   successcount[ui8Chnl]=0;
						   if(C_MODULE_31_05082==ui16CurModule) //4.2V
						   {
						   	 sendEl232Cmd(C_IT8500_CV4V2);						  
						     sendEl232Cmd(C_IT8500_CV4V2);
						     sendEl232Cmd(C_IT8500_MODECV);
						     sendEl232Cmd(C_IT8500_OPEN);
						   }	
						   if(C_MODULE_31_05104==ui16CurModule) //4.5V
						   {
						   	 sendEl232Cmd(C_IT8500_CV4V5);						  
						     sendEl232Cmd(C_IT8500_CV4V5);
						     sendEl232Cmd(C_IT8500_MODECV);
						     sendEl232Cmd(C_IT8500_OPEN);
						   }	
						   if(C_MODULE_31_05211==ui16CurModule) //4.5V
						   {
						   	 sendEl232Cmd(C_IT8500_CV4V5);						  
						     sendEl232Cmd(C_IT8500_CV4V5);
						     sendEl232Cmd(C_IT8500_MODECV);
						     sendEl232Cmd(C_IT8500_OPEN);
						   }				
						   else if((C_MODULE_31_05052==ui16CurModule)||(C_MODULE_31_05053==ui16CurModule))	//4.0V
						   {
						   	 sendEl232Cmd(C_IT8500_CV4V0);						  
						     sendEl232Cmd(C_IT8500_CV4V0);
						     sendEl232Cmd(C_IT8500_MODECV);
						     sendEl232Cmd(C_IT8500_OPEN);
						   }
						   else
						   {
						   	 sendEl232Cmd(C_IT8500_CLOSE);
						   }								  						   
						 	 ui32Time = tmrUsTimeGet();
					     ui32DelayTime3[ui8Chnl] = ui32Time+delay.step5delytime*1000;
						   nextStep(C_WAIT);
						   break;	
						  }
						} 
					}
					break;
				case  5://测空载电压   you xiao gong lv .31-05系列测试过载电流
					ui32Time = tmrUsTimeGet();
					if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
					{						
						//while(testcount<=uiHMIWord[Test_Count])  
						//{
						  if((C_MODULE_31_05052==ui16CurModule)||(C_MODULE_31_05053==ui16CurModule)||(C_MODULE_31_05082==ui16CurModule)||(C_MODULE_31_05104==ui16CurModule)||(C_MODULE_31_05211==ui16CurModule))
							{
								sendEl232Cmd(C_IT8500_READA);
						    ui32Value = ui32ElCur;						    
						  }  
						  else
							{
								sendEl232Cmd(C_IT8500_READV);
                ui32Value = ui32ElVol;                
							}							
						  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
						  if(uiHMIWord[C_TEST_DEBUG]==1) 
						    printf("channel%d DATA%02d emptyvol=%02d  ",ui8Chnl,uis8DataCnt[ui8Chnl],ui32Value);						  
						  if(result==1)  successcount[ui8Chnl]=0;
						  if(result==0)  successcount[ui8Chnl]++; 
						//  if(successcount>=uiHMIWord[Success_Count])  break;
						// myHwWait2(uiHMIWord[next_delaytime]);
						  testcount[ui8Chnl]++;
						//} 
						//testcount=0; 
						//successcount=0;  				    						
						//saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);		
						
						MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
					  if(Jump_case==1)
					  	break;
						if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
						{
							ui32Time = tmrUsTimeGet();
					    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
					    break;
						}
						if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
						{
							nextData(0, 0);	
						  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
						  {
						  	F_SetSW1(0);
						  	testcount[ui8Chnl]=0;
						  	successcount[ui8Chnl]=0;
						  	ui32Time = tmrUsTimeGet();
						  	if(C_MODULE_29_0004!=ui16CurModule)						  	
						  	ui32DelayTime3[ui8Chnl] = ui32Time+delay.step6delytime*1000;
						  	else
						  	ui32DelayTime3[ui8Chnl] = ui32Time+delay.step5delytime*1000;
						  	nextStep(C_WAIT);
						  }
						}																		
					}
					break;
				case  6://测空载功率    guo zai dian ya
					ui32Time = tmrUsTimeGet();
					if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
					{
						//while(testcount<=uiHMIWord[Test_Count])  
						//{
							sendPow232Cmd();
						  ui32Value= ui32PowWatt;							
						  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
						  if(uiHMIWord[C_TEST_DEBUG]==1) 
						    printf("channel%d DATA%02d emptypower=%02d  ",ui8Chnl,uis8DataCnt[ui8Chnl],ui32Value);						  
						  if(result==1)  successcount[ui8Chnl]=0;
						  if(result==0)  successcount[ui8Chnl]++; 
						//  if(successcount>=uiHMIWord[Success_Count])  break;
						//  myHwWait2(uiHMIWord[next_delaytime]);
						  testcount[ui8Chnl]++;
						//} 
						//testcount=0; 
						//successcount=0;  				    						
						//saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);		
						
						MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
					  if(Jump_case==1)
					  	break;
						
						if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
						{
							ui32Time = tmrUsTimeGet();
					    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
					    break;
						}
						if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
						{
							nextData(0, 0);						
						  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
						  {
						  	nextStep(C_WAIT);  							
					      F_SetAcIn1(0);				
				   	    F_SetSW1(0);
				   	    testcount[ui8Chnl]=0;
						    successcount[ui8Chnl]=0;
					      sendEl232Cmd(C_IT8500_CLOSE);
						    if(uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl]==1)
						    {
						    	uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] = 2;								
						    }
						    if(ui8DataBufDL)
						    {
						    	ui32SuccessNum++;
						    	if(uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] == 2)
						    	uiHMIWord[succeessclear]++;
						    	if(uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] == 3)
						    	uiHMIWord[failclear]++;
						    	timeGet(&ptime);
						    	sio_psprintf(strbuf1, "%02d:%02d,%d,", ptime->tm_hour, ptime->tm_min, ui32SuccessNum);
						    	
						    	strcat(ui8DataBufDL, strbuf1);
						    	if(C_MODULE_29_0004!=ui16CurModule)
						    	{
						    		sio_psprintf(strbuf1, " %d.%d%d%d,  %d.%d%d%d, %d.%d%d%d, %d.%d%d%d,  %d.%d%d%d,  %d.%d%d%d,  %s,",
						    	             uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,
						    	             uiHMIWord[PLCA_DATA02+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA02+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA02+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA02+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,
						    	             uiHMIWord[PLCA_DATA03+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA03+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA03+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA03+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,
						    	             uiHMIWord[PLCA_DATA04+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA04+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA04+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA04+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,
						    	             uiHMIWord[PLCA_DATA05+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA05+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA05+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA05+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,
						    	             uiHMIWord[PLCA_DATA06+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA06+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA06+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA06+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,						             
						    	             (uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl]<=2)?"OK ":"ERR");
						    	}
						    	else
						    	{
						    		sio_psprintf(strbuf1, " %d.%d%d%d,  %d.%d%d%d, %d.%d%d%d, %d.%d%d%d,  %d.%d%d%d,  %s,",
						    	             uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,
						    	             uiHMIWord[PLCA_DATA02+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA02+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA02+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA02+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,
						    	             uiHMIWord[PLCA_DATA03+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA03+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA03+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA03+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,
						    	             uiHMIWord[PLCA_DATA04+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA04+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA04+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA04+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,
						    	             uiHMIWord[PLCA_DATA05+ui8Chnl*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA05+ui8Chnl*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA05+ui8Chnl*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA05+ui8Chnl*PLCA_DATA_CNT]%1000%100%10,					  	          						  	            						             
						    	             (uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl]<=2)?"OK ":"ERR");
						    	}						    	            
						    	strcat(ui8DataBufDL, strbuf1);
						    	strcat(ui8DataBufDL, "\r\n");
						    	mySaveDataOnce();
						    }
						  }
						}													
					}
					break;
				default:
					uis8TestStep[ui8Chnl] = 255;
					F_DefaultQ(ui8Chnl);	
					if(ui8Chnl==0)
						flagsource=1;
					if(ui8Chnl==1)
						flagsource=0;			
					break;												
						
			}
		}
		if(C_MODULE_31_12104==ui16CurModule)
		{
			channel=0;
			switch(uis8TestStep[ui8Chnl])
			{
				case  0://设置状态
					sendEl232Cmd(C_IT8500_REMOTE);		
					sendEl232Cmd(C_IT8500_CLOSE);
		      if(test_mode==1)                                                    
			    {                                                                		
		      	if((test_step0!=0)&&(test_step1==0))                                        			
			    	 {                                                             		
		      	 	j=test_step0;		 			 	                                          			
			    	 	for(switchteststep=1;(j=j/2)!=0;switchteststep++);           		
		      	 	if(switchteststep==0)  break;                                   
						 		printf("switchteststep=%d, " ,switchteststep);            		 	                                                    			
		   			 }			                                                          	
		      	if((test_step1!=0)&&(test_step0==0))                                         			
			    	 {                                                             		
		      	 	j=test_step1;		 			 	                                          			
				  	 	for(switchteststep=17;(j=j/2)!=0;switchteststep++);           	
		      	 	if(switchteststep==0)  break;          						 	                                                   			
		      	 }                                                                
		      	if((test_step0==0)&&(test_step1==0))                              
		      	 {                                                                
		      	 	uis8TestStep[ui8Chnl]=0;                                        
		      	 	F_DefaultQ(0);                                                  
		      	 	for(i=0;i<PLCA_DATA_CNT;i++)                                    
		          		uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;         
		          for(i=0;i<PLCS_RESULT_CNT;i++)                                  
		          		uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;     
		          uis8ErrCount[ui8Chnl] = 0;                                      
			 		    uis8DataCnt[ui8Chnl] = 1;                             						                                 			
		      	 	break;                                                          
		      	 }                                                                
		      }	                                                                  
					F_SetAcIn1(1);
					F_SetSW1(1);				
					uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT] = 1;
					successcount[ui8Chnl]=0;
					testcount[ui8Chnl]=0;
					ui32Time = tmrUsTimeGet();
					ui32DelayTime3[ui8Chnl] = ui32Time+delay.step1delytime*1000;					
					nextStep(C_WAIT);
					break;
				case  1:
					ui32Time = tmrUsTimeGet();
			  	if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			  	{						
			  	  sendEl232Cmd(C_IT8500_READV);
			  	  ui32Value = ui32ElVol;
			  	  					  
			  	  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
			  		if(uiHMIWord[C_TEST_DEBUG]==1)
			  		{
			  			printf(" +++++channel-%d-case%d--%d+++++\r\n ",ui8Chnl,uis8TestStep[ui8Chnl],ui8Chnl);
			  			printf(" ==channel--DATA%02d emptyVolt=%d===\r\n  ",uis8DataCnt[ui8Chnl],ui32Value);
			  		} 			  			    						  
			  		if(result==1)  successcount[ui8Chnl]=0;
			  		if(result==0)  successcount[ui8Chnl]++; 								
			  		#if  C_TEST_DEBUG
			  	   printf("+++++channel-%d-case1--%d+++++\r\n",ui8Chnl,channel);					    
			  		#endif
			  	  testcount[ui8Chnl]++;
			  		
			  		MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
			  	  if(Jump_case==1)
			  	  	break;																		
			  	  if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
			  		{								
			  	    sendEl232Cmd(C_IT8500_REMOTE);
		          sendEl232Cmd(C_IT8500_CLOSE);
			  	    F_SetAcIn1(1);
					    F_SetSW1(1);
			  			ui32Time = tmrUsTimeGet();
			  	    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
			  	    break;
			  		}
			  	  if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
			  	  {
			  	  	nextData(0, 0);					
			  		  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
			  		  {					  	  
			  	      testcount[ui8Chnl]=0; 
			  		    successcount[ui8Chnl]=0;
			  	      ui32Time = tmrUsTimeGet();
			  	      ui32DelayTime3[ui8Chnl] = ui32Time+delay.step2delytime*1000;					
			  	      nextStep(C_WAIT);
			  		  }
			  	  }															
			  	}
			  	break;
			 case  2://测空载功率
			  		ui32Time = tmrUsTimeGet();
			  	  if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			  		{						
			  		  sendPow232Cmd();						
			  		  ui32Value= ui32PowWatt;
			  		  					    					  
			  		  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
			  			if(uiHMIWord[C_TEST_DEBUG]==1)
			  			{
			  				printf(" +++++channel-%d-case%d--%d+++++\r\n ",ui8Chnl,uis8TestStep[ui8Chnl],ui8Chnl);
			  				printf(" ==channel--DATA%02d emptyPower=%d===\r\n  ",uis8DataCnt[ui8Chnl],ui32Value);
			  			} 						  
			  			if(result==1)  successcount[ui8Chnl]=0;
			  			if(result==0)  successcount[ui8Chnl]++; 								
			  			#if  C_TEST_DEBUG
			  		   printf("+++++channel-%d-case2+++++\r\n",ui8Chnl);
			  		  // printf("channel%d DATA%02d EmptyPower=%dmW   ",ui8Chnl,uis8DataCnt[ui8Chnl],ui32Value);
			  			#endif
			  		  testcount[ui8Chnl]++;
			  			
			  			MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
			  		  if(Jump_case==1)
			  		  	break;																	
			  		  
			  		  if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
			  			{											      
			  				ui32Time = tmrUsTimeGet();
			  		    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
			  		    break;
			  			}
			  		  if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
			  		  {
			  		  	nextData(0, 0);					
			  			  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
			  			  {					  	  
			  		      testcount[ui8Chnl]=0; 
			  			    successcount[ui8Chnl]=0;						   
			  			    sendEl232Cmd(C_IT8500_CC0A065);						  
			  		      sendEl232Cmd(C_IT8500_CC0A065);
			  		      sendEl232Cmd(C_IT8500_MODECC);
			  		      sendEl232Cmd(C_IT8500_OPEN);
			  		      ui32Time = tmrUsTimeGet();
			  		      ui32DelayTime3[ui8Chnl] = ui32Time+delay.step3delytime*1000;					
			  		      nextStep(C_WAIT);
			  			  }
			  		  }															
			  		}
			  	  break;
			    case  3://测带载电压
			  		ui32Time = tmrUsTimeGet();
			  	  if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			  		{						
			  		  sendEl232Cmd(C_IT8500_READV);
			  		  ui32Value = ui32ElVol;
			  		  					  
			  		  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
			  			if(uiHMIWord[C_TEST_DEBUG]==1)
			  			{
			  				printf(" +++++channel-%d-case%d--%d+++++\r\n ",ui8Chnl,uis8TestStep[ui8Chnl],channel);
			  				printf(" ==channel--DATA%02d loadVolt=%d===\r\n  ",uis8DataCnt[ui8Chnl],ui32Value);
			  			} 							  
			  			if(result==1)  successcount[ui8Chnl]=0;
			  			if(result==0)  successcount[ui8Chnl]++; 								
			  			#if  C_TEST_DEBUG
			  		   printf("+++++channel-%d-case3+++++",ui8Chnl);					   
			  			#endif
			  		  testcount[ui8Chnl]++;
			  			
			  			MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
			  		  if(Jump_case==1)
			  		  	break;																	
			  		  
			  		  if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
			  			{												    
			  		 	  sendEl232Cmd(C_IT8500_CC0A065);						  
			  		    sendEl232Cmd(C_IT8500_CC0A065);
			  		    sendEl232Cmd(C_IT8500_MODECC);
			  		    sendEl232Cmd(C_IT8500_OPEN);
			  			  ui32Time = tmrUsTimeGet();
			  		    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
			  		    break;
			  			}
			  		  if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
			  		  {
			  		  	nextData(0, 0);					
			  			  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
			  			  {					  	  
			  		      testcount[ui8Chnl]=0; 
			  			    successcount[ui8Chnl]=0;
			  		      ui32Time = tmrUsTimeGet();
			  		      ui32DelayTime3[ui8Chnl] = ui32Time+delay.step4delytime*1000;					
			  		      nextStep(C_WAIT);
			  			  }
			  		  }															
			  		}
			  	  break;
			  	case  4://测有效功率
			  		ui32Time = tmrUsTimeGet();
			  	  if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			  		{						
			  		  sendPow232Cmd();						
			  		  ui32Value= ui32PowWatt;
			  		  					    					  
			  		  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
			  			if(uiHMIWord[C_TEST_DEBUG]==1)
			  			{
			  				printf(" +++++channel-%d-case%d--%d+++++\r\n ",ui8Chnl,uis8TestStep[ui8Chnl],channel);
			  				printf(" ==channel--DATA%02d loadPower=%d===\r\n  ",uis8DataCnt[ui8Chnl],ui32Value);
			  			} 							  
			  			if(result==1)  successcount[ui8Chnl]=0;
			  			if(result==0)  successcount[ui8Chnl]++; 								
			  			#if  C_TEST_DEBUG
			  		   printf("+++++channel-%d-case4+++++",ui8Chnl);			  		  
			  			#endif
			  		  testcount[ui8Chnl]++;
			  			
			  			MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
			  		  if(Jump_case==1)
			  		  	break;																	
			  		  
			  		  if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
			  			{											      
			  				ui32Time = tmrUsTimeGet();
			  		    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
			  		    break;
			  			}
			  		  if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
			  		  {
			  		  	nextData(0, 0);					
			  			  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
			  			  {					  	  
			  		     testcount[ui8Chnl]=0; 
			  			   successcount[ui8Chnl]=0;					 	    
			  		     ui32Time = tmrUsTimeGet();
			  		     ui32DelayTime3[ui8Chnl] = ui32Time+delay.step5delytime*1000;					
			  		     nextStep(C_WAIT);
			  			  }
			  		  }															
			  		}
			  	  break;
			  	case  5://测视在功率   you xiao gong lv
			  		ui32Time = tmrUsTimeGet();
			  	  if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			  		{						
			  		  sendPow232Cmd();						
			  		  ui32Value= ui32PowVa;
			  		  					    					  
			  		  result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
			  			if(uiHMIWord[C_TEST_DEBUG]==1)
			  			{
			  				printf(" +++++channel-%d-case%d--%d+++++\r\n ",ui8Chnl,uis8TestStep[ui8Chnl],channel);
			  				printf(" ==channel--DATA%02d loadVA=%d===\r\n  ",uis8DataCnt[ui8Chnl],ui32Value);
			  			}						  
			  			if(result==1)  successcount[ui8Chnl]=0;
			  			if(result==0)  successcount[ui8Chnl]++; 								
			  			#if  C_TEST_DEBUG
			  		   printf("+++++channel-%d-case5+++++\r\n",ui8Chnl);					   
			  			#endif
			  		  testcount[ui8Chnl]++;
			  			
			  			MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
			  		  if(Jump_case==1)
			  		  	break;																	
			  		  
			  		  if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
			  			 {											      
			  				 ui32Time = tmrUsTimeGet();
			  		    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime];
			  		    break;
			  		 	}
			  		  if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
			  		  {
			  		  	nextData(0, 0);					
			  			  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
			  			  {		  			  			    
			  			   sendEl232Cmd(C_IT8500_CC0A095);						  
			  		     sendEl232Cmd(C_IT8500_CC0A095);
			  		     sendEl232Cmd(C_IT8500_MODECC);
			  		     sendEl232Cmd(C_IT8500_OPEN);			  			   
			  		     testcount[ui8Chnl]=0; 
			  			   successcount[ui8Chnl]=0;					 	    
			  		     ui32Time = tmrUsTimeGet();
			  		     ui32DelayTime3[ui8Chnl] = ui32Time+delay.step6delytime*1000;					
			  		     nextStep(C_WAIT);
			  			  }
			  		  }															
			  		}
			  	  break;
			   case  6:
			   	  ui32Time = tmrUsTimeGet();
			  	  if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			  	  {
			  	  	 sendEl232Cmd(C_IT8500_CC0A00);						  
			  		   sendEl232Cmd(C_IT8500_CC0A00);
			  		   sendEl232Cmd(C_IT8500_MODECC);
			  		   sendEl232Cmd(C_IT8500_OPEN);
			  		   ui32Time = tmrUsTimeGet();
			  		   ui32DelayTime3[ui8Chnl] = ui32Time+(delay.step6delytime-200)*1000;					
			  		   nextStep(C_WAIT);
			  	  }
			  	  break;
			   case 7:
			  	 ui32Time = tmrUsTimeGet();
			  		if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			  		{			
			  			sendEl232Cmd(C_IT8500_READV);
			  		  ui32Value = ui32ElVol;
			  			result=saveTestData(PLCA_DATA01+uis8DataCnt[ui8Chnl]-1, ui32Value);
			  			if(uiHMIWord[C_TEST_DEBUG]==1)
			  			{
			  				printf(" +++++channel-%d-case%d--%d+++++\r\n ",ui8Chnl,uis8TestStep[ui8Chnl],ui8Chnl);
			  				printf(" ==channel--DATA%02d MAXCur=%d===\r\n  ",uis8DataCnt[ui8Chnl],ui32Value);
			  			}								  
			  			if(result==1)  successcount[ui8Chnl]=0;
			  			if(result==0)  successcount[ui8Chnl]++; 								
			  			#if  C_TEST_DEBUG
			  		   printf("+++++channel-%d-case5+++++\r\n",ui8Chnl);					   
			  			#endif
			  		  testcount[ui8Chnl]++;
			  			
			  			MANUAL_TESTMODE(uis8DataCnt[ui8Chnl]);
			  		  if(Jump_case==1)
			  		  	break;																	
			  		  
			  		  if((successcount[ui8Chnl]<uiHMIWord[Success_Count])&&(testcount[ui8Chnl]<uiHMIWord[Test_Count]))
			  			{								
			  			 	sendEl232Cmd(C_IT8500_CC0A00);						  
			  		    sendEl232Cmd(C_IT8500_CC0A00);
			  		    sendEl232Cmd(C_IT8500_MODECC);			      
			  				ui32Time = tmrUsTimeGet();
			  		    ui32DelayTime3[ui8Chnl] = ui32Time+uiHMIWord[next_delaytime]+50000;
			  		    break;
			  		 	}
			  		 	if((successcount[ui8Chnl]>=uiHMIWord[Success_Count])||(testcount[ui8Chnl]>=uiHMIWord[Test_Count]))
			  		 	{
			  		 		nextData(0, 0);						
			  			  if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
			  			  {		
			  			   F_SetAcIn1(0);
					       F_SetSW1(0);				 									  		     
			  		     sendEl232Cmd(C_IT8500_CLOSE);	  		    			  		   		  		     
			  			   if(uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+channel]==1)
			  			   {
			  			   	uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+channel] = 2;								
			  			   }
			  			   if(ui8DataBufDL)
			  			   {
			  			   	ui32SuccessNum++;
			  			   	result=ui8Chnl+channel;						 
			  			   	timeGet(&ptime);
			  			   	sio_psprintf(strbuf1, "%02d:%02d,%d,", ptime->tm_hour, ptime->tm_min, ui32SuccessNum);
			  			   	
			  			   	strcat(ui8DataBufDL, strbuf1);
			  			   	sio_psprintf(strbuf1, " %d.%d%d%d,%d.%d%d%d,%d.%d%d%d,%d.%d%d%d,%d.%d%d%d,%d.%d%d%d, %s",
			  			    		             uiHMIWord[PLCA_DATA01+result*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA01+result*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA01+result*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA01+result*PLCA_DATA_CNT]%1000%100%10,
			  			    		             uiHMIWord[PLCA_DATA02+result*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA02+result*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA02+result*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA02+result*PLCA_DATA_CNT]%1000%100%10,
			  			    		             uiHMIWord[PLCA_DATA03+result*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA03+result*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA03+result*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA03+result*PLCA_DATA_CNT]%1000%100%10,
			  			    		             uiHMIWord[PLCA_DATA04+result*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA04+result*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA04+result*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA04+result*PLCA_DATA_CNT]%1000%100%10,
			  			    		             uiHMIWord[PLCA_DATA05+result*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA05+result*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA05+result*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA05+result*PLCA_DATA_CNT]%1000%100%10,								             							             
			  			    		             uiHMIWord[PLCA_DATA06+result*PLCA_DATA_CNT]/1000,uiHMIWord[PLCA_DATA06+result*PLCA_DATA_CNT]%1000/100,uiHMIWord[PLCA_DATA06+result*PLCA_DATA_CNT]%1000%100/10,uiHMIWord[PLCA_DATA06+result*PLCA_DATA_CNT]%1000%100%10,	  			    	             
			  			    		             (uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+channel]<=2)?"OK ":"ERR");
			  			   	strcat(ui8DataBufDL, strbuf1);
			  			   	strcat(ui8DataBufDL, "\r\n");
			  			   	mySaveDataOnce();	
			  			   	nextStep(C_WAIT);		  			 
			  			   }
			  			  }
			  		 	}		  			
			  		}
			  		break;
			  default:
					uis8TestStep[ui8Chnl] = 255;
					F_DefaultQ(ui8Chnl);	
					if(ui8Chnl==0)
						flagsource=1;
					if(ui8Chnl==1)
						flagsource=0;			
					break;	
			}
		}
	//	}
		
		#if  MY_USE_USB  // xouer080909
		    ui32Time = tmrUsTimeGet();
		    if((ui32Time>=ui32DelayTime1)||((ui32Time<ui32DelayTime1)&&((ui32DelayTime1-ui32Time)>600000000)))
		    {
		    	ui32DelayTime1 = ui32Time+1000000;
		    	
		    	myHwWait1(1);
		    	
		    	osTimeDly(1);
		    	switch(uiState)
		    	{
		    		case  UI_MODE_USB:
		    			#if  C_USB_DEBUG
		    			    printf("uiState==UI_MODE_USB  ");
		    			#endif
		    			uiUsbMode();
		    			break;
		    		default:
		    			break;
		    	}
		    	ui32QueWaitTime = 0xffffffff;
		    	keyGet(&key);
		    	ui32QueWaitTime = 0;
		    }
		#endif
	}
}