/*=============================================================================
#     FileName: i2c_eye.c
#         Desc: 
#       Author: INBOL
#        Email: 
#     HomePage: 
#      Version: 2.0
#   LastChange: 2014-03-03 11:02:35
#      History:
#   This Board Use PB6 PB7 as I2C1 Interface
=============================================================================*/
#include "includes.h"


/* ============================================================================
   Macro Defination
   ============================================================================ */
#define I2C1_DR				(INT32U)(I2C1_BASE + 0X10)				//I2C Data Register Address
#define OLED_INIT_NUM								59				//Init Data Length
#define OLED_DATA_NUM							   772				//Display Data Length
#define OLED_REMAP_NUM								27				//Location Data Length
/* ============================================================================
   Global variable
   ============================================================================*/
INT8U eyebuf[1028];
extern System robot;
extern OS_EVENT *I2C_TRANS_SEM;		//I2C translate over sem.
extern OS_EVENT *I2CMutex;
extern OS_EVENT *EyeTransSem;
extern INT16U test;

/* ============================================================================
   Display Init Code
   ============================================================================*/
const INT8U i2ccode[OLED_INIT_NUM] = {
	0x00,0xa8,0x3f,                     //3
	0x00,0xd9,0xf1,                     //3
	0x00,0x20,0x00,
	0x00,0x21,0x12,0x71,
	0x00,0x22,0x00,0x07,
	0x80,0x40,                          //2
	0x00,0x81,0x32,                     //3
	0x00,0x82,0x80,                     //3
	0x80,0xa0,                          //2
	0x80,0xa6,                          //2

	0x00,0xd3,0x40,                     //3
	0x00,0xad,0x8e,                     //3
	0x80,0xc0,                          //2
	0x00,0xd5,0xf0,                     //3
	0x00,0xd8,0x05,                     //3

	0x00,0xda,0x12,                     //3
	0x00,0xdb,0x34,                     //3
	0x00,0x91,0x3f,0x3f,0x3f,0x3f,      //6
	0x80,0xaf,                          //2
	0x80,0xa4};                         //2

/* ============================================================================
   Anew Location Code
   ============================================================================*/
const INT8U i2ccoderemp[OLED_REMAP_NUM] = {
	0x00,0x21,0x12,0x71,
	//0x00,0x21,0x00,0x71,
	0x00,0x22,0x00,0x07,
	0x80,0x40,                          //2
	0x80,0xa0,                          //2
	0x80,0xa6,                          //2

	0x00,0xd3,0x40,                     //3
	0x80,0xc0,                          //2
	0x00,0xd8,0x05,                     //3

	0x00,0xda,0x12,                     //3
	0x80,0xa4}; 

/* ============================================================================
   Function Statement
   ============================================================================*/
static void OLED_SEND(const INT8U address, const INT16U num, const INT8U *ptr);

static void OLED_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	INT16U count;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_5);	
	count=5000;
	while(count--);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	count=5000;
	while(count--);
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
	count=5000;
	while(count--);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/* ============================================================================*/
/**
 * Description :Init Oled's I2c interface
 */
/* ============================================================================*/
void OLED_I2CInt(void)
{
	I2C_InitTypeDef	i2c_str;
	OLED_GPIOInit();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);//

	I2C_DeInit(I2C1);
	i2c_str.I2C_Mode		= I2C_Mode_I2C;
	i2c_str.I2C_DutyCycle	= I2C_DutyCycle_2;
	i2c_str.I2C_OwnAddress1	= 0X30;
	i2c_str.I2C_Ack			= I2C_Ack_Enable;
	i2c_str.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c_str.I2C_ClockSpeed	= 200000;
	I2C_Init(I2C1, &i2c_str);
	I2C_Cmd(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}
/* ============================================================================*/
/**
 * Description :Oled Init
 */
/* ============================================================================*/
void EYE_Module_Init(void)
{
	#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
	OS_CPU_SR  cpu_sr = 0;
	#endif
	memset((INT8U *)eyebuf, 0X00, 1028);
	eyebuf[0]=0x40;
	//右眼初始化
	//OLED_SEND(EYERIGHT, OLED_INIT_NUM, i2ccode);
	//OLED_SEND(EYERIGHT, OLED_DATA_NUM, eyebuf);
	//OLED_SEND(EYERIGHT, OLED_INIT_NUM, i2ccode);
	//OLED_SEND(EYERIGHT, OLED_DATA_NUM, eyebuf);
	//左眼初始化
	OLED_SEND(EYELEFT, OLED_INIT_NUM, i2ccode);
	OLED_SEND(EYELEFT, OLED_DATA_NUM, eyebuf);
	OLED_SEND(EYELEFT, OLED_INIT_NUM, i2ccode);
	OLED_SEND(EYELEFT, OLED_DATA_NUM, eyebuf);
	//右眼初始化
	OLED_SEND(EYERIGHT, OLED_INIT_NUM, i2ccode);
	OLED_SEND(EYERIGHT, OLED_DATA_NUM, eyebuf);
	OLED_SEND(EYERIGHT, OLED_INIT_NUM, i2ccode);
	OLED_SEND(EYERIGHT, OLED_DATA_NUM, eyebuf);
	strcpy((char *)(robot.eyectrl.filename), EYE_SYS);
	OS_ENTER_CRITICAL();
	robot.eyectrl.status |= (EYEBROW_UPDATA | EYEBROW_ALREADY | EYEBROW_BASIC);
	OS_EXIT_CRITICAL();
	OSSemPost(EyeTransSem);
}
/* ============================================================================*/
/**
 * Description :Translate Data to Oled
 *
 * Input    :address:Olde Address
 * Input    :num    :Data Length
 * Input    :ptr    :Data Address
 */
/* ============================================================================*/
static void OLED_SEND(const INT8U address, const INT16U num, const INT8U *ptr)
{
	INT8U err,i=0;
	DMA_InitTypeDef DMA_InitStruct;
	INT16U count;
	//if(err==OS_ERR_NONE)
	{
		do 
		{
			robot.i2cstate=0;
			I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);
			//Generate Start Signal
			I2C_GenerateSTART(I2C1, ENABLE);
			//Check I2C1 be in master mode or not,longest 20ms
			for (err = 0; (err < 10) && (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); err ++)
			{
				OSTimeDlyHMSM(0, 0, 0, 2);
			}
			if (err >= 10){return;}
			//Send Device Address
			I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);
			//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING)); 
			for (err = 0; (err < 10) && (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); err ++)
			{
				OSTimeDlyHMSM(0, 0, 0, 2);
			}
			if (err >= 10){return;}	
			
			I2C_Cmd(I2C1, DISABLE);
			DMA_Cmd(DMA1_Channel6, DISABLE);
			DMA_InitStruct.DMA_PeripheralBaseAddr	= I2C1_DR;		//destination
			DMA_InitStruct.DMA_MemoryBaseAddr		= (INT32U)ptr;
			DMA_InitStruct.DMA_DIR					= DMA_DIR_PeripheralDST;//Set peripheral as destination
			DMA_InitStruct.DMA_BufferSize			= num;
			DMA_InitStruct.DMA_PeripheralInc		= DMA_PeripheralInc_Disable;
			DMA_InitStruct.DMA_MemoryInc			= DMA_MemoryInc_Enable;
			DMA_InitStruct.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
			DMA_InitStruct.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
			DMA_InitStruct.DMA_Mode					= DMA_Mode_Normal;
			DMA_InitStruct.DMA_Priority				= DMA_Priority_Medium;
			DMA_InitStruct.DMA_M2M					= DMA_M2M_Disable;//Memery to memery mode or not
			DMA_Init(DMA1_Channel6, &DMA_InitStruct);
			I2C_DMACmd(I2C1, ENABLE);
			I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);//If error,then interrupt
			robot.i2cstate = 0;//I2C state,can use symbolic constant
			I2C_Cmd(I2C1, ENABLE);
			DMA_Cmd(DMA1_Channel6, ENABLE);
			DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);

			//Pend the task until get I2C_Trans_sem.
			OSSemPend(EyeTransSem, 0 , &err);
			I2C_ITConfig(I2C1, I2C_IT_ERR, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
			i++;
		} while ((robot.i2cstate) && (i < 10));//if translate with no error ,then quit while*/
	}
}
/* ============================================================================*/
/**
 * Description :Update Eye Expression-->updata:update data
 *
 * Input    :buf     the address of data that will display
 * Input    :eyeadd  eye address,is other word device address. 
 */
/* ============================================================================*/
void EYE_Updata(INT8U *buf, INT8U eyeadd)
{
	OLED_SEND(eyeadd, OLED_REMAP_NUM, i2ccoderemp);
	OLED_SEND(eyeadd, OLED_DATA_NUM, buf);
}


void Code_Create(INT8U *buf,INT16U num,INT8U dat)
{
	INT16U i;
	for(i=0; i<num+1; i++)
	{
		buf[i] = dat;
	}
}
