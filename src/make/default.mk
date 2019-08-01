
# The method of switching from the src directory to a build directory
# to do a build comes from Paul D. Smith. You can read the details
# at http://mad-scientist.net/make/multi-arch.html

.SUFFIXES:

ifeq (,$(findstring build/,$(CURDIR)))
############ We are in the source tree - do a few things and change ############

# Calculate the current directory relative to the PROJECT_ROOT
REL_CURDIR := $(patsubst $(PROJECT_ROOT)/%,%,$(CURDIR))

# Calculate the root of the build tree - all output will be rooted here
BUILD_ROOT := $(PROJECT_ROOT)/../build/$(REL_CURDIR)

# Pass this variable to our children so they know where the sources are
SRCDIR := $(CURDIR)

# Now change to the BUILD_ROOT and call make again
.PHONY: $(BUILD_ROOT)
$(BUILD_ROOT) :
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) -C $@ -f $(CURDIR)/Makefile SRCDIR=$(SRCDIR) REL_CURDIR=$(REL_CURDIR) $(MAKECMDGOALS)

Makefile : ;
%.mk :: ;
% :: $(BUILD_ROOT) ; :

# To clean we just remove the build directories
.PHONY: clean
clean:
	@echo    "          RM build/$(REL_CURDIR)"
	@rm -rf $(BUILD_ROOT)

else
############ We are in the build tree now - use VPATH to find our sources ############

VPATH= $(SRCDIR)

include $(PROJECT_ROOT)/make/rules.mk

endif
