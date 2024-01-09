cmake_minimum_required(VERSION 3.20)
project (tucuquery LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

file(GLOB TUCUQUERY_HEADERS_FILES
    ${CMAKE_CURRENT_LIST_DIR}/administrativedata.h
    ${CMAKE_CURRENT_LIST_DIR}/computingquery.h
    ${CMAKE_CURRENT_LIST_DIR}/computingqueryresponse.h
    ${CMAKE_CURRENT_LIST_DIR}/computingqueryresponsexmlexport.h
    ${CMAKE_CURRENT_LIST_DIR}/computingresponseexport.h
    ${CMAKE_CURRENT_LIST_DIR}/fullsample.h
    ${CMAKE_CURRENT_LIST_DIR}/parametersdata.h
    ${CMAKE_CURRENT_LIST_DIR}/querycomputer.h
    ${CMAKE_CURRENT_LIST_DIR}/querydata.h
    ${CMAKE_CURRENT_LIST_DIR}/queryimport.h
    ${CMAKE_CURRENT_LIST_DIR}/querylogger.h
    ${CMAKE_CURRENT_LIST_DIR}/querystatus.h
    ${CMAKE_CURRENT_LIST_DIR}/querytocoreextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/requestdata.h)

file(GLOB TUCUQUERY_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/administrativedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingquery.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingqueryresponse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingqueryresponsexmlexport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/computingresponseexport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/fullsample.cpp
    ${CMAKE_CURRENT_LIST_DIR}/parametersdata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/querycomputer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/querydata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/queryimport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/querylogger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/querytocoreextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/requestdata.cpp)

add_library(tucuquery STATIC ${TUCUQUERY_SOURCE_FILES} ${TUCUQUERY_HEADERS_FILES})
