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

## ---------------------------------------------------------------
## INCLUDES, LIBS and DEFINES are set by specific makefiles...
## 
ifeq ($(TYPE), TEST)
_SOURCES := $(addprefix $(TUCUXI_ROOT)/Test/$(NAME)/, $(SOURCES))			# Source files for a given module are referenced from the module directory
_MODULEDIR := $(TUCUXI_ROOT)/Test/$(NAME)
else
_SOURCES := $(addprefix $(TUCUXI_ROOT)/Src/$(NAME)/, $(SOURCES))			# Source files for a given module are referenced from the module directory
_MODULEDIR := $(TUCUXI_ROOT)/Src/$(NAME)
endif
_INCLUDES := $(addprefix -I, $(INCLUDES)) -I$(TUCUXI_ROOT)/Src				# Include directories are referenced from Tucuxi's root directory
_LIBS := $(foreach _LIB, $(LIBS), $(TUCUXI_ROOT)/Src/$(_LIB)/objs/$(_LIB).a) $(EXTLIBS)	# Libs are rerefenced by their name only 
_DEFINES := $(addprefix -D, $(DEFINES))

## ---------------------------------------------------------------
## Build flags.
##
CFLAGS = -g -Wall $(_INCLUDES) $(_DEFINES) -std=c++11 -ffunction-sections -fdata-sections -foptimize-sibling-calls -DMW_NO_TYPEINFO -D_REENTRANT  
LDFLAGS = -lpthread -lrt -Wl,--gc-sections

## ---------------------------------------------------------------
## Object list. SOURCES is set by specific makefiles
##
_OBJS := $(patsubst %.cpp, objs/%.o, $(filter %.cpp, $(SOURCES)))

## ---------------------------------------------------------------
## Rules for construction of a static library
##
ifeq ($(TYPE),LIB)
all : $(_OBJS)
	$(AR) rcs $(_MODULEDIR)/objs/$(NAME).a $(_OBJS) $(_LIBS)

clean:
	rm objs/*
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ shared library
##
ifeq ($(TYPE),DLL)
CFLAGS += -fPIC
all : $(_OBJS) $(_LIBS)
	$(LD) -shared $(LDFLAGS) -o $(_MODULEDIR)/$(NAME).so $(_OBJS) $(_LIBS)
	cp $(_MODULEDIR)/objs/$(NAME).so $(TUCUXI_ROOT)/bin/

clean:
	rm -f objs/*
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ application
##
ifeq ($(TYPE),APP)
all : $(_OBJS) $(_LIBS)
	$(LD) -rdynamic $(LDFLAGS) -o $(_MODULEDIR)/objs/$(NAME) $(_OBJS) -Wl,--whole-archive $(_LIBS) -Wl,--no-whole-archive -ldl -lrt -lpthread
	cp $(_MODULEDIR)/objs/$(NAME) $(TUCUXI_ROOT)/bin/

clean:
	rm -f objs/*
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ application
##
ifeq ($(TYPE),TEST)
all : $(_OBJS) $(_LIBS)
	$(LD) -rdynamic $(LDFLAGS) -o $(_MODULEDIR)/objs/$(NAME)-Test $(_OBJS) -Wl,--whole-archive $(_LIBS) -Wl,--no-whole-archive -ldl -lrt -lpthread
	cp $(_MODULEDIR)/objs/$(NAME)-Test $(TUCUXI_ROOT)/bin/

clean:
	rm -f objs/*
endif

## ---------------------------------------------------------------
## Generic rules
##
objs/%.o: %.cpp
	$(XCC) -c -o objs/$*.o $(CFLAGS) $<
