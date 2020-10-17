#include "MKL25Z4.h"                    // Device header
#define MAX_DUTY_CYCLE 0x1D4C //7500 (50Hz)
#define LEFT_FW 0 // PTB0 TPM1_CH0
#define LEFT_BK 1 // PTB1 TPM1_CH1
#define RIGHT_FW 2 // PTB2 TPM2_CH0
#define RIGHT_BK 3 // PTB3 TPM2_CH1
#define UART_RX_PORTE23 23 //PTE 23 Rx
#define BAUD_RATE 9600
#define UART2_INT_PRIO 128	
#define FW_MOTOR 3
#define RV_MOTOR 5
#define RT_MOTOR 9
#define LT_MOTOR 7
#define STOP_MOTOR 11
#define MASK(x) (1 << (x))
#define FW_MASK(x) (x&0x03)
#define RV_MASK(x) (x&0x05)
#define STOP_MASK(x) (x&0x0B)
#define RT_MASK(x) (x&0x09)
#define LT_MASK(x) (x&0x07)

volatile int speed = 2;
volatile uint8_t rx_data = 0x01;

void UART2_IRQHandler(void) {
	
	NVIC_ClearPendingIRQ(UART2_IRQn);
	
	if (UART2->S1 & UART_S1_RDRF_MASK) {
	// received a character
		rx_data = UART2->D;
	} 
	
  PORTE->ISFR = 0xffffffff;
	
}

void initPWM() {
	// Enable Clock Gating for PORTB
	SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK);

	// Configure Mode 3 for PWM pin operation
	PORTB->PCR[LEFT_FW] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_FW] |= PORT_PCR_MUX(3);
	PORTB->PCR[LEFT_BK] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_BK] |= PORT_PCR_MUX(3);
	
	PORTB->PCR[RIGHT_FW] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_FW] |= PORT_PCR_MUX(3);
	PORTB->PCR[RIGHT_BK] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_BK] |= PORT_PCR_MUX(3);
	
	//Enable clock gating for Timer1
	SIM->SCGC6 = (SIM_SCGC6_TPM1_MASK)|(SIM_SCGC6_TPM2_MASK);
	
	//Select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK OR MCGPLLCLK/2
	
	//set modulo value 48000000/128 = 375000, 375000Hz/50Hz = 7500	
	TPM1->MOD = 7500;
	TPM2->MOD = 7500;
	
	//Edge-Aligned PWM
	//CMOD - 1 and PS - 111 (128)
	TPM1_SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1_SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //CMOD = 1 => LPTPM counter increments on every LPTPM counter clock
	TPM1_SC &= ~(TPM_SC_CPWMS_MASK); //count up by default (0)
  
	TPM2_SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2_SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //CMOD = 1 => LPTPM counter increments on every LPTPM counter clock
	TPM2_SC &= ~(TPM_SC_CPWMS_MASK); //count up by default (0)
	
	//enable PWM on TPM1 channel 0 - PTB0
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM1 channel 1 - PTB1
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM2 channel 0 - PTB0
	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM2 channel 1 - PTB1
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

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

/* Delay Function */

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

static void delay100x(volatile uint32_t nof) {
 for(int i =0;i<100;i++) {
  delay(nof);
 }
}

/** Stop the motors**/
void stopMotors(){
	TPM1->MOD = 0;
	TPM1_C0V = 0; // stop left fw
	TPM1_C1V = 0; // stop left bk
	
	TPM2->MOD = 0;
	TPM2_C0V = 0; // stop right fw
	TPM2_C1V = 0; // stop right bk
}

/** Move Forward **/
void forward() {
  TPM1->MOD = 7500;
	TPM1_C0V = MAX_DUTY_CYCLE/speed;
	TPM2->MOD = 7500;
	TPM2_C0V = MAX_DUTY_CYCLE/speed;
}

/** Move Reverse **/
void reverse() {
	 TPM1->MOD = 7500;
	 TPM1_C1V = MAX_DUTY_CYCLE/speed;
	 TPM2->MOD = 7500;
	 TPM2_C1V = MAX_DUTY_CYCLE/speed;
}

int main(void) {

 SystemCoreClockUpdate();
 initUART2(BAUD_RATE);
 initPWM();
	
 //TPM1_C0V = 0x0ea6; //50% duty cycle -> 7500/2 = 3750 in hex:0x0ea6
 //TPM1_C1V = 0x0753; //25% duty cycle -> 7500/4 = 1875 in hex:0x0753
 
 while(1) { // Forward, stop, backward, stop
		if(FW_MASK(rx_data)==FW_MOTOR){
			forward();
		}else if(RV_MASK(rx_data)==RV_MOTOR){
			reverse();
		}/**else if(FW_MASK(rx_data)==FW_MOTOR){
			forward();
		}else if(FW_MASK(rx_data)==FW_MOTOR){
			forward();
		}**/else if(STOP_MASK(rx_data)==STOP_MOTOR){
			stopMotors();
		}	
	}
}