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
#include "general.h"
#include "asf_api.h"
#include "dcf_api.h"
#include "snap_api.h"
#include "pb_api.h"
#include "card.h"
#include "os_api.h"
#include "hw_cammode.h"
#include "hw_cdsp.h"
#include "hw_disp.h"
#include "hw_front.h"
#include "hw_sdram.h"
#include "dsc_init.h"
#include "stdio.h"
#include "vfs_api.h"
#include "usb_api.h"
#include "sysg_api.h"
#include "audio_api.h"
#include "hw_disp_ext.h"
#include "osd_common.h"
#include "osd_main.h"
#include "osd_api.h"
#include "panel.h"
#include "exif_custom.h"
#include "snap_custom.h"
#include "zoom.h"

#include "font16x32.h"
//#include "font12x26.h"
#include "font16x16.h"
#include "xiaoao.h"
#include "mp3_scr.h"


#define	OSD_DEBUG
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void	panelSelect(UINT32);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                                                                        *
 *  Function Name: osdCustomInit                                          *
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
void	osdCustomInit(void)
{
#if	MS_READY
	msInit();
	return;
#endif
	#if (SUPPORT_HANZI == 1)
	osdFontLoad();
	#endif
	#if (SUPPORT_FANTI== 1)
	osdFontLoad();
	#endif
	#if	1
	osdClearScreen(0x00);
	hwDispOsdEn(1);
	hwDispRegUpd(0x01);
	#endif
	printf("\nOSD INIT OK\n");
}

