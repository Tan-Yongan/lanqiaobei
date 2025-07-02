#include "headfile.h"

char text[30];

void data_show()
{
	sprintf(text,"        DATA        ");
	LCD_DisplayStringLine(Line0,(uint8_t *)text);
	
	sprintf(text,"     F=%d     ",PA1_Fre);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"     D=%d     ",PA1_Duty);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	sprintf(text,"     A=%.1f     ",R37_V);
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
}

void para_show()
{
	sprintf(text,"        PARA        ");
	LCD_DisplayStringLine(Line0,(uint8_t *)text);
}
void recd_show()
{
	sprintf(text,"        RECD        ");
	LCD_DisplayStringLine(Line0,(uint8_t *)text);
	
}

void fset_show()
{
	sprintf(text,"        FSET        ");
	LCD_DisplayStringLine(Line0,(uint8_t *)text);
	
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
		case 2:
			recd_show();
			break;
		case 3:
			fset_show();
			break;
		default:
			break;
	}
	
}