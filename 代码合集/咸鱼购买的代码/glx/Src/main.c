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

#include "seg.h"
#include "stdio.h"
#include "ds18b20_hal.h"
#include "dht11_hal.h"
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


//ADC2

u32 adc_val1,adc_val2;
double rp5_v,rp6_v;

//问题
//双通道adc时,需要注意两个扫描周期之间的间隔
//当下面有LCD程序等其他程序时，可以忽略延时
void ADC2_Process(void)
{
	
	HAL_ADC_Start(&hadc2);
	adc_val1 = HAL_ADC_GetValue(&hadc2);
	rp5_v = adc_val1 * 3.3/ 4095;
	
	
	HAL_ADC_Start(&hadc2);
	adc_val2 = HAL_ADC_GetValue(&hadc2);
	rp6_v = adc_val2 * 3.3 / 4095;

	//HAL_Delay(5);//扫描周期间隔
}

//ADC KEY
u8 AKEY_Read_IO(void)
{
	u8 key_val = 0x00;
	
	if(adc_val2 < 200)
		key_val |= 0x01;
	else if(adc_val2 < 800)
		key_val |= 0x02;
	else if(adc_val2 < 1400)
		key_val |= 0x04;
	else if(adc_val2 < 2000)
		key_val |= 0x08;
	else if(adc_val2 < 2600)
		key_val |= 0x10;
	else if(adc_val2 < 3200)
		key_val |= 0x20;
	else if(adc_val2 < 3800)
		key_val |= 0x40;
	else if(adc_val2 < 3950)
		key_val |= 0x80;
	
	return key_val;

}

u8 AKEY_Trg;
u8 AKEY_Cont;
u8 AKEY_Up;
void AKEY_Read(void)
{
	u8 ReadData = AKEY_Read_IO();
	AKEY_Trg = ReadData & (ReadData ^ AKEY_Cont);
	AKEY_Up = ~ReadData & (ReadData ^ AKEY_Cont);
	AKEY_Cont = ReadData;
}

uint32_t akeyTick = 0;
void AKEY_Process(void)
{
	if(uwTick - akeyTick < 20) return;
	akeyTick = uwTick;
	
	AKEY_Read();
}

//SEG
uint32_t segTick = 0;
int rp5_v_int;
u8 num = 123;
void SEG_Process(void)
{
	if(uwTick - segTick < 100) return;
	segTick = uwTick;
	
	rp5_v_int = rp5_v*100;
	
//	seg_buf[0] = seg_code[rp5_v_int/100]|0x80;
//	seg_buf[1] = seg_code[rp5_v_int/10%10];
//	seg_buf[2] = seg_code[rp5_v_int%10];
	
  seg_buf[0] = t_display[(int)(num*100)/100] | 0x80;
	seg_buf[1] = t_display[(int)(num*100)/10%10];
	seg_buf[2] = t_display[(int)(num*100)%10];

	Seg_Display();
}




/////////////////////////PWM1,2
//T3_CH1
uint32_t uwIC2Value1_T3CH1 = 0;
uint32_t uwIC2Value2_T3CH1 = 0;
//uint32_t uwDiffCapture_T3CH1 = 0;
double uwhigh_T3CH1 = 0,uwlow_T3CH1 = 0;

/* Capture index */
uint16_t uhCaptureIndex_T3CH1 = 0;

/* Frequency Value */
uint32_t uwFrequency_T3CH1 = 0;
double D_T3CH1 = 0;

//T3_CH2
uint32_t uwIC2Value1_T3CH2 = 0;
uint32_t uwIC2Value2_T3CH2 = 0;
//uint32_t uwDiffCapture_T3CH1 = 0;
double uwhigh_T3CH2 = 0,uwlow_T3CH2 = 0;

/* Capture index */
uint16_t uhCaptureIndex_T3CH2 = 0;

/* Frequency Value */
uint32_t uwFrequency_T3CH2 = 0;
double D_T3CH2 = 0;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	
	if(htim == &htim3)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if(uhCaptureIndex_T3CH1 == 0)
			{
				/* Get the 1st Input Capture value */
				uwIC2Value1_T3CH1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
				uhCaptureIndex_T3CH1 = 1;
			}
			else if(uhCaptureIndex_T3CH1 == 1)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2_T3CH1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);

				/* Capture computation */
				if (uwIC2Value2_T3CH1 > uwIC2Value1_T3CH1)
				{
					uwhigh_T3CH1 = (uwIC2Value2_T3CH1 - uwIC2Value1_T3CH1); 
				}
				else if (uwIC2Value2_T3CH1 < uwIC2Value1_T3CH1)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					uwhigh_T3CH1 = ((0xFFFFFFFF - uwIC2Value1_T3CH1) + uwIC2Value2_T3CH1) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */      
				uhCaptureIndex_T3CH1 = 2;
				uwIC2Value1_T3CH1 = uwIC2Value2_T3CH1;
				
			}
			else if(uhCaptureIndex_T3CH1 == 2)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2_T3CH1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 

				/* Capture computation */
				if (uwIC2Value2_T3CH1 > uwIC2Value1_T3CH1)
				{
					uwlow_T3CH1 = (uwIC2Value2_T3CH1 - uwIC2Value1_T3CH1); 
				}
				else if (uwIC2Value2_T3CH1 < uwIC2Value1_T3CH1)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					uwlow_T3CH1 = ((0xFFFF - uwIC2Value1_T3CH1) + uwIC2Value2_T3CH1) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */      
				uhCaptureIndex_T3CH1 = 0;
				D_T3CH1 = uwhigh_T3CH1 / (uwhigh_T3CH1 + uwlow_T3CH1);
				uwFrequency_T3CH1 = 1e6/(uwhigh_T3CH1 + uwlow_T3CH1);
			}
		}
	
		
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if(uhCaptureIndex_T3CH2 == 0)
			{
				/* Get the 1st Input Capture value */
				uwIC2Value1_T3CH2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);
				uhCaptureIndex_T3CH2 = 1;
			}
			else if(uhCaptureIndex_T3CH2 == 1)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2_T3CH2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); 
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);

				/* Capture computation */
				if (uwIC2Value2_T3CH2 > uwIC2Value1_T3CH2)
				{
					uwhigh_T3CH2 = (uwIC2Value2_T3CH2 - uwIC2Value1_T3CH2); 
				}
				else if (uwIC2Value2_T3CH2 < uwIC2Value1_T3CH2)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					uwhigh_T3CH2 = ((0xFFFF - uwIC2Value1_T3CH2) + uwIC2Value2_T3CH2) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */      
				uhCaptureIndex_T3CH2 = 2;
				uwIC2Value1_T3CH2 = uwIC2Value2_T3CH2;
				
			}
			else if(uhCaptureIndex_T3CH2 == 2)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2_T3CH2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); 

				/* Capture computation */
				if (uwIC2Value2_T3CH2 > uwIC2Value1_T3CH2)
				{
					uwlow_T3CH2 = (uwIC2Value2_T3CH2 - uwIC2Value1_T3CH2); 
				}
				else if (uwIC2Value2_T3CH2 < uwIC2Value1_T3CH2)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					uwlow_T3CH2 = ((0xFFFFFFFF - uwIC2Value1_T3CH2) + uwIC2Value2_T3CH2) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */      
				uhCaptureIndex_T3CH2 = 0;
				D_T3CH2 = uwhigh_T3CH2 / (uwhigh_T3CH2 + uwlow_T3CH2);
				uwFrequency_T3CH2 = 1e6/(uwhigh_T3CH2 + uwlow_T3CH2);
			}
		}
		
	}
	
}


//DS18B20
uint32_t tempTick = 0;
double temp;
void DS18B20_Process(void)
{
	if(uwTick - tempTick < 500) return;
	tempTick = uwTick;
	
	temp = DS18B20_Read();

}

//LCD
uint32_t lcdTick = 0;
double DH,DT;
void LCD_Process(void)
{
	if(uwTick - lcdTick < 100) return;
	lcdTick = uwTick;
	DHT11_Read();
	
	u8 display_buf[30];
	sprintf((char*)display_buf,"Trg:%02x,Up:%02x,Ct:%02x",AKEY_Trg,AKEY_Up,AKEY_Cont);
	LCD_DisplayStringLine(Line0,display_buf);
	
	sprintf((char*)display_buf,"ADC_Val:%-5d",adc_val2);
	LCD_DisplayStringLine(Line1,display_buf);
	
	sprintf((char*)display_buf,"Temp:%7.4f     ",temp); //全精度 
	LCD_DisplayStringLine(Line2,display_buf);

	//将温湿度合成为一个数
	DH = dht11.humidity_high + dht11.humidity_low /10.0;
	DT = dht11.temperature_high + dht11.temperature_low/10.0;
	sprintf((char*)display_buf,"H:%d.%d%% T:%d.%d",dht11.humidity_high,dht11.humidity_low,dht11.temperature_high,dht11.temperature_low); //全精度 
	LCD_DisplayStringLine(Line3,display_buf);
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
  MX_TIM3_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */

  LCD_Init();
		
		
//TIM		
//		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
//		
//		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//		
//		HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
//		HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);

//ADC_KEY
		ADC2_Process();//解决ADC按键上电触发一次问题：使用ADC按键必须先测一次！！！
		
	//DS18B20
		ds18b20_init_x();
		while(DS18B20_Read() >= 85){};
			
	//DHT11
		dht11Init();
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

    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			
		SEG_Process();
		ADC2_Process();
		LCD_Process();
		AKEY_Process();
	  DS18B20_Process();
		
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
