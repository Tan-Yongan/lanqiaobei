#include "headfile.h"

uint32_t fre1,fre2;
uint32_t capture_value1,capture_value2;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)  
	{
		capture_value1 = TIM2->CCR1;
		TIM2->CNT = 0;
		fre1 = 80000000/(80*capture_value1);
	}
	
	if(htim->Instance == TIM16) 
	{
		capture_value2 = TIM16->CCR1;
		TIM16->CNT = 0;
		fre2 = 80000000/(80*capture_value2);
	}
}

uint16_t timerA_3s,timerB_3s;
int fa_max,fa_min;
int fb_max,fb_min;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //0.1s
{
	if(htim->Instance == TIM3)
	{
		fa = fre1 + PX;
		fb = fre2 + PX;	
	}
	
	if(fa>0)
	{
		if(timerA_3s == 0)
		{
			fa_max = fa;
			fa_min = fa;
		}
		else
		{
			if(fa > fa_max) fa_max = fa;
			if(fa < fa_min) fa_min = fa;
		}
		timerA_3s ++;
	}
	
	if(timerA_3s>= 30)
	{
		timerA_3s = 0;
		if(fa_max - fa_min > PD) NDA++;
		fa_max = fa;
		fa_min = fa;
	}
	
	if(fb>0)
	{
		if(timerB_3s == 0)
		{
			fb_max = fb;
			fb_min = fb;
		}
		else
		{
			if(fb > fb_max) fb_max = fb;
			if(fb < fb_min) fb_min = fb;
		}
		timerB_3s++;
	}
	if(timerB_3s>=30)
	{
		timerB_3s = 0;
		if(fb_max - fb_min > PD) NDB++;
		fb_max = fb;
		fb_min = fb;
	}
}
	

