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
 *                                                                        *
 **************************************************************************/
/**************************************************************************
 *                                                                        *
 *  Basic Introduction of uart1.c                                          
 *                                                                        
 *  Purposes:                                                             
 *		The default debug message of SPAC536A use UART0, while if user
 *      want to use UART1 for other communication with external device
 *      The UART1 could support this job and UART0 still used as debug
 *      message port
 *                                                                        
 *  The Basic API:                                                         
 *		hwUart1Init() : 
 *			To enable the UART1 and set up the baud rate, the method 
 *			is identical to UART0
 *		hwUart1Disable() : 
 *			To disable the UART1, please note that the UART1 use 
 *			FMGPIO41,42, while under disable state the UART1 is set 
 *			as GPIO
 *		uart1BufferClear() :
 *			To clear the data port of UART1, this might useful if the 
 *			user are reciving/sending data packet by packet 
 *		uart1Init() :
 *			To intial the UART1 ISR routine and create a meaasge Que
 *			User might have to choose ISR method or Polling method,
 *			From experence, the polling method could avoid possibel data 
 *			loss
 *		uart1RxIsr() :
 *			The ISR service routine, this routine is identical as UART0 
 *		uart1RxPolling() :
 *			The polling method to get data from UART0, please note that
 *			here is just a sample code for demo enter a forever while 
 *			loop and break out if get 1000 bytes from UART0
 *		uart1fputchar() :
 *			To send data to UART1
 *  	uart1fgetchar() :
 *			To receive data from UART1 by ISR message que
 *		uart1SampleCode() :
 *			The sample code could be call directly maybe inside 
 *			customerCmd(), this just demo how UART0 and UART1 work together
 *			The user might need two PC, one is for displaying debugger 
 *			message and enter the SPCA536A's cmd, another PC is used to
 *			send data to UART1, the sending data will display on UART0, 
 *			We often use HyperTerminal to send a *.txt file.
 **************************************************************************/
//#define	RS_232_DBG
 
#include "general.h"
#include "stdio.h"
#include "sysregs.h"
#include "irq.h"
#include "os_api.h"
#include "gpio.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MAX_UART_RX_CH 			4096*2

#define C_LONG_DATA_SIZE		1024

#define RS232_DATA_START		0xc0
#define RS232_DATA_END  		0xd0

#define RS232_CMD_RESET		0x01
#define ID_NUM					16
#define PACKAGE_NUM			8

#define	RECEIVE_KEY			0
#define	RECEIVE_ID			1
#define	RECEIVE_DECODE		2
#define	RECEIVE_PACKAGE		3
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 tmrTickGet(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void uart1RxIsr(void);
void uart1RxFullIsr(void);
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
uartReg_t* puart1Reg;
OS_EVENT*      uart1RxQueId;               /* Message queue ID for key scan     */
UINT32     uart1RxQue[MAX_UART_RX_CH]; /* Message queue buffers            */

static UINT16 ui16RS232Sending = 0;
static UINT8  ui8PackDataIndex = 0;

static UINT8  ui8StartReceive = 0;
static UINT16 ui16DataIndex;
static UINT8 *ui8PackDataPtr;

UINT32 ui32SysKeyState;
/**************************************************************************
 *                                                                        *
 *  Function Name: hwUart1Init                                            *
 *                                                                        *
 *  Descriptions:                                                         *
 *      To enable the UART0(uartIdx = 0) or UART1(uartIdx = 1)            *
 *      The default RS242 debug CMD use UART0                             *
 *                                                                        *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void hwUart1Init(
	UINT32 uartIdx,/* 0:enable UART0, 1:enable UART1 */
	UINT32 baud    /* example : 115200 */
	)
{
	UINT32 baudDiv, baudRemainder;	
	cpuReg_t *pcpuReg;
	UINT32 sr;	

	pcpuReg = (cpuReg_t *)(0xb0000000);
	puart1Reg = (uartReg_t *)0xb0000010;/* The UART1 register mapping */

	ENTER_CRITICAL(sr);
	pcpuReg->irqEn &= (~((1<<UART1_RX_FULL_IRQ) | (1<<UART1_RX_IRQ)));
	EXIT_CRITICAL(sr);
	
	baudDiv       = 48000000 / baud;
	baudRemainder = 48000000 % baud;
	
	puart1Reg->mode = 0;
	puart1Reg->baudDiv[0] = (baudDiv      ) & 0x000000ff;
	puart1Reg->baudDiv[1] = (baudDiv >> 8 ) & 0x000000ff;
	puart1Reg->baudDiv[2] = (baudDiv >> 16) & 0x000000ff;
	
	puart1Reg->enable = 0;
	puart1Reg->enable = 1;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: hwUart1Disable                                         *
 *                                                                        *
 *  Descriptions:                                                         *
 *      To disable the UART1           									  *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void hwUart1Disable(void)
{
	UINT8 temp;
	puart1Reg = (uartReg_t *)0xb0000010;/* The UART1 register mapping */
	puart1Reg->enable = 0;

	/* Here set the UART1 pin as GPIO for stoping interrupt occure */
	/* set uart TXD1 at output and RXD1 at input(RX:FMGPIO41,TX:FMGPIO42) */
//	temp = READ8(0xb0004015) | 0x04;	/* bit 2 = 1 */
//	temp = temp & 0xfd;					/* bit 1 = 0 */
//	WRITE8(0xb0004015,temp);
	hwFmGpioCfgSet(0x00, 0x00, 0x400, 0x600);	/* Paul@2006/01/20 add */
	/* set uart TXD1 and RXD1 output value at high */
	WRITE8(0xb000400d,(READ8(0xb000400d) | 0x06));
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uart1BufferClear                                       *
 *                                                                        *
 *  Descriptions:                                                         *
 *      Read 4 bytes from UART1 buffer to clear(reset) the buffer		  *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void uart1BufferClear(void)
{
	UINT32 count;
	UINT32 ch;

	while (puart1Reg->sts & 0x01);
	for (count = 0; count < 4; count++) 
	{
		ch = puart1Reg->rxData;
		printf("[%x]",ch);
	}	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uart1Init                                              *
 *                                                                        *
 *  Descriptions:                                                         *
 *         Register Uart Irq.                                             *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void uart1Init(UINT32 uartIdx)
{
	UINT32 sr;
	
	if ( uartIdx == 0 ) {
		 #if 0
		 irqIsrReg(UART0_RX_IRQ, uartRxIsr); 
		 #endif
//		ENTER_CRITICAL(sr);		  
// 	    irqIsrRemove(UART1_RX_IRQ);
 		irqIsrReg(UART1_RX_FULL_IRQ, uart1RxFullIsr); 				 
//		EXIT_CRITICAL(sr);		
		/* The uartRxIsr() is default define for RS232 debuger 
		 * which is include inside the libhw.a alraedy
		 */ 
	}
	else {
		/* 
		 * There is two interrupt source but once the UART1_RX_FULL_IRQ
		 * is occuring, the UART1_RX_IRQ will not occure
		 */
// 		irqIsrReg(UART1_RX_FULL_IRQ, uart1RxFullIsr); 				 		 
		irqIsrReg(UART1_RX_IRQ, uart1RxIsr);		
/*		
 	    	ENTER_CRITICAL(sr);		 
 		irqIsrReg(UART1_RX_IRQ, uart1RxIsr);
		irqIsrRemove(UART1_RX_FULL_IRQ);
		EXIT_CRITICAL(sr);
*/		
	}	
	uart1RxQueId = osQueCreate("UART1_RX_FIFO", (void *)uart1RxQue, MAX_UART_RX_CH);
	if ( uart1RxQueId == NULL ) 
	{
		sio_printf("Uart1 que create err!\n");
	}
	gpioConfig((GPIO_TYPE_FM+41), 0);	//xouer081125
	gpioConfig((GPIO_TYPE_FM+42), 1);

}

#if 1
/**************************************************************************
 *                                                                        *
 *  Function Name: uart1RxIsr                                               *
 *                                                                        *
 *  Purposes:                                                             *
 *      Here is use ISR to get the UART1 data from the                    *
 *      data Que uart1RxQueId                                             *
 *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void uart1RxIsr(void)
{
	cpuReg_t *pcpuReg;
	volatile UINT32 reg;
	UINT32 ret;
	UINT32 ch;
	
	pcpuReg = (cpuReg_t *)0xb0000020;
	reg = (1 << UART1_RX_FULL_IRQ) | (1 << UART1_RX_IRQ);
	*(UINT32 *)0xb00010c0 = reg;
//	pcpuReg->irqSts = reg; /* Write "one" to clear */

	/* 
	 * Instead of using ISR method to get the UART data, 
	 * the user could use polling method by using below's
	 * while loop to avoid the UART possible data loss 
	 */
	 
	while ( (puart1Reg->sts & 0x02) == 0x02 ) 
	{
		ch = (UINT32 )puart1Reg->rxData;
		ret = osQuePost(uart1RxQueId, (void *)ch);
		if ( ret == OS_Q_FULL ) 
		{
			while ( (puart1Reg->sts & 0x02) == 0x02 ) 
			{
				ch = (UINT32 )puart1Reg->rxData;
			}
			break;
		}
	}
	
}
#endif

/**************************************************************************
 *                                                                        *
 *  Function Name: uart1RxIsr                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *      Here is use ISR to get the UART1 data from the                    *
 *      data Que uart1RxQueId                                             *
 *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
#if 0
void uart1RxIsr(void)
{
	cpuReg_t *pcpuReg;
	volatile UINT32 reg;		
	UINT32 ret;
	UINT32 ch;
	
	pcpuReg = (cpuReg_t *)0xb0001000;
	reg = ((1 << UART1_RX_IRQ) | (1 << UART1_RX_FULL_IRQ));
	*(UINT32 *)0xb00010c0 = reg;	
//	pcpuReg->irqSts = reg;

	/* 
	 * Instead of using ISR method to get the UART data, 
	 * the user could use polling method by using below's
	 * while loop to avoid the UART possible data loss 
	 */
	
	while ( (puart1Reg->sts & 0x02) == 0x02 ) 
	{		
		ch = (UINT32 )puart1Reg->rxData;		
	}
}
#endif

/**
**/
void uart1RxFullIsr(void)
{
	cpuReg_t *pcpuReg;
	volatile UINT32 reg;		
	UINT32 ch;
	UINT32 ret;
	
	pcpuReg = (cpuReg_t *)0xb0001000;
	reg = ((1 << UART1_RX_IRQ) | (1 << UART1_RX_FULL_IRQ));
	*(UINT32 *)0xb00010c0 = reg;		

	/* 
	 * Instead of using ISR method to get the UART data, 
	 * the user could use polling method by using below's
	 * while loop to avoid the UART possible data loss 
	 */
	while ( (puart1Reg->sts & 0x02) == 0x02 ) 
	{		
		ch = (UINT32 )puart1Reg->rxData;
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uart1RxPoll                                               *
 *                                                                        *
 *  Purposes:                                                             *
 *      Here is use polling method to get the UART1 data from the                    *
 *      data Que uart1RxQueId                                             *
 *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void uart1RxPolling(void)
{
	UINT32 ch,count;

	count = 0;	
	while(1) {
		if( count > 1000 ) {
			/* if receive over 1000 char, break */
			printf("over 1000\n");
			break;
		}	
		while ( (puart1Reg->sts & 0x02) == 0x02 ) {
			count ++;
			ch = (UINT32 )puart1Reg->rxData;
			printf("ch[%x]\n",ch);
		}	
	}	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uart1fputchar                                               *
 *                                                                        *
 *  Purposes:                                                             *
 *    Send a character to UART1.                                           *
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
void uart1fputchar(UINT8 ch)
{
	UINT8 txRdy;

	do{
		txRdy = puart1Reg->sts;
	} while( !(txRdy & 0x20) );		/* Wait to send data */
	
	puart1Reg->txData = ch;
}

int uart1fgetc(void)
{
	UINT32 ch,err;

	ch = (UINT32)osQuePend(uart1RxQueId, 0xffffffff, &err);

	if( err==0 )
		return (int )(ch & 0x000000ff);
	else
		return 0xffff;
}
//*********************************************************************
void 	uart1QFlush()
{
	UINT32	status;
   OS_Q *pq;
   ENTER_CRITICAL(status);
   pq = uart1RxQueId->osEventPtr;
   pq->osQueIn = pq->osQueStart;
   pq->osQueOut = pq->osQueStart;
   pq->osQueEntries = 0;
   EXIT_CRITICAL(status);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uart1fgetchar                                               *
 *                                                                        *
 *  Purposes:                                                             *
 *    Get a character from UART1.                                          *
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
UINT32 uart1fgetchar(UINT32 timeout, UINT32* pError)
{
#if 0	
	UINT32 ch, err;

	ch = (UINT32 )osQuePend(uart1RxQueId, 0, &err);
#endif

	UINT32 ch;
	UINT32 time = 0;

	*pError = 0;
	while (1)
	{
		hwWait(0, 1);
		if ((puart1Reg->sts & 0x02) == 0x02)
		{
			break;
		}
		time++;
		if (time == timeout)
		{
			*pError = 1;
			break;
		}
	}
	
	if (*pError == 0)
	{
		ch = puart1Reg->rxData;
	}

	return (UINT32)(ch & 0x000000ff);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uart1SampleCode                                               *
 *                                                                        *
 *  Purposes:                                                             *
 *    Get data from UART1(could send a file from PC by COM port) 
 *    and printf to UART0(The default debugger message )                  *
 *                                                                        *
 *  Descriptions:                                                         *
 *    This sample procedure might need 2 PC, one is sending data to UART1
 *    Another is the default debugger
 *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void uart1SampleCode(void)
{
	hwUart1Init(1, 115200);
	#if 0
	uart1Init(1);	/* This is used for ISR get/send data */
	#else
	uart1RxPolling();
	#endif
}

/**
**/
UINT32 GetSystemKey(void)
{
	return ui32SysKeyState;
}
