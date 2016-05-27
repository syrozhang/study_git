#include	"general.h"
#include	"panel.h"

/* GPIO Generator */
#if	SUPPORT_PANEL_TOPPOLY
void LC15004_write(UINT32 nData)
{
	UINT32	i, j, tmp;
	/* Load = digtv21 ; SCLK = digtv20 ; SDATA = digtv19 */
	WRITE8(0xb000a0a3,0x00);	/* output value of digtv[21,20,19,18,17,16] */
	WRITE8(0xb000a0a6,0x38);	/* 0x38;output enouble of digtv[21,20,19,18,17,16] */
	
	tmp = 0;
	for(i=0; i<16; i++)
	{
		if(nData & 0x8000)
			tmp |= 0x08;
		else
			tmp &= 0xF7;
		WRITE8(0xb000a0a3, tmp);	/* SDATA */

		tmp |= 0x10;
		WRITE8(0xb000a0a3, tmp);	/* SCLK */
		
		for(j=0; j<3; j++);
		tmp &= 0xEF;
		WRITE8(0xb000a0a3, tmp);	/* SCLK */

		nData = nData << 1;
	}
	tmp |= 0x20;
	WRITE8(0xb000a0a3, tmp);		/* LOAD */
}
#endif
