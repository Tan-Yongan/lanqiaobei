#include "headfile.h"

uint16_t capture_value1,capture_value2,capture_value3;
uint16_t PB4_fre,PA15_fre,PA2_fre;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM16)  //PB4
	{
		capture_value1 = TIM16->CCR1;
		TIM16->CNT = 0;
		PB4_fre = 80000000/(80*capture_value1);
	}
	if(htim->Instance == TIM2)  //PB4
	{
		capture_value2 = TIM2->CCR1;
		TIM2->CNT = 0;
		PA15_fre = 80000000/(80*capture_value2);
	}
	if(htim->Instance == TIM17)  //PA2
	{
		capture_value3 = TIM17->CCR1+1;
		TIM17->CNT = 0;
		PA2_fre = 80000000/(80*capture_value3);
	}
}
