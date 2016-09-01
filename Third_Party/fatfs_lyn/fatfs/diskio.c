/*
*********************************************************************************************************
*                                   Fatfs�ļ�ϵͳ���� CODE
*							(c) Copyright 2003-2006;����ӿƼ�
*
*�ļ�����diskio.c
*�����ˣ�snow
*����ʱ�䣺2012.07.24
*�ļ�˵��������ͨ�ŵײ�����ʵ��
*********************************************************************************************************
*/


#include <diskio.h>
#include <os_cpu.h>
#include "ucos_ii.h"
#include "stm32_eval_sdio_sd.h"
//#include <sys.h>
//#include <bsp.h>
//extern System robot;
#define SECTOR_SIZE		512U
extern OS_EVENT * SdCommunicateSem;
/*
*********************************************************************************************************
* ��������disk_initialize()��
* ����ԭ�ͣ�DSTATUS disk_initialize (BYTE drv)
* ��ڲ�����drv��ָ������ʼ����������������		
* ���ڲ���������һ������״̬��Ϊ���
* ����˵����
*********************************************************************************************************
*/
DSTATUS disk_initialize (BYTE drv)
{
	INT8U i=0;
	INT8U temp;

	if (drv) return STA_NOINIT;

	do 
	{
		temp = SD_Init();
		i++;
		if (i > 100)
			break;
	} while (temp);

	return	temp;
}


/*
*********************************************************************************************************
* ��������disk_status()��
* ����ԭ�ͣ�DSTATUS disk_status (BYTE drv)
* ��ڲ�����drv��ָ������ʼ����������������		
* ���ڲ���������һ������״̬��Ϊ���
* ����˵����
*********************************************************************************************************
*/
DSTATUS disk_status (BYTE drv)
{
	//return robot.sysstate;
	return 0;
}

/*
*********************************************************************************************************
* ��������disk_read()��
* ����ԭ�ͣ�DRESULT disk_read (BYTE drv, BYTE *buf, DWORD sector, BYTE count)
* ��ڲ�����
*			drv��ָ������ʼ����������������		
*			buf����ȡ���ݴ�ŵ��׵�ַ
*			sector��Ҫ��ȡ���ݵ�������
*			count��Ҫ��ȡ��������
* ���ڲ���������һ������״̬��Ϊ���
* ����˵����
*********************************************************************************************************
*/
DRESULT disk_read (BYTE drv, BYTE *buf, DWORD sector, BYTE count)
{
	SD_Error Status = SD_OK;
	if (drv || !count)	return RES_PARERR;	
	if (count == 1) {
		Status = SD_ReadBlock(buf,sector << 9 ,SECTOR_SIZE);
	} else {
		Status = SD_ReadMultiBlocks(buf,sector << 9 ,SECTOR_SIZE,count);	
	}
	Status = SD_WaitReadOperation();
	return (DRESULT)Status;
}


/*
*********************************************************************************************************
* ��������disk_write()��
* ����ԭ�ͣ�DRESULT disk_write (BYTE drv, const BYTE *buf, DWORD sector, BYTE count)
* ��ڲ�����
*			drv��ָ������ʼ����������������		
*			buf����ȡ���ݴ�ŵ��׵�ַ
*			sector��Ҫ��ȡ���ݵ�������
*			count��Ҫ��ȡ��������
* ���ڲ���������һ������״̬��Ϊ���
* ����˵����
*********************************************************************************************************
*/
#if	_READONLY == 0
DRESULT disk_write (BYTE drv, const BYTE *buf, DWORD sector, BYTE count)
{
	SD_Error Status = SD_OK;
	if (drv || !count) return RES_PARERR;
	if (count == 1) {
		Status = SD_WriteBlock((uint8_t *)buf,sector << 9 ,SECTOR_SIZE);
	} else {
		Status = SD_WriteMultiBlocks((uint8_t *)buf,sector << 9 ,SECTOR_SIZE,count);	
	}
	Status = SD_WaitWriteOperation();
	return (DRESULT)Status;
}
#endif


/*
*********************************************************************************************************
* ��������disk_write()��
* ����ԭ�ͣ�DRESULT disk_write (BYTE drv, const BYTE *buf, DWORD sector, BYTE count)
* ��ڲ�����
*			drv��ָ������ʼ����������������		
*			buf����ȡ���ݴ�ŵ��׵�ַ
*			sector��Ҫ��ȡ���ݵ�������
*			count��Ҫ��ȡ��������
* ���ڲ���������һ������״̬��Ϊ���
* ����˵����
*********************************************************************************************************
*/
DRESULT disk_ioctl (BYTE drv, BYTE Ctrl, void* buf)
{
	return RES_OK;
}

DWORD get_fattime(void)
{
	return 0X40A00000;
}
