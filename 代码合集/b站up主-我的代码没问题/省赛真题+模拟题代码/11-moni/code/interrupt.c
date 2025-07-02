#include "headfile.h"

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	
	
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(show_switch==0)
		{
			ld1_count++;
			ld1_count=ld1_count%2;
		}
	}
}


