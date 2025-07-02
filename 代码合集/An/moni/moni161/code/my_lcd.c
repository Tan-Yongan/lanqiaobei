#include "headfile.h"

char text[20];


void lock_show()
{
	LCD_DisplayChar(Line4,13*16,' ');
	LCD_DisplayChar(Line4,11*16,' ');
	LCD_DisplayChar(Line4,9*16,' ');
	sprintf(text,"        Lock       ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"      Pass Word    ");
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	if(code_switch == 0)
	{
		if(get_vol()<1.5) 
			pwd_in[0] = 0;
		else if(get_vol()>2.5)
			pwd_in[0] = 2;
		else
			pwd_in[0] = 1;		
	}
	if(code_switch == 1)
	{
		if(get_vol()<1.5) 
			pwd_in[1] = 0;
		else if(get_vol()>2.5)
			pwd_in[1] = 2;
		else
			pwd_in[1] = 1;		
	}
	if(code_switch == 2)
	{
		if(get_vol()<1.5) 
			pwd_in[2] = 0;
		else if(get_vol()>2.5)
			pwd_in[2] = 2;
		else
			pwd_in[2] = 1;		
	}
	
	if(pwd_in[0] == 3)
		LCD_DisplayChar(Line4,12*16,'*');
	else
		LCD_DisplayChar(Line4,12*16,pwd_in[0]+'0');
	if(pwd_in[1] == 3)
		LCD_DisplayChar(Line4,10*16,'*');
	else
		LCD_DisplayChar(Line4,10*16,pwd_in[1]+'0');
	if(pwd_in[2] == 3)
		LCD_DisplayChar(Line4,8*16,'*');
	else
		LCD_DisplayChar(Line4,8*16,pwd_in[2]+'0');
}

void set_show()
{
	LCD_DisplayChar(Line4,12*16,' ');
	LCD_DisplayChar(Line4,10*16,' ');
	LCD_DisplayChar(Line4,8*16,' ');
	
	sprintf(text,"        Set        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"       Change      ");
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	if(code_switch == 0)
	{
		if(get_vol()<1.5) 
			pwd_real[0] = 0;
		else if(get_vol()>2.5)
			pwd_real[0] = 2;
		else
			pwd_real[0] = 1;
			
		if(pwd_real[0]!=pwd_in[2])
		{
			start_flag = 0;
			timer_3S = 0;
		}
	}
	if(code_switch == 1)
	{
		start_flag = 0;
		timer_3S = 0;
		if(get_vol()<1.5) 
			pwd_real[1] = 0;
		else if(get_vol()>2.5)
			pwd_real[1] = 2;
		else
			pwd_real[1] = 1;		
	}
	if(code_switch == 2)
	{
		if(get_vol()<1.5) 
			pwd_real[2] = 0;
		else if(get_vol()>2.5)
			pwd_real[2] = 2;
		else
			pwd_real[2] = 1;		
	}
	
	if(pwd_real[0] == 3)
		LCD_DisplayChar(Line4,13*16,'*');
	else
		LCD_DisplayChar(Line4,13*16,pwd_real[0]+'0');
	if(pwd_real[1] == 3)
		LCD_DisplayChar(Line4,11*16,'*');
	else
		LCD_DisplayChar(Line4,11*16,pwd_real[1]+'0');
	if(pwd_real[2] == 3)
		LCD_DisplayChar(Line4,9*16,'*');
	else
		LCD_DisplayChar(Line4,9*16,pwd_real[2]+'0');
}
	
void lcd_show()
{
	switch(show_switch)
	{
		case 0:
			lock_show();
			break;
		case 1:
			set_show();
			break;
		default:
			break;
	}
	
//	sprintf(text," %d     ",eeprom_read(0x03));
//	LCD_DisplayStringLine(Line6,(uint8_t *)text);
//	sprintf(text," pwd_in  =%d%d%d      ",pwd_in[0],pwd_in[1],pwd_in[2]);
//	LCD_DisplayStringLine(Line7,(uint8_t *)text);
//	sprintf(text," pwd_real=%d%d%d      ",pwd_real[0],pwd_real[1],pwd_real[2]);
//	LCD_DisplayStringLine(Line8,(uint8_t *)text);
//	sprintf(text,"     %.2f      ",get_vol());
//	LCD_DisplayStringLine(Line9,(uint8_t *)text);
//	
//	sprintf(text,"        A");
//	LCD_DisplayStringLine(Line0,(uint8_t *)text);
	
	(show_switch == 0)?led_show(1,1):led_show(1,0);
	(show_switch == 1)?led_show(2,1):led_show(2,0);
}