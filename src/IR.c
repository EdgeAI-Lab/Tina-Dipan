
#include "includes.h"

#define IRn														 8

const uint16_t IR_PIN[IRn] = {GPIO_Pin_2,
															GPIO_Pin_3,
															GPIO_Pin_4,
															GPIO_Pin_5,
															GPIO_Pin_4,
															GPIO_Pin_5,
															GPIO_Pin_6,
															GPIO_Pin_7};

/*
 *
 *
 */

uint8_t IsDetected(uint8_t IR)
{
	uint8_t IrStatus;
	if(IR<4)
	{
		IrStatus = GPIO_ReadInputDataBit(GPIOE,IR_PIN[IR]);
	}
	else
	{
	  IrStatus = GPIO_ReadInputDataBit(GPIOG,IR_PIN[IR]);
	}
	return !IrStatus;
}

