## ----------------------------------------------------------------------------------
##
## Makefile for the Windows platform
##
## ----------------------------------------------------------------------------------

## ---------------------------------------------------------------
## Compiler and linker...
##
CXX := cl
LD := $(CXX)
LIBRARIAN := lib

## ---------------------------------------------------------------
## INCLUDES, LIBS and DEFINES are set by specific makefiles...
##
ifeq ($(TYPE), TEST)
_SOURCES := $(addprefix $(TUCUXI_ROOT)\test\$(NAME)\, $(SOURCES))						# Source files for a given module are referenced from the module directory
_MODULEDIR := $(TUCUXI_ROOT)\test\$(NAME)
else
_SOURCES := $(addprefix $(TUCUXI_ROOT)\src\$(NAME)\, $(SOURCES))						# Source files for a given module are referenced from the module directory
_MODULEDIR := $(TUCUXI_ROOT)\src\$(NAME)
endif
_INCLUDES := $(addprefix -I, $(INCLUDES)) -I$(TUCUXI_ROOT)\src							# Include directories are referenced from Tucuxi's root directory
_LIBS := $(foreach _LIB, $(LIBS), $(TUCUXI_ROOT)\src\$(_LIB)\$(_LIB).lib) $(EXTLIBS)	# Libs are rerefenced by their name only 
_DEFINES := $(addprefix -D, $(DEFINES))

## ---------------------------------------------------------------
## Build flags.
##
CCFLAG :=  -Os -c -Oy -EHsc -nologo $(_INCLUDES) $(_DEFINES)
ASFLAG := -W $(_INCLUDES)
LDFLAG_APP := -nologo -MT -Fm$(_MODULEDIR)\objs\$(NAME).map
LDFLAG_DLL := -nologo -LD -MD -Fm$(_MODULEDIR)\objs\$(NAME).map

## ---------------------------------------------------------------
## Object list. SOURCES is set by specific makefiles
##
_OBJS := $(patsubst %.cpp, objs\%.o, $(_SOURCES))

## ---------------------------------------------------------------
## Rules for construction of a static library
##
ifeq ($(TYPE), LIB)
all : $(_OBJS)
	$(LIBRARIAN) /NOLOGO /VERBOSE /OUT:$(_MODULEDIR)\objs\$(NAME).lib $(_OBJS) $(_LIBS) 

clean:
	del objs\*
endif

## ---------------------------------------------------------------
## Rules for construction of a dynamic library
##
ifeq ($(TYPE), DLL)
all : $(_OBJS)
	$(LD) $(LDFLAG_DLL) -Fe$(_MODULEDIR)\objs\$(NAME).dll $(_OBJS) $(_LIBS) 
	copy /Y /V $(_MODULEDIR)\objs\$(NAME).dll $(TUCUXI_ROOT)\bin

clean:
	del objs\*
endif

## ---------------------------------------------------------------
## Rules for construction of a c++ application
##
ifeq ($(TYPE), APP)
all : $(_OBJS)
	$(LD) $(LDFLAG_APP) -Fe$(_MODULEDIR)\objs\$(NAME).exe $(_OBJS) $(_LIBS) 
	copy /Y /V $(_MODULEDIR)\objs\$(NAME).exe $(TUCUXI_ROOT)\bin

clean:
	del objs\*
endif

## ---------------------------------------------------------------
## Rules for construction of a c++ unit test application
##
ifeq ($(TYPE), TEST)
all : $(_OBJS)
	$(LD) $(LDFLAG_APP) -Fe$(_MODULEDIR)\objs\$(NAME)-Test.exe $(_OBJS) $(_LIBS) 
	copy /Y /V $(_MODULEDIR)\objs\$(NAME)-Test.exe $(TUCUXI_ROOT)\bin

clean:
	del objs\*
endif


## ---------------------------------------------------------------
## Generic rules
##
objs\%.o: %.cpp
	$(CXX) -Foobjs\$*.o $< $(CCFLAG)


