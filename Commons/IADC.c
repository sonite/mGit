/******************************************************************************
 *                                                                      	  *
 *        	    	none  		       	  *
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
#include "IADC.h"

//** V A R I A B L E S ********************************************************/
uint8_t StartUp;

// Most recent sample values in 10-bit precision
unsigned int IADCsample[IADC_CHANNELS];

// Index of the channel currently being processed
static IADCchannel IADCstate;

// ADCON0 channel selection words
static const rom uint8_t IADCmux[IADC_CHANNELS] =
{
#	define CH(chan) ((chan) << 2 | 0x03)

	CH(15), // IADC_DIO7
	CH(14), // IADC_DIO6
	CH(13), // IADC_DIO5
	CH(12), // IADC_DIO4
	CH(11), // IADC_M1CUR
	CH(10), // IADC_M2CUR
	CH(7),  // IADC_TEMP
	CH(3),  // IADC_24V
	CH(2),  // IADC_12V
	CH(1),  // IADC_5V
	CH(0),  // IADC_3V

#	undef CH
};

//** F U N C T I O N S ********************************************************/
void IADCinit(void)
{
	StartUp = 1;
	
	// Alternate SFR (Shared Address SFR Select bit) is selected to alow writes into shared Space
	WDTCONbits.ADSHR = 1;
	
	/* Orginal Configuration
	 * ANCON0bits.PCFG7  = 0; // IADC_TEMP:  RF2/AN7
	 * ANCON0bits.PCFG4  = 1; // SIP0:		 RA4/AN4
	 * ANCON0bits.PCFG3  = 0; // IADC_24V:   RA3/AN3
	 * ANCON0bits.PCFG2  = 0; // IADC_12V:   RA2/AN2 - M2VOLT
	 * ANCON0bits.PCFG1  = 0; // IADC_5V:    RA1/AN1
	 * ANCON0bits.PCFG0  = 0; // IADC_3V:    RA0/AN0 - M1VOLT
	 */
	ANCON0 = 0b00010000; // Orginal (~0x8F)
	
	/* Orignal Configuration
	 * ANCON1bits.PCFG15 = 1; // IADC_DIO7:  RH7/AN15
	 * ANCON1bits.PCFG14 = 1; // IADC_DIO6:  RH6/AN14
	 * ANCON1bits.PCFG13 = 1; // IADC_DIO5:  RH5/AN13
	 * ANCON1bits.PCFG12 = 1; // IADC_DIO4:  RH4/AN12
	 * ANCON1bits.PCFG11 = 0; // IADC_M1CUR: RF6/AN11
	 * ANCON1bits.PCFG10 = 0; // IADC_M2CUR: RF5/AN10
	 */
	ANCON1 = 0b11110000; // Original (~0x0C)
	//ANCON1 = 0b00000000; 	// All Analog (~0xFC)
	
	WDTCONbits.ADSHR = 0; // Default SFR alows writes to first part of shared space
	
	/* Orginal Configuration
	 * ADCON0bits.VCFG1 = 0; // AVss
	 * ADCON0bits.VCFG0 = 0; // AVDD
	 * ADCON0bits.CHS3 = 0;  // Channel 00 (AN0)
	 * ADCON0bits.CHS2 = 0;  //
	 * ADCON0bits.CHS1 = 0;  //
	 * ADCON0bits.CHS0 = 0;  //
	 * ADCON0bits.DONE = 0;  // A/D Idle
	 * ADCON0bits.ADON = 0;  // A/D Enabled
	 */
	ADCON0 = 0b00000000; // Orginal (0x01)
	
	/* Orginal Configuration
	 * ADCON1bits.ADFM = 1;   // Right justified
	 * ADCON1bits.ADCAL = 1;  // Calibration is performed on next A/D conversion
	 * ADCON1bits.ACQT2 = 1;  // A/D Acquisition Time Select bits (20)
	 * ADCON1bits.ACQT1 = 1;  //
	 * ADCON1bits.ACQT0 = 1;  //
	 * ADCON1bits.ADCS2 = 1;  // A/D Conversion Clock Select bits (FOSC/64)
	 * ADCON1bits.ADCS1 = 1;  //
	 * ADCON1bits.ADCS0 = 0;  //
	 */
	ADCON1 = 0b11111110; // Orginal (0xFE)
	
	ADCON0bits.ADON = 1;  // A/D Module Enabled
	// No A/D Interrupts
	
	// Start Conversion
	ADCON0bits.GO = 1; // A/D conversion in progress
	
	ReadInternalADC();
}

IADCchannel ReadInternalADC(void)
{
	IADCchannel chan = IADC_CHANNELS;
	IADCchannel next;
	uint8_t check = chan;
	
	do {
	
		// Conversion ready?
		while(ADCON0bits.GO); // Wait for Go done
	
		if(ADCON1bits.ADCAL)
			ADCON1bits.ADCAL = 0;
		
		chan = IADCstate;
		IADCsample[chan] = ADRES;
		
		// Start reading from the next channel
		if(next = chan + 1, next >= IADC_CHANNELS)
			next = 0;
		
		IADCstate = next;
		ADCON0 = IADCmux[next];
		
		if (check == chan+1 || StartUp == 2)
		{
			StartUp = 0;
		}
		
	}while(StartUp);
	
	StartUp = 2;
		
	// Return a dummy value or the channel we just read
	return chan;
}

//*****************************************************************************/
