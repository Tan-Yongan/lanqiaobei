#ifndef _uart_h
#define _uart_h
#include "headfile.h"

void data_analysis(void);
int to_num(uint8_t x);

extern int time[6];
extern float fee;
extern uint8_t type[4];
extern uint8_t buff;
extern uint8_t rx_buff[22];
extern uint8_t i;


#endif


