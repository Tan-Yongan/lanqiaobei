#ifndef _my_usart_h
#define _my_usart_h
#include "headfile.h"

extern char send_buff[100];
extern char rx_buff[100];

void rx_handle(void);

#endif