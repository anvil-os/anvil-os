
PROJECT_ROOT := $(CURDIR)

SUBDIRS = bsp kernel 

#ifeq ($(wildcard target.mk),)
#$(warning "*** ERROR: target not specified")
#endif

-include $(PROJECT_ROOT)/make/target.mk

# Todo: check that ARCH and BSP etc are defined

.EXPORT_ALL_VARIABLES:

.PHONY all: $(SUBDIRS)

.PHONY install: $(SUBDIRS)

.PHONY clean: $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@$(MAKE) -s -C $@ PROJECT_ROOT=$(PROJECT_ROOT) $(MAKECMDGOALS) 

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),install)
$(MAKECMDGOALS):
	@echo Switching to target $(MAKECMDGOALS)
	@if [ ! -f target/$(MAKECMDGOALS).mk ]; \
	then  \
		echo "*** ERROR: target/$(MAKECMDGOALS).mk not found" ; \
	else   \
		echo "include $(PROJECT_ROOT)/target/$(MAKECMDGOALS).mk" > make/target.mk ; \
	fi
endif
endif
