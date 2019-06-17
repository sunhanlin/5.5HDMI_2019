/**********************************************************
* @ File name -> pcf8563.h
* @ Version   -> V1.0.3
* @ Date      -> 03-09-2014
* @ Brief     -> 时钟芯片PCF8563驱动函数头文件

 V1.0.1
* @ Revise    -> A、增加显示时间日期格式数组
* @           -> B、增加读取时间处理函数，读取到的时间日期信息直接转换成ASCII保存到时间格式数组中
* @           -> C、调用时间日期处理函数，显示或者打印到串口的话直接显示或者打印时间格式数组即可

 V1.0.2
* @ Revise    -> 修正年结构为16位数值，数值位，比如20xx、19xx

 V1.0.3
* @ Revise    -> 增加PCF8563是否存在检测函数
**********************************************************/

#ifndef _pcf8563_h_ 
#define _pcf8563_h_

/**********************************************************
                     外部函数头文件
         应用到不同的外设头文件请在这里修改即可                        
**********************************************************/

#include "iic.h"

#include "STM32_config.h"

/**********************************************************
                  PCF8563寄存器结构定义                          
**********************************************************/
typedef struct
{
	u16 year;	//年
	u8 month;	//月
	u8 week;	//星期
	u8 date;	//日
	u8 hour;	//小时
	u8 minute;	//分钟
	u8 second;	//秒钟

	u8 cont1;	//控制/状态寄存器1
	u8 cont2;	//控制/状态寄存器2
	
	u8 alarmmin;	//分钟报警
	u8 alarmhour;	//小时报警
	u8 alarmdate;	//日报警
	u8 alarmweek;	//星期报警
	
	u8 CLKcont;	//频率输出控制
	u8 timecont;	//定时器控制
	u8 timeconut;	//定时器倒计时寄存器
}Time_Typedef;

extern Time_Typedef TimeValue;	//定义时间数据缓存

extern u8 Time_Register[16];	//定义时间信息

extern u8 Display_Time[8];	//显示时间格式

extern u8 Display_Date[13];	//显示日期格式

/**********************************************************
                        参数宏定义                       
**********************************************************/

#define PCF8563_Check_Data		0x55	//检测用，可用其他数值

#define PCF8563_YEARDATA		(u16)0x2000	//年结构，如果是19xx年、21xx年的话请修改这个数值即可

#define PCF8563_Write			0xa2	//写命令
#define PCF8563_Read			0xa3	//读命令，或者用（PCF8563_Write + 1）

#define Clear_Register			0x00	//清除寄存器值填充数据

/**********************************************************
                   参数寄存器地址宏定义                    
**********************************************************/

#define Address_control1		0x00	//控制/状态寄存器1
#define Address_control2		0x01	//控制/状态寄存器2

#define Address_CLKOUT			0x0d	//CLKOUT频率寄存器
#define Address_Time			0x0e	//定时器控制寄存器
#define Address_Time_VAL		0x0f	//定时器倒计数寄存器

#define Address_year			0x08	//年
#define Address_month			0x07	//月
#define Address_date			0x05	//日
#define Address_week			0x06	//星期
#define Address_hour			0x04	//小时
#define Address_minute			0x03	//分钟
#define Address_second			0x02	//秒

#define Alarm_minute			0x09	//分钟报警
#define Alarm_hour				0x0a	//小时报警
#define Alarm_date				0x0b	//日报警
#define Alarm_week				0x0c	//星期报警

/**********************************************************
                时间参数屏蔽无效位宏定义                 
**********************************************************/

#define Shield_secondBit		0x7f
#define Shield_minuteBit		0x7f
#define Shield_hourBit			0x3f
#define Shield_weekBit			0x07
#define Shield_dateBit			0x3f
#define Shield_monthBit			0x1f	//bit7位为世纪为，0：指定世纪数为20xx
											//                1：指定世纪数为19xx
											//当年寄存器中的值由99变为00时，世纪位会改变

/**********************************************************
                       参数命令定义                      
**********************************************************/

#define Set_Year19xx			(1<<7)	//设置为19xx年模式
#define Set_Year20xx			(0<<7)	//设置为20xx年模式

//控制/状态寄存器1
#define Control_Test1_Common	(0<<7)	//普通模式
#define Control_Test1_EXT_CLK	(1<<7)	//EXT_CLK测试模式

#define Control_Chip_Run		(0<<5)	//芯片运行
#define Control_Chip_Stop		(1<<5)	//芯片停止运行，所有芯片分频器异步置逻辑0

#define Control_TESTC_Close		(0<<3)	//电源复位功能失效（普通模式时置逻辑0）
#define Control_TESTC_Open		(1<<3)	//电源复位功能有效

//控制/状态寄存器2
#define Control_TI_TF1			(0<<4)	//当TF有效时INT有效，（取决于TIE的状态）
#define Control_TI_TF2			(1<<4)	//INT脉冲有效，（取决于TIE的状态）
										//注意：若AF和AIE有有效时，则INT一直有效

#define Control_clear_AF		(0<<3)	//清除报警
#define Control_clear_TF		(0<<2)
										//当报警发生时，AF被值逻辑1；在定时器倒计数结束时，
										//TF被值逻辑1，他们在被软件重写前一直保持原有值，
										//若定时器和报警中断都请求时，中断源有AF和TF决定，
										//若要使清除一个标志位而防止另一标志位被重写，应运用逻辑
										//指令AND
#define Alarm_INT_Open			(1<<1)	//报警中断有效
#define Alarm_INT_Close			(0<<1)	//报警中断无效

#define Time_INT_Open			(1<<0)	//定时器中断有效
#define Time_INT_Close			(0<<0)	//定时器中断无效

//秒/VL寄存器
#define Accuracy_Clock_Yes		(0<<7)	//保证准确的时钟/日历数据
#define Accuracy_Clock_No		(1<<7)	//不保证准确的时钟/日历数据

//分钟报警寄存器
#define Alarm_minute_Open		(0<<7)	//分钟报警有效
#define Alarm_minute_Close		(1<<7)	//分钟报警无效

//小时报警寄存器
#define Alarm_hour_Open			(0<<7)	//小时报警有效
#define Alarm_hour_Close		(1<<7)	//小时报警无效

//日报警寄存器
#define Alarm_date_Open			(0<<7)	//日报警有效
#define Alarm_date_Close		(1<<7)	//日报警无效

//星期报警寄存器
#define Alarm_week_Open			(0<<7)	//星期报警有效
#define Alarm_week_Close		(1<<7)	//星期报警无效

//CLKOUT频率寄存器
#define CLKOUT_Open				(1<<7)	//CLKOUT输出有效
#define CLKOUT_Close			(0<<7)	//CLKOUT输出被禁止并设成高阻抗

#define CLKOUT_f32768			0x00	//输出32.768KHz
#define CLKOUT_f1024			0x01	//输出1024Hz
#define CLKOUT_f32				0x02	//输出32Hz
#define CLKOUT_f1				0x03	//输出1Hz

//定时器控制器寄存器
#define Time_Open				(1<<7)	//定时器有效
#define Time_Close				(0<<7)	//定时器无效

#define Time_f4096				0x00	//定时器时钟频率为4096Hz
#define Time_f64				0x01	//定时器时钟频率为64Hz
#define Time_f1					0x02	//定时器时钟频率为1Hz
#define Time_f160				0x03	//定时器时钟频率为1/60Hz

/**********************************************************
                       外部功能函数                      
**********************************************************/

u8 PCF8563_Check(void);	//PCF8563检测是否存在

void PCF8563_Write_Byte(u8 REG_ADD,u8 dat);	//PCF8563某寄存器写入一个字节数据

u8 PCF8563_Read_Byte(u8 REG_ADD);	//PCF8563某寄存器读取一个字节数据

void PCF8563_Operate_Register(u8 REG_ADD,u8 *pBuff,u8 num,u8 mode);	//PCF8563对时间日历寄存器操作，写入数据或者读取数据

void PCF8563_Set_Alarm(Time_Typedef *Alarm);	//闹铃设置

void PCF8563_ReadWrite_Time(u8 mode);	//PCF8563读取或者写入时间信息

void PCF8563_Time_Init(Time_Typedef *TimeVAL);	//时间日历初始化

void Time_Handle(void);	//时间日历数据处理函数


#endif
 
