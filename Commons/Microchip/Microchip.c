#include "HardwareProfile.h"
#include "MDD File System/SD-SPI.h"
#include "USB/USB.h"
#include "USB/usb_function_msd.h"
#include "TCPIP Stack/TCPIP.h"
#include "HardwareInfo.h"
#include "Microchip.h"
#include "main.h"

enum Microchip_FileSystem Microchip_FileSystem = MICROCHIP_DISMOUNTED;
static BOOL Microchip_RemountFileSystem = TRUE;
static TICK Microchip_Timer /* = 0 */;
static BOOL Microchip_AllowUSB /* = FALSE */;
static BOOL Microchip_AllowHTTP /* = FALSE */;

APP_CONFIG AppConfig;
unsigned long Microchip_HTTPValue[16];


/****************************************************************************
 * This routine initializes the hardware.  It is a generic initialization
 * routine for many of the Microchip development boards, using definitions
 * in HardwareProfile.h to determine specific initialization.
 ****************************************************************************/
void Microchip_Init(void)
{
#	if defined(USE_USB_BUS_SENSE_IO)
	tris_usb_bus_sense = INPUT_PIN;
#	endif
#	if defined(USE_SELF_POWER_SENSE_IO)
	tris_self_power = INPUT_PIN;
#	endif

	// Enable interrupts
	RCONbits.IPEN = 1;		 	// Enable priority levels on interrupts
	INTCONbits.GIEH = 1;		// Enables all high-priority interrupts (when RCONbits.IPEN = 1)
	INTCONbits.GIEL = 1;		// Enables all low-priority peripheral interrupts (when RCONbits.IPEN = 1)

	// Initialize USB
#	if MICROCHIP_USE_USB
	USBDeviceInit();
	USBDeviceAttach();
#	endif

	// Start tick counter
	TickInit();

#	if MICROCHIP_USE_IP
	{
		// Set basic ethernet configuration
		AppConfig.Flags.bIsDHCPEnabled = TRUE;
		AppConfig.Flags.bInConfigMode = TRUE;
		memcpypgm2ram(&AppConfig.MyMACAddr, (const void far rom *) HardwareInfo.MAC, sizeof AppConfig.MyMACAddr);
		AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
		AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
		AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
		AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
		AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
		/*
		AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
		AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
		*/

		// Initialize ethernet
		StackInit();
	}
#	endif

	// Run for a minimal time to gain access to the file system
	Microchip_Run(0);
}


/****************************************************************************
 * Run the cooperatively-multitasking TCP/IP stack and USB device tasks for
 * a specified (minimum) period of time. In addition this loop tries to
 * tries to mount the file system whenever possible
 ****************************************************************************/
static void Microchip_PerformDismount(BOOL safe)
{
#	if MICROCHIP_USE_IP
	HTTP_FileSystemReset(safe);
#	endif
	Microchip_FileSystem = MICROCHIP_DISMOUNTED;
}

void Microchip_Run(TICK period)
{
	TICK tick;
	bool usbActivity;

	tick = TickGet();
	period += tick;
	usbActivity = false;

	do
	{
		// Show whether the file system is currently mounted or not
		SD_LED = (Microchip_FileSystem != MICROCHIP_MOUNTED);
		
		// Restore the SPI port in case MMC detection/initialization has touched it
		Microchip_RestoreSPI();
		
		// Dismount the file system if the MMC stick has been ejected
		if(Microchip_FileSystem == MICROCHIP_REMOVED)
			Microchip_PerformDismount(FALSE);
			
		// This task performs normal stack task including checking
		// for incoming packet, type of packet and calling
		// appropriate stack entity to process it.
#		if MICROCHIP_USE_IP
		if(!usbActivity)
		{
			StackTask();
			
			// Run the HTTP server
			if(Microchip_AllowHTTP)
				HTTP_Server(Microchip_FileSystem == MICROCHIP_MOUNTED);
		}
#		endif

		// Run USB tasks
#		if MICROCHIP_USE_USB
		if(Microchip_AllowUSB && USBDeviceState >= CONFIGURED_STATE && !USBSuspendControl)
		{
			BYTE timeout;

			// Don't access the file system while the memory stick is mounted
			// via USB
			if(Microchip_FileSystem != MICROCHIP_DISMOUNTED)
				Microchip_PerformDismount(TRUE);

			// Speed up USB processing by running mass-storage processing
			// several times in a row without any intervening logic
			timeout = 64;
			while(MSDTasks() != MSD_WAIT && --timeout)
			{
				// Don't bother processing network packets more than once per update
				// either, StackTask eats a fair chunk of processing power and
				// web pages cannot be served without a file system anyway
				usbActivity = true;
			}
		}
		// Occasionally do some extra processing when not attached as a USB device
		else
#		endif
		if(tick >= Microchip_Timer)
		{
			Microchip_Timer = tick + (TICK_SECOND / 4UL);

			// Mount the file system if a MMC card has been inserted
			if(Microchip_FileSystem == MICROCHIP_DISMOUNTED)
			{
				// Allow the file system to be remounted if the MMC stick
				// has been ejected and reinserted
				if(!Microchip_RemountFileSystem)
				{
					if(!MDD_SDSPI_MediaDetect())
						Microchip_RemountFileSystem = TRUE;
				}
				else if(FSInit())
					Microchip_FileSystem = MICROCHIP_MOUNTED;
			}
			// Make sure the card is still there
			else if(!MDD_SDSPI_MediaDetect())
				Microchip_FileSystem = MICROCHIP_REMOVED;
		}
	}
	while(tick = TickGet(), tick < period);

	// Disable hardware SPI
	Microchip_ReleaseSPI();
}


/****************************************************************************
 * Allow the MMC card to be safely ejected by voluntarily dismounting the 
 * file system
 ****************************************************************************/
void Microchip_Dismount(void)
{
	Microchip_FileSystem = MICROCHIP_REMOVED;
	Microchip_RemountFileSystem = FALSE;
}

// And allow the file system to be remounted after a voluntary dismount
void Microchip_Mount(void)
{
	Microchip_RemountFileSystem = TRUE;
}

// Let the file system be mounted via USB
void Microchip_UnblockUSB(void)
{
	Microchip_AllowUSB = TRUE;
}

// Let the web-server bind the listening sockets to the HTTP port
void Microchip_UnblockHTTP(void)
{
	Microchip_AllowHTTP = TRUE;
}


/****************************************************************************
 * Forcibly enter the bootloader through a software reset
 ****************************************************************************/
void Microchip_EnterBootloader(void)
{
	_asm reset _endasm
}


/****************************************************************************
 * Process updated network settings. At present the only action is update and
 * announce the new MAC address
 ****************************************************************************/
void Microchip_RefreshConfig(void)
{
#	if MICROCHIP_USE_IP
	Microchip_RestoreSPI();

	MACRefreshAddress();
#	ifndef STACK_USE_AUTO_IP
	ARPAnnounce();
#	endif

	Microchip_ReleaseSPI();
#	endif
}


/****************************************************************************
 * These are wrapper functions for opening and closing MDD files.
 * Note that the SPI channel is configured for MMC access while the file is
 * open so no other functions wanting to touch the port should be used
 * simultaneously
 ****************************************************************************/
FSFILE *Microchip_FileOpen(const char *filename, Microchip_Access access)
{
	FSFILE *file = NULL;

	if(Microchip_FileSystem == MICROCHIP_MOUNTED)
	{
		char accessString[] = { access, access >> 8, '\0' };

		// Update file system clock in case a creation time is needed
		if(access != MICROCHIP_READ || access != MICROCHIP_READPLUS)
			Microchip_RefreshFileSystemClock();

		Microchip_RestoreSPI();

		file = FSfopen(filename, accessString);

		if(!file)
			Microchip_ReleaseSPI();
	}

	return file;
}

void Microchip_FileClose(FSFILE *file)
{
	// Update the file system clock in case a last access/modification time is needed
	if(file->flags.write)
	{
		Microchip_ReleaseSPI();
		Microchip_RefreshFileSystemClock();
		Microchip_RestoreSPI();
	}

	FSfclose(file);
	Microchip_ReleaseSPI();
}


/****************************************************************************
 * PIC18 Interrupt Service Routines (ISR).
 *
 * NOTE: Several PICs, including the PIC18F4620 revision A3 have a
 * RETFIE FAST/MOVFF bug. The interruptlow keyword is used to work around the
 * bug when using C18
 ****************************************************************************/
extern void _startup(void);

/* II. No PWM but FLOW */
#ifndef SOFT_PWM_H
	#pragma interrupt Microchip_InterruptHook // interrupt fname
	void Microchip_InterruptHook(void)
	{
		#ifdef STEPPER_H
			Stepper();
		#endif
			
		#ifdef FLOW_H
			Flow_Interrupt();
		#endif
	}
#endif

/* III. No FLOW but PWM */
#ifndef FLOW_H
	#ifdef SOFT_PWM_H
		//SOFT_PWM_FAST_INTERRUPT // Use this if it's the only High-priority interrupt
		// Whole rutine takes 163(decimal) cycles, when declaring like this, shadow
		// operations are added, see in Disassembly listing.
		#pragma interrupt Microchip_InterruptHook
		void Microchip_InterruptHook(void)
		{
			//SoftwarePWM_FAST_INTERRUPT();
			SoftPWM_Interrupt();	
		}
	#endif
#endif

/* IV. FLOW and PWM */
#ifdef FLOW_H
	#ifdef SOFT_PWM_H
		#pragma interrupt Microchip_InterruptHook
		void Microchip_InterruptHook(void)
		{
			Flow_Interrupt();
			SoftPWM_Interrupt();
		}
	#endif
#endif

#pragma interruptlow Microchip_InterruptHandler //interruptlow fname
void Microchip_InterruptHandler(void)
{
	if(INTCONbits.TMR0IF){
		TickUpdate();
		}
#	if MICROCHIP_USE_USB
	if(PIR2bits.USBIF)
		USBDeviceTasks();
#	endif

}

// Default forwarding code vectors used in case the boot-loader is missing
//#pragma code BootResetVector
//void Microchip_BootResetVector(void)
//{ _asm goto _startup _endasm }
#pragma code BootHighISRVector // In lkr file this refers to h0x000008 that is correct but what about Microchip_InterruptHook (C.G)?
void Microchip_BootHighISRVector(void)
{ _asm goto Microchip_InterruptHook _endasm }

#pragma code BootLowISRVector
void Microchip_BootLowISRVector(void) // In lkr file this refers to 0018h, its the low-priority vector.
{ _asm goto Microchip_InterruptHandler _endasm }

// Forwarded application-space code vectors
#pragma code AppResetVector
void Microchip_AppResetVector(void)
{ _asm goto _startup _endasm }

#pragma code AppHighISRVector
void Microchip_AppHighISRVector(void)
{ _asm goto Microchip_InterruptHook _endasm }

#pragma code AppLowISRVector
void Microchip_AppLowISRVector(void)
{ _asm goto Microchip_InterruptHandler _endasm }

#pragma code

/*******************************************************************
 * This function is called from the USB stack to notify a user
 * application that a USB event occured.  This callback is in
 * interrupt context when the USB_INTERRUPT option is selected.
 *******************************************************************/
#if MICROCHIP_USE_USB
const ROM InquiryResponse inq_resp =
{
	// Standard Response to INQUIRY command stored in ROM
	0x00,       // peripheral device is connected, direct access block device
	0x80,       // removable
	0x04,       // version = 00=> does not conform to any standard, 4=> SPC-2
	0x02,       // response is in format specified by SPC-2
	0x20,       // n-4 = 36-4=32= 0x20
	0x00,       // sccs etc.
	0x00,       // bque=1 and cmdque=0,indicates simple queueing 00 is obsolete,
	            // but as in case of other device, we are just using 00
	0x00,       // 00 obsolete, 0x80 for basic task queueing
	{ 'M','i','c','r','o','c','h','p' },
	// this is the T10 assigned Vendor ID
	{ 'M','a','s','s',' ','S','t','o','r','a','g','e',' ',' ',' ',' ' },
	{ '0','0','0','1' }
};

BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
	switch(event)
	{
	case EVENT_CONFIGURED:
#		if (MSD_DATA_IN_EP == MSD_DATA_OUT_EP)
		USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
#		else
		USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
		USBEnableEndpoint(MSD_DATA_OUT_EP,USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
#		endif
		USBMSDInit();
		break;

	case EVENT_EP0_REQUEST:
		USBCheckMSDRequest();
		break;

	/* case EVENT_SET_DESCRIPTOR: USBCBStdSetDscHandler(); break; */
	/* case EVENT_SOF:            USBCB_SOF_Handler();     break; */
	/* case EVENT_SUSPEND:        USBCBSuspend();          break; */
	/* case EVENT_RESUME:         USBCBWakeFromSuspend();  break; */
	/* case EVENT_BUS_ERROR:      USBCBErrorHandler();     break; */
	/* case EVENT_TRANSFER:       Nop();                   break; */
	}

	return TRUE; 
}
#endif
