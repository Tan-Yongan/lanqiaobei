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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
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

#define Display_Charge 0
#define Display_Setting 1

u8 Display_Mode = 0;

//1
//回来试试结构体写法--不一定比单个变量好用，由于这里每个对象只有两个特征
//typedef struct{
//	
//	
//	double price;
//	
//	
//	
//} product;

u8 pdu_number = 1;

double pdu_weight = 8.00;
//2
double pdu_price[3] = {5.56,0.54,9.95};
double pdu_price_temp[3] = {5.56,0.54,9.95};
int set_number = 0;
u8 pdu_chose = 1;
double total_price = 0;

//EEPROM
u8 st = 0;

//LED
u8 led_ctrl = 0x00;
void LED_Proc(void)
{
	Led_Control(led_ctrl);
}



//key

uint32_t longTick1 = 0;
uint32_t idleTick1 = 0;

uint32_t longTick2 = 0;
uint32_t idleTick2 = 0;

//Two ADC
u32 adc_val1,adc_val2;
double rp5_v;
//double rp6_v;//用AKEY之前可以先测试两个通道是否影响
void ADC_Proc(void)
{
	HAL_ADC_Start(&hadc2);
	adc_val1 = HAL_ADC_GetValue(&hadc2);
	rp5_v = adc_val1/4095.0*3.3;
	
	pdu_weight = rp5_v/3.3*10;
	
	HAL_ADC_Start(&hadc2);
	adc_val2 = HAL_ADC_GetValue(&hadc2);
}

//AKEY
u8 Akey_IO(void)
{
	u8 key_val = 0x00;
	if(adc_val2 < 200)
		key_val|=0x01;
	else if(adc_val2 < 800)
		key_val|=0x02;
	else if(adc_val2 < 1400)
		key_val|=0x04;
	else if(adc_val2 < 2000)
		key_val|=0x08;
	else if(adc_val2 < 2600)
		key_val|=0x10;
	else if(adc_val2 < 3200)
		key_val|=0x20;
	else if(adc_val2 < 3800)
		key_val|=0x40;
	else if(adc_val2 < 3950)
		key_val|=0x80;
	
	return key_val;

}

u8 Atrg,Aup,ACont;

void Akey_Read(void)
{
	u8 ReadData = Akey_IO();
	Atrg = ReadData & (ReadData ^ ACont);
	Aup = ~ReadData & (ReadData ^ ACont);
	ACont = ReadData;
}


uint32_t akeyTick = 0;
extern int led_cnt1;

double inter,frac;
void Akey_Proc(void)
{
	if(uwTick - akeyTick < 20)return;
	akeyTick = uwTick;
	
	Akey_Read();
	
	if(Atrg & 0x01)
	{
		LCD_Clear(Black);
		Display_Mode = (Display_Mode + 1)%2;
		
		led_cnt1 = 0;
		
		if(Display_Mode == Display_Charge)
		{
			if(fabs(pdu_price[0] - pdu_price_temp[0]) < 0.001 && fabs(pdu_price[1] - pdu_price_temp[1]) < 0.001 && fabs(pdu_price[2] - pdu_price_temp[2]) < 0.001)
			{
				
			}
			else
			{
				pdu_price_temp[0] = pdu_price[0];
				pdu_price_temp[1] = pdu_price[1];
				pdu_price_temp[2] = pdu_price[2];
				set_number++;
			}
			
			printf("U.W.1:%.2f\r\nU.W.2:%.2f\r\nU.W.3:%.2f\r\n",pdu_price[0],pdu_price[1],pdu_price[2]);
			
      frac = modf(pdu_price[0],&inter);
			
			EEPROM_Write(0x02,(int)(pdu_price[0]*100)/256);
			EEPROM_Write(0x03,(int)(pdu_price[0]*100)%256);//这里一定改为round!!!!
			
//      frac = modf(pdu_price[1],&inter);
//			
//			EEPROM_Write(0x04,round(inter));
//			EEPROM_Write(0x05,round((frac*100.0)));
//			
//      frac = modf(pdu_price[2],&inter);
//			
//			EEPROM_Write(0x06,round(inter));
//			EEPROM_Write(0x07,round((frac*100.0)));
		
			EEPROM_Write(0x08,set_number);
		}
		
		if(Display_Mode == Display_Setting)
		{
			pdu_price[0] = pdu_price_temp[0];
			pdu_price[1] = pdu_price_temp[1];
			pdu_price[2] = pdu_price_temp[2];
		}
	}

	if(Atrg & 0x02) //长度按
	{
		longTick1 = uwTick;
		
	}else if(ACont & 0x02 && uwTick - longTick1 > 800)
	{
		if(Display_Mode == Display_Setting)
		{
			if(uwTick - idleTick1 < 100) return; //间隔100ms加一次
			idleTick1 = uwTick;	
			
			pdu_price[pdu_chose - 1]+=0.01; //考虑间隔加一次
			
			if(pdu_price[pdu_chose - 1] > 10.001)
				pdu_price[pdu_chose - 1] = 0;
		}
	}
	else if(Aup & 0x02 && uwTick - longTick1 < 800)
	{
		if(Display_Mode == Display_Setting)
		{
			pdu_price[pdu_chose - 1]+=0.01;
			
			if(pdu_price[pdu_chose - 1] > 10.001)
				pdu_price[pdu_chose - 1] = 0;
		}
	}
	
	if(Atrg & 0x04) //长短按
	{
		longTick2 = uwTick;
	}
	else if(ACont & 0x04 && uwTick - longTick2 > 800)
	{
		if(Display_Mode == Display_Setting)
		{
			if(uwTick - idleTick2 < 100) return; //间隔100ms一次
			idleTick2 = uwTick;	
			
			pdu_price[pdu_chose - 1]-=0.01; 
			
			if(pdu_price[pdu_chose - 1] < -0.001)
				pdu_price[pdu_chose - 1] = 10;
		}
		
	}
	else if(Aup & 0x04 && uwTick - longTick2 <800)
	{
		if(Display_Mode == Display_Setting)
		{

			pdu_price[pdu_chose - 1]-=0.01; 
			
			if(pdu_price[pdu_chose - 1] < -0.001)
				pdu_price[pdu_chose - 1] = 10;
		}
	}
	
	if(Atrg & 0x08)
	{
		if(Display_Mode == Display_Setting)
		{
			pdu_chose = pdu_chose%3+1;
		}
	}
	
	if(Atrg & 0x10)
	{
		if(Display_Mode == Display_Charge)
			pdu_number = 1;
	}
	
	if(Atrg & 0x20)
	{
		if(Display_Mode == Display_Charge)
			pdu_number = 2;
	}
	
	if(Atrg & 0x40)
	{
		if(Display_Mode == Display_Charge)
			pdu_number = 3;
	}
	
	if(Atrg & 0x80)
	{
		if(Display_Mode == Display_Charge)
		{
			total_price = pdu_price[pdu_number-1] * pdu_weight;
			printf("U.W.%d:%.2f\r\nG.W:%.2f\r\nTotal:%.2f\r\n",pdu_number,pdu_price[pdu_number-1],pdu_weight,total_price); //在题目没有说明的情况下，一般结尾加上回车
			
			
		}
	}
	
}


//USART
int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	HAL_UART_Transmit(&huart1,(u8*)&ch,1,50);
  return ch;
}



//LCD
uint32_t lcdTick = 0;
void LCD_Proc(void)
{
	if(uwTick - lcdTick < 100) return;
	
	//ADC_Proc(); //若考虑到按键响应还是放到外面好
	
	u8 display_buf[30];
	
	
	if(Display_Mode == Display_Charge)
	{
		sprintf((char *)display_buf,"     Charge Price");
		LCD_DisplayStringLine(Line0,display_buf);
		
		sprintf((char *)display_buf,"number:%d",pdu_number);
		LCD_DisplayStringLine(Line2,display_buf);
		
		sprintf((char *)display_buf,"price:%.2f    ",pdu_price[pdu_number-1]);
		LCD_DisplayStringLine(Line4,display_buf);
		
		sprintf((char *)display_buf,"weight:%.2f    ",pdu_weight);
		LCD_DisplayStringLine(Line6,display_buf);
		
		sprintf((char *)display_buf,"total :%.2f     ",total_price);
		LCD_DisplayStringLine(Line8,display_buf);
		
		sprintf((char *)display_buf,"st:%d     ",st);
		LCD_DisplayStringLine(Line9,display_buf);
		
	}
	
	if(Display_Mode == Display_Setting)
	{
		sprintf((char *)display_buf,"     Charge Price");
		LCD_DisplayStringLine(Line0,display_buf);
		
	
		  
		sprintf((char *)display_buf,"pdu1:%.2f /kg         ",pdu_price[0] + 0.000001); //显示-0.00问题
		if(pdu_chose == 1)
		LCD_SetBackColor(Green);
		LCD_DisplayStringLine(Line2,display_buf);
		LCD_SetBackColor(Black);
		
		sprintf((char *)display_buf,"pdu2:%.2f /kg        ",pdu_price[1] + 0.001);
		if(pdu_chose == 2)
		LCD_SetBackColor(Green);
		LCD_DisplayStringLine(Line3,display_buf);
		LCD_SetBackColor(Black);
		
		
		sprintf((char *)display_buf,"pdu3:%.2f /kg       ",pdu_price[2] + 0.001);
		if(pdu_chose == 3)
		LCD_SetBackColor(Green);
		LCD_DisplayStringLine(Line4,display_buf);
		LCD_SetBackColor(Black);
		
		
		sprintf((char *)display_buf,"     Seting number:%d   ",set_number);
		LCD_DisplayStringLine(Line6,display_buf);
	}
	

}

//TIM15
//int cnt;
//_Bool led_flag = 0;
int led_cnt1;
//799 9999
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //100ms
{
	if(htim == &htim15)
	{
		if(Display_Mode == Display_Charge)
		{
			led_cnt1++;
			if(led_cnt1 == 8)
			{
				led_ctrl^=0x01;
				led_cnt1 = 0;
			}
		}
		
		if(Display_Mode == Display_Setting)
		{
			led_cnt1++;
			if(led_cnt1 == 4)
			{
				led_ctrl^=0x01;
				led_cnt1 = 0;
			}
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
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */

    LCD_Init();
		
	//AKEY 
	ADC_Proc(); 
	
	//TIM15
	HAL_TIM_Base_Start_IT(&htim15);
	
	//EEPROM
	I2CInit();
	\
	// 02 03    04 05    0607     08
	//       单价                   次数
  //设单价为 5.56 ， 0.54 ， 9.95
	//解法一 ，整数部分和小数部分分别存到两个地址
	//将小数分为整数部分和小数部分
	//小数拆解函数
	
	//解法二
	//5.56*100
	//a = 556/256; b = 556%256;
	//将a,b分别存入两个地址
	
	//取出来 c = (a*256 + b)/100.0
	
	
	
	
//	double integer,frac;
//	double num = 145.335;
//	frac = modf(num,&inter);
//	printf("int:%f  f:%f\r\n",integer,frac);
	
//	if(EEPROM_Read(0x01) != 24) //第一次上电
//	{
//		EEPROM_Write(0x01,24);
		
//		EEPROM_Write(0x02,556/256);
//		EEPROM_Write(0x03,556%256);
		
//		EEPROM_Write(0x04,0);
//		EEPROM_Write(0x05,0.54*100);
//		
//		EEPROM_Write(0x06,9);
//		EEPROM_Write(0x07,0.95*100);
		
//		EEPROM_Write(0x08,0);
//		
		
//	}
	
	//尽量这里都写幅值
//	pdu_price[0] = (EEPROM_Read(0x02)*256 + EEPROM_Read(0x03))/100.0;
//	pdu_price[1] = EEPROM_Read(0x04) + EEPROM_Read(0x05)/100.0;
//	pdu_price[2] = EEPROM_Read(0x06) + EEPROM_Read(0x07)/100.0;
	
//	pdu_price_temp[0] = pdu_price[0];
//	pdu_price_temp[1] = pdu_price[1];
//	pdu_price_temp[2] = pdu_price[2];
//	
//	set_number = EEPROM_Read(0x08);
	
  EEPROM_Write(0x01,0);
  EEPROM_Write(0x02,0);
  EEPROM_Write(0x03,0);
  EEPROM_Write(0x04,0);
  EEPROM_Write(0x05,0);
  EEPROM_Write(0x07,0);
  EEPROM_Write(0x08,0);
  EEPROM_Write(0x09,0);
	
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
		
		//printf("num:%d\r\n",(int)5.9/2));
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			ADC_Proc();
			LCD_Proc();
			Akey_Proc();
			LED_Proc();
			
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
