cmake_minimum_required(VERSION 3.20)
project (tucucli LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../tucucommon tucucommon)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../tucucore tucucore)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../tucuquery tucuquery)

file(GLOB TUCUCLI_HEADER_FILES    
    ${CMAKE_CURRENT_LIST_DIR}/clicomputer.h)

file(GLOB TUCUCLI_SOURCE_FILES    
    ${CMAKE_CURRENT_LIST_DIR}/clicomputer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tucucli.cpp)

add_executable(tucucli ${TUCUCLI_SOURCE_FILES})

set(LIBS_TO_LINK tucuquery tucucore tucucommon)

if(UNIX)
    set(LIBS_TO_LINK ${LIBS_TO_LINK} pthread)
endif(UNIX)

target_link_libraries(tucucli PRIVATE ${LIBS_TO_LINK})

include(GNUInstallDirs)
install(TARGETS tucucli
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
