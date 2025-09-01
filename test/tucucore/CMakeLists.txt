cmake_minimum_required(VERSION 3.20)
project (tucutestcore LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


option(TUCU_TIME_DEBUG "Builds the dates and times with a string representation for easy debug")
if (TUCU_TIME_DEBUG)
    add_compile_definitions(TUCU_TIME_DEBUG)
endif()

option(TUCU_EASY_DEBUG "Builds some source files with facilitators for debugging. For instance macros replaced by function calls to easily add breakpoints")
if (TUCU_EASY_DEBUG)
    add_compile_definitions(TUCU_EASY_DEBUG)
endif()

option(TUCU_CHECK_DATETIME "Checks that each DateTime object is used initialized to a valid value. To be only used in DEBUG mode, for performance reason")
if (TUCU_CHECK_DATETIME)
    add_compile_definitions(TUCU_CHECK_DATETIME)
endif()

option(TUCU_COMPILE_CHECK_TYPES "Compiles with home-made classes for different classes, in order to check that there usage is correct. Compilers would allow a vector of same types to be compatible, but it would be nice to sometimes know what vectors are really used. Therefore we define Concentrations, MultiCompConcentration, MultiCompConcentrations and MultiCycleConcentrations without compatibility support. To be used to test if the code does not embed a code smell, but not in release mode. It could be implemented for AnalyteId and ActiveMoietyId as well")
if (TUCU_COMPILE_CHECK_TYPES)
    add_compile_definitions(TUCU_COMPILE_CHECK_TYPES)
endif()

# We need the test drugmodels
add_compile_definitions(TUCU_DRUGMODELTESTS)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)
# Include src directory
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../src)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)
include(${CMAKE_CURRENT_LIST_DIR}/../add_gtest)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucommon tucucommon)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucore tucucore)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

file(GLOB TUCUTESTCORE_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildmultianalytessingleactivemoiety.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildconstantelimination.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildgentamicinfuchs2014.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildimatinib.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildmultianalytesmultiactivemoieties.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildpkasymptotic.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/drugmodelbuilder.h
    ${CMAKE_CURRENT_LIST_DIR}/gtest_core.h
    ${CMAKE_CURRENT_LIST_DIR}/gtest_drugmodelimport_xmlstrings.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/constanteliminationbolus.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/multiconstanteliminationbolus.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/pkasymptotic.h
    ${CMAKE_CURRENT_LIST_DIR}/computingresponsecomparator.h
    ${CMAKE_CURRENT_LIST_DIR}/testutils.h
)

file(GLOB TUCUTESTCORE_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/gtest_constanteliminationbolus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/gtest_drug_tobramycin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/gtest_drug_vancomycin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/gtest_michaelismenten1comp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/gtest_michaelismenten2comp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/gtest_multianalytesmultiactivemoieties.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/gtest_multianalytessingleactivemoiety.cpp
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/gtest_pkasymptotic.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_core.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_cachecomputing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_computingcomponentadjustments.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_computingcomponentadjustments2.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_computingcomponentattimes.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_computingcomponentconcentration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_computingcomponentpercentiles.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_concentrationcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_covariateextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_cyclestatistics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_dosage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_drugdomainconstraintsevaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_drugdomainconstraintsevaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_drugmodelimport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_drugmodels.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_intakeextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_intakeintervalcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_likelihood.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_multiconcentrationcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_multiconstanteliminationbolus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_multilikelihood.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_nonmemdrugs.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_operablegraphmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_operation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_operationcollection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_parameter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_parameterextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_percentilecalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_pertinenttimescalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_pkmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_residualerrormodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_sampleextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_tadcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_targetevaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_targetextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/gtest_rkmichaelismentenenzyme.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/gtest_rkmichaelismententwocompvmaxamount.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/gtest_twocompartmentextralag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingresponsecomparator.cpp
)

add_executable(tucutestcore ${TUCUTESTCORE_HEADER_FILES} ${TUCUTESTCORE_SOURCE_FILES})

if(config_coverage)
    target_compile_options(tucutestcore PRIVATE --coverage -O0)
    target_link_options(tucutestcore PRIVATE --coverage)
endif(config_coverage)

if(WIN32)
    target_compile_options(tucutestcore PRIVATE -bigobj)
endif(WIN32)

# Now simply link against gtest or gtest_main as needed. Eg
set(LIBS_TO_LINK gtest_main tucucommon tucucore)

if(UNIX)
    set(LIBS_TO_LINK pthread ${LIBS_TO_LINK})
endif(UNIX)

target_link_libraries(tucutestcore PRIVATE ${LIBS_TO_LINK})

include(GoogleTest)
gtest_discover_tests(tucutestcore)

include(GNUInstallDirs)
install(TARGETS tucutestcore
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
