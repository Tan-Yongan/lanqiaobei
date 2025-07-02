/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "demo.h" 


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


//LED
u8 led_ctrl = 0x00;
void Led_Proc(void)
{
	
	Led_Control(led_ctrl);
}

//KEY
uint32_t keyTick = 0;
void Key_Proc(void)
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Trg & 0x01)
	{
		Led_Control(0x01);
	}
	
	if(Trg & 0x02)
	{
		Led_Control(0x02);
	}
	
	if(Trg & 0x04)
	{
		Led_Control(0x04);
	}
	
	if(Trg & 0x08)
	{
		Led_Control(0x08);
	}
}


//LCD
uint32_t lcdTick = 0;
void LCD_Proc(void)
{
	if(uwTick - lcdTick < 100) return;
	lcdTick = uwTick;
	
	u8 display_buf[30];
	
	sprintf((char*)display_buf,"Hello ");
	LCD_DisplayStringLine(Line0,display_buf);
	
}


//比较输出 100HZ - 10kHZ  0.2 - 0.8
uint32_t PA6_F = 10000;//最好在配置cubemx的时候与要求频率保持一致
double  PA6_D = 0.68;

uint32_t PA4_F = 5000;
double  PA4_D = 0.65;

uint32_t OC_Capture1;
uint32_t OC_Capture2;

uint32_t Pulse1;
uint32_t Pulse2;

void PWMOC_Proc(void)
{
	Pulse1 = 1e6/PA6_F;
	Pulse2 = 1e6/PA4_F;
}


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)//核心，1.pulse为周期  2.ccr在变
{
	if(htim == &htim3)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			OC_Capture1 = __HAL_TIM_GetCompare(&htim3,TIM_CHANNEL_1);
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 0)
			{
				TIM3->CCR1 = OC_Capture1 + Pulse1*(1-PA6_D);
			}
			else 
			{
				TIM3->CCR1 = OC_Capture1 + Pulse1*PA6_D;
			}
			
		}
		
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			OC_Capture2 = __HAL_TIM_GetCompare(&htim3,TIM_CHANNEL_2);
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == 0)
			{
				TIM3->CCR2 = OC_Capture2 + Pulse2*(1-PA4_D);
			}
			else 
			{
				TIM3->CCR2 = OC_Capture2 + Pulse2*PA4_D;
			}
			
		}
		
	}
}

//LED-PWM //100hz呼吸灯  10ms
int led_cnt;
_Bool led_flag = 0;
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  if(++led_cnt == 10)
	{
		led_cnt = 0;
	}
	  if(led_cnt < 4) //占空比为4/10
		  led_ctrl |=0x01;
		else
			led_ctrl &=~0x01;
	
  /* USER CODE END SysTick_IRQn 1 */
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
	LCD_Init();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	
	
	//TIM3 CH1,CH2比较输出
	HAL_TIM_OC_Start_IT(&htim3,TIM_CHANNEL_1);
  HAL_TIM_OC_Start_IT(&htim3,TIM_CHANNEL_2); 
	 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
  LCD_SetTextColor(White); 
  //  LCD_DisplayStringLine(Line4, (unsigned char *)"    Hello,world.   ");

  Led_Control(0x00);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		Led_Proc();
		Key_Proc();
		LCD_Proc();
		PWMOC_Proc();
		
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
