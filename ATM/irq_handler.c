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
#include "general.h"
#include "sysregs.h"
#include "irq.h"
#include "stdio.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define IRQ_DEBUG 1

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void irqLbusErrHandler(void);
void irqInit(void);

/**************************************************************************
 *                          L O C A L    D A T A                          *
 **************************************************************************/
irqIsr_t irqIsrArray[32];

/**************************************************************************
 *                                                                        *
 *  Function Name: irqProcess                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *    Extract the source of interrupts and execute the corresponding      *
 *    service routine.                                                    *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    cause: in, for debugging purpose (CP0 cause).                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
#if (IRQ_DEBUG == 1)
void
irqProcess(
	UINT32 cause,
	UINT32 epc,
	UINT32 sp
)
#else
void
irqProcess(
	void
)
#endif
{
	cpuReg_t *pcpuReg;
	UINT32 irqSts;
	irqIsr_t pisr;
	UINT32 irqVecNo;

	#if (IRQ_DEBUG == 1)
	if ( (cause & 0x000000ff) != 0 ) {
		printf("Program err: CP0_CAUSE=0x%x, CP0_EPC=0x%x\n", cause, epc);
		while ( 1 );
	}
	#endif

	pcpuReg = (cpuReg_t *)(0xb0000000);
	irqSts = pcpuReg->irqSts;
	irqVecNo = 0;
	#if 0
	printf("irqSts=0x%x\n", irqSts);
	#endif
	while ( irqSts ) {
		if ( irqSts & 0x00000001 ) {
			pisr = irqIsrArray[irqVecNo];
			if ( pisr != NULL ) {
				if ( irqVecNo == 31 ) {
					printf("EPC=0x%x, sp=0x%x\n", epc, sp);
				}
				(*pisr)();
			}
		}
		irqSts = irqSts >> 1;
		irqVecNo++;
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: irqIsrReg                                              *
 *                                                                        *
 *  Purposes:                                                             *
 *    Hook an interrupt service routine (ISR) with the specified          *
 *    interrupt vector.                                                   *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    irqVecNo: in, interrupt vector number.                              *
 *    pfunc: in, function pointer of an interrupt service routine.        *
 *                                                                        *
 *  Returns: None.                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
irqIsrReg(
	UINT32 irqVecNo,
	void *pfunc
)
{
	cpuReg_t *pcpuReg;
	volatile UINT32 reg;

	pcpuReg = (cpuReg_t *)(0xb0000000);
	irqIsrArray[irqVecNo] = pfunc;
	/* Workaround for the missed mapping of interrupt mask and enable bit */
	if ( irqVecNo != FRONT_IRQ ) {
		switch ( irqVecNo ) {
		case TG_GPIO_CNT_IRQ_IRQ:
			irqVecNo = MS_IRQ;
			break;
		case RTC_IRQ:
			irqVecNo = FRONT_IRQ;
			break;
		default:
			break;
		}
		reg = pcpuReg->irqEn;
		reg = reg | (1 << irqVecNo);
		pcpuReg->irqEn = reg;
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: irqLbusErrHandler                                      *
 *                                                                        *
 *  Purposes:                                                             *
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
void
irqLbusErrHandler(
	void
)
{
	printf("LBus err!\n");
	while ( 1 );
}

/**************************************************************************
 *                                                                        *
 *  Function Name: irqInit                                                *
 *                                                                        *
 *  Purposes:                                                             *
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
void
irqInit(
	void
)
{
	cpuReg_t *pcpuReg;
	UINT32 i;

	pcpuReg = (cpuReg_t *)(0xb0000000);
	for ( i = 0; i < 32; i++ ) {
		irqIsrArray[i] = NULL;
	}
	#if 0
	irqIsrReg(LBUS_ERR_IRQ, irqLbusErrHandler);
	#endif
	pcpuReg->lbusSts = 0x01;
}
