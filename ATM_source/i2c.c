/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2003 by Worldplus Technology Co., Ltd.           *
 *                                                                        *
 *  This software is copyrighted by and is the property of Worldplus      *
 *  Technology Co., Ltd. All rights are reserved by Worldplus Technology  *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Worldplus Technology Co., Ltd.                     *
 *                                                                        *
 *  Worldplus Technology Co., Ltd. reserves the right to modify this      *
 *  software without notice.                                              *
 *                                                                        *
 *  Worldplus Technology Co., Ltd.                                        *
 *  2nd Floor,R2-B,Hi-tech.S,7th Road, Shenzhen High-tech Industrial Park *
 *  Shenzhen, China                                                       *
 *                                                                        *
 *  Author: Jinch/WorldPlus                                               *
 *                                                                        *
 **************************************************************************/
#include "general.h"
#include "hw_gpio.h"
#include "stdio.h"


#define	SDA_LOW		hwTgGpioMsbMaskWrite(0x00,0x80)		/*hwGenGpioMaskWrite(0x0000,0x0400)*/
#define	SDA_HIGH	hwTgGpioMsbMaskWrite(0x80,0x80)     /*hwGenGpioMaskWrite(0x0400,0x0400)*/
#define	SCL_LOW		hwTgGpioMsbMaskWrite(0x00,0x40)     /*hwGenGpioMaskWrite(0x0000,0x0200)*/
#define	SCL_HIGH	hwTgGpioMsbMaskWrite(0x40,0x40)     /* hwGenGpioMaskWrite(0x0200,0x0200)*/

void i2c_delay(void)
{
	UINT8 i;
	for(i=0;i<1;i++);
}

void i2c_delay64us(void)
{
	UINT32 i;
	for(i=0;i<1000;i++);
}


void i2c_start(void)
{
	SCL_HIGH;
	SDA_HIGH;
	i2c_delay();
    SDA_LOW;
    i2c_delay();
    SCL_LOW;
    i2c_delay();
}

void i2c_stop(void)
{
	SDA_LOW;
	i2c_delay();
	SCL_HIGH;
	i2c_delay();
	SDA_HIGH;
	i2c_delay();
}

void i2c_init(void)
{
	hwTgGpioCfgSet(0x00,0x00,0xc0,0xc0);   /*hwGenGpioCfgSet(0x0600,0x0600);*/
}

void i2c_clock(void)
{
	SCL_HIGH;
	i2c_delay();
	i2c_delay();
	SCL_LOW;
	i2c_delay();
}


UINT8 i2c_send(UINT8 i2c_schar)
{
	UINT8 i,ack;

	hwTgGpioCfgSet(0x00,0x00,0xc0,0xc0);  /*hwGenGpioCfgSet(0x0600,0x0600);*/

	for (i=0;i<8;i++)
	{
		if(i2c_schar&0x80)
			SDA_HIGH;
		else SDA_LOW;

		i2c_schar <<= 1;
		i2c_delay();
		i2c_clock();
	}
	SDA_HIGH;

	hwTgGpioCfgSet(0x00,0x00,0x40,0xc0);   /*hwGenGpioCfgSet(0x0200,0x0600);*/

	SCL_HIGH;
	i2c_delay();
	i2c_delay();
	/*Slave ACK!*/
	if(hwTgGpioMsbRead()&0x80)
		ack = 0;
	else ack = 1;

	SCL_LOW;
	i2c_delay();

	hwTgGpioCfgSet(0x00,0x00,0xc0,0xc0);  /*hwGenGpioCfgSet(0x0600,0x0600);*/

	return ack;
}


UINT8 i2c_receive(void)
{
	UINT8 i2c_rchar = 0;
	UINT8 i;
	hwTgGpioCfgSet(0x00,0x00,0x40,0xc0);   /*hwGenGpioCfgSet(0x0200,0x0600);*/
	for( i=0;i<8;i++)
	{
		i2c_rchar <<=1;

		SCL_HIGH;
		i2c_delay();

		if(hwTgGpioMsbRead()&0x80)
		i2c_rchar++;

		i2c_delay();
		SCL_LOW;
		i2c_delay();
	}
	hwTgGpioCfgSet(0x00,0x00,0xc0,0xc0);   /*hwGenGpioCfgSet(0x0200,0x0600);*/
	return(i2c_rchar);
}


void i2c_nack(void)
{
	SDA_HIGH;
	i2c_delay();
	i2c_clock();
	SDA_HIGH;
}

void i2c_ack(void)
{
	SDA_LOW;
	i2c_delay();
	i2c_clock();
	SDA_HIGH;
}

/*============================================================*
*	read and write functions for devices
*=============================================================*/
void wr_byt_i2c(UINT8 i2caddr,UINT8 regaddr,UINT8 dat)
{
	UINT8 temp;

for(temp=0;temp<5;temp++)
{
	i2c_init();
	i2c_start();

	if( !i2c_send(i2caddr) )
		{
			i2c_stop();
			continue;
		}

	if( !i2c_send(regaddr) )
		{
			i2c_stop();
			continue;
		}
	if( !i2c_send(dat) )
		{
			i2c_stop();
			continue;
		}
		break;
}
	SCL_LOW;
	i2c_delay64us();		/*Wait tvp5150*/
	i2c_stop();
}




UINT8 re_byt_i2c(UINT8 i2caddr,UINT8 regaddr)
{
	UINT8 read;

	i2c_init();
	i2c_start();
	i2c_send(i2caddr);
	i2c_send(regaddr);
	i2c_stop();

	i2c_start();
	i2c_send(i2caddr+1);

	SCL_LOW;
	i2c_delay64us();		/*Wait tvp5150 prepare data*/

	read = i2c_receive();
	i2c_nack();

	i2c_stop();

	return read;
}

