#include "MKL25Z4.h"                    // Device header
#define BAUD_RATE 9600
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128										 	
#define MASK(x) (1 << (x))

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1

#define LED_RED 2
#define LED_MASK(x) (x&0x06)
#define BIT0_MASK(x) (x&0x01)

volatile uint8_t rx_data = 0x01;

/*	Init UART2	*/
void initUART2(uint32_t baud_rate){
	
	uint32_t divisor, bus_clock;
	
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	PORTE-> PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE-> PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
	
	UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
	
	bus_clock = DEFAULT_SYSTEM_CLOCK/2;
	divisor = bus_clock / (baud_rate*16);
	UART2->BDH = UART_BDH_SBR(divisor >> 8);
	UART2->BDL = UART_BDL_SBR(divisor);
	
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C3 = 0;
	
	UART2->C2 |= (UART_C2_RE_MASK);
	NVIC_SetPriority(UART2_IRQn, 128);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	UART2->C2 |= UART_C2_RIE_MASK;

}

unsigned int counter = 0;

typedef enum colours {
	red_led = RED_LED,
	green_led = GREEN_LED, 
	blue_led = BLUE_LED
} colours_t;
	
char led_colours[3][2] = {{0,red_led}, {1,green_led}, {2,blue_led}};

/* GPIO initialization function */
void initGPIO(void) {

	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));

	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);

	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);

}

//switch of the LED
void offLED(void) {
	PTB->PSOR = MASK(RED_LED) | MASK(GREEN_LED); //since LED is active low
	PTD->PSOR = MASK(BLUE_LED);
}

//high-level library function
//takes in colour and controls appropriate LED
void led_control(colours_t colour) {
	offLED();
	switch(colour) {
		case red_led:
			PTB->PCOR = MASK(RED_LED);
			break;
		case green_led:
			PTB->PCOR = MASK(GREEN_LED);
			break;
		case blue_led:
			PTD->PCOR = MASK(BLUE_LED);
			break;
		default:
			offLED();
	}
}
/* UART TRANSMIT POLL */

//void UART2_Transmit_Poll(uint8_t data){
//	while(!(UART2->S1 & UART_S1_TDRE_MASK));
//	UART2->D = data;
//}

//uint8_t UART2_Receive_Poll(void){
//	while(!(UART2->S1 & UART_S1_RDRF_MASK));
//	return (UART2->D);
//}

void UART2_IRQHandler(void) {
	
	NVIC_ClearPendingIRQ(UART2_IRQn);
	
	if (UART2->S1 & UART_S1_RDRF_MASK) {
	// received a character
		rx_data = UART2->D;
	} 
	
	PORTD->ISFR = 0xffffffff;
	
}

/* DELAY ROUTINE*/
static void delay(volatile uint32_t nof){
	while(nof!=0){
		__asm("NOP");
		nof--;
	}
}

/* MAIN FUNCTION */
int main(void){
	
	SystemCoreClockUpdate();
	initUART2(BAUD_RATE);
	initGPIO();
	offLED();
	while(1){
		/* RX and TX*/
		//UART2_Transmit_Poll(rx_data);
		//rx_data = UART2_Receive_Poll();
		
		if(LED_MASK(rx_data)==LED_RED){
			if(BIT0_MASK(rx_data))
				led_control(led_colours[0][1]);
			else
				offLED();
		}
		else
			offLED();
	}
}