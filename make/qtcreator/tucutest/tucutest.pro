TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

win32{
    TARGET = tucutest.exe

    exists(C:/Botan/Botan2-64) {
        CONFIG(debug, debug|release) {
            LIBS += -LC:\Botan\botan2-64\lib -lbotan
#            LIBS += -LC:\Botan\botan2-64\lib -lbotand
        }
        CONFIG(release, debug|release) {
            LIBS += -LC:\Botan\botan2-64\lib -lbotan
        }
        INCLUDEPATH += C:\Botan\botan2-64\include\botan-2
    }
    else {
        error("Error: Botan-64 directory not found in C:/Botan")
    }

    LIBS += Iphlpapi.lib
}

include(../general.pri)
include(../tucucommon.pri)
include(../tinyjs.pri)

HEADERS += \
    ../../../test/tucucommon/test_componentmanager.h \
    ../../../test/tucucommon/test_logger.h \
    ../../../test/tucucommon/test_datetime.h \
    ../../../test/tucucommon/test_licensemanager.h \
    ../../../test/tucucommon/test_cryptohelper.h \
    ../../../test/tucucommon/test_scriptengine.h

SOURCES += \
    ../../../test/tucucommon/tests.cpp
