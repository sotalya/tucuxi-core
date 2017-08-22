TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

LIBS += -lpthread


include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)

HEADERS += \
    ../../../test/tucucore/test_dosage.h \
    ../../../test/tucucore/test_intakeextractor.h \
    ../../../test/tucucore/test_operation.h \
    ../../../test/tucucore/test_intakeintervalcalculator.h \
    ../../../test/tucucore/test_concentrationcalculator.h \
    ../../../test/tucucore/test_pkmodel.h

SOURCES += \
    ../../../test/tucucore/tests.cpp
