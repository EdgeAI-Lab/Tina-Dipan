/************************************************************************************
*  Copyright (c), 2014, HelTec Automatic Technology co.,LTD.
*            All rights reserved.
*
* Http:    www.heltec.cn
* Email:   cn.heltec@gmail.com
* WebShop: heltec.taobao.com
*
* File name: OLED.c
* Project  : HelTec.uvprij
* Processor: STM32F103C8T6
* Compiler : MDK fo ARM
* 
* Author : 小林
* Version: 1.00
* Date   : 2014.2.20
* Email  : hello14blog@gmail.com
* Modification: none
* 
* Description:128*64点阵的OLED显示屏驱动文件，仅适用于惠特自动化(heltec.taobao.com)的SD1306驱动SPI通信方式显示屏
*
* Others: none;
*
* Function List:
*
* 2. void OLED_WrDat(unsigned char dat) -- 向OLED写数据
* 3. void OLED_WrCmd(unsigned char cmd) -- 向OLED写命令
* 4. void OLED_SetPos(unsigned char x, unsigned char y) -- 设置起始点坐标
* 5. void OLED_Fill(unsigned char bmp_dat) -- 全屏填充(0x00可以用于清屏，0xff可以用于全屏点亮)
* 6. void OLED_CLS(void) -- 清屏
* 7. void OLED_Init(void) -- OLED显示屏初始化
* 8. void OLED_6x8Str(unsigned char x, y,unsigned char ch[]) -- 显示6x8的ASCII字符
* 9. void OLED_8x16Str(unsigned char x, y,unsigned char ch[]) -- 显示8x16的ASCII字符
* 10.void OLED_16x16CN(unsigned char x, y, N) -- 显示16x16中文汉字,汉字要先在取模软件中取模
* 11.void OLED_BMP(unsigned char x0, y0,x1, y1,unsigned char BMP[]) -- 全屏显示128*64的BMP图片
*
* History: none;
*
*************************************************************************************/

#include "OLED.h"
#include "includes.h"
//#include "delay.h"
#include "codetab.h"
//#include "stm32f10x_gpio.h"

void OLED_WrDat(unsigned char dat)//写数据
{
	unsigned char i;
	OLED_DC_Set();
	for(i=0;i<8;i++)
	{
		if((dat << i) & 0x80)
		{
			OLED_SDA_Set();
		}
		else
			OLED_SDA_Clr();
		OLED_SCL_Clr();
		OLED_SCL_Set();
	}
}

void OLED_WrCmd(unsigned char cmd)//写命令
{
	unsigned char i;
	OLED_DC_Clr();
	for(i=0;i<8;i++) //发送一个八位数据
	{
		if((cmd << i) & 0x80)
		{
			OLED_SDA_Set();
		}
		else
		{
			OLED_SDA_Clr();
		}
		OLED_SCL_Clr();
		OLED_SCL_Set();
	}
}

void OLED_SetPos(unsigned char x, unsigned char y)//设置起始点坐标
{
	OLED_WrCmd(0xb0 + y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);
	OLED_WrCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char bmp_dat)//全屏填充
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		{
			OLED_WrDat(bmp_dat);
		}
	}
}

void OLED_CLS(void)//清屏
{
	OLED_Fill(0x00);
}

void OLED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE);	 //使能PC,D,G端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	 //PB12,PB13,PB14,PB15推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOB12,13,14,15
 	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);	//PB12,PB13,PB14,PB15 输出高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOE,GPIO_Pin_12);	        //PE12


	OLED_SCL_Set();
	
	OLED_RST_Set_L();
	OLED_RST_Set_R();//add by cgy
//	DelayMs(500);
	OSTimeDlyHMSM(0,0,0,500);

	OLED_RST_Clr_L();
	OLED_RST_Clr_R();//add by cgy
	
	OLED_WrCmd(0xAE);//--turn off oled panel
	
	OLED_WrCmd(0x02);//---set low column address
	OLED_WrCmd(0x10);//---set high column address
	
	OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	
	OLED_WrCmd(0xB0);//--set contrast control register
	
	OLED_WrCmd(0x81); // Set SEG Output Current Brightness
	OLED_WrCmd(0x80);//--Set SEG/Column Mapping     0xa0???? 0xa1??
	
	OLED_WrCmd(0xA1);//Set COM/Row Scan Direction   0xc0???? 0xc8??
	
	OLED_WrCmd(0xA6);//--set normal display
	
	OLED_WrCmd(0xA8);//--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3F);//--1/64 duty
	
	OLED_WrCmd(0xAD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x8B);//-not offset
	
	OLED_WrCmd(0x30);//--set display clock divide ratio/oscillator frequency
	
	OLED_WrCmd(0xC8);//--set divide ratio, Set Clock as 100 Frames/Sec
	
	OLED_WrCmd(0xD3);//--set pre-charge period
	OLED_WrCmd(0x00);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	
	OLED_WrCmd(0xD5);//--set com pins hardware configuration
	OLED_WrCmd(0x80);
	
	OLED_WrCmd(0xD9);//--set vcomh
	OLED_WrCmd(0x1F);//Set VCOM Deselect Level
	
	OLED_WrCmd(0xDA);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x12);//
	
	OLED_WrCmd(0xDB);//--set Charge Pump enable/disable
	OLED_WrCmd(0x40);//--set(0x10) disable

	OLED_WrCmd(0xAF);//--turn on oled panel
	
	OLED_Fill(0x00);
	OLED_SetPos(0,0);
}

void OLED_6x8Str(unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c = ch[j]-32;
		if(x>126)
		{
			x=0;y++;
		}
		OLED_SetPos(x,y);
		for(i=0;i<6;i++)
		{
			OLED_WrDat(F6x8[c][i]);
		}
		x+=6;
		j++;
	}
}

void OLED_8x16Str(unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120)
		{
			x=0;y++;
		}
		OLED_SetPos(x,y);
		for(i=0;i<8;i++)
		{
			OLED_WrDat(F8X16[c*16+i]);
		}
		OLED_SetPos(x,y+1);
		for(i=0;i<8;i++)
		{
			OLED_WrDat(F8X16[c*16+i+8]);
		}
		x+=8;
		j++;
	}
}

void OLED_16x16CN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_SetPos(x+2 , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x+2,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;
  if(y1%8==0)
	{
		y=y1/8;
	}
  else
		y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			OLED_WrDat(BMP[j++]);
		}
	}
}
