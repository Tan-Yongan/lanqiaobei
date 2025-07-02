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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "key.h"
#include "stdio.h"
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

#define Display_Main 0
#define Display_Setting 1

u8 Display_Mode = 0;

//1
u8 status_flag = 0; //0Õý³££¬1¸ß£¬2µÍ

//2
u8 maxv_set = 24;
u8 minv_set = 12;
u8 upper_set = 1;
u8 lower_set = 2;

u8 maxv_com = 24;
u8 minv_com = 12;
u8 upper_com = 1;
u8 lower_com = 2;


u8 setting_index = 1; //1 2 3 4

//EEPROM
u8 start_times = 0; 


//KEY
uint32_t keyTick = 0;
void Key_Process(void)
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Up & 0x01)
	{
		LCD_Clear(Black);
		Display_Mode = (Display_Mode+1)%2;
		
		if(Display_Mode == Display_Setting)
		{
			setting_index = 1;
			 maxv_set = maxv_com;
			 minv_set = minv_com;
			 upper_set = upper_com;
			 lower_set = lower_com;
		}
			
		
		if(Display_Mode == Display_Main)
		{
			if(minv_set < maxv_set && upper_set != lower_set)
			{
				 maxv_com = maxv_set;
				 minv_com = minv_set;
				 upper_com = upper_set;
				 lower_com = lower_set; 
			}
		}
	}
	
	if(Up & 0x02)
	{
		if(Display_Mode == Display_Setting)
			setting_index = setting_index % 4 + 1;
	}
	
	if(Up & 0x04)
	{
		if(Display_Mode == Display_Setting)
		{
			if(setting_index == 1)
			{
				maxv_set += 3;
				if(maxv_set >= 33)
					maxv_set = 33;
			}
			if(setting_index == 2)
			{
				minv_set += 3;
				if(minv_set >= 33)
					minv_set = 33;
			}
			if(setting_index == 3)
			{
				upper_set += 1;
				if(upper_set>=8)
					upper_set = 8;
			}
			if(setting_index == 4)
			{
				lower_set += 1;
				if(lower_set >= 8)
					lower_set = 8;
			}
		}	
	}	
		
	if(Up & 0x08)
	{
		if(Display_Mode == Display_Setting)
		{
			if(setting_index == 1)
			{
				if(maxv_set > 0 )
					maxv_set -= 3;
			}
			if(setting_index == 2)
			{
				if(minv_set > 0)
				minv_set -= 3;
			}
			if(setting_index == 3)
			{
				upper_set -= 1;
				if(upper_set<=1)
					upper_set = 1;
			}
			if(setting_index == 4)
			{
				lower_set -= 1;
				if(lower_set <= 1)
					lower_set = 1;
			}
		}
	}
	
}

//ADC
u32 adc_val;
float r37_v;
void ADC_Process(void)
{
	HAL_ADC_Start(&hadc2);
	adc_val = HAL_ADC_GetValue(&hadc2);
	r37_v = adc_val/4095.0f*3.3f;
	
}

//LED
u8 led_ctrl = 0x00;
uint32_t ledTick;
void Led_Process(void)
{
	if(uwTick - ledTick < 200) return;
	ledTick = uwTick;
	
	if(r37_v > maxv_com/10.0f)
	{
		led_ctrl ^=0x01<<(upper_com-1);
		status_flag = 1;
	}
	else if(r37_v < minv_com / 10.0f)
	{
		led_ctrl ^= 0x01<<(lower_com - 1);
		status_flag = 2;
	}
	else
	{
		led_ctrl = 0x00;
		status_flag = 0;
	}
	
	Led_Control(led_ctrl);
	
}



//LCD
void LCD_Process(void)
{
	u8 display_buf[30];
	
	
	if(Display_Mode == Display_Main)
	{
		sprintf((char *)display_buf,"     Main ");
		LCD_DisplayStringLine(Line0,display_buf);
		
		
		sprintf((char *)display_buf,"  Volt:%4.2fV",r37_v);
		LCD_DisplayStringLine(Line2,display_buf);
		
		if(status_flag == 0)
		{
			sprintf((char *)display_buf,"  Status:Normal   ");
			LCD_DisplayStringLine(Line4,display_buf);
		}
		
		if(status_flag == 1)
		{
			sprintf((char *)display_buf,"  Status:Upper   ");
			LCD_DisplayStringLine(Line4,display_buf);
		}
		
		if(status_flag == 2)
		{
			sprintf((char *)display_buf,"  Status:Lower   ");
			LCD_DisplayStringLine(Line4,display_buf);
		}
		
//		sprintf((char *)display_buf,"Trg:%02x UP:%02x Cont:%02x",Trg,Up,Cont);
//		LCD_DisplayStringLine(Line6,display_buf);
	}
	
	if(Display_Mode == Display_Setting)
	{
		sprintf((char*)display_buf,"     Setting  ");
		LCD_DisplayStringLine(Line0,display_buf);
		
		if(setting_index == 1)
		{
			LCD_SetBackColor(Green);
			sprintf((char*)display_buf,"  Max Volt:%3.1fV     ",maxv_set/10.0f);
		  LCD_DisplayStringLine(Line2,display_buf);
			LCD_SetBackColor(Black);
		}
		else
		{
		  sprintf((char*)display_buf,"  Max Volt:%3.1fV     ",maxv_set/10.0f);
			LCD_DisplayStringLine(Line2,display_buf);
		}

		if(setting_index == 2)
		{
			LCD_SetBackColor(Green);
			sprintf((char*)display_buf,"  Min Volt:%3.1fV     ",minv_set/10.0f);
		  LCD_DisplayStringLine(Line4,display_buf);
			LCD_SetBackColor(Black);
		}
		else
		{
		  sprintf((char*)display_buf,"  Min Volt:%3.1fV     ",minv_set/10.0f);
		  LCD_DisplayStringLine(Line4,display_buf);
		}
		
		if(setting_index == 3)
		{
			LCD_SetBackColor(Green);
			sprintf((char*)display_buf,"  Upper:LED%d        ",upper_set);
	   	LCD_DisplayStringLine(Line6,display_buf);	
			LCD_SetBackColor(Black);
		}
		else
		{
			sprintf((char*)display_buf,"  Upper:LED%d        ",upper_set);
			LCD_DisplayStringLine(Line6,display_buf);
		}
		
		if(setting_index == 4)
		{
			LCD_SetBackColor(Green);
			sprintf((char*)display_buf,"  Lower:LED%d        ",lower_set);
			LCD_DisplayStringLine(Line8,display_buf);
			LCD_SetBackColor(Black);
		}
		else
		{
			sprintf((char*)display_buf,"  Lower:LED%d        ",lower_set);
			LCD_DisplayStringLine(Line8,display_buf);
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
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		
		//EEPROM
		I2CInit();
		
//		if(EEPROM_Read(0x01) != 123)
//		{
//			EEPROM_Write(0x01,123);
//			EEPROM_Write(0x02,0);
//		}
//		
//		start_times = EEPROM_Read(0x02)+1;
//		EEPROM_Write(0x02,start_times);
		
//		EEPROM_Write(0x01,0);
//		EEPROM_Write(0x02,0);
		
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
			ADC_Process();
			LCD_Process();
			Led_Process();
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
