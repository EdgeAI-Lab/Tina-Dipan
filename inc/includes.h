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

/******************************** ָ����� - ����ʽ������ **********************************/
//�����������˶���Ϊ�˷�ֹ����ģ��ĸ��ţ����Բ���0��1��ָ����룩

/*start********************************** �����������˶� ******************************************/

#define RobotWalkForward     2  //ǰ��
#define RobotWalkBackward    3  //����
#define RobotTurnLeft        4  //��ת
#define RobotTurnRight       5  //��ת

/*end********************************** �����������˶� ******************************************/


/*start*********************** �����˸��ؽڶ����˶�(�����۾�) *********************************/

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

/*end*********************** �����˸��ؽڶ����˶�(�����۾�) *********************************/


#define Dance                22

//�����赸����
#define SuspendDanceTask     126

//ֹͣ���������йؽ�
#define AllJointMotorStop    127 //ֹͣ


/* ---------------------------------------------------------------------------*/



#endif
