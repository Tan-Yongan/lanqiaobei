#include "headfile.h"

extern uint8_t led1_hold;
uint8_t counter1,counter2;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		if(led1_hold)
		{
			TIM2->PSC = 400-1;
			TIM2->CCR2 = 10;
			counter1++;
			if(counter1>=50)     //密码输入成功内的五秒要做的事
			{
				led1_hold = 0;
				counter1 = 0;
				
				TIM2->PSC = 800-1;
				TIM2->CCR2 = 50;
			
				psd_sat = 0;
				input_conter = 0;
				
				pwd1_init = 1;
				pwd2_init = 1;
				pwd3_init = 1;
				
				pwd1_input = -1;
				pwd2_input = -1;
				pwd3_input = -1;
				
				
			}
		}
		if(led2_hold)  //连错三次
		{
			counter2++;
			if(counter2>=50)
			{
				led2_hold = 0;
				counter2 = 0;
				
				pwd1_init = 1;
				pwd2_init = 1;
				pwd3_init = 1;
				
				pwd1_input = -1;
				pwd2_input = -1;
				pwd3_input = -1;
							
			}
		}
	}
}
