TARGET = Appel
OBJS = main.o glib2d.o pgeWav.o


CFLAGS = -O3
CXXFLAHS = $(CFLAGS) -std=c++14 -fno-rtti
ASFLAGS = $(CFLAGS)

#PSP
BUILD_PRX = 1
PSP_FW_VERSION = 500
PSP_LARGE_MEMORY = 1
LIBS = -lpng -ljpeg -lz -lpspgu -lm -lpspvram -lpspaudiolib -lpspaudio -lpsppower

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Appel
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak