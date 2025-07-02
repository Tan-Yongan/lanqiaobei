#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

char text[20];

uint8_t show_switch;
uint8_t select;
uint8_t ld_max=1,ld_min=2;
uint8_t tim_flag;
double vol_max=2.4,vol_min=1.2;

void led_show(uint8_t led,uint8_t state)         //ledœ‘ æ
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	if(state)
	{
		HAL_GPIO_WritePin(GPIOC,0x80<<led,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,0x80<<led,GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}


void key_scan()   //∞¥º¸…®√Ë
{
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);	
	
	if(!B1_state&&B1_last_state)
	{
		show_switch++;
		show_switch = show_switch%2;
	}

	if(show_switch)
	{
		if(!B2_state&&B2_last_state)
		{
			select++;
			if(select>3)
				select=0;
		}
		
		if(!B3_state&&B3_last_state)
		{
			if(select==0)
			{
				vol_max+=0.3;
				if(vol_max>3.3)
					vol_max = 3.3;
			}
			else if(select==1)
			{
				if(vol_min+0.3<=vol_max)
				{
					vol_min+=0.3;
					if(vol_min>3.3)
						vol_min = 3.3;
				}
			}
			else if(select==2)
			{
				if((ld_max+1)!=ld_min)
				{
					led_show(ld_max,0);
					ld_max++;
					if(ld_max>8)
						ld_max=8;
			    }
			}
			else if(select==3)
			{
				if((ld_min+1)!=ld_max)
				{
					led_show(ld_min,0);
					ld_min++;
					if(ld_min>8)
						ld_min=8;
			  }
			}
				
		}
		
		if(!B4_state&&B4_last_state)
		{
			if(select==0)
			{
				if(vol_max-0.3>=vol_min)
				{
					vol_max-=0.3;
					if(vol_max<0)
						vol_max = 0;
				}
			}
			else if(select==1)
			{
				vol_min-=0.3;
				if(vol_min<0)
					vol_min = 0;
			}
			else if(select==2)
			{
				if((ld_max-1)!=ld_min)
				{
					led_show(ld_max,0);
					ld_max--;
					if(ld_max<1)
						ld_max=1;
			  }
			}
			else if(select==3)
			{
				if((ld_min-1)!=ld_max)
				{
					led_show(ld_min,0);
					ld_min--;
					if(ld_min<1)
						ld_min=1;
			  }
			}
		}
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}

double get_ad()
{
	uint32_t ad_value;
	HAL_ADC_Start(&hadc2);
	ad_value = HAL_ADC_GetValue(&hadc2);
	return ad_value/4032.0*3.3;
}

void lcd_show()
{
	if(!show_switch)
	{
		sprintf(text,"        Main   ");
		LCD_DisplayStringLine(Line1,(uint8_t *)text);
		sprintf(text,"                   ");
		LCD_DisplayStringLine(Line2,(uint8_t *)text);
		sprintf(text,"                   ");
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		sprintf(text,"   Volt:%.2f        ",get_ad());
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		if(get_ad()>vol_max)
		{
			sprintf(text,"   Status:Upper   ");
			LCD_DisplayStringLine(Line5,(uint8_t *)text);			
		}
		else if(get_ad()<vol_min)
		{
			sprintf(text,"   Status:Lower   ");
			LCD_DisplayStringLine(Line5,(uint8_t *)text);		
		}
		else
		{
			sprintf(text,"   Status:Normal   ");
			LCD_DisplayStringLine(Line5,(uint8_t *)text);		
		}
	}
	else
	{
		sprintf(text,"        Setting   ");
		LCD_DisplayStringLine(Line1,(uint8_t *)text);
		if(select==0)
		{
			LCD_SetBackColor(Yellow);
			sprintf(text,"    Max Volt:%.1f   ",vol_max);
			LCD_DisplayStringLine(Line2,(uint8_t *)text);
			LCD_SetBackColor(Black);
			sprintf(text,"    Min Volt:%.1f   ",vol_min);
			LCD_DisplayStringLine(Line3,(uint8_t *)text);
			sprintf(text,"    Upper:LD%d     ",ld_max);
			LCD_DisplayStringLine(Line4,(uint8_t *)text);
			sprintf(text,"    Lower:LD%d     ",ld_min);
			LCD_DisplayStringLine(Line5,(uint8_t *)text);
		}
		else if(select==1)
		{
			sprintf(text,"    Max Volt:%.1f   ",vol_max);
			LCD_DisplayStringLine(Line2,(uint8_t *)text);
			LCD_SetBackColor(Yellow);
			sprintf(text,"    Min Volt:%.1f   ",vol_min);
			LCD_DisplayStringLine(Line3,(uint8_t *)text);
			LCD_SetBackColor(Black);
			sprintf(text,"    Upper:LD%d     ",ld_max);
			LCD_DisplayStringLine(Line4,(uint8_t *)text);
			sprintf(text,"    Lower:LD%d     ",ld_min);
			LCD_DisplayStringLine(Line5,(uint8_t *)text);
		}
		else if(select==2)
		{
			sprintf(text,"    Max Volt:%.1f   ",vol_max);
			LCD_DisplayStringLine(Line2,(uint8_t *)text);
			sprintf(text,"    Min Volt:%.1f   ",vol_min);
			LCD_DisplayStringLine(Line3,(uint8_t *)text);
			LCD_SetBackColor(Yellow);
			sprintf(text,"    Upper:LD%d     ",ld_max);
			LCD_DisplayStringLine(Line4,(uint8_t *)text);
			LCD_SetBackColor(Black);
			sprintf(text,"    Lower:LD%d     ",ld_min);
			LCD_DisplayStringLine(Line5,(uint8_t *)text);
		}
		else if(select==3)
		{
			sprintf(text,"    Max Volt:%.1f   ",vol_max);
			LCD_DisplayStringLine(Line2,(uint8_t *)text);
			sprintf(text,"    Min Volt:%.1f   ",vol_min);
			LCD_DisplayStringLine(Line3,(uint8_t *)text);
			sprintf(text,"    Upper:LD%d     ",ld_max);
			LCD_DisplayStringLine(Line4,(uint8_t *)text);
			LCD_SetBackColor(Yellow);
			sprintf(text,"    Lower:LD%d     ",ld_min);
			LCD_DisplayStringLine(Line5,(uint8_t *)text);
			LCD_SetBackColor(Black);
		}
		
	}
	
	if(get_ad()>vol_max)
		led_show(ld_max,tim_flag);
	else if(get_ad()<vol_min)
		led_show(ld_min,tim_flag);
	else
	{
		led_show(ld_max,0);
		led_show(ld_min,0);
	}
	
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		if(get_ad()>vol_max||get_ad()<vol_min)
		{
			tim_flag++;
			tim_flag%=2;
		}
	}
}




