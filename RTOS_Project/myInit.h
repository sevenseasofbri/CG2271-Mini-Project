#ifndef INIT_H
#define INIT_H
#include "MKL25Z4.h"                    // Device header

#define MASK(x) (1 << (x))

// Defining Motors
#define LEFT_BK 0 // PTB0 TPM1_CH0
#define LEFT_FW 1 // PTB1 TPM1_CH1
#define RIGHT_BK 2 // PTB2 TPM2_CH0
#define RIGHT_FW 3 // PTB3 TPM2_CH1
#define UART_RX_PORTE23 23 //PTE 23 Rx
#define BAUD_RATE 9600
#define UART2_INT_PRIO 128	

// Rear LED Pin Config
#define LED_R1 1 // PortA Pin 1

// Front LED Pin Config
#define LED_F2 2 // PortE Pin 2
#define LED_F3 3 // PortE Pin 3
#define LED_F4 4 // PortE Pin 4
#define LED_F5 5 // PortE Pin 5
#define LED_F8 8 // PortB Pin 8
#define LED_F9 9 // PortB Pin 9
#define LED_F10 10 // PortB Pin 10
#define LED_F11 11 // PortB Pin 11

// PWM
#define MAX_DUTY_CYCLE 0x1D4C //7500 (50Hz)
#define FW_MOTOR 3
#define RV_MOTOR 5
#define RT_MOTOR 9
#define LT_MOTOR 7
#define STOP_MOTOR 11
#define MASK(x) (1 << (x))
#define FW_MASK 0x03
#define RV_MASK 0x05
#define STOP_MASK 0x0B
#define RT_MASK 0x09
#define LT_MASK 0x07

void initClockGate(void);
void initPWM(void);
void initUART2(uint32_t);
void initLED(void);
#endif
