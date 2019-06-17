#include "ft3517U.h"
#include "touch.h"
#include "ctiic.h"
#include "usart.h"
#include "delay.h" 
#include "string.h" 
#include "hw_config.h"

u8 FT3517U_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	CT_IIC_Start();	 
	CT_IIC_Send_Byte(FT_CMD_WR);	//发送写命令 	 
	CT_IIC_Wait_Ack(); 	 										  		   
	CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//发数据
		ret=CT_IIC_Wait_Ack();
		if(ret)break;  
	}
    CT_IIC_Stop();					//产生一个停止条件	    
	return ret; 
}

//从GT1151读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度	

u8 FT3517U_RD_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i,ret; 
 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(FT_CMD_WR);   //发送写命令 	 
	ret = CT_IIC_Wait_Ack();
 	CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	ret = CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(FT_CMD_RD);   //发送读命令		   
	ret = CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //发数据	  
	} 
    CT_IIC_Stop();//产生一个停止条件 
  return ret;
} 

u8 FT3517U_Init(void)
{
	u8 temp[5];
	//u8 myret = 0;
	//u8 temp1[239]; 
  printf("FT3517U_Init\r\n");
	while(1){
		GPIO_InitTypeDef  GPIO_InitStructure;	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PB端口时钟

		GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1;// PB0和PB1端口配置
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_SetBits(GPIOB,GPIO_Pin_0);						//输出1
		GPIO_SetBits(GPIOB,GPIO_Pin_1);					//输出1

		CT_IIC_Init();      	//初始化电容屏的I2C总线  
		FT_RST=0;				//复位
		delay_ms(10);
		FT_RST=1;				//释放复位 
		delay_ms(10);
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;			//PC1端口配置
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;		//下拉输入
		GPIO_Init(GPIOB, &GPIO_InitStructure);			//PC1下拉输入
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);				//下拉	
		delay_ms(100);
		temp[0] = 0;
	  //读取版本号，参考值：0x3003
	  FT5206_WR_Reg(FT_DEVIDE_MODE,temp,1);	//进入正常操作模式 
	  FT5206_WR_Reg(FT_ID_G_MODE,temp,1);		//查询模式 
	  temp[0]=22;								//触摸有效值，22，越小越灵敏	
	  FT5206_WR_Reg(FT_ID_G_THGROUP,temp,1);	//设置触摸有效值
	  temp[0]=12;								//激活周期，不能小于12，最大14
	  FT5206_WR_Reg(FT_ID_G_PERIODACTIVE,temp,1); 
	  //读取版本号，参考值：0x3003
	  FT5206_RD_Reg(FT_ID_G_LIB_VERSION,&temp[0],2);
		if((((u16)temp[0]<<8)+temp[1]) == 0x01){//版本为0x01
      temp[0] = 0;
      temp[2] = 0;
			return 0;
		}
		//printf("CTP ID:%x\r\n",((u16)temp[0]<<8)+temp[1]);		
		/*
	  if(temp[0]==0X30&&temp[1]==0X03)//版本:0X3003
	  { 
		  printf("CTP ID:%x\r\n",((u16)temp[0]<<8)+temp[1]);
		  return 0;
	  } */
	}
	return 1;
}
const u16 FT3517U_TPX_TBL[10]={FT_TP1_REG,FT_TP2_REG,FT_TP3_REG,FT_TP4_REG,FT_TP5_REG,FT_TP6_REG,FT_TP7_REG,FT_TP8_REG,FT_TP9_REG,FT_TP10_REG};
#if 1
u8 TouchBuf[61]={0};
u8 lastPoint = 0;
u8 FT3517U_Scan(void){
  u8 ret = 0;
  u8 TouchPoint = 0;
  u8 tempPoint = 0;
  u8 i = 0;
  u8 j = 0;
  u8 temp = 0;
  u8 temp_id = 0;
  u8 flag = 0;
  u8 temp_sta = 0x06;
  ret = FT3517U_RD_Reg(FT_REG_NUM_FINGER,TouchBuf,61);
  if(ret == 1){
    for(i = 0; i < 61 ;i++){
      TouchBuf[i] = 0x00;
    }
    FT3517U_Init();
    return 0;
  }
  if(TouchBuf[0] == 0xFF){
    return 0;
  }else{
    TouchPoint = TouchBuf[0]&0x0F;
    if(TouchPoint < 0x0B){
      for(i =0; i < TouchPoint; i++){/*把当前有几个触点先存入*/
        tp_dev.tp_sta[i] = ((TouchBuf[1+6*i]&0xC0)>>6);
        if(tp_dev.tp_sta[i] > 0x02){
          return 0; 
        }
        tp_dev.tp_id[i] = (TouchBuf[3+6*i]&0xF0)>>4;
        if(tp_dev.tp_id[i] > 0x0A){
          return 0;
        }
        tp_dev.x[i] = (1080-(u16)(((u16)(TouchBuf[1+6*i]&0x0F)<<8)|TouchBuf[2+6*i])) * 4095/1080;//0~1080
        tp_dev.y[i] = (1920-(u16)(((u16)(TouchBuf[3+6*i]&0x0F)<<8)|TouchBuf[4+6*i]))* 4095/1920;//0~1920
        if(tp_dev.x[i] > 4095 || tp_dev.x[i] < 0){
          tp_dev.x[i] = 0x00;
          //return 0;
        }
        if(tp_dev.y[i] > 4095 || tp_dev.y[i] < 0){
          tp_dev.y[i] = 0x00;
          //return 0;
        }
        //printf("tp_dev.tp_sta[%d]=0x%X\r\n",i,tp_dev.tp_sta[i]);
        //printf("tp_dev.tp_id[%d]=0x%X\r\n",i,tp_dev.tp_id[i]);
        if(tp_dev.tp_sta[i] == 0x00){//put down
          tp_dev.tp_sta[i] = 0x07;
        }else if(tp_dev.tp_sta[i] == 0x01){//put up
          tp_dev.tp_sta[i] = 0x06;
        }else if(tp_dev.tp_sta[i] == 0x02){//Contact
          tp_dev.tp_sta[i] = 0x07;
        }else{//err
          tp_dev.tp_sta[i] = 0x07;
        }
      }
      if(lastPoint > TouchPoint){/*与上一次触点比较发现有触点被释放*/
        if(TouchPoint == 0){
          for(i = 0; i < lastPoint; i++){
            tp_dev.tp_sta[i] = 0x06;
            tp_dev.tp_id[i] = tp_dev.last_id[i];
            tp_dev.x[i] = tp_dev.last_x[i];
            tp_dev.y[i] = tp_dev.last_y[i];
          }
          MulTouch_Send(tp_dev.x,tp_dev.y,tp_dev.tp_sta,tp_dev.tp_id,lastPoint);
          lastPoint = TouchPoint;
          return 0;
        }
        tempPoint = TouchPoint;
        //printf("lastPoint=%d\r\n",lastPoint);
        //printf("TouchPoint=%d\r\n",TouchPoint);
        for(i = 0; i < lastPoint; i++){/*寻找出释放的触点。将释放的触点重新写进上报触点数组后*/
          for(j = 0; j < TouchPoint; j++){
             if(tp_dev.last_id[i] == tp_dev.tp_id[j]){
                flag = 1;
                break;
             }
          }
          if(flag == 0){
            tp_dev.tp_sta[tempPoint] = 0x06;
            if(tp_dev.last_id[i] > 0x0A){
              //printf("free err\r\n");
              return 0;
            }
            tp_dev.tp_id[tempPoint] = tp_dev.last_id[i];
            tp_dev.x[tempPoint] = tp_dev.last_x[i];
            tp_dev.y[tempPoint] = tp_dev.last_y[i];
            tempPoint++;
            //printf("free tp_id[%d] = %d\r\n",tempPoint,tp_dev.tp_id[tempPoint]);
          } 
          flag = 0;
        }
        MulTouch_Send(&tp_dev.x[TouchPoint],&tp_dev.y[TouchPoint],&tp_dev.tp_sta[TouchPoint],&tp_dev.tp_id[TouchPoint],tempPoint-TouchPoint);
        for(i =0; i < TouchPoint; i++){
          tp_dev.last_sta[i] =  tp_dev.tp_sta[i];
          tp_dev.last_id[i] =  tp_dev.tp_id[i];
          tp_dev.last_x[i] =  tp_dev.x[i];
          tp_dev.last_y[i] =  tp_dev.y[i];
        }
        //MulTouch_Send(&tp_dev.x[TouchPoint],&tp_dev.y[TouchPoint],&tp_dev.tp_sta[TouchPoint],&tp_dev.tp_id[tempPoint],(tempPoint - TouchPoint));
        lastPoint = TouchPoint;
        return TouchPoint;
      }else{/*与上一次比较发现触点没有变少，更新上次数据*/
        for(i =0; i < TouchPoint; i++){
          if(tp_dev.tp_id[i]> 0x0A){
            //printf("errrrr\r\n");
            return 0;
          }
          tp_dev.last_sta[i] =  tp_dev.tp_sta[i];
          tp_dev.last_id[i] =  tp_dev.tp_id[i];
          tp_dev.last_x[i] =  tp_dev.x[i];
          tp_dev.last_y[i] =  tp_dev.y[i];
        }
        lastPoint = TouchPoint;
        return TouchPoint;
      }
    }else{
      for(i = 0; i < 61 ;i++){
        TouchBuf[i] = 0x00;
      }
      return 0;
      
    }
    return 0;
  }
  return 0;
}
#endif
#if 0
u8 FT3517U_Scan(void)
{
	u8 buf[4]={0};
	u8 i=0;
	u8 temp = 0;
	u8 num = 0;
	u16 x = 0,y = 0;
  u8 ret = 0;
	static u8 t=0;//控制查询间隔,从而降低CPU占用率   
	t++;
	if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{
		ret = FT3517U_RD_Reg(FT_REG_NUM_FINGER,&num,1);//读取触摸点的状态 
    if(ret){
      printf("init\r\n");
      //FT3517U_Init();
      //printf("init OK\r\n");
    }else{
      if(num == 255 || num > 0x0B){
        t = 0;
        return 0;
      }
      if((num&0XF)&&((num&0XF)<0x0B))
      {
        temp=0XFF<<(num&0XF);
        tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
        printf("num=%d\r\n",num);
        for(i =0; i< num; i++)
        {
          if(tp_dev.sta&(1<<i))
          {
            ret = FT3517U_RD_Reg(FT3517U_TPX_TBL[i],buf,4);
            if(ret){
              printf("s err\r\n");
              num = 0;
            }else{
              printf("buf[0]=0x%X",buf[0]);
              if((buf[0]&0xC0) == 0x00){
                tp_dev.tp_sta[i] = 0x06;
              }else{
                tp_dev.tp_sta[i] = 0x07;
              }
              tp_dev.tp_id[i] = ((buf[2]&0xF0)>>4);
              y = (1080-(((u16)(buf[0]&0X0F)<<8)+buf[1]));
              x = (1920-(((u16)(buf[2]&0X0F)<<8)+buf[3]));
              if((x == 0) && (y == 0)){
              }else if((x > 1920) || (y > 1080)){
              }else if(x < 0 || y < 0){
                printf("x< 0 || y<0\r\n");
              }else{
                tp_dev.y[i] = y;
                tp_dev.x[i] = x;
              }
            }
          }
          /*else{
            printf("tp_dev.sta&(1<<i)=0x%X\r\n",tp_dev.sta&(1<<i));
            printf("i = 0x%X\r\n",i);
          }*/
        }
      }else{
        num = 0;
      }
    }

		if(tp_dev.x[0]==0 && tp_dev.y[0]==0)num=0;
		t=0;
		
	}
	if((num&0X1F)==0)//无触摸点按下
	{
		if(tp_dev.sta&TP_PRES_DOWN)
		{
			tp_dev.sta&=~(1<<7);
		}else{
			//tp_dev.x[0]=0xffff;
			//tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//清除点有效标记	
		}
			
	}
  if(t>240)t=10;//重新从10开始计数
	return num;
	
}
#endif
