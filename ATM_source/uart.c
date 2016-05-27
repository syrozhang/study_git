
#include "general.h"
#include "stdio.h"
#include "sysregs.h"
#include "irq.h"
#include "os_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MAX_UART_RX_CH 			256

void uartRxIsr(void);
//void uart1RxIsr(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
uartReg_t* puartReg;
OS_EVENT*      uartRxQueId;               /* Message queue ID for key scan     */
UINT32     uartRxQue[MAX_UART_RX_CH]; /* Message queue buffers            */
UINT32 tout;

extern UINT8 linebuf[64];

/**************************************************************************
 *                                                                        *
 *  Function Name: hwUartInit                                             *
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
void hwUartInit(
	UINT32 uartIdx,/* 0:enable UART0, 1:enable UART1 */
	UINT32 baud    /* example : 115200 */
	)
{
	UINT32 baudDiv, baudRemainder;

	if ( uartIdx == 0 )
	{
		puartReg = (uartReg_t *)0xb0000000;/* The UART0 register mapping */
	}
	else 
	{
		puartReg = (uartReg_t *)0xb0000010;/* The UART1 register mapping */
	}
	baudDiv       = 48000000 / baud;
	baudRemainder = 48000000 % baud;
	puartReg->enable = 0;
	puartReg->enable = 1;
	puartReg->mode = 0;
	puartReg->baudDiv[0] = (baudDiv      ) & 0x000000ff;
	puartReg->baudDiv[1] = (baudDiv >> 8 ) & 0x000000ff;
	puartReg->baudDiv[2] = (baudDiv >> 16) & 0x000000ff;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uartInit                                               *
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
void uartInit(UINT32 uartIdx)
{
	if ( uartIdx == 0 ) {
		irqIsrReg(UART0_RX_IRQ, uartRxIsr); 
		/* The uartRxIsr() is default define for RS232 debuger 
		 * which is include inside the libhw.a alraedy
		 */ 
	}
	else {
		/* 
		 * There is two interrupt source but once the UART1_RX_FULL_IRQ
		 * is occuring, the UART1_RX_IRQ will not occure
		 */
//		irqIsrReg(UART1_RX_IRQ, uart1RxIsr);
	}
	uartRxQueId = osQueCreate("UART_RX_FIFO", (void *)uartRxQue, MAX_UART_RX_CH);
	if ( uartRxQueId == NULL ) 
	{
		printf("Uart0 que create err!\n");
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: uartRxIsr                                               *
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
 void uartRxIsr(void)
{
	cpuReg_t *pcpuReg;
	volatile UINT32 reg;	
	UINT32 ret;
	UINT32 ch;
	
	pcpuReg = (cpuReg_t *)0xb0001000;
	reg = (1 << UART0_RX_IRQ);	
	*(UINT32 *)0xb00010c0 = reg;
//	pcpuReg->irqSts = reg;

	/* 
	 * Instead of using ISR method to get the UART data, 
	 * the user could use polling method by using below's
	 * while loop to avoid the UART possible data loss 
	 */
	 
	while ( (puartReg->sts & 0x02) == 0x02 ) {
		ch = (UINT32 )puartReg->rxData;
		ret = osQuePost(uartRxQueId, (void *)ch);
		if ( ret == OS_Q_FULL ) {
			while ( (puartReg->sts & 0x02) == 0x02 ) {
				ch = (UINT32 )puartReg->rxData;
			}
			
			break;
		}
	}

}

/**************************************************************************
 *                                                                        *
 *  Function Name: fgetchar                                               *
 *                                                                        *
 *  Purposes:                                                             *
 *    Get a character from UART.                                          *
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
int fgetchar(void)
{
#if 0
	UINT32 ch, err;

	ch = (UINT32 )osQuePend(uartRxQueId, 1, &err);

	return (int )(ch & 0x000000ff);
#else
	UINT32 ch,err;

	ch = (UINT32)osQuePend(uartRxQueId, 0xffffffff, &err);

	if( err==0 )
		return (int )(ch & 0x000000ff);
	else
		return 0xffff;
#endif
}
/**************************************************************************
 *                                                                        *
 *  Function Name: fputchar                                               *
 *                                                                        *
 *  Purposes:                                                             *
 *    Send a character to UART.                                           *
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
static UINT8 ui8FileFlag = 0;
static UINT8 ui8Index;
UINT8 ui8SerialData[20];

void fputchar(UINT8 ch)
{	
	UINT8 txRdy;
	
#if 0
	if (ui8FileFlag)	
	{/* Paul@2005/10/22 for Mp3 words */
		if (ch == '>')
		{
			ui8FileFlag = 0;
			ui8SerialData[ui8Index] = 0;
		}
		else
		{
			if (ui8Index < 20)
			{
				ui8SerialData[ui8Index] = ch;
				ui8Index++;
			}
		}
	}
	
	do
	{
		txRdy = puartReg->sts;
	} while( !(txRdy & 0x20) );		/* Wait to send data */

	puartReg->txData = ch;
	
	if (ch == '<')
	{/* Paul@2005/10/22 for Mp3 words */
		ui8Index = 0;
		ui8FileFlag = 1;
	}	
#else	
	do
	{
		txRdy = puartReg->sts;
	} while( !(txRdy & 0x20) );		/* Wait to send data */

	puartReg->txData = ch;
#endif

}

/**************************************************************************
 *                                                                        *
 *  Function Name: toutSet                                                *
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
void toutSet(UINT32 flag)
{
	tout = flag;
	
	return;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: sio_printf                                                *
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
#if 0 
void sio_printf(const char *format, ...)
{
	if (tout == 0)
	{
		sio_vprintf(linebuf, format, ...);		
		sio_puts(linebuf);
	}
}
#endif 

 
