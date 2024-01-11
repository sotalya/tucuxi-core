cmake_minimum_required(VERSION 3.20)
project (tucutestquery LANGUAGES CXX)

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
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucuquery tucuquery)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

file(GLOB TUCUTESTQUERY_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/gtest_queryinputstrings.h
)

file(GLOB TUCUTESTQUERY_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/gtest_dosageimportexport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_query.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_query1comp.cpp
)

add_executable(tucutestquery ${TUCUTESTQUERY_HEADER_FILES} ${TUCUTESTQUERY_SOURCE_FILES})

if(config_coverage)
    target_compile_options(tucutestquery PRIVATE --coverage -O0)
    target_link_options(tucutestquery PRIVATE --coverage)
endif(config_coverage)

if(WIN32)
    target_compile_options(tucutestquery PRIVATE -bigobj)
endif(WIN32)

# Now simply link against gtest or gtest_main as needed. Eg
set(LIBS_TO_LINK gtest_main tucuquery tucucore tucucommon)

if(UNIX)
    set(LIBS_TO_LINK pthread ${LIBS_TO_LINK})
endif(UNIX)

target_link_libraries(tucutestquery PRIVATE ${LIBS_TO_LINK})

include(GoogleTest)
gtest_discover_tests(tucutestquery)

include(GNUInstallDirs)
install(TARGETS tucutestquery
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
