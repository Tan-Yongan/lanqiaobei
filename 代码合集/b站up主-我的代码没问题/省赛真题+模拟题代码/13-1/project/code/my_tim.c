#include "headfile.h"

uint8_t led1_work_flag=0;
uint8_t led2_work_flag=0;
uint8_t led1_counter;
uint8_t led2_counter;

uint8_t sta_to_psd_flag;

uint8_t x;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		if(led1_work_flag)
		{
			led1_on();
			led1_counter++;
			if(led1_counter>50)
			{
				led1_off();
				led1_work_flag=0;
				pw_flag=1;
				sta_to_psd_flag=1;
				lcd_show1_init();
				state_psd();
			}
			
		}
		
		if(led2_work_flag)
		{
			x++;
			x=x%2;
			led2_show(x);		
			led2_counter++;
			if(led2_counter>50)
			{
				led2_work_flag=0;
				led2_off();
			}

		}
	}
}

void state_psd(void)
{
//	TIM2->CCR2 = 50;
//	TIM2->PSC = 800-1;
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,50);
	__HAL_TIM_PRESCALER(&htim2,800-1);
}

void state_sta(void)
{
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,10);
	__HAL_TIM_PRESCALER(&htim2,400-1);
}
