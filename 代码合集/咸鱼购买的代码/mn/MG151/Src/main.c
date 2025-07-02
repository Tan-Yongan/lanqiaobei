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
#define Display_WARN 1

u8 Display_Mode = 0;

//1
#define S 0
#define R 1
#define L 2
u8 N = 0; // 0=s,1=R,2=L;
double D = 80.3;

_Bool L_flag = 0; //改变的5s内
_Bool R_flag = 0;
_Bool uart_flag = 0;
extern u32 L_cnt;
extern u32 R_cnt;

//2



//LED
u8 led_ctrl = 0x00;
uint32_t ledTick = 0;
void Led_Process(void)
{
	if(uwTick - ledTick < 100) return;
	ledTick = uwTick;
	
	if(Display_Mode == Display_DATA && N == L)
		led_ctrl ^= 0x01;
	else
		led_ctrl &= ~0x01;
	
	if(Display_Mode == Display_DATA && N == R)
		led_ctrl ^= 0x02;
	else
		led_ctrl &= ~0x02;
	
	if(Display_Mode == Display_WARN)
		led_ctrl |= 0x80;
	else
		led_ctrl &= ~0x80;
	
	//led_ctrl ^= 0x80;
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
		
		if(Display_Mode == Display_WARN)
		{
			LCD_Clear(Black);//最好放里面
			Display_Mode = Display_DATA;
			N = S;
			//led_ctrl &= ~0x80;
			printf("Success\r\n");
		}
	}
	
	if(Trg & 0x02) //这样写按键执行不对，必须加大括号，空的也行
	{}
		//Led_Control(0x02);
	
	if(Trg & 0x04)
	{
		//Led_Control(0x01);
		if(Display_Mode == Display_DATA)
		{
			if(N == L && L_flag == 1)
			{
				N = S;
				L_flag = 0;
				L_cnt = 0;
				printf("Success\r\n");
			}
			
			else if(N == S)
			{
				
				LCD_Clear(Black);
				Display_Mode = Display_WARN;
				printf("Warn\r\n");
			}
		}
		
	}
	
	if(Trg & 0x08)
	{
		if(Display_Mode == Display_DATA)
		{
			if(N == R && R_flag == 1)
			{
				N = S;
				R_flag = 0;
				R_cnt = 0;
				printf("Success\r\n");
			}
			
			else if(N == S)
			{
				LCD_Clear(Black);
				Display_Mode = Display_WARN;
				printf("Warn\r\n");
			}
		}
	}
	
}


//ADC
u32 adc_val;
double r37_v;
void ADC_Process(void)
{
	HAL_ADC_Start(&hadc2);
	adc_val = HAL_ADC_GetValue(&hadc2);
	r37_v = adc_val/4095.0f * 3.3f;
	
	if(r37_v >= 3.0)
		D = 300;
	else
		D = r37_v * 100;
	
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
		if(rx_cnt > 1)
			printf("Len ERROR\r\n");
		rx_cnt = 0;
		memset(rx_buf,'\0',sizeof(rx_buf));
	}
}





void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartTick = uwTick;
	HAL_UART_Receive_IT(&huart1,uart_buf,1);
	
	rx_buf[rx_cnt++] = uart_buf[0];
	
	if(rx_cnt == 1)
	{
		//rx_cnt = 0; //考虑到接收非法长度字符也报错
		
		if(Display_Mode == Display_DATA)
		{
			if(rx_buf[0] == 'R')
			{
				if(uart_flag == 0)
				{
					N = R;
					R_flag = 1;
					uart_flag = 1;
				}

			}
				
			else if(rx_buf[0] == 'L')
			{
				if(uart_flag == 0)
				{
					N = L;
					L_flag = 1;
					uart_flag = 1;
				}

			}
			else
				printf("ERROR\r\n");
		}
		if(Display_Mode == Display_WARN)
		{
			printf("WAIT\r\n");//根据题目要求，接受到任意字符，而不是字符串
		}
		memset(rx_buf,'\0',sizeof(rx_buf));
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
		sprintf((char*)display_buf,"        DATA ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		if(N == S)
		sprintf((char*)display_buf,"       N:S");
		
		if(N == R)
		sprintf((char*)display_buf,"       N:R");
		
		if(N == L)
		sprintf((char*)display_buf,"       N:L");
	
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char*)display_buf,"       D:%.1f      ",D);
		LCD_DisplayStringLine(Line4,display_buf);
	}
	
	if(Display_Mode == Display_WARN)
	{
		sprintf((char*)display_buf,"        WARN");
		LCD_DisplayStringLine(Line4,display_buf);
	}
	
	
}



//定时1ms
u32 L_cnt = 0;
u32 R_cnt = 0;

u32 uart_cnt = 0;

void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
	
	if(L_flag == 1)
	{
		L_cnt++;
		if(L_cnt > 5000)
		{
			L_cnt = 0;
			L_flag = 0;
			LCD_Clear(Black);
			Display_Mode = Display_WARN;
		}
	}
	
	if(R_flag == 1)
	{
		R_cnt++;
		if(R_cnt > 5000)
		{
			R_cnt = 0;
			R_flag = 0;
			LCD_Clear(Black);
			Display_Mode = Display_WARN;
		}
	}
	
	//串口计时
	if(uart_flag == 1)
	{
		uart_cnt++;
		if(uart_cnt > 5000)
		{
				uart_cnt = 0;
				uart_flag = 0;
		}
	}
	
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
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
		//printf("hello\r\n");
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			Key_Process();
			LCD_Process();
			ADC_Process();
			Rx_Idle();
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
