#include "headfile.h"

uint32_t PB4_fre,PA15_fre;
uint32_t capture_value1,capture_value2;
uint32_t F1,F2;

uint8_t buff;
uint8_t rx_buff[5];
uint8_t count;
uint8_t rx_flag;

uint8_t TF1_flag=1;
uint8_t TF2_flag=1;

uint8_t led3_state;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM16)   //PB4
	{
		capture_value1 = TIM16->CCR1;
		TIM16->CNT = 0;
		PB4_fre = 80000000/(80*capture_value1);
	}
	if(htim->Instance == TIM2)   //PA15
	{
		capture_value2 = TIM2->CCR1;
		TIM2->CNT = 0;
		PA15_fre = 80000000/(80*capture_value2);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)   //PB4
	{
		led3_state = !led3_state;
	}
}
void change()
{
	if(!mode_switch)
	{
		if(F1_get_flag) F1 = PB4_fre;
		if(F2_get_flag) F2 = PA15_fre;
			
	}
	else
	{
		if(TF1_flag) F1 = PB4_fre;			
		if(TF2_flag) F2 = PA15_fre;	
	}
	(show_switch==0)?led_show(1,1):led_show(1,0);
	(show_switch==1)?led_show(2,1):led_show(2,0);
	(F1<PF&&F2<PF)?led_show(3,led3_state):led_show(3,0);
	(mode_switch==1)?led_show(8,1):led_show(8,0);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		rx_flag = 1;
		TIM4->CNT = 0;
		rx_buff[count] = buff;
		UART_Start_Receive_IT(&huart1,&buff,1);
		count++;
	}
}

void clearbuff()
{
	rx_flag = 0;
	count = 0;
	memset(rx_buff,'\0',sizeof(rx_buff));
}
uint8_t CheakCmd()
{
	if(mode_switch!=1)
		return 0;
	if(count != 2 && count != 3)
		return 0;
	if(count == 2)
	{
		if(rx_buff[0]!='F'||(rx_buff[1]!='1'&&rx_buff[1]!='2'))
			return 0;
	}
	if(count == 3)
	{
		if((rx_buff[0]!='T'&&rx_buff[0]!='P')||rx_buff[1]!='F'||(rx_buff[2]!='1'&&rx_buff[2]!='2'))
			return 0;
	}
	return 1;
}
void usart_process()
{
	if(rx_flag&&TIM4->CNT>15)
	{
		if(CheakCmd())
		{
			if(count == 3)
			{
				if(rx_buff[0]=='T'&&rx_buff[2]=='1') TF1_flag = 1;
				if(rx_buff[0]=='P'&&rx_buff[2]=='1') TF1_flag = 0;
				if(rx_buff[0]=='T'&&rx_buff[2]=='2') TF2_flag = 1;
				if(rx_buff[0]=='P'&&rx_buff[2]=='2') TF2_flag = 0;
			}
			if(count == 2)
			{
				if(rx_buff[1]=='1') printf("F1:%dHZ\r\n",F1);
				if(rx_buff[1]=='2') printf("F2:%dHZ\r\n",F2);
			}
		}
		else
		{
			printf("NULL\r\n");
		}
//		if(rx_buff[0]=='A'&&rx_buff[1]=='n')
//			printf("An\r\n");
//		else if(rx_buff[0]=='J'&&rx_buff[1]=='i'&&rx_buff[2]=='a')
//			printf("Jia\r\n");
//		else if(rx_buff[0]=='A'&&rx_buff[1]=='n'&&rx_buff[2]=='J'&&rx_buff[3]=='i'&&rx_buff[3]=='a')
//			printf("AnJia\r\n");		
		clearbuff();
	}
}

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
  /* Your implementation of fputc(). */
  return ch;
}

