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
//#include "io.h"
#include "RIMS.h"

////////////////////////////////////////////////////////
////////////////TIMER FUNCTIONS
///////////////////////////////////////////////////////////
/*
volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C Programmer should clear to 0.

unsigned long _avr_timer_M = 1;	       // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
unsigned short count = 0x00;

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;     // bit3 = 0: CTC mode (clear timer on compare)
		          // bit2bit1bit0 = 011: pre-scaler /64
			 // 00001011: 0x0B
			// SO, 8 MHz clock or 8,000,000 / 64 = 125,000 ticks/s
		       // Thus, TCNT1 register will count at 125,000 tick/s

	// AVR output compare register OCR1A.
	OCR1A = 125;    // Timer interrupt will be generated when TCNT1==OCR1A
		       // We want a 1 ms tick. 0.001s * 125,000 ticks/s = 125
		      // So when TCNT1 register equals 125,
		     // 1 ms has passed. Thus, we compare to 125.

	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	// Initialize avr counter
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	// Enable globla interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; /// bit3bit1bit0 = 000: timer off
}


void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP

	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); 	      // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}

}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

*/







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

enum States {Wait, ON, OFF, init} state;

void Tick() {
		unsigned char A2 = (PINA & 0x04);
		unsigned char A1 = (PINA & 0x02);
		unsigned char A0 = (PINA & 0x01); 
		unsigned double frequency = 0x00;
		
		if (A0) {
			button_check++;
		}
		if (A1) {
			button_check++;
		}
		if (A2) {
			button_check++;
		}
		
			
		
		
	switch(state) {
		case init:
			frequency = 0;
			state = Wait;
			break;
		case Wait: //Mealy Transitions
			if (button_check > 3) {
				state = Wait;
				button_check = 0;
			}
			else if (A0) {
				frequency = 261.63; 
				state = ON;
			}
			else if (A1) {
				frequency = 293.66;
				state = ON;
			}
			else if (A2) {
				frequency = 329.63;
				state = ON;
			}

			break;
		case ON:
			PWM_on();
			set_PWM(frequency);
			
			if ((A0 == 0x00) && (A1 == 0x00) && (A2 == 0x00)) {
				state = OFF;
			}
			
			
			
			break;
			
		case OFF:
			PWM_off();
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
	DDRC = 0xFF; PORTC = 0x00;  // output
	DDRD = 0xFF; PORTD = 0x00; // output
	//TimerSet(1000); // Timer Set to: 300 ms
	//TimerOn();
	State = Wait;

    while (1) {
	Tick();
	//while (!TimerFlag); // Wait 1 sec
	//TimerFlag = 0;
    }
    return 1;
}
