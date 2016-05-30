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
 *  Author: Timothy Wu                                                    *
 *                                                                         *
 **************************************************************************/
#define RS_232_DBG
 
#include "general.h"
#include "os_api.h"
#include "dcf_api.h"
#include "exif_api.h"
#include "sysregs.h"
#include "irq.h"
#include "hw_gpio.h"
#include "card.h"
#include "stdio.h"
#include "utility.h"
#include "gpio.h"
#include "ui_main.h"
#include "xiaoao.h"
#include "dpof_api.h"
#include "vfs_api.h"


#define TASK_CARD_STACK_SIZE		1024

extern	OS_EVENT *uiKeyMsgQ;
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define NAND_SUPPORT     1
#define SMC_SUPPORT      0
#define SD_SUPPORT       1		//xouer0506
#define CF_SUPPORT       0
#define MS_SUPPORT       0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/* NOTES of card detection:
 * 1. SMC/CF requires extra hardware (buffers) to separate their pins with
 *    ROM address.
 * 2. The GPIO of SD/MMC and MS detection depends on the hardward design.
 *    Please check with H/W engineers and modify the following definitions.
 *    In the example below, general purpose GPIO[0] is picked.
 */
#if (SMC_SUPPORT == 1)
#define CARD_HOT_PLUG		1
#define GPIO_CARD_DET		(GPIO_TYPE_FM + 6)
#define cardIns()           smcDetect()
#define cardRmv()           smcRemove()
#elif (SD_SUPPORT  == 1)
/*Ly 04/09/20 Rewrite start*/
#define CARD_HOT_PLUG		1
#define GPIO_CARD_DET		GPIO_DET_SD/*(GPIO_TYPE_GE + 3) (GPIO_TYPE_GE + 0)*/
/*Ly 04/09/20 Rewrite end*/
#define cardIns()           sdDetect()
#define cardRmv()           sdRemove()
#elif (CF_SUPPORT  == 1)
#define CARD_HOT_PLUG		1
#define GPIO_CARD_DET		(GPIO_TYPE_FM + 6)
#if 0 /*Select for CF Memory Mode access*/
#define cardIns()           cfMemDetect()
#else /*Select for CF IDE Mode access*/
#define cardIns()           cfIdeDetect()
#endif
#define cardRmv()           cfRemove()
#elif (MS_SUPPORT  == 1)
#define CARD_HOT_PLUG		0
#define GPIO_CARD_DET		(GPIO_TYPE_GE + 0)
#define cardIns()           msDetect()
#define cardRmv()           msRemove()
#else
#define CARD_HOT_PLUG		0
#define GPIO_CARD_DET		(GPIO_TYPE_GE + 0)
#define cardIns()
#define cardRmv()
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 nandDetect(void);
extern UINT32 smcDetect(void);
extern void   smcRemove(void);
extern UINT32 sdDetect(void);
extern void   sdRemove(void);
extern UINT32 cfMemDetect(void);
extern UINT32 cfIdeDetect(void);
extern void   cfRemove(void);
extern UINT32 msDetect(void);
extern void   msRemove(void);
extern void usbLunInit(void);
extern void dirInit();

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 cardTaskStack[TASK_CARD_STACK_SIZE];

static UINT8     cardValid[8];
/*static UINT32    cardExist;*/
/*Ly 04/09/20 add start*/
UINT32    cardExist,cardprotect=0; /*Julie*/
/*Ly 04/09/20 add end*/
static UINT32    cardSelection;
static OS_EVENT* semCard;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void cardInsIsr(void);
void cardRmvIsr(void);
void cardMountTask(void *);

/**************************************************************************
 *                                                                        *
 *  Function Name: cardDetect                                             *
 *                                                                        *
 *  Purposes: Detect storage media after power on.                        *
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
void
cardDetect(
	void
)
{
	hwFmGpioIrqInit();
	hwGenGpioIrqInit();

	#if (NAND_SUPPORT == 1)
	nandCpyBackFuncSet(2);  //fqdao_add for tools_1G 06.5.26
	nandDetect();
	cardValid[DRIVE_NAND] = 1;
	#endif

	semCard = osSemCreate("CARD", 0);
	osTaskCreate("CARD", cardMountTask, (void *)TASK_CARD_STACK_SIZE, (void *)(&cardTaskStack[TASK_CARD_STACK_SIZE-1]), 21);
	
	#if (SMC_SUPPORT == 1)
	cardSelection = DRIVE_SMC;
	cardValid[DRIVE_SMC] = 1;
	#endif

	#if (SD_SUPPORT == 1 )
	cardSelection = DRIVE_SD;
	cardValid[DRIVE_SD] = 1;
	#endif

	#if (CF_SUPPORT == 1 )
	cardSelection = DRIVE_CF;
	cardValid[DRIVE_CF] = 1;
	#endif

	#if (MS_SUPPORT == 1 )
	cardSelection = DRIVE_MS;
	cardValid[DRIVE_MS] = 1;
	#endif

/*Gu@ 2004/08/03, 17:48 
	gpioConfig(GPIO_CARD_POWER, 1);
	gpioSet(GPIO_CARD_POWER, 0);*/
	gpioConfig(GPIO_CARD_DET, 0);
	gpioSet(GPIO_CARD_DET, 0);

	#if (CARD_HOT_PLUG == 0)
	if (1)
	#else
	if (gpioGet(GPIO_CARD_DET) == 0)
	#endif
	{
		cardExist = 1;
		cardIns();
		#if 0
		dpofInit();
		dpofHdrAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_GEN_CRT, "\"Model Name\"");
		dpofHdrAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_USR_NAM, "\"Huiling\", \"Chou\"");	
		#endif
		xdcfActiveDevIdSet(cardSelectionGet());
		osSemPost(semCard);
		gpioIrqReg(GPIO_CARD_DET, cardRmvIsr, 0);
		sPara.StorageModeFlag = Card;
		dirIdx = 0;
		fileIdx = 0;
	}
	else 
	{
		cardExist = 0;
		gpioIrqReg(GPIO_CARD_DET, cardInsIsr, 1);
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: cardInsIsr                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *    Interrupt service routine right after memory card is inserted.      *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None.                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
cardInsIsr(
	void
)
{
	gpioIrqRmv(GPIO_CARD_DET, FALL);
	osSemPost(semCard);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: cardRmvIsr                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *    Interrupt service routine right after memory card is removed.       *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None.                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
cardRmvIsr(
	void
)
{
	gpioIrqRmv(GPIO_CARD_DET, RISE);
	osSemPost(semCard);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: cardMountTask                                          *
 *                                                                        *
 *  Purposes:                                                             *
 *    The task is used to buffer the mount request that will take longer. *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None.                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 extern UINT8 ui8GameFlag ;
void cardMountTask(
	void *pdata
)
{
	UINT32 err;

	while ( 1 ) 
	{
		osSemPend(semCard, 0, &err);
		/* Put the delay here to wait for the card being mounted firmly */
		osTimeDly(15);
		#if (CARD_HOT_PLUG == 0)
		if (1)
		#else
		if (gpioGet(GPIO_CARD_DET) == 0) 
		#endif
		{
			/*Gu@ 2004/08/03, 17:53 gpioConfig(GPIO_CARD_POWER, 1);
			gpioSet(GPIO_CARD_POWER, 1);*/
			osTimeDly(70);
			if ( cardExist == 0 ) {
				cardExist = 1;
				if(ui8GameFlag) 
				{
					osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);
				}
				else  osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);		
				osTimeDly(40) ;
				gpioSet(GPIO_CARD_EN, 1);  
				cardIns();
				    
#if 0				
				dpofInit();
				dpofHdrAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_GEN_CRT, "\"Model Name\"");
				dpofHdrAttrSet(DPOF_PRINT_MODE_STD, DPOF_PARAMID_USR_NAM, "\"Huiling\", \"Chou\"");	
#endif				
				xdcfActiveDevIdSet(cardSelectionGet());
				xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);
				dirInit();/*xouer@081125 */

				#if 0  // tzw add for bug 47 060523	//xouer090617
				sPara.StorageModeFlag = Card;
				err = xdcfActiveDevIdSet(DRIVE_SD);
			
		             if ( err== SUCCESS)
				{
		         		nandWriteParameters();
					printf("Card Insertion tzw add ok \n");
				}
				else
			       {
					cardExist = 0;
				 	sPara.StorageModeFlag = NandFlash;
			    		xdcfActiveDevIdSet(DRIVE_NAND);
					printf("Card Insertion fail \n");
					continue ;
				}	
				#else
				xdcfActiveDevIdSet(DRIVE_NAND);
				sPara.StorageModeFlag = NandFlash;//wendy@050225
				#endif
				
				printf("Card Insertion\n");
				dirIdx = 0;
				fileIdx = 0;
				if(!uiUSBStatus)/*andrew_gu@2005/May/17, 9:14 , don't post while USB connected!*/
				osQuePost(uiKeyMsgQ, &keyRotary[(uiState & UI_MODE_MASK) >> 24]);
			}
			gpioIrqReg(GPIO_CARD_DET, cardRmvIsr, RISE);
		}
		else
		{/*Gu@ 2004/08/03, 17:53 
			gpioConfig(GPIO_CARD_POWER, 1);
			gpioSet(GPIO_CARD_POWER, 0);*/
			osTimeDly(50);//wendy@050225
			if ( cardExist == 1 )
			{
				cardExist = 0;
				if(ui8GameFlag) 
				{
					osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_FUNC_MENU]);
				}
				else  osQuePost(uiKeyMsgQ, &keyButton[UI_KEY_ACCIDENT]);		
				osTimeDly(40) ;
				
				gpioSet(GPIO_CARD_EN, 0);      
				cardRmv();
				#if  0 /*fqdao_modify for bug nn 06.5.4*/
					dirInit();/*andrew_gu@2005/May/11, 13:36 */
				#endif				
				xdcfInit(imageDirNameStr, imageFileNameStr, 0/*xDCF_CONFIG_SORT_IDX|xDCF_CONFIG_DCF_ONLY*/);
				printf("Card Removal\n");
				/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */
				dirIdx = 0;
				fileIdx = 0;
				/* 536@TurnkeyV1.0.1@Joe@2004.03.08 */	
				sPara.StorageModeFlag = NandFlash;//wendy@050225

				if(!uiUSBStatus)/*andrew_gu@2005/May/17, 9:14 , don't post while USB connected!*/
				osQuePost(uiKeyMsgQ, &keyRotary[(uiState & UI_MODE_MASK) >> 24]);
			}
			gpioIrqReg(GPIO_CARD_DET, cardRmvIsr, FALL);
		}
		usbLunInit();
		if(cardExist)	
		{
			/*andrew_gu@2005/May/16, 21:30 (*/			
			if (gpioGet(GPIO_DET_SD_WP))
			{	         	
				vfsDevIoCtrl(DRIVE_SD, IO_CFG_WRITE, IO_READ_ONLY);//set readonly			
				cardprotect = 1;
			}
			else
			{
				vfsDevIoCtrl(DRIVE_SD, IO_CFG_WRITE, IO_READ_WRITE);
				cardprotect = 0;	
			}
		/*andrew_gu@2005/May/16, 21:30 )*/ 
		}
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: cardSelectionGet                                       *
 *                                                                        *
 *  Purposes: Get current option of storage media.                        *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: card type.                                                   *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
cardSelectionGet(
	void
)
{
	return cardSelection;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: cardValidCheck                                         *
 *                                                                        *
 *  Purposes: Check if a drive is supported in current configuration.     *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    driveId: in, drive ID to be polled.                                 *
 *                                                                        *
 *  Returns: 1 - the drive is supported                                   *
 *           0 - the drive is not supported                               *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
cardValidCheck(
	UINT32 driveId
)
{
	return cardValid[driveId];
}
/*#if (MS_SUPPORT  == 1)*/
/**************************************************************************
 *                                                                        *
 *  Function Name: cardPwrMSNotSet                                        *
 *                                                                        *
 *  Purposes: Set the clock source(RE) to on board NAND.                  *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: card type.                                                   *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
cardPwrMSNotSet
(
	void
)
{
	/*
  	 Use GPIO to switch hardware signal from
  	 the "CLK" of MS to "RE" of on board NAND,
  	 here is to switch to on board NAND, that
  	 means , switch to "RE"
  	 if there is only MS without NAND, here no
  	 need to do anything
	*/
	dprint("To on board NAND\n");
}

/**************************************************************************
 *                                                                        *
 *  Function Name: cardPwrMSSet                                           *
 *                                                                        *
 *  Purposes: Set the clock source(CLK) to memory stick.                  *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: card type.                                                   *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
cardPwrMSSet
(
	void
)
{
	/*
  	 Use GPIO to switch hardware signal back to
  	 the "CLK" of MS from "RE" of on board NAND,
  	 here is to switch to MS, that means , switch
  	 to "CLK" if there is only MS without NAND,
  	 here no need to do anything
	*/
	dprint("To Memory Stick\n");
}
/*Gu @2003.12.15 17:15 begin*/
UINT32 isCardExist(void)
{
	return cardExist;
}
/*Gu @2003.12.15 17:15 end*/

/*andrew_gu@2005/May/16, 20:24 (*/
UINT32 isCardProtect(void)
{
	return cardprotect;
}
/*andrew_gu@2005/May/16, 20:24 )*/

/*#endif*/
