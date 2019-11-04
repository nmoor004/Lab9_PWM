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
////////////////////////////////////////////////////
////////////////TIMER FUNCTIONS
//////////////////////////////////////////////////

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
unsigned char Toggle = 0;
double frequency = 0x00;
double note_Array[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25}; 
unsigned char n = 0;

enum States {Wait, ToggleON, init, Up, Down} state;

void Tick() {
		unsigned char A2 = ((PINA & 0x04) >> 2);
		unsigned char A1 = ((PINA & 0x02) >> 1);
		unsigned char A0 = (PINA & 0x01); 
		
		if (!A0) {
			button_check++;
		}
		if (!A1) {
			button_check++;
		}
		if (!A2) {
			button_check++;
		}
			
	
		
			
		
		
	switch(state) {
		case init:
			PORTC = 0x1F;
			state = Wait;
			break;
		case Wait: //Mealy Transitions
			PORTC = 0x1F;
			if (button_check < 2) {
			if ((!A0) && (A1) && (A2)) {
				state = ToggleON;
			}
			else if ((A0) && (!A1) && (A2)) {
				state = Down;
			}
			else if ((A0) && (A1) && (!A2)) {
				state = Up;
			} 
			else {
				state = Wait;
			} 
			}

			break;
		case ToggleON:
			PORTC = 0x02;

			if (button_check > 1) {
			set_PWM(0);
			}
			
			if (Toggle == 0) {
				Toggle = 0x01;
				set_PWM(frequency);
			}
			else if (Toggle == 1) {
				Toggle = 0x00;
				set_PWM(0);
			}
			state = Wait;
			
			break;
			
		case Down:
			PORTC = 0x04;
			if (n == 0) {
				n = 0;
			}
			else {
				n--;
				frequency = note_Array[n];
				set_PWM(frequency);
			}
			state = Wait;
			break;
		case Up:
			PORTC = 0x06;
			if (n == 7) {
				n = 7;
			}
			else {
				n++;
				frequency = note_Array[n];
				set_PWM(frequency);

			}	
			state = Wait;
			
	}
	
	
	
	switch(state) { //Actions, none since mealy
		case init:
			break;
		case Wait:
			break;
		case ToggleON:
			break;
		case Up:
			break;
		case Down:
			break;
	}
	button_check = 0;
}



int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;    // input
	DDRB = 0xFF; PORTB = 0x00;   // output 
	DDRC = 0xFF; PORTC = 0x00;  // output for state checking
	DDRD = 0xFF; PORTD = 0x00; // output 
	TimerSet(1000); // Timer Set to: 300 ms
	TimerOn();
	state = init;
	PWM_on();

    while (1) {
	Tick();
	while (!TimerFlag); // Wait 1 sec
	TimerFlag = 0;
    }
    return 1;
}
