#include "headfile.h"

char text[20];
uint8_t error_flag;
void lcd_show()
{
	if(!switch_state)
	{
		led_show(1,1);
		led_show(2,0);
		sprintf(text,"        DATA       ");
		LCD_DisplayStringLine(Line2,(uint8_t *)text);
		sprintf(text,"     VR37:%.2fV       ",get_adc_value());
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		sprintf(text,"     PA7:%dHZ       ",fre);
		LCD_DisplayStringLine(Line6,(uint8_t *)text);
	}
	else
	{
		led_show(1,0);
		led_show(2,1);
		sprintf(text,"        PARA       ");
		LCD_DisplayStringLine(Line2,(uint8_t *)text);
		sprintf(text,"      VP1:%.1fV       ",voltage);
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		sprintf(text,"                    ");
		LCD_DisplayStringLine(Line6,(uint8_t *)text);
//		sprintf(text,"                    ");
//		LCD_DisplayStringLine(Line2,(uint8_t *)text);
//		sprintf(text,"                    ");
//		LCD_DisplayStringLine(Line4,(uint8_t *)text);
	}
//	sprintf(text,"      fre:%d     ",fre0);
//	LCD_DisplayStringLine(Line8,(uint8_t *)text);
	
	if(get_adc_value()>voltage&&voltage<3.2)
		error_flag=1;
	else
		error_flag=0;
	if(error_flag)
		led_show(3,count);
	else
		led_show(3,0);

}
	
