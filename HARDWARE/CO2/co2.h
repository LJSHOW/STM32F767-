#ifndef _CO2_H



#define _CO2_H

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/6/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//LED端口定义

#define         DC_GAIN                      8.5f  //define the DC gain of amplifier
#define         ZERO_POINT_VOLTAGE           0.324f//define the output of the sensor in volts when the concentration of CO2 is 400PPM
float  MGGetPercentage(float temp, float *pcurve);
#endif
