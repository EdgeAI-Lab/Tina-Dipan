#ifndef  __UL_H__
#define  __UL_H__
#include "includes.h"


void TIM4_Cap_Init(u16 arr,u16 psc);
void UltrasonicWave_StartMeasure(u8 i);

extern INT32U UltrasonicWave_Distance1;
extern u8   TIM4CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u16	TIM4CH1_CAPTURE_VAL;	//输入捕获值	

extern INT32U UltrasonicWave_Distance2;
extern u8  TIM4CH2_CAPTURE_STA;	//输入捕获状态		    				
extern u16 TIM4CH2_CAPTURE_VAL;	//输入捕获值

extern INT32U UltrasonicWave_Distance3;
extern u8  TIM4CH3_CAPTURE_STA;	//输入捕获状态		    				
extern u16 TIM4CH3_CAPTURE_VAL;	//输入捕获值

extern INT32U UltrasonicWave_Distance4;
extern u8  TIM4CH4_CAPTURE_STA;	//输入捕获状态		    				
extern u16 TIM4CH4_CAPTURE_VAL;	//输入捕获值
#endif

