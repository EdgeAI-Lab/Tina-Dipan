
/*
*********************************************************************************************************
*
*                                            ROBOT CODE
*
*                                         Yingborui Big Robot
*                                              with the
*                                   CPU STM32F103VE & Driver DRV8848
*
* Filename      : usart.c
* Version       : 1.0.0
* Programmer(s) : Fan
*********************************************************************************************************
*/

#include "includes.h"

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
	return ch;
}

/************************************************************************************************/


/* **
 *Global Variable
 */
unsigned char serial2Rec[32];
unsigned char rx2Count=0;    //getOutCount=0;

unsigned char serial3Rec[32];
unsigned char rx3Count=0;

extern System myRobot;

extern OS_EVENT *ActionSem; 							////动作信号量(Forward)
extern OS_EVENT *BackwardSem;             //动作信号量(Backward)
extern OS_EVENT *WaistSem;                //摆腰信号量(WaistSem)
extern OS_EVENT *DanceSem;
extern RobotMode robotMode;
//extern u8 rwFlag;
extern u8 swFlag;
extern u8 rrwFlag;

//add by frank 2015-09-15T16:52
extern INT8U actionFlag;
extern INT8U calibrationLFlag, calibrationRFlag;

//add by cgy 
extern  u8 WaistMode;   //robot swing waist
#define SwingToLeft  	0 //向左摆腰
#define SwingToRight 	1 //向右摆腰
#define Swing         2 //左右腰部
#define WaistStop     3 //腰部复位





/* ---------------------------------------------------------------------------*/

/**
 * Description :init the usart
 */
/* ---------------------------------------------------------------------------*/
void UsartInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef Usart_InitStr;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);
	USART_DeInit(USART1);
	USART_DeInit(USART2);
	USART_DeInit(USART3);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Configure USART1 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART2 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* USART3 Tx PD8 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Configure USART1 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART2 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* USART3 Rx PD9 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//Remap USART2
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
	
	//Remap USART3
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	
	USART_StructInit(&Usart_InitStr);

//
	Usart_InitStr.USART_BaudRate			     	= 9600;
	Usart_InitStr.USART_WordLength					= USART_WordLength_8b;
	Usart_InitStr.USART_StopBits						= USART_StopBits_1;
	Usart_InitStr.USART_Parity							= USART_Parity_No;
	Usart_InitStr.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	Usart_InitStr.USART_Mode								= USART_Mode_Rx | USART_Mode_Tx;
//
	USART_Init(USART1, &Usart_InitStr);
	USART_Init(USART2, &Usart_InitStr);
	USART_Init(USART3, &Usart_InitStr);
//
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//
	USART_Cmd(USART1, ENABLE); 
	USART_Cmd(USART2, ENABLE);    
	USART_Cmd(USART3, ENABLE);

}


/* ============================================================================*/
/**
 * Description :Blueteeth translate dma set
 *
 * Input    :buf   ->memmery address
 * Input    :length->translate data length
 */
/* ============================================================================*/
//void Serial_SendSetDMA(const unsigned char *buf, const unsigned short length)
//{
//	DMA_InitTypeDef DMA_InitStr;
//	DMA_Cmd(DMA1_Channel4, DISABLE);
//	DMA_InitStr.DMA_PeripheralBaseAddr	= (uint32_t)(&((USART_TypeDef*)USART2)->DR);
//	DMA_InitStr.DMA_MemoryBaseAddr		= (unsigned long)buf;
//	DMA_InitStr.DMA_DIR					= DMA_DIR_PeripheralDST;
//	DMA_InitStr.DMA_BufferSize			= length;
//	DMA_InitStr.DMA_PeripheralInc		= DMA_PeripheralInc_Disable;
//	DMA_InitStr.DMA_MemoryInc			= DMA_MemoryInc_Enable;
//	DMA_InitStr.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
//	DMA_InitStr.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
//	DMA_InitStr.DMA_Mode				= DMA_Mode_Normal;
//	DMA_InitStr.DMA_Priority			= DMA_Priority_High;
//	DMA_InitStr.DMA_M2M					= DMA_M2M_Disable;

//	DMA_Init(DMA1_Channel4, &DMA_InitStr);
//	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
//	DMA_Cmd(DMA1_Channel4, ENABLE);
//}

/* ============================================================================*/
/**
 * Description :Send Blueteeth data,use dma
 *
 * Input    :buf   ->  translate data address
 * Input    :length->  data length
 */
/* ============================================================================*/
//void Serial_SendDatas(unsigned char *buf)
//{
//	unsigned char length;
//#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
//	OS_CPU_SR  cpu_sr = 0;
//#endif
//	OS_ENTER_CRITICAL();
//	length=buf[4]+5;
//	if (serialsend.count)
//	{
//		serialsend.count++;
//		serialsend.end->next = (SerialSendNode *)buf;
//		serialsend.end = (SerialSendNode *)buf;
//	}
//	else
//	{
//		serialsend.count ++;
//		serialsend.ptr = (SerialSendNode *)buf;
//		serialsend.end = (SerialSendNode *)buf;
//		Serial_SendSetDMA(buf, length);
//		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
//	}
//	OS_EXIT_CRITICAL();

//}

//串口接收任务
void App_SerialTask(void *pdata)
{
	  unsigned char serialRec=0;
		while (1)
		{	
			 (void)pdata;

			 if(serial2Rec[0] != 0)
			 {
				  serialRec = serial2Rec[0];
					rx2Count = 0;
					serial2Rec[0] = 0;
			 }
			 else if(serial3Rec[0] != 0)
			 {
				  serialRec = serial3Rec[0];
					rx3Count = 0;
					serial3Rec[0] = 0;
			 }
			 switch(serialRec)
			 {
				  // 2:Robot Forward
					case RobotWalkForward:
//							 actionFlag = 1;			//1：执行摆臂动作 0：不执行摆臂动作
//							 robotMode = Forward;
//							 OSSemPost(ActionSem);
								LeftWheelMove(80);
									RightWheelMove(80);	
					
					
							 break;
					
					// 3:Robot Backward
					case RobotWalkBackward:				
//							 robotMode=Backward;
//							 actionFlag = 1;
//							 OSSemPost(BackwardSem);//发送信号量
									LeftWheelMove(10);
									RightWheelMove(10);
									
							 break;
					
					// 4:Robot Turn Left
					case RobotTurnLeft:
//							 robotMode=ToLeft;
//							 OSSemPost(ActionSem);
					
									LeftWheelMove(20);
									RightWheelMove(80);
//					WaistMove(20);
					
							 break;
					
					// 5:Robot Turn Right
					case RobotTurnRight:
//							 robotMode=ToRight;
//							 OSSemPost(ActionSem);
					
					LeftWheelMove(80);
					RightWheelMove(20);
//					WaistMove(80);
							 break;
					 
					// 6:Robot Slide Forward
					case RobotSlideForward:
						   robotMode=SlideForward;
					     break;
					
					// 7:Robot Slide Backward
					case RobotSlideBackward:
						   robotMode=SlideBackward;
					     break;
					
					// 8:Robot Head Swing 
					case HeadSwing:
							 HeadMove(50);
						   OSTimeDlyHMSM(0,0,0,500);
               HeadMove(90);
							 OSTimeDlyHMSM(0,0,0,500);
               HeadMove(70);
					     break;			

					// 9:Robot Wave Left Arm 
					case WaveLeftArm:
						
							 //Robot Left Arm Up
							 LeftArmMove(80);
							 OSTimeDlyHMSM(0,0,1,0);
							 LeftArmMove(50);
					
					     //Robot Left Wrist Inside
							 LeftWristMove(50);	
						   OSTimeDlyHMSM(0,0,1,0);
							 LeftWristMove(70);
					
							 //Robot Left Wrist Outside
							 LeftWristMove(90);	
						   OSTimeDlyHMSM(0,0,1,0);
							 LeftWristMove(70);
					
						   //Robot Left Arm Down
							 LeftArmMove(25);
							 while(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9));  //PG9	 
							 LeftArmMove(80);
							 OSTimeDlyHMSM(0,0,0,400);
							 LeftArmMove(50);
							
					     break;
					
					// 10:Robot Wave Right Arm 
					case WaveRightArm:
							
					     //Robot Right Arm Up
							 RightArmMove(80);
							 OSTimeDlyHMSM(0,0,1,0);
							 RightArmMove(50);
					
							 //Robot Right Wrist Inside
						   RightWristMove(50);	
						   OSTimeDlyHMSM(0,0,1,0);
							 RightWristMove(70);
					
							//Robot Right Wrist Outside
							 RightWristMove(90);	
						   OSTimeDlyHMSM(0,0,1,0);
							 RightWristMove(70);
					
							 //Robot Right Arm Down
							 RightArmMove(25);
							 while(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10));  //PG10	
							 RightArmMove(80);					
							 OSTimeDlyHMSM(0,0,0,400);
							 RightArmMove(50);
					
					     break;
				
		
				  // 11:Robot Waist Swing
					case WaistSwing:
							 WaistMode = Swing;
					     break;
					
						
					// 12:Dance 
					case Dance:
						  OSTaskResume(20); //恢复舞蹈任务	
							OSSemPost(DanceSem);//发送信号量
					     break;
						
					// 13:Robot Swing Waist 
//					case TurnOffDecorateLeds:
//							
//					     break;
						
					// 126:suspend dance task 
					case SuspendDanceTask:
							OSTaskSuspend(20); //挂起舞蹈任务	
					     break;
						
					// 127:Robot Stop
					case AllJointMotorStop:
							//非前进模式停止机器人动作即可，不进行复位操作
//							if((robotMode == StopButNoResetRobot) || (robotMode ==TurnRight) || (robotMode ==TurnLeft))
//							if((robotMode == Forward) || (robotMode ==TurnRight) || (robotMode ==TurnLeft))
//							{
//								robotMode=ToStop;
//							}else
//							{
//								robotMode=Stop;
//							}
//							WaistMode = WaistStop;
	//						if(swFlag) //摆腰停止后，仅复位腰部，其他部位不动
	//						{
	//							swFlag = 0;
	//							rwFlag = 1;
	//						}
							
							//制动
	// 						LeftFootMove(50);
	// 						LeftKneeMove(50);
	// 						LeftArmMove(50);
	// 						LeftWristMove(50);
	// 						HeadMove(50);
	// 						WaistMove(50);
	// 						RightFootMove(50);
	// 						RightKneeMove(50);
	// 						RightArmMove(50);
	// 						RightWristMove(50);
	
					LeftWheelMove(50);
					RightWheelMove(50);
	
								break;					   				  
			}
			serialRec = 0;
			OSTimeDlyHMSM(0,0,0,5);
		}
}


/*
 * 根据指令执行相应的动作（组合动作）
 *
 *
 */

void ExecuteAction(u8 action){
				
		 switch(action)
		 {
			// 2:Robot Forward
			case RobotWalkForward:
					 actionFlag = 1;			//1：执行摆臂动作 0：不执行摆臂动作
					 robotMode=Forward;
					 OSSemPost(ActionSem);
					 break;
			
			// 3:Robot Backward
			case RobotWalkBackward:				
					 robotMode=Backward;
					 actionFlag = 1;
					 OSSemPost(BackwardSem);//发送信号量
					 break;
			
			// 4:Robot Turn Left
			case RobotTurnLeft:
					 robotMode=ToLeft;
					 OSSemPost(ActionSem);
					 break;
			
			// 5:Robot Turn Right
			case RobotTurnRight:
					 robotMode=ToRight;
					 OSSemPost(ActionSem);
					 break;
			 
			// 6:Robot Slide Forward
			case RobotSlideForward:
					 robotMode=SlideForward;
					 break;
			
			// 7:Robot Slide Backward
			case RobotSlideBackward:
					 robotMode=SlideBackward;
					 break;
			
			// 8:Robot Head Swing 
			case HeadSwing:
					 HeadMove(50);
					 OSTimeDlyHMSM(0,0,0,500);
					 HeadMove(90);
					 OSTimeDlyHMSM(0,0,0,500);
					 HeadMove(70);
					 break;			

			// 9:Robot Wave Left Arm 
			case WaveLeftArm:
				
					 //Robot Left Arm Up
					 LeftArmMove(80);
					 OSTimeDlyHMSM(0,0,1,0);
					 LeftArmMove(50);
			
					 //Robot Left Wrist Inside
					 LeftWristMove(50);	
					 OSTimeDlyHMSM(0,0,1,0);
					 LeftWristMove(70);
			
					 //Robot Left Wrist Outside
					 LeftWristMove(90);	
					 OSTimeDlyHMSM(0,0,1,0);
					 LeftWristMove(70);
			
					 //Robot Left Arm Down
					 LeftArmMove(25);
					 while(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9));  //PG9	 
					 LeftArmMove(80);
					 OSTimeDlyHMSM(0,0,0,400);
					 LeftArmMove(50);
					
					 break;
			
			// 10:Robot Wave Right Arm 
			case WaveRightArm:
					
					 //Robot Right Arm Up
					 RightArmMove(80);
					 OSTimeDlyHMSM(0,0,1,0);
					 RightArmMove(50);
			
					 //Robot Right Wrist Inside
					 RightWristMove(50);	
					 OSTimeDlyHMSM(0,0,1,0);
					 RightWristMove(70);
			
					//Robot Right Wrist Outside
					 RightWristMove(90);	
					 OSTimeDlyHMSM(0,0,1,0);
					 RightWristMove(70);
			
					 //Robot Right Arm Down
					 RightArmMove(25);
					 while(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10));  //PG10	
					 RightArmMove(80);					
					 OSTimeDlyHMSM(0,0,0,400);
					 RightArmMove(50);
			
					 break;
		

			// 11:Robot Waist Swing
			case WaistSwing:
					 WaistMode = Swing;
					 break;
			
				
			// 12:Robot Move Legs 
			case Dance:
					
					 break;
				
			// 12:Robot Swing Waist 
	//					case TurnOnDecorateLeds:
	//							
	//					     break;
				
			// 13:Robot Swing Waist 
	//					case TurnOffDecorateLeds:
	//							
	//					     break;
				
			// 14:Robot Swing Waist 
	//					case BlinkDecorateLeds:
	//							
	//					     break;
				
			// 127:Robot Stop
			case AllJointMotorStop:
					//非前进模式停止机器人动作即可，不进行复位操作
	//							if((robotMode == StopButNoResetRobot) || (robotMode ==TurnRight) || (robotMode ==TurnLeft))
//					if((robotMode == Forward) || (robotMode ==TurnRight) || (robotMode ==TurnLeft))
//					{
//						robotMode=ToStop;
//					}else
//					{
//						robotMode=Stop;
//					}
//					WaistMode = WaistStop;
	//						if(swFlag) //摆腰停止后，仅复位腰部，其他部位不动
	//						{
	//							swFlag = 0;
	//							rwFlag = 1;
	//						}
					
					//制动
	// 						LeftFootMove(50);
	// 						LeftKneeMove(50);
	// 						LeftArmMove(50);
	// 						LeftWristMove(50);
	// 						HeadMove(50);
	// 						WaistMove(50);
	// 						RightFootMove(50);
	// 						RightKneeMove(50);
	// 						RightArmMove(50);
	// 						RightWristMove(50);
						break;					   				  
	}
				 
}




