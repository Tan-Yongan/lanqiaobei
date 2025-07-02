#include "headfile.h"

char send_buff[100];
char rx_buff[100];
uint8_t rx_flag;
uint8_t rx_size;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART1)
	{
		rx_flag = 1;
		rx_size = Size;
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t *)rx_buff,100);
	}
}

void rx_handle()
{
	if(rx_flag)
	{
		rx_flag = 0;
		if(!strcmp(rx_buff,"X"))
		{
			sprintf(send_buff,"X:%d",X);
		    HAL_UART_Transmit_DMA(&huart1,(uint8_t *)send_buff,strlen(send_buff));
		}
		else if(!strcmp(rx_buff,"Y"))
		{
			sprintf(send_buff,"Y:%d",Y);
		    HAL_UART_Transmit_DMA(&huart1,(uint8_t *)send_buff,strlen(send_buff));
		}
		else if(!strcmp(rx_buff,"PA1"))
		{
			sprintf(send_buff,"PA1:%f",PA1);
		    HAL_UART_Transmit_DMA(&huart1,(uint8_t *)send_buff,strlen(send_buff));
		}	
		else if(!strcmp(rx_buff,"PA4"))
		{
			sprintf(send_buff,"PA4:%.2f",PA4);
		    HAL_UART_Transmit_DMA(&huart1,(uint8_t *)send_buff,strlen(send_buff));
		}
		else if(!strcmp(rx_buff,"PA5"))
		{
			sprintf(send_buff,"PA5:%.2f",PA5);
		    HAL_UART_Transmit_DMA(&huart1,(uint8_t *)send_buff,strlen(send_buff));
		}
		else if(!strcmp(rx_buff,"#"))
		{
			sprintf(send_buff,"#");
		    HAL_UART_Transmit_DMA(&huart1,(uint8_t *)send_buff,strlen(send_buff));
		}
		else
		{
			sprintf(send_buff,"NULL");
		    HAL_UART_Transmit_DMA(&huart1,(uint8_t *)send_buff,strlen(send_buff));
		}
		
		memset(rx_buff,0,100);
	}
}
