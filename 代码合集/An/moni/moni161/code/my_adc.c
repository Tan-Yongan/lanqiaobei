#include "headfile.h"

uint32_t timer_3S;

double get_vol()
{
	HAL_ADC_Start(&hadc2);
	uint32_t adc_value = HAL_ADC_GetValue(&hadc2);
	return 3.3*adc_value/4096;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(start_flag)
		{
			timer_3S++;

			if(timer_3S>30)
			{
				show_switch = 0;
				pwd_real[0] = eeprom_read(0x00);
				pwd_real[1] = eeprom_read(0x01);
				pwd_real[2] = eeprom_read(0x02);
				pwd_in[0] = 3;
				pwd_in[1] = 3;
				pwd_in[2] = 3;
				timer_3S = 0;
				start_flag = 0;
				
			}
		}
	}
}
