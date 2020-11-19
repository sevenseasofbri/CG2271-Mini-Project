/*----------------------------------------------------------------------------
 * SYSTEM CONTROL
 *---------------------------------------------------------------------------*/
#include "MKL25Z4.h"                    // Device header
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "myInit.h"
#include "myMotor.h"
#include "myAudio.h"
#include "myLed.h"

#define MSG_COUNT 1

uint8_t rx_data = RESET;
uint8_t uart_data;

volatile state_t state = STOP;

osMessageQueueId_t audioQ, motorQ, ledRedQ, ledGreenQ;
osMessageQueueId_t controlQ;

/*----------------------------------------------------------------------------
 *  IRQ HANDLER FOR UART 
 *---------------------------------------------------------------------------*/
void UART2_IRQHandler(void) {
	
	NVIC_ClearPendingIRQ(UART2_IRQn);
	
	osStatus_t status; // For debugging purposes.
	
	if (UART2->S1 & UART_S1_RDRF_MASK) {
	// received a character
		rx_data = UART2->D;
		
		if(rx_data == THE_END) { //When "The End" is pressed, overrides everything.
			playMarioGameOver();
		} else if(rx_data == CONNECT){ //On establishing connection with bluetooth
			connect = CONNECT;
			end = CONNECT;
		}
		
		status = osMessageQueuePut(controlQ, &rx_data , NULL, 0);  
	} 
	
  PORTE->ISFR = CLEAR_IRQ;
	
}

/*----------------------------------------------------------------------------
 * Application main threads
 *---------------------------------------------------------------------------*/

/** MOTOR CONTROL THREAD **/
void tMotorControl() {
			uint8_t rx_p = RESET;
	for(;;) {
	osMessageQueueGet(motorQ, &rx_p, NULL, 0);
	if(rx_p == FW_MASK && state != FORWARD){
			forward(state);
			state = FORWARD;
		}else if(rx_p == RV_MASK && state != REVERSE){
			reverse(state);
			state = REVERSE;
		}else if(rx_p == RT_MASK && state != RIGHT){
			right(state);
			state = RIGHT;
		}else if(rx_p== LT_MASK && state != LEFT){
			left(state);
			state = LEFT;
		}else if(rx_p == FWLT_MASK && state != FWLEFT) {
			leftforward(state);
			state = FWLEFT;
		}else if(rx_p == FWRT_MASK && state != FWRIGHT) {
			rightforward(state);
			state = FWRIGHT;
		}else if(rx_p == RVLT_MASK && state != RVLEFT) {
			leftreverse(state);
			state = RVLEFT;
		}else if(rx_p == RVRT_MASK && state != RVRIGHT) {
			rightreverse(state);
			state = RVRIGHT;
		} else {
			stopMotors();
			state = STOP;
		}
	}
}

/** GREEN FRONT LED CONTROL THREAD **/
void  tLedGreen(void *argument) {
  uint8_t rx_p = INIT_VAR;
	int c = 0;
  for (;;) {
		osMessageQueueGet(ledGreenQ, &rx_p, NULL, 0);

		if (rx_p == STOP || rx_p == THE_END) {     // If robot is stationary or has completed the run.
			greenStoppedState();
		} else if(rx_p == CONNECT) {     //On connection with bluetooth.
			greenFlashTwice();
			rx_p = STOP;		//To put LEDs immediately into STOP mode after connection.
			osMessageQueuePut(ledGreenQ, &rx_p, NULL, 0);
			osMessageQueuePut(ledRedQ, &rx_p, NULL, 0);
		} else if(rx_p != INIT_VAR) { // If the robot is moving.
			c = (c + 1) % 8;
			greenMovingState(c);
		}
	}
}

/** RED BACK LED CONTROL THREAD **/
void  tLedRed(void *argument) {
  uint8_t rx_p = INIT_VAR;
  for (;;) {
		osMessageQueueGet(ledRedQ, &rx_p, NULL, 0);
		
		if (rx_p == STOP || rx_p == THE_END) {			// Blink red led at 250ms if the robot is stationary.
			redStoppedState();
		} else if(rx_p != INIT_VAR && rx_p != CONNECT){ 			// Blink red led at 500ms if robot is moving.
			redMovingState();
		}
	}
}

/** 	AUDIO CONTROL THREAD **/
void tAudio() {
	uint8_t rx_p = INIT_VAR;
	
	for(;;){
		osMessageQueueGet(audioQ, &rx_p, NULL, 0);
		if(rx_p == CONNECT) { //Plays star trek startup tune on connection.
			playStarTrekStartUp();
			rx_p = RESET;
			osMessageQueuePut(audioQ, &rx_p, NULL, 0);
		} else if(rx_p == THE_END) { //Makes sure nothing plays after "The End" is pressed.
			rx_p = INIT_VAR;
			osMessageQueuePut(audioQ, &rx_p, NULL, 0);
		}else if(rx_p != INIT_VAR){ // Otherwise plays hedwigs theme all the way through (after connection to the end).
			playHedwigsTheme();
		} 
	}
}

/** MAIN CONTROL THREAD **/
void tBrain(void *argument) {
	for(;;) {
		osMessageQueueGet(controlQ, &uart_data, NULL, osWaitForever);
		osMessageQueuePut(motorQ, &uart_data, NULL, 0);
		osMessageQueuePut(ledGreenQ, &uart_data, NULL, 0); 
		osMessageQueuePut(ledRedQ, &uart_data, NULL, 0); 
		osMessageQueuePut(audioQ, &uart_data, NULL, 0);
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
	
	//Initialise Queues, Threads and Kernel

  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	osThreadNew(tBrain, NULL, NULL);
	controlQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(tMotorControl, NULL, NULL);
	motorQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(tAudio, NULL, NULL);
	audioQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(tLedGreen, NULL, NULL);
	ledGreenQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(tLedRed, NULL, NULL);
	ledRedQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
  osKernelStart();                      // Start thread execution
	
  for (;;) {}
}
