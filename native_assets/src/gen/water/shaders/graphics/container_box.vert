#version 460
// Vulkan 1.3 Graphics Vertex Shader: Tank Container Wireframe & Obstacle Geometry

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(std140, set = 1, binding = 0) uniform FluidUbo {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;

void main() {
    outWorldPos = inPosition;
    outNormal = inNormal;
    gl_Position = ubo.proj * ubo.view * vec4(inPosition, 1.0);
}
