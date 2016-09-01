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

#define  APP_CFG_STARTTASK_PRIO				         3       //��ʼ����

//��������
#define  APP_CFG_UL_PRIO                       4       //��������������
#define  APP_CFG_PD_PRIO                       5       //�����˷����䡢ѭ��
#define	 APP_CFG_SuspendResume_PRIO            6       //�йػ����˶������������ͻָ�
//��ȡϥ�ؽں�����ʵʱλ������
#define  APP_CFG_GLWP_PRIO                     7	     //��ȡϥ�ؽں�����ʵʱλ������GLWP: Get the Legs and Waist Position��
//����ͨ������
#define  APP_CFG_SERIAL_PRIO                   8       //���ڽ�������
//�Ȳ���������
#define  APP_CFG_RW_PRIO                       9       //����ִ������(RW: Robot Walk)
#define  APP_CFG_RFS_PRIO                      10      //������������(RFS: Robot Forward Step)
#define  APP_CFG_RBS_PRIO                      11      //������������(RBS: Robot Backward Step)
//�ֱ�����
#define  APP_CFG_RRA_PRIO                      12      //�ұ۸�λ����(RRA: Reset Right Arm)
#define  APP_CFG_RLA_PRIO                      13      //��۸�λ����(RLA: Reset Left Arm)
#define  APP_CFG_SRA_PRIO                      14      //�Ұڱ�����(SRA: Swing Right Arm)
#define  APP_CFG_SLA_PRIO                      15      //��ڱ�����(SLA: Swing Left Arm)	
//��������
#define  APP_CFG_SW_PRIO                       16			 //��������(SW: Swing Waist)
//�����ͷ��
#define  APP_CFG_SteerWork_PRIO								 17      //���������ͷ��ת������SteerWork��
//�۾�
#define  APP_CFG_EE_PRIO                       18			 //�۾�����(EE: Eye Expression)
//LED
#define  APP_CFG_LED_PRIO                      19			 //LED����
//DANCE
#define  APP_CFG_DANCE_PRIO                    20			 //DANCE����

//ϵͳ����
#define  OS_PROBE_TASK_PRIO              (OS_LOWEST_PRIO - 3)
#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/
#define	 APP_CFG_SuspendResume_STK_SIZE			   128  //�������ͻָ�
#define  APP_CFG_STARTTASK_STK_SIZE			       128  //��ʼ����
#define  APP_CFG_RW_STK_SIZE				           256  //�����˲�������
#define  APP_CFG_RFS_STK_SIZE			             128  //������ǰ���ֲ�
#define  APP_CFG_RBS_STK_SIZE			             128  //�����˺��˷ֲ�
#define  APP_CFG_GLWP_STK_SIZE			           128  //��ȡϥ�ؽں�����λ��
#define  APP_CFG_ARM_STK_SIZE			             128  //�ֱ�����
#define  APP_CFG_UL_STK_SIZE			             128  //������
#define  APP_CFG_SERIAL_STK_SIZE			         128  //����
#define  APP_CFG_EE_STK_SIZE                   128	//�۾�
#define	 APP_CFG_LED_STK_SIZE			             64   //LEDװ�ε�
#define  APP_CFG_DANCE_STK_SIZE                128  //DANCE�赸����
/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED


#endif
