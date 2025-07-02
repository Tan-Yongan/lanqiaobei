#include "headfile.h"

uint32_t capture_value1;
uint32_t capture_value2;
uint32_t fre;
float duty_temp;
uint8_t duty;
uint8_t ccr = 10;
PeakState peak_state = WAITING;
double MH;
double ML;
double candidate_V;

double V;

double Last_V;
double DUTY;
uint32_t V_Tick;

uint32_t waiting_Tick;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		capture_value1 = TIM3->CCR1+1;
		capture_value2 = TIM3->CCR2+1;
		fre = 80000000/(80*capture_value2);
		DUTY=((double)capture_value2/capture_value1) * 100;
	}
}

void change()
{
	if(pwm_Lock == 0)
	{
		if(get_vol()<=1)
			ccr = 10;
		else if(get_vol()>=3)
			ccr = 85;
		else
			ccr = 37.5*get_vol()-27.5;
	}	
	TIM2->CCR2 = ccr;
		
	if(show_switch != 1)  //不在参数界面才传
	{
		Rv = R;
		Kv = K;
	}		
	V = (fre*2*3.14*Rv)/(100*Kv);
	
	if(Last_V != V)   //不相等就不计时，相等才计时
		V_Tick = uwTick;
	if(uwTick - V_Tick > 2000)  //相等的持续时间超过2S了
	{
		if(V>MH&&!H_L) MH = V;
		if(V>ML&&H_L) ML = V;
	}
	Last_V = V;
	
	(show_switch == 0)?led_show(1,1):led_show(1,0);
	(fre_flag == 1 || fre_flag == 3)?led_show(2,state):led_show(2,0);
	(pwm_Lock == 1)?led_show(3,1):led_show(3,0);
}
