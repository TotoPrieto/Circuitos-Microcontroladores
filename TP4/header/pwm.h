#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void initPWM(void);
void PWM_soft_Update(void);

extern volatile uint8_t PWM_DELTA;

#endif