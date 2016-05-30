/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2004 by Worldplus Technology Co., Ltd.           *
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
 *  Author: Joe Zhao                                                      *
 *                                                                        *
 **************************************************************************/
//#define RS_232_DBG 

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "sysregs.h"
#include "irq.h"
#include "zoom.h"
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
#include "smc_api.h"
#include "hw_cdsp.h"
#include "hw_gpio.h"
#include "sysg_api.h"
#include "aaa_api.h"
#include "sysg_api.h"
#include "dpof_api.h"
#include "utility.h"
#include "badpix.h"
#include "mp3_api.h"
#include "audio_api.h"
#include "asf_api.h"
#include "vfs_api.h"
#include "pub_api.h"
#include "otg_api.h"
#include "panel.h"
#include "hw_disp_ext.h"
#include "hw_disp.h"
#include "hw_sdram.h"
#include "hw_cpu.h"
#include "misc_api.h"
#include "exif_api.h"
#include "hw_front.h"
#include "xiaoao.h"
#include "ui_main.h"
#include "card.h"
#include "dps_api.h"
#include "dps_paramdef.h"
#include "dps_custom.h"

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern xdcfAttrElm_t fileattr;
extern xdcfAttrElm_t dirattr;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 dpsPrinterConected=0;
UINT8 G_PrintingNoExit = 0;
UINT8 G_Printing = 0;

/**************************************************************************
 *                                                                        *
 *  Function Name: dpsDiscovered                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 dpsDiscovered(UINT32 status)
{
	if ( status == K_DPS_HEX_PARAVAL_PrintSrvAvail_True )
	{
		/*dpsPrtCapabilityCheck(); sun@1202 am 11:00 mask sovle the DPS issue: change the libvfs.a */
		dpsPrinterConected = 1;
		printf("DPS Printer conected!\n");

	}
	if (status == K_DPS_HEX_PARAVAL_PrintSrvAvail_False)
	{
		dpsPrinterConected = 0;
		printf("DPS Printer disconected!\n");
	}
	
	return SUCCESS;
	
}

/**************************************************************************
 *                                                                        *
 *  Function Name: dpsDeviceStatus                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 dpsDeviceStatus(DPS_PARA_DeviceStatus deviceStatus)
{
	printf("\ndpsPrintServiceStatus %x\n",deviceStatus.dpsPrintServiceStatus);
	printf("jobEndReason %x\n",deviceStatus.jobEndReason);
	printf("errorStatus %x\n",deviceStatus.errorStatus);
	printf("errorReason %x\n",deviceStatus.errorReason);
	printf("disconnectEnable %x\n",deviceStatus.disconnectEnable);
	printf("capabilityChanged %x\n",deviceStatus.capabilityChanged);
	printf("newJobOK %x\n\n",deviceStatus.newJobOK);

	if((deviceStatus.dpsPrintServiceStatus == K_DPS_HEX_PARAVAL_PrintSrvStatus_Printing)&&(deviceStatus.errorStatus == K_DPS_HEX_PARAVAL_ErrorStatus_NoError))
	{
		if (G_Printing)
		{
			G_PrintingNoExit = 1;
		}		
		printf("Printing\n");
	}
	else if(deviceStatus.errorStatus != K_DPS_HEX_PARAVAL_ErrorStatus_NoError)
	{
		G_PrintingNoExit = 0;
		G_Printing = 0;
		printf("Print Error\n");		
	}
	else if(deviceStatus.dpsPrintServiceStatus != K_DPS_HEX_PARAVAL_PrintSrvStatus_Printing)
	{
		G_PrintingNoExit = 0;
		G_Printing = 0;
		printf("print End\n");
	}
	return SUCCESS;

}

/**************************************************************************
 *                                                                        *
 *  Function Name: dpsJobStatus                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 dpsJobStatus(DPS_PARA_JobStatus jobStatus)
{
	printf("\nprtPID = %s\n",jobStatus.prtPID);
	printf("filePath = %s\n",jobStatus.filePath);
	printf("copyID = %s\n",jobStatus.copyID);
	printf("progress = %s\n",jobStatus.progress);
	printf("imagesPrinted = %s\n\n",jobStatus.imagesPrinted);
	return SUCCESS;

}

#if 0
/**************************************************************************
 *                                                                        *
 *  Function Name: dpsPrtCapabilityCheck                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void dpsPrtCapabilityCheck()
{
	UINT32 *pCapabilityArray;
	UINT32 capabilitySize;
	UINT32 ret;
	UINT32 i;

	/* qualities */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_qualities, &pCapabilityArray, &capabilitySize, 0);
	if (ret == SUCCESS)
	{
		printf("Printer supported qualities[%d] = \n", capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}

	/* paperSizes */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_paperSizes, &pCapabilityArray, &capabilitySize, 0);
	if (ret == SUCCESS)
	{
		printf("Printer supported paperSizes[%d] = \n", capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}

	/* paperTypes */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_paperTypes, &pCapabilityArray, &capabilitySize, 0);
	if (ret == SUCCESS)
	{
		printf("Printer supported paperTypes[%d] = \n", capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}

	/* fileTypes */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_fileTypes, &pCapabilityArray, &capabilitySize, 0);
	if (ret == SUCCESS)
	{
		printf("Printer supported fileTypes[%d] = \n", capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}

	/* datePrints */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_datePrints, &pCapabilityArray, &capabilitySize, 0);
	if (ret == SUCCESS)
	{
		printf("Printer supported datePrints[%d] = \n", capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}

	/* fileNamePrints */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_fileNamePrints, &pCapabilityArray, &capabilitySize, 0);
	if (ret == SUCCESS)
	{
		printf("Printer supported fileNamePrints[%d] = \n", capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}

	/* imageOptimize */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_imageOptimizes, &pCapabilityArray, &capabilitySize, 0);
	if (ret == SUCCESS)
	{
		printf("Printer supported imageOptimize[%d] = \n", capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}

	/* layouts */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_layouts, &pCapabilityArray, &capabilitySize, K_DPS_HEX_PARAVAL_PaperSize_Default);
	if (ret == SUCCESS)
	{
		printf("Printer supported layouts[%d] = \n",capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}

	/* fixedSizes */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_fixedSizes, &pCapabilityArray, &capabilitySize, 0);

	if (ret == SUCCESS)
	{
		printf("Printer supported fixedSizes[%d] = \n",capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}

	/* croppings */
	ret = dpsPrtCapabilityGet(K_DPS_CAPABILITY_TYPE_croppings, &pCapabilityArray, &capabilitySize, 0);
	if (ret == SUCCESS)
	{
		printf("Printer supported croppings[%d] = \n", capabilitySize);
		for (i = 0;i < capabilitySize;i++)
		{
			printf("    %x\n", pCapabilityArray[i]);
		}
	}
	else
	{
		printf("dpsPrtCapabilityGet() FAIL with error code [%x]!!!\n", ret);
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: dpsSinglePrint                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *      Example of the PictBridge Single image print                      *
 *                                                                        *
 *  Arguments:                                                            *
 *      filePath : full path of the image to be printed                   *
 *                 Ex: "C:\\DCIM\\100MEDIA\\SUNP0001.JPG"                 *
 *      flag     : addictional print option of the job config             *
 *                 PRT_FLAG_DATE                                          *
 *                 PRT_FLAG_FILENAME                                      *
 *                 PRT_FLAG_IMGOPT                                        *
 *      type     : addictional job config type to set                     *
 *                                                                        *
 *      value    : addictional job config type's value                    *
 *                                                                        *
 *  Returns:                                                              *
 *      no return                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void dpsSinglePrint(UINT8 *filePath, UINT32 flag, UINT32 type, UINT32 value)
{
	UINT32 jobCfgHnd;
	UINT32 prtInfoHnd;
	UINT32 fileID;
	UINT32 ret;

	printf("dspPrtJobNew()...\n");
	ret = dspPrtJobNew(&jobCfgHnd);
	if (ret != SUCCESS)
	{
		printf("dspPrtJobNew() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* job config data */
	printf("dpsJobCfgDataSet()...\n");
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_quality, K_DPS_HEX_PARAVAL_Quality_Default);
	if (ret != SUCCESS)
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
		return;
	}
	printf("dpsJobCfgDataSet()...\n");
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_paperType, K_DPS_HEX_PARAVAL_PaperType_Default);
	if (ret != SUCCESS)
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* FLAGS */
	if ((flag & PRT_FLAG_DATE) == PRT_FLAG_DATE)
	{
		printf("dpsJobCfgDataSet()... date print\n");
		ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_datePrint, K_DPS_HEX_PARAVAL_DatePrint_On);
		if (ret != SUCCESS)
		{
			printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
			return;
		}
	}

	if ((flag & PRT_FLAG_FILENAME) == PRT_FLAG_FILENAME)
	{
		printf("dpsJobCfgDataSet()... file name print\n");
		ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_fileNamePrint, K_DPS_HEX_PARAVAL_FileNamePrint_On);
		if (ret != SUCCESS)
		{
			printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
			return;
		}
	}

	if ((flag & PRT_FLAG_IMGOPT) == PRT_FLAG_IMGOPT)
	{
		printf("dpsJobCfgDataSet()... image opt print\n");
		ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_imageOptimize, K_DPS_HEX_PARAVAL_ImageOptimize_On);
		if (ret != SUCCESS)
		{
			printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
			return;
		}
	}

	/* TYPES */
	if (type != 0)
	{
		printf("dpsJobCfgDataSet()... type = %08x, value = %08x\n", type, value);
		ret = dpsJobCfgDataSet(jobCfgHnd, type, value);
		if (ret != SUCCESS)
		{
			printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
			return;
		}
	}
/* print info data */
	printf("dpsPrtInfoHndGet()...\n");
	ret = dpsPrtInfoHndGet(jobCfgHnd,&prtInfoHnd);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoHndGet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	printf("dpsPrtInfoData_filePath2fileIDGet()...\n");
	ret = dpsPrtInfoData_filePath2fileIDGet(filePath, &fileID);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_filePath2fileIDGet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	printf("dpsPrtInfoData_fileIDSet()...\n");
	ret = dpsPrtInfoData_fileIDSet(prtInfoHnd,fileID);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_fileIDSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	printf("dpsPrtInfoData_fileNameSet()...\n");
	ret = dpsPrtInfoData_fileNameSet(prtInfoHnd,"Test001");
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_fileNameSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	printf("dpsPrtInfoData_dateSet()...\n");
	struct tm* ptime;
	timeGet(&ptime);
	ret = dpsPrtInfoData_dateSet(prtInfoHnd,ptime->tm_year+1900,ptime->tm_mon,ptime->tm_mday,K_DPS_DATE_TYPE1);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_dateSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* print Job Cfg */
	dpsDbg_JobListPrint(jobCfgHnd);

	/* execute this job */
	printf("dspPrtJobExec()...\n");
	ret = dspPrtJobExec();
	if (ret != K_DPS_HEX_RESULT_Ok)
	{
		printf("dspPrtJobExec() FAIL, ret = 0x%x\n", ret);
		return;
	}
	printf("dspPrtJobExec() return.....................\n");
}

/**************************************************************************
 *                                                                        *
 *  Function Name: dpsMultiPrint                                          *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *      Example of the PictBridge Multiple image print                    *
 *                                                                        *
 *  Arguments:                                                            *
 *      filePathArray : array of full path of the image to be printed     *
 *      bufferLen     : full path's string buffer length of first argument*
 *                      Ex: UINT8 filePathArray[100][4];                  *
 *                          then bufferLen = 100                          *
 *      cnt           : file path count of first argument                 *
 *                      Ex: UINT8 filePathArray[100][4];                  *
 *                          then cnt = 4                                  *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void dpsMultiPrint(UINT8* filePathArray, UINT32 bufferLen, UINT32 cnt, UINT32 indexed)
{
	UINT32 jobCfgHnd;
	UINT32 prtInfoHnd;
	UINT32 fileID;
	UINT32 ret;
	UINT32 i;

	printf("dspPrtJobNew()...\n");
	ret = dspPrtJobNew(&jobCfgHnd);
	if (ret != SUCCESS)
	{
		printf("dspPrtJobNew() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* job config data */
	printf("dpsJobCfgDataSet()...\n");
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_quality, K_DPS_HEX_PARAVAL_Quality_Default);
	if (ret != SUCCESS)
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
		return;
	}
	printf("dpsJobCfgDataSet()...\n");
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_paperType, K_DPS_HEX_PARAVAL_PaperType_Default);
	if (ret != SUCCESS)
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	if (indexed == TRUE)
	{
		printf("dpsJobCfgDataSet()...\n");
		ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_layout, K_DPS_HEX_PARAVAL_Layout_IndexPrint);
		if (ret != SUCCESS)
		{
			printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
			return;
		}
	}

	for (i = 0;i < cnt;i++)
	{
		/* print info data */
		printf("dpsPrtInfoHndGet()...\n");
		ret = dpsPrtInfoHndGet(jobCfgHnd,&prtInfoHnd);
		if (ret != SUCCESS)
		{
			printf("dpsPrtInfoHndGet() FAIL, ret = 0x%x\n", ret);
			return;
		}

		printf("dpsPrtInfoData_filePath2fileIDGet()...\n");
		ret = dpsPrtInfoData_filePath2fileIDGet((UINT8 *)(filePathArray + (i * bufferLen)),&fileID);
		if (ret != SUCCESS)
		{
			printf("dpsPrtInfoData_filePath2fileIDGet() FAIL, ret = 0x%x\n", ret);
			return;
		}

		printf("dpsPrtInfoData_fileIDSet()...\n");
		ret = dpsPrtInfoData_fileIDSet(prtInfoHnd,fileID);

		if (ret != SUCCESS)
		{
			printf("dpsPrtInfoData_fileIDSet() FAIL, ret = 0x%x\n", ret);
			return;
		}

		printf("dpsPrtInfoData_fileNameSet()...\n");
		ret = dpsPrtInfoData_fileNameSet(prtInfoHnd,"Test001");
		if (ret != SUCCESS)
		{
			printf("dpsPrtInfoData_fileNameSet() FAIL, ret = 0x%x\n", ret);
			return;
		}

		printf("dpsPrtInfoData_dateSet()...\n");
		ret = dpsPrtInfoData_dateSet(prtInfoHnd,2003,8,7,K_DPS_DATE_TYPE1);
		if (ret != SUCCESS)
		{
			printf("dpsPrtInfoData_dateSet() FAIL, ret = 0x%x\n", ret);
			return;
		}

		if (i == 0)
		{
			printf("dpsPrtInfoData_dateSet()...\n");
			ret = dpsPrtInfoData_copiesSet(prtInfoHnd,2);
			if (ret != SUCCESS)
			{
				printf("dpsPrtInfoData_dateSet() FAIL, ret = 0x%x\n", ret);
				return;
			}
		}
	}

	/* print Job Cfg */
	dpsDbg_JobListPrint(jobCfgHnd);

	/* execute this job */
	printf("dspPrtJobExec()...\n");
	ret = dspPrtJobExec();
	if (ret != K_DPS_HEX_RESULT_Ok)
	{
		printf("dspPrtJobExec() FAIL, ret = 0x%x\n", ret);
		return;
	}
	printf("dspPrtJobExec() return.....................\n");
}

/**************************************************************************
 *                                                                        *
 *  Function Name: dpsDPOFPrint                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *      filePath : full path of the DPOF auto print file to be printed    *
 *                 Ex: "C:\\MISC\\AUTPRINT.MRK"                           *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void dpsDPOFPrint(UINT8 *filePath)
{
	UINT32 jobCfgHnd;
	UINT32 prtInfoHnd;
	UINT32 fileID;
	UINT32 ret;

	printf("dspPrtJobNew()...\n");
	ret = dspPrtJobNew(&jobCfgHnd);
	if (ret != SUCCESS)
	{
		printf("dspPrtJobNew() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* job config data */
	printf("dpsJobCfgDataSet()...\n");
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_fileType, K_DPS_HEX_PARAVAL_FileType_Dpof);
	if (ret != SUCCESS)
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* print info data */
	printf("dpsPrtInfoHndGet()...\n");
	ret = dpsPrtInfoHndGet(jobCfgHnd, &prtInfoHnd);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoHndGet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	printf("dpsPrtInfoData_filePath2fileIDGet()...\n");
	ret = dpsPrtInfoData_filePath2fileIDGet(filePath, &fileID);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_filePath2fileIDGet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	printf("dpsPrtInfoData_fileIDSet()...\n");
	ret = dpsPrtInfoData_fileIDSet(prtInfoHnd, fileID);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_fileIDSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* print Job Cfg */
	dpsDbg_JobListPrint(jobCfgHnd);

	/* execute this job */
	printf("dspPrtJobExec()...\n");
	ret = dspPrtJobExec();
	if (ret != K_DPS_HEX_RESULT_Ok)
	{
		printf("dspPrtJobExec() FAIL, ret = 0x%x\n", ret);
		return;
	}
	printf("dspPrtJobExec() return.....................\n");
}

/**************************************************************************
 *                                                                        *
 *  Function Name: dpsCroppingPrint                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *      filePath : full path of the image to be printed                   *
 *                 Ex: "C:\\DCIM\\100MEDIA\\SUNP0001.JPG"                 *
 *      x        : cropping area offset x                                 *
 *      y        : cropping area offset y                                 *
 *      w        : cropping area size width                               *
 *      h        : cropping area size height                              *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void dpsCroppingPrint(UINT8 *filePath, UINT32 x, UINT32 y, UINT32 w, UINT32 h)
{
	UINT32 jobCfgHnd;
	UINT32 prtInfoHnd;
	UINT32 fileID;
	UINT32 ret;

	printf("dspPrtJobNew()...\n");
	ret = dspPrtJobNew(&jobCfgHnd);
	if (ret != SUCCESS)
	{
		printf("dspPrtJobNew() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* job config data */
	printf("dpsJobCfgDataSet()...\n");
	ret = dpsJobCfgDataSet(jobCfgHnd,K_DPS_JOBCONFIG_TYPE_cropping, K_DPS_HEX_PARAVAL_Cropping_On);
	if (ret != SUCCESS)
	{
		printf("dpsJobCfgDataSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* print info data */
	printf("dpsPrtInfoHndGet()...\n");
	ret = dpsPrtInfoHndGet(jobCfgHnd, &prtInfoHnd);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoHndGet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	printf("dpsPrtInfoData_filePath2fileIDGet()...\n");
	ret = dpsPrtInfoData_filePath2fileIDGet(filePath, &fileID);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_filePath2fileIDGet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	printf("dpsPrtInfoData_fileIDSet()...\n");
	ret = dpsPrtInfoData_fileIDSet(prtInfoHnd,fileID);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_fileIDSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	printf("dpsPrtInfoData_croppingAreaSet()...\n");
	ret = dpsPrtInfoData_croppingAreaSet(prtInfoHnd, x, y, w, h);
	if (ret != SUCCESS)
	{
		printf("dpsPrtInfoData_croppingAreaSet() FAIL, ret = 0x%x\n", ret);
		return;
	}

	/* print Job Cfg */
	dpsDbg_JobListPrint(jobCfgHnd);

	/* execute this job */
	printf("dspPrtJobExec()...\n");
	ret = dspPrtJobExec();
	if (ret != K_DPS_HEX_RESULT_Ok)
	{
		printf("dspPrtJobExec() FAIL, ret = 0x%x\n", ret);
		return;
	}
	printf("dspPrtJobExec() return.....................\n");
}

//wendy@050204 begin
void Dpsprint(void)
{
	UINT8 filepath[100];
	PDPS_PARA_JobStatus pJobStatus;
	DPS_PARA_JobStatus JobStatus;
	pJobStatus=&JobStatus;

	strcpy(filepath,"C:\\DCIM\\100MEDIA\\");
	xdcfCurFileAttrGet(&fileattr);
	strcat(filepath,fileattr.name);
	dpsSinglePrint(filepath, PRT_FLAG_IMGOPT, K_DPS_JOBCONFIG_TYPE_imageOptimize, 
	K_DPS_HEX_PARAVAL_ImageOptimize_Default);


}
//wendy@050204 end
#endif


