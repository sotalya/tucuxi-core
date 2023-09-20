## ----------------------------------------------------------------------------------
##
## Makefile to select the target platform
##
## ----------------------------------------------------------------------------------

## ---------------------------------------------------------------
## Default main rule
## This way of running make for the different step is needed to ensure
## a correct execution order.
all:
	make prebuild
	make build
	make postbuild

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
DEL := rm -rf
COPY := cp
ECHO := @echo
RMDIR := $(DEL)
IFEXIST := if [ -e 
THEN :=  ]; then
ENDIF := ; fi
include $(TUCUXI_ROOT)/make/linuxrules.mak
endif

## ---------------------------------------------------------------
## Windows rules
##
ifeq ($(TARGET), WINDOWS)
MKDIR := mkdir
DEL := del /Q
COPY := copy /Y /V
ECHO := @echo
RMDIR := rmdir /Q /S
IFEXIST := if exist
THEN := 
ENDIF :=
include $(TUCUXI_ROOT)/make/vs2015rules.mak
endif

