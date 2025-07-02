#ifndef _key_h
#define _key_h
#include "headfile.h"

void key_scan(void);

typedef struct
{
	uint8_t B3_short;
	uint8_t B3_long;
	uint8_t B4_short;
	uint8_t B4_long;
}KEY_Read;
extern uint8_t time_select;
#endif
