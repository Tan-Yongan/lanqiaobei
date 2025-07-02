#include "headfile.h"

uint8_t rx_data;
uint8_t rx_buff[7];
uint8_t i=0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //
{
	if(huart->Instance == USART1)
	{
		HAL_UART_Receive_IT(&huart1,&rx_data,1);
		rx_buff[i]=rx_data;
		i++;
		if(i>6)	
		{
			i=0;
			if(rx_buff[0]-48==pw[0]&&rx_buff[1]-48==pw[1]&&rx_buff[2]-48==pw[2])
			{
				pw[0]=rx_buff[4]-48;
				pw[1]=rx_buff[5]-48;
				pw[2]=rx_buff[6]-48;
			}			
		}

	}
}
