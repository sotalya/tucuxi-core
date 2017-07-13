## ----------------------------------------------------------------------------------
##
## Makefile to select the target platform
##
## ----------------------------------------------------------------------------------

## ---------------------------------------------------------------
## Default main rule
##
all: prebuild build postbuild
	$(ECHO) Build done

## ---------------------------------------------------------------
## Default prebuild and postbuild rules
## A project makefile can redefine these rules to implement
## prebuild or postbuild commands. Make sure to defines these rules
## after the line "include $(TUCUXI_ROOT)/make/rules.mak"
prebuild:
	$(ECHO) No prebuild step

postbuild:
	$(ECHO) No postbuild step	

## ---------------------------------------------------------------
## Linux rules
##
ifeq ($(TARGET), LINUX)
MKDIR := mkdir
DEL := rm -f
COPY := cp
ECHO :=
include $(TUCUXI_ROOT)/make/linuxrules.mak
endif

## ---------------------------------------------------------------
## Windows rules
##
ifeq ($(TARGET), VS2015)
MKDIR := mkdir
DEL := del /Q
COPY := copy /Y /V
ECHO := @echo
include $(TUCUXI_ROOT)/make/vs2015rules.mak
endif

