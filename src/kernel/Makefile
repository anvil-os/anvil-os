
BINARY := anvil

LIBS := ../bsp/libbsp.a

MODULE_LDFLAGS := \
    -L../bsp/ \
    -lbsp \
    -nostartfiles \
    -nostdlib \
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
    -nostartfiles \
    -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables

include $(PROJECT_ROOT)/make/default.mk
