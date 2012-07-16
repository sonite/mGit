/******************************************************************************
 *                                                                      	  *
 *        	AD-Converter Protocol for Access Microchip MCP3208 Device     	  *
 *                                                                      	  *
 *              		   Author: Finsun Inresol AB			              *
 ******************************************************************************
 * FileName:        AD_conveter.c											  *
 * Dependencies:	Real_time_clock.h spi.h inttypes.h						  *
 * Processor:       PIC18													  *
 * Complier:        MCC18 v3.13												  *
 * Description: 	Handles AD-Converter IC									  *
 *              		- AD-Converter (MCP3208)							  *
 ******************************************************************************/

//** V A R I A B L E S ********************************************************/
uint16_t adcresult[8];

//** F U N C T I O N S ********************************************************/
// Write the channel channel, stopping just short of then next clock transition
// latching in a new A/D value
void ADCreadprepare(uint8_t channel)
{
	uint8_t i = 5 + 1;
	channel |= 0x18;

	do
	{
		// MSB first
		SPIMOSI = 0;
		if(channel & 0x10)
			SPIMOSI = 1;
		channel <<= 1;

		SPICLK = 0;
		Delay_us(1);
		CS_AD = 0;
		SPICLK = 1;
		Delay_us(1);
	}
	while(--i);
}

// Now clock out the 12 sampled bits from the pre-selected channel
uint16_t ADCreadextract(void)
{
	uint8_t i = 12 + 1;
	uint16_t result = 0;
	
	do
	{
		SPICLK = 0;
		Delay_us(1);
		SPICLK = 1;
		Delay_us(1);

		// MSB first
		result <<= 1;
		if(SPIMISO)
			result |= 1;
	}
	while(--i);

	CS_AD = 1;
	return result;
}
/*
uint16_t ADCread(uint8_t channel)
{
	ADCreadprepare(channel);
	return ADCreadextract();
}

void ADCreadall(uint16_t all[8])
{
	int8_t i;

	for(i = 0; i < 8; ++i)
		*all++ = ADCread(i);
	
}
*/
void ReadExternalADC(void)
{
	uint8_t channel;
	
	for(channel = 0; channel < 8; ++channel)
	{
		ADCreadprepare(channel);
		adcresult[channel] = ADCreadextract();
	}
}

//*****************************************************************************/
