TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

win32{
    TARGET = tucutestquery.exe

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
include(../tucucore.pri)
include(../tucuquery.pri)
include(../tinyjs.pri)

config_coverage {
    QMAKE_CXXFLAGS += --coverage
    QMAKE_CXXFLAGS += -O0
    QMAKE_LFLAGS += --coverage
    # After executing the tests, do:
    # gcovr . -r ../../../../../src/ -r ../../../../../src/ --html report.html
    # lcov --no-external --capture --directory . --base-directory ../../../../../src/ --output-file report.info
}


HEADERS += \
    ../../../test/tucuquery/test_dosageimportexport.h \
    ../../../test/tucuquery/test_query1comp.h

SOURCES += \
    ../../../test/tucuquery/tests.cpp


!win32 {
    LIBS += -lpthread
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt
}
