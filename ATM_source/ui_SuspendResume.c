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
 *                                                                         *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *                                                                        *
 **************************************************************************/
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "irq.h"
#include "timer.h"
#include "stdio.h"
#include "snap_api.h"
#include "pb_api.h"
#include "os_api.h"
#include "dcf_api.h"
#include "exif_api.h"
#include "exif_custom.h"
#include "usb_api.h"
#include "osd_common.h"
#include "osd_api.h"
#include "dsc_init.h"
#include "hw_gpio.h"
#include "sysg_api.h"
#include "aaa_api.h"
#include "sysg_api.h"
#include "dpof_api.h"
#include "hw_cpu.h"
#include "utility.h"
#include "mp3_api.h"
#include "audio_api.h"
#include "asf_api.h"
#include "hw_disp_ext.h"
#include "pub_api.h"
#include "otg_api.h"
#include "panel.h"
#include "hw_uart.h"
#include "dps_api.h"
#include "hw_sdram.h" 
#include "snap_custom.h"
#include "mp3en_api.h"
#include "hw_pwrctrl.h"
#include "sysregs.h"
#include "hw_aud.h"

/**************************************************************************
 *                                                                        *
 *  Function Name: rombootSuspendEnableSet                                                *
 *                                                                        *
 *  Purposes:                                                             *
 *     This is used for firmware code from extern ROM                                                                   *
 *  Descriptions:                                                         *
 *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: Never returns.                                               *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
rombootSuspendEnableSet(
	UINT32 Reset,
	UINT32 SelfRefresh
)
{
	UINT8 reg0;
	globalReg_t *pglobalReg;
    sdramReg_t *psdramReg;

	pglobalReg = (globalReg_t *)0xb0000000;
    psdramReg = (sdramReg_t *)(0xb0000000);
	reg0 = (UINT8)(Reset << 1);
	reg0 = reg0 | 0x01; 	
	pglobalReg->suspend = 0;/*clear suspend */
	pglobalReg->resumeLen = 0x05;
	hwSuspendRegWrite((UINT8)SelfRefresh, reg0);
	/*Sleep here*/  
    psdramReg->srefresh = 0;/*Clear Selfresh*/
	pglobalReg->suspend = 0;/*clear suspend */
}

/**************************************************************************
 *                                                                        *
 *  Function Name: nandbootSuspendEnableSet                                                *
 *                                                                        *
 *  Purposes:                                                             *
 *     This is used for firmware code from on board nand                                                                   *
 *  Descriptions:                                                         *
 *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: Never returns.                                               *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
nandbootSuspendEnableSet(
	UINT32 Reset,
	UINT32 SelfRefresh
)
{
	UINT8 reg0;
	globalReg_t *pglobalReg;
    sdramReg_t *psdramReg;

	pglobalReg = (globalReg_t *)0xb0000000;
    psdramReg = (sdramReg_t *)(0xb0000000);
	reg0 = (UINT8)(Reset << 1);
	reg0 = reg0 | 0x01; 	
	pglobalReg->suspend = 0;/*clear suspend */
	pglobalReg->resumeLen = 0x05;
	psdramReg->srefresh = SelfRefresh;
	pglobalReg->suspend = reg0;
	/*Sleep here*/  
    psdramReg->srefresh = 0;/*Clear Selfresh*/
	pglobalReg->suspend = 0;/*clear suspend */
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uiSuspendResume                                                *
 *                                                                        *
 *  Purposes:                                                             *
 *    Set the EVB at suspend mode and use GPIO0 to resume                 *
 *                                                                        *
 *  Descriptions:                                                         *
 *    This routine is workable for 536A's EVB, user can refer to this 
 *    routine 
 *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: Never returns.                                               *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 void
 uiSuspendResume(
 	void
 )
 {
	UINT32 status;
	
	/* 
	 * Enable UI resume state, this routine has to be called or the resume
	 * function will not work, the system will suspend forever
	 */
	hwUiResumeEnableSet(1);
	
	/* 
	 * Clear UI resume flag in case this flag is set, if the flag is on and 
	 * we do not to clear it, the system can't enter suspend state
	 */
	hwUiResumeFlagSet(0);

	/* 
	 * Any interrupt will cause system resume ,so the user need to set
	 * their favorite resume interrupt method, here use GPIO0 
	 * rising and falling interrupt as the resume source
	 */
#if 0	 
	hwGenGpioIrqInit();
	hwGenGpioIrqReg(0,NULL,0);
	hwGenGpioIrqReg(0,NULL,1);
#endif	
	/* 
	 * Disable all interrupt source 
	 * User has to carefully aware all interrupt source
	 * which is running, maybe it not only below's interrupt
	 * below's interrupt is used for disable the interrupt of
	 * SPCA536A's release code
	 */
	WRITE8(0xB00010d0,0x00);
	WRITE8(0xB00010d1,0x00);	
	WRITE8(0xB00010d2,0x00);	
	WRITE8(0xB00010d3,0x00);	
	WRITE8(0xB00091d0,0x00);/* disable VD interrupt */
	WRITE8(0xB00091c0,0x00);/* clear VD interrupt flag*/
	WRITE8(0xB00010c0,0x00);
	WRITE8(0xB00010c1,0x00);	
	WRITE8(0xB00010c2,0x00);	
	WRITE8(0xB00010c3,0x00);
	
	/* 
	 * Set all floating GPIO(GPIO,FMGPIO,TGGPIO...) as input state  
	 * and set the GPIO as output High if the GPIO is external pull 
	 * high and set the GPIO as output low if the GPIO is external 
	 * pull low
	 * Here is critical for suspend current, theory the suspend current 
	 * could be lower to 1mA but that depend on the power design and maybe
	 * carefully check the GPIO state, please survey all possible current
	 * leakage from GPIO
	 */

	#if 1
	/* GPIO OE */
	hwGenGpioCfgSet(0xffffffff, 0xffffffff);
	/* GPIO OutPut low */
	hwGenGpioWrite(0x00000000);
	
	/* FMGPIO OE */
	WRITE8(0xb0004000,0);/* Set the FMGPIO interface as GPIO */
	/* FMGPIO 0 control */
	WRITE8(0xb00040ff,0);
	hwFmGpioCfgSet(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
	/* FMGPIO OutPut low */
	hwFmGpioLsbWrite(0x00000000);
	hwFmGpioMsbWrite(0x00000000);
	/* FMGPIO 2,3 is not controllable */
	
	/* UGPIO OE */
	hwUsbGpioCfgSet(0xffffffff, 0xffffffff);
	/* UGPIO OutPut low */
	hwUsbGpioWrite(0x00000000);
	
	/* DRAMGPIO OE */
	hwSdramGpioCfg(0x10);/* LdQs set high */
	/* DRAMGPIO OutPut low */
	hwSdramGpioSet(0xffffffff);
		
	/* audio GPIO */	
	hwAudGpioCfg(0xff);
	hwAudGpioSet(0);
	
	/*sel tgGPIO */
	hwTgGpioCfgSet(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
	/* enable tgOut Pad */
	hwTgGpioLsbWrite(0x00200000);/* TGGPIO21 to high */
	hwTgGpioMsbWrite(0x00000000);

	/*tv GPIO */
	hwTvGpioCfgSet(0xffffffff, 0xffffffff);
	hwTvGpioWrite(0);
	
	/* disable TV DA */
	hwDispPnlSelect(0);
	hwDispEn(0);
	
	/* disable ADC */
	hwAdcSet(0, 3, 11); 
	#else
	/* 
	 * Below is to set the GPIO state
	*/
	WRITE8(0xb0001108,0xff);
	WRITE8(0xb0001109,0xff);
	WRITE8(0xb000110A,0xff);

	WRITE8(0xb0004010,0xff);
	WRITE8(0xb0004011,0xff);
	WRITE8(0xb0004012,0xff);
	WRITE8(0xb0004013,0xff);
	WRITE8(0xb0004014,0xff);
	WRITE8(0xb0004015,0xff);

	WRITE8(0xb0005054,0xff);
	WRITE8(0xb0005055,0xff);
	WRITE8(0xb0005056,0xff);
	WRITE8(0xb0005057,0xff);

	WRITE8(0xb0007120,0xff);
	WRITE8(0xb000600e,0xff);
	/*sel tgGPIO */
	WRITE8(0xb0009160,0xff);
	WRITE8(0xb0009161,0xff);
	WRITE8(0xb0009162,0xff);
	WRITE8(0xb0009163,0xff);
	WRITE8(0xb0009164,0xff);
	WRITE8(0xb0009165,0xff);
	/* enable tgOut Pad */
	WRITE8(0xb0009168,0xff);
	WRITE8(0xb0009169,0xff);
	WRITE8(0xb000916a,0xff);
	WRITE8(0xb000916b,0xff);
	WRITE8(0xb000916c,0xff);
	WRITE8(0xb000916d,0xff);

	WRITE8(0xb000a0a4,0xff);
	WRITE8(0xb000a0a5,0xff);
	WRITE8(0xb000a0a6,0xff);

	/* GPIO OutPut low */
	WRITE8(0xb0001100,0x00);
	WRITE8(0xb0001101,0x00);
	WRITE8(0xb0001102,0x00);

	WRITE8(0xb0004008,0x00);
	WRITE8(0xb0004009,0x00);
	WRITE8(0xb000400a,0x00);
	WRITE8(0xb000400b,0x00);
	WRITE8(0xb000400c,0x00);
	WRITE8(0xb000400d,0x00);

	WRITE8(0xb0005050,0x00);
	WRITE8(0xb0005051,0x00);
	WRITE8(0xb0005052,0x00);
	WRITE8(0xb0005053,0x00);

	WRITE8(0xb0007121,0x00);
	WRITE8(0xb000600d,0x00);

	WRITE8(0xb0009170,0x00);
	WRITE8(0xb0009171,0x00);
	WRITE8(0xb0009172,0x20);/* GPIO21 out high */
	WRITE8(0xb0009173,0x00);
	WRITE8(0xb0009174,0x00);
	WRITE8(0xb0009175,0x00);

	WRITE8(0xb000a0a1,0x00);
	WRITE8(0xb000a0a2,0x00);
	WRITE8(0xb000a0a3,0x00);
	/* disable TV DA */
	WRITE8(0xb000a020,0x00);
	WRITE8(0xb000a000,0xfc);

	/* disable ADC */
	hwAdcSet(0, 3, 11); 

	/* audio GPIO */	
	hwAudGpioCfg(0xff);
	hwAudGpioSet(0);
	#endif
	/* 
	 * The next few step is critical, we stop the possibe ISR and
	 * OS task operation
	 */
	ENTER_CRITICAL(status);
	
	/* 
	 * Here we are going to call the suspend routine to enter suspend
	 * There is two type of suspend and it depend on the user use ROM
	 * as boot code or NAND as boot code, there is difference because
	 * of the IC design, please choose the #if - #else for your proper
	 * setting, the default is NAND boot.
	 *
	 * The default will set DRAM under self-reflsh 
	 */

	/* If you use external ROM for firmware code, please choose this */
	#if 0 
	rombootSuspendEnableSet(0,1);
	#else
	/* If you use on board NAND for firmware code, please choose this */
	#ifdef RAM_PROGRAM
	imemRefill(nandbootSuspendEnableSet, nandbootSuspendEnableSet + 1023);
	#endif
	nandbootSuspendEnableSet(0, 1);
	#endif
	
	/*The system Sleep Here */
	
	/* Please put this two routine here for clear the flag */
	hwUiResumeEnableSet(0);
	hwUiResumeFlagSet(0);

	/* 
	 * Re-enable all user's interrupt source again, here is the interrupt 
	 * of SPCA536A's release code
	 */
	WRITE8(0xB00010d0,0x10);
	WRITE8(0xB00010d1,0x00);	
	WRITE8(0xB00010d2,0x02);	
	WRITE8(0xB00010d3,0x00);	
	WRITE8(0xB00091d0,0x02);

	/* Restart the ISR and OS task operation */
	EXIT_CRITICAL(status);
 }		

