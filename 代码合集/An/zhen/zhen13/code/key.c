#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

uint8_t psd_sat;

uint8_t pwd1_init=1;
uint8_t pwd2_init=1;
uint8_t pwd3_init=1;

uint8_t pwd1_input=-1;
uint8_t pwd2_input=-1;
uint8_t pwd3_input=-1;

uint8_t pwd[3] = {1,2,3};

uint8_t led1_hold;
uint8_t led2_hold;
uint8_t input_conter;

int32_t keyTick;
void key_scan()
{
	if(uwTick-keyTick<20) 
		return;
	keyTick = uwTick;
	
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{
		pwd1_init = 0;
		pwd1_input++;
		if(pwd1_input>9)
			pwd1_input = 0;
	}
	if(!B2_state&&B2_last_state)
	{
		pwd2_init = 0;
		pwd2_input++;
		if(pwd2_input>9)
			pwd2_input = 0;
	}
	if(!B3_state&&B3_last_state)
	{
		pwd3_init = 0;
		pwd3_input++;
		if(pwd3_input>9)
			pwd3_input = 0;
	}
	if(!B4_state&&B4_last_state)
	{
		if(pwd1_input == pwd[0]&&pwd2_input == pwd[1]&&pwd3_input == pwd[2])
		{
			psd_sat = 1;
			led1_hold = 1;
		}	
		else   //输入错误
		{
			pwd1_init = 1;
			pwd2_init = 1;
			pwd3_init = 1;
			
			pwd1_input = -1;
			pwd2_input = -1;
			pwd3_input = -1;
			
			input_conter++;
			if(input_conter>=3)   //超过三次
			{
				led2_hold = 1;
				input_conter = 0;
			}
		}
	}
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}
