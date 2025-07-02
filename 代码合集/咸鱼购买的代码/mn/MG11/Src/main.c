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
#include "rtc.h"
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
#define Display_MAIN 0
#define Display_SETTING 1

u8 Display_Mode = 0;

//1




//2

u8 tim_chose = 0;//0小时，1分钟，2秒
u8 hour_set,min_set,sec_set;



//LED
uint32_t ledTick = 0;
u8 led_ctrl = 0x00;
void Led_Process(void)
{
	
}

//RTC
//显示
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};
void RTC_Process(void)
{
	HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);

}

//设置
void Set_Time(u8 hour,u8 min,u8 sec)
{
	RTC_TimeTypeDef setTime = {0};
	
	setTime.Hours = hour;
	setTime.Minutes = min;
	setTime.Seconds = sec;
	
	HAL_RTC_SetTime(&hrtc,&setTime,RTC_FORMAT_BIN);
	
}


//KEY
uint32_t keyTick = 0;

//B1-//按键双击切屏幕
u8 B1_flag = 0;
extern u32 B1_cnt;

//B3
_Bool B3_flag = 0;
u32 B3_1s_cnt = 0;
void Key_Process(void)
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	
	
	Key_Read();
	
	if(Trg & 0x01)
	{
		B1_flag++; //双击间隔
		
		if(B1_flag == 2)
		{
			  B1_flag = 0;
				B1_cnt = 0;
				LCD_Clear(Black);
				Display_Mode = (Display_Mode + 1)%2;
				if(Display_Mode == Display_SETTING)
				{
					tim_chose = 0;
					hour_set = sTime.Hours;
					min_set = sTime.Minutes;
					sec_set = sTime.Seconds;
				}
				
				if(Display_Mode == Display_MAIN)
				{
					Set_Time(hour_set,min_set,sec_set);
				}
				
		}

	}
	
	if(Trg & 0x02)
	{
		if(Display_Mode == Display_SETTING)
		{
			tim_chose = (tim_chose + 1)%3;
		}
	}
	
	if(Trg & 0x04)
	{
		if(Display_Mode == Display_SETTING)
		{
			if(tim_chose == 0)
				if(hour_set< 23)
					hour_set++;
				
			if(tim_chose == 1)
				if(min_set< 59)
					min_set++;
				
			if(tim_chose == 2)
				if(sec_set< 59)
					sec_set++;
		}
			
	}
	
	if(Trg & 0x08)
	{
		if(Display_Mode == Display_SETTING)
		{
			if(tim_chose == 0)
				if(hour_set > 0)
					hour_set--;
				
			if(tim_chose == 1)
				if(min_set > 0)
					min_set--;
				
			if(tim_chose == 2)
				if(sec_set > 0)
					sec_set--;
		}
	}
	
	if(Cont & 0x04)
	{
		B3_flag = 1;
	}
	else
	{
		B3_flag = 0;
		B3_1s_cnt = 0;
	}
	
}




//UART
//发送
u8 tx_buf[100] = {"你好hello!\r\n"};

//接受
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


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartTick = uwTick;
	HAL_UART_Receive_IT(&huart1,uart_buf,1);
  rx_buf[rx_cnt++] = uart_buf[0];
	
	if(uart_buf[0] == '\n')
	{
		//Led_Control(rx_buf[1]);
		//HAL_UART_Transmit(&huart1,rx_buf,3,50); //发送定长，可以首字符为00
		printf("%s",rx_buf); //printf发送第一个字符为00时，无法正常发送
		//printf("\r\n");//0d 0a
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
	u8 i;
	
	if(Display_Mode == Display_MAIN)
	{
		sprintf((char*)display_buf,"     MAIN");
		LCD_DisplayStringLine(Line0,display_buf);
		
		sprintf((char*)display_buf,"   %02d:%02d:%02d",sTime.Hours,sTime.Minutes,sTime.Seconds);
		LCD_DisplayStringLine(Line2,display_buf);
	}
	
	if(Display_Mode == Display_SETTING)
	{
		sprintf((char*)display_buf,"     SETTING");
		LCD_DisplayStringLine(Line0,display_buf);
		
		sprintf((char*)display_buf,"   %02d:%02d:%02d         ",hour_set,min_set,sec_set);//单个字符显示一定要加空格补足
		////////////////////////////012 34 5 67 8 9 10 
		//LCD_DisplayStringLine(Line2,display_buf);
		
		for(i = 0;i< 20;i++)
		{
			if(tim_chose == 0)
			{
				if(i == 3 || i == 4)
				{
					LCD_SetTextColor(Red);
					LCD_DisplayChar(Line2,320-16*i,display_buf[i]);
					LCD_SetTextColor(White);
				}
				else
				LCD_DisplayChar(Line2,320-16*i,display_buf[i]); 
			}

			
			if(tim_chose == 1)
			{
				if(i == 6 || i == 7)
				{
					LCD_SetTextColor(Red);
					LCD_DisplayChar(Line2,(320-16*i),display_buf[i]);
					LCD_SetTextColor(White);
				}
				else
				LCD_DisplayChar(Line2,(320-16*i),display_buf[i]); 
			} 
			
			if(tim_chose == 2)
			{
				if(i == 9 || i == 10)
				{
					LCD_SetTextColor(Red);
					LCD_DisplayChar(Line2,(320-16*i),display_buf[i]);
					LCD_SetTextColor(White);
				}
				else
				LCD_DisplayChar(Line2,(320-16*i),display_buf[i]); 
			}
			
		}
	}


}


//100ms


#define idle 5 //没个多少百毫秒加一次

//B1双击间隔
u32 B1_cnt = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	if(B3_flag == 1) //1s后响应
	{
		B3_1s_cnt++;
		if(B3_1s_cnt >= 10) 
		{
			B3_1s_cnt = (10 - idle);  //第一次响应后每500ms加一次
			
			if(Display_Mode == Display_SETTING)
			{
				if(tim_chose == 0)
					if(hour_set< 23)
						hour_set++;
					
				if(tim_chose == 1)
					if(min_set< 59)
						min_set++;
					
				if(tim_chose == 2)
					if(sec_set< 59)
						sec_set++;
			}
			
		}
	}
	
	//B1
	if(B1_flag == 1) //双击间隔500ms
	{
		B1_cnt++;
		if(B1_cnt >= 5)
		{
			B1_cnt = 0;
			B1_flag = 0;
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
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		//TIM
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
			LCD_Process();
			RTC_Process();
			
			
			
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

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
