#include "headfile.h"

char text[20];
void lcd_show()
{
	sprintf(text, "      test          ");
	LCD_DisplayStringLine(Line0, (uint8_t *)text);

	sprintf(text, "    R37_VOL:%.2f    ", get_vol(&hadc2));
	LCD_DisplayStringLine(Line3, (uint8_t *)text);
	sprintf(text, "    R38_VOL:%.2f    ", get_vol(&hadc1));
	LCD_DisplayStringLine(Line5, (uint8_t *)text);
}

double get_vol(ADC_HandleTypeDef *hadc)
{
	HAL_ADC_Start(hadc);
	uint32_t adc_value = HAL_ADC_GetValue(hadc);
	return 3.3*adc_value/4096;
	
}