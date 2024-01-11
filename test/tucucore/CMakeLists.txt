cmake_minimum_required(VERSION 3.20)
project (tucutestcore LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)
# Include src directory
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../src)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)
include(${CMAKE_CURRENT_LIST_DIR}/../add_gtest)

# We need the test PK models for successful tests
add_compile_definitions(DRUGMODELTESTS)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucommon tucucommon)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucore tucucore)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

file(GLOB TUCUTESTCORE_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildmultianalytessingleactivemoiety.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildconstantelimination.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildimatinib.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildmultianalytesmultiactivemoieties.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/buildpkasymptotic.h
    ${CMAKE_CURRENT_LIST_DIR}/drugmodels/drugmodelbuilder.h
    ${CMAKE_CURRENT_LIST_DIR}/gtest_core.h
    ${CMAKE_CURRENT_LIST_DIR}/gtest_drugmodelimport_xmlstrings.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/constanteliminationbolus.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/multiconstanteliminationbolus.h
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/pkasymptotic.h
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
    ${CMAKE_CURRENT_LIST_DIR}/gtest_targetevaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_targetextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/gtest_rkmichaelismentenenzyme.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/gtest_rkmichaelismententwocompvmaxamount.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pkmodels/gtest_twocompartmentextralag.cpp
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
