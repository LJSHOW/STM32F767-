#include "sys.h"
#include "co2.h"
#include "math.h"







float  MGGetPercentage(float temp, float *pcurve)
{ 
	float CO2_value;
   if (temp >=ZERO_POINT_VOLTAGE) {
      return -1;
   } else { 
      CO2_value= pow(10, ((temp)-pcurve[1])/pcurve[2]+pcurve[0]);


		 return CO2_value;
   }
}