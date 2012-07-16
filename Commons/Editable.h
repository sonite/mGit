/******************************************************************************
 * Line-editing helper functions
 ******************************************************************************/
#ifndef _EDITABLE_H
#define _EDITABLE_H

#include <stdlib.h>
#include "Microchip/Microchip.h"
#include "glcd_functions.h"
#include "Numeric.h"
#include "Buzzer.h"

#define EDIT_WIDTH 21

extern struct _Edit
{
	signed char Cycle;
	signed char Focus;
	signed char Cursor;
	unsigned char ButtonDown;
	unsigned char ButtonLatch;
	char Line[EDIT_WIDTH + 1];
	const char rom *Toolbar[4];
}
_Edit;

typedef const char rom *const rom Edit_Option_t;

void Edit_Begin(void);
void Edit_ExtraLine(unsigned char row);
unsigned char Edit_Update(unsigned char row, const char rom *tool1, const char rom *tool2);
void Edit_PutString(const char rom *line);
bool Edit_Tool(unsigned char index, const char rom *name);
// Edit_Numeric(column, digits, value)
long Edit_Numeric(unsigned char column, unsigned char digits, long value);
unsigned char Edit_Choice(unsigned char column, signed char value, Edit_Option_t *options);
long Edit_Clip(long min, long value, long max);

#define Edit_PutChar(column, char) \
((void) (_Edit.Line[(column)] = (char)))

extern Edit_Option_t Edit_YesOrNo[];
extern Edit_Option_t Edit_TrueOrFalse[];
extern uint8_t OkCancel(uint8_t row);

#endif