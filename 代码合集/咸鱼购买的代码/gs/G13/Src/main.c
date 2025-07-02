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
#include "stdio.h"
#include "string.h"
#include "key.h"
#include "led.h"
#include "i2c_hal.h"
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
#define Display_REC 2

u8 display_mode = 0;


//1

double PA4_Temp = 0,PA5_Temp = 0;
extern double PA4_V;
extern double PA5_V;

//2

u8 X = 1;
u8 Y = 2;


//3

u8 N = 0;
double A = 3.20;
double T = 0.23;
double H = 1.75;
_Bool change_3;

//ohter

_Bool change_f = 0; // 0 倍频 1分频

//记录数组
double PA4_Buf[4][100];// 记录 0 原电压值 ，1 A,2 T,3 H
double PA5_Buf[4][100];

u8 PA4_cnt = 0;
u8 PA4_cnt2  = 0;

u8 PA5_cnt = 0;
u8 PA5_cnt2 = 0;



//LED
u8 led_ctrl = 0x00;
uint32_t ledTick = 0;

void Led_Proc(void)
{
	
	if(uwTick - ledTick <100)return;
	ledTick = uwTick;

	
	if(change_f == 0)
		led_ctrl |= 0x01;
	else
		led_ctrl &= ~0x01;
	
	if(change_f == 1)
		led_ctrl |= 0x02;
	else
		led_ctrl &= ~0x02;
	
	if(PA4_Temp > PA5_Temp * Y)
		led_ctrl ^= 0x04;
	else
		led_ctrl &=~0x04;
	
	
	Led_Control(led_ctrl);
}





//KEY

uint32_t longTick4 = 0;

uint32_t keyTick = 0;
void Key_Proc(void)
{
	if(uwTick - keyTick < 20)return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Trg & 0x01)
	{
		LCD_Clear(Black);
		display_mode = (display_mode + 1) % 3;
		
		change_f = 0;
	}

	if(Trg & 0x02)
	{
		if(display_mode == Display_PARA)
		{
			X++;
			if(X == 5)
				X = 1;
		EEPROM_Write(0x01,X);
		}
	}
	
	if(Trg & 0x04)
	{
		if(display_mode == Display_PARA)
		{
			Y++;
			if(Y == 5)
				Y = 1;
		 EEPROM_Write(0x02,Y);
		}
	}
	
	if(Trg & 0x08)
	{
		longTick4 = uwTick;
	}
	else if(Cont & 0x08 && uwTick - longTick4 > 1000)//长按
	{
		if(display_mode == Display_REC)
		{
			PA4_cnt = 0;
			PA4_cnt2 = 0;
			N = 0;
			memset(PA4_Buf,'\0',sizeof(PA4_Buf));
		}
	}
	else if(Up & 0x08 && uwTick - longTick4 < 1000) //短按
	{
		if(display_mode == Display_DATA)
		{
			
			PA4_Temp = PA4_V;
			PA5_Temp = PA5_V;
			
			//记录
			PA4_Buf[0][PA4_cnt2] = PA4_Temp;
			PA5_Buf[0][PA5_cnt2] = PA5_Temp;
			
			
			
			u8 i;
			double temp = 0,sum = 0;
			//最大值
			for(i = 0;i<= PA4_cnt;i++)
			{
				if(temp < PA4_Buf[0][i])
					temp = PA4_Buf[0][i];
			}
			PA4_Buf[1][0] = temp;
			
			//最小值
			temp = 3.3;
			for(i = 0;i<= PA4_cnt;i++)
			{
				if(temp > PA4_Buf[0][i])
					temp = PA4_Buf[0][i];
			}
			PA4_Buf[2][0] = temp;
			temp = 0;
			//平均值
			for(i = 0;i<= PA4_cnt;i++)
			{
					sum+= PA4_Buf[0][i];
			}
			
			PA4_Buf[3][0] = sum/(PA4_cnt+1); //99就满了
			sum = 0;
			
			PA4_cnt++; 
			PA4_cnt2++;
			
			if(N<100)//记录次数   ---题目没有说明最多加到多少
				N++;
			if(PA4_cnt == 100) //考虑第一遍，用两个计数值。
				PA4_cnt = 99;
			
			if(PA4_cnt2 == 100)
				PA4_cnt2 = 0;
			
			
			//最大值
			
			for(i = 0;i<= PA5_cnt;i++)
			{
				if(temp < PA5_Buf[0][i])
					temp = PA5_Buf[0][i];
			}
			PA5_Buf[1][0] = temp;
			
			//最小值
			temp = 3.3;
			for(i = 0;i<= PA5_cnt;i++)
			{
				if(temp > PA5_Buf[0][i])
					temp = PA5_Buf[0][i];
			}
			PA5_Buf[2][0] = temp;
			temp = 0;
			//平均值
			sum = 0;
			for(i = 0;i<= PA5_cnt;i++)
			{
					sum+= PA5_Buf[0][i];
			}
			
			PA5_Buf[3][0] = sum/(PA5_cnt+1); //99就满了
			sum = 0;
			
			PA5_cnt++; 
			PA5_cnt2++;

			if(PA5_cnt == 100)
				PA5_cnt = 99;
			
			if(PA5_cnt2 == 100)
				PA5_cnt2 = 0;
		}
		
		
		
		if(display_mode == Display_PARA)
		{
			change_f = !change_f;
		}
		
		if(display_mode == Display_REC)
		{
			change_3 = !change_3;
			LCD_Clear(Black);
		}
		
	}
	
	
	
	
	
}

//PA1
uint32_t uwIC2Value1 = 0;
uint32_t uwIC2Value2 = 0;
uint32_t uwDiffCapture = 0;

/* Capture index */
uint16_t uhCaptureIndex = 0;

/* Frequency Value */
uint32_t uwFrequency = 0;

uint32_t F_PA1;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if(uhCaptureIndex == 0)
			{
				/* Get the 1st Input Capture value */
				uwIC2Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
				uhCaptureIndex = 1;
			}
			else if(uhCaptureIndex == 1)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); 

				/* Capture computation */
				if (uwIC2Value2 > uwIC2Value1)
				{
					uwDiffCapture = (uwIC2Value2 - uwIC2Value1); 
				}
				else if (uwIC2Value2 < uwIC2Value1)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					uwDiffCapture = ((0xFFFFFFFF - uwIC2Value1) + uwIC2Value2) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */      
				F_PA1 = 1e6/uwDiffCapture;
				uhCaptureIndex = 0;
			}
		}
		
	}
	
}

//PA7

uint32_t F_PA7 = 8000;
double D_PA7 = 0.5;
void PWM_Proc(void)
{
	
	if(change_f == 0)//倍频
	{
		F_PA7 = F_PA1 * X;
	}
	else
	{
		F_PA7 = F_PA1*1.0 / X;
	}
	
	TIM3->ARR = 1e6/F_PA7 - 1;
	TIM3->CCR2 = (TIM3->ARR + 1)*D_PA7;
	
}


//UART

//TX
int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	HAL_UART_Transmit(&huart1,(u8 *)&ch,1,50);
  return ch;
}

//RX
u8 uart_buf[2];
u8 rx_buf[100];
int rx_cnt = 0;

uint32_t uartTick = 0;
_Bool fz = 0;
void Rx_Idle(void) //以后这个函数可以作为串口接收处理函数
{
	if(uwTick - uartTick < 50)return;
	uartTick = uwTick;
	
	if(rx_cnt > 0) //一次性接收完成
	{
		if(rx_cnt == 3) //确保发送的为一个字符,题目没有说明发送给单片机回车，就肯定不发回车
		{
				if(rx_buf[0] == 'X' && rx_buf[1] == 0x0d && rx_buf[2] == 0x0a) //考虑回车情况下
			{
				printf("X:%d\r\n",X);
			}
			if(rx_buf[0] == 'Y')
			{
				printf("Y:%d\r\n",Y);
			}
			
			if(rx_buf[0] == '#')
			{
				fz = !fz;
				
				if(fz == 0)
				{
					LCD_Clear(Black);
					//正向显示
					LCD_WriteReg(R1,0x0000); //从上往下
					LCD_WriteReg(R96,0x2700);//从左往右
					
					led_ctrl |= 0x08;
					
				}
				else
				{
					LCD_Clear(Black);
					//反向显示
					LCD_WriteReg(R1,0x0100); 
					LCD_WriteReg(R96,0xA700);
					
				  led_ctrl &= ~0x08;
				}
			}
		}
		
		if(rx_cnt == 3) //确保发送的为一个字符，考虑是否会接受回车
		{
				if(rx_buf[0] == 'P' && rx_buf[1] == 'A' && rx_buf[2] == '1')
			{
				printf("PA1:%d\r\n",F_PA1);
			}
			
				if(rx_buf[0] == 'P' && rx_buf[1] == 'A' && rx_buf[2] == '4')
			{
				printf("PA4:%4.2f\r\n",PA4_V);
			}
			
				if(rx_buf[0] == 'P' && rx_buf[1] == 'A' && rx_buf[2] == '5')
			{
				printf("PA5:%4.2f\r\n",PA5_V);
			}
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
	
//	if(rx_cnt > 0) //开始判断
//	{
//		
//		
//	}

}


//ADC

u32 adc_val1,adc_val2;

double PA4_V,PA5_V;
void ADC_Proc(void)
{
	HAL_ADC_Start(&hadc2);
	adc_val1 = HAL_ADC_GetValue(&hadc2);
	PA4_V = adc_val1/4095.0 * 3.3;
	
	HAL_ADC_Start(&hadc2);
	adc_val2 = HAL_ADC_GetValue(&hadc2);
	PA5_V = adc_val2/4095.0 * 3.3;
}

//LCD
uint32_t lcdTick = 0;
void LCD_Proc(void)
{
	if(uwTick - lcdTick < 100)return;
	lcdTick = uwTick;
	
	ADC_Proc();
	
	u8 display_buf[30];
	
	if(display_mode == 0)
	{
		sprintf((char*)display_buf,"        DATA ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char*)display_buf,"     PA4=%4.2f   ",PA4_Temp);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char*)display_buf,"     PA5=%4.2f   ",PA5_Temp);
		LCD_DisplayStringLine(Line4,display_buf);
		
		sprintf((char*)display_buf,"     PA1=%d          ",F_PA1);
		LCD_DisplayStringLine(Line5,display_buf);
	}
	
	if(display_mode == 1)
	{
		sprintf((char*)display_buf,"        PARA ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char*)display_buf,"     X=%d ",X);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char*)display_buf,"     Y=%d ",Y);
		LCD_DisplayStringLine(Line4,display_buf);
		
	}
	
	if(display_mode == 2)
	{
		if(change_3 == 0)
		{
		
			sprintf((char*)display_buf,"        REC-PA4");
			LCD_DisplayStringLine(Line1,display_buf);
			
			sprintf((char*)display_buf,"     N=%d   ",N);
			LCD_DisplayStringLine(Line3,display_buf);
			
			sprintf((char*)display_buf,"     A=%4.2f  ",PA4_Buf[1][0]);
			LCD_DisplayStringLine(Line4,display_buf);
			
			sprintf((char*)display_buf,"     T=%4.2f  ",PA4_Buf[2][0]);
			LCD_DisplayStringLine(Line5,display_buf);
			
			sprintf((char*)display_buf,"     H=%4.2f  ",PA4_Buf[3][0]);
			LCD_DisplayStringLine(Line6,display_buf);
		}
		else
		{
			sprintf((char*)display_buf,"        REC-PA5");
			LCD_DisplayStringLine(Line1,display_buf);
			
			sprintf((char*)display_buf,"     N=%d   ",N);
			LCD_DisplayStringLine(Line3,display_buf);
			
			sprintf((char*)display_buf,"     A=%4.2f  ",PA5_Buf[1][0]);
			LCD_DisplayStringLine(Line4,display_buf);
			
			sprintf((char*)display_buf,"     T=%4.2f  ",PA5_Buf[2][0]);
			LCD_DisplayStringLine(Line5,display_buf);
			
			sprintf((char*)display_buf,"     H=%4.2f  ",PA5_Buf[3][0]);
			LCD_DisplayStringLine(Line6,display_buf);
		}

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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		//IC_T2C2
		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
		
		//PWM
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
		
		//UART
		HAL_UART_Receive_IT(&huart1,uart_buf,1);
		
		//EEPROM
		I2CInit();
		
//		if(EEPROM_Read(0x00) != 123)
//		{
//			EEPROM_Write(0x00,123);
//			EEPROM_Write(0x01,1);
//			EEPROM_Write(0x02,2);
//		}
//		
//		X = EEPROM_Read(0x01);
//		Y = EEPROM_Read(0x02);
		
		
		EEPROM_Write(0x00,0);
		EEPROM_Write(0x01,0);
		EEPROM_Write(0x02,0);
		
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
		//printf("你好\r\n"); //有时候串口不行，可以用来激活串口
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			Key_Proc();
			LCD_Proc();
			PWM_Proc();
			Rx_Idle();
			Led_Proc();
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
