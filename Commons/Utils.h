#ifndef _UTILS_H
#define _UTILS_H
//** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <delays.h>
#include "../Commons/inttypes.h"
#include "../Commons/delay_melacs.h"
//** D E F I N I T I O N S ****************************************************/
/** Watchdog Definitions */
	/* Note: Nominal WTD period is 4ms then multiplied with 16-bit postscaler (available 4ms - 135s)
	   Note: WDTPS have to be set with fuses 
	*/
	#define EnableWDT() (WDTCONbits.SWDTEN = 1)
	#define DisableWDT() (WDTCONbits.SWDTEN = 0)
	#define ClearWDT() _asm CLRWDT _endasm
	/* Watchdog Time-out Flag bit, (0) A WTD time-out occured */
	#define ReadEventWDT() (RCONbits.TO)
/** End of Watchdog Definitions */

//** V A R I A B L E S ********************************************************/
extern uint24_t Breadcrumbs[2];
extern uint24_t PreservedBreadcrumbs[2];
//*****************************************************************************/

//** P R O T O T Y P E S ******************************************************/
//void Delay_1sec( void );
//void Delay_uS(uint8_t uSec);
//void Brakepoint(void);
//void LargeByteToStr(uint16_t value, char string[5]);
//void IntToStr(uint8_t data, char *str);
//void ShortIntToStr(uint16_t data, char *str);
//void LongIntToStr(uint24_t data, char *str);
//void NumberToString(uint24_t data, char *str);
//void RealByteToStr(uint16_t value, char *string);
//void LargeByteToStr(uint16_t value, char string[5]);
//void ByteToStr(uint8_t value, char string[4]);
//void WordToStr(uint16_t value, char string[6]);
void ByteToStrWithZeros(uint16_t input, char *output);
//void WordToStrWithZeros(uint16_t input, char *output);
//void HexToStrWithZeros(uint16_t input, char *output);
void DropBreadcrumb(void);
//*****************************************************************************/
#endif

