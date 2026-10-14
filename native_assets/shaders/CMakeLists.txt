cmake_minimum_required(VERSION 3.22)
project(SphereStudioShaders LANGUAGES CXX)

find_package(Vulkan REQUIRED)
find_program(GLSLC glslc HINTS ${Vulkan_GLSLC_EXECUTABLE} ENV VULKAN_SDK PATH_SUFFIXES bin)

if(NOT GLSLC)
    message(STATUS "glslc not found - skipping offline SPIR-V compilation; shaders will be compiled at runtime.")
    return()
endif()

set(SHADER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glsl/include)
set(SPV_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/spv)
file(MAKE_DIRECTORY ${SPV_OUTPUT_DIR})

set(SHADER_SOURCES
    glsl/pbr.vert
    glsl/pbr.frag
    glsl/shadow.vert
    glsl/shadow.frag
    glsl/sky.vert
    glsl/sky.frag
    glsl/sky.comp
    glsl/cull.comp
    glsl/meshlet.comp
    glsl/debug/debug_line.vert
    glsl/debug/debug_line.frag
    glsl/debug/debug_text.frag
)

set(SPV_OUTPUTS "")
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPV_FILE ${SPV_OUTPUT_DIR}/${SHADER_NAME}.spv)
    add_custom_command(
        OUTPUT ${SPV_FILE}
        COMMAND ${GLSLC} -I ${SHADER_INCLUDE_DIR} --target-env=vulkan1.3 -O ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} -o ${SPV_FILE}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}
        COMMENT "Compiling SPIR-V: ${SHADER} -> ${SPV_FILE}"
        VERBATIM
    )
    list(APPEND SPV_OUTPUTS ${SPV_FILE})
endforeach()

add_custom_target(compile_shaders ALL DEPENDS ${SPV_OUTPUTS})
