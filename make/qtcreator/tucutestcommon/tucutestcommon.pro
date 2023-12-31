TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../botan.pri)
include(../general.pri)
include(../tucucommon.pri)
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
    ../../../test/tucucommon/test_componentmanager.h \
    ../../../test/tucucommon/test_logger.h \
    ../../../test/tucucommon/test_datetime.h \
    ../../../test/tucucommon/test_scriptengine.h \
    ../../../test/tucucommon/test_unit.h \
    ../../../test/tucucommon/test_xmlimporter.h

SOURCES += \
    ../../../test/tucucommon/tests.cpp


!win32 {
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt
}
