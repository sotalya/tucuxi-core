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
_SOURCES := $(addprefix $(TUCUXI_ROOT)\Src\$(NAME)\, $(SOURCES))						# Source files for a given module are referenced from the module directory
_INCLUDES := $(addprefix -I, $(INCLUDES)) -I$(TUCUXI_ROOT)\Src							# Include directories are referenced from Tucuxi's root directory
_LIBS := $(foreach _LIB, $(LIBS), $(TUCUXI_ROOT)\Src\$(_LIB)\$(_LIB).lib) $(EXTLIBS)	# Libs are rerefenced by their name only 
_DEFINES := $(addprefix -D, $(DEFINES))
_MODULEDIR := $(TUCUXI_ROOT)\Src\$(NAME)

## ---------------------------------------------------------------
## Build flags.
##
CCFLAG :=  -Os -c -Oy -EHsc -nologo $(_INCLUDES) $(_DEFINES)
ASFLAG := -W $(_INCLUDES)
LDFLAG_APP := -nologo -MT -Fm$(_MODULEDIR)\$(NAME).map
LDFLAG_DLL := -nologo -LD -MD -Fm$(_MODULEDIR)\$(NAME).map

## ---------------------------------------------------------------
## Object list. SOURCES is set by specific makefiles
##
_OBJS := $(patsubst %.cpp, %.o, $(_SOURCES))

## ---------------------------------------------------------------
## Rules for construction of a static library
##
ifeq ($(TYPE), LIB)
all : $(_OBJS)
	$(LIBRARIAN) /NOLOGO /VERBOSE /OUT:$(_MODULEDIR)\$(NAME).lib $(_OBJS) $(_LIBS) 

clean:
	del $(_OBJS)
	del $(_MODULEDIR)\$(NAME).lib
endif

## ---------------------------------------------------------------
## Rules for construction of a dynamic library
##
ifeq ($(TYPE), DLL)
all : $(_OBJS)
	$(LD) $(LDFLAG_DLL) -Fe$(_MODULEDIR)\$(NAME).dll $(_OBJS) $(_LIBS) 
	copy /Y /V $(_MODULEDIR)\$(NAME).dll $(TUCUXI_ROOT)\Bin

clean:
	del $(_OBJS)
	del $(_MODULEDIR)\$(NAME).dll
	del $(_MODULEDIR)\$(NAME).lib
	del $(_MODULEDIR)\$(NAME).exp
endif

## ---------------------------------------------------------------
## Rules for construction of a c++ application
##
ifeq ($(TYPE), APP)
all : $(_OBJS)
	$(LD) $(LDFLAG_APP) -Fe$(_MODULEDIR)\$(NAME).exe $(_OBJS) $(_LIBS) 
	copy /Y /V $(_MODULEDIR)\$(NAME).exe $(TUCUXI_ROOT)\Bin

clean:
	del $(_OBJS)
	del $(_MODULEDIR)\$(NAME).exe
	del $(_MODULEDIR)\$(NAME).map
endif

## ---------------------------------------------------------------
## Generic rules
##
%.o: %.cpp
	$(CXX) -Fo$*.o $< $(CCFLAG)


