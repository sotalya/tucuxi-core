TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

unix {
    LIBS += -lpthread
}

win32 {
    include(../tinyjs.pri)
    QMAKE_CXXFLAGS += -bigobj
}

config_coverage {
    QMAKE_CXXFLAGS += --coverage
    QMAKE_CXXFLAGS += -O0
    QMAKE_LFLAGS += --coverage
    # After executing the tests, do:
    # gcovr . -r ../../../../../src/ -r ../../../../../src/ --html report.html
    # lcov --no-external --capture --directory . --base-directory ../../../../../src/ --output-file report.info
}


include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)
include(../gtest.pri)

HEADERS += \
    $$PWD/../../../test/tucucore/drugmodels/buildmultianalytessingleactivemoiety.h \
    $$PWD/../../../test/tucucore/drugmodels/buildconstantelimination.h \
    $$PWD/../../../test/tucucore/drugmodels/buildimatinib.h \
    $$PWD/../../../test/tucucore/drugmodels/buildmultianalytesmultiactivemoieties.h \
    $$PWD/../../../test/tucucore/drugmodels/buildpkasymptotic.h \
    $$PWD/../../../test/tucucore/drugmodels/drugmodelbuilder.h \\
    $$PWD/../../../test/tucucore/gtest_core.h \
    $$PWD/../../../test/tucucore/gtest_drugmodelimport_xmlstrings.h \
    $$PWD/../../../test/tucucore/pkmodels/constanteliminationbolus.h \
    $$PWD/../../../test/tucucore/pkmodels/multiconstanteliminationbolus.h \
    $$PWD/../../../test/tucucore/pkmodels/pkasymptotic.h \
    $$PWD/../../../test/tucucore/testutils.h


SOURCES += \
    $$PWD/../../../test/tucucore/drugmodels/gtest_constanteliminationbolus.cpp \
    $$PWD/../../../test/tucucore/drugmodels/gtest_drug_tobramycin.cpp \
    $$PWD/../../../test/tucucore/drugmodels/gtest_drug_vancomycin.cpp \
    $$PWD/../../../test/tucucore/drugmodels/gtest_michaelismenten1comp.cpp \
    $$PWD/../../../test/tucucore/drugmodels/gtest_michaelismenten2comp.cpp \
    $$PWD/../../../test/tucucore/drugmodels/gtest_multianalytesmultiactivemoieties.cpp \
    $$PWD/../../../test/tucucore/drugmodels/gtest_multianalytessingleactivemoiety.cpp \
    $$PWD/../../../test/tucucore/drugmodels/gtest_pkasymptotic.cpp \
    $$PWD/../../../test/tucucore/gtest_core.cpp \
    $$PWD/../../../test/tucucore/gtest_cachecomputing.cpp \
    $$PWD/../../../test/tucucore/gtest_computingcomponentadjustments.cpp \
    $$PWD/../../../test/tucucore/gtest_computingcomponentattimes.cpp \
    $$PWD/../../../test/tucucore/gtest_computingcomponentconcentration.cpp \
    $$PWD/../../../test/tucucore/gtest_computingcomponentpercentiles.cpp \
    $$PWD/../../../test/tucucore/gtest_concentrationcalculator.cpp \
    $$PWD/../../../test/tucucore/gtest_covariateextractor.cpp \
    $$PWD/../../../test/tucucore/gtest_cyclestatistics.cpp \
    $$PWD/../../../test/tucucore/gtest_dosage.cpp \
    $$PWD/../../../test/tucucore/gtest_drugdomainconstraintsevaluator.cpp \
    $$PWD/../../../test/tucucore/gtest_drugmodelimport.cpp \
    $$PWD/../../../test/tucucore/gtest_drugmodels.cpp \
    $$PWD/../../../test/tucucore/gtest_intakeextractor.cpp \
    $$PWD/../../../test/tucucore/gtest_intakeintervalcalculator.cpp \
    $$PWD/../../../test/tucucore/gtest_likelihood.cpp \
    $$PWD/../../../test/tucucore/gtest_multiconcentrationcalculator.cpp \
    $$PWD/../../../test/tucucore/gtest_multiconstanteliminationbolus.cpp \
    $$PWD/../../../test/tucucore/gtest_multilikelihood.cpp \
    $$PWD/../../../test/tucucore/gtest_nonmemdrugs.cpp \
    $$PWD/../../../test/tucucore/gtest_operablegraphmanager.cpp \
    $$PWD/../../../test/tucucore/gtest_operation.cpp \
    $$PWD/../../../test/tucucore/gtest_operationcollection.cpp \
    $$PWD/../../../test/tucucore/gtest_parameter.cpp \
    $$PWD/../../../test/tucucore/gtest_parameterextractor.cpp \
    $$PWD/../../../test/tucucore/gtest_percentilecalculator.cpp \
    $$PWD/../../../test/tucucore/gtest_pertinenttimescalculator.cpp \
    $$PWD/../../../test/tucucore/gtest_pkmodel.cpp \
    $$PWD/../../../test/tucucore/gtest_residualerrormodel.cpp \
    $$PWD/../../../test/tucucore/gtest_sampleextractor.cpp \
    $$PWD/../../../test/tucucore/gtest_targetevaluator.cpp \
    $$PWD/../../../test/tucucore/gtest_targetextractor.cpp \
    $$PWD/../../../test/tucucore/pkmodels/gtest_rkmichaelismentenenzyme.cpp \
    $$PWD/../../../test/tucucore/pkmodels/gtest_rkmichaelismententwocompvmaxamount.cpp \
    $$PWD/../../../test/tucucore/pkmodels/gtest_twocompartmentextralag.cpp

# We need the test PK models for successful tests
DEFINES += DRUGMODELTESTS

!win32 {
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt

}
