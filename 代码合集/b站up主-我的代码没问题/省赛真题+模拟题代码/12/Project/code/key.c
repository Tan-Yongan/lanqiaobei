#include "headfile.h"

uint8_t state_switch;
uint8_t pwm_switch;
double cnbr_rate=3.5;
double vnbr_rate=2.0;

void key_scan()
{
	if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0))   //B1
	{
		HAL_Delay(20);
		while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0));
		HAL_Delay(20);
		state_switch++;
		state_switch=state_switch%2;
	}
	if(state_switch)
	{
		if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1))   //B2
		{
			HAL_Delay(20);
			while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1));
			HAL_Delay(20);
			cnbr_rate+=0.5;
			vnbr_rate+=0.5;
		}
		
		if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2))   //B3
		{
			HAL_Delay(20);
			while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2));
			HAL_Delay(20);
			cnbr_rate-=0.5;
			vnbr_rate-=0.5;
		}
	}
	
	if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))   //B4
	{
		HAL_Delay(20);
		while(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0));
		HAL_Delay(20);
		pwm_switch++;
		pwm_switch=pwm_switch%2;
		if(pwm_switch)
		{
			TIM3->CCR2 = 20;
			led_show(2,1);
		}
		else
		{
			TIM3->CCR2 = 0;
			led_show(2,0);
		}
			
	}

}
