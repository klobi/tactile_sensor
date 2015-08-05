# Run "make help" for target help.

TOOLPATH = $(CURDIR)/tools

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = ADAFRUITU4
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = build/TactileSensor
SRC          = spi.c uart.c master.c timer.c  sensorArray.c Descriptors.c BulkVendor.c $(LUFA_SRC_USB)#lib/usb_vendor.c
LUFA_PATH    = LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/

LD_FLAGS = -Wl,-u,vfprintf -lprintf_flt -lm

# Default target
all: hex

hex: $(TARGET).hex

post_compile: $(TARGET).hex
	@$(abspath $(TOOLPATH))/teensy_post_compile -file="$(basename $<)" -path="$(abspath $(CURDIR))/build" -tools="$(abspath $(TOOLPATH))"

clean:

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk
