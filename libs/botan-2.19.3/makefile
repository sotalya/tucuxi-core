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

ifeq ($(TARGET), WINDOWS)
include SourceWin.mak
endif

INCLUDES := build/include \
            build/include/external

LIBS := 
EXTLIBS := 
DEFINES :=

include $(TUCUXI_ROOT)/make/rules.mak

prebuild: 
	$(IFEXIST) build $(THEN) $(RMDIR) build $(ENDIF)
	$(IFEXIST) objs $(THEN) $(RMDIR) objs $(ENDIF)
	$(MKDIR) build
	$(MKDIR) objs
	$(COPY) makefile makefile.tmp
	$(PYTHON) configure.py --disable-shared --without-documentation  
	$(DEL) Makefile
	$(COPY) makefile.tmp makefile
	$(DEL) makefile.tmp

