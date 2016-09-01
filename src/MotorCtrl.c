
/*
*********************************************************************************************************
*
*                                            ROBOT CODE
*
*                                         Yingborui Big Robot
*                                              with the
*                                   CPU STM32F103VE & Driver DRV8848
*
* Filename      : MotorCtrl.c
* Version       : 1.0.0
* Programmer(s) : Cao
*********************************************************************************************************
*/

#include "includes.h"

extern System myRobot;

/*
*********************************************************************************************************
*                                             getHclkFrequency()
*
* Description : 获取系统时钟
*
* Argument(s) : none.
*
* Return(s)   : rcc_clocks.HCLK_Frequency当前系统时钟.
*
*********************************************************************************************************
*/
INT32U getHclkFrequency(void)
{
	RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	return (INT32U)rcc_clocks.HCLK_Frequency;
}

/*
*********************************************************************************************************
*                                             IO_Enable()
*
* Description : 使能驱动芯片DRV8848 、光电开关、红外等IO
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* Pins				:	LeftArm(PG9)  RightArm(PG10)  Waist Switch(PG11)     (PG9 ~ PG14) 光电开关接口
*               (PE2 ~ PE5) 做为红外避障和循迹接口                  (PG4 ~ PG7) 做为红外避障和循迹接口
*               Sleep1(PA5)、Sleep2(PB2)、Sleep3(PE15) 、Sleep4(PE8)
********************************************************************************************************
*/
void IO_Enable(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOG, ENABLE);

  //LeftArm(PG9)  RightArm(PG10)  Waist Switch(PG11)	    (PG9 ~ PG14) 光电开关接口
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	//IR(PE2 ~ PE5)                                         (PE2 ~ PE5) 做为红外避障和循迹接口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;            
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	//IR(PG4 ~ PG7)                                         (PG4 ~ PG7) 做为红外避障和循迹接口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;            
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	//Enable DRV8848
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	              //Sleep1(PA5)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	              //Sleep2(PB2)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15;	//Sleep3(PE15) 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //Sleep4(PE8) 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	//Enable
	GPIO_SetBits(GPIOA,GPIO_Pin_5);					     //Sleep1
	GPIO_SetBits(GPIOB,GPIO_Pin_2);					     //Sleep2
	GPIO_SetBits(GPIOE,GPIO_Pin_15);             //Sleep3
	GPIO_SetBits(GPIOE,GPIO_Pin_8);					     //Sleep4
}

/*
*************************************************************************************
*                                   motorCfg()
*
* Description : PWM电机驱动
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
*************************************************************************************
*                              Detail
*____________________________________________________________________________________
*
*	  Motor		   Part			         |		  TIM       |      Pin        |       Remap
* -----------------------------------------------------------------
*	M1(PWM1)		Head(steer)            TIM5_CH1            PA0                 NO
*	M2(PWM2)    Left Wirst(steer)      TIM5_CH2            PA1                 NO
*	M3(PWM3)		Right Wirst(steer) 	   TIM5_CH3            PA2                 NO
*
*	M4(PWM4)    Left Foot              TIM3_CH1            PA6                 NO
*	M5(PWM5)		Left Knee         	   TIM3_CH2            PA7                 NO
*
*	M6(PWM6)    Right Foot             TIM3_CH3            PB0                 NO
*	M7(PWM7)		Right Knee             TIM3_CH4            PB1                 NO
*
*	M8(PWM8)    Left Arm               TIM2_CH3            PB10                YES
*	M9(PWM9)		Right Arm     		     TIM2_CH4            PB11                YES
*
*	M10(PWM10)  Waist                  TIM1_CH1            PE9                 YES
* 	
**************************************************************************************************
*/
void motorCfg(void)
{
	/* -1-  */
	GPIO_InitTypeDef Gpio_InitStr;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/* -2-  */
	//TIM2 TIM3 TIM4 TIM5 Clock Enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM5, ENABLE);
	//TIM2 TIM3 TIM4 TIM5 Clock Enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	/* -3-  */
	// GPIOA GPIOB GPIOC GPIOD GPIOE clock enable
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

	/****************************************** 舵机接口*******************************************/
	//TIM5 CH1(PA0) CH2(PA1) CH3(PA2) No Remap 
	Gpio_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;														 //定时器5 IO配置
	Gpio_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
 	Gpio_InitStr.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOA, &Gpio_InitStr);	
	
	TIM_TimeBaseStructure.TIM_Prescaler = getHclkFrequency()/50000-1;  //72-1 即72分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 1000-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);										  //timer5 init
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;										//set PWM mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//Init with duty cycle zero.
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;						//High active
		
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);														//TIM5 (PWM CH1) 
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);														//TIM5 (PWM CH2) 
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);														//TIM5 (PWM CH3) 
	/*********************************************************************************************/
	
	
  /*************************************** 直流电机接口****************************************/
	//TIM3 CH1(PA6) CH2(PA7) CH3(PB0) CH4(PB1)  No Remap
	Gpio_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;														//定时器3 IO配置
	Gpio_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOA, &Gpio_InitStr);
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOB, &Gpio_InitStr);
	
	//TIM2 CH3(PB10) CH4(PB11)  FullRemap  直流电机接口												 
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;                    //定时器2 IO配置
	GPIO_Init(GPIOB, &Gpio_InitStr);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);		                //先初始化，后重映射
	
	//TIM1 CH1(PE9) CH2(PE11) FullRemap
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;                   //定时器1 IO配置
	GPIO_Init(GPIOE, &Gpio_InitStr);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);		                //先初始化，后重映射	
	
	TIM_TimeBaseStructure.TIM_Prescaler = getHclkFrequency()/2000000-1; //72-1 即72分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 100-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);                     //TIM3 init
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                     //TIM2 init
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                     //TIM1 init

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	                  //PWM set
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//Init with duty cycle zero.
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	          //High active
	
	//TIM3 (PWM CH1~CH4) 
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	//TIM2 (PWM CH3~CH4) 
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	//TIM1 (PWM CH3~CH4) 
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	/******************************************************************************************/
	
	//TIM1
	TIM_Cmd(TIM1, ENABLE);
	//TIM2
	TIM_Cmd(TIM2, ENABLE);
	//TIM3
	TIM_Cmd(TIM3, ENABLE);
	//TIM5
	TIM_Cmd(TIM5, ENABLE);
	
	//TIM1 Main Output Enable
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	//TIM2 Main Output Enable
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	
	//TIM3 Main Output Enable
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
	
	//TIM5 Main Output Enable
	TIM_CtrlPWMOutputs(TIM5, ENABLE);
	
	/* -5-  */
	HeadMove(70);
	LeftWristMove(70);
	RightWristMove(70);
	
	WaistMove(50);

	LeftArmMove(50);
	RightArmMove(50);
	
	LeftKneeMove(50);
	RightKneeMove(50);

	LeftFootMove(50);
	RightFootMove(50);
}

