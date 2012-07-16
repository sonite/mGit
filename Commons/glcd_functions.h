#ifndef _glcd_functions_H
#define _glcd_functions_H

#include <p18cxxx.h>
#include <delays.h>
#include "../Commons/inttypes.h"
#include "../Commons/delay_melacs.h"

/** ! G L O B A L  D E C L A R A T I O N S **********************************************/
// Pins
#define LCD_LEDON 				LATDbits.LATD3			// LED ON (Backlight)

/** ! G L O B A L  P R O T O T Y P E S **************************************************/
extern uint8_t Read_Buttons(void);
void Display_Cls(void);
void Line_Cls(int8_t y);
void Display_Init(void);
void Display_SetPixel(uint8_t x, uint8_t y);
void Display_ClearPixel(uint8_t x, uint8_t y);
void Display_Cursor(uint8_t x, uint8_t y);
void Display_Char(uint8_t ch);
void Display_String(rom const char *string);
void Display_StringAt(rom const char *string, uint8_t x, uint8_t y);
void Display_Buffer(const char *string);
void Display_BufferAt(const char *string, uint8_t x, uint8_t y);
void Display_Bitmap(rom const uint8_t *bitmap, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
//void Display_Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a);
//void LineXY(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

extern uint8_t Display_InvertChar;

//** B I T M A P  D E F I N I T I O N S  ***********************************************/
//#include "../Commons/Bitmaps/flanders.h"
//#include "../Commons/Bitmaps/kiwacan.h"
//#include "../Commons/Bitmaps/tester2.h"
//#include "../Commons/Bitmaps/mixed.h"
//#include "../Commons/Bitmaps/card.h"
//#include "../Commons/Bitmaps/arrow.h"
#include "../Commons/Bitmaps/logo.h"

//***************************************************************************************/

//** F O N T  D E F I N I T I O N S  ***************************************************/
#include "../Commons/Fonts/Font5x7.h"
//#include "../Commons/Fonts/Font5x5.h"
//#include "../Commons/Fonts/corsiva_12.h"
//#include "../Commons/Fonts/arial_bold_14.h"

#endif
