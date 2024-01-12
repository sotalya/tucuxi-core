cmake_minimum_required(VERSION 3.20)
project (tucucrypto LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)

# Check if botan exist
if(NOT TARGET botan)
    include(${CMAKE_CURRENT_LIST_DIR}/../../libs/botan/tucuxi-core_add_botan)
endif(NOT TARGET botan)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

file(GLOB TUCUCRYPTO_HEADERS_FILES
    ${CMAKE_CURRENT_LIST_DIR}/cryptohelper.h
    ${CMAKE_CURRENT_LIST_DIR}/licensemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/systeminfo.h)

file(GLOB TUCUCRYPTO_SOURCE_FILES    
    ${CMAKE_CURRENT_LIST_DIR}/cryptohelper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/licensemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/systeminfo.cpp)

add_library(tucucrypto_base STATIC ${TUCUCRYPTO_SOURCE_FILES} ${TUCUCRYPTO_HEADERS_FILES})

add_library(tucucrypto INTERFACE)
target_link_libraries(tucucrypto INTERFACE tucucrypto_base botan)
