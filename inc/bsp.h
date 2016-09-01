#ifndef OS_BSP_H
#define OS_BSP_H
#include "ucos_ii.h"

/* ============================================================================
   struct defination
 ============================================================================*/
typedef struct Steertag{
  
	//Forward
	float positionStart_Float;  //初始位置
	float positionDest_Float;   //目标位置
	
	float positionNext_Float;   //下一步位置
	float positionNow_Float;    //当前位置
	float step;
	
	//Backward
	float positionStart_Float_Backward;  //初始位置
	float positionDest_Float_Backward;   //目标位置
	
	float positionNext_Float_Backward;   //下一步位置
	float positionNow_Float_Backward;    //当前位置
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

