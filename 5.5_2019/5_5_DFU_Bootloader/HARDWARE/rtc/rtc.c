/**********************************************************
* @ File name -> rtc.c
* @ Version   -> V1.0
* @ Date      -> 03-02-2014
* @ Brief     -> STM32内部RTC驱动函数

 V1.
* @ Revise    -> 
**********************************************************/

#include "rtc.h"

/**********************************************************
                   定义显示时间格式
             要改变显示的格式请修改此数组
**********************************************************/

uint8_t RTC_Display_Time[8] = {0x30,0x30,0x3a,0x30,0x30,0x3a,0x30,0x30};	
					//时间显示缓存   格式  00:00:00

uint8_t RTC_Display_Date[13] = {0x32,0x30,0x31,0x33,0x2f,0x31,0x30,0x2f,0x32,0x30,0x20,0x37,0x57};
					//日期显示缓存   格式  2013/10/20 7W

/**********************************************************
                       定义相关变量
**********************************************************/

uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
const uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};	//平年的月份日期表

STMRTC_Typedef RTC_TimeValue;	//定义时间数据缓存

/**********************************************************
* 函数功能 ---> 导入RTC默认时间值
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void RTC_Default(void)
{
	/* 设置默认时间信息 */
	RTC_TimeValue.year = 2014;
	RTC_TimeValue.month = 3;
	RTC_TimeValue.date = 3;
	RTC_TimeValue.week = 1;
	RTC_TimeValue.hour = 22;
	RTC_TimeValue.minute = 8;
	RTC_TimeValue.second = 0;
}

/**********************************************************
* 函数功能 ---> 设置RTC中断优先级
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
static void RTC_SetNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**********************************************************
* 函数功能 ---> 判断年份是不是闰年
* 入口参数 ---> years：年份
* 返回数值 ---> 0：不是闰年
*               1：闰年
* 功能说明 ---> 闰年有366天，2月为29天
*               平年有365天，2月为28天
*               1月共31天，3月共31天，4月共30天，5月共31天
*               6月共30天，7月共31天，8月共31天，9月共30天
*               10月共31天，11月共30天，12月共31天
**********************************************************/
uint8_t RTC_Check_Year(uint16_t years)
{
	if(years % 4 == 0)	//必须能被4整除才能列入闰年范围，还要进一步判断
	{
		if(years % 100 == 0)	//能被100整除，还要能被400整除的才是闰年
		{
			if(years % 400 == 0)	return 1;	//还要能被400整除
			else	return 0;	//平年年
		}
		else	return 1;	//不能被100整除但可以被4整除的就是闰年
	}
	else	return 0;	//平年
}
/**********************************************************
* 函数功能 ---> 设置RTC时间
* 入口参数 ---> none
* 返回数值 ---> 1：失败
*               其他：总秒数
* 功能说明 ---> none
**********************************************************/
uint32_t RTC_SetTime(void)
{	
	uint16_t k;
	uint32_t seconds;

	if((RTC_TimeValue.year < 2001) || (RTC_TimeValue.year > 2099))	return 1;	//不合法的年数

	for(k = 2001;k < RTC_TimeValue.year;k++)
	{
		if(RTC_Check_Year(k))	seconds += 31622400;	//闰年的秒钟数
		else	seconds += 31536000;
	}
	RTC_TimeValue.month -= 1;	//化为0 ~ 11
	for(k = 0;k < RTC_TimeValue.month;k++)	//月份秒钟数相加
	{
		seconds += (uint32_t)mon_table[k] * 86400;	//月份的秒钟数相加
		if(RTC_Check_Year(RTC_TimeValue.year) && k==1)	seconds += 86400;	//闰年2月份增加一天的秒数
	}
	seconds += (uint32_t)(RTC_TimeValue.date - 1) * 86400;	//将日的秒数加上
	seconds += (uint32_t)RTC_TimeValue.hour * 3600;	//将小时的秒数加上
	seconds += (uint32_t)RTC_TimeValue.minute * 60;	//将分钟的秒数加上
	seconds += RTC_TimeValue.second;	//加上秒

	return seconds;
}
/**********************************************************
* 函数功能 ---> 读取RTC时间日历信息
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 读取到的时间信息直接保存在结构体中，方便后面程序调用
**********************************************************/
void RTC_GetTime(void)
{
	static uint16_t daycnt=0;
	uint8_t yearH,yearL;
	uint16_t temp2;
	uint16_t temp1=0;
	uint32_t temp=0;
	uint32_t timecount=0;

	timecount = RTC_GetCounter();	//得到RTC计数器值

	temp = timecount/86400;   //得到天数
	if(daycnt != temp)//超过一天了
	{	  
		daycnt = temp;
		temp1 = 2001;	//从2001年开始
		while(temp >= 365)
		{				 
			if(RTC_Check_Year(temp1))	//是闰年
			{
				if(temp >= 366)	temp -= 366;	//闰年的秒钟数
				else	
				{
					temp1++;
					break;
				}  
			}
			else	temp -= 365;	//平年 
			temp1++;  
		}
		RTC_TimeValue.year = temp1;	//得到年份   
		temp1 = 0;
		while(temp >= 28)	//超过了一个月
		{
			if(RTC_Check_Year(RTC_TimeValue.year)&&temp1==1)	//当年是不是闰年/2月份
			{
				if(temp >= 29)	temp -= 29;	//闰年的秒钟数
				else	break; 
			}
			else 
			{
				if(temp >= mon_table[temp1])	temp -= mon_table[temp1];	//平年
				else break;
			}
			temp1++;  
		}
		RTC_TimeValue.month = temp1 + 1;	//得到月
		RTC_TimeValue.date = temp + 1;	//得到日 
	}
	temp = timecount%86400;	//得到秒钟数
	RTC_TimeValue.hour = temp/3600;	//得到小时
	RTC_TimeValue.minute = (temp%3600)/60;	//得到分钟
	RTC_TimeValue.second = (temp%3600)%60;	//得到秒钟

	//获取星期
	yearH = RTC_TimeValue.year/100;
	yearL = RTC_TimeValue.year%100;
	if(yearH > 19)	yearL += 100;	//21世纪，年份加100

	temp2 = yearL + yearL/4;
	temp2 = temp2%7;
	temp2 = temp2 + RTC_TimeValue.date + table_week[RTC_TimeValue.month-1]; 
	if(yearL%4 == 0 && RTC_TimeValue.month<3)	temp2--;
	RTC_TimeValue.week = temp2%7;	//得到星期	
}
/**********************************************************
* 函数功能 ---> 初始化RTC_Time
* 入口参数 ---> none
* 返回数值 ---> 操作过程中错误代码
*               0：成功
*               1：LSE失败
*               2：LSI失败
*               3：其他错误
* 功能说明 ---> none
**********************************************************/
uint8_t RTC_Init(void)
{
	uint8_t temp=0;
	
	/* 检测是不是第一次配置 */
	if(BKP_Read_Byte(BKP_DR2) != RTC_Init_FlagData)	//第一次配置 
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//打开时钟
		PWR_BackupAccessCmd(ENABLE);	//允许写入RTC和后备寄存器
		BKP_DeInit();	//复位

		/* 选择RTC时钟源 */
#if RTC_CRYSTAL_SELECT==1	/* 使用LSE */
		RCC_LSEConfig(RCC_LSE_ON);	//外部32kHz振荡器开启
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//等待LSE准备好
		{
			temp++;
			delay_ms(10);
			if(temp >= 230)	return 1;	//LSE未就绪
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);	//设置LSE为RTC时钟源
#elif RTC_CRYSTAL_SELECT==2	/* 使用内部LSI */
		RCC_LSICmd(ENABLE);	//内部40kHz RC振荡器开启
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)	//等待LSI准备好
		{
			temp++;
			delay_ms(10);
			if(temp >= 230)	return 2;	//LSI未就绪
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);	//设置LSI为RTC时钟源
#elif RTC_CRYSTAL_SELECT==3	/* 使用HSE/128 */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);	//设置HSE/128为RTC时钟源
#else
		return 3;	//其他错误
#endif	
		/* 结束RTC时钟源选择 */

		RCC_RTCCLKCmd(ENABLE);	//RTC时钟开启
		RTC_WaitForSynchro();	//等待RTC和BKP同步
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器操作完成
		RTC_EnterConfigMode();	//允许配置
#if ((RTC_CRYSTAL_SELECT==1) || (RTC_CRYSTAL_SELECT==2))	/* 选择LSE或者LSI时钟源 */
		RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
#elif RTC_CRYSTAL_SELECT==3	/* 选择HSE/128 */
		RTC_SetPrescaler(62499);
#else
		return 3;
#endif
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器操作完成
		RTC_Default();	//导入默认时间
		RTC_SetCounter(RTC_SetTime());	//设置RTC时间
		RTC_ExitConfigMode();	//退出配置模式
		BKP_Write_Byte(BKP_DR2, RTC_Init_FlagData);	//写入标记
	}
	else	//不是第一次配置，继续计时
	{
		RTC_WaitForSynchro();	//等待RTC和BKP同步
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器操作完成
	}
	RTC_SetNVIC();	//设置RTC中断
	RTC_GetTime();	//读取时间

	return 0;	//成功
}
/**********************************************************
* 函数功能 ---> 时间日历数据处理函数
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 将读取到的时间日期信息转换成ASCII后保存到时间格式数组中
**********************************************************/
void RTC_Time_Handle(void)
{
	/******************************************************
	            时间信息转换为ASCII码可视字符
	******************************************************/
	
	RTC_Display_Time[6] = (RTC_TimeValue.second%100/10) + 0x30;
	RTC_Display_Time[7] = (RTC_TimeValue.second%10) + 0x30;	//Second

	RTC_Display_Time[3] = (RTC_TimeValue.minute%100/10) + 0x30;
	RTC_Display_Time[4] = (RTC_TimeValue.minute%10) + 0x30;	//Minute

	RTC_Display_Time[0] = (RTC_TimeValue.hour%100/10) + 0x30;
	RTC_Display_Time[1] = (RTC_TimeValue.hour%10) + 0x30;	//Hour 

	RTC_Display_Date[8] = (RTC_TimeValue.date%100/10) + 0x30;
	RTC_Display_Date[9] = (RTC_TimeValue.date%10) + 0x30;	//Date

	RTC_Display_Date[5] = (RTC_TimeValue.month%100/10) + 0x30;
	RTC_Display_Date[6] = (RTC_TimeValue.month%10) + 0x30;	//Month

	RTC_Display_Date[0] = (u8)(RTC_TimeValue.year%10000/1000) + 0x30;	//2
	RTC_Display_Date[1] = (u8)(RTC_TimeValue.year%1000/100) + 0x30;	//0
	RTC_Display_Date[2] = (RTC_TimeValue.year%100/10) + 0x30;
	RTC_Display_Date[3] = (RTC_TimeValue.year%10) + 0x30;	//Year

	if(RTC_TimeValue.week == 0)	//转化为7 
	{
		RTC_Display_Date[11] = 0x37;
	}
	else	RTC_Display_Date[11] = (RTC_TimeValue.week%10) + 0x30;	//week
}
/**********************************************************
* 函数功能 ---> RTC中断服务函数
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_SEC) != RESET)	//秒钟中断
	{
		RTC_ClearITPendingBit(RTC_IT_SEC);	//清除秒中断标志
		RTC_WaitForLastTask();	//等待操作完成
		RTC_GetTime();	//读取时间
	}
	if(RTC_GetITStatus(RTC_IT_OW) != RESET)	//溢出中断
	{
		RTC_ClearITPendingBit(RTC_IT_OW);	//清溢出中断标志
		RTC_WaitForLastTask();	//等待操作完成
	}
	if(RTC_GetITStatus(RTC_IT_ALR) != RESET)	//闹钟中断
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);	//清闹钟中断标志
		RTC_WaitForLastTask();	//等待操作完成
	}
	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_ALR|RTC_IT_OW);	//清中断标志
	RTC_WaitForLastTask();	//等待操作完成
}

