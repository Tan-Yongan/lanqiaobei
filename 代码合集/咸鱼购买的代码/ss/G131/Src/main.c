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

#define Display_PSD 0
#define Display_STA 1

u8 Display_Mode = 0;


//1
//为了减少标志位，这里可以用负数形式
int B1_set = -1;
int B2_set = -1;
int B3_set = -1;

int PSD[3] = {1,2,3};

//2
u32 F = 1000;
double D = 0.5;


//切回


//LED
u8 led_ctrl = 0x00; //在某些函数里可以直接操作led_ctrl，因此将该变量放在最前面
u8 error_cnt = 0;
_Bool led2_flag = 0;
extern u32 led2_cnt;
void LED_Process(void)
{
	
	Led_Control(led_ctrl);
}


//KEY
uint32_t keyTick = 0;
uint32_t pwmTick = 0;//不用定时器方法
_Bool pwm_flag = 0;
void Key_Process(void)
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	Key_Read();
	
	if(Trg & 0x01)
	{
		if(Display_Mode == Display_PSD)
		{
			B1_set+=1;
			if(B1_set == 10)
				B1_set = 0;
		}
	}
		
	
	if(Trg & 0x02)
	{
		if(Display_Mode == Display_PSD)
		{
			B2_set+=1;
			if(B2_set == 10)
				B2_set = 0;
		}
	}
	
	if(Trg & 0x04)
	{
		if(Display_Mode == Display_PSD)
		{
			B3_set+=1;
			if(B3_set == 10)
				B3_set = 0;
		}
	}
	
	if(Trg & 0x08)
	{
		Display_Mode = (Display_Mode + 1)%2;
		if(Display_Mode == Display_STA)
		{
			if(PSD[0] == B1_set && PSD[1] == B2_set && PSD[2] == B3_set)//密码正确
			{
					
				LCD_Clear(Black);
				B1_set = -1;
				B2_set = -1;
				B3_set = -1;
				
//				pwmTick = uwTick;
				pwm_flag = 1;
				
				led_ctrl |= 0x01;
				F = 2000;
				D =0.1;
				
			}
			else  //密码错误
			{
				Display_Mode = Display_PSD;
				B1_set = -1;
				B2_set = -1;
				B3_set = -1;
				error_cnt++;
				if(error_cnt == 3)
				{
					error_cnt = 0;
					led2_flag = 1;
					led2_cnt = 0;//错内错，从头计时
					
				}
			}
		}
		
		else if(Display_Mode == Display_PSD)
		{
			LCD_Clear(Black);
			B1_set = -1;
			B2_set = -1;
			B3_set = -1;
		}
		

	}
}

//PA1
void PWM_Process(void)
{
	
	TIM2->ARR = 1e6/F;
	TIM2->CCR2 = (TIM2->ARR + 1)*D;
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
		rx_cnt = 0;
		memset(rx_buf,'\0',sizeof(rx_buf));
	}
}


//格式检查
//123-789
//0123456
u8 Check_Form(u8* str)
{
	u8 i;
	for(i = 0;i<7;i++)
	{
		if(i == 3)
		{
			if(str[i] != '-')
				return 0;
		}
		else 
		{
			if(str[i] >='0'&& str[i] <='9')
			{
			}
			else
				return 0;
		}
	}
	return 1;
}


//123-789
//0123456
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartTick = uwTick;
	HAL_UART_Receive_IT(&huart1,uart_buf,1);
	
	rx_buf[rx_cnt++] = uart_buf[0];
	if(rx_cnt == 7)
	{
		rx_cnt = 0;
		if(Check_Form(rx_buf))
		{
			if((rx_buf[0] - '0') == PSD[0] && (rx_buf[1] - '0')== PSD[1] && (rx_buf[2]- '0') == PSD[2])
			{
				PSD[0] = rx_buf[4] - '0';
				PSD[1] = rx_buf[5] - '0';
				PSD[2] = rx_buf[6] - '0';
			}
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
	
//	if(uwTick - pwmTick > 5000 && pwm_flag == 1)//不用定时器方法，定时一段时间需要有标志位
//	{
//		F = 1000;
//		D = 0.5;
//		pwm_flag = 0;
//	}
//		
	
	u8 display_buf[30];
	
	if(Display_Mode == Display_PSD)
	{
		sprintf((char * )display_buf,"       PSD");
		LCD_DisplayStringLine(Line1,display_buf);
		
		if(B1_set == -1)
			sprintf((char * )display_buf,"    B1:@");
		else
			sprintf((char * )display_buf,"    B1:%d",B1_set);
		LCD_DisplayStringLine(Line3,display_buf);

		if(B2_set == -1)
			sprintf((char * )display_buf,"    B2:@");
		else
			sprintf((char * )display_buf,"    B2:%d",B2_set);
		LCD_DisplayStringLine(Line4,display_buf);
	
		if(B3_set == -1)
			sprintf((char * )display_buf,"    B3:@");
		else
			sprintf((char * )display_buf,"    B3:%d",B3_set);
		LCD_DisplayStringLine(Line5,display_buf);
		
		
	}
	if(Display_Mode == Display_STA)
	{
		sprintf((char * )display_buf,"       STA");
		LCD_DisplayStringLine(Line1,display_buf);

		sprintf((char * )display_buf,"    F:%-dHz",F);
		LCD_DisplayStringLine(Line3,display_buf);

		sprintf((char * )display_buf,"    D:%.0f%%",D*100);
		LCD_DisplayStringLine(Line4,display_buf);

	}
	
	
}


//TIM15 100ms
//u32 cnt = 0;
u32 pwm_cnt = 0;
u32 led2_cnt = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim15)
	{
		//PWM
		if(pwm_flag == 1)
		{
			pwm_cnt++;
			if(pwm_cnt == 50)
			{
				pwm_cnt = 0;
				pwm_flag = 0;
				F = 1000;
				D = 0.5;
				led_ctrl &=~0x01;
			}
		}
		
		//led2
		if(led2_flag == 1)
		{
			led2_cnt++;
			led_ctrl^= 0x02;//间隔闪烁
			if(led2_cnt == 50)
			{
				led_ctrl &=~0x02;
				led2_cnt = 0;
				led2_flag = 0;
			}
			
		}

	}
	
}


//systick
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
		//PWM
//		if(pwm_flag == 1)
//		{
//			pwm_cnt++;
//			if(pwm_cnt == 5000)
//			{
//				pwm_cnt = 0;
//				pwm_flag = 0;
//				F = 1000;
//				D = 0.5;
//			}
//		}
	
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		//PWM
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
		
		//UART
		HAL_UART_Receive_IT(&huart1,uart_buf,1);
		
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
		//printf("你好世界\r\n");
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			Key_Process();
			PWM_Process();
			LCD_Process();
			LED_Process();
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
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
