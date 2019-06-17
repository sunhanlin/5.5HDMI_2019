/**********************************************************
* @ File name -> stmflash.c
* @ Version   -> V1.0
* @ Date      -> 03-08-2014
* @ Brief     -> STM32内部FLASH操作函数

 V1.
* @ Revise    ->
**********************************************************/

#include "stmflash.h"

/**********************************************************
* 函数功能 ---> STM32 FLASH读取半字
* 入口参数 ---> Address：要读取数据的地址
* 返回参数 ---> 返回读取到的半字数据 
* 功能说明 ---> none
**********************************************************/
uint16_t FLASH_ReadHalfWord(uint32_t Address)
{
	return *(vu16*)Address;
}
/**********************************************************
* 函数功能 ---> STM32 FLASH指定地址读取指定长度数据（半字）
* 入口参数 ---> Address：数据的起始地址
*               *pBuffer：数据缓存
*               NumReadData：读取（半字）数量
* 返回参数 ---> none 
* 功能说明 ---> none
**********************************************************/
void FLASH_Read_HaifWord(uint32_t Address, uint16_t *pBuffer, uint16_t NumReadData)
{
	uint16_t i;

	for(i = 0;i < NumReadData;i++)
	{
		*pBuffer++ = FLASH_ReadHalfWord(Address);	//接收数据
		Address += 2;	//地址加2
	}
}

//=========================================================
#if _STM32_FLASH_WRITEOPEN	//打开了写功能
//=========================================================

/**********************************************************
                     定义数据缓存大小
                   只有打开写功能才生效
**********************************************************/

#if STM32_FLASH_SIZE<256	//小容量、中容量芯片

	#define STM32_FLASH_SECTOR_SIZE			1024	//1K大小
	
#else	//大容量芯片

	#define STM32_FLASH_SECTOR_SIZE			2048	//2K大小
	
#endif	//STM32_FLASH_SIZE

uint16_t STM32FLASH_BUFF[STM32_FLASH_SECTOR_SIZE/2];	//操作过程中数据缓存区 

/**********************************************************
* 函数功能 ---> STM32 FLASH写入扇区
* 入口参数 ---> Address：写入数据的起始地址
*               *pBuffer：写入数据缓存
*               NumWriteData：写入（半字）数量
* 返回参数 ---> none 
* 功能说明 ---> none
**********************************************************/
void FLASH_Write_HaifWord_SECTOR(uint32_t Address, uint16_t *pBuffer, uint16_t NumWriteData)
{
	uint16_t i;

	for(i = 0;i < NumWriteData;i++)
	{
		FLASH_ProgramHalfWord(Address, pBuffer[i]);	//写入半字数据
		Address += 2;	//地址增加2
	}
}
/**********************************************************
* 函数功能 ---> STM32 FLASH写入数据
* 入口参数 ---> Address：写入数据的起始地址
*               *pBuffer：写入数据缓存
*               NumWriteData：写入（半字）数量
* 返回参数 ---> none 
* 功能说明 ---> none
**********************************************************/
void STM32_FLASH_WriteData(uint32_t Address, uint16_t *pBuffer, uint16_t NumWriteData)
{
	uint32_t sectorpos;	//扇区地址
	uint32_t OffAddr;	//减去起始地址后的地址空间
	uint16_t sectorexc;	//扇区偏移量
	uint16_t sectorremain;	//扇区剩余空间缓存
	uint16_t i;

	/* 判读输入的地址是否合法 */
	if((Address < STM32_FLASH_STARTAddr) || (Address >= STM32_FLASH_ENDAddr))	return;	//地址非法
	/* 地址合法后解锁FLASH */
	FLASH_Unlock();	//解锁

	OffAddr = Address - STM32_FLASH_STARTAddr;	//得到地址偏移
	sectorpos = OffAddr / STM32_FLASH_SECTOR_SIZE;	//得到扇区地址，0 ~ 255
	sectorexc = (OffAddr % STM32_FLASH_SECTOR_SIZE) / 2;	//扇区内的偏移量
	sectorremain = STM32_FLASH_SECTOR_SIZE / 2 - sectorexc;	//得到扇区剩余空间  
	if(NumWriteData <= sectorremain)	sectorremain = NumWriteData;	//写入数据不大于一个页大小空间，即一个页就能放下这个数据
	
	while(1)	//循环操作写入数据
	{
		//读取扇区数据检验
		FLASH_Read_HaifWord(sectorpos*STM32_FLASH_SECTOR_SIZE+STM32_FLASH_STARTAddr, STM32FLASH_BUFF, STM32_FLASH_SECTOR_SIZE/2);  
		for(i = 0;i < sectorremain;i++)	//开始校验数据
		{
			if(STM32FLASH_BUFF[sectorexc+i] != 0xffff)	break;	//未擦除
		}
		if(i < sectorremain)	//需要擦除
		{
			FLASH_ErasePage(sectorpos*STM32_FLASH_SECTOR_SIZE+STM32_FLASH_STARTAddr);	//擦除
			for(i = 0;i < sectorremain;i++)	//复制数据
			{
				STM32FLASH_BUFF[i+sectorexc] = pBuffer[i];
			}
			//写入扇区数据
			FLASH_Write_HaifWord_SECTOR(sectorpos*STM32_FLASH_SECTOR_SIZE+STM32_FLASH_STARTAddr, STM32FLASH_BUFF, STM32_FLASH_SECTOR_SIZE/2);
		}
		else	//已经擦除过了，直接写入数据
		{
			FLASH_Write_HaifWord_SECTOR(Address, pBuffer, sectorremain);
		}
		if(NumWriteData == sectorremain)	break;	//写入完成
		else	//未完成
		{
			sectorpos++;	//写一个扇区
			sectorexc = 0;	//从0开始写入
			pBuffer += sectorremain;	//数据指针偏移
			Address += sectorremain;	//地址偏移
			NumWriteData -= sectorremain;	//减去已经写入的数据
			if(NumWriteData > (STM32_FLASH_SECTOR_SIZE/2))	sectorremain = STM32_FLASH_SECTOR_SIZE/2;	//超过扇区容量
			else	sectorremain = NumWriteData;	//写完继续下一扇区
		}
	}	//end while
	FLASH_Lock();	//写完数据，上锁
}

//=========================================================
#endif	//_STM32_FLASH_WRITEOPEN
//=========================================================

	