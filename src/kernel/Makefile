
BINARY := anvil

LIBS := ../bsp/libbsp.a

EXTRA_DEPS := ../libc/libc.a

MODULE_LDFLAGS := \
    -L../bsp/ \
    -lbsp \
    -nostartfiles \
    -fno-exceptions \
    -fno-unwind-tables \
    -fno-asynchronous-unwind-tables \
    -T $(SRCDIR)/$(ARCH)/lnfile.ld \
    -ffunction-sections \
    -fdata-sections \
    -fno-move-loop-invariants \
    -Wl,--gc-sections \
    -Wl,-Map,"armcpp.map"
 
MODULE_CPPFLAGS := \
    -I$(SRCDIR)/../bsp \
    -I$(SRCDIR)/../libc/inc \
	-I$(PROJECT_ROOT)/libc/test \
    -nostartfiles \
    -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables

include $(PROJECT_ROOT)/make/default.mk
