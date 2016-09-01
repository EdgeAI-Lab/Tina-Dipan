/*=============================================================================
#     FileName: simulation_i2c_eeprom.c
#         Desc: 
#       Author: Lyn
#        Email: 
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2015-04-08 14:06:10
#      History:
=============================================================================*/
#include "includes.h"
void Systick_Delay_1ms(unsigned long nCount)
{
	OSTimeDlyHMSM(0,0,0,1);
}

//void I2C_WP_(void)

void EEPROM_Config(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	/* Configure I2C1 pins: SCL and SDA */  
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void I2C_delay(void)
{	
	unsigned char i=100; //unsigned char i=100; //这里可以优化速度	，经测试最低到5还能写入
	while(i) 
	{ 
		i--; 
	} 
}

unsigned char I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return 0;	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return 0;	//SDA线为高电平则总线出错,退出
	SDA_L;
	I2C_delay();
	return 1;
}

void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

unsigned char I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
		SCL_L;
		return 0;
	}
	SCL_L;
	return 1;
}

void I2C_SendByte(unsigned char SendByte) //数据从高位到低位//
{
	unsigned char i=8;
	while(i--)
	{
		SCL_L;
		I2C_delay();
		if(SendByte&0x80)
			SDA_H;  
		else 
			SDA_L;   
		SendByte<<=1;
		I2C_delay();
		SCL_H;
		I2C_delay();
	}
	SCL_L;
}

unsigned char I2C_ReceiveByte(void)  //数据从高位到低位//
{ 
	unsigned char i=8;
	unsigned char ReceiveByte=0;

	SDA_H;				
	while(i--)
	{
		ReceiveByte<<=1;      
		SCL_L;
		I2C_delay();
		SCL_H;
		I2C_delay();	
		if(SDA_read)
		{
			ReceiveByte|=0x01;
		}
	}
	SCL_L;
	return ReceiveByte;
}

//写入1字节数据       待写入数据    待写入地址       器件类型(24c16或SD2403)
unsigned char I2C_WriteByte(unsigned char SendByte, unsigned short WriteAddress, unsigned char DeviceAddress)
{		
	if(!I2C_Start())return 0;

	I2C_SendByte(DeviceAddress & 0xFE);//器件地址 
	if(!I2C_WaitAck()){I2C_Stop(); return 0;}
	I2C_SendByte((unsigned char)((WriteAddress & 0x1F00) >>8));   //设置高起始地址     
	if(!I2C_WaitAck()){I2C_Stop(); return 0;}
	I2C_SendByte((unsigned char)(WriteAddress & 0x00FF));   //设置低起始地址

	I2C_WaitAck();	
	I2C_SendByte(SendByte);
	I2C_WaitAck();   
	I2C_Stop(); 
	//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
	Systick_Delay_1ms(1);
	return 1;
}

//注意不能跨页写
//写入1串数据      待写入数组地址    待写入长度      待写入地址       器件类型(24c16或SD2403)
unsigned char I2C_BufferWrite(unsigned char* pBuffer, unsigned char length,     unsigned short WriteAddress, unsigned char DeviceAddress)
{
	if(!I2C_Start())return 0;

	I2C_SendByte(DeviceAddress & 0xFE);//器件地址 
	if(!I2C_WaitAck()){I2C_Stop(); return 0;}
	I2C_SendByte((unsigned char)((WriteAddress & 0x1F00) >>8));   //设置高起始地址     
	if(!I2C_WaitAck()){I2C_Stop(); return 0;}
	I2C_SendByte((unsigned char)(WriteAddress & 0x00FF));   //设置低起始地址

	I2C_WaitAck();	

	while(length--)
	{
		I2C_SendByte(* pBuffer);
		I2C_WaitAck();
		pBuffer++;
	}
	I2C_Stop();
	//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
	Systick_Delay_1ms(1);
	return 1;
}
//读出1串数据         存放读出数据  待读出长度      待读出地址       器件类型(24c16或SD2403)	
unsigned char I2C_ReadByte(unsigned char* pBuffer,   unsigned char length,     unsigned short ReadAddress,  unsigned char DeviceAddress)
{		
	if(!I2C_Start())return 0;

	I2C_SendByte((DeviceAddress & 0xFE));//器件地址 
	if(!I2C_WaitAck()){I2C_Stop(); return 0;}
	I2C_SendByte((unsigned char)((ReadAddress & 0x1F00) >>8));   //设置高起始地址     
	if(!I2C_WaitAck()){I2C_Stop(); return 0;}
	I2C_SendByte((unsigned char)(ReadAddress & 0x00FF));   //设置低起始地址

	I2C_WaitAck();
	I2C_Start();

	I2C_SendByte((DeviceAddress & 0xFE)|0x01);//器件地址 
   
	I2C_WaitAck();
	while(length)
	{
		*pBuffer = I2C_ReceiveByte();
		if(length == 1)I2C_NoAck();
		else I2C_Ack(); 
		pBuffer++;
		length--;
	}
	I2C_Stop();
	return 1;
}
void writeInformation()
{
	I2C_BufferWrite("Inbol", 5, 0, 0xA0);
	I2C_BufferWrite("Lyn", 3, 5, 0xA0);
}
/******************* (C) COPYRIGHT 2011 Tiamaes *****END OF FILE****/

