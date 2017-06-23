## ----------------------------------------------------------------------------------
##
## Makefile to select the target platform
##
## ----------------------------------------------------------------------------------

ifeq ($(TARGET), LINUX)
include $(TUCUXI_ROOT)/make/linuxrules.mak
endif

ifeq ($(TARGET), VS2015)
include $(TUCUXI_ROOT)/make/VS2015Rules.mak
endif

