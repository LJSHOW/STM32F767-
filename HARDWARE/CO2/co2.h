#ifndef _CO2_H



#define _CO2_H

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/6/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//LED�˿ڶ���

#define         DC_GAIN                      8.5f  //define the DC gain of amplifier
#define         ZERO_POINT_VOLTAGE           0.324f//define the output of the sensor in volts when the concentration of CO2 is 400PPM
float  MGGetPercentage(float temp, float *pcurve);
#endif
