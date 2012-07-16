PROJECT = Bootloader
LINKER = "Bootloader.lkr"

MCU = 18F87J50
CFLAGS = -nw=3004 -ms -Oi- -Opa- --extended -I="src"
ASFLAGS = /y+
LDFLAGS = /l ../Libraries /p $(MCU)

all : $(PROJECT).hex

OBJS = \
	"Src/Boot87J50Family.o" \
	"Src/hid.o"             \
	"Src/main.o"            \
	"Src/usb9.o"            \
	"Src/usbctrltrf.o"      \
	"Src/usbdrv.o"          \
	"Src/usbdsc.o"          \
	"Src/usbmmap.o"
	
Src/main.o : ../Commons/PragmaConfig.h


##############################################################################
.SUFFIXES : .c .asm .o

CC = mcc18 -p $(MCU) -w=3
AS = ..\Tools\mpasm /s- /l- /pPIC$(MCU)
LD = mplink /q /u _CRUNTIME /u _EXTENDEDMODE # /i
UPLOAD = Lv18PICFLASH -pPIC$(MCU) -w -Q
RM = ..\Tools\rm -f

$(PROJECT).hex : $(LINKER) $(OBJS)
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