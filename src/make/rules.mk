
# Did the caller supply a directory (list) ?
ifeq ($(DIRS),)
DIRS := .
endif

# Did the user supply SRC files ? If not we calculate them
ifeq ($(SRCS),)
ifdef SRCDIR
ASMSRCS := $(patsubst $(SRCDIR)/%,%, $(shell find $(addprefix $(SRCDIR)/, $(DIRS)) -maxdepth 1 -name "*.S"))
CSRCS := $(patsubst $(SRCDIR)/%,%, $(shell find $(addprefix $(SRCDIR)/, $(DIRS)) -maxdepth 1 -name "*.c"))
CPPSRCS := $(patsubst $(SRCDIR)/%,%, $(shell find $(addprefix $(SRCDIR)/, $(DIRS)) -maxdepth 1 -name "*.cpp"))

ARCHDIRS := $(addprefix $(SRCDIR)/, $(addsuffix /$(ARCH), $(DIRS)))
ASMSRCSARCH := $(patsubst $(SRCDIR)/%,%, $(shell find $(ARCHDIRS) -maxdepth 1 -name "*.S" 2> /dev/null))
CSRCSARCH := $(patsubst $(SRCDIR)/%,%, $(shell find $(ARCHDIRS) -maxdepth 1 -name "*.c" 2> /dev/null))
CPPSRCSARCH := $(patsubst $(SRCDIR)/%,%, $(shell find $(ARCHDIRS) -maxdepth 1 -name "*.cpp" 2> /dev/null))

SRCS := $(ASMSRCS) $(CSRCS) $(CPPSRCS) $(ASMSRCSARCH) $(CSRCSARCH) $(CPPSRCSARCH)
endif
endif

# Now we know what objects we need to build
OBJECTS := \
  $(patsubst %.S,%.o, $(filter %.S,$(SRCS)))  \
  $(patsubst %.c,%.o, $(filter %.c,$(SRCS)))  \
  $(patsubst %.cpp,%.o, $(filter %.cpp,$(SRCS)))

#$(warning objects are $(OBJECTS) )

# This brings in all the .d (dependency) files
-include $(OBJECTS:.o=.d)

CC := $(CROSS_PREFIX)-gcc
CPP := $(CROSS_PREFIX)-g++
AR := $(CROSS_PREFIX)-ar
LD := $(CROSS_PREFIX)-gcc
LDPP := $(CROSS_PREFIX)-g++
AS := $(CROSS_PREFIX)-gcc
STRIP := $(CROSS_PREFIX)-strip
SIZE := $(CROSS_PREFIX)-size

#GLOBAL_CFLAGS := -std=c99 -fgnu89-inline -mcpu=cortex-m3 -mthumb -fno-exceptions
#GLOBAL_CXXFLAGS := -mcpu=cortex-m3 -mthumb -fno-exceptions -fno-rtti
#GLOBAL_CPPFLAGS := -g -Os -DUSE_STDPERIPH_DRIVER
#GLOBAL_LDFLAGS := -g -mcpu=cortex-m3 -mthumb -fno-exceptions -fno-rtti -nostdlib
			
ALL_CFLAGS   := -Wall -Wextra -Wpedantic -g -O2 $(GLOBAL_CFLAGS)   $(GLOBAL_CPPFLAGS) $(MODULE_CFLAGS)   $(MODULE_CPPFLAGS) $(CFLAGS)   $(CPPFLAGS) -I$(SRCDIR)/$(ARCH) -I$(SRCDIR)
ALL_CXXFLAGS := -Wall -Wextra -Wpedantic -g -O2 $(GLOBAL_CXXFLAGS) $(GLOBAL_CPPFLAGS) $(MODULE_CXXFLAGS) $(MODULE_CPPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -I$(SRCDIR)/$(ARCH) -I$(SRCDIR)
ALL_LDFLAGS  := -Wall -Wextra -Wpedantic -g -O2 $(GLOBAL_LDFLAGS)  $(MODULE_LDFLAGS)  $(LDFLAGS)   

NICE_DIR_NAME = $(REL_CURDIR)$(patsubst /.,,/$(@D))

# We know how to build BINARIES
ifdef BINARY
.DEFAULT_GOAL :=
all: $(BINARY)
$(BINARY): $(OBJECTS) $(LIBS) $(EXTRA_DEPS)
	@echo "          LD $(NICE_DIR_NAME)/$(BINARY)"
	$(LD) -o $@ $(OBJECTS) $(LIBS) $(ALL_LDFLAGS)
endif

# We know how to build LIBRARIES
ifdef LIBRARY
.DEFAULT_GOAL :=
all: $(LIBRARY)
$(LIBRARY): $(OBJECTS) 
	@echo "          AR $(NICE_DIR_NAME)/$(LIBRARY)"
	$(AR) -rc $@ $(OBJECTS)
	mkdir -p /Users/gerryg/anvil-os/sysroot-arm/usr/lib
	cp $@ /Users/gerryg/anvil-os/sysroot-arm/usr/lib
	cp $@ /Users/gerryg/anvil-os/sysroot-arm/usr/lib/libg.a
endif

# As per http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
GCC_DEP_OPTS = -MT $@ -MD -MP -MF "$*.D"
# If there's an error we might end up with a half .D file. This line which is only
# executed if gcc succeeds gives the process of creating a .d file atomicity
COMMIT_DEPS = mv -f "$*.D" "$*.d"

# We know how to build C files
%.o : %.c
	@echo "          CC $(NICE_DIR_NAME)/$(<F)"
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $(ALL_CFLAGS) $(GCC_DEP_OPTS) -c -o $@ $<
	$(COMMIT_DEPS)

# We know how to build C++ files
%.o : %.cpp
	@echo "          CPP $(NICE_DIR_NAME)/$(<F)"
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CPP) $(ALL_CXXFLAGS) $(GCC_DEP_OPTS) -c -o $@ $<
	$(COMMIT_DEPS)

# We know how to build asm files
%.o : %.S
	@echo "          AS $(NICE_DIR_NAME)/$(<F)"
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(AS) $(ALL_CFLAGS) $(GCC_DEP_OPTS) -c -o $@ $<
	$(COMMIT_DEPS)
