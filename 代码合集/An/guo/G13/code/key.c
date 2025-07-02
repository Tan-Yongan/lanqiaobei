#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

uint8_t show_switch;
void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{
		show_switch ++;
		show_switch %= 3;
	}
	if(!B2_state&&B2_last_state)
	{
		X++;
		if(X>4)
			X=1;
	}
	if(!B3_state&&B3_last_state)
	{
		Y++;
		if(Y>4)
			Y=1;	
		
		LCD_WriteReg(R1,0x0000);   // set SS and SM bit    正0x0000 反0x0100
		LCD_WriteReg(R96,0x2700);  // Gate Scan Line		正0x2700    反0xA700

	}
	if(!B4_state&&B4_last_state)
	{
		
		LCD_WriteReg(R1,0x0100);   // set SS and SM bit    正0x0000 反0x0100
		LCD_WriteReg(R96,0xA700);  // Gate Scan Line		正0x2700    反0xA700

	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}
