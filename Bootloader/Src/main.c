/*********************************************************************
 *
 *   Microchip USB HID Bootloader v1.01 for PIC18F87J50 Family Devices
 *
 *********************************************************************
 * FileName:        main.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 3.22+
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
 * File Version  Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 1.0			 04/09/2008	Started from MCHPFSUSB v1.3 HID Mouse
 *							demo project.  Main modifications include
 *							changes to HID report descriptor, and
 *							replacement of the "user_mouse.c" file
 *							with the contents contained in the
 *							Boot87J50Family.c file.
 * 1.01			 02/17/2009	Minor modifications.  Added firmware only
 *							entry point (goto 0x001C)
 ********************************************************************/

/*********************************************************************
IMPORTANT NOTE: This code is currently configured to work with the
PIC18F87J50 FS USB Plug-In Module board.  It can be readily adapted to
work with other members of the PIC18F87J50 Family of USB microcontrollers 
as well (PIC18F87J50/86J55/86J50/85J50/65J50/66J50/66J55/67J50).

To do so, modify the linker script for the appropriate FILES includes,
and the new memory ranges (assuming a different memory size device), and
click "Configure --> Select Device" and select the proper
microcontroller.  Also double check to verify that the io_cfg.h and
usbcfg.h are properly configured to match your desired application
platform.

It is also recommended to configure the default I/O pin usage in this code.
See the InitializeSystem() function.

This code is meant to be compiled with the C18 compiler version 3.22+
with all optimizations turned on.  If some of the optimizations are not
enabled, the total code size may grow to exceed the 0x000-0xFFF memory
region this code is designed to occupy.  In this case, linker errors will
occur.  If this happens, the vector remapping in the _entry() function 
will have to be modified, as will the application firmware projects that 
may get programmed using this bootloader firmware (to have an entry
point higher than 0x1000).  Additionally, the linker script in this
project will have to be modified to make the BootPage section larger.
*********************************************************************/


//----------------------------------------------------
//Usage tips for this HID USB bootloader firwmare
//----------------------------------------------------

//To enter this bootloader firmware, hold the RB4 I/O pin low at power
//up or after a reset.  Alternatively, application firmware may enter
//the bootloader firmware by clearing the INTCON<GIE> bit and then
//executing an "_asm goto 0x001C _endasm" instruction.

//If a high priority interrupt occurs, the PC will jump to 0x1008
//If a low priority interrupt occurs, the PC will jump to 0x1018

//If RB4 is high at power up/after reset, this code will jump to
//the application firmware, instead of staying in this bootloader firmware.
//The start of the application firmware should be at 0x1000
//In other words, when developing the application firmware which will be
//programmed with this bootloader, place the following in the code, if
//it is a C18 based project:

//extern void _startup (void);    // See c018i.c in your C18 compiler dir
//#pragma code AppFirmwareStartLocation = 0x1000
//void _reset (void)
//{
//    _asm goto _startup _endasm
//}

//Build the application project with a linker script that marks
//the address range 0x000-0xFFF as "PROTECTED".  This is the program
//memory region that this bootloader is currently configured to occupy.

//Although the bootloader can re-program the program memory page that
//contains the configuration bits (the last page of implemented flash)
//it is not always preferrable to do so in case a user attempts to
//program a hex file with configuration bit settings that are not compatible
//with USB operation.  This would prevent further entry into the bootloader.
//If the bootloader will not be used to program the configuration
//words page, the application firmware's linker script should mark
//the entire page as PROTECTED.


/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "typedefs.h"                   
#include "usb.h"                         
#include "io_cfg.h"                     
#include "Boot87J50Family.h"
#include "../Commons/PragmaConfig.h"


/** V A R I A B L E S ********************************************************/
#pragma udata
unsigned int pll_startup_counter;	//Used for software delay while pll is starting up


/** P R I V A T E  P R O T O T Y P E S ***************************************/
void Main(void);
static void InitializeSystem(void);
extern void LongDelay(void);



//--------------------------------------------------------
//Entry point code and vector remapping
//--------------------------------------------------------
//Be careful if modifying the below code.  The below code is absolute address sensitive, since it contains
//the reset and high/low priority interrupt vectors, as well as the bootloader entry point "BootAppStart:" at address 0x001C.
//note: On PIC18, "goto" instructions take 4 byte of program memory, while most other instructions take only 2 bytes each
#pragma code BootResetVector   = 0x0000
void BootResetVector(void)
{ _asm goto Main _endasm }
#pragma code BootHighISRVector = 0x0008
void BootHighISRVector(void)
{ _asm goto 0x1008 _endasm }
#pragma code BootLowISRVector  = 0x0018
void BootLowInterruptVector(void)
{ _asm goto 0x1018 _endasm }
#pragma code

#pragma code AppResetVector    = 0x1000
char AppResetVector[];// (void);
#pragma code


/******************************************************************************
 * Function:        void Main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *****************************************************************************/
void Main(void)
{   
	//NOTE: The c018.o file is not included in the linker script for this project.
	//The C initialization code in the c018.c (comes with C18 compiler in the src directory)
	//file is instead modified and included here manually.  This is done so as to provide
	//a more convenient entry method into the bootloader firmware.  Ordinarilyy the _entry_scn
	//program code section starts at 0x00 and is created by the code of c018.o.  However,
	//the linker will not work if there is more than one section of code trying to occupy 0x00.
	//Therefore, must not use the c018.o code, must instead manually include the useful code
	//here instead.

	_asm

	// EDAB: Immediately enter the application if the reset was manually forced
	// through a stack-overflow reset. Also make sure to reset the status bit
	// so this case won't be triggered again by mistake
	movf STKPTR,0,0
	bcf STKPTR,7,0
	bnn DontStartYet
	goto 0x1000
DontStartYet:

    // Initialize the C stack pointer, and other compiler managed items as normally done in the c018.c file.
    lfsr 1, _stack
    lfsr 2, _stack
    clrf TBLPTRU,0
	_endasm
	
	Boot_SetShortTimeout();

	// Don't use the timer if the bootloader was voluntarily entered through
	// the software reset instruction
	if(!RCONbits.RI)
		Boot_DisableTimeout();

	// End of the important parts of the C initializer.  This bootloader firmware does not use
	// any C initialized user variables (idata memory sections).  Therefore, the above is all
	// the initialization that is required.

	// Some USB, I/O pins, and other initialization
	InitializeSystem();

	for(;;)
	{
		ClrWdt();

		USBCheckBusStatus();
		USBDriverService();

		if(usb_device_state == CONFIGURED_STATE && !UCONbits.SUSPND)
			ProcessIO();

		Boot_CheckTimeout();
	}
}//end main

/******************************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization routine.
 *                  All required USB initialization routines are called from
 *                  here.
 *
 *                  User application initialization routine should also be
 *                  called from here.                  
 *
 * Note:            None
 *****************************************************************************/
static void InitializeSystem(void)
{
	OSCCON = 0x60;	//Clock switch to primary clock source.  May not have been running
					//from this if the bootloader is called from the application firmware.

	//On the PIC18F87J50 Family of USB microcontrollers, the PLL will not power up and be enabled
	//by default, even if a PLL enabled oscillator configuration is selected (such as HS+PLL).
	//This allows the device to power up at a lower initial operating frequency, which can be
	//advantageous when powered from a source which is not gauranteed to be adequate for 48MHz
	//operation.  On these devices, user firmware needs to manually set the OSCTUNE<PLLEN> bit to
	//power up the PLL.

	#if defined(__18F87J50)||defined(__18F86J55)|| \
    	defined(__18F86J50)||defined(__18F85J50)|| \
    	defined(__18F67J50)||defined(__18F66J55)|| \
    	defined(__18F66J50)||defined(__18F65J50)

    OSCTUNEbits.PLLEN = 1;  //Enable the PLL and wait 2+ms until the PLL locks before enabling USB module
    pll_startup_counter = 600;
    while(pll_startup_counter--)
    {
	    ClrWdt();
	} 
    //Device switches over automatically to PLL output after PLL is locked and ready.

    #else
        #error Double Click this message.  Please make sure the InitializeSystem() function correctly configures your hardware platform.  
		//Also make sure the correct board is selected in usbcfg.h.  If 
		//everything is correct, comment out the above "#error ..." line
		//to suppress the error message.
    #endif

	//USB module may have already been on if the application firmware calls the bootloader
	//without first disabling the USB module.  If this happens, need
	//to temporarily soft-detach from the host, wait a delay (allows cable capacitance
	//to discharge, and to allow host software to recognize detach), then
	//re-enable the USB module, so the host knows to re-enumerate the
	//USB device.
	if(UCONbits.USBEN == 1)
	{
		UCONbits.SUSPND = 0;
		UCON = 0;
		LongDelay();
	}	


//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	usbcfg.h file.    
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See io_cfg.h
    #endif

//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2" 
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in usbcfg.h, and that an appropriate I/O pin has been mapped
//	to it in io_cfg.h.    
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;
    #endif
    
    mInitializeUSBDriver();         // See usbdrv.h
    
    UserInit();                     // See user.c & .h

	//Turn off digital input buffers on analog pins to minimize power consumption
	//if the I/O pins happen to be floating in the target application.
	WDTCONbits.ADSHR = 1;	//ANCON registers in shared address space region
	ANCON0 = 0x00;			//All analog, to disable the digital input buffers
	ANCON1 = 0x00;			//All analog, digital input buffers off
	WDTCONbits.ADSHR = 0;
	//Also to minimize sleep current consumption (sleep used in this bootloader
	//firmware during USB Suspend conditions), use REGSLP feature
	WDTCONbits.REGSLP = 1;


}//end InitializeSystem
