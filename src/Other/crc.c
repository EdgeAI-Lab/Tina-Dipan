/*=============================================================================
#     FileName: crc.c
#         Desc: 
#       Author: Lyn
#        Email: 
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2015-04-02 15:42:52
#      History:
=============================================================================*/

#include <os_cpu.h>

INT16U CRC16_TABLE[16] ={
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef
};

INT16U CRC16_Generation(const INT8U *ptr,INT16U data_length)
{
	INT8U crc_H4;
	INT16U crc = 0;

	while(data_length--)
	{
		crc_H4 = (INT8U)(crc >> 12);
		crc = crc << 4;
		crc = crc ^ CRC16_TABLE[crc_H4 ^ (*ptr >> 4)];
		crc_H4 = (INT8U)(crc >> 12);
		crc = crc << 4;
		crc = crc ^ CRC16_TABLE[ crc_H4 ^ (*ptr & 0x0f)];
		ptr++;
	}
	return crc;
}

INT8U CRC7_Generation(INT8U *buf, INT8U len)
{
	INT8U i,a, crc, data;
	
	crc=0;

	for (a=0; a<len; a++)
	{
		data = buf[a];
		for (i=0;i<8;i++)
		{
			crc <<= 1;

			if ((data & 0x80)^(crc & 0x80))
				crc ^=0x09;
			data <<= 1;
		}
	}
	crc=(crc<<1)|1;
	return(crc); 
}

/*
*********************************************************************************************************
*函数名：异或校验
*函数原型:static INT8U APP_XOR_Generation(const INT8U *buf,const INT8U i)
*入口参数：
*			buf：需要异或校验数据首地址
*			i：长度
*出口参数：无
*函数说明：无
*********************************************************************************************************
*/
INT8U XOR_Generation(const INT8U *buf,const INT8U i)
{
	INT8U cnt,xordata;
	xordata = 0X00;

	for (cnt = 0; cnt < i; cnt++)
		xordata ^= buf[cnt];
	return xordata;
}
