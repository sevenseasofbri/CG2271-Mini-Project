#ifndef LED_H
#define LED_H
#include "MKL25Z4.h"                    // Device header
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "myInit.h"

// LED SPEEDS
#define RED_STOP_TIME 250 // milliseconds
#define RED_MOVING_TIME 500 //milliseconds
#define GREEN_FLASH_TIME 1000 //milliseconds
#define GREEN_MOVING_TIME 250 //milliseconds


void offLED(void);
void greenFlashTwice(void);
void redStoppedState(void);
void redMovingState(void);
void greenStoppedState(void);
void greenMovingState(int);

#endif
