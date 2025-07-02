#include "headfile.h"

uint8_t buff;
uint8_t rx_buff[22];
uint8_t i=0;
uint8_t rx_flag;
char tx[50];
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	if(huart->Instance == USART1)
	{
		rx_buff[i]=buff;
		HAL_UART_Receive_IT(huart,&buff,1);
		i++;			
		if(i==22)	
		{
			i=0;
			rx_flag=1;
		}
		
	}


}

uint8_t type[4];
uint8_t num[4];
int j,k,x;
int time[6];
int time_cha[6];
uint8_t out_flag;
uint8_t error_flag;
uint8_t all_num[32];
uint8_t all_time[48];
int loca=0;
float fee;
int hour;

void data_analysis()
{
	if(rx_flag)
	{
		rx_flag=0;
		j=0;
		
		type[0]=rx_buff[0];
		type[1]=rx_buff[1];
		type[2]=rx_buff[2];
		type[3]=rx_buff[3];   //����
		
		num[0]=rx_buff[5];
		num[1]=rx_buff[6];
		num[2]=rx_buff[7];
		num[3]=rx_buff[8];		//���
		
		time[0]=10*to_num(rx_buff[10])+to_num(rx_buff[11]);//��
		time[1]=10*to_num(rx_buff[12])+to_num(rx_buff[13]);//��
		time[2]=10*to_num(rx_buff[14])+to_num(rx_buff[15]);//��
		time[3]=10*to_num(rx_buff[16])+to_num(rx_buff[17]);//ʱ
		time[4]=10*to_num(rx_buff[18])+to_num(rx_buff[19]);//��
		time[5]=10*to_num(rx_buff[20])+to_num(rx_buff[21]);//��

		while(j<8)      //�����Ƿ��б����ͬ�� ����˵������
		{
			if(num[0]==all_num[0+4*j]&&num[1]==all_num[1+4*j]&&num[2]==all_num[2+4*j]&&num[3]==all_num[3+4*j])
			{
				out_flag=1;
				break;
			}
			j+=1;
		}
		
		if(out_flag)
		{
			if((8-cnbr_num-vnbr_num)<8)
			{
				out_flag=0;
				time_cha[0]=time[0]-all_time[6*j+0];  //��
				time_cha[1]=time[1]-all_time[6*j+1];  //��
				time_cha[2]=time[2]-all_time[6*j+2];  //��
				time_cha[3]=time[3]-all_time[6*j+3];  //ʱ
				time_cha[4]=time[4]-all_time[6*j+4];  //��
				time_cha[5]=time[5]-all_time[6*j+5];  //��	
				
				if(time_cha[2]==0)
				{
					if((time[3]*60+time[4]-all_time[6*j+3]*60-all_time[6*j+4])%60==0)
						hour=(time[3]*60+time[4]-all_time[6*j+3]*60-all_time[6*j+4])/60.0;
					else
						hour=(time[3]*60+time[4]-all_time[6*j+3]*60-all_time[6*j+4])/60.0+1;
				}
				else
				{
					if((time[2]*24*60+time[3]*60+time[4]-all_time[6*j+2]*24*60-all_time[6*j+3]*60-all_time[6*j+4])%60==0)
						hour=(time[2]*24*60+time[3]*60+time[4]-all_time[6*j+2]*24*60-all_time[6*j+3]*60-all_time[6*j+4])/60.0;
					else
						hour=(time[2]*24*60+time[3]*60+time[4]-all_time[6*j+2]*24*60-all_time[6*j+3]*60-all_time[6*j+4])/60.0+1;
				}
				
				if(type[0]=='C'&&type[1]=='N'&&type[2]=='B'&&type[3]=='R')
				{
					cnbr_num--;
					fee=hour*cnbr_rate;
				}
				else if(type[0]=='V'&&type[1]=='N'&&type[2]=='B'&&type[3]=='R')
				{
					vnbr_num--;
					fee=hour*vnbr_rate;
				}
				else
				{
					error_flag=1;
				}
				
				if(error_flag)
				{
					error_flag=0;
					HAL_UART_Transmit(&huart1,(uint8_t *)"Error",5,50);
				}
				else
				{		
					sprintf(tx,"%c%c%c%c:%c%c%c%c:%d:%.2f",type[0],type[1],type[2],type[3],num[0],num[1],num[2],num[3],hour,fee);
					HAL_UART_Transmit(&huart1,(uint8_t *)tx,strlen(tx),50);
				}
				
				all_num[0+j*4]=0;
				all_num[1+j*4]=0;
				all_num[2+j*4]=0;
				all_num[3+j*4]=0;
				all_time[6*j+0]=0;  //��
				all_time[6*j+1]=0;  //��
				all_time[6*j+2]=0;  //��
				all_time[6*j+3]=0;  //ʱ
				all_time[6*j+4]=0;  //��
				all_time[6*j+5]=0;  //��
				
				for(int u=j;u<7;u++)  //λ����ǰ�ƶ�
				{
					all_num[0+u*4]=all_num[0+(u+1)*4];
					all_num[1+u*4]=all_num[1+(u+1)*4];
					all_num[2+u*4]=all_num[2+(u+1)*4];
					all_num[3+u*4]=all_num[3+(u+1)*4];
					all_time[6*u+0]=all_time[6*(u+1)+0];
					all_time[6*u+1]=all_time[6*(u+1)+1];
					all_time[6*u+2]=all_time[6*(u+1)+2];
					all_time[6*u+3]=all_time[6*(u+1)+3];
					all_time[6*u+4]=all_time[6*(u+1)+4];
					all_time[6*u+5]=all_time[6*(u+1)+5];
					
					if(all_num[0+(u+1)*4]==0)
					{
						loca=u;
						break;
						
					}
						
				}

			}
			else
			{
				HAL_UART_Transmit(&huart1,(uint8_t *)"Error",5,50);
			}
		}
		else
		{
			if((8-cnbr_num-vnbr_num)>0)
			{
				all_time[0+loca*6]=time[0];
				all_time[1+loca*6]=time[1];
				all_time[2+loca*6]=time[2];
				all_time[3+loca*6]=time[3];
				all_time[4+loca*6]=time[4];
				all_time[5+loca*6]=time[5];
				all_num[0+loca*4]=num[0];
				all_num[1+loca*4]=num[1];
				all_num[2+loca*4]=num[2];
				all_num[3+loca*4]=num[3];
				loca++;
				if(type[0]=='C'&&type[1]=='N'&&type[2]=='B'&&type[3]=='R')
				{
					cnbr_num++;
				}
				else if(type[0]=='V'&&type[1]=='N'&&type[2]=='B'&&type[3]=='R')
				{
					vnbr_num++;
				}
				else
				{
					error_flag=1;
				}
					
				if(error_flag)
				{
					error_flag=0;
					HAL_UART_Transmit(&huart1,(uint8_t *)"Error",5,50);
				}
			}
			else
			{
				HAL_UART_Transmit(&huart1,(uint8_t *)"Error",5,50);
			}
		}
		
//		HAL_UART_Transmit(&huart1,(uint8_t *)"2",1,50);
		
	}
}

int to_num(uint8_t x)
{
	return ((int)x - 48);
}
