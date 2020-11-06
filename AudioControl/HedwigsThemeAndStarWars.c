#include "MKL25Z4.h"                    // Device header
#include <math.h>

/*
PTB18 -> TM2 CH0
PTB19 -> TM2 CH1
*/

#define PTD0_Pin 0
#define PTD1_Pin 1
#define NOTE_CNT 25
#define TO_MOD(x) 375000/(x)

/*int notes[NOTE_CNT] = { 262, 262, 294, 262, 349, 330, 262, 262, 294, 262, 392,
 
                      you, Hap py  Birth Day  dear  xxxx      Hap  py   birth
                         F4   C4   C4   C5   A4   F4   E4   D4   B4b  B4b  A4 
                         349, 262, 262, 523, 440, 349, 330, 294, 466, 466, 440,
 
                      day  to  you
                         F4   G4   F4  
                         349, 392, 349
                        };*/
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


//int win7StartUpMelody [] = {
//	NOTE_B5, 250, NOTE_E1, 500, NOTE_FS2, 250, NOTE_B5, 1000
//};

int starTrekStartUpMelody[] = {
	
  //available at https://musescore.com/user/10768291/scores/4594271
 
  NOTE_D4, -8, NOTE_G4, 16, NOTE_C5, -4, 
  NOTE_B4, 8, NOTE_G4, -16, NOTE_E4, -16, NOTE_A4, -16,
  NOTE_D5, 2,
  
};

int marioGameOverMelody [] = {
	  //game over sound
  NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, //45
  NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8,
  NOTE_G4,8, NOTE_D4,8, NOTE_E4,-2,  
};

void initPWM() {
	// Enable Clock Gating for PORTB
	SIM->SCGC5 = (SIM_SCGC5_PORTD_MASK);

	// Configure Mode 3 for PWM pin operation
	PORTD->PCR[PTD0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD0_Pin] |= PORT_PCR_MUX(4);
	PORTD->PCR[PTD1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD1_Pin] |= PORT_PCR_MUX(4);
	
	//Enable clock gating for Timer1
	SIM->SCGC6 = (SIM_SCGC6_TPM0_MASK);
	
	//Select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK OR MCGPLLCLK/2
	
	//set modulo value 48000000/128 = 375000, 375000Hz/50Hz = 7500	
	TPM0->MOD = 7500;
	
	//Edge-Aligned PWM
	//CMOD - 1 and PS - 111 (128)
	TPM0_SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0_SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //CMOD = 1 => LPTPM counter increments on every LPTPM counter clock
	TPM0_SC &= ~(TPM_SC_CPWMS_MASK); //count up by default (0)

	//enable PWM on TPM0 channel 0 - PTD0
	TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM0 channel 1 - PTD1
	TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

/* Delay Function */

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

void playHedwigsTheme() {
	// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
	// there are two values per note (pitch and duration), so for each note there are four bytes
	int notes = sizeof(hedwigsThemeMelody) / sizeof(hedwigsThemeMelody[0]);

	// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
	int wholenote = (60000 * 4) / tempo;

	int divider = 0, noteDuration = 0;

	uint32_t period;

	while(1) {

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
			delay100x(2*9*noteDuration);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			delay100x(2*10*noteDuration);
		}

	}
}

void playStarWars() {
	int notes = sizeof(starWarsMelody) / sizeof(starWarsMelody[0]);

	int noteDuration = 0, period = 0;
	
	while(1) {
		
		for(int i = 0; i<notes; i+=2) {
			// calculates the duration of each note
			noteDuration = starWarsMelody[i + 1];
			period = TO_MOD(starWarsMelody[i]);
			TPM0->MOD = period;
			TPM0_C0V = period / 4; //12.5% duty cycle
			delay100x(25*noteDuration);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			delay100x(24*noteDuration);
		}
		
	}
}

//void playWin7StartUp () {
//	int notes = sizeof(win7StartUpMelody) / sizeof(win7StartUpMelody[0]);
//	
//	int noteDuration = 0, period = 0;
//	
//	while(1) {
//		
//		for(int i = 0; i<notes; i+=2) {
//			// calculates the duration of each note
//			noteDuration = win7StartUpMelody[i + 1];
//			period = TO_MOD(win7StartUpMelody[i]);
//			TPM0->MOD = period;
//			TPM0_C0V = period / 4; //12.5% duty cycle
//			delay100x(25*noteDuration);
//			TPM0->MOD = 0;
//			TPM0_C0V = 0;
//			delay100x(24*noteDuration);
//		}
//		delay100x(0x1000);
//	}
//}

void playMarioGameOver () {
	int notes = sizeof(marioGameOverMelody) / sizeof(marioGameOverMelody[0]);

	int wholenote = (60000 * 4) / tempo;

	int divider = 0, noteDuration = 0;

	uint32_t period;

	while(1) {

		for(int i = 0; i<notes; i+=2) {
			divider = marioGameOverMelody[i + 1];
			if (divider > 0) {
				noteDuration = (wholenote) / divider;
			} else if (divider < 0) {
				noteDuration = (wholenote) / (int)fabs((float)divider);
				noteDuration *= 1.5;
			}
			period = TO_MOD(marioGameOverMelody[i]);
			TPM0->MOD = period;
			TPM0_C0V = period / 8; //12.5% duty cycle
			delay100x(2*9*noteDuration);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			delay100x(2*10*noteDuration);
		}

	}
}

void playStarTrekStartUp () {
	int notes = sizeof(starTrekStartUpMelody) / sizeof(starTrekStartUpMelody[0]);

	int wholenote = (60000 * 4) / tempo;

	int divider = 0, noteDuration = 0;

	uint32_t period;

	while(1) {

		for(int i = 0; i<notes; i+=2) {
			divider = starTrekStartUpMelody[i + 1];
			if (divider > 0) {
				noteDuration = (wholenote) / divider;
			} else if (divider < 0) {
				noteDuration = (wholenote) / (int)fabs((float)divider);
				noteDuration *= 1.5;
			}
			period = TO_MOD(starTrekStartUpMelody[i]);
			TPM0->MOD = period;
			TPM0_C0V = period / 8; //12.5% duty cycle
			delay100x(2*9*noteDuration);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			delay100x(2*10*noteDuration);
		}

	}
}

int main(void) {

	SystemCoreClockUpdate();
	initPWM();
	//playHedwigsTheme();
	//playStarWars();
	//playWin7StartUp();
	//playMarioGameOver();
	playStarTrekStartUp();
	
}
