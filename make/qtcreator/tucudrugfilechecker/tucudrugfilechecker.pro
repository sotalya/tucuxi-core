TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET      = tucudrugfilechecker

LIBS += -lpthread


include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)

HEADERS +=

SOURCES += \
    ../../../src/tucudrugfilechecker/tucudrugfilechecker.cpp
