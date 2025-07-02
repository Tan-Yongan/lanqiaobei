#include "headfile.h"

uint32_t ad_value;

double get_volt()
{
	HAL_ADC_Start(&hadc2);
	
	ad_value = HAL_ADC_GetValue(&hadc2);
	
	return ad_value/4032.0*3.3;

}
