#include "headfile.h"

//LED��ʾ����
void led_show(uint8_t led,uint8_t state)
{
	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	if(state)
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8<<(led-1), GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8<<(led-1), GPIO_PIN_SET);
	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

//ADC����
float R37_V;
void vol_get(float *R37_V)
{
	HAL_ADC_Start(&hadc2);
	*R37_V = 3.3*HAL_ADC_GetValue(&hadc2)/4095;
}

// ���벶��
uint8_t cap_state = 0;
uint16_t rise1 = 0, fall = 0, rise2 = 0;

uint16_t PA1_Fre = 0;
uint8_t PA1_Duty = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		switch(cap_state)
		{
			case 0: // ��һ�������أ�������㣩
				rise1 = TIM2->CCR2;
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
				cap_state = 1;
				break;

			case 1: // �½��أ��ߵ�ƽ������
				fall = TIM2->CCR2;
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
				cap_state = 2;
				break;

			case 2: // �ڶ��������أ������յ㣩
				rise2 = TIM2->CCR2;

				uint16_t period = (uint16_t)(rise2 - rise1);  // ����ʱ��
				uint16_t high = (uint16_t)(fall - rise1);     // �ߵ�ƽʱ��

				PA1_Fre = 1000000 / period;               // ���趨ʱ��Ƶ��Ϊ1MHz
				PA1_Duty = (uint8_t)((high * 100) / period);

				cap_state = 0;
			
				break;
		}
	}
}
