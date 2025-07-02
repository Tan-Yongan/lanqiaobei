#include "headfile.h"

uint8_t B1_state;
uint8_t B1_last_state = 1;

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);

//按键B1
	if(B1_state == 0 && B1_last_state == 1)  // 按键B1按下
	{
		
	}
	
	B1_last_state = B1_state;

}

char text[20];
void lcd_show()
{
	sprintf(text, "      test          ");
	LCD_DisplayStringLine(Line0, (uint8_t *)text);

}


