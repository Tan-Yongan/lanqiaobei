#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state = 1;
uint8_t B2_last_state = 1;
uint8_t B3_last_state = 1;
uint8_t B4_last_state = 1;
uint8_t B5_last_state = 1;

uint8_t show_switch;
uint8_t select;

uint8_t ld_max=1,ld_min=2;
double vol_max=2.4,vol_min=1.2;

uint8_t tim_flag;

char text[20];

void led_show(uint8_t led,uint8_t state)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	if(state)
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1),GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1),GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);

	if(!B1_state&&B1_last_state)  //B1����
	{
		show_switch++;
		show_switch %=2;     //��֤show_switch��0��1֮��仯		
	}
	if(show_switch)          //��֤��show_switch == 1 ʱ��Setting�������������Ĳ�������Ч
	{
		if(!B2_state&&B2_last_state)  //B2����
		{
			select++;                 //ѡ��߹ⰴť����0��1��2��3 ѭ��
			select %= 4;
		}
		if(!B3_state&&B3_last_state)  //B3���� (��)
		{
			if(select == 0)    //Max Volt:
			{
				vol_max+=0.3;
				if(vol_max>3.3)
					vol_max = 3.3;
			}
			else if(select == 1)
			{
				if(vol_min+0.3<=vol_max)
				{
					vol_min+=0.3;
					if(vol_min>3.3)
						vol_min = 3.3;
				}
			}
			else if(select == 2)
			{
				if((ld_max+1)== ld_min && ld_min!=8)
					ld_max+=2;
				else if(ld_min == 8 && ld_max == 7)
					ld_max = 7;
				else
					ld_max++;
				if(ld_max>8)
					ld_max = 8;
			}
			else if(select == 3)
			{
				if((ld_min+1)== ld_max && ld_max!=8)
					ld_min+=2;
				else if(ld_max == 8 && ld_min == 7)
					ld_min = 7;
				else  
					ld_min++;
				if(ld_min>8)
					ld_min = 8;
			}
		}
		if(!B4_state&&B4_last_state)   //B4���£�����
		{
			if(select == 0)
			{
				if(vol_max-0.3>=vol_min)
				{
					vol_max -= 0.3;
					if(vol_max<0)
						vol_max = 0;
				}
			}
			if(select == 1)
			{
				vol_min -= 0.3;
				if(vol_min<=0)
					vol_min = 0;
			}
			if(select == 2)
			{
				if((ld_max-1) == ld_min && ld_min!=1)
					ld_max -=2;
				else if(ld_min == 1 && ld_max == 2)
					ld_max = 2;
				else
					ld_max --;
				if(ld_max < 1)
					ld_max = 1;
			}
			if(select == 3)
			{
				if((ld_min-1) == ld_max && ld_max!=1)
					ld_min -=2;
				else if(ld_max == 1 && ld_min == 2)
					ld_min = 2;
				else
					ld_min --;
				if(ld_min < 1)
					ld_min = 1;
			}		
		}
	}
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}

double get_vol()
{
	uint32_t vol_value;
	HAL_ADC_Start(&hadc2);
	vol_value = HAL_ADC_GetValue(&hadc2);
	return 3.3*vol_value/4096;
}

void lcd_show()
{
	if(!show_switch)
	{
		sprintf(text,"        Main           ");
		LCD_DisplayStringLine(Line0, (uint8_t *)text);
		sprintf(text,"                       ");
		LCD_DisplayStringLine(Line6,(uint8_t *)text);
		sprintf(text,"                       ");
		LCD_DisplayStringLine(Line8,(uint8_t *)text);		
		sprintf(text,"     Volt:%.2fV        ",get_vol());
		LCD_DisplayStringLine(Line2, (uint8_t *)text);
		
		if(get_vol()>vol_max)
		{
			sprintf(text,"   Status:Upper    ");
			LCD_DisplayStringLine(Line4, (uint8_t *)text);		
		}
		else if(get_vol()<vol_min)
		{
			sprintf(text,"   Status:Lower    ");
			LCD_DisplayStringLine(Line4, (uint8_t *)text);		
		}
		else
		{
			sprintf(text,"   Status:Normal    ");
			LCD_DisplayStringLine(Line4, (uint8_t *)text);		
		}
	}
	else
	{
		sprintf(text,"      Setting         ");
		LCD_DisplayStringLine(Line0, (uint8_t *)text);
		if(select == 0)
		{
			LCD_SetBackColor(Green);
			sprintf(text,"    Max Volt:%.1fV   ",vol_max);
			LCD_DisplayStringLine(Line2,(uint8_t *)text);
			LCD_SetBackColor(White);
			sprintf(text,"    Min Volt:%.1fV   ",vol_min);
			LCD_DisplayStringLine(Line4,(uint8_t *)text);
			sprintf(text,"    Upper:LD%d       ",ld_max);
			LCD_DisplayStringLine(Line6,(uint8_t *)text);
			sprintf(text,"    Lower:LD%d       ",ld_min);
			LCD_DisplayStringLine(Line8,(uint8_t *)text);
		}
		else if(select == 1)
		{
			sprintf(text,"    Max Volt:%.1fV   ",vol_max);
			LCD_DisplayStringLine(Line2,(uint8_t *)text);
			LCD_SetBackColor(Green);
			sprintf(text,"    Min Volt:%.1fV   ",vol_min);
			LCD_DisplayStringLine(Line4,(uint8_t *)text);
			LCD_SetBackColor(White);
			sprintf(text,"    Upper:LD%d       ",ld_max);
			LCD_DisplayStringLine(Line6,(uint8_t *)text);
			sprintf(text,"    Lower:LD%d       ",ld_min);
			LCD_DisplayStringLine(Line8,(uint8_t *)text);
		}
		else if(select == 2)
		{
			sprintf(text,"    Max Volt:%.1fV   ",vol_max);
			LCD_DisplayStringLine(Line2,(uint8_t *)text);
			sprintf(text,"    Min Volt:%.1fV   ",vol_min);
			LCD_DisplayStringLine(Line4,(uint8_t *)text);
			LCD_SetBackColor(Green);
			sprintf(text,"    Upper:LD%d       ",ld_max);
			LCD_DisplayStringLine(Line6,(uint8_t *)text);
			LCD_SetBackColor(White);
			sprintf(text,"    Lower:LD%d       ",ld_min);
			LCD_DisplayStringLine(Line8,(uint8_t *)text);
		}	
		else if(select == 3)
		{
			sprintf(text,"    Max Volt:%.1fV  ",vol_max);
			LCD_DisplayStringLine(Line2,(uint8_t *)text);
			sprintf(text,"    Min Volt:%.1fV   ",vol_min);
			LCD_DisplayStringLine(Line4,(uint8_t *)text);
			sprintf(text,"    Upper:LD%d       ",ld_max);
			LCD_DisplayStringLine(Line6,(uint8_t *)text);
			LCD_SetBackColor(Green);
			sprintf(text,"    Lower:LD%d       ",ld_min);
			LCD_DisplayStringLine(Line8,(uint8_t *)text);
			LCD_SetBackColor(White);
		}
	}
}

void led()
{
	if(get_vol()>vol_max)
		led_show(ld_max,tim_flag);
	else if(get_vol()<vol_min)
		led_show(ld_min,tim_flag);
	else
	{
		led_show(ld_max,0);
		led_show(ld_min,0);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(get_vol()>vol_max || get_vol()<vol_min)
		{
			tim_flag++;
			tim_flag %= 2;       //ÿ0.02s��һ�ζ�ʱ�ж� ��0��1֮����
		}
	}
}
