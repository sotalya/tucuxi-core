TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread


include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)

HEADERS += \
    ../../../test/tucucommon/test_componentmanager.h \
    ../../../test/tucucommon/test_logger.h

SOURCES += \
    ../../../test/tucucommon/tests.cpp
