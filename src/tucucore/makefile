
## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := tucucore
TYPE := LIB

# Let's keep it in alphabetical order
SOURCES := \
           drugmodel/activesubstance.cpp \
           drugmodel/analyte.cpp \
           drugmodel/drugmodel.cpp \
           drugmodel/drugmodeldomain.cpp \
           drugmodel/formulationandroute.cpp \
           drugmodel/parameterdefinition.cpp \
           drugmodel/targetdefinition.cpp \
		   aposteriorietascalculator.cpp \
           cachedexponentials.cpp \
           concentrationcalculator.cpp \
           corecomponent.cpp \
           covariateextractor.cpp \
           dosage.cpp \
           drugtreatment.cpp \
           hardcodedoperation.cpp \
           intakeextractor.cpp \
           intakeintervalcalculator.cpp \
           intaketocalculatorassociator.cpp \
		   likelihood.cpp \
		   montecarlopercentilecalculator.cpp \
           onecompartmentbolus.cpp \
           onecompartmentextra.cpp \
           onecompartmentinfusion.cpp \
		   operablegraphmanager.cpp \
           operation.cpp \
           parameter.cpp \
           parametersextractor.cpp \
           pkmodel.cpp \
		   residualerrormodel.cpp \
           sampleextractor.cpp \
           targetextractor.cpp \
           threecompartmentbolus.cpp \
           threecompartmentextra.cpp \
           threecompartmentinfusion.cpp \
           twocompartmentbolus.cpp \
           twocompartmentextra.cpp \
           twocompartmentinfusion.cpp

INCLUDES := . \
            ../../libs/eigen-3.3.2 \
            ../../libs/boost-1.63.0 \
            ../../libs/date-master-20170711 \
            ../../libs/spdlog-master-20170622/include
LIBS :=
EXTLIBS :=
DEFINES :=

include $(TUCUXI_ROOT)/make/rules.mak
