#include "touch.h" 
#include "delay.h"
#include "stdlib.h"
#include "math.h"
    


_m_tp_dev tp_dev=
{
	TP_Init,
	FT3517U_Scan,
	0,0,0,0,0,
	0,0,0,0,0,
  0,0,0,0,0,	
	0,	
  0
};

//触摸屏初始化  		    
//返回值:0,没有进行校准
//       1,进行过校准
u8 TP_Init(void)
{		
	
	//GT1151_Init();	
	FT3517U_Init();	
	tp_dev.scan=FT3517U_Scan;	//扫描函数指向GT1151触摸屏扫描

	return 1; 									 
}






