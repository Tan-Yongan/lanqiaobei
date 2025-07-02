#include "headfile.h"

char text[20];

extern uint8_t pwd1_init;
extern uint8_t pwd2_init;
extern uint8_t pwd3_init;
extern uint8_t pwd4_init;

extern uint8_t pwd1_input;
extern uint8_t pwd2_input;
extern uint8_t pwd3_input;
extern uint8_t pwd4_input;

extern uint8_t psd_sat;

extern uint8_t led1_hold;

void lcd_show()
{
	if(!psd_sat)   //PSD½çÃæ
	{
		sprintf(text,"       PSD         ");
		LCD_DisplayStringLine(Line1,(uint8_t*)text);
		if(pwd1_init)
			sprintf(text,"     B1:@          ");
		else
			sprintf(text,"     B1:%d          ",pwd1_input);
			LCD_DisplayStringLine(Line3,(uint8_t*)text);
		if(pwd2_init)
			sprintf(text,"     B2:@          ");
		else
			sprintf(text,"     B2:%d          ",pwd2_input);
			LCD_DisplayStringLine(Line4,(uint8_t*)text);
		if(pwd3_init)
			sprintf(text,"     B3:@          ");
		else
			sprintf(text,"     B3:%d          ",pwd3_input);
			LCD_DisplayStringLine(Line5,(uint8_t*)text);			
	}
	else
	{
		sprintf(text,"       SAT         ");
		LCD_DisplayStringLine(Line1,(uint8_t*)text);	
		sprintf(text,"       F:%d        ",80000000/((TIM2->PSC+1)*(TIM2->ARR+1)));
		LCD_DisplayStringLine(Line3,(uint8_t*)text);
		sprintf(text,"       D:%d%%       ",TIM2->CCR2);
		LCD_DisplayStringLine(Line4,(uint8_t*)text);
		sprintf(text,"                    ");
		LCD_DisplayStringLine(Line5,(uint8_t*)text);			
	
	}
	led_show(1,led1_hold);
	led_show(2,counter2%2);
	
}
