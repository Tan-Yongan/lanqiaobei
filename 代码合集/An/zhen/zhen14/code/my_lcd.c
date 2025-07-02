#include "headfile.h"

char text[20];

uint8_t R = 1;
uint8_t K = 1;

uint8_t Rv;
uint8_t Kv;

void data_show()
{
	sprintf(text,"        DATA             ");
	LCD_DisplayStringLine(Line1,(uint8_t*)text);
	if(H_L == 0)
		sprintf(text,"     M=H            ");
	else
		sprintf(text,"     M=L            ");
	LCD_DisplayStringLine(Line3,(uint8_t*)text);
	sprintf(text,"     P=%2d%%            ",TIM2->CCR2);
	LCD_DisplayStringLine(Line4,(uint8_t*)text);
	sprintf(text,"     V=%.1f             ",V);
	LCD_DisplayStringLine(Line5,(uint8_t*)text);	
}

void para_show()
{
	sprintf(text,"        PARA             ");
	LCD_DisplayStringLine(Line1,(uint8_t*)text);
	sprintf(text,"     R=%d            ",R);
	LCD_DisplayStringLine(Line3,(uint8_t*)text);
	LCD_SetBackColor(Black);
	sprintf(text,"     K=%d            ",K);
	LCD_DisplayStringLine(Line4,(uint8_t*)text);
	sprintf(text,"                   ");
	LCD_DisplayStringLine(Line5,(uint8_t*)text);
}

void recd_show()
{
	sprintf(text,"        RECD             ");
	LCD_DisplayStringLine(Line1,(uint8_t*)text);
	sprintf(text,"     N=%d                ",N);
	LCD_DisplayStringLine(Line3,(uint8_t*)text);
	sprintf(text,"     MH=%.1f            ",MH);
	LCD_DisplayStringLine(Line4,(uint8_t*)text);
	sprintf(text,"     ML=%.1f           ",ML);
	LCD_DisplayStringLine(Line5,(uint8_t*)text);
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
		default:
			break;
	}
}
