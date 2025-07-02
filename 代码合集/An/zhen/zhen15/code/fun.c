#include "headfile.h"

uint8_t waitA_flag=1;
uint8_t waitB_flag=1;
void data_cheak()
{
	if(fa<=PH)
	{
		waitA_flag = 0;
	}

	if(waitA_flag == 0)
	{
		if(fa>PH)
		{
			waitA_flag = 1;
			NHA++;
		}
	}
	
	if(fb<=PH)
	{
		waitB_flag = 0;
	}
	if(waitB_flag == 0)
	{
		if(fb>PH)
		{
			waitB_flag = 1;
			NHB++;
		}
	}
		
}
