#include "headfile.h"

double buff1[2];
int i;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(count_flag)
			count++;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
  {
		buff1[0]=(buff[0]-48)+0.1*(buff[2]-48);
		buff1[1]=(buff[4]-48)+0.1*(buff[6]-48);
		
		if(buff1[0]<=3.3&&buff1[0]>=0&&buff1[1]<=3.3&&buff1[1]>=0&&buff[1]=='.'&&buff[5]=='.'&&buff[3]==',')
		{
			if((((buff[0]-48)+0.1*(buff[2]-48))>=(((buff[4]-48)+0.1*(buff[6]-48))+1)))
			{
				vmax=buff1[0];
				vmin=buff1[1];
				err_flag1=0;
			}
			else
			{
				err_flag1=1;
			}
	  }
		else
		{
			err_flag1=1;
		}
	
		HAL_UART_Receive_IT(&huart1,buff,7);
	}

}

