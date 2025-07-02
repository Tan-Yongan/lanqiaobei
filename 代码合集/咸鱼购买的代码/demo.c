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
#include "adc.h"
#include "dac.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
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

//RTC

u8 tim_chose = 0; // 1 2 3 //时 分 秒



//EEPROM
u8 st = 0;

//MCP4017

u8 mcp;

//LED
u8 led_ctrl = 0x00; 
void Led_Proc(void)
{
	
	Led_Control(led_ctrl);
}


//KEY
_Bool double_flag = 0;//双击
uint32_t doubleTick2 = 0;//双击
uint32_t longTick1 = 0;//长按
int num = 0;
uint32_t keyTick = 0;
void Key_Proc(void)
{
	if(uwTick - keyTick < 20)return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Trg & 0x01)
	{
		longTick1 = uwTick;
	}
	else if(Cont & 0x01 && uwTick - longTick1 >1000) //长按
	{
		num++;
	}
	else if(Up & 0x01 && uwTick - longTick1 < 1000) //短按
	{
		num++;
	}

	if(Trg & 0x02 && double_flag == 0)
	{
		doubleTick2 = uwTick;
		double_flag = 1;
	}
	else if(double_flag == 1)
	{
		if(Trg & 0x02 && uwTick - doubleTick2 < 300) //双击
		{
			double_flag = 0;
			num+=10;
		}
		else if(uwTick - doubleTick2 > 300) //单机
		{
			double_flag = 0;
			num++;
		}
	}
	
	
	if(Trg & 0x04)
	{
		//选择时钟
		tim_chose = (tim_chose + 1)%4;
	}
	
	if(Trg & 0x08)
	{
		//LCD翻转
		
		//正向显示
		LCD_Clear(Black);
		LCD_WriteReg(R1,0x0000); //从上往下
		LCD_WriteReg(R96,0x2700);//从左往右
		
		//和正向左右镜面对称
		LCD_WriteReg(R1,0x0000);
		LCD_WriteReg(R96,0xA700);//从右往左
		
	  //和反向左右镜面对称
		LCD_WriteReg(R1,0x0100);//从下往上
		LCD_WriteReg(R96,0x2700);
		
		//反向显示
		LCD_WriteReg(R1,0x0100);
		LCD_WriteReg(R96,0xA700);
		
	}
	
}

//ADC
double r37_v;
double r38_v;
u32 adc_val1,adc_val2;
void ADC_Proc(void) //ADC直接采集的电压会和实际电压相差0.05V
{
	HAL_ADC_Start(&hadc2);
	adc_val1 = HAL_ADC_GetValue(&hadc2);
	r37_v = adc_val1*3.3/4095;
	
	HAL_ADC_Start(&hadc1);
	adc_val2 = HAL_ADC_GetValue(&hadc1);
	r38_v = adc_val2*3.3/4095;
}

//DAC
double PA4_V = 1.25,PA5_V = 3.15;
u32 dac_val1,dac_val2;
void DAC_Proc(void)
{
	dac_val1 = PA4_V/3.3 *4095;
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dac_val1);
	HAL_DAC_Start(&hdac1,DAC_CHANNEL_1);
	
	dac_val2 = PA5_V/3.3 *4095;
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_2,DAC_ALIGN_12B_R,dac_val2);
	HAL_DAC_Start(&hdac1,DAC1_CHANNEL_2);
}

//RTC
u8 hour,min,sec;

void RTC_Proc(void)
{
	RTC_TimeTypeDef Time = {0};
  RTC_DateTypeDef Date = {0};
	HAL_RTC_GetTime(&hrtc,&Time,RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc,&Date,RTC_FORMAT_BIN);
	
	hour = Time.Hours;
	min = Time.Minutes;
	sec = Time.Seconds;
}

void Set_Time(u8 hour,u8 min,u8 sec)
{
	RTC_TimeTypeDef sTime = {0};
	sTime.Hours = hour;
	sTime.Minutes = min;
	sTime.Seconds = sec;
	
	HAL_RTC_SetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
}

//串口
//发
//1
u8 tx_buf[100] = {"您好1!\r\n"};

//2
int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	HAL_UART_Transmit(&huart1,(u8*)&ch,1,50);
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
		//printf("%s\r\n",rx_buf);
		Led_Control(uart_buf[0]);
		
		//HAL_UART_Transmit(&huart1,rx_buf,3,50); //发送定长，可以首字符为00
		printf("%s",rx_buf); //printf发送第一个字符为00时，无法正常发送
		//printf("\r\n");//0d 0a
		rx_cnt = 0;
		memset(rx_buf,'\0',sizeof(rx_buf));
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartTick = uwTick;
	HAL_UART_Receive_IT(&huart1,uart_buf,1);
	
	rx_buf[rx_cnt++] = uart_buf[0];
}


//PWM
uint32_t PA7_F = 8000;
double PA7_D = 0.65;

void PWM_Proc(void)
{
	TIM17->ARR = 1e6/PA7_F - 1;
	TIM17->CCR1 = (TIM17->ARR + 1)*PA7_D;
}	

//IC

uint32_t uwIC2Value1_R39 = 0;
uint32_t uwIC2Value2_R39 = 0;
uint32_t uwDiffCapture = 0;
uint32_t low_R39,high_R39;
/* Capture index */
uint16_t uhCaptureIndex_R39 = 0;

/* Frequency Value */
uint32_t uwFrequency = 0;

uint32_t R39_F;
double R39_D;


uint32_t uwIC2Value1_R40 = 0;
uint32_t uwIC2Value2_R40 = 0;

uint32_t low_R40,high_R40;
/* Capture index */
uint16_t uhCaptureIndex_R40 = 0;

/* Frequency Value */

uint32_t R40_F;
double R40_D;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim3)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if(uhCaptureIndex_R39 == 0)
			{
				/* Get the 1st Input Capture value */
				uwIC2Value1_R39 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
				uhCaptureIndex_R39 = 1;
			}
			else if(uhCaptureIndex_R39 == 1)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2_R39 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
				/* Capture computation */
				if (uwIC2Value2_R39 > uwIC2Value1_R39)
				{
					high_R39 = (uwIC2Value2_R39 - uwIC2Value1_R39); 
				}
				else if (uwIC2Value2_R39 < uwIC2Value1_R39)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					high_R39 = ((0xFFFF - uwIC2Value1_R39) + uwIC2Value2_R39) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */      
				uwIC2Value1_R39 = uwIC2Value2_R39;
				uhCaptureIndex_R39 = 2;
			}
			else if(uhCaptureIndex_R39 == 2)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2_R39 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
				/* Capture computation */
				if (uwIC2Value2_R39 > uwIC2Value1_R39)
				{
					low_R39 = (uwIC2Value2_R39 - uwIC2Value1_R39); 
				}
				else if (uwIC2Value2_R39 < uwIC2Value1_R39)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					low_R39 = ((0xFFFF - uwIC2Value1_R39) + uwIC2Value2_R39) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */      
				uhCaptureIndex_R39 = 0;
				R39_F = 1e6/(low_R39 + high_R39);
				R39_D = high_R39 *1.0/(low_R39 + high_R39);
			}
			
			
		}
	}
	
	if(htim == &htim2)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if(uhCaptureIndex_R40 == 0)
			{
				/* Get the 1st Input Capture value */
				uwIC2Value1_R40 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
				uhCaptureIndex_R40 = 1;
			}
			else if(uhCaptureIndex_R40 == 1)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2_R40 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
				/* Capture computation */
				if (uwIC2Value2_R40 > uwIC2Value1_R40)
				{
					high_R40 = (uwIC2Value2_R40 - uwIC2Value1_R40); 
				}
				else if (uwIC2Value2_R40 < uwIC2Value1_R40)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					high_R40 = ((0xFFFFFFFF - uwIC2Value1_R40) + uwIC2Value2_R40) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */      
				uwIC2Value1_R40 = uwIC2Value2_R40;
				uhCaptureIndex_R40 = 2;
			}
			else if(uhCaptureIndex_R40 == 2)
			{
				/* Get the 2nd Input Capture value */
				uwIC2Value2_R40 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
				/* Capture computation */
				if (uwIC2Value2_R40 > uwIC2Value1_R40)
				{
					low_R40 = (uwIC2Value2_R40 - uwIC2Value1_R40); 
				}
				else if (uwIC2Value2_R40 < uwIC2Value1_R40)
				{
					/* 0xFFFF is max TIM1_CCRx value */
					low_R40 = ((0xFFFFFFFF - uwIC2Value1_R40) + uwIC2Value2_R40) + 1;
				}
				else
				{
					/* If capture values are equal, we have reached the limit of frequency
						 measures */
					Error_Handler();
				}

				/* Frequency computation: for this example TIMx (TIM1) is clocked by
					 APB2Clk */      
				uhCaptureIndex_R40 = 0;
				R40_F = 1e6/(low_R40 + high_R40);
				R40_D = high_R40 *1.0/(low_R40 + high_R40);
			}
			
		}
	}
	
}


//LCD
extern _Bool flag_1s;
uint32_t lcdTick = 0;
void LCD_Proc(void)
{
	if(uwTick - lcdTick < 100)return;
	lcdTick = uwTick;
	
	ADC_Proc(); 
	DAC_Proc();
	
	u8 display_buf[30];
	
//	sprintf((char *)display_buf,"num:%d ST:%d  ",num,st);
//	LCD_DisplayStringLine(Line0,display_buf);
	
//	sprintf((char *)display_buf,"MCP_Read:00%02x  ",mcp);
//	LCD_DisplayStringLine(Line1,display_buf);
	
	sprintf((char *)display_buf,"r37:%4.2f   r38:%4.2f",r37_v,r38_v);
	LCD_DisplayStringLine(Line2,display_buf);
	
	sprintf((char *)display_buf,"R39F:%d  D:%.1f%%   ",R39_F,R39_D*100);
	LCD_DisplayStringLine(Line4,display_buf);
	
	sprintf((char *)display_buf,"R40F:%d  D:%.1f%%   ",R40_F,R40_D*100);
	LCD_DisplayStringLine(Line5,display_buf);
	
  sprintf((char *)display_buf,"PA7F:%d  D:%.1f%%   ",PA7_F,PA7_D*100);
	LCD_DisplayStringLine(Line7,display_buf);
  
  sprintf((char *)display_buf,"R37-J11    R38-J12");
	LCD_DisplayStringLine(Line8,display_buf);
	
  sprintf((char *)display_buf,"R39-J9     R40-J10");
	LCD_DisplayStringLine(Line9,display_buf);
	//                           01 23 4 56 7 89 
	sprintf((char *)display_buf,"T:%02d-%02d-%02d               ",hour,min,sec);
	
	if(tim_chose == 0)
	{
	  LCD_DisplayStringLine(Line0,display_buf);
	}
	else if(tim_chose == 1)   //若要显示可有可无，可以将字体颜色调成背景颜色相同
	{
		u8 i;
		for(i = 0;i<20;i++)
		{
			if(i == 2 ||i == 3)
			{
				LCD_SetBackColor(Green);
				LCD_DisplayChar(Line0,320 - 16*i,display_buf[i]);
				LCD_SetBackColor(Black);
			}
			else
			LCD_DisplayChar(Line0,320 - 16*i,display_buf[i]);
		}
	}
	else if(tim_chose == 2)
	{
		u8 i;
		for(i = 0;i<20;i++)
		{
			if(i == 5 ||i == 6)
			{
				LCD_SetBackColor(Green);
				LCD_DisplayChar(Line0,320 - 16*i,display_buf[i]);
				LCD_SetBackColor(Black);
			}
			else
			LCD_DisplayChar(Line0,320 - 16*i,display_buf[i]);
		}
	}
	else if(tim_chose == 3)
	{
		u8 i;
		for(i = 0;i<20;i++)
		{
			if(i == 8 ||i == 9)
			{
				LCD_SetBackColor(Green);
				LCD_DisplayChar(Line0,320 - 16*i,display_buf[i]);
				LCD_SetBackColor(Black);
			}
			else
			LCD_DisplayChar(Line0,320 - 16*i,display_buf[i]);
		}
	}
	
}

//TIM15 -- 100ms
//799 - 9999
u8 cnt_1s = 0;
_Bool flag_1s = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim15)
	{
		if(++cnt_1s == 10)
		{
			cnt_1s = 0;
			flag_1s = !flag_1s;
			//led_ctrl ^= 0xff;
		}
	}
	
}


//int led_cnt = 0;
//void SysTick_Handler(void)
//{
//  /* USER CODE BEGIN SysTick_IRQn 0 */

//  /* USER CODE END SysTick_IRQn 0 */
//  HAL_IncTick();
//  /* USER CODE BEGIN SysTick_IRQn 1 */
//	if(++led_cnt == 1000)
//	{
//		led_cnt = 0;
//		led_ctrl^=0xff;
//	}

//  /* USER CODE END SysTick_IRQn 1 */
//}


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
  MX_TIM15_Init();
  MX_USART1_UART_Init();
  MX_TIM17_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	
	LCD_Init(); 
	
	//I2C
	I2CInit();
	
//	if(EEPROM_Read(0x01) != 123)
//	{
//		EEPROM_Write(0x01,123);
//		EEPROM_Write(0x02,0);
//	}
//	
//	st = EEPROM_Read(0x02)+1;
//	EEPROM_Write(0x02,st);
	
//	EEPROM_Write(0x01,0);
//	EEPROM_Write(0x02,0);
//	
//	//MCP4017
//	MCP_Write(0x3F);
//	mcp = MCP_Read();
	
   //RTC
	 Set_Time(0,0,0);
	 
	 //TIM15
	 HAL_TIM_Base_Start_IT(&htim15);
	 
	 //UART
	 HAL_UART_Receive_IT(&huart1,uart_buf,1);
	 
	 //PWM
	 HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
	 
	 //TIM_IC
	 HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	 HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	  LCD_Clear(Black);
		LCD_SetBackColor(Black);
		LCD_SetTextColor(White);
		//LCD_DisplayStringLine(Line4, (unsigned char *)"    Hello,world.   ");
		
	 Led_Control(0x00);
//	HAL_UART_Transmit(&huart1,(u8 *)tx_buf,sizeof(tx_buf)-1,50);//两次普通发送之间要有间隔。
//	HAL_UART_Transmit(&huart1,(u8 *)"我没号\r\n",sizeof("我没号\r\n")-1,50);
	 printf("你好\r\n");

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		Key_Proc();
		//Led_Proc();
		LCD_Proc();
		RTC_Proc();
		Rx_Idle();
		PWM_Proc();
		
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
