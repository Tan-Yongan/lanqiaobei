#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

uint8_t show_switch;

uint8_t press_start=1;
uint32_t press_time;

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{
		show_switch++;
		show_switch %= 4;
	}
	
	if(!B2_state&&B2_last_state)
	{
		
	}

	if(!B3_state&&B3_last_state)
	{
		
	}
	
	if(!B4_state&&B4_last_state)
	{
		led_show(8,0);
	}
	
	if(!B3_state&&!B3_last_state&&!B4_state&&!B4_last_state)   //B3,B4同时按下超过两秒
	{
		if(press_start)
		{
			press_time = uwTick;
			press_start = 0;			
		}
		if(uwTick - press_time > 2000)
		{
			led_show(8,1);
			press_start = 1;
		}
			
	}
	else
	{
		press_start = 1;
	}

	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}