
#include "stm32_config.h"

#include "led.h"
#include "key.h"
#include "lcd_tft.h"
#include "stmflash.h"


#include "hw_config.h"	//USB相关头文件
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "dfu_mal.h"

//#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)
u8 Dis_buffer[16];	//显示缓存

typedef  void (*pFunction)(void);

uint8_t DeviceState;
uint8_t DeviceStatus[6];
pFunction Jump_To_Application;
uint32_t JumpAddress;

void SGPIO_Init()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
 	
 //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA,PD端口时钟
	
 
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LED0-->PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	
}
/**********************************************************
                           主函数
**********************************************************/
int main(void)
{
	u8 i=0;

	delay_init(72);	//初始化延时函数
	USART1_Init(9600);	//初始化串口1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE); 	
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);      /*使能SWD 禁用JTAG*/

	DFU_Button_Config();	//初始化跳转APP程序按键
  GPIO_SetBits(GPIOA,GPIO_Pin_14);
	//检测是否进入DFU模式按键，开机没有按下则跳转到APP程序中执行
	if(DFU_Button_Read() == 1)
	{
		if(((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)	//检测APP地址是否合法
		{
			//跳转到APP地址开始执行，地址+4位置是复位中断入口
			JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
			Jump_To_Application = (pFunction) JumpAddress;

			//设置APP程序堆栈指针
			__set_MSP(*(__IO uint32_t*) ApplicationAddress);
			
			//跳转到APP程序中执行
			Jump_To_Application();
		}
	}

    SGPIO_Init();
	//按键已经按下，则进入APP升级模式
	DeviceState = STATE_dfuERROR;
	DeviceStatus[0] = STATUS_ERRFIRMWARE;
	DeviceStatus[4] = DeviceState;
	
	Set_System();
	Set_USBClock();
	USB_Init();
	
	while(1)
	{
		i++;
		delay_ms(10);
		if(i == 20)
		{
//			LED0 = ~LED0;
			i = 0;
		}
	}
} 
