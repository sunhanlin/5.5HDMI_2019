#ifndef __FT3517U_H
#define __FT3517U_H	
#include "sys.h"	


//����ݴ��������ӵ�оƬ����(δ����IIC����) 
//IO��������	 
#define FT_RST    		PBout(0)	//GT1151��λ����
#define FT_INT    		PBin(1)		//GT1151�ж�����	

//I2C��д����	
#define FT_CMD_WR 				0X70    	//д����
#define FT_CMD_RD 				0X71		//������
  
//FT5206 ���ּĴ������� 
#define FT_DEVIDE_MODE 			0x00   		//FT3517Uģʽ���ƼĴ���
#define FT_REG_NUM_FINGER       0x02		//����״̬�Ĵ���

#define FT_TP1_REG 				0X03	  	//��һ�����������ݵ�ַ
#define FT_TP2_REG 				0X09		//�ڶ������������ݵ�ַ
#define FT_TP3_REG 				0X0F		//���������������ݵ�ַ
#define FT_TP4_REG 				0X15		//���ĸ����������ݵ�ַ
#define FT_TP5_REG 				0X1B		//��������������ݵ�ַ
#define FT_TP6_REG 				0X21	  //��һ�����������ݵ�ַ
#define FT_TP7_REG 				0X27		//�ڶ������������ݵ�ַ
#define FT_TP8_REG 				0X2D		//���������������ݵ�ַ
#define FT_TP9_REG 				0X33		//���ĸ����������ݵ�ַ
#define FT_TP10_REG 		  0X39		//��������������ݵ�ַ  
 

#define	FT_ID_G_LIB_VERSION		0xA1		//�汾		
#define FT_ID_G_MODE 			0xA4   		//FT5206�ж�ģʽ���ƼĴ���
#define FT_ID_G_THGROUP			0x80   		//������Чֵ���üĴ���
#define FT_ID_G_PERIODACTIVE	0x88   		//����״̬�������üĴ���


u8 FT3517U_WR_Reg(u16 reg,u8 *buf,u8 len);
u8 FT3517U_RD_Reg(u16 reg,u8 *buf,u8 len);
u8 FT3517U_Init(void);
u8 FT3517U_Scan(void);

#endif

















