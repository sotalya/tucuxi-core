## ----------------------------------------------------------------------------------
##
## Makefile for the Linux platform
##
## ----------------------------------------------------------------------------------

## ---------------------------------------------------------------
## Compiler and linker...
##
XCC = g++
XC = gcc
LD = $(XCC)
AR = ar

## ---------------------------------------------------------------
## INCLUDES, LIBS and DEFINES are set by specific makefiles...
## 
_SOURCES := $(SOURCES)																		# Source files for a given module are referenced from the module directory
_OBJS := $(patsubst %.cpp, objs/%.o, $(SOURCES))											# List of object files
_INCLUDES := $(addprefix -I, $(INCLUDES)) -I$(TUCUXI_ROOT)/src								# Include directories are referenced from Tucuxi's root directory
_LIBS := $(foreach _LIB, $(LIBS), $(TUCUXI_ROOT)/bin/$(_LIB).a) $(EXTLIBS)					# Libs are rerefenced by their name only 
_DEFINES := $(addprefix -D, $(DEFINES))

## ---------------------------------------------------------------
## Build flags.
##
CFLAGS = -g -Wall $(_INCLUDES) $(_DEFINES) -std=c++14 -ffunction-sections -fdata-sections -foptimize-sibling-calls -fpermissive -DMW_NO_TYPEINFO -D_REENTRANT  
LDFLAGS = -lpthread -lrt -Wl,--gc-sections

## ---------------------------------------------------------------
## Rules for construction of a static library
##
ifeq ($(TYPE),LIB)
build : $(_OBJS)
	$(DEL) objs/ar.mri
	$(ECHO) create objs/$(NAME).a > objs/ar.mri
	$(foreach _OBJLIB, $(_LIBS), echo addlib $(_OBJLIB) >> objs/ar.mri; )
	$(ECHO) addmod $(_OBJS) >> objs/ar.mri
	$(ECHO) save >> objs/ar.mri
	$(ECHO) end >> objs/ar.mri
	$(AR) -M < objs/ar.mri
	$(COPY) objs/$(NAME).a $(TUCUXI_ROOT)/bin/

clean:
	$(DEL) objs/*
	$(DEL) $(TUCUXI_ROOT)/bin/$(NAME).a
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ shared library
##
ifeq ($(TYPE),DLL)
CFLAGS += -fPIC
build : $(_OBJS) $(_LIBS)
	$(LD) -shared $(LDFLAGS) -o objs/$(NAME).so $(_OBJS) $(_LIBS)
	$(COPY) objs/$(NAME).so $(TUCUXI_ROOT)/bin/

clean:
	$(DEL) objs/*
	$(DEL) $(TUCUXI_ROOT)/bin/$(NAME).so
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ application
##
ifeq ($(TYPE),APP)
build : $(_OBJS) $(_LIBS)
	$(LD) -rdynamic $(LDFLAGS) -o objs/$(NAME) $(_OBJS) -Wl,--whole-archive $(_LIBS) -Wl,--no-whole-archive -ldl -lrt -lpthread
	$(COPY) objs/$(NAME) $(TUCUXI_ROOT)/bin/

clean:
	$(DEL) objs/*
	$(DEL) $(TUCUXI_ROOT)/bin/$(NAME)
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ application
##
ifeq ($(TYPE),TEST)
build : $(_OBJS) $(_LIBS)
	$(LD) -rdynamic $(LDFLAGS) -o objs/$(NAME)-test $(_OBJS) -Wl,--whole-archive $(_LIBS) -Wl,--no-whole-archive -ldl -lrt -lpthread
	$(COPY) objs/$(NAME)-test $(TUCUXI_ROOT)/bin/

clean:
	$(DEL) -f objs/*
	$(DEL) $(TUCUXI_ROOT)/bin/$(NAME)-test
endif

## ---------------------------------------------------------------
## Generic rules
##
objs/%.o: %.cpp
	$(MKDIR) -p objs
	$(XCC) -c -o objs/$*.o $(CFLAGS) $<
