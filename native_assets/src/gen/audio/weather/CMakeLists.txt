cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE AUDIO_WEATHER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(audio_weather STATIC ${AUDIO_WEATHER_SOURCES})

target_include_directories(audio_weather PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../shared
)
