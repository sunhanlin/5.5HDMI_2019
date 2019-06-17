/**********************************************************
* @ File name -> key.c
* @ Version   -> V1.0
* @ Date      -> 01-18-2014
* @ Brief     -> 按键驱动函数

 V1.*
* @ Revise    ->
**********************************************************/

#include "key.h"

/**********************************************************
* 函数功能 ---> KEY接口初始化
* 入口参数 ---> none
* 返回参数 ---> none 
* 功能说明 ---> none
**********************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);	//开启GPIOC外设时钟	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;	//初始化GPIOx.2---------->KEY0---->PC.2
																						//初始化GPIOx.3---------->KEY1---->PC.3
																						//初始化GPIOx.4---------->KEY2---->PC.4
																						//初始化GPIOx.5---------->KEY3---->PC.5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//设置为上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);	//初始化GPIO相关结构体

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//初始化GPIOx.0---------->KEY_UP---->PA.0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//设置为下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化GPIO相关结构体
}

/**********************************************************
* 函数功能 ---> 按键键值读取
* 入口参数 ---> mode：是否支持连按
*                    0：不支持连按
*                    1：支持连按（连按按键修改请修改宏定义的相关参数）
* 返回参数 ---> 返回按键值 
* 功能说明 ---> none
**********************************************************/
//u8 KEY_Scan(u8 mode)
//{
//	static u8 KeyStatus = 0;	//静态变量，保存按键状态
//	static u8 KeyReturn = 0;	//静态变量，保存当前按键的键值
//           u8 KeyPress;			//读取按键按下值

////	KeyPress = (u8)((((GPIOC->IDR & 0x0000003c)) >> 1) | (~(GPIOA->IDR & 0x00000001)));	//读取按键值
//	
//	KeyPress = KeyInput & 0x1f;	//得到按键值
//  
//    switch(KeyStatus)
//    {
//		case 0:	//按键初始状态
//				if(KeyPress != 0x1f)	KeyStatus = 1;	//如果有按键按下，进入按下确认状态
//				break;

//		case 1:	//按键确认状态
//				if(KeyPress == (KeyInput & 0x1f))	//按键被按下
//				{
//					     if(KeyPress == 0x1d)	KeyReturn = KEY_LEFT;
//					else if(KeyPress == 0x1b)	KeyReturn = KEY_UP;
//					else if(KeyPress == 0x17)	KeyReturn = KEY_DOWN;
//					else if(KeyPress == 0x0f)	KeyReturn = KEY_RIGHT;
//					else if(KeyPress == 0x1e)	KeyReturn = KEY_WKUP;
//				
//					KeyStatus = 2; //按键下一个状态为释放和连按判断状态
//				}
//				else	KeyStatus = 0;	//按键已经弹起
//				
//				break;

//	    case 2:	//按键释放状态
//				if(KeyPress == 0x1f)         //按键释放
//				{
//					KeyStatus = 0;  //按键下一个状态为释放状态
//				}
//				
//				/******************************************
//				              进入连按确认环节
//				******************************************/
//				
//				if(mode)	//支持连按
//				{
//					KeyReturn = KeyPress | Key_ContPress;	//返回连按的按键值
//				}
//				
//				/*****************************************/
//				break; 
//				
//        default: break;
//    }

//	return KeyReturn;  //返回当前按键
//}



u8 KEY_Scan(u8 mode)
{
	static u8 key_loo=1;	//按键松开标志
	if(mode)	key_loo = 1;	//按键松开
	if(key_loo && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 1))	//按键按下
	{					
		delay_ms(10);	//延时消抖动
        key_loo = 0;
             if(KEY0 == 0)	return KEY_LEFT;
        else if(KEY1 == 0)	return KEY_UP;
        else if(KEY2 == 0)  return KEY_DOWN;
        else if(KEY3 == 0)  return KEY_RIGHT;
		else if(KEY4 == 1)	return KEY_WKUP;
	}
	else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 0)	key_loo = 1;	//按键已经释放	
		
	return 0;

}
 
