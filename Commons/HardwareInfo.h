#ifndef HARDWARE_INFO_H
#define HARDWARE_INFO_H

extern const rom struct HardwareInfo
{
	/* 0x00 */ char Signature[7];
	/* 0x07 */ char SerialNo[10];
	/* 0x11 */ unsigned char MAC[6];
	/* 0x17 */ char Timestamp[21];
	/* 0x2C */
}
HardwareInfo;

#endif