/******************************************************************************
 * Routines for writing to the processor's internal FLASH
 ******************************************************************************/
#ifndef _FLASH_H
#define _FLASH_H

#include <p18cxxx.h>
#include "inttypes.h"

enum
{
	FLASH_ERASE_BLOCK  = 1024,
	FLASH_WRITE_BLOCK  = 64,
	FLASH_UNPROGRAMMED = 0xFF
};

void Flash_Erase(void rom *ptr);
void rom *Flash_Write(void rom *dst, const void *src);

#endif