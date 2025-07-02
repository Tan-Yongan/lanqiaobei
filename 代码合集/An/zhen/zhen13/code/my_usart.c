#include "headfile.h"

uint8_t buff;
uint8_t rx_buff[20];
uint8_t rx_counter;
uint8_t rx_flag;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		TIM4->CNT = 0;
		rx_buff[rx_counter] = buff;
		HAL_UART_Receive_IT(&huart1,&buff,1);
		rx_counter ++;
		rx_flag = 1;
	}
}

void clear_buff()
{
	rx_flag = 0;
	rx_counter = 0;
	memset(rx_buff,'\0',sizeof(rx_buff));
}

uint8_t CheakCmd(uint8_t *str)
{
	if(rx_counter!=7)
		return 0;
	for(uint8_t i=0;i<3;i++)
	{
		if(str[i]<'0'||str[i]>'9')
			return 0;
	}
	if(str[3]!='-')
		return 0;
	for(uint8_t i=4;i<7;i++)
	{
		if(str[i]<'0'||str[i]>'9')
			return 0;
	}
	return 1;
}

void usart_process()
{
	if(rx_flag && TIM4->CNT>15)   //接收到一条完整的数据
	{
		if(CheakCmd(rx_buff))  //数据格式正确
		{
			if(((rx_buff[0]-'0')==pwd[0])&&((rx_buff[1]-'0')==pwd[1])&&((rx_buff[2]-'0')==pwd[2]))  //原密码正确
			{
				pwd[0] = rx_buff[4]-'0';
				pwd[1] = rx_buff[5]-'0';
				pwd[2] = rx_buff[6]-'0';  //改密码	

				clear_buff();
				printf("修改密码成功!!!\r\n");

			}
			else
			{
				clear_buff();
				printf("原密码错误!!!\r\n");
			}			
		}
		else
		{
			clear_buff();
			printf("Form Error!!!\r\n");
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
	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,50);

  /* Your implementation of fputc(). */
  return ch;
}
