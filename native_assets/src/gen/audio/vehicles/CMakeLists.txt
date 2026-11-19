cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE AUDIO_VEHICLES_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(audio_vehicles STATIC ${AUDIO_VEHICLES_SOURCES})

target_include_directories(audio_vehicles PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../shared
)
