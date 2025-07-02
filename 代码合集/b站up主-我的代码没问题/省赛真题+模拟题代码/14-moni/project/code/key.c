#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;

uint8_t B1_last_state;
uint8_t B2_last_state;
uint8_t B3_last_state;
uint8_t B4_last_state;

uint8_t switch_state;
double voltage=0.3;
int fre=1000;

void key_scan()
{
	B1_last_state=B1_state;
	B2_last_state=B2_state;
	B3_last_state=B3_state;
	B4_last_state=B4_state;
	B1_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);	
	
	if(B1_state==0&&B1_last_state==1) //B1
	{
		HAL_Delay(20);
		switch_state++;
		switch_state=switch_state%2;
	}
	
	if(switch_state)
	{
		if(B2_state==0&&B2_last_state==1) //B2
		{
			HAL_Delay(20);
			voltage+=0.3;
			if(voltage>3.3)
				voltage=0;
		}
	}
	else
	{
		if(B3_state==0&&B3_last_state==1) //B3
		{
			HAL_Delay(20);
			fre+=1000;
			if(fre>10000)
			{
				fre=1000;
			//	LCD_ClearLine(Line6);
			}
			TIM3->PSC=80000000/(fre*100)-1;
			
		}		
	}
	
//	if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)) //B4
//	{
//		HAL_Delay(20);
//		while(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0));
//		HAL_Delay(20);
//		
//	}

}
