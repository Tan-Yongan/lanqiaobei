#include "headfile.h"

uint8_t count;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		if(error_flag)
		{
			count++;
			count=count%2;
		}		
	}

}

//uint8_t count0;
//uint32_t val_1=10,val_2,fre0;
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//	if(htim->Instance == TIM2)
//	{
//		count0++;
//		switch(count0)
//		{
//			case 1:
//			{
//				val_1 = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
//				break;
//			}
//			case 2:
//			{
//				count0=0;
//				val_2 = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
//				fre0 = 80000000/((val_2-val_1)*80);
//			}
//		}
//		
//	}
//}
