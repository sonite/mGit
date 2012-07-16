#include "Editable.h"

struct _Edit _Edit;

// Toolbar button names
#define Edit_ToolUnused (&Edit_ToolOk[2])
static const char rom Edit_ToolOk[] = "OK";

#if 1
static const char rom Edit_ToolPlus[] = "\244  ";
static const char rom Edit_ToolMinus[] = " \243" ;
static const char rom Edit_ToolLeft[] = "\242 ";
static const char rom Edit_ToolRight[] = "  \241";
#else
static const char rom Edit_ToolPlus[] = "Plus";
static const char rom Edit_ToolMinus[] = "Minus";
static const char rom Edit_ToolLeft[] = "Left";
static const char rom Edit_ToolRight[] = "Right";
#endif

/******************************************************************************
 * Built-in boolean option strings
 ******************************************************************************/
Edit_Option_t Edit_YesOrNo[] =
{ "No", "Yes", NULL };
Edit_Option_t Edit_TrueOrFalse[] =
{ "False", "True", NULL };

/******************************************************************************
 * General update and initialization functions
 ******************************************************************************/
// Clear the edit line
static void Edit_PrepareLine(void)
{
	_Edit.Cursor = -1;
	memset(_Edit.Line, ' ', EDIT_WIDTH);
}

static void Edit_PrepareUpdate(void)
{
	signed char index;

	// Check whether any of the buttons have been pressed
	index = Read_Buttons();
	_Edit.ButtonDown = index & ~_Edit.ButtonLatch;
	_Edit.ButtonLatch = index;

	if(_Edit.ButtonDown)
		Buzzer_Click(BUZZER_BUTTON);

	// Clear the buttons by default
	index = 3;
	do
		Edit_Tool(index, Edit_ToolUnused);
	while(--index >= 0);

	// Prepare for a new sweep through the editable columns
	_Edit.Cycle = 0;
}

// Reset the state of the interface
void Edit_Begin(void)
{
	_Edit.Focus = 1;
	_Edit.ButtonLatch = ~0;
	Edit_PrepareUpdate();
	Edit_PrepareLine();
}

// Display the line just having been updated and prepare for the next one
void Edit_ExtraLine(unsigned char row)
{
	signed char index;
	char *ptr;

	// Show the edited line 
	Display_Cursor(0, row);
	index = 0;
	ptr = _Edit.Line;
	do
	{
		// Draw the cursor if it was found on this line
		if(index == _Edit.Cursor)
			Display_InvertChar = 0xFF;

		Display_Char(*ptr++);
		Display_InvertChar = 0x00;
	}
	while(++index != EDIT_WIDTH);

	// Prepare for the next line
	Edit_PrepareLine();
}

// Call this continually at the start of a frame until the form has been
// submitted (that is false is returned)
unsigned char Edit_Update(unsigned char row, const char rom *tool1, const char rom *tool2)
{
	signed long tick;
	signed char index;
	const char rom **ptr;

	// Show the final line
	Edit_ExtraLine(row);

	// Respond to navigation requests and wrap the focus
	if(Edit_Tool(2, Edit_ToolLeft))
		--_Edit.Focus;
	if(Edit_Tool(3, Edit_ToolRight))
		++_Edit.Focus;

	if(_Edit.Focus < 0)
		_Edit.Focus = _Edit.Cycle;
	if(_Edit.Focus > _Edit.Cycle)
		_Edit.Focus = 0;

	// Show the supplied action buttons when no column is selected
	if(!_Edit.Focus)
	{
		if(tool1 && Edit_Tool(0, tool1))
			return +1;
		if(tool2 && Edit_Tool(1, tool2))
			return +2;
	}

	// Render toolbar and try to space out the buttons a little
	index = EDIT_WIDTH - 3;
	ptr = &_Edit.Toolbar[4];
	do
		index -= strlenpgm((const char far rom *) *--ptr);
	while(ptr != _Edit.Toolbar);

	Display_Cursor(0, 0);
	Display_InvertChar = 0x80;
	goto enter;

	do
	{
		do
			Display_Char(' ');
		while(--index >= 0);
enter:
		Display_String(*ptr++);
		Display_Char(' ');
		Display_String(*ptr++);
	}
	while(index >= 0);

	Display_Char(' ');
	Display_InvertChar = 0x00;

	// Start the next run
	Edit_PrepareUpdate();
	return 0;
}

// Set a background text for the edit line
void Edit_PutString(const char rom *line)
{
	unsigned char len = EDIT_WIDTH;
	char *ptr = _Edit.Line;

	do
	{
		char ch = *line++;
		
		if(!ch)
			break;
		
		*ptr++ = ch;
	}
	while(--len);
}

// Set a button tool and check whether it is being used
bool Edit_Tool(unsigned char index, const char rom *name)
{
	_Edit.Toolbar[index] = name;
	return _Edit.ButtonDown >> index & 1;
}

// Call this to handle focus and navigation for each editable column
static bool Edit_Focus(unsigned char column)
{
	// Is the current column in focus?
	if(++_Edit.Cycle == _Edit.Focus)
	{
		// Then remember the display column
		_Edit.Cursor = column;
		return true;
	}

	return false;
}

/******************************************************************************
 * Editable controls and utilities
 ******************************************************************************/
// Edit generic numbers. Note that all of the usual numeric formatting settings
// are supported
long Edit_Numeric(unsigned char column, unsigned char digits, long value)
{
	unsigned char radix;
	signed char sign;
	bool zero;
	char *ptr, *end;

	// Print the value to a temporary buffer using the chosen format,
	// we do need to preserve the radix though
	radix = Num.Radix;
	Num.Alignment = digits;
	Num.Flags |= NUM_ZERO_EXPANSION | NUM_UNTERMINATED;
	ptr = &_Edit.Line[column];
	end = Numeric(ptr, value);

	// Keep track of whether the number was negative
	sign = +1;
	if(value < 0)
		sign = -1;
	zero = (value == 0);
	value = 0;

	// Process the digits
	do
	{

		// Ignore anyting but digits, including signs and decimal points
		signed char digit = *ptr++;

		if(digit >= '0' && digit <= '9')
			digit -= '0';
		else if(digit >= NUM_ALPHA_CASE && digit <= NUM_ALPHA_CASE + 'Z' - 'A')
			digit -= NUM_ALPHA_CASE - 10;
		else
			continue;

		// Continually rebuild the number after modification
		value *= radix;
		value += digit * sign;

		// Let the user modify the currently selected digit
		digit = 0;

		if(Edit_Focus(column))
		{
			if(Edit_Tool(0, Edit_ToolPlus))
				++digit;
			if(Edit_Tool(1, Edit_ToolMinus))
				--digit;

			// Just flip the sign without modifying any remaining digits when passing zero
			// except if the original value was zero
			if(digit && !zero)
			{
				if(digit != sign && !value)
				{
					sign = -sign;
					digit = 0;
				}
			}
		}

		value += digit;
	}
	// Continue until we reach the end of the (unterminated) string
	while(++column, ptr != end);

	return value;
}

// Simple multiple-choice selector. The array is supposed to be null-terminated
unsigned char Edit_Choice(unsigned char column, signed char value, Edit_Option_t *options)
{
	signed char lim;
	const char rom *ptr;

	// Find the number of options and print the current selection
	lim = 0;
	while(ptr = *options++)
	{
		if(lim == value)
		{
			memcpypgm2ram
			(
				&_Edit.Line[column],
				(const void far rom *) ptr,
				strlenpgm((const void far rom *) ptr)
			);
		}
		++lim;
	}

	// Let the user cycle among the choices
	if(Edit_Focus(column))
	{
		if(Edit_Tool(0, Edit_ToolPlus))
			++value;
		if(Edit_Tool(1, Edit_ToolMinus))
			--value;
	}

	// Wrap the selection
	if(value < 0)
		value = lim - 1;
	else if(value >= lim)
		value = 0;
	return value;
}

long Edit_Clip(long min, long value, long max)
{
	if(value > max)
		return max;
	if(value < min)
		return min;
	return value;
}

uint8_t OkCancel(uint8_t row)
{
/*		uint8_t status = Edit_Update(row, Edit_ToolOk, Edit_ToolCancel);
	Edit_Update(row, Edit_ToolOk, Edit_ToolCancel);

	Line_Cls(row);
	
	 switch(status)
	 {
	 case 1:
		  Display_StringAt(AnswerOk, 5, row); 
		  Microchip_RefreshSettings();
		  break;
	 case 2:
		  Display_StringAt(AnswerCancel, 5, row);
		  break;
	 }

	return status;
	*/
}