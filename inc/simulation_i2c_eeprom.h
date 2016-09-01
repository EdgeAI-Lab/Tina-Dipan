/*=============================================================================
#     FileName: simulation_i2c_eeprom.h
#         Desc: 
#       Author: Lyn
#        Email: 
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2015-04-08 15:08:09
#      History:
=============================================================================*/

#ifndef __I2C_Driver_H
#define __I2C_Driver_H

/* Includes ------------------------------------------------------------------*/

//#include u8  unsigned char
//#include u16 unsigned int
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/**                                              24C02������ַ���
 **                                              1 0 1 0 A2 A1 A0 R/W
 **                                              R/W=1: READ
 **                                              R/W=0: WRITE
 */

/*******************************************************************/

#define TWOBYTEADD

#define SCL_H          ( GPIOE->BSRR = GPIO_Pin_2 )
#define SCL_L          ( GPIOE->BRR  = GPIO_Pin_2 ) 

#define SDA_H          ( GPIOE->BSRR = GPIO_Pin_3 )
#define SDA_L          ( GPIOE->BRR  = GPIO_Pin_3 )

#define SCL_read       ( GPIOE->IDR  & GPIO_Pin_2 )
#define SDA_read       ( GPIOE->IDR  & GPIO_Pin_3 )
/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

void EEPROM_Config(void);

void Systick_Delay_1ms(unsigned long nCount);

//д��1�ֽ�����     ��д������    ��д���ַ     ��������(24c16��SD2403)
unsigned char I2C_WriteByte(unsigned char SendByte,unsigned short WriteAddress,unsigned char DeviceAddress);

//ע�ⲻ�ܿ�ҳд
//д��1������      ��д�������ַ ��д�볤��  ��д���ַ    ��������(24c16��SD2403)
unsigned char I2C_BufferWrite(unsigned char* pBuffer,unsigned char length,unsigned short WriteAddress,unsigned char DeviceAddress);

//��ҳд��1������  ��д�������ַ ��д�볤��  ��д���ַ   ��������(24c16��SD2403)
void I2C_PageWrite(unsigned char* pBuffer,unsigned char length,unsigned short WriteAddress,unsigned char DeviceAddress);
//����1������     ��Ŷ�������  ����������   ��������ַ  ��������(24c16��SD2403)
unsigned char I2C_ReadByte(unsigned char* pBuffer,unsigned char length,unsigned short ReadAddress,unsigned char DeviceAddress);

extern void Delays_1ms(unsigned int n)	;
void writeInformation(void);
#endif 
/******************* (C) COPYRIGHT 2011 Tiamaes *****END OF FILE****/
