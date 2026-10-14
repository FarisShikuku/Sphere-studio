#ifndef BINDLESS_GLSL
#define BINDLESS_GLSL
#include "vulkan_compat.glsl"

layout(set = 0, binding = 0) uniform sampler2D u_BindlessTextures[];
#endif\n