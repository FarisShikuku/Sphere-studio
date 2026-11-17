cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE VEHICLE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/library/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lorry_rigid/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lorry_articulated/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bus_city/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bus_coach/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bus_minibus/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cab_components/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/chassis_components/src/*.cpp"
)

add_library(vehicles STATIC ${VEHICLE_SOURCES})

target_include_directories(vehicles PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lorry_rigid/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lorry_articulated/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bus_city/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bus_coach/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bus_minibus/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cab_components/include
    ${CMAKE_CURRENT_SOURCE_DIR}/chassis_components/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../trailers/library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../trailers/running_gear/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../cargo/library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../scene
    ${CMAKE_CURRENT_SOURCE_DIR}/../../shared
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../base
)

target_link_libraries(vehicles PUBLIC trailers cargo)
