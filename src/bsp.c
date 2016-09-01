
/*
*********************************************************************************************************
*
*                                            ROBOT CODE
*
*                                         Yingborui Big Robot
*                                              with the
*                                   CPU STM32F103VE & Driver DRV8848
*
* Filename      : bsp.c
* Version       : 1.0.0
* Programmer(s) : Fan
*********************************************************************************************************
*/

#include "includes.h"
#include "LED.h"

extern System myRobot;
FATFS fs;
FIL CfgFile,oledFile;
/* ============================================================================*/
/**
 * Description :Set systick for ucos-ii
 */
/* ============================================================================*/
void  SysTick_Configure(void)
{
    RCC_ClocksTypeDef rcc_clocks;
    INT32U cnts;
    RCC_GetClocksFreq(&rcc_clocks);
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;
	  SysTick->LOAD = cnts;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick->CTRL |= ((u32)0x00000001);
    SysTick->CTRL |= ((u32)0x00000002);
}

/*
*****************************************************************************************************
*
*																		               Cortex-M3 中断优先级
*
* -----------------------------------------------------------------------------------------------------------------------------
* | NVIC_PriorityGroup    |    NVIC_IRQChannel的先占式优先级	  |  NVIC_IRQChannel的从优先级  |               描述						|
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_0  |              0											|					0~15                |   先占式优先级0位 从优先级4位 |
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_1  |							0~1											|					0~7									|	  先占式优先级1位 从优先级3位	|
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_2	|							0~3											|					0~3									|		先占式优先级2位 从优先级2位	|
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_3	|							0~7											|					0~1									|		先占式优先级3位 从优先级1位	|
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_4	|							0~15										|					 0									|		先占式优先级4位 从优先级0位	|
* -----------------------------------------------------------------------------------------------------------------------------
*
* @选中NVIC_PriorityGroup_0，则参数NVIC_IRQChannelPreemptionPriority对中断通道的设置不产生影响。
*	@选中NVIC_PriorityGroup_4，则参数NVIC_IRQChannelSubPriority对中断通道的设置不产生影响。
*
*                NVICInit()
* Description :Init the NVIC-->global interrupt
*
******************************************************************************************************
*/
void NVICInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel					= USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel					= DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
*********************************************************************************************************
*                                             RobotSystemInit()
*
* Description : 机器人各模块初始化
*
* Argument(s) : none.
*
* Return(s)   : none.
*
*********************************************************************************************************
*/
void RobotSystemInit()
{
 	FRESULT result;
	
	//中断向量初始化
	NVICInit();
	
  //	MemInit();
	
	// LED初始化
	LED_init();
	
	//串口2、串口3初始化
	UsartInit();
	
	//使能电机驱动芯片DRV8848、红外、光电开关等IO
	IO_Enable();
	
	//PWM配置
	motorCfg();
	
	//ADC初始化，用于测量膝关节处电位器
	Adc_Init();//ad转换初始化
	
	
//	//定时器4捕捉初始化，用于超声波
	TIM4_Cap_Init(0xFFFF,72-1);
	
	delay_init();
//	
//	//I2C接口初始化，用于OLED
//	I2C_Configuration();
//	
//	//OLED初始化(必须接上OLED才能执行其初始化函数，不然I2C指令发送不出去，会导致程序卡死)
		OLED_Init();//SPI接口
//	
//	//SD卡插入检测暂时不用(在bsp_sdio_sd.c中已将相关配置注释掉)
//	/* Mount File System */
		result = f_mount(0, &fs);			/* Mount a logical drive */
		if (result != FR_OK)
		{
			printf("Mount Failed(%d)\r\n", result);
		}
}
