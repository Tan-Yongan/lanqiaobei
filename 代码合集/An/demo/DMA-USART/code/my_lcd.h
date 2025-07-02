#ifndef _my_lcd_h
#define _my_lcd_h
#include "headfile.h"

void lcd_show(void);
extern char text[30];

extern uint8_t led2_state;
extern uint8_t led3_state;
extern int x1,x2,y1,y2;

#endif
