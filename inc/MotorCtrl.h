#ifndef __MotorCtrl_H
#define __MotorCtrl_H

typedef enum RobotModeTag{
	ToForward,
	Forward,
	ToBackward,
	Backward,
	ToLeft,
	TurnLeft,
	ToRight,
	TurnRight,
	ToSlideForward,
	SlideForward,
	ToSlideBackward,
	SlideBackward,
	ToStop,
	ToStopBackward,
	StopBackward,
	Stop,
	StopButNoResetRobot, //用于停止机器人动作，但是不进如stop模式(stop模式会使机器人全身复位)
	NoTaskMode,          //用于机器人无任务模式
}RobotMode;

void IO_Enable(void);
void motorCfg(void);
void Adc_Init(void);

/*
*************************************************************************************
*
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
// Steer 舵机PWM（头、左腕、右腕）
#define HeadMove(Speed)		        	TIM_SetCompare1(TIM5, Speed)
#define LeftWristMove(Speed)				TIM_SetCompare2(TIM5, Speed)
#define	RightWristMove(Speed)				TIM_SetCompare3(TIM5, Speed)

// 左臂、右臂
// 抬起 > 50    放下 < 50
#define	LeftArmMove(Speed)					TIM_SetCompare3(TIM2, Speed)
#define RightArmMove(Speed)				  TIM_SetCompare4(TIM2, Speed)

// waist
// 向左摆 < 50   向右摆 > 50
#define WaistMove(Speed)				  	TIM_SetCompare1(TIM1, Speed)

// 底盘左轮、底盘右轮（驱动轮为前、万向轮为后）
// 前进 > 50     后退 < 50
#define LeftWheelMove(Speed)					TIM_SetCompare1(TIM3, Speed)
#define RightWheelMove(Speed)				  TIM_SetCompare2(TIM3, Speed)

//机器人移动
// Forward: Speed[50,100]     Backward: Speed[0,50]
#define RobotMove(Speed)						  TIM_SetCompare1(TIM3, Speed);TIM_SetCompare2(TIM3, Speed)

#endif





