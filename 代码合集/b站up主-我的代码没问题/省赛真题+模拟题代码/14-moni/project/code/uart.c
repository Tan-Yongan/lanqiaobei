#include "headfile.h"

uint8_t dat;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		if(dat-48>0&&dat-48<10)
		{
			TIM3->CCR2 = 10*(dat-48);
		}
		else
		{
			HAL_UART_Transmit(huart,(uint8_t *)"error",5,50);
		}
		HAL_UART_Receive_IT(huart,&dat,1);
	}

}


