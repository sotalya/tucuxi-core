cmake_minimum_required(VERSION 3.20)
project (tucucore LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


option(TUCU_NO_PERCENTILES "Builds tucucore with percentiles computation disabled. Only used for easing development.")
if (TUCU_NO_PERCENTILES)
    add_compile_definitions(TUCU_NO_PERCENTILES)
endif()

option(TUCU_DRUGMODELTESTS "Compiles tucucore with additional drug models embeded for tests.")
if (TUCU_DRUGMODELTESTS)
    add_compile_definitions(TUCU_DRUGMODELTESTS)
endif()

option(TUCU_NOCATCH "Converts try{} catch{} to try/catch at all. Allows to ease debug stages.")
if (TUCU_NOCATCH)
    add_compile_definitions(TUCU_NOCATCH)
endif()

option(TUCU_POPPARAMETERSFROMDEFAULTVALUES "Does not compute population PK parameters from the typical patient with a priori computation, but directly takes the values fixed in the drug model. Only used for testing purpose.")
if (TUCU_POPPARAMETERSFROMDEFAULTVALUES)
    add_compile_definitions(TUCU_POPPARAMETERSFROMDEFAULTVALUES)
endif()

option(TUCU_INTAKECALCULATOR_VERBOSE "Displays information of intakecalculators parameters. Only used for debugging purpose.")
if (TUCU_INTAKECALCULATOR_VERBOSE)
    add_compile_definitions(TUCU_INTAKECALCULATOR_VERBOSE)
endif()

option(TUCU_SINGLETHREADEDPERCENTILESORT "Runs a single-threaded version of percentiles sort.")
if (TUCU_SINGLETHREADEDPERCENTILESORT)
    add_compile_definitions(TUCU_SINGLETHREADEDPERCENTILESORT)
endif()


option(TUCU_MULTITHREADEDPERCENTILESUBOPTIMALSORT "If run with the multithreaded version of percentiles sort, selects a suboptimal multithreaded version.")
if (TUCU_MULTITHREADEDPERCENTILESUBOPTIMALSORT)
    add_compile_definitions(TUCU_MULTITHREADEDPERCENTILESUBOPTIMALSORT)
endif()

option(TUCU_SINGLETHREADEDPERCENTILESORTOPTION "If not in multithreaded version, selects a sort option over the other.")
if (TUCU_SINGLETHREADEDPERCENTILESORTOPTION)
    add_compile_definitions(TUCU_SINGLETHREADEDPERCENTILESORTOPTION)
endif()

option(TUCU_SINGLETHREADEDPERCENTILECOMPUTATION "Runs a multithreaded version of percentiles computation.")
if (TUCU_SINGLETHREADEDPERCENTILECOMPUTATION)
    add_compile_definitions(TUCU_SINGLETHREADEDPERCENTILECOMPUTATION)
endif()

option(TUCU_CONFIG_OPENMP "Runs percentiles computation with OpenMP.")
if (TUCU_CONFIG_OPENMP)
    add_compile_definitions(TUCU_CONFIG_OPENMP)
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)

if(config_openmp)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fopenmp")
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")
    add_compile_definitions(CONFIG_OPENMP)
endif(config_openmp)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

file(GLOB TUCUCORE_HEADERS_FILES
    ${CMAKE_CURRENT_LIST_DIR}/aposteriorietascalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/cachecomputing.h
    ${CMAKE_CURRENT_LIST_DIR}/cachedexponentials.h
    ${CMAKE_CURRENT_LIST_DIR}/computingadjustments.h
    ${CMAKE_CURRENT_LIST_DIR}/computingcomponent.h
    ${CMAKE_CURRENT_LIST_DIR}/computingrequestxmlexport.h
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingaborter.h
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingrequest.h
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingresponse.h
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingresult.h
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingtrait.h
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/icomputingservice.h
    ${CMAKE_CURRENT_LIST_DIR}/computingutils.h
    ${CMAKE_CURRENT_LIST_DIR}/concentrationcalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/concentrationprediction.h
    ${CMAKE_CURRENT_LIST_DIR}/covariateevent.h
    ${CMAKE_CURRENT_LIST_DIR}/covariateextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/cyclestatistics.h
    ${CMAKE_CURRENT_LIST_DIR}/cyclestatisticscalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/definitions.h
    ${CMAKE_CURRENT_LIST_DIR}/deriv.h
    ${CMAKE_CURRENT_LIST_DIR}/dosage.h
    ${CMAKE_CURRENT_LIST_DIR}/drugdefinitions.h
    ${CMAKE_CURRENT_LIST_DIR}/drugdomainconstraintsevaluator.h
    ${CMAKE_CURRENT_LIST_DIR}/drugfilevalidator.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/activemoiety.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/analyte.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/covariatedefinition.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/drugmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/drugmodeldomain.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/drugmodelmetadata.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/errormodel.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/formulationandroute.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/parameterdefinition.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/targetdefinition.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/timeconsiderations.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/validdose.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/validduration.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodelchecker.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodelimport.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodelrepository.h
    ${CMAKE_CURRENT_LIST_DIR}/drugtreatment/drugtreatment.h
    ${CMAKE_CURRENT_LIST_DIR}/drugtreatment/patientcovariate.h
    ${CMAKE_CURRENT_LIST_DIR}/drugtreatment/sample.h
    ${CMAKE_CURRENT_LIST_DIR}/drugtreatment/target.h
    ${CMAKE_CURRENT_LIST_DIR}/generalextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/hardcodedoperation.h
    ${CMAKE_CURRENT_LIST_DIR}/idatamodelservices.h
    ${CMAKE_CURRENT_LIST_DIR}/intakeevent.h
    ${CMAKE_CURRENT_LIST_DIR}/intakeextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/intakeintervalcalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/intakeintervalcalculatoranalytical.h
    ${CMAKE_CURRENT_LIST_DIR}/intakeintervalcalculatorrk4.h
    ${CMAKE_CURRENT_LIST_DIR}/intaketocalculatorassociator.h
    ${CMAKE_CURRENT_LIST_DIR}/invariants.h
    ${CMAKE_CURRENT_LIST_DIR}/iresidualerrormodel.h
    ${CMAKE_CURRENT_LIST_DIR}/likelihood.h
    ${CMAKE_CURRENT_LIST_DIR}/minimize.h
    ${CMAKE_CURRENT_LIST_DIR}/montecarlopercentilecalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/multicomputingadjustments.h
    ${CMAKE_CURRENT_LIST_DIR}/multicomputingcomponent.h
    ${CMAKE_CURRENT_LIST_DIR}/multiconcentrationcalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/multiconcentrationprediction.h
    ${CMAKE_CURRENT_LIST_DIR}/multilikelihood.h
    ${CMAKE_CURRENT_LIST_DIR}/multimontecarlopercentilecalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/operablegraphmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/operation.h
    ${CMAKE_CURRENT_LIST_DIR}/overloadevaluator.h
    ${CMAKE_CURRENT_LIST_DIR}/parameter.h
    ${CMAKE_CURRENT_LIST_DIR}/parameterids.h
    ${CMAKE_CURRENT_LIST_DIR}/parametersextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/percentilesprediction.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/onecompartmentbolus.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/onecompartmentextra.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/onecompartmentextralag.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/onecompartmentinfusion.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenenzyme.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenlinearonecomp.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenlinearonecompvmaxamount.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenlineartwocomp.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenlineartwocompvmaxamount.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenonecomp.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenonecompvmaxamount.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismententwocomp.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismententwocompvmaxamount.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkonecompartmentextra.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkonecompartmentgammaextra.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rktwocompartmenterlang.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rktwocompartmentextralag.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkthreecompartment.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/threecompartmentbolus.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/threecompartmentextra.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/threecompartmentinfusion.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/twocompartmentbolus.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/twocompartmentextra.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/twocompartmentextralag.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/twocompartmentinfusion.h
    ${CMAKE_CURRENT_LIST_DIR}/residualerrormodel.h
    ${CMAKE_CURRENT_LIST_DIR}/residualerrormodelextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/sampleevent.h
    ${CMAKE_CURRENT_LIST_DIR}/sampleextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/targetevaluationresult.h
    ${CMAKE_CURRENT_LIST_DIR}/targetevaluator.h
    ${CMAKE_CURRENT_LIST_DIR}/targetevent.h
    ${CMAKE_CURRENT_LIST_DIR}/targetextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/timedevent.h
    ${CMAKE_CURRENT_LIST_DIR}/treatmentdrugmodelcompatibilitychecker.h
    ${CMAKE_CURRENT_LIST_DIR}/validvalues.h
    ${CMAKE_CURRENT_LIST_DIR}/version.h)

file(GLOB TUCUCORE_SOURCE_FILES    
    ${CMAKE_CURRENT_LIST_DIR}/aposteriorietascalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cachecomputing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cachedexponentials.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingadjustments.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingcomponent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingrequestxmlexport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingaborter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingrequest.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingresponse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingresult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/computingtrait.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingservice/icomputingservice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingutils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/concentrationcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/covariateevent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/covariateextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cyclestatistics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cyclestatisticscalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dosage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugdomainconstraintsevaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugfilevalidator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/activemoiety.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/analyte.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/drugmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/drugmodeldomain.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/drugmodelmetadata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/errormodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/formulationandroute.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/parameterdefinition.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/targetdefinition.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/timeconsiderations.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/validdose.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodel/validduration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodelchecker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodelimport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodelrepository.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugtreatment/drugtreatment.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugtreatment/sample.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugtreatment/target.cpp
    ${CMAKE_CURRENT_LIST_DIR}/generalextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hardcodedoperation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/intakeevent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/intakeextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/intakeintervalcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/intakeintervalcalculatoranalytical.cpp
    ${CMAKE_CURRENT_LIST_DIR}/intakeintervalcalculatorrk4.cpp
    ${CMAKE_CURRENT_LIST_DIR}/intaketocalculatorassociator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/likelihood.cpp
    ${CMAKE_CURRENT_LIST_DIR}/montecarlopercentilecalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/multicomputingadjustments.cpp
    ${CMAKE_CURRENT_LIST_DIR}/multicomputingcomponent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/multiconcentrationcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/multilikelihood.cpp
    ${CMAKE_CURRENT_LIST_DIR}/multimontecarlopercentilecalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/operablegraphmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/operation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/overloadevaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/parameter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/parametersextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/onecompartmentbolus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/onecompartmentextra.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/onecompartmentextralag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/onecompartmentinfusion.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenenzyme.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenlinearonecomp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenlinearonecompvmaxamount.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenlineartwocomp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenlineartwocompvmaxamount.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenonecomp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismentenonecompvmaxamount.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismententwocomp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkmichaelismententwocompvmaxamount.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkonecompartmentextra.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkonecompartmentgammaextra.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rktwocompartmenterlang.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rktwocompartmentextralag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/rkthreecompartment.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/threecompartmentbolus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/threecompartmentextra.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/threecompartmentinfusion.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/twocompartmentbolus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/twocompartmentextra.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/twocompartmentextralag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/twocompartmentinfusion.cpp
    ${CMAKE_CURRENT_LIST_DIR}/residualerrormodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/residualerrormodelextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sampleevent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sampleextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/targetevaluationresult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/targetevaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/targetevent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/targetextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/treatmentdrugmodelcompatibilitychecker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/validvalues.cpp
    ${CMAKE_CURRENT_LIST_DIR}/version.cpp)

add_library(tucucore STATIC ${TUCUCORE_SOURCE_FILES} ${TUCUCORE_HEADERS_FILES})
