
MODULE_CFLAGS :=					\
  -nostdlib							\
  -isystem $(SRCDIR)/inc			\
  -fno-stack-protector				\
  -fno-builtin

LIBRARY := libc.a
MODULE_CFLAGS += -static
DIRS := string

include $(PROJECT_ROOT)/make/default.mk
