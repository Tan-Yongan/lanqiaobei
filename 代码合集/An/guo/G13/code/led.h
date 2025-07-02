#ifndef _led_h
#define _led_h
#include "headfile.h"

void led_show(uint8_t led,uint8_t state);
void get_many_vol(double *data);
extern double data[2];
double get_vol(ADC_HandleTypeDef *hadc);
extern uint16_t R39_F,PA6_F;

extern uint8_t LED1_state;

#endif