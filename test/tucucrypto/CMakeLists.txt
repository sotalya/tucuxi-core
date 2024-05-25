cmake_minimum_required(VERSION 3.20)
project (tucutestcrypto LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)
# Include src directory
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../src)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core_libs)
include(${CMAKE_CURRENT_LIST_DIR}/../add_gtest)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucommon tucucommon)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucrypto tucucrypto)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

file(GLOB TUCUTESTCRYPTO_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/gtest_crypto.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_cryptohelper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gtest_licensemanager.cpp
)

add_executable(tucutestcrypto ${TUCUTESTCRYPTO_SOURCE_FILES})

# Now simply link against gtest or gtest_main as needed. Eg
set(LIBS_TO_LINK gtest_main tucucommon tucucrypto)

if(UNIX)
    set(LIBS_TO_LINK pthread ${LIBS_TO_LINK})
endif(UNIX)

target_link_libraries(tucutestcrypto PRIVATE ${LIBS_TO_LINK})

include(GoogleTest)
gtest_discover_tests(tucutestcrypto)

include(GNUInstallDirs)
install(TARGETS tucutestcrypto
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
