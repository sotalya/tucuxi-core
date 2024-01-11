TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../botan.pri)
include(../general.pri)
include(../tucucommon.pri)
include(../tucucrypto.pri)
include(../gtest.pri)

HEADERS += \

SOURCES += \
    ../../../test/tucucrypto/gtest_crypto.cpp \
    ../../../test/tucucrypto/gtest_cryptohelper.cpp \
    ../../../test/tucucrypto/gtest_licensemanager.cpp


!win32 {
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt
}
