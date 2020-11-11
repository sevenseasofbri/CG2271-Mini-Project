#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"                    // Device header
#include "myInit.h"
#include "myMotor.h"
#include "myAudio.h"

#define MSG_COUNT 1

// Define commands for led rear control
#define LED_RED_MOVE 0x01
#define LED_RED_STOP 0x02

uint8_t rx_data = 0x00;
uint8_t uart_data;

volatile state_t state = STOP;

osMessageQueueId_t audioQ, motorQ, connectQ, ledRedQ, ledGreenQ;
osMessageQueueId_t controlQ;


/*----------------------------------------------------------------------------
 * LED LIBRARY FUNCTIONS
 *---------------------------------------------------------------------------*/

void offLED(void) {
 // should add some control here to decide which led to off (take in an arg?)
		PTA->PCOR |= MASK(LED_R1);
 // PTB->PCOR |= (MASK(LED_F8) | MASK(LED_F9) | MASK(LED_F10) | MASK(LED_F11));
 // PTE->PCOR |= (MASK(LED_F2) | MASK(LED_F3) | MASK(LED_F4) | MASK(LED_F5));
}

void greenFlashTwice() {
	int i;
	for(i = 0; i<2; i++) {
		PTE -> PSOR |= MASK(LED_F2) | MASK(LED_F3) | MASK(LED_F4) | MASK(LED_F5);
		PTB -> PSOR |= MASK(LED_F8) | MASK(LED_F9) | MASK(LED_F10) | MASK(LED_F11);
		osDelay(1000);
		PTE -> PCOR |= MASK(LED_F2) | MASK(LED_F3) | MASK(LED_F4) | MASK(LED_F5);
		PTB -> PCOR |= MASK(LED_F8) | MASK(LED_F9) | MASK(LED_F10) | MASK(LED_F11);
		osDelay(1000);
	}
}

/*----------------------------------------------------------------------------
 *  IRQ HANDLER FOR UART 
 *---------------------------------------------------------------------------*/
void UART2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	osStatus_t status;
	if (UART2->S1 & UART_S1_RDRF_MASK) {
	// received a character
		rx_data = UART2->D;
		status = osMessageQueuePut(controlQ, &rx_data , NULL, 0);  
	} 
	
  PORTE->ISFR = 0xffffffff;
	
}

/*----------------------------------------------------------------------------
 * Application main threads
 *---------------------------------------------------------------------------*/

/** MOTOR CONTROL THREAD **/
void motor_thread () {
	uint8_t rx_p;
	for(;;) {
	osMessageQueueGet(motorQ, &rx_p, NULL, osWaitForever);
	if(rx_p == STOP_MASK || rx_p == 0xFF){
			stopMotors();
			state = STOP;
		}else if(rx_p == FW_MASK && state != FORWARD){
			forward(state);
			state = FORWARD;
		}else if(rx_p == RV_MASK && state != REVERSE){
			reverse(state);
			state = REVERSE;
		}else if(rx_p == RT_MASK && state != RIGHT){
			right(state);
			state = RIGHT;
		}else if(rx_p == LT_MASK && state != LEFT){
			left(state);
			state = LEFT;
		}else if(rx_p == 0x02 && state != FWLEFT) {
			leftforward(state);
			state = FWLEFT;
		}else if(rx_p == 0x04 && state != FWRIGHT) {
			rightforward(state);
			state = FWRIGHT;
		}else if(rx_p == 0x06 && state != RVLEFT) {
			leftreverse(state);
			state = RVLEFT;
		}else if(rx_p == 0x08 && state != RVRIGHT) {
			rightreverse(state);
			state = RVRIGHT;
		}
	}
}

/** GREEN FRONT LED CONTROL THREAD **/
void  led_green_thread (void *argument) {
  uint8_t rx_p;
	uint8_t leds[8] = {LED_F2, LED_F3, LED_F4, LED_F5, LED_F11, LED_F10, LED_F9, LED_F8};
	int c = 0;
  for (;;) {
		osMessageQueueGet(ledGreenQ, &rx_p, NULL, 0);

    // if robot is stationary
		if (rx_p == 0x0B) {
			PTE -> PSOR |= MASK(LED_F2) | MASK(LED_F3) | MASK(LED_F4) | MASK(LED_F5);
			PTB -> PSOR |= MASK(LED_F8) | MASK(LED_F9) | MASK(LED_F10) | MASK(LED_F11);

		} else if(rx_p == 0x01) {
			greenFlashTwice();
			rx_p = 0;
			osMessageQueuePut(ledGreenQ, &rx_p, NULL, 0);
		} else { 
			c = (c + 1) % 8;
			if (c < 4) {
				PTE -> PSOR |= MASK(leds[c]);
				osDelay(250);
				PTE -> PCOR |= MASK(leds[c]);
			} else if (c >= 4) {
				PTB -> PSOR |= MASK(leds[c]);
				osDelay(250);
				PTB -> PCOR |= MASK(leds[c]);
			} 
		}
	}
}

/** RED BACK LED CONTROL THREAD **/
void  led_red_thread (void *argument) {
  uint8_t rx_p;
  for (;;) {
		osMessageQueueGet(ledRedQ, &rx_p, NULL, 0);
		// if robot is stationary
		if (rx_p == 0x0B) {
			// blink red led at 250ms 
			PTA -> PSOR |= MASK(LED_R1);
			osDelay(250);
			PTA -> PCOR |= MASK(LED_R1);
			osDelay(250);
		} else { 
			// blink red led at 500ms
			PTA -> PSOR |= MASK(LED_R1);
			osDelay(500);
			PTA -> PCOR |= MASK(LED_R1);
			osDelay(500);
		}
 }
}

/** 	AUDIO CONTROL THREAD **/
void audio_thread() {
	uint8_t rx_p = 0xFE;
	
	for(;;){
		osMessageQueueGet(audioQ, &rx_p, NULL, 0);
		if(rx_p == 0x01) {
			playStarTrekStartUp();
			rx_p = 0x00;
			osMessageQueuePut(audioQ, &rx_p, NULL, 0);
		} else if(rx_p != 0xFE && rx_p != 0xFF){
			playStarWars();
		} else if(rx_p == 0xFF) {
			playMarioGameOver();
			rx_p = 0x00;
			osMessageQueuePut(audioQ, &rx_p, NULL, 0);
		}
	}
}

/** MAIN CONTROL THREAD **/
void control_thread(void *argument) {
	for(;;) {
		osMessageQueueGet(controlQ, &uart_data, NULL, osWaitForever);
		osMessageQueuePut(motorQ, &uart_data, NULL, 0);
		osMessageQueuePut(ledGreenQ, &uart_data, NULL, 0); 
		osMessageQueuePut(ledRedQ, &uart_data, NULL, 0); 
		osMessageQueuePut(audioQ, &uart_data, NULL, 0);
		if(uart_data == 0x01) {
			connect = 0x01;
		} else if(uart_data == 0xFF) {
			end = 0xFF;
		} else {
			connect = 0xFE;
			end = 0xFE;
		}
	}
}
 
/** MAIN FUNCTION **/
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initClockGate();
	initUART2(BAUD_RATE);
	initPWM();
	initLED();
	initAudio();
	
	//Initialise Queues and Threads

  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	osThreadNew(control_thread, NULL, NULL);
	controlQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(motor_thread, NULL, NULL);
	motorQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(audio_thread, NULL, NULL);
	audioQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(led_green_thread, NULL, NULL);
	ledGreenQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(led_red_thread, NULL, NULL);
	ledRedQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
