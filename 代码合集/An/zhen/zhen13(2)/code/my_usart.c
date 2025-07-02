#include "headfile.h"
#include <stdio.h>

uint8_t buff;
//uint8_t rx_buff[5];
//uint8_t counter;
//uint8_t rx_flag;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		if(buff == '?')
			printf("X:%.1f,Y:%.1f\r\n",X_price,Y_price);
		HAL_UART_Receive_IT(&huart1,&buff,1);	
//		TIM3->CNT = 0;
//		rx_flag = 1;
//		rx_buff[counter] = buff;
//		HAL_UART_Receive_IT(&huart1,&buff,1);	
//		counter++;		
	}
}

//void clear_buff()
//{
//	rx_flag = 0;
//	counter = 0;
//	memset(rx_buff,'\0',sizeof(rx_buff));
//}

//uint8_t CheckCmd(uint8_t* str)
//{
//	if(counter!=1)
//		return 0;
//	if(str[0] != '?')
//		return 0;
//	return 1;
//}

//void usart_process()
//{
//	if(rx_flag&&TIM3->CNT>15)
//	{
//		if(CheckCmd(rx_buff))
//		{
//			printf("X:%.1f,Y:%.1f\r\n",X_price,Y_price);
//			printf("counter=%d   CheckCmd=%d\r\n",counter,CheckCmd(rx_buff));
//			printf("Received Data: %s\r\n", rx_buff);  // 打印 rx_buff 内容		
//		}
//		else
//		{
//			printf("Form Error!\r\n");		
//			printf("counter=%d   CheckCmd=%d\r\n",counter,CheckCmd(rx_buff));
//			printf("Received Data: %s\r\n", rx_buff);  // 打印 rx_buff 内容		
//		}
//	}
//	clear_buff();	
//}

struct __FILE
{
  int handle;
  /* Whatever you require here. If the only file you are using is */
  /* standard output using printf() for debugging, no file handling */
  /* is required. */
};
/* FILE is typedef'd in stdio.h. */
FILE __stdout;
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
  /* Your implementation of fputc(). */
  return ch;
}

