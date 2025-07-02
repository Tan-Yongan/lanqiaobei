#ifndef _fun_h
#define _fun_h
#include "headfile.h"

void led_show(uint8_t led,uint8_t state);
void lcd_show(void);
void key_scan(void);

extern uint8_t buff;
extern uint8_t rx_buff[50];
uint8_t CheakCmd(uint8_t* str);
void usart_process(void);

typedef struct{
	uint8_t in_type[5];
	uint8_t in_id[5];
	uint8_t in_year;
	uint8_t in_month;
	uint8_t in_day;
	uint8_t in_hour;
	uint8_t in_min;
	uint8_t in_sec;
	uint8_t in_pos;
}car;

uint8_t isIN(uint8_t* str);
#endif
