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
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "stdio.h"
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
u8 M_set = 0;//0为低频
u32 f_low = 4000,f_high = 8000;
u32 f_set = 4000;
u32 Pwm_cnt = 0;
float PA1_Dset = 10.0;
//2
u8 R_Set = 1,K_Set = 1;//退出界面有效
u8 R_Real = 1,K_Real = 1;
u8 RK_Chose = 0;
float v_temp = 0.0;
float v_wait = 0.0;
u8 v_flag = 0;
//3
float v_high = 0;
float v_low = 0;
u32 v_cnt = 0;
u32 N_Cnt = 0;
//Cont_Key
u8 D_Clk = 0;
u8 Cont_Flag = 0;

//KEY
uint32_t keyTick = 0;
extern u32 Key_Cnt;
void Key_Process(void)
{
	if(uwTick - keyTick <20) return;
	keyTick = uwTick;
	Key_Read();
	
	if(Trg & 0x01)
	{
		LCD_Clear(Black);
		Display_Mode = (Display_Mode +1)%3;
		
		if(Display_Mode == Display_PARA )
		{
			R_Real = R_Set;
			K_Real = K_Set;
		}
		
		RK_Chose = 0;
	}
	if(Trg & 0x02)
	{
		if(Display_Mode == Display_DATA )
		{
			Pwm_cnt = 0;
			N_Cnt++;
			if(f_set == 4000||f_set == 8000)
			M_set =(M_set +1)%2;
		}
		if(Display_Mode == Display_PARA )
		{
			RK_Chose = (RK_Chose +1)%2;
		}
	}
	if(Trg & 0x04)
	{
		
		if(Display_Mode == Display_PARA )
		{
			if(RK_Chose == 0)
			{
				R_Set++;
				if(R_Set == 11)
					R_Set = 1;
			}
			if(RK_Chose == 1)
			{
				K_Set++;
			  if(K_Set == 11)
					K_Set = 1;
			}
		}
	}
	if(Trg & 0x08)
	{	
		if(D_Clk == 1)
		{
			D_Clk = 0;
			Key_Cnt = 0;
		}
		
		if(Display_Mode == Display_PARA )
		{
			if(RK_Chose == 0)
			{
				R_Set--;
				if(R_Set == 0)
					R_Set = 10;
			}
			if(RK_Chose == 1)
			{
				K_Set--;
			  if(K_Set == 0)
					K_Set = 10;
			}
		}
		
	}
	
	if(Cont & 0x08)
	{
		if(Display_Mode == Display_DATA)
			Cont_Flag = 1;
	}
	
}

//ADC
u32 adc2_val;
float r37_volt;
void ADC_Process(void)
{
	//R37
	HAL_ADC_Start(&hadc2);
	adc2_val = HAL_ADC_GetValue(&hadc2);
	r37_volt = adc2_val /4095.0f*3.3f;
	
	if(D_Clk == 0)
	{
			if(r37_volt <1)
		{
			PA1_Dset = 10.0;
		}else if(r37_volt >3)
		{
			PA1_Dset = 85.0;
		}
		else
		{
			PA1_Dset  = 37.5f*r37_volt -27.5f;
		}
	}
	
}

//PWM_OUT
void PWM_Process(void)
{
	
	TIM2 ->ARR = (1e6*1.0/f_set-1);
	
	TIM2 ->CCR2 = (TIM2->ARR +1)*PA1_Dset/100.0f;
}

//PWM_IC

u8 tim17_state = 0;
u32 tim17_cnt,tim17_cntu;
u32 f_PA7;
float d_PA7;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim17)
	{
		if(tim17_state == 0)
		{
			__HAL_TIM_SetCounter (&htim17,0);
			TIM17 ->CCER |=0x02;
			tim17_state = 1;
		}
		else if(tim17_state == 1)
		{
			tim17_cntu = __HAL_TIM_GetCounter (&htim17);
			TIM17->CCER &=~0x02;
			tim17_state = 2;
		}
		else if(tim17_state == 2)
		{
			tim17_cnt = __HAL_TIM_GetCounter(&htim17);
			tim17_state = 0;
			__HAL_TIM_SetCounter (&htim17,0);
			f_PA7 = 1e6/tim17_cnt;
			d_PA7 = tim17_cntu*100.0f/tim17_cnt;
			
			v_temp = (f_PA7*3.14f*2*R_Real)/(100.0f*K_Real);
			
			if(M_set == 0)
			{
				if(v_flag == 0)
				{
						if((v_temp - v_low)>= 0.1f)
					{
						
						v_wait = v_temp;
						v_flag = 1;
						v_cnt = 0;
					}
				}
				
			}else if(M_set == 1)
			{
				if(v_flag == 0)
				{
						if((v_temp - v_high)>=0.1f)
					{
						
						v_wait = v_temp;
						v_flag = 1;
						v_cnt = 0;
					}
				}
			}
				
		}
		HAL_TIM_IC_Start(&htim17,TIM_CHANNEL_1);
	}
	
}
//TIME

u32 Key_Cnt = 0;

u8 led2_flag = 0;
u32 led2_cnt = 0;
_Bool blink = 0;
u8 led_ct = 0x00;

float v_tt = 0;
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

	if(M_set == 1)
	{
		Pwm_cnt++;
		if(Pwm_cnt == 200)
		{
			Pwm_cnt = 0;
			if(f_set <8000)
			f_set +=160;
		}
	}else if(M_set == 0)
	{
		Pwm_cnt++;
		if(Pwm_cnt == 200)
		{
			Pwm_cnt = 0;
			if(f_set >4000)
			f_set -=160;
		}
	}
	
	//V
	if(v_flag == 1)
	{
		v_cnt++;
		if(v_cnt == 2000)
		{
			v_cnt = 0;
			v_flag = 0;
			
			v_tt  =fabs(v_wait - v_temp );
			if(fabs(v_wait - v_temp)<2.0)
			{
				if(M_set == 0)
				v_low = v_wait;
				if(M_set == 1)
				v_high = v_wait;
			}
		}
	}
	//KEY
	if(Cont_Flag == 1)
	{
		Key_Cnt ++;
		if(Key_Cnt  == 2000)
		{
			Key_Cnt = 0;
			Cont_Flag = 0;
			if(Cont & 0x08)
			{
				D_Clk = 1;
			}
		}
	}
	//LED
	if(led2_flag == 1)
	{
		led2_cnt++;
		if(led2_cnt == 100)
		{
			led2_cnt = 0;
			if(blink == 0)
				led_ct|= 0x02;
			else
				led_ct &= ~0x02;
			blink = !blink;
		}
	}
  /* USER CODE END SysTick_IRQn 1 */
}

//LED


//uint32_t ledTick = 0;
void LED_Process(void)
{
//	if(uwTick - ledTick < 100) return;
//	ledTick = uwTick;
	
	if(f_set>4000&&f_set<8000)
	{	
		led2_flag = 1;
	}
	
	else if(f_set == 4000||f_set == 8000)
	{
		led2_flag = 0;
		led2_cnt = 0;
		blink = 0;
		led_ct &= ~0x02;
	}
		
	
	if(Display_Mode == Display_DATA)
		led_ct |= 0x01;
	else
		led_ct &= ~0x01;
	
	if(D_Clk == 1)
		led_ct |= 0x04;
	else
		led_ct &= ~0x04;
	
	Led_Control(led_ct);
}


//LCD
void LCD_Process(void)
{
	u8 Display_Buf[30];
	
	if(Display_Mode == Display_DATA )
	{
		sprintf((char*)Display_Buf,"        DATA       ");
	  LCD_DisplayStringLine(Line1,Display_Buf);
		
		if(M_set == 0&&f_set == 4000)
		{
			sprintf((char*)Display_Buf,"     M=L    ");
	    LCD_DisplayStringLine(Line3,Display_Buf);
		
		}else if(M_set == 1&&f_set == 8000)
		{
			sprintf((char*)Display_Buf,"     M=H    ");
	    LCD_DisplayStringLine(Line3,Display_Buf);
		}
		
		sprintf((char*)Display_Buf,"     P=%2.0f%%     ",PA1_Dset*1.0f);
	  LCD_DisplayStringLine(Line4,Display_Buf);
		
		sprintf((char*)Display_Buf,"     V=%-13.1f        ",v_temp);
	  LCD_DisplayStringLine(Line5,Display_Buf);
		
	}
	
	if(Display_Mode == Display_PARA )
	{
		sprintf((char*)Display_Buf,"        PARA       ");
	  LCD_DisplayStringLine(Line1,Display_Buf);
		
		sprintf((char*)Display_Buf,"     R=%d     ",R_Set);
	  LCD_DisplayStringLine(Line3,Display_Buf);
		
		sprintf((char*)Display_Buf,"     K=%d      ",K_Set);
	  LCD_DisplayStringLine(Line4,Display_Buf);
	}
	
	if(Display_Mode == Display_RECD )
		{
			sprintf((char*)Display_Buf,"        RECD       ");
			LCD_DisplayStringLine(Line1,Display_Buf);
			
			sprintf((char*)Display_Buf,"     N=%-d         ",N_Cnt);
			LCD_DisplayStringLine(Line3,Display_Buf);
			
			sprintf((char*)Display_Buf,"     MH=%-5.1f       ",v_high);
			LCD_DisplayStringLine(Line4,Display_Buf);
			
			sprintf((char*)Display_Buf,"     ML=%-5.1f       ",v_low);
			LCD_DisplayStringLine(Line5,Display_Buf);
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
  /* USER CODE BEGIN 2 */
	
	LCD_Init();
	//PWM_OUT
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	
	//PWM_IC
	HAL_TIM_IC_Start_IT(&htim17,TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
//	LCD_DisplayStringLine(Line0, (uint8_t *)"                    ");
//	LCD_DisplayStringLine(Line1, (uint8_t *)"                    ");
//	LCD_DisplayStringLine(Line2, (uint8_t *)"      LCD Test      ");
//	LCD_DisplayStringLine(Line3, (uint8_t *)"                    ");
//	LCD_DisplayStringLine(Line4, (uint8_t *)"                    ");
//	
//	LCD_SetBackColor(White);
//	LCD_SetTextColor(Blue);

//	LCD_DisplayStringLine(Line5, (uint8_t *)"                    ");
//	LCD_DisplayStringLine(Line6, (uint8_t *)"       HAL LIB      ");
//	LCD_DisplayStringLine(Line7, (uint8_t *)"                    ");
//	LCD_DisplayStringLine(Line8, (uint8_t *)"         @80        ");
//	LCD_DisplayStringLine(Line9, (uint8_t *)"                    ");
	///
	Led_Control(0x00);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		PWM_Process();
		ADC_Process();
		LED_Process();
		Key_Process();
		LCD_Process();
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
