/**********************************************************
* @ File name -> lcd_drive.h
* @ Version   -> V1.0.2
* @ Date      -> 03-09-2014
* @ Brief     -> TFT彩屏底层驱动函数头文件

 V1.0.1
* @ Revise    -> 编写完成GPIO控制LCD驱动的初始化函数

 V1.0.2
* @ Revise    -> 优化移植时的方便性，尽量做到只是修改少量的宏定义即可
**********************************************************/

#ifndef _lcd_drive_h_
#define _lcd_drive_h_

/**********************************************************
                     外部函数头文件
         应用到不同的外设头文件请在这里修改即可                        
**********************************************************/

#include "STM32_config.h"

/**********************************************************
           新增不同STM32开发板子切换开关定义
**********************************************************/

#define _USE_WarShip				1	//1：使用战舰开发板
                                        //0：使用STM32F103VET6最小系统板

//使用不同液晶时所使用的开关
#define _H8bit_To_L8bit				0	//0：不交换高8位和低8位
                                        //1：将高8位和低8位交换

/**********************************************************
                    控制LCD相关的宏定义
**********************************************************/

#define _USER_GPIO_OR_FSMC           1     /* 控制方式。FSMC or GPIO */
                                          //0: 使用普通的GPIO控制
                                          //1: 使用FSMC控制，类似控制NOR SRAM一样

#define _LCD_RESET_Soft              0     /* LCD复位是用软件控制还是硬件控制定义 */
                                          //0: 使用硬件控制复位
                                          //1: 使用软件控制复位

/**********************************************************
                    定义背光接口信息
               不同的控制接口请修改这里                       
**********************************************************/

#if _USE_WarShip	//定义了则使用的是战舰开发板

#define LCD_BLControl               PBout(0)    //背光控制

//                                      GPIO时钟        GPIO组   引脚号     翻转速度       输入输出模式
#define LCD_BLInit					RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_0,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
/*
取值范围：（对应宏定义顺序）
RCC_APB2Periph_GPIOA ~ RCC_APB2Periph_GPIOG,
GPIOA ~ GPIOG,
GPIO_Pin_0 ~ GPIO_Pin_15 and GPIO_Pin_All,
GPIO_Speed_2MHz或GPIO_Speed_10MHz或GPIO_Speed_50MHz,
GPIOMode_TypeDef结构体范围
*/

#else	//否则使用STM32F103VET6最小系统板

#define LCD_BLControl               PBout(1)    //背光控制

//                                      GPIO时钟        GPIO组   引脚号     翻转速度       输入输出模式
#define LCD_BLInit					RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_1,GPIO_Speed_2MHz,GPIO_Mode_Out_PP

#endif

/**********************************************************
             定义了软件复位时管脚定义信息
**********************************************************/
#if _LCD_RESET_Soft	//定义了则使用IO控制LCD复位操作

	#define LCD_RST							PBout(0)	/* 软件控制复位管脚定义。比如PAout(0) */

	//                                      GPIO时钟        GPIO组   引脚号     翻转速度       输入输出模式
	#define LCD_RSTInit					RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_0,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
/*
取值范围：（对应宏定义顺序）
RCC_APB2Periph_GPIOA ~ RCC_APB2Periph_GPIOG,
GPIOA ~ GPIOG,
GPIO_Pin_0 ~ GPIO_Pin_15 and GPIO_Pin_All,
GPIO_Speed_2MHz或GPIO_Speed_10MHz或GPIO_Speed_50MHz,
GPIOMode_TypeDef结构体范围
*/

#endif	//end _LCD_RESET_Soft
/* 结束软件控制LCD复位管脚信息定义 */

//=========================================================
#if !_USER_GPIO_OR_FSMC	//只有开启了GPIO控制时才要设置下面参数
//=========================================================
/* 下面两个宏定义只有开启了GPIO控制LCD接口才要设置，硬件（或FSMC）控制的时候不管 */

	#define _USER_PSB_Soft               0     /* 是否软件控制数据总线位数选择【具体输出电平在GPIO初始化里面完成】 */
                                               //0: 使用硬件控制
                                               //1: 使用软件控制

	#define _USER_8BIT_16BIT             1     /* 8 or 16位数据接口选择【FSMC数据总线宽度由初始化是决定】 */
                                               //0: 使用8bit数据总线
                                               //1: 使用16bit数据总线，默认值

/* 下面是定义软件控制选择数据总线宽度的IO初始化信息 */
//=========================================================
#if _USER_PSB_Soft    //定义了则使用软件控制数据总线宽度
//=========================================================
	
	#define LCD_PSB							PBout(1)	/* 软件控制复位管脚定义。比如PAout(0) */

	//                                      GPIO时钟        GPIO组   引脚号     翻转速度       输入输出模式
	#define LCD_PSBInit					RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_1,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
/*
取值范围：（对应宏定义顺序）
RCC_APB2Periph_GPIOA ~ RCC_APB2Periph_GPIOG,
GPIOA ~ GPIOG,
GPIO_Pin_0 ~ GPIO_Pin_15 and GPIO_Pin_All,
GPIO_Speed_2MHz或GPIO_Speed_10MHz或GPIO_Speed_50MHz,
GPIOMode_TypeDef结构体范围
*/

//=========================================================
#endif	//end _USER_PSB_Soft
//=========================================================

//=========================================================
#endif	//end _USER_GPIO_OR_FSMC
//=========================================================

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

//=========================================================
#if _USER_GPIO_OR_FSMC    //定义则使用FSMC控制LCD
//========================================================= 

/**********************************************************
              FSMC Bank1所管理的区域说明
**********************************************************/
/*
 _______________________________________________________________________________
|  所选的Bank   | 片选信号 |          地址范围         | [27:26] |    [25:0]    |
|_______________|__________|___________________________|_________|______________|
| Bank1.region1 | FSMC_NE1 | 0x6000,0000 ~ 0x63ff,ffff |    00   |              |
|_______________|__________|___________________________|_________|              |
| Bank1.region2 | FSMC_NE2 | 0x6400,0000 ~ 0x67ff,ffff |    01   | FSMC_A[25:0] |
|_______________|__________|___________________________|_________|              |
| Bank1.region3 | FSMC_NE3 | 0x6800,0000 ~ 0x6bff,ffff |    10   |              |
|_______________|__________|___________________________|_________|              |
| Bank1.region4 | FSMC_NE4 | 0x6c00,0000 ~ 0x6fff,ffff |    11   |              |
|_______________|__________|___________________________|_________|______________|

*/ 
//所接的地址线地址相对于块管理基地址的偏移量计算
//RAM地址 = Bank(x).region(y) + 2 * (2^n)
//参数解说：x：块编号
//          y：管理区域
//          n：外部所接的地址线号

/* 具体不同的块请修改lcd_drive.c里面的STM32_FSMC_Init()这个函数完成对FSMC初始化处理 */

#if _USE_WarShip==1	//使用战舰开发板
                                 
	/* 使用不同的控制块基地址请修改这里 */
	#define LCD_FSMC_REGION_BASE	(u32)0x6c000000	//FSMC块控制基地址
	/* 控制LCD_RS地址线不一样请修改这里 */
	#define LCD_RAM_ADDR_LINE		(u32)0x00000800	//LCD_RS控制线所接FSMC地址线偏移量

	/* 下面是与操作LCD相关的定义，不用修改 */
	#define LCD_REG_ADD				(u32)LCD_FSMC_REGION_BASE //寄存器地址，命令,RS = 0
	#define LCD_DAT_ADD				(u32)(LCD_FSMC_REGION_BASE | LCD_RAM_ADDR_LINE) //显示数据地址，显示RAM,RS = 1

	#define LCD_WR_REG(index)		((*(__IO u16 *)(LCD_REG_ADD)) = ((u16)index))	//写LCD寄存器写入数值index
	#define LCD_WR_Data(val)		((*(__IO u16 *)(LCD_DAT_ADD)) = ((u16)val))		//往LCD的RAM写入数据val
	#define LCD_RD_Data()			(*(__IO u16 *)(LCD_DAT_ADD))	//从LCD读取数据

/* 下面是结构体的实现方法 */

//	typedef struct
//	{
//		u16 LCD_REG;	/* 地址为：LCD_BASE */
//		u16 LCD_RAM;	/* 地址为：LCD_BASE + 2 */
//	}LCD_TypeDef;
//
//	#define LCD_BASE				((u32)(0x6c000000 | 0x000007fe))	//FSMC块控制基地址 + LCD_RS控制线所接FSMC地址线偏移量
//	#define LCD						((LCD_TypeDef *)LCD_BASE)
//
//	#define LCD_WR_REG(index)		(LCD->LCD_REG = (u16)index)	//写LCD寄存器写入数值index
//	#define LCD_WR_Data(val)		(LCD->LCD_RAM = (u16)val)	//往LCD的RAM写入数据val
//	#define LCD_RD_Data()			(u16)LCD->LCD_RAM			//从LCD读取数据

#else

	/* 使用不同的控制块基地址请修改这里 */
	#define LCD_FSMC_REGION_BASE	(u32)0x60000000	//FSMC块控制基地址
	/* 控制LCD_RS地址线不一样请修改这里 */
	#define LCD_RAM_ADDR_LINE		(u32)0x00020000	//LCD_RS控制线所接FSMC地址线偏移量

	/* 下面是与操作LCD相关的定义，不用修改 */
	#define LCD_REG_ADD				(u32)LCD_FSMC_REGION_BASE //寄存器地址，命令,RS = 0
	#define LCD_DAT_ADD				(u32)(LCD_FSMC_REGION_BASE | LCD_RAM_ADDR_LINE) //显示数据地址，显示RAM,RS = 1

	#define LCD_WR_REG(index)		((*(__IO u16 *)(LCD_REG_ADD)) = ((u16)index))	//写LCD寄存器写入数值index
	#define LCD_WR_Data(val)		((*(__IO u16 *)(LCD_DAT_ADD)) = ((u16)val))		//往LCD的RAM写入数据val
	#define LCD_RD_Data()			(*(__IO u16 *)(LCD_DAT_ADD))	//从LCD读取数据

/* 下面是结构体的实现方法 */

//	typedef struct
//	{
//		u16 LCD_REG;	/* 地址为：LCD_BASE */
//		u16 LCD_RAM;	/* 地址为：LCD_BASE + 2 */
//	}LCD_TypeDef;
//
//	#define LCD_BASE				((u32)(0x60000000 | 0x0001fffe))	//FSMC块控制基地址 + LCD_RS控制线所接FSMC地址线偏移量
//	#define LCD						((LCD_TypeDef *)LCD_BASE)
//
//	#define LCD_WR_REG(index)		(LCD->LCD_REG = (u16)index)	//写LCD寄存器写入数值index
//	#define LCD_WR_Data(val)		(LCD->LCD_RAM = (u16)val)	//往LCD的RAM写入数据val
//	#define LCD_RD_Data()			(u16)LCD->LCD_RAM			//从LCD读取数据

#endif

//=========================================================
#else //否则使用GPIO控制
//=========================================================

	/* 具体需要编写lcd_drive.c里面的LCD_GPIO_Init()这个函数完成IO的初始化处理 */

	/* 根据实际连接进行改写下面宏定义 */
	//                                      GPIO时钟        GPIO组   引脚号     翻转速度       输入输出模式
	#define LCD_RSInit					RCC_APB2Periph_GPIOA,GPIOA,GPIO_Pin_0,GPIO_Speed_50MHz,GPIO_Mode_Out_PP
	#define LCD_CSInit					RCC_APB2Periph_GPIOA,GPIOA,GPIO_Pin_0,GPIO_Speed_50MHz,GPIO_Mode_Out_PP
	#define LCD_RDInit					RCC_APB2Periph_GPIOA,GPIOA,GPIO_Pin_0,GPIO_Speed_50MHz,GPIO_Mode_Out_PP
	#define LCD_WRInit					RCC_APB2Periph_GPIOA,GPIOA,GPIO_Pin_0,GPIO_Speed_50MHz,GPIO_Mode_Out_PP

	#define LCD_RS						PAout(0)	/* LCD命令 <---> 数据选择控制线定义，比如PAout(0) */
	#define LCD_CS						PAout(0)	/* LCD片选控制线定义，比如PAout(0) */
	#define LCD_RD						PAout(0)	/* LCD读控制线定义，比如PAout(0) */
	#define LCD_WR						PAout(0)	/* LCD写控制线定义，比如PAout(0) */

	//===================================================
	#if _USER_8BIT_16BIT  //如果定义了则使用的是16bit总线
	//===================================================

	/* 根据实际连接的数据总线进行下面宏定义的改写 */

	#define LCD_BUS_DBout16(val)    /* LCD写入数据函数。比如：GPIO_Write(GPIOB, (uint16_t)val) */
	#define LCD_BUS_DBin16()        /* LCD读取数据函数。比如：(uint16_t)GPIO_ReadInputData(GPIOB) */

	/* 读取数据时设置数据总线方向为输入 */
	/* 比如：#define LCD_BUS_IN16()	{GPIOB->CRL &= 0xffffffff;GPIOB->CRL |= 0x88888888;GPIOB->CRH &= 0xffffffff;GPIOB->CRH |= 0x88888888;} */
	#define LCD_BUS_IN16()

	/* 写入数据时设置数据总线方向为输出 */
	/* 比如：#define LCD_BUS_OUT16()	{GPIOB->CRL &= 0xffffffff;GPIOB->CRL |= 0x33333333;GPIOB->CRH &= 0xffffffff;GPIOB->CRH |= 0x33333333;} */
	#define LCD_BUS_OUT16()

	//===================================================
	#else //否则使用8bit总线
	//===================================================

	/* 根据实际连接的数据总线进行下面宏定义的改写 */

	#define LCD_BUS_DBout8(val)    /* LCD写入数据函数。比如：GPIO_Write(GPIOB, (uint8_t)val) */
	#define LCD_BUS_DBin8()        /* LCD读取数据函数。比如：(uint8_t)GPIO_ReadInputData(GPIOB) */

	/* 读取数据时设置数据总线方向为输入 */
	/* 比如：#define LCD_BUS_IN8()	{GPIOB->CRL &= 0xffffffff;GPIOB->CRL |= 0x88888888;} */
	#define LCD_BUS_IN8()
	 
	/* 写入数据时设置数据总线方向为输出 */
	/* 比如：#define LCD_BUS_OUT8()	{GPIOB->CRL &= 0xffffffff;GPIOB->CRL |= 0x33333333;} */
	#define LCD_BUS_OUT8()

	//===================================================
	#endif	//end _USER_8BIT_16BIT
	//===================================================

//=========================================================
#endif	//end _USER_GPIO_OR_FSMC
//=========================================================

/**********************************************************
                       外部功能函数                      
**********************************************************/

#if _USER_GPIO_OR_FSMC      //定义了则使用FSMC控制

void STM32_FSMC_Init(void);   //FSMC初始化

#else	//使用GPIO控制时调用下面函数

void LCD_GPIO_Init(void);     //通讯、控制端口初始化

#endif	//end _USER_GPIO_OR_FSMC

/* 下面通用外调函数 */
void LCD_RST_BL_Init(void);   //背光、复位控制端口初始化     
void LCD_Write_Register(u16 reg_val);     //写数据到LCD寄存器（主要写命令或寄存器地址）
void LCD_Write_Data(u16 dat); //写数据到LCD RAM
u16 LCD_Read_Data(void);      //读取LCD数据
void LCD_WriteRegisterData(u16 reg, u16 dat);   //向LCD某个寄存器写入数据
u16 LCD_ReadRegisterData(u16 reg);  //读取LCD某个寄存器数值


#endif

