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

/**************************************************ָ�����********************************************************/
//�����������˶���Ϊ�˷�ֹ����ģ��ĸ��ţ����Բ���0��1��ָ����룩
#define RobotWalkForward     2  //ǰ��
#define RobotWalkBackward    3  //����
#define RobotTurnLeft        4  //��ת
#define RobotTurnRight       5  //��ת
#define RobotSlideForward    6  //��ǰ����
#define RobotSlideBackward   7  //�����

//ͷ��
#define HeadSwing            8  //ҡͷ ҡҡͷ ����С���� ���˹�ҡ����

//�ֱ�
#define WaveLeftArm	         9  //�ӻ�����
#define WaveRightArm         10  //�ӻ�����

//����
#define WaistSwing           11 //�ڶ����� ŤŤ�� ŤŤСƨ�� ����ҡ��

//����
#define Dance                12 //����

//װ�ε�
#define TurnOnDecorateLeds   13 //��װ�ε�
#define TurnOffDecorateLeds  14 //�ر�װ�ε�
#define BlinkDecorateLeds    15 //��˸װ�ε�

//��϶���
#define Great                16 //�Һܰ�
#define Helpless             17 //��Ҳ������
#define Excited              18 //�Һ��˷�

//�۾�

//�����赸����
#define SuspendDanceTask     126

//ֹͣ���������йؽ�
#define AllJointMotorStop    127 //ֹͣ



/* ---------------------------------------------------------------------------*/



#endif
