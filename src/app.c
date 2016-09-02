
/*
*********************************************************************************************************
*
*                                            ROBOT CODE
*
*                                         Yingborui Big Robot
*                                              with the
*                                   CPU STM32F103ZE & Driver DRV8848
*
* Filename      : app.c
* Version       : 2.0.0
* Programmer(s) : Fan and Cao
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "includes.h"
char SDbuf[1024];
uint8_t PDrop=0;
uint8_t PDropR=0; 
uint8_t PDropL=0;  			    //循迹、防跌落任务
uint8_t	PD_RightFoot1_IR=0;
uint8_t	PD_RightFoot2_IR=0;
uint8_t	PD_LeftFoot1_IR=0;
uint8_t	PD_LeftFoot2_IR=0;

uint8_t UltrTask=0;			//超声波避障任务
uint8_t SuspendResume=0;//机器人动作任务挂起和恢复

uint8_t speach_mode = 0;
uint8_t speach_flag = 0;
uint8_t light_mode = 0;

const float Pi  = 3.14159265;//
const int   D   = 80;//脚轮直径(mm)
const int   RPM = 52;//脚部电机转速(RPM)
/***************步行分辨率*********************
*RESOLUTION     FootSpeed     FootSpeedBack
*   500            18			         82               90%
*   400            12              88
*   300            6               94
*   250            3               97
*
************************************/
#define RESOLUTION 250 //步行分辨率
#define FootSpeed  3   //脚上速度
#define FootSpeedBack  97  //脚上速度
#define THEANGLE 15     //膝关节弯曲角度
#define X 2             //走路任务调度时间(ms)

/***********************摆腰相关宏定义*******************************/
extern  u8 rwFlag;
#define SwingToLeft  	0 //向左摆腰
#define SwingToRight 	1 //向右摆腰
#define Swing         2 //左右腰部
/******************************************ADC有关参数************************************************/
#define N 10
extern u16 adc_10[N];
extern u16 adc_11[N];
extern u16 adc_12[N];
extern u8 LeftKnee;
extern u8 RightKnee;
extern u8 Waist;
/*******************************************************************************************************/

/*
*********************************************************************************************************
*                                      GLOBAL VARIABLES
*********************************************************************************************************
*/
System myRobot;
RobotMode robotMode;
extern FIL CfgFile,oledFile;
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
//系统启动任务
static void App_StartTask(void *pdata);
__align(8) static OS_STK APP_StartSTK[APP_CFG_STARTTASK_STK_SIZE];

//超声波避障
static void App_Ultrasonic(void *pdata);
__align(8) static OS_STK APP_UltrasonicARMSTK[APP_CFG_UL_STK_SIZE];

//机器人循迹、防跌落(PD: Prevent Drop)
static void App_PDTask(void *pdata);
__align(8) static OS_STK APP_PD_STK[APP_CFG_ARM_STK_SIZE];

//机器人动作任务挂起和恢复
static void App_SuspendResumeTask(void *pdata);
__align(8) static OS_STK APP_SuspendResumeSTK[APP_CFG_SuspendResume_STK_SIZE];

//获取膝关节位置任务(GLWP: Get the Legs and Waist Position)
static void App_GLWPTask(void *pdata);
__align(8) static OS_STK APP_GLWPStk[APP_CFG_GLWP_STK_SIZE];

//串口接收任务
void App_SerialTask(void *pdata);
__align(8) static OS_STK APP_SERIALSTK[APP_CFG_SERIAL_STK_SIZE];

//机器人步行任务(RW: Robot Walk)
static void App_RWTask(void *pdata);
__align(8) static OS_STK APP_RWStk[APP_CFG_RW_STK_SIZE];

//机器人前进分步(RFS: Robot Forward Step)
static void App_RFSTask(void *pdata);
__align(8) static OS_STK APP_RFSStk[APP_CFG_RFS_STK_SIZE];

//机器人后退分步(RBS: Robot Backward Step)
static void App_RBSTask(void *pdata);
__align(8) static OS_STK APP_RBSStk[APP_CFG_RBS_STK_SIZE];

//右臂复位任务(RRA: Reset Right Arm )
static void App_RRATask(void *pdata);	
__align(8) static OS_STK APP_RRAStk[APP_CFG_ARM_STK_SIZE];

//左臂复位任务(RLA: Reset Left Arm )
static void App_RLATask(void *pdata);
__align(8) static OS_STK APP_RLAStk[APP_CFG_ARM_STK_SIZE];

//右摆臂任务(SRA: Swing Right Arm)
static void App_SRATask(void *pdata);
__align(8) static OS_STK APP_SRAStk[APP_CFG_ARM_STK_SIZE];

//左摆臂任务(SLA: Swing Left Arm)
static void App_SLATask(void *pdata);
__align(8) static OS_STK APP_SLAStk[APP_CFG_ARM_STK_SIZE];

//摆腰任务(SW: Swing Waist)
static void App_SWTask(void *pdata);
__align(8) static OS_STK APP_SWStk[APP_CFG_ARM_STK_SIZE];

//舵机控制手腕和头转动任务(SteerWork：左右手腕和头部的舵机控制)
static void App_SteerWorkTask(void *pdata);
__align(8) static OS_STK APP_SteerWorkStk[APP_CFG_ARM_STK_SIZE];

//机器人眼睛表情(EE: Eye Expression)
static void App_EETask(void *pdata);
__align(8) static OS_STK APP_EE_STK[APP_CFG_EE_STK_SIZE];

//LED任务
static void App_LedTask(void *pdata);
__align(8) static OS_STK APP_LED_STK[APP_CFG_LED_STK_SIZE];

//Dance任务
static void App_DanceTask(void *pdata);
__align(8) static OS_STK APP_DANCE_STK[APP_CFG_DANCE_STK_SIZE];

INT8U flagCount=0;

static void App_EventCreate(void);
static void App_TaskCreate(void);
float Get_Angle(u16 *adc,u8 ch,u8 *i);
unsigned short ReadSpeed(unsigned char witch);

/*
 *********************************************************************************************************
 *                                      定义事件
 *********************************************************************************************************
 */
OS_EVENT *ActionSem;               //动作信号量(Forward)
OS_EVENT *BackwardSem;             //动作信号量(Backward)
OS_EVENT *WaistSem;								 //动作信号量(Waist)
// OS_EVENT *EyesSem;								 //动作信号量(Eyes)
OS_EVENT *DanceSem;


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
int  main (void)
{

	__asm("CPSID I"); 
	OSInit();				//初始化uC/OS-II
	OSTaskCreateExt((void (*)(void *)) App_StartTask,										//任务地址
			(void			*) 0,																									//传递给任务的参数
			(OS_STK			*)&APP_StartSTK[APP_CFG_STARTTASK_STK_SIZE-1],			//堆栈顶指针
			(INT8U			 ) APP_CFG_STARTTASK_PRIO,													//优先级
			(INT16U			 ) APP_CFG_STARTTASK_PRIO,													//任务的识别
			(OS_STK			*)&APP_StartSTK[0],																	//堆栈栈低指针
			(INT32U			 ) APP_CFG_STARTTASK_STK_SIZE,											//堆栈容量
			(void			*) 0,																									//附加数据域指针
			(INT16U			 )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));			//操作选型
	OSStart();

	return (0);
}

/*
*********************************************************************************************************
*                                            App_StartTask()
*
* Description : 系统任务初始化
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*********************************************************************************************************
*/
static void App_StartTask(void *pdata)
{
	(void)pdata;

	//创建事件
	App_EventCreate();
	//系统滴答设置
	SysTick_Configure();
	//机器人系统初始化
	RobotSystemInit();
	//创建任务
	App_TaskCreate();
	
	 //挂起任务
	 OSTaskSuspend(4); //超声波避障任务
	 OSTaskSuspend(5); //机器人防跌落(PD:  Prevent Drop)
	 OSTaskSuspend(6); //机器人动作任务挂起和恢复
// 	 OSTaskSuspend(7); //GLWP: Get the Legs and Waist Position//储存膝关节位置信息，用于指导机器人摆臂
// 	 OSTaskSuspend(8); //串口接收任务
// 	 OSTaskSuspend(9); //动作执行任务(RW: Robot Walk)
// 	 OSTaskSuspend(10); //动作步骤任务(RFS: Robot Forward Step)
//	 OSTaskSuspend(11); //动作步骤任务(RBS: Robot Backward Step)
//	 OSTaskSuspend(12); //右臂复位任务(RRA: Reset Right Arm)
//	 OSTaskSuspend(13); //左臂复位任务(RLA: Reset Left Arm)
//	 OSTaskSuspend(14); //右摆臂任务(SRA: Swing Right Arm)
//	 OSTaskSuspend(15); //左摆臂任务(SLA: Swing Left Arm)	
//	 OSTaskSuspend(16); //摆腰任务(SW: Swing Waist)
	 OSTaskSuspend(17); //左右手腕和头部转动任务（SteerWork：Steer control）
//	 OSTaskSuspend(18); //眼睛任务(EE: Eye Expression)
//	 OSTaskSuspend(19); //LED任务(LED:decorative lighting)
//	 OSTaskSuspend(20); //舞蹈任务
//挂起自己
	 OSTaskDel(OS_PRIO_SELF);
}


/*
*********************************************************************************************************
*                                             App_EventCreate()
*
* Description : 创建事件
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
static  void  App_EventCreate (void)
{
//	INT8U err;
	//Forward
	ActionSem=OSSemCreate(0);
	
	//Backward
	BackwardSem=OSSemCreate(0);
	
	//Waist
 	WaistSem=OSSemCreate(0);
	
	//Eyes
// 	EyesSem=OSSemCreate(0);
	
	//Dance
 	DanceSem=OSSemCreate(0);
	
}


/*
*********************************************************************************************************
*                                             App_TaskCreate()
*
* Description : 创建任务
*
* Argument(s) : none.
*
* Return(s)   : none.
*  
* Note        : 共有17个任务
*********************************************************************************************************
*/
static void App_TaskCreate(void)
{
	OSTaskCreateExt(//获取腿部及腰部当前位置任务(GLWP: Get the Legs and Waist Position)
			(void (*)(void *)) App_GLWPTask,  
			(void			*) 0,
			(OS_STK			*)&APP_GLWPStk[APP_CFG_GLWP_STK_SIZE-1],
			(INT8U			 ) APP_CFG_GLWP_PRIO,
			(INT16U			 ) APP_CFG_GLWP_PRIO,
			(OS_STK			*)&APP_GLWPStk[0],
			(INT32U			 ) APP_CFG_GLWP_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
	OSTaskCreateExt(//机器人腿部步骤任务(RFS: Robot Forward Step)
			(void (*)(void *)) App_RFSTask,	
			(void			*) 0,
			(OS_STK			*)&APP_RFSStk[APP_CFG_RFS_STK_SIZE-1],
			(INT8U			 ) APP_CFG_RFS_PRIO,
			(INT16U			 ) APP_CFG_RFS_PRIO,
			(OS_STK			*)&APP_RFSStk[0],
			(INT32U			 ) APP_CFG_RFS_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
	OSTaskCreateExt(//机器人腿部步骤任务(RBS: Robot Backward Step)
			(void (*)(void *)) App_RBSTask,	
			(void			*) 0,
			(OS_STK			*)&APP_RBSStk[APP_CFG_RBS_STK_SIZE-1],
			(INT8U			 ) APP_CFG_RBS_PRIO,
			(INT16U			 ) APP_CFG_RBS_PRIO,
			(OS_STK			*)&APP_RBSStk[0],
			(INT32U			 ) APP_CFG_RBS_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	OSTaskCreateExt(//机器人腿部动作任务(RW: Robot Walk)
			(void (*)(void *)) App_RWTask,   
			(void			*) 0,
			(OS_STK			*)&APP_RWStk[APP_CFG_RW_STK_SIZE-1],
			(INT8U			 ) APP_CFG_RW_PRIO,
			(INT16U			 ) APP_CFG_RW_PRIO,
			(OS_STK			*)&APP_RWStk[0],
			(INT32U			 ) APP_CFG_RW_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);

	OSTaskCreateExt(//串口接收任务
			(void (*)(void *)) App_SerialTask,
			(void			*) 0,
			(OS_STK			*)&APP_SERIALSTK[APP_CFG_SERIAL_STK_SIZE-1],
			(INT8U			 ) APP_CFG_SERIAL_PRIO,
			(INT16U			 ) APP_CFG_SERIAL_PRIO,
			(OS_STK			*)&APP_SERIALSTK[0],
			(INT32U			 ) APP_CFG_SERIAL_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	OSTaskCreateExt(//右臂复位任务(RRA: Reset Right Arm )
			(void (*)(void *)) App_RRATask,
			(void			*) 0,
			(OS_STK			*)&APP_RRAStk[APP_CFG_ARM_STK_SIZE-1],
			(INT8U			 ) APP_CFG_RRA_PRIO,	
			(INT16U			 ) APP_CFG_RRA_PRIO,
			(OS_STK			*)&APP_RRAStk[0],
			(INT32U			 ) APP_CFG_ARM_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	OSTaskCreateExt(//左臂复位任务(RLA: Reset Left Arm)
			(void (*)(void *)) App_RLATask,
			(void			*) 0,
			(OS_STK			*)&APP_RLAStk[APP_CFG_ARM_STK_SIZE-1],
			(INT8U			 ) APP_CFG_RLA_PRIO,	
			(INT16U			 ) APP_CFG_RLA_PRIO,
			(OS_STK			*)&APP_RLAStk[0],
			(INT32U			 ) APP_CFG_ARM_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	OSTaskCreateExt(//机器人动作任务挂起和恢复
			(void (*)(void *)) App_SuspendResumeTask,
			(void			*) 0,
			(OS_STK			*)&APP_SuspendResumeSTK[APP_CFG_SuspendResume_STK_SIZE-1],
			(INT8U			 ) APP_CFG_SuspendResume_PRIO,
			(INT16U			 ) APP_CFG_SuspendResume_PRIO,
			(OS_STK			*)&APP_SuspendResumeSTK[0],
			(INT32U			 ) APP_CFG_SuspendResume_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	OSTaskCreateExt(//超声波
			(void (*)(void *)) App_Ultrasonic,
			(void			*) 0,
			(OS_STK			*)&APP_UltrasonicARMSTK[APP_CFG_UL_STK_SIZE-1],
			(INT8U			 ) APP_CFG_UL_PRIO,
			(INT16U			 ) APP_CFG_UL_PRIO,
			(OS_STK			*)&APP_UltrasonicARMSTK[0],
			(INT32U			 ) APP_CFG_UL_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	OSTaskCreateExt(//右摆臂任务(SRA: Swing Right Arm)
			(void (*)(void *)) App_SRATask,
			(void			*) 0,
			(OS_STK			*)&APP_SRAStk[APP_CFG_ARM_STK_SIZE-1],
			(INT8U			 ) APP_CFG_SRA_PRIO,	
			(INT16U			 ) APP_CFG_SRA_PRIO,
			(OS_STK			*)&APP_SRAStk[0],
			(INT32U			 ) APP_CFG_ARM_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	OSTaskCreateExt(//左摆臂任务(SLA: Swing Left Arm)
			(void (*)(void *)) App_SLATask,
			(void			*) 0,
			(OS_STK			*)&APP_SLAStk[APP_CFG_ARM_STK_SIZE-1],
			(INT8U			 ) APP_CFG_SLA_PRIO,	
			(INT16U			 ) APP_CFG_SLA_PRIO,
			(OS_STK			*)&APP_SLAStk[0],
			(INT32U			 ) APP_CFG_ARM_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	OSTaskCreateExt(//摆腰任务(SW: Swing Waist)
			(void (*)(void *)) App_SWTask,
			(void			*) 0,
			(OS_STK			*)&APP_SWStk[APP_CFG_ARM_STK_SIZE-1],
			(INT8U			 ) APP_CFG_SW_PRIO,	
			(INT16U			 ) APP_CFG_SW_PRIO,
			(OS_STK			*)&APP_SWStk[0],
			(INT32U			 ) APP_CFG_ARM_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	OSTaskCreateExt(//左右手腕和头部转动任务（SteerWork：Steer control）
			(void (*)(void *)) App_SteerWorkTask,
			(void			*) 0,
			(OS_STK			*)&APP_SteerWorkStk[APP_CFG_ARM_STK_SIZE-1],
			(INT8U			 ) APP_CFG_SteerWork_PRIO,	
			(INT16U			 ) APP_CFG_SteerWork_PRIO,
			(OS_STK			*)&APP_SteerWorkStk[0],
			(INT32U			 ) APP_CFG_ARM_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);

	OSTaskCreateExt(//机器人防跌落(PD:  Prevent Drop)
			(void (*)(void *)) App_PDTask,
			(void			*) 0,
			(OS_STK			*)&APP_PD_STK[APP_CFG_ARM_STK_SIZE-1],
			(INT8U			 ) APP_CFG_PD_PRIO,	
			(INT16U			 ) APP_CFG_PD_PRIO,
			(OS_STK			*)&APP_PD_STK[0],
			(INT32U			 ) APP_CFG_ARM_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
	/*
	 * 由于机器人的眼睛计较大，使用OLED的成本较高，所以目前决定使用LED灯泡
	 *
	 */
	OSTaskCreateExt(//机器人眼睛表情(EE: Eye Expression)
			(void (*)(void *)) App_EETask,
			(void			*) 0,
			(OS_STK			*)&APP_EE_STK[APP_CFG_EE_STK_SIZE-1],
			(INT8U			 ) APP_CFG_EE_PRIO,	
			(INT16U			 ) APP_CFG_EE_PRIO,
			(OS_STK			*)&APP_EE_STK[0],
			(INT32U			 ) APP_CFG_EE_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
	OSTaskCreateExt(//LED任务
			(void (*)(void *)) App_LedTask,  
			(void			*) 0,
			(OS_STK			*)&APP_LED_STK[APP_CFG_LED_STK_SIZE-1],
			(INT8U			 ) APP_CFG_LED_PRIO,
			(INT16U			 ) APP_CFG_LED_PRIO,
			(OS_STK			*)&APP_LED_STK[0],
			(INT32U			 ) APP_CFG_LED_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
	OSTaskCreateExt(//DANCE任务
			(void (*)(void *)) App_DanceTask,  
			(void			*) 0,
			(OS_STK			*)&APP_DANCE_STK[APP_CFG_DANCE_STK_SIZE-1],
			(INT8U			 ) APP_CFG_DANCE_PRIO,
			(INT16U			 ) APP_CFG_DANCE_PRIO,
			(OS_STK			*)&APP_DANCE_STK[0],
			(INT32U			 ) APP_CFG_DANCE_STK_SIZE,
			(void			*) 0,
			(INT16U			 ) OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
			);
			
}


/*
*********************************************************************************************************
*                                             App_RWTask()
*
* Description : 机器人步行任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* Notes				: 机器人走路的速度(或者说膝关节往返循环运动的速度)跟3个参数有关: 
*								1) 机器人走路任务的调度时间OSTimeDlyHMSM(0,0,0,x)
*								2) 机器人走路的分辨率WR
*               假如 x=3, WR=1000, 那么机器人每走一步所需的时间为 x*WR = 3*1000(ms) = 3(s)
*								3) 膝关节前后弯曲的角度(这个角度与膝关节的位移和脚位移有定量关系)
*								
*********************************************************************************************************
*/

#define POWER 20.0
unsigned short WR;//Walk Resolution(步行分辨率)

//Forward
unsigned char flagStart=0,actionStart=0;
unsigned char stage=0;

//Backward
unsigned char flagStart_Backward=0,actionStart_Backward=0;
unsigned char stage_Backward=0;

//校准手臂和头部(0: 默认开机校准)
INT8U calibrationLFlag = 0;
INT8U calibrationRFlag = 0;
INT8U calibrationHeadFlag = 1;

//是否挥动手臂标志位
INT8U actionFlag = 0;
unsigned short WRC = 0;//Walk Resolution Counter(步行分辨率计数器)
//RW: Robot Walk
static void App_RWTask(void *pdata)
{	

	(void)pdata;
	while (1)
	{

		//每隔2ms进行一次更新
		OSTimeDlyHMSM(0,0,0,X);
	}
}

/*
*********************************************************************************************************
*                                             App_GLWPTask()
*
* Description : 获取腿部（膝关节）和腰部位置
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* GLWP				: Get the Legs and Waist Position
*********************************************************************************************************
*/

float  positionArryL[500];
float  positionArryR[500];
float  WaistPosition=0;
static INT16U i=0,j=0;
//static int Line=0;
static void App_GLWPTask(void *pdata)
{
	(void)pdata;
	
	//开机校准
	if((Get_Angle(adc_10,ADC_Channel_10,&LeftKnee) != 0) || (Get_Angle(adc_11,ADC_Channel_11,&RightKnee) != 0))
		{
				  //开机初始化状态
// 				robotMode=Forward;
// 				actionFlag = 1;//1：执行摆臂动作 0：不执行摆臂动作
// 			
// 				robotMode=ToStop;
// 				calibrationLFlag = 0;
// 				calibrationRFlag = 0;
// 			
//   			OSSemPost(ActionSem);
// 			  robotMode=ToStopBackward;
//			  OSSemPost(BackwardSem);
			
			
		}
		
	while (1)
	{		
//		//获取左膝当前位置
//		positionArryR[i++] = myRobot.actionctrl.leftLeg.positionNow_Float  = Get_Angle(adc_10,ADC_Channel_10,&LeftKnee);
//		
//		//获取右膝当前位置
//		positionArryL[j++] = myRobot.actionctrl.rightLeg.positionNow_Float = Get_Angle(adc_11,ADC_Channel_11,&RightKnee);
		
		//获取腰部当前位置
		WaistPosition = myRobot.actionctrl.waist.positionNow_Float = Get_Angle(adc_12,ADC_Channel_12,&Waist);
		
// 		printf("%d:  L: %f  R: %f \n",Line++,positionArryR[i-1],positionArryL[j-1]);
		if(i == 500)
		{
			i = 0;
		}
		if(j == 500)
		{
			j = 0;
		}
		
		//腰部复位
   	resetWaist(WaistPosition);
		
		//每2ms检测一次
 		OSTimeDlyHMSM(0,0,0,2);
	}
}

/*
*********************************************************************************************************
*                                             App_RFSTask()
*
* Description : 任务获取动作信号量(ActionSem)之后,根据动作状态进行不同的行为(Forward)
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* RFS				  : Robot Forward Step
*********************************************************************************************************
*/
static void App_RFSTask(void *pdata)
{
	INT8U err;

	(void)pdata;
	while (1)
	{
		OSSemPend(ActionSem,0,&err);
		
		switch(stage)
		{
			case 0:
				myRobot.actionctrl.leftLeg.positionDest_Float  = THEANGLE;
				myRobot.actionctrl.rightLeg.positionDest_Float = -THEANGLE;
				//WR: Walk Resolution
				WR=RESOLUTION;//
				actionStart=1;	
				stage=1;
				flagStart=1;
				break;
			case 1:
				myRobot.actionctrl.leftLeg.positionDest_Float  = 0;
				myRobot.actionctrl.rightLeg.positionDest_Float = 0;
				//WR: Walk Resolution
				WR=RESOLUTION;//
				actionStart=1;
				stage=2;
				flagStart=1;
				break;
			case 2:
				myRobot.actionctrl.leftLeg.positionDest_Float  = -THEANGLE;
				myRobot.actionctrl.rightLeg.positionDest_Float = THEANGLE;
				//WR: Walk Resolution
				WR=RESOLUTION;//
				actionStart=1;
				stage=3;
				flagStart=1;
				break;
			case 3:
				myRobot.actionctrl.leftLeg.positionDest_Float  = 0;
				myRobot.actionctrl.rightLeg.positionDest_Float = 0;
				//WR: Walk Resolution
				WR=RESOLUTION;//
				actionStart=1;	
				stage=0;
				flagStart=1;
				break;
			default:
			break;
		}
	}
}

/*
*********************************************************************************************************
*                                             App_RBSTask()
*
* Description : 任务获取动作信号量(BackwardSem)之后,根据动作状态进行不同的行为(Backward)
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* RBS				  : Robot Backward Step
*********************************************************************************************************
*/

static void App_RBSTask(void *pdata)
{
	INT8U err;

	(void)pdata;
	while (1)
	{
		OSSemPend(BackwardSem,0,&err);

		switch(stage_Backward)
		{
			case 0:
				//此程序只让其进行走路。
				//LegLength腿部长度
				//Scale脚上滑动距离与脚部电机转动角度比例。
				myRobot.actionctrl.leftLeg.positionDest_Float  = -THEANGLE;
				myRobot.actionctrl.rightLeg.positionDest_Float = THEANGLE;
				WR=RESOLUTION;
				actionStart_Backward=1;	
				stage_Backward=1;
				flagStart_Backward=1;
				//脚上的运动等具体运动的时候进行计算
				break;
			case 1:
				myRobot.actionctrl.leftLeg.positionDest_Float  = 0;
				myRobot.actionctrl.rightLeg.positionDest_Float = 0;
				WR=RESOLUTION;	
				actionStart_Backward=1;
				stage_Backward=2;
				flagStart_Backward=1;
				break;
			case 2:
				myRobot.actionctrl.leftLeg.positionDest_Float  = THEANGLE;
				myRobot.actionctrl.rightLeg.positionDest_Float = -THEANGLE;
				WR=RESOLUTION;	
				actionStart_Backward=1;
				stage_Backward=3;
				flagStart_Backward=1;
				break;
			case 3:
				myRobot.actionctrl.leftLeg.positionDest_Float  = 0;
				myRobot.actionctrl.rightLeg.positionDest_Float = 0;
				WR=RESOLUTION;
				actionStart_Backward=1;	
				stage_Backward=0;
				flagStart_Backward=1;
				break;
			default:
			  break;
		}
	}
}

/*
*********************************************************************************************************
*                                             App_RRATask()
*
* Description : 右臂复位任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* RRA				  : Reset Right Arm
*********************************************************************************************************
*/

//右臂复位任务 ---- 碰撞开关PG10 电机驱动TIM3_CH3  ADC右腿PC1
static void App_RRATask(void *pdata)
{
 	calibrationRFlag = 0;
	
	(void)pdata;
	
	while(1)
	{
//		if(!calibrationRFlag)
//		{
//			    RightArmMove(25);
////						printf("R Backward\n");
//					if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10))
//					{
////						printf("R OK\n");
//						RightArmMove(80);
//						OSTimeDlyHMSM(0,0,0,500);
//// 						GPIO_SetBits(GPIOG,GPIO_Pin_14);
//						RightArmMove(50);
//						calibrationRFlag = 1;
//					}
//	   }
		 OSTimeDlyHMSM(0,0,0,2); 
	}
}


/*
*********************************************************************************************************
*                                             App_RLATask()
*
* Description : 左臂复位任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* RLA				  : Reset Left Arm
*********************************************************************************************************
*/
//左臂复位任务 ----  碰撞开关PG9  电机驱动TIM3_CH2  ADC左腿PC0
//注意：50%<PWM 前摆 50%>PWM 后摆 
static void App_RLATask(void *pdata)
{
 	calibrationLFlag = 0;		 
	(void)pdata;
	
	while(1)
	{
//		if(!calibrationLFlag)
//		{
//			LeftArmMove(25);
//			if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9))  //PG9
//				{
//						LeftArmMove(80);
//						OSTimeDlyHMSM(0,0,0,500);
//						LeftArmMove(50);
//						calibrationLFlag = 1;
//				}
//		 }
		OSTimeDlyHMSM(0,0,0,2);
	}
}

/*
*********************************************************************************************************
*                                             App_SRATask()
*
* Description : 摆右臂任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* SRA					: Swing Right Arm
*********************************************************************************************************
*/
static void App_SRATask(void *pdata)
{
	while(1)
	{
//		if(actionFlag)
//		 {
//				if((robotMode==Forward) && (actionStart==1) && (WRC<WR))
//				{
//						if(stage==1 || stage==2)
//						{	
//								RightArmMove(15);						
//								if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10))
//								{
//									RightArmMove(50);
//								}
//						}
//						if(stage==0 || stage==3)
//						{
//								RightArmMove(85);
//						}
//				}
//				else if((robotMode==Backward) && (actionStart_Backward==1) && (WRC<WR))
//				{
//						if(stage_Backward==0 || stage_Backward==3)
//						{	
//								RightArmMove(15);						
//								if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10))
//								{
//									RightArmMove(50);
//								}
//						}
//						if(stage_Backward==1 || stage_Backward==2)
//						{
//								RightArmMove(85);
//						}
//				}
//		    else
//				{
//						RightArmMove(50);
//				}
//		 }
		OSTimeDlyHMSM(0,0,0,5);
	}
}


/*
*********************************************************************************************************
*                                             App_SLATask()
*
* Description : 摆左臂任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
*SLA					: Swing Left Arm 
*********************************************************************************************************
*/
static void App_SLATask(void *pdata)
{
	while(1)
	{
//		if(actionFlag)
//		{	
//				if((robotMode==Forward) && (actionStart==1) && (WRC<WR))
//				{
//						if(stage==0 || stage==3)
//						{
//								LeftArmMove(15);
//								if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9))
//								{
//									LeftArmMove(50);
//								}
//						}
//						if(stage==1 || stage==2)
//						{
//								LeftArmMove(85);
//						}
//				}
//				else if((robotMode==Backward) && (actionStart_Backward==1) && (WRC<WR))
//				{
//						if(stage_Backward==1 || stage_Backward==2)
//						{	
//								LeftArmMove(15);						
//								if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9))
//								{
//									LeftArmMove(50);
//								}
//						}
//						if(stage_Backward==0 || stage_Backward==3)
//						{
//								LeftArmMove(85);
//						}
//				}
//				else
//				{
//						LeftArmMove(50);
//				}
//		}
		 OSTimeDlyHMSM(0,0,0,5);
	}
}


/*
*********************************************************************************************************
*                                             App_SWTask()
*
* Description : 摆腰臂任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* SW					: SW: Swing Waist
*********************************************************************************************************
*/
u8 swFlag = 0;
u8 WaistMode=3;
static void App_SWTask(void *pdata)
{	
	while(1)
	{	
		if(WaistMode == SwingToLeft)
		{
			moveWaist(-5,0,20);
		}
		else if(WaistMode == SwingToRight)
		{
			moveWaist(0,5,20);
		}
		else if(WaistMode == Swing)
		{
			moveWaist(-5,5,30);
		}
		else
		{
			rwFlag = 1;
		}
		OSTimeDlyHMSM(0,0,0,10);
	}
}

/*
*********************************************************************************************************
*                                             App_SteerWorkTask()
*
* Description : 手腕和头部转动任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* RRW					: Left、Right Wrist OR Head Work
*********************************************************************************************************
*/
u8 rrwFlag = 0;
static void App_SteerWorkTask(void *pdata)
{
	while(1)
	{
		/*
		 * 抬起手臂 ---> 转动手腕 ---> 暂停1S ---> 复位手臂
		 */
		if(rrwFlag)
		{
			RightArmMove(20);
			OSTimeDlyHMSM(0,0,4,0);
			RightArmMove(50);
//			RightWristMove(30);
			OSTimeDlyHMSM(0,0,2,0);
//			RightWristMove(70);
			OSTimeDlyHMSM(0,0,2,0);
//			RightWristMove(50);
			
			OSTimeDlyHMSM(0,0,1,0); //停止1S后复位右臂
			calibrationRFlag = 0; //此处仅复位右臂即可
			rrwFlag = 0;
		}
		
		OSTimeDlyHMSM(0,0,0,10);
	}
}


/*
*********************************************************************************************************
*                                             App_Ultrasonic()
*
* Description : 超声波避障任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*********************************************************************************************************
*/
static void App_Ultrasonic(void *pdata)
{
	static uint8_t UltrFlag = 0;
	INT32U temp = 0;
	while(1)
	{
		  UltrasonicWave_StartMeasure(1);
			OSTimeDlyHMSM(0,0,0,30);	
		 if(TIM4CH1_CAPTURE_STA&0X80)
			{
					temp=TIM4CH1_CAPTURE_STA&0X3F;
					temp*=65536;
					temp+=TIM4CH1_CAPTURE_VAL;
					UltrasonicWave_Distance1 = temp/58;
					TIM4CH1_CAPTURE_STA=0;
			}
			
		 UltrasonicWave_StartMeasure(2);
		 OSTimeDlyHMSM(0,0,0,30);
		 if(TIM4CH2_CAPTURE_STA&0X80)
			{
					temp=TIM4CH2_CAPTURE_STA&0X3F;
					temp*=65536;
					temp+=TIM4CH2_CAPTURE_VAL;
					UltrasonicWave_Distance2 = temp/58;
					TIM4CH2_CAPTURE_STA=0;
			}
			
		 UltrasonicWave_StartMeasure(3);	
		 OSTimeDlyHMSM(0,0,0,30);
		 if(TIM4CH3_CAPTURE_STA&0X80)
			{
					temp=TIM4CH3_CAPTURE_STA&0X3F;
					temp*=65536;
					temp+=TIM4CH3_CAPTURE_VAL;
					UltrasonicWave_Distance3 = temp/58;
					TIM4CH3_CAPTURE_STA=0;
			}
			
			UltrasonicWave_StartMeasure(4);
			OSTimeDlyHMSM(0,0,0,30);	
		 if(TIM4CH4_CAPTURE_STA&0X80)
			{
					temp=TIM4CH4_CAPTURE_STA&0X3F;
					temp*=65536;
					temp+=TIM4CH4_CAPTURE_VAL;
					UltrasonicWave_Distance4 = temp/58;
					TIM4CH4_CAPTURE_STA=0;
			}
			
			if((UltrFlag == 0) && ((UltrasonicWave_Distance1 < 60.0) || (UltrasonicWave_Distance2 < 60.0) || (UltrasonicWave_Distance3 < 60.0)))
			{
					//suspend the tasks
					//stop all motors
					UltrTask = 1;
					UltrFlag = 1;					
			}
			else if((UltrFlag == 1) && (UltrasonicWave_Distance1 > 80.0) && (UltrasonicWave_Distance2 > 80.0) && (UltrasonicWave_Distance3 > 80.0))
			{
					//resume the tasks
					UltrTask = 0;
					UltrFlag = 0;
			}
	}
}
/*
*********************************************************************************************************
*                                             App_PDTask()
*
* Description : 机器人防跌落、循迹
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* PD					: Prevent Drop
*********************************************************************************************************
*/
static void App_PDTask(void *pdata)
{
	while(1)
	{
		
		PD_RightFoot1_IR=IsDetected(RightFoot1_IR);
		PD_RightFoot2_IR=IsDetected(RightFoot2_IR);
		
		PD_LeftFoot1_IR=IsDetected(LeftFoot1_IR);
		PD_LeftFoot2_IR=IsDetected(LeftFoot2_IR);
		
		if((robotMode==Forward || robotMode == SlideForward) && (!(PD_RightFoot1_IR && PD_RightFoot2_IR && PD_LeftFoot1_IR && PD_LeftFoot2_IR)))
		{	
				if((!(PD_LeftFoot1_IR && PD_LeftFoot2_IR))&& PD_RightFoot1_IR && PD_RightFoot2_IR)
				{
						//suspend the tasks
						//stop all motors
					  //左转矫正
						PDropL=1;
				}
				else if((!(PD_RightFoot1_IR && PD_RightFoot2_IR)) && PD_LeftFoot1_IR && PD_LeftFoot2_IR)
				{
						//suspend the tasks
						//stop all motors
						//右转矫正
						PDropR=1;
				}
				else if(!(PD_RightFoot1_IR || PD_RightFoot2_IR || PD_LeftFoot1_IR || PD_LeftFoot2_IR))
				{   //双脚离地或者两脚都检测到黑线
						PDropL=1;
						PDropR=1;
				}
		}
		else if(PD_RightFoot1_IR && PD_RightFoot2_IR && PD_LeftFoot1_IR && PD_LeftFoot2_IR)
		{
          //OSTimeDlyHMSM(0,0,0,60);
					//resume the tasks
					PDropL=0;
					PDropR=0;
		}
		OSTimeDlyHMSM(0,0,0,10);
	}
}


/*
*********************************************************************************************************
*                                             App_SuspendResumeTask()
*
* Description : 机器人动作挂起和恢复任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*********************************************************************************************************
*/
static void App_SuspendResumeTask(void *pdata)
{
		static uint8_t SuspendResume = 0;
	//临界区
// 	OS_CPU_SR  cpu_sr;
	while(1)
	{
// 		OS_ENTER_CRITICAL();				//进入临界区
		if((SuspendResume==0) && ((PDropL==1) || (PDropR==1) || (UltrTask==1)))
		{	
			  //suspend the tasks
			  OSTaskSuspend(9); //动作执行任务(RW: Robot Walk)
			  OSTaskSuspend(10); //动作步骤任务(RFS: Robot Forward Step)
			  OSTaskSuspend(11); //动作步骤任务(RBS: Robot Backward Step)
			  OSTaskSuspend(12); //右臂复位任务(RRA: Reset Right Arm)
			  OSTaskSuspend(13); //左臂复位任务(RLA: Reset Left Arm)
			  OSTaskSuspend(14); //右摆臂任务(SRA: Swing Right Arm)
			  OSTaskSuspend(15); //左摆臂任务(SLA: Swing Left Arm)
			
			//stop all motors
				HeadMove(70);
				LeftWristMove(70);
				RightWristMove(70);
				
				WaistMove(50);

				LeftArmMove(50);
				RightArmMove(50);

				if((PDropL==1) && (PDropR==0))
				{
						//左转矫正

				}
				else if((PDropR==1) && (PDropL==0))
				{
						//右转矫正

				}
				SuspendResume = 1;	
		}
		else if((SuspendResume==1) && (PDropL==0) && (PDropR==0) && (UltrTask==0))
		{
				//resume the tasks
				OSTaskResume(9);  //动作执行任务(RW: Robot Walk)
				OSTaskResume(10); //动作步骤任务(RFS: Robot Forward Step)
				OSTaskResume(11); //动作步骤任务(RBS: Robot Backward Step)
	 		  OSTaskResume(12); //右臂复位任务(RRA: Reset Right Arm)
				OSTaskResume(13); //左臂复位任务(RLA: Reset Left Arm)
				OSTaskResume(14); //右摆臂任务(SRA: Swing Right Arm)
				OSTaskResume(15); //左摆臂任务(SLA: Swing Left Arm)
			
//				if(robotMode == SlideForward)
//				{

//				}
				SuspendResume = 0;
		}
// 		OS_EXIT_CRITICAL();					//退出临界区
		OSTimeDlyHMSM(0,0,0,10);
	}
}

/*
*********************************************************************************************************
*                                             App_EETask()
*
* Description : 机器人眼睛表情(由于表情任务有SDIO接口和I2C接口通信，所以应在临界区内执行)              
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
* EE					: Eye Expression
*
* Note				: 由于机器人的眼睛计较大，使用OLED的成本较高，所以目前决定使用LED灯泡(将OLED程序备份在backup.txt中)
*
*********************************************************************************************************
*/
static void App_EETask(void *pdata) // 
{
 	while(1)
	{
		
			
			OSTimeDlyHMSM(0,0,0,1);
	}
}


/*
*********************************************************************************************************
*                                            App_LedTask()
*
* Description : 机器人状态LED灯和装饰LED
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
*********************************************************************************************************
*/
static void App_LedTask(void *pdata)
{
	int i=0,j=0;
	while(1)
	{
		if(light_mode == 1)
		{
				for(i=0;i<3;i++)
				{
						GPIO_SetBits(GPIOF,GPIO_Pin_All);
						GPIO_SetBits(GPIOG,GPIO_Pin_0 | GPIO_Pin_1);
						OSTimeDlyHMSM(0,0,0,200);
						GPIO_ResetBits(GPIOF,GPIO_Pin_All);
						GPIO_ResetBits(GPIOG,GPIO_Pin_0 | GPIO_Pin_1);
						OSTimeDlyHMSM(0,0,0,200);
				}

				for(j=0;j<6;j++)
				{
						GPIO_SetBits(GPIOF,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_11);
						GPIO_SetBits(GPIOG,GPIO_Pin_1);
						OSTimeDlyHMSM(0,0,0,200);
						GPIO_ResetBits(GPIOF,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_11);
						GPIO_ResetBits(GPIOG,GPIO_Pin_1);
						OSTimeDlyHMSM(0,0,0,200);
					
						GPIO_SetBits(GPIOF,GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_13);
						GPIO_SetBits(GPIOG,GPIO_Pin_0);
						OSTimeDlyHMSM(0,0,0,200);
						GPIO_ResetBits(GPIOF,GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_13);
						GPIO_ResetBits(GPIOG,GPIO_Pin_0);
						OSTimeDlyHMSM(0,0,0,200);
					
						GPIO_SetBits(GPIOF,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_12 | GPIO_Pin_15);
						OSTimeDlyHMSM(0,0,0,200);
						GPIO_ResetBits(GPIOF,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_12 | GPIO_Pin_15);
						OSTimeDlyHMSM(0,0,0,200);
				}
		}
		OSTimeDlyHMSM(0,0,0,20);
	}
}

/*
*********************************************************************************************************
*                                            App_DanceTask()
*
* Description : 机器人舞蹈管理任务
*
* Argument(s) : pdata.
*
* Return(s)   : none.
*
*********************************************************************************************************
*/
static void App_DanceTask(void *pdata)
{
	FRESULT result;
 	char ContentOfTheLine[6] = {0};
	
	uint32_t action = 0;
	uint32_t action_delay  = 0;
	
	INT8U err;

	while(1)
	{	
		OSSemPend(DanceSem,0,&err);
		
		/*##-1- 打开表情配置文件 */
		result = f_open(&CfgFile, "0:/dance/apple.txt", FA_OPEN_EXISTING | FA_READ);

		/*##-2- 逐行读取表情配置文件内容 */	
			do
			{
				
				f_gets(ContentOfTheLine,6,&CfgFile);
//				f_lseek(&CfgFile, f_tell(&CfgFile) + 2);
				sscanf((const char *)ContentOfTheLine, "%u %u" ,&action, &action_delay);
				
				ExecuteAction(action);
				OSTimeDlyHMSM(0,0,action_delay,10);
				ExecuteAction(127);
				
				
				if(f_eof(&CfgFile) != 0) //若文件读完，则将移动指针到文件头
				{
					//文件读完的时候，也就是舞蹈跳完的时候，跳出循环
					break;
					
				}

				
			}while(1);
			
		/*##-3- 关闭表情配置文件 */
			f_close(&CfgFile);
			
			OSTimeDlyHMSM(0,0,0,10);
		
	}
}
