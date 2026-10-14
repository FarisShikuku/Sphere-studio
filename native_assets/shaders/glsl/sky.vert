#version 460
layout(location = 0) in vec3 inPosition;

layout(push_constant) uniform SkyPush {
    mat4 invViewProj;
} push;

layout(location = 0) out vec3 outWorldDir;

void main() {
    vec4 unprojected = push.invViewProj * vec4(inPosition.xy, 1.0, 1.0);
    outWorldDir = unprojected.xyz / unprojected.w;
    gl_Position = vec4(inPosition.xy, 1.0, 1.0);
}
