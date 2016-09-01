
/*
*********************************************************************************************************
*
*                                            ROBOT CODE
*
*                                         Yingborui Big Robot
*                                              with the
*                                   CPU STM32F103VE & Driver DRV8848
*
* Filename      : UL.c
* Version       : 2.0.0
* Programmer(s) : Cao
*********************************************************************************************************
*/

#include "includes.h"

//定时器4通道1、2、3、4输入捕获配置

TIM_ICInitTypeDef  TIM4_ICInitStructure;

void TIM4_Cap_Init(u16 arr,u16 psc)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能TIM4时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIOD、GPIOE时钟
	
	//PD12 --->Echo1
	//PD13 --->Echo2
  //PD14 --->Echo3
	//PD15 --->Echo4
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  //PD12、PD13、PD14、PD15清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;              //PD12、PD13、PD14、PD15输入  
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);              //先初始化后重映射
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	//PD12、PD13、PD14、PD15下拉
	
	//PD10 --->Trig1
	//PD11 --->Trig2
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11;  //PD10 PD11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //PD10 PD11输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//PG2 --->Trig3
	//PG3 --->Trig4
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3;    //PG2 PG3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	//初始化定时器4 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM4输入捕获参数
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;            //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;         //从优先级7级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                            //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC1,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC2,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC3,ENABLE);//允许更新中断 ,允许CC1IE捕获中断
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC1IE捕获中断
  TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4
}
/************************************************************************************
 *
 *
 *
 */
INT32U UltrasonicWave_Distance1;
u8  TIM4CH1_CAPTURE_STA=0;	//输入捕获状态		    				
u16	TIM4CH1_CAPTURE_VAL;	//输入捕获值

INT32U UltrasonicWave_Distance2;
u8  TIM4CH2_CAPTURE_STA=0;	//输入捕获状态		    				
u16	TIM4CH2_CAPTURE_VAL;	//输入捕获值

INT32U UltrasonicWave_Distance3;
u8  TIM4CH3_CAPTURE_STA=0;	//输入捕获状态		    				
u16	TIM4CH3_CAPTURE_VAL;	//输入捕获值

INT32U UltrasonicWave_Distance4;
u8  TIM4CH4_CAPTURE_STA=0;	//输入捕获状态		    				
u16	TIM4CH4_CAPTURE_VAL;	//输入捕获值
 
//定时器2中断服务程序	 
void TIM4_IRQHandler(void)
{ 
	OSIntEnter();
	// " TIM_IT_Update " 计数器溢出中断	  
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) 
		{	   
				TIM_ClearITPendingBit(TIM4, TIM_IT_Update); //清除中断标志位			
				if(TIM4CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
				{
						TIM4CH1_CAPTURE_STA |=0X80;
						TIM4CH1_CAPTURE_VAL=TIM_GetCapture1(TIM4);
	  				TIM4CH1_CAPTURE_STA++;
				}	 
				if(TIM4CH2_CAPTURE_STA&0X40)//已经捕获到高电平了
				{
						TIM4CH2_CAPTURE_STA |=0X80;
						TIM4CH2_CAPTURE_VAL=TIM_GetCapture2(TIM4);
	  				TIM4CH2_CAPTURE_STA++;
				}
				if(TIM4CH3_CAPTURE_STA&0X40)//已经捕获到高电平了
				{
						TIM4CH3_CAPTURE_STA |=0X80;
						TIM4CH3_CAPTURE_VAL=TIM_GetCapture3(TIM4);
	  				TIM4CH3_CAPTURE_STA++;
				}	
				if(TIM4CH4_CAPTURE_STA&0X40)//已经捕获到高电平了
				{
						TIM4CH4_CAPTURE_STA |=0X80;
						TIM4CH4_CAPTURE_VAL=TIM_GetCapture4(TIM4);
	  				TIM4CH4_CAPTURE_STA++;
				}	 		
		}
		
	// " TIM_IT_CC1 " 定时器捕获通道1捕获(高电平或者低电平)中断
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
		{	
				TIM_ClearITPendingBit(TIM4,TIM_IT_CC1); //清除中断标志位	
				if(TIM4CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
				{	  			
					TIM4CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次上升沿
					TIM4CH1_CAPTURE_VAL=TIM_GetCapture1(TIM4); //函数TIM_GetCapture1(TIM2)获取的是寄存器TIMx->CCR1的值，TIMx->CCR1寄存器用来储存捕获发生时TIMx->CNT的值
					TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
				}else  								//还未开始,第一次捕获上升沿
				{
					TIM4CH1_CAPTURE_STA=0;			//清空
					TIM4CH1_CAPTURE_VAL=0;
					TIM_SetCounter(TIM4,0);
					TIM4CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
					TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
				}		    
		}		
	// " TIM_IT_CC2 " 定时器捕获通道2捕获(高电平或者低电平)中断
	if (TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)//捕获2发生捕获事件
		{	
				TIM_ClearITPendingBit(TIM4,TIM_IT_CC2); //清除中断标志位	
				if(TIM4CH2_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
				{	  			
					TIM4CH2_CAPTURE_STA|=0X80;		//标记成功捕获到一次上升沿
					TIM4CH2_CAPTURE_VAL=TIM_GetCapture2(TIM4); //函数TIM_GetCapture2(TIM4)获取的是寄存器TIMx->CCR2的值，TIMx->CCR2寄存器用来储存捕获发生时TIMx->CNT的值
					TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
				}else  								//还未开始,第一次捕获上升沿
				{
					TIM4CH2_CAPTURE_STA=0;			//清空
					TIM4CH2_CAPTURE_VAL=0;
					TIM_SetCounter(TIM4,0);
					TIM4CH2_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
					TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
				}					
		}
	// " TIM_IT_CC3 " 定时器捕获通道3捕获(高电平或者低电平)中断
	if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)//捕获3发生捕获事件
		{	
				TIM_ClearITPendingBit(TIM4,TIM_IT_CC3); //清除中断标志位	
				if(TIM4CH3_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
				{	  			
					TIM4CH3_CAPTURE_STA|=0X80;		//标记成功捕获到一次上升沿
					TIM4CH3_CAPTURE_VAL=TIM_GetCapture3(TIM4); //函数TIM_GetCapture3(TIM4)获取的是寄存器TIMx->CCR3的值，TIMx->CCR3寄存器用来储存捕获发生时TIMx->CNT的值
					TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
				}else  								//还未开始,第一次捕获上升沿
				{
					TIM4CH3_CAPTURE_STA=0;			//清空
					TIM4CH3_CAPTURE_VAL=0;
					TIM_SetCounter(TIM4,0);
					TIM4CH3_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
					TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
				}						
		}			     	    	
	// " TIM_IT_CC4 " 定时器捕获通道4捕获(高电平或者低电平)中断
	if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)//捕获4发生捕获事件
		{	
				TIM_ClearITPendingBit(TIM4,TIM_IT_CC4); //清除中断标志位	
				if(TIM4CH4_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
				{	  			
					TIM4CH4_CAPTURE_STA|=0X80;		//标记成功捕获到一次上升沿
					TIM4CH4_CAPTURE_VAL=TIM_GetCapture4(TIM4); //函数TIM_GetCapture4(TIM4)获取的是寄存器TIMx->CCR1的值，TIMx->CCR4寄存器用来储存捕获发生时TIMx->CNT的值
					TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
				}else  								//还未开始,第一次捕获上升沿
				{
					TIM4CH4_CAPTURE_STA=0;			//清空
					TIM4CH4_CAPTURE_VAL=0;
					TIM_SetCounter(TIM4,0);
					TIM4CH4_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
					TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
				}				
		}			     	    					   
  OSIntExit();
}

/*
 * Function: UltrasonicWave_StartMeasure
 * Brief   : 开始测距，发出一个>10us的脉冲
 * Input   : None
 * Output  : None
 */
void UltrasonicWave_StartMeasure(u8 i)
{
	if(i==1)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_10); 		  
		delay_us(15);
		GPIO_ResetBits(GPIOD,GPIO_Pin_10);
	}
	else if(i==2)
	{
	  GPIO_SetBits(GPIOD,GPIO_Pin_11); 		  
		delay_us(15);
		GPIO_ResetBits(GPIOD,GPIO_Pin_11);
	}
	else if(i==3)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_2); 		  
		delay_us(15);
		GPIO_ResetBits(GPIOG,GPIO_Pin_2);
	}
	else if(i==4)
	{
	  GPIO_SetBits(GPIOG,GPIO_Pin_3); 		  
		delay_us(15);
		GPIO_ResetBits(GPIOG,GPIO_Pin_3);
	}
}

