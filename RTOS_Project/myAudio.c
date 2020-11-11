/*----------------------------------------------------------------------------
 * AUDIO CONTROL
 *---------------------------------------------------------------------------*/
 
#include "MKL25Z4.h"                    // Device header
#include "myAudio.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

// change this to make the song slower or faster
int tempo = 144;

volatile uint8_t connect, end;

// notes of the melody followed by the duration.
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

int win7StartUpMelody [] = {
	NOTE_B5, 250, NOTE_E1, 500, NOTE_FS2, 250, NOTE_B5, 1000
};

int marioGameOverMelody [] = {
	  //game over sound
  NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, //45
  NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8,
  NOTE_G4,8, NOTE_D4,8, NOTE_E4,-2,  
};

int starTrekStartUpMelody[] = {

  //available at https://musescore.com/user/10768291/scores/4594271

  NOTE_D4, -8, NOTE_G4, 16, NOTE_C5, -4, 
  NOTE_B4, 8, NOTE_G4, -16, NOTE_E4, -16, NOTE_A4, -16,
  NOTE_D5, 2,

};

/** HEDWIGS THEME **/
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
			TPM0_C0V = period / 4; //25% duty cycle
			osDelay(noteDuration*5/9);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			osDelay(noteDuration*25/81);
		}
}

/** STAR WARS THEME **/
void playStarWars() {
	int notes = sizeof(starWarsMelody) / sizeof(starWarsMelody[0]);

	int noteDuration = 0, period = 0;
		
		for(int i = 0; (i<notes && connect == 0xFE && end == 0xFE); i+=2) {
			// calculates the duration of each note
			noteDuration = starWarsMelody[i + 1];
			period = TO_MOD(starWarsMelody[i]);
			TPM0->MOD = period;
			TPM0_C0V = period / 4; //25% duty cycle
			osDelay(noteDuration*2/3);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			osDelay(noteDuration*16/25);
		}
}

/** WINDOWS 7 STARTUP **/
void playWin7StartUp () {
	int notes = sizeof(win7StartUpMelody) / sizeof(win7StartUpMelody[0]);
	
	int noteDuration = 0, period = 0;
		
		for(int i = 0; i<notes; i+=2) {
			// calculates the duration of each note
			noteDuration = win7StartUpMelody[i + 1];
			period = TO_MOD(win7StartUpMelody[i]);
			TPM0->MOD = period;
			TPM0_C0V = period / 2; //50% duty cycle
			//delay100x(noteDuration*25);
			osDelay(noteDuration);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			//delay100x(24*noteDuration);
			osDelay(noteDuration*24/25);
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
			TPM0_C0V = period / 4; //25% duty cycle
			osDelay(noteDuration*5/9);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			osDelay(noteDuration*25/81);
		}
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
			TPM0_C0V = period / 8; //12.5% duty cycle
			osDelay(noteDuration*5/9);
			TPM0->MOD = 0;
			TPM0_C0V = 0;
			osDelay(noteDuration*25/81);
		}
}
