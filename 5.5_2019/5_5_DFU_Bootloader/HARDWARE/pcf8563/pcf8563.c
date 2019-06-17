/**********************************************************
* @ File name -> pcf8563.c
* @ Version   -> V1.0.3
* @ Date      -> 03-09-2014
* @ Brief     -> 时钟芯片PCF8563驱动函数

 V1.0.1
* @ Revise    -> A、增加显示时间日期格式数组
* @           -> B、增加读取时间处理函数，读取到的时间日期信息直接转换成ASCII保存到时间格式数组中
* @           -> C、调用时间日期处理函数，显示或者打印到串口的话直接显示或者打印时间格式数组即可

 V1.0.2
* @ Revise    -> 修正年结构为16位数值，数值位，比如20xx、19xx

 V1.0.3
* @ Revise    -> 增加PCF8563是否存在检测函数
**********************************************************/

#include "pcf8563.h"

/**********************************************************
                   定义显示时间格式
             要改变显示的格式请修改此数组
**********************************************************/

u8 Display_Time[8] = {0x30,0x30,0x3a,0x30,0x30,0x3a,0x30,0x30};	
					//时间显示缓存   格式  00:00:00

u8 Display_Date[13] = {0x32,0x30,0x31,0x33,0x2f,0x31,0x30,0x2f,0x32,0x30,0x20,0x37,0x57};
					//日期显示缓存   格式  2013/10/20 7W

/**********************************************************
                    定义相关的变量函数
**********************************************************/

Time_Typedef TimeValue;	//定义时间数据指针

u8 Time_Buffer[16];	//时间日历数据缓存

/**********************************************************
* 函数功能 ---> PCF8563某寄存器写入一个字节数据
* 入口参数 ---> REG_ADD：要操作寄存器地址
*               dat：要写入的数据
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void PCF8563_Write_Byte(u8 REG_ADD,u8 dat)
{
	IIC_Start();
	if(!(IIC_Write_Byte(PCF8563_Write)))	//发送写命令并检查应答位
	{
		IIC_Write_Byte(REG_ADD);
		IIC_Write_Byte(dat);	//发送数据
	}
	IIC_Stop();
} 
/**********************************************************
* 函数功能 ---> PCF8563某寄存器读取一个字节数据
* 入口参数 ---> REG_ADD：要操作寄存器地址
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
u8 PCF8563_Read_Byte(u8 REG_ADD)
{
	u8 ReData;
	IIC_Start();
	if(!(IIC_Write_Byte(PCF8563_Write)))	//发送写命令并检查应答位
	{
		IIC_Write_Byte(REG_ADD);	//确定要操作的寄存器
		IIC_Start();	//重启总线
		IIC_Write_Byte(PCF8563_Read);	//发送读取命令
		ReData = IIC_Read_Byte();	//读取数据
		IIC_Ack(1);	//发送非应答信号结束数据传送
	}
	IIC_Stop();
	return ReData;
}
/**********************************************************
* 函数功能 ---> PCF8563检测是否存在
* 入口参数 ---> none
* 返回数值 ---> 0：正常
*               1：PCF8563错误或者损坏
* 功能说明 ---> 向定时器倒计时寄存器写入一个数值再读取出来做对比，相同正确，不同则错误
**********************************************************/
u8 PCF8563_Check(void)
{
	u8 test_value=0;
	u8 Time_Count=0;	//定时器倒计时数据缓存

	if(PCF8563_Read_Byte(Address_Time) & 0x80)	//如果打开了定时器，则先关闭
	{
		PCF8563_Write_Byte(Address_Time, Time_Close);	//先关闭定时器
		Time_Count = PCF8563_Read_Byte(Address_Time_VAL);	//先保存计数值
	}

	PCF8563_Write_Byte(Address_Time_VAL, PCF8563_Check_Data);	//写入检测值
	for(test_value = 0;test_value < 250;test_value++)	{}	//延时一定时间再读取
	test_value = PCF8563_Read_Byte(Address_Time_VAL);	//再读取回来

	if(Time_Count != 0)	//启动了定时器功能，则恢复
	{
		PCF8563_Write_Byte(Address_Time_VAL, Time_Count);	//恢复现场
		PCF8563_Write_Byte(Address_Time, Time_Open);	//启动定时器
	}

	if(test_value != PCF8563_Check_Data)	return 1;	//器件错误或者损坏
	
	return 0;	//正常
}
/**********************************************************
* 函数功能 ---> PCF8563对时间日历寄存器操作，写入数据或者读取数据
* 入口参数 ---> REG_ADD：要操作寄存器起始地址
*               *WBuff：写入数据缓存
*               num：写入数据数量
*               mode：操作模式。0：写入数据操作。1：读取数据操作
* 返回数值 ---> none
* 功能说明 ---> 连续写入n字节或者连续读取n字节数据
**********************************************************/	
void PCF8563_Operate_Register(u8 REG_ADD,u8 *pBuff,u8 num,u8 mode)
{
	u8 i;
	if(mode)	//读取数据
	{
		IIC_Start();
		if(!(IIC_Write_Byte(PCF8563_Write)))	//发送写命令并检查应答位
		{
			IIC_Write_Byte(REG_ADD);	//定位起始寄存器地址
			IIC_Start();	//重启总线
			IIC_Write_Byte(PCF8563_Read);	//发送读取命令
			for(i = 0;i < num;i++)
			{
				*pBuff = IIC_Read_Byte();	//读取数据
				if(i == (num - 1))	IIC_Ack(1);	//发送非应答信号
				else IIC_Ack(0);	//发送应答信号
				pBuff++;
			}
		}
		IIC_Stop();	
	}
	else	//写入数据
	{		 	
		IIC_Start();
		if(!(IIC_Write_Byte(PCF8563_Write)))	//发送写命令并检查应答位
		{
			IIC_Write_Byte(REG_ADD);	//定位起始寄存器地址
			for(i = 0;i < num;i++)
			{
				IIC_Write_Byte(*pBuff);	//写入数据
				pBuff++;
			}
		}
		IIC_Stop();
	}
}
/**********************************************************
* 函数功能 ---> PCF8563读取或者写入时间信息
* 入口参数 ---> *pBuff：写入数据缓存
*               mode：操作模式。0：写入数据操作。1：读取数据操作
* 返回数值 ---> none
* 功能说明 ---> 连续写入n字节或者连续读取n字节数据
**********************************************************/
void PCF8563_ReadWrite_Time(u8 mode)
{
	u8 Time_Register[8];	//定义时间缓存
	
	if(mode)	//读取时间信息
	{
		PCF8563_Operate_Register(Address_second,Time_Register,7,1);	//从秒地址（0x02）开始读取时间日历数据
		
		/******将数据复制到时间结构体中，方便后面程序调用******/
		TimeValue.second = Time_Register[0] & Shield_secondBit;	//秒数据
		TimeValue.minute = Time_Register[1] & Shield_minuteBit;	//分钟数据
		TimeValue.hour   = Time_Register[2] & Shield_hourBit;	//小时数据
		TimeValue.date   = Time_Register[3] & Shield_dateBit;	//日数据
		TimeValue.week   = Time_Register[4] & Shield_weekBit;	//星期数据
		TimeValue.month  = Time_Register[5] & Shield_monthBit;	//月数据
		TimeValue.year   = Time_Register[6] | PCF8563_YEARDATA;	//年数据
	}
	else
	{
		if(TimeValue.week == 0x07)	TimeValue.week = 0x00;	//寄存器数值为0 ~ 6

		/******从时间结构体中复制数据进来******/
		Time_Register[0] = TimeValue.second | Accuracy_Clock_Yes;	//秒，保证准确时间
		Time_Register[1] = TimeValue.minute;						//分钟
		Time_Register[2] = TimeValue.hour;							//小时
		Time_Register[3] = TimeValue.date;							//日
		Time_Register[4] = TimeValue.week;							//星期
		Time_Register[5] = TimeValue.month | (0<<7);				//月，设置年数据位20xx年
		Time_Register[6] = (u8)TimeValue.year;						//年
		
//		PCF8563_Write_Byte(Address_control1,Control_Chip_Stop);	//芯片停止运行
		PCF8563_Operate_Register(Address_second,Time_Register,7,0);	//从秒地址（0x00）开始写入时间日历数据
//		PCF8563_Write_Byte(Address_control1,Control_Chip_Run);	//启动芯片
	}
}

/**********************************************************
* 函数功能 ---> 时间日历初始化
* 入口参数 ---> *TimeVAL：RTC芯片寄存器值指针
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void PCF8563_Time_Init(Time_Typedef *TimeVAL)
{
	if(TimeVAL->week == 0x07)	TimeVAL->week = 0x00;	//寄存器数值为0 ~ 6
	
	//控制/状态寄存器
	Time_Buffer[0]  = TimeVAL->cont1;	//控制/状态寄存器1
	Time_Buffer[1]  = TimeVAL->cont2;	//控制/状态寄存器2
	//时间日历数据
	Time_Buffer[2]  = TimeVAL->second | Accuracy_Clock_Yes;	//保证准确时间
	Time_Buffer[3]  = TimeVAL->minute;
	Time_Buffer[4]  = TimeVAL->hour;
	Time_Buffer[5]  = TimeVAL->date;
	Time_Buffer[6]  = TimeVAL->week;
	Time_Buffer[7]  = TimeVAL->month | Set_Year20xx;	//设置为20xx年
	Time_Buffer[8]  = (u8)TimeVAL->year;
	//闹铃报警
	Time_Buffer[9]  = TimeVAL->alarmmin | Alarm_minute_Close;	//分钟报警
	Time_Buffer[10] = TimeVAL->alarmhour | Alarm_hour_Close;	//小时报警
	Time_Buffer[11] = TimeVAL->alarmdate | Alarm_date_Close;	//日报警
	Time_Buffer[12] = TimeVAL->alarmweek | Alarm_week_Close;	//星期报警
	//频率输出设置；定时器设置
	Time_Buffer[13] = TimeVAL->CLKcont;	//频率输出控制
	Time_Buffer[14] = TimeVAL->timecont;	//定时器控制
	Time_Buffer[15] = TimeVAL->timeconut;	//定时器倒计时
	
//	PCF8563_Write_Byte(Address_control1,Control_Chip_Stop);	//芯片停止运行
	PCF8563_Operate_Register(Address_control1,Time_Buffer,16,0);	//从控制/状态寄存器1（0x00）开始写入16组数据
//	PCF8563_Write_Byte(Address_control1,Control_Chip_Run);	//启动芯片
}
/**********************************************************
* 函数功能 ---> 时间日历数据处理函数
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 将读取到的时间日期信息转换成ASCII后保存到时间格式数组中
**********************************************************/
void Time_Handle(void)
{
	/******************************************************
	                   读取时间日期信息
	******************************************************/
	
	PCF8563_ReadWrite_Time(1);	//获取时间日历数据
	
	/******************************************************
	            时间信息转换为ASCII码可视字符
	******************************************************/
	
	Display_Time[6] = (TimeValue.second >> 4) + 0x30;
	Display_Time[7] = (TimeValue.second & 0x0f) + 0x30;	//Second

	Display_Time[3] = (TimeValue.minute >> 4) + 0x30;
	Display_Time[4] = (TimeValue.minute & 0x0f) + 0x30;	//Minute

	Display_Time[0] = (TimeValue.hour >> 4) + 0x30;
	Display_Time[1] = (TimeValue.hour & 0x0f) + 0x30;	//Hour 

	Display_Date[8] = (TimeValue.date >> 4) + 0x30;
	Display_Date[9] = (TimeValue.date & 0x0f) + 0x30;	//Date

	Display_Date[5] = (TimeValue.month >> 4) + 0x30;
	Display_Date[6] = (TimeValue.month & 0x0f) + 0x30;	//Month

	Display_Date[0] = (u8)(TimeValue.year >> 12) + 0x30;	//2
	Display_Date[1] = (u8)((TimeValue.year & 0x0f00) >> 8) + 0x30;	//0
	Display_Date[2] = (TimeValue.year >> 4) + 0x30;
	Display_Date[3] = (TimeValue.year & 0x0f) + 0x30;	//Year

	/******************************************************
	   星期日显示的是7，其他的就是对应的数字显示，将0转换为7
	******************************************************/
	
	if(TimeValue.week == 0x00)
	{	Display_Date[11] = 0x37;	}	//week
	else
	{	Display_Date[11] = (TimeValue.week & 0x0f) + 0x30;	}

}

