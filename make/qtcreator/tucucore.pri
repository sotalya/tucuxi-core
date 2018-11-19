
HEADERS += \
    $$PWD/../../src/tucucore/drugmodel/drugmodel.h \
    $$PWD/../../src/tucucore/cachedexponentials.h \
    $$PWD/../../src/tucucore/concentrationcalculator.h \
    $$PWD/../../src/tucucore/concentrationprediction.h \
    $$PWD/../../src/tucucore/cyclestatisticscalculator.h \
    $$PWD/../../src/tucucore/definitions.h \
    $$PWD/../../src/tucucore/dosage.h \
    $$PWD/../../src/tucucore/drugdomainconstraintsevaluator.h \
    $$PWD/../../src/tucucore/intakeevent.h \
    $$PWD/../../src/tucucore/intakeintervalcalculator.h \
    $$PWD/../../src/tucucore/intakeextractor.h \
    $$PWD/../../src/tucucore/parameter.h \
    $$PWD/../../src/tucucore/timedevent.h \
    $$PWD/../../src/tucucore/operation.h \
    $$PWD/../../src/tucucore/pkmodels/onecompartmentbolus.h \
    $$PWD/../../src/tucucore/pkmodels/onecompartmentextra.h \
    $$PWD/../../src/tucucore/pkmodels/rkonecompartmentextra.h \
    $$PWD/../../src/tucucore/pkmodels/onecompartmentinfusion.h \
    $$PWD/../../src/tucucore/pkmodels/threecompartmentbolus.h \
    $$PWD/../../src/tucucore/pkmodels/threecompartmentextra.h \
    $$PWD/../../src/tucucore/pkmodels/threecompartmentinfusion.h \
    $$PWD/../../src/tucucore/pkmodels/twocompartmentbolus.h \
    $$PWD/../../src/tucucore/pkmodels/twocompartmentextra.h \
    $$PWD/../../src/tucucore/pkmodels/twocompartmentinfusion.h \
    $$PWD/../../src/tucucore/computingcomponent.h \
    $$PWD/../../src/tucucore/covariateextractor.h \
    $$PWD/../../src/tucucore/idatamodelservices.h \
    $$PWD/../../src/tucucore/parametersextractor.h \
    $$PWD/../../src/tucucore/percentilesprediction.h \
    $$PWD/../../src/tucucore/residualerrormodel.h \
    $$PWD/../../src/tucucore/sampleextractor.h \
    $$PWD/../../src/tucucore/targetextractor.h \
    $$PWD/../../src/tucucore/targetevaluator.h \
    $$PWD/../../src/tucucore/targetevaluationresult.h \
    $$PWD/../../src/tucucore/pkmodel.h \
    $$PWD/../../src/tucucore/intaketocalculatorassociator.h \
    $$PWD/../../src/tucucore/hardcodedoperation.h \
    $$PWD/../../src/tucucore/montecarlopercentilecalculator.h \
    $$PWD/../../src/tucucore/deriv.h \
    $$PWD/../../src/tucucore/likelihood.h \
    $$PWD/../../src/tucucore/minimize.h \
    $$PWD/../../src/tucucore/aposteriorietascalculator.h \
    $$PWD/../../src/tucucore/operablegraphmanager.h \
    $$PWD/../../src/tucucore/drugdefinitions.h \
    $$PWD/../../src/tucucore/drugmodel/activemoiety.h \
    $$PWD/../../src/tucucore/drugmodel/analyte.h \
    $$PWD/../../src/tucucore/drugmodel/activesubstance.h \
    $$PWD/../../src/tucucore/drugmodel/drugmodeldomain.h \
    $$PWD/../../src/tucucore/drugmodel/formulationandroute.h \
    $$PWD/../../src/tucucore/drugmodel/parameterdefinition.h \
    $$PWD/../../src/tucucore/drugmodel/targetdefinition.h \
    $$PWD/../../src/tucucore/drugmodel/validdose.h \
    $$PWD/../../src/tucucore/drugmodel/validduration.h \
    $$PWD/../../src/tucucore/drugmodel/timeconsiderations.h \
    $$PWD/../../src/tucucore/sampleevent.h \
    $$PWD/../../src/tucucore/targetevent.h \
    $$PWD/../../src/tucucore/drugtreatment/sample.h \
    $$PWD/../../src/tucucore/drugtreatment/target.h \
    $$PWD/../../src/tucucore/drugtreatment/drugtreatment.h \
    $$PWD/../../src/tucucore/drugmodel/covariatedefinition.h \
    $$PWD/../../src/tucucore/drugmodelimport.h \
    $$PWD/../../src/tucucore/drugtreatment/patientcovariate.h \
    $$PWD/../../src/tucucore/covariateevent.h \
    $$PWD/../../src/tucucore/computingservice/computingrequest.h \
    $$PWD/../../src/tucucore/computingservice/computingresponse.h \
    $$PWD/../../src/tucucore/computingservice/computingtrait.h \
    $$PWD/../../src/tucucore/computingservice/icomputingservice.h \
    $$PWD/../../src/tucucore/validvalues.h \
    $$PWD/../../src/tucucore/computingservice/computingaborter.h \
    $$PWD/../../src/tucucore/unit.h \
    $$PWD/../../src/tucucore/drugmodel/drugmodelmetadata.h

SOURCES += \
    $$PWD/../../src/tucucore/drugmodel/drugmodel.cpp \
    $$PWD/../../src/tucucore/cachedexponentials.cpp \
    $$PWD/../../src/tucucore/concentrationcalculator.cpp \
    $$PWD/../../src/tucucore/cyclestatisticscalculator.cpp \
    $$PWD/../../src/tucucore/intakeintervalcalculator.cpp \
    $$PWD/../../src/tucucore/intakeextractor.cpp \
    $$PWD/../../src/tucucore/dosage.cpp \
    $$PWD/../../src/tucucore/drugdomainconstraintsevaluator.cpp \
    $$PWD/../../src/tucucore/operation.cpp \
    $$PWD/../../src/tucucore/pkmodels/onecompartmentbolus.cpp \
    $$PWD/../../src/tucucore/pkmodels/onecompartmentextra.cpp \
    $$PWD/../../src/tucucore/pkmodels/rkonecompartmentextra.cpp \
    $$PWD/../../src/tucucore/pkmodels/onecompartmentinfusion.cpp \
    $$PWD/../../src/tucucore/pkmodels/threecompartmentbolus.cpp \
    $$PWD/../../src/tucucore/pkmodels/threecompartmentextra.cpp \
    $$PWD/../../src/tucucore/pkmodels/threecompartmentinfusion.cpp \
    $$PWD/../../src/tucucore/pkmodels/twocompartmentbolus.cpp \
    $$PWD/../../src/tucucore/pkmodels/twocompartmentextra.cpp \
    $$PWD/../../src/tucucore/pkmodels/twocompartmentinfusion.cpp \
    $$PWD/../../src/tucucore/computingcomponent.cpp \
    $$PWD/../../src/tucucore/covariateextractor.cpp \
    $$PWD/../../src/tucucore/parameter.cpp \
    $$PWD/../../src/tucucore/parametersextractor.cpp \
    $$PWD/../../src/tucucore/sampleextractor.cpp \
    $$PWD/../../src/tucucore/targetextractor.cpp \
    $$PWD/../../src/tucucore/targetevaluator.cpp \
    $$PWD/../../src/tucucore/targetevaluationresult.cpp \
    $$PWD/../../src/tucucore/pkmodel.cpp \
    $$PWD/../../src/tucucore/intaketocalculatorassociator.cpp \
    $$PWD/../../src/tucucore/hardcodedoperation.cpp \
    $$PWD/../../src/tucucore/montecarlopercentilecalculator.cpp \
    $$PWD/../../src/tucucore/likelihood.cpp \
    $$PWD/../../src/tucucore/aposteriorietascalculator.cpp \
    $$PWD/../../src/tucucore/operablegraphmanager.cpp \
    $$PWD/../../src/tucucore/residualerrormodel.cpp \
    $$PWD/../../src/tucucore/drugmodel/activemoiety.cpp \
    $$PWD/../../src/tucucore/drugmodel/analyte.cpp \
    $$PWD/../../src/tucucore/drugmodel/activesubstance.cpp \
    $$PWD/../../src/tucucore/drugmodel/drugmodeldomain.cpp \
    $$PWD/../../src/tucucore/drugmodel/formulationandroute.cpp \
    $$PWD/../../src/tucucore/drugmodel/parameterdefinition.cpp \
    $$PWD/../../src/tucucore/drugmodel/targetdefinition.cpp \
    $$PWD/../../src/tucucore/drugmodel/validdose.cpp \
    $$PWD/../../src/tucucore/drugmodel/validduration.cpp \
    $$PWD/../../src/tucucore/drugmodel/timeconsiderations.cpp \
    $$PWD/../../src/tucucore/drugmodelimport.cpp \
    $$PWD/../../src/tucucore/sampleevent.cpp \
    $$PWD/../../src/tucucore/drugtreatment/sample.cpp \
    $$PWD/../../src/tucucore/drugtreatment/drugtreatment.cpp \
    $$PWD/../../src/tucucore/drugtreatment/target.cpp \
    $$PWD/../../src/tucucore/targetevent.cpp \
    $$PWD/../../src/tucucore/covariateevent.cpp \
    $$PWD/../../src/tucucore/computingservice/computingrequest.cpp \
    $$PWD/../../src/tucucore/computingservice/computingresponse.cpp \
    $$PWD/../../src/tucucore/computingservice/computingtrait.cpp \
    $$PWD/../../src/tucucore/computingservice/icomputingservice.cpp \
    $$PWD/../../src/tucucore/validvalues.cpp \
    $$PWD/../../src/tucucore/computingservice/computingaborter.cpp \
    $$PWD/../../src/tucucore/unit.cpp \
    $$PWD/../../src/tucucore/drugmodel/drugmodelmetadata.cpp

DISTFILES += \
    $$PWD/../../src/tucucore/makefile
