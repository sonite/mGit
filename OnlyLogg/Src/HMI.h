#ifndef HMI_H
#define HMI_H

#include "inttypes.h"

//** G L O B A L  V A R I A B L E S *******************************************/
extern uint8_t lpage;   // HMI page memory
extern uint8_t HMIUpdateRate;

//** G L O B A L  P R O T O T Y P E S *****************************************/
void HMIupdate(void);
void Options(uint8_t set);
void ProcessError(int error);
void FileError(int error, const char rom *name);
void Edit_Clock(void);

//*****************************************************************************/

#endif
