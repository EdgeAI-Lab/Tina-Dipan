
/*
*********************************************************************************************************
*
*                                            ROBOT CODE
*
*                                         Yingborui Big Robot
*                                              with the
*                                   CPU STM32F103VE & Driver DRV8848
*
* Filename      : myFun.c
* Version       : 1.0.0
* Programmer(s) : Fan
*********************************************************************************************************
*/

#include "includes.h"
#include <string.h>
extern float  WaistPosition;
/*
 *	ʹ������λ
 *	�˺�����ѭ��ִ�У�������λ������ֹͣ
 *  �ӻ����˱��濴����λ���м�Ϊ0�����Ϊ��ֵ���ұ�Ϊ��ֵ������ѧ��X��һ����
 *  ... -20  -10  0  10  20 ...  
 *
 */
 
u8 rwFlag = 1;
void resetWaist(float a)
{
// 	static u8 rwFlag = 1;
	if(rwFlag) //rwFlag: the flag of Reset Waist 
	{
		  if(-2.0>a )
			{
				WaistMove(30);
			}
			else if(a>2.0)
			{
				WaistMove(70);
			}
			else if((a>-2.0) && (a<2.0))
			{
				WaistMove(50);
				rwFlag = 0;
			}
	}
}

/*
 *	�˺������ڰڶ�����
 *
 *	LA: Left Angle(minus)
 *	RA: Right Angle(positive number)
 *  zero point in the middle of the potentiometer
 */
void moveWaist(int LA, int RA, unsigned char speed)
{
	static u8 ToggleDirection = 0;//�����л��������� 
	static u8 arwFlag = 1;//arwFlag: the flag of Auxiliary Swing Waist(����������־λ����ʵ������������λ) 
	static u8 swFlag = 0;//swFlag: the flag of Swing Waist

	if(arwFlag) //rwFlag: the flag of Reset Waist 
	{
		  if(-2.0>WaistPosition )
			{
				WaistMove(30);
			}else if(WaistPosition>2.0){
				WaistMove(70);
			}
			else if((WaistPosition>-2.0) && (WaistPosition<2.0))
			{
				WaistMove(50);
				arwFlag = 0;
				swFlag = 1;
			}

	}
	
	if(swFlag)	//swFlag: the flag of Swing Waist
		{
			if(ToggleDirection%2 == 0)
			{
				WaistMove(speed); //�������
			}
			else
			{
				WaistMove(100-speed);
			}
			if( (LA>(int)WaistPosition) || (RA<(int)WaistPosition) )
				{
					arwFlag = 1;
					ToggleDirection++;
					swFlag = 0;
				}
		}
}


/*
 *	��ͷ
 *	
 *	
 *	
 */
void SH(void)
{
	
}

/*
*********************************************************************************************************
*	�� �� ��: ReadFileData
*	����˵��: ��ȡ�ļ�armfly.txtǰ128���ַ�������ӡ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
extern FIL CfgFile,oledFile;

void ReadFileData(void)
{
	/* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */
//	FRESULT result;
//	FATFS fs;
//	FIL file;
//	uint32_t bw;
//	char buf[1028];
//	
// 	/* �����ļ�ϵͳ */
//	result = f_mount(0, &fs);			/* Mount a logical drive */
//	if (result != FR_OK)
//	{
//		printf("Mount failed(%d)\r\n", result);
//	}

//	/* ���ļ� */
//	result = f_open(&file, "0:/STM32.TXT", FA_OPEN_EXISTING | FA_READ);
//	if (result !=  FR_OK)
//	{
//		printf("Don't Find File : STM32.TXT\r\n");
//		return;		
//	}

//	/* ��ȡ�ļ� */
//	result = f_read(&file, &buf, sizeof(buf) - 1, &bw);
//	if (bw > 0)
//	{
//		buf[bw] = 0;
//		printf("\rSTM32.TXT �ļ����� : \r\n%s\r\n", buf);
//	}
//	else
//	{
//		printf("\rSTM32.TXT �ļ����� : \r\n");		
//	}
//	
//	/* �ر��ļ�*/
//	f_close(&file);
// 	
//	/* ж���ļ�ϵͳ */
//	f_mount(0, NULL);

/* ====================================================== */

//	FRESULT result;
//	//FATFS fs;
//// FIL file,oledFile;
//	uint32_t bw;
//	char buf[1028];
//	char strBuf[50];
//	char sss[35];
//	
//	uint32_t cnt = 0;
//	uint8_t leftflname[30];
//	uint8_t rightflname[30];
//	
//	
//	static uint8_t count = 0;
//	static uint8_t lineNum = 1;



//	
//	

//			/* Read the file */
//		if (f_eof(&CfgFile)==0)//
//			{
//				do
//				{
////					f_read(&CfgFile, &sss, sizeof(sss) - 1, &bw);
//					f_gets(sss,34,&CfgFile);
//					count++;
//				}while(count<lineNum);
//				
//				
//			}else{
//					/* Close the file*/
//				printf("the end.\n");
//				f_lseek(&CfgFile,0);
//			  f_close(&CfgFile);	
//			}
//			
//			sscanf((const char *)sss, "%u %s %[^;]" ,&cnt, leftflname, rightflname);
//			
//			sprintf((char *)strBuf, "0:/eye/bin/%s", leftflname);
//				
//			result = f_open(&oledFile, (const TCHAR*)strBuf, FA_OPEN_EXISTING | FA_READ);
//				
//			result = f_read(&oledFile, &buf, sizeof(buf) - 1, &bw);
//			
//			
//			if (bw > 0)
//			{
//				buf[bw] = 0;
////				OLED_CLS();//Clear OLED
//				OLED_DrawBMP(20,0,116,8,(unsigned char *)buf);
//				count = 0;
//				lineNum++;
//				OSTimeDlyHMSM(0,0,0,cnt);
//		//		printf("\r\TheFile.TXT : \r\n%s\r\n", buf);
//			}
//			else
//			{
//		//		printf("\r\TheFile.TXT : \r\n");		
//			}
//			
//			/* Close the file*/
//			f_close(&oledFile);
//			
//		//	/* Unmount file system */
////			f_mount(0, NULL);
}





