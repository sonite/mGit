/******************************************************************************
 *                                                                      	  *
 *        	       GLCD Protocol for Access S6B0724 Device           		  *
 *                                                                      	  *
 *              	       Author: Finsun Inresol AB			              *
 ******************************************************************************
 * FileName:        glcd_routines.c											  *
 * Dependencies:	glcd_routines.h, inttypes.h								  *
 * Processor:       PIC18													  *
 * Complier:        MCC18 v3.13												  *
 * Description: 	Graphic LCD routines such as							  *
 *              		- SendLcd Read/Write								  *
 *              		- Read Buttons										  *
 *              		- Display Cls										  *
 *              		- Display Init										  *
 *						- Fill_Pixel										  *
 ******************************************************************************/
 
//** I N C L U D E S **********************************************************/
#include "glcd_functions.h"

//** I N T E R N A L  D E C L A R A T I O N S *********************************/

enum
	{
		LCD_RS_BIT				=			 1,
		LCD_SI_BIT				=			 4,
		LCD_SCLK_BIT			=			 0,
		LCD_CS_BIT				=			 5,
		
		/* Commands */
		LCD_DISPLAY_ON 			=	0b10101111,
		LCD_DISPLAY_OFF			=	0b10101110,
		LCD_INIT_DISPLAY_LINE	= 	0b01000000,	// Set start line
		LCD_SET_PAGE			=	0xB0,
		LCD_COLUMN_ADR_MSB		=	0b00010000,
		LCD_COLUMN_ADR_LSB		=	0b00000000,
		LCD_CMD_ADC 			=	0b10100000,	// Select SEG Output Direction, Normal
		LCD_REVERSE_OFF 		=	0b10100110,
		LCD_ENTIRE_DISPLAY_OFF	=	0b10100100,	// Normal
		LCD_CMD_BIAS 			=	0b10100011,
		LCD_CMD_RESET	  		=	0b11100010,
		LCD_CMD_RESET	  		=	0b11100010,
		LCD_CMD_SHL 			=	0b11000000,	// Normal Direction
		LCD_CMD_SHL_REV			=	0b11001000,	// Reversed Direction
		LCD_POWERCONTROL1 		=	0b00101000,
		LCD_POWERCONTROL2 		=	0b00101100,
		LCD_POWERCONTROL3 		=	0b00101110,
		LCD_POWERCONTROL4 		=	0b00101111,
		LCD_REGULATOR_RESISTOR 	=	0b00100011, // Set regulator resistor
		LCD_REFERENCE_VOLT_MODE	=	0b10000001, // Set reference voltage mode
		LCD_CONTRAST 			=	0b00100000,	// Set contrast...
		LCD_REVERSE_ON			=	0b10100111,
		LCD_CMD_ADC_REV			=	0b10100001,

		LCD_ENTIRE_DISPLAY_ON	=	0b10100101,	// Entire
		LCD_STATIC_INDIC_MODE1	=	0b10101101,
		LCD_STATIC_INDIC_MODE2	=	0b10101100,
//		LCD_STATIC_INDIC_REG	=	0bxxxxxxRR,
//		LCD_REFERENCE_VOLT_REG 	=	0bxxRRRRRR,
//		LCD_POWER_SAVE			=	0b--------,
		LCD_NOP					=	0b11100011,	// Non-Operation command

		CURSOR_ON   			=	0b00001111,	// Cursor on
		CURSOR_OFF  			=	0b00001101,	// Cursor off
		BLINK_ON    			=	0b00001111,	// Cursor Blink
		BLINK_OFF   			=	0b00001110,	// Cursor No Blink

		/* Definitions */
		CTRL_NUMBER_OF_COLUMNS 	=		   132,
		NUMBER_OF_PAGES 		=			 8,
		NUMBER_OF_COLUMNS 		=		   128,
		STATUS_BUSY 			=		  0x80
	};
	
#define	LCD_LEDON_DIR			TRISDbits.TRISD3
#define LCD_RS    				LATHbits.LATH1	// LCD Register Select Input
#define LCD_RS_DIR				TRISHbits.TRISH1
#define LCD_RS_PORT             LATH
#define LCD_SI    				LATDbits.LATD4	// LCD Serial Input Data
#define LCD_SI_DIR 				TRISDbits.TRISD4
#define LCD_SI_PORT             LATD
#define LCD_SCLK  				LATHbits.LATH0	// LCD Serial Input Clock
#define LCD_SCLK_DIR 			TRISHbits.TRISH0
#define LCD_SCLK_PORT           LATH
#define LCD_CS    				LATDbits.LATD5	// LCD Chip Select Input
#define LCD_CS_DIR 				TRISDbits.TRISD5
#define LCD_CS_PORT             LATD

/* Buttons */
#define LCD_SW1					PORTHbits.RH2	// LCD SW1
#define LCD_SW1_DIR				TRISHbits.TRISH2
#define LCD_SW2   				PORTHbits.RH3	// LCD SW2
#define LCD_SW2_DIR				TRISHbits.TRISH3
#define LCD_SW3   				PORTGbits.RG1	// LCD SW3
#define LCD_SW3_DIR				TRISGbits.TRISG1
#define LCD_SW4   				PORTGbits.RG2	// LCD SW4
#define LCD_SW4_DIR				TRISGbits.TRISG2

//#define POSITION_NORMAL
#define POSITION_REVERSE

#ifdef POSITION_NORMAL
  #define COLUMN_OFFSET	0
#endif

#ifdef POSITION_REVERSE
  #define COLUMN_OFFSET	(CTRL_NUMBER_OF_COLUMNS - NUMBER_OF_COLUMNS)
#endif

//** I N T E R N A L  V A R I A B L E S ***************************************/
uint8_t Display_InvertChar;
static bool Display_Touch;

//** F U N C T I O N S ********************************************************/
#define SendData(value)    (WREG = (value), _SendData())
#define SendControl(value) (WREG = (value), _SendControl())

// Transfer a single byte serially in big-endian format
static void _SendData(void)
{
	LCD_CS = 0;
	
#	define _BIT(mask) \
	if(WREG & (mask)) \
		LCD_SI = 1;   \
	LCD_SCLK = 1;     \
	LCD_SI = 0;       \
	LCD_SCLK = 0;

	_BIT(0x80)
	_BIT(0x40)
	_BIT(0x20)
	_BIT(0x10)
	_BIT(0x08)
	_BIT(0x04)
	_BIT(0x02)
	_BIT(0x01)

#	undef _BIT

	LCD_CS = 1;
}

// Send a control byte
static void _SendControl(void)
{
	LCD_RS = 0;
	_SendData();
	LCD_RS = 1;
}

uint8_t Read_Buttons(void)
{
	uint8_t result;

	if(Display_Touch)
	{
		result =
			LCD_SW1      |
			LCD_SW2 << 1 |
			LCD_SW3 << 2 |
			LCD_SW4 << 3;
	}
	else
	{
		result = 
			LCD_SW1 << 1 | 
			LCD_SW2 << 3 | 
			LCD_SW3      | 
			LCD_SW4 << 2 ;
		result ^= 0x0f;
	}

	return result;
}

void Display_Cls(void)
{
	int8_t x, y;

	y = NUMBER_OF_PAGES - 1;

	do
	{
		Display_Cursor(0, y);

		x = NUMBER_OF_COLUMNS - 1;

		do
			SendData(0);
		while(--x >= 0);
	}
	while(--y >= 0);

	Display_Cursor(0, 0);
}

void Line_Cls(int8_t y)
{
	int8_t x;
	{
		Display_Cursor(0, y);
		x = NUMBER_OF_COLUMNS - 1;
		
		do
			SendData(0);
		while(--x >= 0);
	}
	Display_Cursor(0, y);
}

void Display_Init(void)
{
	// Reset I/O pins
	LCD_SW1   = 0; // LCD_SW1_DIR   = 1; 
	LCD_SW2   = 0; // LCD_SW2_DIR   = 1; 	
 	LCD_SW3   = 0; // LCD_SW3_DIR   = 1;
	LCD_SW4   = 0; // LCD_SW4_DIR   = 1;

	LCD_RS    = 1; // LCD_RS_DIR    = 0; 	 
	LCD_SI    = 0; // LCD_SI_DIR    = 0;
	LCD_CS    = 1; // LCD_CS_DIR    = 0;	
	LCD_SCLK  = 0; // LCD_SCLK_DIR  = 0;

	LCD_LEDON = 1; // LCD_LEDON_DIR = 0; 				

	// Try to detect whether the screen uses regular or touch buttons.
	// Aside from the different pin orderings the real buttons are active-low
	// so assume that's what's being used if all pins are high
	Display_Touch = !(LCD_SW1 && LCD_SW2 && LCD_SW3 && LCD_SW4);

	// Run the register initialization script.
	// This dinky little interpreter saves a little ROM compared to just
	// calling the control and delay functions directly
	{
		static const rom uint8_t script[] =
		{
			LCD_NOP, DELAY_US_10X(200), // Delay_us(200)
			LCD_CMD_RESET,              // RESET
			LCD_CMD_ADC,                // Set ADC
			LCD_CMD_SHL,                // Set HSL
			LCD_CMD_BIAS,               // Set LCD bias
			LCD_REVERSE_OFF,            // Set not reversed
			
			/* Power control */
			LCD_POWERCONTROL1,
			LCD_NOP, DELAY_US_10X(200),
			LCD_POWERCONTROL2,
			LCD_NOP, DELAY_US_10X(200),
			LCD_POWERCONTROL3,
			LCD_NOP, DELAY_US_10X(40),
			LCD_POWERCONTROL4,
			LCD_NOP, DELAY_US_10X(30),
			
			LCD_REGULATOR_RESISTOR,
			LCD_REFERENCE_VOLT_MODE,
			LCD_CONTRAST,
			LCD_INIT_DISPLAY_LINE,
			LCD_DISPLAY_ON,
			LCD_ENTIRE_DISPLAY_OFF,
			
			CURSOR_ON,
			BLINK_ON,
			
			// End of script
			LCD_NOP, 0
		};
		WREG = *script;
		
		do
		{
			do
			{
				_asm TBLRDPOSTINC _endasm
				SendControl(TABLAT);
			}
			while(TABLAT != LCD_NOP);
			
			_asm TBLRDPOSTINC _endasm
			Delay10TCYx(TABLAT);
		}
		while(TABLAT);
	}
	// Clear the screen
	Display_Cls();
}

void Display_SetPixel(uint8_t x, uint8_t y)
{
	y = 64 - y;
	Display_Cursor(x, y >> 3);
	SendData(1 << (y & 7)); // ex (5 & 7) (101 & 111) = 1 Left-Shift = 101 = 1 0000 
}

void Display_ClearPixel(uint8_t x, uint8_t y)
{
	y = 64 - y;
	Display_Cursor(x, y >> 3);
	SendData(~(1 << (y & 7))); // ex (5 & 7) (101 & 111) = 101 Left-Shift = 1010 then inverse = 0101
}

void Display_Cursor(uint8_t x, uint8_t y)
{
	SendControl(LCD_SET_PAGE | y);              // Set row address
	SendControl(LCD_COLUMN_ADR_LSB | (x & 15)); // Set column address low
	SendControl(LCD_COLUMN_ADR_MSB | (x >> 4)); // Set column address high
}

void Display_Char(uint8_t charCode)
{
	uint8_t width;

	width = CHARACTER_WIDTH;
	*Font[charCode - FONT_OFFSET];

	do
	{
		_asm TBLRDPOSTINC _endasm
		SendData(TABLAT ^ Display_InvertChar);
	}
	while(--width);

	SendData(Display_InvertChar);
}

void Display_String(rom const char *string)
{
	uint8_t charCode;

	while(charCode = *string++, charCode)
		Display_Char(charCode);
}

void Display_StringAt(rom const char *string, uint8_t x, uint8_t y)
{
	Display_Cursor(x, y);
	Display_String(string);
}

void Display_Buffer(const char *string)
{
	uint8_t charCode;

	while(charCode = *string++, charCode)
		Display_Char(charCode);
}

void Display_BufferAt(const char *string, uint8_t x, uint8_t y)
{
	Display_Cursor(x, y);
	Display_Buffer(string);
}

void Display_Bitmap(rom const uint8_t *bitmap, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	uint8_t column;

	*bitmap;

	do
	{
		Display_Cursor(x, y);
		++y;
		
		column = width;
		
		do
		{
			_asm TBLRDPOSTINC _endasm
			SendData(TABLAT);
		}
		while(--column);
	}
	while(height -= 8, (int8_t) height >= 0);
}

//Display_Rectangle(0,0,128,10);
/*void Display_Rectangle(unsigned char xPos, unsigned char yPos, unsigned char xLine, unsigned char yLine)
{
	unsigned char j;
	yPos = 64 - (yPos+1);
	for (j = 0; j < xLine; j++)	{
		SendControl(LCD_SET_PAGE | (yPos >> 3));         // Set row address inherency 7-0
		SendControl(LCD_COLUMN_ADR_LSB | (xPos+j & 15)); // Set column address low
		SendControl(LCD_COLUMN_ADR_MSB | (xPos+j >> 4)); // Set column address high
		SendData(1 << (yPos & 7));
	}
	for (j = yLine; j > 0; j--)	{
		SendControl(LCD_SET_PAGE | ((yPos-j) >> 3));   // Set row address inherency 7-0
		SendControl(LCD_COLUMN_ADR_LSB | (xPos & 15)); // Set column address low
		SendControl(LCD_COLUMN_ADR_MSB | (xPos >> 4)); // Set column address high
		SendData(~(0b11111111 << (yPos-j) & 7));
	}
}*/
	//Display_Cursor(0, 63 >> 3); // 111111 rightshift by 0011 = 0000 0111
	//SendData(1 << (63 & 7)); // ex (63 & 7) (111111 & 111) = 0001 Leftshift by 0111 = 1000 0000 = 128
	//Display_Cursor(0x00,0x07);
	//SendData(0x80); 
	
	//Display_Cursor(0, 62 >> 3); // 111110 rightshift by 0011 = 0000 0111
	//SendData(1 << (62 & 7)); // ex (62 & 7) (111110 & 111) = 0001 Leftshift by 0110 = 0100 0000 = 64
	
	//SendControl(0xB0 | 0x07);     // Set page address (10110000 | 111) = 10110111 -> 176 -183
	
	//SendControl(0x00 | (0 & 15)); // Set column address low = 0 LSB
	//SendControl(0x10 | (0 >> 4)); // Set column address high 0001 0000 or 0 = 1 0000 - 16 MSB
	/*
	SendControl(0xB0 | 0x07);
	SendControl(0b00000001); //1 x
	SendControl(0b00010000);
	SendData(0x80);
	SendControl(0xB0 | 0x07); 
	SendControl(0b00000010); //2 x
	SendControl(0b00010000);
	SendData(0x80);
	SendControl(0xB0 | 0x07); 
	SendControl(0b00000000); //2 x
	SendControl(0b00010000);
	SendData(0b11111111);*/
/*
64&7 = 0
55&7 = 7 0100 0000 0x40
54&7 = 6 0010      0x20
53&7 = 5			0x10
52 = 4				0x08
51 = 3				0x04
50 = 2				0x02
49 = 1				0x01
48 = 0*/

/*
	SendControl(0xB0 | 0x07);
	SendControl(0b00001111); //127
	SendControl(0b00010111);
	SendData(0x80);
	*/
	//Display_Cursor(0x00,0x07);
	//SendData(0x40);
	//Display_Cursor(0x00,0x06);//55
	//SendData(0x80);
	
	//Display_SetPixel(0, 6); // Left
	//Display_SetPixel(127, 0 + j); // Right
	
	/*
		DS(1,1);
		DS(1,2);
		DS(1,3);
	*/
/*
	for (j = 0; j < yLine; j++) {
		Display_ClearPixel(xPos, yPos + j); // Left
		Display_ClearPixel(xPos + xLine - 1, yPos + j); // Right
	}

	for (j = 0; j < xLine; j++)	{
		Display_ClearPixel(xPos + j, yPos + 1); // Top
		Display_ClearPixel(xPos + j, yPos + yLine); // Bottom
		Display_SetPixel(xPos + j, yPos + 1); // Top
		Display_SetPixel(xPos + j, yPos + yLine); // Bottom
	}
	*/
//}


/*
void LineXY(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	int16_t dx, dy;
	uint16_t x, y;
	uint8_t max;

	dx = x2 - x1;
	dy = y2 - y1;
	x = (dx >= 0) ? +dx : -dx;
	y = (dy >= 0) ? +dy : -dy;
	max = (x > y) ? x : y;

	dx = (dx << 8) / max;
	dy = (dy << 8) / max;
	x = x1 << 8;
	y = y1 << 8;

	do
	{
		Fill_Pixel(x >> 8, y >> 8);
		x += dx;
		y += dy;
	}
	while(max--);
}
*/

//*****************************************************************************/
