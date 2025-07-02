#include "headfile.h"

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
  /* Your implementation of fputc(). */
  return ch;
}

uint8_t rx_flag;
uint8_t rx_size;
char rx_buff[100];

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART1)
	{
		rx_flag = 1;
		rx_size = Size;
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t *)rx_buff,100);
	}
}

void usart_process()
{
	if(rx_flag)
	{
		rx_flag = 0;
		
		printf("%s",rx_buff);
		
		memset(rx_buff,'\0',100);
	}
}
//uint8_t buff;
//uint8_t rx_buff[20];
//uint8_t rx_flag;
//uint8_t counter;

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(huart->Instance == USART1)
//	{
//		TIM4->CNT = 0;
//		rx_flag = 1;
//		rx_buff[counter] = buff;
//		HAL_UART_Receive_IT(&huart1,&buff,1);
//		counter++;
//	}
//}

//void clear_buff()
//{
//	rx_flag = 0;
//	counter = 0;
//	memset(rx_buff,'\0',sizeof(rx_buff));
//}

//void checkCmd()
//{
//	
//}

//void usart_process()
//{
//	if(rx_flag && TIM4->CNT>15)  //接收完一次数据流
//	{
//		if(rx_buff[0]=='A'&&rx_buff[1]=='n')
//			printf("An\r\n");
//		else if(rx_buff[0]=='J'&&rx_buff[1]=='i'&&rx_buff[2]=='a')
//			printf("Jia\r\n");
//		else if(rx_buff[0]=='?')
//			printf("?\r\n");
//		else
//			printf("NULL\r\n");
//		clear_buff();
//	}
//}

