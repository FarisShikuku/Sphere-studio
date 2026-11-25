#version 450
#include "include/building_common.glsl"

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in uint inMaterialId;

layout(push_constant) uniform CameraPushConstants {
    mat4 viewProj;
    mat4 model;
} pc;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outWorldNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) flat out uint outMaterialId;

void main() {
    vec4 wp = pc.model * vec4(inPos, 1.0);
    outWorldPos = wp.xyz;
    outWorldNormal = normalize(mat3(pc.model) * inNormal);
    outUV = inUV;
    outMaterialId = inMaterialId;
    gl_Position = pc.viewProj * wp;
}
