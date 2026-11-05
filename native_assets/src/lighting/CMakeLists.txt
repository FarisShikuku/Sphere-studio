file(GLOB_RECURSE LIGHTING_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(lighting STATIC ${LIGHTING_SOURCES})
target_include_directories(lighting PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene
    ${CMAKE_CURRENT_SOURCE_DIR}/../../base
)
