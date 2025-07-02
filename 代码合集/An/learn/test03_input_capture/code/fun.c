#include "headfile.h"

uint32_t fre1,capture_value1,fre2,capture_value2;

char text[20];  //这块屏幕一行最多显示20个字符
void lcd_show()
{
	sprintf(text,"    test    ");   //把后面的字符拷贝到text里面 
	LCD_DisplayStringLine(Line0,(uint8_t *)text);  //因为这个接收的类型是u8 (uint8_t *)强制转换
	
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
		//capture_value = HAL_TIM_ReadCapturedValue(&htim17,TIM_CHANNEL_1);   //在捕获到上升沿时，会将CNT赋值给CCR
		TIM2->CNT = 0;
		fre1 = 80000000/(80*capture_value1);		
	}
	
	if(htim->Instance == TIM16)
	{
		capture_value2 = TIM16->CCR1;
		//capture_value = HAL_TIM_ReadCapturedValue(&htim17,TIM_CHANNEL_1);   //在捕获到上升沿时，会将CNT赋值给CCR
		TIM16->CNT = 0;
		fre2 = 80000000/(80*capture_value2);		
	}	
}

