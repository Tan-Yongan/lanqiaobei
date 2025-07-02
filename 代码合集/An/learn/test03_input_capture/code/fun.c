#include "headfile.h"

uint32_t fre1,capture_value1,fre2,capture_value2;

char text[20];  //�����Ļһ�������ʾ20���ַ�
void lcd_show()
{
	sprintf(text,"    test    ");   //�Ѻ�����ַ�������text���� 
	LCD_DisplayStringLine(Line0,(uint8_t *)text);  //��Ϊ������յ�������u8 (uint8_t *)ǿ��ת��
	
	sprintf(text,"     fre1:%d        ",fre1);
	LCD_DisplayStringLine(Line2,(uint8_t *)text);
	
	sprintf(text,"     fre2:%d        ",fre2);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		capture_value1 = TIM2->CCR1;
		//capture_value = HAL_TIM_ReadCapturedValue(&htim17,TIM_CHANNEL_1);   //�ڲ���������ʱ���ὫCNT��ֵ��CCR
		TIM2->CNT = 0;
		fre1 = 80000000/(80*capture_value1);		
	}
	
	if(htim->Instance == TIM16)
	{
		capture_value2 = TIM16->CCR1;
		//capture_value = HAL_TIM_ReadCapturedValue(&htim17,TIM_CHANNEL_1);   //�ڲ���������ʱ���ὫCNT��ֵ��CCR
		TIM16->CNT = 0;
		fre2 = 80000000/(80*capture_value2);		
	}	
}

