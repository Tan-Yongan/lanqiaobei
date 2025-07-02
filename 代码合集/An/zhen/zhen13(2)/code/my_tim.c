#include "headfile.h"

uint8_t led1_state;
uint8_t led2_state;
uint8_t timer_5S;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		if(X == 0 && Y == 0)
			led2_state = !led2_state;
		else
			led2_state = 0;
		
		if(buy_flag)
		{
			timer_5S++;
			led1_state = 1;
			if(timer_5S>50)
			{
				led1_state = 0;
				TIM2->CCR2 = 5;
				timer_5S = 0;
				buy_flag = 0;
			}
		}
	}
}

