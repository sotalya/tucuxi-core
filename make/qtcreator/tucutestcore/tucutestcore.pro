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


include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)

HEADERS += \
    ../../../test/tucucore/drugmodels/buildpkasymptotic.h \
    ../../../test/tucucore/drugmodels/test_michaelismenten1comp.h \
    ../../../test/tucucore/drugmodels/test_michaelismenten2comp.h \
    ../../../test/tucucore/drugmodels/test_pkasymptotic.h \
    ../../../test/tucucore/pkmodels/multiconstanteliminationbolus.h \
    ../../../test/tucucore/pkmodels/pkasymptotic.h \
    ../../../test/tucucore/pkmodels/test_rkmichaelismentenenzyme.h \
    ../../../test/tucucore/test_cachecomputing.h \
    ../../../test/tucucore/test_computingcomponentadjustments.h \
    ../../../test/tucucore/test_concentrationcalculator.h \ \
    ../../../test/tucucore/test_dosage.h \
    ../../../test/tucucore/test_drugmodels.h \
    ../../../test/tucucore/test_intakeextractor.h \
    ../../../test/tucucore/test_intakeintervalcalculator.h \
    ../../../test/tucucore/test_multiconstanteliminationbolus.h \
    ../../../test/tucucore/test_multiconcentrationcalculator.h \
    ../../../test/tucucore/test_parameter.h \
    ../../../test/tucucore/test_pkmodel.h \
    ../../../test/tucucore/test_operablegraphmanager.h \
    ../../../test/tucucore/test_operation.h \
    ../../../test/tucucore/test_nonmemdrugs.h \
    ../../../test/tucucore/drugmodels/buildimatinib.h \
    ../../../test/tucucore/drugmodels/drugmodelbuilder.h \
    ../../../test/tucucore/test_percentilecalculator.h \
    ../../../test/tucucore/test_covariateextractor.h \
    ../../../test/tucucore/test_parameterextractor.h \
    ../../../test/tucucore/test_targetevaluator.h \
    ../../../test/tucucore/testutils.h \
    ../../../test/tucucore/test_drugmodelimport.h \
    ../../../test/tucucore/test_drugdomainconstraintsevaluator.h \
    ../../../test/tucucore/test_computingcomponentconcentration.h \
    ../../../test/tucucore/test_computingcomponentpercentiles.h \
    ../../../test/tucucore/test_operationcollection.h \
    ../../../test/tucucore/drugmodels/test_drug_tobramycin.h \
    ../../../test/tucucore/drugmodels/test_drug_vancomycin.h \
    ../../../test/tucucore/test_targetextractor.h \
    ../../../test/tucucore/test_pertinenttimescalculator.h \
    ../../../test/tucucore/test_residualerrormodel.h \
    ../../../test/tucucore/test_sampleextractor.h \
    ../../../test/tucucore/test_cyclestatistics.h \
    ../../../test/tucucore/pkmodels/constanteliminationbolus.h \
    ../../../test/tucucore/drugmodels/buildconstantelimination.h \
    ../../../test/tucucore/drugmodels/test_constanteliminationbolus.h \
    ../../../test/tucucore/drugmodels/buildmultianalytesmultiactivemoieties.h \
    ../../../test/tucucore/drugmodels/test_multianalytesmultiactivemoieties.h

SOURCES += \
    ../../../test/tucucore/tests.cpp

# We need the test PK models for successful tests
DEFINES += DRUGMODELTESTS

!win32 {
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt

}
