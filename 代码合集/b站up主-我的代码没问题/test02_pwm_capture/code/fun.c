#include "headfile.h"

uint32_t fre, capture_value;
char text[20];
void lcd_show()
{
	sprintf(text, "      test          ");
	LCD_DisplayStringLine(Line0, (uint8_t *)text);
	sprintf(text, "      fre:%d          ", fre);
	LCD_DisplayStringLine(Line2, (uint8_t *)text);

}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM17)
	{
		//capture_value = TIM17->CCR1;
		capture_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  //在捕获到上升沿时，会将CNT赋值给CCR 
		TIM17->CNT = 0;
		fre = 80000000/(80*capture_value);
	}
}
