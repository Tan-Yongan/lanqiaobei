#include "headfile.h"

float get_vol(ADC_HandleTypeDef *hadc)
{
	HAL_ADC_Start(hadc);
	uint16_t adc_value = HAL_ADC_GetValue(hadc);
	return 3.3*adc_value/4096;
}

float get_vol_R37()
{
	HAL_ADC_Start(&hadc2);
	uint16_t adc_value = HAL_ADC_GetValue(&hadc2);
	return 3.3*adc_value/4096;
}