#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{
		show_switch++;
		show_switch=show_switch%2;
	}
	
	if(mode_switch)     //手动模式
	{
		if(show_switch)
		{
			if(!B2_state&&B2_last_state)
			{
				pwm_PA6+=10;
				if(pwm_PA6>90)
					pwm_PA6=10;
				TIM3->CCR1 = pwm_PA6*10;
			}
			
			if(!B3_state&&B3_last_state)
			{
				pwm_PA7+=10;
				if(pwm_PA7>90)
					pwm_PA7=10;
				TIM17->CCR1 = pwm_PA7*10;
			}
		}
	}
	else    //自动模式
	{
		pwm_PA6 = ad_get()/3.3*100;
		pwm_PA7 = ad_get()/3.3*100;
		TIM3->CCR1 = pwm_PA6*10;
		TIM17->CCR1 = pwm_PA7*10;
	}
	
	if(!B4_state&&B4_last_state)
	{
		mode_switch++;
		mode_switch=mode_switch%2;
	}
	

	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}
