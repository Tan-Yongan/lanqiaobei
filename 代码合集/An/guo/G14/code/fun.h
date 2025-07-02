#ifndef _fun_h
#define _fun_h
#include "headfile.h"
void led_show(uint8_t led,uint8_t state);

extern float R37_V;
void vol_get(float *R37_V);
extern uint8_t R37_flag;

extern uint16_t PA1_Fre;
extern uint8_t PA1_Duty;

#endif 