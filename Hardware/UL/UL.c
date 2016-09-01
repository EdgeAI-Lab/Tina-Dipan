
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

//��ʱ��4ͨ��1��2��3��4���벶������

TIM_ICInitTypeDef  TIM4_ICInitStructure;

void TIM4_Cap_Init(u16 arr,u16 psc)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ��TIM4ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIOD��GPIOEʱ��
	
	//PD12 --->Echo1
	//PD13 --->Echo2
  //PD14 --->Echo3
	//PD15 --->Echo4
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  //PD12��PD13��PD14��PD15���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;              //PD12��PD13��PD14��PD15����  
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);              //�ȳ�ʼ������ӳ��
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	//PD12��PD13��PD14��PD15����
	
	//PD10 --->Trig1
	//PD11 --->Trig2
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11;  //PD10 PD11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //PD10 PD11���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//PG2 --->Trig3
	//PG3 --->Trig4
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3;    //PG2 PG3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	//��ʼ����ʱ��4 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM4���벶�����
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;            //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;         //�����ȼ�7��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                            //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������ж� ,����CC1IE�����ж�	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC2,ENABLE);//��������ж� ,����CC1IE�����ж�	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC3,ENABLE);//��������ж� ,����CC1IE�����ж�
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//��������ж� ,����CC1IE�����ж�
  TIM_Cmd(TIM4,ENABLE ); 	//ʹ�ܶ�ʱ��4
}
/************************************************************************************
 *
 *
 *
 */
INT32U UltrasonicWave_Distance1;
u8  TIM4CH1_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM4CH1_CAPTURE_VAL;	//���벶��ֵ

INT32U UltrasonicWave_Distance2;
u8  TIM4CH2_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM4CH2_CAPTURE_VAL;	//���벶��ֵ

INT32U UltrasonicWave_Distance3;
u8  TIM4CH3_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM4CH3_CAPTURE_VAL;	//���벶��ֵ

INT32U UltrasonicWave_Distance4;
u8  TIM4CH4_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM4CH4_CAPTURE_VAL;	//���벶��ֵ
 
//��ʱ��2�жϷ������	 
void TIM4_IRQHandler(void)
{ 
	OSIntEnter();
	// " TIM_IT_Update " ����������ж�	  
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) 
		{	   
				TIM_ClearITPendingBit(TIM4, TIM_IT_Update); //����жϱ�־λ			
				if(TIM4CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
				{
						TIM4CH1_CAPTURE_STA |=0X80;
						TIM4CH1_CAPTURE_VAL=TIM_GetCapture1(TIM4);
	  				TIM4CH1_CAPTURE_STA++;
				}	 
				if(TIM4CH2_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
				{
						TIM4CH2_CAPTURE_STA |=0X80;
						TIM4CH2_CAPTURE_VAL=TIM_GetCapture2(TIM4);
	  				TIM4CH2_CAPTURE_STA++;
				}
				if(TIM4CH3_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
				{
						TIM4CH3_CAPTURE_STA |=0X80;
						TIM4CH3_CAPTURE_VAL=TIM_GetCapture3(TIM4);
	  				TIM4CH3_CAPTURE_STA++;
				}	
				if(TIM4CH4_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
				{
						TIM4CH4_CAPTURE_STA |=0X80;
						TIM4CH4_CAPTURE_VAL=TIM_GetCapture4(TIM4);
	  				TIM4CH4_CAPTURE_STA++;
				}	 		
		}
		
	// " TIM_IT_CC1 " ��ʱ������ͨ��1����(�ߵ�ƽ���ߵ͵�ƽ)�ж�
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)//����1���������¼�
		{	
				TIM_ClearITPendingBit(TIM4,TIM_IT_CC1); //����жϱ�־λ	
				if(TIM4CH1_CAPTURE_STA&0X40)		//����һ���½��� 		
				{	  			
					TIM4CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ��������
					TIM4CH1_CAPTURE_VAL=TIM_GetCapture1(TIM4); //����TIM_GetCapture1(TIM2)��ȡ���ǼĴ���TIMx->CCR1��ֵ��TIMx->CCR1�Ĵ����������沶����ʱTIMx->CNT��ֵ
					TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
				}else  								//��δ��ʼ,��һ�β���������
				{
					TIM4CH1_CAPTURE_STA=0;			//���
					TIM4CH1_CAPTURE_VAL=0;
					TIM_SetCounter(TIM4,0);
					TIM4CH1_CAPTURE_STA|=0X40;		//��ǲ�����������
					TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
				}		    
		}		
	// " TIM_IT_CC2 " ��ʱ������ͨ��2����(�ߵ�ƽ���ߵ͵�ƽ)�ж�
	if (TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)//����2���������¼�
		{	
				TIM_ClearITPendingBit(TIM4,TIM_IT_CC2); //����жϱ�־λ	
				if(TIM4CH2_CAPTURE_STA&0X40)		//����һ���½��� 		
				{	  			
					TIM4CH2_CAPTURE_STA|=0X80;		//��ǳɹ�����һ��������
					TIM4CH2_CAPTURE_VAL=TIM_GetCapture2(TIM4); //����TIM_GetCapture2(TIM4)��ȡ���ǼĴ���TIMx->CCR2��ֵ��TIMx->CCR2�Ĵ����������沶����ʱTIMx->CNT��ֵ
					TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
				}else  								//��δ��ʼ,��һ�β���������
				{
					TIM4CH2_CAPTURE_STA=0;			//���
					TIM4CH2_CAPTURE_VAL=0;
					TIM_SetCounter(TIM4,0);
					TIM4CH2_CAPTURE_STA|=0X40;		//��ǲ�����������
					TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
				}					
		}
	// " TIM_IT_CC3 " ��ʱ������ͨ��3����(�ߵ�ƽ���ߵ͵�ƽ)�ж�
	if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)//����3���������¼�
		{	
				TIM_ClearITPendingBit(TIM4,TIM_IT_CC3); //����жϱ�־λ	
				if(TIM4CH3_CAPTURE_STA&0X40)		//����һ���½��� 		
				{	  			
					TIM4CH3_CAPTURE_STA|=0X80;		//��ǳɹ�����һ��������
					TIM4CH3_CAPTURE_VAL=TIM_GetCapture3(TIM4); //����TIM_GetCapture3(TIM4)��ȡ���ǼĴ���TIMx->CCR3��ֵ��TIMx->CCR3�Ĵ����������沶����ʱTIMx->CNT��ֵ
					TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
				}else  								//��δ��ʼ,��һ�β���������
				{
					TIM4CH3_CAPTURE_STA=0;			//���
					TIM4CH3_CAPTURE_VAL=0;
					TIM_SetCounter(TIM4,0);
					TIM4CH3_CAPTURE_STA|=0X40;		//��ǲ�����������
					TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
				}						
		}			     	    	
	// " TIM_IT_CC4 " ��ʱ������ͨ��4����(�ߵ�ƽ���ߵ͵�ƽ)�ж�
	if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)//����4���������¼�
		{	
				TIM_ClearITPendingBit(TIM4,TIM_IT_CC4); //����жϱ�־λ	
				if(TIM4CH4_CAPTURE_STA&0X40)		//����һ���½��� 		
				{	  			
					TIM4CH4_CAPTURE_STA|=0X80;		//��ǳɹ�����һ��������
					TIM4CH4_CAPTURE_VAL=TIM_GetCapture4(TIM4); //����TIM_GetCapture4(TIM4)��ȡ���ǼĴ���TIMx->CCR1��ֵ��TIMx->CCR4�Ĵ����������沶����ʱTIMx->CNT��ֵ
					TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
				}else  								//��δ��ʼ,��һ�β���������
				{
					TIM4CH4_CAPTURE_STA=0;			//���
					TIM4CH4_CAPTURE_VAL=0;
					TIM_SetCounter(TIM4,0);
					TIM4CH4_CAPTURE_STA|=0X40;		//��ǲ�����������
					TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
				}				
		}			     	    					   
  OSIntExit();
}

/*
 * Function: UltrasonicWave_StartMeasure
 * Brief   : ��ʼ��࣬����һ��>10us������
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

