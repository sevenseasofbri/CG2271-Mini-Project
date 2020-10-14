/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#include "MKL25Z4.h"                    // Device header

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define A 2 // PortE Pin 2
#define B 3 //PortE Pin 3
#define MASK(x) (1 << (x))

unsigned int counter = 0;

typedef enum colours {
	red_led = RED_LED,
	green_led = GREEN_LED, 
	blue_led = BLUE_LED
} colours_t;

typedef enum onOff {
	on,
	off
} ledOnOff;
	
char led_colours[3][2] = {{0,red_led}, {1,green_led}, {2,blue_led}};

/* GPIO initialization function */
void initGPIO(void) {

	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTE_MASK));

	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
	PORTE->PCR[A] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[A] |= PORT_PCR_MUX(1);
	PORTE->PCR[B] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[B] |= PORT_PCR_MUX(1);

	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
	PTE->PDDR |= (MASK(A) | MASK(B));

}

/*switch off the LED*/
void offLED(void) {
	PTB->PSOR = MASK(RED_LED) | MASK(GREEN_LED); //since LED is active low
	PTD->PSOR = MASK(BLUE_LED);
	PTE->PCOR |= MASK(A) | MASK(B); //LED here is active high
}

/**high-level library function
*takes in colour and controls appropriate LED*/
void led_control(colours_t colour, ledOnOff on_off) {

	if(on_off == on) {
		switch(colour) {
			case red_led:
				PTB->PCOR |= MASK(RED_LED);
			  PTE->PSOR |= MASK(A);
				break;
			case green_led:
				PTB->PCOR |= MASK(GREEN_LED);
			  PTE->PSOR |= MASK(B);
				break;
			case blue_led:
				PTD->PCOR |= MASK(BLUE_LED);
				break;
			default:
				offLED();
		} 
	} else if(on_off == off) {
			switch(colour) {
			case red_led:
				PTB->PSOR |= MASK(RED_LED);
			  PTE->PCOR |= MASK(A); 
				break;
			case green_led:
				PTB->PSOR |= MASK(GREEN_LED);
			  PTE->PCOR |= MASK(B);
				break;
			case blue_led:
				PTD->PSOR |= MASK(BLUE_LED);
				break;
			default:
				offLED();
		}
	}
}
 
/* Delay Function */

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

const osThreadAttr_t thread_attr = {
	.priority = osPriorityAboveNormal1
};

osMutexId_t myMutex;

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void  led_red_thread (void *argument) {
 
  // ...
  for (;;) {
		osMutexAcquire(myMutex, osWaitForever);
		led_control(red_led, on);
		osDelay(1000);
		led_control(red_led, off);
		osDelay(1000);
		osMutexRelease(myMutex);
	}
}

void  green_red_thread (void *argument) {
 
  // ...
  for (;;) {
		osMutexAcquire(myMutex, osWaitForever);
		led_control(green_led, on);
		osDelay(1000);
		led_control(green_led, off);
		osDelay(1000);
		osMutexRelease(myMutex);
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
  initGPIO();
	offLED();
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	myMutex = osMutexNew(NULL);
  osThreadNew(led_red_thread, NULL, NULL);    // Create application main thread
	osThreadNew(green_red_thread, NULL, NULL);    // Create application main thread
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
