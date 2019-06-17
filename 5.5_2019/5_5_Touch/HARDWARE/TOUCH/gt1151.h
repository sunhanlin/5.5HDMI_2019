#ifndef __GT1151_H
#define __GT1151_H	
#include "sys.h"	


//IO操作函数	 
#define GT_RST    		PBout(0)	//GT1151复位引脚
#define GT_INT    		PBin(1)		//GT1151中断引脚	
   	
 
//I2C读写命令	
#define GT_CMD_WR 		0X28     	//写命令
#define GT_CMD_RD 		0X29		//读命令
  
//GT1151 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT1151控制寄存器
#define GT_CFGS_REG 	0X8050   	//GT1151配置起始地址寄存器
#define GT_CHECK_REG 	0X813C   	//GT1151校验和寄存器
#define GT_PID_REG 		0X8140   	//GT1151产品ID寄存器

#define GT_GSTID_REG 	0X814E   	//GT1151当前检测到的触摸情况
#define GT_TP1_REG 		0X814F  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8157		//第二个触摸点数据地址
#define GT_TP3_REG 		0X815F		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8167		//第四个触摸点数据地址
#define GT_TP5_REG 		0X816F		//第五个触摸点数据地址  
#define GT_TP6_REG 		0X8177
#define GT_TP7_REG 		0X817F
#define GT_TP8_REG 		0X8187
#define GT_TP9_REG 		0X818F
#define GT_TP10_REG 	0X8197
 
u8 GT1151_Send_Cfg(u8 mode);
u8 GT1151_WR_Reg(u16 reg,u8 *buf,u8 len);
void GT1151_RD_Reg(u16 reg,u8 *buf,u8 len); 
u8 GT1151_Init(void);
u8 GT1151_Scan(void); 
#endif













