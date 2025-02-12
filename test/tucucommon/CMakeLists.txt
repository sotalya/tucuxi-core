cmake_minimum_required(VERSION 3.20)
project (tucutestcommon LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(EASY_DEBUG "Builds the dates and times with a string representation for easy debug")
if (EASY_DEBUG)
    add_compile_definitions(EASY_DEBUG)
endif()

option(CHECK_DATETIME "Checks that each DateTime object is used initialized to a valid value. To be only used in DEBUG mode, for performance reason")
if (CHECK_DATETIME)
    add_compile_definitions(CHECK_DATETIME)
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)
# Include src directory
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../src)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)
include(${CMAKE_CURRENT_LIST_DIR}/../add_gtest)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucommon tucucommon)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

file(GLOB TUCUTESTCOMMON_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/gtest_common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_componentmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_datetime.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_iterator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_logger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_scriptengine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_unit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_xml.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_xmlimporter.cpp
)

add_executable(tucutestcommon ${TUCUTESTCOMMON_SOURCE_FILES})

if(config_coverage)
    target_compile_options(tucutestcommon PRIVATE --coverage -O0)
    target_link_options(tucutestcommon PRIVATE --coverage)
endif(config_coverage)

# Now simply link against gtest or gtest_main as needed. Eg
set(LIBS_TO_LINK gtest_main tucucommon)

if(UNIX)
    set(LIBS_TO_LINK pthread ${LIBS_TO_LINK})
endif(UNIX)

target_link_libraries(tucutestcommon PRIVATE ${LIBS_TO_LINK})

include(GoogleTest)
gtest_discover_tests(tucutestcommon)

include(GNUInstallDirs)
install(TARGETS tucutestcommon
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
