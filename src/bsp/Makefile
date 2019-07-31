
ifneq ($(SRCDIR),)
include $(SRCDIR)/$(BSP)/Makefile 
endif

MODULE_CPPFLAGS := $(BSP_CFLAGS) $(BSP_INCLUDE_PATHS)
  
SRCS = $(addprefix $(BSP)/, $(BSP_SRCS))
 
LIBRARY := libbsp.a
MODULE_CFLAGS += -static 
INSTALLDIR := /lib

include $(BUILDROOT)/default.mk
