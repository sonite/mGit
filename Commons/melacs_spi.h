#ifndef _melacs_spi_H
#define _melacs_spi_H

//#define VERSION_MELACS6 1

#include <string.h>
#include "Utils.h"
#include "OnboardRTCC.h"
#include "OnboardADC.h"

//** G L O B A L  V A R I A B L E S *******************************************/
uint16_t MAX31855ReadCelsius(void);

//** G L O B A L  P R O T O T Y P E S *****************************************/
void SPIinit(void);

//*****************************************************************************/

#endif
