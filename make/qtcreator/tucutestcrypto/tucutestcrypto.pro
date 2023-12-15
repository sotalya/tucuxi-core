TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../botan.pri)
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
