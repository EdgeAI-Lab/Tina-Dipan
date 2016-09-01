#ifndef __CRC_H
#define __CRC_H
INT16U CRC16_Generation(const INT8U *ptr,INT16U data_length);

INT8U CRC7_Generation(INT8U *buf, INT8U len);

INT8U XOR_Generation(const INT8U *buf,const INT8U i);
#endif


