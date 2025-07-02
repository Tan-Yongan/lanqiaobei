#include "headfile.h"

uint8_t pw_flag=1;  //置1表示在密码输入界面

int pw[3]={1,2,3};
int pw_wait[3]={-1,-1,-1};

uint8_t pw_err_counter;

void key_scan(void)
{
	if(pw_flag)
	{
		if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0))  //B1
		{
			HAL_Delay(20);
			while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0));
			HAL_Delay(20);
			pw_init_flag[0]=0;
			pw_wait[0]++;
			if(pw_wait[0]>9)	pw_wait[0]=0;
		}
		
		if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1))  //B2
		{
			HAL_Delay(20);
			while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1));
			HAL_Delay(20);
			pw_init_flag[1]=0;
			pw_wait[1]++;
			if(pw_wait[1]>9)	pw_wait[1]=0;
		}
		
		if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2))  //B3
		{
			HAL_Delay(20);
			while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2));
			HAL_Delay(20);
			pw_init_flag[2]=0;
			pw_wait[2]++;
			if(pw_wait[2]>9)	pw_wait[2]=0;
		}
		
		if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))  //B4
		{
			HAL_Delay(20);
			while(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0));
			HAL_Delay(20);
			if(pw_wait[0]==pw[0]&&pw_wait[1]==pw[1]&&pw_wait[2]==pw[2])
			{
				pw_flag=0;
				led1_counter=0;
				led1_work_flag=1;
				state_sta();
			}
			else
			{
				pw_err_counter++;
				if(pw_err_counter>=3)
				{
					pw_err_counter=0;
					led2_counter=0;
					led2_work_flag=1;
				}
			}
		}
	}
}
