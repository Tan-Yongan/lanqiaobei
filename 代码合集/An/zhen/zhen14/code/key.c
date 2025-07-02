#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

uint8_t show_switch;

uint8_t fre_flag;

uint8_t H_L = 1;
uint8_t R_K;

uint8_t pwm_Lock;

uint8_t B2_Lock;

uint32_t B4Tick;

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(!B1_state && B1_last_state)
	{
		show_switch++;
		show_switch %= 3;
		if(show_switch == 1)
			R_K = 0;			
	}
	
	if(!B2_state && B2_last_state)
	{
		if(show_switch == 0)
		{
			if(B2_Lock == 0)
			{
				B2_Lock = 1;
				if(fre_flag == 0)
					fre_flag = 1;
				else if(fre_flag == 2)
					fre_flag = 3	;		
			}
		}
		
		if(show_switch == 1)
		{
			R_K++;
			R_K %= 2;
		}
	}
	
	if(!B3_state && B3_last_state)
	{
		if(show_switch == 1)  //参数界面
		{
			if(R_K == 0)  //改R
			{
				R++;
				if(R>=10)
					R = 1;
			}
			else   //改K
			{
				K++;
				if(K>=10)
					K = 1;
			}			
		}
	}
	
	if(!B4_state && B4_last_state)
	{
		if(show_switch == 1)  //参数界面
		{
			if(R_K == 0)  //改R
			{
				R--;
				if(R<=1)
					R = 10;
			}
			else   //改K
			{
				K--;
				if(K<=1)
					K = 10;
			}			
		}
		else if(show_switch == 0)  //数据界面
		{
			B4Tick = uwTick;
		}

	}
	else if(B4_state && !B4_last_state)
	{
		if(show_switch == 0)
		{
			if(uwTick - B4Tick > 2000)  //长按2s松开 uwTick1ms自增一次
			{
				pwm_Lock = 1;
			}
			else
			{
				pwm_Lock = 0;
			}
		}
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}

