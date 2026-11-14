cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE CARGO_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/library/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bagged_goods/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/boxes_crates/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/liquid_bulk/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/raw_materials/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers_bottles/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/specialty_industrial/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/perishable_goods/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/building_materials/src/*.cpp"
)

add_library(cargo STATIC ${CARGO_SOURCES})

target_include_directories(cargo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bagged_goods/include
    ${CMAKE_CURRENT_SOURCE_DIR}/boxes_crates/include
    ${CMAKE_CURRENT_SOURCE_DIR}/liquid_bulk/include
    ${CMAKE_CURRENT_SOURCE_DIR}/raw_materials/include
    ${CMAKE_CURRENT_SOURCE_DIR}/containers_bottles/include
    ${CMAKE_CURRENT_SOURCE_DIR}/specialty_industrial/include
    ${CMAKE_CURRENT_SOURCE_DIR}/perishable_goods/include
    ${CMAKE_CURRENT_SOURCE_DIR}/building_materials/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../scene
    ${CMAKE_CURRENT_SOURCE_DIR}/../../shared
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../base
)
