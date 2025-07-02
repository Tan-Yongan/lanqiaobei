#include "headfile.h"

char text[20];

uint8_t B1_state;
uint8_t B2_state;
uint8_t B3_state;
uint8_t B4_state;
uint8_t B1_last_state=1;
uint8_t B2_last_state=1;
uint8_t B3_last_state=1;
uint8_t B4_last_state=1;

uint8_t pwm_switch;
uint8_t lcd_switch;

double CNBR = 3.5;
double VNBR = 2.0;

uint8_t cnbr = 0;
uint8_t vnbr = 0;

uint8_t IDLE = 8;

car in_car[8];

u8 temp_year;
u8 temp_month;
u8 temp_day;
u8 temp_hour;
u8 temp_min;
u8 temp_sec;

double hour_sum;
double fee;

void led_show(uint8_t led,uint8_t state)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET);
	if(state)
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1), GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1), GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET);
}

void lcd_show()
{
	if(!lcd_switch)
	{
		sprintf(text,"       Data        ");
		LCD_DisplayStringLine(Line1,(uint8_t *)text);	
		sprintf(text,"    CNBR:%d        ",cnbr);
		LCD_DisplayStringLine(Line3,(uint8_t *)text);	
		sprintf(text,"    VNBR:%d        ",vnbr);
		LCD_DisplayStringLine(Line5,(uint8_t *)text);	
		sprintf(text,"    IDLE:%d        ",IDLE);
		LCD_DisplayStringLine(Line7,(uint8_t *)text);	
	}
	else
	{
		sprintf(text,"       Para        ");
		LCD_DisplayStringLine(Line1,(uint8_t *)text);	
		sprintf(text,"   CNBR:%.2f       ",CNBR);
		LCD_DisplayStringLine(Line3,(uint8_t *)text);	
		sprintf(text,"   VNBR:%.2f       ",VNBR);
		LCD_DisplayStringLine(Line5,(uint8_t *)text);	
		sprintf(text,"                   ");
		LCD_DisplayStringLine(Line7,(uint8_t *)text);	
	}
	if(IDLE>0)
		led_show(1,1);
	else
		led_show(1,0);
}

uint32_t keyTick=0;
void key_scan()
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	
	B1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	B2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	B3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	B4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);

	if(!B1_state&&B1_last_state)    //B1
	{
		lcd_switch++;
		lcd_switch %= 2;
	}
	if(lcd_switch)               //在Para界面下
	{
		if(!B2_state&&B2_last_state)   //B2
		{
			CNBR += 0.5;
			VNBR += 0.5;
		}
		if(!B3_state&&B3_last_state)   //B2
		{
			CNBR -= 0.5;
			VNBR -= 0.5;
		}
	}
	
	if(!B4_state&&B4_last_state)    //B4
	{
		pwm_switch++;
		pwm_switch %= 2;
		if(pwm_switch)
		{
			TIM3->CCR2 = 20;
			led_show(2,1);
		}
		else
		{
			TIM3->CCR2 = 0;
			led_show(2,0);
		}
	}
		
	B1_last_state = B1_state;
	B2_last_state = B2_state;
	B3_last_state = B3_state;
	B4_last_state = B4_state;
}

uint8_t buff;
uint8_t rx_buff[50];
uint8_t rx_counter = 0;
uint8_t rx_flag = 0;

void clear_buff()
{
	rx_flag = 0;
	rx_counter = 0;
	memset(rx_buff,'\0',sizeof(rx_buff));   //处理完就已经另存了，缓冲区肯定要清理的
}

uint8_t isIN(uint8_t* str)  //判断车在不在库里
{
	for(u8 i=0;i<8;i++)   //循环8次，检查8个车位
	{
		if(in_car[i].in_id[0]==str[5]&&in_car[i].in_id[1]==str[6]&&in_car[i].in_id[2]==str[7]&&in_car[i].in_id[3]==str[8])
			return 1;       //重复编号，出库算账 1
	}
	return 0;
}

uint8_t CheakCmd(uint8_t* str)   //检查格式是否合法
{
	if(rx_counter != 22)
		return 0;
	if((str[0]=='V'||str[0]=='C')&&str[1]=='N'&&str[2]=='B'&&str[3]=='R'&&str[4]==':'&&str[9]==':')
	{
		for(u8 i=5;i<9;i++)
		{
			if(str[i]>='a'&&str[i]<='z')
				continue;
			if(str[i]>='A'&&str[i]<='Z')
				continue;
			if(str[i]>='0'&&str[i]<='9')
				continue;
			return 0;
		}
		for(u8 i=11;i<22;i++)
		{
			if(str[i]>='0'&&str[i]<='9')
				continue;
			return 0;		
		}
		temp_year = (str[10]-'0')*10+(str[11]-'0');
		temp_month = (str[12]-'0')*10+(str[13]-'0');
		temp_day = (str[14]-'0')*10+(str[15]-'0');
		temp_hour = (str[16]-'0')*10+(str[17]-'0');
		temp_min = (str[18]-'0')*10+(str[19]-'0');
		temp_sec = (str[20]-'0')*10+(str[21]-'0');
		if(temp_month>12||temp_day>31||temp_hour>23||temp_min>59||temp_sec>59)
			return 0;
		return 1;
	}
	return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		TIM2->CNT = 0;        //每次进来CNT都为0
		rx_flag = 1;
		rx_buff[rx_counter] = buff;
		HAL_UART_Receive_IT(huart,&buff,1);
		rx_counter++;
	}
}

void usart_process()
{
	if(rx_flag&&TIM2->CNT>15)  //不定长字符串接收完毕 ，也就是得到了一条字符串，可以在while循环处理了
	{
		if(CheakCmd(rx_buff))   //接收到的数据正确，那就开始处理
		{
			if(isIN(rx_buff))    //存在
			{		
				
				//printf("not1");
				u8 i;
				for(i = 0;i<(8-IDLE);i++)
				{
						if(in_car[i].in_id[0] == rx_buff[5] && in_car[i].in_id[1] == rx_buff[6] && in_car[i].in_id[2] == rx_buff[7] && in_car[i].in_id[3] == rx_buff[8]) //找到出库的车了
						{
							//计算费用
							hour_sum = (temp_year - in_car[i].in_year)*12*30*24 + (temp_month - in_car[i].in_month)*30*24 + 
							(temp_day - in_car[i].in_day)*24 + (temp_hour - in_car[i].in_hour)+(temp_min - in_car[i].in_min)/60.0f +
							(temp_sec - in_car[i].in_sec)/3600.0f;
							
							hour_sum = ceil(hour_sum);//向上取整
							
							if(hour_sum < 0)
							{
								printf("time error\r\n");
								clear_buff();
								return;
							}
								
							if(rx_buff[0] == 'C')
							{
								fee = hour_sum * CNBR;
								cnbr--;
							}
							if(rx_buff[0] == 'V')
							{
								fee = hour_sum * VNBR;
								vnbr--;
							}
							IDLE++;
							
							printf("%s:%s:%.0f:%5.2f\r\n",in_car[i].in_type,in_car[i].in_id,hour_sum,fee); //若不限制几位，则用.0f
							memset(&in_car[i],'\0' ,sizeof(in_car[i]));//清除该车位的信息
							
							clear_buff();
						}
					
				}
			}
			else      //不存在就存进来
			{
				if(IDLE>0)   //还有空车位
				{
					for(u8 i=0;i<4;i++)
					{
						in_car[8-IDLE].in_type[i] = rx_buff[i];	
						in_car[8-IDLE].in_id[i] = rx_buff[i+5];
					}
					in_car[8-IDLE].in_year = temp_year;
					in_car[8-IDLE].in_month = temp_month;
					in_car[8-IDLE].in_day = temp_day;
					in_car[8-IDLE].in_hour = temp_hour;
					in_car[8-IDLE].in_min = temp_min;
					in_car[8-IDLE].in_sec = temp_sec;
					in_car[8-IDLE].in_pos = 8-IDLE;
					
					if(rx_buff[0] == 'C')
						cnbr++;
					if(rx_buff[0] == 'V')
						vnbr++;
					
					IDLE --;
					
					
					clear_buff();   //处理完就清理
					
				}
			}
		}
		else   //接收到的数据格式不对，那就清空重来
		{
			printf("Form Error\r\n");
			clear_buff();
		}
	}
}

struct __FILE
{
  int handle;
  /* Whatever you require here. If the only file you are using is */
  /* standard output using printf() for debugging, no file handling */
  /* is required. */
};
/* FILE is typedef'd in stdio.h. */
FILE __stdout;
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 50);
  /* Your implementation of fputc(). */
  return ch;
}
