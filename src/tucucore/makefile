
## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := tucucore
TYPE := LIB

# Let's keep it in alphabetical order
SOURCES := \
           drugmodel/activemoiety.cpp \
           drugmodel/activesubstance.cpp \
           drugmodel/analyte.cpp \
           drugmodel/drugmodel.cpp \
           drugmodel/drugmodeldomain.cpp \
           drugmodel/drugmodelmetadata.cpp \
           drugmodel/errormodel.cpp \
           drugmodel/formulationandroute.cpp \
           drugmodel/parameterdefinition.cpp \
           drugmodel/targetdefinition.cpp \
           drugmodel/timeconsiderations.cpp \
           drugmodel/validdose.cpp \
           drugmodel/validduration.cpp \
           drugtreatment/drugtreatment.cpp \
           drugtreatment/sample.cpp \
           drugtreatment/target.cpp \
           computingservice/computingrequest.cpp \
           computingservice/computingresponse.cpp \
           computingservice/computingtrait.cpp \
           aposteriorietascalculator.cpp \
           cachecomputing.cpp \
           cachedexponentials.cpp \
           concentrationcalculator.cpp \
           computingadjustments.cpp \
           computingcomponent.cpp \
           computingutils.cpp \
           covariateevent.cpp \
           covariateextractor.cpp \
           cyclestatisticscalculator.cpp \
           dosage.cpp \
           drugdomainconstraintsevaluator.cpp \
           drugfilevalidator.cpp \
           drugmodelchecker.cpp \
           drugmodelimport.cpp \
           drugmodelrepository.cpp \
           generalextractor.cpp \
           hardcodedoperation.cpp \
           intakeextractor.cpp \
           intakeevent.cpp \
           intakeintervalcalculator.cpp \
           intakeintervalcalculatoranalytical.cpp \
           intakeintervalcalculatorrk4.cpp \
           intaketocalculatorassociator.cpp \
           likelihood.cpp \
           montecarlopercentilecalculator.cpp \
           multicomputingadjustments.cpp \
           multicomputingcomponent.cpp \
           multiconcentrationcalculator.cpp \
           multilikelihood.cpp \
           multimontecarlopercentilecalculator.cpp \
           operablegraphmanager.cpp \
           operation.cpp \
           overloadevaluator.cpp \
           parameter.cpp \
           parametersextractor.cpp \
           pkmodel.cpp \
           residualerrormodel.cpp \
           residualerrormodelextractor.cpp \
           sampleevent.cpp \
           sampleextractor.cpp \
           targetevent.cpp \
           targetextractor.cpp \
           targetevaluator.cpp \
           targetevaluationresult.cpp \
           treatmentdrugmodelcompatibilitychecker.cpp \
           validvalues.cpp \
           pkmodels/onecompartmentbolus.cpp \
           pkmodels/onecompartmentextra.cpp \
           pkmodels/onecompartmentextralag.cpp \
           pkmodels/onecompartmentinfusion.cpp \
           pkmodels/rkmichaelismentenenzyme.cpp \
           pkmodels/rkmichaelismentenonecomp.cpp \
           pkmodels/rkmichaelismententwocomp.cpp \
           pkmodels/rkmichaelismententwocompvmaxamount.cpp \
           pkmodels/rkonecompartmentextra.cpp \
           pkmodels/rkonecompartmentgammaextra.cpp \
           pkmodels/rktwocompartmenterlang.cpp \
           pkmodels/threecompartmentbolus.cpp \
           pkmodels/threecompartmentextra.cpp \
           pkmodels/threecompartmentinfusion.cpp \
           pkmodels/twocompartmentbolus.cpp \
           pkmodels/twocompartmentextra.cpp \
           pkmodels/twocompartmentextralag.cpp \
           pkmodels/twocompartmentinfusion.cpp

INCLUDES := . \
            ../../libs/eigen-3.3.2 \
            ../../libs/boost-1.83.0 \
            ../../libs/date/include \
            ../../libs/spdlog/include \
            ../../libs/rapidjson/include \
            ../../libs/tiny-js-master-20170629 \
LIBS :=
EXTLIBS :=
DEFINES :=

include $(TUCUXI_ROOT)/make/rules.mak
