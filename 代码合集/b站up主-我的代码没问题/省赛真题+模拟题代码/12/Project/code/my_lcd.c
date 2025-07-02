#include "headfile.h"

int cnbr_num,vnbr_num;
char text[20];
char text0[20];
void lcd_show()
{
	if(!state_switch)
	{
		sprintf(text,"       Data        ");
		LCD_DisplayStringLine(Line1,(uint8_t *)text);
		sprintf(text,"   CNBR:%d    ",cnbr_num);
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		sprintf(text,"   VNBR:%d    ",vnbr_num);
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		sprintf(text,"   IDLE:%d    ",8-cnbr_num-vnbr_num);
		LCD_DisplayStringLine(Line7,(uint8_t *)text);
	}
	else
	{
		sprintf(text,"       Para        ");
		LCD_DisplayStringLine(Line1,(uint8_t *)text);
		sprintf(text,"   CNBR:%.2f    ",cnbr_rate);
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		sprintf(text,"   VNBR:%.2f    ",vnbr_rate);
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		sprintf(text,"              ");
		LCD_DisplayStringLine(Line7,(uint8_t *)text);		
	}
	
	if((8-cnbr_num-vnbr_num)>0)
		led_show(1,1);
	else
		led_show(1,0);
//	sprintf(text0,"%f",fee);
//	LCD_DisplayStringLine(Line9,(uint8_t *)text0);	
}
