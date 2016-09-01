#ifndef IR_H
#define IR_H

#include "includes.h"

uint8_t IsDetected(uint8_t IR);

/*
 *  红外引脚在机器人身上的意义
 * 
 *  ----------------------------------------------------------------------- 
 *  The pins on board:
 *
 *   IR1   IR2    IR3    IR4              IR5    IR6    IR7    IR8
 *   PE2   PE3    PE4    PE5              PG4    PG5    PG6    PG7
 *   
 *  -----------------------------------------------------------------------
 *
 * The IRs on robot's Foot:
 *
 *						_IR4__IR3_             _IR1__IR2_           	 
 *							        |           |                     	   |      |
 *						          |           |                     	   |      |
 *					       Left |           | Right               	   |      |
 *						     Foot |           | Foot                	   |      |
 *										  |           |                 Left Arm_|      |_Right Arm
 *							    ____|           |____                    |          |
 *                                                          IR6        IR5
 *                                                           |          |
 *  -----------------------------------------------------------------------
 *
 *  acronym(缩略词):
 *
 *  FAO: Foot Avoid Obstacle
 *  PD : Prevent Drop
 *
 *  ------------------------------------------------------------------------
 *
 */


typedef enum 
{
	RightFoot1_IR   = 0,
  RightFoot2_IR   = 1,
  LeftFoot1_IR    = 2,
  LeftFoot2_IR    = 3,
	
  speach_IR_N     = 4,
  LeftFoot_IR     = 5,
  speach_IR       = 6,
  Reserved2_IR    = 7
} IR_TypeDef;



#endif

