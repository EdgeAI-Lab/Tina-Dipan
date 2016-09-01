/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM32-SK Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.10
* Programmer(s) : BAN
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_CFG_STARTTASK_PRIO				         3       //起始任务

//避障任务
#define  APP_CFG_UL_PRIO                       4       //超声波避障任务
#define  APP_CFG_PD_PRIO                       5       //机器人防跌落、循迹
#define	 APP_CFG_SuspendResume_PRIO            6       //有关机器人动作的任务挂起和恢复
//获取膝关节和腰部实时位置任务
#define  APP_CFG_GLWP_PRIO                     7	     //获取膝关节和腰部实时位置任务（GLWP: Get the Legs and Waist Position）
//串口通信任务
#define  APP_CFG_SERIAL_PRIO                   8       //串口接收任务
//腿部动作任务
#define  APP_CFG_RW_PRIO                       9       //动作执行任务(RW: Robot Walk)
#define  APP_CFG_RFS_PRIO                      10      //动作步骤任务(RFS: Robot Forward Step)
#define  APP_CFG_RBS_PRIO                      11      //动作步骤任务(RBS: Robot Backward Step)
//手臂任务
#define  APP_CFG_RRA_PRIO                      12      //右臂复位任务(RRA: Reset Right Arm)
#define  APP_CFG_RLA_PRIO                      13      //左臂复位任务(RLA: Reset Left Arm)
#define  APP_CFG_SRA_PRIO                      14      //右摆臂任务(SRA: Swing Right Arm)
#define  APP_CFG_SLA_PRIO                      15      //左摆臂任务(SLA: Swing Left Arm)	
//腰部任务
#define  APP_CFG_SW_PRIO                       16			 //摆腰任务(SW: Swing Waist)
//手腕和头部
#define  APP_CFG_SteerWork_PRIO								 17      //左右手腕和头部转动任务（SteerWork）
//眼睛
#define  APP_CFG_EE_PRIO                       18			 //眼睛任务(EE: Eye Expression)
//LED
#define  APP_CFG_LED_PRIO                      19			 //LED任务
//DANCE
#define  APP_CFG_DANCE_PRIO                    20			 //DANCE任务

//系统任务
#define  OS_PROBE_TASK_PRIO              (OS_LOWEST_PRIO - 3)
#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/
#define	 APP_CFG_SuspendResume_STK_SIZE			   128  //任务挂起和恢复
#define  APP_CFG_STARTTASK_STK_SIZE			       128  //开始任务
#define  APP_CFG_RW_STK_SIZE				           256  //机器人步行任务
#define  APP_CFG_RFS_STK_SIZE			             128  //机器人前进分步
#define  APP_CFG_RBS_STK_SIZE			             128  //机器人后退分步
#define  APP_CFG_GLWP_STK_SIZE			           128  //获取膝关节和腰部位置
#define  APP_CFG_ARM_STK_SIZE			             128  //手臂任务
#define  APP_CFG_UL_STK_SIZE			             128  //超声波
#define  APP_CFG_SERIAL_STK_SIZE			         128  //串口
#define  APP_CFG_EE_STK_SIZE                   128	//眼睛
#define	 APP_CFG_LED_STK_SIZE			             64   //LED装饰灯
#define  APP_CFG_DANCE_STK_SIZE                128  //DANCE舞蹈任务
/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED


#endif
