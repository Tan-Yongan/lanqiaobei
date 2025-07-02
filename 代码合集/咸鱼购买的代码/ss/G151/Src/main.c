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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "lcd.h"

#include "led.h"
#include "key.h"

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
#define Display_PARA 1
#define Display_RECD 2

u8 Display_Mode = 0;


//性能要求

//频率1秒刷新10次
int PA15_FF;
int PB4_FF;

uint32_t PA15_T;
uint32_t PB4_T;

//1
//频率小数转换
double PA15_fl;
double PB4_fl;

//周期//默认为us为单位,周期转换
double PA15_ti;
double PB4_ti;

_Bool data_flag = 0;//0 显示频率 1显示周期


//2
int PD = 1000;
int PH = 5000;
int PX = 0;

u8 para_chose = 0;

//3
int NDA = 0;
int NDB = 0;
int NHA = 0;
int NHB = 0;



//LED
u8 led_ctrl = 0x00;
void LED_Proc(void)
{
	if(Display_Mode == Display_DATA)
		led_ctrl |= 0x01;
	else
		led_ctrl &=~0x01;
	
	if(PA15_FF > PH)
		led_ctrl |= 0x02;
	else
		led_ctrl &=~0x02;
	
	if(PB4_FF > PH)
		led_ctrl |= 0x04;
	else
		led_ctrl &= ~0x04;
	
	if(NDA >=3 ||NDB >=3)
	{
		led_ctrl |= 0x80;
	}
	else
		led_ctrl &=~0x80;
	
	
	Led_Control(led_ctrl);
}

//KEY

uint32_t B3Tick = 0;
uint32_t keyTick = 0;
void Key_Proc(void)
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Trg & 0x01)
	{
		if(Display_Mode == Display_PARA)
		{
			if(para_chose == 0)
			{
				PD+=100;
				if(PD >= 1100)
					PD = 1000;
			}
			
			if(para_chose == 1)
			{
				PH+=100;
				if(PH >= 10100)
					PH = 10000;
			}
		
			if(para_chose == 2)
			{
				PX+=100;
				if(PX >= 1100)
					PX = 1000;
			}
		}
	}
	
	if(Trg & 0x02)
	{
		if(Display_Mode == Display_PARA)
		{
			if(para_chose == 0)
			{
				if(PD >= 200)
				   PD-=100;
			}
			
			if(para_chose == 1)
			{
				if(PH >= 1100)
				   PH-=100;
			}
		
			if(para_chose == 2)
			{
				if(PX >= -900)
				   PX-=100;
			}
		}
	}
	
	
	//长短按
	if(Trg & 0x04)
	{
		B3Tick = uwTick;
	}
	else if(Cont & 0x04 && uwTick - B3Tick > 1000)//长按
	{
		if(Display_Mode == Display_RECD)
		{
			NDA = 0;
			NDB = 0;
			
			NHA = 0;
			NHB = 0;
			
		}
	
	}
	else if(Up & 0x04 && uwTick - B3Tick < 1000)//短按
	{
		
		if(Display_Mode == Display_DATA)
		{
			LCD_Clear(Black);
			data_flag = !data_flag;
		}
		
		if(Display_Mode == Display_PARA)
		{
			para_chose = (para_chose + 1)%3;
		}
	}
	
	
	
	if(Trg & 0x08)
	{
		LCD_Clear(Black);
		Display_Mode = (Display_Mode + 1)%3;
		
		if(Display_Mode == Display_DATA)
		{
			data_flag = 0;
		}
		
		if(Display_Mode == Display_PARA)
		{
			para_chose = 0;
		}
	}
	

}


//PA15 -TIM2CH1
//PB4 TIM16CH1


uint32_t uwIC2Value1_PA15 = 0;
uint32_t uwIC2Value2_PA15 = 0;
uint32_t uwDiffCapture_PA15 = 0;

/* Capture index */
uint16_t uhCaptureIndex_PA15 = 0;

/* Frequency Value */
uint32_t uwFrequency = 0;

uint32_t PA15_F;
//
uint32_t uwIC2Value1_PB4 = 0;
uint32_t uwIC2Value2_PB4 = 0;
uint32_t uwDiffCapture_PB4 = 0;

/* Capture index */
uint16_t uhCaptureIndex_PB4 = 0;

/* Frequency Value */

uint32_t PB4_F;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
			if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			{
				if(uhCaptureIndex_PA15 == 0)
				{
					/* Get the 1st Input Capture value */
					uwIC2Value1_PA15 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
					uhCaptureIndex_PA15 = 1;
				}
				else if(uhCaptureIndex_PA15 == 1)
				{
					/* Get the 2nd Input Capture value */
					uwIC2Value2_PA15 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 

					/* Capture computation */
					if (uwIC2Value2_PA15 > uwIC2Value1_PA15)
					{
						uwDiffCapture_PA15 = (uwIC2Value2_PA15 - uwIC2Value1_PA15); 
					}
					else if (uwIC2Value2_PA15 < uwIC2Value1_PA15)
					{
						/* 0xFFFF is max TIM1_CCRx value */
						uwDiffCapture_PA15 = ((0xFFFFFFFF - uwIC2Value1_PA15) + uwIC2Value2_PA15) + 1;
					}
					else
					{
						/* If capture values are equal, we have reached the limit of frequency
							 measures */
						Error_Handler();
					}

					/* Frequency computation: for this example TIMx (TIM1) is clocked by
						 APB2Clk */      
					PA15_F = 1e6/uwDiffCapture_PA15;
					uhCaptureIndex_PA15 = 0;
				}
			}

		
	}
	
	if(htim == &htim16)
	{
			if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			{
				if(uhCaptureIndex_PB4 == 0)
				{
					/* Get the 1st Input Capture value */
					uwIC2Value1_PB4 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
					uhCaptureIndex_PB4 = 1;
				}
				else if(uhCaptureIndex_PB4 == 1)
				{
					/* Get the 2nd Input Capture value */
					uwIC2Value2_PB4 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 

					/* Capture computation */
					if (uwIC2Value2_PB4 > uwIC2Value1_PB4)
					{
						uwDiffCapture_PB4 = (uwIC2Value2_PB4 - uwIC2Value1_PB4); 
					}
					else if (uwIC2Value2_PB4 < uwIC2Value1_PB4)
					{
						/* 0xFFFF is max TIM1_CCRx value */
						uwDiffCapture_PB4 = ((0xFFFF - uwIC2Value1_PB4) + uwIC2Value2_PB4) + 1;
					}
					else
					{
						/* If capture values are equal, we have reached the limit of frequency
							 measures */
						Error_Handler();
					}

					/* Frequency computation: for this example TIMx (TIM1) is clocked by
						 APB2Clk */      
					PB4_F = 1e6/uwDiffCapture_PB4;
					uhCaptureIndex_PB4 = 0;
				}
			}
		
	}
}


//频率超限
 //是否需要考虑第一上电超限？？？
int PA15_F_temp = 5000; //这里设置和上电超限参数相等即可
int PB4_F_temp  = 5000;

uint32_t fhTick = 0;
void FH_Proc(void)
{
	if(uwTick - fhTick < 100) return;
	fhTick = uwTick;
	
	
	if(PA15_FF > PH && PA15_F_temp <= PH)
	{
		NHA++;
	}
	
	if(PB4_FF > PH && PB4_F_temp <= PH)
	{
		NHB++;
	}
  
	PA15_F_temp = PA15_FF;
	PB4_F_temp = PB4_FF;
	
}


//频率采样
int a_cy[32];//0-29个数

u8 a_cnt = 0;

int a_max = -600;
int a_min = 21000;

//
int b_cy[32];//0-29个数

u8 b_cnt = 0;

int b_max = -600;
int b_min = 21000;

void FD_Proc(void)  //第一次上电
{
		if(a_cnt >= 30) //采样完成
		{
			a_cnt = 0;
			
			u8 i;
			
			for(i = 0;i< 30;i++)
			{
				if(a_cy[i] >= a_max)
				{
					 a_max = a_cy[i];
				}

				
			}
			
			for(i = 0;i< 30;i++)
			{
				if(a_min >= a_cy[i])
				{
					//if(a_cy[i] != 0) //程序跳变的bug
					 a_min = a_cy[i];
				}

			}
			
			if((a_max - a_min) > PD)
			{
				NDA++;
   
			}
			
			a_max = -600;
			a_min = 21000;
			
		}
		
		if(b_cnt >= 30) //采样完成
		{
			b_cnt = 0;
			
			u8 i;
			
			for(i = 0;i< 30;i++)
			{
				if(b_cy[i] >= b_max)
				{
					 b_max = b_cy[i];
				}

				
			}
			
			for(i = 0;i< 30;i++)
			{
				if(b_min >= b_cy[i])
				{
					//if(a_cy[i] != 0) //程序跳变的bug
					 b_min = b_cy[i];
				}

			}
			
			if((b_max - b_min) > PD)
			{
				NDB++;
			}
			
			
			b_max = -600;
			b_min = 21000;
			
		}

}




//LCD
uint32_t lcdTick = 0;
void LCD_Proc(void)
{
	if(uwTick - lcdTick < 100) return;
	lcdTick = uwTick;
	
	u8 display_buf[30];
	

	if(Display_Mode == Display_DATA)
	{
	  sprintf((char*)display_buf,"        DATA ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		if(data_flag == 0)
		{
			 //A
				if(PA15_FF <= 1000 && PA15_FF >= 0)
				{
					sprintf((char*)display_buf,"     A=%dHz           ",PA15_FF);
				}

				else if(PA15_FF > 1000)
				{
					PA15_fl = (double)PA15_FF/1000.0;
					sprintf((char*)display_buf,"     A=%.2fKHz        ",PA15_fl);//左对齐
				}
				
				else if(PA15_FF < 0)
				{
					sprintf((char*)display_buf,"     A=NULL            ");
				}
				LCD_DisplayStringLine(Line3,display_buf);
				
				//B
				if(PB4_FF <= 1000 && PB4_FF >= 0)
				{
					sprintf((char*)display_buf,"     B=%dHz            ",PB4_FF);
				}

				else if(PB4_FF > 1000)
				{
					PB4_fl = (double)PB4_FF/1000.0;
					sprintf((char*)display_buf,"     B=%.2fKHz         ",PB4_fl);
				}
				else if(PB4_FF < 0)
				{
					sprintf((char*)display_buf,"     B=NULL             ");
				}
				LCD_DisplayStringLine(Line4,display_buf);
		


		}
		else //显示周期
		{
			if(PA15_FF >= 0)
			{
			  //默认为us为单位
				if(PA15_T <= 1000)
				{
					sprintf((char*)display_buf,"     A=%duS           ",PA15_T);
				}

				else
				{
					PA15_ti = (double)PA15_T/1000.0;
					sprintf((char*)display_buf,"     A=%.2fmS        ",PA15_ti);
				}
				
			}
			else
			{
					sprintf((char*)display_buf,"     A=NULL           ");
			}
			
			LCD_DisplayStringLine(Line3,display_buf);
			
			if(PB4_FF >= 0)
			{
				//
				if(PB4_T <= 1000)
				{
					sprintf((char*)display_buf,"     B=%duS           ",PB4_T);
				}

				else
				{
					PB4_ti = (double)PB4_T/1000.0;
					sprintf((char*)display_buf,"     B=%.2fmS        ",PB4_ti);
				}
			}
			else
			{
				 sprintf((char*)display_buf,"     B=NULL           ");
			}

			LCD_DisplayStringLine(Line4,display_buf);
		}
	}
	
	if(Display_Mode == Display_PARA)
	{
	  sprintf((char*)display_buf,"        PARA ");
		LCD_DisplayStringLine(Line1,display_buf);
		
	  sprintf((char*)display_buf,"     PD=%dHz        ",PD);
		LCD_DisplayStringLine(Line3,display_buf);
		
	  sprintf((char*)display_buf,"     PH=%dHz        ",PH);
		LCD_DisplayStringLine(Line4,display_buf);
		
	  sprintf((char*)display_buf,"     PX=%dHz        ",PX);
		LCD_DisplayStringLine(Line5,display_buf);
	}
	
	if(Display_Mode == Display_RECD)
	{
	  sprintf((char*)display_buf,"        RECD ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		
		
		if(NDA >= 0)
	  sprintf((char*)display_buf,"     NDA=%d        ",NDA);
		else
		sprintf((char*)display_buf,"     NDA=%d        ",0);
		
		
		LCD_DisplayStringLine(Line3,display_buf);
		
		if(NDB >= 0)
	  sprintf((char*)display_buf,"     NDB=%d        ",NDB);
		else
	  sprintf((char*)display_buf,"     NDB=%d        ",0);
		LCD_DisplayStringLine(Line4,display_buf);
		
		
	  sprintf((char*)display_buf,"     NHA=%d        ",NHA);
		LCD_DisplayStringLine(Line5,display_buf);
		
	  sprintf((char*)display_buf,"     NHB=%d        ",NHB);
		LCD_DisplayStringLine(Line6,display_buf);
	}
	
}


//TIM15 100ms
//int led_cnt = 0;
//1s刷新10次-->100ms刷新一次

//频率突变
//频率的正负？

//这里先可以考虑负频率进行计算


// 间隔3s，间隔采样？，以100ms为间隔进行采样，以3秒为周期进行判断

//采样 3s，100ms为周期，共采样30个   //这里刚好对应 频率每秒刷新10次

//考虑第一次采样满

//第三秒的时候判断？，还是每次采样完成后判断
//先考虑第三秒时后判断

_Bool PA15_flag = 0;
_Bool PB4_flag = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim == &htim15)//100ms
	{

  //测试用
//		if(++led_cnt == 10)
//		{
//			led_cnt = 0;
//			led_ctrl^=0xff;
//		}
		
   //频率刷新 和周期刷新
		PA15_FF = ((int)PA15_F + PX);
		PB4_FF = ((int)PB4_F + PX);
		
		

			
			if(PA15_FF > 1200 && PA15_flag == 0)
			{
				NDA = -1;
				PA15_flag = 1;
			}
			if(PB4_FF > 1200 && PB4_flag == 0)
			{
				NDB = -1;
				PB4_flag = 1;
			}
		
		//采样
		a_cy[a_cnt++] = PA15_FF;
		b_cy[b_cnt++] = PB4_FF;
		
	  //LCD显示
		if(PA15_FF > 0) //考虑小于0时候
		{
		  PA15_T = (double)(1e6/PA15_FF);
		}
		
    if(PB4_FF > 0)
		{
			PB4_T = (double)(1e6/PB4_FF);
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
  MX_TIM2_Init();
  MX_TIM15_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
	
	LCD_Init();
	
	
	
	//PWM_IC
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim16,TIM_CHANNEL_1);
	
	//TIM15
	HAL_TIM_Base_Start_IT(&htim15);
	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
	  LCD_Clear(Black);
   // LCD_DisplayStringLine(Line4, (unsigned char *)"    Hello,world.   ");
		
	Led_Control(0x00);
	//memset(a_cy,10000,sizeof(a_cy));
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		Key_Proc();
		LCD_Proc();
		LED_Proc();
		FH_Proc();
		FD_Proc();
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
