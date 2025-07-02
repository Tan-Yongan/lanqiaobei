#include "headfile.h"

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

uint32_t B1Tick;
uint32_t B2Tick;
uint32_t B3Tick;
uint32_t B4Tick;

uint8_t B1_click_time;
uint8_t B4_click_flag;

void key_scan()
{
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
	if(!B1_state&&B1_last_state)
	{
		if(!B1_click_time)  //��һ�ΰ���
		{
			B1Tick = uwTick;
			B1_click_time = 1;
		}
		else
		{
			B1_click_time ++;
			if(B1_click_time == 3 && uwTick - B1Tick < 400)
			{
				count+=100;
				B1_click_time = 0;
			}
		}
	}
	if(B1_click_time == 1 && uwTick - B1Tick > 400)
	{
		count+=1;
		B1_click_time = 0;
	}
	if(B1_click_time == 2 && uwTick - B1Tick > 400)
	{
		count+=2;
		B1_click_time = 0;
	}
	
	if(!B2_state&&B2_last_state)  //��������˲��
	{
		//��ʼ��ʱ
		B2Tick = uwTick;
	}
	else if(B2_state&&!B2_last_state)  //�����ɿ�˲��
	{
		if(uwTick - B2Tick > 1000)  //����  ����1s
		{
			led_show(8,1);
		}
		else    //�̰�
		{
			led_show(8,0);
		}
	}
	
	if(!B3_state&&B3_last_state) //����˲��
	{
		B3Tick = uwTick;  //��ʼ��ʱ
	}
	else if(!B3_state&&!B3_last_state)  //��ס������
	{
		if(uwTick - B3Tick >= 1000)   //��ס�����ֳ���һ����
		{
			count-=5;
		}
	}
	else if(B3_state&&!B3_last_state)  //����˲��
	{
		if(uwTick - B3Tick <1000)
		{
			count = 0;
		}
	}
	
	
	if(!B4_state&&B4_last_state)
	{
		if(!B4_click_flag)  //�����һ�ΰ��¾Ϳ�ʼ��ʱ
		{
			B4Tick = uwTick;
			B4_click_flag = 1;
		}
		else
		{
			if(uwTick - B4Tick < 250) //˫��
			{
				count+=10;
				B4_click_flag = 0;
			}
		}
	}
	if(B4_click_flag && uwTick - B4Tick > 250)
	{
		count-=10;
		B4_click_flag  = 0;
	}
	
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}
