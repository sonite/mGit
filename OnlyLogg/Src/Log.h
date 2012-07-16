#ifndef LOG_H
#define LOG_H
#include <string.h>
#include "Microchip/Microchip.h"
#include "spi.h"
#include "Numeric.h"

typedef enum
{
	LOG_AD7,
	LOG_AD6,
	LOG_AD5,
	LOG_AD4,
	LOG_AD3,
	LOG_AD2,
	LOG_AD1,
	LOG_AD0,
	LOG_ITEMP,
	LOG_I24V,
	LOG_DIO7,
	LOG_DIO6,
	LOG_DIO5,
	LOG_DIO4,
	LOG_DIO3,
	LOG_DIO2,
	LOG_DIO1,
	LOG_DIO0,
	LOG_PULSEPORT1,
	LOG_PULSEPORT2,

	LOG_NUM_FIELDS
}
Log_Field;

void Log_Run(void);
char *Log_Timestamp(char *buffer);

// Callback to get the log values
extern long Log_Value(Log_Field field);

extern uint16_t Log_LongPeriod;

#endif