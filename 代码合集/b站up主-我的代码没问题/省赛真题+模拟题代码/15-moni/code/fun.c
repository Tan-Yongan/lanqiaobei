#include "headfile.h"

uint8_t led1_flag, led2_flag;
uint8_t switch_flag = 0;
float r37_rate, r38_rate;
float r37_vol, r38_vol;
uint8_t r37_count, r38_count;
uint8_t r37_pass_count, r38_pass_count;

float r37_up = 2.2, r37_down = 1.2;
float r38_up = 3.0, r38_down = 1.4;

uint8_t select_flag;

double get_vol(ADC_HandleTypeDef *hadc)
{
	HAL_ADC_Start(hadc);
	uint32_t adc_value = HAL_ADC_GetValue(hadc);
	return 3.3*adc_value/4096;
	
}


void led_show(uint8_t led, uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	if(mode)
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

uint8_t B1_state;
uint8_t B1_last_state = 1;
uint8_t B2_state;
uint8_t B2_last_state = 1;
uint8_t B3_state;
uint8_t B3_last_state = 1;
uint8_t B4_state;
uint8_t B4_last_state = 1;

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	
//按键B1
	if(B1_state == 0 && B1_last_state == 1)  // 按键B1按下
	{
		switch_flag++;
		switch_flag = switch_flag % 3;
	}

//按键B2
	if(B2_state == 0 && B2_last_state == 1)  // 按键B2按下
	{
		if(switch_flag == 0)
		{
			r37_count++;
			if(r37_vol <= r37_up && r37_vol >= r37_down)
			{
				led1_flag = 1;
				r37_pass_count++;
			}
			r37_rate = ((float)(r37_pass_count)) / ((float)(r37_count));
			r37_rate *= 100;
		}
		else if(switch_flag == 1)
		{
			select_flag++;
			select_flag = select_flag % 4;
		}
		
	}
	
//按键B3
	if(B3_state == 0 && B3_last_state == 1)  // 按键B3按下
	{
		if(switch_flag == 0)
		{
			r38_count++;
			if(r38_vol <= r38_up && r38_vol >= r38_down)
			{
				led2_flag = 1;
				r38_pass_count++;
			}
			r38_rate = ((float)(r38_pass_count)) / ((float)(r38_count));
		  r38_rate *= 100;
		}
		else if(switch_flag == 1)
		{
			if(select_flag == 0)
			{
				r37_count = 0; 
				r37_pass_count = 0;
				r37_rate = 0;
				r37_up += 0.2f;
				if(r37_up > 3.1f)
					r37_up = 2.2f;
			}
			else if(select_flag == 1)
			{
				r37_count = 0; 
				r37_pass_count = 0;
				r37_rate = 0;
				r37_down += 0.2f;
				if(r37_down > 2.1f)
					r37_down = 1.2f;
			}
			else if(select_flag == 2)
			{
				r38_count = 0; 
				r38_pass_count = 0;
				r38_rate = 0;
				r38_up += 0.2f;
				if(r38_up > 3.1f)
					r38_up = 2.2f;
			}
			else if(select_flag == 3)
			{
				r38_count = 0; 
				r38_pass_count = 0;
				r38_rate = 0;
				r38_down += 0.2f;
				if(r38_down > 2.1f)
					r38_down = 1.2f;
			}
		}			
	}
	
//按键B4
	if(B4_state == 0 && B4_last_state == 1)  // 按键B4按下
	{
		if(switch_flag == 2)
		{
			r37_count = 0;
			r37_pass_count = 0;
			r37_rate = 0;
			r38_count = 0;
			r38_pass_count = 0;
			r38_rate = 0;
		}
		else if(switch_flag == 1)
		{
			if(select_flag == 0)
			{
				r37_count = 0; 
				r37_pass_count = 0;
				r37_rate = 0;
				r37_up -= 0.2f;
				if(r37_up < 2.1f)
					r37_up = 3.0f;
			}
			else if(select_flag == 1)
			{
				r37_count = 0; 
				r37_pass_count = 0;
				r37_rate = 0;
				r37_down -= 0.2f;
				if(r37_down < 1.1f)
					r37_down = 2.0f;
			}
			else if(select_flag == 2)
			{
				r38_count = 0; 
				r38_pass_count = 0;
				r38_rate = 0;
				r38_up -= 0.2f;
				if(r38_up < 2.1f)
					r38_up = 3.0f;
			}
			else if(select_flag == 3)
			{
				r38_count = 0; 
				r38_pass_count = 0;
				r38_rate = 0;
				r38_down -= 0.2f;
				if(r38_down < 1.1f)
					r38_down = 2.0f;
			}
		}			
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}

char text[20];
void lcd_show()
{
	r37_vol = get_vol(&hadc2);
	r38_vol = get_vol(&hadc1);
	
	if(switch_flag == 0)
	{
		sprintf(text, "       GOODS        ");
		LCD_DisplayStringLine(Line1, (uint8_t *)text);	
		sprintf(text, "     R37:%.2fV      ", r37_vol);
		LCD_DisplayStringLine(Line3, (uint8_t *)text);	
		sprintf(text, "     R38:%.2fV      ", r38_vol);
		LCD_DisplayStringLine(Line4, (uint8_t *)text);			
	}
	else if(switch_flag == 1)
	{
		sprintf(text, "      STANDARD      ");
		LCD_DisplayStringLine(Line1, (uint8_t *)text);			
		sprintf(text, "    SR37:%.1f-%.1f  ", r37_down, r37_up);
		LCD_DisplayStringLine(Line3, (uint8_t *)text);		
		sprintf(text, "    SR38:%.1f-%.1f  ", r38_down, r38_up);
		LCD_DisplayStringLine(Line4, (uint8_t *)text);		
	}
	else if(switch_flag == 2)
	{
		sprintf(text, "        PASS         ");
		LCD_DisplayStringLine(Line1, (uint8_t *)text);
		sprintf(text, "    PR37:%.1f%%      ", r37_rate);
		LCD_DisplayStringLine(Line3, (uint8_t *)text);		
		sprintf(text, "    PR38:%.1f%%      ", r38_rate);
		LCD_DisplayStringLine(Line4, (uint8_t *)text);				
	}
	
	if(led1_flag == 1)
	{
		TIM2->CNT = 0;
		led1_flag = 2;
		led_show(1, 1);
	}
	else if(led1_flag == 2)
	{
		if(TIM2->CNT >= 10000)
		{
			led_show(1, 0);
			led1_flag = 0;
		}
	}	
	
	if(led2_flag == 1)
	{
		TIM2->CNT = 0;
		led2_flag = 2;
		led_show(2, 1);
	}
	else if(led2_flag == 2)
	{
		if(TIM2->CNT >= 10000)
		{
			led_show(2, 0);
			led2_flag = 0;
		}
	}	
	
	
	if(switch_flag == 0)
		led_show(3, 1);
	else 
		led_show(3, 0);
	
	if(switch_flag == 1)
		led_show(4, 1);
	else 
		led_show(4, 0);
	
	if(switch_flag == 2)
		led_show(5, 1);
	else 
		led_show(5, 0);
}

uint8_t dat, buff[3], count;
char send_buff[30];
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart -> Instance == USART1)
	{
		if(count == 0)
		{
			if(dat == 'R')
				count = 1;
			else
				count = 0;
		}
		else if(count == 1)
		{
			if(dat == '3')
				count = 2;
			else 
				count = 0;
		}
		else if(count == 2)
		{
			if(dat == '7')
			{
				sprintf(send_buff, "R37:%d,%d,%.1f%%\r\n", r37_count, r37_pass_count, r37_rate);
				HAL_UART_Transmit(huart, (uint8_t *)send_buff, sizeof(send_buff), 50);
				count = 0;
			}
			else if(dat == '8')
			{
				sprintf(send_buff, "R38:%d,%d,%.1f%%\r\n", r38_count, r38_pass_count, r38_rate);
				HAL_UART_Transmit(huart, (uint8_t *)send_buff, sizeof(send_buff), 50);
				count = 0;
			}
			else
				count = 0;
		}
			
		HAL_UART_Receive_IT(huart, &dat, 1);
	}
}


