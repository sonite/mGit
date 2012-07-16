/******************************************************************************
 *
 *                Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    None
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33/PIC32
 * Compiler:        C18/C30/C32
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
*****************************************************************************/
/********************************************************************
 Change History:
  Rev            Description
  ----           -----------------------
  1.3.4          Added support for PIC18F8722,PIC24FJ256DA210,
                 dsPIC33E & PIC24E microcontrollers.
                 Added macro "SPI_INTERRUPT_FLAG_ASM" for PIC18F
                 microcontrollers to support SD card SPI driver.
********************************************************************/

#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

#include "GenericTypeDefs.h"
#include "../Commons/Microchip/Compiler.h"

// Define your clock speed here

    #define GetSystemClock()		(48000000ul)      // Hz

	#define GetInstructionClock()   (GetSystemClock() / 4)          // Instruction clock freq.
    #define GetPeripheralClock()    GetInstructionClock()                // Peripheral clock freq.
    
// Select your interface type
// This library currently only supports a single physical interface layer at a time


// Description: Macro used to enable the SD-SPI physical layer (SD-SPI.c and .h)
#define USE_SD_INTERFACE_WITH_SPI

// Description: Macro used to enable the CF-PMP physical layer (CF-PMP.c and .h)
//#define USE_CF_INTERFACE_WITH_PMP

// Description: Macro used to enable the CF-Manual physical layer (CF-Bit transaction.c and .h)                                                                
//#define USE_MANUAL_CF_INTERFACE

// Description: Macro used to enable the USB Host physical layer (USB host MSD library)
//#define USE_USB_INTERFACE


/*********************************************************************/
/******************* Pin and Register Definitions ********************/
/*********************************************************************/

/* SD Card definitions: Change these to fit your application when using
   an SD-card-based physical layer                                   */

#ifdef USE_SD_INTERFACE_WITH_SPI
    /* Removed by Finsun - #ifdef PIC18F87J50_PIM */
    
        #define USE_PIC18
        #define USE_SD_INTERFACE_WITH_SPI
    
        #define INPUT_PIN           1
        #define OUTPUT_PIN          0
    
        // Chip Select Signal
        #define SD_CS               LATJbits.LATJ7
        #define SD_CS_TRIS          TRISJbits.TRISJ7
/*        
        // Card detect signal
        #define SD_CD               PORTBbits.RB4
        #define SD_CD_TRIS          TRISBbits.TRISB4
        
        // Write protect signal
        #define SD_WE               PORTAbits.RA4
        #define SD_WE_TRIS          TRISAbits.TRISA4
*/        
        // Registers for the SPI module you want to use

        // Description: The main SPI control register
        #define SPICON1             SSP1CON1
        // Description: The SPI status register
        #define SPISTAT             SSP1STAT
        // Description: The SPI buffer
        #define SPIBUF              SSP1BUF
        // Description: The receive buffer full bit in the SPI status register
        #define SPISTAT_RBF         SSP1STATbits.BF
        // Description: The bitwise define for the SPI control register (i.e. _____bits)
        #define SPICON1bits         SSP1CON1bits
        // Description: The bitwise define for the SPI status register (i.e. _____bits)
        #define SPISTATbits         SSP1STATbits

        // Description: The interrupt flag for the SPI module
        #define SPI_INTERRUPT_FLAG  PIR1bits.SSPIF   
		#define SPI_INTERRUPT_FLAG_ASM  PIR1, 3

        // Description: The enable bit for the SPI module
        #define SPIENABLE           SSPCON1bits.SSPEN
/*
        // Defines for the HPC Explorer board
        #define SPICLOCK            TRISCbits.TRISC3
        #define SPIIN               TRISCbits.TRISC4
        #define SPIOUT              TRISCbits.TRISC5
*/    
        // Latch pins for SCK/SDI/SDO lines
        #define SPICLOCKLAT         LATCbits.LATC3
        #define SPIINLAT            LATCbits.LATC4
        #define SPIOUTLAT           LATCbits.LATC5
    
        // Port pins for SCK/SDI/SDO lines
        #define SPICLOCKPORT        PORTCbits.RC3
        #define SPIINPORT           PORTCbits.RC4
        #define SPIOUTPORT          PORTCbits.RC5
    
		#define SD_LED              PORTJbits.RJ2

        // Will generate an error if the clock speed is too low to interface to the card
        #if (GetSystemClock() < 400000)
            #error System clock speed must exceed 400 kHz
        #endif
/*	#elif defined(__18F8722)
    
        #define USE_PIC18
        #define USE_SD_INTERFACE_WITH_SPI
    
        #define INPUT_PIN           1
        #define OUTPUT_PIN          0
    
        // Chip Select Signal
        #define SD_CS               PORTBbits.RB3
        #define SD_CS_TRIS          TRISBbits.TRISB3
        
        // Card detect signal
        #define SD_CD               PORTBbits.RB4
        #define SD_CD_TRIS          TRISBbits.TRISB4
        
        // Write protect signal
        #define SD_WE               PORTAbits.RA4
        #define SD_WE_TRIS          TRISAbits.TRISA4
        
        // Registers for the SPI module you want to use
        #define SPICON1             SSP1CON1
        #define SPISTAT             SSP1STAT
        #define SPIBUF              SSP1BUF
        #define SPISTAT_RBF         SSP1STATbits.BF
        #define SPICON1bits         SSP1CON1bits
        #define SPISTATbits         SSP1STATbits
    
        #define SPI_INTERRUPT_FLAG  PIR1bits.SSPIF   
    
        // Defines for the HPC Explorer board
        #define SPICLOCK            TRISCbits.TRISC3
        #define SPIIN               TRISCbits.TRISC4
        #define SPIOUT              TRISCbits.TRISC5
    
        // Latch pins for SCK/SDI/SDO lines
        #define SPICLOCKLAT         LATCbits.LATC3
        #define SPIINLAT            LATCbits.LATC4
        #define SPIOUTLAT           LATCbits.LATC5
    
        // Port pins for SCK/SDI/SDO lines
        #define SPICLOCKPORT        PORTCbits.RC3
        #define SPIINPORT           PORTCbits.RC4
        #define SPIOUTPORT          PORTCbits.RC5
    
        #define SPIENABLE           SSPCON1bits.SSPEN

		#define SPI_INTERRUPT_FLAG_ASM  PIR1, 3

        // Will generate an error if the clock speed is too low to interface to the card
        #if (GetSystemClock() < 400000)
            #error System clock speed must exceed 400 kHz
        #endif
	//PM
    #elif defined PIC18F46J50_PIM
        #define USE_PIC18
        #define USE_SD_INTERFACE_WITH_SPI
    
        #define INPUT_PIN           1
        #define OUTPUT_PIN          0

        #define USE_SD_INTERFACE_WITH_SPI
    
        #define TRIS_CARD_DETECT    TRISBbits.TRISB4    // Input
        #define CARD_DETECT         PORTBbits.RB4
        
        #define TRIS_WRITE_DETECT   TRISDbits.TRISD7    // Input
        #define WRITE_DETECT        PORTDbits.RD7
     
        // Chip Select Signal
        #define SD_CS               PORTBbits.RB3
        #define SD_CS_TRIS          TRISBbits.TRISB3
            
        // Card detect signal
        #define SD_CD               PORTBbits.RB4
        #define SD_CD_TRIS          TRISBbits.TRISB4
            
        // Write protect signal
        #define SD_WE               PORTDbits.RD7
        #define SD_WE_TRIS          TRISDbits.TRISD7
    
        // TRIS pins for the SCK/SDI/SDO lines
        #define SPICLOCK            TRISBbits.TRISB1
        #define SPIIN               TRISAbits.TRISA1
        #define SPIOUT              TRISAbits.TRISA5
    
        // Latch pins for SCK/SDI/SDO lines
        #define SPICLOCKLAT         LATBbits.LATB1
        #define SPIINLAT            LATAbits.LATA1
        #define SPIOUTLAT           LATAbits.LATA5
    
        // Port pins for SCK/SDI/SDO lines
        #define SPICLOCKPORT        PORTBbits.RB1
        #define SPIINPORT           PORTAbits.RA1
        #define SPIOUTPORT          PORTAbits.RA5
    
        // Registers for the SPI module you want to use
        #define SPICON1             SSP2CON1
        #define SPISTAT             SSP2STAT
        #define SPIBUF              SSP2BUF
        #define SPISTAT_RBF         SSP2STATbits.BF
        #define SPICON1bits         SSP2CON1bits
        #define SPISTATbits         SSP2STATbits
    
        #define SPI_INTERRUPT_FLAG  PIR3bits.SSP2IF 
        #define SPIENABLE           SSP2CON1bits.SSPEN
    
		#define SPI_INTERRUPT_FLAG_ASM  PIR1, 3
        
        // Will generate an error if the clock speed is too low to interface to the card
        #if (GetSystemClock() < 400000)
            #error System clock speed must exceed 400 kHz
        #endif

    #elif defined __PIC24F__

		#if defined (__PIC24FJ128GA010__)

	        // Description: SD-SPI Chip Select Output bit
	        #define SD_CS				LATBbits.LATB1
	        // Description: SD-SPI Chip Select TRIS bit
	        #define SD_CS_TRIS			TRISBbits.TRISB1
	        
	        // Description: SD-SPI Card Detect Input bit
	        #define SD_CD               PORTFbits.RF0
	        // Description: SD-SPI Card Detect TRIS bit
	        #define SD_CD_TRIS          TRISFbits.TRISF0
	        
	        // Description: SD-SPI Write Protect Check Input bit
	        #define SD_WE               PORTFbits.RF1
	        // Description: SD-SPI Write Protect Check TRIS bit
	        #define SD_WE_TRIS          TRISFbits.TRISF1
	        
	        // Registers for the SPI module you want to use

	        // Description: The main SPI control register
	        #define SPICON1             SPI1CON1
	        // Description: The SPI status register
	        #define SPISTAT             SPI1STAT
	        // Description: The SPI Buffer
	        #define SPIBUF              SPI1BUF
	        // Description: The receive buffer full bit in the SPI status register
	        #define SPISTAT_RBF         SPI1STATbits.SPIRBF
	        // Description: The bitwise define for the SPI control register (i.e. _____bits)
	        #define SPICON1bits         SPI1CON1bits
	        // Description: The bitwise define for the SPI status register (i.e. _____bits)
	        #define SPISTATbits         SPI1STATbits
	        // Description: The enable bit for the SPI module
	        #define SPIENABLE           SPISTATbits.SPIEN

	        // Tris pins for SCK/SDI/SDO lines

	        // Description: The TRIS bit for the SCK pin
	        #define SPICLOCK            TRISFbits.TRISF6
	        // Description: The TRIS bit for the SDI pin
	        #define SPIIN               TRISFbits.TRISF7
	        // Description: The TRIS bit for the SDO pin
	        #define SPIOUT              TRISFbits.TRISF8

		#elif defined (__PIC24FJ256GB110__)

	        // Description: SD-SPI Chip Select Output bit
	        #define SD_CS				LATBbits.LATB1
	        // Description: SD-SPI Chip Select TRIS bit
	        #define SD_CS_TRIS			TRISBbits.TRISB1
	        
	        // Description: SD-SPI Card Detect Input bit
	        #define SD_CD               PORTFbits.RF0
	        // Description: SD-SPI Card Detect TRIS bit
	        #define SD_CD_TRIS          TRISFbits.TRISF0
	        
	        // Description: SD-SPI Write Protect Check Input bit
	        #define SD_WE               PORTFbits.RF1
	        // Description: SD-SPI Write Protect Check TRIS bit
	        #define SD_WE_TRIS          TRISFbits.TRISF1
	        
	        // Registers for the SPI module you want to use

	        // Description: The main SPI control register
	        #define SPICON1             SPI1CON1
	        // Description: The SPI status register
	        #define SPISTAT             SPI1STAT
	        // Description: The SPI Buffer
	        #define SPIBUF              SPI1BUF
	        // Description: The receive buffer full bit in the SPI status register
	        #define SPISTAT_RBF         SPI1STATbits.SPIRBF
	        // Description: The bitwise define for the SPI control register (i.e. _____bits)
	        #define SPICON1bits         SPI1CON1bits
	        // Description: The bitwise define for the SPI status register (i.e. _____bits)
	        #define SPISTATbits         SPI1STATbits
	        // Description: The enable bit for the SPI module
	        #define SPIENABLE           SPISTATbits.SPIEN

	        // Tris pins for SCK/SDI/SDO lines

	        // Description: The TRIS bit for the SCK pin
	        #define SPICLOCK            TRISBbits.TRISB0
	        // Description: The TRIS bit for the SDI pin
	        #define SPIIN               TRISDbits.TRISD2
	        // Description: The TRIS bit for the SDO pin
	        #define SPIOUT              TRISFbits.TRISF8

		#elif defined(__PIC24FJ256DA210__)

			// Description: SD-SPI Chip Select Output bit
			#define SD_CS   PORTGbits.RG6

			// Description: SD-SPI Chip Select TRIS bit
			#define SD_CS_TRIS  TRISGbits.TRISG6

			// Description: SD-SPI Card Detect Input bit
			#define SD_CD   PORTAbits.RA15

			// Description: SD-SPI Card Detect TRIS bit
			#define SD_CD_TRIS  TRISAbits.TRISA15

			// Description: SD-SPI Write Protect Check Input bit
			#define SD_WE   PORTCbits.RC13   //SD_WE   PORTAbits.RA7

			// Description: SD-SPI Write Protect Check TRIS bit
			#define SD_WE_TRIS  TRISCbits.TRISC13  //SD_WE_TRIS  TRISAbits.TRISA7

			// Registers for the SPI module you want to use
			// Description: The main SPI control register
			#define SPICON1 SPI1CON1

			// Description: The SPI status register
			#define SPISTAT SPI1STAT

			// Description: The SPI Buffer
			#define SPIBUF  SPI1BUF

			// Description: The receive buffer full bit in the SPI status register
			#define SPISTAT_RBF SPI1STATbits.SPIRBF

			// Description: The bitwise define for the SPI control register (i.e. _____bits)
			#define SPICON1bits SPI1CON1bits

			// Description: The bitwise define for the SPI status register (i.e. _____bits)
			#define SPISTATbits SPI1STATbits

			// Description: The enable bit for the SPI module
			#define SPIENABLE   SPISTATbits.SPIEN

    	    #define SPICON2  SPI2CON2

			// Tris pins for SCK/SDI/SDO lines

			// Description: The TRIS bit for the SCK pin
			#define SPICLOCK    TRISDbits.TRISD8
			// Description: The TRIS bit for the SDI pin
			#define SPIIN       TRISBbits.TRISB0
			// Description: The TRIS bit for the SDO pin
			#define SPIOUT      TRISBbits.TRISB1

		#endif

        // Will generate an error if the clock speed is too low to interface to the card
        #if (GetSystemClock() < 100000)
            #error Clock speed must exceed 100 kHz
        #endif    

    #elif defined (__dsPIC33E__) || defined (__PIC24E__)

            // Description: SD-SPI Chip Select Output bit
            #define SD_CS               LATBbits.LATB9
            // Description: SD-SPI Chip Select TRIS bit
            #define SD_CS_TRIS          TRISBbits.TRISB9

		    // Description: SD-SPI Analog/Digital Select ANSEL bit
            #define SD_CS_ANSEL			ANSELBbits.ANSB9
            
            // Description: SD-SPI Card Detect Input bit
            #define SD_CD               PORTGbits.RG0
            // Description: SD-SPI Card Detect TRIS bit
            #define SD_CD_TRIS          TRISGbits.TRISG0

            // Description: SD-SPI Write Protect Check Input bit
            #define SD_WE               PORTGbits.RG1
            // Description: SD-SPI Write Protect Check TRIS bit
            #define SD_WE_TRIS          TRISGbits.TRISG1

		    // Description: SD-SPI Analog/Digital Select ANSEL bit
            #define SD_SCK_ANSEL	ANSELGbits.ANSG6
            #define SD_SDI_ANSEL	ANSELGbits.ANSG7
            #define SD_SDO_ANSEL	ANSELGbits.ANSG8
            
            // Description: The main SPI control register
            #define SPICON1             SPI2CON1
            // Description: The SPI status register
            #define SPISTAT             SPI2STAT
            // Description: The SPI Buffer
            #define SPIBUF              SPI2BUF
            // Description: The receive buffer full bit in the SPI status register
            #define SPISTAT_RBF         SPI2STATbits.SPIRBF
            // Description: The bitwise define for the SPI control register (i.e. _____bits)
            #define SPICON1bits         SPI2CON1bits
            // Description: The bitwise define for the SPI status register (i.e. _____bits)
            #define SPISTATbits         SPI2STATbits
            // Description: The enable bit for the SPI module
            #define SPIENABLE           SPI2STATbits.SPIEN
            // Description: The definition for the SPI baud rate generator register
            #define SPIBRG			    SPI2BRG

            // Tris pins for SCK/SDI/SDO lines

            // Description: The TRIS bit for the SCK pin
            #define SPICLOCK            TRISGbits.TRISG6
            // Description: The TRIS bit for the SDI pin
            #define SPIIN               TRISGbits.TRISG7
            // Description: The TRIS bit for the SDO pin
            #define SPIOUT              TRISGbits.TRISG8

    #elif defined (__PIC32MX__)
        // Registers for the SPI module you want to use
        #define MDD_USE_SPI_1
        //#define MDD_USE_SPI_2

		//SPI Configuration
		#define SPI_START_CFG_1     (PRI_PRESCAL_64_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON)
        #define SPI_START_CFG_2     (SPI_ENABLE)

        // Define the SPI frequency
        #define SPI_FREQUENCY			(20000000)
    
        #if defined MDD_USE_SPI_1
            // Description: SD-SPI Chip Select Output bit
        	#define SD_CS               LATBbits.LATB1
        	// Description: SD-SPI Chip Select TRIS bit
            #define SD_CS_TRIS          TRISBbits.TRISB1
            
            // Description: SD-SPI Card Detect Input bit
            #define SD_CD               PORTFbits.RF0
            // Description: SD-SPI Card Detect TRIS bit
            #define SD_CD_TRIS          TRISFbits.TRISF0

            // Description: SD-SPI Write Protect Check Input bit
            #define SD_WE               PORTFbits.RF1
            // Description: SD-SPI Write Protect Check TRIS bit
            #define SD_WE_TRIS          TRISFbits.TRISF1
                   
            // Description: The main SPI control register
            #define SPICON1             SPI1CON
            // Description: The SPI status register
            #define SPISTAT             SPI1STAT
            // Description: The SPI Buffer
            #define SPIBUF              SPI1BUF
            // Description: The receive buffer full bit in the SPI status register
            #define SPISTAT_RBF         SPI1STATbits.SPIRBF
            // Description: The bitwise define for the SPI control register (i.e. _____bits)
            #define SPICON1bits         SPI1CONbits
            // Description: The bitwise define for the SPI status register (i.e. _____bits)
            #define SPISTATbits         SPI1STATbits
            // Description: The enable bit for the SPI module
            #define SPIENABLE           SPICON1bits.ON
            // Description: The definition for the SPI baud rate generator register (PIC32)
            #define SPIBRG			    SPI1BRG

            // Tris pins for SCK/SDI/SDO lines

            // Tris pins for SCK/SDI/SDO lines
			#if defined(__32MX460F512L__)
            	// Description: The TRIS bit for the SCK pin
            	#define SPICLOCK            TRISDbits.TRISD10
            	// Description: The TRIS bit for the SDI pin
            	#define SPIIN               TRISCbits.TRISC4
            	// Description: The TRIS bit for the SDO pin
            	#define SPIOUT              TRISDbits.TRISD0
			#else	// example: PIC32MX360F512L
            	// Description: The TRIS bit for the SCK pin
            	#define SPICLOCK            TRISFbits.TRISF6
            	// Description: The TRIS bit for the SDI pin
            	#define SPIIN               TRISFbits.TRISF7
            	// Description: The TRIS bit for the SDO pin
            	#define SPIOUT              TRISFbits.TRISF8
			#endif

            //SPI library functions
            #define putcSPI             putcSPI1
            #define getcSPI             getcSPI1
            #define OpenSPI(config1, config2)   OpenSPI1(config1, config2)
            
        #elif defined MDD_USE_SPI_2
            // Description: SD-SPI Chip Select Output bit
            #define SD_CS               LATBbits.LATB9
            // Description: SD-SPI Chip Select TRIS bit
            #define SD_CS_TRIS          TRISBbits.TRISB9
            
            // Description: SD-SPI Card Detect Input bit
            #define SD_CD               PORTGbits.RG0
            // Description: SD-SPI Card Detect TRIS bit
            #define SD_CD_TRIS          TRISGbits.TRISG0

            // Description: SD-SPI Write Protect Check Input bit
            #define SD_WE               PORTGbits.RG1
            // Description: SD-SPI Write Protect Check TRIS bit
            #define SD_WE_TRIS          TRISGbits.TRISG1
            
            // Description: The main SPI control register
            #define SPICON1             SPI2CON
            // Description: The SPI status register
            #define SPISTAT             SPI2STAT
            // Description: The SPI Buffer
            #define SPIBUF              SPI2BUF
            // Description: The receive buffer full bit in the SPI status register
            #define SPISTAT_RBF         SPI2STATbits.SPIRBF
            // Description: The bitwise define for the SPI control register (i.e. _____bits)
            #define SPICON1bits         SPI2CONbits
            // Description: The bitwise define for the SPI status register (i.e. _____bits)
            #define SPISTATbits         SPI2STATbits
            // Description: The enable bit for the SPI module
            #define SPIENABLE           SPI2CONbits.ON
            // Description: The definition for the SPI baud rate generator register (PIC32)
            #define SPIBRG			    SPI2BRG

            // Tris pins for SCK/SDI/SDO lines

            // Description: The TRIS bit for the SCK pin
            #define SPICLOCK            TRISGbits.TRISG6
            // Description: The TRIS bit for the SDI pin
            #define SPIIN               TRISGbits.TRISG7
            // Description: The TRIS bit for the SDO pin
            #define SPIOUT              TRISGbits.TRISG8
            //SPI library functions
            #define putcSPI             putcSPI2
            #define getcSPI             getcSPI2
            #define OpenSPI(config1, config2)   OpenSPI2(config1, config2)
        #endif       
        // Will generate an error if the clock speed is too low to interface to the card
        #if (GetSystemClock() < 100000)
            #error Clock speed must exceed 100 kHz
        #endif    
    
    #endif
*/
#endif

/*
#ifdef USE_CF_INTERFACE_WITH_PMP

    /* CompactFlash-PMP card definitions: change these to fit your application when
    using the PMP module to interface with CF cards                                */
 /*   
    #ifdef __18CXX
        #error The PIC18 architecture does not currently support PMP interface to CF cards
    #elif defined __dsPIC30F__
    
        // Sample dsPIC30 defines
        
        // Description: The output latch for the CF Reset signal
        #define CF_PMP_RST		    _RD0
        // Description: The TRIS bit for the CF Reset signal
        #define CF_PMP_RESETDIR	    _TRISD0
        // Description: The input port for the CF Ready signal
        #define CF_PMP_RDY	 	    _RD12
        // Description: The TRIS bit for the CF Ready signal
        #define CF_PMP_READYDIR	    _TRISD12
        // Description: The input port for the CF card detect signal
        #define CF_PMP_CD1		    _RC4
        // Description: The TRIS bit for the CF card detect signal
        #define CF_PMP_CD1DIR	    _TRISC4
    
    #elif defined (__dsPIC33F__) || defined (__PIC24H__) || defined (__dsPIC33E__) || defined (__PIC24E__)
    
        // Sample dsPIC33 defines

        // Description: The output latch for the CF Reset signal
        #define CF_PMP_RST		    _RD0
        // Description: The TRIS bit for the CF Reset signal
        #define CF_PMP_RESETDIR	    _TRISD0
        // Description: The input port for the CF Ready signal
        #define CF_PMP_RDY	 	    _RD12
        // Description: The TRIS bit for the CF Ready signal
        #define CF_PMP_READYDIR	    _TRISD12
        // Description: The input port for the CF card detect signal
        #define CF_PMP_CD1		    _RC4
        // Description: The TRIS bit for the CF card detect signal
        #define CF_PMP_CD1DIR	    _TRISC4
    
    #elif defined __PIC24F__
    
        // Default case for PIC24F

        // Description: The output latch for the CF Reset signal
        #define CF_PMP_RST		    LATDbits.LATD0
        // Description: The TRIS bit for the CF Reset signal
        #define CF_PMP_RESETDIR	    TRISDbits.TRISD0
        // Description: The input port for the CF Ready signal
        #define CF_PMP_RDY	 	    PORTDbits.RD12
        // Description: The TRIS bit for the CF Ready signal
        #define CF_PMP_READYDIR	    TRISDbits.TRISD12
        // Description: The input port for the CF card detect signal
        #define CF_PMP_CD1		    PORTCbits.RC4
        // Description: The TRIS bit for the CF card detect signal
        #define CF_PMP_CD1DIR	    TRISCbits.TRISC4
    
    #endif
    
    // Description: Defines the PMP data bus direction register
    #define MDD_CFPMP_DATADIR       TRISE
#endif


#ifdef USE_MANUAL_CF_INTERFACE
    // Use these definitions with CF-Bit transaction.c and .h
    // This will manually perform parallel port transactions

    #ifdef __18CXX
    
        // Address lines

        // Description: The CF address bus output latch register (for PIC18)
        #define ADDBL                   LATA
        // Description: The CF address bus TRIS register (for PIC18)
        #define ADDDIR                  TRISA
    	
        // Data bus

        // Description: The Manual CF data bus port register
        #define MDD_CFBT_DATABIN        PORTD
        // Description: The Manual CF data bus output latch register
        #define MDD_CFBT_DATABOUT       LATD
        // Description: The Manual CF data bus TRIS register
        #define MDD_CFBT_DATADIR        TRISD

        // control bus lines

        // Description: The CF card chip select output latch bit
        #define CF_CE                   LATEbits.LATE1
        // Description: The CF card chip select TRIS bit
        #define CF_CEDIR                TRISEbits.TRISE1
        // Description: The CF card output enable strobe latch bit
        #define CF_OE                   LATAbits.LATA5
        // Description: The CF card output enable strobe TRIS bit
        #define CF_OEDIR                TRISAbits.TRISA5
        // Description: The CF card write enable strobe latch bit
        #define CF_WE                   LATAbits.LATA4
        // Description: The CF card write enable strobe TRIS bit
        #define CF_WEDIR                TRISAbits.TRISA4
        // Description: The CF card reset signal latch bit
        #define CF_BT_RST               LATEbits.LATE0
        // Description: The CF card reset signal TRIS bit
        #define CF_BT_RESETDIR          TRISEbits.TRISE0
        // Description: The CF card ready signal port bit
        #define CF_BT_RDY               PORTEbits.RE2
        // Description: The CF card ready signal TRIS bit
        #define CF_BT_READYDIR          TRISEbits.TRISE2
        // Description: The CF card detect signal port bit
        #define CF_BT_CD1               PORTCbits.RC2
        // Description: The CF card detect signal TRIS bit
        #define CF_BT_CD1DIR            TRISCbits.TRISC2
    
    #elif defined __dsPIC30F__
    
        // Address lines

         // Description: The CF address bus bit 0 output latch definition (for PIC24/30/33/32)
        #define ADDR0                   _LATB15
        // Description: The CF address bus bit 1 output latch definition (for PIC24/30/33/32)
        #define	ADDR1                   _LATB14
        // Description: The CF address bus bit 2 output latch definition (for PIC24/30/33/32)
        #define ADDR2                   _LATG9
        // Description: The CF address bus bit 3 output latch definition (for PIC24/30/33/32)
        #define ADDR3                   _LATG8
        // Description: The CF address bus bit 0 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS0                _TRISB15
        // Description: The CF address bus bit 1 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS1                _TRISB14
        // Description: The CF address bus bit 2 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS2                _TRISG9
        // Description: The CF address bus bit 3 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS3                _TRISG8
    	
        // Data bus

        // Description: The Manual CF data bus port register
        #define MDD_CFBT_DATABIN        PORTE
        // Description: The Manual CF data bus output latch register
        #define MDD_CFBT_DATABOUT       PORTE
        // Description: The Manual CF data bus TRIS register
        #define MDD_CFBT_DATADIR        TRISE
    	
        // control bus lines

        // Description: The CF card chip select output latch bit
        #define CF_CE                   _RD11
        // Description: The CF card chip select TRIS bit
        #define CF_CEDIR                _TRISD11
        // Description: The CF card output enable strobe latch bit
        #define CF_OE                   _RD5
        // Description: The CF card output enable strobe TRIS bit
        #define CF_OEDIR                _TRISD5	
        // Description: The CF card write enable strobe latch bit
        #define CF_WE                   _RD4
        // Description: The CF card write enable strobe TRIS bit
        #define CF_WEDIR                _TRISD4
        // Description: The CF card reset signal latch bit
        #define CF_BT_RST               _RD0
        // Description: The CF card reset signal TRIS bit
        #define CF_BT_RESETDIR          _TRISD0
        // Description: The CF card ready signal port bit
        #define CF_BT_RDY               _RD12
        // Description: The CF card ready signal TRIS bit
        #define CF_BT_READYDIR          _TRISD12
        // Description: The CF card detect signal port bit
        #define CF_BT_CD1               _RC4
        // Description: The CF card detect signal TRIS bit
        #define CF_BT_CD1DIR            _TRISC4

    #elif defined (__dsPIC33F__) || defined (__PIC24H__) || defined (__dsPIC33E__) || defined (__PIC24E__)

        // Address lines

        // Description: The CF address bus bit 0 output latch definition (for PIC24/30/33/32)
        #define ADDR0                   _LATB15
        // Description: The CF address bus bit 1 output latch definition (for PIC24/30/33/32)
        #define	ADDR1                   _LATB14
        // Description: The CF address bus bit 2 output latch definition (for PIC24/30/33/32)
        #define ADDR2                   _LATG9
        // Description: The CF address bus bit 3 output latch definition (for PIC24/30/33/32)
        #define ADDR3                   _LATG8
        // Description: The CF address bus bit 0 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS0                _TRISB15
        // Description: The CF address bus bit 1 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS1                _TRISB14
        // Description: The CF address bus bit 2 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS2                _TRISG9
        // Description: The CF address bus bit 3 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS3                _TRISG8
    	
        // Data bus

        // Description: The Manual CF data bus port register
        #define MDD_CFBT_DATABIN        PORTE
        // Description: The Manual CF data bus output latch register
        #define MDD_CFBT_DATABOUT       PORTE
        // Description: The Manual CF data bus TRIS register
        #define MDD_CFBT_DATADIR        TRISE
    	
        // control bus lines

        // Description: The CF card chip select output latch bit
        #define CF_CE                   _RD11
        // Description: The CF card chip select TRIS bit
        #define CF_CEDIR                _TRISD11
        // Description: The CF card output enable strobe latch bit
        #define CF_OE                   _RD5
        // Description: The CF card output enable strobe TRIS bit
        #define CF_OEDIR                _TRISD5	
        // Description: The CF card write enable strobe latch bit
        #define CF_WE                   _RD4
        // Description: The CF card write enable strobe TRIS bit
        #define CF_WEDIR                _TRISD4
        // Description: The CF card reset signal latch bit
        #define CF_BT_RST               _RD0
        // Description: The CF card reset signal TRIS bit
        #define CF_BT_RESETDIR          _TRISD0
        // Description: The CF card ready signal port bit
        #define CF_BT_RDY               _RD12
        // Description: The CF card ready signal TRIS bit
        #define CF_BT_READYDIR          _TRISD12
        // Description: The CF card detect signal port bit
        #define CF_BT_CD1               _RC4
        // Description: The CF card detect signal TRIS bit
        #define CF_BT_CD1DIR            _TRISC4
    
    #elif defined __PIC24F__
    
        // Address lines

        // Description: The CF address bus bit 0 output latch definition (for PIC24/30/33/32)
        #define ADDR0                   LATBbits.LATB15
        // Description: The CF address bus bit 1 output latch definition (for PIC24/30/33/32)
        #define	ADDR1                   LATBbits.LATB14
        // Description: The CF address bus bit 2 output latch definition (for PIC24/30/33/32)
        #define ADDR2                   LATGbits.LATG9
        // Description: The CF address bus bit 3 output latch definition (for PIC24/30/33/32)
        #define ADDR3                   LATGbits.LATG8
        // Description: The CF address bus bit 0 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS0                TRISBbits.TRISB15
        // Description: The CF address bus bit 1 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS1                TRISBbits.TRISB14
        // Description: The CF address bus bit 2 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS2                TRISGbits.TRISG9
        // Description: The CF address bus bit 3 TRIS definition (for PIC24/30/33/32)
        #define ADRTRIS3                TRISGbits.TRISG8
    	
        // Data bus

        // Description: The Manual CF data bus port register
        #define MDD_CFBT_DATABIN        PORTE
        // Description: The Manual CF data bus output latch register
        #define MDD_CFBT_DATABOUT       PORTE
        // Description: The Manual CF data bus TRIS register
        #define MDD_CFBT_DATADIR        TRISE
    	
        // control bus lines

        // Description: The CF card chip select output latch bit
        #define CF_CE                   PORTDbits.RD11
        // Description: The CF card chip select TRIS bit
        #define CF_CEDIR                TRISDbits.TRISD11
        // Description: The CF card output enable strobe latch bit
        #define CF_OE                   PORTDbits.RD5
        // Description: The CF card output enable strobe TRIS bit
        #define CF_OEDIR                TRISDbits.TRISD5	
        // Description: The CF card write enable strobe latch bit
        #define CF_WE                   PORTDbits.RD4
        // Description: The CF card write enable strobe TRIS bit
        #define CF_WEDIR                TRISDbits.TRISD4
        // Description: The CF card reset signal latch bit
        #define CF_BT_RST               PORTDbits.RD0
        // Description: The CF card reset signal TRIS bit
        #define CF_BT_RESETDIR          TRISDbits.TRISD0
        // Description: The CF card ready signal port bit
        #define CF_BT_RDY               PORTDbits.RD12
        // Description: The CF card ready signal TRIS bit
        #define CF_BT_READYDIR          TRISDbits.TRISD12
        // Description: The CF card detect signal port bit
        #define CF_BT_CD1               PORTCbits.RC4
        // Description: The CF card detect signal TRIS bit
        #define CF_BT_CD1DIR            TRISCbits.TRISC4
    #endif
#endif
*/
// #include <uart2.h>

/* Hardware Profile TCP/IP STack
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		10/03/06	Original, copied from Compiler.h
 * Ken Hesky            07/01/08    Added ZG2100-specific features
 ********************************************************************/

// Hardware mappings
/* #if defined(EXPLORER_16)
// Explorer 16 + PIC24FJ128GA010

	#define LED0_TRIS			(TRISAbits.TRISA0)	// Ref D3
	#define LED0_IO				(LATAbits.LATA0)	
	#define LED1_TRIS			(TRISAbits.TRISA1)	// Ref D4
	#define LED1_IO				(LATAbits.LATA1)
	#define LED2_TRIS			(TRISAbits.TRISA2)	// Ref D5
	#define LED2_IO				(LATAbits.LATA2)
	#define LED3_TRIS			(TRISAbits.TRISA3)	// Ref D6
	#define LED3_IO				(LATAbits.LATA3)
	#define LED4_TRIS			(TRISAbits.TRISA4)	// Ref D7
	#define LED4_IO				(LATAbits.LATA4)
	#define LED5_TRIS			(TRISAbits.TRISA5)	// Ref D8
	#define LED5_IO				(LATAbits.LATA5)
	#define LED6_TRIS			(TRISAbits.TRISA6)	// Ref D9
	#define LED6_IO				(LATAbits.LATA6)
	#define LED7_TRIS			(LATAbits.LATA7)	// Ref D10;  Note: This is multiplexed with BUTTON1, so this LED can't be used.  However, it will glow very dimmly due to a weak pull up resistor.
	#define LED7_IO				(LATAbits.LATA7)
	#define LED_GET()			(*((volatile unsigned char*)(&LATA)))
	#define LED_PUT(a)			(*((volatile unsigned char*)(&LATA)) = (a))


	#define BUTTON0_TRIS		(TRISDbits.TRISD13)	// Ref S4
	#define	BUTTON0_IO			(PORTDbits.RD13)
	#define BUTTON1_TRIS		(TRISAbits.TRISA7)	// Ref S5;  Note: This is multiplexed with LED7
	#define	BUTTON1_IO			(PORTAbits.RA7)
	#define BUTTON2_TRIS		(TRISDbits.TRISD7)	// Ref S6
	#define	BUTTON2_IO			(PORTDbits.RD7)
	#define BUTTON3_TRIS		(TRISDbits.TRISD6)	// Ref S3
	#define	BUTTON3_IO			(PORTDbits.RD6)

	#define UARTTX_TRIS			(TRISFbits.TRISF5)
	#define UARTTX_IO			(PORTFbits.RF5)
	#define UARTRX_TRIS			(TRISFbits.TRISF4)
	#define UARTRX_IO			(PORTFbits.RF4)
*/
	// ENC28J60 I/O pins
	#define ENC_CS_TRIS			(TRISDbits.TRISD7) // Comment this line out if you are using the ENC424J600/624J600, MRF24WB0M, or other network controller.
	#define ENC_CS_IO			(LATDbits.LATD7)
	#define ENC_RST_TRIS		(TRISDbits.TRISD6)
	#define ENC_RST_IO			(LATDbits.LATD6)
	// SPI SCK, SDI, SDO pins are automatically controlled by the 
	// PIC24/dsPIC/PIC32 SPI module 
/*	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
		#define ENC_SPI_IF			(IFS0bits.SPI1IF)
		#define ENC_SSPBUF			(SPI1BUF)
		#define ENC_SPISTAT			(SPI1STAT)
		#define ENC_SPISTATbits		(SPI1STATbits)
		#define ENC_SPICON1			(SPI1CON1)
		#define ENC_SPICON1bits		(SPI1CON1bits)
		#define ENC_SPICON2			(SPI1CON2)
	#endif
*/
	#define ENC_SCK_TRIS		(TRISCbits.TRISC3)
	#define ENC_SDI_TRIS		(TRISCbits.TRISC4)
	#define ENC_SDO_TRIS		(TRISCbits.TRISC5)
	#define ENC_SPI_IF			(PIR1bits.SSPIF)
	#define ENC_SSPBUF			(SSP1BUF)
	#define ENC_SPISTAT			(SSP1STAT)
	#define ENC_SPISTATbits		(SSP1STATbits)
	#define ENC_SPICON1			(SSP1CON1)
	#define ENC_SPICON1bits		(SSP1CON1bits)
	#define ENC_SPICON2			(SSP1CON2)

	// Debugging LEDs
	#define DEBUG_LED1          (LATAbits.LATA4)
	#define DEBUG_LED2          (LATAbits.LATA5)
	#define DEBUG_LED3          (LATCbits.LATC0)
	#define DEBUG_LED4          (LATCbits.LATC1)
	#define DEBUG_LED5          (LATJbits.LATJ2)
	
	#define MAX_SPI_IF				(PIR1bits.SSPIF)
	#define MAX_WaitForDataByte()   {while(!MAX_SPI_IF); MAX_SPI_IF = 0;}
	#define MAX_ClearSPIDoneFlag()  {ENC_SPI_IF = 0;}
	
	// Register name fix up for certain processors
	#define SPBRGH				SPBRGH1
	#if defined(__18F87J50) || defined(_18F87J50) || defined(__18F87J11) || defined(_18F87J11)
	#	define ADCON2		ADCON1
	#endif
/*
	// Select which UART the STACK_USE_UART and STACK_USE_UART2TCP_BRIDGE 
	// options will use.  You can change these to U1BRG, U1MODE, etc. if you 
	// want to use the UART1 module instead of UART2.
	#define UBRG					U2BRG
	#define UMODE					U2MODE
	#define USTA					U2STA
	#define BusyUART()				BusyUART2()
	#define CloseUART()				CloseUART2()
	#define ConfigIntUART(a)		ConfigIntUART2(a)
	#define DataRdyUART()			DataRdyUART2()
	#define OpenUART(a,b,c)			OpenUART2(a,b,c)
	#define ReadUART()				ReadUART2()
	#define WriteUART(a)			WriteUART2(a)
	#define getsUART(a,b,c)			getsUART2(a,b,c)
	#define putsUART(a)				putsUART2((unsigned int*)a)
	#define getcUART()				getcUART2()
	#define putcUART(a)				do{while(BusyUART()); WriteUART(a); while(BusyUART()); }while(0)
	#define putrsUART(a)			putrsUART2(a)

		// LCD Module I/O pins.  NOTE: On the Explorer 16, the LCD is wired to the 
	// same PMP lines required to communicate with an ENCX24J600 in parallel 
	// mode.  Since the LCD does not have a chip select wire, if you are using 
	// the ENC424J600/624J600 in parallel mode, the LCD cannot be used.
	#if !defined(ENC100_INTERFACE_MODE) || (ENC100_INTERFACE_MODE == 0)	// SPI only
		#define LCD_DATA_TRIS		(*((volatile BYTE*)&TRISE))
		#define LCD_DATA_IO			(*((volatile BYTE*)&LATE))
		#define LCD_RD_WR_TRIS		(TRISDbits.TRISD5)
		#define LCD_RD_WR_IO		(LATDbits.LATD5)
		#define LCD_RS_TRIS			(TRISBbits.TRISB15)
		#define LCD_RS_IO			(LATBbits.LATB15)
		#define LCD_E_TRIS			(TRISDbits.TRISD4)
		#define LCD_E_IO			(LATDbits.LATD4)
	#endif


#endif */
   

    /*******************************************************************/
    /******** USB stack hardware selection options *********************/
    /*******************************************************************/
    //This section is the set of definitions required by the MCHPFSUSB
    //  framework.  These definitions tell the firmware what mode it is
    //  running in, and where it can find the results to some information
    //  that the stack needs.
    //These definitions are required by every application developed with
    //  this revision of the MCHPFSUSB framework.  Please review each
    //  option carefully and determine which options are desired/required
    //  for your application.

    //#define USE_SELF_POWER_SENSE_IO
    #define tris_self_power     TRISAbits.TRISA2    // Input
    #define self_power          1

    //#define USE_USB_BUS_SENSE_IO
    #define tris_usb_bus_sense  TRISBbits.TRISB5    // Input
    #define USB_BUS_SENSE       1 // This is also in usb_device.c far up
 
    //Uncomment this to make the output HEX of this project 
    //   to be able to be bootloaded using the HID bootloader
	//#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER		


 /*********************************************************************
 *
 *	Hardware specific definitions for:
 *    - PIC18 Explorer
 *    - PIC18F8722 and other PIC18s
 *    - Ethernet PICtail (ENC28J60) and (ENC624J600) from HWP PIC18EX_ENC*
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    Compiler.h
 * Processor:       PIC18
 * Compiler:        Microchip C18 v3.36 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		09/16/2010	Regenerated for specific boards
 ********************************************************************/
/* TODO implement this instead of above when upgraded Stack
#if defined(CFG_INCLUDE_PIC18MELACS_ENC28)
	//// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
	//#define ENC100_INTERFACE_MODE			0
	//
	//// ENC100_MDIX, ENC100_POR, and ENC100_INT are all optional.  Simply leave 
	//// them commented out if you don't have such a hardware feature on your 
	//// board.
	//#define ENC100_MDIX_TRIS				(TRISBbits.TRISB4)
	//#define ENC100_MDIX_IO					(LATBbits.LATB4)
	////#define ENC100_POR_TRIS					(TRISBbits.TRISB5)
	////#define ENC100_POR_IO					(LATBbits.LATB5)
	////#define ENC100_INT_TRIS					(TRISBbits.TRISB2)
	////#define ENC100_INT_IO					(PORTBbits.RB2)
	//
	//// ENC424J600/624J600 SPI pinout
	//#define ENC100_CS_TRIS					(TRISBbits.TRISB3)
	//#define ENC100_CS_IO					(LATBbits.LATB3)
	//#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISCbits.TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
	//#define ENC100_SO_WR_B0SEL_EN_IO		(PORTCbits.RC4)
	//#define ENC100_SI_RD_RW_TRIS			(TRISCbits.TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
	//#define ENC100_SI_RD_RW_IO				(LATCbits.LATC5)
	//#define ENC100_SCK_AL_TRIS				(TRISCbits.TRISC3)
	//#define ENC100_SCK_AL_IO				(PORTCbits.RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
	//
	//// ENC424J600/624J600 SPI SFR register selection (controls which SPI 
	//// peripheral to use on PICs with multiple SPI peripherals).
	////#define ENC100_ISR_ENABLE		(INTCON3bits.INT2IE)
	////#define ENC100_ISR_FLAG			(INTCON3bits.INT2IF)
	////#define ENC100_ISR_POLARITY		(INTCON2bits.INTEDG2)
	////#define ENC100_ISR_PRIORITY		(INTCON3bits.INT2IP)
	//#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
	//#define ENC100_SPI_IF			(PIR1bits.SSPIF)
	//#define ENC100_SSPBUF			(SSP1BUF)
	//#define ENC100_SPISTAT			(SSP1STAT)
	//#define ENC100_SPISTATbits		(SSP1STATbits)
	//#define ENC100_SPICON1			(SSP1CON1)
	//#define ENC100_SPICON1bits		(SSP1CON1bits)
	//#define ENC100_SPICON2			(SSP1CON2)

	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISDbits.TRISD6)
	#define ENC_RST_IO			(LATDbits.LATD6)
	#define ENC_CS_TRIS			(TRISDbits.TRISD7)
	#define ENC_CS_IO			(LATDbits.LATD7)
	#define ENC_SCK_TRIS		(TRISCbits.TRISC3)
	#define ENC_SDI_TRIS		(TRISCbits.TRISC4)
	#define ENC_SDO_TRIS		(TRISCbits.TRISC5)
	#define ENC_SPI_IF			(PIR1bits.SSPIF)
	#define ENC_SSPBUF			(SSP1BUF)
	#define ENC_SPISTAT			(SSP1STAT)
	#define ENC_SPISTATbits		(SSP1STATbits)
	#define ENC_SPICON1			(SSP1CON1)
	#define ENC_SPICON1bits		(SSP1CON1bits)
	#define ENC_SPICON2			(SSP1CON2)
	
	// Debugging LEDs
	#define DEBUG_LED1          (LATAbits.LATA4)
	#define DEBUG_LED2          (LATAbits.LATA5)
	#define DEBUG_LED3          (LATCbits.LATC0)
	#define DEBUG_LED4          (LATCbits.LATC1)
	#define DEBUG_LED5          (LATJbits.LATJ2)
	
	#define MAX_SPI_IF				(PIR1bits.SSPIF)
	#define MAX_WaitForDataByte()   {while(!MAX_SPI_IF); MAX_SPI_IF = 0;}
	#define MAX_ClearSPIDoneFlag()  {ENC_SPI_IF = 0;}
	
#elif defined(CFG_INCLUDE_PIC18MELACS_ENC624)
	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
	#define ENC100_INTERFACE_MODE			0

	// ENC100_MDIX, ENC100_POR, and ENC100_INT are all optional.  Simply leave 
	// them commented out if you don't have such a hardware feature on your 
	// board.
	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB4)
	#define ENC100_MDIX_IO					(LATBbits.LATB4)
	//#define ENC100_POR_TRIS					(TRISBbits.TRISB5)
	//#define ENC100_POR_IO					(LATBbits.LATB5)
	//#define ENC100_INT_TRIS					(TRISBbits.TRISB2)
	//#define ENC100_INT_IO					(PORTBbits.RB2)

	// ENC424J600/624J600 SPI pinout
	#define ENC100_CS_TRIS					(TRISBbits.TRISB3)
	#define ENC100_CS_IO					(LATBbits.LATB3)
	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISCbits.TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
	#define ENC100_SO_WR_B0SEL_EN_IO		(PORTCbits.RC4)
	#define ENC100_SI_RD_RW_TRIS			(TRISCbits.TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
	#define ENC100_SI_RD_RW_IO				(LATCbits.LATC5)
	#define ENC100_SCK_AL_TRIS				(TRISCbits.TRISC3)
	#define ENC100_SCK_AL_IO				(PORTCbits.RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.

	// ENC424J600/624J600 SPI SFR register selection (controls which SPI 
	// peripheral to use on PICs with multiple SPI peripherals).
	//#define ENC100_ISR_ENABLE		(INTCON3bits.INT2IE)
	//#define ENC100_ISR_FLAG			(INTCON3bits.INT2IF)
	//#define ENC100_ISR_POLARITY		(INTCON2bits.INTEDG2)
	//#define ENC100_ISR_PRIORITY		(INTCON3bits.INT2IP)
	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
	#define ENC100_SPI_IF			(PIR1bits.SSPIF)
	#define ENC100_SSPBUF			(SSP1BUF)
	#define ENC100_SPISTAT			(SSP1STAT)
	#define ENC100_SPISTATbits		(SSP1STATbits)
	#define ENC100_SPICON1			(SSP1CON1)
	#define ENC100_SPICON1bits		(SSP1CON1bits)
	#define ENC100_SPICON2			(SSP1CON2)

	//// ENC28J60 I/O pins
	//#define ENC_RST_TRIS		(TRISBbits.TRISB5)
	//#define ENC_RST_IO			(LATBbits.LATB5)
	//#define ENC_CS_TRIS			(TRISBbits.TRISB3)
	//#define ENC_CS_IO			(LATBbits.LATB3)
	//#define ENC_SCK_TRIS		(TRISCbits.TRISC3)
	//#define ENC_SDI_TRIS		(TRISCbits.TRISC4)
	//#define ENC_SDO_TRIS		(TRISCbits.TRISC5)
	//#define ENC_SPI_IF			(PIR1bits.SSPIF)
	//#define ENC_SSPBUF			(SSP1BUF)
	//#define ENC_SPISTAT			(SSP1STAT)
	//#define ENC_SPISTATbits		(SSP1STATbits)
	//#define ENC_SPICON1			(SSP1CON1)
	//#define ENC_SPICON1bits		(SSP1CON1bits)
	//#define ENC_SPICON2			(SSP1CON2)

#else
	#warning "No extended HWP .h for Ethernet Stack included."
#endif */

#endif
