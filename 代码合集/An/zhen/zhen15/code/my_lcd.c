#include "headfile.h"

char text[20];

uint32_t PD = 1000;
uint32_t PH = 5000;
int PX = 0;

uint8_t NDA,NDB,NHA,NHB;

int fa,fb;
double ta,tb;

void data_show()
{
	ta = 1.0/fa;
	tb = 1.0/fb;
	sprintf(text,"        DATA        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	if(FT_switch == 0)
	{
		if(fa>1000)
		{	
			double fa_temp = fa/1000.0;
			sprintf(text,"     A=%.2fKHz      ",fa_temp);
		}
		else if(fa>0&&fa<=1000)	
		{
			sprintf(text,"     A=%dHz       ",fa);
		}
		else
		{
			sprintf(text,"     A=NULL      ");
		}
		LCD_DisplayStringLine(Line3,(uint8_t *)text);		
		if(fb>1000)
		{	
			double fb_temp = fb/1000.0;
			sprintf(text,"     B=%.2fKHz      ",fb_temp);
		}
		else if(fb>0&&fb<=1000)	
		{
			sprintf(text,"     B=%dHz       ",fb);
		}
		else
		{
			sprintf(text,"     B=NULL      ");
		}	
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
	}
	else
	{
		if(ta>0.001)
		{
			double ta_ms = ta*1000;
			sprintf(text,"     A=%.2fmS      ",ta_ms);			
		}
		else if(ta>0&&ta<=0.001)
		{
			uint16_t ta_us = ta*1000000;
			sprintf(text,"     A=%duS      ",ta_us);			
		}
		else
		{
			sprintf(text,"     A=NULL      ");
		}
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		
		if(tb>0.001)
		{
			double tb_ms = tb*1000;
			sprintf(text,"     B=%.2fmS      ",tb_ms);			
		}
		else if(tb>0&&tb<=0.001)
		{
			uint16_t tb_us = tb*1000000;
			sprintf(text,"     B=%duS      ",tb_us);			
		}
		else
		{
			sprintf(text,"     B=NULL      ");
		}		
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
	}
	sprintf(text,"                    ");
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
	sprintf(text,"                    ");
	LCD_DisplayStringLine(Line6,(uint8_t *)text);
}
void para_show()
{
	sprintf(text,"        PARA        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"     PD=%dHz      ",PD);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"     PH=%dHz      ",PH);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	sprintf(text,"     PX=%dHz      ",PX);
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
}
void recd_show()
{
	sprintf(text,"        RECD        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"     NDA=%d      ",NDA);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"     NDB=%d      ",NDB);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	sprintf(text,"     NHA=%d      ",NHA);
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
	sprintf(text,"     NHB=%d      ",NHB);
	LCD_DisplayStringLine(Line6,(uint8_t *)text);
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
	(show_switch == 0)?led_show(1,1):led_show(1,0);
	(fa>PH)?led_show(2,1):led_show(2,0);
	(fb>PH)?led_show(3,1):led_show(3,0);
	(NDA>=3||NDB>=3)?led_show(8,1):led_show(8,0);
}

