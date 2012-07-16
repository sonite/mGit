/******************************************************************************
 * FileName:        PragmaConfig.h                                            *
 * Dependencies:    none                                                      *
 * Processor:       PIC18                                                     *
 * Complier:        MCC18 v3.13                                               *
 * Description:     Pragma Configurationbits for PIC 18F87J50                 *
 *                                                                            *
 * Author:          chris                                                     *
 ******************************************************************************/
#ifndef _PRAGMA_CONFIG_H
#define _PRAGMA_CONFIG_H

#include <p18cxxx.h>

#pragma config DEBUG    = OFF
#ifdef __EXTENDED18__
#	pragma config XINST = ON
#else
#	pragma config XINST = OFF
#endif
#pragma config STVREN   = ON        // Reset on stack overflow/underflow enabled


/** OSCILLATOR **************************************************************************************************************
/* Frequency Multiplier Enable bit, Input 8MHz external through FOSC2 (enabled by fuse FOSC = HSPLL)
/* FOSC sets FOSC2:FOSC0 = 101 and in return SCS1:SCS0 to 00 (OSCCON<1:0>)
/* Fuse PLL = 2 (Prescaler): 8/2 gives 4MHz, this Enable 96MHz/2 = 48MHz through PLLEN, PLLEN is enabled by fuse fuse FOSC.
/* Fuse CPUDIV = OSC1 sets CPDIV1:0 = 11 (No CPU system clock divide) */
/***************************************************************************************************************************/	
#pragma config FOSC     = HSPLL     // High-Speed Crystal/Resonator oscillator with PLL enabled, HSPLL oscillator used by USB
#pragma config PLLDIV   = 2         // Prescaler Selection bits: Divide by 2 (8 MHz oscillator input)
#pragma config CPUDIV 	= OSC1	 	// CPU Clock 48/1 = 48 MHz

/* Two-Speed Start-up Enable */
#pragma config IESO     = ON        // During startup of external Clock, circuit are enabled to use internal clock.
/* Fail-Safe Clock Monitor Enabled */
#pragma config FCMEN    = ON        // If failure of external clock, switch over to internal alowed.

#pragma config WDTEN    = OFF       // Watchdog timer initially Disabled
#pragma config WDTPS    = 8192     	// Watchdog Postscale ~ 30s
#pragma config CP0      = OFF       // Program memory is not code-protected
#pragma config WAIT     = OFF       // Wait States on the external bus are Disabled
#pragma config BW       = 16        // Data Bus: 16-Bit Data Width modes
#pragma config MODE     = MM        // Microcontroller mode (no external memory bus)
#pragma config EASHFT   = ON        // Address shifting enabled – external address bus is shifted to start at 000000h
#pragma config MSSPMSK  = MSK7      // 7-Bit Address Masking mode enable
#pragma config PMPMX    = DEFAULT   // PMP port pins connected to EMB

/** MOTOR CONTROL */
/* To be continued.... Alternate sends internal HIP control to DIO ports*/
#pragma config CCP2MX   = ALTERNATE // DEFAULT // ECCP2/P2A is multiplexed with RE7 in Microcontroller mode or with RB3 in Extended Microcontroller mode
#pragma config ECCPMX	= DEFAULT	//ALTERNATE Default to H?
#endif