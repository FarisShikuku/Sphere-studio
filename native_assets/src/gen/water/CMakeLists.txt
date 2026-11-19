cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE WATER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(water STATIC ${WATER_SOURCES})

target_include_directories(water PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../scene
    ${CMAKE_CURRENT_SOURCE_DIR}/../../shared
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../base
)

target_link_libraries(water PUBLIC Vulkan::Vulkan)
