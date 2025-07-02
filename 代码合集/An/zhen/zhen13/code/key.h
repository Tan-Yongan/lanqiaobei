#ifndef _key_h
#define _key_h
#include "headfile.h"

void key_scan(void);
extern uint8_t led2_hold;
extern uint8_t input_conter;
extern uint8_t psd_sat;

extern uint8_t pwd1_init;
extern uint8_t pwd2_init;
extern uint8_t pwd3_init;

extern uint8_t pwd1_input;
extern uint8_t pwd2_input;
extern uint8_t pwd3_input;

extern uint8_t pwd[3];

#endif
