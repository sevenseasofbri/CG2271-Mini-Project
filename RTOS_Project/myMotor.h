#ifndef MOTOR_H
#define MOTOR_H
#include "MKL25Z4.h"                    // Device header
#include <math.h>

#define MAX_DUTY_CYCLE 0x1D4C //7500 (50Hz)

typedef enum state {
	FORWARD,
	REVERSE,
	RIGHT,
	LEFT,
	FWLEFT,
	RVLEFT,
	FWRIGHT,
	RVRIGHT,
	STOP
} state_t;

void stopMotors(void);
void reverse(state_t);
void forward(state_t);
void right(state_t);
void left(state_t);
void leftforward(state_t);
void leftreverse(state_t);
void rightforward(state_t);
void rightreverse(state_t);

#endif
