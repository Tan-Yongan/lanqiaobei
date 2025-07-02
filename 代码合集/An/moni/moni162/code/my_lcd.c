#include "headfile.h"

char text[20];
uint32_t PF = 1000;
void data_show()
{
	sprintf(text,"        DATA        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"     F1=%dHZ  ",F1);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"     F2=%dHZ  ",F2);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	if(mode_switch == 0)
		sprintf(text,"     MODE=KEY  ");
	else
		sprintf(text,"     MODE=USART  ");
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
}
void para_show()
{
	sprintf(text,"                    ");
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	sprintf(text,"                    ");
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
	sprintf(text,"        PARA        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"      PF=%dHZ        ",PF);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	
}
void lcd_show()
{
	switch(show_switch)
	{
		case 0:
			data_show();
			break;
		case 1:
			para_show();
			break;
		default:
			break;
	}
}
