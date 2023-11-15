
## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := tucucommon
TYPE := LIB

# Let's keep it in alphabetical order
SOURCES := componentmanager.cpp \
           datetime.cpp \
           duration.cpp \
           jsengine.cpp \
           logger.cpp \
           timeofday.cpp \
           translatablestring.cpp \
           unit.cpp \
           utils.cpp \
           xmlattribute.cpp \
           xmldocument.cpp \
           xmlimporter.cpp \
           xmlnode.cpp

INCLUDES := ../../libs/spdlog/include \
            ../../libs/eigen-3.3.2 \
            ../../libs/tiny-js \
            ../../libs/date/include \
            ../../libs/rapidxml \
            ../../libs/botan-2.19.3/build/include
LIBS := tinyjs
EXTLIBS :=
DEFINES :=

include $(TUCUXI_ROOT)/make/rules.mak
