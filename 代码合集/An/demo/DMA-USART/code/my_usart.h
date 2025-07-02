#ifndef _my_usart_h
#define _my_usart_h
#include "headfile.h"

extern char send_buf[100];
extern char rx_buf[100];

void rx_handle(void);

#endif
