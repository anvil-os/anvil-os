
MODULE_CFLAGS :=					\
  -nostdlib							\
  -isystem $(SRCDIR)/inc			\
  -fno-stack-protector				\
  -fno-builtin						\
  -I $(PROJECT_ROOT)/bsp

LIBRARY := libc.a
MODULE_CFLAGS += -static
DIRS := string ctype stdlib errno stdio threads time anvil test

include $(PROJECT_ROOT)/make/default.mk
