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
#include "dac.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "key.h"
#include "stdio.h"
#include "i2c_hal.h"
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
		//正向显示
		LCD_WriteReg(R1,0x0000); //从上往下
		LCD_WriteReg(R96,0x2700);//从左往右
		
	}
	
	if(Trg & 0x02)
	{
		LCD_Clear(Black);
		//和正向左右镜面对称
		LCD_WriteReg(R1,0x0000);
		LCD_WriteReg(R96,0xA700);//从右往左
	}
	
	if(Trg & 0x04)
	{
		LCD_Clear(Black);
		//和反向左右镜面对称
		LCD_WriteReg(R1,0x0100);//从下往上
		LCD_WriteReg(R96,0x2700);
	}
	
	if(Trg & 0x08)
	{
		//反向显示
		LCD_Clear(Black);
		LCD_WriteReg(R1,0x0100);
		LCD_WriteReg(R96,0xA700);
	}
}

//ADC
u32 adc_val1,adc_val2;
float r37_v,r38_v;
u32 mcp_adc;
float mcp_v;

void ADC_Process(void)
{
	HAL_ADC_Start(&hadc2);
	adc_val2 = HAL_ADC_GetValue(&hadc2);
	r37_v = adc_val2/4095.0f * 3.3f;
	
	//r38
	HAL_ADC_Start(&hadc1);
	adc_val1 = HAL_ADC_GetValue(&hadc1);
	r38_v = adc_val1/4095.0f * 3.3f;
	//mcp
	HAL_ADC_Start(&hadc1);
	mcp_adc = HAL_ADC_GetValue(&hadc1);
	mcp_v = mcp_adc/4095.0f*3.3f;
	
}

//EEPROM
u8 start_times = 0;

//MCP4017
u8 mcp_val;

//DAC
u16 dac_ch1_val,dac_ch2_val;
double dac_ch1_v = 1.1,dac_ch2_v = 2.2;
//void DAC_Process(void)
//{
//	//4095 -- 3.3v
//	dac_ch1_val = (dac_ch1_v/3.3f*4095.0f);
//	dac_ch2_val = (dac_ch2_v/3.3f*4095.0f);
//	
//	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dac_ch1_val); 
//	HAL_DAC_Start(&hdac1,DAC_CHANNEL_1);
//	
//	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_2,DAC_ALIGN_12B_R,dac_ch2_val); 
//	HAL_DAC_Start(&hdac1,DAC_CHANNEL_2);
//	
//}

void DAC_Process(void)
{
	dac_ch1_val = (u16)(dac_ch1_v/3.3*4095); 
	dac_ch2_val = (u16)(dac_ch2_v/3.3*4095);
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dac_ch1_val); 
	HAL_DAC_Start(&hdac1,DAC_CHANNEL_1);
	
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_2,DAC_ALIGN_12B_R,dac_ch2_val);
	HAL_DAC_Start(&hdac1,DAC_CHANNEL_2);

}


//RTC
RTC_TimeTypeDef rtc_Time = {0};
RTC_DateTypeDef rtc_Date = {0};

void RTC_Process(void)
{
	HAL_RTC_GetTime(&hrtc,&rtc_Time,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&rtc_Date,RTC_FORMAT_BIN);
}

void Set_Time(u8 hour,u8 min,u8 sec)
{
	RTC_TimeTypeDef s_time = {0};
	
	s_time.Hours = hour;
	s_time.Minutes = min;
	s_time.Seconds = sec;
	
	HAL_RTC_SetTime(&hrtc,&s_time,RTC_FORMAT_BIN);
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
		rx_cnt = 0;
		//Led_Control(rx_buf[1]);
		//HAL_UART_Transmit(&huart1,rx_buf,3,50); //发送定长，可以首字符为00
		printf("%s",rx_buf); //printf发送第一个字符为00时，无法正常发送
		//printf("\r\n");//0d 0a
		memset(rx_buf,'\0',sizeof(rx_buf));
	}
}

//TIM_IC
u32 tim2_cnt1;
u32 f40;

u32 tim3_cnt1;
u32 f39;

//duty
u8 tim2_state = 0;
u32 tim2_cnt2;
float d40;

u8 tim3_state = 0;
u32 tim3_cnt2;
float d39;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
		if(tim2_state == 0)
		{
			__HAL_TIM_SetCounter(&htim2,0);
			TIM2->CCER |=0x02;
			tim2_state = 1;
		}
		else if(tim2_state == 1)
		{
			tim2_cnt1 = __HAL_TIM_GetCounter(&htim2);
			TIM2->CCER &=~0x02;
			tim2_state = 2;
		}
		else if(tim2_state == 2)
		{
			tim2_cnt2 = __HAL_TIM_GetCounter(&htim2);
			f40 = 1e6/tim2_cnt2;
			d40 = tim2_cnt1*100.0f/tim2_cnt2;
			
			tim2_state = 0;
		}
		
		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	}
	
	if(htim == &htim3)
	{
		if(tim3_state == 0)
		{
			__HAL_TIM_SetCounter(&htim3,0);
			TIM3->CCER |=0x02;
			tim3_state = 1;
		}
		else if(tim3_state == 1)
		{
			tim3_cnt1 = __HAL_TIM_GetCounter(&htim3);
			TIM3->CCER &=~0x02;
			tim3_state = 2;
		}
		else if(tim3_state == 2)
		{
			tim3_cnt2 = __HAL_TIM_GetCounter(&htim3);
			f39 = 1e6/tim3_cnt2;
			d39 = tim3_cnt1*100.0f/tim3_cnt2;
			
			tim3_state = 0;
		}
		
		HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
	}
}

//PWM
void PWM_Process(void)
{
	TIM16->ARR = 99;//10k
	TIM16->CCR1 = (TIM16->ARR + 1)*0.35f; //加上f，更加准确
	
	TIM17->ARR = 49;//20K
	TIM17->CCR1 = (TIM17->ARR + 1)*0.75f;
	
	//需要写
//  TIM4->ARR = 19;//20K
//	TIM4->CCR2 = (TIM4->ARR + 1)*0.45f;
	
}


//TIM7
u32 tim15_cnt = 0;
_Bool led_blink = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//  if(htim->Instance == TIM15)
//	{
//		tim15_cnt++;
//		if(tim15_cnt == 5)//500ms
//		{
//			tim15_cnt = 0;
//			led_blink = !led_blink;
//		}

//	}

}


//LED
uint32_t ledTick = 0;
u8 led_ctrl = 0xff;
void Led_Process(void)
{
//	if(uwTick - ledTick < 500) return;
//	ledTick = uwTick;
	if(led_blink == 0)
		led_ctrl = 0xff;
	else
		led_ctrl = 0x00;
	//led_ctrl = ~led_ctrl;
	Led_Control(led_ctrl);
	
}


//LCD
uint32_t lcdTick = 0;
void LCD_Process(void)
{
	if(uwTick - lcdTick < 200) return;
	lcdTick = uwTick;
	
	u8 Display_Buf[30];
	
	sprintf((char*)Display_Buf,"%02x",15);
	LCD_DisplayStringLine(Line0,Display_Buf);
	
	sprintf((char*)Display_Buf,"r37:%4.2f,r38:%4.2f",r37_v,r38_v);
	LCD_DisplayStringLine(Line1,Display_Buf);
	
	sprintf((char*)Display_Buf,"st:%d  mcp:%4.2f",start_times,mcp_v);
	LCD_DisplayStringLine(Line2,Display_Buf);
	
	sprintf((char*)Display_Buf,"T:%02d-%02d-%02d",rtc_Time.Hours,rtc_Time.Minutes,rtc_Time.Seconds);
	LCD_DisplayStringLine(Line3,Display_Buf);
	
	sprintf((char*)Display_Buf,"D:%02d-%02d-%02d",rtc_Date.Year,rtc_Date.Month,rtc_Date.Date);
	LCD_DisplayStringLine(Line4,Display_Buf);
	
	sprintf((char*)Display_Buf,"f39:%-5d f40:%-5d",f39,f40);
	LCD_DisplayStringLine(Line5,Display_Buf);
	
	sprintf((char*)Display_Buf,"d39:%4.1f d40:%4.1f",d39,d40); 
	LCD_DisplayStringLine(Line6,Display_Buf);
	
	sprintf((char*)Display_Buf,"CCR1:%-5d",TIM16->CCR1); 
	LCD_DisplayStringLine(Line7,Display_Buf);
	

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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_DAC1_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_TIM4_Init();
  MX_TIM15_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

   LCD_Init();
//	 
//	 //EEPROM
//	 I2CInit();
//	
//	 //统计开机次数
//	 if(EEPROM_Read(0x10)!= 123)
//	 {
//		 EEPROM_Write(0x10,123);
//		 EEPROM_Write(0x01,0);
//	 }
//	 
//	 start_times = EEPROM_Read(0x01)+1;
//	 EEPROM_Write(0x01,start_times);
//	 
//	 EEPROM_Write(0x01,0);
//	 EEPROM_Write(0x10,0);
//	 
	 
	 
	 //MCP4017
	 MCP4017_Write(0x20);
	 mcp_val = MCP4017_Read();
	 
	 //UART
	 HAL_UART_Receive_IT(&huart1,uart_buf,1);
	 
	 //TIM_IC
	 HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	 HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
	 
	 //PWM
	 HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
	 HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
	 
	 HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2); 
	
	 //tim15
	 HAL_TIM_Base_Start_IT(&htim15);//开启定时器中断
	 
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

	//Led_Control(0x00);
	//Set_Time(22,11,54);
	//HAL_UART_Transmit(&huart1,tx_buf,sizeof(tx_buf)-1,50);//50ms
	  printf("你好\r\n");
		
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		Led_Control(0x55);
//		HAL_Delay(500);
//		Led_Control(0x00);
//		HAL_Delay(500);
			//Led_Process();
			Key_Process();
			ADC_Process();
			LCD_Process();
			Led_Process(); 
			DAC_Process();
			RTC_Process();
			Rx_Idle();
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
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
