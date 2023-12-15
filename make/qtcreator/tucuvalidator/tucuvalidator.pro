TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET      = tucuvalidator

unix {
    LIBS += -lpthread
}

win32 {
    include(../tinyjs.pri)
}


include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)

HEADERS +=

SOURCES += \
    ../../../src/tucuvalidator/tucuvalidator.cpp


!win32 {
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt
}
