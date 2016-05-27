//#define RS_232_DBG 
#include <stdlib.h>
#include "general.h"
#include "hw_usb.h"
#include "stdio.h"
#include "usb_api.h"
#include "dsc_init.h"

#include "snap_api.h"/*badpix*/
#include "hw_front.h"/*badpix*/
#include "hw_cdsp.h"/*badpix*/
#include "sysg_api.h"/*badpix*/
#include "badpix.h"/*badpix*/


UINT32 lastBulkOutAddr;

void
appDoL1Function_PC(
	UINT8 funcID,
	UINT32 *argBuf
)
{

	switch(funcID) {
#if 0		
		case 0x00:
			L1_UploadFAT((UINT8)argBuf[0]);			
			break;
			
		case 0x01:
			L1_UploadFile((UINT8)argBuf[0]);			
			break;
			
		case 0x02:
			L1_UploadAll((UINT8)argBuf[0]);			
			break;			
#endif			

		case 0xf1: 
			/* ISP function */
			printf("argBuf[0]=0x%x\n",argBuf[0]);
			printf("argBuf[1]=0x%x\n",argBuf[1]);
			printf("argBuf[2]=0x%x\n",argBuf[2]);
			printf("argBuf[3]=0x%x\n",argBuf[3]);
			usbIsp(argBuf[0],argBuf[1],argBuf[2],argBuf[3],lastBulkOutAddr);
			break;	
			
		case 0xf2:
			hwUsbToPcBulkin(argBuf[0], argBuf[1]);
			break;

		case 0xf3:
			printf("argBuf[0]=0x%x\n",argBuf[0]);
			printf("argBuf[1]=0x%x\n",argBuf[1]);
			printf("argBuf[2]=0x%x\n",argBuf[2]);
			printf("argBuf[3]=0x%x\n",argBuf[3]);
			hwPcToUsbBulkout(argBuf[0], argBuf[1]);
			lastBulkOutAddr = argBuf[0];
			break;
			
#if 0			
		case 0xf4:
			L1_UploadFromFlash(argBuf[0], argBuf[1]);
			break;
			
		case 0xf5:
			L1_DownloadToFlash(argBuf[0], argBuf[1]);			
			break;
#endif

		case 0xf6:/*badpix*/
			badPixelWriteToRsvBlk(argBuf);                        
			break;

		default:
			break;
	}

}


void/*badpix*/
App_DoL1Function_Preview(
	UINT8 funcID,
	UINT32 *argBuf
)
{
	switch(funcID) {
		case 0x0a:
			snapRGBRawImgCapture(1);
			dprint("Snap Raw Data\n");
			break;	
#if 0
		case 0x0b:
			snapMemRawBufferGet(&Rawaddr);
			sysgSensorSizeGet(&Hsize,&Vsize);
			badPixelStillModeCel(Rawaddr,Hsize,Vsize,Hsize*3/8);			
			break;			
#endif			
		
		default:
			break;
	}

}


void/*badpix*/
App_DoL2Function_Front(
	UINT8 funcID,
	UINT32 *argBuf
)
{
	switch(funcID) {
		case 0x32: 
			hwFrontExpTimeSet(200,1);/* need to be adjust according to request*/
			dprint("Set Shutter\n");
			break;	
			
		case 0x33:
			hwFrontGainSet(0x3f,0);/* need to be adjust according to request*/
			dprint("Set Gain\n");
			break;			
		default:
			break;
	}

}

void
appDoHwFunc(
	UINT32 usbRequestLock,
	UINT32 usbL1FuncID,
	UINT32 usbL2FuncID,
	UINT32 usbL1ModuleID,
	UINT32 usbL2ModuleID,
	UINT32 *usbArgBuf
)
{
	
	switch (usbRequestLock) {
		case 0xfd:
			/*-------------------------------------------
			// L1_Function
			//-------------------------------------------*/
			printf("*********** L1(%x,%x) ***********\n",usbL1FuncID,usbL1ModuleID);
			switch(usbL1ModuleID) {
#if 0			
				case 0x00:
					App_DoL1Function_System(G_L1_FuncID, G_ArgBuf);                                                  
					break;
#endif					
				case 0x01:/*badpix*/
					App_DoL1Function_Preview(usbL1FuncID, usbArgBuf);                                                         
					break;
#if 0										
				case 0x02:
					App_DoL1Function_Playback(G_L1_FuncID, G_ArgBuf);                                                        
					break;
				case 0x03:
					App_DoL1Function_Audio(G_L1_FuncID, G_ArgBuf);                                                   
					break;
#endif

				case 0x04:					
					appDoL1Function_PC(usbL1FuncID, usbArgBuf);
					break;
					
#if 0
				case 0x05:
					App_DoL1Function_OSD(G_L1_FuncID, G_ArgBuf);                                                     
					break;
					
				case 0x06:
					App_DoL1Function_FileSystem(G_L1_FuncID, G_ArgBuf);                                                      
					break;
					
				case 0x07:
					App_DoL1Function_Storage(G_L1_FuncID, G_ArgBuf);                                                      
					break;
					
				case 0x0f:
					App_DoL1Function_Test(G_L1_FuncID, G_ArgBuf);                                                    
					break;
#endif

				default:
					break;
			}
			break;

		case 0xfe:
			/*-------------------------------------------
			// L2_Function
			//-------------------------------------------*/
			printf("*********** L2(%x,%x) ***********\n",usbL2FuncID,usbL2ModuleID);
			switch(usbL2ModuleID) {
#if 0
				case 0x00:
					App_DoL2Function_Global(G_L2_FuncID, G_ArgBuf);                                                  
					break;
					
#endif 										
				case 0x01:/*badpix*/
					App_DoL2Function_Front(usbL2FuncID, usbArgBuf);                                                   
					break;
					
#if 0					
					
				case 0x02:
					App_DoL2Function_CDSP(G_L2_FuncID, G_ArgBuf);                                                    
					break;
					
				case 0x03:
					App_DoL2Function_SDRAM(G_L2_FuncID, G_ArgBuf);                                                   
					break;
					
				case 0x04:
					App_DoL2Function_DMAC(G_L2_FuncID, G_ArgBuf);                                                    
					break;
					
				case 0x05:
					App_DoL2Function_CPU(G_L2_FuncID, G_ArgBuf);                                                     
					break;
					
				case 0x06:
					App_DoL2Function_FM(G_L2_FuncID, G_ArgBuf);                                                      
					break;
					
				case 0x07:
					App_DoL2Function_Audio(G_L2_FuncID, G_ArgBuf);                                                   
					break;
					
				case 0x08:
					App_DoL2Function_LCDTV(G_L2_FuncID, G_ArgBuf);                                                   
					break;
					
				case 0x09:
					App_DoL2Function_USB(G_L2_FuncID, G_ArgBuf);                                                     
					break;
					
				case 0x0f:
					App_DoL2Function_Misc(G_L2_FuncID, G_ArgBuf);                                                    
					break;
#endif
					
				default:
					break;
			}
			break;
		case 0x31:
			pcCamSet(usbArgBuf[0],usbArgBuf[1],1);
/*			hwCdspBadPixelSet(1,0x20,0x20,0x20);*//*badpix*/
			break;
	}
	
}

