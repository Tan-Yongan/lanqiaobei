#include "headfile.h"

char text[20];

RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};

uint8_t LED1_mode;

void lcd_show()
{
	HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
	sprintf(text,"        DATE        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"  %2d:%2d:%2dWeek%d       ",sDate.Year,sDate.Month,sDate.Date,sDate.WeekDay);	
	LCD_DisplayStringLine(Line2,(uint8_t *)text);
	sprintf(text,"        TIME        ");
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	sprintf(text,"  %2d:%2d:%2d        ",sTime.Hours,sTime.Minutes,sTime.Seconds);	
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
	
	led_show(1,LED1_mode);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)   //闹钟时间到执行的操作
{
	LED1_mode = 1;
}
