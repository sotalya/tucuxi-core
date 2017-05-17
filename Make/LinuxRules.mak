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
_INCLUDES = $(addprefix -I, $(INCLUDES)) -I../Src
_DEFINES = $(addprefix -D, $(DEFINES))
_LIBS1 = $(addprefix $(YDEEROOTDIR)/bin/LinuxDebug/, $(addsuffix Lib.a, $(LIBS)))
_LIBS2 = $(_LIBS1) $(SYSLIBS)

## ---------------------------------------------------------------
## Build flags.
##
#CFLAGS = -g -Wall $(_INCLUDES) $(_DEFINES) -ffunction-sections -fdata-sections -foptimize-sibling-calls -fno-implicit-templates -DMW_NO_TYPEINFO -DMW_NO_WIN32 -DLINUX -D_REENTRANT
CFLAGS = -g -Wall $(_INCLUDES) $(_DEFINES) -ffunction-sections -fdata-sections -foptimize-sibling-calls -DMW_NO_TYPEINFO -DMW_NO_WIN32 -DLINUX -D_REENTRANT  
#LDFLAGS	= -nostartfiles -L$(PKG_INSTALL_DIR)/lib -Wl,--gc-sections -Wl,--Map -Wl,MapFile.map
LDFLAGS	= -lpthread -lrt -Wl,--gc-sections

## ---------------------------------------------------------------
## Object list. SOURCES is set by specific makefiles
##
OBJS := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SOURCES))) $(patsubst %.txt, %.o, $(filter %.txt, $(SOURCES)))

## ---------------------------------------------------------------
## Rules for construction of c and c++ shared library
##
ifeq ($(TYPE),DLL)
CFLAGS += -fPIC
all : $(OBJS) $(_LIBS1)
	$(LD) -shared $(LDFLAGS) -o $(NAME).so $(OBJS) $(_LIBS2) 
	cp $(NAME).so $(YDEEROOTDIR)/bin/LinuxDebug

clean:
	rm -f $(addsuffix /*.o, $(SRCDIRS))
	rm -f $(NAME).so
endif

## ---------------------------------------------------------------
## Rules for construction of c and c++ application
##
ifeq ($(TYPE),APP)
all : $(OBJS) $(_LIBS1)
	$(LD) -rdynamic $(LDFLAGS) -o $(NAME) $(OBJS) -Wl,--whole-archive $(_LIBS2) -Wl,--no-whole-archive -ldl -lrt -lpthread
	cp $(NAME) $(YDEEROOTDIR)/bin/LinuxDebug

clean:
	rm -f $(addsuffix /*.o, $(SRCDIRS))	
	rm -f *.map
endif

## ---------------------------------------------------------------
## Generic rules
##
%.o: %.cpp
	$(XCC) -c -o $*.o $(CFLAGS) $<

%.o: %.txt	
	objcopy --input binary --output elf32-i386 --binary-architecture i386 $*.txt $*.o

