#include "headfile.h"

double get_vol()
{
	HAL_ADC_Start(&hadc2);
	uint32_t ad_value = HAL_ADC_GetValue(&hadc2);
	return 3.3*ad_value/4096;
}
