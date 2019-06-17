/**********************************************************
* @ File name -> soft_spi.c
* @ Version   -> V1.0
* @ Date      -> 11-15-2013
* @ Brief     -> GPIO模拟SPI接口函数

 V1.
* @ Revise    -> 
**********************************************************/

#include "soft_spi.h"

/**********************************************************
* 函数功能 ---> 模拟SPI初始化GPIO
* 入口参数 ---> none
* 返回参数 ---> none
* 功能说明 ---> none 
**********************************************************/
void Soft_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟   
    	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;  //CLK and MOSI
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    
 	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化指定IO，CLK and MOSI
 	
 	GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);    //设置低电平

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;   //MISO
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //上拉输入

    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化指定IO，MISO
}
/**********************************************************
* 函数功能 ---> 模拟SPI发送数据函数
* 入口参数 ---> send_data: 要发送的数据
* 返回参数 ---> 接收到的数据
* 功能说明 ---> 优先发送MSB，需要先发送LSB的请修改此函数
**********************************************************/
u8 Soft_SPI_ReadWrite_Byte(u8 send_data)
{
    u8 i;

//    Soft_SPI_MISO_IN(); //设置MISO为输入模式

    for(i = 0;i < 8;i++)
    {
        if(send_data & 0x80)    Soft_SPI_MOSI = 1;  //放上数据，优先发送LSB请修改为0x01
        else    Soft_SPI_MOSI = 0;

        send_data <<= 1;    //优先发送LSB请改为右移

        Soft_SPI_CLK = 0;
        Soft_SPI_CLK = 1;   //上升沿发送数据
//        Soft_SPI_CLK = 0;   //下降沿从设备输出数据请加上此句

        if(Soft_SPI_MISO)   send_data |= 0x01;  //返回来的是"1"，返回LSB时请修改为0x80
    }
    Soft_SPI_CLK = 0;
    return send_data;   //返回接收到的数据
}

