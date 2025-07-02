#include "headfile.h"

uint8_t lcd_highshow;
int count = 0;
uint8_t led_mode;

void led_show(uint8_t led,uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	
	if(mode)
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led - 1),GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led - 1),GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);

}

//代码一直在循环，按键还没有按下，B1_state就读到1,赋给B1_last_state.直到按键按下，if才满足。
uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state = 1;
uint8_t B2_last_state = 1;
uint8_t B3_last_state = 1;
uint8_t B4_last_state = 1;

uint32_t B2Tick;
uint32_t B2_gap;
uint8_t click_count_B2;
uint8_t click_flag_B2;

uint8_t B4_click_flag;
uint8_t B4count;
uint32_t B4_gap;
uint32_t B4Tick;
uint32_t B1Tick;
uint32_t keyTick;
uint32_t B3Tick;
void key_scan()
{
	if(uwTick-keyTick<20) return;
	keyTick = uwTick;
	
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(B1_state == 0 && B1_last_state == 1)   //按键B1按下
	{
		B1Tick = uwTick;
	}
	else if(B1_state == 0 && B1_last_state == 0)  //按键B1一直按着
	{
		if(uwTick - B1Tick >= 1000)  //按键B1长按1s
		{
			count++;
		}
	}
	else if(B1_state == 1 && B1_last_state == 0)  //按键B1松开
	{
		if(uwTick - B1Tick < 1000)  //按键B1短按
		{
			count += 2; 
		}
	}
	
	
	if(B2_state == 0 && B2_last_state == 1)   //按键B2按下
	{
		if(click_count_B2>0 && (uwTick - B2Tick)<=400)  
		{
			click_count_B2++;
			if(click_count_B2 == 3)
			{
				//三击操作
				count+=30;	
				click_count_B2 = 0;
				click_flag_B2 = 0;				
			}
		}
		else
		{
			B2Tick = uwTick;
			click_count_B2 = 1;
			click_flag_B2 = 1;
			B2_gap = 0;
		}
	}		
	
	if(B3_state == 0 && B3_last_state == 1)   //按键B3按下
	{
		B3Tick = uwTick;
	}
	else if(B3_state == 1 && B3_last_state == 0)
	{
		if(uwTick-B3Tick>1000)
			count = 0;
		else
			count+=5;
	}
	
	if(B4_state == 0 && B4_last_state == 1)   //按键B4按下
	{
		//双击
		if(B4_click_flag && uwTick-B4Tick<=250)
		{
			//双击操作
			count-=10;
			B4_click_flag = 0;
		}
		else
		{
			B4Tick = uwTick;
 			B4_click_flag = 1;
			B4_gap = 0;

		}
//		lcd_highshow++;
//		lcd_highshow %= 3;   //使得其在012中循环
//		led_show(2,0);
	}		
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
	
}
	
	
char text[20];  //这块屏幕一行最多显示20个字符
void lcd_show()
{
	sprintf(text,"    an love jia    ");   //把后面的字符拷贝到text里面 
	LCD_DisplayStringLine(Line0,(uint8_t *)text);  //因为这个接收的类型是u8 (uint8_t *)强制转换
	sprintf(text,"    count:%d    ",count);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	if(lcd_highshow == 0)
	{
		LCD_SetBackColor(Yellow);
		sprintf(text,"      I        ");
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		LCD_SetBackColor(Black);	
		sprintf(text,"    LOVE       ");
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		sprintf(text,"      U        ");
		LCD_DisplayStringLine(Line6,(uint8_t *)text);	
	}
	else if(lcd_highshow == 1)
	{
		sprintf(text,"      I        ");
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		LCD_SetBackColor(Yellow);
		sprintf(text,"    LOVE       ");
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		LCD_SetBackColor(Black);	
		sprintf(text,"      U        ");
		LCD_DisplayStringLine(Line6,(uint8_t *)text);		
	}
	else if(lcd_highshow == 2)
	{
		sprintf(text,"      I        ");
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		sprintf(text,"    LOVE       ");
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		LCD_SetBackColor(Yellow);
		sprintf(text,"      U        ");
		LCD_DisplayStringLine(Line6,(uint8_t *)text);		
		LCD_SetBackColor(Black);	
	}
	led_show(1,led_mode);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)  //TIM2中断函数
	{
		//count++;
		led_mode = !led_mode;
	}
	
	if(htim->Instance == TIM4)
	{
		if(click_flag_B2)
		{
			B2_gap++;
			//单击操作
			if(B2_gap>400)
			{
				if(click_count_B2 == 1)
				{
					//单击
					count+=10;
				}
				else if(click_count_B2 == 2)
				{
					count+=20;
				}
				click_flag_B2 = 0;
				B2_gap = 0;
				click_count_B2 = 0;
			}
		}
		if(B4_click_flag)
		{
			B4_gap++;
			if(B4_gap>250)
			{
				count-=2;
				B4_click_flag = 0;
				B4_gap = 0;
			}
		}
	}
}
