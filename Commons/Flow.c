#include "Flow.h"

// Per-channel data
struct _Flow _Flow[2];
// 16-bit timer overflow counter
uint8_t _Flow_Carry;

void Flow_Init(void)
{
	// Configure timer
	T3CON =
		0x80 | // Enables register read/write of Timer3 in one 16-bit operation
		0x40 | // Timer3 and Timer4 are the clock sources for ECCP3, CCP4 and CCP5;
		0x01;  // Start Timer3
	
	// Clear the Timer registers
    TMR3H = 0;
    TMR3L = 0;
	
	// Input-capture on the rising edge of RG0 and RG3
	TRISGbits.TRISG0 = 1;
	TRISGbits.TRISG3 = 1;
	CCP3CON = 0x05; // Capture mode: every rising edge
	CCP4CON = 0x05; // Capture mode: every rising edge

	// Enable timer overflow interrupts
	IPR2bits.TMR3IP = 1;	// TMR3 Overflow Interrupt Priority bit High
	PIE2bits.TMR3IE = 1;	// TMR3 Overflow Interrupt Enable bit Enabled

	// Enable input capture interrupts
	IPR3bits.CCP3IP = 1; // ECCP3 Interrupt Priority bit High Priority
	IPR3bits.CCP4IP = 1; // CCP4 Interrupt Priority bit High Priority
	PIE3bits.CCP3IE = 1; // ECCP3 Interrupt Enable bit Enabled
	PIE3bits.CCP4IE = 1; // CCP4 Interrupt Enable bit Enabled
}

// Disable interrupts while fetching the value to avoid race-conditions.
// Note that the interrupt request flag will still get set regardless of the
// enable bit so wont risk missing any interrupts
uint24_t _Flow_Get(const uint24_t *ptr)
{
	uint24_t value;
	PIE3 &= ~0x03;
	value = *ptr;
	PIE3 |= 0x03;
	return value;
}