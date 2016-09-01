
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



/*  STM32�⺯���汾��V3.5
 *  �轫��ʱ����delay_ms()�滻Ϊ�Լ�����ʱ����
 *
 */


/*########################################  ADC ����   ##############################################*/
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
* ������Adc_Init(void)   
* ��飺��ʼ��ADC1��ͨ��1(PA1)
*
*/															   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PC0 PC1 PC2 PC3��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}

/***************************************************************
* ������  Get_Adc(u8 ch)
* ��飺  ���ADCֵ
*
*/	
u16 Get_Adc(u8 ch);
u16 Get_Adc(u8 ch)   
{
  //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

#define A 10 
u16 value=0; 
u16 filter_clipping(u8 ch);//�޷��˲�
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
* Description :  ��ȡ�Ȳ�(ϥ�ؽ�)������ADCֵ
*
* Argument(s) : adc:�洢��ADCͨ��ֵ������
                ch :��ǰADC��ͨ����
                i  :����ͳ�ƻ������ڵ�λ��
*
* Return(s)   : ��Ǵ������ĵ��ƾ�ֵ
*
*********************************************************************************************************/
u16 filter_average(u16 *adc,u8 ch,u8 *i);//���ƻ����˲�
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
* ������  Get_Angle(void)
* ��飺  ���ݲɵ���ADCֵ��ת��Ϊ����ֵ�����������м��ι�ϵ��������Ƕ�ֵ
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
				//����ADCֵ�������ó���ֵ(�����"0��")
				length=(float)adcx*(40.0/4096.0);
				//������Ƕ�ֵ
				angle = atan(length/46.3)*180.0/Pi;
		}
		else
		{
				//��ǰ "0��" λ��ADCֵ��2047���˴����������õ������"0��"��ADCֵ
				adcx = 2047-adcx;
				//������Ƕ�ֵ
				angle = adcx*(360.0/4095.0);
		}
		return angle;
 }

