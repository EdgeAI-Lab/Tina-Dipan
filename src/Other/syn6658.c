/*=============================================================================
#     FileName: TTS.c
#         Desc: Drive the Syn6288  use pc10(tx) pc11(rx) as usart3(afio)
#       Author: Inbol
#        Email: 
#     HomePage: 
#      Version: 2.0
#   LastChange: 2014-03-04 15:13:59
#      History:
=============================================================================*/
#include "stm32f10x_conf.h"
#include "ucos_ii.h"
#include "bsp.h"
#include "crc.h"
#include "memmanage.h"
/* ============================================================================
   Macro defination
   ============================================================================*/
#define USART3_DR (INT32U)(USART3_BASE + 0X04)

/* ============================================================================
   Global variable,,,,,,,
   ============================================================================*/
const unsigned char SYN_PWRDN[5] = {0xfd, 0x00, 0x02, 0x88, 0x77};
const unsigned char SYN_PASUE[5] = {0XFD, 0X00, 0X02, 0X03, 0XFC};
const unsigned char SYN_STOP[5] = {0XFD, 0X00, 0X02, 0X02, 0XFD};
const unsigned char SYN_RECOVER[5] = {0XFD, 0X00, 0X02, 0X04, 0XFB};
const unsigned char SYN_WELCOME[33] = {
	0xFD,0x00,0x1E,0x01,0x01,0xBB,0xB6,0xD3,0xAD,0xB9,0xDB,0xBF,0xB4,0xD3,0xEF,0xD2,0xF4,0xBA,0xCF,0xB3,0xC9,0xCF,0xB5,0xCD,0xB3,0xB5,0xC4,0xD1,0xDD,0xCA,0xBE,0xA3,0xA1};


//extern System robot;
extern OS_EVENT *VoiceSpeakSem;
extern System myRobot;
extern OS_EVENT *VOICE_MBox;
/* ============================================================================*/
/**
 * Description :Init Syn6288 usart
 */
/* ============================================================================*/
void SynGPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,GPIO_Pin_14);
	OSTimeDlyHMSM(0,0,0,25);
	GPIO_SetBits(GPIOE,GPIO_Pin_14);

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD,GPIO_Pin_14);
}
/* ---------------------------------------------------------------------------*/
/**
 * Description :syn6658:外部中断
 */
/* ---------------------------------------------------------------------------*/
void SynEXTISet(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource15);

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

}
/* ============================================================================*/
/**
 * Description :Init Syn6288 chip's interface
 */
/* ============================================================================*/
void SynUsartInit(void)
{
	USART_InitTypeDef Usart_InitStr;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOB, ENABLE);//Afio-->????
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//Afio-->????
	SynGPIOInit();
	SynEXTISet();
	USART_DeInit(USART3);

	Usart_InitStr.USART_BaudRate			= 9600;
	Usart_InitStr.USART_WordLength			= USART_WordLength_8b;
	Usart_InitStr.USART_StopBits			= USART_StopBits_1;
	Usart_InitStr.USART_Parity				= USART_Parity_No;
	Usart_InitStr.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	Usart_InitStr.USART_Mode				= USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(USART3, &Usart_InitStr);
	USART_Cmd(USART3, ENABLE);
	
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);			//Translate use dma
	
	DMA_DeInit(DMA1_Channel2);
}


/* ============================================================================*/
/**
 * Description :中文语音数据发送
 *
 * Input    :buf-->数据首地址
 */
/* ============================================================================*/
//void SynSendData(const unsigned char *buf)
//{
//	unsigned char count;
//	for(count=0;count<buf[2]+3;count++)
//	{
//		USART_SendData(USART3, buf[count]);
//		/* Loop until USARTy DR register is empty */ 
//		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
//		{
//		}
//	}
//}
void Syn_SendSetDMA(const unsigned char *buf)
{
	DMA_InitTypeDef DMA_InitStr;
	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_InitStr.DMA_PeripheralBaseAddr	= (uint32_t)(&((USART_TypeDef*)USART3)->DR);
	DMA_InitStr.DMA_MemoryBaseAddr		= (unsigned long)buf;
	DMA_InitStr.DMA_DIR					= DMA_DIR_PeripheralDST;
	DMA_InitStr.DMA_BufferSize			= buf[2]+3;
	DMA_InitStr.DMA_PeripheralInc		= DMA_PeripheralInc_Disable;
	DMA_InitStr.DMA_MemoryInc			= DMA_MemoryInc_Enable;
	DMA_InitStr.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
	DMA_InitStr.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
	DMA_InitStr.DMA_Mode				= DMA_Mode_Normal;
	DMA_InitStr.DMA_Priority			= DMA_Priority_High;
	DMA_InitStr.DMA_M2M					= DMA_M2M_Disable;

	DMA_Init(DMA1_Channel2, &DMA_InitStr);
	DMA_Cmd(DMA1_Channel2, ENABLE);
}
/*
 *********************************************************************************************************
 * 函数名：获取中文语音芯片状态
 * 函数原型：unsigned char BSP_GetSYNStatus(void)
 * 入口参数：无
 * 出口参数：
 * 函数说明：
 *********************************************************************************************************
 */
unsigned char GetSynStatus(void)
{
	return GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15);
}


/*
 *********************************************************************************************************
 *                            void	TTS_SetVol(void)
 *
 * 描述			: 设置中文语音模块音量
 *
 * 调用函数		：中文语音任务开始时调用
 *
 * 入口参数		: 无
 *
 * 出口参数		: 无
 *********************************************************************************************************
 */
//void SynSetVol(void)
//{
//	unsigned char *buf;
//	unsigned char err;
//	OS_SEM_DATA sem_data;
//	//初始化SYN和VS1003器件
//	buf = (unsigned char *)GetMem(32, &err);
//	buf[0] = 0XFD;
//	buf[1] = 0X00;
//	buf[3] = 0x01;
//	buf[4] = 0x00;
//	sprintf((char *)(&buf[5]), "[v%d]", robot.voicevol);
//	if (robot.voicevol > 9)
//		buf[2] = 8;
//	else
//		buf[2] = 7;
//	buf[buf[2] + 2] = XOR_Generation(buf, buf[2] + 2);

//	//开启语音播放完毕中断
//	SynEXTISet();
//	SynSendData(buf);

//	OSTimeDlyHMSM(0, 0, 0, 500);

//	if(GetSynStatus()) 
//		OSSemPend(VoiceSpeakSem, 0, &err);
//	else
//	{
//		OSSemQuery(VoiceSpeakSem, &sem_data);
//		if (sem_data.OSCnt)
//		{
//			OSSemPend(VoiceSpeakSem, 0, &err);
//		}
//	}

//	PutMem((void *)buf, 32);

//	robot.voicecom.busyflg = 0;

//	SynEXTIReset();
//}
void say(const unsigned char * tempBuff)
{
	INT8U err;
	unsigned char length;
	unsigned char * voicep;
	length=strlen(tempBuff);
	
	voicep=(unsigned char *)GetMem(length+10 ,&err);
	voicep[2]=length+3;
	memcpy(voicep+5,tempBuff,length);
	OSMboxPost(VOICE_MBox, voicep);
}
/*
*********************************************************************************************************
* 函数名：语音播放任务
* 函数原型：static	void	App_Voice(void *pdata)
* 入口参数：
*			pdata：传递的参数
* 出口参数：无
* 函数说明：
*********************************************************************************************************
*/
void App_Voice(void *pdata)
{
	INT8U err;
	INT8U *tmpptr;
	(void)pdata;
	while (1)
	{
		tmpptr = OSMboxPend(VOICE_MBox, 0, &err);
		if (err == OS_ERR_NONE)
		{
			while(GetSynStatus())
			{
				OSTimeDlyHMSM(0, 0, 0, 10);
			}
			tmpptr[0] = 0xfd;
			tmpptr[1] = 0x00;
			tmpptr[3] = 0x01;
			tmpptr[4] = 0x00;
			tmpptr[tmpptr[2] + 2] = XOR_Generation((INT8U *)(tmpptr), tmpptr[2] + 2);
			Syn_SendSetDMA((INT8U *)tmpptr);
			OSTimeDlyHMSM(0, 0, 2, 0);
			PutMem(tmpptr, tmpptr[2] + 3);
		}
	}
}