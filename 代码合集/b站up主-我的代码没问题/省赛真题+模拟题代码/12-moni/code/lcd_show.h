#ifndef _lcd_show_h
#define _lcd_show_h
#include "headfile.h"

void lcd_show(void);

extern int count;
extern double vmax,vmin,vmax_last,vmin_last;
extern uint8_t count_flag;
extern uint8_t buff[7];
#endif

