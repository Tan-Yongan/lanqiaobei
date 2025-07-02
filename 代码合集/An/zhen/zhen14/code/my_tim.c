#include "headfile.h"

uint8_t state;
uint8_t N;

uint32_t timer_4S;
uint32_t timer_5S;

uint32_t fre_temp = 4000;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		if(B2_Lock == 1)
		{
			timer_5S++;
			if(timer_5S>50)
			{
				timer_5S = 0;
				B2_Lock = 0;
			}				
		}
		
		if(fre_flag == 1)
		{
			timer_4S++;
			fre_temp+=80;
			TIM2->PSC = 80000000/(100*fre_temp)-1;
			if(timer_4S>=50)  //到达4s
			{
//				candidate_V = 0;
//				peak_state = WAITING;

				timer_4S = 0;
				fre_flag = 2;
				H_L = 0;
				N++;
			}
		}
		else if(fre_flag == 3)
		{
			timer_4S++;
			fre_temp-=80;
			TIM2->PSC = 80000000/(100*fre_temp)-1;
			if(timer_4S>=50)  //到达4s
			{
//				candidate_V = 0;
//				peak_state = WAITING;
				
				timer_4S = 0;
				fre_flag = 0;
				H_L = 1;
				N++;
			}			
		}
		state = !state;
	}
}
