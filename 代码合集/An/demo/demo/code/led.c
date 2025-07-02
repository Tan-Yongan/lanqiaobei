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
