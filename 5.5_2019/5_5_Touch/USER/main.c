#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"  
#include "24cxx.h" 
#include "myiic.h"
#include "touch.h" 
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h" 
#include "ctiic.h"
 //#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled */
void SGPIO_Init()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
 	
//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA,PD端口时钟
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE); 
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);      /*使能SWD 禁用JTAG*/
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LED0-->PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	
}
 int main(void)
 {
	u8 num,num_last,i=0,j=0,m=0;
	u16 x_last[10]={0},y_last[10]={0};
	u16 x0[10]={0},y0[10]={0};
	u8 touch_sta[10]={0};
	u8 pre_id[10]={0};
	u8 id_send[10]={0};
  u8 tempBuf[61];
	
	SCB->VTOR = FLASH_BASE | 0X8000; 
	delay_init();	//延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组设置	 
	uart_init(9600);//串口初始化为9600
  SGPIO_Init();
	
	
	USB_Port_Set(0); 	//USB先断开
	delay_ms(300);
	USB_Port_Set(1);	//USB再次连接
	//USB配置
	USB_Interrupts_Config();    
	Set_USBClock();   
	USB_Init();	

	
	TP_Init();		//初始化触摸屏      						    	   
 	while(1)
	{
		num=tp_dev.scan(); 
    if(num > 0){
      MulTouch_Send(tp_dev.x,tp_dev.y,tp_dev.tp_sta,tp_dev.tp_id,num);
    }
    /*
    FT3517U_RD_Reg(FT_REG_NUM_FINGER,tempBuf,1);
    for(i = 0; i < (tempBuf[0]&0x0F); i++){
      FT3517U_RD_Reg(FT_REG_NUM_FINGER+1+i*6,tempBuf+1+i*6,4);
      tp_dev.tp_sta[i] = ((tempBuf[1]&0xC0)>>6);
      tp_dev.tp_id[i] = (tempBuf[3]&0xF0)>>4;
      tp_dev.x[i] = (1080-(u16)(((u16)(tempBuf[1]&0x0F)<<8)|tempBuf[2])) * 4095/1080;//0~1080
      tp_dev.y[i] = (1920-(u16)(((u16)(tempBuf[3]&0x0F)<<8)|tempBuf[4]))* 4095/1920;//0~1920
      if(tp_dev.tp_sta[0] == 0x00){//put down
        tp_dev.tp_sta[0] = 0x07;
      }else if(tp_dev.tp_sta[0] == 0x01){//put up
        tp_dev.tp_sta[0] = 0x06;
      }else if(tp_dev.tp_sta[0] == 0x02){//Contact
        tp_dev.tp_sta[0] = 0x07;
      }else{//err
        tp_dev.tp_sta[0] = 0x00;
      }
    }
    MulTouch_Send(tp_dev.x,tp_dev.y,tp_dev.tp_sta,tp_dev.tp_id,(tempBuf[0]&0x0F));*/
#if 0
		if(num!=0)	//触摸屏被按下
		{
			//printf("num=%d\r\n",num);
			if(num<num_last)  //有触摸点被释放
			{
				u8 n=0;
				for(i=0;i<num_last;i++)
				{
					u8 flag=0;
					for(j=0;j<num;j++)         //遍历当前id
					{
						if(tp_dev.tp_id[j]==pre_id[i])
						{
							flag=1;
							break;
						}
					}
					if(flag==0)            //被释放
					{
						x0[n]=x_last[i];
						y0[n]=y_last[i];
						touch_sta[n]=0x06;
						id_send[n]=pre_id[i];
						//printf("11111x:%d  y:%d \r\n",x0[n],y0[n]);
						n++;
					}
				}
				if(n>0&&bDeviceState==CONFIGURED)
			{
				//printf("MulTouch_Send1:\r\n");
				//printf("x0[0]=%d",x0[0]);
				//printf("y0[0]=%d",y0[0]);
				printf("n>0&&bDeviceState==CONFIGURED\r\n");
				MulTouch_Send(y0,x0,tp_dev.tp_sta,id_send,n);
				//printf("11111x:%d  y:%d \r\n",x0[0],y0[0]);
			}
				
			}
			
			for(i=0;i<num;i++)
			{
				pre_id[i]=tp_dev.tp_id[i];
				x0[i]=tp_dev.x[i]*4095/1920;
				x_last[i]=x0[i];
				y0[i]=(tp_dev.y[i]*4096/1080);
				y_last[i]=y0[i];
				touch_sta[i]=0x07;
				//printf("22222x:%d  y:%d \r\n",tp_dev.x[i],tp_dev.y[i]);
			}	
			num_last=num;
			if(bDeviceState==CONFIGURED)
			{
        printf("num_last=num");
				MulTouch_Send(y0,x0,tp_dev.tp_sta,tp_dev.tp_id,num);
				
			}
		}else if(num_last!=0)    //触摸屏没被按下，释放上次触摸
		{
			
			m++;
			if(m>10)
			{
				for(i=0;i<num_last;i++)
			{
				touch_sta[i]=0x06;
				//printf(" 3333333  x:%d  y:%d \r\n",x_last[i],y_last[i]);
			}
			if(bDeviceState==CONFIGURED)
			{
        printf("bDeviceState==CONFIGURED\r\n");
				MulTouch_Send(y_last,x_last,touch_sta,pre_id,num_last);
				
			}	
			num_last=0;
			m=0;
			}
			
		}
#endif
}									    			    
}

