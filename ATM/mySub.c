// Input: 0: Key; 1: Frequency; 2: Tp207; 3: Tp208; 4: Tp209; 5: Tp205; 6: Tp202
UINT16  uisGpioQ[] = {  GPIO_KEY_G17A,GPIO_KEY_G17B, //GPIO_KEY_G15C, GPIO_KEY_G09C, GPIO_KEY_G09D, GPIO_KEY_G10C, GPIO_KEY_G10D, GPIO_KEY_G15D,
	                    //GPIO_KEY_G17A, GPIO_KEY_G15C, GPIO_KEY_G09C, GPIO_KEY_G09D, GPIO_KEY_G10C, GPIO_KEY_G10D, GPIO_KEY_G15D
	                 };


// Output: 0: Tp201; 1: AC in
UINT16  uisGpioK[] =
{	//DC			// AC
	GPIO_KEY_G09A, GPIO_KEY_G09B,	//右通道
	GPIO_KEY_G09C, GPIO_KEY_G09D,	//左通道
	//5.1K ohms		//5.1K ohms
	GPIO_KEY_G10D, GPIO_KEY_G10C,   
};

#define  C_SIZE_Q  (sizeof(uisGpioQ)/(MY_CHANNEL*sizeof(UINT16)))
#define  C_SIZE_K  (sizeof(uisGpioK)/(MY_CHANNEL*sizeof(UINT16)))

#define  GPIO_HA  (GPIO_KEY_G18C)
#define  GPIO_HB  (GPIO_KEY_G18D)

#define  PLCA_DATA01  400
#define  PLCA_DATA02  401
#define  PLCA_DATA03  402
#define  PLCA_DATA04  403
#define  PLCA_DATA05  404
#define  PLCA_DATA06  405
#define  PLCA_DATA07  406
#define  PLCA_DATA08  407
#define  PLCA_DATA09  408
#define  PLCA_DATA10  409
#define  PLCA_DATA11  410
#define  PLCA_DATA12  411
#define  PLCA_DATA13  412
#define  PLCA_DATA14  413
#define  PLCA_DATA15  414
#define  PLCA_DATA16  415
#define  PLCA_DATA17  416
#define  PLCA_DATA18  417
#define  PLCA_DATA19  418
#define  PLCA_DATA20  419
#define  PLCA_DATA21  420
#define  PLCA_DATA22  421
#define  PLCA_DATA23  422
#define  PLCA_DATA24  423
#define  PLCA_DATA25  424
#define  PLCA_DATA26  425
#define  PLCA_DATA27  426
#define  PLCA_DATA28  427
#define  PLCA_DATA29  428
#define  PLCA_DATA30  429
#define  PLCA_DATA31  430
#define  PLCA_DATA32  431
#define  PLCA_DATA33  432
#define  PLCA_DATA34  433
#define  PLCA_DATA35  434
#define  PLCA_DATA36  435
#define  PLCA_DATA37  436
#define  PLCA_DATA38  437
#define  PLCA_DATA39  438
#define  PLCA_DATA40  439
#define  PLCA_DATA41  440
#define  PLCA_DATA42  441
#define  PLCA_DATA43  442
#define  PLCA_DATA44  443
#define  PLCA_DATA45  444
#define  PLCA_DATA46  445
#define  PLCA_DATA47  446
#define  PLCA_DATA48  447
#define  PLCA_DATA49  448
#define  PLCA_DATA50  449
#define  PLCA_DATA51  450
#define  PLCA_DATA52  451
#define  PLCA_DATA53  452
#define  PLCA_DATA54  453
#define  PLCA_DATA55  454
#define  PLCA_DATA56  455
#define  PLCA_DATA57  456
#define  PLCA_DATA58  457
#define  PLCA_DATA59  458
#define  PLCA_DATA60  459
#define  PLCA_DATA61  460
#define  PLCA_DATA62  461
#define  PLCA_DATA63  462
#define  PLCA_DATA64  463
#define  PLCA_DATA65  464
#define  PLCA_DATA66  465
#define  PLCA_DATA67  466
#define  PLCA_DATA68  467
#define  PLCA_DATA69  468
#define  PLCA_DATA70  469
#define  PLCA_DATA71  470
#define  PLCA_DATA72  471
#define  PLCA_DATA73  472
#define  PLCA_DATA74  473
#define  PLCA_DATA75  474
#define  PLCA_DATA76  475
#define  PLCA_DATA77  476
#define  PLCA_DATA78  477
#define  PLCA_DATA79  478
#define  PLCA_DATA80  479
#define  PLCA_DATA81  480
#define  PLCA_DATA82  481
#define  PLCA_DATA83  482
#define  PLCA_DATA84  483
#define  PLCA_DATA85  484
#define  PLCA_DATA86  485
#define  PLCA_DATA87  486
#define  PLCA_DATA88  487
#define  PLCA_DATA89  488
#define  PLCA_DATA90  489
#define  PLCA_DATA91  490
#define  PLCA_DATA92  491
#define  PLCA_DATA93  492
#define  PLCA_DATA94  493
#define  PLCA_DATA95  494
#define  PLCA_DATA96  495
#define  PLCA_DATA97  496
#define  PLCA_DATA98  497
#define  PLCA_DATA99  498
#define  PLCA_DATA100  499


#define  PLCS_RESULT01  600
#define  PLCS_RESULT02  601
#define  PLCS_RESULT03  602
#define  PLCS_RESULT04  603
#define  PLCS_RESULT05  604
#define  PLCS_RESULT06  605
#define  PLCS_RESULT07  606
#define  PLCS_RESULT08  607
#define  PLCS_RESULT09  608
#define  PLCS_RESULT10  609
#define  PLCS_RESULT11  610
#define  PLCS_RESULT12  611
#define  PLCS_RESULT13  612
#define  PLCS_RESULT14  613
#define  PLCS_RESULT15  614
#define  PLCS_RESULT16  615
#define  PLCS_RESULT17  616
#define  PLCS_RESULT18  617
#define  PLCS_RESULT19  618
#define  PLCS_RESULT20  619
#define  PLCS_RESULT21  620
#define  PLCS_RESULT22  621
#define  PLCS_RESULT23  622
#define  PLCS_RESULT24  623
#define  PLCS_RESULT25  624
#define  PLCS_RESULT26  625
#define  PLCS_RESULT27  626
#define  PLCS_RESULT28  627
#define  PLCS_RESULT29  628
#define  PLCS_RESULT30  629
#define  PLCS_RESULT31  630
#define  PLCS_RESULT32  631
#define  PLCS_RESULT33  632
#define  PLCS_RESULT34  633
#define  PLCS_RESULT35  634
#define  PLCS_RESULT36  635
#define  PLCS_RESULT37  636
#define  PLCS_RESULT38  637
#define  PLCS_RESULT39  638
#define  PLCS_RESULT40  639
#define  PLCS_RESULT41  640
#define  PLCS_RESULT42  641
#define  PLCS_RESULT43  642
#define  PLCS_RESULT44  643
#define  PLCS_RESULT45  644
#define  PLCS_RESULT46  645
#define  PLCS_RESULT47  646
#define  PLCS_RESULT48  647
#define  PLCS_RESULT49  648
#define  PLCS_RESULT50  649
#define  PLCS_RESULT51  650
#define  PLCS_RESULT52  651
#define  PLCS_RESULT53  652
#define  PLCS_RESULT54  653
#define  PLCS_RESULT55  654
#define  PLCS_RESULT56  655
#define  PLCS_RESULT57  656
#define  PLCS_RESULT58  657
#define  PLCS_RESULT59  658
#define  PLCS_RESULT60  659
#define  PLCS_RESULT61  660
#define  PLCS_RESULT62  661
#define  PLCS_RESULT63  662
#define  PLCS_RESULT64  663
#define  PLCS_RESULT65  664
#define  PLCS_RESULT66  665
#define  PLCS_RESULT67  666
#define  PLCS_RESULT68  667
#define  PLCS_RESULT69  668
#define  PLCS_RESULT70  669
#define  PLCS_RESULT71  670
#define  PLCS_RESULT72  671
#define  PLCS_RESULT73  672
#define  PLCS_RESULT74  673
#define  PLCS_RESULT75  674
#define  PLCS_RESULT76  675
#define  PLCS_RESULT77  676
#define  PLCS_RESULT78  677
#define  PLCS_RESULT79  678
#define  PLCS_RESULT80  679
#define  PLCS_RESULT81  680
#define  PLCS_RESULT82  681
#define  PLCS_RESULT83  682
#define  PLCS_RESULT84  683
#define  PLCS_RESULT85  684
#define  PLCS_RESULT86  685
#define  PLCS_RESULT87  686
#define  PLCS_RESULT88  687
#define  PLCS_RESULT89  688
#define  PLCS_RESULT90  689
#define  PLCS_RESULT91  690
#define  PLCS_RESULT92  691
#define  PLCS_RESULT93  692
#define  PLCS_RESULT94  693
#define  PLCS_RESULT95  694
#define  PLCS_RESULT96  695
#define  PLCS_RESULT97  696
#define  PLCS_RESULT98  697
#define  PLCS_RESULT99  698
#define  PLCS_RESULT100  699

#define  PLCA_DATA_CNT		10
#define  PLCS_RESULT_CNT	10

#define  C_CURRENT_SWITCH_SIGN  0x01
#define  C_PRE_SWITCH_SIGN      0x02
#define  C_BREAK_OFF_TEST       0x04
#define  C_CURRENT_TEST_RESULT  0x08
#define  C_SET_NEXT_STEP        0x10

UINT8  ui8Chnl = 0;

UINT8  bChnlFlag[MY_CHANNEL];
//***************************************
//为DefaultGernt函数添加的变量
UINT8  button[MY_CHANNEL];
UINT8  switchon[MY_CHANNEL];
UINT8  quenwait[MY_CHANNEL];
UINT8  testover[MY_CHANNEL];
UINT8  flagsource=0;
//**********************************************
UINT8  uis8TestStep[MY_CHANNEL];
UINT8  uis8DataCnt[MY_CHANNEL];

UINT8  uis8ErrCount[MY_CHANNEL];

#define  C_VID16       0
#define  C_POW_DEVICE  C_VID16

#define  C_IT8500     0
#define  C_EL_DEVICE  C_IT8500

//**************************************************************************************************
//    IT8500系列电子负载
//**************************************************************************************************
#define  C_IT8500_REMOTE		0
#define  C_IT8500_OPEN			1
#define  C_IT8500_CLOSE			2
#define  C_IT8500_READV			3//* 读取EL的输入电压、输入电流、输入功率及相关状态
#define  C_IT8500_READA			4//* 读取EL的输入电压、输入电流、输入功率及相关状态
#define  C_IT8500_MODECV		5
#define  C_IT8500_MODECC		6
#define  C_IT8500_MODEFIXED		7
#define  C_IT8500_MODESHORT 	8
#define  C_IT8500_CV_10V		9
#define  C_IT8500_CV_41V5		10
#define  C_IT8500_CV_55V		11
#define  C_IT8500_CC_500mA		12
#define  C_IT8500_MaxVolt_65V	13
#define  C_IT8500_MaxCurt_3A	14
#define  C_IT8500_CV_57V6		15
#define  C_IT8500_CV_58V1		16
#define  C_IT8500_CV_57V5		17
#define  C_IT8500_CV_40V		18
#define  C_IT8500_CV_41V3		19
#define  C_IT8500_CV_57V		20
#define  C_IT8500_CV_57V65		21
#define  C_IT8500_CC_3A			22
#define  C_IT8500_CV_4V5		23

static UINT8 uis8IT8500Cmd[][25] =
{
/*0*/  0xaa,0x00,0x20,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*1*/  0xaa,0x00,0x21,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*2*/  0xaa,0x00,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*3*/  0xaa,0x00,0x5f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*4*/  0xaa,0x00,0x5f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*5*/  0xaa,0x00,0x28,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*6*/  0xaa,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*7*/  0xaa,0x00,0x5d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*8*/  0xaa,0x00,0x5d,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*9*/  0xaa,0x00,0x2c,0x10,0x27,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*10*/ 0xaa,0x00,0x2c,0x1c,0xa2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*11*/ 0xaa,0x00,0x2c,0xd8,0xd6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*12*/ 0xaa,0x00,0x2a,0x88,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*13*/ 0xaa,0x00,0x22,0xe8,0xfd,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*14*/ 0xaa,0x00,0x24,0x30,0x75,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*15*/ 0xaa,0x00,0x2c,0x00,0xe1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*16*/ 0xaa,0x00,0x2c,0xf4,0xe2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*17*/ 0xaa,0x00,0x2c,0x9c,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*18*/ 0xaa,0x00,0x2c,0x40,0x9c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*19*/ 0xaa,0x00,0x2c,0x54,0xa1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*20*/ 0xaa,0x00,0x2c,0xa8,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*21*/ 0xaa,0x00,0x2c,0x32,0xe1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*22*/ 0xaa,0x00,0x2a,0x30,0x75,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*23*/ 0xaa,0x00,0x2c,0x94,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

#define  C_TTI1906_REMOTE    0
#define  C_TTI1906_RANGE_3   1
#define  C_TTI1906_MODE_DCV  2
#define  C_TTI1906_MODE_ACV  3
#define  C_TTI1906_READV     4

static  UINT8  uis8TTI1906Cmd[5][9] = {  "REMOTE \n",
	                                     "RANGE 3 \n",
	                                     "VDC \n",
	                                     "VAC \n",
	                                     "READ? \n"
	                                  };

#define  C_WAIT      0
#define  C_STEP      1

#define  C_RS232_RX_BUF_LEN  64

UINT8  ui8Port = 3;
UINT8  data232[C_RS232_RX_BUF_LEN];

UINT32  ui32PowWatt;
UINT32  ui32PowVa;
UINT32  ui32PowVol1;
UINT32  ui32ElVol;
UINT32  ui32ElCur;
UINT32  ui32TtiVol;

/*****************************************************************************/
void F_set_DC_right(UINT8 value)	//电池
{
	mySetGpio(uisGpioK[0], value);
}

void F_set_AC_right(UINT8 value) //DC-5V
{
	mySetGpio(uisGpioK[1], value);
}

void F_set_DC_left(UINT8 value)	//电池
{
	mySetGpio(uisGpioK[2], value);
}

void F_set_AC_left(UINT8 value) //DC-5V
{
	mySetGpio(uisGpioK[3], value);
}

void F_set_5K1_1(UINT8 value) //5K1 OHMS
{
	mySetGpio(uisGpioK[4], value);
}
void F_set_5K1_2(UINT8 value) //5K1 OHMS
{
	mySetGpio(uisGpioK[5], value);
}

/*****************************************************************************/
UINT8  F_GetKey(UINT8 ui8Chnl)
{
	return  myGetGpio(uisGpioQ[ui8Chnl*C_SIZE_Q+0]);
}

UINT8  F_GetKey2(UINT8 ui8Chnl)
{
	return  myGetGpio(uisGpioQ[ui8Chnl*C_SIZE_Q+1]);
}

UINT8  F_GetFreq()
{
	return  myGetGpio(uisGpioQ[ui8Chnl*C_SIZE_Q+1]);
}

UINT32  readBoardIO()
{
	UINT32  i;
	
	UINT32  status = 0;
	
	for(i=2;i<7;i++)
	{
		status = status<<1;
		status |= myGetGpio(uisGpioQ[ui8Chnl*C_SIZE_Q+i]);
		//printf("uisGpioQ%d=%d  ",ui8Chnl*C_SIZE_Q+i,myGetGpio(uisGpioQ[ui8Chnl*C_SIZE_Q+i]));//ADD.LIU
	}
	return  status;
}

void  F_SetAcIn1(UINT8 val)
{
	mySetGpio(uisGpioK[ui8Chnl*C_SIZE_K+0], val);
}

void  F_SetAcIn2(UINT8 val)
{
	mySetGpio(uisGpioK[ui8Chnl*C_SIZE_K+2], val);
}

void  F_SetSW1(UINT8 val)//ADD.LIU@101119@8:17
{
	mySetGpio(uisGpioK[ui8Chnl*C_SIZE_K+1], val);
}
void  F_SetTp201(UINT8 val)//ADD.LIU@101119@8:17
{
	mySetGpio(uisGpioK[ui8Chnl*C_SIZE_K+1], val);
}

void  F_DefaultQ(UINT8 ui8Chnl)
{
	UINT8  i;
	UINT8  ui8Kinit[4] = {0,0,0,0};
	
	for(i=0;i<C_SIZE_K;i++)
	{
		mySetGpio(uisGpioK[ui8Chnl*C_SIZE_K+i], ui8Kinit[i]);
	}
}

void  F_DealUrgent0()
{
//	UINT8  ui8Chnl;
	UINT8  switchSign;
	
	UINT16  i;
	
	ui32Time = tmrUsTimeGet();
	//for(ui8Chnl=0;ui8Chnl<MY_CHANNEL;ui8Chnl++)
//	{
    ui8Chnl=0;
		switchSign = F_GetKey(ui8Chnl);
		if((switchSign&&(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)==0)||((switchSign==0)&&(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)))
		{
			bChnlFlag[ui8Chnl] ^= C_CURRENT_SWITCH_SIGN;
			ui32DelayTime4[ui8Chnl] = ui32Time+50000;
		}
		
		#if  (C_TEST_MODE==C_AUTO)
		    if((ui32Time>=ui32DelayTime4[ui8Chnl])||((ui32Time<ui32DelayTime4[ui8Chnl])&&((ui32DelayTime4[ui8Chnl]-ui32Time)>600000000)))
		    {
		    	if(((bChnlFlag[ui8Chnl]&C_PRE_SWITCH_SIGN)==0)&&(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN))
		    	{
		    		#if  C_SWITCH_DEBUG
		    		    printf("channel%d start  ",ui8Chnl);
		    		#endif
		    		uis8TestStep[ui8Chnl] = 0;
		    		bChnlFlag[ui8Chnl] &= ~C_BREAK_OFF_TEST;
		    		for(i=0;i<PLCA_DATA_CNT;i++)
		    		    uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;
		    		for(i=0;i<PLCS_RESULT_CNT;i++)
		    		    uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;
		    		uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] = 1;
		    		
		    		uis8ErrCount[ui8Chnl] = 0;
		    		uis8DataCnt[ui8Chnl] = 1;
		    	}
		    	else if((bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)==0)
		    	{
		    		if((uis8TestStep[ui8Chnl]!=255)||(bChnlFlag[ui8Chnl]&C_BREAK_OFF_TEST))
		    		{
		    			#if  C_SWITCH_DEBUG
		    			    printf("channel%d stop  ",ui8Chnl);
		    			#endif
		    			uis8TestStep[ui8Chnl] = 255;
		    			flagsource=1;
		    			bChnlFlag[ui8Chnl] |= C_BREAK_OFF_TEST;
		    			for(i=0;i<PLCA_DATA_CNT;i++)
		    			    uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;
		    			for(i=0;i<PLCS_RESULT_CNT;i++)
		    			    uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;
		    			uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] = 0;
		    			uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+1] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+2] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+3] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+4] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+5] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+6] = 0;
		    			F_DefaultQ(ui8Chnl);
		    		}
		    	}
		    	if(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)
		    	{
		    		bChnlFlag[ui8Chnl] |= C_PRE_SWITCH_SIGN;
		    	}
		    	else
		    	{
		    		bChnlFlag[ui8Chnl] &= ~C_PRE_SWITCH_SIGN;
		    	}
		    }
		#elif  (C_TEST_MODE==C_MANUAL)
		    if((ui32Time>=ui32DelayTime4[ui8Chnl])||((ui32Time<ui32DelayTime4[ui8Chnl])&&((ui32DelayTime4[ui8Chnl]-ui32Time)>600000000)))
		    {
		    	if(((bChnlFlag[ui8Chnl]&C_PRE_SWITCH_SIGN)==0)&&(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN))
		    	{
		    		bChnlFlag[ui8Chnl] |= C_SET_NEXT_STEP;
		    		if(uis8TestStep[ui8Chnl]==255)
		    		{
		    			#if  C_SWITCH_DEBUG
		    			    printf("channel%d start  ",ui8Chnl);
		    			#endif
		    			for(i=0;i<PLCA_DATA_CNT;i++)
		    			    uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;
		    			for(i=0;i<PLCS_RESULT_CNT;i++)
		    			    uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;
		    			uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] = 1;
		    			uis8ErrCount[ui8Chnl] = 0;
		    			uis8DataCnt[ui8Chnl] = 1;
		    		}
		    	}
		    	if(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)
		    	{
		    		bChnlFlag[ui8Chnl] |= C_PRE_SWITCH_SIGN;
		    	}
		    	else
		    	{
		    		bChnlFlag[ui8Chnl] &= ~C_PRE_SWITCH_SIGN;
		    	}
		    }
		#endif
//	}
}

void  F_DealUrgent1()
{
	//UINT8  ui8Chnl;
	UINT8  switchSign;
	
	UINT16  i;
	
	ui32Time = tmrUsTimeGet();
	//for(ui8Chnl=0;ui8Chnl<MY_CHANNEL;ui8Chnl++)
//	{
  ui8Chnl=1;
		switchSign = F_GetKey(ui8Chnl);
		if((switchSign&&(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)==0)||((switchSign==0)&&(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)))
		{
			bChnlFlag[ui8Chnl] ^= C_CURRENT_SWITCH_SIGN;
			ui32DelayTime4[ui8Chnl] = ui32Time+50000;
		}
		
		#if  (C_TEST_MODE==C_AUTO)
		    if((ui32Time>=ui32DelayTime4[ui8Chnl])||((ui32Time<ui32DelayTime4[ui8Chnl])&&((ui32DelayTime4[ui8Chnl]-ui32Time)>600000000)))
		    {
		    	if(((bChnlFlag[ui8Chnl]&C_PRE_SWITCH_SIGN)==0)&&(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN))
		    	{
		    		#if  C_SWITCH_DEBUG
		    		    printf("channel%d start  ",ui8Chnl);
		    		#endif
		    		uis8TestStep[ui8Chnl] = 0;
		    		bChnlFlag[ui8Chnl] &= ~C_BREAK_OFF_TEST;
		    		for(i=0;i<PLCA_DATA_CNT;i++)
		    		    uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;
		    		for(i=0;i<PLCS_RESULT_CNT;i++)
		    		    uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;
		    		uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] = 1;
		    		
		    		uis8ErrCount[ui8Chnl] = 0;
		    		uis8DataCnt[ui8Chnl] = 1;
		    	}
		    	else if((bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)==0)
		    	{
		    		if((uis8TestStep[ui8Chnl]!=255)||(bChnlFlag[ui8Chnl]&C_BREAK_OFF_TEST))
		    		{
		    			#if  C_SWITCH_DEBUG
		    			    printf("channel%d stop  ",ui8Chnl);
		    			#endif
		    			uis8TestStep[ui8Chnl] = 255;
		    			flagsource=0;
		    			bChnlFlag[ui8Chnl] |= C_BREAK_OFF_TEST;
		    			for(i=0;i<PLCA_DATA_CNT;i++)
		    			    uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;
		    			for(i=0;i<PLCS_RESULT_CNT;i++)
		    			    uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;
		    			uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] = 0;
		    			uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+1] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+2] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+3] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+4] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+5] = 0;
		    		  uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl+6] = 0;
		    			F_DefaultQ(ui8Chnl);
		    		}
		    	}
		    	if(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)
		    	{
		    		bChnlFlag[ui8Chnl] |= C_PRE_SWITCH_SIGN;
		    	}
		    	else
		    	{
		    		bChnlFlag[ui8Chnl] &= ~C_PRE_SWITCH_SIGN;
		    	}
		    }
		#elif  (C_TEST_MODE==C_MANUAL)
		    if((ui32Time>=ui32DelayTime4[ui8Chnl])||((ui32Time<ui32DelayTime4[ui8Chnl])&&((ui32DelayTime4[ui8Chnl]-ui32Time)>600000000)))
		    {
		    	if(((bChnlFlag[ui8Chnl]&C_PRE_SWITCH_SIGN)==0)&&(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN))
		    	{
		    		bChnlFlag[ui8Chnl] |= C_SET_NEXT_STEP;
		    		if(uis8TestStep[ui8Chnl]==255)
		    		{
		    			#if  C_SWITCH_DEBUG
		    			    printf("channel%d start  ",ui8Chnl);
		    			#endif
		    			for(i=0;i<PLCA_DATA_CNT;i++)
		    			    uiHMIWord[PLCA_DATA01+ui8Chnl*PLCA_DATA_CNT+i] = 0;
		    			for(i=0;i<PLCS_RESULT_CNT;i++)
		    			    uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+i] = 0;
		    			uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] = 1;
		    			uis8ErrCount[ui8Chnl] = 0;
		    			uis8DataCnt[ui8Chnl] = 1;
		    		}
		    	}
		    	if(bChnlFlag[ui8Chnl]&C_CURRENT_SWITCH_SIGN)
		    	{
		    		bChnlFlag[ui8Chnl] |= C_PRE_SWITCH_SIGN;
		    	}
		    	else
		    	{
		    		bChnlFlag[ui8Chnl] &= ~C_PRE_SWITCH_SIGN;
		    	}
		    }
		#endif
//	}
}

void  myHwWait1(UINT32 time)
{
	while(time)
	{
		hwWait(1, 3);  // units: 10us
		//F_DealUrgent();
		if(flagsource==0)
		{
			F_DealUrgent0();
			if((resultgetkey=F_GetKey(1))==0)
				{
					F_DealUrgent1();
				}
			ui8Chnl=0;
		}		
		if(flagsource==1)
		{
			F_DealUrgent1();
			if((resultgetkey=F_GetKey(0))==0)
				{
					F_DealUrgent0();
				}
			ui8Chnl=1;
		}
		commLevi();
		time--;
	}
}

void  myHwWait2(UINT32 time)
{
	while(time)
	{
		hwWait(0, 1);  // units: 1ms
		//F_DealUrgent();
		if(flagsource==0)
		{
			F_DealUrgent0();
			if((resultgetkey=F_GetKey(1))==0)
				{
					F_DealUrgent1();
				}
			ui8Chnl=0;
		}		
		if(flagsource==1)
		{
			F_DealUrgent1();
			if((resultgetkey=F_GetKey(0))==0)
				{
					F_DealUrgent0();
				}
			ui8Chnl=1;
		}
		commLevi();
		time--;
	}
}

void  waitUart0Idle()
{
	UINT8  txRdy;
	
	uartReg_t*  puartReg;
	
	puartReg = (uartReg_t *)0xb0000000;  // The UART0 register mapping
	do{
		txRdy = puartReg->sts;
	} while(!(txRdy&0x20));  // Wait to send data
	hwWait(0, 5);  // units: 1ms
}

void  setUart0Baud(UINT32 baud)
{
	UINT32  baudDiv,baudRemainder;
	
	uartReg_t*  puartReg;
	
	puartReg = (uartReg_t *)0xb0000000;  // The UART0 register mapping
	
	baudDiv = 48000000/baud;
	baudRemainder = 48000000%baud;
	
	puartReg->baudDiv[0] = baudDiv&0x000000ff;
	puartReg->baudDiv[1] = (baudDiv>>8)&0x000000ff;
	puartReg->baudDiv[2] = (baudDiv>>16)&0x000000ff;
	
	while(fgetchar()!=0xffff)
	{
	}
	hwWait(1, 3);  // units: 10us
}

void  uart0Printf(UINT8 *str)
{
	UINT8  i;
	
	for(i=0;i<strlen(str);i++)
	    fputchar(str[i]);
}

void  F_Set232Port(UINT16 port, UINT32 baud)
{
	if((port&0x03)==ui8Port)
		return;
	
	myHwWait2(1);
	
	waitUart0Idle();
	ui8Port = port&0x03;
	switch(port&0x03)
	{
		case  0:
			mySetGpio(GPIO_HA, 0);
			mySetGpio(GPIO_HB, 0);
			break;
		case  1:
			mySetGpio(GPIO_HA, 1);
			mySetGpio(GPIO_HB, 0);
			break;
		case  2:
			mySetGpio(GPIO_HA, 0);
			mySetGpio(GPIO_HB, 1);
			break;
		case  3:
			mySetGpio(GPIO_HA, 1);
			mySetGpio(GPIO_HB, 1);
			break;
		default:
			break;
	}
	setUart0Baud(baud);
}

void  nextStep(UINT8 type)
{
	#if  (C_TEST_MODE==C_AUTO)
	    if(uis8TestStep[ui8Chnl]!=255)
	    {
	    	uis8TestStep[ui8Chnl] += 1;
	    	if(type==C_STEP)
	    	{
	    		while(1)
	    		{
	    			ui32Time = tmrUsTimeGet();
	    			if((ui32Time>=ui32DelayTime5[ui8Chnl])||((ui32Time<ui32DelayTime5[ui8Chnl])&&((ui32DelayTime5[ui8Chnl]-ui32Time)>600000000)))
	    			{
	    				F_SetTp201(MY_LOW);
	    				myHwWait2(20);
	    				F_SetTp201(MY_HIGH);
	    				
	    				ui32Time = tmrUsTimeGet();
	    				ui32DelayTime5[ui8Chnl] = ui32Time+20000;
	    				break;
	    			}
	    			myHwWait2(1);
	    		}
	    	}
	    }
	#elif  (C_TEST_MODE==C_MANUAL)
	    if(bChnlFlag[ui8Chnl]&C_SET_NEXT_STEP)
	    {
	    	bChnlFlag[ui8Chnl] &= ~C_SET_NEXT_STEP;
	    	uis8TestStep[ui8Chnl] += 1;
	    	if(type==C_STEP)
	    	{
	    		while(1)
	    		{
	    			ui32Time = tmrUsTimeGet();
	    			if((ui32Time>=ui32DelayTime5[ui8Chnl])||((ui32Time<ui32DelayTime5[ui8Chnl])&&((ui32DelayTime5[ui8Chnl]-ui32Time)>600000000)))
	    			{
	    				F_SetTp201(MY_LOW);
	    				myHwWait2(20);
	    				F_SetTp201(MY_HIGH);
	    				
	    				ui32Time = tmrUsTimeGet();
	    				ui32DelayTime5[ui8Chnl] = ui32Time+20000;
	    				break;
	    			}
	    			myHwWait2(1);
	    		}
	    	}
	    }
	#endif
}

void  nextData(UINT8 repeatTimes, UINT32 waitTime)
{
	if(((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)||(uis8ErrCount[ui8Chnl]>=repeatTimes))
	{
		if((bChnlFlag[ui8Chnl]&C_CURRENT_TEST_RESULT)==0)
		{
			uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+uis8DataCnt[ui8Chnl]-1] = 2;
		}
		else
		{
			uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+uis8DataCnt[ui8Chnl]-1] = 3;
			uiHMIWord[PLCA_ALL_RESULT01+ui8Chnl] = 3;
		}
		bChnlFlag[ui8Chnl] &= ~C_CURRENT_TEST_RESULT;
		uis8ErrCount[ui8Chnl] = 0;
		
		if(uis8TestStep[ui8Chnl] != PLCS_RESULT_CNT)//去掉另一通道的黄块
		{
			uis8DataCnt[ui8Chnl] += 1;
		    uiHMIWord[PLCS_RESULT01+ui8Chnl*PLCS_RESULT_CNT+uis8DataCnt[ui8Chnl]-1] = 1;
		}
		
			
	}
	else
	{
		ui32Time = tmrUsTimeGet();
		ui32DelayTime3[ui8Chnl] = ui32Time+waitTime;
	}
}

UINT8  saveTestData(UINT16 type, UINT32 value)
{
	UINT8  result;
	
	if(uis8TestStep[ui8Chnl]==255)
		return;
	
	uiHMIWord[type+ui8Chnl*PLCA_DATA_CNT] = value;
	
	result = 0;
	 switch(type) //======================22-0011=======================//*********************LIUJINSEN@ADD,2011-07-29**********
	{
		case  PLCA_DATA01:
			if(((UINT16)(value)<curDevice.STEP1_min)||((UINT16)(value)>curDevice.STEP1_max))
				result = 1;//* 不在范围内，result置1
			break;
		case  PLCA_DATA02:
			if(((UINT16)(value)<curDevice.STEP2_min)||((UINT16)(value)>curDevice.STEP2_max))
				result = 1;//* 不在范围内，result置1
			break;
		case  PLCA_DATA03:
			if(((UINT16)(value)<curDevice.STEP3_min)||((UINT16)(value)>curDevice.STEP3_max))
				result = 1;//* 不在范围内，result置1
			break;
		case  PLCA_DATA04:
			if(((UINT16)(value)<curDevice.STEP4_min)||((UINT16)(value)>curDevice.STEP4_max))
				result = 1;//* 不在范围内，result置1
			break;
		case  PLCA_DATA05:
			if(((UINT16)(value)<curDevice.STEP5_min)||((UINT16)(value)>curDevice.STEP5_max))
				result = 1;//* 不在范围内，result置1
			break;
		case  PLCA_DATA06:
			if(((UINT16)(value)<curDevice.STEP6_min)||((UINT16)(value)>curDevice.STEP6_max))
				result = 1;//* 不在范围内，result置1
			break;
		case  PLCA_DATA07:
			if(((UINT16)(value)<curDevice.STEP7_min)||((UINT16)(value)>curDevice.STEP7_max))
				result = 1;//* 不在范围内，result置1
			break;
		default:
			break;
	}//=====================================================
	
	if(result==1)
	{
		bChnlFlag[ui8Chnl] |= C_CURRENT_TEST_RESULT;
		uis8ErrCount[ui8Chnl] += 1;
		return result;
	}
	else
	{
		bChnlFlag[ui8Chnl] &= ~C_CURRENT_TEST_RESULT;
		return result;
	}
}

UINT32  myVid16Val(UINT8* str, UINT8 mode)
{
	UINT8  ch;
	
	UINT32  ul32Value;
	
	ul32Value = (str[0]|((UINT32)str[1]<<8)|((UINT32)(str[2]&0x7f)<<16));
//	if(ul32Value==0)
//		return  -1;
	if(ul32Value==0) //SyroZhang:如果没有数据就返回0
		return  0;
	ul32Value = ul32Value + 0x00800000;
	ch = ((str[3]&0x7f)<<1);
	if(str[2]&0x80)
		ch++;
	if(ch>=127)
	{
		ch = ch-127;
		ch = 23-ch;
	}
	else
	{
		ch = 127-ch;
		ch = 23+ch;
	}
	if(mode==0)
	{
		ul32Value = ul32Value>>4;
		ul32Value = (ul32Value*1000)>>(ch-4);
	}
	else if(mode==1)
	{
		ul32Value = ul32Value>>16;
		ul32Value = (ul32Value*100000)>>(ch-16);  // 0.01mA
	}
	if(str[3]&0x80)
		return  ul32Value*(-1);
	else
		return  ul32Value;
}

UINT32  myIt8500Val(UINT8* str, UINT8 mode)
{
	UINT32  ul32Value;
	
	ul32Value = ((UINT32)str[0]|((UINT32)str[1]<<8)|((UINT32)str[2]<<16)|((UINT32)str[3]<<24));
	if(mode==1)
		ul32Value /= 10;
	
	return  ul32Value;
}

UINT32  myTti1906Val(UINT8* str)
{
	UINT8  i;
	
	UINT32  ul32Value;
	
	ul32Value = str[1]-'0';
	for(i=3;i<8;i++)
	{
		ul32Value *= 10;
		ul32Value += str[i]-'0';
	}
	if(str[9]=='+')
	{
		i = str[10]-'0';
		while(i)
		{
			ul32Value *= 10;
			i -= 1;
		}
	}
	else
	{
		i = str[10]-'0';
		while(i)
		{
			ul32Value /= 10;
			i -= 1;
		}
	}
	ul32Value /= 100;
	
	return  ul32Value;
}

void  readVid16Data()
{
	UINT8  i;
	UINT8  delayFlag;
	UINT8  ptr;
	
	int  temp;
	
	for(ptr=0;ptr<C_RS232_RX_BUF_LEN;ptr++)
	    data232[ptr] = 0;
	
	ui32Time = tmrUsTimeGet();
	ui32DelayTime3[ui8Chnl] = ui32Time+100000;
	delayFlag = 0;
	ptr = 0;
	while(1)
	{
		myHwWait2(1);
		while(1)
		{
			temp = fgetchar();
			if(temp==0xffff)
				break;
			data232[ptr++] = temp;
		}
		
		ui32Time = tmrUsTimeGet();
		for(i=0;i<ptr;i++)
		{
			if((data232[i]==0xaa)&&(data232[i+1]==0x00)&&(data232[i+2]==0x10))
			{
				if(delayFlag==0)
					ui32DelayTime3[ui8Chnl] = ui32Time+31250;
				if(ptr>=(i+24))
					ui32DelayTime3[ui8Chnl] = ui32Time;
				delayFlag = 1;
				break;
			}
		}
		if(delayFlag==1)
		{
			if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			{
				UINT32  ui32PowVol,ui32PowCur;
				
				ui32PowVol = myVid16Val(&data232[i+3], 0);
				ui32PowCur = myVid16Val(&data232[i+7], 1)/100;
				ui32PowWatt = myVid16Val(&data232[i+11], 0);
				ui32PowVa = ui32PowVol*ui32PowCur/1000;
				ui32PowVol1 = ui32PowVol;
				break;
			}
		}
		else
		{
			if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			{
//				ui32PowWatt = -1;
//				ui32PowVa = -1;
				ui32PowWatt = 0;
				ui32PowVa = 0;
				break;
			}
		}
	}
}

void  readIt8500Data()
{
	UINT8  i;
	UINT8  delayFlag;
	UINT8  ptr;
	
	int  temp;
	
	for(ptr=0;ptr<C_RS232_RX_BUF_LEN;ptr++)
	    data232[ptr] = 0;
	
	ui32Time = tmrUsTimeGet();
	ui32DelayTime3[ui8Chnl] = ui32Time+100000;
	delayFlag = 0;
	ptr = 0;
	while(1)
	{
		myHwWait2(1);
		while(1)
		{
			temp = fgetchar();
			if(temp==0xffff)
				break;
			data232[ptr++] = temp;
		}
		
		ui32Time = tmrUsTimeGet();
		for(i=0;i<ptr;i++)
		{
			if((data232[i]==0xaa)&&(data232[i+1]==0x00)&&(data232[i+2]==0x5f))
			{
				if(delayFlag==0)
					ui32DelayTime3[ui8Chnl] = ui32Time+12188;
				if(ptr>=(i+26))
					ui32DelayTime3[ui8Chnl] = ui32Time;
				delayFlag = 1;
				break;
			}
		}
		if(delayFlag==1)
		{
			if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			{
				ui32ElVol = myIt8500Val(&data232[i+3], 0);
				ui32ElCur = myIt8500Val(&data232[i+7], 1);
				break;
			}
		}
		else
		{
			if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			{
				ui32ElVol = -1;
				ui32ElCur = -1;
				break;
			}
		}
	}
}

void  readTti1906Data()
{
	UINT8  i;
	UINT8  delayFlag;
	UINT8  ptr;
	
	int  temp;
	
	for(ptr=0;ptr<C_RS232_RX_BUF_LEN;ptr++)
	    data232[ptr] = 0;
	
	ui32Time = tmrUsTimeGet();
	ui32DelayTime3[ui8Chnl] = ui32Time+500000;
	delayFlag = 0;
	ptr = 0;
	while(1)
	{
		myHwWait2(1);
		while(1)
		{
			temp = fgetchar();
			if(temp==0xffff)
				break;
			data232[ptr++] = temp;
		}
		
		ui32Time = tmrUsTimeGet();
		for(i=0;i<ptr;i++)
		{
			if((data232[i]=='+')&&(data232[i+2]=='.')&&((data232[i+8]=='e')||(data232[i+8]=='E'))&&((data232[i+9]=='+')||(data232[i+9]=='-')))
			{
				if(delayFlag==0)
					ui32DelayTime3[ui8Chnl] = ui32Time+15000;
				if(ptr>=(i+15))
					ui32DelayTime3[ui8Chnl] = ui32Time;
				delayFlag = 1;
				break;
			}
		}
		if(delayFlag==1)
		{
			if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			{
				ui32TtiVol = myTti1906Val(&data232[i]);
				break;
			}
		}
		else
		{
			if((ui32Time>=ui32DelayTime3[ui8Chnl])||((ui32Time<ui32DelayTime3[ui8Chnl])&&((ui32DelayTime3[ui8Chnl]-ui32Time)>600000000)))
			{
				ui32TtiVol = -1;
				break;
			}
		}
	}
}

void  sendPow232Cmd()
{
	#if  (C_POW_DEVICE==C_VID16)
	    F_Set232Port(C_COM1, 9600);
	    
	    fputchar(0x55);
	    fputchar(0x00);
	    fputchar(0x10);
	    fputchar(0x65);
	    
	    readVid16Data();
	    
	    F_Set232Port(C_DEFAULT_COM, 115200);
	    
	    #if  C_RS232_DEBUG
	        UINT8  i;
	        printf("TtiData=");
	        for(i=0;i<64;i++)
	            printf("%02x",data232[i]);
	        printf("  ");
	    #endif
	#endif
}

void  sendEl232Cmd(UINT8 cmd)
{
	#if  (C_EL_DEVICE==C_IT8500)
	    UINT8  i,sum;
	    
	    F_Set232Port(C_COM2, 38400);
	    
	    sum = 0;
	    for(i=0;i<25;i++)
	    {
	    	fputchar(uis8IT8500Cmd[cmd][i]);
	    	sum += uis8IT8500Cmd[cmd][i];
	    }
	    fputchar(sum);
	    
	    if((cmd==C_IT8500_READV)||(cmd==C_IT8500_READA))
	    	readIt8500Data();
	    
	    F_Set232Port(C_DEFAULT_COM, 115200);
	    
	    #if  C_RS232_DEBUG
	        if((cmd==C_IT8500_READV)||(cmd==C_IT8500_READA))
	        {
	        	printf("ElData=");
	        	for(i=0;i<64;i++)
	        	    printf("%02x",data232[i]);
	        	printf("  ");
	        }
	    #endif
	#endif
}

void  sendTti232Cmd(UINT8 cmd)
{
	UINT8  i;
	
	F_Set232Port(C_COM0, 9600);
	
	for(i=0;uis8TTI1906Cmd[cmd][i]!='\n';i++)
	{
	    fputchar(uis8TTI1906Cmd[cmd][i]);
	}
	fputchar(0x0d);
	fputchar(0x0a);
	
	if(cmd==C_TTI1906_READV)
		readTti1906Data();
	
	F_Set232Port(C_DEFAULT_COM, 115200);
	
	#if  C_RS232_DEBUG
	    if(cmd==C_TTI1906_READV)
	    {
	    	printf("TtiData=");
	    	for(i=0;i<64;i++)
	    	    printf("%c",data232[i]);
	    	printf("  ");
	    }
	#endif
}