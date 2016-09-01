#ifndef  OS_CPU_H
#define  OS_CPU_H
#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif
#ifndef  OS_CPU_EXCEPT_STK_SIZE
#define  OS_CPU_EXCEPT_STK_SIZE    128u
#endif
//设置操作系统跟硬件的数据类型接口
typedef unsigned	char  	BOOLEAN;	//设置布尔变量
typedef unsigned	char  	INT8U;      //无符号8位整数
typedef signed	char  	INT8S;		// 有符号8位整数 
typedef unsigned short  	INT16U;		//无符号16位整数
typedef signed	short  	INT16S;		// 有符号16位整数
typedef unsigned	int  	INT32U;		// 无符号32位整数
typedef signed	int  	INT32S;		//有符号32位整数
typedef float          	FP32;		// 单精度浮点数
typedef double         	FP64;		// 双精度浮点数
typedef unsigned int		OS_STK;		// 堆栈入口宽度为16位
typedef unsigned int   	OS_CPU_SR;	//进入临界区使用方法3
//进入临界区使用方法3
#define  OS_CRITICAL_METHOD   3u
//进入与退出临界区使用第三种方法
#if OS_CRITICAL_METHOD == 3u
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}
#endif
//设置堆栈增长方向
#define  OS_STK_GROWTH        1u
//设置任务切换函数
#define  OS_TASK_SW()         OSCtxSw()
//异常堆栈
OS_CPU_EXT  OS_STK   OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
OS_CPU_EXT  OS_STK  *OS_CPU_ExceptStkBase;
//函数声明
#if OS_CRITICAL_METHOD == 3u 
OS_CPU_SR  OS_CPU_SR_Save(void);
void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
#endif
void       OSCtxSw(void);
void       OSIntCtxSw(void);
void       OSStartHighRdy(void);
#endif
