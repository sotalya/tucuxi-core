cmake_minimum_required(VERSION 3.20)
project (tucucommon LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../libs/tiny-js tiny-js)

file(GLOB TUCUCOMMON_HEADERS_FILES
    ${CMAKE_CURRENT_LIST_DIR}/basetypes.h
    ${CMAKE_CURRENT_LIST_DIR}/component.h
    ${CMAKE_CURRENT_LIST_DIR}/componentmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/datetime.h
    ${CMAKE_CURRENT_LIST_DIR}/duration.h
    ${CMAKE_CURRENT_LIST_DIR}/general.h
    ${CMAKE_CURRENT_LIST_DIR}/iimport.h
    ${CMAKE_CURRENT_LIST_DIR}/ilogger.h
    ${CMAKE_CURRENT_LIST_DIR}/interface.h
    ${CMAKE_CURRENT_LIST_DIR}/jsengine.h
    ${CMAKE_CURRENT_LIST_DIR}/logger.h
    ${CMAKE_CURRENT_LIST_DIR}/loggerhelper.h
    ${CMAKE_CURRENT_LIST_DIR}/timeofday.h
    ${CMAKE_CURRENT_LIST_DIR}/translatablestring.h
    ${CMAKE_CURRENT_LIST_DIR}/unit.h
    ${CMAKE_CURRENT_LIST_DIR}/utils.h
    ${CMAKE_CURRENT_LIST_DIR}/xmlattribute.h
    ${CMAKE_CURRENT_LIST_DIR}/xmldocument.h
    ${CMAKE_CURRENT_LIST_DIR}/xmlimporter.h
    ${CMAKE_CURRENT_LIST_DIR}/xmliterator.h
    ${CMAKE_CURRENT_LIST_DIR}/xmlnode.h
    ${CMAKE_CURRENT_LIST_DIR}/xmltypes.h)

file(GLOB TUCUCOMMON_SOURCE_FILES    
    ${CMAKE_CURRENT_LIST_DIR}/componentmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/datetime.cpp
    ${CMAKE_CURRENT_LIST_DIR}/duration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/iimport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/jsengine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/logger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/timeofday.cpp
    ${CMAKE_CURRENT_LIST_DIR}/translatablestring.cpp
    ${CMAKE_CURRENT_LIST_DIR}/unit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmlattribute.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmldocument.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmlimporter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmlnode.cpp)

add_library(tucucommon_base STATIC ${TUCUCOMMON_SOURCE_FILES} ${TUCUCOMMON_HEADERS_FILES})

add_library(tucucommon INTERFACE)
target_link_libraries(tucucommon INTERFACE tucucommon_base tiny-js)
