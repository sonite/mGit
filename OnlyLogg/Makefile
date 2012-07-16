PROJECT = OnlyLogg_1.5-rc3
LINKER = "linker.lkr"

MCU = 18F87J50
CFLAGS = -nw=3004 -ml -Oi- -Opa- --extended -I="src" -I="../Commons" -I="../Commons/Microchip"
ASFLAGS = /y+
LDFLAGS = /l ../Libraries /p $(MCU)

all : $(PROJECT).hex

OBJS = \
	"Src/Main.o"                                       \
	"Src/HMI.o"										   \
	"Src/Log.o"                                        \
	                                                   \
	"../Commons/Ports.o"                               \
	"../Commons/Buzzer.o"                              \
	"../Commons/Config.o"                              \
	"../Commons/Editable.o"                            \
	"../Commons/Flash.o"                               \
	"../Commons/Flow.o"                                \
	"../Commons/glcd_functions.o"                      \
	"../Commons/HardwareInfo.o"                        \
	"../Commons/IADC.o"                                \
#	"../Commons/Motor.o"                               \
	"../Commons/Numeric.o"                             \
	"../Commons/melacs_spi.o"                          \
#	"../Commons/Startup.o"                             \
	"../Commons/Utils.o"                               \
#	"../Commons/SoftPWM.o"                             \
	                                                   \
	"../Commons/Microchip/Microchip.o"                 \
	"../Commons/Microchip/MDD File System/FSIO.o"      \
	"../Commons/Microchip/MDD File System/SD-SPI.o"    \
	"../Commons/Microchip/TCPIP Stack/Tick.o"          \
	                                                   \
	"../Commons/Microchip/TCPIP Stack/ARP.o"           \
	"../Commons/Microchip/TCPIP Stack/Delay.o"         \
	"../Commons/Microchip/TCPIP Stack/ENC28J60.o"      \
	"../Commons/Microchip/TCPIP Stack/Helpers.o"       \
	"../Commons/Microchip/TCPIP Stack/HTTP.o"          \
	"../Commons/Microchip/TCPIP Stack/ICMP.o"          \
	"../Commons/Microchip/TCPIP Stack/IP.o"            \
	"../Commons/Microchip/TCPIP Stack/StackTsk.o"      \
	"../Commons/Microchip/TCPIP Stack/TCP.o"           \
	                                                   \
	"../Commons/Microchip/USB/usb_descriptors.o"       \
	"../Commons/Microchip/USB/usb_device.o"            \
	"../Commons/Microchip/USB/usb_function_generic.o"  \
	"../Commons/Microchip/USB/usb_function_msd.o"      \

COMMON_HEADERS = \
	"../Commons/Microchip/Microchip.h"                 \
	"../Commons/Config.h"                              \
	"../Commons/delay_melacs.h"                        \
	"../Commons/Editable.h"                            \
	"../Commons/Flash.h"                               \
	"../Commons/Flow.h"                                \
	"../Commons/glcd_functions.h"                      \
	"../Commons/IADC.h"                                \
	"../Commons/inttypes.h"                            \
#	"../Commons/Motor.h"                               \
	"../Commons/Numeric.h"                             \
#	"../Commons/SoftPWM.h"                             \
	"../Commons/melacs_spi.h"                          \
	"../Commons/sip_relays.h"                          \
	"../Commons/Utils.h"                               \

Src/Main.o                  : $(COMMON_HEADERS) Src/Main.h Src/HMI.h Src/Log.h
Src/HMI.o                   : $(COMMON_HEADERS) Src/Main.h Src/HMI.h
Src/Log.o                   : $(COMMON_HEADERS) Src/Log.h
../Commons/Config.o         : $(COMMON_HEADERS)
../Commons/Editable.o       : $(COMMON_HEADERS)
../Commons/Flash.o          : $(COMMON_HEADERS)
../Commons/Flow.o           : $(COMMON_HEADERS)
../Commons/glcd_functions.o : $(COMMON_HEADERS)
../Commons/HardwareInfo.o   : ../Commons/HardwareInfo.h
../Commons/IADC.o           : $(COMMON_HEADERS)
#../Commons/Motor.o          : $(COMMON_HEADERS)
../Commons/Numeric.o        : ../Commons/Numeric.h
../Commons/Ports.o          : $(COMMON_HEADERS) ../Commons/PragmaConfig.h
../Commons/melacs_spi.o            : $(COMMON_HEADERS) ../Commons/OnboardRTCC.h ../Commons/OnboardADC.h
../Commons/Utils.o          : $(COMMON_HEADERS)
#../Commons/SoftPWM.o        : $(COMMON_HEADERS)

##############################################################################
.SUFFIXES : .c .asm .o

CC = mcc18 -p $(MCU) -w=3
AS = ..\Tools\mpasm /s- /l- /pPIC$(MCU)
LD = mplink /q /u _CRUNTIME /u _EXTENDEDMODE # /i
UPLOAD = Lv18PICFLASH -pPIC$(MCU) -w -Q
RM = ..\Tools\rm -f
MERGE = ..\Tools\srec_cat

$(PROJECT).hex : Application.hex "../Bootloader/Bootloader.hex"
	$(MERGE) Application.hex -intel -exclude 0x000000 0x0001F "../Bootloader/Bootloader.hex" -intel -exclude 0x1FFF8 0x1FFFE -o $@ -intel

Application.hex : $(LINKER) $(OBJS)
	$(LD) $(LDFLAGS) /o $*.cof /m $*.map $(LINKER) $(OBJS)

upload : $(PROJECT).hex
	$(UPLOAD) -f$?

rebuild : clean
	$(MAKE)

reload : rebuild
	$(MAKE) upload

clean :
	$(RM) $(OBJS) $(PROJECT).hex $(PROJECT).out $(PROJECT).lst $(PROJECT).map

.c.o :
	$(CC) $(CFLAGS) -fo=$@ $<

.asm.o :
	$(AS) $(ASFLAGS) /o$@ $<