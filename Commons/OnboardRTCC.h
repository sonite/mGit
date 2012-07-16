
#include "Microchip/MDD File System/FSIO.h" // SetClockVars

//** G L O B A L  D E F I N I T I O N S ***************************************/
#define yr (RTCclk.year)
#define mt (RTCclk.month)
#define dy (RTCclk.mday)
#define hr (RTCclk.hour)
#define mn (RTCclk.minute)
#define sc (RTCclk.second)
#define wd (RTCclk.wday)
#define pr (RTCclk.protect)

//** G L O B A L  V A R I A B L E S *******************************************/
extern struct RTCclk
{
	// Clock fields (8 bytes)
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t mday;
	uint8_t month;
	uint8_t wday;
	uint8_t year;
	uint8_t protect;
}
RTCclk;

extern struct RTCram
{
	// User-data storage (max 31 bytes)
	uint8_t Tracking   : 1;
	uint8_t Sensor     : 1;
	uint8_t Pumps      : 1;
	uint8_t Mounted    : 1;
	uint8_t dummy;
}
RTCram;

//** G L O B A L  P R O T O T Y P E S *****************************************/
void RTC_Read_Clock(void);
void RTC_Read_RAM(void);
void RTC_Write_Clock(void);
void RTC_Write_RAM(void);

//*****************************************************************************/
