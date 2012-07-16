/******************************************************************************
 *                                                                      	  *
 *        	Real-Time Clock Protocol for Access Maxim DS1302Z  Device     	  *
 *                                                                      	  *
 *              		   Author: Finsun Inresol AB			              *
 ******************************************************************************
 * FileName:        Real_time_clock.c										  *
 * Dependencies:	Real_time_clock.h spi.h inttypes.h						  *
 * Processor:       PIC18													  *
 * Complier:        MCC18 v3.13												  *
 * Description: 	Handles Real-Time Clock IC´								  *
 *              		- Real-time clock (DS1302)							  *
 ******************************************************************************/

#	if defined(VERSION_MELACS6)
#include <spi2.h>
#	endif

//** V A R I A B L E S ********************************************************/
struct RTCclk RTCclk;
struct RTCram RTCram;
struct RTCclk RTCclkBuffert;

typedef char RTCramcheck[sizeof(RTCram) <= 31 ? +1 : -1];

static const uint8_t rom RTCrange[] =
{
	0, 59, // Seconds
	0, 59, // Minutes
	0, 23, // Hours
	1, 31, // Days
	1, 12, // Months
	#	if defined(VERSION_MELACS6)
	1, 7, // Weekdays
	#	else
	0, 99, // Weekdays
	#	endif
	0, 99, // Years
};

//** I N T E R N A L  P R O T O T Y P E S *************************************/
static uint8_t RTC_Read_Byte(void);
static void RTC_Write_Byte(uint8_t value);
static void RTC_Command(uint8_t cmd);

//** F U N C T I O N S ********************************************************/
static uint8_t RTC_Read_Byte(void)
{
	uint8_t value;
	uint8_t i = 8;
#	if defined(VERSION_MELACS6)		// All data MSB first
	do
	{
		SPICLK = 1;					// Create a clock pulse
		Delay_us(1);
		SPICLK = 0;
		Delay_us(1);

		value <<= 1;				// Shift over bits to the left by 1
		if(SPIMISO)
			value  |= 0x01;			// Then OR in a 1 if not then leave it a zero
	}
	while(--i);
#	else
	do
	{
		SPICLK = 1;					// Create a clock pulse
		Delay_us(1);
		SPICLK = 0;
		Delay_us(1);

		value >>= 1;				// Shift over bits to the right by 1
		if(SPIMISO)
			value  |= 0x80;			// Then OR in a 1 if not then leave it a zero
	}
	while(--i);
#	endif
	return value;
}

static void RTC_Write_Byte(uint8_t value)
{
	uint8_t i = 8;
#	if defined(VERSION_MELACS6)		// All data MSB first
	do
	{
		SPICLK = 1;
		Delay_us(1);

		SPIMOSI = 0;				// Data out Low
		if(value & 0x80)			// Check if MSB of byte is one
			SPIMOSI = 1;			// Data out high
		value <<= 1;				// Move next bit into position of being checked (left shift)

		SPICLK = 0;
		Delay_us(1);
	}
	while(--i);
#	else							// All data LSB first
	do
	{
		SPICLK = 0;					// Clock Low
		Delay_us(1);

		SPIMOSI = 0;				// Data out Low
		if(value & 0x01)			// Check if LSB of byte is one
			SPIMOSI = 1;			// Data out high
		value >>= 1;				// Move next bit into position of being checked (right shift)

		SPICLK = 1;
		Delay_us(1);
	}
	while(--i);
#	endif
}

static void RTC_Command(uint8_t cmd)
{
#	if defined(VERSION_MELACS6)
	SPICLK = 1;						// Initiate High
#	else
	SPICLK = 0;						// Initiate Low
#	endif

	SPIoff();						// CSSD & CSAD -> Standby(High) | CSRTC -> Standby(High)
	Delay_us(1);
	RTCinit();						// CSSD & CSAD -> Standby(High) | CSRTC -> Ready(Low)
#	ifndef VERSION_MELACS6
	Delay_us(1);
#endif	
	RTC_Write_Byte(cmd);
}

/*
void RTC_Read_Clock_SPI(void)
{
	uint8_t *ptr;
	uint8_t len;
	ptr = (uint8_t *) &RTCclk;
	len = offsetof(struct RTCclk, protect);
	
	SPIoff();						// CSSD & CSAD -> Standby(High) | CSRTC -> Standby(High)
	SSPCON1bits.WCOL = 0;
	SSP1STATbits.BF = 0;
	RTCinit();						// CSSD & CSAD -> Standby(High) | CSRTC -> Ready(Low)
	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);
	WriteSPI1(0xBF);
	DataRdySPI1();					// Wait for Data Transmit/Receipt complete
	do
		{
			// Automatically convert binary to BCD
			uint8_t value = ReadSPI1();
			value = (value >> 4) * 10 + (value & 15);
			
			*ptr++ = value;
		}
		while(--len);
		
	SPIoff();
	CloseSPI1();
}*/

void RTC_Read_Clock(void)
{
	uint8_t *ptr;
	uint8_t len;
	const uint8_t rom *range;
#	if VERSION_MELACS6
	ptr = (uint8_t *) &RTCclk;
	len = offsetof(struct RTCclk, protect);
	range = RTCrange;
#endif		
	INTCONbits.GIE = 0;
	#	ifndef VERSION_MELACS6
	do
	{
		RTCclkBuffert = RTCclk; // First Copy Existing Values
	# 	endif
		
		//Burst-Read Clock Fields Command
		RTC_Command(0xBF);				// 0b10111111 (<#Read/Write><Ram/#Clock><1><1><1><1><1><Read/#Write>)
#	ifndef VERSION_MELACS6
	ptr = (uint8_t *) &RTCclk;
	len = offsetof(struct RTCclk, protect);
	range = RTCrange;
#endif
		
		do
		{
			// Automatically convert binary to BCD
			uint8_t value = RTC_Read_Byte();
			value = (value >> 4) * 10 + (value & 15);
			
			// Is the field out of range?
			#	ifndef VERSION_MELACS6
			if(value < *range++ || value > *range++)
			{
				// Then return a zeroed dummy date instead
				memset(&RTCclk, 0, sizeof RTCclk);
				break;
			}
			#	endif
			
			*ptr++ = value;
		}
		while(--len);
		
		//if (!memcmp(&RTCclk, &RTCclkBuffert, sizeof RTCclk))
		//memmove(&RTCclkBuffert, &RTCclk, sizeof RTCclk);
	
	SPIoff();
		#	ifndef VERSION_MELACS6
	}
	while
	(
		RTCclk.second != RTCclkBuffert.second ||
		RTCclk.minute != RTCclkBuffert.minute ||
		RTCclk.hour != RTCclkBuffert.hour ||
		RTCclk.mday != RTCclkBuffert.mday ||
		RTCclk.month != RTCclkBuffert.month ||
		RTCclk.wday != RTCclkBuffert.wday ||
		RTCclk.year != RTCclkBuffert.year ||
		RTCclk.protect != RTCclkBuffert.protect
	);
	# endif
	
	// !memcmp(&RTCclk, &RTCclkBuffert, sizeof RTCclk));
	
	INTCONbits.GIE = 1;
}

void RTC_Write_Clock(void)
{
	const uint8_t *ptr;
	uint8_t len;
#	if defined(VERSION_MELACS6)	
	ptr = (const uint8_t *) &RTCclk;
	len = sizeof RTCclk;
#endif
	INTCONbits.GIE = 0;

	// Disable Write-Protection
#	if defined(VERSION_MELACS6)
	RTC_Command(0x0F);				// Address 0Fh
	RTC_Write_Byte(0x00);
#	else
	RTC_Command(0x8E);				// Address 8Eh
	RTC_Write_Byte(0x00);
#	endif

	// Burst-Write Clock Fields Command
#	if defined(VERSION_MELACS6)	
	RTC_Command(0x3F);				// 0b00111111 (<Read/#Write><Ram/#Clock><1><1><1><1><1><Read/#Write>)
#	else
	RTC_Command(0xBE);				// 0b10111110 (<1><Ram/#Clock><1><1><1><1><1><Read/#Write>)
	ptr = (const uint8_t *) &RTCclk;
	len = sizeof RTCclk;

#	endif
		
	do
	{
		uint8_t bin = *ptr++;
		// This to automatically convert clock fields from binary to BCD
			uint8_t bcd = bin;
		bcd = (bcd / 10) << 4;
   		bcd = bcd | (bin % 10);
		RTC_Write_Byte(bcd);
	}
	while(--len);
#	if definedVERSION_MELACS6
	RTC_Write_Byte(0x00);
#endif
	SPIoff();

	INTCONbits.GIE = 1;
}

void RTC_Read_RAM(void)
{
#	ifndef VERSION_MELACS6
	uint8_t *ptr;
	uint8_t len;

	// Burst-Read RAM Fields Command
#	if defined(VERSION_MELACS6)
	RTC_Command(0x7F);
#	else
	RTC_Command(0xFF);				// 0b11111111 (<1><#Ram/Clock><1><1><1><1><1><#Read/Write>)
#	endif
	ptr = (uint8_t *) &RTCram;
	len = sizeof RTCram;

	do
		*ptr++ = RTC_Read_Byte();
	while(--len);

	SPIoff();
	# endif
}

void RTC_Write_RAM(void)
{
#	ifndef VERSION_MELACS6
	const uint8_t *ptr;
	uint8_t len;

	// Disable Write-Protection
#	if defined(VERSION_MELACS6)
	RTC_Command(0x0F); 				// Address 0Fh
	RTC_Write_Byte(0x00);
#	else
	RTC_Command(0x8E); 				// Address 8Eh
	RTC_Write_Byte(0x00);
#	endif

	// Burst-Write RAM Fields Command
#	if defined(VERSION_MELACS6)
	RTC_Command(0x7F);
#	else
	RTC_Command(0xFE); 				//1111 1110 (<1><#Ram/Clock><1><1><1><1><1><Read/#Write>)
#	endif
	ptr = (const uint8_t *) &RTCram;
	len = sizeof RTCram;

	do
		RTC_Write_Byte(*ptr++);
	while(--len);
	#endif
}

// Callback function for the file system to get the current date and time
void Microchip_RefreshFileSystemClock(void)
{
	RTC_Read_Clock();

	SetClockVars
	(
		RTCclk.year + 2000, // year
		RTCclk.month,       // month
		RTCclk.mday,        // day
		RTCclk.hour,        // hour
		RTCclk.minute,      // minute
		RTCclk.second       // second
	);

}
//*****************************************************************************/
