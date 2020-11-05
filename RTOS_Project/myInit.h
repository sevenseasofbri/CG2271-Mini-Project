#ifndef INIT_H
#define INIT_H
#include "MKL25Z4.h"                    // Device header
#include <math.h>

#define MASK(x) (1 << (x))

// Defining Motors Remember to reverse the connection when physically making the connections AAAAAAAAAAAAAAA
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

// SOUND

#define PTD0_Pin 0
#define PTD1_Pin 1
#define NOTE_CNT 25
#define TO_MOD(x) 375000/(x)
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  261
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS 455
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 830
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0



void initClockGate(void);
void initPWM(void);
void initUART2(uint32_t);
void initLED(void);
void initAudio(void);
#endif
