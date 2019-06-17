/**
  ******************************************************************************
  * @file    hw_config.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Hardware Configuration & Setup
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usart.h"
#include "usb_istr.h"

//????  1:??
 __IO uint8_t PrevXferComplete=1;

/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Private functions ---------------------------------------------------------*/

//USB唤醒中断服务函数
void USBWakeUp_IRQHandler(void) 
{
	EXTI_ClearITPendingBit(EXTI_Line18);   //EXTI_18连接到USB唤醒事件
} 
//USB中断处理函数
void USB_LP_CAN1_RX0_IRQHandler(void) 
{
	USB_Istr();
} 


/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
//USB时钟配置函数,USBclk=48Mhz@HCLK=72Mhz
void Set_USBClock(void)
{
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);	//USBclk=PLLclk/1.5=48Mhz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);	
} 


/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
 	printf("usb enter low power mode\r\n");
	bDeviceState=SUSPENDED;//bDeviceState记录USB连接状态，在usb_pwr.c里面定义
} 





/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;
  
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else 
  {
    bDeviceState = ATTACHED;
  }
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure; 
	/* Configure the EXTI line 18 connected internally to the USB IP */
	EXTI_ClearITPendingBit(EXTI_Line18);		//  开启线18上的中断
	EXTI_InitStructure.EXTI_Line = EXTI_Line18; 	// USB resume from suspend mode
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//line 18上事件上升降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 	

	/* Enable the USB interrupt */
#include "stm32f10x.h"                  // Device header
#include "stm32f10x.h"                  // Device header
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;//组2，优先级次之 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USB Wake-up interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;   //组2，优先级最高	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);  	
}


/*******************************************************************************
* Function Name  : USB_Cable_Config.
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : NewState: new state.
* Output         : None.
* Return         : None
*******************************************************************************/

//USB接口配置(配置1.5K上拉电阻,ALIENTEK的M3系列开发板，不需要配置,固定加了上拉电阻)
//NewState:DISABLE,不上拉，ENABLE,上拉
void USB_Cable_Config (FunctionalState NewState)
{ 
	if (NewState!=DISABLE)printf("usb pull up enable\r\n"); 
	else printf("usb pull up disable\r\n"); 
}


/*******************************************************************************
* Function Name : Joystick_Send.
* Description   : prepares buffer to be sent containing Joystick event infos.
* Input         : Keys: keys received from terminal.
* Output        : None.
* Return value  : None.
*******************************************************************************/
void Joystick_Send(u16 x,u16 y,u8 s)
{
	u16 m;
	uint8_t Mouse_Buffer[6] = {0};
	Mouse_Buffer[0]=0x01;
	Mouse_Buffer[1]=s;
	Mouse_Buffer[2]=x&0xff;
	Mouse_Buffer[3]=(x>>8)&0xff;
	Mouse_Buffer[4]=y&0xff;
	Mouse_Buffer[5]=(y>>8)&0xff;    
	while(PrevXferComplete==0)
	{
		m++;
		if(m>0xff00) return;
	}
	UserToPMABufferCopy(Mouse_Buffer, GetEPTxAddr(ENDP1), 6);
	SetEPTxCount(ENDP1, 6);
	SetEPTxValid(ENDP1); 
	PrevXferComplete=0;	
}

void MulTouch_Send(u16 *x,u16 *y,u8 *s,u8 *id,u8 n)
{
	u8 i;
 u8 ReportBuf[14];
 u32 m;
 if(n == 0) return;
 if(n > MAX_TOUCH_POINT) 
 {
  n = MAX_TOUCH_POINT;
 }
 ReportBuf[0] = 0x02; 
 for(i = 0; i < n;) 
 {
  ReportBuf[1] = s[i]; 
  ReportBuf[2] = id[i]; 
  ReportBuf[3] = x[i] & 0xFF; 
  ReportBuf[4] = (x[i] >> 8) & 0xFF;
  ReportBuf[5] = y[i] & 0xFF; 
  ReportBuf[6] = (y[i] >> 8) & 0xFF; 
  if(i == 0) 
  {
   ReportBuf[13] = n; 
  }
  else 
  {
   ReportBuf[13] = 0;
  }
  i ++;
  if(i < n) 
  {
   ReportBuf[7] = s[i]; 
   ReportBuf[8] = id[i]; 
   ReportBuf[9] = x[i] & 0xFF; 
   ReportBuf[10] = (x[i] >> 8) & 0xFF; 
   ReportBuf[11] = y[i] & 0xFF; 
   ReportBuf[12] = (y[i] >> 8) & 0xFF; 
   i ++;
  }
  else 
  {
   u8 j;
   for(j = 7; j < 13; j++)
   {
    ReportBuf[j] = 0;
   }
  }
	while(PrevXferComplete==0)
	{
		//m++;
		//if(m>0x000ff000) return;
	}
	
	UserToPMABufferCopy(ReportBuf, GetEPTxAddr(ENDP1), 14);
	SetEPTxCount(ENDP1, 14);
	SetEPTxValid(ENDP1);	
  PrevXferComplete=0;	
	}	
  
}


//USB使能连接/断线   enable:0,断开;1,允许连接	
void USB_Port_Set(u8 enable)
{/*
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    //使能PORTA时钟	 
	if(enable)_SetCNTR(_GetCNTR()&(~(1<<1)));//退出断电模式
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;	    		  
	}*/
	 
	if(enable)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_3);
	}else
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	}
	
} 



/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(uint32_t*)ID1;
  Device_Serial1 = *(uint32_t*)ID2;
  Device_Serial2 = *(uint32_t*)ID3;
  
  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &Joystick_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &Joystick_StringSerial[18], 4);
  }
}

/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
