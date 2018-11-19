cmake_minimum_required (VERSION 2.6)
project (tucuxi)

set(TUCUXI_ROOT ${CMAKE_BINARY_DIR}/../../)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT ${TUCUXI_ROOT}/libs/boost-1.63.0)
find_package(Boost 1.63.0)

set(TINYJS_ROOT ${TUCUXI_ROOT}/libs/tiny-js-master-20170629)
set(FRUCTOSE_ROOT ${TUCUXI_ROOT}/libs/fructose-1.3/fructose)
set(SPDLOG_ROOT ${TUCUXI_ROOT}/libs/spdlog-master-20170622)
set(EIGEN_ROOT ${TUCUXI_ROOT}/libs/eigen-3.3.2)
set(DATEMASTER_ROOT ${TUCUXI_ROOT}/libs/date-master-20170711)
set(RAPIDXML_ROOT ${TUCUXI_ROOT}/libs/rapidxml-1.13)
set(BOTAN_ROOT ${TUCUXI_ROOT}/libs/botan-2.1.0)
add_subdirectory(${BOTAN_ROOT} ${BOTAN_ROOT}/build)

set(TUCUXI_SRC ${TUCUXI_ROOT}/src)
set(TUCUXI_SRC ${TUCUXI_ROOT}/src)

include_directories(
    ${TUCUXI_SRC}
    ${Boost_INCLUDE_DIRS}
    ${TINYJS_ROOT}
    ${EIGEN_ROOT}
    ${FRUCTOSE_ROOT}/include
    ${SPDLOG_ROOT}/include
    ${DATEMASTER_ROOT}
    ${RAPIDXML_ROOT}
    ${BOTAN_ROOT}/build/include)

set(TINYJS_SOURCES ${TINYJS_ROOT}/TinyJS.cpp
           ${TINYJS_ROOT}/TinyJS_Functions.cpp
           ${TINYJS_ROOT}/TinyJS_MathFunctions.cpp
           ${TINYJS_ROOT}/TinyJS_SyntaxTree.cpp)

add_library(tinyjs STATIC ${TINYJS_SOURCES})

add_definitions(-DMACOS)

set(COMMON_DIR ${TUCUXI_SRC}/tucucommon)
set(TUCUXICOMMON_SOURCES
    ${COMMON_DIR}/componentmanager.cpp
    ${COMMON_DIR}/cryptohelper.cpp
    ${COMMON_DIR}/logger.cpp
    ${COMMON_DIR}/licensemanager.cpp
    ${COMMON_DIR}/utils.cpp
    ${COMMON_DIR}/datetime.cpp
    ${COMMON_DIR}/duration.cpp
    ${COMMON_DIR}/jsengine.cpp
    ${COMMON_DIR}/timeofday.cpp
    ${COMMON_DIR}/systeminfo.cpp
    ${COMMON_DIR}/xmlattribute.cpp
    ${COMMON_DIR}/xmldocument.cpp
    ${COMMON_DIR}/xmlnode.cpp
    ${COMMON_DIR}/translatablestring.cpp)

add_library(tucuxicommon STATIC ${TUCUXICOMMON_SOURCES})
set(CORE_DIR ${TUCUXI_SRC}/tucucore)
set(TUCUXICORE_SOURCES
    ${CORE_DIR}/drugmodel/drugmodel.cpp
    ${CORE_DIR}/cachedexponentials.cpp
    ${CORE_DIR}/concentrationcalculator.cpp
    ${CORE_DIR}/cyclestatisticscalculator.cpp
    ${CORE_DIR}/intakeintervalcalculator.cpp
    ${CORE_DIR}/intakeextractor.cpp
    ${CORE_DIR}/dosage.cpp
    ${CORE_DIR}/drugdomainconstraintsevaluator.cpp
    ${CORE_DIR}/operation.cpp
    ${CORE_DIR}/pkmodels/onecompartmentbolus.cpp
    ${CORE_DIR}/pkmodels/onecompartmentextra.cpp
    ${CORE_DIR}/pkmodels/rkonecompartmentextra.cpp
    ${CORE_DIR}/pkmodels/onecompartmentinfusion.cpp
    ${CORE_DIR}/pkmodels/threecompartmentbolus.cpp
    ${CORE_DIR}/pkmodels/threecompartmentextra.cpp
    ${CORE_DIR}/pkmodels/threecompartmentinfusion.cpp
    ${CORE_DIR}/pkmodels/twocompartmentbolus.cpp
    ${CORE_DIR}/pkmodels/twocompartmentextra.cpp
    ${CORE_DIR}/pkmodels/twocompartmentinfusion.cpp
    ${CORE_DIR}/computingcomponent.cpp
    ${CORE_DIR}/covariateextractor.cpp
    ${CORE_DIR}/parameter.cpp
    ${CORE_DIR}/parametersextractor.cpp
    ${CORE_DIR}/sampleextractor.cpp
    ${CORE_DIR}/targetextractor.cpp
    ${CORE_DIR}/targetevaluator.cpp
    ${CORE_DIR}/targetevaluationresult.cpp
    ${CORE_DIR}/pkmodel.cpp
    ${CORE_DIR}/intaketocalculatorassociator.cpp
    ${CORE_DIR}/hardcodedoperation.cpp
    ${CORE_DIR}/montecarlopercentilecalculator.cpp
    ${CORE_DIR}/likelihood.cpp
    ${CORE_DIR}/aposteriorietascalculator.cpp
    ${CORE_DIR}/operablegraphmanager.cpp
    ${CORE_DIR}/residualerrormodel.cpp
    ${CORE_DIR}/drugmodel/activemoiety.cpp
    ${CORE_DIR}/drugmodel/analyte.cpp
    ${CORE_DIR}/drugmodel/activesubstance.cpp
    ${CORE_DIR}/drugmodel/drugmodeldomain.cpp
    ${CORE_DIR}/drugmodel/formulationandroute.cpp
    ${CORE_DIR}/drugmodel/parameterdefinition.cpp
    ${CORE_DIR}/drugmodel/targetdefinition.cpp
    ${CORE_DIR}/drugmodel/validdose.cpp
    ${CORE_DIR}/drugmodel/validduration.cpp
    ${CORE_DIR}/drugmodel/timeconsiderations.cpp
    ${CORE_DIR}/drugmodelimport.cpp
    ${CORE_DIR}/sampleevent.cpp
    ${CORE_DIR}/drugtreatment/sample.cpp
    ${CORE_DIR}/drugtreatment/drugtreatment.cpp
    ${CORE_DIR}/drugtreatment/target.cpp
    ${CORE_DIR}/targetevent.cpp
    ${CORE_DIR}/covariateevent.cpp
    ${CORE_DIR}/computingservice/computingrequest.cpp
    ${CORE_DIR}/computingservice/computingresponse.cpp
    ${CORE_DIR}/computingservice/computingtrait.cpp
    ${CORE_DIR}/computingservice/icomputingservice.cpp
    ${CORE_DIR}/validvalues.cpp
    ${CORE_DIR}/computingservice/computingaborter.cpp
    ${CORE_DIR}/unit.cpp
    ${CORE_DIR}/drugmodel/drugmodelmetadata.cpp)

add_library(tucuxicore STATIC ${TUCUXICORE_SOURCES})

set(TUCUXI_TEST ${TUCUXI_ROOT}/test)
add_executable(tucucore ${TUCUXI_TEST}/tucucore/tests.cpp)
add_executable(tucucommon ${TUCUXI_TEST}/tucucommon/tests.cpp)
#add_executable(tuculicense ${TUCUXI_TEST}/tuculicense/tests.cpp)
target_link_libraries(tucucore pthread tucuxicore tucuxicommon tinyjs)
target_link_libraries(tucucommon pthread tucuxicore tucuxicommon tinyjs botan)
#target_link_libraries(tuculicense pthread tucuxicore tucuxicommon tinyjs)
