#include "headfile.h"
#include "usart.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state = 1;
uint8_t B2_last_state = 1;
uint8_t B3_last_state = 1;
uint8_t B4_last_state = 1;

uint8_t lcd_switch;
uint8_t buy_flag;

char send_buff[20];

uint32_t keyTick;
void key_sacn()
{
	if(uwTick - keyTick < 20)return;
	keyTick = uwTick;
	
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{
		lcd_switch++;
		lcd_switch %= 3;
	}
	
	if(!B2_state&&B2_last_state)
	{
		if(lcd_switch == 0)
		{
			X_buy++;
			if(X_buy>X)
				X_buy = 0;
		}
		if(lcd_switch == 1)
		{
			X_price += 0.1;
			if(X_price > 2.01)
				X_price = 1.0;
			eeprom_write(2,X_price*10);
		}
		if(lcd_switch == 2)
		{
			X++;
			eeprom_write(0,X);

		}
	}
	
	if(!B3_state&&B3_last_state)
	{
		if(lcd_switch == 0)
		{
			Y_buy++;
			if(Y_buy>Y)
				Y_buy = 0;
		}
		if(lcd_switch == 1)
		{
			Y_price += 0.1;
			if(Y_price > 2.01)
				Y_price = 1.0;
			eeprom_write(3,Y_price*10);
		}
		if(lcd_switch == 2)
		{
			Y++;
			eeprom_write(1,Y);

		}
	}
	
	if(!B4_state&&B4_last_state)
	{
		if(lcd_switch == 0)
		{
			TIM2->CCR2 = 30;
			buy_flag = 1;
			X -=X_buy;
			Y -=Y_buy;
			printf("X:%d,Y:%d,Z:%.1f\r\n",X_buy,Y_buy,((X_buy*X_price)+(Y_buy*Y_price)));
			X_buy = 0;
			Y_buy = 0;
			eeprom_write(0,X);
			eeprom_write(1,Y);
		}
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}
