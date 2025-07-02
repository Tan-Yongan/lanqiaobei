#ifndef _key_h
#define _key_h
#include "headfile.h"

void key_scan(void);
extern uint8_t show_switch;
extern uint8_t code_switch;
extern uint8_t pwd_in[3];
extern uint8_t pwd_real[3];

extern uint8_t enter_flag;
extern uint8_t start_flag;
extern uint8_t temp_p1;

#endif