#include "co.h"
#include "math.h"
#include "adc.h"


int boot_time_ms;
float Vrl,RS,ppm,R0_value;


static float R0;
float MQ7_PPM_Calibration(float RS)
{
    R0 = RS / pow(CAL_PPM / 98.322, 1 / -1.458f);
	
	return R0;
}

float MQ7_PPM_GET(float temp)
{
	
	
	 boot_time_ms++;
   RS = (3.3f - temp) / temp* RL ;
 if(boot_time_ms < 20) // ????????,3s?????
    {
  	  R0_value=MQ7_PPM_Calibration(RS);
    }
    ppm = 98.322f * pow(RS/R0_value, -1.458f);

   return ppm;
}




