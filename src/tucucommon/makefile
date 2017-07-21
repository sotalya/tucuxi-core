
## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := tucucommon
TYPE := LIB

SOURCES := componentmanager.cpp \
           logger.cpp \
           utils.cpp \
           jsengine.cpp \
           duration.cpp \
           timeofday.cpp \
           datetime.cpp \
           xmldocument.cpp \
           xmlnode.cpp \
           xmlattribute.cpp \
           systeminfo.cpp \
           licensemanager.cpp \
           cryptohelper.cpp
INCLUDES := ../../libs/spdlog-master-20170622/include \
            ../../libs/tiny-js-master-20170629 \
            ../../libs/date-master-20170711 \
            ../../libs/rapidxml-1.13 \
            ../../libs/botan-2.1.0/build/include
LIBS := tinyjs botan
EXTLIBS := 
DEFINES := 

include $(TUCUXI_ROOT)/make/rules.mak
