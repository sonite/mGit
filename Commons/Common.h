#ifndef _COMMON_H
#define _COMMON_H
#include <p18cxxx.h>
#include <delays.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


/** ! MICROCHIP APPLICATION L I B R A R I E S********************************************/
#include "Microchip/Microchip.h"

/** ! U T I L S  P R O T O T Y P E S ****************************************************/
#include "Utils.h"

/** ! G L C D  P R O T O T Y P E S ******************************************************/
#include "glcd_functions.h"

/** ! S P I  P R O T O T Y P E S ********************************************************/
#include "spi.h" //ADC and RTC

/** ! S I P  R E L A Y S  P R O T O T Y P E S *******************************************/
#include "sip_relays.h"

/** ! MOTOR CONTROL P R O T O T Y P E S *************************************************/
#include "Motor.h"

/** ! INTERNAL ADC  P R O T O T Y P E S *************************************************/
#include "IADC.h"

/** ! CONFIGURATION FILE PARSER P R O T O T Y P E S *************************************/
#include "Config.h"

/** ! LINE EDITING P R O T O T Y P E S **************************************************/
#include "Editable.h"

/** ! NUMERIC DISPLAY P R O T O T Y P E S ***********************************************/
#include "Numeric.h"

/** ! H A R D W A R E  P R O T O T Y P E S **********************************************/
void Ports_Init(void);

/** ! FLASH WRITING P R O T O T Y P E S *************************************************/
#include "Flash.h"

/** ! BUZZER P R O T O T Y P E S ********************************************************/
#include "Buzzer.h"
//#include "Statemachine.h"
//#include "Flow.h"

#endif
