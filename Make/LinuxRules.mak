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
_SOURCES := $(addprefix $(TUCUXI_ROOT)/Test/$(NAME)/, $(SOURCES))						# Source files for a given module are referenced from the module directory
_MODULEDIR := $(TUCUXI_ROOT)/Test/$(NAME)
else
_SOURCES := $(addprefix $(TUCUXI_ROOT)/Src/$(NAME)/, $(SOURCES))						# Source files for a given module are referenced from the module directory
_MODULEDIR := $(TUCUXI_ROOT)/Src/$(NAME)
endif
_INCLUDES := $(addprefix -I, $(INCLUDES)) -I$(TUCUXI_ROOT)/Src							# Include directories are referenced from Tucuxi's root directory
_LIBS := $(foreach _LIB, $(LIBS), $(TUCUXI_ROOT)/Src/$(_LIB)/$(_LIB).a) $(EXTLIBS)	# Libs are rerefenced by their name only 
_DEFINES := $(addprefix -D, $(DEFINES))

## ---------------------------------------------------------------
## Build flags.
##
CFLAGS = -g -Wall $(_INCLUDES) $(_DEFINES) -std=c++11 -ffunction-sections -fdata-sections -foptimize-sibling-calls -DMW_NO_TYPEINFO -D_REENTRANT  
LDFLAGS = -lpthread -lrt -Wl,--gc-sections

## ---------------------------------------------------------------
## Object list. SOURCES is set by specific makefiles
##
_OBJS := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SOURCES)))

## ---------------------------------------------------------------
## Rules for construction of a static library
##
ifeq ($(TYPE),LIB)
all : $(_OBJS)
	$(AR) rcs $(_MODULEDIR)/$(NAME).a $(_OBJS) $(_LIBS)

clean:
	del $(_OBJS)
	del $(_MODULEDIR)/$(NAME).a
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ shared library
##
ifeq ($(TYPE),DLL)
CFLAGS += -fPIC
all : $(_OBJS) $(_LIBS)
	$(LD) -shared $(LDFLAGS) -o $(_MODULEDIR)/$(NAME).so $(_OBJS) $(_LIBS)
	cp $(_MODULEDIR)/$(NAME).so $(TUCUXI_ROOT)/bin/

clean:
	rm -f $(_OBJS)
	rm -f $(NAME).so
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ application
##
ifeq ($(TYPE),APP)
all : $(_OBJS) $(_LIBS)
	$(LD) -rdynamic $(LDFLAGS) -o $(_MODULEDIR)/$(NAME) $(_OBJS) -Wl,--whole-archive $(_LIBS) -Wl,--no-whole-archive -ldl -lrt -lpthread
	cp $(_MODULEDIR)/$(NAME) $(TUCUXI_ROOT)/bin/

clean:
	rm -f $(_OBJS)
	rm -f *.map
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ application
##
ifeq ($(TYPE),TEST
all : $(_OBJS) $(_LIBS)
	$(LD) -rdynamic $(LDFLAGS) -o $(_MODULEDIR)/$(NAME)-Test $(_OBJS) -Wl,--whole-archive $(_LIBS) -Wl,--no-whole-archive -ldl -lrt -lpthread
	cp $(_MODULEDIR)/$(NAME)-Test $(TUCUXI_ROOT)/bin/

clean:
	rm -f $(_OBJS)
	rm -f *.map
endif

## ---------------------------------------------------------------
## Generic rules
##
%.o: %.cpp
	$(XCC) -c -o $*.o $(CFLAGS) $<
