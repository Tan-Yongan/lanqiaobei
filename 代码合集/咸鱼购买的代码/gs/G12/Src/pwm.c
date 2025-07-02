



uint32_t uwIC2Value1_PA7 = 0;
uint32_t uwIC2Value2_PA7 = 0;
uint32_t low_PA7,high_PA7;

/* Capture index */
uint16_t uhCaptureIndex_PA7 = 0;

/* Frequency Value */

double D_PA7;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

					if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
					{
						if(uhCaptureIndex_PA7 == 0)
						{
							/* Get the 1st Input Capture value */
							uwIC2Value1_PA7 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
							__HAL_TIM_SET_CAPTUREPOLARITY(&htim17,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
							uhCaptureIndex_PA7 = 1;
						}
						else if(uhCaptureIndex_PA7 == 1)
						{
							/* Get the 2nd Input Capture value */
							uwIC2Value2_PA7 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
							__HAL_TIM_SET_CAPTUREPOLARITY(&htim17,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
							/* Capture computation */
							if (uwIC2Value2_PA7 > uwIC2Value1_PA7)
							{
								high_PA7 = (uwIC2Value2_PA7 - uwIC2Value1_PA7); 
							}
							else if (uwIC2Value2_PA7 < uwIC2Value1_PA7)
							{
								/* 0xFFFF is max TIM1_CCRx value */
								high_PA7 = ((0xFFFF - uwIC2Value1_PA7) + uwIC2Value2_PA7) + 1;
							}
							else
							{
								/* If capture values are equal, we have reached the limit of frequency
									 measures */
								Error_Handler();
							}

							/* Frequency computation: for this example TIMx (TIM1) is clocked by
								 APB2Clk */      
							uwIC2Value1_PA7 = uwIC2Value2_PA7;
							uhCaptureIndex_PA7 = 2;
						}
						else if(uhCaptureIndex_PA7 == 2)
						{
							/* Get the 2nd Input Capture value */
							uwIC2Value2_PA7 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 
							/* Capture computation */
							if (uwIC2Value2_PA7 > uwIC2Value1_PA7)
							{
								low_PA7 = (uwIC2Value2_PA7 - uwIC2Value1_PA7); 
							}
							else if (uwIC2Value2_PA7 < uwIC2Value1_PA7)
							{
								/* 0xFFFF is max TIM1_CCRx value */
								low_PA7 = ((0xFFFF - uwIC2Value1_PA7) + uwIC2Value2_PA7) + 1;
							}
							else
							{
								/* If capture values are equal, we have reached the limit of frequency
									 measures */
								Error_Handler();
							}

							/* Frequency computation: for this example TIMx (TIM1) is clocked by
								 APB2Clk */      
							
							D_PA7 = high_PA7*1.0/(low_PA7 + high_PA7);
							uhCaptureIndex_PA7 =  0;
						}
						
					}
}