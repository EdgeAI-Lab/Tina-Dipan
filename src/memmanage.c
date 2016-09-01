/*=============================================================================
#     FileName: app_str.c
#         Desc: Program Data init.
#       Author: Inbol
#        Email: 
#     HomePage: 
#      Version: 2.0
#   LastChange: 2014-03-05 09:00:45
#      History:
=============================================================================*/
#include "includes.h"
/* ============================================================================
   Macro Defination
   ============================================================================*/
#define MEM_32_NBLKS		15
#define MEM_32_BLKSIZE		32

#define MEM_64_NBLKS		15
#define MEM_64_BLKSIZE		64

#define MEM_256_NBLKS		15
#define MEM_256_BLKSIZE		256

#define MEM_512_NBLKS		15
#define MEM_512_BLKSIZE		512
/* ============================================================================
   RAM block
   ============================================================================*/
OS_MEM	*MemBuf_32;
INT8U	MemPart_32[MEM_32_NBLKS][MEM_32_BLKSIZE];

OS_MEM	*MemBuf_64;
INT8U	MemPart_64[MEM_64_NBLKS][MEM_64_BLKSIZE];

OS_MEM	*MemBuf_256;
INT8U	MemPart_256[MEM_256_NBLKS][MEM_256_BLKSIZE];

OS_MEM	*MemBuf_512;
INT8U	MemPart_512[MEM_512_NBLKS][MEM_512_BLKSIZE];

/* ============================================================================*/
/**
 * Description :Init the RAM
 *
 * Output   :0->successful   1->fail
 */
/* ============================================================================*/
INT8U MemInit(void)
{
	INT8U err;

	MemBuf_32 = OSMemCreate((INT8U *)MemPart_32, MEM_32_NBLKS, MEM_32_BLKSIZE, &err);
	if (err != OS_ERR_NONE) return 0x01;

	MemBuf_64 = OSMemCreate((INT8U *)MemPart_64, MEM_64_NBLKS, MEM_64_BLKSIZE, &err);
	if (err != OS_ERR_NONE) return 0x01;

	MemBuf_256 = OSMemCreate((INT8U *)MemPart_256, MEM_256_NBLKS, MEM_256_BLKSIZE, &err);
	if (err != OS_ERR_NONE) return 0X01;

	MemBuf_512 = OSMemCreate((INT8U *)MemPart_512, MEM_512_NBLKS, MEM_512_BLKSIZE, &err);
	if (err != OS_ERR_NONE) return 0x01;

	return 0;
}
/* ============================================================================*/
/**
 * Description :Gem Ram accdording to appointed large,should larger than 0,and smaller than 513
 *
 * Input    :num->size of ram to get
 * Input    :perr->OS_ERR_NONE                     return right
 *               ->OS_ERR_MEN_NO_FREE_BLKS         no enough ram
 *               ->OS_ERR_MEN_INVALID_PMEN         parameter error
 *
 * Output   :
 */
/* ============================================================================*/
void  *GetMem (const INT16U num, INT8U *perr)
{
	if (!num)
	{
		*perr = OS_ERR_MEM_INVALID_PMEM;
		return 0;
	}
	if (num < 33)
		return OSMemGet(MemBuf_32, perr);
	if (num < 65)
		return	OSMemGet(MemBuf_64, perr); 
	if (num < 257)
		return	OSMemGet(MemBuf_256, perr);
	if (num < 513)
		return	OSMemGet(MemBuf_512, perr);
	else
	{
		*perr=OS_ERR_MEM_INVALID_PMEM;
		return 0;
	}
}
/* ============================================================================*/
/**
 * Description :Release the RAM
 *
 * Input    :pblk->address of ram to release
 * Input    :num->length of the ram 
 *
 * Output   :OS_ERR_NONE                release success
 *           OS_ERR_MEM_FULL            the ram to release is full
 *           OS_ERR_MEM_INVALID_PMEM    parameter error
 *           OS_ERR_MEM_INVALID_PBLK    pblk is an null pointer
 */
/* ============================================================================*/
INT8U  PutMem (void *pblk, const INT16U num)
{
	if ((INT32U)pblk == 0x20000000)
	{
		return 1;
	}
	if (!num)
		return	OS_ERR_MEM_INVALID_PMEM; 
	if(num>512)
		return OS_ERR_MEM_INVALID_PMEM;
	if (num > 256)
		return OSMemPut(MemBuf_512, pblk);
	if (num > 64)
		return OSMemPut(MemBuf_256, pblk);	
	if (num > 32)
		return OSMemPut(MemBuf_64, pblk);
	else return	OSMemPut(MemBuf_32, pblk);

}

