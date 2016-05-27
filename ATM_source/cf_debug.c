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
 *  Author:                                                               *
 *                                                                        *
 **************************************************************************/
 /*#define RS_232_DBG */
 #include <stdlib.h>
 #include <string.h>
 #include "general.h"
 #include "os_api.h"
 #include "vfs_api.h"
 #include "mem_util.h"
 #include "hw_flash.h"
 #include "sysregs.h"
 #include "stdio.h"
 #include "hw_cf.h"
 #include "cf_api.h"
 #include "hw_dma.h"
 #include "timer.h"
 #include "hw_tmr.h"
 #include "dcf_api.h"

/**************************************************************************
 *                                                                        *
 *                                                                        *
 **************************************************************************/
/* CF status register (offset 7 & eh) */
#define CF_STATUS_BUSY (1 << 7)
#define CF_STATUS_RDY  (1 << 6)
#define CF_STATUS_DWF  (1 << 5)
#define CF_STATUS_DSC  (1 << 4)
#define CF_STATUS_DRQ  (1 << 3)
#define CF_STATUS_CORR (1 << 2)
#define CF_STATUS_ERR  (1 << 0)

void debugDisplaymatrix(UINT32,UINT32,UINT32,UINT32 ,UINT8 *);
UINT32 cfIdentify(UINT8 *);
UINT32 misc_Save2File(UINT32,UINT32,UINT32,UINT8 *);
typedef struct cfDev_s {
	devInfo_t common; /* Common information for all cards */
	UINT32    IdeMode;
} cfDebugDev_t;
cfDebugDev_t cfDebugDev;

/**************************************************************************
 *                                                                        *
 *                                                                        *
 **************************************************************************/
 UINT32
 cfTest(
 	UINT8* param
 )
 {

	UINT32 ret = 0;
	flashReg_t *pflashReg;
	pflashReg = (flashReg_t *)(0xb0000000);
 	UINT8 *pBuf;
	UINT32 flashMode;
	cfDebugDev_t *ppcf;
	ppcf = &cfDebugDev;
	static	UINT8	*arg1, *arg2, *arg3;
	UINT32 vlaue, i, j, len, v2, v3;
	#if 1
	hwFlashMode(3, 0, 1);/* IDE mode */
	#else
	hwFlashMode(4, 0, 1);/* Memory mode */
	#endif
	
	arg1 = param;
	len = strlen(param);
	for(i=0; i<len; i++)
	{
		if(param[i]==' ')
		{
			param[i] = 0;
			break;
		}
	}
	do
	{
		i++;
	}while(i<len && param[i]==' ');

	arg2 = &param[i];
	for(; i<len; i++)
	{
		if(param[i]==' ')
		{
			param[i] = 0;
			break;
		}
	}
	do
	{
		i++;
	}while(i<len && param[i]==' ');
	arg3 = &param[i];
	for(; i<len; i++)
	{
		if(param[i]==' ')
		{
			param[i] = 0;
			break;
		}
	}

	v2 = strtoul(arg2, NULL, 0);
	v3 = strtoul(arg3, NULL, 0);
	
	flashMode = hwFlashModeGet();	
	cfFlashModePreSet(flashMode);
	
	pBuf = osMemAlloc(512);
	if ( pBuf == NULL ) {
		printf("No memory available for CF\n");
		return FAIL;
	}
	printf("pBuf = 0x%x\n",pBuf);

	if(strcmp(param, "cfhwinit")==0) {
		UINT32 ready;
		UINT32 retry,tmp;
		printf("cfhwinit v2 %d,v3 %d\n",v2,v3);
		retry = 0;
		while ( !hwCfRdyCheck() ) {
			printf("Poll-0\n");
		}
		retry = 0;
		ready = 0xff;
		while ( (ready & CF_STATUS_BUSY) != 0 ) {
			hwCfWordCsaReadSet(0x17, &ready); /* check if busy */
			printf("Poll-1\n");
		}
		retry = 0;
		ready = 0;
		while ( (ready & CF_STATUS_RDY) == 0 ) {
			hwCfWordCsaReadSet(0x17, &ready); /* check if busy */
			printf("Poll-2\n");
		}
	}
	if(strcmp(param, "rwspeed")==0) {
		printf("rwspeed v2 %d,v3 %d\n",v2,v3);
		misc_Save2File(v2,0xac200000,v3*1024,"TEST.BIN");
	}
	if(strcmp(param, "idlecnt")==0) {
		printf("idlecnt v2 %d,v3 %d\n",v2,v3);
	    cfCmdSend(0x16,0);      /* drive  */
	    cfCmdSend(0x12,v2);     /* cnt  */
	    cfCmdSend(0x17,0x97);   /* idle  */
	}
	if(strcmp(param, "cmdset")==0) {
		printf("cmdset v2 %d,v3 %d\n",v2,v3);
	    cfCmdSend(v2,v3);     /* Drive  */
	}
	if(strcmp(param, "cmdget")==0) {
		printf("cmdget v2 %d,v3 %d\n",v2,v3);
	    cfCmdRead(0x07,v2,pBuf);     /* set command to read */
		debugDisplaymatrix(32,16,3,16,pBuf);
	}
	if(strcmp(param, "identify")==0) {
		printf("identify v2 %d,v3 %d\n",v2,v3);
	    cfCmdRead(0x07,0xec,pBuf);     /* set command to read */
		debugDisplaymatrix(32,16,3,16,pBuf);
	}
	if(strcmp(param, "regget")==0) {
		printf("regget v2 %d,v3 %d\n",v2,v3);
		printf("07[%2x]\n",cfRegRead(0x07));/*cmd,status*/
		printf("06[%2x]\n",cfRegRead(0x06));/*Drive*/
		printf("05[%2x]\n",cfRegRead(0x05));/*Cylinder high*/
		printf("04[%2x]\n",cfRegRead(0x04));/*Cylinder low*/
		printf("03[%2x]\n",cfRegRead(0x03));/*Sector number*/
		printf("02[%2x]\n",cfRegRead(0x02));/*Sector count*/
		printf("01[%2x]\n",cfRegRead(0x01));/*error register*/
	}
	if(strcmp(param, "cfread")==0) {
		printf("cfread v2 %d,v3 %d\n",v2,v3);
		cfRead(ppcf, v2, 1, pBuf);
		debugDisplaymatrix(32,16,3,16,pBuf);
	}
	if(strcmp(param, "cfwrite")==0) {
		printf("cfwrite v2 %d,v3 %d\n",v2,v3);
		debugDisplaymatrix(32,16,3,16,pBuf);
		cfWrite(ppcf, v2, 1, pBuf);
	}

	return ret;
 }


/**************************************************************************
 *                                                                        *
 *                                                                        *
 **************************************************************************/
 void
 debugDisplaymatrix(
 	UINT32 hori,
 	UINT32 Veri,
 	UINT32 nDigit,
 	UINT32 Dec,
 	UINT8 *pBuf
 )
 {
 	UINT32 i,j;
	printf("\n");
	for(i=0;i<hori;i++){
		for(j=0;j<Veri;j++){
			if(Dec == 16){
				if(nDigit == 1)
					printf("%x ",*(pBuf + i*Veri + j));
				else if(nDigit == 2)
					printf("%2x ",*(pBuf + i*Veri + j));
				else if(nDigit == 3)
					printf("%3x ",*(pBuf + i*Veri + j));
				else if(nDigit == 4)
					printf("%4x ",*(pBuf + i*Veri + j));
				else if(nDigit == 5)
					printf("%5x ",*(pBuf + i*Veri + j));
				else
					printf("%2x ",*(pBuf + i*Veri + j));
			}
			else{
				if(nDigit == 1)
					printf("%d ",*(pBuf + i*Veri + j));
				else if(nDigit == 2)
					printf("%2d ",*(pBuf + i*Veri + j));
				else if(nDigit == 3)
					printf("%3d ",*(pBuf + i*Veri + j));
				else if(nDigit == 4)
					printf("%4d ",*(pBuf + i*Veri + j));
				else if(nDigit == 5)
					printf("%5d ",*(pBuf + i*Veri + j));
				else
					printf("%2d ",*(pBuf + i*Veri + j));
			}
		}
		printf("\n");
	}
	printf("\n");
 }

UINT32
misc_Save2File(
	UINT32  read,
	UINT32	addr,			/* address of SDRAM */
	UINT32	fileSize,		/* file size */
	UINT8	*pfilename		/* file name */
)
{
	SINT32 fd,err = 0;
	UINT32 writeSize;
	UINT32 temp;

	if(pfilename == NULL){
		return 0;
	}
	hwTmrCfg(1,1);
	hwTmrEnable(1);
	hwTmrWrite(1,0);
	temp=hwTmrRead(1);
	if(read == 1){
		#if 0
		printf("read to Dram address = %x\n",addr);
		printf("raed file %s, please wait.....\n",pfilename);
		#endif
		fd = vfsOpen(pfilename, 0x04, 0);
		writeSize = vfsRead(fd, (UINT8*)addr, fileSize);
		err = vfsClose(fd);
		if(err){
			printf("File Close error !\n");
			return err;
		}
		#if 0
		printf("Save file size = %d Bytes\n", writeSize);
		printf("Save file complete.....\n");
		#endif
	}
	else{
		#if 0
		printf("Save from Dram address = %x\n",addr);
		printf("Saving file %s, please wait.....\n",pfilename);
		#endif
		err = xdcfOneFileErase(pfilename);
		fd = vfsOpen(pfilename, 0x04, 0);
		writeSize = vfsWrite(fd, (UINT8*)addr, fileSize);
		err = vfsClose(fd);
		if(err){
			printf("File Close error !\n");
			return err;
		}
		#if 0
		printf("Save file size = %d Bytes\n", writeSize);
		printf("Save file complete.....\n");
		#endif
	}
	temp = hwTmrRead(1) - temp;
	if(read == 1)
		printf("Read size = %d kbyte/time = %d\n",writeSize/1024,temp);
	else
		printf("Write size = %d kbyte/time = %d\n",writeSize/1024,temp);
	return err;
}

