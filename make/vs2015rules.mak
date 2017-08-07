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
PYTHON := python

## ---------------------------------------------------------------
## SOURCES, INCLUDES, LIBS, EXTLIBS and DEFINES are set by specific makefiles...
_SOURCES := $(SOURCES)																		# Source files for a given module are referenced from the module directory
_OBJS := $(patsubst %.cpp, objs/%.o, $(SOURCES))											# List of object files
_INCLUDES := $(addprefix -I, $(INCLUDES)) -I$(TUCUXI_ROOT)\src								# Include directories are referenced from Tucuxi's root directory
_LIBS := $(foreach _LIB, $(LIBS), $(TUCUXI_ROOT)\bin\$(_LIB).lib) $(EXTLIBS)				# Libs are rerefenced by their name only 
_DEFINES := $(addprefix -D, $(DEFINES))

## ---------------------------------------------------------------
## Build flags.
##
CCFLAG :=  -Os -c -Oy -EHsc -nologo $(_INCLUDES) $(_DEFINES)
ASFLAG := -W $(_INCLUDES)
LDFLAG_APP := -nologo -MT -Fmobjs\$(NAME).map
LDFLAG_DLL := -nologo -LD -MD -Fmobjs\$(NAME).map

## ---------------------------------------------------------------
## Rules for construction of a static library
##
ifeq ($(TYPE), LIB)
build : $(_OBJS)
	$(LIBRARIAN) /NOLOGO /VERBOSE /OUT:objs\$(NAME).lib $(_OBJS) $(_LIBS) 
	$(COPY) objs\$(NAME).lib $(TUCUXI_ROOT)\bin

clean:
	if exist objs $(DEL) objs\*
	if exist $(TUCUXI_ROOT)\bin\$(NAME).lib $(DEL) $(TUCUXI_ROOT)\bin\$(NAME).lib
endif

## ---------------------------------------------------------------
## Rules for construction of a dynamic library
##
ifeq ($(TYPE), DLL)
build : $(_OBJS) postbuild
	$(LD) $(LDFLAG_DLL) -Feobjs\$(NAME).dll $(_OBJS) $(_LIBS) "$(WindowsSdkDir)Lib\$(WindowsSDKVersion)um\x64\advapi32.lib" "$(WindowsSdkDir)Lib\$(WindowsSDKVersion)um\x64\user32.lib"
	$(COPY) objs\$(NAME).lib $(TUCUXI_ROOT)\bin
	$(COPY) objs\$(NAME).dll $(TUCUXI_ROOT)\bin

clean:
	if exist objs $(DEL) objs\*
	if exist $(TUCUXI_ROOT)\bin\$(NAME).lib $(DEL) $(TUCUXI_ROOT)\bin\$(NAME).lib
	if exist $(TUCUXI_ROOT)\bin\$(NAME).dll $(DEL) $(TUCUXI_ROOT)\bin\$(NAME).dll
endif

## ---------------------------------------------------------------
## Rules for construction of a c++ application
##
ifeq ($(TYPE), APP)
build : $(_OBJS) postbuild
	$(LD) $(LDFLAG_APP) -Feobjs\$(NAME).exe $(_OBJS) $(_LIBS) 
	$(COPY) objs\$(NAME).exe $(TUCUXI_ROOT)\bin

clean:
	if exist objs $(DEL) objs\*
	if exist $(TUCUXI_ROOT)\bin\$(NAME).exe $(DEL) $(TUCUXI_ROOT)\bin\$(NAME).exe
endif

## ---------------------------------------------------------------
## Rules for construction of a c++ unit test application
##
ifeq ($(TYPE), TEST)
build : $(_OBJS) postbuild
	$(LD) $(LDFLAG_APP) -Feobjs\$(NAME)-Test.exe $(_OBJS) $(_LIBS) 
	$(COPY) objs\$(NAME)-test.exe $(TUCUXI_ROOT)\bin

clean:
	if exist objs $(DEL) objs\*
	if exist $(TUCUXI_ROOT)\bin\$(NAME)-test.exe $(DEL) $(TUCUXI_ROOT)\bin\$(NAME)-test.exe
endif

## ---------------------------------------------------------------
## Generic rules
##
objs/%.o: %.cpp
	if not exist objs $(MKDIR) objs
	if not exist $(dir objs\$*.o) $(MKDIR) $(subst /,\, $(dir objs\$*.o))
	$(CXX) -Foobjs/$*.o $< $(CCFLAG)
