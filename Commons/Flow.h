#ifndef FLOW_H
#define FLOW_H

#include <p18cxxx.h>
#include "inttypes.h"
/*
#include "Microchip/Microchip.h"
#include "Utils.h"
#include <delays.h>
#include "Numeric.h"
#include "spi.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
*/
// The sampling frequency is the full 12 MHz
#define FLOW_FREQUENCY GetInstructionClock()
// Include one sixteenth of the new period when averaging
#define FLOW_WEIGHT    4
// Reset the flow to zero after ~200 ms without a pulse (max 1398 ms)
#define FLOW_TIMEOUT   200 //Orginal 200

// Interface functions
void Flow_Init(void);
#define Flow_Rate(chan) _Flow_Get(&_Flow[(chan)].average)
#define Flow_Pulses(chan) _Flow_Get(&_Flow[(chan)].counter)
uint24_t _Flow_Get(const uint24_t *ptr);

// Exposed flow measurement channels
extern struct _Flow
{
	// Timeout counter
	uint8_t active;
	// Last tick number
	uint24_t tick;
	// Track the current average flow period
	volatile uint24_t average;
	// Count the number of pulses to measure the total flow
	volatile uint24_t counter;
}
_Flow[];

// Calculate the number of timer wraparounds before the channel is considered inactive
#define _FLOW_TIMEOUT_TICKS \
CONST_RANGE \
( \
	3, \
	(unsigned long) FLOW_TIMEOUT * FLOW_FREQUENCY / (65536UL * 1000), \
	255 \
)

// Inline interrupt handlers
#define Flow_Interrupt() \
{ \
	extern uint8_t _Flow_Carry; \
	uint24_t tick; \
	\
	/* Handle input-capture interrupts on both channels */ \
	_Flow_InterruptEvent(_Flow[0], CCP3IF, CCPR3) /* DIO 0 */ \
	_Flow_InterruptEvent(_Flow[1], CCP4IF, CCPR4) /* DIO 3 */ \
	\
	/* Is it an overflow interrupt? */ \
	if(PIR2bits.TMR3IF) \
	{ \
		PIR2bits.TMR3IF = 0; \
		\
		++_Flow_Carry; \
		\
		/* Reset the average period when no pulse has been received for a sufficiently long time */ \
		if(_Flow[0].active) \
			--_Flow[0].active; \
		else \
			_Flow[0].average = 0; \
		\
		if(_Flow[1].active) \
			--_Flow[1].active; \
		else \
			_Flow[1].average = 0; \
	} \
}

#define _Flow_InterruptEvent(chan, flag, capt) \
/* Is it a capture interrupt? */ \
if(PIR3bits.flag) \
{ \
	/* Read captured value and account for simultaneous overflows */ \
	tick = (uint24_t) _Flow_Carry << 16 | capt; \
	\
	/* Correct for timer overflow */ \
	if(!(tick & 0x8000) && PIR2bits.TMR3IF) \
		tick += 0x10000; \
	\
	/* Measure the time elapsed between pulses and calculate the average period */ \
	if(chan.active) \
		/* Note: MCC18 doesn't do arithmetic right shifts, hence the 32-bit extension */ \
		chan.average += (uint32_t) (tick - chan.tick) - chan.average >> FLOW_WEIGHT; \
	\
	chan.tick = tick; \
	chan.active = _FLOW_TIMEOUT_TICKS; \
	\
	/* Increment pulse counter */ \
	++chan.counter; \
	\
	/* Acknowledge the interrupt */ \
	PIR3bits.flag = 0; \
}

#endif