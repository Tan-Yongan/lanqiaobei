#include "headfile.h"

int count;
uint8_t count_flag;
uint8_t buff[7];
double vmax=3.0,vmin=1.0;
double vmax_last,vmin_last;
double volt;
double last_volt;
char text[20];

void lcd_show()
{
	volt=get_volt();

	if(volt>vmin&&last_volt<vmin&&last_volt!=0)
	{
		count=0;
		count_flag=1;
		led_show(1,1);
	}
	if(volt>vmax&&last_volt<vmax)
	{
		count_flag=0;
		led_show(1,0);
	}
	
	if(!show_switch)
	{
		sprintf(text,"      Data      ");
		LCD_DisplayStringLine(Line0,(uint8_t *)text);
		sprintf(text," V:%.2f      ",volt);
		LCD_DisplayStringLine(Line2,(uint8_t *)text);
		sprintf(text," T:%d      ",count);
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		

	}
	else
	{
		sprintf(text,"      Para      ");
		LCD_DisplayStringLine(Line0,(uint8_t *)text);
		sprintf(text," Vmax:%.1f      ",vmax);
		LCD_DisplayStringLine(Line2,(uint8_t *)text);
		sprintf(text," Vmin:%.1f      ",vmin);
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		
	}
	last_volt=volt;
	

	
	

	
}
