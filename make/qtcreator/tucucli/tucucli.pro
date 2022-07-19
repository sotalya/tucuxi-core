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
include(../tucusign.pri)

# copy the root ca certificate in the output directory
copydata.commands = $(COPY_DIR) $$PWD/../../../src/tucusign/ca.cert.pem $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

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
