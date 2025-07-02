#ifndef _fun_h
#define _fun_h
#include "headfile.h"

extern uint32_t PB4_fre,PA15_fre;
void change(void);
extern uint32_t F1,F2;

extern uint8_t buff;
extern uint8_t rx_buff[5];
extern uint8_t count;
extern uint8_t rx_flag;

void usart_process(void);

extern uint8_t TF1_flag;
extern uint8_t TF2_flag;

extern uint8_t led3_state;

#endif
