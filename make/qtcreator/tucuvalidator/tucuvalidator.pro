TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET      = tucuvalidator

LIBS += -lpthread


include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)

HEADERS +=

SOURCES += \
    ../../../src/tucuvalidator/tucuvalidator.cpp
