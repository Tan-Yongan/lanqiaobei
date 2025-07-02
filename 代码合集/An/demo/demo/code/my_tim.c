#include "headfile.h"

uint8_t LED1_mode;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	LED1_mode = !LED1_mode;
}
