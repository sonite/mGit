#include "HardwareInfo.h"

#pragma romdata HardwareInfo

const rom struct HardwareInfo HardwareInfo =
{
	// Signature string
	"MELACS",

	// Serial number
	"CF4AB6QTD",

	// MAC address
	{ 0x00, 0x04, 0xA3, 0x00, 0x00, 0x01 },

	// Build date
	__DATE__ " " __TIME__
};