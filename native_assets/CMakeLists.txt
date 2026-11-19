cmake_minimum_required(VERSION 3.22)
project(SphereStudioEngine LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-O3 -fno-math-errno)
find_package(Vulkan REQUIRED)
find_library(log-lib log)
find_library(android-lib android)
find_library(jnigraphics-lib jnigraphics)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/cargo/building_materials/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/flatbed/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/tanker/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/box_van/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/chassis/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/logging/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/curtainsider/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/bulk_hopper/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/car_carrier/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/heavy_haulage/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/dolly/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/recreational/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/specialty/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/trailers/running_gear/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/vehicles/library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/vehicles/lorry_rigid/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/vehicles/lorry_articulated/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/vehicles/bus_city/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/vehicles/bus_coach/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/vehicles/bus_minibus/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/vehicles/cab_components/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/vehicles/chassis_components/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/audio/vehicles/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/audio/weather/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gen/water/include
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
add_subdirectory(src/gen/trailers)
add_subdirectory(src/gen/vehicles)
add_subdirectory(src/gen/audio/vehicles)
add_subdirectory(src/gen/audio/weather)
add_subdirectory(src/gen/water)
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
    trailers
    vehicles
    audio_vehicles
    audio_weather
    water
    Vulkan::Vulkan
    ${log-lib}
    ${android-lib}
    ${jnigraphics-lib}
)