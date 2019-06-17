/**********************************************************
* @ File name -> hw_iic.h
* @ Version   -> V1.0
* @ Date      -> 02-19-2014
* @ Brief     -> STM32硬件IIC驱动函数头文件

 V1.
* @ Revise    -> 
**********************************************************/

#ifndef _hw_iic_h_
#define _hw_iic_h_

/**********************************************************
                     外部函数头文件                        
**********************************************************/

#include "stm32f10x_i2c.h"

#include "STM32_config.h"

/**********************************************************
               IIC操作过程当中错误信息定义
**********************************************************/

extern u8 IIC_RES_STA;

#define IIC_START_ERROR					0x01	//发送起始位错误
#define IIC_SLAVE_ERROR					0x02	//设备地址 + 写命令错误
#define IIC_READ_ERROR					0x04	//设备地址 + 读命令错误
#define IIC_ADD_H_ERROR					0x08	//16位地址时发送高8位错误
#define IIC_ADD_L_ERROR					0x10	//16位地址时发送低8位错误
#define IIC_DATA_ERROR					0x20	//发送/接收数据错误
#define IIC_ACK_ERROR					0x40	//应答错误

#define IIC_NoBUSY						(u8)0	//总线空闲
#define IIC_BUSYING						(u8)1	//总线忙

/**********************************************************
                     外部功能函数
**********************************************************/

u8 IIC_Check_Busy(void);	//检查总线状态

void STM32hwIIC_Init(u8 Slave_Add);	//IIC接口初始化

u8 STM32hw_Write_nByte(u8 Slave_Add, u16 Data_Add, u8 *Bytebuff, u8 len, u8 mode);	//IIC写入数据

u8 STM32hw_Read_nByte(u8 Slave_Add, u16 Data_Add, u8 *Bytebuff, u8 len, u8 mode);	//IIC读取数据


#endif

