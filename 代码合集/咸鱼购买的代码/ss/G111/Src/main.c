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

#define Display_Data 0
#define Display_Para 1

u8 Display_Mode = 0;


//1
_Bool tim_flag = 0; //0计时结束，1计时中

u32 sec = 0;
u32 sec_cnt = 0;


//2
double vmax_set = 3;
double vmin_set = 2;

double vmax_com = 3;
double vmin_com = 2;



//KEY
uint32_t keyTick = 0;
_Bool key_flag = 0;
void Key_Process(void)
{
	if(uwTick - keyTick < 20) return;
	keyTick = uwTick;
	
	Key_Read();
	
	if(Trg & 0x01)
	{
		LCD_Clear(Black);
		Display_Mode = (Display_Mode + 1)%2;
		
		if(Display_Mode == Display_Data)
		{
			if(vmax_set >= (vmin_set + 1))
			{
				vmax_com = vmax_set;
				vmin_com = vmin_set;
				key_flag = 0;
			}
			else
				key_flag = 1;
		}
		
		if(Display_Mode == Display_Para) // vmax_com,vmin_com 为确定参数，set为设置参数，在设置界面需要显示set。
		{
			vmax_set = vmax_com;
			vmin_set = vmin_com;//进入界面和退出界面值相反，即可实现退出界面生效
		}
	}
	
	if(Trg & 0x02)
	{
		if(Display_Mode == Display_Para)
		{
			vmax_set += 0.1;
			if(vmax_set > 3.31) //小数不精确问题
				vmax_set = 0;
		}
	}
	
	if(Trg & 0x04)
	{
		if(Display_Mode == Display_Para)
		{
			vmin_set += 0.1;
			if(vmin_set >3.31)
				vmin_set = 0;
		}
	}
	
	if(Trg & 0x08)
		Led_Control(0x08);
}


//ADC
u32 adc_val;
u32 val_sum = 0;
u8 val_cnt = 0;
//对于时间态变量，开始赋初值以防第一次上电误触发
double r37_v = 3.3;
double r37_v_pre = 3.3;


void ADC_Process(void)
{
	HAL_ADC_Start(&hadc2);
	adc_val = HAL_ADC_GetValue(&hadc2);
	
	val_cnt++;
	val_sum += adc_val;
	if(val_cnt == 10)
	{
		val_cnt = 0;
		r37_v = val_sum /10.0f/ 4095.0f*3.3f;//均值滤波
		val_sum = 0;
	}
	
}

//计时
//!!!
uint32_t timTick = 0; //判断上一时刻值和本时刻值时需要间隔一段时间，不然会误触发

void Time_Process(void)
{
	
	if(uwTick - timTick < 100) return; //100ms左右，和100ms之前的值对比 ,防止在电压一直下滑过程中重新计时误判
	timTick = uwTick;
	
	//先判断，再更新旧值
	if(r37_v_pre < vmin_com && r37_v >= vmin_com) //计时开始
	{
		sec = 0;
		sec_cnt = 0;
		tim_flag = 1;
	}
	
	if(r37_v_pre < vmax_com && r37_v >= vmax_com) //停止计时
	{
		tim_flag = 0; 
	}
	
	r37_v_pre = r37_v;//记录上一次的adc
	
}


void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
	
	if(tim_flag == 1)
	{
		sec_cnt++;
		if(sec_cnt == 1000)
		{
			sec_cnt = 0;
			sec++;
		}
	}

  /* USER CODE END SysTick_IRQn 1 */
}

//UART
//发送

u8 tx_buf[] = {"你好\r\n"};
int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	HAL_UART_Transmit(&huart1,(u8 *)&ch,1,50);
  return ch;
}

//接受
u8 uart_buf[2];
u8 rx_cnt = 0;
u8 rx_buf[100];
_Bool uart_flag = 0;

uint32_t uartTick = 0;
void Rx_Idle(void)
{
	if(uwTick - uartTick < 50) return;
	uartTick = uwTick;
	
	if(rx_cnt > 0)
	{
		if(rx_cnt < 7)
		{
			uart_flag = 1;
			printf("length error!\r\n");
		}
		rx_cnt = 0;
		memset(rx_buf,'\0',sizeof(rx_buf));
	}
}



//3.2,1.4
//0123456
double vmax_temp;
double vmin_temp;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uartTick = uwTick;
	HAL_UART_Receive_IT(&huart1,uart_buf,1);
	
	rx_buf[rx_cnt++] = uart_buf[0];
	
	if(rx_cnt == 7)
	{
		rx_cnt = 0;
		//格式判断
		if(rx_buf[0] >='0' && rx_buf[2] <='9')
			if(rx_buf[4] >='0' && rx_buf[6] <='9')
				if(rx_buf[1] =='.' && rx_buf[5] =='.'&& rx_buf[3] == ',')
				{
					vmax_temp = (rx_buf[0]-'0') + (rx_buf[2]-'0')/10.0;
					vmin_temp = (rx_buf[4]-'0') + (rx_buf[6]-'0')/10.0;
					
					if(vmin_temp + 1 <= vmax_temp )
					{
						uart_flag = 0;
						vmax_com = vmax_set = vmax_temp ;
						vmin_com = vmin_set = vmin_temp ;	
					}
					else
					{
						uart_flag = 1;
						printf("logic error!\r\n");
					}
						
				}
				else
				{
					uart_flag = 1;
					printf("form error!\r\n");
				}
					
			else
			{
				uart_flag = 1;
				printf("form error!\r\n");
			}
				
		else
		{
			uart_flag = 1;
			printf("form error!\r\n");
		}
			
	}
	
	//Led_Control(uart_buf[0]);
}

//LED
u8 led_ctrl = 0x00;
void LED_Process(void)
{
	if(tim_flag == 1)
		led_ctrl |= 0x01;
	else
		led_ctrl &= ~0x01;
	
	if(key_flag == 1)
		led_ctrl |= 0x02;
	else
		led_ctrl &= ~0x02;
	
	if(uart_flag == 1)
		led_ctrl |= 0x04;
	else
		led_ctrl &= ~0x04;
	
	
	Led_Control(led_ctrl);
}



//LCD
uint32_t lcdTick = 0;
void LCD_Process(void)
{
	if(uwTick - lcdTick < 50) return;
	lcdTick = uwTick;
	
	u8 display_buf[30];
	
	if(Display_Mode == Display_Data)
	{
		sprintf((char *)display_buf,"      Data  ");
		LCD_DisplayStringLine(Line0,display_buf);
		
		sprintf((char *)display_buf," V:%4.2fV  ",r37_v);
		LCD_DisplayStringLine(Line2,display_buf);
		
		sprintf((char *)display_buf," T:%ds    ",sec);
		LCD_DisplayStringLine(Line3,display_buf);
	}
	
	if(Display_Mode == Display_Para)
	{
		sprintf((char *)display_buf,"      Para  ");
		LCD_DisplayStringLine(Line0,display_buf);
		
		sprintf((char *)display_buf," Vmax:%3.1fV  ",vmax_set);
		LCD_DisplayStringLine(Line2,display_buf);
		
		sprintf((char *)display_buf," Vmin:%3.1fV  ",vmin_set);
		LCD_DisplayStringLine(Line3,display_buf);
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
		//HAL_UART_Transmit(&huart1,(u8 *)&tx_buf,sizeof(tx_buf)-1,50);
		//printf("hello\r\n");
    while (1) //while(1)中最多几毫秒
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			
			Key_Process();
			ADC_Process();
			LCD_Process();
			LED_Process();
			Rx_Idle();
			Time_Process();
			
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
