


uint32_t uwIC2Value1_PB4 = 0;
uint32_t uwIC2Value2_PB4 = 0;
uint32_t uwDiffCapture_PB4 = 0;

/* Capture index */
uint16_t uhCaptureIndex_PB4 = 0;

/* Frequency Value */

uint32_t PB4_F;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
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