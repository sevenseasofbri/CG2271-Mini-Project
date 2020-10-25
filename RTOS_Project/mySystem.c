#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"                    // Device header
#include "myInit.h"

#define MSG_COUNT 1



// Define commands for led rear control
#define LED_RED_MOVE 0x01
#define LED_RED_STOP 0x02

volatile int speed = 2;
uint8_t rx_data = 0x01;
uint8_t uart_data;

unsigned int counter = 0;

typedef struct {
	uint8_t cmd;
	uint8_t data;
} dataPkt;

typedef enum colours {
 led_a = LED_R1, 
} colours_t;

typedef enum onOff {
 on,
 off
} ledOnOff;

char led_colours[2][2] = {{0, led_a}};

	
osMessageQueueId_t ledQ, audioQ, motorQ, connectQ;
osMessageQueueId_t controlQ;


/*switch off the LED*/
void offLED(void) {
 // should add some control here to decide which led to off (take in an arg?)
 PTA->PCOR |= MASK(LED_R1);
 // PTB->PCOR |= (MASK(LED_F8) | MASK(LED_F9) | MASK(LED_F10) | MASK(LED_F11));
 // PTE->PCOR |= (MASK(LED_F2) | MASK(LED_F3) | MASK(LED_F4) | MASK(LED_F5));
}

/**high-level library function
*takes in colour and controls appropriate LED*/
void led_control(colours_t colour, ledOnOff on_off) {

 if(on_off == on) {
  switch(colour) {
   case led_a:
     PTA->PSOR |= MASK(LED_R1);
    break;
   default:
    offLED();
  } 
 } else if(on_off == off) {
   switch(colour) {
   case led_a:
     PTA->PCOR |= MASK(LED_R1); 
    break;
   default:
    offLED();
  }
 }
}

/* IRQ HANDLER FOR UART */
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

/* Delay Function */

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}
/* MOTORS */
/** Stop the motors**/
void stopMotors(){
	TPM1->MOD = 0;
	TPM1_C0V = 0; // stop left fw
	TPM1_C1V = 0; // stop left bk
	
	TPM2->MOD = 0;
	TPM2_C0V = 0; // stop right fw
	TPM2_C1V = 0; // stop right bk
}

/** Move Reverse **/ 
void reverse() {
  TPM1->MOD = 7500;
	TPM1_C0V = MAX_DUTY_CYCLE/speed;
	TPM2->MOD = 7500;
	TPM2_C0V = MAX_DUTY_CYCLE/speed;
}

/** Move Forward **/
void forward() {
	 TPM1->MOD = 7500;
	 TPM1_C1V = MAX_DUTY_CYCLE/speed;
	 TPM2->MOD = 7500;
	 TPM2_C1V = MAX_DUTY_CYCLE/speed;
}

/** Rotate Right **/
void right() {
	 TPM1->MOD = 7500;
	 TPM1_C1V = MAX_DUTY_CYCLE/speed;
	 TPM2->MOD = 0;
	 TPM2_C1V = 0;
}

/** Rotate Left **/
void left() {
	 TPM2->MOD = 7500;
	 TPM2_C1V = MAX_DUTY_CYCLE/speed;
	 TPM1->MOD = 0;
	 TPM1_C1V = 0;
}
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
//Need a status flag to prevent continously resetting the MODs
void motor_thread () {
	uint8_t rx_p;
	for(;;) {
	osMessageQueueGet(motorQ, &rx_p, NULL, osWaitForever);
	if(rx_p == FW_MASK){
			forward();
		}else if(rx_p == RV_MASK){
			reverse();
		}else if(rx_p == RT_MASK){
			right();
		}else if(rx_p == LT_MASK){
			left();
		}else if(rx_p == STOP_MASK){
			stopMotors();
		}	
	}
}

void  led_rear_thread (void *argument) {
  uint8_t rx_p;
  for (;;) {
		osMessageQueueGet(ledQ, &rx_p, NULL, osWaitForever);
    
		if(rx_p == 0x03) {
			PTE -> PSOR |= MASK(2);
		} else {
			PTE -> PCOR |= MASK(2);
		}
 }
}

void control_thread(void *argument) {
	for(;;) {
		osMessageQueueGet(controlQ, &uart_data, NULL, osWaitForever);
		osMessageQueuePut(ledQ, &uart_data, NULL, 0); 
		osMessageQueuePut(motorQ, &uart_data, NULL, 0);
		osDelay(1000);
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initClockGate();
	initUART2(BAUD_RATE);
	initPWM();
	initLED();
	//Initialise Queues
	
	
	// audioQ = osMessageQueueNew(MSG_COUNT, sizeof(dataPkt), NULL);
	// connectQ = osMessageQueueNew(MSG_COUNT, sizeof(dataPkt), NULL);
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  //led_flag = osEventFlagsNew(NULL);
	osThreadNew(control_thread, NULL, NULL);
	controlQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	osThreadNew(led_rear_thread, NULL, NULL);
	ledQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	osThreadNew (motor_thread, NULL, NULL);
	motorQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
