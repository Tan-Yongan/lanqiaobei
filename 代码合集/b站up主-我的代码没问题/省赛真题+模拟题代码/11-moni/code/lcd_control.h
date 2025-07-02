#ifndef _lcd_control_h
#define _lcd_control_h
#include "headfile.h"

void lcd_show(void);
void alarm_init(void);
extern uint8_t ld1_count;
extern uint8_t show_switch,set_time_number,set_alarm_number;
extern RTC_TimeTypeDef time,time1;
extern RTC_AlarmTypeDef time_alarm;
extern RTC_DateTypeDef data;
#endif
