
#ifndef _CO_H
#define _CO_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//KEY��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////



#define CAL_PPM  10  // ?????PPM?
#define RL	10  // RL??


float MQ7_PPM_GET(float Vrl1);
float MQ7_PPM_Calibration(float RS);


#endif
