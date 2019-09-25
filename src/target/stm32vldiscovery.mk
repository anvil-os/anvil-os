
CROSS_PREFIX := arm-anvil-eabi
ARCH := arm
BSP := stm32

GLOBAL_CFLAGS := -std=c11 -fgnu89-inline -mcpu=cortex-m3 -mthumb -fno-exceptions -mno-thumb-interwork
GLOBAL_CXXFLAGS := -mcpu=cortex-m3 -mthumb -fno-exceptions -fno-rtti -mno-thumb-interwork
GLOBAL_CPPFLAGS := -g -O2 -DUSE_STDPERIPH_DRIVER
GLOBAL_LDFLAGS := -g -mcpu=cortex-m3 -mthumb -fno-exceptions -fno-rtti -mno-thumb-interwork
