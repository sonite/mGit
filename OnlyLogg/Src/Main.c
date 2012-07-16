/******************(MCU: PIC18F8J50 | Oscillator: 48 MHz)**********************
 *                                                                      	  *
 * Logger System															  *
 *																			  *
 * Version:        1.5.4													  *
 * Date:           2012-07-11												  *
 * Code:           Finsun Inresol AB										  *
 * Author:         Christian Gruffman										  *
 ***********************(   Microship MCC18 v3.48    )*************************/

//** I N C L U D E S **********************************************************/
#include "Main.h"

char rom SoftInfo[] = "1.5.4";

//** I N T E R N A L  V A R I A B L E S ***************************************/
/*
enum PPL // PULSES_PER_LITER
{
	Meister1 = 855,
	Kamstrup1 = 5760,
	Kamstrup2 = 4600,
	Windmill = 0,
};
// 12,000,000 Hz * 100 centi-scale * 60 seconds / 855 pulses per liter = 84210526.3 84210526UL
// 12,000,00 Hz * 1,000 meter-scale * 0.0980 (I want one less value, remove 10) = 117600000UL
enum SFR // Sensor Flow Rate
{
	sMeister1 = 84210526,
	sKamstrup1 = 12500000,
	sKamstrup2 = 15652174,
	sWindmill = 117600000,
};
*/
enum { NUMBER_OF_SENSORS = 2 }; //Pulses Per liter
struct Pulse_t PulsePort[];

uint8_t ITEMP;
uint16_t I24V;
struct Port_t Port[];

//** I N T E R N A L  P R O T O T Y P E S *************************************/
static void InitializeSystem(void);
static void InitializeSettings(void);
static void Init_TimeSettings(void);
static void GLCDinit(void);
static int LoadConfig(const rom Cfg_Variable *set, const char rom *name);
static void LoadSettings(void);
unsigned char Melacs_Powerup(void);

/******************************************************************************
 * Configuration files on SD card
 *****************************************************************************/
const char rom Microchip_DefaultPage[] = "INDEX.HTM";
static const char rom Microchip_SettingsFile[] = "MEMVARS.INI";
static const char rom Main_NetworkFile[] = "MELACS.INI";

//static unsigned char DummyOptions[8];

static const rom Cfg_Variable Main_Network[] =
{
#	if MICROCHIP_USE_IP
	CFG_MAC("MAC",     Microchip_Config.MyMACAddr),
	CFG_IP ("IP",      Microchip_Config.MyIPAddr ),
	CFG_IP ("GATEWAY", Microchip_Config.MyGateway),
	CFG_IP ("NETMASK", Microchip_Config.MyMask   ),
	CFG_UINT16("PORT", HTTP_Port                 ),
#	endif
	CFG_END()
};

const rom Cfg_Variable Microchip_Settings[] =
{
	CFG_INT24("AD0", Port[0].AD),
	CFG_INT24("AD1", Port[1].AD),
	CFG_INT24("AD2", Port[2].AD),
	CFG_INT24("AD3", Port[3].AD),
	CFG_INT24("AD4", Port[4].AD),
	CFG_INT24("AD5", Port[5].AD),
	CFG_INT24("AD6", Port[6].AD),
	CFG_INT24("AD7", Port[7].AD),
	/*
	CFG_UINT8("OPT1", DummyOptions[0]),
	CFG_UINT8("OPT2", DummyOptions[1]),
	CFG_UINT8("OPT3", DummyOptions[2]),
	CFG_UINT8("OPT4", DummyOptions[3]),
	CFG_UINT8("OPT5", DummyOptions[4]),
	CFG_UINT8("OPT6", DummyOptions[5]),
	CFG_UINT8("OPT7", DummyOptions[6]),
	CFG_UINT8("OPT8", DummyOptions[7]),
	*/
	CFG_INT16("ANO0", Port[0].Offset),
	CFG_INT16("ANO1", Port[1].Offset),
	CFG_INT16("ANO2", Port[2].Offset),
	CFG_INT16("ANO3", Port[3].Offset),
	CFG_INT16("ANO4", Port[4].Offset),
	CFG_INT16("ANO5", Port[5].Offset),
	CFG_INT16("ANO6", Port[6].Offset),
	CFG_INT16("ANO7", Port[7].Offset),
	
	CFG_INT16("ANF0", Port[0].Factor),
	CFG_INT16("ANF1", Port[1].Factor),
	CFG_INT16("ANF2", Port[2].Factor),
	CFG_INT16("ANF3", Port[3].Factor),
	CFG_INT16("ANF4", Port[4].Factor),
	CFG_INT16("ANF5", Port[5].Factor),
	CFG_INT16("ANF6", Port[6].Factor),
	CFG_INT16("ANF7", Port[7].Factor),
	
	CFG_UINT8("PP0a", PulsePort[0].active),
	CFG_UINT8("PP2a", PulsePort[1].active),
	CFG_UINT16("PP0pu", PulsePort[0].punits),
	CFG_UINT16("PP2pu", PulsePort[1].punits),
	
	CFG_UINT16("LOGG", Log_LongPeriod),
	CFG_UINT8("HMIupdate",HMIUpdateRate),
	CFG_END()
};

/******************************************************************************
 * Log file callback
 *****************************************************************************/
long Log_Value(Log_Field field)
{
	int24_t value = 0;

	// Keep in mind that the AD/DIO ports are mirrored in the field list as
	// well as on the board, hence the funky index calculations

	// Analog inputs
	if(field >= LOG_AD7 && field <= LOG_AD0)
	{
		value = Port[LOG_AD0 - field].AD;
		Num.Decimals = 3;
	}
	// Digital inputs
	if(field >= LOG_DIO7 && field <= LOG_DIO0)
	{
		value = Port[LOG_DIO0 - field].DIO;
	}
	// Voltage
	else if(field == LOG_I24V)
	{
		value = I24V;
		Num.Decimals = 3;
	}
	// Temperature
	else if(field == LOG_ITEMP)
	{
		value = ITEMP;
	}
	else if(field == LOG_PULSEPORT1)
	{
		value = value = PulsePort[0].rate;
		Num.Decimals = 3;
	}
	else if(field == LOG_PULSEPORT2)
	{
		value = value = PulsePort[1].rate;
		Num.Decimals = 3;
	}
	//else if(field == LOG_FLOWM2)
	//{
	//	value = PulsePort[1].liters;
	//}
	return value;
}

//** M A I N  B O D Y *********************************************************/
void main(void)
{
	InitializeSystem();
	Display_Cls();
	
	for(;;)
	{
		ClearWDT();
		ReadExternalADC();
		ReadInternalADC();
		RTC_Read_Clock();
		TreatSignals();
		AlarmCheck();
		Control();
		HMIupdate();
		Log_Run();
		
		Microchip_Run(TICK_SECOND / 50);
	}
}

/******************************************************************************
* Function:        static void Init(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Initiates functions.
*******************************************************************************/
static void InitializeSystem(void)
{
	//On the PIC18F87J50 Family of USB microcontrollers, the PLL will not power up and be enabled
	//by default, even if a PLL enabled oscillator configuration is selected (such as HS+PLL).
	//This allows the device to power up at a lower initial operating frequency, which can be
	//advantageous when powered from a source which is not gauranteed to be adequate for 48MHz
	//operation.  On these devices, user firmware needs to manually set the OSCTUNE<PLLEN> bit to
	//power up the PLL.
	{
		unsigned int pll_startup_counter = 600;
		OSCTUNEbits.PLLEN = 1; //Enable the PLL and wait 2+ms until the PLL locks before enabling USB module
		while(pll_startup_counter--);
	}
	
	WDTCONbits.ADSHR = 1; // Enable SFR
	ADCON0 = 0b00000000; /*
               '  0-------- VCFG1, Vref- 0 = AVss (1=AN2)
               '  -0------- VCFG2, Vref+ 0 = AVdd (1=AN3)
               '  --0000--- CHS<3:0>, Channel select -- 0
               '  ------0-- Go/Done, don't care for now
               '  -------0- ADON, Disable ADC */

	//ADCON1 |= 0x0f; // Configure all ADC inputs as Digital IO
	ADCON1 = 0b00001111; /*
               '  0-------- ADFM, Left justified result
               '  -0------- ADCAL, Normal A/D converter operation
               '  --001---- ACQT<2:0>, A/D Aquisition time = 2 Tad
               '  -----111- ADCS<2:0>, A/D conversion clock = FRC */

	ANCON0 = 0b00010000; /*
               '  -xx------ Not Implemented
               '  0--1000-- AN7 & AN<3:1> = Analog, AN4 = Digital
               '  -------0- AN0 = Analog */

	ANCON1 = 0b11110000; /*
	           '  1111----- AN<15:12> = Digital
               '  ----00--- AN<11:10> = Analog
               '  ------xx- Not Implemented */

	WDTCONbits.ADSHR = 0; // ' Back to Default SFR

	lpage = 1;      	// Show page 1 at display
	
	InitializeSettings();
	Init_TimeSettings();
}

static void InitializeSettings(void)
{
	uint8_t i = 0;
	
	Ports_Init();	
	SPIinit();
	IADCinit();
	Microchip_Init();
	GLCDinit();

	// Parse configuration file early on for any modules needing the
	// right settings on startup and to avoid USB being mounted and
	// preventing access to the file system
#	if MICROCHIP_USE_IP
	LoadConfig(Main_Network, Main_NetworkFile);
#	endif
	Microchip_RefreshConfig();

	// Any values not covered by the configuration file will keep
	// their default values
	LoadSettings();
	Microchip_RefreshSettings();

	// Allow the file system to be mounted via USB not that we've read
	// all of the configuration files
	Microchip_UnblockUSB();

	// Similarly let the HTTP sockets bind to the listening port
	Microchip_UnblockHTTP();

	Flow_Init();
//	Motor_Init();
//	SoftwarePWM_Init();

	do
	{
		if(Port[i].Factor == 0)
			Port[i].Factor = 1000.0;
		i++;
	}
	while(i < 8);
}

static void GLCDinit(void)
{
	Display_Init();
	if(Melacs_Powerup())
	{
		Microchip_Run(TICK_SECOND);
		Display_Init();
		Display_Bitmap(logo,0,0,128,64);
		Display_StringAt(SoftInfo, 0, 0);
		
		Microchip_Run(TICK_SECOND * 2);
	}
	else
	{/*
		unsigned char i;

		for(i = 0; i < 2; ++i)
		{
			Num.Flags |= NUM_ZERO_EXPANSION;
			Num.Radix = 0x10;
			Num.Alignment = 6;
			Numeric(Num_Buffer, PreservedBreadcrumbs[i]);
			Display_BufferAt(Num_Buffer, 0, i);
		}

		for(i = 0; i < 20; ++i)
		{
			ClearWDT();
			Microchip_Run(TICK_SECOND);
		} */
		ClearWDT();
	}

	Display_Cls();
	LCD_LEDON = 0;
}

static void Init_TimeSettings(void)
{
	// Read clock configuration variables
	//RTC_Read_RAM();
	RTC_Read_Clock(); // Already gathered from Microchip_Init(); But will not work there if no SD card found

	// Set start-up date and time
	/*
	sc = 0x02; // 31
	mn = 0x06; // 11
	hr = 0x10; // 12 -> 24h mode and 12 o clock
	dy = 0x1A; // 4
	mt = 0x04; // July
	wd = 0x05;
	yr = 0x0C; // 02
	pr = 0x00; //protect
	
	RTC_Write_Clock(); */
	
	if (!mt && !dy)
	{
		do {
			Display_Cls();
			LCD_LEDON = 1;
			Display_StringAt("Warning!", 40,7);
			Display_StringAt("Correct Time Settings", 0,5);
			Display_StringAt("are Required.",0,4);
			ClearWDT();
			Microchip_Run(TICK_SECOND * 10);
			Display_Cls();
			Edit_Clock();
		} while (!mt && !dy);
	}
}

/**************************
 * TreatSignals
 **************************/
void TreatSignals(void)
{
	uint8_t i;
	
	enum { M2CUR_OFFSET = 497 };
	
	ITEMP = (IADCsample[IADC_TEMP] * 330UL >> 10) - 50;
	I24V = IADCsample[IADC_24V] * 36792UL >> 10;

	// Convert to millivolts with 3300 mV reference voltage and 12-bits ADC
	// Translate and scale according to the user-definable settings.
	for(i = 0; i < 8; i++)
	{
		Port[i].AD = ((((adcresult[i] * 3300L) >> 12) + Port[i].Offset) * Port[i].Factor /1000L);
	}
/*
	Port[5].AD = ((((adcresult[5] * 3300L) >> 12)-350)*(31.8%));
	Port[6].AD = ((((adcresult[6] * 3300L) >> 12)-350)*(12.7%);
	Port[7].AD = ((((adcresult[7] * 3300L) >> 12)-250)*(3.8%));
	
	Port[5].AD = ((((adcresult[5] * 3300L) >> 12)-350)/(315/100));
	Port[6].AD = ((((adcresult[6] * 3300L) >> 12)-350)/(315/40));
	Port[7].AD = ((((adcresult[7] * 3300L) >> 12)-250)/(325/12));
*/

	//Port[0].DIO = PORTGbits.RG0;
	Port[1].DIO = PORTEbits.RE4;
	Port[2].DIO = PORTEbits.RE3;
	//Port[3].DIO = PORTGbits.RG3;
	Port[4].DIO = PORTHbits.RH4;
	Port[5].DIO = PORTHbits.RH5;
	Port[6].DIO = PORTHbits.RH6;
	Port[7].DIO = PORTHbits.RH7;
	
	/** Produced Energy Calculation Momentaneous **/
	/* Hence the Solar Irradiation start/stop the pumps, a condition like if sun above 150W/m2 calculate is not needed 
		risk of negative values? */
	/* PECm = Q[L/s] * (Tout [°C] -Tin[°C])*Cp 4188[J/kg/K] * (0,998 [kg/L])  [W] */
	//PECm = ((Port[5].AD/10)/60) * ((Port[0].AD - Port[1].AD)/10) * 4188 * (988/100);
	/** Possible energy measurenent **/
	//PEM = Port7 x m2....
	
	//För Logfilen
	/*if(Flow_Pulses(0) > Old_Pulses)
	{
	
	uint24_t Pulses = Flow_Pulses(0);
		Pulse_Container += (Pulses-Old_Pulses);
		Microchip_RefreshSettings();
		Old_Pulses = Pulses;
	}*/
	
	for(i = 0; i < NUMBER_OF_SENSORS; ++i)
	{
/*		uint24_t threshold = PulsePort[i].pulses;
		
		uint16_t liters = 0;
		
		if (!i)
		{	//855 - THD
			while(Flow_Pulses(i) - threshold >= PulsePort[0].punits){ threshold += PulsePort[0].punits; ++liters; }
			// Could bee used in HMI to get liters and deciliters
			PulsePort[i].decimal = (threshold - Flow_Pulses(i)) * 10 / PulsePort[0].punits;
		}
		else { // 5760 - Kamstrup
			while(Flow_Pulses(i) - threshold >= PulsePort[1].punits){ threshold += PulsePort[1].punits; ++liters; }
			// Could bee used in HMI to get liters and deciliters
			PulsePort[i].decimal = (threshold - Flow_Pulses(i)) * 10 / PulsePort[1].punits;
		}
		if(liters)
		{
		    PulsePort[i].pulses = threshold;
			PulsePort[i].liters += liters;
			Microchip_RefreshSettings();
		}
*/		
		if(PulsePort[i].rate = Flow_Rate(i), PulsePort[i].rate)
		{
			// 12,000,000 Hz * 100 centi-scale * 60 seconds / 855 pulses per liter = 84210526.3 84210526UL
			if (!i) //(PULSES_PER_LITER == 855)
				PulsePort[i].rate = ((720000000UL / PulsePort[i].punits) * 100) / Flow_Rate(i); // L/min
				//15652173,913043478260869565217391
			else //(PULSES_PER_LITER == 5760) 4600= 15652174UL
				PulsePort[i].rate = ((720000000UL / PulsePort[i].punits) * 100) / Flow_Rate(i); // L/min
		}
	}

}

/**************************
 * AlarmCheck
 **************************/
void AlarmCheck(void)
{

}

/**************************
 * Control
 **************************/
void Control(void)
{
	if (!mt && !dy)
		Init_TimeSettings();
}
/**************************
 * Load and save configuration files
 **************************/
static int LoadConfig(const rom Cfg_Variable *set, const char rom *name)
{
	int error = Cfg_ParseFile(set, name);
	if(error)
		FileError(error, name);
	return error;
}

// Try loading the settings from file, but use the FLASH configuration if the
// file cannot be accessed at all
static void LoadSettings(void)
{
	if(LoadConfig(Microchip_Settings, Microchip_SettingsFile) == -1)
		Cfg_ParseFlash(Microchip_Settings);
}

// The settings might have changed, so save the new variables to file along with
// a backup in FLASH
void Microchip_RefreshSettings(void)
{
	Cfg_SaveFile(Microchip_Settings, Microchip_SettingsFile);
	Cfg_SaveFlash(Microchip_Settings);
}

unsigned char Melacs_Powerup(void)
{
	if (!RCONbits.BOR && !RCONbits.POR && RCONbits.TO)
	{
		RCONbits.BOR = 1;
		RCONbits.POR = 1;
		EnableWDT();
		return 1;			// Ordinary Powerup
	}
	else
	{/*
		memcpy
		(
			(void *) PreservedBreadcrumbs,
			(void *) Breadcrumbs,
			sizeof PreservedBreadcrumbs
		);

		memset
		(
			(void *) Breadcrumbs,
			0xFF,
			sizeof Breadcrumbs
		);*/

		EnableWDT();
		ClearWDT();
		return 0;			// WDT occured
	}
}
//****************************************************************************/
