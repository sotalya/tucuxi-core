
## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := botan
TYPE := LIB

SOURCES := botan_all.cpp
INCLUDES := 

LIBS := 
EXTLIBS := 
DEFINES :=

include $(TUCUXI_ROOT)/make/rules.mak

prebuild: 
	$(COPY) makefile makefile.tmp
	python configure.py --minimized-build --single-amalgamation-file --amalgamation --disable-shared
	$(COPY) makefile.tmp makefile
	$(DEL) makefile.tmp


