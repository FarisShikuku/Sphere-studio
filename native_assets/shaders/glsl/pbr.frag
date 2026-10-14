#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 N = normalize(inNormal);
    vec3 L = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(N, L), 0.15);
    outColor = vec4(vec3(0.7, 0.8, 0.9) * diff, 1.0);
}\n