#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B1_last_state;
uint8_t B2_last_state;
uint8_t B3_last_state;

uint8_t show_switch,flag=1,err_flag,err_flag1;

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	
	if(B1_state==0&&B1_last_state==1)//B1����
	{
		show_switch++;
		show_switch=show_switch%2;
	}

	
	if(show_switch)  //���ν���
	{
		err_flag=0;
		if(flag)
		{
			flag=0;
			vmax_last=vmax;
			vmin_last=vmin;
		}
		
		if(B2_state==0&&B2_last_state==1)//B2����
		{
			vmax+=0.1;
			if(vmax>3.35)
				vmax=0;
		}
		
		if(B3_state==0&&B3_last_state==1)//B3����
		{
			vmin+=0.1;
			if(vmin>3.35)
				vmin=0;		
		}
	}
	else     //��ʾ����
	{
		if(!(vmax>=(vmin+1)))
		{
			err_flag=1;
			vmax = vmax_last;
		  vmin = vmin_last;
		}
		if(err_flag)
		{
			led_show(2,1);
		}
		else
		{
			led_show(2,0);
		}
		
		flag=1;
	}
	
	if(err_flag1)
	{
		led_show(3,1);
	}
	else
	{
		led_show(3,0);		
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
}
