#ifndef BINDLESS_GLSL
#define BINDLESS_GLSL
#include "vulkan_compat.glsl"

// Unbounded bindless texture array
layout(set = BINDLESS_SET, binding = 0) uniform sampler2D u_BindlessTextures2D[];
layout(set = BINDLESS_SET, binding = 1) uniform samplerCube u_BindlessTexturesCube[];

// Bindless texture sampling helpers
vec4 SampleBindless2D(uint index, vec2 uv) {
    return texture(u_BindlessTextures2D[nonuniformEXT(index)], uv);
}

vec4 SampleBindlessLod2D(uint index, vec2 uv, float lod) {
    return textureLod(u_BindlessTextures2D[nonuniformEXT(index)], uv, lod);
}

vec4 SampleBindlessCube(uint index, vec3 dir) {
    return texture(u_BindlessTexturesCube[nonuniformEXT(index)], dir);
}

#endif // BINDLESS_GLSL
