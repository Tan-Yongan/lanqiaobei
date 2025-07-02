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
uint8_t P_switch;
uint8_t FT_switch;
uint32_t clearTick;
uint32_t keyTick;
void key_scan()
{
	if(uwTick-keyTick<20) return;
	keyTick = uwTick;
	
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{
		if(show_switch==1)
		{
			if(P_switch==0)
			{
				PD += 100;
				if(PD>1000)
					PD = 1000;
			}
			if(P_switch==1)
			{
				PH += 100;
				if(PH>10000)
					PH = 10000;
			}
			if(P_switch==2)
			{
				PX += 100;
				if(PX>1000)
					PX = 1000;
			}
		}
		
	}
	if(!B2_state&&B2_last_state)
	{
		if(show_switch==1)
		{
			if(P_switch==0)
			{
				PD -= 100;
				if(PD<100)
					PD = 100;
			}
			if(P_switch==1)
			{
				PH -= 100;
				if(PH<1000)
					PH = 1000;
			}
			if(P_switch==2)
			{
				PX -= 100;
				if(PX<-1000)
					PX = -1000;
			}
		}
	}
	if(!B3_state&&B3_last_state)
	{
		if(show_switch==0)
		{
			FT_switch++;
			FT_switch %= 2;
		}
		if(show_switch==1)
		{
			P_switch++;
			P_switch %= 3;
		}
		if(show_switch==2)
		{
			clearTick = uwTick;
		}
	}
	else if(B3_state&&!B3_last_state)
	{
		if(show_switch==2)
		{
			if(uwTick-clearTick>1000)
			{
				NDA = 0;
				NDB = 0;
				NHA = 0;
				NHB = 0;
			}
		}			
	}
	if(!B4_state&&B4_last_state)
	{
		show_switch++;
		show_switch %= 3;
		if(show_switch!=1)
			P_switch = 0;
		if(show_switch!=0)
			FT_switch = 0;
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}
