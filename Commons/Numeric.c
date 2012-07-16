#include "Numeric.h"

char Num_Buffer[48];

// The default format
#define NUM_DEFAULTS          \
{                             \
	0x00, /* No flags set */  \
	10,   /* Base ten */      \
	0,    /* No alignment */  \
	0,    /* No decimals  */  \
}

// Format settings
struct Num_Format Num = NUM_DEFAULTS;
static rom const struct Num_Format Num_Defaults = NUM_DEFAULTS;

// Generic printing function
char *Numeric(char *buffer, long value)
{
	unsigned char digit;
	signed char prefix;
	char *rewind = buffer;
	char *end;

	unsigned char flags = Num.Flags;
	unsigned char decimals = Num.Decimals;
	unsigned char grouping;

	// Set up the grouping counter
	grouping = 0;
	if(flags & NUM_GROUPING)
		grouping = NUM_GROUP_WIDTH + 1;

	// Add a minus sign if necessary
	prefix = '\0';
	if(flags & NUM_FORCE_SIGN)
		prefix = '+';
	if(value < 0)
	{
		value = -value;
		prefix = '-';
	}

	// Figure out the right-alignment target, with an exception to reserve space
	// for the sign after zero-extension
	end = &buffer[Num.Alignment];
	if(flags & NUM_ZERO_EXPANSION && prefix)
		--end;

	// Divide numer into digits and write them in reverse order to the buffer
	do
	{
		for(;;)
		{
			// Divide by the radix and extract the remainder as the next digit
			digit = (unsigned char) value;
			value = (unsigned long) value / Num.Radix;
			digit -= (unsigned char) value * Num.Radix;

			if(digit += '0', digit > '9')
				digit += NUM_ALPHA_CASE - '0' - 10;

			// Continue forcing out digits until we've passed the decimal point
			if(!decimals)
				break;

			*buffer++ = digit;

			if(!--decimals)
				*buffer++ = NUM_DECIMAL_POINT;
		}

		// Insert grouping characters into the integer part every N digits if
		// the counter is initially non-zero
		if(!--grouping)
		{
			grouping = NUM_GROUP_WIDTH;
			*buffer++ = NUM_GROUP_SEPARATOR;
		}

		// Continue emitting digits until the value has reached zero, except make
		// sure there's always at least one zero in the output
		*buffer++ = digit;
	}
	// Also continue emitting zeroes if the number is to be zero-extended
	while(value || flags & NUM_ZERO_EXPANSION && buffer < end);

	// Optionally add the sign before the digits
	if(prefix)
		*buffer++ = prefix;

	// Right-justify the number if it's smaller than the specified field alignment
	while(buffer < end)
		*buffer++ = ' ';

	// Optionally write null-terminator
	if(!(flags & NUM_UNTERMINATED))
		*buffer = '\0';

	// Reverse the output digits
	end = buffer;

	while(rewind < buffer)
	{
		digit = *rewind;
		*rewind++ = *--buffer;
		*buffer = digit;
	}

	// Restore the default format
	memcpypgm2ram(&Num, (const void far rom *) &Num_Defaults, sizeof Num);

	// Return a pointer to the end of the buffer, e.g. for attaching a suffix
	return end;
}

// Helper functions
char *Num_Integer(char *buffer, int value)
{
	return Numeric(buffer, value);
}