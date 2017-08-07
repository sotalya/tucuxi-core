
## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := tucucommon
TYPE := TEST

SOURCES := tests.cpp
INCLUDES := ../../libs/fructose-1.3/fructose/include \
            ../../libs/spdlog-master-20170622/include \
            ../../libs/date-master-20170711 \
            ../../libs/rapidxml-1.13 \
            ../../libs/botan-2.1.0/build/include

LIBS := tucucommon botan
EXTLIBS := "$(WindowsSdkDir)Lib\$(WindowsSDKVersion)um\x64\advapi32.lib" "$(WindowsSdkDir)Lib\$(WindowsSDKVersion)um\x64\user32.lib"
DEFINES := 


include $(TUCUXI_ROOT)/make/rules.mak
