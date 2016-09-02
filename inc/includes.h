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

/******************************** 指令编码 - 底盘式机器人 **********************************/
//机器人整体运动（为了防止蓝牙模块的干扰，所以不用0和1做指令编码）

/*start********************************** 机器人整体运动 ******************************************/

#define RobotWalkForward     2  //前进
#define RobotWalkBackward    3  //后退
#define RobotTurnLeft        4  //左转
#define RobotTurnRight       5  //右转

/*end********************************** 机器人整体运动 ******************************************/


/*start*********************** 机器人各关节独立运动(包括眼睛) *********************************/

//
#define HeadTurnLeft         6  //
#define HeadTurnRight	       7  //
#define HeadSwing            8  //

//
#define LeftArmUp            9   //
#define LeftArmDown          10  //
#define WaveLeftArm	         11  //

#define RightArmUp           12  //
#define RightArmDown         13  //
#define WaveRightArm         14  //

//
#define LeftWristTurnLeft    15  //
#define LeftWristTurnRight   16  //

#define RightWristTurnLeft   17  //
#define RightWristTurnRight  18  //

//
#define WaistTurnLeft        19  //
#define WaistTurnRight       20  //
#define WaistSwing           21  //
//

/*end*********************** 机器人各关节独立运动(包括眼睛) *********************************/


#define Dance                22

//挂起舞蹈任务
#define SuspendDanceTask     126

//停止机器人所有关节
#define AllJointMotorStop    127 //停止


/* ---------------------------------------------------------------------------*/



#endif
