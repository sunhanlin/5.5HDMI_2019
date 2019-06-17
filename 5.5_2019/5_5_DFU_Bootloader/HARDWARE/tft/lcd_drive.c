/**********************************************************
* @ File name -> lcd_drive.c
* @ Version   -> V1.0.2
* @ Date      -> 03-09-2014
* @ Brief     -> TFT彩屏底层驱动函数

 V1.0.1
* @ Revise    -> 编写完成GPIO控制LCD驱动的初始化函数

 V1.0.2
* @ Revise    -> 优化移植时的方便性，尽量做到只是修改少量的宏定义即可
**********************************************************/

#include "lcd_drive.h"

/**********************************************************
                 |                        |
                 |  下面进入移植时修改区  |
                \|/                      \|/
**********************************************************/

//=========================================================
#if _USER_GPIO_OR_FSMC      //定义了则使用FSMC控制LCD
//========================================================

/**********************************************************
* 函数功能 ---> FSMC初始化
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void STM32_FSMC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
      
	FSMC_NORSRAMInitTypeDef FSMC_TFTLCD_InitStructure;

	FSMC_NORSRAMTimingInitTypeDef ReadWrite_Time;
	FSMC_NORSRAMTimingInitTypeDef Write_Time;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	//使能FSMC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//开启复用时钟

#if _USE_WarShip==1	//定义了则使用的是战舰开发板

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | \
	                        RCC_APB2Periph_GPIOG, ENABLE);	//开启外设时钟

	/*	初始化GPIOD为复用推挽输出	*/
	/*	PD.0为FSMC_D2    PD.1为FSMC_D3	*/
	/*	PD.4为FSMC_NOE    PD.5为FSMC_NWE	*/
	/*	PD.8为FSMC_D13    PD.9为FSMC_D14    PD.10为FSMC_D15	*/
	/*	PD.14为FSMC_D0    PD.15为FSMC_D1	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | \
	                              GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//GPIO翻转速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//设置为复用推挽输出

	GPIO_Init(GPIOD, &GPIO_InitStructure);	//初始化GPIO相关结构体

	/*	初始化GPIOE为复用推挽输出	*/
	/*	PE.7为FSMC_D4    PE.8为FSMC_D5	*/
	/*	PE.9为FSMC_D6    PE.10为FSMC_D7	*/
	/*	PE.11为FSMC_D8    PE.12为FSMC_D9	*/
	/*	PE.13为FSMC_D10    PE.14为FSMC_D11	*/
	/*	PE.5为FSMC_D12	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | \
	                              GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//GPIO翻转速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//设置为复用推挽输出

	GPIO_Init(GPIOE, &GPIO_InitStructure);	//初始化GPIO相关结构体

	/*	初始化GPIOG为复用推挽输出	*/
	/*	PG.0为FSMC_A10    PG.12为FSMC_NE4	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//GPIO翻转速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//设置为复用推挽输出

	GPIO_Init(GPIOG, &GPIO_InitStructure);	//初始化GPIO相关结构体
	
	/*	初始化FSMC外设寄存器	*/

	/*	初始化读写时序	*/
	ReadWrite_Time.FSMC_AddressSetupTime = 0x01;	//FSMC地址建立时间为2个HCLK时钟，1/36MHz = 27ns
	ReadWrite_Time.FSMC_AddressHoldTime = 0x00;	//地址保持时间，模式A未用到
	ReadWrite_Time.FSMC_DataSetupTime = 0xff;	//数据建立时间为16个HCLK
	ReadWrite_Time.FSMC_BusTurnAroundDuration = 0x00;	//
	ReadWrite_Time.FSMC_CLKDivision = 0x00;	//
	ReadWrite_Time.FSMC_DataLatency = 0x00;	//
	ReadWrite_Time.FSMC_AccessMode = FSMC_AccessMode_A;	//选择模式A

	/*	初始化写时序	*/
	Write_Time.FSMC_AddressSetupTime = 0x00;	//地址建立时间为1个HCLK
	Write_Time.FSMC_AddressHoldTime = 0x00;	//地址保持时间，模式A未用到
	Write_Time.FSMC_DataSetupTime = 0x03;	//数据建立时间在第四个HCLK时钟开始
	Write_Time.FSMC_BusTurnAroundDuration = 0x00;	//
	Write_Time.FSMC_CLKDivision = 0x00;	//
	Write_Time.FSMC_DataLatency = 0x00;	//
	Write_Time.FSMC_AccessMode = FSMC_AccessMode_A;	//选择模式A	
	
	FSMC_TFTLCD_InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;	//用bank1区号4
	FSMC_TFTLCD_InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;	//地址/数据地址复用功能关闭
	FSMC_TFTLCD_InitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;	//将液晶显示器当成是SRAM用
	FSMC_TFTLCD_InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;	//选择数据宽度为16bit
	FSMC_TFTLCD_InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;	//成组传输模式关闭
	FSMC_TFTLCD_InitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	// 
	FSMC_TFTLCD_InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;	//低电平改变数据 
	FSMC_TFTLCD_InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;	//
	FSMC_TFTLCD_InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;	//
	FSMC_TFTLCD_InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//打开写功能
	FSMC_TFTLCD_InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;	//禁用NWAIT信号，不检测闪存插入等待信号
	FSMC_TFTLCD_InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;	//打开扩展模式，允许读写使用不同的时序
	FSMC_TFTLCD_InitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;	//写操作始终处于异步模式

	FSMC_TFTLCD_InitStructure.FSMC_ReadWriteTimingStruct = &ReadWrite_Time;	//读写时序
	FSMC_TFTLCD_InitStructure.FSMC_WriteTimingStruct = &Write_Time;	//写时序
	
	FSMC_NORSRAMInit(&FSMC_TFTLCD_InitStructure);	//初始化FSMC外设

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);	//使能FSMC块1第四区
	
#else	//使用的是STM32F103VET6最小系统板子
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);	//开启外设时钟

	/*	初始化GPIOD为复用推挽输出	*/
	/*	PD.0为FSMC_D2    PD.1为FSMC_D3	*/
	/*	PD.4为FSMC_NOE    PD.5为FSMC_NWE	*/
	/*	PD.7为FSMC_NE1	*/
	/*	PD.8为FSMC_D13    PD.9为FSMC_D14    PD.10为FSMC_D15	*/
	/*	PD.11为FSMC_A16	*/
	/*	PD.14为FSMC_D0    PD.15为FSMC_D1	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | \
	                              GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//GPIO翻转速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//设置为复用推挽输出

	GPIO_Init(GPIOD, &GPIO_InitStructure);	//初始化GPIO相关结构体

	/*	初始化GPIOE为复用推挽输出	*/
	/*	PE.7为FSMC_D4    PE.8为FSMC_D5	*/
	/*	PE.9为FSMC_D6    PE.10为FSMC_D7	*/
	/*	PE.11为FSMC_D8    PE.12为FSMC_D9	*/
	/*	PE.13为FSMC_D10    PE.14为FSMC_D11	*/
	/*	PE.5为FSMC_D12	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | \
	                              GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//GPIO翻转速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//设置为复用推挽输出

	GPIO_Init(GPIOE, &GPIO_InitStructure);	//初始化GPIO相关结构体

	
	/*	初始化FSMC外设寄存器	*/

	/*	初始化读写时序	*/
	ReadWrite_Time.FSMC_AddressSetupTime = 0x01;	//FSMC地址建立时间为2个HCLK时钟，1/36MHz = 27ns
	ReadWrite_Time.FSMC_AddressHoldTime = 0x00;	//地址保持时间，模式A未用到
	ReadWrite_Time.FSMC_DataSetupTime = 0xff;	//数据建立时间为16个HCLK
	ReadWrite_Time.FSMC_BusTurnAroundDuration = 0x00;	//
	ReadWrite_Time.FSMC_CLKDivision = 0x00;	//
	ReadWrite_Time.FSMC_DataLatency = 0x00;	//
	ReadWrite_Time.FSMC_AccessMode = FSMC_AccessMode_A;	//选择模式A

	/*	初始化写时序	*/
	Write_Time.FSMC_AddressSetupTime = 0x00;	//地址建立时间为1个HCLK
	Write_Time.FSMC_AddressHoldTime = 0x00;	//地址保持时间，模式A未用到
	Write_Time.FSMC_DataSetupTime = 0x03;	//数据建立时间在第四个HCLK时钟开始
	Write_Time.FSMC_BusTurnAroundDuration = 0x00;	//
	Write_Time.FSMC_CLKDivision = 0x00;	//
	Write_Time.FSMC_DataLatency = 0x00;	//
	Write_Time.FSMC_AccessMode = FSMC_AccessMode_A;	//选择模式A	
	
	FSMC_TFTLCD_InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;	//用bank1区号1
	FSMC_TFTLCD_InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;	//地址/数据地址复用功能关闭
	FSMC_TFTLCD_InitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;	//将液晶显示器当成是SRAM用
	FSMC_TFTLCD_InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;	//选择数据宽度为16bit
	FSMC_TFTLCD_InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;	//成组传输模式关闭
	FSMC_TFTLCD_InitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	// 
	FSMC_TFTLCD_InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;	//低电平改变数据 
	FSMC_TFTLCD_InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;	//
	FSMC_TFTLCD_InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;	//
	FSMC_TFTLCD_InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//打开写功能
	FSMC_TFTLCD_InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;	//禁用NWAIT信号，不检测闪存插入等待信号
	FSMC_TFTLCD_InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;	//打开扩展模式，允许读写使用不同的时序
	FSMC_TFTLCD_InitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;	//写操作始终处于异步模式

	FSMC_TFTLCD_InitStructure.FSMC_ReadWriteTimingStruct = &ReadWrite_Time;	//读写时序
	FSMC_TFTLCD_InitStructure.FSMC_WriteTimingStruct = &Write_Time;	//写时序
	
	FSMC_NORSRAMInit(&FSMC_TFTLCD_InitStructure);	//初始化FSMC外设

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);	//使能FSMC块1第一区

#endif
}

//=========================================================
#else //否则使用普通GPIO控制LCD
//=========================================================

/**********************************************************
* 函数功能 ---> 通讯、控制端口初始化
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_GPIO_Init(void)
{
	/* 编写GPIO控制LCD控制线等初始化代码 */
	STM32_GPIOx_Init(LCD_RSInit);	//RS
	STM32_GPIOx_Init(LCD_CSInit);	//CS
	STM32_GPIOx_Init(LCD_RDInit);	//RD
	STM32_GPIOx_Init(LCD_WRInit);	//WR

	/* 编写GPIO连接LCD数据总线管脚初始化代码 */

#if _USER_8BIT_16BIT  //如果定义了则使用的是16bit总线
	/* 编写16bits数据宽度初始化代码 */

#else	//否则使用8bit总线
	/* 编写8bits数据宽度初始化代码 */

#endif
}
/**********************************************************
* 函数功能 ---> 写数据到总线
* 入口参数 ---> val：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_Write_Bus(u16 val)
{
#if _USER_8BIT_16BIT  //定义了则使用16bit总线
/* 使用16bits数据总线宽度时的逻辑控制时序 */

	LCD_BUS_OUT16();	//设置总线为输出方向
	LCD_BUS_DBout16(val);	//放上数据
	LCD_WR = 0;
	LCD_WR = 1;

#else //否则使用8bit总线
/* 使用8bits数据总线宽度时的逻辑控制时序 */

	LCD_BUS_OUT8();	//设置总线为输出方向
	LCD_BUS_DBout8((u8)(val/256));	//先发送高8位
	LCD_WR = 0;
	LCD_WR = 1;
	LCD_BUS_DBout8((u8)(val%256));	//再发送低8位
	LCD_WR = 0;
	LCD_WR = 1;

#endif
}
/**********************************************************
* 函数功能 ---> 读取LCD数据
* 入口参数 ---> none
* 返回数值 ---> 读取到的数据
* 功能说明 ---> none
**********************************************************/
u16 LCD_Read_Data(void)
{
	u16 temp=0;	//接收数据缓存

#if _USER_8BIT_16BIT  //定义了则使用16bit总线
/* 使用16bits数据总线宽度时的逻辑控制时序 */

	LCD_BUS_IN16();	//设置总线为输入方向
	LCD_WR = 1;
	LCD_RS = 1;	//读取数据
	LCD_RD = 1;
	LCD_RD = 0;	//产生时序
	LCD_RD = 1;
	temp = LCD_BUS_DBin16();

#else //否则使用8bit总线
/* 使用8bits数据总线宽度时的逻辑控制时序，实际接线则决定了屏蔽位 */

	LCD_BUS_IN8();	//设置总线为输入方向
	LCD_WR = 1;
	LCD_RS = 1;	//读取数据
	LCD_RD = 1;
	LCD_RD = 0;	//产生时序
	LCD_RD = 1;
	temp = (LCD_BUS_DBin8() & 0x00ff)<<8;	//首先接收到的是高8位
	LCD_RD = 0;	//产生时序
	LCD_RD = 1;
	temp |= (LCD_BUS_DBin8() & 0x00ff);	//再接收低8位	

#endif

	return temp;	//返回读取到的数据     
}

//=========================================================
#endif	//end _USER_GPIO_OR_FSMC
//=========================================================

/**********************************************************
                /|\                   /|\
                 |   结束移植时修改区  |
				 |                     |
**********************************************************/

/**********************************************************
* 函数功能 ---> 背光、复位控制端口初始化
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_RST_BL_Init(void)
{
	/******背光管脚******/

	STM32_GPIOx_Init(LCD_BLInit);

	/******************************************************
	         下面是条件选择所需要编写的初始化代码
	******************************************************/

	/******复位管脚******/
      
#if _LCD_RESET_Soft   //定义了则使用软件控制复位，初始化端口

	/* 编写控制LCD_RST管脚初始化代码 */
	STM32_GPIOx_Init(LCD_RSTInit);
	LCD_RST = 1;	//输出高电平，因为LCD低电平产生复位

#endif

	/******数据总线宽度选择管脚******/

#if _USER_PSB_Soft    //定义了则使用软件控制数据总线宽度选择

	/* 编写初始化LCD数据总线宽度控制线初始化代码 */
	STM32_GPIOx_Init(LCD_PSBInit);

#if _USER_8BIT_16BIT /* 定义了则使用16bit */
	LCD_PSB = 0;	//输出低电平，默认为16bits数据宽度

#else	/* 否则使用8bit */
	LCD_PSB = 1;	//输出高电平

#endif	//end _USER_8BIT_16BIT

#endif	//end _USER_PSB_Soft
}

/**********************************************************
        下面是相关的应用调用API函数，非必要可不修改
**********************************************************/


//=========================================================
#if _USER_GPIO_OR_FSMC      //定义了则使用FSMC控制LCD
//=========================================================

/**********************************************************
* 函数功能 ---> 写数据到LCD寄存器（主要写命令或寄存器地址）
* 入口参数 ---> reg_val：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_Write_Register(u16 reg_val)
{
#if _H8bit_To_L8bit	//交换高低8位

	u16 temp;
	temp = (reg_val%256)<<8;
	reg_val = (reg_val>>8) | temp;

#endif

	LCD_WR_REG(reg_val);
}
/**********************************************************
* 函数功能 ---> 写数据到LCD RAM
* 入口参数 ---> dat：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_Write_Data(u16 dat)
{
#if _H8bit_To_L8bit	//交换高低8位

	u16 temp;
	temp = (dat%256)<<8;
	dat = (dat>>8) | temp;

#endif

	LCD_WR_Data(dat);
}
/**********************************************************
* 函数功能 ---> 读取LCD数据
* 入口参数 ---> none
* 返回数值 ---> 读取到的数据
* 功能说明 ---> none
**********************************************************/
u16 LCD_Read_Data(void)
{
#if _H8bit_To_L8bit	//交换高低8位

	u16 temp1,temp2;
	temp1 = LCD_RD_Data();
	temp2 = (temp1%256)<<8;
	temp2 = (temp1>>8) | temp2;
	return temp2;

#else

	return (u16)LCD_RD_Data();

#endif
}
/**********************************************************
* 函数功能 ---> 向LCD某个寄存器写入数据
* 入口参数 ---> reg：要写入数据的寄存器地址
*               dat：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_WriteRegisterData(u16 reg, u16 dat)
{
	LCD_Write_Register(reg);
	LCD_Write_Data(dat);

//	LCD_WR_REG(reg);
//	LCD_WR_Data(dat);
}
/**********************************************************
* 函数功能 ---> 读取LCD某个寄存器数值
* 入口参数 ---> reg：要读取数据的寄存器地址
* 返回数值 ---> 寄存器数值
* 功能说明 ---> none
**********************************************************/
u16 LCD_ReadRegisterData(u16 reg)
{
	LCD_Write_Register(reg);
	delay_us(5);
	return LCD_Read_Data();
}

//=========================================================
#else //否则使用普通GPIO控制LCD
//=========================================================

/**********************************************************
* 函数功能 ---> 写数据到LCD寄存器（主要写命令或寄存器地址）
* 入口参数 ---> reg_val：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_Write_Register(u16 reg_val)
{
	LCD_RS = 0; //发送的是COM Address
	LCD_RD = 1;
	LCD_Write_Bus(reg_val);
	LCD_RS = 1;      
}
/**********************************************************
* 函数功能 ---> 写数据到LCD RAM
* 入口参数 ---> dat：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_Write_Data(u16 dat)
{
	LCD_RS = 1; //发送的是data
	LCD_RD = 1;
	LCD_Write_Bus(reg_val);
}
/**********************************************************
* 函数功能 ---> 向LCD某个寄存器写入数据
* 入口参数 ---> reg：要写入数据的寄存器地址
*               dat：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void LCD_WriteRegisterData(u16 reg, u16 dat)
{
	LCD_CS = 0;
	LCD_Write_Register(reg);
	LCD_Write_Data(dat);
	LCD_CS = 1;
}
/**********************************************************
* 函数功能 ---> 读取LCD某个寄存器数值
* 入口参数 ---> reg：要读取数据的寄存器地址
* 返回数值 ---> 寄存器数值
* 功能说明 ---> none
**********************************************************/
u16 LCD_ReadRegisterData(u16 reg)
{
	u16 tem;
      
	LCD_CS = 0;
	LCD_Write_Register(reg);
	delay_us(5);
	tem = LCD_Read_Data();
	LCD_CS = 1;
	return tem;
}

//=========================================================
#endif	//end _USER_GPIO_OR_FSMC
//=========================================================

