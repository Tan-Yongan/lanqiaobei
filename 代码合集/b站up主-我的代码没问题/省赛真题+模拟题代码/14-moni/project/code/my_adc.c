#include "headfile.h"

float get_adc_value()
{
	HAL_ADC_Start(&hadc2);
	uint32_t temp = HAL_ADC_GetValue(&hadc2);
	return temp/4032.0*3.3;
}
