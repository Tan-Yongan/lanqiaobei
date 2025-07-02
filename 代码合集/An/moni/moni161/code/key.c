#include "headfile.h"

uint8_t B1_state;
uint8_t B1_last_state=1;

uint8_t show_switch;
uint8_t code_switch;
uint8_t pwd_in[3] = {3,3,3};
uint8_t pwd_real[3];

uint8_t enter_flag;
uint8_t start_flag;
uint8_t temp_p1;

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{		
		if(show_switch == 0)
		{
			code_switch++;
			if(code_switch == 3)
			{
				enter_flag = 1;
				code_switch = 0;
			}
		}
		else
		{
			code_switch++;
			if(code_switch == 3)
			{
				code_switch = 0;
				show_switch = 0;
				eeprom_write(0x00,pwd_real[0]);
				eeprom_write(0x01,pwd_real[1]);
				eeprom_write(0x02,pwd_real[2]);	
				pwd_in[0] = 3;
				pwd_in[1] = 3;
				pwd_in[2] = 3;
				
			}
		}
	}
	
	if(enter_flag)
	{
		if(pwd_in[0]==pwd_real[0]&&pwd_in[1]==pwd_real[1]&&pwd_in[2]==pwd_real[2])  //密码正确
		{
			led_show(1,1);
			show_switch = 1;
			pwd_real[0] = 3;
			pwd_real[1] = 3;
			pwd_real[2] = 3;
			start_flag = 1;    //密码正确开始计时三秒
		}
		else
		{
			pwd_in[0] = 3;
			pwd_in[1] = 3;
			pwd_in[2] = 3;
		}
		enter_flag = 0;
	}
	
	B1_last_state = B1_state;
}