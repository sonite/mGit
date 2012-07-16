/*********************************************************************
 *
 * Microchip USB C18 Firmware -  HID Bootloader Version 1.0 for PIC18F87J50 Family Devices
 *
 *********************************************************************
 * FileName:        Boot87J50Family.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * File version         Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 1.0					04/09/2008	Original
 ********************************************************************/
#ifndef BOOT87J50FAMILY_H
#define BOOT87J50FAMILY_H

/** P U B L I C  P R O T O T Y P E S *****************************************/
void UserInit(void);
void ProcessIO(void);

void Boot_ResetToApp(void);
void Boot_CheckTimeout(void);

extern unsigned char Boot_Timeout;

// EDAB: Wait for some USB activity for one and a half seconds before
// giving up and starting the app. If contact is made then wait a further
// fifteen seconds to contact the boot loader program
#define BOOT_SHORT_TIMEOUT 1500
#define BOOT_LONG_TIMEOUT 15000

#define BOOT_SHORT_TIMEOUT_TICKS (-(BOOT_SHORT_TIMEOUT * 375UL + 4 >> 3))
#define BOOT_LONG_TIMEOUT_TICKS (-(BOOT_LONG_TIMEOUT * 375UL + 4 >> 3))

// EDAB: Initialize the startup timer
#define Boot_SetShortTimeout()                      \
(                                                   \
	Boot_Timeout = BOOT_SHORT_TIMEOUT_TICKS >> 16,  \
	TMR0H = BOOT_SHORT_TIMEOUT_TICKS >> 8,          \
	TMR0L = BOOT_SHORT_TIMEOUT_TICKS,               \
	T0CON = 0x87                                    \
)

#define Boot_SetLongTimeout()                       \
(                                                   \
	Boot_Timeout = BOOT_LONG_TIMEOUT_TICKS >> 16,   \
	TMR0H = BOOT_LONG_TIMEOUT_TICKS >> 8,           \
	TMR0L = BOOT_LONG_TIMEOUT_TICKS                 \
)

// EDAB: Stop the interrupt when we've started communicating properly with the
// bootloader tool. The timer itself still needs to run so as to keep the
// LED blinking
#define Boot_DisableTimeout() \
(Boot_Timeout = 0)

#endif //BOOT87J50FAMILY_H
