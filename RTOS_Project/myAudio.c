/*----------------------------------------------------------------------------
 * AUDIO CONTROL
 *---------------------------------------------------------------------------*/
#include "myAudio.h"
#include "myInit.h"

// change this to make the song slower or faster
int tempo = 144;

volatile uint8_t end, connect;

/**
* Notes of a melody followed by duration.
* A 4 means a quarter note, 8 an eighth, 16 sixteenth and so on.
* Negative numbers are used to represent dotted notes.
* So, a -4 means a dotted quarter note i.e. a quarter + eighteenth.
*/

// Hedwig's theme from the Harry Potter Movies
// Score from https://musescore.com/user/3811306/scores/4906610
int hedwigsThemeMelody[] = {
  
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

// Mario game over sound, played at the end.
int marioGameOverMelody [] = {
  NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, //45
  NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8,
  NOTE_G4,8, NOTE_D4,8, NOTE_E4,-2,  
};

// Star trek startup melody, played on connection.
// Score from https://musescore.com/user/10768291/scores/4594271
int starTrekStartUpMelody[] = {

  NOTE_D4, -8, NOTE_G4, 16, NOTE_C5, -4, 
  NOTE_B4, 8, NOTE_G4, -16, NOTE_E4, -16, NOTE_A4, -16,
  NOTE_D5, 2,

};

/* Delay Functions */
//1 nop takes 1 cycle. So 1 delay is 1/48 microsec

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}


/*
Usage Example:

delay(0x80000);
*/

void delay100x(volatile uint32_t nof) {
	for(int i =0;i<100;i++) {
		delay(nof);
	}
}

/** Duty Cycle Function **/

uint8_t dutyCycle(int period, int div) {
	return (period/div);
}

/** PLAY MUSIC FUNCTIONS **/
	// Sizeof gives the number of bytes, each int value is composed of two bytes (16 bits).
	// There are two values per note (pitch and duration), so for each note there are four bytes.

/** HEDWIGS THEME **/

void playHedwigsTheme() {
	int notes = sizeof(hedwigsThemeMelody) / sizeof(hedwigsThemeMelody[0]);

	// This calculates the duration of a whole note in ms (60s/tempo)*4 beats.
	int wholenote = (60000 * 4) / tempo;

	int divider = 0, noteDuration = 0;

	uint32_t period;


		for(int i = 0; (i<notes && end != THE_END && connect != CONNECT) ; i+=2) {
			// Calculates the duration of each note.
			divider = hedwigsThemeMelody[i + 1];
			if (divider > 0) {
				// Regular note, just proceed.
				noteDuration = (wholenote) / divider;
			} else if (divider < 0) {
				// Dotted notes are represented with negative durations.
				noteDuration = (wholenote) / (int)fabs((float)divider);
				noteDuration *= 1.5; // Increases the duration in half for dotted notes.
			}
			period = TO_MOD(hedwigsThemeMelody[i]);
			TPM0->MOD = period;
			TPM0_C0V = dutyCycle(period, 4); //25% duty cycle
			osDelay(HEDWIGS_STARTREK_PLAY(noteDuration));
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			osDelay(HEDWIGS_STARTREK_PAUSE(noteDuration));
		}
}


/** MARIO GAME OVER **/
void playMarioGameOver () {
	int notes = sizeof(marioGameOverMelody) / sizeof(marioGameOverMelody[0]);

	int wholenote = (60000 * 4) / tempo;

	int divider = 0, noteDuration = 0;

	uint32_t period;

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
			TPM0_C0V = dutyCycle(period, 2); //50% duty cycle
			delay100x(MARIO_PLAY(noteDuration));
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			delay100x(MARIO_PAUSE(noteDuration));
		}
		end = CHANGE_VAR;
}

/** STAR TREK THEME **/
void playStarTrekStartUp () {
	int notes = sizeof(starTrekStartUpMelody) / sizeof(starTrekStartUpMelody[0]);

	int wholenote = (60000 * 4) / tempo;

	int divider = 0, noteDuration = 0;

	uint32_t period;

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
			TPM0_C0V = dutyCycle(period, 8); //12.5% duty cycle
			osDelay(HEDWIGS_STARTREK_PLAY(noteDuration));
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			osDelay(HEDWIGS_STARTREK_PAUSE(noteDuration));
		}
		connect = CHANGE_VAR;
}
