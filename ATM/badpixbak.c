/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2002 by Sunplus Technology Co., Ltd.             *
 *                                                                         *
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
 *  Author: Eric Su                                                       *
 *                                                                        *
 **************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "device.h"
#include "general.h"
#include "stdio.h"
#include "usb_api.h"
#include "sysg_api.h"
#include "aaa_api.h"
#include "hw_usb.h"
#include "hw_front.h"
#include "sysregs.h"

#include "hw_sdram.h"
#include "badpix.h"
#include "os_api.h"

/* **************************************************************
 * The BNDRY_CANCEL is to do the boundary bad pixel compensation
 * To open the "BNDRY_CANCEL", the user has to update the and/after 
 * V186 "libutil.a" 
 ****************************************************************/
#if 0 
#define    BNDRY_CANCEL
#endif
/* **************************************************************
 * To open the "BAD_2_CONTINOUS_CEL", the user has to update 
 * the after V186 "libdsp.a" and "lib2bdp.a" 
 ****************************************************************/
#if 0 
#define    BAD_2_CONTINOUS_CEL
#endif

extern void    nandRsvBlkRead(UINT32, UINT32, UINT8*);
extern void    nandRsvBlkWrite(UINT32, UINT32, UINT8*);
#ifdef BNDRY_CANCEL
extern UINT32 badPixelBndryOneCel(UINT32 , UINT32 ,UINT32 ,UINT32 , UINT32 ,UINT32);
#endif
#ifdef BAD_2_CONTINOUS_CEL
extern UINT32 badPixel2ContiniousCel(UINT32, UINT32 , UINT32 ,UINT32 ,UINT32 , UINT32 ,UINT32);
extern UINT32 badpixel_Rearrange(UINT8 * ,UINT8 * ,UINT8 *, UINT32, UINT32);
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: badPixelInit                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions: 
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void 
badPixelInit(
	void
)
{        
	UINT32 addr;
	UINT32 badpixoffset,badpixcount;	
	dprint("\n");                 	
	sysgMemBadPixCoordAddrGet(&addr);/*Get badpix SDRAM address*/
	addr = (addr << 1) | 0xac000000;
	dprint1("badpix coordinate addr=0x%x\n",addr);
		
	sysgBadPixInfoSet(0,0x1000);/*reserve block offset 0 , 4096 badpixel*/
	/*Get badpix offset,count inside reserve block*/
	sysgBadPixInfoGet(&badpixoffset,&badpixcount);
	dprint1("badpixoffset=%x\n",badpixoffset);
	dprint1("badpixcount=%d\n",badpixcount);                 
	badPixelRsvBlkLoad(addr,badpixoffset,badpixcount);
	
	/* Set the threshold of the edge detection*/
	hwBadPelThdSet(25);
	dprint("\n");                 
}


/**************************************************************************
 *                                                                        *
 *  Function Name: badPixelRsvBlkLoad                                             *
 *                                                                        *
 *  Purposes:                        *
 *                                                                        *
 *  Descriptions: 
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
badPixelRsvBlkLoad
(
	UINT32 addr,
	UINT32 badpixoffset,
	UINT32 badpixcount
)
{
	UINT32 buf[2];
	UINT8 *dramAddr = (UINT8 *)addr;
	UINT32 skipPages, len;
	usbReg_t* pusbReg;

	buf[0] = badpixoffset;
	buf[1] = badpixcount * BytePerBadPixel;
	
	if(badpixcount == 0)
		return;
		
	pusbReg = (usbReg_t *)(0xb0000000);		
	len = buf[1];

	skipPages = 0;
	while (buf[0] >= 512) {
		buf[0] -= 512;
		skipPages++;				
	}
	
	dprint2("Load badpix data %d bytes to Sdram Addr = 0x%x\n",buf[1],dramAddr);                 			
	nandRsvBlkRead(skipPages,(((buf[0] + buf[1]) - 1) >> 9) + 1,dramAddr);
	
}


/**************************************************************************
 *                                                                        *
 *  Function Name: badPixelWriteToRsvBlk                                   *
 *                                                                        *
 *  Purposes:                        *
 *                                                                        *
 *  Descriptions: 
 *                                                                        *
 *  Arguments:                                                            *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
badPixelWriteToRsvBlk(
	UINT32 *argBuf
)
{
	UINT32 badpixoffset;
	UINT32 badpixcount;
	UINT8 *pbuf;
	#ifdef BAD_2_CONTINOUS_CEL
	UINT8 *pbuf1;
	UINT8 *pbuf2;
	UINT32 width,hight;
	#endif
	
	sysgBadPixInfoGet(&badpixoffset,&badpixcount);
	dprint1("badpixoffset= %x\n",badpixoffset);
	dprint1("badpixcount = %x\n",badpixcount);

	argBuf[1] = badpixcount * BytePerBadPixel;

	pbuf = osMemAlloc( badpixcount * BytePerBadPixel );
	if( pbuf == NULL ){
		printf("Buf Fail\n");
		return;
	}	
	/* Get data from USB bulk tp pbuf */
	hwPcToUsbBulkout((UINT32)pbuf, (badpixcount * BytePerBadPixel));
	
	#ifdef BAD_2_CONTINOUS_CEL
	/* **************************************************************************
	 * Because to do the 2 continious bad pixel compensation is different to
	 * 1 bad pixel compensation, we need to identify the two different approach.
	 * Here is to analyze the original "BadPixel.bin" which is sorted by PC 
	 * software and was arranged from left-top to down-right with ended mark
	 * by 0xffff 0xffff
	 *
	 * Here we are going to analyze the "BadPixel.bin", sort out the 2 
	 * continious bad pixels and add a new marker "0xfefe 0xfefe" to tell the 
	 * routine badPixelStillModeCel() that there is 2 bad pixel exit and call 
	 * the realted compensation routine badPixel2ContiniousCel()
	 *
	 * If the PC software will do this sorting, than this firmware rearrange 
	 * should be removed.
	 * The re-arrange routine is badpixel_Rearrange()
	 * 
	 * To open the "BAD_2_CONTINOUS_CEL" option, the user has to add the 
	 * "libdsp.a", "lib2bdp.a"
	 ****************************************************************************/
	printf("Buf 0x%x\n",pbuf);
	pbuf1 = osMemAlloc( badpixcount * BytePerBadPixel );
	printf("Buf1 0x%x\n",pbuf1);
	if( pbuf1 == NULL ){
		printf("Buf1 Fail\n");
		osMemFree(pbuf);
		return;
	}	
	pbuf2 = osMemAlloc( badpixcount * BytePerBadPixel );
	printf("Buf2 0x%x\n",pbuf2);
	if( pbuf2 == NULL ){
		printf("Buf2 Fail\n");
		osMemFree(pbuf1);
		osMemFree(pbuf);
		return;
	}	
	memset(pbuf1, 0xff, badpixcount * BytePerBadPixel);
	memset(pbuf2, 0xff, badpixcount * BytePerBadPixel);
	hwFrontSensorSizeGet(&width,&hight);
	badpixel_Rearrange(pbuf,pbuf1,pbuf2,width,hight);
	osMemFree(pbuf2);
	osMemFree(pbuf1);
	#endif
	
	#if 1
	nandRsvBlkWrite(badpixoffset,(badpixcount * BytePerBadPixel)/512, pbuf);
	#else
	/* other storage device access, such as NOR, EEPROM ...*/
	/*
	 * if use NOR flash, 
	 * you could refer to the routine sample_AccessFlashROM()
	 * which is inside Sample_Code.c
	 */
	#endif
	osMemFree(pbuf);
	printf("Write Rsv Data\n");
	dprint("Write Rsv Data\n");
}		


/**************************************************************************
 *                                                                        *
 *  Function Name: badPixelStillModeCel                                             *
 *                                                                        *
 *  Purposes:                        *
 *                                                                        *
 *  Descriptions: 
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 badPixelStillModeCel(
	UINT32 addr, 
	UINT32 Hsize, 
	UINT32 Vsize,
	UINT32 Offset
)
{
	UINT32 xoff,yoff;
	UINT8  xh,xl,yh,yl;
	UINT32 badpixoffset,badpixcount;	
	#ifdef BAD_2_CONTINOUS_CEL
	UINT32 bad2marker = 0;
	UINT32 rawAddress = addr;
	#endif

	sysgBadPixInfoGet(&badpixoffset,&badpixcount);

	dprint1("addr = %x\n",addr);
	hwAGprBufAddrSet(addr);/* set image starting address and size*/
	dprint2("Hsize = %d ,Vsize= %d\n",Hsize ,Vsize);
	hwAGprBufSizeSet(Hsize,Vsize);
	hwAGprBufHoffSet(Offset);
	dprint1("offset = %d\n",Offset);
	
	sysgMemBadPixCoordAddrGet(&addr);/*Get badpix SDRAM address*/
	addr = (addr << 1) | 0xac000000;
	
	while(badpixcount > 0){
		xl = READ8(addr);
		addr++;
		xh = READ8(addr);
		addr++;
		yl = READ8(addr);
		addr++;
		yh = READ8(addr);
		addr++;
				
		xoff = ((UINT32)xh << 8) + (UINT32)xl;
		yoff = ((UINT32)yh << 8) + (UINT32)yl;
		
		if((xoff == 0xffff)&&(yoff == 0xffff)){
			dprint1("xoff = %x\n",xoff);
			dprint1("yoff = %x\n",yoff);
			break;
		}	
		#ifdef BAD_2_CONTINOUS_CEL
		/* use x = 0xfefe and y = 0xfefe as marker */
		else if((xoff == 0xfefe)&&(yoff == 0xfefe)&&(bad2marker == 0)){
			printf("2 Continious bad pixel marker\n");
			bad2marker = 1;
			xl = READ8(addr);
			addr++;
			xh = READ8(addr);
			addr++;
			yl = READ8(addr);
			addr++;
			yh = READ8(addr);
			addr++;
					
			xoff = ((UINT32)xh << 8) + (UINT32)xl;
			yoff = ((UINT32)yh << 8) + (UINT32)yl;
			dprint1("xoff = %x\n",xoff);
			dprint1("yoff = %x\n",yoff);
		}
		#endif
		
		dprint1("xoff = %d\n",xoff);
		dprint1("yoff = %d\n",yoff);
				
		if((xoff < Hsize) && (yoff < Vsize)){
			#ifdef BAD_2_CONTINOUS_CEL
			if( bad2marker == 0 ){
			#endif	
				hwOnePelFixDo(xoff, yoff);
				#ifdef BNDRY_CANCEL
				badPixelBndryOneCel((rawAddress << 1) | 0xac000000, xoff, yoff, Hsize, Vsize, Offset);
				#endif
			#ifdef BAD_2_CONTINOUS_CEL
			}
			else{
				addr = badPixel2ContiniousCel(rawAddress, addr, xoff, yoff, Hsize, Vsize, Offset);
			}
			#endif
		}
		else{
			dprint1("Size over",badpixcount);
		}	
		
		badpixcount --;
		dprint1("badpixcount = %d\n",badpixcount);
	}

	return 1;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: badPixelStillEnable                                    *
 *                                                                        *
 *  Purposes: call for enable badpix compensation                         *
 *                                                                        *
 *  Descriptions: 
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void 
badPixelStillEnable(
	void
)
{
	badPixelInit();
}
/**************************************************************************
 *                                                                        *
 *  Function Name: badPixelStillDisable                                   *
 *                                                                        *
 *  Purposes: call for disable badpix compensation                        *
 *                                                                        *
 *  Descriptions: 
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void 
badPixelStillDisable(
	void
)
{
	UINT32 addr;
	dprint("\n");                 	
	sysgMemBadPixCoordAddrGet(&addr);/*Get badpix SDRAM address*/
	addr = (addr << 1) | 0xac000000;
	dprint1("badpix coordinate addr=0x%x\n",addr);		
	WRITE32(addr,0xffffffff);
}
