/******************************************************************************
 *                                                                      	  *
 *        	    			Global Utility Functions			          	  *
 *                                                                      	  *
 *              		    Author: Finsun Inresol AB                         *
 ******************************************************************************
 * FileName:        Utils.c													  *
 * Dependencies:	Utils.h, delay_melacs.h, inttypes.h 					  *
 * Processor:       PIC18													  *
 * Complier:        MCC18 v3.13												  *
 * Description: 	Global Utility Functions as Delays and String Conversions *
 ******************************************************************************/

//** I N C L U D E S **********************************************************/ 
#include "Utils.h"

//** F U N C T I O N S ********************************************************/
/*
void Delay_1sec(void)
{
	uint8_t i = 5;
	
	do
		Delay10KTCYx(240);
	while(--i);
}*/
/*
void Delay_uS(uint8_t uSec) 
{ 
   if ( uSec <= OVERHEAD ) return; // prevent underflow 
   uSec  -= OVERHEAD ;             // overhead of function call in us. 
 
   Nop();       // 1  extra overhead to make function overhead an even us. 
   Nop();       // 1  add or remove Nop's as necessary. 
   Nop();       // 1 
   //Nop();       // 1 
   //Nop();       // 1 
   //Nop();       // 1 
   //Nop();       // 1 
   //Nop();       // 1 
   //Nop();       // 1 
   //Nop();       // 1 
   //Nop();       // 1 
   //Nop();       // 1 
 
   do   // loop needs to be 12 cycles so each cycle is 1us. 
   { 
      Nop();       // 1 
      Nop();       // 1 
      Nop();       // 1 
      Nop();       // 1 
      Nop();       // 1 
      Nop();       // 1 
      Nop();       // 1 
      Nop();       // 1 
      ClrWdt();    // 1 
   } while(--uSec);  // 3 
} */
/*
void Brakepoint(void)
{
	Delay1TCY();
}

void LargeByteToStr(uint16_t value, char string[5])
{
	char digit;

	*string++ = ' ';
	*string++ = ' ';
	*string++ = ' ';
	*string++ = ' ';
	*string = '\0';
	
	do
	{
		digit = value;
		value /= 10;
		digit -= (char) value * 10;
		*--string = digit + '0';
	}
	while(value);
}

void IntToStr(uint8_t data, char *str) // ByteToStr
{
	char digit;

	*str++ = ' ';
	*str++ = ' ';
	*str++ = ' ';
	*str = '\0';
	
	do
	{
		digit = data;
		data /= 10;
		digit -= (char) data * 10;
		*--str = digit + '0';
	}
	while(data);
}

void ShortIntToStr(uint16_t data, char *str) // WordToStr
{
	char digit;

	*str++ = ' ';
	*str++ = ' ';
	*str++ = ' ';
	*str++ = ' ';
	*str++ = ' ';
	*str = '\0';
	
	do
	{
		digit = data;
		data /= 10;
		digit -= (char) data * 10;
		*--str = digit + '0';
	}
	while(data);
}

void LongIntToStr(uint24_t data, char *str) // ?ToStr
{
	char digit;

	*str++ = ' ';
	*str++ = ' ';
	*str++ = ' ';
	*str++ = ' ';
	*str++ = ' ';
	*str++ = ' ';
	*str++ = ' ';
	*str = '\0';
	
	do
	{
		digit = data;
		data /= 10;
		digit -= (char) data * 10;
		*--str = digit + '0';
	}
	while(data);
}

void NumberToString(uint24_t data, char *str)
{
	char digit;
	
	if(data >= 10)
		*str++ = ' ';
	
	if(data >= 100)
		*str++ = ' ';

	if(data >= 1000)
		*str++ = ' ';

	if(data >= 10000)
		*str++ = ' ';

	if(data >= 100000)
		*str++ = ' ';
	
	if(data >= 1000000)
	*str++ = ' ';
	
	if(data >= 10000000)
	*str++ = ' ';
	
	*str++ = ' ';
	*str = '\0';
	
	while(data)
	{
		digit = data;
		data /= 10;
		digit -= (char) data * 10;
		*--str = digit + '0';
	}
}
void ByteToStr(uint8_t value, char string[4])
{
	char digit;

	*string++ = ' ';
	*string++ = ' ';
	*string++ = ' ';
	*string = '\0';
	
	do
	{
		digit = value;
		value /= 10;
		digit -= (char) value * 10;
		*--string = digit + '0';
	}
	while(value);
}

void RealByteToStr(uint16_t data, char *str)
{
	uint8_t zero = 1;
	uint8_t i = 0;
		str[1] = ' ';  str[2] = ' ';  // initialize last two bytes to blank.
	while (data >= 100)
	{
		data -= 100;
		++i;
	}
	if (i >= 0 )
	{
		*str = '0' + i;
		zero = 0;
		++ str;
	}
	i = 0;
	
	while (data >= 10)
	{
		data -= 10;
		++i;
	}
	if (i == 0 && zero)
	{
	}
	else
	{
		*str = '0' + i;
        ++str;
	}

	*str = '0' + data;
}
*//*
void WordToStr(uint16_t value, char string[5])
{
	char digit;

	*string++ = ' ';
	*string++ = ' ';
	*string++ = ' ';
	*string++ = ' ';
	*string = '\0';
	
	do
	{
		digit = value;
		value /= 10;
		digit -= (char) value * 10;
		*--string = digit + '0';
	}
	while(value);
}
*/
void ByteToStrWithZeros(uint16_t input, char *output)
{
	uint8_t i = 3;

	output += i;
	*output = '\0';
	
	do
	{
		*--output = input % 10 + '0';
		input /= 10;
	}
	while(--i);
}
/*
 void WordToStrWithZeros(uint16_t input, char *output)
{
	uint8_t i = 5;

	output += i;
	*output = '\0';

	do
	{
		*--output = input % 10 + '0'; // % reminder of a/b // Reference to object at address output after decrement
		input /= 10; //input = input / 10;
	}
	while(--i);
}

void HexToStrWithZeros(uint16_t input, char *output)
{
	static const rom uint8_t digits[] = "0123456789ABCDEF";

	uint8_t i = 2;
	output += i;
	*output = '\0';
	do
	{

		*--output = digits[input & 15];
		input >>= 4;
	}
	while(--i);
}
*/

// EDAB: Save the calling function's address in a special buffer not reset on
// startup, to be used to trace final pair of "crumbs" after a watchdog reset
uint24_t PreservedBreadcrumbs[];
#pragma udata Breadcrumbs
uint24_t Breadcrumbs[];
#pragma udata

void DropBreadcrumb(void)
{
	Breadcrumbs[1] = Breadcrumbs[0];
	Breadcrumbs[0] = TOS;
}
//*****************************************************************************/
