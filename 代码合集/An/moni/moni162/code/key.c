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
uint8_t mode_switch;
uint8_t F1_get_flag=1;
uint8_t F2_get_flag=1;

void key_scan()
{	
	B1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{
		show_switch++;
		show_switch %= 2;
	}
	
	if(!B2_state&&B2_last_state)
	{
		mode_switch++;
		mode_switch %= 2;
	}
	
	if(!B3_state&&B3_last_state)
	{
		if(show_switch==0)
		{
			F1_get_flag ++;
			F1_get_flag %= 2;
		}
		else if(show_switch==1)
		{
			PF += 1000;
			if(PF>10000)
				PF = 1000;			
		}
	}
	
	if(!B4_state&&B4_last_state)
	{
		if(show_switch==0)
		{
			F2_get_flag ++;
			F2_get_flag %= 2;
		}
		else if(show_switch==1)
		{
			PF -= 1000;
			if(PF<1000)
				PF = 10000;			
		}	}
	
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}