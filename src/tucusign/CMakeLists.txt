cmake_minimum_required(VERSION 3.20)
project (tucusign LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

file(GLOB TUCUSIGN_HEADERS_FILES
    ${CMAKE_CURRENT_LIST_DIR}/signature.h
    ${CMAKE_CURRENT_LIST_DIR}/signer.h
    ${CMAKE_CURRENT_LIST_DIR}/signparser.h
    ${CMAKE_CURRENT_LIST_DIR}/signvalidator.h)

file(GLOB TUCUSIGN_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/signparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/signvalidator.cpp)

add_library(tucusign STATIC ${TUCUSIGN_SOURCE_FILES} ${TUCUSIGN_HEADERS_FILES})
