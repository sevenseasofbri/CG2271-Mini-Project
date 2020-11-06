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
uint8_t rx_data = 0x00;
uint8_t uart_data;

unsigned int counter = 0;

typedef struct {
	uint8_t cmd;
	uint8_t data;
} dataPkt;

typedef enum colours {
 led_a = LED_R1, 
} colours_t;

typedef enum state {
	FORWARD,
	REVERSE,
	RIGHT,
	LEFT,
	STOP
} state_t;

typedef enum onOff {
 on,
 off
} ledOnOff;

char led_colours[2][2] = {{0, led_a}};

// change this to make the song slower or faster
int tempo = 144;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int hedwigsThemeMelody[] = {


  // Hedwig's theme fromn the Harry Potter Movies
  // Socre from https://musescore.com/user/3811306/scores/4906610
  
  REST, 2, NOTE_D4, 4,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_C5, -2, 
  NOTE_A4, -2,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_F4, 2, NOTE_GS4, 4,
  NOTE_D4, -1, 
  NOTE_D4, 4,

  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4, //10
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_B4, 4,
  NOTE_G4, -1,
  NOTE_AS4, 4,
     
  NOTE_D5, 2, NOTE_AS4, 4,//18
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_DS5, 2, NOTE_D5, 4,
  NOTE_CS5, 2, NOTE_A4, 4,
  NOTE_AS4, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_D4, 4,
  NOTE_D5, -1, 
  REST,4, NOTE_AS4,4,  

  NOTE_D5, 2, NOTE_AS4, 4,//26
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_AS4, 4,
  NOTE_G4, -1, 
  
};

int starWarsMelody [] = {
	NOTE_A4, 500, NOTE_A4, 500, NOTE_A4, 500, NOTE_A4, 350, NOTE_C5, 150, NOTE_A4, 500, NOTE_F4, 350, 
	NOTE_C5, 150, NOTE_A4, 650, REST, 500, NOTE_E5, 500, NOTE_E5, 500, NOTE_E5, 500, NOTE_F5, 350, 
	NOTE_C5, 150, NOTE_GS4, 500, NOTE_F4, 350, NOTE_C5, 150, NOTE_A4, 650, REST, 500, NOTE_A5, 500, 
	NOTE_A4, 300, NOTE_A4, 150, NOTE_A5, 500, NOTE_GS5, 325, NOTE_G5, 175, NOTE_FS5, 125, NOTE_F5, 125, 
	NOTE_FS5, 250, REST, 325, NOTE_AS, 250, NOTE_DS5, 500, NOTE_D5, 325, NOTE_CS5, 175, NOTE_C5, 125, 
	NOTE_AS4, 125, NOTE_C5, 250, REST, 350, NOTE_F4, 250, NOTE_GS4, 500, NOTE_F4, 350, NOTE_A4, 125, 
	NOTE_C5, 500, NOTE_A4, 375, NOTE_C5, 125, NOTE_E5, 650, REST, 500, NOTE_A5, 500, NOTE_A4, 300, 
	NOTE_A4, 150, NOTE_A5, 500, NOTE_GS5, 325, NOTE_G5, 175, NOTE_FS5, 125, NOTE_F5, 125, NOTE_FS5, 250, 
	REST, 325, NOTE_AS, 250, NOTE_DS5, 500, NOTE_D5, 325, NOTE_CS5, 175, NOTE_C5, 125, NOTE_AS4, 125, 
	NOTE_C5, 250, REST, 350, NOTE_F4, 250, NOTE_GS4, 500, NOTE_F4, 375, NOTE_C5, 125, NOTE_A4, 500, 
	NOTE_F4, 375, NOTE_C5, 125, NOTE_A4, 650, REST, 650
};


volatile state_t state = STOP;

osMessageQueueId_t ledQ, audioQ, motorQ, connectQ;
osMessageQueueId_t controlQ;
osEventFlagsId_t control;

/* Delay Functions */

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

//1 nop takes 1 cycle? So 1 delay is 1/48 microsec

/*
Usage Example:
delay(0x80000);
*/

static void delay100x(volatile uint32_t nof) {
	for(int i =0;i<100;i++) {
		delay(nof);
	}
}


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

/* AUDIO FUNCTIONS */

void playHedwigsTheme() {
	// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
	// there are two values per note (pitch and duration), so for each note there are four bytes
	int notes = sizeof(hedwigsThemeMelody) / sizeof(hedwigsThemeMelody[0]);

	// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
	int wholenote = (60000 * 4) / tempo;

	int divider = 0, noteDuration = 0;

	uint32_t period;


		for(int i = 0; i<notes; i+=2) {
			// calculates the duration of each note
			divider = hedwigsThemeMelody[i + 1];
			if (divider > 0) {
				// regular note, just proceed
				noteDuration = (wholenote) / divider;
			} else if (divider < 0) {
				// dotted notes are represented with negative durations!!
				noteDuration = (wholenote) / (int)fabs((float)divider);
				noteDuration *= 1.5; // increases the duration in half for dotted notes
			}
			period = TO_MOD(hedwigsThemeMelody[i]);
			TPM0->MOD = period;
			TPM0_C0V = period / 8; //12.5% duty cycle
			osDelay(noteDuration*5/9);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			osDelay(noteDuration*25/81);
		}
}

void playStarWars() {
	int notes = sizeof(starWarsMelody) / sizeof(starWarsMelody[0]);

	int noteDuration = 0, period = 0;
		
		for(int i = 0; i<notes; i+=2) {
			// calculates the duration of each note
			noteDuration = starWarsMelody[i + 1];
			period = TO_MOD(starWarsMelody[i]);
			TPM0->MOD = period;
			TPM0_C0V = period / 4; //12.5% duty cycle
			osDelay(noteDuration*2/3);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			osDelay(noteDuration*16/25);
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
void reverse(state_t s) {
	if(state != REVERSE) {
		TPM1_C1V = 0;
		TPM2_C1V = 0;
	}
  TPM1->MOD = 7500;
	TPM1_C0V = MAX_DUTY_CYCLE;
	TPM2->MOD = 7500;
	TPM2_C0V = MAX_DUTY_CYCLE;
}

/** Move Forward **/
void forward(state_t s) {
	 if(s != FORWARD) {
		TPM1_C0V = 0;
		TPM2_C0V = 0;
	 }
	 TPM1->MOD = 7500;
	 TPM1_C1V = MAX_DUTY_CYCLE;
	 TPM2->MOD = 7500;
	 TPM2_C1V = MAX_DUTY_CYCLE;
}

/** Rotate Right **/
void right(state_t s) {
	 if (s != RIGHT) {
		  TPM1_C0V = 0;
		  TPM2_C1V = 0;
	 }
	 TPM1->MOD = 7500;
	 TPM1_C1V = MAX_DUTY_CYCLE/2;
	 TPM2->MOD = 7500;
	 TPM2_C0V = MAX_DUTY_CYCLE/2;
}

/** Rotate Left **/
void left(state_t s) {
	 if (s != LEFT) { 
		 TPM1_C1V = 0;
		 TPM2_C0V = 0;
	 }
	 TPM2->MOD = 7500;
	 TPM2_C1V = MAX_DUTY_CYCLE/2;
	 TPM1->MOD = 7500;
	 TPM1_C0V = MAX_DUTY_CYCLE/2;
}

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/


void motor_thread () {
	uint8_t rx_p;
	for(;;) {
	osMessageQueueGet(motorQ, &rx_p, NULL, osWaitForever);
	if(rx_p == STOP_MASK){
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
		}
	}
}

void  led_green_thread (void *argument) {
  uint8_t rx_p;
	uint8_t leds[8] = {LED_F2, LED_F3, LED_F4, LED_F5, LED_F11, LED_F10, LED_F9, LED_F8};
	int c = 0;
  for (;;) {
		osMessageQueueGet(ledGreenQ, &rx_p, NULL, osWaitForever);
		
    // if robot is stationary
		if (rx_p == 0x11) {
			PTE -> PSOR |= MASK(LED_F2) | MASK(LED_F3) | MASK(LED_F4) | MASK(LED_F5);
			PTB -> PSOR |= MASK(LED_F8) | MASK(LED_F9) | MASK(LED_F10) | MASK(LED_F11);
			
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

void  led_red_thread (void *argument) {
  uint8_t rx_p;
  for (;;) {
		osMessageQueueGet(ledRedQ, &rx_p, NULL, osWaitForever);
    
		// if robot is stationary
		if (rx_p == 0x11) {
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

/** 	AUDIO THREAD **/
void audio_thread() {
	uint8_t rx_p = 0xFF;
	
	for(;;){
		osMessageQueueGet(audioQ, &rx_p, NULL, 0);
		if(rx_p == 0x01) {
			playStarWars();
			rx_p = 0x00;
			osMessageQueuePut(audioQ, &rx_p, NULL, 0);
		} else if(rx_p != 0xFF){
			playHedwigsTheme();
		}
	}
}

void control_thread(void *argument) {
	for(;;) {
		osMessageQueueGet(controlQ, &uart_data, NULL, osWaitForever);
		osMessageQueuePut(ledGreenQ, &uart_data, NULL, 0); 
		osMessageQueuePut(ledRedQ, &uart_data, NULL, 0); 
		osMessageQueuePut(motorQ, &uart_data, NULL, 0);
		osMessageQueuePut(audioQ, &uart_data, NULL, 0);
		//osDelay(1000);
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initClockGate();
	initUART2(BAUD_RATE);
	initPWM();
	initLED();
	initAudio();
	//Initialise Queues
	
	// connectQ = osMessageQueueNew(MSG_COUNT, sizeof(dataPkt), NULL);
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	osThreadNew(control_thread, NULL, NULL);
	controlQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(led_rear_thread, NULL, NULL);
	ledQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(motor_thread, NULL, NULL);
	motorQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
	osThreadNew(audio_thread, NULL, NULL);
	audioQ = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
