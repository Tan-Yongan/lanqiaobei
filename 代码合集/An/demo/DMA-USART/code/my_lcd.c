#include "headfile.h"

char text[30];

uint8_t led2_state;
uint8_t led3_state;

int x1,x2,y1,y2;

void lcd_show()
{
	sprintf(text,"   TEXT   ");
	LCD_DisplayStringLine(Line0,(uint8_t *)text);
	
	sprintf(text,"(%d,%d);(%d,%d)",x1,y1,x2,y2);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	
	led_show(2,led2_state);
	led_show(3,led3_state);
	
}
