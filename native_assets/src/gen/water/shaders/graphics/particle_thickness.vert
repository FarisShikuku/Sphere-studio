#version 460
// Vulkan 1.3 Graphics Vertex Shader: Fluid Thickness Impostor Pass
// Ported from talvinckb/OpenGL-Water-Simulation

layout(location = 0) in vec2 inCorner;

layout(std430, set = 0, binding = 0) readonly buffer PositionsBuffer {
    vec4 positions[];
};

layout(std140, set = 1, binding = 0) uniform FluidUbo {
    mat4 view;
    mat4 proj;
    mat4 invProj;
    vec4 cameraPosAndTime;
    vec4 boundsHalfAndRadius;
} ubo;

layout(location = 0) out vec2 outQuadUv;

void main() {
    vec3 worldPos = positions[gl_InstanceIndex].xyz;
    vec4 viewCenter = ubo.view * vec4(worldPos, 1.0);
    float r = ubo.boundsHalfAndRadius.w * 1.15;

    vec3 quadViewPos = viewCenter.xyz + vec3(inCorner * r, 0.0);
    gl_Position = ubo.proj * vec4(quadViewPos, 1.0);
    outQuadUv = inCorner;
}
