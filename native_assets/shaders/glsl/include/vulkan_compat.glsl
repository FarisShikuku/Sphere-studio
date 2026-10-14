#ifndef VULKAN_COMPAT_GLSL
#define VULKAN_COMPAT_GLSL

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : enable
#extension GL_EXT_buffer_reference : enable

#define BINDLESS_SET 0
#define SCENE_SET 1
#define MATERIAL_SET 2

#endif // VULKAN_COMPAT_GLSL
