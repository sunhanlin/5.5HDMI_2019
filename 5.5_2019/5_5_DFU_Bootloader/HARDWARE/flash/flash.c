/**********************************************************
* @ File name -> flash.c
* @ Version   -> V1.0
* @ Date      -> 06-03-2014
* @ Brief     -> SPI Flash驱动函数
**********************************************************/

#include "flash.h"

/**********************************************************
                       定义变量缓存
**********************************************************/

static u8 Flash_Data_Buff[4096];	//扇区数据缓存区，一次至少擦除一个扇区

u16 SPI_FLASH_TYPE = W25Q64;    //默认为W25Q64
u16 SPI_Flash_ID;	//设备ID缓存

/**********************************************************
                        接口定义
                    硬件移植修改区域
**********************************************************/

/**********************************************************
* 函数功能 ---> GPIO初始化
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 主要是从设备的片选引脚初始化
**********************************************************/
void FlashSPI_CS_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG, ENABLE );//GPIO时钟使能 

	/*	GPIOB12初始化，Flash片选引脚	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);

	/*	GPIOD2初始化，SD卡片选	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // PD2 推挽 

	GPIO_Init(GPIOD, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOD,GPIO_Pin_2);

	/*	GPIOG7初始化，无线模块片选	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  // PG7 推挽
 
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOG,GPIO_Pin_7);

	SPI_GPIO_Init();		   	//初始化SPI
	SPIx_SetSpeed(SPI_BaudRatePrescaler_4);//设置为9M时钟,高速模式
	SPI_Flash_ID = Flash_Read_ID();//读取FLASH ID.  
}
/**********************************************************
* 函数功能 ---> flash芯片SPI读取数据
* 入口参数 ---> dat：要写入的数据
* 返回数值 ---> 返回设备数据
* 功能说明 ---> none
**********************************************************/
static u8 Flash_SPI_WriteRead_Data(u8 dat)
{
	return SPIx_WriteRead_Data(dat);
}

/**********************************************************
                  结束硬件移植修改区域
**********************************************************/

/**********************************************************
* 函数功能 ---> 读取W25Qxx序列号（ID）
* 入口参数 ---> none
* 返回数值 ---> ID参数，16位数据
* 功能说明 ---> none
**********************************************************/
u16 Flash_Read_ID(void)
{
	u16 IDtemp=0;
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(W25Qxx_Manufacturer);	//发送读取命令
	Flash_SPI_WriteRead_Data(0x00);	//读取24bits地址空间
	Flash_SPI_WriteRead_Data(0x00);
	Flash_SPI_WriteRead_Data(0x00);
	IDtemp |= Flash_SPI_WriteRead_Data(0xff) << 8;	//读取高8bit
	IDtemp |= Flash_SPI_WriteRead_Data(0xff);	//读取低8bit
	SPI_FLASH_CS = 1;	//释放芯片
	return IDtemp;	//返回ID号
}
/**********************************************************
* 函数功能 ---> W25Qxx进入掉电模式
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 在电池供电系统中，不用到Flash时候就让它睡觉，省电
**********************************************************/
void Flash_Enter_Power_Down(void)
{
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(W25Qxx_Power_Down);	//发送power down命令0xb9
	SPI_FLASH_CS = 1;	//释放芯片
	delay_us(3);
}
/**********************************************************
* 函数功能 ---> 唤醒W25Qxx
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 叫Flash起床
**********************************************************/
void Flash_Exit_Power_Down(void)
{
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(W25Qxx_Release_Power_Down);	//发送唤醒命令0xab
	SPI_FLASH_CS = 1;	//释放芯片
	delay_us(3);
}
/**********************************************************
* 函数功能 ---> W25Qxx读取状态寄存器
* 入口参数 ---> REG_CMD: 寄存器读取命令
* 返回数值 ---> 返回状态寄存器数据
* 功能说明 ---> Bit 15	Bit 14	Bit 13	Bit 12	Bit 11	Bit 10	Bit 9	Bit 8
*                   R      R       R      R       R        R      QE     SRP1
*
*               Bit 7	Bit 6	Bit 5	Bit 4	Bit 3	Bit 2	Bit 1	Bit 0    默认值为0x00
*                SRP0   SEC     TB      BP2     BP1     BP0     WEL     BUSY
**********************************************************/
u8 Flash_Read_Register_Status(u8 REG_CMD)
{
	u8 RS_byte=0;
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(REG_CMD);	//发送读取命令
	RS_byte = Flash_SPI_WriteRead_Data(0xff);	//读取一个字节数据 
	SPI_FLASH_CS = 1;	//释放芯片
	return RS_byte;	//返回读到的数据 
}
/**********************************************************
* 函数功能 ---> 等待W25Qxx忙完
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void Flash_Wait_Busy(void)
{
//      u8 temp,Bitbusy;

//      do
//      {
//            temp++;
//            if(temp > 200)    return;     //超时退出

//            Bitbusy = Flash_Read_Register_Status(W25Qxx_Read_Status_Register);  //读取状态寄存器值
//      }while(Bitbusy & 0x01);
	while((Flash_Read_Register_Status(W25Qxx_Read_Status_Register) & 0x01) == 0x01);	//等待忙标志位清零
}
/**********************************************************
* 函数功能 ---> W25Qxx写状态寄存器
* 入口参数 ---> RS_dat：写入的值
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void Flash_Write_Register_Status(u8 RS_dat)
{
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(W25Qxx_Write_Status_Register);	//发送命令
	Flash_SPI_WriteRead_Data(RS_dat);	//写入数据
	SPI_FLASH_CS = 1;	//释放芯片
}
/**********************************************************
* 函数功能 ---> W25Qxx打开写功能
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void Flash_WriteEnable(void)
{
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(W25Qxx_Write_Enable);	//发送命令
	SPI_FLASH_CS = 1;	//释放芯片
}
/**********************************************************
* 函数功能 ---> W25Qxx禁止写功能
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void Flash_WriteDisable(void)
{
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(W25Qxx_Write_Disable);	//发送命令
	SPI_FLASH_CS = 1;	//释放芯片
}
/**********************************************************
* 函数功能 ---> W25Qxx擦除一个扇区4K，或者擦除一个块
* 入口参数 ---> Sectoradd：扇区地址
*               ORD：擦除命令选择。0x00：擦除一个扇区。0x01：擦除一个块
* 返回数值 ---> none
* 功能说明 ---> 要擦除一个块发送擦除块指令W25Qxx_Erase_Block64Kb/W25Qxx_Erase_Block32Kb，视不同的芯片而定  
**********************************************************/
void Flash_EraseSector(u32 Sectoradd,u8 ORD)
{
	u8 ORD_temp;

	if(ORD == 0x00)
	{
		Sectoradd *= 4096;	//得到整个扇区的大小，4K
		ORD_temp = W25Qxx_Erase_Sector4Kb;
	} 
	if(ORD == 0x01)
	{
		Sectoradd *= 65536;	//得到整个块的大小，32K/64K
		ORD_temp = W25Qxx_Erase_Block64Kb;
	}

	Flash_WriteEnable();	//允许芯片写操作
	Flash_Wait_Busy();	//芯片处于空闲状态下才执行擦除
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(ORD_temp);	//发送擦除指令
	Flash_SPI_WriteRead_Data((u8)(Sectoradd >> 16));	//发送24bits地址
	Flash_SPI_WriteRead_Data((u8)(Sectoradd >> 8));
	Flash_SPI_WriteRead_Data((u8)Sectoradd);
	SPI_FLASH_CS = 1;	//释放芯片
	Flash_Wait_Busy();	//等待扇区擦除完成
}
/**********************************************************
* 函数功能 ---> W25Qxx擦除整个芯片
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 容量越大，擦除时间就越长，视不同的芯片而定  
**********************************************************/
void Flash_EraseChip(void)
{
	Flash_WriteEnable();	//允许芯片写操作
	Flash_Wait_Busy();	//芯片处于空闲状态下才执行擦除
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(W25Qxx_Erase_Chip);	//发送擦除指令
	SPI_FLASH_CS = 1;	//释放芯片
	Flash_Wait_Busy();	//等待擦除完成
}
/**********************************************************
* 函数功能 ---> W25Qxx在一个页内写入数据
* 入口参数 ---> *datBuff：数据组存储区
*               startadd：写入数据起始地址
*               num：写入数据长度（最大不超过256个字节）
* 返回数值 ---> none
* 功能说明 ---> 每个扇区有16页，地址从0x0000 ~ 0x00ff组成总共有4K   
**********************************************************/
void Flash_Write_Page(u32 startadd,u8* wbuffer,u16 num)
{
	u16 n;

	Flash_WriteEnable();	//允许芯片写操作
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(W25Qxx_Page_Program);	//发送页写指令
	Flash_SPI_WriteRead_Data((u8)(startadd >> 16));	//发送24bits地址
	Flash_SPI_WriteRead_Data((u8)(startadd >> 8));
	Flash_SPI_WriteRead_Data((u8)startadd);
	for(n = 0;n < num;n++)	Flash_SPI_WriteRead_Data(wbuffer[n]);	//开始写入数据
	SPI_FLASH_CS = 1;	//释放芯片
	Flash_Wait_Busy();	//等待数据写入完成
}
/**********************************************************
* 函数功能 ---> W25Qxx写扇区数据
* 入口参数 ---> *datBuff：数据组存储区
*               startadd：写入数据起始地址
*               num：写入数据长度（最大不超过65535个字节），不超过一个扇区4K
* 返回数值 ---> none
* 功能说明 ---> 每个扇区有16页，地址从0x0000 ~ 0x00ff组成总共有4K     
**********************************************************/
void Flash_Write_Sector(u32 writeadd,u8* wbuffer,u16 sum)
{
	u16 page_remain;	//页剩余数量

//	if((sum + writeadd%256) >= 256)     page_remain = 256 - writeadd%256;
//	else  page_remain = sum;
	
	page_remain = 256 - writeadd%256;	//得到首次写入数据多少
	if(page_remain >= sum)	page_remain = sum;	//数据大小超出页最大存储量

	while(1)	//循环写入数据到扇区
	{
		Flash_Write_Page(writeadd,wbuffer,page_remain);	//开始写入数据
		if(sum == page_remain)	break;	//数据写入完毕，退出
		else
		{
			writeadd += page_remain;	//写下一页
			wbuffer += page_remain;	//写下一页数据
			
			sum -= page_remain;	//减去已经写入完成的页数
			
			if(sum > 256)	page_remain = 256;	//超出页范围，256字节
			else	page_remain = sum;	//没超出
		}
	}
}
/**********************************************************
* 函数功能 ---> W25Qxx写数据
* 入口参数 ---> *pdBuff：数据组存储区
*               writeadd：写入数据起始地址
*               sum：写入数据长度（最大不超过65535个字节），不超过一个扇区4K
* 返回数值 ---> none
* 功能说明 ---> 每个扇区有16页，每页有256字节   
**********************************************************/
void Flash_WriteData(u8* pdbuffer,u32 writeadd,u16 sum)
{
	u16 i;
	u16 sectorremain;	//扇区剩余空间缓存
	u32 sectorpos;	//扇区地址
	u16 sectorexc;	//扇区偏移量
	
	sectorpos = writeadd / 4096;	//扇区地址，0 ~ 15
	sectorexc = writeadd % 4096;	//扇区偏移量，0 ~ 255
	sectorremain = 4096 - sectorexc;	//扇区剩余空间大小，一次写入256个字节
	
	if(sum <= sectorremain) sectorremain = sum;	//写入数据不大于一个页大小空间256，即一个页就能放下这个数据

	while(1)	//循环操作
	{
		Flash_ReadData(Flash_Data_Buff,sectorpos*4096,4096);	//读取整个扇区内容
		for(i = 0;i < sectorremain;i++)	//校验数据是否为0xff，即整个扇区是否擦除过
		{
			if(Flash_Data_Buff[sectorexc+i] != 0xff)	break;	//只要有一个数据不是0xff都要实现擦除操作	
		}
		if(i < sectorremain)	//需要擦除扇区
		{
			Flash_EraseSector(sectorpos,0x00);	//擦除整个扇区
			for(i = 0;i < sectorremain;i++)	//复制将要写入的数据
			{
				Flash_Data_Buff[i+sectorexc] = pdbuffer[i];	//在读出来的数据剩下空间中插入写入的数据
			}
			Flash_Write_Sector(sectorpos*4096,Flash_Data_Buff,4096);	//数据写入整个扇区
		}
		else	//扇区已经是擦除了的
		{
			Flash_Write_Sector(writeadd,pdbuffer,sectorremain);	//直接写入数据
		}

		if(sum == sectorremain)	break;	//数据写入完成
		else
		{
			sectorpos++;	//下一扇区
			sectorexc = 0;	//下一扇区的0地址写起
			
			pdbuffer += sectorremain;	//数据指针偏移
			writeadd += sectorremain;	//写地址也跟着偏移
			
			sum -= sectorremain;	//数值递减
			
			if(sum > 4096)	sectorremain = 4096;	//扇区放不下数据了
			else	sectorremain = sum;	//写完本扇区，继续写下一扇区
		}
	}
}
/**********************************************************
* 函数功能 ---> W25Qxx读数据
* 入口参数 ---> *datBuff：数据组存储区
*               startadd：读取数据起始地址
*               num：读取数据长度（最大不超过65535个字节），不超过一个扇区4K
* 返回数值 ---> none
* 功能说明 ---> W25Qxx在指定地址读取指定长度的数据，每个扇区有16块数据区，地址从0x0000 ~ 0x00ff组成总共有4K    
**********************************************************/
void Flash_ReadData(u8* rbuffer,u32 startadd,u16 num)
{
	u16 i;
	
	SPI_FLASH_CS = 0;	//选中Flash
	Flash_SPI_WriteRead_Data(W25Qxx_Read_Data);	//发送读取指令
//	Flash_SPI_WriteRead_Data(W25Qxx_Fast_Data);	//发送读取指令
	Flash_SPI_WriteRead_Data((u8)(startadd >> 16));	//发送24bits地址
	Flash_SPI_WriteRead_Data((u8)(startadd >> 8));
	Flash_SPI_WriteRead_Data((u8)startadd);
//	Flash_SPI_WriteRead_Data(0xff);	//假读一次
	for(i = 0;i < num;i++)
	{
		rbuffer[i] = Flash_SPI_WriteRead_Data(0xff);	//读取数据
	}
	SPI_FLASH_CS = 1;	//释放芯片
}

