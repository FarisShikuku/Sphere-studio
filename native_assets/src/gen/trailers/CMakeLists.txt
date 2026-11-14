cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE TRAILER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/library/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/flatbed/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tanker/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/box_van/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/chassis/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/logging/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/curtainsider/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bulk_hopper/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/car_carrier/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/heavy_haulage/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/dolly/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/recreational/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/specialty/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/running_gear/src/*.cpp"
)

add_library(trailers STATIC ${TRAILER_SOURCES})

target_include_directories(trailers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/flatbed/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tanker/include
    ${CMAKE_CURRENT_SOURCE_DIR}/box_van/include
    ${CMAKE_CURRENT_SOURCE_DIR}/chassis/include
    ${CMAKE_CURRENT_SOURCE_DIR}/logging/include
    ${CMAKE_CURRENT_SOURCE_DIR}/curtainsider/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bulk_hopper/include
    ${CMAKE_CURRENT_SOURCE_DIR}/car_carrier/include
    ${CMAKE_CURRENT_SOURCE_DIR}/heavy_haulage/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dolly/include
    ${CMAKE_CURRENT_SOURCE_DIR}/recreational/include
    ${CMAKE_CURRENT_SOURCE_DIR}/specialty/include
    ${CMAKE_CURRENT_SOURCE_DIR}/running_gear/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../cargo/library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../scene
    ${CMAKE_CURRENT_SOURCE_DIR}/../../shared
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../base
)

target_link_libraries(trailers PUBLIC cargo)
