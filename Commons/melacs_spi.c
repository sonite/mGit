/******************************************************************************
/******************************************************************************
 *                                                                      	  *
 *        	    	SPI Protocol for Access Various Devices   		       	  *
 *                                                                      	  *
 *              		   Author: Finsun Inresol AB			              *
 ******************************************************************************
 * FileName:        spi.c													  *
 * Dependencies:	spi.h inttypes.h										  *
 * Processor:       PIC18													  *
 * Complier:        MCC18 v3.13												  *
 * Description: 	Handles SPI bus IC´s									  *
 *              		- A/D converter (MCP3208)							  *
 *              		- Real-time clock (DS1302)							  *
 *              		- SD memory card									  *
 *              		- Add-on SPI circuits								  *
 ******************************************************************************/

//** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "melacs_spi.h"

//** I N T E R N A L  D E C L A R A T I O N S *********************************/
//	Pins
#define CS_SD				LATJbits.LATJ7
#define CS_SD_DIR			TRISJbits.TRISJ7	
#define CS_RTC				LATJbits.LATJ1
#define CS_RTC_DIR			TRISJbits.TRISJ1
#define CS_AD				LATJbits.LATJ0
#define CS_AD_DIR			TRISJbits.TRISJ0
#define CS_SPI2				LATBbits.LATB0
#define CS_SPI2_DIR			TRISBbits.TRISB0
#define CS_SPI1				LATBbits.LATB1
#define CS_SPI1_DIR 		TRISBbits.TRISB1
#define SPICLK  			LATCbits.LATC3
#define SPICLK_DIR  		TRISCbits.TRISC3
#define SPIMISO 			PORTCbits.RC4
#define SPIMISO_DIR			TRISCbits.TRISC4
#define SPIMOSI 			PORTCbits.RC5
#define SPIMOSI_DIR 		TRISCbits.TRISC5

// Commands
/**
ADCinit() används inte längre, men om den gjorde det
LATJ kommer med 1000 0101
LATJ = (1000 0101) ^ (1000 0101) & ~(0111 1100) ^ (1000 0000)
LATJ = (1000 0101) XOR (1000 0101) AND Twidle(0111 1100) XOR (1000 0000)
LATJ = 0000 0000 AND 0000 0011 = 0000 0000
*/
// LATJ incoming with 1000 0111 (87) from Ports_init else 1000 0101 (85)
#	if defined(VERSION_MELACS6)
	#define ADCinit() 		(LATJ ^= LATJ & ~0x7C ^ 0x82) // 1--- --10 (CSSD HIGH / CSRTC HIGH / CSAD LOW)
	#define RTCinit() 		(LATJ ^= LATJ & ~0x7C ^ 0x81) // 1--- --01 (CSSD HIGH / CSRTC LOW  / CSAD HIGH)
	#define SDinit()  		(LATJ ^= LATJ & ~0x7C ^ 0x03) // 0--- --11 (CSSD LOW  / CSRTC HIGH / CSAD HIGH)
	#define SPIoff()		(LATJ ^= LATJ & ~0x7C ^ 0x83) // 1--- --11 (CSSD HIHG / CSAD HIGH  / CSRTC LOW)
#	else
	#define ADCinit() 		(LATJ ^= LATJ & ~0x7C ^ 0x80) // 1--- --00 (CSSD HIGH - CSRTC/CSAD LOW)
	#define RTCinit() 		(LATJ ^= LATJ & ~0x7C ^ 0x83) // 1--- --11 (CSSD/CSRTC/CSAD HIGH) OK
	#define SDinit()  		(LATJ ^= LATJ & ~0x7C ^ 0x01) // 0--- --01 (CSSD/CSRTC LOW - CSAD HIGH)
	#define SPIoff()		(LATJ ^= LATJ & ~0x7C ^ 0x81) // 1--- --01 (CSSD/CSAD HIGH - CSRTC LOW)
#	endif

//** I N T E R N A L  V A R I A B L E S ***************************************/


//** I N T E R N A L  P R O T O T Y P E S *************************************/

//** F U N C T I O N S ********************************************************/
void SPIinit(void)
{
	CS_SD = 1; 			CS_SD_DIR = 0;		// Standby mode
	CS_RTC = 1; 		CS_RTC_DIR = 0;		// Standby mode
	CS_AD = 1; 			CS_AD_DIR = 0;		// Standby mode
	CS_SPI1 = 1;		CS_SPI1_DIR = 0;	// Standby mode
	
	SPICLK = 0;			SPICLK_DIR = 0;
	SPIMISO = 0;		SPIMISO_DIR = 1;
	SPIMOSI = 0;		SPIMOSI_DIR = 0;
}

/******************************************************************************
* Function:        ADCReadPrepare(), ADCReadExtract(), ADCRead(), ADCReadAll()
* PreCondition:    None
* Input:		   None	
* Output:		   None	
* Overview:		   A/D Converter.
*******************************************************************************/
#include "OnboardADC.c"

/******************************************************************************
* Function:        RTCReadByte(), RTCWriteByte(), RTCCommenad(), RTCReadClock(), 
*				   RTCWriteClock(), RTCReadRAM(), RTCWriteRAM()RTCwritebyte(),
*				   RTCwritecommand(), Setclk(), Getclkreg()
* PreCondition:    None
* Input:		   None	
* Output:		   None	
* Overview:		   Real-Time Clock.
*******************************************************************************/
#include "OnboardRTCC.c"

static uint32_t MAX31855_SpiRead32(void);
static uint16_t MAX31855UpperBits(void);
//** F U N C T I O N S ********************************************************/

uint32_t MAX31855_SpiRead32(void) 
{
	uint8_t i = 32 + 1;
	uint32_t noArray1 = 0;
	
	SPIoff();
	SSP1CON1bits.SSPEN = 0;
	INTCONbits.GIE = 0;
	CS_SPI1 = 0; // Driving CS Low to alow read
	SPICLK = 0;
	
	// Clock out 32-bits to noArray1
	do
	{
		
		SPICLK = 0; // Read at falling edge
		
		// MSB First
		noArray1 <<=1;
		if(SPIMISO)
			noArray1 |= 1;
		
		SPICLK = 1;
	}
	while(--i);
	
	CS_SPI1 = 1;
	INTCONbits.GIE = 1;
	
	return noArray1;
}

uint16_t MAX31855UpperBits(void) 
{
	uint32_t RawRegisterValue = 0;
	uint16_t Thermo14 = 0;
	uint8_t i = 16 - 1;
	
	RawRegisterValue = MAX31855_SpiRead32();
	
	// 16bit to small, will discard last 16bits so loop is ok
	do
	{
		RawRegisterValue >>= 1;
	}
	while(i--);
	Thermo14 = RawRegisterValue;
	
	return Thermo14;
}

uint16_t MAX31855ReadCelsius(void) 
{
	int16_t RegisterUpperBits;
	
	RegisterUpperBits = MAX31855UpperBits();
	
	// Check if Fault bit is set (D16 = 1)
	// Is set if D2,D1 or D0 is set
	if (RegisterUpperBits & 0x1) 
		return 2000;
	
	RegisterUpperBits &= 0xfffc; // Mask the lower two bits D17 and D16
	RegisterUpperBits /= 16.0;
	
	return RegisterUpperBits;
}   
//*****************************************************************************/
