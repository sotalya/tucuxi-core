TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

LIBS += -lpthread


include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)

HEADERS +=

SOURCES += \
    ../../../src/tucucli/tucucli.cpp
