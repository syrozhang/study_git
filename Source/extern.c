//****************************************************************************
UINT32				fileType;
UINT32				fileCnt;
UINT32				dirCnt;
UINT32				fileIdx;
UINT32				dirIdx;

UINT32				uiState = UI_KEY_MODE_MAINMENU;
UINT32              ui32NextState; /* Paul@2006/02/18-15:05 add */

UINT32				uiMenu;  
UINT32				isMenuOn = 0;
OS_EVENT			*uiKeyMsgQ;

//****************************************************************************
/* Paul@2006/04/22 add for verify ID */
/* 
	DDX = 0x0001 
*/
UINT8				ui8CustomID[2] = {0x01, 0x00};
/* 
	DC268 = 0x01
	DC269 = 0x02
	DC270 = 0x03
	DC318 = 0x04
*/
UINT8				ui8ProductID = 0x01;


//****************************************************************************

//****************************************************************************

