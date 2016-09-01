/*
*********************************************************************************************************
*
*                                            ROBOT CODE
*
*                                         Yingborui Big Robot
*                                              with the
*                                   CPU STM32F103VE & Driver DRV8848
*
* Filename      : app.c
* Version       : 1.0.0
* Programmer(s) : Cao
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "stm32f10x_gpio.h"
#include "LED.h"

/*
*********************************************************************************************************
*                                             void LED_init(void)
*
* Description : LED_init
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* Pins				:	PF(GPIO_Pin_All),PG0,PG1
*
*********************************************************************************************************
*/

void LED_init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF |RCC_APB2Periph_GPIOG, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	//Enable
	GPIO_SetBits(GPIOF,GPIO_Pin_All);
	GPIO_SetBits(GPIOG,GPIO_Pin_0);
	GPIO_SetBits(GPIOG,GPIO_Pin_1);
}
