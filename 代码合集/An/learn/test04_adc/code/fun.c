#include "headfile.h"

uint32_t fre1,capture_value1,fre2,capture_value2;

char text[20];  //这块屏幕一行最多显示20个字符
void lcd_show()
{
	sprintf(text,"        test        ");   //把后面的字符拷贝到text里面 
	LCD_DisplayStringLine(Line0,(uint8_t *)text);  //因为这个接收的类型是u8 (uint8_t *)强制转换
	
	sprintf(text,"   R38_vol:%.2fV    ",get_vol(&hadc1));
	LCD_DisplayStringLine(Line2,(uint8_t *)text); 
	
	sprintf(text,"   R37_vol:%.2fV    ",get_vol(&hadc2));  
	LCD_DisplayStringLine(Line4,(uint8_t *)text); 
}


double get_vol(ADC_HandleTypeDef *hadc)
{
	HAL_ADC_Start(hadc);
	uint32_t adc_value = HAL_ADC_GetValue(hadc);
	return 3.3*adc_value/4096;
}


int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
  /* Your implementation of fputc(). */
  return ch;
}
