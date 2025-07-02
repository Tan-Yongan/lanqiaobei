#include "headfile.h"

uint8_t ld1_count;
uint8_t set_time_number=0,set_alarm_number=0;
uint8_t show_switch=0;
char text[20];

RTC_TimeTypeDef time,time1;
RTC_AlarmTypeDef time_alarm={{12,0,0}};
RTC_DateTypeDef data;

void alarm_init()
{
	if(eeprom_read(3)!=255&&eeprom_read(4)!=255&&eeprom_read(5)!=255)
	{
		time_alarm.AlarmTime.Hours=eeprom_read(3);
	  time_alarm.AlarmTime.Minutes=eeprom_read(4);
		time_alarm.AlarmTime.Seconds=eeprom_read(5);
		HAL_RTC_SetAlarm(&hrtc,&time_alarm,RTC_FORMAT_BIN);
	}
	set_time_number=eeprom_read(1);
	set_alarm_number=eeprom_read(2);
}
void lcd_show()
{
	HAL_RTC_GetTime(&hrtc,&time,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&data,RTC_FORMAT_BIN);
	
	if(show_switch==0)  //主界面
	{
		sprintf(text,"       MAIN        ");
		LCD_DisplayStringLine(Line0,(uint8_t *)text);
		sprintf(text,"    RTC:%02d:%02d:%02d ",time.Hours,time.Minutes,time.Seconds);   //注意LCD显示最多20个空格 多了会出错
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		time1=time;
		led_show(1,ld1_count);
		led_show(2,0);
		led_show(3,0);
	}
	else if(show_switch==1)  //时钟设置界面
	{
		sprintf(text,"    RTC-SETTING");
		LCD_DisplayStringLine(Line0,(uint8_t *)text);
		sprintf(text,"    RTC:");
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		if(time_select==0)
		{
			LCD_SetTextColor(Red);
			LCD_DisplayChar(Line3,319-16*8,time1.Hours/10+'0');
			LCD_DisplayChar(Line3,319-16*9,time1.Hours%10+'0');
			LCD_SetTextColor(White);
			LCD_DisplayChar(Line3,319-16*10,':');
			LCD_DisplayChar(Line3,319-16*11,time1.Minutes/10+'0');
			LCD_DisplayChar(Line3,319-16*12,time1.Minutes%10+'0');
			LCD_DisplayChar(Line3,319-16*13,':');
			LCD_DisplayChar(Line3,319-16*14,time1.Seconds/10+'0');
			LCD_DisplayChar(Line3,319-16*15,time1.Seconds%10+'0');
		}
		else if(time_select==1)
		{
			LCD_DisplayChar(Line3,319-16*8,time1.Hours/10+'0');
			LCD_DisplayChar(Line3,319-16*9,time1.Hours%10+'0');
			LCD_DisplayChar(Line3,319-16*10,':');
			LCD_SetTextColor(Red);
			LCD_DisplayChar(Line3,319-16*11,time1.Minutes/10+'0');
			LCD_DisplayChar(Line3,319-16*12,time1.Minutes%10+'0');
			LCD_SetTextColor(White);
			LCD_DisplayChar(Line3,319-16*13,':');
			LCD_DisplayChar(Line3,319-16*14,time1.Seconds/10+'0');
			LCD_DisplayChar(Line3,319-16*15,time1.Seconds%10+'0');
		}
		else
		{
			LCD_DisplayChar(Line3,319-16*8,time1.Hours/10+'0');
			LCD_DisplayChar(Line3,319-16*9,time1.Hours%10+'0');
			LCD_DisplayChar(Line3,319-16*10,':');
			LCD_DisplayChar(Line3,319-16*11,time1.Minutes/10+'0');
			LCD_DisplayChar(Line3,319-16*12,time1.Minutes%10+'0');
			LCD_DisplayChar(Line3,319-16*13,':');
			LCD_SetTextColor(Red);
			LCD_DisplayChar(Line3,319-16*14,time1.Seconds/10+'0');
			LCD_DisplayChar(Line3,319-16*15,time1.Seconds%10+'0');
			LCD_SetTextColor(White);
		}
		led_show(1,0);
		led_show(2,1);
		led_show(3,0);
	}
	else  //报警设置界面
	{
		sprintf(text,"    ALARM-SETTING");
		LCD_DisplayStringLine(Line0,(uint8_t *)text);
		sprintf(text,"  Alarm:");
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		if(time_select==0)
		{
			LCD_SetTextColor(Red);
			LCD_DisplayChar(Line3,319-16*8,time_alarm.AlarmTime.Hours/10+'0');
			LCD_DisplayChar(Line3,319-16*9,time_alarm.AlarmTime.Hours%10+'0');
			LCD_SetTextColor(White);
			LCD_DisplayChar(Line3,319-16*10,':');
			LCD_DisplayChar(Line3,319-16*11,time_alarm.AlarmTime.Minutes/10+'0');
			LCD_DisplayChar(Line3,319-16*12,time_alarm.AlarmTime.Minutes%10+'0');
			LCD_DisplayChar(Line3,319-16*13,':');
			LCD_DisplayChar(Line3,319-16*14,time_alarm.AlarmTime.Seconds/10+'0');
			LCD_DisplayChar(Line3,319-16*15,time_alarm.AlarmTime.Seconds%10+'0');
		}
		else if(time_select==1)
		{
			LCD_DisplayChar(Line3,319-16*8,time_alarm.AlarmTime.Hours/10+'0');
			LCD_DisplayChar(Line3,319-16*9,time_alarm.AlarmTime.Hours%10+'0');
			LCD_DisplayChar(Line3,319-16*10,':');
			LCD_SetTextColor(Red);
			LCD_DisplayChar(Line3,319-16*11,time_alarm.AlarmTime.Minutes/10+'0');
			LCD_DisplayChar(Line3,319-16*12,time_alarm.AlarmTime.Minutes%10+'0');
			LCD_SetTextColor(White);
			LCD_DisplayChar(Line3,319-16*13,':');
			LCD_DisplayChar(Line3,319-16*14,time_alarm.AlarmTime.Seconds/10+'0');
			LCD_DisplayChar(Line3,319-16*15,time_alarm.AlarmTime.Seconds%10+'0');
		}
		else
		{
			LCD_DisplayChar(Line3,319-16*8,time_alarm.AlarmTime.Hours/10+'0');
			LCD_DisplayChar(Line3,319-16*9,time_alarm.AlarmTime.Hours%10+'0');
			LCD_DisplayChar(Line3,319-16*10,':');
			LCD_DisplayChar(Line3,319-16*11,time_alarm.AlarmTime.Minutes/10+'0');
			LCD_DisplayChar(Line3,319-16*12,time_alarm.AlarmTime.Minutes%10+'0');
			LCD_DisplayChar(Line3,319-16*13,':');
			LCD_SetTextColor(Red);
			LCD_DisplayChar(Line3,319-16*14,time_alarm.AlarmTime.Seconds/10+'0');
			LCD_DisplayChar(Line3,319-16*15,time_alarm.AlarmTime.Seconds%10+'0');
			LCD_SetTextColor(White);
		}
		led_show(1,0);
		led_show(2,0);
		led_show(3,1);
	}
	
	
	
	//
}
