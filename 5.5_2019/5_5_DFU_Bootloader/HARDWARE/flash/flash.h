/**********************************************************
* @ File name -> flash.h
* @ Version   -> V1.0
* @ Date      -> 06-03-2014
* @ Brief     -> SPI Flash驱动函数头文件
**********************************************************/

#ifndef _flash_h_
#define _flash_h_

/**********************************************************
                     外部函数头文件
         应用到不同的外设头文件请在这里修改即可                        
**********************************************************/

#include "STM32_config.h"

#include "spi.h"

/**********************************************************
                      定义模拟接口
                所接的IO不一样请修改这里
    设置IO的方向请修改这里，对于不是具有准双向IO的MCU
**********************************************************/

#define	SPI_FLASH_CS			PBout(12)  //FLASH片选控制

/**********************************************************
                        芯片ID宏定义
**********************************************************/

//W25Qxx系列Flash芯片ID
#define W25Q80					0xef13 	
#define W25Q16					0xef14
#define W25Q32					0xef15
#define W25Q64					0xef16
#define W25Q128					0xef17

extern u16 SPI_FLASH_TYPE;  //SPI Flash类型
extern u16 SPI_Flash_ID;	//设备ID缓存

/**********************************************************
                     命令和参数宏定义
**********************************************************/

//指令宏定义
#define W25Qxx_Write_Enable					0x06
#define W25Qxx_Write_Disable				0x04
#define W25Qxx_Read_Status_Register			0x05
#define W25Qxx_Read_Status_Register2		0x35
#define W25Qxx_Write_Status_Register		0x01
#define W25Qxx_Page_Program					0x02
#define W25Qxx_Quad_Page_Program			0x32
#define W25Qxx_Erase_Block64Kb				0xd8	//擦除一个块，64Kb
#define W25Qxx_Erase_Block32Kb				0x52	//擦除一个块，32Kb
#define W25Qxx_Erase_Sector4Kb				0x20	//擦除一个扇区，4Kb
#define W25Qxx_Erase_Chip					0xc7	//or 0x60
#define W25Qxx_Erase_Suspend				0x75
#define W25Qxx_Erase_Resume					0x7a
#define W25Qxx_Power_Down					0xb9	//进入掉电模式
#define W25Qxx_High_Performance_Mode		0xa3	//性能
#define W25Qxx_Continuous_Read_Mode			0xff	//Reset
#define W25Qxx_Release_Power_Down			0xab	//or HPM/Device ID，ID7 ~ ID0
#define W25Qxx_Manufacturer					0x90	//or Device ID，ID7 ~ ID0
#define W25Qxx_Read_Unique_ID				0x4b
#define W25Qxx_Jedec_ID						0x9f	//ID15 ~ ID0

#define W25Qxx_Read_Data					0x03
#define W25Qxx_Fast_Data					0x0b
#define W25Qxx_Fast_Read_Dual_Output		0x3b
#define W25Qxx_Fast_Read_Dual_IO			0xbb
#define W25Qxx_Fast_Read_Quad_Output		0x6b
#define W25Qxx_Fast_Read_Quad_IO			0xeb
#define W25Qxx_Octal_Word_Read_Quad_IO		0xe3

/**********************************************************
                        外部功能函数
**********************************************************/

void FlashSPI_CS_Init(void);	//GPIO初始化

u16 Flash_Read_ID(void);	//读取W25Qxx序列号（ID）

void Flash_Enter_Power_Down(void);	//W25Qxx进入掉电模式
void Flash_Exit_Power_Down(void);	//唤醒W25Qxx

void Flash_Wait_Busy(void);	//等待W25Qxx忙完

u8 Flash_Read_Register_Status(u8 REG_CMD);	//W25Qxx读取状态寄存器
void Flash_Write_Register_Status(u8 RS_dat);	//W25Qxx写状态寄存器

void Flash_EraseSector(u32 Sectoradd,u8 ORD);	//W25Qxx擦除一个扇区4K，或者擦除一个块
void Flash_EraseChip(void);	//W25Qxx擦除整个芯片

void Flash_Write_Sector(u32 writeadd,u8* wbuffer,u16 sum);	//W25Qxx写扇区数据
void Flash_Write_Page(u32 startadd,u8* wbuffer,u16 num);	//W25Qxx在一个页内写入数据

void Flash_ReadData(u8* rbuffer,u32 startadd,u16 num);	//W25Qxx读数据
void Flash_WriteData(u8* pdbuffer,u32 writeadd,u16 sum);	//W25Qxx写数据


#endif

/*****************************************************
例如W25Q64BV芯片，容量结构图

8M字节分为128个块-->16扇区/块-->16页/扇区。页的大小为256

*****************************************************/

