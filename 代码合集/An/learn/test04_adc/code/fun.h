#ifndef _fun_h
#define _fun_h
#include "stm32g4xx.h"                  // Device header

void lcd_show(void);

double get_vol(ADC_HandleTypeDef *hadc);

#endif
