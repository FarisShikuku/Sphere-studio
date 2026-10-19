add_library(sky STATIC
    atmosphere/PreethamModel.cpp
    sun/SunDisk.cpp
)

target_include_directories(sky PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../base
    ${CMAKE_CURRENT_SOURCE_DIR}/atmosphere
    ${CMAKE_CURRENT_SOURCE_DIR}/sun
)

target_link_libraries(sky PUBLIC core scene shared vulkan_wrap)
