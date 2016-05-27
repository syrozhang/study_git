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
#include	"general.h"
#include	"stdio.h"

#if	1
/*
17:	BACKLIGHT
18: _RESET
19: DATA
20: CLK
21: LOAD
*/
	#define	GPO_ENABLE	0xb000a0a6
	#define	GPO_VALUE	0xb000a0a3
	#define	DATA	0x08
	#define	CLK		0x10
	#define	LOAD	0x20
	#define	ACTIVE	(0x04+0x02)	/* RESET + BL */
#else
/*
DIGTVGPIO 12        <=> XCS 
DIGTVGPIO 13        <=> DAT 
DIGTVGPIO 14        <=> CLK 
*/
	#define	GPO_ENABLE	0xb000a0a5
	#define	GPO_VALUE	0xb000a0a2
	#define	DATA	0x20
	#define	CLK		0x40
	#define	LOAD	0x10
	#define	ACTIVE	(0x04+0x02)	/* RESET + BL */
#endif

/*
    Vendor      BitLength   LowBitFirst
    ------      ---------   -----------
    AUO             16          0
    CASIO           12          1
    SAMSUNG         24          0
    SONY            16          0
    Toppoly         16          0
*/
void	DispSerialCtrl(UINT32 InputData, UINT32 BitLength, UINT32 LowBitFirst)
{
	UINT32	i, j, tmp;	/* Load = digtv21 ; SCLK = digtv20 ; SDAT: digtv19 */
	UINT32	data;
	if(LowBitFirst)
		data = InputData;
	else
	{
		data = 0;
		for(i=0; i<24; i++)
		{
			if(InputData & 0x800000)
				data |= (0x0001 << i);
			InputData <<= 1;
		}
		data >>= (24 - BitLength);
	}
	WRITE8(GPO_VALUE,ACTIVE);	/* output value of digtv[21,20,19,18,17,16] */
	WRITE8(GPO_ENABLE,DATA+CLK+LOAD+ACTIVE);	/* 0x38;output enable of digtv[21,20,19,18,17,16] */
	tmp = 0;
	for(i=0; i<BitLength; i++)
	{                                           	
		if(data & 0x0001)                     
			tmp |= DATA;
		else
			tmp &= ~DATA;
		tmp |= ACTIVE;
		WRITE8(GPO_VALUE, tmp);	/* SDATA */
		tmp |= CLK;
		WRITE8(GPO_VALUE, tmp);	/* SCLK */ 
		for(j=0; j<3; j++);
		tmp &= ~CLK;
		data >>= 1;
		WRITE8(GPO_VALUE, tmp);	/* SCLK */ 
	}
	tmp |= LOAD;
	WRITE8(GPO_VALUE, tmp);	/* LOAD */ 
}

#if	0
/* COM15T1181, bit[2:0]=reg, bit[3]=TEST_MODE, bit[11:4]=data */
void Casio2G_write(UINT32 nData)
{
	
	UINT32	i, tmp;
	UINT32	status;
	ENTER_CRITICAL(status);
	/* Load = digtv21 ; SCLK = digtv20 ; SDATA = digtv19 */
	WRITE8(0xb000a0a3,0x00);	/* output value of digtv[21,20,19,18,17,16] */
	WRITE8(0xb000a0a6,0x38);	/* 0x38;output enouble of digtv[21,20,19,18,17,16] */
	
	tmp = 0;
	for(i=0; i<12; i++)
	{
		if(nData & 0x01)
			tmp |= 0x08;
		else
			tmp &= 0xF7;
		WRITE8(0xb000a0a3, tmp);	/* SDATA */

		tmp |= 0x10;
		WRITE8(0xb000a0a3, tmp);	/* SCLK */
		nData = nData >> 1;
		tmp &= ~0x10;
		WRITE8(0xb000a0a3, tmp);
	}
	tmp |= 0x20;
	WRITE8(0xb000a0a3, tmp);		/* LOAD */
	EXIT_CRITICAL(status);
}
#endif












