TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET      = tuculicense

LIBS += -lpthread


include(../general.pri)
include(../tucucommon.pri)
include(../tucucrypto.pri)

SOURCES += \
    ../../../src/tuculicense/main.cpp


!win32 {
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt
}
