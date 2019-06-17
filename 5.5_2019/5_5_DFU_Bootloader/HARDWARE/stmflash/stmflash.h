/**********************************************************
* @ File name -> stmflash.h
* @ Version   -> V1.0
* @ Date      -> 03-08-2014
* @ Brief     -> STM32内部FLASH操作函数头文件

 V1.
* @ Revise    ->
**********************************************************/

#ifndef _stmflash_h_
#define _stmflash_h_

/**********************************************************
                     外部函数头文件
         应用到不同的外设头文件请在这里修改即可                        
**********************************************************/

#include "stm32_config.h"

#include "stm32f10x_flash.h"

/**********************************************************
                      操作功能定义
      需要打开写功能、芯片flash容量大小请修改这里
**********************************************************/

#define _STM32_FLASH_WRITEOPEN					1	/* STM32 FLASH写功能开关 */
													//0：关闭
													//1：开启

#define STM32_FLASH_SIZE						512	/* 所用芯片的容量，单位：KB */
													//这里决定页写入数据大小，小容量和中容量芯片为1K，大容量为2K
													//flash容量有:64K、128K、256K、512K、1024K等

/**********************************************************
                   定义FLASH容量地址范围
                 不同容量的芯片请修改这里
**********************************************************/

#define STM32_FLASH_STARTAddr		(uint32_t)0x08000000	//flash起始地址，不同的flash地址请修改这里

/* flash结束地址，非必要可以不修改 */ 
#define STM32_FLASH_ENDAddr			(uint32_t)((STM32_FLASH_STARTAddr + STM32_FLASH_SIZE*1024) -1)

/**********************************************************
                       外部功能函数                      
**********************************************************/

uint16_t FLASH_ReadHalfWord(uint32_t Address);	//STM32 FLASH读取半字
void FLASH_Read_HaifWord(uint32_t Address, uint16_t *pBuffer, uint16_t NumReadData);	//STM32 FLASH指定地址读取指定长度数据（半字）

#if _STM32_FLASH_WRITEOPEN	//打开了写功能

void FLASH_Write_HaifWord_SECTOR(uint32_t Address, uint16_t *pBuffer, uint16_t NumWriteData);	//STM32 FLASH写入扇区
void STM32_FLASH_WriteData(uint32_t Address, uint16_t *pBuffer, uint16_t NumWriteData);	//STM32 FLASH写入数据

#endif	//_STM32_FLASH_WRITEOPEN


#endif

