cmake_minimum_required(VERSION 3.20)
project (tucudrugfilechecker LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../tucucommon tucucommon)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../tucucore tucucore)

file(GLOB TUCUDRUGFILECHECKER_SOURCE_FILES    
    ${CMAKE_CURRENT_LIST_DIR}/tucudrugfilechecker.cpp)

add_executable(tucudrugfilechecker ${TUCUDRUGFILECHECKER_SOURCE_FILES})

target_link_libraries(tucudrugfilechecker tucucore tucucommon)

include(GNUInstallDirs)
install(TARGETS tucudrugfilechecker
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
