/*=============================================================================
#     FileName: ld3320.c
#         Desc: 
#       Author: Lyn
#        Email: 
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2015-04-08 17:55:55
#      History:
=============================================================================*/

#include "stm32f10x_conf.h"
#include "ucos_ii.h"
#include "bsp.h"
#include "ld3320.h"

extern System myRobot;

/* ============================================================================*/
/**
 * Description :Init Syn6288 usart
 */
/* ============================================================================*/
void LdGPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//SPI接口
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	//GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//LD3320 rst设置
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	OSTimeDlyHMSM(0,0,0,25);
	GPIO_ResetBits(GPIOD,GPIO_Pin_6);
	OSTimeDlyHMSM(0,0,0,25);
	GPIO_SetBits(GPIOD,GPIO_Pin_6);

	//LD3320卡CS设置
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD,GPIO_Pin_7);
	OSTimeDlyHMSM(0,0,0,25);
	GPIO_SetBits(GPIOD,GPIO_Pin_7);

	//LD3320 INT设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
void LdEXTISet(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource5);

	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

}
void LdSPIInit(void)
{
	SPI_InitTypeDef	SPI_InitStr;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	LdGPIOInit();
	LdEXTISet();
	SPI_Cmd(SPI3, DISABLE);
	SPI_InitStr.SPI_Direction			= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStr.SPI_Mode				= SPI_Mode_Master;
	SPI_InitStr.SPI_DataSize			= SPI_DataSize_8b;
	SPI_InitStr.SPI_NSS					= SPI_NSS_Soft;
	SPI_InitStr.SPI_FirstBit			= SPI_FirstBit_MSB;

	SPI_InitStr.SPI_CPOL				= SPI_CPOL_High;
	SPI_InitStr.SPI_CPHA				= SPI_CPHA_1Edge;
	SPI_InitStr.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_4;			
	SPI_InitStr.SPI_CRCPolynomial		= 7;

	SPI_Init(SPI3, &SPI_InitStr);

	SPI_Cmd(SPI3, ENABLE);
}
unsigned char LD_SPITR(unsigned char data)
{
	//等待发送缓冲区空
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	//发一个字节
	SPI_I2S_SendData(SPI3, data);

	//等待数据接收
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	//取数据
	return SPI_I2S_ReceiveData(SPI3);
}
void LD_WriteReg(unsigned char address,unsigned char regTemp)
{
	GPIO_ResetBits(GPIOD,GPIO_Pin_7);
	// 这里添加硬件 SPI 口的操作代码：
	LD_SPITR(0x04); // 发送 0x04
	LD_SPITR(address); // 发送 address
	LD_SPITR(regTemp); // 发送 dataout
	GPIO_SetBits(GPIOD,GPIO_Pin_7);
}
unsigned char LD_ReadReg( unsigned char address )
 {
	 // 这里添加硬件 SPI 口的操作代码：
	 unsigned char temp;
	 GPIO_ResetBits(GPIOD,GPIO_Pin_7);
	 LD_SPITR(0x05); // 发送 0x05
	 LD_SPITR(address); // 发送 address
	 temp=LD_SPITR(0);
	 GPIO_SetBits(GPIOD,GPIO_Pin_7);
	 return temp; // 读出数据, 并返回
	 
 }
void delay(unsigned char count)
 {
	 unsigned short i=65535;
	 while(count--)
	 {
		 while(i--);
	 }
 }
void LD_Init_Common(void)
{
	//bMp3Play = 0;
	LD_ReadReg(0x06);
	LD_WriteReg(0x17, 0x35);
	delay(10);
	LD_ReadReg(0x06);
	LD_WriteReg(0x89, 0x03);
	delay(5);
	LD_WriteReg(0xCF, 0x43);
	delay(5);
	LD_WriteReg(0xCB, 0x02);
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);
	if (myRobot.ld3320State.mode == LD_MODE_MP3)
	{
		LD_WriteReg(0x1E, 0x00);
		//!! 注意，下面三个寄存器，会随晶振频率变化而设置不同
		//!! 注意, 请根据使用的晶振频率修改参考程序中的 CLK_IN
		LD_WriteReg(0x19, LD_PLL_MP3_19);
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);
		LD_WriteReg(0x1D, LD_PLL_MP3_1D); }
	else
	{
		//
		LD_WriteReg(0x1E,0x00);
		//!! 注意，下面三个寄存器，会随晶振频率变化而设置不同
		//!! 注意, 请根据使用的晶振频率修改参考程序中的 CLK_IN
		LD_WriteReg(0x19, LD_PLL_ASR_19);
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);
		LD_WriteReg(0x1D, LD_PLL_ASR_1D);
	}
	LD_WriteReg(0xCD, 0x04);
	LD_WriteReg(0x17, 0x4c);
	delay(5);
	LD_WriteReg(0xB9, 0x00);
	LD_WriteReg(0xCF, 0x4f);
	LD_WriteReg(0x6F, 0xFF);
}	
