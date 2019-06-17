/**********************************************************
* @ File name -> Adjust_time.c
* @ Version   -> V1.0
* @ Date      -> 01-19-2014
* @ Brief     -> 调整时间函数

 V1.
* @ Revise    -> 
**********************************************************/

#include "Adjust_time.h"

/**********************************************************
                        定义变量
**********************************************************/

u8 dis_mode;	//显示切换状态变量

u8 Time_Variable;
u8 Display_Value[5];	//调整时间用到缓存

//u8 time_display[13];

/**********************************************************
* 函数功能 ---> 时间日历显示刷新函数
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 每调整一个时间日历变量时，在要调整的变量有蓝色的背景颜色显示
**********************************************************/
void Refresh_Display_Time(void)
{	
	switch(dis_mode)
	{
		case 0:	//正常显示时间
				Time_Handle();	//读取时钟日期信息
				LCD_Display_String_Num(64, 271, Display_Date, 16, 13, 13, RED, WHITH);	//显示日期
				LCD_Display_String_Num(88, 287, Display_Time, 16, 8, 8, RED, WHITH);	//显示时间
				break;
		
		case 1:	//调整年
				Display_Value[0] = 0x32;
				Display_Value[1] = 0x30;
				Display_Value[2] = (TimeValue.year >> 4) + 0x30;
				Display_Value[3] = (TimeValue.year & 0x0f) + 0x30;
//				LCD_Display_String_Num(64, 271, (u8*)Display_Value, 4);
				LCD_Display_String_Num(64, 271, (u8*)Display_Value, 16, 5, 4, RED, WHITH);
				break;
		
		case 2:	//调整月
				Display_Value[0] = (TimeValue.month >> 4) + 0x30;
				Display_Value[1] = (TimeValue.month & 0x0f) + 0x30;
//				LCD_Display_String_Num(104, 271, (u8*)Display_Value, 2);
				LCD_Display_String_Num(104, 271, (u8*)Display_Value, 16, 5, 2, RED, WHITH);
				break;
		
		case 3:	//调整日
				Display_Value[0] = (TimeValue.date >> 4) + 0x30;
				Display_Value[1] = (TimeValue.date & 0x0f) + 0x30;
//				LCD_Display_String_Num(128, 271, (u8*)Display_Value, 2);
				LCD_Display_String_Num(128, 271, (u8*)Display_Value, 16, 5, 2, RED, WHITH);
				break;
		
		case 4:	//调整星期
				Display_Value[0] = (TimeValue.week >> 4) + 0x30;
				Display_Value[1] = (TimeValue.week & 0x0f) + 0x30;
//				LCD_Display_String_Num(152, 271, (u8*)Display_Value, 2);
				LCD_Display_String_Num(152, 271, (u8*)Display_Value, 16, 5, 2, RED, WHITH);
				break;
		
		case 5:	//调整小时
				Display_Value[0] = (TimeValue.hour >> 4) + 0x30;
				Display_Value[1] = (TimeValue.hour & 0x0f) + 0x30;
//				LCD_Display_String_Num(88, 287, (u8*)Display_Value, 2);
				LCD_Display_String_Num(88, 271, (u8*)Display_Value, 16, 5, 2, RED, WHITH);
				break;
		
		case 6:	//调整分钟
				Display_Value[0] = (TimeValue.minute >> 4) + 0x30;
				Display_Value[1] = (TimeValue.minute & 0x0f) + 0x30;
//				LCD_Display_String_Num(112, 287, (u8*)Display_Value, 2);
				LCD_Display_String_Num(112, 271, (u8*)Display_Value, 16, 5, 2, RED, WHITH);
				break;
		
		default:	//缺省值
				break;
	}
}
/**********************************************************
* 函数功能 ---> 按键处理函数
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void Key_Handle(void)
{
	u8 Key_Value;	//按键缓存
	
	Key_Value = KEY_Scan(0);	//读取按键值
//	if(Key_Value == 0)	return;
	
	//菜单按键
	if(Key_Value == KEY_LEFT)
	{
		if(dis_mode == 6)	//调整时间完成
		{
			dis_mode = 0;	//回到初始显示
//			Point_color = RED;	//画笔颜色
//			LCD_Draw_Char(10, 287, " ", 0);
			LCD_Display_String_BK(10, 0, "  ", 16, WHITH, WHITH);
			TimeValue.second = 0x00;	//置秒为0，重新开始计算
			PCF8563_ReadWrite_Time(0);	//更新时间信息
		}
		else
		{
//			Point_color = BLUE;	//画笔颜色
//			LCD_Draw_Char(10, 287, "A", 0);
			LCD_Display_String_BK(10, 0, "Ad", 16, BLUE, WHITH);
			dis_mode++;
		}
	}
	
	//取消按键
	if(Key_Value == KEY_RIGHT)
	{
		dis_mode = 0;
	}
	
	//加按键
	if(Key_Value == KEY_UP)
	{
		switch(dis_mode)
		{
			case 1: //调整年
					Time_Variable = HEX_to_BCD(TimeValue.year);	//转换成BCD
					if(Time_Variable == 99)	Time_Variable = 0;
					else Time_Variable++;

					TimeValue.year = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 2:	//调整月
					Time_Variable = HEX_to_BCD(TimeValue.month);	//转换成BCD
					if(Time_Variable == 12)	Time_Variable = 1;
					else Time_Variable++;

					TimeValue.month = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 3:	//调整日
					Time_Variable = HEX_to_BCD(TimeValue.date);	//转换成BCD
					if(Time_Variable == 31)	Time_Variable = 1;
					else Time_Variable++;

					TimeValue.date = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 4:	//调整星期
					Time_Variable = HEX_to_BCD(TimeValue.week);	//转换成BCD
					if(Time_Variable == 7)	Time_Variable = 1;
					else Time_Variable++;

					TimeValue.week = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 5:	//调整小时
					Time_Variable = HEX_to_BCD(TimeValue.hour);	//转换成BCD
					if(Time_Variable == 23)	Time_Variable = 0;
					else Time_Variable++;

					TimeValue.hour = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 6:	//调整分钟
					Time_Variable = HEX_to_BCD(TimeValue.minute);	//转换成BCD
					if(Time_Variable == 59)	Time_Variable = 0;
					else Time_Variable++;

					TimeValue.minute = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			default:
					break;
		}
	}
	
	//减按键
	if(Key_Value ==KEY_DOWN)
	{
		switch(dis_mode)
		{
			case 1:	//调整年
					Time_Variable = HEX_to_BCD(TimeValue.year);	//转换成BCD
					if(Time_Variable == 0)	Time_Variable = 99;
					else Time_Variable--;

					TimeValue.year = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 2:	//调整月
					Time_Variable = HEX_to_BCD(TimeValue.month);	//转换成BCD
					if(Time_Variable == 1)	Time_Variable = 12;
					else Time_Variable--;

					TimeValue.month = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 3:	//调整日
					Time_Variable = HEX_to_BCD(TimeValue.date);	//转换成BCD
					if(Time_Variable == 1)	Time_Variable = 31;
					else Time_Variable--;

					TimeValue.date = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 4:	//调整星期
					Time_Variable = HEX_to_BCD(TimeValue.week);	//转换成BCD
					if(Time_Variable == 1)	Time_Variable = 7;
					else Time_Variable--;

					TimeValue.week = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 5:	//调整小时
					Time_Variable = HEX_to_BCD(TimeValue.hour);	//转换成BCD
					if(Time_Variable == 0)	Time_Variable = 23;
					else Time_Variable--;

					TimeValue.hour = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			case 6:	//调整分钟
					Time_Variable = HEX_to_BCD(TimeValue.minute);	//转换成BCD
					if(Time_Variable == 0)	Time_Variable = 59;
					else Time_Variable--;

					TimeValue.minute = BCD_to_HEX(Time_Variable);	//更新数据
					break;
			
			default:
					break;
		}
	}
}
