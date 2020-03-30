TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET      = tucucli

unix {
    LIBS += -lpthread
}

win32 {
    include(../tinyjs.pri)
}

include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)
include(../tucuquery.pri)

HEADERS += \
    ../../../src/tucucli/clicomputer.h

SOURCES += \
    ../../../src/tucucli/tucucli.cpp \
    ../../../src/tucucli/clicomputer.cpp

!win32 {
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt
}
