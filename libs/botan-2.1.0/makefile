
## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := botan
TYPE := LIB


ifeq ($(TARGET), LINUX)
include SourceLinux.mak
endif

ifeq ($(TARGET), VS2015)
include SourceWin.mak
endif

INCLUDES := build/include \
            build/include/external

LIBS := 
EXTLIBS := 
DEFINES :=

include $(TUCUXI_ROOT)/make/rules.mak

prebuild: 
	if exist build $(RMDIR) build
	if exist objs $(RMDIR) objs
	$(MKDIR) build
	$(MKDIR) objs
	$(COPY) makefile makefile.tmp
	python configure.py --disable-shared
	$(DEL) Makefile
	$(COPY) makefile.tmp makefile
	$(DEL) makefile.tmp


