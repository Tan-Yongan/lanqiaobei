#include "headfile.h"

int8_t count = -10;

char text[20];
void lcd_show()
{
	sprintf(text,"count=%d   ",abs(count));
	LCD_DisplayStringLine(Line0,(uint8_t *)text);
	
	sprintf(text,"R37_V=%.2f   ",get_vol(&hadc2));
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"R38_V=%.2f   ",get_vol(&hadc1));
	LCD_DisplayStringLine(Line2,(uint8_t *)text);
//	sprintf(text,"R37_V=%.2f   ",get_vol_R37());
//	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	
	sprintf(text,"PB4_fre=%d   ",PB4_fre);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	sprintf(text,"PA15_fre=%d   ",PA15_fre);
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
	sprintf(text,"PA2_fre=%d duty=%d ",PA2_fre,TIM15->CCR1);
	LCD_DisplayStringLine(Line6,(uint8_t *)text);
	
	sprintf(text,"eeprom=%d   ",eeprom_read(0));
	LCD_DisplayStringLine(Line7,(uint8_t *)text);
}