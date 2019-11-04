/*	Author: nmoor004
 *  Partner(s) Name: 
 *	Lab Section: 
 *	Assignment: Lab # 9 Exercise # 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>



////////////////////////////////////////////////
/////////////PWM FUNCTIONS
///////////////////////////////////////////////



void set_PWM(double frequency) {
	static double current_frequency;
	
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }
		
	if (frequency < 0.954 ) { OCR3A = 0xFFFF; }
	
	else if (frequency > 31250) { OCR3A = 0x0000; }
	
	else if (frequency > 31250) { OCR3A = 0x0000; }
	
	else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
	
	TCNT3 = 0;
	current_frequency = frequency;
	}
	
}


void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	
	set_PWM(0);
	
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

///////////////////////////////
/////////////////MAIN PROGRAM
////////////////////////////////////////

unsigned char button_check = 0x00;
enum States {Wait, ON, OFF, init} state;
		double frequency = 0x00;
void Tick() {
		unsigned char A2 = ((PINA & 0x04) >> 2);
		unsigned char A1 = ((PINA & 0x02) >> 1);
		unsigned char A0 = (PINA & 0x01); 

	
		
			
		
		
	switch(state) {
		case init:
			PORTC = 0x1F;
			state = Wait;
			break;
		case Wait: //Mealy Transitions
			PORTC = 0x1F;
			if ((!A0) && (A1) && (A2)) {
				frequency = 261.63; 
				set_PWM(frequency);
				state = ON;
			}
			else if ((A0) && (!A1) && (A2)) {
				frequency = 293.66;
				set_PWM(frequency);
				state = ON;
			}
			else if ((A0) && (A1) && (!A2)) {
				frequency = 329.63;
				set_PWM(frequency);
				state = ON;
			} 
			else {
				state = Wait;
			} 

			break;
		case ON:
			PORTC = 0x02;

			
			
			if ((A0) && (A1) && (A2)) {
				state = OFF;
			}
			else {
				state = ON;
			}
			
			
			break;
			
		case OFF:
			PORTC = 0x04;
			set_PWM(0);
			state = Wait;

		
			break;
			
	}
	
	
	
	switch(state) { //Actions, none since mealy
		case init:
			break;
		case Wait:
			break;
		case ON:
			break;
		case OFF:
			break;
		
	}
	
}



int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;    // input
	DDRB = 0xFF; PORTB = 0x00;   // output 
	DDRC = 0xFF; PORTC = 0x00;  // output for state checking
	DDRD = 0xFF; PORTD = 0x00; // output 
	//TimerSet(1000); // Timer Set to: 300 ms
	//TimerOn();
	state = init;
	PWM_on();

    while (1) {
	Tick();
	//while (!TimerFlag); // Wait 1 sec
	//TimerFlag = 0;
    }
    return 1;
}
