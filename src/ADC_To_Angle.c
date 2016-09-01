
/*
*********************************************************************************************************
*
*                                            ROBOT CODE
*
*                                         Yingborui Big Robot
*                                              with the
*                                   CPU STM32F103VE & Driver DRV8848
*
* Filename      : ADC_To_Angle.c
* Version       : 1.0.0
* Programmer(s) : Fan
*********************************************************************************************************
*/



/*  STM32库函数版本：V3.5
 *  需将延时函数delay_ms()替换为自己的延时函数
 *
 */


/*########################################  ADC 操作   ##############################################*/
#include "includes.h"

#define N 10
u16 adc_10[N]={0};
u16 adc_11[N]={0};
u16 adc_12[N]={0};

u8 LeftKnee=0;
u8 RightKnee=0;
u8 Waist = 0;

#define Pi 3.14159265
/***************************************************************************
* 函数：Adc_Init(void)   
* 简介：初始化ADC1的通道1(PA1)
*
*/															   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PC0 PC1 PC2 PC3作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}

/***************************************************************
* 函数：  Get_Adc(u8 ch)
* 简介：  获得ADC值
*
*/	
u16 Get_Adc(u8 ch);
u16 Get_Adc(u8 ch)   
{
  //设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

#define A 10 
u16 value=0; 
u16 filter_clipping(u8 ch);//限幅滤波
u16 filter_clipping(u8 ch) 
{ 
	u16 new_value;
	new_value=Get_Adc(ch);
	if((new_value-value)>A || (value-new_value)>A)
	{
		return value;
	}
	return new_value;
} 

/********************************************************************************************************
*                                   filter_average(u16 *adc,u8 ch,u8 *i)
*
* Description :  获取腿部(膝关节)和腰部ADC值
*
* Argument(s) : adc:存储该ADC通道值的数组
                ch :当前ADC的通道数
                i  :用于统计滑动窗口的位置
*
* Return(s)   : 倾角传感器的递推均值
*
*********************************************************************************************************/
u16 filter_average(u16 *adc,u8 ch,u8 *i);//递推滑动滤波
u16 filter_average(u16 *adc,u8 ch,u8 *i)
{
  u16 count;
  u16 sum=0;
	adc[*i] = Get_Adc(ch);
	(*i)++;
	if(*i==N) 
	{
			*i=0;
	}
	if(adc[*i]==0)
	{
		  return adc[*i-1];
	}
	for(count=0;count<N;count++)
	{
			sum += adc[count];	
	}
	value = sum/N;
	return value;
}


/***************************************************************
* 函数：  Get_Angle(void)
* 简介：  根据采到的ADC值，转换为长度值，并根据已有几何关系，计算出角度值
*
*/
float Get_Angle(u16 *adc,u8 ch,u8 *i)
 {
 	   s16 adcx;
	   float length;
	   float angle;
 	   adcx=filter_average(adc,ch,i);
		if(ch==ADC_Channel_12)
		{
				adcx -= 2047;
				//根据ADC值，计算获得长度值(相对于"0点")
				length=(float)adcx*(40.0/4096.0);
				//计算出角度值
				angle = atan(length/46.3)*180.0/Pi;
		}
		else
		{
				//当前 "0点" 位置ADC值是2047。此处做出处理，得到相对于"0点"的ADC值
				adcx = 2047-adcx;
				//计算出角度值
				angle = adcx*(360.0/4095.0);
		}
		return angle;
 }

