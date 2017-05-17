## ----------------------------------------------------------------------------------
##
## Makefile to select the target platform
##
## ----------------------------------------------------------------------------------

ifeq ($(TARGET), LINUX)
include $(TUCUXI_ROOT)/Make/LinuxRules.mak
endif

ifeq ($(TARGET), VS2015)
include $(TUCUXI_ROOT)/Make/VS2015Rules.mak
endif

