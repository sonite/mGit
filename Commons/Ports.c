/******************************************************************************
 * FileName:        Ports.c											      	  *
 * Dependencies:	none													  *
 * Processor:       PIC18													  *
 * Complier:        MCC18 v3.13												  *
 * Description: 	Port Setup at Startup of System							  *
 *                                                                      	  *
 *              		        Author: Finsun Inresol AB                     *
 ******************************************************************************/
//#define VERSION_MELACS6 1

#include "PragmaConfig.h"


/******************************************************************************
 * Port initialization                                                        *
 ******************************************************************************/
void Ports_Init(void)
{
	TRISA = 0x0F; LATA = 0x00;
	TRISB = 0xCF; LATB = 0x30;
	TRISC = 0x10; LATC = 0x00;
	TRISD = 0x00; LATD = 0xE1;
	TRISE = 0x18; LATE = 0x00;
	TRISF = 0x64; LATF = 0x80;
	TRISG = 0x0F; LATG = 0x00;
	TRISH = 0xFC; LATH = 0x00;
#	if defined(VERSION_MELACS6)
	TRISJ = 0x00; LATJ = 0x87; // RTC Disabled (High)
#	else
	TRISJ = 0x00; LATJ = 0x85; // RTC Disabled (Low)
#	endif

	// 0 = Output / 1 = Input
	//*TRISAbits.TRISA0 = 1;	/* empty/M1VOLT */ 	PORTAbits.RA0 = 0;
	//*TRISAbits.TRISA1 = 1;	/* 5Vm */		PORTAbits.RA1 = 0;
	//*TRISAbits.TRISA2 = 1;	/* 12Vm/M2VOLT */  	PORTAbits.RA2 = 0;
	//*TRISAbits.TRISA3 = 1;	/* 24Vm */  	PORTAbits.RA3 = 0;
	//*TRISAbits.TRISA4 = 0;	/* SIP1 */  	PORTAbits.RA4 = 0;
	//*TRISAbits.TRISA5 = 0;	/* SIP0 */  	PORTAbits.RA5 = 0;
	//*TRISAbits.TRISA6 = 0;	/* OSC2 */  	PORTAbits.RA6 = 0;
	//*TRISAbits.TRISA7 = 0;	/* OSC1 */  	PORTAbits.RA7 = 0;

	//*TRISBbits.TRISB0 = 0;	/* CSPI2 */    	PORTBbits.RB0 = 0;
	//*TRISBbits.TRISB1 = 0;	/* CSPI1 */    	PORTBbits.RB1 = 0;
	//*TRISBbits.TRISB2 = 0;	/* CSPI0 */    	PORTBbits.RB2 = 0;
	//*TRISBbits.TRISB3 = 0;	/* empty */    	PORTBbits.RB3 = 0;
	//*TRISBbits.TRISB4 = 0;	/* M2DIS */    	PORTBbits.RB4 = 1;
	//*TRISBbits.TRISB5 = 0;	/* M1DIS */    	PORTBbits.RB5 = 1;
	//*TRISBbits.TRISB6 = 1;	/* SLAVEPGC */ 	PORTBbits.RB6 = 0;
	//*TRISBbits.TRISB7 = 1;	/* SLAVEPGD */ 	PORTBbits.RB7 = 0;

	//*TRISCbits.TRISC0 = 0;	/* SIP3 */	  	PORTCbits.RC0 = 0;
	//*TRISCbits.TRISC1 = 0;	/* SIP2*/	  	PORTCbits.RC1 = 0;
	//*TRISCbits.TRISC2 = 0;	/* M1BHI */   	PORTCbits.RC2 = 0;
	//*TRISCbits.TRISC3 = 0;	/* SPICLK */  	PORTCbits.RC3 = 0;
	//*TRISCbits.TRISC4 = 1;	/* SPIMISO */ 	PORTCbits.RC4 = 0;
	//*TRISCbits.TRISC5 = 0;	/* SPIMOSI */ 	PORTCbits.RC5 = 0;
	//*TRISCbits.TRISC6 = 0;	/* SIP4 */	  	PORTCbits.RC6 = 0;
	//*TRISCbits.TRISC7 = 0;	/* SIP5 */	  	PORTCbits.RC7 = 0;

	//*TRISDbits.TRISD0 = 0;	/* empty/MEMLED2 */	   	PORTDbits.RD0 = 1;
	//*TRISDbits.TRISD1 = 0;	/* empty/M1CUR_H */    	PORTDbits.RD1 = 0;
	//*TRISDbits.TRISD2 = 0;	/* empty/M2CUR_H */    	PORTDbits.RD2 = 0;
	//*TRISDbits.TRISD3 = 0;	/* LED_ON */   	PORTDbits.RD3 = 0;
	//*TRISDbits.TRISD4 = 0;	/* SI */	   	PORTDbits.RD4 = 0;
	//*TRISDbits.TRISD5 = 0;	/* CSI */	   	PORTDbits.RD5 = 1;
	//*TRISDbits.TRISD6 = 0;	/* LANRESET */ 	PORTDbits.RD6 = 1;
	//*TRISDbits.TRISD7 = 0;	/* CSLAN */	   	PORTDbits.RD7 = 1;

	//*TRISEbits.TRISE0 = 0;	/* M2ALI */		PORTEbits.RE0 = 0;
	//*TRISEbits.TRISE1 = 0;	/* M2AHI */ 	PORTEbits.RE1 = 0;
	//*TRISEbits.TRISE2 = 0;	/* M2BLI */ 	PORTEbits.RE2 = 0;
	//*TRISEbits.TRISE3 = 1;	/* DIO2 */		PORTEbits.RE3 = 0;
	//*TRISEbits.TRISE4 = 1;	/* DIO1 */		PORTEbits.RE4 = 0;
	//*TRISEbits.TRISE5 = 0;	/* M1AHI */ 	PORTEbits.RE5 = 0;
	//*TRISEbits.TRISE6 = 0;	/* M1BLI */ 	PORTEbits.RE6 = 0;
	//*TRISEbits.TRISE7 = 0;	/* M2BHI */ 	PORTEbits.RE7 = 0;

	//*TRISFbits.TRISF0 = 0;	/* ENVREG */  	PORTFbits.RF0 = 0;
	//*TRISFbits.TRISF1 = 0;	/* V_USB */	  	PORTFbits.RF1 = 0;
	//*TRISFbits.TRISF2 = 1;	/* TEMP */	  	PORTFbits.RF2 = 0;
	//*TRISFbits.TRISF3 = 0;	/* SLAVED- */ 	PORTFbits.RF3 = 0;
	//*TRISFbits.TRISF4 = 0;	/* SLAVED+ */ 	PORTFbits.RF4 = 0;
	//*TRISFbits.TRISF5 = 1;	/* M2CUR */	  	PORTFbits.RF5 = 0;
	//*TRISFbits.TRISF6 = 1;	/* M1CUR */   	PORTFbits.RF6 = 0;
	//*TRISFbits.TRISF7 = 0;	/* empty/VCCout_ON */   	PORTFbits.RF7 = 0;

	//*TRISGbits.TRISG0 = 1;	/* DIO0 */		PORTGbits.RG0 = 0;
	//*TRISGbits.TRISG1 = 1;	/* SW3 */		PORTGbits.RG1 = 0;
	//*TRISGbits.TRISG2 = 1;	/* SW4 */		PORTGbits.RG2 = 0;
	//*TRISGbits.TRISG3 = 1;	/* DIO3 */		PORTGbits.RG3 = 0;
	//*TRISGbits.TRISG4 = 0;	/* M1ALI */ 	PORTGbits.RG4 = 0;
	//*TRISGbits.TRISG5 = 0;	/* SLAVEMCLR */ PORTGbits.RG5 = 0;
	//*TRISGbits.TRISG6 = 0;	/* VSS */		PORTGbits.RG6 = 0;
	//*TRISGbits.TRISG7 = 0;	/* VDD */		PORTGbits.RG7 = 0;

	//*TRISHbits.TRISH0 = 0;	/* SCL */		PORTHbits.RH0 = 0;
	//*TRISHbits.TRISH1 = 0;	/* RS */		PORTHbits.RH1 = 0;
	//*TRISHbits.TRISH2 = 1;	/* SW1 */		PORTHbits.RH2 = 0;
	//*TRISHbits.TRISH3 = 1;	/* SW2 */		PORTHbits.RH3 = 0;
	//*TRISHbits.TRISH4 = 1;	/* DIO4 */		PORTHbits.RH4 = 0;
	//*TRISHbits.TRISH5 = 1;	/* DIO5 */		PORTHbits.RH5 = 0;
	//*TRISHbits.TRISH6 = 1;	/* DIO6 */		PORTHbits.RH6 = 0;
	//*TRISHbits.TRISH7 = 1;	/* DIO7 */		PORTHbits.RH7 = 0;

	//*TRISJbits.TRISJ0 = 0;	/* CSAD */	 	PORTJbits.RJ0 = 1;
	//*TRISJbits.TRISJ1 = 0;	/* CSRTC */  	PORTJbits.RJ1 = 0;
	//*TRISJbits.TRISJ2 = 0;	/* MEMLED */ 	PORTJbits.RJ2 = 1;
	//*TRISJbits.TRISJ3 = 0;	/* CSPI3 */	 	PORTJbits.RJ3 = 0;
	//*TRISJbits.TRISJ4 = 0;	/* SIP6 */	 	PORTJbits.RJ4 = 0;
	//*TRISJbits.TRISJ5 = 0;	/* SIP7 */	 	PORTJbits.RJ5 = 0;
	//*TRISJbits.TRISJ6 = 0;	/* BUZZER */ 	PORTJbits.RJ6 = 0;
	//*TRISJbits.TRISJ7 = 0;	/* CSSD */	 	PORTJbits.RJ7 = 1;
	
	//*TRISHbits.TRISH7 = 1;	/* DIO7 */		PORTHbits.RH7 = 0;	
	//*TRISHbits.TRISH6 = 1;	/* DIO6 */		PORTHbits.RH6 = 0;
	//*TRISHbits.TRISH5 = 1;	/* DIO5 */		PORTHbits.RH5 = 0;
	//*TRISHbits.TRISH4 = 1;	/* DIO4 */		PORTHbits.RH4 = 0;
	//*TRISGbits.TRISG3 = 1;	/* DIO3 */		PORTGbits.RG3 = 0;
	//*TRISEbits.TRISE3 = 1;	/* DIO2 */		PORTEbits.RE3 = 0;
	//*TRISEbits.TRISE4 = 1;	/* DIO1 */		PORTEbits.RE4 = 0;
	//*TRISGbits.TRISG0 = 1;	/* DIO0 */		PORTGbits.RG0 = 0;
}