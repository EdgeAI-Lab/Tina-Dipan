#ifndef OS_BSP_H
#define OS_BSP_H
#include "ucos_ii.h"

/* ============================================================================
   struct defination
 ============================================================================*/
typedef struct Steertag{
  
	//Forward
	float positionStart_Float;  //��ʼλ��
	float positionDest_Float;   //Ŀ��λ��
	
	float positionNext_Float;   //��һ��λ��
	float positionNow_Float;    //��ǰλ��
	float step;
	
	//Backward
	float positionStart_Float_Backward;  //��ʼλ��
	float positionDest_Float_Backward;   //Ŀ��λ��
	
	float positionNext_Float_Backward;   //��һ��λ��
	float positionNow_Float_Backward;    //��ǰλ��
	float step_Backward;
	
	
	signed long LastPosition;
	
}Steer;

typedef struct ActionTag{
	
	Steer head;
	Steer leftArm;
	Steer rightArm;
	Steer leftAllow;
	Steer rightAllow;
	Steer waist;
	Steer leftLeg;
	Steer rightLeg;
	Steer leftFoot;
	Steer rightFoot;
}Action;//







typedef struct SystemTag{
	
	Action      	actionctrl;		

}System;


void  SysTick_Configure(void);

void NVICInit(void);

void SynUsartInit(void);

void RobotSystemInit(void);

void SynUsartInit(void);

void SynEXTISet(void);

void SynEXTIReset(void);

void SynSendData(const unsigned char *buf);

void Syn_SendSetDMA(const unsigned char *buf);

unsigned char GetSynStatus(void);


#endif

