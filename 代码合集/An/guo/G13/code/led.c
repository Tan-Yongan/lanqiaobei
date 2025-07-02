#include "headfile.h"

void led_show(uint8_t led,uint8_t state)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	if(state)
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8<<(led-1), GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8<<(led-1), GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

double get_vol(ADC_HandleTypeDef *hadc)
{
	HAL_ADC_Start(hadc);
	uint32_t adc_value = HAL_ADC_GetValue(hadc);
	return 3.3*adc_value/4096;
}

double data[2];
void get_many_vol(double *data)
{
	for(int i=0;i<2;i++)
	{
		HAL_ADC_Start(&hadc2);
		uint32_t adc_value = HAL_ADC_GetValue(&hadc2);
		data[i] = 3.3*adc_value/4096;
	}
	PA5 = data[0];
	PA4 = data[1];
}


uint8_t LED1_state;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)   //0.1s进入一次
	{
		LED1_state = !LED1_state;
	}
}

uint8_t state1,state2;
uint16_t tim1[2],tim2[2];
uint16_t R39_F,PA6_F;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if(!state1)
			{
				tim1[0] = TIM3->CCR1;
				state1 = 1;
			}
			else
			{
				tim1[1] = TIM3->CCR1;
				uint16_t diff1 = (tim1[1] > tim1[0])?(tim1[1] - tim1[0]):(65535 - (tim1[0] - tim1[1]));
				R39_F = 1000000/diff1;
				state1 = 0;
			}
			
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if(!state2)
			{
				tim2[0] = TIM3->CCR2;
				state2 = 1;
			}
			else
			{
				tim2[1] = TIM3->CCR2;
				uint16_t diff2 = (tim2[1] > tim2[0])?(tim2[1] - tim2[0]):(65535 - (tim2[0] - tim2[1]));
				PA6_F = 1000000/diff2;
				state2 = 0;
			}
			
		}
	}
	
}
