
MODULE_CFLAGS :=					\
  -nostdlib							\
  -isystem $(SRCDIR)/inc			\
  -fno-stack-protector				\
  -fno-builtin

MODULE_CFLAGS_NO_FPU :=				\
  -mno-mmx							\
  -mno-sse							\
  -mno-sse2							\
  -mno-sse3							\
  -mno-3dnow						

LIBRARY := libc.a
MODULE_CFLAGS += -static -m32 $(MODULE_CFLAGS_NO_FPU)
DIRS := string

include $(PROJECT_ROOT)/make/default.mk
