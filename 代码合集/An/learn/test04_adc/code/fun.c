#include "headfile.h"

uint32_t fre1,capture_value1,fre2,capture_value2;

char text[20];  //�����Ļһ�������ʾ20���ַ�
void lcd_show()
{
	sprintf(text,"        test        ");   //�Ѻ�����ַ�������text���� 
	LCD_DisplayStringLine(Line0,(uint8_t *)text);  //��Ϊ������յ�������u8 (uint8_t *)ǿ��ת��
	
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
