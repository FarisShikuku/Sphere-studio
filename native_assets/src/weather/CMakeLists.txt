add_library(weather STATIC
    WeatherSystem.cpp
    rain/RainSystem.cpp
    rain/RainSplash.cpp
    snow/SnowSystem.cpp
    snow/Blizzard.cpp
    fog/FogVolume.cpp
    clouds/CloudGen.cpp
    clouds/CloudRaymarch.cpp
    storms/StormPreset.cpp
    storms/Thunderstorm.cpp
    storms/Hail.cpp
    wind/GlobalWind.cpp
    lightning/LightningGen.cpp
    puddles/WetnessMap.cpp
    puddles/PuddleMask.cpp
    sky/SkyGenerator.cpp
    presets/WeatherPreset.cpp
    presets/WeatherClear.cpp
    presets/WeatherRain.cpp
    presets/WeatherSnow.cpp
    presets/WeatherStorm.cpp
)

target_include_directories(weather PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../base
    ${CMAKE_CURRENT_SOURCE_DIR}/rain
    ${CMAKE_CURRENT_SOURCE_DIR}/snow
    ${CMAKE_CURRENT_SOURCE_DIR}/fog
    ${CMAKE_CURRENT_SOURCE_DIR}/clouds
    ${CMAKE_CURRENT_SOURCE_DIR}/storms
    ${CMAKE_CURRENT_SOURCE_DIR}/wind
    ${CMAKE_CURRENT_SOURCE_DIR}/lightning
    ${CMAKE_CURRENT_SOURCE_DIR}/puddles
    ${CMAKE_CURRENT_SOURCE_DIR}/sky
    ${CMAKE_CURRENT_SOURCE_DIR}/presets
)

target_link_libraries(weather PUBLIC core scene shared vulkan_wrap)
