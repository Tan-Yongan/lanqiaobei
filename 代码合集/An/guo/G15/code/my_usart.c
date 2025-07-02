#include "headfile.h"

int fputc(int ch, FILE *f)
{
  /* Your implementation of fputc(). */
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
  return ch;
}

uint8_t rx_flag;
uint8_t rx_size;
char rx_buff[100];
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART1)
	{
		rx_flag = 1;
		rx_size = Size;
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t *)rx_buff,100);
	}
}


//(11,22,33,44,55,66,77,88,99)
char temp_str[100];
uint8_t data[100];
uint8_t count;
void rx_handle()
{
	if(rx_flag)
	{
		rx_flag = 0;
		count = 0;
		
		sscanf(rx_buff, "(%[^)])", temp_str);

	    printf("%s",temp_str);
		
		char *token = strtok(temp_str,",");
		
		while(token!=NULL)
		{
			data[count] = atoi(token);
			count++;
			token = strtok(NULL,",");
		}
		
		for(int i = 0;i<count;i++)
		{
			printf("%d\n",data[i]);
		}
		
		memset(data,'\0',100);
		memset(temp_str,'\0',100);
		memset(rx_buff,'\0',100);
	}
}


void delete_coordinate(uint8_t x, uint8_t y)
{
    for (int i = 0; i < count - 1; i += 2)
    {
        if (data[i] == x && data[i + 1] == y)
        {
            // 找到了匹配的坐标，删除
            for (int j = i; j < count - 2; j++)
            {
                data[j] = data[j + 2]; // 向前移动两个元素
            }
            count -= 2; // 减少两个数据量
            break; // 如果只删一个坐标，找到后退出
        }
    }
}


void delete_first_coordinate()
{
    if (count >= 2)
    {
        for (int i = 0; i < count - 2; i++)
        {
            data[i] = data[i + 2]; // 把后面的往前搬
        }
        count -= 2;
        printf("Deleted the first coordinate.\n");
    }
    else
    {
        printf("No coordinate to delete.\n");
    }
}


float dist[50]; // 最多支持 50 个距离，对应 51 个点（100/2）
int dist_count = 0;

void calculate_distances()
{
    dist_count = 0;
    
    for (int i = 0; i < count - 3; i += 2)
    {
        int x1 = data[i];
        int y1 = data[i + 1];
        int x2 = data[i + 2];
        int y2 = data[i + 3];

        float d = sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        dist[dist_count++] = d;
    }
}
