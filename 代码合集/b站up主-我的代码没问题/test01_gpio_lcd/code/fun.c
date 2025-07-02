#include "headfile.h"

// 本代码在原视频基础上添加了 按键双击 功能
// 双击功能代码仅供参考 还有改进空间

/* 双击功能实现思路
	在原视频代码基础上新增三个变量 double_time、double_flag、tick
	1.按键双击，也就是两次按下，当两次按下按键之间的间隔时间小于double_time时，认为双击发生
	
	2.tick用于记录按键按下的一瞬间的计数值。 
	  HAL_GetTick()这个函数会返回一个计数值，跟定时器的计数器类似，默认是每1ms计数加1
		在按键按下时用变量tick记录下HAL_GetTick()返回的计数值
		在之后就可以用 HAL_GetTick() - tick 来得到 相对于按键按下时 所经过的时间
	  
	3.double_flag用来设定按键双击这个功能的状态：
	   double_flag为0时，表示按键没有按下
		 double_flag为1时，表示按键第一次按下
		 double_flag为2时，表示双击发生
		 但这里会出现两个问题：第一个问题，如果你的目的是进行一次短按，那么依然会使double_flag变为1，所以要加上下面的代码
		                                  	if(double_flag == 1)
																				{
																					if(HAL_GetTick() - tick > double_time)  // 双击未发生
																					{
																						double_flag = 0;
																					}
																				}
											     如果按键按下一次之后，经过的时间超过了设定的双击间隔，认为双击没有发生，将double_flag重新置为0
													 
													 第二个问题，每次按键松开的瞬间 都会认为是一次短按，防止在双击结束之后，重复执行短按里面的代码，要在短按的判断语句里重写代码
																				if(double_flag == 2) // 双击发生 不执行短按操作 并将标志位清0
																				{
																					double_flag = 0;
																				}
																				else                //按键B1短按
																				{
																					count += 2;
																				}
													
													第三个问题，在第二个问题中，只解决了双击里第二次按键按下后的重复执行，第一次按键按下依然会执行短按里的代码。该问题没有解决，可自行思考。
*/

#define double_time 500  // 双击的间隔时间

uint8_t lcd_highshow;

int count = 0;
uint8_t led_mode;

void led_show(uint8_t led, uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	if(mode)
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

uint8_t B1_state;
uint8_t B1_last_state = 1;
uint8_t B2_state;
uint8_t B2_last_state = 1;
uint8_t B3_state;
uint8_t B3_last_state = 1;
uint8_t B4_state;
uint8_t B4_last_state = 1;

uint8_t double_flag = 0;  // 双击状态
uint32_t tick;            // 时间计数

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	

	
//按键B1
	if(B1_state == 0 && B1_last_state == 1)  // 按键B1按下
	{
		// 双击判断
		if(double_flag == 0)
		{
			tick = HAL_GetTick();
			double_flag = 1;
		}
		else if(double_flag == 1)
		{
			if(HAL_GetTick() - tick < double_time)  // 双击发生
			{
				double_flag = 2;
				// 此处编写双击代码
				count = 0;
			}
		}
		
		TIM3->CNT = 0;
	}
	
  if(B1_state == 0 && B1_last_state == 0)  // 按键B1一直按着
	{
		if(TIM3->CNT >= 10000)   //按键B1长按1s
		{
			count++;
		}
	}
	
	if(double_flag == 1)
	{
		if(HAL_GetTick() - tick > double_time)  // 双击未发生
		{
			double_flag = 0;
		}
	}
	
  if(B1_state == 1 && B1_last_state == 0)  // 按键B1松开
	{
		if(TIM3->CNT < 10000)   
		{
			if(double_flag == 2) // 双击发生 不执行短按操作 并将标志位清0
			{
				double_flag = 0;
			}
			else                //按键B1短按
			{
				count += 2;
			}
			
		}
	}
	

	

// 按键B2
	if(B2_state == 0 && B2_last_state == 1)  // 按键B2按下
	{
		TIM3->CNT = 0;
	}
	else if(B2_state == 0 && B2_last_state == 0)  // 按键B2一直按着
	{
		if(TIM3->CNT >= 10000)   //按键B2长按1s
		{
			count--;
		}
	}
	else if(B2_state == 1 && B2_last_state == 0)  // 按键B2松开
	{
		if(TIM3->CNT < 10000)    //按键B2短按
		{
			count -= 2;
		}
	}
	
//按键B3
	if(B3_state == 0 && B3_last_state == 1)  // 按键B3按下
	{
		led_show(2, 1);
	}
	
//按键B4
	if(B4_state == 0 && B4_last_state == 1)  // 按键B4按下
	{
		lcd_highshow ++;
		lcd_highshow %= 3;
		led_show(2, 0);
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}

char text[20];
void lcd_show()
{
	sprintf(text, "      test          ");
	LCD_DisplayStringLine(Line0, (uint8_t *)text);
	if(lcd_highshow == 0)
	{
		LCD_SetBackColor(Yellow);
		sprintf(text, "      count: %d     ", count);
		LCD_DisplayStringLine(Line3, (uint8_t *)text);
		LCD_SetBackColor(Black);		
		sprintf(text, "      para1         ");
		LCD_DisplayStringLine(Line4, (uint8_t *)text);
		sprintf(text, "      para2         ");
		LCD_DisplayStringLine(Line5, (uint8_t *)text);		
	}
	else if(lcd_highshow == 1)
	{

		sprintf(text, "      count: %d     ", count);
		LCD_DisplayStringLine(Line3, (uint8_t *)text);	
		LCD_SetBackColor(Yellow);
		sprintf(text, "      para1         ");
		LCD_DisplayStringLine(Line4, (uint8_t *)text);
		LCD_SetBackColor(Black);	
		sprintf(text, "      para2         ");
		LCD_DisplayStringLine(Line5, (uint8_t *)text);			
	}
	else if(lcd_highshow == 2)
	{

		sprintf(text, "      count: %d     ", count);
		LCD_DisplayStringLine(Line3, (uint8_t *)text);	
		sprintf(text, "      para1         ");
		LCD_DisplayStringLine(Line4, (uint8_t *)text);
		LCD_SetBackColor(Yellow);
		sprintf(text, "      para2         ");
		LCD_DisplayStringLine(Line5, (uint8_t *)text);	
		LCD_SetBackColor(Black);	
	}

	
	led_show(1, led_mode);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)  //TIM2中断函数
	{
		led_mode++;
		led_mode = led_mode % 2;
	}
}

