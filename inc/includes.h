#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#include "ucos_ii.h"
#include "stm32f10x_conf.h"
#include "ff.h"
#include "usart.h"
#include "memmanage.h"
#include "bsp_sdio_sd.h"
#include "bsp.h"
#include <stdio.h>
#include <math.h>
#include "MotorCtrl.h"
#include "UL.h"
#include "myFun.h"
#include "OLED_I2C.h"
#include "delay.h"
#include "IR.h"
#include "LED.h"


void ExecuteAction(u8 action);

/**************************************************指令编码********************************************************/
//机器人整体运动（为了防止蓝牙模块的干扰，所以不用0和1做指令编码）
#define RobotWalkForward     2  //前进
#define RobotWalkBackward    3  //后退
#define RobotTurnLeft        4  //左转
#define RobotTurnRight       5  //右转
#define RobotSlideForward    6  //向前滑行
#define RobotSlideBackward   7  //向后滑行

//头部
#define HeadSwing            8  //摇头 摇摇头 动动小脖子 拨浪鼓摇起来

//手臂
#define WaveLeftArm	         9  //挥挥左手
#define WaveRightArm         10  //挥挥右手

//腰部
#define WaistSwing           11 //摆动腰部 扭扭腰 扭扭小屁股 风中摇摆

//跳舞
#define Dance                12 //跳舞

//装饰灯
#define TurnOnDecorateLeds   13 //打开装饰灯
#define TurnOffDecorateLeds  14 //关闭装饰灯
#define BlinkDecorateLeds    15 //闪烁装饰灯

//组合动作
#define Great                16 //我很棒
#define Helpless             17 //我也很无奈
#define Excited              18 //我好兴奋

//眼睛

//挂起舞蹈任务
#define SuspendDanceTask     126

//停止机器人所有关节
#define AllJointMotorStop    127 //停止



/* ---------------------------------------------------------------------------*/



#endif
