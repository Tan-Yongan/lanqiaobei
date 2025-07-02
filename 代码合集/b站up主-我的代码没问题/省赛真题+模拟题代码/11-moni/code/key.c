#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

uint8_t time_select;

char text1[20];

KEY_Read key;

void key_scan()
{
	B1_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state) //B1按下
	{
		if(show_switch==0)  //主界面
		{
			time_select=0;
			show_switch=1;		//进入时钟设置界面
			set_time_number++;
			eeprom_write(1,set_time_number);
		}
		else if(show_switch==1)  //时钟设置界面
		{
			show_switch=0;  //回到主界面
			HAL_RTC_SetTime(&hrtc,&time1,RTC_FORMAT_BIN);
			sprintf(text1,"New RTC:%02d:%02d:%02d\r\n",time1.Hours,time1.Minutes,time1.Seconds);
			HAL_UART_Transmit(&huart1,(uint8_t *)text1,20,50);
		}
		else      //报警设置界面
		{
			time_select++;
			time_select%=3;
		}
	}
	

	if(!B2_state&&B2_last_state) //B2按下
	{
		if(show_switch==0)  //主界面
		{
			time_select=0;
			show_switch=2;	//进入报警设置页面
			set_alarm_number++;
			eeprom_write(2,set_alarm_number);
		}
		else if(show_switch==1)  //时钟设置界面
		{
			time_select++;
			time_select%=3;
		}
		else       
		{
			show_switch=0;  //回到主界面
			HAL_RTC_SetAlarm(&hrtc,&time_alarm,RTC_FORMAT_BIN);
			sprintf(text1,"New Alarm:%02d:%02d:%02d\r\n",time_alarm.AlarmTime.Hours,time_alarm.AlarmTime.Minutes,time_alarm.AlarmTime.Seconds);
			eeprom_write(3,time_alarm.AlarmTime.Hours);
			eeprom_write(4,time_alarm.AlarmTime.Minutes);
			eeprom_write(5,time_alarm.AlarmTime.Seconds);
			HAL_UART_Transmit(&huart1,(uint8_t *)text1,20,50);
		}
	}

//************************B3/B4长按短按检测*******************************//
	if(!B3_state&&B3_last_state)        //B3按下
	{
		TIM4->CNT=0;
	}
	if(!B3_state&&(TIM4->CNT>=10000))   //长按检测
	{
		key.B3_long=1;
	}
	if(B3_state&&!B3_last_state&&(TIM4->CNT<10000))      //短按检测
	{
		key.B3_short=1;
	}
	if(!B4_state&&B4_last_state)        //B4按下
	{
		TIM4->CNT=0;
	}
	if(!B4_state&&(TIM4->CNT>=10000))   //长按检测
	{
		key.B4_long=1;
	}
	if(B4_state&&!B4_last_state&&(TIM4->CNT<10000))      //短按检测
	{
		key.B4_short=1;
	}
//************************B3/B4长按短按检测*******************************//
	
if(show_switch==1)   //设置时钟时间
{
	if(key.B3_short)    //B3短按
	{
		key.B3_short=0;
		if(time_select==0)
		{
			time1.Hours++;
			if(time1.Hours==24)
				time1.Hours=0;
		}
		else if(time_select==1)
		{
			time1.Minutes++;
			if(time1.Minutes==60)
				time1.Minutes=0;					
		}
		else
		{
			time1.Seconds++;
			if(time1.Seconds==60)
				time1.Seconds=0;		
		}
		
	}
	
	if(key.B3_long)   //B3长按
	{
		key.B3_long=0;
		if(time_select==0)
		{
			time1.Hours++;
			if(time1.Hours==24)
				time1.Hours=0;
		}
		else if(time_select==1)
		{
			time1.Minutes++;
			if(time1.Minutes==60)
				time1.Minutes=0;					
		}
		else
		{
			time1.Seconds++;
			if(time1.Seconds==60)
				time1.Seconds=0;		
		}
		
	}
	
	if(key.B4_short)  //B4短按
	{
		key.B4_short=0;
		if(time_select==0)
		{
			time1.Hours--;
			if(time1.Hours==255)
				time1.Hours=23;
		}
		else if(time_select==1)
		{
			time1.Minutes--;
			if(time1.Minutes==255)
				time1.Minutes=59;					
		}
		else
		{
			time1.Seconds--;
			if(time1.Seconds==255)
				time1.Seconds=59;		
		}
		
	}
	
	if(key.B4_long)  //B4长按
	{
		key.B4_long=0;
		if(time_select==0)
		{
			time1.Hours--;
			if(time1.Hours==255)
				time1.Hours=23;
		}
		else if(time_select==1)
		{
			time1.Minutes--;
			if(time1.Minutes==255)
				time1.Minutes=59;					
		}
		else
		{
			time1.Seconds--;
			if(time1.Seconds==255)
				time1.Seconds=59;		
		}
	}
}
else if(show_switch==2)   //设置报警时间
{
	if(key.B3_short)    //B3短按
	{
		key.B3_short=0;
		if(time_select==0)
		{
			time_alarm.AlarmTime.Hours++;
			if(time_alarm.AlarmTime.Hours==24)
				time_alarm.AlarmTime.Hours=0;
		}
		else if(time_select==1)
		{
			time_alarm.AlarmTime.Minutes++;
			if(time_alarm.AlarmTime.Minutes==60)
				time_alarm.AlarmTime.Minutes=0;					
		}
		else
		{
			time_alarm.AlarmTime.Seconds++;
			if(time_alarm.AlarmTime.Seconds==60)
				time_alarm.AlarmTime.Seconds=0;		
		}
		
	}
	
	if(key.B3_long)   //B3长按
	{
		key.B3_long=0;
		if(time_select==0)
		{
			time_alarm.AlarmTime.Hours++;
			if(time_alarm.AlarmTime.Hours==24)
				time_alarm.AlarmTime.Hours=0;
		}
		else if(time_select==1)
		{
			time_alarm.AlarmTime.Minutes++;
			if(time_alarm.AlarmTime.Minutes==60)
				time_alarm.AlarmTime.Minutes=0;					
		}
		else
		{
			time_alarm.AlarmTime.Seconds++;
			if(time_alarm.AlarmTime.Seconds==60)
				time_alarm.AlarmTime.Seconds=0;		
		}
		
	}
	
	if(key.B4_short)  //B4短按
	{
		key.B4_short=0;
		if(time_select==0)
		{
			time_alarm.AlarmTime.Hours--;
			if(time_alarm.AlarmTime.Hours==255)
				time_alarm.AlarmTime.Hours=23;
		}
		else if(time_select==1)
		{
			time_alarm.AlarmTime.Minutes--;
			if(time_alarm.AlarmTime.Minutes==255)
				time_alarm.AlarmTime.Minutes=59;					
		}
		else
		{
			time_alarm.AlarmTime.Seconds--;
			if(time_alarm.AlarmTime.Seconds==255)
				time_alarm.AlarmTime.Seconds=59;		
		}
		
	}
	
	if(key.B4_long)  //B4长按
	{
		key.B4_long=0;
		if(time_select==0)
		{
			time_alarm.AlarmTime.Hours--;
			if(time_alarm.AlarmTime.Hours==255)
				time_alarm.AlarmTime.Hours=23;
		}
		else if(time_select==1)
		{
			time_alarm.AlarmTime.Minutes--;
			if(time_alarm.AlarmTime.Minutes==255)
				time_alarm.AlarmTime.Minutes=59;					
		}
		else
		{
			time_alarm.AlarmTime.Seconds--;
			if(time_alarm.AlarmTime.Seconds==255)
				time_alarm.AlarmTime.Seconds=59;		
		}
	}
}

	





	
	
	B1_last_state=B1_state;
	B2_last_state=B2_state;
	B3_last_state=B3_state;
	B4_last_state=B4_state;
}
