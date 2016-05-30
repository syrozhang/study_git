extern  void  uart1fputchar(UINT8);
extern  int  uart1fgetc(void);

extern  void  myHwWait1(UINT32 time);

void  getModuleName(UINT8 model, UINT8* name);
void  readPara();
void  writePara();

#define  C_WORD_LENGHT  4096

#define  C_READ_SECTION  30*1024

#define  GPIO_CARD_DET  (GPIO_TYPE_GE + 3)

UINT8  ucHMIAddress = 1;
UINT8  ucHMI_RWflag;
UINT8  ucHMI_RWmode;
UINT8  ucHMI_len;
UINT8  ucHMI_bcc;
UINT8  ucHMIBit[1024];
UINT8  ucUart1RxBuf[1024];
UINT8*  ucHMIptr;

UINT16  ucHMI_addr;
UINT16  uiUart1RxPtr;
UINT16  uiHMIWord[C_WORD_LENGHT];
UINT16  test_step0,test_step1,test_mode=0;

static  UINT8  ucChar[]="0123456789ABCDEF";

void  myUpper(UINT8* filename)
{
	UINT8  ch;
	
	UINT16  i;
	
	for(i=0;i<strlen(filename);i++)
	{
		ch = filename[i];
		if((ch>='a')&&(ch<='z'))
			filename[i] = ch+32;
	}
}

void  myReadFilename(UINT8* dstname, UINT8* scrame)
{
	UINT8  *longname;
	
	longname = vfsLongFileNameGet(scrame);
	if(strlen(longname)==0)
		strcpy(dstname, scrame);
	else
		strcpy(dstname, longname);
	myUpper(dstname);
}

UINT32  myFindFile(UINT8* name)
{
	UINT8  filename[20];
	
	UINT16  len;
	
	UINT32  i;
	
	UINT32  ui32FileCnt;
	UINT32  err = SUCCESS;
	
	xdcfAttrElm_t  xdcfFileAttr1;
	
	len = strlen(name);
	xdcfFileCountGet(&ui32FileCnt);
	#if  C_FILE_DEBUG
	    printf("file counter=%d  ",ui32FileCnt);
	#endif
	if(ui32FileCnt==0)
	{
		#if  C_FILE_DEBUG
		    printf("not find file  ");
		#endif
		return  NULL;
	}
	for(i=0;i<ui32FileCnt;i++)
	{
		xdcfCurFileByPosSet(i+1);
		err = xdcfCurFileAttrGet(&xdcfFileAttr1);
		myReadFilename(filename, xdcfFileAttr1.name);
		#if  C_FILE_DEBUG
		    printf("file=%d=%s  ",i+1,filename);
		#endif
		if(strncmp(filename, name, len)==0)
			break;
	}
	#if  C_FILE_DEBUG
	    printf("find file=%d=%s  ",i+1,name);
	#endif
	if(i<ui32FileCnt)
		return  i+1;
	else
		return  NULL;
}

void  myGetSingleDayDL(UINT32 pos, UINT32 *ui32Data)
{
	UINT8  *pStr;
	UINT8  *ui32Addr[32];
	
	UINT32  j,k,len;
	UINT32  err = SUCCESS;
	UINT32  filesize;
	
	SINT32  i32DLfd;
	
	xdcfAttrElm_t  xdcfFileAttr1;
	
	ui32Data[0] = 0;
	if((pos==0)||(ui8TimeFlag==0))
		return;
	xdcfCurFileByPosSet(pos);
	err = xdcfCurFileAttrGet(&xdcfFileAttr1);
	
	i32DLfd = vfsOpen(xdcfFileAttr1.name, O_RDWR, 0);
	if(!i32DLfd)
	{
		#if  C_FILE_DEBUG
		    printf("open %s error  ",xdcfFileAttr1.name);
		#endif
		return;
	}
	else
	{
		filesize = xdcfFileSizeGet(i32DLfd);
		j = strlen(ui8DLHeader1)+7+sizeof(ui8DLHeader2);
		for(k=0;k<C_DL_HEADER_3_LEN;k++)
		{
			j += strlen(ui8DLHeader3[k]);
		}
		j += 2;
		#if  C_FILE_DEBUG
		    printf("file header=%d  ",j);
		#endif
		if(filesize<=j)
			return;
		len = filesize-j;
		pStr = osMemAlloc(len);
		if(!pStr)
		{
			#if  C_FILE_DEBUG
			    printf("alloc memory fail=%d  ",len+1);
			#endif
			return;
		}
		vfsLseek(i32DLfd, j, SEEK_SET);
		vfsRead(i32DLfd, pStr, len);
		pStr[len] = '\0';
		
		k = 0;
		while(k<len)
		{
			for(j=1;(pStr[k]!='\n')&&(pStr[k]!='\0');k++)
			{
				/*if(pStr[k]==',')
				{
					pStr[k] = '\0';
					ui32Addr[j++] = &pStr[k+1];
				}*/
			}
			if(pStr[k]=='\0')
				return;
			k++;
			ui32Data[0]++;
		}
		vfsClose(i32DLfd);
		osMemFree(pStr);
	}
}

UINT32  myXdcfDelFile()
{
	UINT8  filename[20];
	
	UINT32  err = SUCCESS;
	UINT32  ui32FileIdx;
	UINT32  ui32FileCnt;
	
	xdcfAttrElm_t  xdcfFileAttr;
	
	xdcfFileCountGet(&ui32FileCnt);
	if(!ui32FileCnt)
		goto  delFileOver;
	
	ui32FileIdx = 1;
	for(;ui32FileIdx<=ui32FileCnt;ui32FileIdx++)
	{
		xdcfCurFileByPosSet(ui32FileIdx);
		xdcfCurFileAttrGet(&xdcfFileAttr);
		myReadFilename(filename, xdcfFileAttr.name);
		#if  C_FILE_DEBUG
		    printf("delete %s ok  ",filename);
		#endif
		err |= xdcfOneFileErase(xdcfFileAttr.name);
	}
delFileOver:
	return  err;
}

UINT32  myXdcfCopyDisk(UINT32 ui32dstDirPos)
{
	UINT8  filename[20];
	
	UINT32  temp,temp1;
	UINT32  err = SUCCESS;
	UINT32  ui32FileIdx;
	UINT32  ui32FileCnt;
	UINT32  ui32DirCnt;
	UINT32  ui32DirIdx;
	UINT32  ui32DirPos;
	UINT32  ui32DirPos2;
	UINT32  ui32FileSize;
	UINT32  *pui32SavePage = NULL;
	
	SINT32  fd,fd2;
	
	xdcfAttrElm_t  xdcfFileAttr;
	xdcfAttrElm_t  xdcfDirAttr;
	
	xdcfCurDirPosGet(&ui32DirPos);
	xdcfFileCountGet(&ui32FileCnt);
	#if  C_FILE_DEBUG
	    printf("fine counter=%d,%d  ",ui32DirPos,ui32FileCnt);
	#endif
	if(!ui32FileCnt)
		goto  copyFileOver;
	pui32SavePage = osMemAlloc(C_READ_SECTION);
	if(!pui32SavePage)
	{
		#if  C_FILE_DEBUG
		    printf("alloc 100k fail  ");
		#endif
		goto  copyFileOver;
	}
	
	ui32FileIdx = 1;
	for(;ui32FileIdx<=ui32FileCnt;ui32FileIdx++)
	{
		xdcfActiveDevIdSet(DRIVE_NAND);
		xdcfCurDirByPosSet(ui32DirPos);
		xdcfCurFileByPosSet(ui32FileIdx);
		xdcfCurFileAttrGet(&xdcfFileAttr);
		myReadFilename(filename, xdcfFileAttr.name);
		fd = vfsOpen(xdcfFileAttr.name, O_BINARY, S_IREAD);
		ui32FileSize = vfsFileSizeGet(fd);
		xdcfActiveDevIdSet(DRIVE_SD);
		xdcfCurDirByPosSet(ui32dstDirPos);
		#if  C_FILE_DEBUG
		    printf("file %s=%d  ",filename,ui32FileSize);
		#endif
		
		fd2 = vfsOpen(filename, O_CREATE, 0);
		while(ui32FileSize)
		{
			if(ui32FileSize>=C_READ_SECTION)
				temp = C_READ_SECTION;
			else
				temp = ui32FileSize;
			ui32FileSize -= temp;
			xdcfActiveDevIdSet(DRIVE_NAND);
			vfsRead(fd, pui32SavePage, temp);
			xdcfActiveDevIdSet(DRIVE_SD);
			temp1 = vfsWrite(fd2, pui32SavePage, temp);
			if(temp!=temp1)
				err |= FAIL;
		}
		vfsClose(fd);
		vfsClose(fd2);
	}
copyFileOver:
	#if  0
	    #if  C_FILE_DEBUG
	        printf("dircnt=%d  ",ui32DirCnt);
	    #endif
	    if(!ui32DirCnt)
	    	goto  copyDirOver;
	    ui32DirIdx = 1;
	    xdcfActiveDevIdSet(DRIVE_NAND);
	    err |= xdcfCurDirByPosSet(ui32DirPos);
	    vfsCurrDirReset();
	    #if  C_FILE_DEBUG
	        printf("dir0=%s   ",vfsGetCurrDirName());
	    #endif
	    xdcfDirCountGet(&ui32DirCnt);
	    xdcfCurDirAttrGet(&xdcfDirAttr);
	    #if  C_FILE_DEBUG
	        printf("dir=%s  ",xdcfDirAttr.name);
	    #endif
	    while((err|=xdcfCurDirByDirectSet(xDCF_MOVE_NEXT))==SUCCESS)
	    {
	    	xdcfCurDirAttrGet(&xdcfDirAttr);
	    	#if  C_FILE_DEBUG
	    	    printf("dir=%s  ",xdcfDirAttr.name);
	    	#endif
	    }
	    #if  0
	        for(;ui32DirIdx<=ui32DirCnt;ui32DirIdx++)
	        {
	        	xdcfActiveDevIdSet(DRIVE_SD);
	        	err |= xdcfCurDirByPosSet(ui32dstDirPos);
	        	vfsMkdir(xdcfDirAttr.name);
	        	vfsChdir(xdcfDirAttr.name);
	        	xdcfCurDirPosGet(&ui32DirPos2);
	        	
	        	xdcfActiveDevIdSet(DRIVE_NAND);
	        	err |= xdcfCurDirByPosSet(ui32DirPos);
	        	
	        	myXdcfCopyDisk(ui32DirPos2);
	        }
	    #endif
copyDirOver:
	#endif
	if(pui32SavePage)
	{
		osMemFree(pui32SavePage);
		pui32SavePage = NULL;
	}
	xdcfActiveDevIdSet(DRIVE_NAND);
	xdcfCurDirByPosSet(ui32DirPos);
	return  err;
}

UINT32  mp3_GetFileList(void)
{
	UINT8  ui8NameStr[20];
	
	UINT32  i = 0;
	UINT32  ui32FileAttr;
	UINT32  ui32FileSize;
	UINT32  err;
	
	struct  tm  time;
	
	vfsCurrDirReset();
	#if  C_FILE_DEBUG
	    printf("current dir=%s  ",vfsGetCurrDirName());
	#endif
	while(1)
	{
		err = vfsDirRead(ui8NameStr, &ui32FileAttr, &ui32FileSize, &time);
		if(err!=SUCCESS)
			break;
		if(strlen(ui8NameStr)==0)
			continue;
		#if  C_FILE_DEBUG
		    printf("%d,%s,%x,%d  ",i,ui8NameStr,ui32FileAttr,ui32FileSize);
		#endif
		if(ui32FileAttr&xDCF_ATTR_DIR)
		{
			if(ui8NameStr[0]!='.')
				i++;
			continue;
		}
		else
			i++;
	}
	return  i;
}

void  performCommandPLC()
{
	UINT8  ui8NameStrDL[15];
	
	UINT32  pos;
	UINT32  err = SUCCESS;
	UINT32  ui32dstDirPos;
	
	switch(ucHMI_addr)
	{
		case  PLC_TIME:
			stime.tm_sec = uiHMIWord[PLC_TIME+5];
			stime.tm_min = uiHMIWord[PLC_TIME+4];
			stime.tm_hour = uiHMIWord[PLC_TIME+3];
			stime.tm_mday = uiHMIWord[PLC_TIME+2];
			stime.tm_mon = uiHMIWord[PLC_TIME+1];
			stime.tm_year = uiHMIWord[PLC_TIME]-1900;
			timeSet(&stime);
			#if  C_LEVI_DEBUG
			    printf("time=%d/%d/%d %d:%d:%d  ",uiHMIWord[PLC_TIME],uiHMIWord[PLC_TIME+1],uiHMIWord[PLC_TIME+2],uiHMIWord[PLC_TIME+3],uiHMIWord[PLC_TIME+4],uiHMIWord[PLC_TIME+5]);
			#endif
			if((uiHMIWord[PLC_TIME]!=2000)||(uiHMIWord[PLC_TIME+1]!=01))
				ui8TimeFlag = 1;
			timeGet(&ptime);
			sio_psprintf(ui8NameStrDL, "%02d%02d%02dDC.CSV", (UINT32)(ptime->tm_year-100)%100, ptime->tm_mon,ptime->tm_mday);
			xdcfCurRootDirSet(ui8TermModule);
			//mp3_GetFileList();
			
			pos = myFindFile(ui8NameStrDL);
			myGetSingleDayDL(pos, &ui32SuccessNum);
			break;
		case  PLC_PARA_BUF:
			memcpy((UINT8 *)&curDevice, (UINT8 *)&uiHMIWord[PLC_PARA_BUF], C_PLC_PARA_NUM*sizeof(UINT16));
			#if  C_LEVI_DEBUG
			    printf("device=%d, class=%d  ",uiHMIWord[PLC_MODULE_NO],uiHMIWord[PLC_MODULE_NO+1]);
			#endif
			getModuleName(ui16CurModule, ui8TermModule);
			
			writePara();
			//readPara();
			break;
		case  PLC_MODULE_NO:
			#if  C_LEVI_DEBUG
			    printf("device=%d, class=%d  ",uiHMIWord[PLC_MODULE_NO],uiHMIWord[PLC_MODULE_NO+1]);
			#endif
			ui16CurModule = uiHMIWord[PLC_MODULE_NO];
			ui16CurClass = uiHMIWord[PLC_MODULE_NO+1];
			getModuleName(ui16CurModule, ui8TermModule);
			
			readPara();
			writePara();			
			readParadelaytime();			
			writeParadelaytime();
			break;			
	  case TESTSTEP0:
	  	test_step0=(~uiHMIWord[TESTSTEP0])&0xffff;
	  	test_step1=(~uiHMIWord[TESTSTEP1])&0xff;
	  	test_mode=(uiHMIWord[TestMode]&0x01);
	  	printf("test_step0=%d, test_step1=%d, test_mode=%d ",test_step0,test_step1,test_mode);
	  	break;
	  case delay_time:
	  	memcpy((UINT8 *)&delay,(UINT8 *)&uiHMIWord[delay_time],C_delay_NUM*sizeof(UINT16));
	  	//memcpy((UINT8 *)&uiHMIWord[delay_time], (UINT8 *)&delay, C_delay_NUM*sizeof(UINT16));
	  	getModuleName(ui16CurModule, ui8TermModule);
			writeParadelaytime();
			//readParadelaytime();
	  	break;
		case  PLC_BAKCMD:
			gpioConfig(GPIO_CARD_DET, 0);
			gpioSet(GPIO_CARD_DET, 1);
			if(gpioGet(GPIO_CARD_DET)==0)
			{
				err = xdcfActiveDevIdSet(DRIVE_SD);
				xdcfInit(imageDirNameStr, imageFileNameStr, 0);
				dirInit();
				if(err==SUCCESS)
				{
					xdcfChange2Root();
					vfsMkdir(ui8TermModule);
					xdcfCurRootDirSet(ui8TermModule);
					xdcfCurDirPosGet(&ui32dstDirPos);
					myXdcfDelFile();
					xdcfActiveDevIdSet(DRIVE_NAND);
					xdcfCurRootDirSet(ui8TermModule);
					xdcfChange2Root();
					vfsChdir(ui8TermModule);
					err |= myXdcfCopyDisk(ui32dstDirPos);
					if(uiHMIWord[PLC_BAKCMD])
					{
						myXdcfDelFile();
					}
					if(err==SUCCESS)
						uiHMIWord[PLC_BAKINFO] = 2;
					else
						uiHMIWord[PLC_BAKINFO] = 4;  // ¶ÁSD¿¨´íÎó
				}
				else
				{
					uiHMIWord[PLC_BAKINFO] = 4;  // ¶ÁSD¿¨´íÎó
				}
			}
			else
			{
				uiHMIWord[PLC_BAKINFO] = 3;  // Ã»ÓÐSD¿¨
			}
			#if  C_FILE_DEBUG
			    printf("backup end  ");
			#endif
			break;
		default:
			break;
	}
}

void  F_Uart1ReturnMsg(UINT8* ucReturnMsg)
{
	UINT8  ucParityBCC,ch;
	
	UINT16  i;
	
	ucParityBCC = 0;
	i = 0;
	while((ch=ucReturnMsg[i++])!=0x00)
	{
		uart1fputchar(ch);
		ucParityBCC ^= ch;
	}
	uart1fputchar(ucChar[ucParityBCC>>4]);
	uart1fputchar(ucChar[ucParityBCC&0x0f]);
	uart1fputchar(0x0d);
}

void  F_Uart1ReturnErr(UINT8 index)
{
	UINT8  str[15];
	
	str[0] = 0x15;
	str[1] = ucChar[ucHMIAddress>>4];
	str[2] = ucChar[ucHMIAddress&0x0f];
	str[3] = ucHMI_RWflag;
	str[4] = ucChar[index>>4];
	str[5] = ucChar[index&0x0f];
	F_Uart1ReturnMsg(str);
}

UINT16  hexStrToi(UINT8* str, UINT8 len)
{
	UINT8  i,ch;
	
	UINT16  val;
	
	val = 0;
	for(i=0;i<len;i++)
	{
		ch = str[i];
		if(ch==0)
			break;
		if((ch>='0')&&(ch<='9'))
		{
			val = val*16;
			val += (ch-'0');
		}
		else if((ch>='A')&&(ch<='F'))
		{
			val = val*16;
			val += (ch-'A')+10;
		}
		else
			break;
	}
	if(i==0)
		return  -1;
	else
		return  val;
}

void  commLevi()
{
	int  temp;
	
	UINT8  *ptr;
	
	SINT16  i;
	
	UINT16  j,k;
	UINT16  address;
	UINT16  ui16ArraySign;
	
	ui32Time = tmrUsTimeGet();
	if((ui32Time>=ui32DelayTime2)||((ui32Time<ui32DelayTime2)&&((ui32DelayTime2-ui32Time)>600000000)))
	{
		ui32DelayTime2 = ui32Time+20000;
	}
	else
	{
		return;
	}
	
	while(1)
	{
		temp = uart1fgetc();
		if(temp==0xffff)
			break;
		if(temp!=0x0d)
		{
			ucUart1RxBuf[uiUart1RxPtr++] = temp;
		}
		else
		{
			for(i=0;(ucUart1RxBuf[i]!=0x05)&&(i<uiUart1RxPtr);i++);
			ui16ArraySign = i;
			ucHMIptr = &ucUart1RxBuf[i];
			ucHMI_RWflag = ucUart1RxBuf[i+3];
			if(i<uiUart1RxPtr)
			{
				address = hexStrToi(&ucUart1RxBuf[i+1], 2);
				if(address==ucHMIAddress)
				{
					ucHMI_RWmode = ucUart1RxBuf[i+4];
					ucHMI_addr = hexStrToi(&ucUart1RxBuf[i+5], 4);
					ucHMI_len = hexStrToi(&ucUart1RxBuf[i+9], 2);
					
					if((ucHMI_RWflag!='R')&&(ucHMI_RWflag!='W'))
					{
						#if  C_LEVI_DEBUG
						    printf("05(30h,35h)-invalid command  ");
						#endif
						F_Uart1ReturnErr(5);
						goto  hmiOver;
					}
					if((ucHMI_RWmode!='W')&&(ucHMI_RWmode!='B')&&(ucHMI_RWmode!='b'))
					{
						#if  C_LEVI_DEBUG
						    printf("05(30h,35h)-invalid command  ");
						#endif
						F_Uart1ReturnErr(5);
						goto  hmiOver;
					}
					if(ucHMI_RWmode!='b')
					{
						if(ucHMI_addr>=0x0fff)
						{
							#if  C_LEVI_DEBUG
							    printf("07(30h,37h)-invalid command  ");
							#endif
							F_Uart1ReturnErr(7);
							goto  hmiOver;
						}
						if((ucHMI_addr+ucHMI_len)>=0x0fff)
						{
							#if  C_LEVI_DEBUG
							    printf("08(30h,38h)-address over range  ");
							#endif
							F_Uart1ReturnErr(8);
							goto  hmiOver;
						}
					}
					else
					{
						if(ucHMI_addr>=0x03ff)
						{
							#if  C_LEVI_DEBUG
							    printf("07(30h,37h)-invalid address  ");
							#endif
							F_Uart1ReturnErr(7);
							goto  hmiOver;
						}
						if((ucHMI_addr+ucHMI_len)>=0x03ff)
						{
							#if  C_LEVI_DEBUG
							    printf("08(30h,38h)-address over range  ");
							#endif
							F_Uart1ReturnErr(8);
							goto  hmiOver;
						}
					}
					if(ucHMI_len>=0x40)
					{
						#if  C_LEVI_DEBUG
						    printf("09(30h,39h)-invalid data size  ");
						#endif
						F_Uart1ReturnErr(9);
						goto  hmiOver;
					}
					if(ucHMI_RWflag=='R')
						j = 0;
					else if(ucHMI_RWmode!='b')
						j = ucHMI_len*4;
					else
						j = ucHMI_len;
					if((i+13+j)!=uiUart1RxPtr)
					{
						#if  C_LEVI_DEBUG
						    printf("07(30h,34h)-invalid message size  ");
						#endif
						F_Uart1ReturnErr(4);
						goto  hmiOver;
					}
					
					j += i+11;
					ucHMI_bcc = hexStrToi(&ucUart1RxBuf[j], 2);
					for(k=i;k<j;k++)
					    ucHMI_bcc ^= ucUart1RxBuf[k];
					if(ucHMI_bcc!=0)
					{
						#if  C_LEVI_DEBUG
						    printf("08(30h,33h)-bcc error  ");
						#endif
						F_Uart1ReturnErr(3);
						goto  hmiOver;
					}
					// valid message
					#if  C_LEVI_DEBUG
					    printf("frame ok, ucHMI_addr=%x,ucHMI_len=%d  ",ucHMI_addr,ucHMI_len);
					#endif
					if(ucHMI_RWflag=='W')
					{
						ptr = &ucUart1RxBuf[i+11];
						
						if(ucHMI_RWmode!='b')
						{
							for(j=0;j<ucHMI_len;j++)
							{
								uiHMIWord[ucHMI_addr+j] = hexStrToi(ptr, 4);
								ptr += 4;
							}
						}
						else
						{
							for(j=0;j<ucHMI_len;j++)
							{
								ucHMIBit[ucHMI_addr+j] = *ptr-'0';
								ptr++;
							}
						}
						ucHMIptr[0] = 0x06;
						ucHMIptr[4] = '0';
						ucHMIptr[5] = '0';
						ucHMIptr[6] = 0;
						F_Uart1ReturnMsg(&ucUart1RxBuf[ui16ArraySign]);
						performCommandPLC();
					}
					else
					{
						ucHMIptr[0] = 0x06;
						ptr = &ucHMIptr[4];
						
						if(ucHMI_RWmode=='b')
						{
							for(j=0;j<ucHMI_len;j++)
							{
								*ptr++ = ucHMIBit[ucHMI_addr+j]+'0';
							}
						}
						else
						{
							for(j=0;j<ucHMI_len;j++)
							{
								k = uiHMIWord[ucHMI_addr+j];
								*ptr++ = ucChar[(k>>12)&0x000f];
								*ptr++ = ucChar[(k>>8)&0x000f];
								*ptr++ = ucChar[(k>>4)&0x000f];
								*ptr++ = ucChar[(k>>0)&0x000f];
							}
						}
						
						*ptr = 0;
						F_Uart1ReturnMsg(&ucUart1RxBuf[ui16ArraySign]);
						#if  0
						    switch(ucHMI_addr)
						    {
						    	case  PLC_REST_ROOM:
						    	case  PLC_REST_DAY:
						    	default:
						    		break;
						    }
						#endif
					}
				}
				else
				{
					#if  C_LEVI_DEBUG
					    printf("not fine addr=%x  ",address);
					#endif
					F_Uart1ReturnErr(6);
				}
			}
			else
			{
				#if  C_LEVI_DEBUG
				    printf("not fine 05,ptr=%x  ",uiUart1RxPtr);
				#endif
				F_Uart1ReturnErr(1);
			}
		hmiOver:
			uiUart1RxPtr = 0;
		}
	}
}

UINT32  myGetDevFreeSize()
{
	devInfo_t  pdevInfo;
	
	vfsDevInfoGet(DRIVE_NAND, 0, &pdevInfo);
	uiHMIWord[PLC_REST_ROOM] = pdevInfo.nrFreeBytes>>20;
	uiHMIWord[PLC_REST_DAY] = (pdevInfo.nrFreeBytes/1000)/400;
	#if  C_FILE_DEBUG
	    printf("room=%d,day=%d  ",uiHMIWord[PLC_REST_ROOM],uiHMIWord[PLC_REST_DAY]);
	#endif
	return  pdevInfo.nrFreeBytes/1000;
}

void  mySaveDataOnce()
{
	UINT8  ui8NameStrDL[20];
	UINT8  ui8NameStrAL[20];
	
	UINT32  pos;
	UINT32  err = SUCCESS;
	
	SINT32  i32DLfd = 0;
	
	xdcfAttrElm_t  xdcfFileAttr1;
	
	if(ui8TimeFlag==0)
		return;
	if(myGetDevFreeSize()<10)
	{
		#if  C_FILE_DEBUG
		     printf("not enough free room  ");
		#endif
		return;
	}
	sio_psprintf(ui8NameStrDL, "%02d%02d%02dDC.CSV", (UINT32)(ptime->tm_year-100)%100, ptime->tm_mon, ptime->tm_mday);
	strcpy(ui8NameStrAL, ui8NameStrDL);
	ui8NameStrAL[6] = 'A';
	#if  C_FILE_DEBUG
	    printf("DL=%s, AL=%s  ",ui8NameStrDL,ui8NameStrAL);
	#endif
	
	xdcfCurRootDirSet(ui8TermModule);
	vfsCurrDirReset();
	
	if((strlen(ui8DataBufDL)==0)||(ui8DataBufDL==NULL))
		goto  saveAlData;
	myHwWait1(1);
	pos = myFindFile(ui8NameStrDL);
	myHwWait1(1);
	if(pos==0)
	{
		i32DLfd = vfsOpen(ui8NameStrDL, O_CREATE, 0);
		myHwWait1(1);
		if(!i32DLfd)
		{
			#if  C_FILE_DEBUG
			    printf("create file fail=%s  ",ui8NameStrDL);
			#endif
			return;
		}
		else
		{
			#if  C_FILE_DEBUG
			    printf("create file ok  ");
			#endif
			vfsWrite(i32DLfd, ui8DLHeader1, strlen(ui8DLHeader1));
			myHwWait1(1);
			ui8TermName[2] = '0'+(ui16CurClass+1)/10;
			ui8TermName[3] = '0'+(ui16CurClass+1)%10;
			sio_psprintf(strbuf1, "%s,\r\n", ui8TermName);
			vfsWrite(i32DLfd, strbuf1, strlen(strbuf1));
			myHwWait1(1);
			vfsWrite(i32DLfd, ui8DLHeader2, strlen(ui8DLHeader2));
			myHwWait1(1);
			
			UINT8  k;
			if(C_MODULE_29_0004!=ui16CurModule)
			{
				if((C_MODULE_31_12103==ui16CurModule)||(C_MODULE_31_12100==ui16CurModule))
				{
					for(k=0;k<C_DL_HEADER_3_LEN;k++)
			    {
				    vfsWrite(i32DLfd, ui8DLHeader3[k], strlen(ui8DLHeader3[k]));
			    }
				}
				else if(C_MODULE_31_12104==ui16CurModule)
				{				  
					 for(k=0;k<C_DL_HEADER_3_LEN;k++)
			     {
				     vfsWrite(i32DLfd, ui8DLHeader3_31_12104[k], strlen(ui8DLHeader3[k]));
			     }			    
				}
				else
				{
					for(k=0;k<C_DL_HEADER_3_LEN;k++)
			    {
				    vfsWrite(i32DLfd, ui8DLHeader3_31_05052[k], strlen(ui8DLHeader3_31_05052[k]));
			    }
				}				
			}
			else
			{
				for(k=0;k<C_DL_HEADER_3_LEN-1;k++)
			  {
				  vfsWrite(i32DLfd, ui8DLHeader3_29_0004[k], strlen(ui8DLHeader3_29_0004[k]));
			  }
			}
			
			myHwWait1(1);
			vfsWrite(i32DLfd, "\r\n", 2);
			myHwWait1(1);
			vfsWrite(i32DLfd, ui8DataBufDL, strlen(ui8DataBufDL));
			myHwWait1(1);
			vfsClose(i32DLfd);
			myHwWait1(1);
		}
	}
	else
	{
		xdcfCurFileByPosSet(pos);
		err = xdcfCurFileAttrGet(&xdcfFileAttr1);
		i32DLfd = vfsOpen(xdcfFileAttr1.name, O_RDWR, S_IREAD);
		if(!i32DLfd)
		{
			#if  C_FILE_DEBUG
			    printf("open %s error  ",xdcfFileAttr1.name);
			#endif
			return;
		}
		else
		{
			vfsLseek(i32DLfd, 0, SEEK_END);
			vfsWrite(i32DLfd, ui8DataBufDL, strlen(ui8DataBufDL));
			vfsClose(i32DLfd);
		}
	}
	ui8DataBufDL[0] = '\0';
saveAlData:
	#if  C_FILE_DEBUG
	    printf("save file once  ");
	#endif
}