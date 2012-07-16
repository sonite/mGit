#ifndef _IADC_H
#define _IADC_H

#include <p18cxxx.h>
#include "inttypes.h"

//** V A R I A B L E S ********************************************************/
typedef enum
{
	IADC_DIO7,
	IADC_DIO6,
	IADC_DIO5,
	IADC_DIO4,
	IADC_M1CUR,
	IADC_M2CUR,
	IADC_TEMP,
	IADC_24V,
	IADC_12V,
	IADC_5V,
	IADC_3V,

	IADC_CHANNELS
}
IADCchannel;

extern unsigned int IADCsample[IADC_CHANNELS];

//*****************************************************************************/

//** P R O T O T Y P E S ******************************************************/
IADCchannel ReadInternalADC(void);
void IADCinit(void);

#endif
