
## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := tucucommon
TYPE := LIB

# Let's keep it in alphabetical order
SOURCES := componentmanager.cpp \
           cryptohelper.cpp \
           datetime.cpp \
           duration.cpp \
           jsengine.cpp \
           licensemanager.cpp \
           logger.cpp \
           systeminfo.cpp \
           timeofday.cpp \
					 translatablestring.cpp \
           utils.cpp \
           xmlattribute.cpp \
           xmldocument.cpp \
           xmlnode.cpp

INCLUDES := ../../libs/spdlog-master-20170622/include \
            ../../libs/eigen-3.3.2 \
            ../../libs/tiny-js-master-20170629 \
            ../../libs/date-master-20170711 \
            ../../libs/rapidxml-1.13 \
            ../../libs/botan-2.1.0/build/include
LIBS := tinyjs
EXTLIBS :=
DEFINES :=

include $(TUCUXI_ROOT)/make/rules.mak
