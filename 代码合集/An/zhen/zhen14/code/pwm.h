#ifndef _pwm_h
#define _pwm_h
#include "headfile.h"

extern uint32_t fre;

void change(void);

extern uint8_t duty;

extern double MH;
extern double ML;

extern double V;

typedef enum{
	WAITING,
	MONITORING,
}PeakState;

extern PeakState peak_state;

extern double candidate_V;

extern double DUTY;

#endif
