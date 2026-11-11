cmake_minimum_required(VERSION 3.22)
project(SphereStudioEngine LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Vulkan REQUIRED)
find_library(log-lib log)
find_library(android-lib android)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scene
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sky
    ${CMAKE_CURRENT_SOURCE_DIR}/src/weather
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lighting
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/cargo/library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/cargo/bagged_goods/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/cargo/boxes_crates/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/cargo/liquid_bulk/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/cargo/raw_materials/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/cargo/containers_bottles/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/cargo/specialty_industrial/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/cargo/perishable_goods/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jni
    ${Vulkan_INCLUDE_DIRS}
)
add_subdirectory(base)
add_subdirectory(src/core)
add_subdirectory(src/vulkan)
add_subdirectory(src/scene)
add_subdirectory(src/shared)
add_subdirectory(src/sky)
add_subdirectory(src/weather)
add_subdirectory(src/lighting)
add_subdirectory(src/gen/cargo)
add_library(SphereStudioEngine SHARED
    src/jni/SphereStudioJni.cpp
)
target_link_libraries(SphereStudioEngine PRIVATE
    base
    core
    vulkan_wrap
    scene
    shared
    sky
    weather
    lighting
    cargo
    Vulkan::Vulkan
    ${log-lib}
    ${android-lib}
)