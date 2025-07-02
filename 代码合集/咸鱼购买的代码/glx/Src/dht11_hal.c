#include "dht11_hal.h"


dht11Data dht11;

//
static void usDelay(uint32_t us)
{
	uint16_t i = 0;
	while(us--){
		i = 30;
		while(i--);
	}
}

//
void outDQ(uint8_t i)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	GPIO_InitStructure.Pin = HDQ;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	if(!i) 
		HAL_GPIO_WritePin(GPIOA, HDQ, GPIO_PIN_RESET);
	else 
		HAL_GPIO_WritePin(GPIOA, HDQ, GPIO_PIN_SET);
}

//
void inDQ(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Pin = HDQ;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//
void dht11Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
	outDQ(1);			
}

//
uint8_t recData(void)
{
	uint8_t i,temp=0,j=220;
	
	for(i=0; i<8; i++){
		
		while(!HAL_GPIO_ReadPin(GPIOA,HDQ));
		usDelay(40);
		if(HAL_GPIO_ReadPin(GPIOA,HDQ))
		{
			temp=(temp<<1)|1;
			while(HAL_GPIO_ReadPin(GPIOA,HDQ)&&(j--));	
		}	
		else
		{
			temp=(temp<<1)|0;
		}
	}
	return temp;
}

//±àÐ´
void DHT11_Read(void)
{
	u8 temp[5];
	int j=5000;
	outDQ(0);
	usDelay(18000);
	outDQ(1);
	usDelay(30);
	inDQ();
	while(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) && (j--));
	j = 5000;
	while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) && (j--));
	
	temp[0] = recData();
	temp[1] = recData();
	temp[2] = recData();
	temp[3] = recData();
	temp[4] = recData();
	
	outDQ(1);
	
	if(temp[0] + temp[1] + temp[2] + temp[3] - temp[4]) return;
	else
	{
		dht11.humidity_high = temp[0];
		dht11.humidity_low = temp[1];
		dht11.temperature_high = temp[2];
		dht11.temperature_low = temp[3];
	}
	
	

	
}
