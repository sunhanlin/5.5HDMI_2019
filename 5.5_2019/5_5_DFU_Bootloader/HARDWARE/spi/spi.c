/**********************************************************
* @ File name -> spi.c
* @ Version   -> V1.1
* @ Date      -> 11-05-2013
* @ Brief     -> STM32硬件SPI驱动函数

 V1.
* @ Revise    -> 
**********************************************************/

#include "spi.h"

/**********************************************************
* 函数功能 ---> SPI_CS初始化
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void SPIx_CSPin_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);	 //使能PB,D,G端口时钟
    
	//W25Q32片选引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PA4上拉 防止W25X的干扰
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化指定IO
 	GPIO_SetBits(GPIOA, GPIO_Pin_4);//置高电平				
 	
	//SD卡SPI控制时的片选引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //PD2推挽输出上拉   禁止SD卡的干扰
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);	//初始化指定IO
 	GPIO_SetBits(GPIOD, GPIO_Pin_2);//置高电平
	
	//无线模块的片选和使能引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	//PC6/7 推挽，无线模块的CE和CSN	  
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化指定IO
	GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7);//置高电平

	//无线模块的中断检测引脚
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;	//无线模块的IRQ   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA8 输入
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_8);//置高电平
}
/**********************************************************
* 函数功能 ---> GPIO初始化
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void SPI_GPIO_Init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	//开启SPI1外设时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);	//开启SPI2外设时钟
	
	SPIx_CSPin_Init();	//初始化相关的CS管脚

	/*	初始化GPIOA	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PA5/6/7复用推挽输出 

	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);	//PA5.6.7上拉
	
//	SPI_Cmd(SPI1, DISABLE); // SPI外设不使能

	/*	初始化SPI1	*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//设置SPI单向或者双向的数据模式：设置SPI为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	//设置SPI工作模式：设置SPI为主设备
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	//设置SPI数据帧结构：设置SPI数据帧结构为8bit模式
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	//设置SPI串行时钟的稳态：设置SPI时钟悬空为高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//设置SPI数据捕获方式：设置SPI数据捕获方式为第二个时钟沿
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	//设置SPI串行时钟的稳态：设置SPI时钟悬空为低
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//设置SPI数据捕获方式：设置SPI数据捕获方式为第一个时钟沿      
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	//设置SPI NSS（片选信号）控制方式：设置SPI NSS控制方式为软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;	//设置SPI波特率预分频值：设置SPI预分频值为4分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定SPI数据传送是从MSB开始还是LSB开始：设置SPI传送数据从MSB开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//设置SPI CRC值计算的多项式：设置SPI CRC计算多项式7

	SPI_Init(SPI1, &SPI_InitStructure);	//初始化SPI1外设

	SPI_Cmd(SPI1, ENABLE);	//使能SPI1外设

	SPIx_WriteRead_Data(0xff);
}
/**********************************************************
* 函数功能 ---> 设置SPIx传送速度
* 入口参数 ---> SPI_Speed：要改变的速度值
* 返回数值 ---> none
* 功能说明 ---> SPI_BaudRatePrescaler_2		2分频
*               SPI_BaudRatePrescaler_4		4分频
*               SPI_BaudRatePrescaler_8		8分频
*               SPI_BaudRatePrescaler_16	16分频
*               SPI_BaudRatePrescaler_32	32分频
*               SPI_BaudRatePrescaler_64	64分频
*               SPI_BaudRatePrescaler_128	128分频
*               SPI_BaudRatePrescaler_256	256分频
**********************************************************/
void SPIx_SetSpeed(u8 SPI_Speed)
{
//	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_Speed));
	SPI1->CR1 &= 0xffc7; 
	SPI1->CR1 |= SPI_Speed;	//写入分频值
	SPI_Cmd(SPI1, ENABLE);	//使能SPI1外设
}
/**********************************************************
* 函数功能 ---> SPIx数据读写
* 入口参数 ---> 写入的数据或者命令
* 返回参数 ---> MOSI接收到的数据
*               1：表示发送或者接收出错或者超时 
* 功能说明 ---> None
**********************************************************/
u8 SPIx_WriteRead_Data(u8 write_dat)
{
	u8 time_count;

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)	//DR Register发送缓冲空标志
	{
		time_count++;
		if(time_count > 200)	return 1;	//超时退出，防止程序死循环在这里
	}	

	SPI_I2S_SendData(SPI1, write_dat);	//向SPI设备发送一个字节数据或者命令

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)	//等待接收完成，接收非空标志
	{
		time_count++;
		if(time_count > 200)	return 1;	//超时退出，防止程序死循环在这里
	}

	return SPI_I2S_ReceiveData(SPI1);	//返回从SPIx设备上发出的数据
}

