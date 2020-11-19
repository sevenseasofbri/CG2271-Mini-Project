/*----------------------------------------------------------------------------
 * MOTOR CONTROL
 *---------------------------------------------------------------------------*/
#include "myMotor.h"

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
	if(s != REVERSE) {
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
	 TPM1_C1V = MAX_DUTY_CYCLE/3;
	 TPM2->MOD = 7500;
	 TPM2_C0V = MAX_DUTY_CYCLE/3;
}

/** Rotate Left **/
void left(state_t s) {
	 if (s != LEFT) { 
		 TPM1_C1V = 0;
		 TPM2_C0V = 0;
	 }
	 TPM2->MOD = 7500;
	 TPM2_C1V = MAX_DUTY_CYCLE/3;
	 TPM1->MOD = 7500;
	 TPM1_C0V = MAX_DUTY_CYCLE/3;
}

/** Curved Forward Left**/
void leftforward(state_t s) {
	if(s != FWLEFT) {
		TPM1_C0V = 0;
		TPM2_C0V = 0;
	}
	 TPM2->MOD = 7500; //right
	 TPM2_C1V = MAX_DUTY_CYCLE;
	 TPM1->MOD = 7500; //left
	 TPM1_C1V = MAX_DUTY_CYCLE/6;
	
}

/** Curved Reverse Left**/
void leftreverse(state_t s) {
	if(s != RVLEFT) {
		TPM1_C1V = 0;
		TPM2_C1V = 0;
	}
	 TPM2->MOD = 7500; //right
	 TPM2_C0V = MAX_DUTY_CYCLE;
	 TPM1->MOD = 7500; //left
	 TPM1_C0V = MAX_DUTY_CYCLE/6;
	
}

/** Curved Forward Right**/
void rightforward(state_t s) {
	if(s != FWRIGHT) {
		TPM1_C0V = 0;
		TPM2_C0V = 0;
	}
	 TPM2->MOD = 7500; //right
	 TPM2_C1V = MAX_DUTY_CYCLE/6;
	 TPM1->MOD = 7500; //left
	 TPM1_C1V = MAX_DUTY_CYCLE;
	
}

/** Curved Reverse Right**/
void rightreverse(state_t s) {
	if(s != RVRIGHT) {
		TPM1_C1V = 0;
		TPM2_C1V = 0;
	}
	 TPM2->MOD = 7500; //right
	 TPM2_C0V = MAX_DUTY_CYCLE/6;
	 TPM1->MOD = 7500; //left
	 TPM1_C0V = MAX_DUTY_CYCLE;
	
}
