#include "headfile.h"

uint32_t fre,capture_value;
float duty_temp;
uint8_t duty;

char text[20];  //这块屏幕一行最多显示20个字符
void lcd_show()
{
	sprintf(text,"         DATA   ");   //把后面的字符拷贝到text里面 
	LCD_DisplayStringLine(Line1,(uint8_t *)text);  //因为这个接收的类型是u8 (uint8_t *)强制转换
	sprintf(text,"     fre:%d     ",fre);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	duty_temp = (float)TIM2->CCR2/(TIM2->ARR+1);
	duty = duty_temp*100;
	sprintf(text,"     duty:%2d%%   ",duty);
	LCD_DisplayStringLine(Line5,(uint8_t *)text);

}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM17)
	{
		capture_value = TIM17->CCR1 + 1;
		//capture_value = HAL_TIM_ReadCapturedValue(&htim17,TIM_CHANNEL_1);   //在捕获到上升沿时，会将CNT赋值给CCR
		TIM17->CNT = 0;
		fre = 80000000/(80*capture_value);		
	}
}

uint8_t Timer_1S;
uint32_t FER = 1000;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		FER+=100;
		TIM2->PSC = 80000000/(FER*100)-1;		
		//TIM2->ARR = 80000000/(FER*800)-1;
	}
}
