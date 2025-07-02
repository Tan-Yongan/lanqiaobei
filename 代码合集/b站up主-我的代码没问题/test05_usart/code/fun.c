#include "headfile.h"

char send_buff[20];
uint8_t rec_data, count;
uint8_t rec_flag;
uint8_t rec_buff[20];
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		//HAL_UART_Transmit(huart, &rec_data, 1, 50);
		TIM4->CNT = 0;
		rec_flag = 1;
		rec_buff[count] = rec_data;
		count++;
		HAL_UART_Receive_IT(huart, &rec_data, 1);
	}
}

void uart_data_rec()
{
	if(rec_flag)
	{
		if(TIM4->CNT > 15)  //数据接收完成
		{
			//处理数据
			if(rec_buff[0] == 'l' && rec_buff[1] == 'a' && rec_buff[2] == 'n')   //lan
			{
				sprintf(send_buff, "lan\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t *)send_buff, sizeof(send_buff), 50);
			}
			else if(rec_buff[0] == 'q' && rec_buff[1] == 'i' && rec_buff[2] == 'a' && rec_buff[3] == 'o')  //qiao
			{
				sprintf(send_buff, "qiao\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t *)send_buff, sizeof(send_buff), 50);
			}
			else if(rec_buff[0] == 'b' && rec_buff[1] == 'e' && rec_buff[2] == 'i')  //bei
			{
				sprintf(send_buff, "bei\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t *)send_buff, sizeof(send_buff), 50);
			}
			else
			{
				sprintf(send_buff, "error!\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t *)send_buff, sizeof(send_buff), 50);			
			}
			
			rec_flag = 0;
			for(int i=0; i<count; i++)
				rec_buff[i] = 0;
			count = 0;
		}
	}
}


