#version 450
#include "include/prop_common.glsl"

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;

layout(push_constant) uniform PropRenderPush {
    mat4 viewProj;
    mat4 model;
} pc;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outWorldNormal;
layout(location = 2) out vec4 outColor;

void main() {
    vec4 wp = pc.model * vec4(inPos, 1.0);
    outWorldPos = wp.xyz;
    outWorldNormal = normalize(mat3(pc.model) * inNormal);
    outColor = inColor;
    gl_Position = pc.viewProj * wp;
}
