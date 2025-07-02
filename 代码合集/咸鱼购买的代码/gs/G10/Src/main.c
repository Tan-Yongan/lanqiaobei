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
#include "seg.h"
#include "ds18b20_hal.h"

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
#define Display_Para 1

u8 display_mode = 0;

//1

u16 N;//最大65535


//2

u8 T = 5;
u8 X = 1;// AO1

u8 T_temp = 5;
u8 X_temp = 1;
_Bool change = 0;


//LED
u8 led_ctrl = 0x00;

void Led_Proc(void)
{
	Led_Control(led_ctrl);//切记以后只操作led_ctrl即可
}


//KEY

uint32_t longTick3 = 0;
uint32_t longTick4 = 0;

uint32_t keyTick = 0;
void Key_Proc(void)
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Trg & 0x01)
	{
		LCD_Clear(Black);
		display_mode = (display_mode + 1)%2;
		
		if(display_mode == Display_Main)
		{
			if(X == X_temp && T == T_temp)
			{
			
			}
			else
			{
				N++;
				X_temp = X;
				T_temp = T;
			}
		}
		
		if(display_mode == Display_Para)
		{
			X = X_temp;
			T = T_temp;
		}
	}

	if(Trg & 0x02)
	{
		if(display_mode == Display_Para)
		{
			change = !change;
		}
	}
	
	if(display_mode == Display_Para)
	{
		if(change == 0)
		{
			if(Trg & 0x04)
			{
				longTick3 = uwTick;
			}
			else if(Cont & 0x04 && uwTick - longTick3 > 1000)
			{
				T++;
				if(T >= 41)
					T = 40;

			}
			else if(Up & 0x04 && uwTick - longTick3 < 1000)
			{
				T++;
				if(T >= 41)
					T = 40;
			}
		}
		else 
		{
			if(Trg & 0x04)
			{
				X = X%2 + 1;
			}
		}
	}	

	if(display_mode == Display_Para)
	{
		if(change == 0)
		{
			if(Trg & 0x08)
			{
				longTick4 = uwTick;
			}
			else if(Cont & 0x08 && uwTick - longTick4 > 1000) //这里假设0-40,先判断
			{
				if(T > 0)
					T--;

			}
			else if(Up & 0x08 && uwTick - longTick4 < 1000)
			{
				if(T > 0)
					T--;
			}
		}
		else 
		{
			if(Trg & 0x08)
			{
				X = X%2 + 1;
			}
		}
	}	
}

//ADC---以后可以放在LCD函数里，每100ms读取一次

u32 adc_val1,adc_val2;
double AO1,AO2;

void ADC_Proc(void)
{
	HAL_ADC_Start(&hadc2);
	adc_val1 = HAL_ADC_GetValue(&hadc2);
	AO1 = adc_val1/4095.0*3.3;
	
	HAL_ADC_Start(&hadc2);
	adc_val2 = HAL_ADC_GetValue(&hadc2);
	AO2 = adc_val2/4095.0*3.3;
}

//PWM

uint32_t uwIC2Value1 = 0;
uint32_t uwIC2Value2 = 0;
uint32_t uwDiffCapture = 0;
uint32_t low,high;
/* Capture index */
uint16_t uhCaptureIndex = 0;

/* Frequency Value */
uint32_t uwFrequency = 0;
uint32_t F;
double D;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim17)
	{
	 if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if(uhCaptureIndex == 0)
			{
				/* Get the 1st Input Capture value */
				uwIC2Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim17,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
				uhCaptureIndex = 1;
			}
			else if(uhCaptureIndex == 1)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim17,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
				/* Capture computation */
				if (uwIC2Value2 > uwIC2Value1)
				{
					high = (uwIC2Value2 - uwIC2Value1); 
				}
				else if (uwIC2Value2 < uwIC2Value1)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					high = ((0xFFFF - uwIC2Value1) + uwIC2Value2) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */
			uhCaptureIndex = 2;
			uwIC2Value1 = uwIC2Value2;
				
			}
			else if(uhCaptureIndex == 2)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
				/* Capture computation */
				if (uwIC2Value2 > uwIC2Value1)
				{
					low = (uwIC2Value2 - uwIC2Value1); 
				}
				else if (uwIC2Value2 < uwIC2Value1)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					low = ((0xFFFF - uwIC2Value1) + uwIC2Value2) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */
				uhCaptureIndex = 0;
				F = 1e6/(high + low);
				D = high*1.0/(high+low);
				
			}
		}
	}
		
}




//SEG--就这样定了，因为可以操控小数点和其他数
uint32_t segTick = 0;
_Bool seg_flag = 0;
u8 seg_cnt = 0;
void SEG_Proc(void)
{
	//交替显示
	if(uwTick - segTick < 100) return; //这里两秒刷新一次值太慢
	segTick = uwTick;
	
	if(seg_flag == 0)
	{
		seg_buf[0] = 0x39;  //0x00为熄灭
		seg_buf[1] = t_display[T/10];
		seg_buf[2] = t_display[T%10];
	}
	
	else
	{
		seg_buf[0] = 0x77;  
		seg_buf[1] = 0x5C;
		seg_buf[2] = t_display[X];
	}
	
	if(++seg_cnt >= 20)
	{
		seg_flag = !seg_flag;
		seg_cnt = 0;
	}

	
	Display(); 
	
}

//DS18B20 -- 温度间隔500ms读取一次//以后这个就是温度专用函数，温度放在定时器里
//放在TIM15或systick里
double T_C;
void DS18B20_Proc(void)
{
	static u8 cnt_temp;
	if(++cnt_temp >= 5)
	{
		T_C = DS18B20_Read();
		cnt_temp = 0;
	}
}


//UART
//发

int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	HAL_UART_Transmit(&huart1,(u8 *)&ch,1,50);
  return ch;
}

//收
u8 uart_buf[2];
u8 rx_buf[100];
u8 rx_cnt = 0;

uint32_t uartTick = 0;

void Rx_Idle(void)
{
	if(uwTick - uartTick < 50) return;
	uartTick = uwTick;
	
	if(rx_cnt>0)
	{
		rx_cnt = 0;
		memset(rx_buf,'\0',sizeof(rx_buf));
	}	
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartTick = uwTick;
  HAL_UART_Receive_IT(&huart1,uart_buf,1);
	
	rx_buf[rx_cnt++] = uart_buf[0];
	
	if(uart_buf[0] == '\n')
	{
		printf("%s",rx_buf);
	}
}



//LCD
uint32_t lcdTick = 0;

void LCD_Proc(void)
{
	if(uwTick - lcdTick < 100)return;
	lcdTick = uwTick;
	
	ADC_Proc();
	
	
	u8 display_buf[30];
	
	
	if(display_mode == Display_Main)
	{
		sprintf((char *)display_buf,"       Main ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char *)display_buf,"   A01:%4.2fV",AO1);
		LCD_DisplayStringLine(Line2,display_buf);
		
		sprintf((char *)display_buf,"   AO2:%4.2fV",AO2);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char *)display_buf,"   PWM2:%2.0f%%  ",D*100);
		LCD_DisplayStringLine(Line4,display_buf);
		
		sprintf((char *)display_buf,"   Temp:%5.2fC",T_C);
		LCD_DisplayStringLine(Line5,display_buf);
		
		sprintf((char *)display_buf,"   N:%d",N);
		LCD_DisplayStringLine(Line6,display_buf);
		
	}
	
	if(display_mode == Display_Para)
	{
		sprintf((char *)display_buf,"       Para ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char *)display_buf,"   T:%-2d               ",T);
		if(change == 0)
			LCD_SetBackColor(Green);
		LCD_DisplayStringLine(Line2,display_buf);
	  LCD_SetBackColor(Black);
		
		if(change == 1)
			LCD_SetBackColor(Green);
		sprintf((char *)display_buf,"   X:AO%d               ",X);
		LCD_DisplayStringLine(Line3,display_buf);
	 	LCD_SetBackColor(Black);
		
		
	}
	

}



//void SysTick_Handler(void)
//{
//  /* USER CODE BEGIN SysTick_IRQn 0 */

//  /* USER CODE END SysTick_IRQn 0 */
//  HAL_IncTick();
//  /* USER CODE BEGIN SysTick_IRQn 1 */

//	//DS18B20_Proc();
//  /* USER CODE END SysTick_IRQn 1 */
//}

//TIM15
//uint32_t cnt;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim15)
	{
//		if(++cnt >= 2)
//		{
//			cnt = 0;
//			led_ctrl^=0x01;
//		}
		DS18B20_Proc();
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
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		//capture
		HAL_TIM_IC_Start_IT(&htim17,TIM_CHANNEL_1);
		
		//DS18B20
		ds18b20_init_x();
		while(DS18B20_Read() >= 85) {};
			
		//TIM15
		HAL_TIM_Base_Start_IT(&htim15);
			
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
		printf("哈喽\r\n");
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			Key_Proc();	
			LCD_Proc();
			SEG_Proc();
			Led_Proc();
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
