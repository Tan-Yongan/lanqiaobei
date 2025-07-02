#ifndef _my_adc_h
#define _my_adc_h
#include "headfile.h"

float get_vol(ADC_HandleTypeDef *hadc);
float get_vol_R37(void);

#endif