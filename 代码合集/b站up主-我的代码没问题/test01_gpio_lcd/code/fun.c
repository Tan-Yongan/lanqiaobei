#include "headfile.h"

// ��������ԭ��Ƶ����������� ����˫�� ����
// ˫�����ܴ�������ο� ���иĽ��ռ�

/* ˫������ʵ��˼·
	��ԭ��Ƶ��������������������� double_time��double_flag��tick
	1.����˫����Ҳ�������ΰ��£������ΰ��°���֮��ļ��ʱ��С��double_timeʱ����Ϊ˫������
	
	2.tick���ڼ�¼�������µ�һ˲��ļ���ֵ�� 
	  HAL_GetTick()��������᷵��һ������ֵ������ʱ���ļ��������ƣ�Ĭ����ÿ1ms������1
		�ڰ�������ʱ�ñ���tick��¼��HAL_GetTick()���صļ���ֵ
		��֮��Ϳ����� HAL_GetTick() - tick ���õ� ����ڰ�������ʱ ��������ʱ��
	  
	3.double_flag�����趨����˫��������ܵ�״̬��
	   double_flagΪ0ʱ����ʾ����û�а���
		 double_flagΪ1ʱ����ʾ������һ�ΰ���
		 double_flagΪ2ʱ����ʾ˫������
		 �����������������⣺��һ�����⣬������Ŀ���ǽ���һ�ζ̰�����ô��Ȼ��ʹdouble_flag��Ϊ1������Ҫ��������Ĵ���
		                                  	if(double_flag == 1)
																				{
																					if(HAL_GetTick() - tick > double_time)  // ˫��δ����
																					{
																						double_flag = 0;
																					}
																				}
											     �����������һ��֮�󣬾�����ʱ�䳬�����趨��˫���������Ϊ˫��û�з�������double_flag������Ϊ0
													 
													 �ڶ������⣬ÿ�ΰ����ɿ���˲�� ������Ϊ��һ�ζ̰�����ֹ��˫������֮���ظ�ִ�ж̰�����Ĵ��룬Ҫ�ڶ̰����ж��������д����
																				if(double_flag == 2) // ˫������ ��ִ�ж̰����� ������־λ��0
																				{
																					double_flag = 0;
																				}
																				else                //����B1�̰�
																				{
																					count += 2;
																				}
													
													���������⣬�ڵڶ��������У�ֻ�����˫����ڶ��ΰ������º���ظ�ִ�У���һ�ΰ���������Ȼ��ִ�ж̰���Ĵ��롣������û�н����������˼����
*/

#define double_time 500  // ˫���ļ��ʱ��

uint8_t lcd_highshow;

int count = 0;
uint8_t led_mode;

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

uint8_t double_flag = 0;  // ˫��״̬
uint32_t tick;            // ʱ�����

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	

	
//����B1
	if(B1_state == 0 && B1_last_state == 1)  // ����B1����
	{
		// ˫���ж�
		if(double_flag == 0)
		{
			tick = HAL_GetTick();
			double_flag = 1;
		}
		else if(double_flag == 1)
		{
			if(HAL_GetTick() - tick < double_time)  // ˫������
			{
				double_flag = 2;
				// �˴���д˫������
				count = 0;
			}
		}
		
		TIM3->CNT = 0;
	}
	
  if(B1_state == 0 && B1_last_state == 0)  // ����B1һֱ����
	{
		if(TIM3->CNT >= 10000)   //����B1����1s
		{
			count++;
		}
	}
	
	if(double_flag == 1)
	{
		if(HAL_GetTick() - tick > double_time)  // ˫��δ����
		{
			double_flag = 0;
		}
	}
	
  if(B1_state == 1 && B1_last_state == 0)  // ����B1�ɿ�
	{
		if(TIM3->CNT < 10000)   
		{
			if(double_flag == 2) // ˫������ ��ִ�ж̰����� ������־λ��0
			{
				double_flag = 0;
			}
			else                //����B1�̰�
			{
				count += 2;
			}
			
		}
	}
	

	

// ����B2
	if(B2_state == 0 && B2_last_state == 1)  // ����B2����
	{
		TIM3->CNT = 0;
	}
	else if(B2_state == 0 && B2_last_state == 0)  // ����B2һֱ����
	{
		if(TIM3->CNT >= 10000)   //����B2����1s
		{
			count--;
		}
	}
	else if(B2_state == 1 && B2_last_state == 0)  // ����B2�ɿ�
	{
		if(TIM3->CNT < 10000)    //����B2�̰�
		{
			count -= 2;
		}
	}
	
//����B3
	if(B3_state == 0 && B3_last_state == 1)  // ����B3����
	{
		led_show(2, 1);
	}
	
//����B4
	if(B4_state == 0 && B4_last_state == 1)  // ����B4����
	{
		lcd_highshow ++;
		lcd_highshow %= 3;
		led_show(2, 0);
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}

char text[20];
void lcd_show()
{
	sprintf(text, "      test          ");
	LCD_DisplayStringLine(Line0, (uint8_t *)text);
	if(lcd_highshow == 0)
	{
		LCD_SetBackColor(Yellow);
		sprintf(text, "      count: %d     ", count);
		LCD_DisplayStringLine(Line3, (uint8_t *)text);
		LCD_SetBackColor(Black);		
		sprintf(text, "      para1         ");
		LCD_DisplayStringLine(Line4, (uint8_t *)text);
		sprintf(text, "      para2         ");
		LCD_DisplayStringLine(Line5, (uint8_t *)text);		
	}
	else if(lcd_highshow == 1)
	{

		sprintf(text, "      count: %d     ", count);
		LCD_DisplayStringLine(Line3, (uint8_t *)text);	
		LCD_SetBackColor(Yellow);
		sprintf(text, "      para1         ");
		LCD_DisplayStringLine(Line4, (uint8_t *)text);
		LCD_SetBackColor(Black);	
		sprintf(text, "      para2         ");
		LCD_DisplayStringLine(Line5, (uint8_t *)text);			
	}
	else if(lcd_highshow == 2)
	{

		sprintf(text, "      count: %d     ", count);
		LCD_DisplayStringLine(Line3, (uint8_t *)text);	
		sprintf(text, "      para1         ");
		LCD_DisplayStringLine(Line4, (uint8_t *)text);
		LCD_SetBackColor(Yellow);
		sprintf(text, "      para2         ");
		LCD_DisplayStringLine(Line5, (uint8_t *)text);	
		LCD_SetBackColor(Black);	
	}

	
	led_show(1, led_mode);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)  //TIM2�жϺ���
	{
		led_mode++;
		led_mode = led_mode % 2;
	}
}

