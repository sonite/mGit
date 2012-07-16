/******************************************************************************
* Function:        HMIupdate(),LastPage(),NewPage(),ShowPage1(),ShowPage2(),ShowPage3(),Options(),
*			       ClearMessageBuffer(),SetClock(),SetADconv(),SetTemp(),SetSol(),SetClock_Edit(),
*				   SetADconv_Edit(),SetTemp_Edit(),SetSol_Edit(),Options_Loop(),Toolbar1(),
*				   Toolbar2(),Toolbar3(),Toolbar4(),DisplayToolbar(),ShowDate(),Failure()
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Contains functions regarding HMI.
*******************************************************************************/

//** I N C L U D E S **********************************************************/
#include "Main.h"
#include "HardwareInfo.h"
#include "lang.h" /** Common Language Support **/
#define ENG

//** I N T E R N A L  V A R I A B L E S ***************************************/
#define STLEN 20

/* Usage for remember values outside of function */
static uint8_t GoHome, GoBack, GoForward, Return, failure_flag = 0;
static uint8_t buttons; 	// glcd card buttons
uint8_t menuP;
uint8_t lpage;
uint8_t lastValue;
uint8_t HMIUpdate;
uint8_t HMIUpdateReturn;
uint8_t HMIUpdateRate = 2;

/* Used only within different functions */
int24_t TempValue1 = 0;
int24_t TempValue2 = 0;
int24_t TempValue3 = 0;
uint8_t status;

// static rom const char celsius[] = "°C"; 	// Centigrade
// static rom const char fahrenheit[] = "°F";	// Fahrenheit
// static rom const char degree[] = "°";		// Degree

/** Application Language Support **/
#ifdef SWE
	static const char rom AnswerTempOut[] = "Temp Ut:";
	static const char rom AnswerAlgorithm[] = "ALGORITM ";
	static const char rom AnswerHeatProtection[] = "VÄRMESKYDD:";
	
#else
	static const char rom AnswerTempOut[] = "Temp Out:";
	static const char rom AnswerAlgorithm[] = "ALGORITHM ";
	static const char rom AnswerHeatProtection[] = " HEATING PROTECTION: ";
	static const char rom AnswerEvent[] = "OCCURRED:";
#endif

//** I N T E R N A L  P R O T O T Y P E S *************************************/
static void LastPage(void);
static void NewPage(void);
static void ShowPage1(void);
static void ShowPage2(void);
static void ShowPage3(void);
static void Edit_ADconv(void);
static void Edit_Compile_Time(void);
static void Show_Network(void);
static void Edit_Bootloader(void);
static void Show_Info(void);
static void Edit_DigitalPortConversion(void);
static void Edit_HMIUpdateFreqency(void);
static void ShowDate(void);
static void Toolbar1(void);
static void Toolbar3(void);
static void Toolbar4(void);
static void Toolbar5(void);
static void DisplayToolbar(rom const char *message);
static void ScreenSaver(void);
static void HMIUpdateFrequence(uint8_t Secs);
uint8_t MenuStep(uint8_t value, uint8_t value2, uint8_t steps);
static void FunctionChoice(uint8_t MsgValue);
static void HMI_FullRootFunc(void);
static void HMI_RootFunc(uint8_t Rchoice, uint8_t Fchoice);

//** F U N C T I O N S ********************************************************/
void HMIupdate(void)
{
	ScreenSaver();

	buttons = Read_Buttons();

	if(!buttons)
	{
		HMIUpdateFrequence(HMIUpdateRate); // Secounds
		if(HMIUpdate)
		{
			Display_Cls();
			LastPage();
		}
	}
	else
		NewPage();
}

static void LastPage(void)
{
	switch(lpage)
	{
	case 1: ShowPage1(); break;
	case 2: ShowPage2(); break;
	case 3: ShowPage3(); break;
	}
}

static void NewPage(void)
{
	Display_Cls();
	Buzzer_Click(BUZZER_BUTTON);
	Microchip_Run(TICK_SECOND);
	if(buttons & 1)
		ShowPage1();
	if(buttons & 2)
		ShowPage2();
	if(buttons & 4)
		ShowPage3();
	if(buttons & 8)
		Options(1);
}

static void ShowPage1(void)
{
	uint8_t i; 
/*	
	for(i = 0; i < 8; ++i)
	{
		Num.Radix = 0x10;
		Numeric(Num_Buffer, serial_number[i]);
		Display_BufferAt(Num_Buffer, 0, i);
	}
	*//*
	 i = 8;
	Numeric(Num_Buffer, serial_number[0]); Display_StringAt("NR: " , 0, 7); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, serial_number[1]); Display_StringAt("NR: " , 0, 6); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, serial_number[2]); Display_StringAt("NR: " , 0, 5); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, serial_number[3]); Display_StringAt("NR: " , 0, 4); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, serial_number[4]); Display_StringAt("NR: " , 0, 3); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, serial_number[5]); Display_StringAt("NR: " , 0, 2); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, serial_number[6]); Display_StringAt("NR: " , 0, 1); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, serial_number[7]); Display_StringAt("NR: " , 0, 0); Display_Buffer(Num_Buffer);
*/
	Numeric(Num_Buffer, Port[7].AD); Display_StringAt("AN 7: " , 0, 7); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, Port[6].AD); Display_StringAt("AN 6: " , 0, 6); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, Port[5].AD); Display_StringAt("AN 5: " , 0, 5); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, Port[4].AD); Display_StringAt("AN 4: " , 0, 4); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, Port[3].AD); Display_StringAt("AN 3: " , 0, 3); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, Port[2].AD); Display_StringAt("AN 2: " , 0, 2); Display_Buffer(Num_Buffer);

	Toolbar1();
	lpage = 1;
}

static void ShowPage2(void)
{
	Numeric(Num_Buffer, Port[1].AD); Display_StringAt("AN 1: " , 0, 7); Display_Buffer(Num_Buffer);
	Numeric(Num_Buffer, Port[0].AD); Display_StringAt("AN 0: " , 0, 6); Display_Buffer(Num_Buffer);
	Display_StringAt("IO 7: " , 0, 5); Display_String((Port[7].DIO) ? " High" : " Low-");
	Display_StringAt("IO 6: " , 0, 4); Display_String((Port[6].DIO) ? " High" : " Low-");
	Display_StringAt("IO 5: " , 0, 3); Display_String((Port[5].DIO) ? " High" : " Low-");
	Display_StringAt("IO 4: " , 0, 2); Display_String((Port[4].DIO) ? " High" : " Low-");

	Toolbar1();
	lpage = 2;
}

static void ShowPage3(void)
{
	Numeric(Num_Buffer, PulsePort[1].rate); Display_StringAt("IO 3: " , 0, 7); Display_Buffer(Num_Buffer);
	Display_StringAt("IO 2: " , 0, 6); Display_String((Port[2].DIO) ? " High" : " Low-");
	Display_StringAt("IO 1: " , 0, 5); Display_String((Port[1].DIO) ? " High" : " Low-");
	Numeric(Num_Buffer, PulsePort[0].rate); Display_StringAt("IO 0: " , 0, 4); Display_Buffer(Num_Buffer);
	/*
	Num.Flags |= NUM_GROUPING;
	Num.Alignment = 4;
	Num.Decimals = 2;
	Numeric(Num_Buffer, FlowData[0].rate); Display_StringAt("IO 0: " , 0, 4); Display_BufferAt(Num_Buffer, 0, 4); Display_String(" L/min");
	*/
	ShowDate();
	Toolbar1();
	
	lpage = 3;
}

void Options(uint8_t menuP)
{
	do
	{
		Display_StringAt(MenuTitle, 1, 7);
		Toolbar4();
		
		switch(menuP)
		{
			case 1:
				Display_StringAt("Clock Settings",12,6);
				Display_StringAt("A/D   Settings",12,5);
				Display_StringAt("Log   Settings",12,4);
				Display_StringAt("Network Lookup",12,3);
				Display_StringAt("BootLoader",12,2);
				Display_StringAt("~",6,6);
				break;
			case 2:
				Display_StringAt("Clock Settings",12,6);
				Display_StringAt("A/D   Settings",12,5);
				Display_StringAt("Log   Settings",12,4);
				Display_StringAt("Network Lookup",12,3);
				Display_StringAt("BootLoader",12,2);
				Display_StringAt("~",6,5);
				break;
			case 3:
				Display_StringAt("Clock Settings",12,6);
				Display_StringAt("A/D   Settings",12,5);
				Display_StringAt("Log   Settings",12,4);
				Display_StringAt("Network Lookup",12,3);
				Display_StringAt("BootLoader",12,2);
				Display_StringAt("~",6,4);
				break;
			case 4:
				Display_StringAt("Clock Settings",12,6);
				Display_StringAt("A/D   Settings",12,5);
				Display_StringAt("Log   Settings",12,4);
				Display_StringAt("Network Lookup",12,3);
				Display_StringAt("BootLoader",12,2);
				Display_StringAt("~",6,3);
				break;
			case 5:
				Display_StringAt("Clock Settings",12,6);
				Display_StringAt("A/D   Settings",12,5);
				Display_StringAt("Log   Settings",12,4);
				Display_StringAt("Network Lookup",12,3);
				Display_StringAt("BootLoader",12,2);
				Display_StringAt("~",6,2);
				break;
			case 6:
				Display_StringAt("A/D   Settings",12,6);
				Display_StringAt("Log   Settings",12,5);
				Display_StringAt("Network Lookup",12,4);
				Display_StringAt("BootLoader",12,3);
				Display_StringAt("System Info",12,2);
				Display_StringAt("~",6,2);
				break;
			case 7:
				Display_StringAt("Log   Settings",12,6);
				Display_StringAt("Network Lookup",12,5);
				Display_StringAt("BootLoader",12,4);
				Display_StringAt("System Info",12,3);
				Display_StringAt("Pulse Settings",12,2);
				Display_StringAt("~",6,2);
				break;
			case 8:
				Display_StringAt("Network Lookup",12,6);
				Display_StringAt("BootLoader",12,5);
				Display_StringAt("System Info",12,4);
				Display_StringAt("Pulse Settings",12,3);
				Display_StringAt("HMI Update Rate",12,2);
				Display_StringAt("~",6,2);
				break;
				
			default: break;
		}
		
		do{
			HMI_FullRootFunc();
		}while(!buttons);
		
		Buzzer_Click(BUZZER_BUTTON);
		
		menuP = MenuStep(buttons, menuP, 8);
		
		// EXECUTE (OK)
		if(buttons & 2)
		{
			TempValue1 = 0; // No delay
			
			Microchip_Run(TICK_SECOND);
			Display_Cls();
			buttons = 0;
			
			switch(menuP)
			{	
				case 1:
					Edit_Clock();
					break;
				case 2:
					Edit_ADconv();
					TempValue1 = 1;
					break;
				case 3:
					Edit_Compile_Time();
					break;
				case 4:
					Show_Network();
					break;
				case 5:
					Edit_Bootloader();
					break;
				case 6:
					Show_Info();
					break;
				case 7:
					Edit_DigitalPortConversion();
					break;
				case 8:
					Edit_HMIUpdateFreqency();
					break;
					
				default: break;
			}
			buttons = 0;
			if (!TempValue1) // no Delay
				Microchip_Run(TICK_SECOND * 2);
		}
		Display_Cls();
		
	}while(menuP);
}

void Edit_Clock(void)
{
	Display_StringAt(AnswerTimeDate, 0, 7);
	Display_StringAt(" YY-MM-DD  HH:MM:SS  ", 0, 5);
	//                 12 45 78  12 45 78

	RTC_Read_Clock();
	Edit_Begin();

	do{
		HMI_RootFunc(1,0);
		yr = Edit_Clip(0, Edit_Numeric( 1, 2, yr), 99);
		mt = Edit_Clip(1, Edit_Numeric( 4, 2, mt), 12);
		dy = Edit_Clip(1, Edit_Numeric( 7, 2, dy), 31);
		hr = Edit_Clip(1, Edit_Numeric(11, 2, hr), 23);
		mn = Edit_Clip(1, Edit_Numeric(14, 2, mn), 59);
		sc = Edit_Clip(1, Edit_Numeric(17, 2, sc), 59);
	}while(status = Edit_Update(3, Button_Save, Button_Cancel), !status);
	
	if(status == 1)
		status = 3;
	else
		RTC_Read_Clock();
		
	FunctionChoice(status);
}

static void Edit_ADconv(void)
{
	menuP = 1;
	do{
		Display_StringAt(SubMenuTitle,1,7);
		Toolbar4();
		
		switch(menuP)
		{	uint8_t i;
			case 1:
				for(i = 5; i > 0; i--){
					Numeric(Num_Buffer, i+2);
					Display_StringAt("AN",12,(i+1)); Display_Buffer(Num_Buffer); Display_String("   Settings");
				}
				Display_StringAt("~",6,6);
				break;
			case 2:
				for(i = 5; i > 0; i--){
					Numeric(Num_Buffer, i+2);
					Display_StringAt("AN",12,(i+1)); Display_Buffer(Num_Buffer); Display_String("   Settings");
				}
				Display_StringAt("~",6,5);
				break;
			case 3:
				for(i = 5; i > 0; i--){
					Numeric(Num_Buffer, i+2);
					Display_StringAt("AN",12,(i+1)); Display_Buffer(Num_Buffer); Display_String("   Settings");
				}
				Display_StringAt("~",6,4);
				break;
			case 4:
				for(i = 5; i > 0; i--){
					Numeric(Num_Buffer, i+2);
					Display_StringAt("AN",12,(i+1)); Display_Buffer(Num_Buffer); Display_String("   Settings");
				}
				Display_StringAt("~",6,3);
				break;
			case 5:
				for(i = 5; i > 0; i--){
					Numeric(Num_Buffer, i+2);
					Display_StringAt("AN",12,(i+1)); Display_Buffer(Num_Buffer); Display_String("   Settings");
				}
				Display_StringAt("~",6,2);
				break;
			case 6: 
				for(i = 5; i > 0; i--){
					Numeric(Num_Buffer, i+1);
					Display_StringAt("AN",12,(i+1)); Display_Buffer(Num_Buffer); Display_String("   Settings");
				}
				Display_StringAt("~",6,2);
				break;
			case 7:
				for(i = 5; i > 0; i--){
					Numeric(Num_Buffer, i);
					Display_StringAt("AN",12,(i+1)); Display_Buffer(Num_Buffer); Display_String("   Settings");
				}
				Display_StringAt("~",6,2);
				break;
			case 8:
				for(i = 5; i > 0; i--){
					Numeric(Num_Buffer, i-1);
					Display_StringAt("AN",12,(i+1)); Display_Buffer(Num_Buffer); Display_String("   Settings");
				}
				Display_StringAt("~",6,2);
				break;
				
			default: break;
		}
		
		do{
			HMI_FullRootFunc();
		}while(!buttons);
		Buzzer_Click(BUZZER_BUTTON);
		
		menuP = MenuStep(buttons, menuP, 8);
		
		if(buttons & 2)		// Execute
		{
			
			TempValue1 = 8; // Choice
			TempValue1 -= menuP;
			
			TempValue2 = Port[TempValue1].Factor;
			TempValue3 = Port[TempValue1].Offset;
			
			Microchip_Run(TICK_SECOND);
			Display_Cls();
			Display_StringAt(" OFFSET VALUE: AD ", 0, 7); Numeric(Num_Buffer, TempValue1); Display_Buffer(Num_Buffer);
			
			Edit_Begin();
			
			do{
				HMI_RootFunc(1,0);
				
			    Edit_PutString("Scaling:    +100,0% ");
				Num.Decimals = 1;
				Num.Flags |= NUM_FORCE_SIGN;
				Port[TempValue1].Factor = Edit_Clip(-1000, Edit_Numeric(12, 6, Port[TempValue1].Factor), +1000);
				
				Edit_ExtraLine(4);
				
				Edit_PutString("Offset:     +1,000 V");
				Num.Decimals = 3;
				Num.Flags |= NUM_FORCE_SIGN;
				Port[TempValue1].Offset = Edit_Clip(-1000, Edit_Numeric(12, 6, Port[TempValue1].Offset), +1000);
			}while(status = Edit_Update(3, Button_Save, Button_Cancel), !status);
			
			// Rewind Values
			if(status != 1)
			{
				Port[TempValue1].Factor = TempValue2;
				Port[TempValue1].Offset = TempValue3;
			}
			
			HMI_RootFunc(2, status);
		}
		Display_Cls();
		
	}while(menuP);
}

static void Edit_Compile_Time(void)
{
	TempValue1 = Log_LongPeriod;
	
	Display_StringAt(" Compile Seconds:  ", 5, 5);

	Edit_Begin();

	do{
		HMI_RootFunc(1,0);
		Log_LongPeriod = Edit_Clip(60, Edit_Numeric( 1, 3, Log_LongPeriod), 600);
	}while(status = Edit_Update(3, Button_Save, Button_Cancel), !status);
	
	if(status != 1)
		Log_LongPeriod = TempValue1;
	
	HMI_RootFunc(2, status);
}

static void Show_Network(void)
{
	uint8_t i;
	Toolbar5();
	
	Display_StringAt("IP: ", 0, 7);
	for (i=0; i < 4; i++){ Numeric(Num_Buffer, AppConfig.MyIPAddr.v[i]); Display_Buffer(Num_Buffer); if (3 > i) Display_String("."); }
	
	Display_StringAt("NETMASK:", 0, 6);
	for (i=0; i < 4; i++){ Numeric(Num_Buffer, AppConfig.MyMask.v[i]); Display_Buffer(Num_Buffer); if (3 > i) Display_String("."); }
	
	Display_StringAt("GATEWAY:", 0, 5);
	for (i=0; i < 4; i++){ Numeric(Num_Buffer, AppConfig.MyGateway.v[i]); Display_Buffer(Num_Buffer); if (3 > i) Display_String("."); }
	
	Display_StringAt("MAC:", 0, 4);
	for (i=0; i < 6; i++)
	{
		Num.Flags |= NUM_ZERO_EXPANSION; Num.Alignment = 2; Num.Radix = 16; 
		Numeric(Num_Buffer, AppConfig.MyMACAddr.v[i]); Display_Buffer(Num_Buffer); if (5 > i) Display_String("-");
	}

	do{
		HMI_FullRootFunc();
	}while(!buttons);
	Buzzer_Click(BUZZER_BUTTON); 
}

static void Edit_Bootloader(void)
{
	Toolbar3();
	
	Display_StringAt("BootLoader: ", 0, 7);
	Display_StringAt("Start?", 0, 3);
		
	do{
		HMI_FullRootFunc();
	}while(!buttons);
	
	if(buttons & 1)
	{
		Microchip_EnterBootloader();
	}
	
	if(buttons & 8)
	{
	
	}
	Buzzer_Click(BUZZER_BUTTON);
}

static void Show_Info(void)
{
	Toolbar5();
	
	Display_StringAt("SERIAL: ", 5, 6);  
	Display_String(HardwareInfo.SerialNo);
	
	Display_StringAt("Software rev" , 5, 5);
	Display_String(SoftInfo);
	
	do{
		Num.Decimals = 3;
		Display_StringAt("Voltage Feed: ", 5, 3);
		Numeric(Num_Buffer, I24V);
		Display_Buffer(Num_Buffer);
		
		Display_StringAt("INT Temp: ", 5, 2);
		Numeric(Num_Buffer, ITEMP);
		Display_Buffer(Num_Buffer);
		Display_String("°C");
		
		HMI_FullRootFunc();
	}while(!buttons);
	Buzzer_Click(BUZZER_BUTTON);
}

static void Edit_HMIUpdateFreqency(void)
{
	TempValue1 = HMIUpdateRate;
	
	Display_StringAt(" HMI Update Rate", 6, 5);

	Edit_Begin();

	do{
		HMI_FullRootFunc();
		Edit_PutString("  in Seconds: 01s ");
		HMIUpdateRate = Edit_Clip(0, Edit_Numeric( 14, 2, HMIUpdateRate), 10);
	}while(status = Edit_Update(4, Button_Save, Button_Cancel), !status);
	
	if(status != 1)
		HMIUpdateRate = TempValue1;
	
	HMI_RootFunc(2, status);
}

static void Edit_DigitalPortConversion(void)
{
	uint8_t TempValue4 = PulsePort[0].active;
	TempValue1 = PulsePort[0].punits;
	TempValue2 = PulsePort[1].active;
	TempValue3 = PulsePort[1].punits;
	
	Edit_Begin();
	
	do{
		HMI_FullRootFunc();
		Edit_PutString("IO-0 ACTIVE:  1");
		Num.Decimals = 0;
		PulsePort[0].active = Edit_Clip(0, Edit_Numeric(14, 1, PulsePort[0].active), +1);
		
		Edit_ExtraLine(7);
		
		Edit_PutString("IO-0 P/Units: 10000");
		Num.Decimals = 0;
		Num.Flags |= NUM_FORCE_SIGN;
		PulsePort[0].punits = Edit_Clip(300, Edit_Numeric(14, 6, PulsePort[0].punits), +65535);
		
		Edit_ExtraLine(6);
		
		Edit_PutString("IO-3 ACTIVE:  1");
		Num.Decimals = 0;
		PulsePort[1].active = Edit_Clip(0, Edit_Numeric(14, 1, PulsePort[1].active), +1);
		
		Edit_ExtraLine(5);
		
		Edit_PutString("IO-3 P/Units: 10000");
		Num.Decimals = 0;
		Num.Flags |= NUM_FORCE_SIGN;
		PulsePort[1].punits = Edit_Clip(300, Edit_Numeric(14, 6, PulsePort[1].punits), +65535);
	}
	while(status = Edit_Update(4, Button_Save, Button_Cancel), !status);

	if(status != 1)
	{
		PulsePort[0].active = TempValue4;
		PulsePort[0].punits = TempValue1;
		PulsePort[1].active = TempValue2;
		PulsePort[1].punits = TempValue3;
	}
	
	HMI_RootFunc(2, status);
}

uint8_t MenuStep(uint8_t buttons, uint8_t menuP, uint8_t steps)
{
	if(buttons & 1) // HOME
	{
		buttons = 0;
		Microchip_Run(TICK_SECOND);
		Display_Cls();
		menuP = 0;
	}
	
	if(buttons & 4) // UP
	{
		buttons = 0;
		Microchip_Run(TICK_SECOND/4);
		Display_Cls();
		if (menuP - 1 <= 0)
			menuP = steps;
		else
			menuP -= 1;
	}
	
	if(buttons & 8) // DOWN
	{
		buttons = 0;
		Microchip_Run(TICK_SECOND/4);
		Display_Cls();
		if (menuP + 1 >= (steps+1))
			menuP = 1;
		else
			menuP += 1;
	}
	return menuP;
}

static void ShowDate(void)
{
	Display_StringAt("TIME:", 5, 3);
	ByteToStrWithZeros(hr, Num_Buffer); Num_Buffer[0] = ' '; Display_Buffer(Num_Buffer);
	ByteToStrWithZeros(mn, Num_Buffer); Num_Buffer[0] = ':'; Display_Buffer(Num_Buffer);
	ByteToStrWithZeros(sc, Num_Buffer); Num_Buffer[0] = ':'; Display_Buffer(Num_Buffer);

	Display_StringAt("DATE:", 5, 2);
	ByteToStrWithZeros(yr, Num_Buffer); Num_Buffer[0] = ' '; Display_Buffer(Num_Buffer);
	ByteToStrWithZeros(mt, Num_Buffer); Num_Buffer[0] = '-'; Display_Buffer(Num_Buffer);
	ByteToStrWithZeros(dy, Num_Buffer); Num_Buffer[0] = '-'; Display_Buffer(Num_Buffer);
}

static void Toolbar1(void)
{
	DisplayToolbar("P1    P2    P3    OP");
}

static void Toolbar3(void)
{
	DisplayToolbar("YES                NO");
}

static void Toolbar4(void)
{
	DisplayToolbar(" Home  OK  UP  DOWN  ");
}

static void Toolbar5(void)
{
	DisplayToolbar("                   OK");
}

static void DisplayToolbar(rom const char *message)
{
	uint8_t x;

	Display_StringAt(message, 2, 0);

	for(x = 0; x < 128; ++x)
		Display_SetPixel(x, 56);

}

static void ScreenSaver(void)
{
	// Disable the backlight when 60 seconds have elapsed without a button being pressed
	signed long tick;
	static signed long threshold;

	tick = TickGet();

	if(Read_Buttons())
	{
		threshold = tick + TICKS_PER_SECOND * 60;
		LCD_LEDON = 1;
	}

	if(tick - threshold > 0)
		LCD_LEDON = 0;
}

static void HMIUpdateFrequence(uint8_t Secs)
{
	signed long UpdateTick;
	static signed long UpdateThreshold;

	UpdateTick = TickGet();

	if(!HMIUpdateReturn)
	{
		UpdateThreshold = UpdateTick + TICKS_PER_SECOND * Secs;
		HMIUpdate = 0; // don't do anything
		HMIUpdateReturn = 1;
	}

	if(UpdateTick - UpdateThreshold > 0)
	{
		HMIUpdate = 1; // Update
		HMIUpdateReturn = 0;
	}
}

void FileError(int error, const char rom *name)
{
	LCD_LEDON = 1;
	Display_Cls();

	Display_StringAt("Configuration error", 0, 7);
	Display_StringAt("File: ", 0, 6);
	Display_String(name);
	Display_StringAt("Line: ", 0, 5);
	Numeric(Num_Buffer, error);
	Display_Buffer(Num_Buffer);
	HMI_FullRootFunc();
	Microchip_Run(TICK_SECOND * 3);
	Display_Cls();
}

void ProcessError(int error)
{
	Display_StringAt("Status: KOD ", 0, 4);
	Numeric(Num_Buffer, error);
	Display_Buffer(Num_Buffer);
}

static void FunctionChoice(uint8_t MsgValue)
{
	int i;

	for(i = 7; i > 1; i--)
		Line_Cls(i);
	
	switch(MsgValue)
	{
		case 1:
			Display_StringAt(AnswerSave, 40, 5); 
			Microchip_RefreshSettings();
			break;
		case 2:
			Display_StringAt(AnswerCancel, 40, 5);
			break;
		case 3:
			Display_StringAt(AnswerSave, 40, 5);
			RTC_Write_Clock();
			break;
	 }
}

static void HMI_FullRootFunc(void)
{
	buttons = Read_Buttons();
	ReadExternalADC();
	ReadInternalADC();
	RTC_Read_Clock();
	TreatSignals();
	Microchip_Run(TICK_SECOND / 50);
	Log_Run();
	ClearWDT();
}

static void HMI_RootFunc(uint8_t Rchoice, uint8_t Fchoice)
{
	ClearWDT();
	switch(Rchoice)
	{
		case 1:
			Microchip_Run(TICK_SECOND / 50);
			buttons = Read_Buttons();
			break;
		case 2:
			FunctionChoice(Fchoice);
			Microchip_Run(TICK_SECOND);
			break;
		case 3:
			RTC_Write_RAM();
			RTC_Read_RAM();
	}
}

//*****************************************************************************/
