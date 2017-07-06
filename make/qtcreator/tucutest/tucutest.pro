TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

INCLUDEPATH += ../../../src \
    ../../../libs/spdlog-master-20170622/include/ \
    ../../../libs/boost-1.63.0 \
    ../../../libs/eigen-3.3.2/ \
    ../../../libs/fructose-1.3/fructose/include

HEADERS += \
    ../../../src/tucucommon/component.h \
    ../../../src/tucucommon/componentmanager.h \
    ../../../src/tucucommon/ilogger.h \
    ../../../src/tucucommon/interface.h \
    ../../../src/tucucommon/logger.h \
    ../../../src/tucucommon/loggerhelper.h \
    ../../../src/tucucommon/utils.h \
    ../../../src/tucucore/cachedlogarithms.h \
    ../../../src/tucucore/concentrationcalculator.h \
    ../../../src/tucucore/definitions.h \
    ../../../src/tucucore/intakeevent.h \
    ../../../src/tucucore/intakeintervalcalculator.h \
    ../../../src/tucucore/onecompartmentbolus.h \
    ../../../src/tucucore/onecompartmentextra.h \
    ../../../src/tucucore/parameter.h \
    ../../../src/tucucore/timedevent.h \
    ../../../test/tucucommon/test_componentmanager.h \
    ../../../test/tucucommon/test_logger.h

SOURCES += \
    ../../../src/tucucommon/componentmanager.cpp \
    ../../../src/tucucommon/logger.cpp \
    ../../../src/tucucommon/utils.cpp \
    ../../../src/tucucore/cachedlogarithms.cpp \
    ../../../src/tucucore/concentrationcalculator.cpp \
    ../../../src/tucucore/intakeintervalcalculator.cpp \
    ../../../src/tucucore/onecompartmentbolus.cpp \
    ../../../src/tucucore/onecompartmentextra.cpp \
    ../../../test/tucucommon/tests.cpp
