/* Includes ------------------------------------------------------------------*/
#include "includes.h"

extern unsigned char serial2Rec[32];
extern unsigned char rx2Count;

extern unsigned char serial3Rec[32];
extern unsigned char rx3Count;
/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern unsigned short WRC;
void SysTick_Handler(void)
{
	OSIntEnter();
	OSTimeTick();     /* Call uC/OS-II's OSTimeTick(),在os_core.c文件里定义,主要判断延时的任务是否计时到*/
	WRC++;
	OSIntExit();     
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void SDIO_IRQHandler(void)
{
	/* Process All SDIO Interrupt Sources */
	OSIntEnter();
	
	SD_ProcessIRQSrc();

	OSIntExit();
}


/**
  * @brief  This function handles DMA1_Channel7 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel7_IRQHandler(void)
{
	OSIntEnter();
	

	OSIntExit();
}	

/**
  * @brief  This function handles DMA1_Channel4 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel4_IRQHandler(void)
{
	OSIntEnter();
//	if (DMA_GetITStatus(DMA1_IT_TC4) == SET)//Translate complete
//	{
//		//Disable the dma,clear the interrupt bit
//		DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, DISABLE);
//		DMA_ClearITPendingBit(DMA1_IT_TC4);
//		DMA_Cmd(DMA1_Channel4, DISABLE);
//	}
//	if (DMA_GetITStatus(DMA1_IT_TE4) == SET)//Translate error
//	{
//		DMA_ClearITPendingBit(DMA1_IT_TE4);
//	}

	OSIntExit();
}	

/**
  * @brief  This function handles USART2 interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	OSIntEnter();
	if (USART_GetITStatus(USART2, USART_IT_TC) == SET)
	{
		
		USART_ClearITPendingBit(USART2, USART_IT_TC);
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		serial2Rec[rx2Count++]=USART_ReceiveData(USART2);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}  
	OSIntExit();
}

/**
  * @brief  This function handles USART3 interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	OSIntEnter();
	if (USART_GetITStatus(USART3, USART_IT_TC) == SET)
	{
		
		USART_ClearITPendingBit(USART3, USART_IT_TC);
	}
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		serial3Rec[rx3Count++]=USART_ReceiveData(USART3);
//		printf("%d\n",serialRec[rxCount-1]);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}  
	OSIntExit();
}
/*
*********************************************************************************************************
*函数名：DMA1通道2中断（SPI1接收中断）
*函数原型:void  BSP_IntHandlerDMA1_CH4      (void)   
*入口参数：无
*出口参数：无
*函数说明：
*********************************************************************************************************
*/
void DMA1_Channel2_IRQHandler(void)  
{
	OSIntEnter();

	
	OSIntExit();
}


/*
*********************************************************************************************************
*函数名：DMA1通道3中断（SPI2发送中断）
*函数原型:void  BSP_IntHandlerDMA1_CH5      (void)  
*入口参数：无
*出口参数：无
*函数说明：
*********************************************************************************************************
*/
void DMA1_Channel3_IRQHandler(void)  
{
	OSIntEnter();




	OSIntExit();
	       
}
/*
*********************************************************************************************************
*函数名：I2C传输中断
*函数原型:void  BSP_IntHandlerDMA1_CH6      (void) 
*入口参数：无
*出口参数：无
*函数说明：
*********************************************************************************************************
*/
void  DMA1_Channel6_IRQHandler      (void)
{
	
	OSIntEnter();


	
	OSIntExit();
}

/*
 *********************************************************************************************************
 *函数名：外部中断5-9
 *函数原型:void  BSP_IntHandlerEXTI9_5       (void)
 *入口参数：无
 *出口参数：无
 *函数说明：MP3播放
 *********************************************************************************************************
 */
// void EXTI9_5_IRQHandler(void)
// {
// 	OSIntEnter();
// 	if (EXTI_GetITStatus(EXTI_Line5))
// 	{
// 		EXTI_ClearFlag(EXTI_Line5);
// 		EXTI_ClearITPendingBit(EXTI_Line5);
// 	}
// 	OSIntExit();

// }
/*
*********************************************************************************************************
*函数名：外部中断15
*函数原型:void  BSP_IntHandlerEXTI1         (void)
*入口参数：无
*出口参数：无
*函数说明：中文语音播放完成中断
*********************************************************************************************************
*/
//void  EXTI15_10_IRQHandler         (void)
//{	

//	OSIntEnter();

//	if (EXTI_GetITStatus(EXTI_Line15))
//	{
//		EXTI_ClearFlag(EXTI_Line15);
//		EXTI_ClearITPendingBit(EXTI_Line15);
//	}
//	OSIntExit();	
//}

/*
 *********************************************************************************************************
 *函数名：I2C1错误中断
 *函数原型:void I2C1_ER_IRQHandler(void)  
 *入口参数：无
 *出口参数：无
 *函数说明：
 *********************************************************************************************************
 */
void I2C1_ER_IRQHandler(void)  
{
	
	OSIntEnter();


	OSIntExit();
}
