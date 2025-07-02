/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "key.h"
#include "stdio.h"
#include "string.h"
#include "math.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define Display_DATA 0
#define Display_PARA 1

u8 Display_Mode = 0;




//1

double a = 120.4;
double b = 42.8;


_Bool max_flag = 0;
u8 a_index = 0;//考虑数据存满之前和存满之后
u8 b_index = 0;
u8 index_cnt = 0;
double a_buf[6];  //1 - 5分别存5个数据0
double b_buf[6];


double qa_buf[6];  //1 - 5分别存5个数
double qb_buf[6];

int ax = 0;
int bx = 0;

_Bool mode = 0; // 0A  ; 1B


//2

u8 Pax = 20;
u8 Pbx = 20;
int Pf = 1000;



extern double PA6_D;
extern double PA7_D;
extern double PA4_V_temp;
extern uint32_t PA1_F;


//LED
u8 led_ctrl = 0x00;
//c为角度
double c;
void Led_Proc(void)
{
	
	if(ax > Pax)
		led_ctrl |= 0x01;
	else
		led_ctrl&=~0x01;
	
	if(bx > Pbx)
		led_ctrl |= 0x02;
	else
		led_ctrl&=~0x02;
	
  if(PA1_F > Pf)
		led_ctrl |= 0x04;
	else
		led_ctrl &= ~0x04;
	
	if(mode == 0)
		led_ctrl|= 0x08;
	else
		led_ctrl&=~0x08;
	
	//LED5
	if(a <= 90)
	{
		c = 90 - a + b;
	}else
	{
		c = 90-(180-a);
	}
	
	if(c < 10)
		led_ctrl |= 0x10;
	else
		led_ctrl &= ~0x10;
	
	
	
	
	
	
	Led_Control(led_ctrl);
}



//KEY
uint32_t keyTick = 0;
void Key_Proc(void)
{
	if(uwTick - keyTick < 20)return;
	keyTick = uwTick;
	
  Key_Read();
	
	if(Trg & 0x01)
	{
		LCD_Clear(Black);
		Display_Mode = (Display_Mode + 1)%2;
	}

	if(Trg & 0x02)
	{
		if(Display_Mode == Display_PARA)
		{
			if(Pax < 60)
				Pax+=10;
			else
				Pax = 10;
			
			if(Pbx < 60)
				Pbx+=10;
			else
				Pbx = 10;
		}
	}
	
	if(Trg & 0x04)
	{
		if(Display_Mode == Display_PARA)
		{
			if(Pf < 10000)
				Pf += 1000;
			else
				Pf = 1000;
		}
		
		if(Display_Mode == Display_DATA)
		{
			mode = !mode;
			if(mode == 1)
				PA4_V_temp = 3.3;
		}
	}
	
	if(Trg & 0x08)
	{
		if(mode == 0)
		{
			if(PA6_D <= 0.1)
				a = 0;
		  else if(PA6_D > 0.1 && PA6_D <= 0.9)
			  a = 180/0.8 *(PA6_D - 0.1);
			else
				a = 180;
			
			
			if(PA7_D <= 0.1)
				b = 0;
		  else if(PA7_D > 0.1 && PA7_D <= 0.9)
		    b = 90/0.8 *(PA7_D - 0.1);
			else
				b = 90;
			
			u8 i;
			for(i = 4;i>0;i--)
			{
				a_buf[i+1] = a_buf[i];
			}
			
			for(i = 4;i>0;i--)
			{
				b_buf[i+1] = b_buf[i];
			}
			
			a_buf[1] = a;
			b_buf[1] = b;
			
			ax = fabs(a_buf[1] - a_buf[2]); 
			bx = fabs(b_buf[1] - b_buf[2]);

//			
//			if(a_index == 5)
//			{
//				a_index = 0;
//				b_index = 0;
//				max_flag = 1;//存满了  //方法一 标志位,不如操作数组
//				//方法二：存满后再存，数组先轮流替换,根据题目，存数时间顺序
//				
//				
//			}
			
		}
	}

}



//PA1  PA6  PA7
uint32_t uwIC2Value1_PA1 = 0;
uint32_t uwIC2Value2_PA1 = 0;
uint32_t uwDiffCapture_PA1 = 0;

/* Capture index */
uint16_t uhCaptureIndex_PA1 = 0;

/* Frequency Value */

uint32_t PA1_F;


//PA6
uint32_t uwIC2Value1_PA6 = 0;
uint32_t uwIC2Value2_PA6 = 0;
uint32_t low_PA6,high_PA6;

/* Capture index */
uint16_t uhCaptureIndex_PA6 = 0;

/* Frequency Value */

double PA6_D;


//PA7

uint32_t uwIC2Value1_PA7 = 0;
uint32_t uwIC2Value2_PA7 = 0;
uint32_t low_PA7,high_PA7;

/* Capture index */
uint16_t uhCaptureIndex_PA7 = 0;

/* Frequency Value */

double PA7_D;



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
			if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
			{
				if(uhCaptureIndex_PA1 == 0)
				{
					/* Get the 1st Input Capture value */
					uwIC2Value1_PA1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
					uhCaptureIndex_PA1 = 1;
				}
				else if(uhCaptureIndex_PA1 == 1)
				{
					/* Get the 2nd Input Capture value */
					uwIC2Value2_PA1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); 

					/* Capture computation */
					if (uwIC2Value2_PA1 > uwIC2Value1_PA1)
					{
						uwDiffCapture_PA1 = (uwIC2Value2_PA1 - uwIC2Value1_PA1); 
					}
					else if (uwIC2Value2_PA1 < uwIC2Value1_PA1)
					{
						/* 0xFFFF is max TIM1_CCRx value */
						uwDiffCapture_PA1 = ((0xFFFFFFFF - uwIC2Value1_PA1) + uwIC2Value2_PA1) + 1;
					}
					else
					{
						/* If capture values are equal, we have reached the limit of frequency
							 measures */
						Error_Handler();
					}

					/* Frequency computation: for this example TIMx (TIM1) is clocked by
						 APB2Clk */      
					PA1_F = 1e6/uwDiffCapture_PA1;
					uhCaptureIndex_PA1 = 0;
				}
			}
	
	}
	
	if(htim == &htim16)
	{
			if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			{
				if(uhCaptureIndex_PA6 == 0)
				{
					/* Get the 1st Input Capture value */
					uwIC2Value1_PA6 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
					__HAL_TIM_SET_CAPTUREPOLARITY(&htim16,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
					uhCaptureIndex_PA6 = 1;
				}
				else if(uhCaptureIndex_PA6 == 1)
				{
					/* Get the 2nd Input Capture value */
					uwIC2Value2_PA6 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
					__HAL_TIM_SET_CAPTUREPOLARITY(&htim16,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
					/* Capture computation */
					if (uwIC2Value2_PA6 > uwIC2Value1_PA6)
					{
						high_PA6 = (uwIC2Value2_PA6 - uwIC2Value1_PA6); 
					}
					else if (uwIC2Value2_PA6 < uwIC2Value1_PA6)
					{
						/* 0xFFFF is max TIM1_CCRx value */
						high_PA6 = ((0xFFFF - uwIC2Value1_PA6) + uwIC2Value2_PA6) + 1;
					}
					else
					{
						/* If capture values are equal, we have reached the limit of frequency
							 measures */
						Error_Handler();
					}

					/* Frequency computation: for this example TIMx (TIM1) is clocked by
						 APB2Clk */      
					uwIC2Value1_PA6 = uwIC2Value2_PA6;
					uhCaptureIndex_PA6 = 2;
				}
				else if(uhCaptureIndex_PA6 == 2)
				{
					/* Get the 2nd Input Capture value */
					uwIC2Value2_PA6 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
					/* Capture computation */
					if (uwIC2Value2_PA6 > uwIC2Value1_PA6)
					{
						low_PA6 = (uwIC2Value2_PA6 - uwIC2Value1_PA6); 
					}
					else if (uwIC2Value2_PA6 < uwIC2Value1_PA6)
					{
						/* 0xFFFF is max TIM1_CCRx value */
						low_PA6 = ((0xFFFF - uwIC2Value1_PA6) + uwIC2Value2_PA6) + 1;
					}
					else
					{
						/* If capture values are equal, we have reached the limit of frequency
							 measures */
						Error_Handler();
					}

					/* Frequency computation: for this example TIMx (TIM1) is clocked by
						 APB2Clk */      
					
					PA6_D = high_PA6*1.0/(low_PA6 + high_PA6);
					uhCaptureIndex_PA6 =  0;
				}
				
				
			}
	
	}
	
	if(htim == &htim17)
	{
					if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
					{
						if(uhCaptureIndex_PA7 == 0)
						{
							/* Get the 1st Input Capture value */
							uwIC2Value1_PA7 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
							__HAL_TIM_SET_CAPTUREPOLARITY(&htim17,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
							uhCaptureIndex_PA7 = 1;
						}
						else if(uhCaptureIndex_PA7 == 1)
						{
							/* Get the 2nd Input Capture value */
							uwIC2Value2_PA7 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
							__HAL_TIM_SET_CAPTUREPOLARITY(&htim17,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
							/* Capture computation */
							if (uwIC2Value2_PA7 > uwIC2Value1_PA7)
							{
								high_PA7 = (uwIC2Value2_PA7 - uwIC2Value1_PA7); 
							}
							else if (uwIC2Value2_PA7 < uwIC2Value1_PA7)
							{
								/* 0xFFFF is max TIM1_CCRx value */
								high_PA7 = ((0xFFFF - uwIC2Value1_PA7) + uwIC2Value2_PA7) + 1;
							}
							else
							{
								/* If capture values are equal, we have reached the limit of frequency
									 measures */
								Error_Handler();
							}

							/* Frequency computation: for this example TIMx (TIM1) is clocked by
								 APB2Clk */      
							uwIC2Value1_PA7 = uwIC2Value2_PA7;
							uhCaptureIndex_PA7 = 2;
						}
						else if(uhCaptureIndex_PA7 == 2)
						{
							/* Get the 2nd Input Capture value */
							uwIC2Value2_PA7 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
							/* Capture computation */
							if (uwIC2Value2_PA7 > uwIC2Value1_PA7)
							{
								low_PA7 = (uwIC2Value2_PA7 - uwIC2Value1_PA7); 
							}
							else if (uwIC2Value2_PA7 < uwIC2Value1_PA7)
							{
								/* 0xFFFF is max TIM1_CCRx value */
								low_PA7 = ((0xFFFF - uwIC2Value1_PA7) + uwIC2Value2_PA7) + 1;
							}
							else
							{
								/* If capture values are equal, we have reached the limit of frequency
									 measures */
								Error_Handler();
							}

							/* Frequency computation: for this example TIMx (TIM1) is clocked by
								 APB2Clk */      
							
							PA7_D = high_PA7*1.0/(low_PA7 + high_PA7);
							uhCaptureIndex_PA7 =  0;
						}
						
					}

	}
	
}
	

//PA4
double PA4_V;
void ADC_Proc(void)
{
	HAL_ADC_Start(&hadc2);
	PA4_V = HAL_ADC_GetValue(&hadc2)*3.3/4095;
}


//自动刷新
//亮变暗时候，遮挡光敏电阻时候,电压变大
double PA4_V_temp = 3.3;
uint32_t atTick = 0;
void Auto_Proc(void)
{
	if(uwTick - atTick < 100) return;
	atTick = uwTick;
	
	if(mode == 1) //考虑刚切换时的值改变
	{
		if(PA4_V_temp < (PA4_V - 0.5))  //注意设定一个阈值 
		{
				if(PA6_D <= 0.1)
					a = 0;
				else if(PA6_D > 0.1 && PA6_D <= 0.9)
					a = 180/0.8 *(PA6_D - 0.1);
				else
					a = 180;
				
				
				if(PA7_D <= 0.1)
					b = 0;
				else if(PA7_D > 0.1 && PA7_D <= 0.9)
					b = 90/0.8 *(PA7_D - 0.1);
				else
					b = 90;
				
				u8 i;
				for(i = 4;i>0;i--)
				{
					a_buf[i+1] = a_buf[i];
				}

				for(i = 4;i>0;i--)
				{
					b_buf[i+1] = b_buf[i];
				}
				
				a_buf[1] = a;
				b_buf[1] = b;
				
				ax = fabs(a_buf[1] - a_buf[2]); 
				bx = fabs(b_buf[1] - b_buf[2]);
		}
		PA4_V_temp = PA4_V;
	}
	
}

//串口
//发送
u8 tx_buf[100] = {"你好hello!\r\n"};

int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	
	HAL_UART_Transmit(&huart1,(u8 *)&ch,1,50);
  return ch;
}


//接受
u8 uart_buf[2];
u8 rx_buf[100];
u8 rx_cnt = 0;

uint32_t uartTick = 0;
void Rx_Idle(void)
{
	if(uwTick - uartTick < 50) return;
	uartTick = uwTick;
	
	if(rx_cnt > 0)
	{
		
		if(rx_buf[0] == 'a' && rx_buf[1] == '?')
		{
			printf("a:%.1f\r\n",a);
			
		}
		
		else if(rx_buf[0] == 'b' && rx_buf[1] == '?')
		{
			printf("b:%.1f\r\n",b);
			
		}
		
		else if(rx_buf[0] == 'a' && rx_buf[1] == 'a' && rx_buf[2] == '?')
		{
			printf("aa:%.1f-%.1f-%.1f-%.1f-%.1f\r\n",a_buf[1],a_buf[2],a_buf[3],a_buf[4],a_buf[5]);
		
		}
		
		else if(rx_buf[0] == 'b' && rx_buf[1] == 'b' && rx_buf[2] == '?')
		{
			printf("bb:%.1f-%.1f-%.1f-%.1f-%.1f\r\n",b_buf[1],b_buf[2],b_buf[3],b_buf[4],b_buf[5]);
		
		}
		
		else if(rx_buf[0] == 'q' && rx_buf[1] == 'a' && rx_buf[2] == '?')
		{
			u8 i;
			for(i = 0;i<6;i++)
			{
				qa_buf[i] = a_buf[i];
			}

		  //排序//冒泡//每次从第一个元素开始比较
		  u8 j,k;
		  double t;
			for(j = 1;j< 5;j++) //5个数，比较4次
			{
				for(k = 1;k< 6-j;k++) //最后一次比较 a[4] 和a[5] ;k = 4;因此k < 5
				{
					if(qa_buf[k] > qa_buf[k+1])
					{
						t = qa_buf[k];
						qa_buf[k] = qa_buf[k+1];
						qa_buf[k+1] = t; 
					}
				}
			}
	   printf("qa:%.1f-%.1f-%.1f-%.1f-%.1f\r\n",qa_buf[1],qa_buf[2],qa_buf[3],qa_buf[4],qa_buf[5]);
			
	}
		
		else if(rx_buf[0] == 'q' && rx_buf[1] == 'b' && rx_buf[2] == '?')
		{
			u8 i;
			for(i = 0;i<6;i++)
			{
				qb_buf[i] = b_buf[i];
			}
			
		  //排序//冒泡
		 u8 j,k;
		 double t;
			//a[k] 1 - 5共 5 个数
			for(j = 1;j< 5;j++) //5个数，比较4次
			{
				for(k = 1;k<6 - j;k++) //最后一次比较 a[4] 和a[5] ;k = 4;
				{
					if(qb_buf[k] > qb_buf[k+1])
					{
						t = qb_buf[k];
						qb_buf[k] = qb_buf[k+1];
						qb_buf[k+1] = t; 
					}
				}
			}
			
			printf("qb:%.1f-%.1f-%.1f-%.1f-%.1f\r\n",qb_buf[1],qb_buf[2],qb_buf[3],qb_buf[4],qb_buf[5]);
		}
		else
		{
			printf("error\r\n");
		}
		
		
		rx_cnt = 0;
		memset(rx_buf,'\0',sizeof(rx_buf));
	}
	
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartTick = uwTick;
	HAL_UART_Receive_IT(&huart1,uart_buf,1);
  rx_buf[rx_cnt++] = uart_buf[0];
	
	
	
}




//LCD
uint32_t lcdTicK = 0;

void LCD_Proc(void)
{
	if(uwTick - lcdTicK < 100) return;
	lcdTicK = uwTick;
	
	ADC_Proc();
	
	u8 display_buf[30];
	
	if(Display_Mode == Display_DATA)
	{
		sprintf((char*)display_buf,"        DATA ");
		LCD_DisplayStringLine(Line1,display_buf);

		sprintf((char*)display_buf,"   a:%.1f %4.2f   ",a,PA4_V);
		LCD_DisplayStringLine(Line2,display_buf);
		
		sprintf((char*)display_buf,"   b:%.1f      ",b);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char*)display_buf,"   f:%dHz       ",PA1_F);
		LCD_DisplayStringLine(Line4,display_buf);
		
		sprintf((char*)display_buf,"   ax:%d       ",ax);
		LCD_DisplayStringLine(Line6,display_buf);
		
		sprintf((char*)display_buf,"   bx:%d       ",bx);
		LCD_DisplayStringLine(Line7,display_buf);
				if(mode == 0)
			sprintf((char*)display_buf,"   mode:A ");
		else
			sprintf((char*)display_buf,"   mode:B ");
		LCD_DisplayStringLine(Line8,display_buf);
	}
	
	if(Display_Mode == Display_PARA)
	{
		sprintf((char*)display_buf,"        PARA ");
		LCD_DisplayStringLine(Line1,display_buf);

		sprintf((char*)display_buf,"   Pax:%d      ",Pax);
		LCD_DisplayStringLine(Line2,display_buf);
		
		sprintf((char*)display_buf,"   Pbx:%d      ",Pbx);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char*)display_buf,"   Pf:%d       ",Pf);
		LCD_DisplayStringLine(Line4,display_buf);
	}
	

}




//TIM15 -- 100ms

u8 led_cnt;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim15)
	{
	
//		if(++led_cnt == 10)
//		{
//				led_ctrl^=0x01;
//			led_cnt = 0;
//		}
	
	}
}





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM15_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		
		//TIM15
		HAL_TIM_Base_Start_IT(&htim15);
		
		//PA1  PA6 PA7
		
		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
		HAL_TIM_IC_Start_IT(&htim16,TIM_CHANNEL_1);
		HAL_TIM_IC_Start_IT(&htim17,TIM_CHANNEL_1);
		
		//UART
		HAL_UART_Receive_IT(&huart1,uart_buf,1);
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);


//    LCD_DrawLine(120, 0, 320, Horizontal);
//    LCD_DrawLine(0, 160, 240, Vertical);
//    HAL_Delay(1000);
//    LCD_Clear(Blue);

//    LCD_DrawRect(70, 210, 100, 100);
//    HAL_Delay(1000);
//    LCD_Clear(Blue);

//    LCD_DrawCircle(120, 160, 50);
//    HAL_Delay(1000);

//    LCD_Clear(Blue);
//    LCD_DisplayStringLine(Line4, (unsigned char *)"    Hello,world.   ");
//    HAL_Delay(1000);

//    LCD_SetBackColor(White);
//    LCD_DisplayStringLine(Line0, (unsigned char *)"                    ");
//    LCD_SetBackColor(Black);
//    LCD_DisplayStringLine(Line1, (unsigned char *)"                    ");
//    LCD_SetBackColor(Grey);
//    LCD_DisplayStringLine(Line2, (unsigned char *)"                    ");
//    LCD_SetBackColor(Blue);
//    LCD_DisplayStringLine(Line3, (unsigned char *)"                    ");
//    LCD_SetBackColor(Blue2);
//    LCD_DisplayStringLine(Line4, (unsigned char *)"                    ");
//    LCD_SetBackColor(Red);
//    LCD_DisplayStringLine(Line5, (unsigned char *)"                    ");
//    LCD_SetBackColor(Magenta);
//    LCD_DisplayStringLine(Line6, (unsigned char *)"                    ");
//    LCD_SetBackColor(Green);
//    LCD_DisplayStringLine(Line7, (unsigned char *)"                    ");
//    LCD_SetBackColor(Cyan);
//    LCD_DisplayStringLine(Line8, (unsigned char *)"                    ");
//    LCD_SetBackColor(Yellow);
//    LCD_DisplayStringLine(Line9, (unsigned char *)"                    ");


    Led_Control(0x00);
		printf("你好！\r\n");
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			Led_Proc();
			Key_Proc();
			LCD_Proc();
			Auto_Proc();
			Rx_Idle();
			
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
