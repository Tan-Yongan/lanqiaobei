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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "key.h"
#include "string.h"
#include "stdio.h"
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

#define Display_DATA 0
#define Display_PARA 1
#define Display_RECD 2
#define Display_FSET 3

u8 Display_Mode = 0;


//回放思想：保存在数组里即可
//记录时间  2- 10s  ,100ms采样一次状态，最多采样100次，即10s



//记录pwm
_Bool record_st = 0;
u8 rec_index = 0;
uint32_t record_f[110];
double record_d[110];

//回放pwm

_Bool playback_st = 0;

u8 bac_index = 0;



//记录电压

u8 rec_index_v = 0;
double record_v[110];


//回放电压，考虑利用DAC进行回放
_Bool playback_st_v = 0;



//1



//2

u8 para_chose = 0;//

uint32_t FH = 2000;
double AH = 3.0;
u8 TH = 30;


uint32_t FH_temp = 2000;
double AH_temp = 3.0;
u8 TH_temp = 30;


//3
int FN;
int AN;
int TN;


//4

u8 fset_chose = 0;
uint32_t FP = 1;
double VP = 0.9;
int TT = 6;

uint32_t FP_temp = 1;
double VP_temp = 0.9;
int TT_temp = 6;



//DAC
double PA4_V = 2.1;
u32 dac_val;

void DAC_Proc(void)
{
	dac_val = PA4_V/3.3 * 4095;
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dac_val);
	
	HAL_DAC_Start(&hdac1,DAC_CHANNEL_1);


}




//DS18B20
double temp = 0;
int temp_cnt = 0;


//KEY

_Bool key34_st = 0;
u8 key34_cnt = 0;


uint32_t keyTick = 0;
void Key_Proc(void)
{
	if(uwTick - keyTick < 20)return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Trg & 0x01 && record_st == 0)
	{
		LCD_Clear(Black);
		Display_Mode = (Display_Mode + 1)%4;
		
		if(Display_Mode == Display_PARA) //退出生效，则有效值为temp
		{
			para_chose = 0;
			FH = FH_temp;
			AH = AH_temp;
			TH = TH_temp;
		}
		else
		{
			FH_temp = FH;
			AH_temp = AH;
			TH_temp = TH;
		}
			
		
		
		if(Display_Mode == Display_FSET)
		{
			fset_chose = 0;
			FP = FP_temp;
			VP = VP_temp;
			TT = TT_temp;
   	}
		else
		{
			FP_temp = FP;
			VP_temp = VP;
			TT_temp = TT;
		}
	}
	
	if(Trg & 0x02 && record_st == 0)
	{
		if(Display_Mode == Display_DATA)
		{
			record_st = 1;
			rec_index = 0;
			
		}
		
		if(Display_Mode == Display_PARA)
		{
			para_chose = (para_chose + 1)%3;
		}
		
		if(Display_Mode == Display_FSET)
		{
			fset_chose = (fset_chose + 1)%3;
		}
		
		if(Display_Mode == Display_RECD)
		{
			FN = 0;
			AN = 0;
			TN = 0;
		}
	}
	
	if(Trg & 0x04 && record_st == 0)
	{
		if(Display_Mode == Display_DATA)
		{
			if(rec_index > 0)
			{
				playback_st_v = 1;
			}
		}
		
		
		if(Display_Mode == Display_PARA)
		{
			if(para_chose == 0)
			{
				FH+=1000;
				if(FH == 11000)
					FH = 10000;
			}
		
			if(para_chose == 1)
			{
				AH+=0.3;
				if(AH > 3.31)
					AH = 3.3;
			}
			
			if(para_chose == 2)
			{
				TH+=1;
				if(TH == 81 )
					TH = 80;
			}
			
		}
		
		if(Display_Mode == Display_FSET)
		{
			if(fset_chose == 0)
			{
				FP+=1;
				if(FP == 11)
					FP = 10;
			}
		
			if(fset_chose == 1)
			{
				VP+=0.3;
				if(VP > 3.31)
					VP = 3.3;
			}
			
			if(fset_chose == 2)
			{
				TT+=2;
				if(TT == 12 )
					TT = 10;
			}
		}
		
	}
	
	if(Trg & 0x08 && record_st == 0)
	{
		if(Display_Mode == Display_DATA)
		{
			if(rec_index > 0)
			{
				playback_st = 1;
			}
		}
		
		
		if(Display_Mode == Display_PARA)
		{
			if(para_chose == 0)
			{
				FH-=1000;
				if(FH == 0)
					FH = 1000;
			}
		
			if(para_chose == 1)
			{
				AH-=0.3;
				if(AH < 0.01)
					AH = 0;
			}
			
			if(para_chose == 2)
			{
				if(TH > 0)
				   TH-=1;
				
			}
			
		}
		
		if(Display_Mode == Display_FSET)
		{
			if(fset_chose == 0)
			{
				FP-=1;
				if(FP == 0)
					FP = 1;
			}
		
			if(fset_chose == 1)
			{
				VP-=0.3;
				if(VP < 0.01)
					VP = 0;
			}
			
			if(fset_chose == 2)
			{
				TT-=2;
				if(TT == 0 )
					TT = 2;
			}
		}
		
		
	}
	
	if(Cont & 0x0C  && record_st == 0)
	{
		key34_st = 1;
	}
	else
		key34_st = 0;
	
}


//PA1
uint32_t uwIC2Value1 = 0;
uint32_t uwIC2Value2 = 0;
uint32_t uwDiffCapture = 0;

uint32_t low,high;

/* Capture index */
uint16_t uhCaptureIndex = 0;

/* Frequency Value */
uint32_t uwFrequency = 0;

uint32_t PA1_F = 0;  //频率就这样显示即可
double PA1_D = 0;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) //当有输入捕获和其他定时器（systick除外）时，其他定时器响应和抢占优先级都低//分组为2
{
	if(htim == &htim2)
	{
			if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
			{
				if(uhCaptureIndex == 0)
				{
					/* Get the 1st Input Capture value */
					uwIC2Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
					__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);
					uhCaptureIndex = 1;
				}
				else if(uhCaptureIndex == 1)
				{
					/* Get the 2nd Input Capture value */
					uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); 
          __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
					/* Capture computation */
					if (uwIC2Value2 > uwIC2Value1)
					{
						high = (uwIC2Value2 - uwIC2Value1); 
					}
					else if (uwIC2Value2 < uwIC2Value1)
					{
						/* 0xFFFF is max TIM1_CCRx value */
						high = ((0xFFFFFFFF - uwIC2Value1) + uwIC2Value2) + 1;
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
					uwIC2Value1  = uwIC2Value2;
				}
				else if(uhCaptureIndex == 2)
				{
					/* Get the 2nd Input Capture value */
					uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); 
					/* Capture computation */
					if (uwIC2Value2 > uwIC2Value1)
					{
						low = (uwIC2Value2 - uwIC2Value1); 
					}
					else if (uwIC2Value2 < uwIC2Value1)
					{
						/* 0xFFFF is max TIM1_CCRx value */
						low = ((0xFFFFFFFF - uwIC2Value1) + uwIC2Value2) + 1;
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
					
					PA1_F = 1e6/(low + high);
					PA1_D = low*1.0/(low + high);
				}
				
			}
	}
	
}


//PA7

uint32_t PA7_F = 8000;
double PA7_D = 0; //低电平为0，高电平为1
void PWM_Proc(void)
{
	TIM17->ARR = 1e6/PA7_F - 1;
	TIM17->CCR1 = (TIM17->ARR + 1)*PA7_D;
}


//ADC

double r37_v;
void ADC_Proc(void)
{
	HAL_ADC_Start(&hadc2);
	r37_v = HAL_ADC_GetValue(&hadc2)/4095.0 * 3.3;

}
	

//报警 比较 累加   
uint32_t F_temp = 2000; //初值设置和初始阈值相等，防止刚上电累加//考虑刚上电时候大于阈值累加
double r37_v_temp = 3.0;
u8 temp_temp = 30;

uint32_t addTick = 0;
void Add_Proc(void)
{
	if(uwTick - addTick < 100) return;
	addTick = uwTick;
	
	if(PA1_F > FH && F_temp <= FH)
	{
		FN++;
	}
  
	if(r37_v > AH && r37_v_temp <= AH)
	{
		AN++;
	}
	
	if(temp > TH && temp_temp <= TH)
	{
		TN++;
	}
	
	
	F_temp = PA1_F;
	r37_v_temp = r37_v;
	temp_temp = temp;

}


//LED
u8 led_ctrl = 0x00;
void Led_Proc(void)
{
	
	if(PA1_F > FH_temp)
	{
		led_ctrl |= 0x08;
	}
	else
		led_ctrl &= ~0x08;
	
	if(r37_v > AH_temp)
	{
		led_ctrl |= 0x10;
	}
	else
		led_ctrl &= ~0x10;
	
	if(temp > TH_temp)
	{
		led_ctrl |= 0x20;
	}
	else
		led_ctrl &= ~0x20;
	
	
	Led_Control(led_ctrl);
}



//LCD
uint32_t lcdTick = 0;
void LCD_Proc(void)
{
	if(uwTick - lcdTick < 100) return;
	lcdTick = uwTick;
	
	ADC_Proc();
	
	DAC_Proc();
	
	u8 display_buf[30];

	
	if(Display_Mode == Display_DATA)
	{
		sprintf((char*)display_buf,"        DATA");
		LCD_DisplayStringLine(Line1,display_buf);	
		
		sprintf((char*)display_buf,"     F=%d       %d",PA1_F,FN);
		LCD_DisplayStringLine(Line3,display_buf);	
		
		sprintf((char*)display_buf,"     D=%.0f%%   ",PA1_D*100);
		LCD_DisplayStringLine(Line4,display_buf);	
		
		sprintf((char*)display_buf,"     A=%.1f     %d",r37_v,AN);
		LCD_DisplayStringLine(Line5,display_buf);	
		
		sprintf((char*)display_buf,"     T=%.1f     %d",temp,TN);
		LCD_DisplayStringLine(Line6,display_buf);	
	}
	
	if(Display_Mode == Display_PARA)
	{
		sprintf((char*)display_buf,"        PARA ");
		LCD_DisplayStringLine(Line1,display_buf);	
		
		sprintf((char*)display_buf,"     FH=%d        ",FH);
		LCD_DisplayStringLine(Line3,display_buf);	
		
		sprintf((char*)display_buf,"     AH=%.1f   ",AH);
		LCD_DisplayStringLine(Line4,display_buf);	
		
		sprintf((char*)display_buf,"     TH=%d     ",TH);
		LCD_DisplayStringLine(Line5,display_buf);	
			
	}
	
	if(Display_Mode == Display_RECD)
	{
		sprintf((char*)display_buf,"        RECD");
		LCD_DisplayStringLine(Line1,display_buf);	
		
		sprintf((char*)display_buf,"     FN=%d     ",FN);
		LCD_DisplayStringLine(Line3,display_buf);	
		
		sprintf((char*)display_buf,"     AN=%d     ",AN);
		LCD_DisplayStringLine(Line4,display_buf);	
		
		sprintf((char*)display_buf,"     TN=%d     ",TN);
		LCD_DisplayStringLine(Line5,display_buf);	
	}
	
	if(Display_Mode == Display_FSET)
	{
		sprintf((char*)display_buf,"        FSET");
		LCD_DisplayStringLine(Line1,display_buf);	
		
		sprintf((char*)display_buf,"     FP=%d     ",FP);
		LCD_DisplayStringLine(Line3,display_buf);	
		
		sprintf((char*)display_buf,"     VP=%.1f     ",VP);
		LCD_DisplayStringLine(Line4,display_buf);	
		
		sprintf((char*)display_buf,"     TT=%d     ",TT);
		LCD_DisplayStringLine(Line5,display_buf);	
	}
	
}

//TIM15 - 100ms -- 存放变量或者简单动作(非读数)
//799 - 9999
//u8 ledcnt = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim15)
	{
//		if(++ledcnt == 2)
//		{
//			ledcnt = 0;
//			led_ctrl ^=0x02;
//		}
	    if(key34_st == 1)
			{
				if(++key34_cnt == 20)
				{
					key34_cnt = 0;
					key34_st = 0;
					LCD_Clear(Black);
					Display_Mode = Display_DATA;
					
					FH = FH_temp = 2000;
					AH = AH_temp = 3.0;
					TH = TH_temp = 30;
					
					FP = FP_temp = 1;
					VP = VP_temp = 0.9;
					TT = TT_temp = 6;
					
					FN = AN = TN = 0;
					para_chose = fset_chose = 0;
					
				  F_temp = 2000; //初值设置和初始阈值相等，防止刚上电累加//考虑刚上电时候大于阈值累加
			    r37_v_temp = 3.0;
				  temp_temp = 30;
				}
			}
			else
			{
				key34_cnt = 0;
			}
			
			//记录
			if(record_st == 1 )
			{
				//记录pwm
				record_f[++rec_index] = PA1_F; //存放1-60个数据,共计60个数据,（仅保留最近一次的一组数据）
				record_d[rec_index]   = PA1_D;
				
				//记录电压
				record_v[rec_index] = r37_v;
				
				led_ctrl^= 0x01;
				
				
				if(rec_index == TT * 10) //记录完成
				{
					record_st = 0;
          led_ctrl &=~0x01;
				}
				
			}
			
			//回放pwm
			if(playback_st == 1)
			{
				
				PA7_F = record_f[++bac_index]/FP_temp; //注意有效值
        PA7_D = record_d[bac_index];
				
				
				led_ctrl ^= 0x02;
				if(bac_index == TT * 10)
				{
					playback_st = 0;
					bac_index = 0;
					PA7_F = 1000;
					PA7_D = 0; //低电平
					
					led_ctrl &= ~0x02;
				}
			}
			
			//回放电压
			if(playback_st_v == 1)
			{
				
				PA7_F = 1000; 
				
//				if(record_v[++bac_index] <= VP_temp)
//				{
//					PA7_D = 0.1;
//				}
//				else
//				{
//					PA7_D = (record_v[bac_index] - 3.3)*0.9/(3.3 - VP_temp) + 1;
//				}
				
				PA4_V = record_v[++bac_index];
				led_ctrl ^= 0x04;
				
				if(bac_index == TT * 10)
				{
					playback_st_v = 0;
					bac_index = 0;
					PA7_F = 1000;
					PA7_D = 0; //低电平
					
					led_ctrl &= ~0x04;
					
					PA4_V = 0;
				}
			}
		
	}
}



void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
	if(++temp_cnt == 500)   //固定了，由于优先级都是最高，温度只能放systick
		{
			temp_cnt = 0;
			temp = DS18B20_Read();
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
  MX_TIM2_Init();
  MX_TIM17_Init();
  MX_TIM15_Init();
  MX_DAC1_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		//PA1
		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
		
		//PA7
		HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
		
		
		
		//DS18B20
		ds18b20_init_x();
		
		while(DS18B20_Read() >= 85){};
			
		//TIM15 -- 用新的定时器会干扰频率捕获
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
			LCD_Proc();
			PWM_Proc();
			Key_Proc();
			Add_Proc();
			Led_Proc();
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
