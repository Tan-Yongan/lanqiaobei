#include "headfile.h"

uint8_t lcd_highshow;
int count = 0;
uint8_t led_mode;

void led_show(uint8_t led,uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	
	if(mode)
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led - 1),GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led - 1),GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);

}

//����һֱ��ѭ����������û�а��£�B1_state�Ͷ���1,����B1_last_state.ֱ���������£�if�����㡣
uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state = 1;
uint8_t B2_last_state = 1;
uint8_t B3_last_state = 1;
uint8_t B4_last_state = 1;

uint32_t B2Tick;
uint32_t B2_gap;
uint8_t click_count_B2;
uint8_t click_flag_B2;

uint8_t B4_click_flag;
uint8_t B4count;
uint32_t B4_gap;
uint32_t B4Tick;
uint32_t B1Tick;
uint32_t keyTick;
uint32_t B3Tick;
void key_scan()
{
	if(uwTick-keyTick<20) return;
	keyTick = uwTick;
	
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(B1_state == 0 && B1_last_state == 1)   //����B1����
	{
		B1Tick = uwTick;
	}
	else if(B1_state == 0 && B1_last_state == 0)  //����B1һֱ����
	{
		if(uwTick - B1Tick >= 1000)  //����B1����1s
		{
			count++;
		}
	}
	else if(B1_state == 1 && B1_last_state == 0)  //����B1�ɿ�
	{
		if(uwTick - B1Tick < 1000)  //����B1�̰�
		{
			count += 2; 
		}
	}
	
	
	if(B2_state == 0 && B2_last_state == 1)   //����B2����
	{
		if(click_count_B2>0 && (uwTick - B2Tick)<=400)  
		{
			click_count_B2++;
			if(click_count_B2 == 3)
			{
				//��������
				count+=30;	
				click_count_B2 = 0;
				click_flag_B2 = 0;				
			}
		}
		else
		{
			B2Tick = uwTick;
			click_count_B2 = 1;
			click_flag_B2 = 1;
			B2_gap = 0;
		}
	}		
	
	if(B3_state == 0 && B3_last_state == 1)   //����B3����
	{
		B3Tick = uwTick;
	}
	else if(B3_state == 1 && B3_last_state == 0)
	{
		if(uwTick-B3Tick>1000)
			count = 0;
		else
			count+=5;
	}
	
	if(B4_state == 0 && B4_last_state == 1)   //����B4����
	{
		//˫��
		if(B4_click_flag && uwTick-B4Tick<=250)
		{
			//˫������
			count-=10;
			B4_click_flag = 0;
		}
		else
		{
			B4Tick = uwTick;
 			B4_click_flag = 1;
			B4_gap = 0;

		}
//		lcd_highshow++;
//		lcd_highshow %= 3;   //ʹ������012��ѭ��
//		led_show(2,0);
	}		
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
	
}
	
	
char text[20];  //�����Ļһ�������ʾ20���ַ�
void lcd_show()
{
	sprintf(text,"    an love jia    ");   //�Ѻ�����ַ�������text���� 
	LCD_DisplayStringLine(Line0,(uint8_t *)text);  //��Ϊ������յ�������u8 (uint8_t *)ǿ��ת��
	sprintf(text,"    count:%d    ",count);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	if(lcd_highshow == 0)
	{
		LCD_SetBackColor(Yellow);
		sprintf(text,"      I        ");
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		LCD_SetBackColor(Black);	
		sprintf(text,"    LOVE       ");
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		sprintf(text,"      U        ");
		LCD_DisplayStringLine(Line6,(uint8_t *)text);	
	}
	else if(lcd_highshow == 1)
	{
		sprintf(text,"      I        ");
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		LCD_SetBackColor(Yellow);
		sprintf(text,"    LOVE       ");
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		LCD_SetBackColor(Black);	
		sprintf(text,"      U        ");
		LCD_DisplayStringLine(Line6,(uint8_t *)text);		
	}
	else if(lcd_highshow == 2)
	{
		sprintf(text,"      I        ");
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		sprintf(text,"    LOVE       ");
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		LCD_SetBackColor(Yellow);
		sprintf(text,"      U        ");
		LCD_DisplayStringLine(Line6,(uint8_t *)text);		
		LCD_SetBackColor(Black);	
	}
	led_show(1,led_mode);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)  //TIM2�жϺ���
	{
		//count++;
		led_mode = !led_mode;
	}
	
	if(htim->Instance == TIM4)
	{
		if(click_flag_B2)
		{
			B2_gap++;
			//��������
			if(B2_gap>400)
			{
				if(click_count_B2 == 1)
				{
					//����
					count+=10;
				}
				else if(click_count_B2 == 2)
				{
					count+=20;
				}
				click_flag_B2 = 0;
				B2_gap = 0;
				click_count_B2 = 0;
			}
		}
		if(B4_click_flag)
		{
			B4_gap++;
			if(B4_gap>250)
			{
				count-=2;
				B4_click_flag = 0;
				B4_gap = 0;
			}
		}
	}
}
