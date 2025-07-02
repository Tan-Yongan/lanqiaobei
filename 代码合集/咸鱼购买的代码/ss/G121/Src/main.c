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
#include "math.h"

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

u8 Dispaly_Mode = 0;


//1

u8 CNBR = 0;
u8 VNBR = 0;
u8 IDLE = 8;


//2

double CNBR_F = 3.5;
double VNBR_F = 2.0;

//PWM
_Bool pwm_flag = 1;

//计费系统
typedef struct
{
	u8 type[5];//这里数据个数必须必存放数据大一位，最后一位用来结束该地址储存
	u8 code[5];

	
	u8 in_year;
	u8 in_mouth;
	u8 in_day;
	u8 in_hour;
	u8 in_min;
	u8 in_sec;
	u8 pos; //该车的当前车位
}car;

car in_car[10] = {0};//存放每个入库车的数据

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
		Dispaly_Mode = (Dispaly_Mode + 1) % 2;
	}
	
	if(Trg & 0x02)
	{
		if(Dispaly_Mode == Display_Para)
		{
			CNBR_F += 0.5;
			VNBR_F +=0.5;
		}
	}
	
	if(Trg & 0x04)
	{
		if(Dispaly_Mode == Display_Para)
		{
			
			
			CNBR_F -= 0.5;
			VNBR_F -=0.5; //减到0
			if(VNBR_F < 0)
			{
				CNBR_F = 1.5;
				VNBR_F =0;
			}
			
		}
	}
	
	if(Trg & 0x08)
		pwm_flag = !pwm_flag;

}

//PA7
void PWM_Process(void)
{
	if(pwm_flag == 0)
	{
		TIM17->ARR = 499;
	  TIM17->CCR1 = (TIM17->ARR + 1)* 0.2;
	}
	else
	{
		TIM17->ARR = 499;
	  TIM17->CCR1 = 0; //输出高低电平，直接写0和写满比较保险
		
		//or
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
		//_HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,0);
	}
	
}

//UART
//发
u8 tx_buf[100] = {"世界"};


int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	HAL_UART_Transmit(&huart1,(u8 *)&ch,1,50);
  return ch;
}

//收
u8 uart_buf[2];
u8 rx_cnt = 0;
u8 rx_buf[100];

uint32_t uartTick = 0;
void Rx_Idle(void)
{
	if(uwTick - uartTick < 50) return;
	uartTick = uwTick;
	
	if(rx_cnt > 0)
	{
		if(rx_cnt != 22)
			printf("Lenth error\r\n");
		
		rx_cnt = 0;
		memset(rx_buf,'\0',sizeof(rx_buf));
	}

}

//VNBR:D583: 2  0  0  2  0   2  1  3  2  5  0 0
//0123456789 10 11 12 13 14 15 16 17 18 19 20 21    
//共计22个字符

//检查字符合格 0 不合格，1 合格
u8 Check_Form(u8* str)
{
	u8 i;
	if((str[0] == 'C' || str[0] == 'V') && str[4] == ':' && str[9] == ':' && str[1] == 'N' && str[2] == 'B'&& str[3] == 'R')
	{
		//code
		for(i = 5;i<9;i++)
		{
			if(str[i] >='a' && str[i] <='z')
				continue;
			
			if(str[i] >='A' && str[i] <='Z')
				continue;
				
			if(str[i] >='0' && str[i] <='9')
				continue;
			
			return 0;
		}
		
		for(i = 10;i<22;i++)
		{
			if(str[i] >='0' && str[i] <= '9')
				continue;
			else
				return 0;
		}
		
		return 1;
	}
	
	return 0;
}

//入库出库检查 0 入库， 1出库
//VNBR:D583: 2  0  0  2  0   2  1  3  2  5  0 0
//0123456789 10 11 12 13 14 15 16 17 18 19 20 
u8 Check_In_Out(u8* str)
{
	u8 i;
	for(i = 0;i<8;i++)
	{
		if(in_car[i].code[0] == str[5] && in_car[i].code[1] == str[6] && in_car[i].code[2] == str[7] && in_car[i].code[3] == str[8])
		{
			return 1;
		}
	}
	
	return 0;
}
u8 temp_type[4];

u8 temp_code[4];

u8 temp_year;
u8 temp_mouth;
u8 temp_day;
u8 temp_hour;
u8 temp_min;
u8 temp_sec;
double hour_sum;
double fee;
//VNBR:D583:200202132500
//0123456789 10 11 12 13 14 15 16 17 18 19 20 2
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uartTick = uwTick;
	HAL_UART_Receive_IT(&huart1,uart_buf,1);
	
	rx_buf[rx_cnt++] = uart_buf[0];
	
	if(rx_cnt == 22)
	{
		rx_cnt = 0;
		//printf("%d\r\n",Check_Form(rx_buf));
		if(Check_Form(rx_buf)) //格式判断
		{
			//逻辑判断
			temp_year = (rx_buf[10] - '0')*10 + (rx_buf[11] - '0');
			temp_mouth = (rx_buf[12] - '0')*10 + (rx_buf[13] - '0');
			temp_day = (rx_buf[14] - '0')*10 + (rx_buf[15] - '0');
			temp_hour = (rx_buf[16] - '0')*10 + (rx_buf[17] - '0');
			temp_min = (rx_buf[18] - '0')*10 + (rx_buf[19] - '0');
			temp_sec = (rx_buf[20] - '0')*10 + (rx_buf[21] - '0');
			
			if(temp_mouth > 12 || temp_day > 30 || temp_hour >23 || temp_min > 59 || temp_sec > 59)
			{
				printf("logic error\r\n");
			}
			else
			{
				//出库
				if(Check_In_Out(rx_buf))
				{
					//printf("not1");
					u8 i;
					for(i = 0;i<(8-IDLE);i++)
					{
							if(in_car[i].code[0] == rx_buf[5] && in_car[i].code[1] == rx_buf[6] && in_car[i].code[2] == rx_buf[7] && in_car[i].code[3] == rx_buf[8]) //找到出库的车了
							{
								//计算费用
								hour_sum = (temp_year - in_car[i].in_year)*12*30*24 + (temp_mouth - in_car[i].in_mouth)*30*24 + 
								(temp_day - in_car[i].in_day)*24 + (temp_hour - in_car[i].in_hour)+(temp_min - in_car[i].in_min)/60.0f +
								(temp_sec - in_car[i].in_sec)/3600.0f;
								
								hour_sum = ceil(hour_sum);//向上取整
								
								if(hour_sum < 0)
								{
									printf("tim error\r\n");
									return;
								}
									
								if(rx_buf[0] == 'C')
								{
									fee = hour_sum * CNBR_F;
									CNBR--;
								}
								if(rx_buf[0] == 'V')
								{
									fee = hour_sum * VNBR_F;
									VNBR--;
								}
								IDLE++;
								
								printf("%s:%s:%.0f:%5.2f\r\n",in_car[i].type,in_car[i].code,hour_sum,fee); //若不限制几位，则用.0f
								memset(&in_car[i],'\0' ,sizeof(in_car[i]));//清除该车位的信息
								
							}
						
					}
					
				}
				//入库
				else
				{

					//还有车位
					if(IDLE > 0)
					{
							u8 i;
							for(i = 0;i<4;i++) //问题，结构体的储存控件连续
							{
								in_car[8 - IDLE].type[i] = rx_buf[i];
								in_car[8 - IDLE].code[i] = rx_buf[i+5];
							}

							in_car[8 - IDLE].in_year = temp_year;
							in_car[8 - IDLE].in_mouth = temp_mouth;
							in_car[8 - IDLE].in_day = temp_day;
							in_car[8 - IDLE].in_hour = temp_hour;
							in_car[8 - IDLE].in_min = temp_min;
							in_car[8 - IDLE].in_sec = temp_sec;
							in_car[8 - IDLE].pos = 8-IDLE; 
							
							IDLE--;
							if(rx_buf[0] == 'C')
							{
								CNBR++;
							}
								
							if(rx_buf[0] == 'V')
							{
								VNBR++;
							}
					}

				}
			}
			
		}
		else
			printf("form error\r\n");
		
	}
}


//LCD
uint32_t lcdTick = 0;
void LCD_Process(void)
{
	if(uwTick - lcdTick < 100) return;
	lcdTick = uwTick;
	
	u8 display_buf[30];
	
	if(Dispaly_Mode == Display_Data)
	{
		sprintf((char *)display_buf,"       Data ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char *)display_buf,"   CNBR:%d",CNBR);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char *)display_buf,"   VNBR:%d",VNBR);//考虑超过10个占位
		LCD_DisplayStringLine(Line5,display_buf);
		
		sprintf((char *)display_buf,"   IDLE:%d",IDLE);
		LCD_DisplayStringLine(Line7,display_buf);
	}
	
	if(Dispaly_Mode == Display_Para)
	{
		sprintf((char *)display_buf,"       Para ");
		LCD_DisplayStringLine(Line1,display_buf);
		
		sprintf((char *)display_buf,"   CNBR:%4.2f",CNBR_F);
		LCD_DisplayStringLine(Line3,display_buf);
		
		sprintf((char *)display_buf,"   VNBR:%4.2f",VNBR_F);//考虑超过10个占位
		LCD_DisplayStringLine(Line5,display_buf);
		
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
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
		//PWM
		HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1); 
		
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
		
//		printf("你好\r\n");
//		HAL_UART_Transmit(&huart1,tx_buf,sizeof(tx_buf),50);
//		printf("%3.1f",floor(3.2));
//		printf("%3.1f",ceil(3.2));
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			Key_Process();
			LCD_Process();
			PWM_Process();
			Rx_Idle();
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
