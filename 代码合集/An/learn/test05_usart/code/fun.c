#include "headfile.h"

char send_buff[20];
uint8_t rec_data,count;
uint8_t rec_flag;
uint8_t rec_buff[20];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		TIM4->CNT = 0;
		rec_flag = 1;
		rec_buff[count] = rec_data;
		count++;
//		HAL_UART_Transmit(huart,&rec_data,1,50);
		HAL_UART_Receive_IT(huart,&rec_data,1);
	}
}

void uart_data_rec()
{
	if(rec_flag)
	{
		if(TIM4->CNT > 15)  //数据接收完成
		{
			//处理数据
			if(rec_buff[0] == 'L' && rec_buff[1] == 'a' && rec_buff[2] == 'n') //Lan
			{
				sprintf(send_buff,"Lan\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t *)send_buff,sizeof(send_buff),50);
			}
			else if(rec_buff[0] == 'Q' && rec_buff[1] == 'i' && rec_buff[2] == 'a' && rec_buff[3] == 'o')  //Qiao
			{
				sprintf(send_buff,"Qiao\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t *)send_buff,sizeof(send_buff),50);
			}
			else if(rec_buff[0] == 'B' && rec_buff[1] == 'e' && rec_buff[2] == 'i')  //Bei
			{
				sprintf(send_buff,"Bei\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t *)send_buff,sizeof(send_buff),50);
			}
			else
			{
				sprintf(send_buff,"error\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t *)send_buff,sizeof(send_buff),50);
			}

			rec_flag = 0;
			for(int i = 0;i<count;i++)
				rec_buff[i] = '\0';
			count = 0;
		}
	}
}

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
