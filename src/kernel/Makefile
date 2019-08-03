
BINARY := anvil

LIBS := ../bsp/libbsp.a

MODULE_LDFLAGS := \
    -L../bsp/ \
    -lbsp \
    -D STM32F10X_MD_VL \
    -nostartfiles \
    -fno-exceptions \
    -fno-unwind-tables \
    -fno-asynchronous-unwind-tables \
    -T $(SRCDIR)/$(ARCH)/lnfile.ld \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-move-loop-invariants \
    -Wl,--gc-sections \
    -Wl,-Map,"armcpp.map"

MODULE_CPPFLAGS := \
    -I$(SRCDIR)/../bsp \
    -I$(SRCDIR)/../bsp/stm32 \
    -I$(SRCDIR)/../bsp/stm32/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
    -I$(SRCDIR)/../bsp/stm32/CMSIS/CM3/CoreSupport \
    -I$(SRCDIR)/../bsp/stm32/STM32F10x_StdPeriph_Driver/inc/ \
    -DSTM32F10X_MD_VL \
    -nostartfiles \
    -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables

include $(PROJECT_ROOT)/make/default.mk
