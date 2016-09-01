#ifndef __MEMMANAGE_H
#define __MEMMANAGE_H
#include "includes.h"
INT8U MemInit(void);

void  *GetMem (const INT16U num, INT8U *perr);

INT8U  PutMem (void *pblk, const INT16U num);
#endif

