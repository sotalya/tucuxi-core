TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

win32{
    exists(C:/Botan/Botan2-64) {
        CONFIG(debug, debug|release) {
            LIBS += -LC:\Botan\botan2-64\lib -lbotan \
                    -luser32
#            LIBS += -LC:\Botan\botan2-64\lib -lbotand
        }
        CONFIG(release, debug|release) {
            LIBS += -LC:\Botan\botan2-64\lib -lbotan \
                    -luser32
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
include(../tucucrypto.pri)

HEADERS += \
    ../../../test/tucucrypto/test_licensemanager.h \
    ../../../test/tucucrypto/test_cryptohelper.h \

SOURCES += \
    ../../../test/tucucrypto/tests.cpp


!win32 {
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt
}
