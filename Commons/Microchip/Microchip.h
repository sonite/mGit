#ifndef _MICROCHIP_H
#define _MICROCHIP_H

#include "../Commons/Microchip/TCPIP Stack/TCPIP.h"
#include "../Commons/Microchip/MDD File System/FSIO.h"
#include "../Commons/Config.h"


#define MICROCHIP_USE_USB 1
#define MICROCHIP_USE_IP  1

#define Microchip_Config AppConfig

/* SSP1CON1 = 1010 0000, SSP1STAT = 1000000 */
//SSP1CON1bits.WCOL = 1; The SSPxBUF register is written while it is still transmitting the previous word
//SSP1CON1bits.SSPOV = 0; No overflow
//SSP1CON1bits.SSPEN = 1; Enables serial port and configures SCKx, SDOx, SDIx and SSx as serial port pins
//SSP1CON1bits.CKP = 0; Idle state for clock is a low level
//SSP1CON1bits.SSPM = 0000; SPI Master mode, clock = FOSC/4
//SSP1STAT1bits.SMP = 1; Input data sampled at end of data output time
//SSP1STAT1bits.CKE = 0; Transmit occurs on transition from Idle to active clock state
//SSP1STAT1bits.BF = 0; Receive not complete, SSPxBUF is empty
#define Microchip_RestoreSPI() (SSP1STAT = 0x40, SSP1CON1 = 0xA0)
#define Microchip_ReleaseSPI() (SSP1CON1bits.SSPEN = 0)

extern unsigned char PulseChoice;

extern enum Microchip_FileSystem
{
	MICROCHIP_DISMOUNTED,
	MICROCHIP_REMOVED,
	MICROCHIP_MOUNTED
}
Microchip_FileSystem;

typedef enum
{
	MICROCHIP_READ       = 'r',
	MICROCHIP_WRITE      = 'w',
	MICROCHIP_APPEND     = 'a',
	MICROCHIP_READPLUS   = 'r' | ('+' << 8),
	MICROCHIP_WRITEPLUS  = 'w' | ('+' << 8),
	MICROCHIP_APPENDPLUS = 'a' | ('+' << 8)
}
Microchip_Access;

void Microchip_Init(void);
void Microchip_Run(TICK period);
void Microchip_RefreshConfig(void);
void Microchip_Mount(void);
void Microchip_Dismount(void);
void Microchip_UnblockUSB(void);
void Microchip_UnblockHTTP(void);
void Microchip_EnterBootloader(void);
FSFILE *Microchip_FileOpen(const char *filename, Microchip_Access access);
void Microchip_FileClose(FSFILE *file);
void Microchip_InterruptHandler(void); // Implemented 20120106

// These are callback functions to be implemented by the user!
extern void Microchip_InterruptHook(void);
extern void Microchip_RefreshFileSystemClock(void);
extern void Microchip_RefreshSettings(void);
extern const char rom Microchip_DefaultPage[];
extern const struct Cfg_Variable rom Microchip_Settings[];

#endif