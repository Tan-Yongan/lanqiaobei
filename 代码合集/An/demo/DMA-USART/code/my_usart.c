#include "headfile.h"

char send_buf[100];
char rx_buf[100];
uint8_t rx_flag;
uint8_t rx_size;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART1)
	{
		rx_flag = 1;
		rx_size = Size;
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t *)rx_buf,100);
	}
}

void rx_handle()
{
	if(rx_flag)
	{
		rx_flag = 0;
		
		//�ش�������
		sprintf(send_buf,"%s",rx_buf);
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)send_buf,strlen(send_buf));
		
		//ͨ��LCD��ʾ
		sprintf(text,"   %s   ",rx_buf);
		LCD_DisplayStringLine(Line1,(uint8_t *)text);
		
		//���յ��
		if(rx_size == 1)
		{
			if(rx_buf[0] == '2')
				led2_state = !led2_state;
			if(rx_buf[0] == '3')
				led3_state = !led3_state;
		}
		
		//sscanf��������  ���͵ĸ�ʽ(11,22,33,44),��Ӧx1,x2,y1,y2
		sscanf(rx_buf,"(%d,%d,%d,%d)",&x1,&x2,&y1,&y2);

		memset(rx_buf,0,100);
	}
}
