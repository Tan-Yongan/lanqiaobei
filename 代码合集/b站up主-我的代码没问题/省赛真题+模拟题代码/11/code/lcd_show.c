#include "headfile.h"

char text[20];
uint8_t show_switch,mode_switch;
uint8_t pwm_PA7=10,pwm_PA6=10;
void lcd_show()
{
	if(!show_switch)   //数据显示界面
	{
		sprintf(text,"      Data       ");
		LCD_DisplayStringLine(Line0,(uint8_t *)text);
		sprintf(text,"    V:%.2fV       ",ad_get());
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		if(!mode_switch)
		{
			sprintf(text,"    MODE:AUTO       ");
			LCD_DisplayStringLine(Line5,(uint8_t *)text);
		}
		else
		{
			sprintf(text,"    MODE:MANU       ");
			LCD_DisplayStringLine(Line5,(uint8_t *)text);
		}
	}
	else
	{
		sprintf(text,"      Para       ");
		LCD_DisplayStringLine(Line0,(uint8_t *)text);
		sprintf(text,"    PA6:%d%%       ",pwm_PA6);
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		sprintf(text,"    PA7:%d%%       ",pwm_PA7);
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
	}
	
	if(!show_switch)
		led_show(2,1);
	else
		led_show(2,0);
	if(!mode_switch)
		led_show(1,1);
	else
		led_show(1,0);
	
}
