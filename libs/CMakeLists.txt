set(LIBS_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/boost
        ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/eigen
        ${CMAKE_CURRENT_SOURCE_DIR}/date/include
        ${CMAKE_CURRENT_SOURCE_DIR}/rapidxml
        ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/botan/build/include
        ${CMAKE_CURRENT_SOURCE_DIR}/cxxopts/include
        ${CMAKE_CURRENT_SOURCE_DIR}/tiny-js
    PARENT_SCOPE
)

include(${CMAKE_CURRENT_SOURCE_DIR}/botan/tucuxi-core_add_botan)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tiny-js tiny-js)
