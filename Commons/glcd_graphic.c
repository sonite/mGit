/******************************************************************************
 *                                                                      	  *
 *        	    		GLCD Protocol for graphic routines        		  	  *
 *                                                                      	  *
 *              		        Author: chris			                      *
 ******************************************************************************
 * FileName:        glcd_graphic.c											  *
 * Dependencies:	glcd_routines.h, inttypes.h								  *
 * Processor:       PIC18													  *
 * Complier:        MCC18 v3.13												  *
 * Description: 	Graphic LCD routines such as							  *
 *						- Display String									  *
 *						- Display Buffer									  *
 *              		- Display Bitmap									  *
 *              		- Draw Rectangle									  *
 *              		- Draw Line											  *
 *              		- Draw Circle										  *
 ******************************************************************************/
 
//** V A R I A B L E S ********************************************************/
extern uint8_t cursorX;
extern uint8_t cursorY;

//** I N T E R N A L  P R O T O T Y P E S *************************************/
void SendLcd(uint8_t value, uint8_t isData);
void GotoXY(uint8_t x, uint8_t y);
void putch(uint8_t charCode);

//** F U N C T I O N S ********************************************************/
void Display_String(rom const char *string)
{
	uint8_t charCode;

	while(charCode = *string++, charCode)
		putch(charCode);
}

void Display_StringAt(rom const char *string, uint8_t x, uint8_t y)
{
	cursorX = x;
	cursorY = y;
	Display_String(string);
}

void Display_Buffer(const char *string)
{
	uint8_t charCode;

	while(charCode = *string++, charCode)
		putch(charCode);
}

void Display_BufferAt(const char *string, uint8_t x, uint8_t y)
{
	cursorX = x;
	cursorY = y;
	Display_Buffer(string);
}

void Display_Bitmap(rom const uint8_t *bitmap, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	uint8_t pageIndex, columnIndex;
	cursorX = x;
	cursorY = y;

	for(pageIndex = 0; pageIndex < height / 8; pageIndex++)
	{
		GotoXY(cursorX, cursorY + pageIndex);
		for(columnIndex = 0; columnIndex < width; columnIndex++)
			SendLcd(*(bitmap++), 1);
	}
}

void Display_Rectangle(unsigned char xPos, unsigned char yPos, unsigned char xLine, unsigned char yLine)
{
  unsigned char j;
  for (j = 0; j < yLine; j++) {
		Fill_Pixel2(xPos, yPos + j, 1); // Left
		Fill_Pixel2(xPos + xLine - 1, yPos + j, 1); // Right
	}
  for (j = 0; j < xLine; j++)	{
		Fill_Pixel(xPos + j, yPos + 1); // Top
		Fill_Pixel(xPos + j, yPos + yLine); // Bottom
	}
}

/*
void LineXY(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	uint16_t dx, dy;
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
