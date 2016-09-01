#include "ucos_ii.h"
#include "ff.h"
#include "ld3320.h"
#include "bsp.h"
#include "stm32f10x_conf.h"
extern System myRobot;
extern const unsigned char myName[],myPlace[],yourName[];
extern FIL lis,musicf, actionf, dancef, eyebrowf, eyelisf;//
extern OS_EVENT *Mp3TransSem;				//MP3信号量
extern OS_EVENT *ActionSem;	
static void LD_Init_ASR(void)
{
	LD_Init_Common();

	LD_WriteReg(0xBD, 0x00);
	LD_WriteReg(0x17, 0x48);	
	delay(5);
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
	LD_WriteReg(0x40, 0);          
	LD_WriteReg(0x42, 8);
	LD_WriteReg(0x44, 0);    
	LD_WriteReg(0x46, 8); 
	delay(1);
}

void ProcessInt(void)
{
	unsigned char nAsrResCount=0;
	unsigned char ucRegVal;
	
	unsigned char ucHighInt;
	unsigned char ucLowInt;
	ucRegVal = LD_ReadReg(0x2B);

	if(myRobot.ld3320State.mode==LD_MODE_ASR_RUN)
	{
		//语音识别产生的中断
		//有声音输入，不论是别成功或失败都有中断
		LD_WriteReg(0x29,0) ;
		LD_WriteReg(0x02,0) ;

		if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
		{	 
				nAsrResCount = LD_ReadReg(0xba);

				if(nAsrResCount>0 && nAsrResCount<=4) 
				{
					myRobot.ld3320State.state=LD_ASR_FOUNDOK; 		
				}
				else
				{
					myRobot.ld3320State.state=LD_ASR_FOUNDZERO;
				}	
		}
		else
		{
			myRobot.ld3320State.state=LD_ASR_FOUNDZERO;
		}

		LD_WriteReg(0x2b,0);
		LD_WriteReg(0x1C,0);
		return;
		
	}
	
	
	
	
	
	
	
	ucHighInt = LD_ReadReg(0x29); 
	ucLowInt=LD_ReadReg(0x02); 
	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;
    if(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END)
    {
	// A. 声音数据全部播放完毕

			LD_WriteReg(0x2B,  0);
			LD_WriteReg(0xBA, 0);	
			LD_WriteReg(0xBC,0x0);	
			
		
			LD_WriteReg(0x08,1);

			LD_WriteReg(0x08,0);
			LD_WriteReg(0x33, 0);
		
			myRobot.ld3320State.mode=LD_MODE_ASR_RUN;
			myRobot.ld3320State.state=LD_ASR_NONE;
		
			return ;
    }

//	if(nMp3Pos>=nMp3Size)
//	{
//	// B. ÉùÒôÊý¾ÝÒÑ·¢ËÍÍê±Ï¡£

//		LD_Write(0xBC, 0x01);
//		LD_Write(0x29, 0x10);
//		f_close(&fsrc);
//		nLD_Mode = LD_MODE_ASR_RUN;
//		nAsrStatus = LD_ASR_NONE;
//		return;	
//	}
	LD_WriteReg(0x29,ucHighInt); 
	LD_WriteReg(0x02,ucLowInt) ;
}

static unsigned char LD_Check_ASRBusyFlag_b2(void)
{
	unsigned char j;
	unsigned char temp=0;
	unsigned char flag = 0;
	for (j=0; j<10; j++)
	{
		temp=LD_ReadReg(0xb2);
		if (temp == 0x21)
		{
			flag = 1;
			break;
		}
		delay(10);		
	}
	return flag;
}
static unsigned char LD_AsrAddFixed(void)
{
	unsigned char k, flag;
	unsigned char nAsrAddLength;
	#define DATE_A 12    //数组二维数值
	#define DATE_B 50		//数组以为数值
	//添加关键词
	unsigned char  sRecog[DATE_A][DATE_B] = {
	 			"ni shi shui",	\
				"ni zai na li",		\
				"wo shi shui",	\
				"xuan zhuan",			\
				"ting zhi",		\
				"chang shou ge",		\
				"xiang qian zou",		\
				"xiang hou tui",		\
				"yao tou",		\
				"niu yao",		\
				"zuo ge bo",		\
				"you ge bo",		\
		};	
	unsigned char  pCode[DATE_A] = {
									1,	\
									2,	\
									3,	\
									4,	\
									5,	\
									6,	\
		7,	\
		8,	\
		9,	\
		10,	\
		11,	\
		12,	\
									};	
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}

		LD_WriteReg(0xc1, pCode[k] );
		LD_WriteReg(0xc3, 0);
		LD_WriteReg(0x08, 0x04);
		delay(1);
		LD_WriteReg(0x08, 0x00);
		delay(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD_WriteReg(0xb9, nAsrAddLength);
		LD_WriteReg(0xb2, 0xff);
		LD_WriteReg(0x37, 0x04);
	}	 
	return flag;
}
static unsigned char LD_AsrRun(void)		
{
	LD_WriteReg(0x35, MIC_VOL);	   //麦克风音量
//	LD_Write(0xB3, 0x0D);	//
	LD_WriteReg(0x1C, 0x09);	//ADC开关控制，写09H Reserve 保留命令字
	LD_WriteReg(0xBD, 0x20);	//初始化控制寄存器 写入20H；
	LD_WriteReg(0x08, 0x01);	//清除FIFO内容第0位：写入->清除FIFO_DATA第二位
	delay( 1 );
	LD_WriteReg(0x08, 0x00);	//
	delay( 1 );

	if(LD_Check_ASRBusyFlag_b2() == 0)	 
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	  //
	LD_WriteReg(0x37, 0x06);	  //
	delay( 5 );
	//LD_TEST();
	LD_WriteReg(0x1C, 0x0b);	  //
	LD_WriteReg(0x29, 0x10);	  //
	
	LD_WriteReg(0xBD, 0x00);	  //
	//EX0=1;		  //
	EXTI_ClearITPendingBit(EXTI_Line5);
	return 1;
}
static void LD_Reset(void)
{
	GPIO_SetBits(GPIOD,GPIO_Pin_6);
	OSTimeDlyHMSM(0,0,0,25);
	GPIO_ResetBits(GPIOD,GPIO_Pin_6);
	OSTimeDlyHMSM(0,0,0,25);
	GPIO_SetBits(GPIOD,GPIO_Pin_6);
}
//启动一次识别
unsigned char RunASR(void)
{
	unsigned char i=0;
	unsigned char j=0;
	unsigned char asrflag=0;
	for (i=0; i<5; i++)	//
	{
		LD_Init_ASR();
		OSTimeDly(1);	
		if (LD_AsrAddFixed()==0)
		{
			LD_Reset();			//
			OSTimeDly(1);		//
			continue;
		}
		j= LD_AsrRun();
		if (j == 0)
		{
			LD_Reset();			//
			OSTimeDly(1);		//
			continue;
		}
		asrflag=1;
		break;					//
	}	
	return asrflag;
}
static unsigned char LD_GetResult(void)
{
	unsigned char res;
	res = LD_ReadReg(0xc5);
	return res;
}
void App_ASRTask(void *pdata)
{
	unsigned char tempResult;
	unsigned char err;
	unsigned short temp;
#if OS_CRITICAL_METHOD == 3/* Allocate storage for CPU status register      */
	OS_CPU_SR  cpu_sr = 0;
#endif
	(void)pdata;
	while (1)
	{
		if(myRobot.ld3320State.mode==LD_MODE_ASR_RUN)
		{			
			switch(myRobot.ld3320State.state)
			{
				case LD_ASR_RUNING:
					break;
				case LD_ASR_ERROR:		
					break;
				case LD_ASR_NONE:
				{
					myRobot.ld3320State.state=LD_ASR_RUNING;
					if (RunASR()==0)															
					{
						myRobot.ld3320State.state = LD_ASR_ERROR;
					
					}
					break;						 
				}
				case LD_ASR_FOUNDOK:
				{
		
					tempResult = LD_GetResult();											
					switch(tempResult)
					{
						case 0: 	
							break;
						case 1:
							say(myName);
							break;
						case 2:
							say(myPlace);
							break;
						case 3:
							say(yourName);
							break;
						case 4: 
							motorSpeed(20,1,1);
							break;
						case 5: 
							Stop(1);
							break;
						case 6:
							err = f_open(&lis, MUSICLIS_DIR, FA_OPEN_EXISTING | FA_READ );
							if (err == FR_OK)
							{
								//打开成功，开始查找文件
								err = SeekFile(&lis, 47, myRobot.ld3320State.filename, &temp);//查找列表文件,得到文件名,并将其保存至filename中
								if (err == 0)//没有错误就改变音乐播放状态,并发送信号量,表示可以播放.
								{
									myRobot.ld3320State.mp3State = MP3_NEW_PLAY;

									OSSemPost(Mp3TransSem); //再次添加信号量
								}//
								
								//关闭文件
								f_close(&lis);
							
							}
							break;
						case 7: 
							err = f_open(&lis, ACTION_DIR, FA_OPEN_EXISTING | FA_READ );
							if (err == FR_OK)
							{
								err = SeekFile(&lis, 3, myRobot.actionctrl.filename, &temp);
								if (err == 0)
								{
									myRobot.actionctrl.status |= ACTION_START;
									if (temp & ACTION_BASIC)	//如果是基本动作，则将基本动作标志置位
										myRobot.actionctrl.status |= ACTION_BASIC;
									else
										myRobot.actionctrl.status &= ~ACTION_BASIC;
									f_close(&lis);
									OSSemPost(ActionSem);
									//在此添加信号量
								}
							}
							break;
						case 8:	
							err = f_open(&lis, ACTION_DIR, FA_OPEN_EXISTING | FA_READ );
							if (err == FR_OK)
							{
								err = SeekFile(&lis, 4, myRobot.actionctrl.filename, &temp);
								if (err == 0)
								{
									myRobot.actionctrl.status |= ACTION_START;
									if (temp & ACTION_BASIC)	//如果是基本动作，则将基本动作标志置位
										myRobot.actionctrl.status |= ACTION_BASIC;
									else
										myRobot.actionctrl.status &= ~ACTION_BASIC;
									f_close(&lis);
									OSSemPost(ActionSem);
									//在此添加信号量
								}
							}
							break;
						case 9:
							err = f_open(&lis, ACTION_DIR, FA_OPEN_EXISTING | FA_READ );
							if (err == FR_OK)
							{
								err = SeekFile(&lis, 7, myRobot.actionctrl.filename, &temp);
								if (err == 0)
								{
									myRobot.actionctrl.status |= ACTION_START;
									if (temp & ACTION_BASIC)	//如果是基本动作，则将基本动作标志置位
										myRobot.actionctrl.status |= ACTION_BASIC;
									else
										myRobot.actionctrl.status &= ~ACTION_BASIC;
									f_close(&lis);
									OSSemPost(ActionSem);
									//在此添加信号量
								}
							}
							break;
						case 10:
							err = f_open(&lis, ACTION_DIR, FA_OPEN_EXISTING | FA_READ );
							if (err == FR_OK)
							{
								err = SeekFile(&lis, 8, myRobot.actionctrl.filename, &temp);
								if (err == 0)
								{
									myRobot.actionctrl.status |= ACTION_START;
									if (temp & ACTION_BASIC)	//如果是基本动作，则将基本动作标志置位
										myRobot.actionctrl.status |= ACTION_BASIC;
									else
										myRobot.actionctrl.status &= ~ACTION_BASIC;
									f_close(&lis);
									OSSemPost(ActionSem);
									//在此添加信号量
								}
							}
							break;
						case 11:
							err = f_open(&lis, ACTION_DIR, FA_OPEN_EXISTING | FA_READ );
							if (err == FR_OK)
							{
								err = SeekFile(&lis, 32, myRobot.actionctrl.filename, &temp);
								if (err == 0)
								{
									myRobot.actionctrl.status |= ACTION_START;
									if (temp & ACTION_BASIC)	//如果是基本动作，则将基本动作标志置位
										myRobot.actionctrl.status |= ACTION_BASIC;
									else
										myRobot.actionctrl.status &= ~ACTION_BASIC;
									f_close(&lis);
									OSSemPost(ActionSem);
									//在此添加信号量
								}
							}
							break;
						case 12:
							err = f_open(&lis, ACTION_DIR, FA_OPEN_EXISTING | FA_READ );
							if (err == FR_OK)
							{
								err = SeekFile(&lis, 33, myRobot.actionctrl.filename, &temp);
								if (err == 0)
								{
									myRobot.actionctrl.status |= ACTION_START;
									if (temp & ACTION_BASIC)	//如果是基本动作，则将基本动作标志置位
										myRobot.actionctrl.status |= ACTION_BASIC;
									else
										myRobot.actionctrl.status &= ~ACTION_BASIC;
									f_close(&lis);
									OSSemPost(ActionSem);
									//在此添加信号量
								}
							}
							break;
						case 13:
							break;
						case 14:
							break;
						case 15:
							break;
						case 16:
							break;
						case 17:
							break;
						case 18:
							break;
						case 19:
							break;
						default:
							break;
					}
					myRobot.ld3320State.state = LD_ASR_NONE;
					break;
				}
				case LD_ASR_FOUNDZERO:
				default:
					myRobot.ld3320State.state = LD_ASR_NONE;
					break;
			}
		}
	}
}
