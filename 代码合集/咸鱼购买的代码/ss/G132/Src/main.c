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
#define Display_SHOP 0
#define Display_PRICE 1
#define Display_REP 2

u8 Display_Mode = 0;

//1

u8 X_SHOP = 1;
u8 Y_SHOP = 8;

_Bool tim_st = 0;

//2

double X_PRICE = 1.0;
double Y_PRICE = 2.0;


//3

u8 X_REP = 23;
u8 Y_REP = 10;


//EEPROM
u8 start_times;

//LED
u8 led_ctrl = 0x00;
uint32_t ledTick = 0;
void LED_Process(void)
{
	if(uwTick - ledTick < 1000) return;
	ledTick = uwTick;
	
//	if(X_REP == 0 && Y_REP == 0)
//		led_ctrl ^= 0x02;
//	else
//		led_ctrl &= ~0x02;

	led_ctrl ^=0x80;
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
		LCD_Clear(Black);
		Display_Mode = (Display_Mode + 1)% 3;
	}
	
	if(Trg & 0x02)
	{
		if(Display_Mode == Display_SHOP)
		{
			if(X_SHOP < X_REP)
			{
				X_SHOP+=1;
				
			}
					
		}
		
		if(Display_Mode == Display_PRICE)
		{
			X_PRICE += 0.1;
			if(X_PRICE > 2.01)//小数不准
				X_PRICE = 0.1;
			
			EEPROM_Write(0x02,X_PRICE*10);
		}
		
		if(Display_Mode == Display_REP)
		{
			X_REP +=1;
			EEPROM_Write(0x00,X_REP);
		}
		
	}
	
	if(Trg & 0x04)
	{
		if(Display_Mode == Display_SHOP)
		{
			if(Y_SHOP < Y_REP)
			{
				Y_SHOP+=1;
			}
		}
		
		if(Display_Mode == Display_PRICE)
		{
			Y_PRICE += 0.1;
			if(Y_PRICE > 2.01)//小数不准
				Y_PRICE = 0.1;
			EEPROM_Write(0x03,Y_PRICE*10);
		}
		
		if(Display_Mode == Display_REP)
		{
			Y_REP +=1; 
			EEPROM_Write(0x01,Y_REP);
		}
	}
	
	if(Trg & 0x08)
	{
		if(Display_Mode == Display_SHOP)
		{
			
			
			X_REP -= X_SHOP;
			Y_REP -= Y_SHOP;
			
			
			printf("X:%d,Y:%d,Z:%3.1f\r\n",X_SHOP,Y_SHOP,X_SHOP*X_PRICE + Y_SHOP * Y_PRICE);
			
			X_SHOP = 0;
			Y_SHOP = 0;
	
			tim_st = 1;
			led_ctrl |= 0x01;
		}
	}
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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartTick = uwTick;
	HAL_UART_Receive_IT(&huart1,uart_buf,1);
	
	rx_buf[rx_cnt++] = uart_buf[0];
	if(rx_cnt == 1)
	{
		rx_cnt = 0;
		
		if(rx_buf[0] == '?')
			printf("X:%3.1f,Y:%3.1f\r\n",X_PRICE,Y_PRICE);
		
		memset(rx_buf,'\0',sizeof(rx_buf));
		
	}
	
	
}


//PWM
void PWM_Process(void)
{
	TIM2->ARR = 499;
	
	if(tim_st == 0)
		TIM2->CCR2 = (TIM2->ARR + 1) * 0.3;
	else
		TIM2->CCR2 = (TIM2->ARR + 1) * 0.05;
	
}


//LCD
uint32_t lcdTick = 0;
void LCD_Process(void)
{
	if(uwTick - lcdTick < 100)
		lcdTick = uwTick;
	
	u8 display_buf[30];

	
	if(Display_Mode == Display_SHOP)
	{
		sprintf((char*)display_buf,"        SHOP");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char*)display_buf,"     X:%-5d",X_SHOP); //空格占位
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char*)display_buf,"     Y:%-5d",Y_SHOP);
		LCD_DisplayStringLine(Line4,display_buf);
	}
	
	if(Display_Mode == Display_PRICE)
	{
		sprintf((char*)display_buf,"        PRICE");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char*)display_buf,"     X:%3.1f",X_PRICE);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char*)display_buf,"     Y:%3.1f",Y_PRICE);
		LCD_DisplayStringLine(Line4,display_buf);
	}
	
	if(Display_Mode == Display_REP)
	{
		sprintf((char*)display_buf,"        REP");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char*)display_buf,"     X:%-5d",X_REP);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char*)display_buf,"     Y:%-5d",Y_REP);
		LCD_DisplayStringLine(Line4,display_buf);
	}
	
	
}



//TIM15

u32 tim_cnt = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(tim_st == 1)
	{
		tim_cnt++;
		if(tim_cnt == 50)
		{
			tim_cnt = 0;
			tim_st = 0;
			led_ctrl &= ~0x01;
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
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
	
		//UART
		HAL_UART_Receive_IT(&huart1,uart_buf,1);
		
		//EEPROM
		I2CInit();
		
		//TIM15
		HAL_TIM_Base_Start_IT(&htim15);
		
		//PWM
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
		
		
		if(EEPROM_Read(0x10) != 12) //第一次上电
		{
			EEPROM_Write(0x10,12);
			//在变量定义处初始化和这里保持一致
			X_REP = 10;
			X_PRICE = 1.0;
			Y_REP = 10;
			Y_PRICE = 1.0;
			
			X_SHOP = 0;
			Y_SHOP = 0;
			
			EEPROM_Write(0x00,X_REP);
			EEPROM_Write(0x01,Y_REP);
			//注意EEPROM小于1的数，不能存入
			EEPROM_Write(0x02,X_PRICE*10);
			EEPROM_Write(0x03,Y_PRICE*10);
		}
		else
		{
			X_SHOP = 0;
			Y_SHOP = 0; 
			
			X_REP = EEPROM_Read(0x00);
			Y_REP = EEPROM_Read(0x01);
			
			X_PRICE = EEPROM_Read(0x02)/10.0;
			Y_PRICE = EEPROM_Read(0x03)/10.0;
		}
		
//		EEPROM_Write(0x10,0);
//		EEPROM_Write(0x00,0);
//		EEPROM_Write(0x01,0);
//		EEPROM_Write(0x02,0);
//		EEPROM_Write(0x03,0);
		
//		start_times = EEPROM_Read(0x00)+1;
//		EEPROM_Write(0x00,start_times);
		
//		EEPROM_Write(0x00,0);
//		EEPROM_Write(0x01,0);
		
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
		
		printf("你好\r\n");
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			Key_Process();
			LCD_Process();
			PWM_Process();
			Rx_Idle();
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
