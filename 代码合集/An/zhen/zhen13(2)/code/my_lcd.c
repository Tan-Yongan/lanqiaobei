#include "headfile.h"

char text[20];


uint8_t X_buy;
uint8_t Y_buy;

uint8_t X;
uint8_t Y;		
double X_price;
double Y_price;

void shop_show()
{
	sprintf(text,"        SHOP        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"        X:%d        ",X_buy);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"        Y:%d        ",Y_buy);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
}

void price_show()
{
	sprintf(text,"        PRICE       ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"        X:%.1f        ",X_price);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"        Y:%.1f        ",Y_price);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	
}

void rep_show()
{
	sprintf(text,"        REP        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"        X:%d        ",X);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"        Y:%d        ",Y);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
}

void lcd_show()
{
	switch(lcd_switch)
	{
		case 0:
			shop_show();
			break;
		case 1:
			price_show();
			break;
		case 2:
			rep_show();
			break;
		default:
			break;
	}
	sprintf(text,"     %d        ",TIM2->CCR2);
	LCD_DisplayStringLine(Line7,(uint8_t *)text);
	sprintf(text,"     %d        ",eeprom_read(4));
	LCD_DisplayStringLine(Line8,(uint8_t *)text);
	sprintf(text,"  %d,%d,%d,%d           ",eeprom_read(0),eeprom_read(1),eeprom_read(2),eeprom_read(3));
	LCD_DisplayStringLine(Line9,(uint8_t *)text);
	
	led_show(1,led1_state);
	led_show(2,led2_state);
}
