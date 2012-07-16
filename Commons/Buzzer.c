#include "Buzzer.h"

//** I N T E R N A L  D E C L A R A T I O N S *********************************/
#define	BUZZER_DIR  TRISJbits.TRISJ6
#define BUZZER_PORT LATJbits.LATJ6			// LCD Register Select Input
#define F_CPU       GetInstructionClock() 				//12MHz //

// Buzzer() plays a tone with the frequency in Hz and the duration
// in milliseconds.
/*
void Buzzer_Play(uint16_t frequency, uint16_t duration)
{
	uint16_t wait_units, loops, i;

	// Set the buzzer as an output (Putting this here saves having
	// to call an initialization routine!)
	BUZZER_DIR = 0;
	//BUZZER_DIR |= (1 << BUZZER_BIT); // Kanske funkar
	//buzzer_ddr = buzzer_ddr | (1 << buzzer_bit)
	
	
	// Calculate how long we need to wait between turning the buzzer
	// on and off.  This should be half of the tone frequency,
	// expressed in delay loop units.
	wait_units = F_CPU / (8UL * (uint32_t) frequency);

	// Calculate the number of loops to execute in order to hit the
	// duration asked for.
	loops = duration * F_CPU / (8000UL * wait_units);

	// The loop
	for (i=1; i<=loops; i++)
	{
		// Turn the buzzer on
		//BUZZER_PORT |= (1 << BUZZER_BIT);
		BUZZER_PORT = 1;
		// Wait 1/2 tone period
		//_delay_loop_2(wait_units);
		Delay_ms(wait_units);
		// Turn the buzzer off
		//BUZZER_PORT &= ~(1 << BUZZER_BIT);
		BUZZER_PORT = 0;
		// Wait 1/2 tone period
		//_delay_loop_2(wait_units);
		Delay_ms(wait_units);
		}
}
*/

// Play a single click
void Buzzer_Click(uint8_t period)
{
	BUZZER_DIR = 0;
	BUZZER_PORT = 1;
	Delay_us(period);
	BUZZER_PORT = 0;
}