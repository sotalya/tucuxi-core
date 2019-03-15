TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

LIBS += -lpthread


include(../general.pri)
include(../tucucommon.pri)
include(../tucucore.pri)

HEADERS += \
    ../../../test/tucucore/test_computingcomponentadjustments.h \
    ../../../test/tucucore/test_concentrationcalculator.h \ \
    ../../../test/tucucore/test_dosage.h \
    ../../../test/tucucore/test_drugmodels.h \
    ../../../test/tucucore/test_intakeextractor.h \
    ../../../test/tucucore/test_intakeintervalcalculator.h \
    ../../../test/tucucore/test_pkmodel.h \
    ../../../test/tucucore/test_operablegraphmanager.h \
    ../../../test/tucucore/test_operation.h \
    ../../../test/tucucore/test_nonmemdrugs.h \
    ../../../test/tucucore/drugmodels/buildimatinib.h \
    ../../../test/tucucore/drugmodels/drugmodelbuilder.h \
    ../../../test/tucucore/test_percentilecalculator.h \
    ../../../test/tucucore/test_covariateextractor.h \
    ../../../test/tucucore/test_parameterextractor.h \
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
    ../../../test/tucucore/pkmodels/constanteliminationbolus.h

SOURCES += \
    ../../../test/tucucore/tests.cpp
