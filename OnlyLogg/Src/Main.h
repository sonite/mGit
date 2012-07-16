#ifndef MAIN_H
#define MAIN_H

//#define VERSION_MELACS6 1

//** I N C L U D E S **********************************************************/
#include <stdlib.h>
//#include <capture.h>
//#include <timers.h>

/** ! SPECIAL APPLICATION  P R O T O T Y P E S ******************************************/
#include "HMI.h"
#include "Log.h"
//#include "Tracking.h"

/** ! UTILS  P R O T O T Y P E S ********************************************************/
#include "Utils.h"

/** ! MICROCHIP APPLICATION  P R O T O T Y P E S*****************************************/
#include "Microchip/Microchip.h" //FISO, TCPIP, TICK, USB

/** ! SPI  P R O T O T Y P E S **********************************************************/
#include "melacs_spi.h" //ADC and RTC

/** ! INTERNAL ADC  P R O T O T Y P E S *************************************************/
#include "IADC.h"

/** ! GLCD  P R O T O T Y P E S *********************************************************/
#include "glcd_functions.h"

/** ! SIP  R E L A Y S  P R O T O T Y P E S *********************************************/
#include "sip_relays.h"

/** ! MOTOR CONTROL P R O T O T Y P E S *************************************************/
//#include "Motor.h"
//#include "Statemachine.h"

/** ! CONFIGURATION FILE PARSER P R O T O T Y P E S *************************************/
#include "Config.h"

/** ! LINE EDITING P R O T O T Y P E S **************************************************/
#include "Editable.h"

/** ! NUMERIC DISPLAY P R O T O T Y P E S ***********************************************/
#include "Numeric.h"

/** ! FLASH WRITING P R O T O T Y P E S *************************************************/
#include "Flash.h"

/** ! BUZZER P R O T O T Y P E S ********************************************************/
#include "Buzzer.h"

/** ! FLOW P R O T O T Y P E S **********************************************************/
#include "Flow.h"

/** ! SOFT PWM P R O T O T Y P E S ******************************************************/
//#include "SoftPWM.h"

/** ! GLOBAL P R O T O T Y P E S ********************************************************/
void Ports_Init(void);
void TreatSignals(void);
void AlarmCheck(void);
void Control(void);

//** G L O B A L  V A R I A B L E S *******************************************/
extern char rom SoftInfo[];
extern unsigned char serial_number[8];

//extern enum { PPL_SENSOR0 = 855, NUMBER_OF_SENSORS = 1};
extern enum { NUMBER_OF_SENSORS = 2};

extern struct Pulse_t // Array Structure
{
	uint24_t liters;
	uint24_t pulses;
	uint16_t rate;
	uint8_t decimal;
	uint8_t active;
	uint16_t punits;
}
PulsePort[NUMBER_OF_SENSORS]; // With index

extern struct Port_t
{
	int24_t AD;
	int16_t Offset;
	int16_t Factor;
	uint8_t DIO;
} 
Port[8];

extern uint8_t ITEMP;
extern uint16_t I24V;


#endif
