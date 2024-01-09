cmake_minimum_required(VERSION 3.20)
project (tucucrypto LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../../libs/spdlog/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../libs/date/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../libs/eigen)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../libs/botan/build/include)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

file(GLOB TUCUCORE_HEADERS_FILES
    ${CMAKE_CURRENT_LIST_DIR}/cryptohelper.h
    ${CMAKE_CURRENT_LIST_DIR}/licensemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/systeminfo.h)

file(GLOB TUCUCORE_SOURCE_FILES    
    ${CMAKE_CURRENT_LIST_DIR}/cryptohelper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/licensemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/systeminfo.cpp)

add_library(tucucrypto STATIC ${TUCUCORE_SOURCE_FILES} ${TUCUCORE_HEADERS_FILES})