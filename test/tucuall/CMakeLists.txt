cmake_minimum_required(VERSION 3.20)
project (tucutestall LANGUAGES CXX)

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

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucuquery tucuquery)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucommon tucucommon)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucrypto tucucrypto)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucore tucucore)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

file(GLOB TUCUTESTALL_HEADER_FILES
    # tucutestcore
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/buildmultianalytessingleactivemoiety.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/buildconstantelimination.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/buildimatinib.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/buildmultianalytesmultiactivemoieties.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/buildpkasymptotic.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/drugmodelbuilder.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_core.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_drugmodelimport_xmlstrings.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/pkmodels/constanteliminationbolus.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/pkmodels/multiconstanteliminationbolus.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/pkmodels/pkasymptotic.h
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/testutils.h
    # tucutestquery
    ${CMAKE_CURRENT_LIST_DIR}/../tucuquery/gtest_queryinputstrings.h
)

file(GLOB TUCUTESTALL_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/gtest_all.cpp
    # tucutestcommon
    ${CMAKE_CURRENT_LIST_DIR}/../tucucommon/gtest_componentmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucommon/gtest_datetime.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucommon/gtest_iterator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucommon/gtest_logger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucommon/gtest_scriptengine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucommon/gtest_unit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucommon/gtest_xml.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucommon/gtest_xmlimporter.cpp
    # tucutestcore
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/gtest_constanteliminationbolus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/gtest_drug_tobramycin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/gtest_drug_vancomycin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/gtest_michaelismenten1comp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/gtest_michaelismenten2comp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/gtest_multianalytesmultiactivemoieties.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/gtest_multianalytessingleactivemoiety.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/drugmodels/gtest_pkasymptotic.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_cachecomputing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_computingcomponentadjustments.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_computingcomponentattimes.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_computingcomponentconcentration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_computingcomponentpercentiles.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_concentrationcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_covariateextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_cyclestatistics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_dosage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_drugdomainconstraintsevaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_drugmodelimport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_drugmodels.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_intakeextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_intakeintervalcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_likelihood.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_multiconcentrationcalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_multiconstanteliminationbolus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_multilikelihood.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_nonmemdrugs.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_operablegraphmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_operation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_operationcollection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_parameter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_parameterextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_percentilecalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_pertinenttimescalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_pkmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_residualerrormodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_sampleextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_targetevaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/gtest_targetextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/pkmodels/gtest_rkmichaelismentenenzyme.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/pkmodels/gtest_rkmichaelismententwocompvmaxamount.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucore/pkmodels/gtest_twocompartmentextralag.cpp
    # tucutestcrypto
    ${CMAKE_CURRENT_LIST_DIR}/../tucucrypto/gtest_cryptohelper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucucrypto/gtest_licensemanager.cpp
    # tucutestquery
    ${CMAKE_CURRENT_LIST_DIR}/../tucuquery/gtest_dosageimportexport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../tucuquery/gtest_query1comp.cpp
)

add_executable(tucutestall ${TUCUTESTALL_HEADER_FILES} ${TUCUTESTALL_SOURCE_FILES})

if(config_coverage)
    target_compile_options(tucutestall PRIVATE --coverage -O0)
    target_link_options(tucutestall PRIVATE --coverage)
endif(config_coverage)

if(WIN32)
    target_compile_options(tucutestall PRIVATE -bigobj)
endif(WIN32)

# Now simply link against gtest or gtest_main as needed. Eg
# Now simply link against gtest or gtest_main as needed. Eg
set(LIBS_TO_LINK gtest_main tucucrypto tucuquery tucucore tucucommon)

if(UNIX)
    set(LIBS_TO_LINK pthread ${LIBS_TO_LINK})
endif(UNIX)

target_link_libraries(tucutestall PRIVATE ${LIBS_TO_LINK})

include(GoogleTest)
gtest_discover_tests(tucutestall)

include(GNUInstallDirs)
install(TARGETS tucutestall
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
