#ifndef NUMERIC_H
#define NUMERIC_H

#include <string.h>

// Generic number-printer
char *Numeric(char *buffer, long value);

// Compile-time localization settings
#define NUM_DECIMAL_POINT   ','
#define NUM_GROUP_SEPARATOR '\''
#define NUM_GROUP_WIDTH     3
#define NUM_ALPHA_CASE      'A'

// Format settings. Used as extended parameters to the printer
// and reset back to default values after each write
enum
{
	NUM_GROUPING       = 0x01, // Show thousand separator
	NUM_ZERO_EXPANSION = 0x02, // Right adjust with zero expansion
	NUM_FORCE_SIGN     = 0x04, // Show leading '+' for positive numbers
	NUM_UNTERMINATED   = 0x08  // Do not write string terminator
};

extern struct Num_Format
{
	// Various behavioural flags
	unsigned char Flags;
	// Numeric base (2-16)
	unsigned char Radix;
	// Try to right-align to this field width
	unsigned char Alignment;
	// Number of fractional decimals
	unsigned char Decimals;
}
Num;

// A shared buffer large enough to fit any reasonable number
extern char Num_Buffer[];

// Utility functions for printing values in certain common formats
// Feel free to add any common formats you find yourself using
char *Num_Integer(char *buffer, int value);

#endif