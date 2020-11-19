/*----------------------------------------------------------------------------
 * LED CONTROL
 *---------------------------------------------------------------------------*/
 #include "myLED.h"
 
 uint8_t leds[8] = {LED_F5, LED_F4, LED_F3, LED_F2, LED_F11, LED_F10, LED_F9, LED_F8};
 
 void offLED(void) {
		PTA->PCOR |= MASK(LED_R1);
}

void greenFlashTwice() {
	int i;
	for(i = 0; i<2; i++) {
		PTE -> PSOR |= MASK(LED_F2) | MASK(LED_F3) | MASK(LED_F4) | MASK(LED_F5);
		PTB -> PSOR |= MASK(LED_F8) | MASK(LED_F9) | MASK(LED_F10) | MASK(LED_F11);
		osDelay(GREEN_FLASH_TIME);
		PTE -> PCOR |= MASK(LED_F2) | MASK(LED_F3) | MASK(LED_F4) | MASK(LED_F5);
		PTB -> PCOR |= MASK(LED_F8) | MASK(LED_F9) | MASK(LED_F10) | MASK(LED_F11);
		osDelay(GREEN_FLASH_TIME);
	}
}

void redStoppedState() {
	PTA -> PSOR |= MASK(LED_R1);
	osDelay(RED_STOP_TIME);
	PTA -> PCOR |= MASK(LED_R1);
	osDelay(RED_STOP_TIME);
}

void redMovingState() {
	PTA -> PSOR |= MASK(LED_R1);
	osDelay(RED_MOVING_TIME);
	PTA -> PCOR |= MASK(LED_R1);
	osDelay(RED_MOVING_TIME);
}

void greenStoppedState() {
	PTE -> PSOR |= MASK(LED_F2) | MASK(LED_F3) | MASK(LED_F4) | MASK(LED_F5);
	PTB -> PSOR |= MASK(LED_F8) | MASK(LED_F9) | MASK(LED_F10) | MASK(LED_F11);
}

void greenMovingState(int c) {
	if (c < 4) {
				PTE -> PSOR |= MASK(leds[c]);
				osDelay(GREEN_MOVING_TIME);
				PTE -> PCOR |= MASK(leds[c]);
			} else if (c >= 4) {
				PTB -> PSOR |= MASK(leds[c]);
				osDelay(GREEN_MOVING_TIME);
				PTB -> PCOR |= MASK(leds[c]);
			} 
}
