#include "headfile.h"

char text[20];

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

uint8_t lcd_switch;
uint8_t mode_switch;

uint8_t pwm_PA6 = 10;
uint8_t pwm_PA7 = 10;

void led_show(uint8_t led,uint8_t state)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	if(state)
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1),GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1),GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void lcd_show()
{
	if(!lcd_switch)
	{
		sprintf(text,"      Data         ");
		LCD_DisplayStringLine(Line0, (uint8_t *)text);	
	}
	else
	{
		sprintf(text,"      Para         ");
		LCD_DisplayStringLine(Line0, (uint8_t *)text);	
	}
}

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{
		lcd_switch++;
		lcd_switch %= 2;
	}
	if(mode_switch)
	{
		if(lcd_switch)
		{
			if(!B2_state&&B2_last_state)
			{
				pwm_PA6 += 10;
				if(pwm_PA6>90)
					pwm_PA6 = 10;
				TIM3->CCR1 = pwm_PA6;
			}
			if(!B3_state&&B3_last_state)
			{
				pwm_PA7 += 10;
				if(pwm_PA7>90)
					pwm_PA7 = 10;
				TIM17->CCR1 = pwm_PA7;
			}
		}
	}
	if(!B4_state&&B4_last_state)
	{
		mode_switch++;
		mode_switch %= 2;
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
	
}

double vol_get()
{
	uint32_t ad_value;
	HAL_ADC_Start(&hadc2);
	ad_value = HAL_ADC_GetValue(&hadc2);
	return 3.3*ad_value/4096;
}
