#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "sys.h"
#include "ott2001a.h"	    
#include "gt1151.h"	      
#include "ft5206.h"	    
#include "ft3517U.h"	    



#define TP_PRES_DOWN 0x80  //触屏被按下	  
#define TP_CATH_PRES 0x40  //有按键按下了 
#define CT_MAX_TOUCH  10    //电容屏支持的点数,固定为5点

//触摸屏控制器
typedef struct
{
	u8 (*init)(void);			//初始化触摸屏控制器
	u8 (*scan)(void);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	u16 x[CT_MAX_TOUCH]; 		//当前坐标
	u16 y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								          //x[4],y[4]存储第一次按下时的坐标. 
	u8 tp_id[CT_MAX_TOUCH];
	u8 tp_sta[CT_MAX_TOUCH];
	u8  sta;					//笔的状态 
								//b7:按下1/松开0; 
	              //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
  u8 touchtype;
  u16 last_x[CT_MAX_TOUCH];
  u16 last_y[CT_MAX_TOUCH];
  u8  last_id[CT_MAX_TOUCH];
  u8  last_sta[CT_MAX_TOUCH];
}_m_tp_dev;

extern _m_tp_dev tp_dev;	 	//触屏控制器在touch.c里面定义

u8 TP_Init(void);								//初始化
 
#endif

















