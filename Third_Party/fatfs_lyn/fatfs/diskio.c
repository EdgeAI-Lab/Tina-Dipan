/*
*********************************************************************************************************
*                                   Fatfs文件系统驱动 CODE
*							(c) Copyright 2003-2006;联睿电子科技
*
*文件名：diskio.c
*创建人：snow
*创建时间：2012.07.24
*文件说明：蓝牙通信底层驱动实现
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
* 函数名：disk_initialize()；
* 函数原型：DSTATUS disk_initialize (BYTE drv)
* 入口参数：drv：指定待初始化的物理驱动器号		
* 出口参数：返回一个磁盘状态作为结果
* 函数说明：
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
* 函数名：disk_status()；
* 函数原型：DSTATUS disk_status (BYTE drv)
* 入口参数：drv：指定待初始化的物理驱动器号		
* 出口参数：返回一个磁盘状态作为结果
* 函数说明：
*********************************************************************************************************
*/
DSTATUS disk_status (BYTE drv)
{
	//return robot.sysstate;
	return 0;
}

/*
*********************************************************************************************************
* 函数名：disk_read()；
* 函数原型：DRESULT disk_read (BYTE drv, BYTE *buf, DWORD sector, BYTE count)
* 入口参数：
*			drv：指定待初始化的物理驱动器号		
*			buf：读取数据存放的首地址
*			sector：要读取数据的扇区号
*			count：要读取的扇区数
* 出口参数：返回一个磁盘状态作为结果
* 函数说明：
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
* 函数名：disk_write()；
* 函数原型：DRESULT disk_write (BYTE drv, const BYTE *buf, DWORD sector, BYTE count)
* 入口参数：
*			drv：指定待初始化的物理驱动器号		
*			buf：读取数据存放的首地址
*			sector：要读取数据的扇区号
*			count：要读取的扇区数
* 出口参数：返回一个磁盘状态作为结果
* 函数说明：
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
* 函数名：disk_write()；
* 函数原型：DRESULT disk_write (BYTE drv, const BYTE *buf, DWORD sector, BYTE count)
* 入口参数：
*			drv：指定待初始化的物理驱动器号		
*			buf：读取数据存放的首地址
*			sector：要读取数据的扇区号
*			count：要读取的扇区数
* 出口参数：返回一个磁盘状态作为结果
* 函数说明：
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
