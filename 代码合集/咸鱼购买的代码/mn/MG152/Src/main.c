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
#define Display_SLEEPING 1

u8 Display_Mode = 0;

//1
double TEMP = 23.5;

#define Auto 0
#define Manu 1
u8 MODE = 0;

u8 GEAR = 1;

//2

_Bool change_flag = 0; // 变为1切到界面2
u32 tim_cnt = 0;


//led4
u32 uart_cnt = 0;


//LED
u8 led_ctrl = 0;

void Led_Process(void)
{
	if(MODE == Auto)
		led_ctrl |=0x80;
	else
		led_ctrl &= ~0x80;
	
	if(GEAR == 1)
		led_ctrl |= 0x01;
	else
		led_ctrl &= ~0x01;
	
	if(GEAR == 2)
		led_ctrl |= 0x02;
	else
		led_ctrl &= ~0x02;
	
	if(GEAR == 3)
		led_ctrl |= 0x04;
	else
		led_ctrl &= ~0x04;
	
	
	Led_Control(led_ctrl);
}	



//KEY
uint32_t keyTick = 0;
void Key_Process(void)
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Trg & 0x01)
	{
		tim_cnt = 0;
		change_flag = 0;
		
		if(Display_Mode == Display_DATA)
			MODE = (MODE+1)%2;
		if(Display_Mode == Display_SLEEPING)
		{
			LCD_Clear(Black);
			Display_Mode = Display_DATA;
		}
	}
	
	if(Trg & 0x02)
	{
		tim_cnt = 0;
		change_flag = 0;
		if(Display_Mode == Display_DATA)
		{
			if(MODE == Manu)
			{
				GEAR++;
				if(GEAR == 4)
					GEAR = 3;
			}
		}
		
		if(Display_Mode == Display_SLEEPING)
		{
			LCD_Clear(Black);
			Display_Mode = Display_DATA;
		}
	}
	
	if(Trg & 0x04)
	{
		tim_cnt = 0;
		change_flag = 0;
		if(Display_Mode == Display_DATA)
		{
			if(MODE == Manu)
			{
				if(GEAR > 1)
					GEAR--;
			}
		}
		if(Display_Mode == Display_SLEEPING)
		{
			LCD_Clear(Black);
			Display_Mode = Display_DATA;
		}
	}
	
	if(Trg & 0x08)
	{
		Led_Control(0x08);
	}

}

//ADC
//ADC
u32 adc_val;
double r37_v;
void ADC_Process(void)
{
	HAL_ADC_Start(&hadc2);
	adc_val = HAL_ADC_GetValue(&hadc2);
	r37_v = adc_val/4095.0f * 3.3f;
	
	if(r37_v > 3)
		TEMP = 40;
	else if(r37_v < 1)
		TEMP = 20;
	else
		TEMP = 10*(r37_v - 1) + 20;
	
	
	
	if(MODE == Auto)
	{
		if(TEMP < 25)
			GEAR = 1;
		else if(TEMP > 30)
			GEAR = 3;
		else
			GEAR = 2;
	}
}


//PWM
void PWM_Process(void)
{
	TIM2->ARR = 499;
	if(GEAR == 1)
		TIM2->CCR1 = (TIM2->ARR + 1)*0.1;
	if(GEAR == 2)
		TIM2->CCR1 = (TIM2->ARR + 1)*0.4;
	if(GEAR == 3)
		TIM2->CCR1 = (TIM2->ARR + 1)*0.8;
	
}



//串口
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
	
	if(rx_cnt > 0)
	{
		if(rx_cnt != 2)
			printf("NULL\r\n");
		rx_cnt = 0;
		memset(rx_buf,'\0',sizeof(rx_buf));
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartTick = uwTick;
	HAL_UART_Receive_IT(&huart1,uart_buf,1);
	
	rx_buf[rx_cnt++] = uart_buf[0];
	
	if(rx_cnt == 2)
	{
		if(rx_buf[0] == 'B'&& (rx_buf[1] == '1' ||rx_buf[1] == '2' || rx_buf[1] == '3'))
		{
			led_ctrl |= 0x08;
			uart_cnt = 0;
			
			if(rx_buf[1] == '1')
			{
					tim_cnt = 0;
					change_flag = 0;

					if(Display_Mode == Display_DATA)
					MODE = (MODE+1)%2;
					if(Display_Mode == Display_SLEEPING)
					{
					LCD_Clear(Black);
					Display_Mode = Display_DATA;
					}
			}
			
			if(rx_buf[1] == '2')
			{
				tim_cnt = 0;
				change_flag = 0;
				if(Display_Mode == Display_DATA)
				{
				if(MODE == Manu)
				{
				GEAR++;
				if(GEAR == 4)
				GEAR = 3;
				}
				}

				if(Display_Mode == Display_SLEEPING)
				{
				LCD_Clear(Black);
				Display_Mode = Display_DATA;
				}
				
			}
			
			if(rx_buf[1] == '3')
			{
				tim_cnt = 0;
				change_flag = 0;
				if(Display_Mode == Display_DATA)
				{
					if(MODE == Manu)
					{
						if(GEAR > 1)
							GEAR--;
					}
				}
				if(Display_Mode == Display_SLEEPING)
				{
					LCD_Clear(Black);
					Display_Mode = Display_DATA;
				}
			}
				
		}
		else
			printf("NULL\r\n");
	}
	
}


//LCD
uint32_t lcdTick = 0;
void LCD_Process(void)
{
	if(uwTick - lcdTick < 100) return;
	lcdTick = uwTick;
	
	u8 display_buf[30];
	

	if(Display_Mode == Display_DATA)
	{
		sprintf((char*)display_buf,"        DATA");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char*)display_buf,"     TEMP:%.1f    ",TEMP);
		LCD_DisplayStringLine(Line3,display_buf);
		
		if(MODE == Auto)
			sprintf((char*)display_buf,"     MODE:Atuo");
		if(MODE == Manu)
			sprintf((char*)display_buf,"     MODE:Manu");
		LCD_DisplayStringLine(Line4,display_buf);
		
	  sprintf((char*)display_buf,"     GEAR:%d ",GEAR);
		LCD_DisplayStringLine(Line5,display_buf);
		
	}
	
	if(Display_Mode == Display_SLEEPING)
	{
		sprintf((char*)display_buf,"     SLEEPING");
		LCD_DisplayStringLine(Line4,display_buf);
		
		sprintf((char*)display_buf,"     TEMP:%.1f  ",TEMP);
		LCD_DisplayStringLine(Line5,display_buf);
	}
	
}


//TIM  100ms

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim15)
	{
		if(change_flag == 0)
		{
			tim_cnt++;
			if(tim_cnt >= 50) //5s
			{
				tim_cnt = 0;
				change_flag = 1;
				LCD_Clear(Black);
				Display_Mode = Display_SLEEPING;
			}
			
			if(led_ctrl & 0x08)
			{
				uart_cnt++;
				if(uart_cnt >= 30) //3s
				{
					led_ctrl &= ~0x08;
					uart_cnt = 0;
				}
			}
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
  MX_TIM15_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		//TIM15
		HAL_TIM_Base_Start_IT(&htim15);
		
		
		//PWM
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
		
		//uart
		
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
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		Led_Process();
		Key_Process();
		
		ADC_Process();
		Rx_Idle();
		LCD_Process();
		PWM_Process();
			
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
