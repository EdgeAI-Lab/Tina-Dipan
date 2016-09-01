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
//���ò���ϵͳ��Ӳ�����������ͽӿ�
typedef unsigned	char  	BOOLEAN;	//���ò�������
typedef unsigned	char  	INT8U;      //�޷���8λ����
typedef signed	char  	INT8S;		// �з���8λ���� 
typedef unsigned short  	INT16U;		//�޷���16λ����
typedef signed	short  	INT16S;		// �з���16λ����
typedef unsigned	int  	INT32U;		// �޷���32λ����
typedef signed	int  	INT32S;		//�з���32λ����
typedef float          	FP32;		// �����ȸ�����
typedef double         	FP64;		// ˫���ȸ�����
typedef unsigned int		OS_STK;		// ��ջ��ڿ��Ϊ16λ
typedef unsigned int   	OS_CPU_SR;	//�����ٽ���ʹ�÷���3
//�����ٽ���ʹ�÷���3
#define  OS_CRITICAL_METHOD   3u
//�������˳��ٽ���ʹ�õ����ַ���
#if OS_CRITICAL_METHOD == 3u
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}
#endif
//���ö�ջ��������
#define  OS_STK_GROWTH        1u
//���������л�����
#define  OS_TASK_SW()         OSCtxSw()
//�쳣��ջ
OS_CPU_EXT  OS_STK   OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
OS_CPU_EXT  OS_STK  *OS_CPU_ExceptStkBase;
//��������
#if OS_CRITICAL_METHOD == 3u 
OS_CPU_SR  OS_CPU_SR_Save(void);
void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
#endif
void       OSCtxSw(void);
void       OSIntCtxSw(void);
void       OSStartHighRdy(void);
#endif
