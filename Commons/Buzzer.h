#ifndef _BUZZER_H
#define _BUZZER_H

#include <p18cxxx.h>
#include <delays.h>
#include "Utils.h"

enum
{
	BUZZER_BUTTON = 25, // Click Sound
	BUZZER_THIN   = 10  // Other kind of Sound (thinner?)
};

// buzzer
//
// Courtesy of Jim Remington
// Comments by Tom Benedict

// The Orangutan and Orangutan-X2 both have piezo buzzers built into
// them.  Jim Remington has written some cool code to play music out
// the speaker using TIMER2 to provide both frequency control and
// note duration.  This code is posted on the Pololu Forum.
//
// Jim also wrote a very lightweight routine that will simply play a
// given frequency (in Hz) for a given duration (in ms).  The trade-
// off is that unlike his TIMER2 based routines, when the note is
// being played, the CPU is tied up.  For some applications this is
// more than acceptable, and it means TIMER2 is still available for
// other purposes.
//
// The neatest part of Jim's code is the math that determines how
// many clock cycles to wait between turning the buzzer's I/O line on
// and off (note frequency) and how many clock cycles to play the
// note (duration).  The resulting equations are very small.
//
// So let's get to it!

//** P R O T O T Y P E S ******************************************************/
void Buzzer_Play(uint16_t frequency, uint16_t duration);
void Buzzer_Click(uint8_t duration);

//*****************************************************************************/
#endif
