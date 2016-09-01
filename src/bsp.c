
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
*																		               Cortex-M3 �ж����ȼ�
*
* -----------------------------------------------------------------------------------------------------------------------------
* | NVIC_PriorityGroup    |    NVIC_IRQChannel����ռʽ���ȼ�	  |  NVIC_IRQChannel�Ĵ����ȼ�  |               ����						|
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_0  |              0											|					0~15                |   ��ռʽ���ȼ�0λ �����ȼ�4λ |
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_1  |							0~1											|					0~7									|	  ��ռʽ���ȼ�1λ �����ȼ�3λ	|
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_2	|							0~3											|					0~3									|		��ռʽ���ȼ�2λ �����ȼ�2λ	|
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_3	|							0~7											|					0~1									|		��ռʽ���ȼ�3λ �����ȼ�1λ	|
* -----------------------------------------------------------------------------------------------------------------------------
*	| NVIC_PriorityGroup_4	|							0~15										|					 0									|		��ռʽ���ȼ�4λ �����ȼ�0λ	|
* -----------------------------------------------------------------------------------------------------------------------------
*
* @ѡ��NVIC_PriorityGroup_0�������NVIC_IRQChannelPreemptionPriority���ж�ͨ�������ò�����Ӱ�졣
*	@ѡ��NVIC_PriorityGroup_4�������NVIC_IRQChannelSubPriority���ж�ͨ�������ò�����Ӱ�졣
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
* Description : �����˸�ģ���ʼ��
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
	
	//�ж�������ʼ��
	NVICInit();
	
  //	MemInit();
	
	// LED��ʼ��
	LED_init();
	
	//����2������3��ʼ��
	UsartInit();
	
	//ʹ�ܵ������оƬDRV8848�����⡢��翪�ص�IO
	IO_Enable();
	
	//PWM����
	motorCfg();
	
	//ADC��ʼ�������ڲ���ϥ�ؽڴ���λ��
	Adc_Init();//adת����ʼ��
	
	
//	//��ʱ��4��׽��ʼ�������ڳ�����
	TIM4_Cap_Init(0xFFFF,72-1);
	
	delay_init();
//	
//	//I2C�ӿڳ�ʼ��������OLED
//	I2C_Configuration();
//	
//	//OLED��ʼ��(�������OLED����ִ�����ʼ����������ȻI2Cָ��Ͳ���ȥ���ᵼ�³�����)
		OLED_Init();//SPI�ӿ�
//	
//	//SD����������ʱ����(��bsp_sdio_sd.c���ѽ��������ע�͵�)
//	/* Mount File System */
		result = f_mount(0, &fs);			/* Mount a logical drive */
		if (result != FR_OK)
		{
			printf("Mount Failed(%d)\r\n", result);
		}
}
