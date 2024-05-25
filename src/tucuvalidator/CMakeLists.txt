cmake_minimum_required(VERSION 3.20)
project (tucuvalidator LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../tucucommon tucucommon)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../tucucore tucucore)

file(GLOB TUCUVALIDATOR_SOURCE_FILES    
    ${CMAKE_CURRENT_LIST_DIR}/tucuvalidator.cpp)

add_executable(tucuvalidator ${TUCUVALIDATOR_SOURCE_FILES})

set(LIBS_TO_LINK tucucore tucucommon)

if(UNIX)
    set(LIBS_TO_LINK pthread ${LIBS_TO_LINK})
endif(UNIX)

target_link_libraries(tucuvalidator PRIVATE ${LIBS_TO_LINK})

include(GNUInstallDirs)
install(TARGETS tucuvalidator
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
