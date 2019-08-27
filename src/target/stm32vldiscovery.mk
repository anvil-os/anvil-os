
CROSS_PREFIX := arm-none-eabi
ARCH := arm
BSP := stm32

GLOBAL_CFLAGS := -std=c11 -fgnu89-inline -mcpu=cortex-m3 -mthumb -fno-exceptions
GLOBAL_CXXFLAGS := -mcpu=cortex-m3 -mthumb -fno-exceptions -fno-rtti
GLOBAL_CPPFLAGS := -g -Os -DUSE_STDPERIPH_DRIVER
GLOBAL_LDFLAGS := -g -mcpu=cortex-m3 -mthumb -fno-exceptions -fno-rtti -nostdlib
