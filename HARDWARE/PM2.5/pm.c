#include "pm.h"









int pm_valueget(float temp)
{
  int pm;
  pm = (0.17*temp-0.1)*1000;
  
	return pm;

}