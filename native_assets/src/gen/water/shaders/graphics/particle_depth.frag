#version 460
// Vulkan 1.3 Graphics Fragment Shader: Ray-Sphere Impostor Linear Depth
// Ported from talvinckb/OpenGL-Water-Simulation

layout(location = 0) in vec2 inQuadUv;
layout(location = 1) in vec3 inViewCenter;
layout(location = 2) in float inRadius;

layout(std140, set = 1, binding = 0) uniform FluidUbo {
    mat4 view;
    mat4 proj;
    mat4 invProj;
} ubo;

layout(location = 0) out float outLinearDepth;

void main() {
    float r2 = dot(inQuadUv, inQuadUv);
    if (r2 > 1.0) discard;

    float zOffset = sqrt(1.0 - r2);
    vec3 viewPos = inViewCenter + vec3(inQuadUv * inRadius, zOffset * inRadius);

    vec4 clipPos = ubo.proj * vec4(viewPos, 1.0);
    gl_FragDepth = clipPos.z / clipPos.w;
    outLinearDepth = -viewPos.z;
}
