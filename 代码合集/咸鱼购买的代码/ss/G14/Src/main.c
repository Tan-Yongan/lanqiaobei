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
#define Display_RECD 2

u8 Display_Mode = 0;

//1
u8 M = 0;//0 L,1 H
double P = 0.32;
double V = 0.0;

double V_temp  = 0.0;
//2
u8 R = 1;
u8 K = 1;

u8 R_temp = 1;
u8 K_temp = 1;


//3
u8 N = 0;
double MH = 0.0;
double ML = 0.0;

//输出频率
u32 F = 4000;

//KEY
uint32_t keyTick  = 0;

_Bool m_flag = 0;
_Bool rk_change = 0;

//lock
_Bool B4_flag = 0;
_Bool lock = 0;
u32 B4_cnt = 0;


//LED

u8 led_ctrl = 0x00;
void Led_Process(void)
{
	if(Display_Mode == Display_DATA)
		led_ctrl |= 0x01;
	else
		led_ctrl &= ~0x01;
	
	if(lock == 1)
		led_ctrl |= 0x04;
	else
		led_ctrl &= ~0x04;
	
	
	Led_Control(led_ctrl);
}
void Key_Process(void)
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Trg & 0x01)
	{
		LCD_Clear(Black);
		Display_Mode = (Display_Mode + 1)%3;
		if(Display_Mode == Display_PARA)
		{
			rk_change = 0;
			R_temp = R;
			K_temp = K;
		}
		
		if(Display_Mode == Display_DATA)
		{
			R = R_temp;
			K = K_temp;
		}
			
	}
	
	if(Trg & 0x02)
	{
		if(Display_Mode == Display_DATA)
		{
			if(m_flag == 0)
			{
				
				m_flag = 1;
				N++;
			}
		}
		
		if(Display_Mode == Display_PARA)
		{
			rk_change = !rk_change;
		}
	}
	
	if(Trg & 0x04)
	{
		if(Display_Mode == Display_PARA)
		{
			if(rk_change == 0)
			{
					R_temp++;
				if(R_temp == 11)
					R_temp = 1;
			}
			if(rk_change == 1)
			{
					K_temp++;
				if(K_temp == 11)
					K_temp = 1;
			}
		}
	}
	
	if(Trg & 0x08)
	{
		if(Display_Mode == Display_PARA)
		{
			if(rk_change == 0)
			{
					R_temp--;
				if(R_temp == 0)
					R_temp = 10;
			}
			if(rk_change == 1)
			{
					K_temp--;
				if(K_temp == 0)
					K_temp = 10;
			}
		}
		
		if(Display_Mode == Display_DATA)
		{
			if(lock == 1)
				lock = 0;
		}
	
	}
	
	if(Cont & 0x08)
	{
		if(Display_Mode == Display_DATA)
		{
			B4_flag = 1;
		}
	}
	else
	{
		B4_flag = 0;
		B4_cnt = 0;
	}
		
	
}

//PA1
void PWM_Process(void)
{
	//
	
//	if(M == 0)
//	TIM2->ARR = 1e6/4000 - 1;
//	if(M == 1)
//	TIM2->ARR = 1e6/8000 - 1;
	TIM2->ARR = 1e6/F - 1;
	TIM2->CCR2 = (TIM2->ARR + 1) * P;
}

//PA7
u32 tim_cnt = 0;
u32 PA7_F;

//ML
_Bool ML_flag = 0;
u32 ML_cnt = 0;
//MH
_Bool MH_flag = 0;
u32 MH_cnt = 0;

uint32_t mlTick = 0;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	
		tim_cnt = __HAL_TIM_GetCounter(&htim17);
	__HAL_TIM_SetCounter(&htim17,0);
	
	PA7_F = 1e6/tim_cnt;

	V = PA7_F*2*3.14*R/(100 * K); //变化的值可以进行滤波
	
	
//	if(M == 0) //低频
//	{

//		if((V -ML) >= 0.1 && ML_flag == 0) //这里精确到小数点后一位
//		{
//			V_temp = V;//2s前的变量
//			ML_flag = 1;
//			//ML_cnt = 0; //每次有新的最大值出现，从新计时问题？本身不准,遵守兼容原则
//		}

//	}
//	
//	if(M == 1) //高频
//	{

//		if((V -MH) >= 0.1 && MH_flag == 0) //这里精确到小数点后一位
//		{
//			V_temp = V;//2s前的变量
//			MH_flag = 1;
//			//ML_cnt = 0; //每次有新的最大值出现，从新计时问题？本身不准
//		}

//	}

//电子设计方法
	if(M == 0)
	{
		//条件一：第一次出现最大速度
		if(V > ML && ML_flag == 0)
		{
			ML_flag = 1;
			V_temp = V;
			mlTick = uwTick; //从头计时
			
		}
		else if(ML_flag == 1)
		{
			//条件二：保持2s不变---嵌套的条件
			if(fabs(V - V_temp) < 0.5) //精度0.5
			{
				if(uwTick - mlTick > 2000)
				{
						ML = V;
						ML_flag = 0;
						Led_Control(0x40);
				}
			}
			else
				ML_flag = 0;
		}
		else
			ML_flag = 0;

		
		
	}
	
	
	HAL_TIM_IC_Start_IT(&htim17,TIM_CHANNEL_1);
	
}


//ADC
u32 adc_val;
double r37_v;
void ADC_Process(void)
{
	HAL_ADC_Start(&hadc2);
	adc_val = HAL_ADC_GetValue(&hadc2);
	
	r37_v = adc_val/4095.0f*3.3f;
	
	if(lock == 0)
	{
		if(r37_v < 1)
			P = 0.1;
		else if(r37_v > 3)
			P = 0.85;
		else
			P = 0.75/2.0 * (r37_v - 1) + 0.1;
	}	

	
}

//LCD
//uint32_t lcdTick = 0;
void LCD_Process(void)
{
//	if(uwTick - lcdTick < 100) return;
//	lcdTick = uwTick;
	
	u8 display_buf[30];
	
	
	if(Display_Mode == Display_DATA)
	{
		sprintf((char *)display_buf,"        DATA  ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		if(M == 0)
		sprintf((char *)display_buf,"     M=L");
		if(M == 1)
		sprintf((char *)display_buf,"     M=H");
		LCD_DisplayStringLine(Line3,display_buf);
		
		
		sprintf((char *)display_buf,"     P=%2.0f%%  ",P*100);
		LCD_DisplayStringLine(Line4,display_buf);
		
		sprintf((char *)display_buf,"     V=%3.1f      ",V);
		LCD_DisplayStringLine(Line5,display_buf);
	}
	
	if(Display_Mode == Display_PARA)
	{
		sprintf((char *)display_buf,"        PARA  ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char *)display_buf,"     R=%d  ",R_temp);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char *)display_buf,"     K=%d  ",K_temp);
		LCD_DisplayStringLine(Line4,display_buf);
		
	}
	
	if(Display_Mode == Display_RECD)
	{
		sprintf((char *)display_buf,"        RECD  ");
		LCD_DisplayStringLine(Line1,display_buf);
		

		sprintf((char *)display_buf,"     N=%d",N);
		LCD_DisplayStringLine(Line3,display_buf);

		sprintf((char *)display_buf,"     MH=%.1f        ",MH);
		LCD_DisplayStringLine(Line4,display_buf);
		
		
		sprintf((char *)display_buf,"     ML=%.1f        ",ML);
		LCD_DisplayStringLine(Line5,display_buf);
	}

}
//100ms

//m
u32 m_cnt = 0;
//B4

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	if(htim == &htim15)
	{
		if(m_flag == 1)
		{
			m_cnt++;
			led_ctrl ^= 0x02;
			if(m_cnt>= 50)
			{
				if(Display_Mode == Display_DATA)
					M = (M+1)%2;
				m_flag = 0;
				m_cnt = 0;
				led_ctrl &=~0x02;
			}
				//模式切换
			if(M == 1)
				F +=80;
			if(M == 0)
				F -=80;
		}
			
	
	//lock
		if(B4_flag == 1)
		{
			B4_cnt++;
			if(B4_cnt >= 20)
			{
				B4_flag = 0;
				B4_cnt = 0;
				lock = 1;
			}
		}
		
//		//ML
//		if(ML_flag == 1)
//		{
//			ML_cnt++;
//			if(ML_cnt >= 20) //相当与每200ms比较一次
//			{
//				ML_cnt = 0;
//				ML_flag = 0;
//				if(fabs(V - V_temp) < 0.1)
//				{
//					ML = V_temp;
//				}
//			}
//		}
//		
//		//MH
//		if(MH_flag == 1)
//		{
//			MH_cnt++;
//			if(MH_cnt >= 20) //相当与每200ms比较一次
//			{
//				MH_cnt = 0;
//				MH_flag = 0;
//				if(fabs(V - V_temp) < 0.1)
//				{
//					MH = V_temp;
//				}
//			}
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
  MX_TIM17_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		//PWM
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
		
		//PWM_IC
		HAL_TIM_IC_Start_IT(&htim17,TIM_CHANNEL_1);
		
		
		//TIM15
		HAL_TIM_Base_Start_IT(&htim15);
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
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			Key_Process();
			PWM_Process();
			ADC_Process();
			LCD_Process();
			//Led_Process();
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
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
