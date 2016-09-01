#include "ld3320.h"
#include "bsp.h"
#include "ff.h"
#include "stm32f10x_conf.h"
extern System myRobot;
extern OS_EVENT *Mp3TransSem,*Mp3PlaySem;
extern FIL musicf;
//播放初始化

//音量调整
void LD_AdjustMIX2SPVolume(unsigned char val)	  
{
	val = ((15-val)&0x0f) << 2;	
	LD_WriteReg(0x8E, val | 0xc3); 
	LD_WriteReg(0x87, 0x78); 
}
void LD_Init_MP3(void)	
{
	myRobot.ld3320State.mode = LD_MODE_MP3;	   //当前进行MP3播放
	LD_Init_Common();		  		 //通用初始化

	LD_WriteReg(0xBD,0x02);	 		 	//内部增益控制 初始化时写入FFH
	LD_WriteReg(0x17, 0x48);				//写48H可以激活DSP
		delay(10);

	LD_WriteReg(0x85, 0x52); 	//内部反馈设置 初始化时写入52H
	LD_WriteReg(0x8F, 0x00);  	//LineOut(线路输出)选择 初始化时写入00H
	LD_WriteReg(0x81, 0x00);		//耳机左音量 设置为00H为最大音量
	LD_WriteReg(0x83, 0x00);		//耳机右音量 设置为00H为最大音量
	LD_WriteReg(0x8E, 0xff);		//喇叭输出音量  本寄存器设置为00H为最大音量	此处声音关闭
	LD_WriteReg(0x8D, 0xff);		//内部增益控制 初始化时写入FFH
    delay(1);
	LD_WriteReg(0x87, 0xff);	   //模拟电路控制 MP3播放初始化时写 FFH
	LD_WriteReg(0x89, 0xff);    //模拟电路控制 MP3播放时写 FFH
		delay(1);
	LD_WriteReg(0x22, 0x00);    //FIFO_DATA下限低8位
	LD_WriteReg(0x23, 0x00);		 //FIFO_DATA下限高8位
	LD_WriteReg(0x20, 0xef);    //FIFO_DATA上限低8位
	LD_WriteReg(0x21, 0x07);		 //FIFO_DATA上限高8位
	LD_WriteReg(0x24, 0x77);        
	LD_WriteReg(0x25, 0x03);	
	LD_WriteReg(0x26, 0xbb);    
	LD_WriteReg(0x27, 0x01); 	
	LD_AdjustMIX2SPVolume(1);
}
void App_MusicTask(void *pdata)
{
	unsigned char ucStatus,tempMode;
	FRESULT	result;
	INT16U byteread, alreadycnt;
	INT8U err,cnt;
	INT8U *buf;
	OS_SEM_DATA	tmpsemdata;
	(void)pdata;
#if (OS_TASK_STAT_EN > 0)
	OSStatInit();                                               /* Determine CPU capacity.                              */
#endif

	while (1) {                                          /* Task body, always written as an infinite loop.       */
		OSSemPend(Mp3TransSem, 0, &err);
		tempMode=myRobot.ld3320State.mode;
		myRobot.ld3320State.mode=LD_MODE_MP3;
		myRobot.ld3320State.mp3State = MP3_PLAY_CONTINUE;
		LD_Init_MP3();				//对芯片进行MP3模式初始化
		fill_the_fifo();
		LD_WriteReg(0xBA, 0x00);	  //中断辅助信息，（读或设为00
		LD_WriteReg(0x17, 0x48);	  //写48H可以激活DSP
		LD_WriteReg(0x33, 0x01);	  //MP3播放用设置 开始播放时写入01H, 播放完写入00H
		LD_WriteReg(0x29, 0x04);	  //中断允许 第2位：FIFO 中断允许 第4位：同步中断允许 1允许；0不允许
		LD_WriteReg(0x02, 0x01); 	  //FIFO中断允许 第0位：允许FIFO_DATA中断； 第2位：允许FIFO_EXT中断；
		LD_WriteReg(0x85, 0x5A);	  //内部反馈设置 初始化时写入52H 播放MP3时写入5AH (改变内部增益)
		EXTI_ClearITPendingBit(EXTI_Line5);
		
		
		ucStatus = LD_ReadReg(0x06);//读取FIFO状态
		
		buf = (INT8U *)GetMem(512,&err);
		sprintf((char *)buf, "0:/music/%s", myRobot.ld3320State.filename);

		result = f_open(&musicf, (const char *)buf, FA_OPEN_EXISTING | FA_READ);
		if (result == FR_OK)
		{
			do 
			{
				result = f_read(&musicf, (INT8U *)buf, 512, (UINT *)&byteread);
				if (result == FR_OK)
				{
					alreadycnt = 0;
					do 
					{
						switch (myRobot.ld3320State.mp3State)
						{
							case MP3_PAUSE_PLAY:
								OSTimeDlyHMSM(0, 0, 0, 500);//??500ms????,????????...
								break;
							case MP3_NEW_PLAY:
							case MP3_NO_PLAY:
								alreadycnt = 512;
								byteread = 100;
								break;
							case MP3_PLAY_CONTINUE:
								
								ucStatus = LD_ReadReg(0x06);
								//fifo是否满了
								while ((LD_ReadReg(0x06)&MASK_FIFO_STATUS_AFULL));
								{
									alreadycnt++;
									LD_WriteReg(0x01,buf[alreadycnt]);
									
								}
								break;
							default:
								break;
						}

					} while ((alreadycnt < byteread));

					if (myRobot.ld3320State.mp3State != MP3_PLAY_CONTINUE)
					{
						myRobot.ld3320State.mode=tempMode;
						myRobot.ld3320State.state=LD_ASR_NONE;
						break;
					}
					//OSTimeDlyHMSM(0,0,0,2);
				}
			} while (byteread == 512);
			//MP3_End();
			
			LD_WriteReg(0xBC, 0x01);
			LD_WriteReg(0x29, 0x10);
	//		while(!(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END));
	//		myRobot.ld3320State.mode=LD_MODE_ASR_RUN;
	//		myRobot.ld3320State.state=LD_ASR_NONE;
			f_close(&musicf);
		}//if (result == FR_OK)
		PutMem(buf, 512);
	}
}




